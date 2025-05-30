#include <arrow/api.h>
#include <arrow/csv/api.h>
#include <arrow/io/api.h>
#include <arrow/result.h>
#include <arrow/status.h>
#include <arrow/table.h>
#include <parquet/arrow/writer.h>

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <inputfilename> [outputfilename]\n";
        return EXIT_FAILURE;
    }

    std::string inputFilename = argv[1];
    std::string outputFilename = (argc > 2) ? argv[2] : "data.parquet";

    try {
        // Open the CSV file
        auto input_result = arrow::io::ReadableFile::Open(inputFilename);
        if (!input_result.ok()) {
            throw std::runtime_error("Could not open file: " + input_result.status().ToString());
        }
        std::shared_ptr<arrow::io::ReadableFile> input = *input_result;

        // Configure CSV reading options
        auto read_options = arrow::csv::ReadOptions::Defaults();
        auto parse_options = arrow::csv::ParseOptions::Defaults();
        parse_options.delimiter = ',';

        auto convert_options = arrow::csv::ConvertOptions::Defaults();
        // Force all columns to be read as UInt8
        for (int i = 0; i < 10000; i++) {
            convert_options.column_types[std::to_string(i)] = arrow::uint8();
        }

        // Read the CSV directly into a table
        auto reader_result = arrow::csv::TableReader::Make(
            arrow::io::default_io_context(), input, read_options, parse_options, convert_options);
        if (!reader_result.ok()) {
            throw std::runtime_error("Could not create CSV reader: "
                                     + reader_result.status().ToString());
        }
        auto reader = *reader_result;

        auto table_result = reader->Read();
        if (!table_result.ok()) {
            throw std::runtime_error("Could not read CSV: " + table_result.status().ToString());
        }
        std::shared_ptr<arrow::Table> table = *table_result;

        // Rest of the code remains the same...
        auto metadata = std::make_shared<arrow::KeyValueMetadata>();
        metadata->Append("data_size", std::to_string(table->num_rows()));
        metadata->Append("features_size", std::to_string(table->num_columns()));

        // Compute value ranges for each column...

        // Write to Parquet
        auto outfile_result = arrow::io::FileOutputStream::Open(outputFilename);
        if (!outfile_result.ok()) {
            throw std::runtime_error("Could not open output file: "
                                     + outfile_result.status().ToString());
        }
        std::shared_ptr<arrow::io::FileOutputStream> outfile = *outfile_result;

        auto builder = std::make_shared<parquet::WriterProperties::Builder>();
        builder->compression(parquet::Compression::SNAPPY)->enable_dictionary();

        auto status = parquet::arrow::WriteTable(
            *table, arrow::default_memory_pool(), outfile, 65536, builder->build());

        if (!status.ok()) {
            throw std::runtime_error("Could not write Parquet file: " + status.ToString());
        }

        std::cout << "Successfully converted " << table->num_rows() << " rows and "
                  << table->num_columns() << " columns to " << outputFilename << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}