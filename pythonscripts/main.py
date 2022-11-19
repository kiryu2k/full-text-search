import fts

import argparse
import os

config = fts.api_loader.parser_configuration_new()
fts.api_loader.parser_configuration_load(
    config, "ParserConfig.json".encode("utf-8"))

command_line_parser = argparse.ArgumentParser()
command_line_parser.add_argument("-i", "--index", dest="index_path", default="index",
                                 help="path to folder with indexes", type=str)
args = command_line_parser.parse_args()

index = fts.api_loader.index_new()
fts.api_loader.text_index_read(index, args.index_path.encode("utf-8"))
accessor = fts.api_loader.index_accessor_new(index)

os.system("clear")

# enter 'q.' to exit
while True:
    query = input("> ")
    if (query == "q."):
        break
    result = fts.ctypes.c_char_p(fts.api_loader.search(
        query.encode("utf-8"), config, accessor))
    print(str(result.value.decode("utf-8")), end="")
    fts.api_loader.search_result_delete(result)

fts.api_loader.index_accessor_delete(accessor)
fts.api_loader.index_delete(index)
fts.api_loader.parser_configuration_delete(config)
