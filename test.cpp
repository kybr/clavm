#include <cassert>
#include <chrono>
#include <cmath>
#include <thread>
#include <vector>

#include "HotSwap.hpp"
#include "globals.h"
float* __cc = nullptr;

const char* code = R"(
float* _float(int);
float _rate(void);
void _out(float);
double sin(double);

float phasor(float hz) {
  float* phase = _float(1);
  *phase += ((hz < 0) ? -hz : hz) / _rate();
  *phase -= (int)(*phase);
  return *phase;
}

float onepole(float x, float a) {
  float* history = _float(1);
  *history = (1 - a) * x + a * *history;
  return *history;
}

void play(void) {
  float hz = onepole(%f, 0.991);
  float phase = phasor(hz);
  float out = sin(phase * 6.283185307179586);
  _out(0.1 * out);
}
)";

int main(int argc, char* argv[]) {
  __cc = new float[100];
  HotSwap hotswap;

  bool run = true;
  std::thread t([&]() {
    char buffer[1000];
    float m = 0;

    auto mtof = [](float midi) {
      return 440.0f * pow(2.0f, (midi - 69.0f) / 12.0f);
    };

    while (run) {
      snprintf(buffer, sizeof(buffer), code, mtof(m));
      m += 0.7;
      if (m > 127)  //
        m = 0;

      // blocks until complete
      if (!hotswap.compile(buffer)) {
        fprintf(stderr, "ERROR: %s\n", hotswap.error());
        exit(1);
      }
    }
  });

  using namespace std::chrono;

  std::vector<float> data;
  for (int k = 0; k < 10000; k++) {
    float* block = hotswap.process(1024, k * 1024, 44100);
    data.insert(data.end(), block, block + 1024);

    // these hang the program
    // std::this_thread::sleep_until(system_clock::now() + milliseconds(23));
    // std::this_thread::sleep_for(milliseconds(23));
  }

  run = false;
  t.join();

  if (argc > 1)
    for (auto f : data)  //
      printf("%f\n", f);
  else
#ifdef __PI__
    fprintf(stderr, "%u samples\n", data.size());
#else
    fprintf(stderr, "%lu samples\n", data.size());
#endif

  hotswap.info();
}
