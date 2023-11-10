float* _float(int);
int* _int(int);
double _time();
float _rate();
void _out(float, float);

double pow(double, double);

float mtof(float m) { return 440.0 * pow(2.0, (m - 69.0) / 12.0); }

float uniform() {
  int* history = _int(1);
  *history *= 1103515245;
  *history += 12345;
  return *history / 2147483647.0;
}

int floor(double x) {
  int xi = (int)x;
  return x < xi ? xi - 1 : xi;
}

float phasor(float hz) {
  float* phase = _float(1);
  *phase += hz / 44100;
  *phase -= floor(*phase);
  return *phase;
}

int edge(float v) {
  float* history = _float(1);
  float value = v < *history;
  *history = v;
  return value;
}

float frac(double v) {
  return v - (int)v;
  //
}

float tone(float phase) {
  unsigned i = phase * 4;
  float f = phase * 4 - i;
  if (i == 0 || i == 2) f = 1 - f;
  f = f * f;
  if (i < 2)
    f = 1 - f;
  else
    f = f - 1;
  return f;
}

float subd(float phase, float division) {  //
  return frac(phase * division);
}

float imp(float phase) {
  unsigned i = phase * 4;
  float f = phase * 4 - i;
  if (i == 1 || i == 3)  //
    f = 1 - f;
  f = f * f;
  if (i > 1)  //
    f = -f;
  return f;
}

float onepole(float x, float a) {
  float* history = _float(1);
  *history = (1 - a) * x + a * *history;
  return *history;
}

// byte(t, 0xDEADBEEF)
int byte(float phase, int pattern) {
  unsigned i = 32 * phase;
  return 1 & (pattern >> i);
}

double exp(double);
double tanh(double);

// aka lerp
float mix(float a, float b, float t) {  //
  return (1 - t) * a + t * b;
}

float ms20(float x, float f, float q) {
  float* z = _float(2);
  float t = exp(-6.283185307179586 * f / _rate());
  float s = tanh(q * z[1]);
  z[0] = mix(x - s, z[0], t);
  z[1] = mix(z[0] + s, z[1], t);
  return z[1];
}

// like subd but it returns 0 for even "beats", leaving the odds
//
float subo(float phase, float division) {
  unsigned i = phase * division;
  if (i % 2 == 0)
    return 0;
  else
    return 1 - (phase * division - i);
}

void play() {
  float t = _time();

  float s = 0.25 * t;
  s = frac(s);

  float l = 0;
  float r = 0;

  {
    float f = mtof((float[]){59, 60, 57, 60, 54, 50, 60, 59}[(int)t % 8]);
    f = onepole(f, 0.992);
    float d = tone(phasor(f));
    d *= byte(s, 0xDEADBEEF);
    d = onepole(d, 0.997);
    l += d;
  }
  {
    float f = mtof((float[]){66, 67, 65, 67, 62, 57, 66, 68}[(int)t % 8]);
    f = onepole(f, 0.992);
    float d = tone(phasor(f));
    d *= byte(s, 0xBEEFDEAD);
    d = onepole(d, 0.997);
    r += d;
  }
  _out(l, r);
}
