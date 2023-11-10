#include "synth.h"

int rand();

float r() {
  // this is the number on macOS; it might be different on Linux or Windows
  return rand() / 2147483647.0f;
}

void begin() {  //
  synth_begin();
}

float f;
void process(double t, float* input, float* o) {
  synth_process();
  if (r() < 0.0001) {
    f = mtof(r() * 40 + 30);
  }

  float hz = onepole(f, 0.996);
  o[7] = ftom(hz) / 127 * 7;
  float p = phasor(hz);
  float g = sine(p);
  o[0] = o[1] = g * 0.1;
}
