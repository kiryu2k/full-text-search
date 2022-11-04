#include <libfts/absolutepath.hpp>
#include <libfts/search.hpp>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <fast-cpp-csv-parser/csv.h>
#include <fmt/core.h>
#include <replxx.hxx>

int main(int argc, char **argv) {
    CLI::App app{"fts"};
    auto *indexer_sub = app.add_subcommand("index", "Indexer");
    auto *searcher_sub = app.add_subcommand("search", "Searcher");
    std::filesystem::path path;
    std::string query;
    indexer_sub
        ->add_option<std::filesystem::path>(
            "-i,--index", path, "path to folder with indexes")
        ->required();
    searcher_sub
        ->add_option<std::filesystem::path>(
            "-i,--index", path, "path to folder with indexes")
        ->required();
    searcher_sub->add_option<std::string>("-q,--query", query, "search query");
    CLI11_PARSE(app, argc, argv);
    try {
        const auto config =
            libfts::load_config(c_absolute_path / "ParserConfig.json");
        if (indexer_sub->parsed()) {
            libfts::IndexBuilder indexer;
            const size_t column_count = 3;
            io::CSVReader<column_count> csv(c_absolute_path / "books.csv");
            csv.read_header(
                io::ignore_extra_column, "bookID", "title", "language_code");
            libfts::DocId document_id = 0;
            libfts::Doc text;
            std::string language;
            while (csv.read_row(document_id, text, language)) {
                if (language == "eng" || language == "en-US") {
                    indexer.add_document(document_id, text, config);
                }
            }
            libfts::TextIndexWriter::write(
                c_absolute_path / "index", indexer.get_index());
        } else if (searcher_sub->parsed()) {
            if (searcher_sub->count("--query") != 1) {
                replxx::Replxx editor;
                fmt::print("input query text\n");
                query = editor.input("> ");
            }
            libfts::IndexBuilder indexer;
            indexer.read(c_absolute_path / "index");
            libfts::IndexAccessor accessor(indexer.get_index());
            auto result = libfts::search(query, config, accessor);
            fmt::print("{}", libfts::get_string_search_result(result));
        }
    } catch (libfts::ConfigurationException &ex) {
        fmt::print("parse error: {}\n", ex.what());
        return -1;
    } catch (libfts::AccessorException &ex) {
        fmt::print("error: {}\n", ex.what());
        return -1;
    };
    return 0;
}