float frac(double d) {  //
  return d - (int)d;
}

void process(double t, float* i, float* o) {
  t *= 1;  // make me big
  o[0] = frac(t) * 0.4;
}
