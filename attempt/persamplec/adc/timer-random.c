#include "synth.h"

// oscillator that outputs booleans
int timer(float frequency) {
  static float phase = 0;
  phase += frequency / SAMPLE_RATE;
  if (phase > 1) {
    phase -= 1;
    return 1;
  }
  return 0;
}

int rand();

float r() {
  // this is the number on macOS; it might be different on Linux or Windows
  return rand() / 2147483647.0f;
}

void begin() {  //
  synth_begin();
}

float f = 0;
void process(double d, float *i, float *o) {
  synth_process();

  if (timer(5))  //
    f = mtof(20 + 70 * r());
  float hz = onepole(f, 0.993);
  o[7] = ftom(hz) / 127 * 5;
  float g = sine(phasor(hz));

  o[0] = o[1] = 0.1 * g;
}
