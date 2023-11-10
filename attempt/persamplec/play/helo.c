#include "synth.h"

void begin() {  //
  synth_begin();
}

void process(double d, float* i, float* o) {  //
  synth_process();

  d *= 1.1;

  float e = frac(4 * d);
  e = 1 - e;
  e = onepole(e, 0.99);
  o[2] = e;

  float g = frac(8 * d);
  g = 1 - g;
  g = onepole(g, 0.99);
  o[3] = g;

  float h = ((float[]){54, 67, 34, 65, 34, 67, 54, 71})[(int)(d * 4) % 8];

  h = onepole(h, 0.996);
  o[4] = h / 127;

  float f = quasi(mtof(h) / 2, 0.6 * g);

  f = f * e;

  f *= 1 - frac(d * 0.125);

  o[0] = f * 0.5;
  o[1] = f * 0.5;
}
