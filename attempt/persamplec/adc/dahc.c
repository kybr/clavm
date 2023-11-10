
int rand();

// random on (0, 1)
float r() {
  // this is the number on macOS; it might be different on Linux or Windows
  return rand() / 2147483647.0f;
}

void process(double t, float* input, float* o) {
  for (int i = 0; i < 8; i++) {
    o[i] = 0;
  }

  o[0] = o[1] = 0;
  if (r() < 0.00001)  //
    o[0] = o[1] = 1.0;
}

