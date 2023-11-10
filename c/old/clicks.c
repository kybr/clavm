#include "math.h"
#define N (1000)

float memory[N];
unsigned _index = 0;

float mix(float a, float b, float t) {  //
  return (1 - t) * a + t * b;
}

float ms20(float x, float f, float q) {
  float* z = &memory[_index];
  _index += 2;
  float t = exp(-2 * M_PI * f / 44100);
  float s = tanh(q * z[1]);
  z[0] = mix(x - s, z[0], t);
  z[1] = mix(z[0] + s, z[1], t);
  return z[1];
}

// returns identity if the amplitude is under t
// and
float squash(float x, float t, float s) {  //
  if (x > 0)
    return x < t ? x : t + s * (x - t);
  else
    return x > -t ? x : s * (x + t) - t;
}

void process(double t, float* i, float* o) {
  _index = 0;
  t *= 1;
  int T = t;
  t -= T;
  // t = 1 - t;
  t = ms20(t, 220, 1.9);
  t = ms20(t, 110, 1.8);
  t = squash(2 * t, 0.8, 0.2);
  // t = tanh(5 * t);
  o[0] = o[1] = t * 0.9;
}
