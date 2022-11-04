#include <libfts/search.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <cmath>

namespace libfts {

static Results sort_by_score(
    const std::map<DocId, double> &search_result, IndexAccessor &index) {
    Results sorted_result;
    for (const auto &[document_id, score] : search_result) {
        sorted_result.push_back(
            {document_id, score, index.get_document_by_id(document_id)});
    }
    std::sort(
        sorted_result.begin(),
        sorted_result.end(),
        [](const auto &lhs, const auto &rhs) {
            return lhs.score_ != rhs.score_
                ? lhs.score_ > rhs.score_
                : lhs.document_id_ < rhs.document_id_;
        });
    return sorted_result;
}

std::string get_string_search_result(const Results &search_result) {
    std::string result = "\tid\tscore\ttext\n";
    for (const auto &[id, score, text] : search_result) {
        result += fmt::format("\t{}\t{}\t{}\n", id, score, text);
    }
    return result;
}

Results search(
    const std::string &query,
    const ParserConfiguration &config,
    IndexAccessor &index) {
    auto parsed_query = parse(query, config);
    std::map<DocId, double> result;
    const auto total_doc_count =
        static_cast<double>(index.get_document_count());
    for (const auto &word : parsed_query) {
        for (const auto &term : word.ngrams_) {
            auto docs = index.get_documents_by_term(term);
            auto doc_freq = static_cast<double>(docs.size());
            for (const auto &identifier : docs) {
                auto term_freq = static_cast<double>(
                    index.get_term_positions_in_document(term, identifier)
                        .size());
                result[identifier] +=
                    term_freq * log(total_doc_count / doc_freq);
            }
        }
    }
    return sort_by_score(result, index);
}

} // namespace libfts