import fts

import os
import argparse

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
    # Remove the first 2 and the last characters of the resulting
    # C string, bringing it to str python data type.
    # The resulting string has '\\t' and '\\n' instead of
    # non-printing character (npc), so we call fts.replace_with_npc()
    # to replace them with control characters.
    print(fts.replace_with_npc(str(result.value)[2:][:-1]), end="")
    fts.api_loader.search_result_delete(result)

fts.api_loader.index_accessor_delete(accessor)
fts.api_loader.index_delete(index)
fts.api_loader.parser_configuration_delete(config)
