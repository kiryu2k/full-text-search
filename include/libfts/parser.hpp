#pragma once

#include <filesystem>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace libfts {

class ConfigurationException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class ParserConfiguration {
private:
    std::set<std::string> stop_words_;
    size_t min_ngram_length_;
    size_t max_ngram_length_;

public:
    ParserConfiguration(
        std::set<std::string> stop_words,
        size_t min_ngram_length,
        size_t max_ngram_length);
    size_t get_min_ngram_length() const { return min_ngram_length_; }
    size_t get_max_ngram_length() const { return max_ngram_length_; }
    const std::set<std::string> &get_stop_words() const { return stop_words_; }
};

struct ParsedString {
    size_t text_position_ = 0;
    std::vector<std::string> ngrams_;
};

ParserConfiguration
load_config(const std::filesystem::path &filename = "ParserConfig.json");

std::vector<ParsedString>
parse(std::string text, const ParserConfiguration &config);

} // namespace libfts