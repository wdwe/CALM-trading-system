#include <arrow/api.h>
#include <arrow/compute/api.h>
#include <arrow/table.h>
#include <arrow/ipc/api.h>
#include <arrow/csv/api.h>
#include <arrow/io/memory.h>
#include <iostream>
#include <arrow/io/api.h>
#include <fmt/format.h>
#include <limits>

using namespace arrow;
using namespace arrow::compute;

arrow::Status run_main() {
    // Create a column: "id" (int32)
    std::shared_ptr<Array> id_array;
    Int32Builder id_builder;
    ARROW_RETURN_NOT_OK(id_builder.AppendValues({3, 1, 4, 2, 5}));
    ARROW_ASSIGN_OR_RAISE(id_array, id_builder.Finish())

    // Create another column: "value" (double)
    std::shared_ptr<Array> value_array;
    DoubleBuilder value_builder;
    ARROW_RETURN_NOT_OK(value_builder.AppendValues({10.5, 20.7, 15.2, 18.4, std::numeric_limits<double>::quiet_NaN()}));
    ARROW_RETURN_NOT_OK(value_builder.Finish(&value_array));


    // Define table schema
    auto schema = arrow::schema({
                                        field("id", int32()),
                                        field("value", float64())
                                });

    // Create Table
    auto table = Table::Make(schema, {id_array, value_array});

    std::cout << "Original Table:\n";
    std::cout << table->ToString() << std::endl;

    // Define sorting options (sort by "id" in ascending order)
    SortOptions sort_options({SortKey("id", SortOrder::Ascending)});

    // Get sorted indices
    ARROW_ASSIGN_OR_RAISE(auto sort_indices, SortIndices(table->GetColumnByName("id"), sort_options))

    // Apply sorted indices to reorder rows
    ARROW_ASSIGN_OR_RAISE(auto sorted_table, Take(table, *sort_indices))
//    sorted_table.
    std::cout << "Sorted Table:\n";
//    std::cout << sorted_table.ToString() << std::endl;
    std::cout << sorted_table.table()->ToString() << std::endl;

    auto const& csv_table = sorted_table.table();
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    ARROW_ASSIGN_OR_RAISE(outfile, arrow::io::FileOutputStream::Open("test_out.csv"));

    // The CSV writer has simpler defaults, review API documentation for more complex usage.

    ARROW_ASSIGN_OR_RAISE(auto csv_writer,

                          arrow::csv::MakeCSVWriter(outfile, csv_table->schema()));

    ARROW_RETURN_NOT_OK(csv_writer->WriteTable(*csv_table));

    // Not necessary, but a safe practice.

    ARROW_RETURN_NOT_OK(csv_writer->Close());
    return arrow::Status::OK();
}

int main() {
    arrow::Status st = run_main();
    std::cout << st << std::endl;
    std::cout << fmt::format("status is {}", st.ToString()) << std::endl;
    if (!st.ok()) return 1;
    return 0;
}