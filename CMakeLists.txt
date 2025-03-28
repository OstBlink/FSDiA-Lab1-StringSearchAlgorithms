cmake_minimum_required(VERSION 3.25)
project(SearchAlgorithms LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.1.1
)

FetchContent_MakeAvailable(fmt)

add_executable(SearchAlgorithms main.cpp)
target_link_libraries(SearchAlgorithms fmt::fmt)