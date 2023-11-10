
float* _float(int);
int* _int(int);
double _time();
float _rate();
void _out(float, float);

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
float byte(float phase, int pattern) {
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
  t *= 0.9;
  int T = t;
  int F = 2 * t;
  t = frac(t);

  float l = 0;
  float r = 0;

  float u = uniform();
  float w = uniform();

  if (1) {
    if (edge(subd(t, 4))) l = w;
    if (edge(subd(t, 8))) r = u;
  }

  if (1) {
    float e = subo(t, 2);
    e = e * e;
    l += 0.1 * w * e;
    r += 0.1 * u * e;
  }

  if (0)  //
  {
    float e = imp(subd(t, 0.5));
    for (int i = 0; i < 2; ++i)  //
      e *= e;
    e *= 1 - subd(t, 2);

    l += e * u * 0.4;
    r += e * w * 0.4;
  }

  float hz = (float[]){0, 0, 27.5, 55, 0, 55, 27.5, 880}[(F) % 8];
  hz = onepole(hz, 0.998);
  float k = tone(phasor(hz));
  k *= 1 - subd(t, 2);

  l += 0.6 * k;
  r += 0.6 * k;

  l = tanh(l);
  r = tanh(r);

  _out(l, r);
}
