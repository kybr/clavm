void process(double d, float* _, float* o) {
  // simulate 8000 Hz playback rate
  d *= 8000.0 * 1.5;
  int t = d;
  char r = t * 5 & (t >> 7) | t * 3 & (t * 4 >> 10);
  o[0] = o[1] = r / 128.0 * 0.1;
}
