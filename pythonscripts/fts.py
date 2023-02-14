import ctypes

search_lib = ctypes.CDLL("pythonscripts/libsearchapi.so")

# create parser config object
parser_config_new = search_lib.parser_configuration_new
parser_config_new.restype = ctypes.c_void_p
parser_config_new.argtypes = [ctypes.c_char_p]

# delete config object
parser_config_del = search_lib.parser_configuration_delete
parser_config_del.argtypes = [ctypes.c_void_p]

# map binary data
b_data_map = search_lib.binary_data_map
b_data_map.restype = ctypes.c_void_p
b_data_map.argtypes = [ctypes.c_char_p]

# unmap binary data
b_data_unmap = search_lib.binary_data_unmap
b_data_unmap.argtypes = [ctypes.c_void_p]

# create index accessor object
index_accessor_new = search_lib.index_accessor_new
index_accessor_new.restype = ctypes.c_void_p
index_accessor_new.argtypes = [ctypes.c_void_p]

# delete index accessor object
index_accessor_del = search_lib.index_accessor_delete
index_accessor_del.argtypes = [ctypes.c_void_p]

# launch searcher
search_lib.search.restype = ctypes.c_void_p
search_lib.search.argtypes = [
    ctypes.c_char_p, ctypes.c_void_p, ctypes.c_void_p]

# free search result memory
search_lib.search_result_delete.argtypes = [ctypes.c_char_p]


class ParserConfiguration:
    def __init__(self, path: str) -> None:
        self.obj = parser_config_new(path.encode("utf-8"))

    def __del__(self) -> None:
        parser_config_del(self.obj)


class BinaryData:
    def __init__(self, index_dir: str) -> None:
        self.obj = b_data_map(index_dir.encode("utf-8"))

    def __del__(self) -> None:
        b_data_unmap(self.obj)


class IndexAccessor:
    def __init__(self, index: BinaryData) -> None:
        self.obj = index_accessor_new(index.obj)

    def __del__(self) -> None:
        index_accessor_del(self.obj)


def search(query: str, config: ParserConfiguration, index: IndexAccessor):
    ctypes_result = ctypes.c_char_p(search_lib.search(
        query.encode("utf-8"), config.obj, index.obj))
    result = str(ctypes_result.value.decode("utf-8"))
    search_lib.search_result_delete(ctypes_result)
    return result
