cmake_minimum_required(VERSION 3.25)
project(cascade_generation)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(cascade_generation
        include/optimizer.hpp
        include/parser_utils.hpp
        src/cascades.cpp)
