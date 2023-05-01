cmake_minimum_required(VERSION 2.6)
project(shell)
set(CMAKE_C_STANDAR 17)

file(GLOB_RECURSE compile_units src/*.c src/*.cpp)
include_directories(include)
add_executable(shell ${compile_units})
