#include <libfts/parser.hpp>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <vector>

namespace libfts {

ParserConfiguration::ParserConfiguration(const std::string &filename) {
    std::ifstream file(filename);
    nlohmann::json config = nlohmann::json::parse(file, nullptr, false);
    if (config.is_discarded()) {
        parse_result = "parse error: incorrect configuration format";
    } else if (
        config["minimum ngram length"].get<int>() < 0 ||
        config["maximum ngram length"].get<int>() < 0) {
        parse_result = "parse error: ngram lengths must be unsigned integers";
    } else if (
        config["minimum ngram length"].get<int>() >
        config["maximum ngram length"].get<int>()) {
        parse_result = "parse error: maximum ngram length must be greater "
                       "than minimum ngram length";
    } else {
        min_ngram_length_ = config["minimum ngram length"].get<size_t>();
        max_ngram_length_ = config["maximum ngram length"].get<size_t>();
        stop_words_ = config["stop words"].get<std::set<std::string>>();
        parse_result = "successful";
    }
    file.close();
}

static void remove_punct(std::string &str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::ispunct), str.end());
}

static void string_to_lower(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char chr) {
        return std::tolower(chr);
    });
}

static std::vector<std::string> split_string(std::string &str) {
    std::vector<std::string> arr;
    auto first = std::find_if_not(str.begin(), str.end(), isspace);
    while (first != std::end(str)) {
        auto last = std::find_if(first, str.end(), isspace);
        std::string tmp = str.substr(first - str.begin(), last - first);
        arr.push_back(tmp);
        first = std::find_if_not(last, str.end(), isspace);
    }
    return arr;
}

static std::vector<std::string> remove_stop_words(
    const std::vector<std::string> &text,
    const std::set<std::string> &stop_words) {
    std::vector<std::string> text_without_stop_words;
    for (const auto &word : text) {
        if (stop_words.find(word) == stop_words.end()) {
            text_without_stop_words.push_back(word);
        }
    }
    return text_without_stop_words;
}

static std::multimap<int, std::string> generate_ngrams(
    const std::vector<std::string> &words, const ParserConfiguration &config) {
    std::multimap<int, std::string> ngrams;
    for (size_t i = 0; i < words.size(); ++i) {
        for (size_t j = config.get_min_ngram_length();
             j <= config.get_max_ngram_length();
             ++j) {
            if (words[i].length() < j) {
                break;
            }
            ngrams.insert({i, words[i].substr(0, j)});
        }
    }
    return ngrams;
}

std::multimap<Position, Ngram>
parse(std::string text, const ParserConfiguration &config) {
    remove_punct(text);
    string_to_lower(text);
    std::vector<std::string> words = split_string(text);
    words = remove_stop_words(words, config.get_stop_words());
    std::multimap<Position, Ngram> ngrams = generate_ngrams(words, config);
    return ngrams;
}

std::string get_string_ngrams(const std::multimap<Position, Ngram> &ngrams) {
    std::string string_ngrams;
    for (const auto &[position, ngram] : ngrams) {
        string_ngrams += ngram + " " + std::to_string(position) + " ";
    }
    return string_ngrams;
}

} // namespace libfts