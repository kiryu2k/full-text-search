#include <libfts/parser.hpp>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <fmt/core.h>
#include <replxx.hxx>

int main(int argc, char **argv) {
    CLI::App app{"fts"};
    auto *indexer_sub = app.add_subcommand("index", "Indexer");
    auto *searcher_sub = app.add_subcommand("search", "Searcher");
    std::string path;
    std::string query;
    indexer_sub
        ->add_option<std::string>(
            "-i,--index", path, "path to folder with indexes")
        ->required();
    searcher_sub
        ->add_option<std::string>(
            "-i,--index", path, "path to folder with indexes")
        ->required();
    searcher_sub->add_option<std::string>("-q,--query", query, "search query");
    CLI11_PARSE(app, argc, argv);
    try {
        if (indexer_sub->parsed()) {
            fmt::print("indexer!!!!\n");
        } else if (searcher_sub->parsed()) {
            fmt::print("searcher!!!!\n");
            if (searcher_sub->count("--query") != 1) {
                replxx::Replxx editor;
                fmt::print("input query text\n");
                query = editor.input("> ");
            }
            fmt::print("query: {}\n", query);
        }
    } catch (libfts::ConfigurationException &ex) {
        fmt::print("parse error: {}\n", ex.what());
        return -1;
    };
    return 0;
}