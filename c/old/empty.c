void init(void) {}

void process(double t, float* i, float* o) {
  t *= 100;
  int T = t;
  t -= T;
  t *= 0.1;
  o[0] = o[1] = t;
}
