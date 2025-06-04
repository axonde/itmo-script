add_library(memory memory.cpp)

target_include_directories(memory PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../lexer
)

target_link_libraries(memory PUBLIC lexer)
