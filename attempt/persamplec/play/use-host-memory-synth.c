#include "synth.h"

void process(double t, float *i, float *o) {
  for (int i = 0; i < 8; i++) {
    o[i] = 0;
  }

  o[0] = 0.3 * quasi(220, 0.5);
  o[1] = 0.3 * quasi(221, 0.5);
}
