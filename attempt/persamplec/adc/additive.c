#include "math.h"
#define N (1000)
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

void init(void) {
  for (int i = 0; i < N; i++)  //
    memory[i] = 0;
}

void process(double t, float* i, float* o) {
  _index = 0;  // reset the memory pointer

  t *= 7.4;
  int T = t;

  // https://www.desmos.com/calculator/fcj35px60j
  float e = fabs(sin(M_PI * t)) * 5;
  if (e > 1) e = 1;

  float hz = ((float[8]){55, 110, 27.5, 110, 55, 27.5, 220, 880})[T % 8];
  hz = onepole(hz, 0.996);

  float f = 0;
  {
    float s = 0;

    // int H = T % 15;
    int H = 10;
    for (int h = 1; h <= 1 + H; h++) {
      float a = 1.0 / 1;
      f += sin(2 * M_PI * phasor(hz * h)) * a;
      s += a;
    }
    f *= 1.0 / s;
  }

  float g = 0;
  {
    float s = 0;

    // int H = T % 17;
    int H = 100;
    for (int h = 1; h <= 1 + H; h++) {
      float a = 1.0 / h;
      g += sin(2 * M_PI * phasor(hz * h * 1.01)) * a;
      s += a;
    }
    g *= 1.0 / s;
  }

  o[0] = f * 0.9 * e;
  o[1] = g * 0.9 * e;
}
