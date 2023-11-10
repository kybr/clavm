#include "math.h"
#define N (10000)
float table[N];

// called once just after a compile
// before process ever runs
void begin() {
  for (int i = 0; i < N; i++) {
    table[i] = sin(M_PI * 2.0 * i / N);
  }
}

float line(float x, float t) {
  // has the input changed?
  static float target = 0;
  static float increment = 1;
  static float actual = 0;
  static int i = 0;

  // detect input change
  if (x != target) {
    increment = (x - target) / (t * SAMPLE_RATE);
    target = x;
  }

  if (actual != target) {
    actual += increment;

    // how do we know when we've gone too far?
  }

  return actual;
}

void process(double t, float* i, float* o) {
  t *= 100;
  int T = t;
  t -= T;  // map to (0, 1)
  int index = t * (N - 1.0);
  o[0] = o[1] = 0.1 * table[index];
}
