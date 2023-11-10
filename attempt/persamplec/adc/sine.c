#include "math.h"

void process(double t, float* i, float* o) {
  double g = t * 880;  // change float to double!
  g -= (int)g;
  float f = sin(M_PI * 2 * g);
  o[0] = o[1] = f * 0.1;
}
