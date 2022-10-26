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
    try {
        libfts::IndexAccessor accessor(idx.get_index());
        std::string query = "hello world";
        libfts::ScoreTable expected_score = {
            {100, log(3.0 / 2) * 2, "Hello World"},
            {101, log(3.0 / 2), "Byeee World"},
            {102, log(3.0 / 2), "Hello Earth"}};
        auto score_tab = libfts::search(query, config, accessor);
        for (size_t i = 0; i < score_tab.size(); ++i) {
            EXPECT_EQ(
                score_tab[i].document_id_, expected_score[i].document_id_);
            EXPECT_EQ(score_tab[i].score_, expected_score[i].score_);
            EXPECT_EQ(score_tab[i].text_, expected_score[i].text_);
        }
        EXPECT_EQ(
            libfts::get_string_search_result(score_tab),
            "\tid\tscore\ttext\n\t100\t0.8109302162163288\tHello "
            "World\n\t101\t0.4054651081081644\tByeee "
            "World\n\t102\t0.4054651081081644\tHello Earth\n");
    } catch (libfts::AccessorException &ex) {
    };
}

TEST(SearchTest, CalculateAnotherOneScore) {
    std::set<std::string> stop_words;
    size_t min_ngram_len = 3;
    size_t max_ngram_len = 4;
    libfts::ParserConfiguration config(
        stop_words, min_ngram_len, max_ngram_len);
    libfts::IndexBuilder idx;
    idx.add_document(100, "Hel Wor", config);
    idx.add_document(101, "Bye Wor", config);
    idx.add_document(102, "Hel Ear", config);
    try {
        libfts::IndexAccessor accessor(idx.get_index());
        std::string query = "bye ear";
        libfts::ScoreTable expected_score = {
            {101, log(3.0 / 1), "Bye Wor"}, {102, log(3.0 / 1), "Hel Ear"}};
        auto score_tab = libfts::search(query, config, accessor);
        for (size_t i = 0; i < score_tab.size(); ++i) {
            EXPECT_EQ(
                score_tab[i].document_id_, expected_score[i].document_id_);
            EXPECT_EQ(score_tab[i].score_, expected_score[i].score_);
            EXPECT_EQ(score_tab[i].text_, expected_score[i].text_);
        }
        EXPECT_EQ(
            libfts::get_string_search_result(score_tab),
            "\tid\tscore\ttext\n\t101\t1.0986122886681098\tBye "
            "Wor\n\t102\t1.0986122886681098\tHel Ear\n");
    } catch (libfts::AccessorException &ex) {
    };
}