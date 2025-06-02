add_library(memory memory.cpp)

target_include_directories(memory PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../utils
)

target_link_libraries(memory PUBLIC utils)
