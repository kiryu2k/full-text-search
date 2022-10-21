#include <libfts/search.hpp>

#include <gtest/gtest.h>

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
    std::map<libfts::DocId, double> expected_score = {
        {100, 0.810930216}, {101, 0.405465108}, {102, 0.405465108}};
    EXPECT_EQ(libfts::search(query, config, accessor), expected_score);
}