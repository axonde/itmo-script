add_subdirectory(utils)
add_subdirectory(lexer)
add_subdirectory(built_in)
add_subdirectory(parser)
add_subdirectory(memory)
add_subdirectory(operators)

add_library(itmoscript interpreter.cpp)

target_include_directories(itmoscript PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/built-in
    ${CMAKE_CURRENT_SOURCE_DIR}/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/memory
    ${CMAKE_CURRENT_SOURCE_DIR}/operators
)

target_link_libraries(itmoscript PUBLIC built_in parser memory operators)