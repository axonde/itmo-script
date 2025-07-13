add_subdirectory(utils)
add_subdirectory(lexer)
add_subdirectory(memory)
add_subdirectory(built_in)
add_subdirectory(parser)
add_subdirectory(operators)

add_library(itmoscript interpreter.cpp)

target_include_directories(itmoscript PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer
    ${CMAKE_CURRENT_SOURCE_DIR}/memory
    ${CMAKE_CURRENT_SOURCE_DIR}/built-in
    ${CMAKE_CURRENT_SOURCE_DIR}/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/operators
)

target_link_libraries(itmoscript PUBLIC utils lexer memory built_in parser operators)
