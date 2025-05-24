cmake_minimum_required(VERSION 3.12)

project(
    itmoscript_interpreter
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

# enable_testing()
# add_subdirectory(tests)