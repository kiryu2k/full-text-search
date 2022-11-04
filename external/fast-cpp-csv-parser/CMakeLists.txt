set(target_name fast-cpp-csv-parser)

add_library(${target_name} STATIC
  csv.h
  )

include(CompileOptions)
set_compile_options(${target_name})
set_target_properties(${target_name} PROPERTIES LINKER_LANGUAGE CXX)