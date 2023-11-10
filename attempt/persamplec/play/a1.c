void process(double d, float* input, float* o) {
  d *= 1;
  int D = d;
  d -= D;
  d = 2 * d - 1;
  o[0] = o[1] = d * 0.5;
}
