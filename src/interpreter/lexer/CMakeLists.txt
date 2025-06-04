add_library(lexer lexer.cpp)

target_include_directories(lexer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../utils
)

target_link_libraries(lexer PUBLIC utils)