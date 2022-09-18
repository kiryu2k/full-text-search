#include <libfts/parser.hpp>

/* until the parser configuration file is ready, I added global constants to
 * change some parser parameters */
const size_t c_min_ngram_length = 3;
const size_t c_max_ngram_length = 7;

int main() {
    std::string text =
        "yOu aRE such a @#!k. I can't understand yOU ANYmore!!((((((";
    libfts::ParserConfiguration config;
    config.init("stopwords.dat", c_min_ngram_length, c_max_ngram_length);
    std::multimap<int, std::string> result = libfts::parse(text, config);
    libfts::print_ngrams(result);
    return 0;
}