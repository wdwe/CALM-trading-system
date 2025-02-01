#ifndef CALM_TRADER_HISTORICAL_DATA_H
#define CALM_TRADER_HISTORICAL_DATA_H

#include <string>
#include <vector>
#include <mutex>
#include "gateway/gateway.h"
#include "utils/lock_based_queue.h"
#include "utils/logging.h"
#include "spdlog/async_logger.h"
#include <arrow/api.h>

namespace calm {
    struct HistBarReq {
        std::string symbol;
        std::string start;
        std::string end;
        std::string tz;
        std::string wts;
        std::string save_path;
    };
    std::string to_string(HistBarReq const& req);

    struct IBHistBarReq {
        std::string symbol;
        std::string end;
        std::string dur;
        std::string bar_size;
        std::string wts;
    };
    std::string to_string(IBHistBarReq const& req);

    enum class HistBarStatus {
        in_prog,
        success,
        error
    };


    class HistBarGetter {
    public:
        HistBarGetter(EventEngine& event_engine, IBGateway& gateway, std::vector<HistBarReq> const& reqs);
        void run();
    private:
        std::vector<IBHistBarReq> generate_ib_reqs(HistBarReq const& bar_req, int days=1);
        void process_hist_bar(Event const& event);
        void process_hist_bar_end(Event const& event);
        void process_err_msg(Event const& event);
        arrow::Status process_data(std::string const& symbol);
        void register_cb(EventType const& e_type, std::string const& cb_name, void(HistBarGetter::*cb)(Event const&));
        void log_status();


        std::shared_ptr<spdlog::async_logger> logger;
        EventEngine& event_engine;
        IBGateway &gateway;
        std::mutex req_id_m;
        std::mutex status_m;
        LBQueue<IBHistBarReq> q;
        std::unordered_map<std::string, HistBarReq> sym_req;
        std::unordered_map<std::string, std::vector<IBHistBarReq>> sym_chunks;
        std::unordered_map<std::string, std::size_t> sym_chunk_id;
        std::unordered_map<std::string, HistBarStatus> sym_status;
        std::unordered_map<TickerId, std::string> req_id_sym;
        std::size_t remaining;
        std::unordered_map<std::string, std::vector<HistBar>> sym_data;

    };

}




#endif //CALM_TRADER_HISTORICAL_DATA_H
