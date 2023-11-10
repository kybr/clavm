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

int negedge(float value) {
  static float history = 0;
  int condition = value < history;
  history = value;
  return condition;
}

int posedge(float value) {
  static float history = 0;
  int condition = value > history;
  history = value;
  return condition;
}

int compare(float value) {
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

float f;
void process(double t, float* input, float* o) {
  if (r() < 0.0001) {
    f = mtof(r() * 40 + 20);
  }

  double g = t * f;
  g -= (int)g;
  g = sin(2 * M_PI * g);
  o[0] = o[1] = g * 0.1;

  // only gets negative edges
  o[2] = negedge(f);
  o[3] = posedge(f);
  o[4] = compare(f);
  o[5] = o[7] = 0;
  o[6] = diff(f) / 80;
}
