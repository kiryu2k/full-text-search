#include <libfts/absolutepath.hpp>
#include <libfts/indexer.hpp>
#include <libfts/parser.hpp>

#include <gtest/gtest.h>

TEST(IndexerTest, Indexer) {
    libfts::ParserConfiguration config =
        libfts::load_config(ABSOLUTE_PATH + "/ParserConfig.json");
    libfts::IndexBuilder idx;
    idx.add_document(199903, "The Matrix matrix awwww", config);
    idx.add_document(200305, "The Matrix Reloaded", config);
    idx.add_document(200311, "The Matrix Revolutions", config);
    libfts::TextIndexWriter writer;
    writer.write(ABSOLUTE_PATH + "/index/", idx.get_index());
    idx.add_document(200458, "The Matrix in the red bottoms MATR", config);
    writer.write(ABSOLUTE_PATH + "/index/", idx.get_index());
}