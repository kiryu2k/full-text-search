set(target_name searchapi)

add_library(${target_name} SHARED
  parser.cpp
  indexer.cpp
  search.cpp
  ${PROJECT_SOURCE_DIR}/include/searchapi/parser.h
  ${PROJECT_SOURCE_DIR}/include/searchapi/indexer.h
  ${PROJECT_SOURCE_DIR}/include/searchapi/search.h
  )

include(CompileOptions)
set_compile_options(${target_name})

target_include_directories(
  ${target_name}
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/src
)

target_link_libraries(
  ${target_name}
  PRIVATE
    fts
)

add_custom_command(TARGET ${target_name} 
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   $<TARGET_FILE:${target_name}> ${PROJECT_SOURCE_DIR}/pythonscripts)