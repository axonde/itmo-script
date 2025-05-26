add_library(operators operators.cpp)

target_include_directories(operators PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../lexer
)
target_include_directories(operators PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../parser
)

target_link_libraries(operators PUBLIC lexer)
target_link_libraries(operators PUBLIC parser)
