enable_testing()
find_package(GTest)
include(GoogleTest)

find_package(fmt REQUIRED)

function(add_unit_test name path)
  message("-- patrick: Add ${name} unit test")
  set(target "test_${name}")
  add_executable(${target} ${path})
  set_target_properties(${target} PROPERTIES LINKER_LANGUAGE CXX)
  target_link_libraries(${target} PRIVATE GTest::gtest_main patrick fmt::fmt)
  target_include_directories(${target} PRIVATE ../patrick/include)
  gtest_discover_tests(${target})
  add_test(NAME ${name} COMMAND ${target})
endfunction()

add_unit_test(it_works test_it_works.cpp)
add_unit_test(word test_word.cpp)
