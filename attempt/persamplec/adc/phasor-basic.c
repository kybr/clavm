// phase wrap
float fract(double t) { return t - (int)t; }

// [0, 1)
float saw(float phase) { return 2 * phase - 1; }

float phasor(unsigned which, float frequency) {
  // trick with static memory
  static struct { float phase; } state[10];
  float phase = state[which].phase;
  phase += frequency / SAMPLE_RATE;
  if (phase > 1) phase -= 1;
  state[which].phase = phase;
  return phase;
}

void process(double t, float* i, float* o) {
  float e = fract(t * 4.3);

  e = 1 - e;
  e = e * e;

  float p = saw(phasor(0, 55.0));
  float q = saw(phasor(1, 55.1));
  // float p = saw(fract(t * 27.5));
  // float q = saw(fract(t * 55.1));

  p *= e;
  q *= e;

  p *= 0.4;
  q *= 0.4;

  o[0] = p;
  o[1] = q;
}
