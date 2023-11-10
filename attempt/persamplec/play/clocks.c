#include "math.h"
#define N (100)
float memory[N];
unsigned _index = 0;

float phasor(float frequency) {
  float phase = memory[_index];
  phase += frequency / SAMPLE_RATE;
  while (phase > 1)  //
    phase -= 1;
  memory[_index] = phase;
  _index++;
  return phase;
}

float quasi(float frequency, float filter) {
  float* z = &memory[_index];
  _index += 2;
  float w = frequency / 44100;
  float b = 13 * pow(0.5 - w, 4.0) * filter;
  float phase = 2 * phasor(frequency) - 1;
  z[0] = (z[0] + sin(M_PI * (phase + b * z[0]))) / 2;
  float o = 2.5 * z[0] - 1.5 * z[1];
  z[1] = z[0];
  return (o + (0.376 - w * 0.752)) * (1.0 - 2.0 * w);
}

typedef struct {
  float f;  // ramp value; fractional part
  int i;    // ramp index; integer part
} clock;

clock make(double t) {
  int i = t;
  float f = t - i;
  return (clock){f, i};
}

float mtof(float m) {  //
  return 8.175799 * pow(2.0, m / 12.0);
}

float onepole(float x0, float a1) {
  float y1 = memory[_index];
  y1 = (1 - a1) * x0 + a1 * y1;
  memory[_index] = y1;
  _index++;
  return y1;
}

float az(const char* s) {
  switch (s[0]) {
    case 'a':
      return 0;
  }
  return 0;
}

void process(double t, float* i, float* o) {
  _index = 0;
  t *= 2.1;
  clock c = make(t * 2);
  clock k = make(t / 2);
  clock s = make(t / 8);
  float x = mtof(((float[]){46, 41, 34, 36})[s.i & 3]);
  float f = quasi(x, k.f - (c.i & 1 ? 0 : 1));
  f *= onepole(1 - c.f, 0.99 + 0.0099 * s.f);

  float w = mtof(((float[]){50, 44, 41, 48})[s.i & 3]);
  float q = quasi(w, 0.3 * onepole(k.f, 0.999) + 0.15);

  f += q;

  o[0] = o[1] = f * 0.2;
}
