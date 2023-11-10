#include "math.h"

#define N (1000)
float memory[N];
unsigned _index = 0;

float mtof(float m) { return 8.175799 * pow(2.0, m / 12.0); }

float phasor(float frequency) {
  float phase = memory[_index];
  phase += frequency / SAMPLE_RATE;
  while (phase > 1)  //
    phase -= 1;
  memory[_index] = phase;
  _index++;
  return phase;
}

float onepole(float x0, float a1) {
  float y1 = memory[_index];
  y1 = (1 - a1) * x0 + a1 * y1;
  memory[_index] = y1;
  _index++;
  return y1;
}

void init(void) {
  for (int i = 0; i < N; i++)  //
    memory[i] = 0;
}

void process(double t, float* i, float* o) {
  _index = 0;  // reset the memory pointer

  t *= 4.4;
  int T = t;

  float hz = mtof(60 + ("13713518"[T % 8] - '0'));

  float f = 0;
  f += sin(2 * M_PI * phasor(hz));

  o[0] = o[1] = f;
}
