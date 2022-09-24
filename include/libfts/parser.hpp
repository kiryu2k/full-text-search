#pragma once

#include <map>
#include <set>
#include <string>

namespace libfts {

class ParserConfiguration {
private:
    std::set<std::string> stop_words_;
    size_t min_ngram_length_;
    size_t max_ngram_length_;
    std::string parse_result_;

public:
    explicit ParserConfiguration(
        const std::string &filename = "ParserConfig.json");
    size_t get_min_ngram_length() const { return min_ngram_length_; }
    size_t get_max_ngram_length() const { return max_ngram_length_; }
    const std::set<std::string> &get_stop_words() const { return stop_words_; }
    std::string get_parser_result() { return parse_result_; }
};

using Position = int;
using Ngram = std::string;

std::multimap<Position, Ngram>
parse(std::string text, const ParserConfiguration &config);

std::string get_string_ngrams(const std::multimap<Position, Ngram> &ngrams);

} // namespace libfts