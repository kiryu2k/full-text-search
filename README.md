# Full-text search
## By me
This is a full-text search system for a database of books. The project consists of the following components:
1) A library or several libraries in C++, in which the indexing, search and ranking of documents is implemented.
2) Console utilities for using the library (C++).
3) C wrapper for cross-language use of the library.
4) A search client written in the high-level Python programming language using the ctypes library.
5) All functionality must be covered by unit tests.

## How to use
```bash
$ bash ./run.sh
```
After running the bash script the project is configured and built, and then indexes the books from the csv file with them and starts the search engine.
It's also worth noting the search can be run separately with the following command:
```bash
$ python3 pythonscripts/main.py
```

## About searcher
The full-text search system is quite primitive, but still has a little positive functionality:
1) The output is rebuilt when a query is refined.
2) The search is capable of handling queries with typos.
3) To exit write `q.` as a query. ٩(◕‿◕｡)۶
