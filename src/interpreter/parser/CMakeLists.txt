add_library(parser parser.cpp)

target_include_directories(parser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../lexer
)

target_link_libraries(parser PUBLIC lexer)
