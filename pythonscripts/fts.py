import ctypes


def replace_with_npc(string):
    # non-printing character
    return string.replace("\\n", "\n").replace("\\t", "\t")


api_loader = ctypes.CDLL("pythonscripts/libsearchapi.so")

# create parser config object
api_loader.parser_configuration_new.restype = ctypes.c_void_p

# load configuration from file
api_loader.parser_configuration_load.argtypes = [
    ctypes.c_void_p, ctypes.c_char_p]

# delete config object
api_loader.parser_configuration_delete.argtypes = [ctypes.c_void_p]

# create index object
api_loader.index_new.restype = ctypes.c_void_p

# read text index
api_loader.text_index_read.argtypes = [ctypes.c_void_p, ctypes.c_char_p]

# delete index object
api_loader.index_delete.argtypes = [ctypes.c_void_p]

# create index accessor object
api_loader.index_accessor_new.restype = ctypes.c_void_p
api_loader.index_accessor_new.argtypes = [ctypes.c_void_p]

# delete index accessor object
api_loader.index_accessor_delete.argtypes = [ctypes.c_void_p]

# launch searcher
api_loader.search.restype = ctypes.c_void_p
api_loader.search.argtypes = [
    ctypes.c_char_p, ctypes.c_void_p, ctypes.c_void_p]

# free search result memory
api_loader.search_result_delete.argtypes = [ctypes.c_char_p]
