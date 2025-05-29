add_library(parser parser.cpp)

target_include_directories(parser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../lexer
)

target_link_libraries(parser PUBLIC lexer)

# JSON
include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(serializer serializer.cpp)
target_link_libraries(serializer PRIVATE parser nlohmann_json::nlohmann_json)