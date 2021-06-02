#pragma once

#include <chrono>
#include <string>
#include <vector>

std::string slurp(const std::string& fileName);
std::string slurp();

struct Entry {
  std::string name;
  double time;
};

class Timer {
  std::vector<Entry> entry;
  std::chrono::high_resolution_clock::time_point creation;
  double total{0};

 public:
  Timer();

  void check(const char* name);
  void print(const char* pattern = "%s:%g ");
  void string(std::string& into);
  void delta();

  void percent();
};
