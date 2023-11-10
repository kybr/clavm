#include "synth.h"

float ar[] = {55, 110, 220, 550};

void process(double d, float* input, float* o) {
  d *= 1;
  int D = d;
  d -= D;  // take the fractional part

  // float f = ar[D % 4];
  float f = (float[]){55, 110, 220, 550}[D % 4];  // 0...0000011
  // float f = (float[]){55, 110, 220, 550}[D & 3]; // 0...0000011

  o[2] = ftom(f) / 127;

  d = 2 * d - 1;
  o[7] = (int)D % 8;
  o[0] = o[1] = d * 0.30;
}
