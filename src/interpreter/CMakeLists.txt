add_subdirectory(utils)
add_subdirectory(lexer)

add_library(itmoscript interpreter.cpp)

target_link_libraries(itmoscript PUBLIC utils lexer)