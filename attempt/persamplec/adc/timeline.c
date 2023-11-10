#include "synth.h"

void begin() {  //
  synth_begin();
}

typedef struct {
  int i;
  float f;
} timeline_t;

timeline_t timeline(double d) {  //
  return (timeline_t){(int)d, d - (int)d};
}

void process(double d, float* i, float* o) {  //
  synth_process();

  d *= 0.7;

  timeline_t quarter = timeline(4 * d);
  quarter.f = 1 - quarter.f;
  quarter.f = onepole(quarter.f, 0.99);
  o[2] = quarter.f;

  timeline_t eighth = timeline(8 * d);
  eighth.f = 1 - eighth.f;
  eighth.f = onepole(eighth.f, 0.99);
  o[3] = eighth.f;

  float midi = ((float[]){54, 67, 34, 65, 34, 67, 54, 71})[quarter.i % 8];

  midi = onepole(midi, 0.996);
  o[7] = 5 * midi / 127;

  float saw = quasi(mtof(midi) / 2, 0.3 * eighth.f);

  saw = saw * eighth.f;

  timeline_t fourbar = timeline(0.25 * d);

  saw *= fourbar.f;

  o[0] = saw * 0.4;
  o[1] = saw * 0.4;
}
