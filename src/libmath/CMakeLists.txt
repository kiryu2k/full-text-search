set(target_name math_lib)

add_library(${target_name} STATIC
  sum.cpp
  )

include(CompileOptions)
set_compile_options(${target_name})

target_include_directories(
  ${target_name}
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)