void process(double d, float* input, float* o) {
  float e = 110 * d;
  e -= (int)e;
  e = 2 * e - 1;

  d *= 5;
  int D = d;
  d -= D;
  d = 1 - d;

  o[0] = e * d * 0.2;
  o[1] = e * (1 - d) * 0.2;
}
