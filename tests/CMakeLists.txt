include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  itmoscript_tests
  lexer_ut.cpp
  ast_ut.cpp
  # function_test.cpp
  # types_test.cpp
  # loop_and_branch_test.cpp
)

target_link_libraries(
  itmoscript_tests
  itmoscript
  serializer_lib
  GTest::gtest_main
)

target_include_directories(itmoscript_tests PUBLIC ${PROJECT_SOURCE_DIR}/src/interpreter)
target_include_directories(itmoscript_tests PUBLIC ${PROJECT_SOURCE_DIR}/src/interpreter/parser/serializer)

include(GoogleTest)

gtest_discover_tests(itmoscript_tests)