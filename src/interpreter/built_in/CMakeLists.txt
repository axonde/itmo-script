add_library(built_in built_in.cpp)

target_include_directories(built_in PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../memory
)

target_link_libraries(built_in PUBLIC memory)