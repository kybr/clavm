#include "synth.h"

void begin() { synth_begin(); }

// precondition: s * SAMPLE_RATE < size
//
float delay(float x, float s, int size) {
  int n = (int)_memory[_index];
  float* memory = &_memory[_index + 1];
  memory[n] = x;
  n++;
  if (n >= size)  //
    n = 0;
  _memory[_index] = n;
  float t = n - s * SAMPLE_RATE;
  if (t < 0)  //
    t += size;
  float v = memory[(int)t];
  _index++;  // n
  _index += size;
  return v;
}

float v = 0;
void process(double d, float* input, float* o) {
  synth_process();
  for (int i = 0; i < 8; i++) {
    o[i] = 0;
  }

  d *= 2.3;

  float hz = (float[]){55, 110, 220, 440}[(int)d & 3];
  o[4] = hz / 440;

  float e = 1 - frac(2 * d);
  e = onepole(e * e, 0.991);
  o[3] = e;

  float f = sine(phasor(hz)) * e;

  f = v = delay((f + v) / 2, 0.125, 44100);

  o[0] = o[1] = f * 0.9;
}

