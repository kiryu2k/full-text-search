set(target_name fts)

add_library(${target_name} STATIC
  parser.cpp
  ${PROJECT_SOURCE_DIR}/include/libfts/parser.hpp
  )

include(CompileOptions)
set_compile_options(${target_name})

target_include_directories(
  ${target_name}
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(
  ${target_name}
  PRIVATE
    nlohmann_json
)