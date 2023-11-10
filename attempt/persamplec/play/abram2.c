
#include "math.h"

void process(double d, float* i, float* o) {
  //
  // d = pow(d, 2);
  // d *= 2 * 2 * 2 * 2 * 2 * 2 * 2;
  d *= 1;
  int T = d;        // integer
  float f = d - T;  // fraction
  o[0] = o[1] = sin(d / f);
}
