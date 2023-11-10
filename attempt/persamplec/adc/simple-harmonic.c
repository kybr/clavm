#include "math.h"

#define N (1000000)
float table[N];

// called once just after a compile
// before process ever runs
void begin() {
  for (int i = 0; i < N; i++) {
    table[i] = sin(M_PI * 2.0 * i / N);
  }
}

void process(double t, float* i, float* o) {
  float g = t * 300;
  g -= (int)g;
  int index = g * (N - 1.0);
  float f = 0.1 * table[index];

  {
    float g = t * 500;
    g -= (int)g;
    int index = g * (N - 1.0);
    f += 0.1 * table[index];
  }
  o[0] = o[1] = f * 0.9;
}
