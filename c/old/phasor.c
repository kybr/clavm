
static float f = 0;
void process(double t, float* i, float* o) {
  f += 0.001;
  if (f > 1.0)  //
    f -= 1.0;
  // f = 2 * f - 1;
  o[0] = o[1] = 2 * f - 1;
}
