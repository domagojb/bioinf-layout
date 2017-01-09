//
// Created by Ivan Jurin on 12/15/16.
//
#pragma once
#ifdef UTILS_TIMER
#include <chrono>
#include <iostream>
#include <iomanip>
#endif

typedef int read_id_t; // read id type (aId..)
typedef int read_size_t; // read size type (aStart, aEnd...)

#ifdef UTILS_TIMER
typedef std::chrono::high_resolution_clock Clock;
#endif


#ifdef UTILS_TIMER
#define TIMER_START(msg)                                \
    std::cout << msg << std::endl;                      \
    auto t1 = Clock::now()

#define TIMER_END(msg)                                                                  \
    do {                                                                                \
    auto t2 = Clock::now();                                                             \
    std::cout << msg;                                                                   \
    std::cout <<\
            std::fixed << std::setw(11)\
            << std::setprecision(3)                                                                        \
            << (static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count())/1000000.f)    \
            << " ms" << std::endl;                                                       \
    } while(false)
#else
#define TIMER_START(msg)
#define TIMER_END(msg)

#endif
