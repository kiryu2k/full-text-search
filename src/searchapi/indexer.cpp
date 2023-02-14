#include <libfts/indexer.hpp>
#include <searchapi/indexer.h>

extern "C" {

BinaryData *binary_data_map(const char *data_path) {
    auto *binary_data = new libfts::BinaryData(data_path);
    return reinterpret_cast<BinaryData *>(binary_data);
}

void binary_data_unmap(BinaryData *p_data) {
    auto *data = reinterpret_cast<libfts::BinaryData *>(p_data);
    delete data;
}

BinaryIndexAccessor *index_accessor_new(BinaryData *p_index) {
    try {
        const auto *index = reinterpret_cast<libfts::BinaryData *>(p_index);
        libfts::Header header(index->data());
        auto *accessor = new libfts::BinaryIndexAccessor(index->data(), header);
        // auto *accessor = new libfts::TextIndexAccessor(index_path);
        return reinterpret_cast<BinaryIndexAccessor *>(accessor);
    } catch (const libfts::AccessorException &ex) {
        return nullptr;
    } catch (const std::filesystem::__cxx11::filesystem_error &ex) {
        return nullptr;
    }
}

void index_accessor_delete(BinaryIndexAccessor *p_accessor) {
    auto *accessor =
        reinterpret_cast<libfts::BinaryIndexAccessor *>(p_accessor);
    delete accessor;
}
}