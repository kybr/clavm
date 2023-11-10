#include "math.h"

#define N (10000)
float table[N];
float memory[N];
unsigned _index;

// called once just after a compile
// before process ever runs
void begin() {
  for (int i = 0; i < N; i++) {
    table[i] = sin(M_PI * 2.0 * i / N);
  }
}

float phasor(float frequency) {
  float* phase = &memory[_index];
  _index += 1;
  phase[0] += frequency / 44100;
  if (phase[0] > 1)  //
    phase[0] -= 1;
  return phase[0];
}

float onepole(float x, float a) {
  float* history = &memory[_index];
  _index += 1;
  return history[0] = history[0] * a + (1 - a) * x;
}

void process(double t, float* i, float* o) {
  _index = 0;
  t *= 4.0;
  int T = t;
  t -= T;  // map to (0, 1)
  t = 1 - t;
  t = onepole(t * t, 0.94);
  int index = phasor(220) * (N - 1.0);
  float a = 0.2 * table[index];
  o[0] = o[1] = a * t;
}
