#include <libfts/parser.hpp>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <fmt/core.h>

static std::string
get_string_ngrams(const std::vector<libfts::ParsedString> &words) {
    std::string string_ngrams;
    for (const auto &word : words) {
        size_t pos = word.text_position_;
        for (const auto &ngram : word.ngrams_) {
            string_ngrams += ngram + " " + std::to_string(pos) + " ";
        }
    }
    return string_ngrams;
}

int main(int argc, char **argv) {
    CLI::App app{"wow, it's working!!"};
    std::string text;
    app.add_option<std::string>("-t,--text", text, "Parse typed text")
        ->required();
    CLI11_PARSE(app, argc, argv);
    if (app.count("--text") != 1) {
        fmt::print("{}", app.help());
        return -1;
    }
    try {
        libfts::ParserConfiguration config = libfts::load_config();
        std::vector<libfts::ParsedString> result = libfts::parse(text, config);
        fmt::print("{}\n", get_string_ngrams(result));
    } catch (libfts::ConfigurationException &ex) {
        fmt::print("parse error: {}\n", ex.what());
        return -1;
    };
    return 0;
}