#include <libfts/absolutepath.hpp>
#include <libfts/indexer.hpp>
#include <libfts/parser.hpp>

#include <gtest/gtest.h>

TEST(IndexerTest, AddMultipleDocuments) {
    const auto config =
        libfts::load_config(ABSOLUTE_PATH + "/ParserConfig.json");
    libfts::IndexBuilder idx;
    idx.add_document(199903, "The Matrix matrix awwww", config);
    idx.add_document(200305, "The Matrix Reloaded", config);
    idx.add_document(200311, "The Matrix Revolutions", config);
    libfts::TextIndexWriter writer;
    writer.write(ABSOLUTE_PATH + "/index/", idx.get_index());
    idx.add_document(200458, "The Matrix in the red bottoms MATR", config);
    writer.write(ABSOLUTE_PATH + "/index/", idx.get_index());
    std::map<libfts::term, libfts::entry> entry;
    libfts::parse_entry(
        ABSOLUTE_PATH + "/index/entries/" + libfts::generate_hash("matrix"),
        entry);
    std::map<libfts::term, libfts::entry> expected_entry;
    expected_entry.insert(
        {"matrix",
         {{199903, {0, 1}}, {200305, {0}}, {200311, {0}}, {200458, {0}}}});
    EXPECT_EQ(entry, expected_entry);
}

TEST(IndexerTest, AddDocumentWithExistingId) {
    const auto config =
        libfts::load_config(ABSOLUTE_PATH + "/ParserConfig.json");
    libfts::IndexBuilder idx;
    idx.add_document(101, "On Hawaii island", config);
    idx.add_document(101, "Mirror, mirror, the reflection of the law", config);
    std::map<libfts::doc_id, libfts::doc> expected_docs = {
        {101, "On Hawaii island"}};
    std::vector<libfts::term> expected_terms = {
        "haw", "hawa", "hawai", "hawaii", "isl", "isla", "islan", "island"};
    std::vector<libfts::pos> expected_pos = {
        libfts::pos{0},
        libfts::pos{0},
        libfts::pos{0},
        libfts::pos{0},
        libfts::pos{1},
        libfts::pos{1},
        libfts::pos{1},
        libfts::pos{1}};
    EXPECT_EQ(idx.get_index().get_docs(), expected_docs);
    size_t i = 0;
    for (const auto &[terms, entries] : idx.get_index().get_entries()) {
        EXPECT_EQ(expected_terms[i], terms);
        for (const auto &[doc_id, pos] : entries) {
            EXPECT_EQ(101, doc_id);
            EXPECT_EQ(expected_pos[i], pos);
        }
        ++i;
    }
}