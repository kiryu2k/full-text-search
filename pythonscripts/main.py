import fts

import argparse
import os


def main(args) -> None:
    config = fts.ParserConfiguration("ParserConfig.json")
    accessor = fts.IndexAccessor(args.index_dir)
    os.system("clear")
    # enter 'q.' to exit
    while True:
        query = input("> ")
        if (query == "q."):
            break
        print(fts.search(query, config, accessor), end="")


if __name__ == "__main__":
    command_line_parser = argparse.ArgumentParser()
    command_line_parser.add_argument("-i", "--index", dest="index_dir", default="index",
                                     help="path to folder with indexes", type=str)
    args = command_line_parser.parse_args()
    main(args)
