#include "math.h"
void process(double d, float* input, float* o) {
  float e = 110 * d;
  e -= (int)e;
  e = sin(M_PI * 2 * e);

  d *= 3;
  int D = d;
  d -= D;
  d = 1 - d;

  o[0] = o[1] = e * d * 0.2;
}
