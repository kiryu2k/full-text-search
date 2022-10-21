#pragma once

#include <libfts/indexer.hpp>

namespace libfts {

std::map<DocId, double> search(
    const std::string &query,
    const ParserConfiguration &config,
    IndexAccessor &index);

} // namespace libfts