#include <libfts/parser.hpp>

#include <iostream>

int main() {
    std::string text =
        "yOu aRE such a @#!k. I can't understand yOU ANYmore!!((((((";
    libfts::ParserConfiguration config;
    if (config.get_parser_result() != "successful") {
        std::cout << config.get_parser_result() << "\n";
        return -1;
    }
    std::multimap<libfts::Position, libfts::Ngram> result =
        libfts::parse(text, config);
    std::cout << libfts::get_string_ngrams(result) << "\n";
    return 0;
}