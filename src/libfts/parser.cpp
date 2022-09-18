#include <libfts/parser.hpp>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <vector>

namespace libfts {

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
        for (size_t j = config.min_ngram_length_; j <= config.max_ngram_length_;
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
    words = remove_stop_words(words, config.stop_words_);
    std::multimap<Position, Ngram> ngrams = generate_ngrams(words, config);
    return ngrams;
}

void print_ngrams(const std::multimap<Position, Ngram> &ngrams) {
    for (const auto &[position, ngram] : ngrams) {
        std::cout << ngram << " " << position << " ";
    }
    std::cout << "\n";
}

} // namespace libfts