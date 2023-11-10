#include "synth.h"

void begin() { synth_begin(); }

void process(double d, float* i, float* o) {  //
  synth_process();

  float a = frac(d / 4);
  a = 1 - a;
  a = a * a;
  a = a * a;
  a = 1 - a;
  a = 0.5 * a + 0.5;
  o[0] = o[1] = onepole(1 - frac(4 * d), a) * sine(phasor(55));

  // o[0] = o[1] = onepole(edge(frac(16 * d)), a);
}
