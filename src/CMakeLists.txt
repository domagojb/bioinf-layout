cmake_minimum_required(VERSION 3.6)
project(bioinf_layout)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp IO.cpp IO.h Overlap.cpp Overlap.h Graph.cpp Graph.h Read.cpp Read.h OverlapUtils.cpp OverlapUtils.h)
add_executable(bioinf_layout ${SOURCE_FILES})