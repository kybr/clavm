// example of sequencing
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
  // break the phase ramp into 4 phase ramps
  //
  unsigned i = phase * 4;
  float f = phase * 4 - i;

  // flip
  if (i == 0 || i == 2)  //
    f = 1 - f;

  f = f * f;  // square

  if (i < 2)
    f = 1 - f;
  else
    f = f - 1;  // lower

  return f;
}

float tri(float phase) {
  unsigned i = phase * 4;
  float f = phase * 4 - i;
  // flip
  if (i == 1 || i == 2)  //
    f = 1 - f;
  // lower
  if (i > 1)  //
    f = f - 1;
  return f;
}

float tri2(float phase) {
  unsigned i = phase * 4;
  float f = phase * 4 - i;
  switch (i) {
    case 0:
      return f;  // noop
    case 1:
      return 1 - f;  // flip
    case 2:
      return -f;  // negate
    case 3:
      return f - 1;  // lower
  }
}

float imp(float phase) {
  unsigned i = phase * 4;
  float f = phase * 4 - i;

  if (i == 1 || i == 3)  //
    f = 1 - f;

  f = f * f;

  if (i > 1)  //
    f = -f;

  return f;
}

void init(void) {
  for (int i = 0; i < N; i++)  //
    memory[i] = 0;
}

void process(double t, float* i, float* o) {
  index = 0;

  t *= 3.1;
  int T = t;

  float g = 0;
  {
    float f = ((float[8]){55, 110, 220, 440, 55, 440, 110, 220})[T % 8];
    float p = phasor(f);
    for (int i = 0; i < T % 7; i++)  //
      p *= p;

    switch (T % 3) {
      case 0:
        g = tri(p);
        break;
      case 1:
        g = tone(p);
        break;
      case 2:
        g = imp(p);
        break;
    }
  }

  float h = 0;
  {
    float f = ((float[8]){55, 110, 220, 440, 55, 440, 110, 220})[T % 7];
    float p = phasor(3 * f / 2);
    for (int i = 0; i < T % 5; i++)  //
      p *= p;

    switch (T % 3) {
      case 0:
        h = tri(p);
        break;
      case 1:
        h = tone(p);
        break;
      case 2:
        h = imp(p);
        break;
    }
  }

  o[0] = h * 0.2;
  o[1] = g * 0.2;
}
