// tick randomly
//
#include "math.h"

int rand();

float r() {
  // this is the number on macOS; it might be different on Linux or Windows
  return rand() / 2147483647.0f;
}

float mtof(float m) {  //
  return 8.175799 * pow(2.0, m / 12.0);
}

int edge(float value) {
  static float history = 0;
  int condition = value < history;
  history = value;
  return condition;
}

int meh(float value) {
  static float history = 0;
  int condition = value != history;
  if (condition)  //
    condition = value < history ? -1 : 1;
  history = value;
  return condition;
}

float diff(float value) {
  static float history = 0;
  float difference = value - history;
  history = value;
  return difference;
}

float edge2(float value) {
  float difference = diff(value);
  difference *= difference;
  return difference == 0 ? 0 : 1;
}

float f;
void process(double t, float* input, float* o) {
  if (r() < 0.0001) {
    f = mtof(r() * 40 + 20);
  }
  // only gets negative edges
  // o[2] = edge(f);

  //
  // o[2] = meh(f);

  // o[3] = diff(f) / 100;
  o[3] = edge2(f);

  float g = t * f;
  g -= (int)g;
  g = sin(2 * M_PI * g);
  o[0] = o[1] = g * 0.1;
}
