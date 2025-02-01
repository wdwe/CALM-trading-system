#include "historical_data.h"

#include <sstream>
#include <algorithm>
#include "fmt/format.h"
#include "date/date.h"
#include "date/tz.h"
#include "spdlog/spdlog.h"
#include "arrow/table.h"
#include "arrow/io/api.h"
#include "arrow/ipc/api.h"
#include "arrow/csv/api.h"
#include "arrow/compute/api.h"

namespace calm {
    std::string to_string(HistBarReq const& req) {
        return fmt::format("HistBarReq(symbol:{}, start:{}, end:{}, tz:{}, wts:{}, save_path:{})",
                           req.symbol, req.start, req.end, req.tz, req.wts, req.save_path);
    }

    std::string to_string(IBHistBarReq const& req) {
        return fmt::format("IBHistBarReq(symbol:{}, end:{}, dur:{}, bar_size:{}, wts:{})",
                           req.symbol, req.end, req.dur, req.bar_size, req.wts);
    }


    HistBarGetter::HistBarGetter(EventEngine& event_engine, IBGateway& gateway, std::vector<HistBarReq> const &reqs):
            gateway(gateway), event_engine{event_engine}{
        logger = init_sub_logger("hist_getter");
        for (auto &bar_req: reqs) {
            sym_req[bar_req.symbol] = bar_req;

            auto chunks = generate_ib_reqs(bar_req, 2);
            if (!chunks.empty()) {
                sym_chunks[bar_req.symbol] = chunks;
                sym_chunk_id[bar_req.symbol] = 0;
            }
        }
        remaining = sym_chunk_id.size();

        // register callbacks
        register_cb(EventType::hist_bar, "process_hist_bar", &HistBarGetter::process_hist_bar);
        register_cb(EventType::hist_bar_end, "process_hist_bar_end", &HistBarGetter::process_hist_bar_end);
        register_cb(EventType::ib_err_msg, "process_err_msg", &HistBarGetter::process_err_msg);

    }

    void HistBarGetter::run() {
        using namespace std::chrono_literals;
        for (auto const &[symbol, v]: sym_chunks) {
            q.wait_push(v[0]);
        }

        auto target_time = std::chrono::system_clock::now() + 10s;
        while (remaining) {
            IBHistBarReq req;
            if (q.try_pop(req, 1000)) {
                logger->info("in run - sending request {}", to_string(req));
                int req_id = gateway.req_historical_bar(req.symbol, req.end, req.dur, req.bar_size, req.wts, 1, 1);
                {
                    std::lock_guard lock{req_id_m};
                    req_id_sym[req_id] = req.symbol;
                }
                while (std::chrono::system_clock::now() < target_time) std::this_thread::sleep_for(1ms);
                target_time += 10s;
            }
        }
        log_status();

    }


    std::vector<IBHistBarReq> HistBarGetter::generate_ib_reqs(HistBarReq const& bar_req, int days) {
        using namespace std::chrono_literals;
        std::chrono::time_point<date::local_t, std::chrono::seconds> tp_st, tp_end;
        std::istringstream iss;
        iss.str(bar_req.start);
        iss >> date::parse("%F %T", tp_st);
        iss.clear();
        iss.str(bar_req.end);
        iss >> date::parse("%F %T", tp_end);

        std::vector<IBHistBarReq> reqs;
        auto st = tp_st;
        while (st < tp_end) {
            auto en = st + std::chrono::days(days);
            std::string dur;
            if (en < tp_end) {
                dur = fmt::format("{} D", days);
            } else if (tp_end - st >= 24h) {
                auto diff_sec = tp_end - st;
                auto secs = diff_sec.count();
                auto d = secs / 86'400;
                en = st + std::chrono::days(d);
                dur = fmt::format("{} D", d);
            } else { // less than a day remaining, use seconds
                auto diff_sec = tp_end - st;
                auto secs = diff_sec.count();
                dur = fmt::format("{} S", secs);
                en = tp_end;
            }
            std::string end_str;

            if (bar_req.tz == "UTC" || bar_req.tz == "utc") end_str = date::format("%Y%m%d-%T", en);
            else end_str = date::format("%Y%m%d %T", en) + " " + bar_req.tz;

            reqs.emplace_back(bar_req.symbol, end_str, dur, "5 secs", bar_req.wts);
            st = en;
        }
        return reqs;
    }


    void HistBarGetter::process_hist_bar(const calm::Event &event) {
        auto data = std::static_pointer_cast<HistBar>(event.data);
        std::string symbol;
        {
            std::lock_guard lock{req_id_m};
            if (!req_id_sym.contains(data->id)) {
                logger->warn("in process_hist_bar - req_id_sym does not contain {}", data->id);
                return;
            }
            symbol = req_id_sym[data->id];
        }
        sym_data[symbol].push_back(*data);
    }

    void HistBarGetter::process_hist_bar_end(const calm::Event &event) {
        auto data = std::static_pointer_cast<HistBarEnd>(event.data);
        std::string symbol;
        {
            std::lock_guard lock{req_id_m};
            if (!req_id_sym.contains(data->id)) {
                logger->warn("in process_hist_bar - req_id_sym does not contain {}", data->id);
                return;
            }
            symbol = req_id_sym[data->id];
        }

        ++sym_chunk_id[symbol];
        // TODO: run process_date() in another thread so it does not block the event queue

        if (sym_chunk_id[symbol] == sym_chunks[symbol].size()) {
            auto status = process_data(symbol);
            if (!status.ok()) {
                {
                    std::lock_guard lock{status_m};
                    sym_status[symbol] = HistBarStatus::error;
                }
                logger->warn("in process_hist_bar_end - when processing for {}, error occurred:{}", symbol, status.ToString());
            } else {
                logger->info("in process_hist_bar_end - data saved for {}", symbol);
                {
                    std::lock_guard lock{status_m};
                    if (sym_status[symbol] == HistBarStatus::in_prog) {
                        sym_status[symbol] = HistBarStatus::success;
                        --remaining;
                    }
                }
            }
        } else { // process next chunk
            std::size_t &chunk_id = sym_chunk_id[symbol];
            auto chunk = sym_chunks[symbol][chunk_id];
            q.wait_push(chunk);
        }
    }

    void HistBarGetter::process_err_msg(Event const &event) {
        auto err_msg = std::static_pointer_cast<ErrMsg>(event.data);
        bool flag{false};
        std::string symbol;
        {
            std::lock_guard lock{req_id_m};
            if (req_id_sym.contains(err_msg->id)) {
                flag = true;
                symbol = req_id_sym[err_msg->id];
            }
        }
        if (flag) {
            {
                std::lock_guard lock{status_m};
                if (sym_status[symbol] == HistBarStatus::in_prog) --remaining;
                sym_status[symbol] = HistBarStatus::error;
            }
            logger->warn("in process_err_msg - symbol:{}, error:{}", symbol, to_string(*err_msg));
        }
    }

    arrow::Status HistBarGetter::process_data(const std::string &symbol) {
        logger->info("In process_data - merging and saving data for {}", symbol);
        auto& data = sym_data[symbol];
        // sort data
        std::sort(data.begin(), data.end(), [](HistBar const& lhs, HistBar const& rhs) {
            return lhs.time < rhs.time;
        });

        // remove duplicate
        data.erase(std::unique(data.begin(), data.end(), [](HistBar const& lhs, HistBar const& rhs) {
            return lhs.time == rhs.time;
        }), data.end());

        std::size_t sz = data.size();

        arrow::StringBuilder time_builder;
        ARROW_RETURN_NOT_OK(time_builder.Reserve(sz));
        arrow::DoubleBuilder high_builder;
        ARROW_RETURN_NOT_OK(high_builder.Reserve(sz));
        arrow::DoubleBuilder low_builder;
        ARROW_RETURN_NOT_OK(low_builder.Reserve(sz));
        arrow::DoubleBuilder open_builder;
        ARROW_RETURN_NOT_OK(open_builder.Reserve(sz));
        arrow::DoubleBuilder close_builder;
        ARROW_RETURN_NOT_OK(close_builder.Reserve(sz));
        arrow::DoubleBuilder wap_builder;
        ARROW_RETURN_NOT_OK(wap_builder.Reserve(sz));
        arrow::DoubleBuilder volume_builder;
        ARROW_RETURN_NOT_OK(volume_builder.Reserve(sz));
        arrow::Int32Builder count_builder;
        ARROW_RETURN_NOT_OK(count_builder.Reserve(sz));

        for (auto const &d: data) {
            ARROW_RETURN_NOT_OK(time_builder.Append(d.time));
            ARROW_RETURN_NOT_OK(open_builder.Append(d.open));
            ARROW_RETURN_NOT_OK(high_builder.Append(d.high));
            ARROW_RETURN_NOT_OK(low_builder.Append(d.low));
            ARROW_RETURN_NOT_OK(close_builder.Append(d.close));
            ARROW_RETURN_NOT_OK(wap_builder.Append(d.wap));
            ARROW_RETURN_NOT_OK(volume_builder.Append(d.volume));
            ARROW_RETURN_NOT_OK(count_builder.Append(d.count));
        }

        std::shared_ptr<arrow::Array> time_a, open_a, high_a, low_a, close_a, wap_a, volume_a, count_a;
        ARROW_RETURN_NOT_OK(time_builder.Finish(&time_a));
        ARROW_RETURN_NOT_OK(open_builder.Finish(&open_a));
        ARROW_RETURN_NOT_OK(high_builder.Finish(&high_a));
        ARROW_RETURN_NOT_OK(low_builder.Finish(&low_a));
        ARROW_RETURN_NOT_OK(close_builder.Finish(&close_a));
        ARROW_RETURN_NOT_OK(wap_builder.Finish(&wap_a));
        ARROW_RETURN_NOT_OK(volume_builder.Finish(&volume_a));
        ARROW_RETURN_NOT_OK(count_builder.Finish(&count_a));

        auto schema = arrow::schema({
            arrow::field("time", arrow::utf8()),
            arrow::field("open", arrow::float64()),
            arrow::field("high", arrow::float64()),
            arrow::field("low", arrow::float64()),
            arrow::field("close", arrow::float64()),
            arrow::field("wap", arrow::float64()),
            arrow::field("volume", arrow::float64()),
            arrow::field("count", arrow::int32())
        });

        auto table = arrow::Table::Make(schema, {time_a, open_a, high_a, low_a, close_a, wap_a, volume_a, count_a});
//        // sort based on time
//        arrow::compute::SortOptions sort_options({arrow::compute::SortKey("time", arrow::compute::SortOrder::Ascending)});
//        ARROW_ASSIGN_OR_RAISE(auto sort_indices, arrow::compute::SortIndices(table->GetColumnByName("time"), sort_options));
//        ARROW_ASSIGN_OR_RAISE(auto sorted_table, arrow::compute::Take(table, *sort_indices));
//        table = sorted_table.table();

        // csv writer
        std::shared_ptr<arrow::io::FileOutputStream> out_file;
        std::string const & out_path = sym_req[symbol].save_path;
        ARROW_ASSIGN_OR_RAISE(out_file, arrow::io::FileOutputStream::Open(out_path));
        ARROW_ASSIGN_OR_RAISE(auto csv_writer, arrow::csv::MakeCSVWriter(out_file, table->schema()));
        ARROW_RETURN_NOT_OK(csv_writer->WriteTable(*table));
        ARROW_RETURN_NOT_OK(csv_writer->Close());

        return arrow::Status::OK();
    }

    void HistBarGetter::register_cb(EventType const& e_type, std::string const& cb_name, void(HistBarGetter::*cb)(Event const&)) {
        event_engine.register_cb(e_type, cb_name, [this, cb](Event const& event) {(this->*cb)(event);});
    }

    void HistBarGetter::log_status() {
        std::unordered_map<std::string, HistBarStatus> status;
        {
            std::lock_guard lock{status_m};
            status = sym_status;
        }
        std::string status_str;
        for (auto const&[k, v]: status) {
            status_str += fmt::format("{}:{}, ", k, static_cast<int>(v));
        }
        status_str.pop_back();
        status_str.pop_back();
        logger->info("in log_status - The final status is {}", status_str);
    };


}