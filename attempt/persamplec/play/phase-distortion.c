#include "math.h"

#define N (10000)
float memory[N];
unsigned _index = 0;

float table[N];

void begin() {
  for (int i = 0; i < N; i++) {
    table[i] = sin(M_PI * 2.0 * i / N);
  }
}

float phasor(float frequency) {
  float phase = memory[_index];
  phase += frequency / SAMPLE_RATE;
  while (phase > 1)  //
    phase -= 1;
  memory[_index] = phase;
  _index++;
  return phase;
}

float pd(float phase, float x, float y) {
  if (phase < x) return phase * y / x;
  return y + (phase - x) * x / y;
  // return y + (phase - x) * (1 - y) / (1 - x);
}

float pda(float phase, float a) {
  a = 0.5 + a / 2;  // accept normalized value
  float slope = 1 / a - 1;
  if (phase < a) return phase * slope;
  return (1 - a) + (phase - a) / slope;
  // if (phase < a) return phase * (1 - a) / a;
  // return (1 - a) + (phase - a) * a / (1 - a);
}

float pdb(float phase, float a) {  //
  return pd(phase, a, 1 - a);
}

float sine(float phase) {
  //
  return table[(int)(phase * (N - 1.0))];
}

void process(double t, float* i, float* o) {
  _index = 0;
  t *= 1 / 10.0;
  int T = t;
  t -= T;
  t = 1 - t;
  float f = sine(pda(phasor(110 + 2 * sine(phasor(7))), t * t));
  f += sine(pda(phasor(330 + 3 * sine(phasor(5))), 1 - t * t));
  f += sine(pda(phasor(550 + 4 * sine(phasor(9))), 1 - t * (1 - t)));
  o[0] = o[1] = f * 0.1;
}
