cmake_minimum_required (VERSION 3.16)
project (CPP_PLAYGROUND)

set (CMAKE_CXX_COMPILER "clang++")
set (CMAKE_CXX_FLAGS "-O3 -std=c++20")

include_directories(include)
find_library(gpiod NAMES gpiod)

add_executable (main src/main.cpp)
target_link_libraries(main gpiod)