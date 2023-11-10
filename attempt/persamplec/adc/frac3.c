
float frac(double d) {  //
  return d - (int)d;
}

void process(double t, float* i, float* o) {
  t *= 1;  // tempo adjust

  float saw = 2 * frac(55 * t) - 1;
  float env = frac(4 * t);  // make me big

  env = 1 - env;  // comment me out

  o[0] = o[1] = saw * env * 0.1;
}
