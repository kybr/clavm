#include "synth.h"

void begin() { synth_begin(); }

// frac / intm
int counter(double d, int mod) {
  //
  return (int)d % mod;
}

void process(double d, float* i, float* o) {
  synth_process();

  float e = frac(d * 1) * 0.5;
  o[2] = e;

  float foo[] = {46, 62, 23, 38};
  float hz = mtof(foo[counter(d, 4)]);
  // float hz = mtof(foo[(int)d & 3]);
  // float hz = mtof((float[]){46, 62, 23, 38}[(int)d % 4]);

  float phase = pd(phasor(hz), e, 0.5);

  float f = sine(phase);

  o[0] = 0.2 * f;
  o[1] = 0.2 * f;
}
