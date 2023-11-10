#include "synth.h"

#define S (90000)
float data[S];
int n = 0;
void begin() {
  synth_begin();

  for (int i = 0; i < S; i++) {
    data[i] = 0;
  }
}

void write(float f) {
  data[n] = f;
  n++;
  if (n >= S)  //
    n = 0;
}

float read(float f) {
  int index = n - (int)(f * SAMPLE_RATE);
  if (index < 0)  //
    index += S;
  return data[index];
}

float allpass(float x, float frequency) {  //
  return 0;
}
float comb_ff(float x, float b0, float bm, float m) {  //
  return mix(read(m), 0, 0);
}

void process(double d, float* input, float* o) {
  synth_process();
  for (int i = 0; i < 8; i++) {
    o[i] = 0;
  }

  float hz = (float[]){55, 110, 220, 440}[(int)d & 3];
  o[4] = hz / 440;

  float e = 1 - frac(2 * d);
  e = onepole(e * e, 0.96);
  o[3] = e;

  float f = sine(phasor(hz)) * e;

  write(f);

  f *= 0.5;
  float v = 1.0 / 100;

  f += read(v * 19) / 10;
  f += read(v * 23) / 10;
  f += read(v * 29) / 10;
  f += read(v * 31) / 10;
  f += read(v * 37) / 10;
  f += read(v * 73) / 10;
  f += read(v * 79) / 10;
  f += read(v * 83) / 10;
  f += read(v * 89) / 10;
  f += read(v * 97) / 10;

  o[0] = o[1] = f * 0.9;
}

