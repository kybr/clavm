// example of sequencing
//
#include "math.h"

float map(float value, float low, float high, float Low, float High) {
  return Low + (High - Low) * ((value - low) / (high - low));
}
float norm(float value, float low, float high) {
  return (value - low) / (high - low);
}
float lerp(float a, float b, float t) { return (1 - t) * a + t * b; }

float dbtoa(float db) { return 1.0 * pow(10.0, db / 20.0); }
float atodb(float a) { return 20.0 * log10(a / 1.0); }
float sigmoid(float x) { return 2.0 / (1.0 + exp(-x)) - 1.0; }

float mtof(float m) { return 8.175799 * pow(2.0, m / 12.0); }
float ftom(float f) { return 12.0 * log2(f / 8.175799); }

#define N (500)

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

float onepole(float x0, float a1) {
  float y1 = memory[_index];
  y1 = (1 - a1) * x0 + a1 * y1;
  memory[_index] = y1;
  _index++;
  return y1;
}

float biquad(float x0, float b0, float b1, float b2, float a1, float a2) {
  float* m = &memory[_index];
  float x1 = m[0], x2 = m[1], y1 = m[2], y2 = m[3];
  float y0 = b0 * x0 + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
  m[0] = x0;
  m[1] = x1;
  m[2] = y0;
  m[3] = y1;
  _index += 4;
  return y0;
}

float lpf(float x0, float f0, float Q) {
  float omega = 2 * M_PI * f0 / SAMPLE_RATE;
  float alpha = sin(omega) / (2 * Q);

  float a0 = 1 + alpha;
  float a1 = -2 * cos(omega);
  float a2 = 1 - alpha;
  float b0 = (1 - cos(omega)) / 2;
  float b1 = 1 - cos(omega);
  float b2 = (1 - cos(omega)) / 2;

  return biquad(x0, b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
}

float notch(float x0, float f0, float Q) {
  float omega = 2 * M_PI * f0 / SAMPLE_RATE;
  float alpha = sin(omega) / (2 * Q);

  float a0 = 1 + alpha;
  float b0 = 1;
  float b1 = -2 * cos(omega);
  float b2 = 1;
  float a1 = -2 * cos(omega);
  float a2 = 1 - alpha;

  return biquad(x0, b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
}

float multi(float phase, float t) {
  unsigned i = phase * 4;
  float f = phase * 4 - i;
  float g = f;

  switch (i) {
    default:
    case 0:
      break;
    case 1:
      f = 1 - f;
      break;
    case 2:
      f = -f;
      break;
    case 3:
      f = f - 1;
      break;
  }

  if (t > 0) {
    if (i & 1)
      ;
    else
      g = 1 - g;

    g = g * g;

    if (i > 1)
      g = g - 1;
    else
      g = 1 - g;
  } else {
    if (i & 1)  //
      g = 1 - g;

    g = g * g;

    if (i > 1)  //
      g = -g;

    // fix t
    t = -t;
  }

  return (1 - t) * f + t * g;
}

void init(void) {
  for (int i = 0; i < N; i++)  //
    memory[i] = 0;
}

void process(double t, float* i, float* o) {
  _index = 0;

  t *= 5.1;
  int T = t;
  float e = t - T;

  float f = mtof(((float[8]){60, 47, 62, 48, 51, 63, 75, 59})[T % 8]);
  f = onepole(f, 0.997);
  f *= 1.0;

  float z = mtof(40 + 80 * (1 - e));
  z = onepole(z, 0.997);

  float r = sin(13.2 * t);

  float g = 0;
  {
    float p = phasor(f);
    g = multi(p, r) / 2;
    g = lpf(g, z, 1.9);
  }

  // i can write this without clicks

  float h = 0;
  {
    float p = phasor(f * 0.987);
    h = multi(p, r) / 2;
    h = lpf(h, z, 1.8);
  }

  o[0] = h * 0.9;
  o[1] = g * 0.9;
}
