
float* _float(int);
int* _int(int);
double _time();
float _rate();
void _out(float, float);
double sin(double);

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

// this should really be a double or a long int
float accum(float hz) {
  float* value = _float(1);
  *value += hz / _rate();
  return *value;
}

void play() {
  float time = _time() * 1.7;
  //float time = accum(1.5); // broken? weird slowdowns

  // snare drum
  float f = 0;
  {
    float t = time;
    t *= 0.25;
    t = frac(t);
    for (int i = 0; i < 3; ++i)  //
      t = t * t;
    t = 1 - t;

    float u = uniform();
    if (abs(u) > t)  //
      f = u;

    float e = _time();
    e *= 8;
    e = frac(e);
    e = 1 - e;
    e *= e;

    f *= e;
  }

  // bass synth
  float v = 0;
  {
    float t = time;
    t *= 0.25;
    int T = t;
    t = frac(t);
    t = 1 - t;

    float hz = ((float[]){33, 31, 29, 28})[T % 4];
    v = quasi(mtof(hz), 0.8 + 0.1 * sin(3.1415927 * 2 * phasor(4.31)));

    v *= t;
  }

  // harmony synth
  float s = 0;
  {
    float t = time;
    t *= 0.25;
    int T = t;
    t = frac(t);
    t = 1 - t;

    float hz = ((float[]){48, 47, 55, 52})[T % 4];
    s = quasi(2 * mtof(hz), 0.6 + 0.1 * sin(3.1415927 * 2 * phasor(5.101)));

    s *= t;
  }

  // ?
  float c = 0;
  {
    float t = frac(_time() * 8);
    c = edge(t) * uniform();
  }

  // "melody" synth
  float w = 0;
  {
    float t = time;
    t *= 0.5;
    int T = t;
    t = frac(t);
    t = 1 - t;

    float hz = ((float[]){48, 52, 47, 52, 47, 55, 48, 47})[T % 8];
    w = quasi(mtof(hz) * 4, 0.8 + 0.2 * sin(3.1415927 * 2 * phasor(7.101)));

    w *= t;
  }

  float o = 0;
  o += 0.2 * c; // click (unfinished kick drum)
  o += 0.2 * s; // harmony
  o += 0.2 * f; // snare drum
  o += 0.3 * v; // bass
  o += 0.2 * w; // high notes

  o *= 0.2;
  _out(o, o);
}
