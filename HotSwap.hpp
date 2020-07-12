#pragma once

#include <atomic>
#include <mutex>

#include "Compiler.hpp"

class HotSwap {
  Compiler* active{nullptr};
  Compiler* available{nullptr};
  std::atomic<bool> new_code_ready{false};
  int count_swap{0};
  int total_microsecond_wait{0};
  int microsecond_max_wait{0};

 public:
  HotSwap();
  void info();
  const char* error();
  bool compile(const char* code);
  float* process(int N, int sample, int samplerate);
};

