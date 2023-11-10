//
//

#define N (100)
float memory[N];
unsigned index = 0;

float phasor(float frequency) {
  float phase = memory[index];
  phase += frequency / SAMPLE_RATE;
  while (phase > 1)  //
    phase -= 1;
  memory[index] = phase;
  index++;
  return phase;
}

float saw(float phase) {
  return 2 * phase - 1;  //
}

// a sine-like waveform without any trig
//
float tone(float phase) {
  unsigned i = phase * 4;      // make an integer: 0, 1, 2, or 3
  float f = (phase * 4) - i;   // get the fractional part
  if ((i + 1) & 1) f = 1 - f;  // pre-invert if 0 or 2
  if ((2 + i) & 2)
    f = 0.5 * (1 - f * f) + 0.5;
  else
    f = 0.5 * (f * f);
  return 2 * f - 1;
}

void init(void) {
  for (int i = 0; i < N; i++)  //
    memory[i] = 0;
}

void process(double t, float* i, float* o) {
  index = 0;

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
