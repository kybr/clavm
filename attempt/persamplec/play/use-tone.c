
#include "synth.h"

void process(double t, float* i, float* o) {
  synth_process();

  int T = t;

  float p = phasor(110);
  float f = tone(p);
  float g = sin(2 * M_PI * p);

  o[0] = o[1] = T % 2 ? g : f;
}
