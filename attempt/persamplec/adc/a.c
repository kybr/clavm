#include "math.h"

float frac(double d) {  //
  return d - (int)d;
}

void process(double d, float* i, float* o) {  //
  float phase = frac(110 * d);
  o[0] = sin(M_PI * 2 * phase) * 0.1;
}
