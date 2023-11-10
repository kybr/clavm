#include "synth.h"

void begin() { synth_begin(); }

void process(double d, float* i, float* o) {  //
  synth_process();

  //  if (edge(phasor(1)))
  //    o[0] = o[1] = 1;
  //  else
  //    o[0] = o[1] = 0;

  o[0] = o[1] = edge(phasor(1));
}
