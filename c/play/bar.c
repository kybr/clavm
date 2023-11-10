
// external functions
//
float* _float(int);       // host memory access
float _rate(void);        // current sample rate
void _out(float, float);  // output audio
double pow(double, double);
double cos(double);

// aka lerp
float mix(float a, float b, float t) {  //
  return (1.0 - t) * a + t * b;
}

int floor(double x) {
  int xi = (int)x;
  return x < xi ? xi - 1 : xi;
}

float Phasor(float hz) {
  float* phase = _float(1);
  *phase += hz / 44100;
  *phase -= floor(*phase);
  return *phase;
}

double tri(double phase) {
  int i = phase * 4.0;
  double f = phase * 4.0 - i;
  f = (i == 1) ? 1.0 - f : f;
  f = (i == 2) ? 1.0 - f : f;
  f = (i > 1) ? f - 1.0 : f;
  return f;
}

double round(double value) {
  return value < 0.5 ? 0.0 : 1.0;
}

// for t on (-0.25, 1.25)
//
double bhaskara(double t) {
  if (t < 0.25) {
    double tt = t * t;
    return (1 - 16 * tt) / (1 + tt);
  }
  else if (t < 0.75) {
    t -= 0.5;
    double tt = t * t;
    return -(1 - 16 * tt) / (1 + tt);
  }
  else if (t >= 0.75) {
    t -= 1;
    double tt = t * t;
    return (1 - 16 * tt) / (1 + tt);
  }
  return 0;
}

float tone(float phase) {
  // break the phase ramp into 4 phase ramps
  //
  unsigned i = phase * 4;
  float f = phase * 4 - i;

  // flip
  if (i == 0 || i == 2)  //
    f = 1 - f;

  f = f * f;  // square

  if (i < 2)
    f = 1 - f;
  else
    f = f - 1;  // lower

  return f;
}

void play() {
  float f = 0;
  float p = Phasor(880);
  //float t = tri(p);
  //float m = pow(1 - pow(1 - tri(t), 1.1401), 0.867431);
  //f = mix(t, round(t)l, m);
  //f = bhaskara(p * 3.141592 - 3.141592 / 2 );
  f = tone(p);
  f = bhaskara(p);
  _out(f, f);
}
