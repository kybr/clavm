
#include "math.h"

void process(double d, float* i, float* o) {
  //
  double p1 = d * 1000;
  double p2 = d * (sin(d) + 1000);
  o[2] = p1 * 0.1;
  float a = sin(p1) * 0.1;
  float b = sin(p2) * 0.1;
  o[0] = o[1] = a + b;
}
