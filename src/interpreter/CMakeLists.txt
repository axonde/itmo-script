add_subdirectory(utils)
add_subdirectory(lexer)
add_subdirectory(parser)

add_library(itmoscript interpreter.cpp)

target_include_directories(itmoscript PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer
    ${CMAKE_CURRENT_SOURCE_DIR}/parser
)

target_link_libraries(itmoscript PUBLIC utils lexer parser)