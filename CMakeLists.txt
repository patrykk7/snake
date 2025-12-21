cmake_minimum_required(VERSION 4.0)
project(snake)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        raylib URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
)
FetchContent_MakeAvailable(raylib)

add_executable(snake main.cpp)

target_link_libraries(snake PRIVATE raylib)