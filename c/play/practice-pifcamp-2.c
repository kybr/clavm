// {{{ C•L•A•V•M

float* _float(int);
double* _double(int);
int* _int(int);
double _time();
float _rate();
void _out(float, float);
double exp(double);
double log2(double);
double log10(double);
double tanh(double);
double sin(double);
double cos(double);
double pow(double, double);
double M_PI = 3.1415926535;

float uniform() {
  // from the FAUST:
  // rand  = +(12345)~*(1103515245);
  // w   = rand/2147483647.0;
  int* history = _int(1);
  *history *= 1103515245;
  *history += 12345;
  return *history / 2147483647.0;
}


float mtof(float m) { return 440.0 * pow(2.0, (m - 69.0) / 12.0); }

float ftom(float f) { return 12.0 * log2(f / 440.0) - 69.0; }

float dbtoa(float db) { return pow(10.0, db / 20.0); }

float atodb(float a) {
  return 20.0 * log10(a / 1.0);
  //
}

// aka lerp
float mix(float a, float b, float t) {  //
  return (1.0 - t) * a + t * b;
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

double tri(double phase) {
  int i = phase * 4.0;
  double f = phase * 4.0 - i;
  f = (i == 1) ? 1.0 - f : f;
  f = (i == 2) ? 1.0 - f : f;
  f = (i > 1) ? f - 1.0 : f;
  return f;
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


double pd(double phase, double x, double y) {
  if (phase < x) return phase * y / x;
  return y + (phase - x) * x / y;
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

double saw(double t) { return 2.0 * t - 1.0; }

double sine(double x) {  // expects x on (-1, 1) !!!!!
  double xx = x * x;
  return x * (3.138982 + xx * (-5.133625 + xx * (2.428288 - xx * 0.433645f)));
}

// piecewise parabola mimicking sine
double tone(double phase) {
  int i = phase * 4;
  double f = phase * 4.0 - i;
  f = (i % 2 == 0) ? 1.0 - f : f;  // flip, maybe
  f = f * f;
  f = (i < 2) ? 1.0 - f : f - 1.0;  // flip or lower
  return f;
}

// aka scale
float map(float value, float low, float high, float Low, float High) {
  return Low + (High - Low) * ((value - low) / (high - low));
}

float norm(float value, float low, float high) {
  return (value - low) / (high - low);
}

float squash(float x, float t, float s) {  //
  if (x > 0)
    return x < t ? x : t + s * (x - t);
  else
    return x > -t ? x : s * (x + t) - t;
}

float subdiv(float p, float d) {
  p *= d;
  return p - (int)p;
}

int hex(double phase, int hex) {
  int index = phase * 32;
  int value = 1 & (hex >> (31 - index));
  return value;
}

double onepole(double x, double a) {
  double* history = _double(1);
  *history = (1.0 - a) * x + a * *history;
  return *history;
}

double MS20(double x, double f, double q) {
  double* z = _double(2);
  double t = exp(-2 * M_PI * f / _rate());
  double s = tanh(q * z[1]);
  z[0] = mix(x - s, z[0], t);
  z[1] = mix(z[0] + s, z[1], t);
  return z[1];
}


// }}}
void play() {
  float t = accum(1.4);
  float whole = frac(t);
  float env = 1 - subdiv(whole, 8);
  env = onepole(env, 0.9);
  float o = 0;
  float mod = sine(saw(whole)) / 2 + 0.5;
  o += env * sine(tone(pd(phasor(55 + 10 * mod), 0.5, 0.5)));
  o += env * sine(tone(pd(phasor(55), 0.5, 0.5)));
  {
    float hz = mtof(69-24 - ((float[]){-7, 0, -12, 0, -24, 24, 12, 0})[((int)(t * 2)) % 8]);
    o += quasi(onepole(hz, 0.999), 0.5 * mod);
  }
  {
    float hz = mtof(69-24 - ((float[]){-7, 0, -12, 0, -24, 24, 12, 0})[((int)(t * 2)) % 8]);
    o += quasi(onepole(hz, 0.99), mod);
  }
  o *= 0.3;
  _out(o, o);
}

