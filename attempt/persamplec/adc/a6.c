#include "math.h"

#define N (1000)
float table[N];

void begin() {
  for (int i = 0; i < N; i++) {
    table[i] = sin(M_PI * 2 * i / N);
  }
}

void process(double d, float* input, float* o) {
  float e = 110 * d;
  e -= (int)e;
  // e = sin(M_PI * 2 * e);
  e = table[(int)(e * (N - 1.0))];

  d *= 2;
  int D = d;
  d -= D;
  d = 1 - d;

  o[0] = o[1] = e * d * 0.4;
}
