#include "Utilities.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Timer::Timer() : creation(std::chrono::high_resolution_clock::now()) {}

void Timer::check(const char* name) {
  total = 1000 * std::chrono::duration<double>(
                     std::chrono::high_resolution_clock::now() - creation)
                     .count();
  entry.push_back({name, total});
}

void Timer::print(const char* pattern) {
  for (auto e : entry)  //
    printf(pattern, e.name.c_str(), e.time);
}

void Timer::string(std::string& into) {
  std::ostringstream stream;
  stream.precision(2);
  for (auto e : entry)  //
    stream << e.name << ":" << e.time << " ";
  into = stream.str();
}

void Timer::delta() {
  for (int i = entry.size() - 1; i > 0; i--)  //
    entry[i].time -= entry[i - 1].time;
}

void Timer::percent() {
  delta();
  for (auto& p : entry)  //
    p.time = 100 * p.time / total;
}

//
//
//

std::string slurp(const std::string& fileName) {
  using namespace std;
  ifstream ifs(fileName.c_str(), ios::in | ios::binary | ios::ate);

  ifstream::pos_type fileSize = ifs.tellg();
  ifs.seekg(0, ios::beg);

  vector<char> bytes(fileSize);
  ifs.read(bytes.data(), fileSize);

  return string(bytes.data(), fileSize);
}

std::string slurp() {
  using namespace std;
  vector<char> cin_str;
  // 64k buffer seems sufficient
  streamsize buffer_sz = 65536;
  vector<char> buffer(buffer_sz);
  cin_str.reserve(buffer_sz);

  auto rdbuf = cin.rdbuf();
  while (auto cnt_char = rdbuf->sgetn(buffer.data(), buffer_sz))
    cin_str.insert(cin_str.end(), buffer.data(), buffer.data() + cnt_char);
  return string(cin_str.data(), cin_str.size());
}
