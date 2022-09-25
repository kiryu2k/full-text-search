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
    try {
        libfts::ParserConfiguration config = libfts::load_config();
        std::vector<libfts::ParsedString> result = libfts::parse(text, config);
        fmt::print("{}\n", libfts::get_string_ngrams(result));
    } catch (libfts::ConfigurationException &ex) {
        std::cerr << "parse error: " << ex.what() << "\n";
        return -1;
    };
    return 0;
}