#include "synth.h"

void begin() { synth_begin(); }

float delay(float x, float s) {  //
  int n = (int)_memory[_index];
}

void process(double d, float* input, float* o) {
  synth_process();
  for (int i = 0; i < 8; i++) {
    o[i] = 0;
  }

  float hz = (float[]){55, 110, 220, 440}[(int)d & 3];
  o[4] = hz / 440;

  float e = 1 - frac(2 * d);
  e = onepole(e * e, 0.96);
  o[3] = e;

  float f = sine(phasor(hz)) * e;

  o[0] = o[1] = f * 0.9;
}

