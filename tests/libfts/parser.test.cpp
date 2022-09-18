#include <libfts/parser.hpp>

#include <gtest/gtest.h>

#include <iostream>

TEST(ParserTest, ParseTextWithSpaces) {
    const size_t c_min_ngram_length = 3;
    const size_t c_max_ngram_length = 7;
    std::string text = "                \t";
    libfts::ParserConfiguration config;
    config.init(
        "../../../stopwords.dat", c_min_ngram_length, c_max_ngram_length);
    std::multimap<libfts::Position, libfts::Ngram> result =
        libfts::parse(text, config);
    EXPECT_EQ(result.size(), 0);
}

TEST(ParserTest, ParseTypicalText) {
    const size_t c_min_ngram_length = 3;
    const size_t c_max_ngram_length = 5;
    std::string text = "Life is a long lesson in humility!..";
    std::string expected_ngrams[10] = {
        "lif",
        "life",
        "lon",
        "long",
        "les",
        "less",
        "lesso",
        "hum",
        "humi",
        "humil"};
    int expected_positions[10] = {0, 0, 1, 1, 2, 2, 2, 3, 3, 3};
    libfts::ParserConfiguration config;
    config.init(
        "../../../stopwords.dat", c_min_ngram_length, c_max_ngram_length);
    std::multimap<libfts::Position, libfts::Ngram> result =
        libfts::parse(text, config);
    EXPECT_EQ(result.size(), 10);
    int i = 0;
    for (const auto &[position, ngram] : result) {
        EXPECT_EQ(ngram, expected_ngrams[i]);
        EXPECT_EQ(position, expected_positions[i]);
        std::cout << position << " " << expected_positions[i] << "\n";
        ++i;
    }
}

TEST(ParserTest, InitParserConfiguration) {
    const size_t c_min_ngram_length = 3;
    const size_t c_max_ngram_length = 8;
    libfts::ParserConfiguration config;
    config.init(
        "../../../stopwords.dat", c_min_ngram_length, c_max_ngram_length);
    EXPECT_EQ(config.min_ngram_length_, c_min_ngram_length);
    EXPECT_EQ(config.max_ngram_length_, c_max_ngram_length);
    EXPECT_EQ(config.stop_words_.size(), 35);
}