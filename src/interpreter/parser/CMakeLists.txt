add_library(parser parser.cpp)

target_include_directories(parser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../lexer
)
target_include_directories(parser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../built_in
)

target_link_libraries(parser PUBLIC lexer)
target_link_libraries(parser PUBLIC built_in)

# JSON
include(FetchContent)
FetchContent_Declare(
    json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE 
)
FetchContent_MakeAvailable(json)

# SERIALIZER INTERFACE FOR TESTING
add_library(serializer_lib INTERFACE)
target_include_directories(serializer_lib INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/serializer
)
target_link_libraries(serializer_lib INTERFACE 
    parser 
    nlohmann_json::nlohmann_json
)

add_executable(serializer ${CMAKE_CURRENT_SOURCE_DIR}/serializer/serializer.cpp)
target_link_libraries(serializer PRIVATE parser nlohmann_json::nlohmann_json)
target_include_directories(serializer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})