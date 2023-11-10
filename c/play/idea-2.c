
float* _float(int);
int* _int(int);
double _time();
float _rate();
void _out(float, float);

float uniform() {
  // from the FAUST:
  // rand  = +(12345)~*(1103515245);
  // w   = rand/2147483647.0;
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

float abs(float f) {  //
  return f < 0 ? -f : f;
}

double _pow(double d, int power) {
  if (power < 1)  //
    return 1;
  double value = d;
  while (--power)  //
    value *= d;
  return value;
}

double pow(double, double);

float mtof(float m) {
  return 8.175799f * pow(2.0f, m / 12.0f);
  // return 440.0f * powf(2.0f, (m - 69.0f) / 12.0f);
}

double sin(double);

// http://scp.web.elte.hu/papers/synthesis1.pdf
float quasi(float frequency, float filter) {
  float* z = _float(2);
  float w = frequency / _rate();
  float b = 13 * _pow(0.5 - w, 4) * filter;
  float phase = 2 * phasor(frequency) - 1;
  z[0] = (z[0] + sin(3.1415927 * (phase + b * z[0]))) / 2;
  float o = 2.5 * z[0] - 1.5 * z[1];
  z[1] = z[0];
  return (o + (0.376 - w * 0.752)) * (1.0 - 2.0 * w);
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

void play() {
  float t = _time();
  t *= 1.1;
  float slow = 0.25 * t;
  int SLOW = slow;
  float fast = 8 * t;
  int FAST = fast;

  float v = 0;
  {
    float e = frac(slow);
    e = 1 - e;

    float hz = (float[]){25, 35, 40, 33}[SLOW % 4];
    v = quasi(mtof(hz), 0.8 + 0.1 * tone(phasor(4.31)));

    v *= e;
  }

  float s = 0;
  {
    float e = frac(slow);
    e = 1 - e;

    float hz = (float[]){49, 51, 47, 45}[SLOW % 4];
    s = quasi(mtof(hz), 0.6 + 0.1 * tone(phasor(5.101)));

    s *= e;
  }

  float l = 0;
  float r = 0;
  l += 0.4 * s;
  r += 0.4 * v;

  _out(l, r);
}
