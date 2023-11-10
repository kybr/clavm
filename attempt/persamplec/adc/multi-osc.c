#include "synth.h"

float multi(float phase, float t) {
  unsigned i = phase * 4;
  float f = phase * 4 - i;
  float g = f;

  switch (i) {
    default:
    case 0:
      break;
    case 1:
      f = 1 - f;
      break;
    case 2:
      f = -f;
      break;
    case 3:
      f = f - 1;
      break;
  }

  if (t > 0) {
    if (i & 1)
      ;
    else
      g = 1 - g;

    g = g * g;

    if (i > 1)
      g = g - 1;
    else
      g = 1 - g;
  } else {
    if (i & 1)  //
      g = 1 - g;

    g = g * g;

    if (i > 1)  //
      g = -g;

    // fix t
    t = -t;
  }

  return (1 - t) * f + t * g;
}

void begin(void) {  //
  synth_begin();
}

void process(double t, float* i, float* o) {
  synth_process();

  t *= 5.1;
  int T = t;
  float e = t - T;

  float f = mtof(((float[8]){60, 47, 62, 48, 51, 63, 75, 59})[T % 8]);
  f = onepole(f, 0.997);
  f *= 0.5;

  float z = mtof(40 + 80 * (1 - e));
  z = onepole(z, 0.997);

  float r = sin(1.2 * t + 0.5);
  o[2] = r;
  float l = sin(1.2 * t);
  o[3] = l;

  float g = 0;
  {
    float p = phasor(f);
    g = multi(p, r) / 2;
    g = lpf(g, z, 1.9);
  }

  // i can write this without clicks

  float h = 0;
  {
    float p = phasor(f * 0.987);
    h = multi(p, r) / 2;
    h = lpf(h, z, 1.8);
  }

  o[0] = h * 0.6 * l;
  o[1] = g * 0.6 * r;
}
