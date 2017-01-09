//
// Created by Ivan Jurin on 12/15/16.
//
#pragma once
#ifdef UTILS_TIMER
#include <chrono>
#include <iostream>
#endif

typedef int read_id_t; // read id type (aId..)
typedef int read_size_t; // read size type (aStart, aEnd...)

#ifdef UTILS_TIMER
typedef std::chrono::high_resolution_clock Clock;

#define TIMER_START(msg)                                \
    std::cout << msg << std::endl;                      \
    auto t1 = Clock::now()

#define TIMER_END(msg)                                                                  \
    do {                                                                                \
    auto t2 = Clock::now();                                                             \
    std::cout << msg;                                                                   \
    std::cout                                                                           \
            << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()    \
            << "ns" << std::endl;                                                       \
    } while(false)
#endif
