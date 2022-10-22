set(target_name fts_test)

add_executable(${target_name} libfts/parser.test.cpp libfts/indexer.test.cpp libfts/search.test.cpp)

target_link_libraries(
  ${target_name}
  PRIVATE
    fts
    gtest_main
)

include(GoogleTest)

gtest_discover_tests(${target_name})