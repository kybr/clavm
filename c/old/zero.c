float frac(double d) {
  return d - (int)d;
  //
}

void process(double t, float *i, float *o) {
  for (int i = 0; i < 8; i++) {
    o[i] = 0;
  }

  float e = frac(4 * t);
  o[0] = e;
}
