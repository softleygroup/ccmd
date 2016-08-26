/** 
 * @file timer.cpp
 * @brief Function definitions timer class
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

/**
 * @class timer
 * @brief A simple timing class that can track wall and cpu time.
 */

#include "include/timer.h"

#include <ctime>
#include <chrono>

/**
 * @brief Initialise timer class and start clock and timer.
 */
Timer::Timer() {
    clock_start_ = std::clock();
    timer_start_ = std::chrono::high_resolution_clock::now();
}

/**
 * @brief Record current time of both timers.
 */
void Timer::stop() {
    clock_end_ = std::clock();
    timer_end_ = std::chrono::high_resolution_clock::now();
}

/**
 * @brief Return the CPU time using clock_end_.
 */
double Timer::get_cpu_time() const {
    return (clock_end_ - clock_start_)/CLOCKS_PER_SEC;
}

/**
 * @brief Return the wall time using timer_end_.
 */
double Timer::get_wall_time() const {
    auto duration = std::chrono::duration<double>(timer_end_ - timer_start_);
    return duration.count();
}


std::string Timer::get_cpu_string() const {
    char buffer[256];
    snprintf(buffer, 256, "CPU time  = %.2f s", get_cpu_time());
    return std::string(buffer);
}

std::string Timer::get_wall_string() const {
    char buffer[256];
    snprintf(buffer, 256, "Wall time = %.2f s", get_wall_time());
    return std::string(buffer);
}

