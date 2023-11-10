

void process(double t, float* i, float* o) {
  t *= 1;  // make me big
  int T = t;
  float fraction = t - T;
  o[0] = fraction * 0.4;
}
