/** 
 * @file timer.h
 * @brief Class declaration for a timer class
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#ifndef INCLUDE_TIMER_H_
#define INCLUDE_TIMER_H_

#include <ctime>
#include <chrono>
#include <string>

class Timer {
 public:
  Timer();
  void stop();
  double get_cpu_time() const;
  double get_wall_time() const;

  std::string get_cpu_string() const;
  std::string get_wall_string() const;
 private:
  /// Start time for CPU time clock.
  std::clock_t clock_start_;
  /// Stop time for CPU time clock.
  std::clock_t clock_end_;
  /// Start time for wall time clock.
  std::chrono::high_resolution_clock::time_point timer_start_;
  /// Stop time for wall time clock.
  std::chrono::high_resolution_clock::time_point timer_end_;
};

#endif  // INCLUDE_TIMER_H_
