#include <libfts/parser.hpp>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <fstream>

namespace libfts {

ParserConfiguration::ParserConfiguration(
    std::set<std::string> stop_words,
    size_t min_ngram_length,
    size_t max_ngram_length)
    : stop_words_{std::move(stop_words)}, min_ngram_length_{min_ngram_length},
      max_ngram_length_{max_ngram_length} {
    if (min_ngram_length >= max_ngram_length) {
        throw ConfigurationException(
            "maximum ngram length must be greater than minimum ngram length");
    }
}

ParserConfiguration load_config(const std::string &filename) {
    std::ifstream file(filename);
    nlohmann::json config = nlohmann::json::parse(file, nullptr, false);
    if (config.is_discarded()) {
        throw ConfigurationException("incorrect configuration format");
    }
    auto min_ngram_length = config["minimum_ngram_length"].get<int>();
    auto max_ngram_length = config["maximum_ngram_length"].get<int>();
    if (min_ngram_length < 0 || max_ngram_length < 0) {
        throw ConfigurationException("ngram lengths must be unsigned integers");
    }
    auto stop_words = config["stop_words"].get<std::set<std::string>>();
    return {
        ParserConfiguration(stop_words, min_ngram_length, max_ngram_length)};
}

static void remove_punct(std::string &str) {
    str.erase(
        std::remove_if(
            str.begin(),
            str.end(),
            [](unsigned char chr) { return std::ispunct(chr); }),
        str.end());
}

static void string_to_lower(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char chr) {
        return std::tolower(chr);
    });
}

static std::vector<std::string> split_string(std::string &str) {
    std::vector<std::string> words;
    auto first = std::find_if_not(str.begin(), str.end(), isspace);
    while (first != str.end()) {
        auto last = std::find_if(first, str.end(), isspace);
        std::string word = str.substr(first - str.begin(), last - first);
        words.push_back(word);
        first = std::find_if_not(last, str.end(), isspace);
    }
    return words;
}

static std::vector<std::string> remove_stop_words(
    const std::vector<std::string> &text,
    const std::set<std::string> &stop_words) {
    std::vector<std::string> text_without_stop_words;
    std::copy_if(
        text.begin(),
        text.end(),
        std::back_inserter(text_without_stop_words),
        [&stop_words](const std::string &word) {
            return stop_words.find(word) == stop_words.end();
        });
    return text_without_stop_words;
}

static std::vector<ParsedString> generate_ngrams(
    const std::vector<std::string> &words, const ParserConfiguration &config) {
    std::vector<ParsedString> parsed_query;
    for (size_t i = 0; i < words.size(); ++i) {
        ParsedString word;
        for (size_t j = config.get_min_ngram_length();
             j <= config.get_max_ngram_length();
             ++j) {
            if (words[i].length() < j) {
                break;
            }
            word.ngrams_.push_back(words[i].substr(0, j));
        }
        if (!word.ngrams_.empty()) {
            word.text_position_ = i;
            parsed_query.push_back(word);
        }
    }
    return parsed_query;
}

std::vector<ParsedString>
parse(std::string text, const ParserConfiguration &config) {
    remove_punct(text);
    string_to_lower(text);
    std::vector<std::string> words = split_string(text);
    words = remove_stop_words(words, config.get_stop_words());
    std::vector<ParsedString> parsed_query = generate_ngrams(words, config);
    return parsed_query;
}

} // namespace libfts