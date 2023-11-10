#include "synth.h"

void begin() { synth_begin(); }

//  multitap delay
void multitap(float x, float maximumTime, float* tap, int size) {
  int maximum = 1 + (int)(SAMPLE_RATE * maximumTime);
  int n = (int)_memory[_index];
  float* memory = &_memory[_index + 1];
  memory[n] = x;
  n++;
  if (n >= maximum)  //
    n = 0;
  _memory[_index] = n;

  for (int i = 0; i < size; i++) {
    float t = n - tap[i] * SAMPLE_RATE;
    if (t < 0)  //
      t += maximum;
    int a = t;
    t -= a;
    int b = 1 + a;
    if (b >= maximum)  //
      b = 0;
    tap[i] = (1 - t) * memory[a] + t * memory[b];
  }

  _index++;  // n
  _index += maximum;
}

float delay_modulation(float x, float t) {  //
  // it probably does not have to be so big
  float ar[1];
  ar[0] = t > 0.5 ? 0.499 : t;
  multitap(x, 0.5, ar, 1);
  return ar[0];
}

void process(double d, float* input, float* o) {
  synth_process();
  for (int i = 0; i < 8; i++) {
    o[i] = 0;
  }

  d *= 1;

  float hz = (float[]){55, 110, 220, 440}[(int)d & 3];
  o[4] = hz / 440;

  float e = 1 - frac(2 * d);
  e = onepole(e * e, 0.991);
  o[3] = e;

  float f = sine(phasor(hz)) * e;

  // f += reverb(f) * 2;
  float ar[12] = {17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61};
  for (int i = 0; i < 12; i++)  //
    ar[i] *= 0.01;
  multitap(f, 1.0, ar, 12);
  float w = 0;
  for (int i = 0; i < 12; i++) {
    w += ar[i] / 12;
  }

  float br[12] = {17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61};
  for (int i = 0; i < 12; i++)  //
    br[i] *= 0.01;
  multitap(w, 1.0, br, 12);
  float g = 0;
  for (int i = 0; i < 12; i++) {
    g += br[i] / 12;
  }

  // global pitch warp
  float p = sine(phasor(1.3));
  o[7] = p;
  p *= 0.004;
  p += 0.0041;
  float v = delay_modulation(f, p);

  o[0] = v * 0.4 + g * 0.4;
  o[1] = v * 0.4 + w * 0.4;
}

