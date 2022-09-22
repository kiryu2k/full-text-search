#include <libfts/parser.hpp>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <fmt/core.h>

int main(int argc, char **argv) {
    CLI::App app{"wow, it's working!!"};
    std::string text;
    app.add_option<std::string>("--text", text, "text");
    CLI11_PARSE(app, argc, argv);
    if (app.count("--text") != 1) {
        fmt::print(
            "wrong command line arguments\ntry this: ./app --text [string]\n");
        return -1;
    }
    libfts::ParserConfiguration config;
    if (config.get_parser_result() != "successful") {
        fmt::print("{}\n", config.get_parser_result());
        return -1;
    }
    std::multimap<libfts::Position, libfts::Ngram> result =
        libfts::parse(text, config);
    fmt::print("{}\n", libfts::get_string_ngrams(result));
    return 0;
}