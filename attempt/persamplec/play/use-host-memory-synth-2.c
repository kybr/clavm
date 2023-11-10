#include "synth.h"

void process(double d, float* i, float* o) {
  d *= 2.1;

  int which = d / 8;
  float mult = (float[]){8, 8, 4, 2, 1, 1.0 / 2, 1.0 / 4, 1.0 / 4}[which % 8];

  float e = frac(mult * d);
  e = 1 - e;
  e *= e;
  e *= e;
  e = 1 - e;
  e = onepole(e, 0.992);
  o[2] = e;

  float m = (float[]){58, 65, 60, 61, 58, 63, 65, 61}[(int)d % 8];
  float f = mtof(onepole(m, 0.9996));

  f *= mult;
  float fa = f, fb = f;
  fa *= 1 + tri(phasor(7.1)) * 0.02;
  fb *= 1 + tri(phasor(0.1)) * 0.02;

  float a = quasi(fa, 0.9 * tri(frac(0.2 * d)));
  float b = quasi(fb, 0.9 * tri(frac(0.2 * d + 0.7)));

  a *= e;
  b *= e;

  float ar[25] = {2,  3,  5,  7,  11, 13, 17, 19, 23, 29, 31, 37, 41,
                  43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
  for (int i = 0; i < 25; i++)  //
    ar[i] = 2.0 * ar[i] / 100;
  multitap(a, 2.0, ar, 25);
  float w = 0;
  for (int i = 0; i < 25; i++) {
    float t = (25 - i) / 25.0;
    // t = 1;
    // t = i / 25.0;
    w += t * ar[i] / 25;
  }

  float br[25] = {2,  3,  5,  7,  11, 13, 17, 19, 23, 29, 31, 37, 41,
                  43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
  for (int i = 0; i < 25; i++)  //
    br[i] = 1 * br[i] / 100;
  multitap(b, 1.0, br, 25);
  float y = 0;
  for (int i = 0; i < 25; i++) {
    float t = (25 - i) / 25.0;
    // t = 1;
    // t = i / 25.0;
    y += t * br[i] / 25;
  }

  float r = onepole(frac(d / 31), 0.999);
  o[3] = r;
  o[0] = 0.5 * dcblock(mix(b, y, r));
  o[1] = 0.5 * dcblock(mix(a, w, r));
}
