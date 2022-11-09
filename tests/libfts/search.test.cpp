#include <libfts/search.hpp>

#include <gtest/gtest.h>

#include <cmath>

TEST(SearchTest, CalculateScore) {
    const std::set<std::string> stop_words;
    const size_t min_ngram_len = 5;
    const size_t max_ngram_len = 7;
    const double cutoff_factor = 0;
    const libfts::ParserConfiguration config(
        stop_words, min_ngram_len, max_ngram_len, cutoff_factor);
    libfts::IndexBuilder idx;
    idx.add_document(100, "Hello World", config);
    idx.add_document(101, "Byeee World", config);
    idx.add_document(102, "Hello Earth", config);
    try {
        const libfts::IndexAccessor accessor(idx.get_index());
        const std::string query = "hello world";
        const libfts::Results expected_result = {
            {100, log(3.0 / 2) * 2, "Hello World"},
            {101, log(3.0 / 2), "Byeee World"},
            {102, log(3.0 / 2), "Hello Earth"}};
        const auto result = libfts::search(query, config, accessor);
        for (size_t i = 0; i < result.size(); ++i) {
            EXPECT_EQ(result[i].document_id_, expected_result[i].document_id_);
            EXPECT_EQ(result[i].score_, expected_result[i].score_);
            EXPECT_EQ(result[i].text_, expected_result[i].text_);
        }
        EXPECT_EQ(
            libfts::get_string_search_result(result),
            "1\t0.8109302162163288\t100\tHello "
            "World\n2\t0.4054651081081644\t101\tByeee "
            "World\n3\t0.4054651081081644\t102\tHello Earth\n");
    } catch (const libfts::AccessorException &ex) {
    };
}

TEST(SearchTest, CalculateAnotherOneScore) {
    const std::set<std::string> stop_words;
    const size_t min_ngram_len = 3;
    const size_t max_ngram_len = 4;
    const double cutoff_factor = 0;
    const libfts::ParserConfiguration config(
        stop_words, min_ngram_len, max_ngram_len, cutoff_factor);
    libfts::IndexBuilder idx;
    idx.add_document(100, "Hel Wor", config);
    idx.add_document(101, "Bye Wor", config);
    idx.add_document(102, "Hel Ear", config);
    try {
        const libfts::IndexAccessor accessor(idx.get_index());
        const std::string query = "bye ear";
        const libfts::Results expected_result = {
            {101, log(3.0 / 1), "Bye Wor"}, {102, log(3.0 / 1), "Hel Ear"}};
        const auto result = libfts::search(query, config, accessor);
        for (size_t i = 0; i < result.size(); ++i) {
            EXPECT_EQ(result[i].document_id_, expected_result[i].document_id_);
            EXPECT_EQ(result[i].score_, expected_result[i].score_);
            EXPECT_EQ(result[i].text_, expected_result[i].text_);
        }
        EXPECT_EQ(
            libfts::get_string_search_result(result),
            "1\t1.0986122886681098\t101\tBye "
            "Wor\n2\t1.0986122886681098\t102\tHel Ear\n");
    } catch (const libfts::AccessorException &ex) {
    };
}