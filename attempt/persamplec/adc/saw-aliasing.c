// example of a naive aliasing sawtooth
//

void process(double t, float* i, float* o) {
  //
  float frequency = 110;

  //
  float phase = t * frequency;

  // "wrap" phase
  phase -= (int)phase;

  // make a full-range saw from a ramp
  float a = 2 * phase - 1;

  // reduce amplitude
  a *= 0.1;

  // output
  o[0] = o[1] = a;
}
