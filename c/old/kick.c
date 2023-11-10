#include "math.h"

#define N (1000)
float table[N];
void begin() {
  for (int i = 0; i < N; i++) {
    table[i] = sin(2 * M_PI * i / N);
  }
}

void process(double t, float* i, float* o) {
  double e = t * 2;
  e -= (long)e;
  e = 1 - e;
  e = e * e;
  e = e * e;
  o[2] = e;

  double phase = t * 220 * e;
  phase -= (long)phase;
  float g = table[(int)(phase * (N - 1))];
  o[0] = o[1] = g * 0.5;
}
