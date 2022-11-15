import search

import os

config = search.api_loader.parser_configuration_new()
config = search.api_loader.parser_configuration_load(
    config, "ParserConfig.json".encode("utf-8"))

index = search.api_loader.index_new()
index = search.api_loader.text_index_read(index, "index".encode("utf-8"))
accessor = search.api_loader.index_accessor_new(index)

os.system("clear")

# enter 'q.' to exit
while True:
    query = input("> ")
    if (query == "q."):
        break
    result = search.api_loader.search(query.encode("utf-8"), config, accessor)
    print(result.decode("utf-8"))
    # searcher.search_result_delete(result)

search.api_loader.index_accessor_delete(accessor)
search.api_loader.index_delete(index)
search.api_loader.parser_configuration_delete(config)
