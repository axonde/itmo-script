cmake_minimum_required(VERSION 3.12)

project(
    itmoscript_interpreter
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

# Arg parser fetching
include(FetchContent)
FetchContent_Declare(
  arg-parser
  GIT_REPOSITORY https://github.com/axonde/arg-parser
  GIT_TAG v1.0.1
)
FetchContent_MakeAvailable(arg-parser)

add_subdirectory(src)
add_subdirectory(tests)
