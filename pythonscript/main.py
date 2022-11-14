import ctypes
import os
import time

searcher = ctypes.CDLL(
    "./build/debug/src/searchapi/libsearchapi.so")

# create parser config object
searcher.parser_configuration_new.restype = ctypes.c_void_p
config = searcher.parser_configuration_new()

# load configuration from file
searcher.parser_configuration_load.restype = ctypes.c_void_p
searcher.parser_configuration_load.argtypes = [
    ctypes.c_void_p, ctypes.c_char_p]
config = searcher.parser_configuration_load(
    config, "ParserConfig.json".encode("utf-8"))

# create index object
searcher.index_new.restype = ctypes.c_void_p
index = searcher.index_new()

# read text index
searcher.text_index_read.restype = ctypes.c_void_p
searcher.text_index_read.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
index = searcher.text_index_read(index, "index".encode("utf-8"))

# create index accessor object
searcher.index_accessor_new.restype = ctypes.c_void_p
searcher.index_accessor_new.argtypes = [ctypes.c_void_p]
accessor = searcher.index_accessor_new(index)

# launch searcher
searcher.search.restype = ctypes.c_char_p
searcher.search.argtypes = [ctypes.c_char_p, ctypes.c_void_p, ctypes.c_void_p]

# free search result memory
searcher.search_result_delete.argtypes = [ctypes.c_char_p]

print("\nenter 'q.' to exit app\n")
time.sleep(2)
os.system("clear")

while True:
    query = input("> ")
    if (query == "q."):
        break
    result = searcher.search(query.encode("utf-8"), config, accessor)
    print(result.decode("utf-8"))
    # searcher.search_result_delete(result)

# delete index accessor object
searcher.index_accessor_delete.argtypes = [ctypes.c_void_p]
searcher.index_accessor_delete(accessor)

# delete index object
searcher.index_delete.argtypes = [ctypes.c_void_p]
searcher.index_delete(index)

# delete config object
searcher.parser_configuration_delete.argtypes = [ctypes.c_void_p]
searcher.parser_configuration_delete(config)
