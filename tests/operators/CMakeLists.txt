add_executable(
  itmoscript_operator_tests
	unary_operators_ut.cpp
	binary_operators_ut.cpp
)

target_link_libraries(
  itmoscript_operator_tests
  itmoscript
  GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(itmoscript_operator_tests)

target_include_directories(itmoscript_operator_tests PUBLIC ${PROJECT_SOURCE_DIR}/src/interpreter)