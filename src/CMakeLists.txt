cmake_minimum_required(VERSION 3.6)
project(bioinf_layout)

set(CMAKE_CXX "clang++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -DNDEBUG -flto -O3")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -DNDEBUG -g3")

#add_definitions(-DUTILS_TIMER)

file(GLOB SOURCE_FILES "*.h" "*.cpp")

add_executable(bioinf_layout ${SOURCE_FILES})
