#include "synth.h"

void begin() {
  synth_begin();
  //
}

void process(double t, float* i, float* o) {
  synth_process();

  float e = 4 * t;
  e -= (int)e;
  e = e * e;
  e = 1 - e;
  float p = pda(phasor(55), phasor(0.499));
  float f = sine(p);
  f = lpf(f, mtof(phasor(2.01) * 90 + 20), 1.5);
  f *= onepole(e, 0.91);
  o[0] = f * 0.2;
  o[1] = f * 0.2;
}
