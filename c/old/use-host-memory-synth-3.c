#include "synth.h"

void process(double d, float* i, float* o) {
  d *= 1.1;

  int which = d / 6;
  float mult = (float[]){4, 2, 1, 1.0 / 2, 1.0 / 4}[which % 5];

  float e = frac(mult * d);
  e = e * e;
  e = 1 - e;
  e = onepole(e, 0.991);

  float m = (float[]){58, 65, 60, 61, 58, 63, 65, 61}[(int)d % 8];
  float f = mtof(onepole(m, 0.9996));

  f *= 0.5;

  float a = tri(pda(phasor(f), 0.66));

  a = diode(a, 1 - frac(0.125 * d) * 0.9, e * 7);

  o[0] = 0.7 * a;
  o[1] = 0.7 * a;
}
