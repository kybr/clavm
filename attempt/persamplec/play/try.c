#define SAMPLE_RATE (44100)
#include "reverb3.c"

void printf(char*, ...);

float i[8];
float o[8];

int main() {
  for (float t = 0; t < 5; t += 1.0 / 44100) {
    process(t, i, o);
    printf("%f\n", o[0]);
  }
}
