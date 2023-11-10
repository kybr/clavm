#pragma once

#include <iostream>
#include <chrono>

void say(const char* message) { printf("%s\n", message); }

#define DEBUG

#ifdef DEBUG
#define TRACE(fmt, ...) \
  printf(fmt, ##__VA_ARGS__)
#else
#define TRACE(fmt, ...)
#endif

struct BlockTimer {
  std::chrono::high_resolution_clock::time_point begin;
  BlockTimer() : begin(std::chrono::high_resolution_clock::now()) {}
  ~BlockTimer() {
    double t = std::chrono::duration<double>(
                   std::chrono::high_resolution_clock::now() - begin)
                   .count();
    if (t > 0) std::cout << "...took " << t << " seconds." << std::endl;
  }
};

struct StopWatch {
  std::chrono::high_resolution_clock::time_point begin;

  void tic() {
      begin = std::chrono::high_resolution_clock::now();
  }
  double toc() {
    double t = std::chrono::duration<double>(
                   std::chrono::high_resolution_clock::now() - begin)
                   .count();
    return t;
  }
};
