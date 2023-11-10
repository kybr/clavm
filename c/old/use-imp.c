#include "synth.h"

void begin() { synth_begin(); }

void process(double t, float* i, float* o) {
  synth_process();
  float p = pda(phasor(55), phasor(0.01));
  float f = sine(p);
  o[0] = o[1] = f * 0.7;
}
