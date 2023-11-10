
float* _float(int);       // host memory access
float _rate(void);        // current sample rate
void _out(float, float);  // output audio
double sin(double);       // math.h
double pow(double, double);

float phasor(float hz) {
  float* phase = _float(1);
  *phase += ((hz < 0) ? -hz : hz) / _rate();
  *phase -= (int)(*phase);
  return *phase;
}

float onepole(float x, float a) {
  float* history = _float(1);
  *history = (1 - a) * x + a * *history;
  return *history;
}

void play() {
  // test the effects of "randomized" ordering or just reordering on the system, the sound.
  // idea: ask which functions/processes are sensitive to "bad" values. such as a filter
  // with sensitive coefficients? functions with shared history
  float f = phasor(2);
  if (2 * f > 1) {
    f = onepole(phasor(300), 0.5);
  }
  else {
    f = onepole(phasor(300), 0.99);
  }
  f = 2 * f - 1;
  _out(f, f);
}
