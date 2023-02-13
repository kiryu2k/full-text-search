#include <libfts/absolutepath.hpp>
#include <libfts/search.hpp>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <fast-cpp-csv-parser/csv.h>
#include <fmt/core.h>
#include <replxx.hxx>

static void launch_indexer(
    const std::filesystem::path &csv_file,
    const libfts::ParserConfiguration &config,
    const std::filesystem::path &index_dir) {
    libfts::IndexBuilder indexer;
    const std::size_t column_count = 3;
    io::CSVReader<column_count> csv(csv_file);
    csv.read_header(
        io::ignore_extra_column, "bookID", "title", "language_code");
    libfts::DocId document_id = 0;
    libfts::Doc text;
    std::string language;
    std::size_t count = 0;
    const std::size_t output_freq = 500;
    while (csv.read_row(document_id, text, language)) {
        if ((language == "eng" || language == "en-US")) {
            indexer.add_document(document_id, text, config);
            if (++count % output_freq == 0) {
                fmt::print("{} documents...\n", count);
            }
        }
    }
    libfts::TextIndexWriter text_writer;
    text_writer.write(index_dir, indexer.get_index());
    libfts::BinaryIndexWriter binary_writer;
    binary_writer.write(index_dir, indexer.get_index());
}

static void launch_searcher(
    const libfts::ParserConfiguration &config,
    const std::filesystem::path &index_dir,
    const std::string &query) {
    try {
        // const libfts::TextIndexAccessor accessor(index_dir);
        libfts::BinaryData index(index_dir);
        libfts::Header header(index.data());
        libfts::BinaryIndexAccessor accessor(index.data(), header);
        const auto result = libfts::search(query, config, accessor);
        fmt::print("{}", libfts::get_string_search_result(result));
    } catch (const libfts::AccessorException &ex) {
        fmt::print("error: {}\n", ex.what());
    }
}

static void launch_interactive_searcher(
    const libfts::ParserConfiguration &config,
    const std::filesystem::path &index_dir) {
    replxx::Replxx editor;
    editor.clear_screen();
    while (true) {
        char const *cinput{nullptr};
        do {
            cinput = editor.input("> ");
        } while ((cinput == nullptr) && (errno == EAGAIN));
        if (cinput == nullptr) {
            break;
        }
        const std::string query = cinput;
        if (query.empty()) {
            /* user hits enter on an empty line */
            continue;
        }
        launch_searcher(config, index_dir, query);
    }
}

int main(int argc, char **argv) {
    CLI::App app{"fts"};
    auto *indexer_sub = app.add_subcommand("index", "Indexer");
    auto *searcher_sub = app.add_subcommand("search", "Searcher");
    std::filesystem::path index_dir;
    std::filesystem::path csv_file;
    std::string query;
    indexer_sub
        ->add_option<std::filesystem::path>(
            "-i,--index", index_dir, "path to folder with indexes")
        ->required();
    indexer_sub
        ->add_option<std::filesystem::path>(
            "-c,--csv",
            csv_file,
            "path to csv file in which we're gonna search something")
        ->required();
    searcher_sub
        ->add_option<std::filesystem::path>(
            "-i,--index", index_dir, "path to folder with indexes")
        ->required();
    searcher_sub->add_option<std::string>("-q,--query", query, "search query");
    CLI11_PARSE(app, argc, argv);
    try {
        const std::filesystem::path path(c_absolute_path);
        const auto config = libfts::load_config(path / "ParserConfig.json");
        if (indexer_sub->parsed()) {
            launch_indexer(path / csv_file, config, path / index_dir);
        } else if (searcher_sub->parsed()) {
            if (searcher_sub->count("--query") != 1) {
                launch_interactive_searcher(config, path / index_dir);
            } else {
                launch_searcher(config, path / index_dir, query);
            }
        }
    } catch (const libfts::ConfigurationException &ex) {
        fmt::print("parse error: {}\n", ex.what());
        return -1;
    } catch (const std::filesystem::filesystem_error &ex) {
        fmt::print("{}\n", ex.what());
        return -1;
    }
    return 0;
}