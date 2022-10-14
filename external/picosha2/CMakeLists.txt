set(target_name picosha2)

add_library(${target_name} STATIC
  picosha2.h
  )

include(CompileOptions)
set_compile_options(${target_name})
set_target_properties(${target_name} PROPERTIES LINKER_LANGUAGE CXX)