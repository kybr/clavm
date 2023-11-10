#include "synth.h"

void begin(void) { synth_begin(); }

void process(double t, float* i, float* o) {
  synth_process();

  int T = t * 4.4;

  float midi = 60 + ("9169854819"[T % 10] - '0');
  // float midi = 60 + ("13713518"[T % 8] - '0');

  o[7] = 7.0 * midi / 127;

  float f = 0;
  f += sine(phasor(mtof(midi)));

  o[0] = o[1] = f * 0.1;
}
