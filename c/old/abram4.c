
#include "math.h"

float frac(double d) { return d - (int)d; }

// return a ramp on (0, 1) at the given frequency
float memory[100];
int index = 0;
float phasor(float frequency) {
  float* phase = &memory[index++];
  *phase += frequency / 44100;
  if (*phase > 1)  //
    *phase -= 1;
  return *phase;
}

void process(double d, float* i, float* o) {
  index = 0;
  d *= 1;
  float e = frac(d);
  e = 1 - e;
  e *= e;
  o[0] = sin(2 * M_PI * phasor(110)) * e;
  o[1] = sin(2 * M_PI * phasor(225)) * e;
}
