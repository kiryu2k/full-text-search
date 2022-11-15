import search

import os
import argparse

config = search.api_loader.parser_configuration_new()
search.api_loader.parser_configuration_load(
    config, "ParserConfig.json".encode("utf-8"))

command_line_parser = argparse.ArgumentParser()
command_line_parser.add_argument("-i", "--index", dest="index_path", default="index",
                                 help="path to folder with indexes", type=str)
args = command_line_parser.parse_args()

index = search.api_loader.index_new()
search.api_loader.text_index_read(index, args.index_path.encode("utf-8"))
accessor = search.api_loader.index_accessor_new(index)

os.system("clear")

# enter 'q.' to exit
while True:
    query = input("> ")
    if (query == "q."):
        break
    result = search.api_loader.search(query.encode("utf-8"), config, accessor)
    print(result.decode("utf-8"), end="")
    search.api_loader.search_result_delete(result)

search.api_loader.index_accessor_delete(accessor)
search.api_loader.index_delete(index)
search.api_loader.parser_configuration_delete(config)
