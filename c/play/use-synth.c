#define SAMPLE_RATE (44100)
#include "play/synth.h"

double _time();
void _out(float, float);

void play() {  //
  double d = _time();
  d *= 1;
  float hz = (float[]){55, 110, 220, 440}[(int)d & 3];
  float e = 1 - frac(2 * d);
  e = onepole(e * e, 0.991);
  float f = tone(phasor(0.3));
  float v = quasi(hz + f * 3, 0.7) * e;

  float l = 0, r = 0;

  l += 0.2 * v;
  r += 0.2 * v;
  _out(l, r);
}

