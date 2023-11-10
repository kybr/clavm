#include "math.h"
void process(double t, float* i, float* o) {
  float g = t * 110;
  g -= (int)g;
  float f = sin(M_PI * 2 * g);
  o[0] = o[1] = f * 0.1;
}
