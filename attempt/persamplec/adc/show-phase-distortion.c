#include "synth.h"

void begin() { synth_begin(); }

void process(double t, float* i, float* o) {
  synth_process();

  float e = frac(t * 0.0625);

  float q = pda(frac(220.1 * t), e);
  float g = sine(q);
  o[0] = g * 0.2;
  float p = pda(frac(220 * t), e);
  float f = sine(p);
  o[1] = f * 0.2;

  // show a lower frequency; easier to see what's happening
  float y = pda(frac(t), e);
  o[2] = y;
  o[3] = sine(y);
}
