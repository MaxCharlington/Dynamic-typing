cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_COMPILER g++-10)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -std=c++2a -Wall -Wextra -Wpedantic")

include_directories(dynamic_typing)
add_subdirectory(dynamic_typing)
add_executable(prog prog.cpp)
target_link_libraries(prog dynamic_typing)
