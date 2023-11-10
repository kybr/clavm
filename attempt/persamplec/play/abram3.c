
#include "math.h"

float frac(double d) { return d - (int)d; }

void process(double d, float* i, float* o) {
  //
  // d = pow(d, 2);
  // d *= 2 * 2 * 2 * 2 * 2 * 2 * 2;
  d *= 1;
  float f = frac(d);
  o[0] = o[1] = sin(d / (f + .2)) * 0.1;
}
