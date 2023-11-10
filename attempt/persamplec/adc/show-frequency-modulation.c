#include "synth.h"

void begin() { synth_begin(); }

void process(double t, float* i, float* o) {
  synth_process();

  float index = 150;       // modify me
  float modulation = 200;  // modify me

  float f = 440 + index * sine(phasor(modulation));

  o[0] = o[1] = sine(phasor(f)) * 0.1;

  float g = sine(phasor(f / 20));
  o[2] = g;

  o[7] = f / 300;
}
