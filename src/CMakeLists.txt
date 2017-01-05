cmake_minimum_required(VERSION 3.6)
project(bioinf_layout)

set(CMAKE_CXX "clang++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DNDEBUG")


file(GLOB SOURCE_FILES "*.h" "*.cpp")

add_executable(bioinf_layout ${SOURCE_FILES})
