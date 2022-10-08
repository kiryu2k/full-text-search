#include <libfts/parser.hpp>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <fmt/core.h>

int main(int argc, char **argv) {
    CLI::App app{"wow, it's working!!"};
    std::string text;
    app.add_option<std::string>("-t,--text", text, "Parse typed text")
        ->required();
    CLI11_PARSE(app, argc, argv);
    try {
        libfts::ParserConfiguration config = libfts::load_config();
        std::vector<libfts::ParsedString> result = libfts::parse(text, config);
        for (const auto &word : result) {
            fmt::print("position in text: {}\nngrams: ", word.text_position_);
            for (const auto &ngram : word.ngrams_) {
                fmt::print("{} ", ngram);
            }
            fmt::print("\n\n");
        }
    } catch (libfts::ConfigurationException &ex) {
        fmt::print("parse error: {}\n", ex.what());
        return -1;
    };
    return 0;
}