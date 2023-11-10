void process(double d, float* input, float* o) {
  d *= 10;
  int D = d;
  d -= D;
  o[0] = o[1] = d * 0.2;
}
