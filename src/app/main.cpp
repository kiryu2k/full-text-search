#include <libfts/absolutepath.hpp>
#include <libfts/search.hpp>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <fast-cpp-csv-parser/csv.h>
#include <fmt/core.h>
#include <replxx.hxx>

static void launch_indexer(
    const libfts::ParserConfiguration &config,
    const std::filesystem::path &path) {
    libfts::IndexBuilder indexer;
    const size_t column_count = 3;
    io::CSVReader<column_count> csv(path / "books.csv");
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
    libfts::TextIndexWriter::write(path / "index", indexer.get_index());
}

static void launch_searcher(
    const libfts::ParserConfiguration &config,
    const std::filesystem::path &path,
    std::string &query) {
    try {
        libfts::IndexAccessor accessor(
            libfts::TextIndexReader::read(path / "index"));
        auto result = libfts::search(query, config, accessor);
        fmt::print("{}", libfts::get_string_search_result(result));
    } catch (libfts::AccessorException &ex) {
        fmt::print("error: {}\n", ex.what());
    }
}

static void launch_interactive_searcher(
    const libfts::ParserConfiguration &config,
    const std::filesystem::path &path,
    std::string &query) {
    replxx::Replxx editor;
    while (true) {
        char const *cinput{nullptr};
        do {
            cinput = editor.input("> ");
        } while ((cinput == nullptr) && (errno == EAGAIN));
        if (cinput == nullptr) {
            break;
        }
        query = cinput;
        if (query.empty()) {
            /* user hits enter on an empty line */
            continue;
        }
        if (query.compare(0, 5, ".quit") == 0 ||
            query.compare(0, 5, ".exit") == 0) {
            break;
        }
        if (query.compare(0, 6, ".clear") == 0) {
            /* clear the screen */
            editor.clear_screen();
            continue;
        }
        launch_searcher(config, path, query);
    }
}

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
        std::filesystem::path path(c_absolute_path);
        const auto config = libfts::load_config(path / "ParserConfig.json");
        if (indexer_sub->parsed()) {
            launch_indexer(config, path);
        } else if (searcher_sub->parsed()) {
            if (searcher_sub->count("--query") != 1) {
                launch_interactive_searcher(config, path, query);
            } else {
                launch_searcher(config, path, query);
            }
        }
    } catch (libfts::ConfigurationException &ex) {
        fmt::print("parse error: {}\n", ex.what());
        return -1;
    };
    return 0;
}