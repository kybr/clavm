void process(double d, float* i, float* o) {
  d *= 8000 * 1.0;
  int t = d;

  int b = (t >> 7 - (t >> 15) & -t >> 7 - (t >> 15));
  if (b == 0) b = 1;
  char c = t >> 4 | t & (t >> 5) / b;

  //
  o[0] = o[1] = c / 256.0;
  // o[0] = o[1] = c / 128.0 - 1;
}
