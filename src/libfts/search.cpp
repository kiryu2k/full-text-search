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

static bool is_added(const Results &results, const std::string &text) {
    return std::find_if(
               results.begin(), results.end(), [text](const auto &result) {
                   return result.text_ == text;
               }) != results.end();
}

static Results cutoff_by_factor(
    const std::map<DocId, double> &search_result,
    const TextIndexAccessor &index,
    double cutoff_factor) {
    Results results;
    const auto max_score = get_max_score(search_result);
    for (const auto &[document_id, score] : search_result) {
        if (score > max_score * cutoff_factor) {
            const auto text = index.get_document_by_id(document_id);
            if (!is_added(results, text)) {
                results.push_back({document_id, score, text});
            }
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
    const TextIndexAccessor &index) {
    const auto parsed_query = parse(query, config);
    std::map<DocId, double> result;
    const auto total_doc_count =
        static_cast<double>(index.get_document_count());
    for (const auto &word : parsed_query) {
        for (const auto &term : word.ngrams_) {
            Pos docs;
            try {
                /* if term isn't found in the list of docs, exception will be
                 * thrown */
                docs = index.get_documents_by_term(term);
            } catch (const libfts::AccessorException &ex) {
                /* but since the query may contain errors and we'd like to
                 * see the result anyway, we need to handle this exception and
                 * keep working */
                continue;
            }
            const auto doc_freq = static_cast<double>(docs.size());
            for (const auto &identifier : docs) {
                const auto term_freq = static_cast<double>(
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