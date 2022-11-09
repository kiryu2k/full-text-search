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

struct NgramLength {
    size_t min_ = 0;
    size_t max_ = 0;
};

class ParserConfiguration {
private:
    std::set<std::string> stop_words_;
    NgramLength ngram_length_;
    double cutoff_factor_;

public:
    ParserConfiguration(
        std::set<std::string> stop_words,
        const NgramLength &ngram_length,
        double cutoff_factor);
    double get_cutoff_factor() const { return cutoff_factor_; }
    size_t get_min_ngram_length() const { return ngram_length_.min_; }
    size_t get_max_ngram_length() const { return ngram_length_.max_; }
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