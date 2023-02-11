#pragma once

#include <libfts/indexer.hpp>

namespace libfts {

struct Result {
    DocId document_id_ = 0;
    double score_ = 0;
    Doc text_;
};

using Results = std::vector<Result>;

std::string get_string_search_result(const Results &search_result);

Results search(
    const std::string &query,
    const ParserConfiguration &config,
    const TextIndexAccessor &index);

} // namespace libfts