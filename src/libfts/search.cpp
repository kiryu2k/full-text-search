#include <libfts/search.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <cmath>

namespace libfts {

static double get_max_score(const std::map<DocId, double> &search_result) {
    double max_score = 0;
    for (const auto &[document_id, score] : search_result) {
        max_score = std::max(max_score, score);
    }
    return max_score;
}

static void sort_by_score(Results &search_result) {
    std::sort(
        search_result.begin(),
        search_result.end(),
        [](const auto &lhs, const auto &rhs) {
            return lhs.score_ != rhs.score_
                ? lhs.score_ > rhs.score_
                : lhs.document_id_ < rhs.document_id_;
        });
}

static Results cutoff_by_factor(
    const std::map<DocId, double> &search_result,
    IndexAccessor &index,
    double cutoff_factor) {
    Results results;
    const auto max_score = get_max_score(search_result);
    for (const auto &[document_id, score] : search_result) {
        if (score > max_score * cutoff_factor) {
            results.push_back(
                {document_id, score, index.get_document_by_id(document_id)});
        }
    }
    sort_by_score(results);
    return results;
}

std::string get_string_search_result(const Results &search_result) {
    std::string result;
    size_t ordinal_num = 1;
    for (const auto &[id, score, text] : search_result) {
        result +=
            fmt::format("{}\t{}\t{}\t{}\n", ordinal_num++, score, id, text);
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
    return cutoff_by_factor(result, index, config.get_cutoff_factor());
}

} // namespace libfts