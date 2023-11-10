#include "synth.h"

// a sine-like waveform without any trig
//
void begin(void) {  //
  synth_begin();
}

void process(double t, float* i, float* o) {
  synth_process();

  t *= 2.4;

  float g = 0;
  {
    float f = ((float[8]){55, 110, 220, 440, 55, 440, 110, 220})[((int)t) % 8];
    float p = phasor(f);
    for (int i = 0; i < (((int)t) % 7); i++)  //
      p *= p;

    g = tone(p);
  }

  float h = 0;
  {
    float f = ((float[8]){55, 110, 220, 440, 55, 440, 110, 220})[((int)t) % 7];
    float p = phasor(f);
    for (int i = 0; i < (((int)t) % 5); i++)  //
      p *= p;

    h = tone(p);
  }

  o[0] = h * 0.2;
  o[1] = g * 0.2;
}
