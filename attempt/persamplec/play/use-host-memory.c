
void host_reset();
float *host_float(unsigned advance);
int *host_int(unsigned advance);
char *host_char(unsigned advance);

float phasor(float hertz) {
  float *phase = host_float(1);
  *phase += hertz / SAMPLE_RATE;
  if (*phase > 1)  //
    *phase -= 1;
  return *phase;
}

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
  o[1] = phasor(5);
}
