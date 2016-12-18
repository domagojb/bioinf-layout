cmake_minimum_required(VERSION 3.6)
project(bioinf_layout)

#set(CMAKE_CXX "clang++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES common.h main.cpp IO.cpp IO.h Overlap.cpp Overlap.h OverlapUtils.h OverlapUtils.cpp Graph.h Graph.cpp params.h params.cpp ReadTrim.h  Read.cpp Read.h GraphUtils.cpp GraphUtils.h)
add_executable(bioinf_layout ${SOURCE_FILES})