#include "synth.h"

void begin() {  //
  synth_begin();
}

void process(double t, float* i, float* o) {
  synth_process();

  int T = t;

  float p = phasor(110);
  float f = tone(p) * 0.7;
  float g = sine(p) * 0.7;

  o[0] = o[1] = T % 2 ? g : f;

  o[2] = T % 2 ? -0.5 : 0.5;
}
