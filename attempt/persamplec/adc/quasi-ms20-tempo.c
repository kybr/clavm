#include "synth.h"

void process(double t, float* i, float* o) {
  synth_process();

  if (0) {
    static double k = 0;
    float r = pow(2, 1.3 * (2 * phasor(1.0 / 17) - 1));
    k += 0.00005 * r;
    t = k;
  } else {
    t *= 5.23;
  }

  int T = t;
  float e = t - T;  //
  e = 1 - e;
  // e = e * e;
  e = onepole(e, 0.97);
  o[2] = e;
  float f = onepole(((float[]){34, 37, 44, 53, 36, 57, 33, 48})[T % 8], 0.996);
  o[7] = f / 127;
  float v = quasi(mtof(f), 0.9 * (1 - e));
  o[3] = 1 - e;
  v = e * ms20(v, mtof(190 * mix(0.1, 0.7, subdiv(e, 3))), 1.5);
  o[0] = o[1] = v * 0.407;
}
