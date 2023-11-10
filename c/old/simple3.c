#include "math.h"

#define M (10000)
float table[M];

// XXX
// libc++abi.dylib: terminating with uncaught exception of type
// std::invalid_argument: stoi: no conversion
#include "synth.h"

// called once just after a compile
// before process ever runs
void begin() {
  for (int i = 0; i < N; i++) {
    table[i] = sin(M_PI * 2.0 * i / N);
  }
}

void process(double t, float* i, float* o) {
  _index = 0;
  t *= 4.0;
  int T = t;
  t -= T;  // map to (0, 1)
  t = 1 - t;
  t = onepole(t * t, 0.9);
  int index = phasor(440) * (N - 1.0);
  float a = 0.08 * table[index];
  o[0] = o[1] = a * t;
}
