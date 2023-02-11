#include <libfts/absolutepath.hpp>
#include <libfts/indexer.hpp>
#include <libfts/parser.hpp>

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>

TEST(IndexerTest, AddMultipleDocuments) {
    const std::filesystem::path path(c_absolute_path);
    const auto config = libfts::load_config(path / "ParserConfig.json");
    libfts::IndexBuilder idx;
    idx.add_document(199903, "The Matrix matrix awwww", config);
    idx.add_document(200305, "The Matrix Reloaded", config);
    idx.add_document(200311, "The Matrix Revolutions", config);
    char dir_name_template[] = "/tmp/index_XXXXXX";
    char *temporary_dir_name = mkdtemp(dir_name_template);
    libfts::TextIndexWriter writer;
    writer.write(temporary_dir_name, idx.get_index());
    idx.add_document(200458, "The Matrix in the red bottoms MATR", config);
    writer.write(temporary_dir_name, idx.get_index());
    const auto idx2 = libfts::TextIndexReader::read(temporary_dir_name);
    EXPECT_EQ(idx.get_index().get_docs(), idx2.get_docs());
    EXPECT_EQ(idx.get_index().get_entries(), idx2.get_entries());
    std::filesystem::remove_all(temporary_dir_name);
}

TEST(IndexerTest, AddDocumentWithExistingId) {
    const std::filesystem::path path(c_absolute_path);
    const auto config = libfts::load_config(path / "ParserConfig.json");
    libfts::IndexBuilder idx;
    idx.add_document(101, "On Hawaii island", config);
    idx.add_document(101, "Mirror, mirror, the reflection of the law", config);
    const std::map<libfts::DocId, libfts::Doc> expected_docs = {
        {101, "On Hawaii island"}};
    const std::map<libfts::Term, libfts::Entry> expected_entries = {
        {"haw", {{101, libfts::Pos{0}}}},
        {"hawa", {{101, libfts::Pos{0}}}},
        {"hawai", {{101, libfts::Pos{0}}}},
        {"hawaii", {{101, libfts::Pos{0}}}},
        {"isl", {{101, libfts::Pos{1}}}},
        {"isla", {{101, libfts::Pos{1}}}},
        {"islan", {{101, libfts::Pos{1}}}},
        {"island", {{101, libfts::Pos{1}}}}};
    EXPECT_EQ(idx.get_index().get_docs(), expected_docs);
    EXPECT_EQ(idx.get_index().get_entries(), expected_entries);
}

TEST(IndexerTest, GetDocumentById) {
    const std::filesystem::path path(c_absolute_path);
    const auto config = libfts::load_config(path / "ParserConfig.json");
    libfts::IndexBuilder idx;
    idx.add_document(100, "The Ides of March", config);
    idx.add_document(101, "The Place Beyond the Pines", config);
    idx.add_document(102, "All Good Things", config);
    char dir_name_template[] = "/tmp/index_XXXXXX";
    char *temporary_dir_name = mkdtemp(dir_name_template);
    libfts::TextIndexWriter text_writer;
    text_writer.write(temporary_dir_name, idx.get_index());
    try {
        const libfts::TextIndexAccessor accessor(temporary_dir_name);
        EXPECT_EQ(accessor.get_document_by_id(100), "The Ides of March");
        EXPECT_EQ(accessor.get_document_by_id(102), "All Good Things");
        EXPECT_EQ(
            accessor.get_document_by_id(101), "The Place Beyond the Pines");
    } catch (const libfts::AccessorException &ex) {
    };
    /* where to place the removal? */
    std::filesystem::remove_all(temporary_dir_name);
}

TEST(IndexerTest, GetDocumentsByTerm) {
    const std::filesystem::path path(c_absolute_path);
    const auto config = libfts::load_config(path / "ParserConfig.json");
    libfts::IndexBuilder idx;
    idx.add_document(100, "Hello World", config);
    idx.add_document(101, "Bye World", config);
    idx.add_document(102, "Hello Earth", config);
    char dir_name_template[] = "/tmp/index_XXXXXX";
    char *temporary_dir_name = mkdtemp(dir_name_template);
    libfts::TextIndexWriter text_writer;
    text_writer.write(temporary_dir_name, idx.get_index());
    try {
        libfts::TextIndexAccessor accessor(temporary_dir_name);
        std::vector<libfts::DocId> expected_docs{100, 101};
        EXPECT_EQ(accessor.get_documents_by_term("world"), expected_docs);
        expected_docs = {100, 102};
        EXPECT_EQ(accessor.get_documents_by_term("hello"), expected_docs);
        expected_docs = {101};
        EXPECT_EQ(accessor.get_documents_by_term("bye"), expected_docs);
    } catch (libfts::AccessorException &ex) {
    };
    /* where to place the removal? */
    std::filesystem::remove_all(temporary_dir_name);
}

TEST(IndexerTest, BinaryIndex) {
    const std::filesystem::path path(c_absolute_path);
    const auto config = libfts::load_config(path / "ParserConfig.json");
    libfts::IndexBuilder idx;
    idx.add_document(100, "pot", config);
    idx.add_document(125, "past", config);
    idx.add_document(3123, "pass", config);
    idx.add_document(54334, "part", config);
    char dir_name_template[] = "/tmp/index_XXXXXX";
    char *temporary_dir_name = mkdtemp(dir_name_template);
    libfts::BinaryIndexWriter writer;
    writer.write(temporary_dir_name, idx.get_index());
    libfts::BinaryData bin_idx(temporary_dir_name);
    libfts::Header header(bin_idx.data());
    libfts::BinaryIndexAccessor accessor(bin_idx.data(), header);
    std::filesystem::remove_all(temporary_dir_name);
}