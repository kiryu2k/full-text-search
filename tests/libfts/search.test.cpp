#include <libfts/search.hpp>

#include <gtest/gtest.h>

#include <cmath>

TEST(SearchTest, CalculateScore) {
    std::set<std::string> stop_words;
    size_t min_ngram_len = 5;
    size_t max_ngram_len = 7;
    libfts::ParserConfiguration config(
        stop_words, min_ngram_len, max_ngram_len);
    libfts::IndexBuilder idx;
    idx.add_document(100, "Hello World", config);
    idx.add_document(101, "Byeee World", config);
    idx.add_document(102, "Hello Earth", config);
    libfts::IndexAccessor accessor(idx.get_index());
    std::string query = "hello world";
    libfts::ScoreTable expected_score = {
        {100, log(3.0 / 2) * 2}, {101, log(3.0 / 2)}, {102, log(3.0 / 2)}};
    EXPECT_EQ(libfts::calculate_score(query, config, accessor), expected_score);
}