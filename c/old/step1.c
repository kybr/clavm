void process(double d, float* i, float* o) {
  float t = 4 * d;
  int T = t;
  t -= T;
  t = t * t;

  o[0] = o[1] = t * 0.1;
}
