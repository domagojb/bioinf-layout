cmake_minimum_required(VERSION 3.6)
project(bioinf_layout)

set(CMAKE_CXX "clang++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -DNDEBUG -flto -O3")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -DNDEBUG -g3")

set(LAYOUT_SOURCES
    main.cpp
    common.h
    dotter.cpp
    dotter.h
    Graph.h
    GraphUtils.h
    GraphUtils.cpp
    IO.cpp
    IO.h
    Overlap.cpp
    Overlap.h
    OverlapUtils.cpp
    OverlapUtils.h
    params.cpp
    params.h
    ReadTrim.h
    Unitig.h
    UnitigUtils.cpp
    UnitigUtils.h)

set(GEPARD_PATH "${CMAKE_SOURCE_DIR}/../../gepard")
set(GEPARD_VERSION 1.40)
set(GEPARD_JAR "Gepard-${GEPARD_VERSION}.jar")
set(GEPARD_JAR_PATH ${GEPARD_PATH}/dist/${GEPARD_JAR})
set(GEPARD_MAT "edna.mat")
set(GEPARD_MAT_PATH ${GEPARD_PATH}/src/matrices/${GEPARD_MAT})


add_definitions(-DUTILS_TIMER)
add_definitions(-DGEPARD_JAR="${GEPARD_JAR_PATH}")
add_definitions(-DGEPARD_MAT="${GEPARD_MAT_PATH}")
#add_definitions(-DLOG_UNITIGS)
#add_definitions(-DLOG_UNITIGS_SEQUENCES)
#add_definitions(-DENABLE_DOTTER)

add_executable(layout ${LAYOUT_SOURCES})
