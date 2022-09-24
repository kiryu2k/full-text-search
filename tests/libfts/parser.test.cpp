#include <libfts/parser.hpp>

#include <gtest/gtest.h>

TEST(ParserTest, ParseTextWithSpaces) {
    std::string text = "                \t";
    libfts::ParserConfiguration config("../../../ParserConfig.json");
    std::vector<libfts::ParsedString> result = libfts::parse(text, config);
    EXPECT_EQ(result.size(), 0);
    std::string expected_string_ngrams = "";
    EXPECT_EQ(libfts::get_string_ngrams(result), expected_string_ngrams);
}

TEST(ParserTest, ParseTypicalText) {
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
    int expected_positions[] = {0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3};
    libfts::ParserConfiguration config("../../../ParserConfig.json");
    std::vector<libfts::ParsedString> result = libfts::parse(text, config);
    // EXPECT_EQ(result.size(), 13);
    // int i = 0;
    // for (const auto &[position, ngram] : result) {
    //     EXPECT_EQ(ngram, expected_ngrams[i]);
    //     EXPECT_EQ(position, expected_positions[i]);
    //     ++i;
    // }
    std::string expected_string_ngrams =
        "lif 0 life 0 lon 1 long 1 les 2 less 2 lesso 2 lesson 2 hum 3 humi 3 "
        "humil 3 humili 3 humilit 3 ";
    EXPECT_EQ(libfts::get_string_ngrams(result), expected_string_ngrams);
}

TEST(ParserTest, IncorrectParserConfigPath) {
    libfts::ParserConfiguration config(
        "../../../../../../../../ParserConfig.json");
    std::string expected_parser_result =
        "parse error: incorrect configuration format";
    EXPECT_EQ(config.get_parser_result(), expected_parser_result);
}