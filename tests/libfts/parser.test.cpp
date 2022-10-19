#include <libfts/absolutepath.hpp>
#include <libfts/parser.hpp>

#include <gtest/gtest.h>

static std::string
get_string_ngrams(const std::vector<libfts::ParsedString> &words) {
    std::string string_ngrams;
    for (const auto &word : words) {
        size_t pos = word.text_position_;
        for (const auto &ngram : word.ngrams_) {
            string_ngrams += ngram + " " + std::to_string(pos) + " ";
        }
    }
    return string_ngrams;
}

TEST(ParserTest, ParseTextWithSpaces) {
    try {
        std::string text = "                \t";
        libfts::ParserConfiguration config =
            libfts::load_config(c_absolute_path / "ParserConfig.json");
        std::vector<libfts::ParsedString> result = libfts::parse(text, config);
        EXPECT_EQ(result.size(), 0);
        std::string expected_string_ngrams = "";
        EXPECT_EQ(get_string_ngrams(result), expected_string_ngrams);
    } catch (libfts::ConfigurationException &ex) {
    };
}

TEST(ParserTest, ParseTypicalText) {
    try {
        std::string text = "Life is a long lesson in humility!..";
        std::string expected_ngrams[] = {
            "lif",
            "life",
            "lon",
            "long",
            "les",
            "less",
            "lesso",
            "lesson",
            "hum",
            "humi",
            "humil",
            "humili",
            "humilit"};
        int expected_positions[] = {0, 1, 2, 3};
        libfts::ParserConfiguration config =
            libfts::load_config(c_absolute_path / "ParserConfig.json");
        std::vector<libfts::ParsedString> result = libfts::parse(text, config);
        int i = 0, j = 0;
        for (const auto &word : result) {
            for (const auto &ngram : word.ngrams_) {
                EXPECT_EQ(ngram, expected_ngrams[j]);
                ++j;
            }
            EXPECT_EQ(word.text_position_, expected_positions[i]);
            ++i;
        }
        EXPECT_EQ(result.size(), i);
        std::string expected_string_ngrams =
            "lif 0 life 0 lon 1 long 1 les 2 less 2 lesso 2 lesson 2 hum 3 "
            "humi 3 humil 3 humili 3 humilit 3 ";
        EXPECT_EQ(get_string_ngrams(result), expected_string_ngrams);
    } catch (libfts::ConfigurationException &ex) {
    };
}