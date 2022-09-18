#pragma once

#include <fstream>
#include <map>
#include <set>
#include <string>

namespace libfts {

struct ParserConfiguration {
    std::set<std::string> stop_words_;
    size_t min_ngram_length_;
    size_t max_ngram_length_;

    void init(
        std::string filename,
        size_t min_ngram_length,
        size_t max_ngram_length) {
        std::ifstream file(filename);
        std::string line;
        while (getline(file, line)) {
            stop_words_.insert(line);
        }
        file.close();
        min_ngram_length_ = min_ngram_length;
        max_ngram_length_ = max_ngram_length;
    }
};

using Position = int;
using Ngram = std::string;

std::multimap<Position, Ngram>
parse(std::string text, const ParserConfiguration &config);

void print_ngrams(const std::multimap<Position, Ngram> &ngrams);

} // namespace libfts