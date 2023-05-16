cmake_minimum_required(VERSION 3.26)

set(CMAKE_C_STANDAR 11)
set(CMAKE_CXX_STANDAR 11)
project(shell)

file(GLOB_RECURSE compile_units src/*.c src/*.cpp)
include_directories(include)
add_executable(shell ${compile_units})
