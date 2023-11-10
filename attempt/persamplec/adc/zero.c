void process(double t, float* i, float* o) {
  for (int i = 0; i < 8; i++) {
    o[i] = 0;
  }
  o[2] = 0.5;
  // 0 / 0;
  // o[1000000] = 0;
}
