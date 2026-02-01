cmake_minimum_required(VERSION 3.16)

project(vault
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(vault
    src/main.cpp
)

target_include_directories(vault
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

include(FetchContent)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
)

FetchContent_MakeAvailable(doctest)

enable_testing()
add_subdirectory(tests)
