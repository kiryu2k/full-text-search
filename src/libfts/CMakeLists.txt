set(target_name fts)

configure_file(${PROJECT_SOURCE_DIR}/include/libfts/absolutepath.hpp.in absolutepath.hpp)

add_library(${target_name} STATIC
  parser.cpp
  indexer.cpp
  ${PROJECT_SOURCE_DIR}/include/libfts/parser.hpp
  ${PROJECT_SOURCE_DIR}/include/libfts/indexer.hpp
  )

include(CompileOptions)
set_compile_options(${target_name})

target_include_directories(
  ${target_name}
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/external
    ${CMAKE_BINARY_DIR}/src
)

target_link_libraries(
  ${target_name}
  PRIVATE
    nlohmann_json
    picosha2
    fmt
)