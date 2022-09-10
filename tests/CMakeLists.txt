set(target_name math_test)

add_executable(${target_name} libmath/sum.test.cpp)

target_link_libraries(
  ${target_name}
  PRIVATE
    math_lib
    gtest_main
)

include(GoogleTest)

gtest_discover_tests(${target_name})