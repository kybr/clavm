// external functions
//
float* _float(int);       // host memory access
int* _int(int);       // host memory access
float _rate(void);        // current sample rate
void _out(float, float);  // output audio

double onepole(double x, double a) {
  double* history = _double(1);
  *history = (1.0 - a) * x + a * *history;
  return *history;
}

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

double tri(double phase) {
  int i = phase * 4.0;
  double f = phase * 4.0 - i;
  f = (i == 1) ? 1.0 - f : f;
  f = (i == 2) ? 1.0 - f : f;
  f = (i > 1) ? f - 1.0 : f;
  return f;
}

float svf(float x, float g, float R, float h, float b, float l) {
  float* z = _float(2);
  float yb = z[0];
  float yl = z[1];
  float yh = x - (2 * R * yb + yl);
  z[0] += g * yh;
  z[1] += g * yb;
  return h * yh + b * yb + l * yl;
}

// from THE ART OF VA FILTER DESIGN 
// section 4.4 page 110-111
float svf2(float x, float g, float R, float h, float b, float l) {
  float *s = _float(2);
  float d = 1 / (1 + 2 * R * g + g * g);
  float g1 = 2 * R + g;
  float HP = (x - g1 * s[0] - s[1] ) * d;
  float v1 = g * HP;
  float BP = v1 + s[0];
  s[0] = BP + v1;
  float v2 = g * BP;
  float LP = v2 + s[1];
  s[1] = LP + v2;
  return HP * h + BP * b + LP * l;
}

float svf3(float x, float g, float R, float h, float b, float l) {
  float *s = _float(2);
  float HP = (x - (2 * R + g) * s[0] - s[1] ) / (1 + 2 * R * g + g * g);
  float v1 = g * HP;
  float BP = v1 + s[0];
  s[0] = BP + v1;
  float v2 = g * BP;
  float LP = v2 + s[1];
  s[1] = LP + v2;
  return HP * h + BP * b + LP * l;
}

void play() {
  float f = 0.1 * uniform();
  f = svf3(f, 0.999 * phasor(0.8), 0.2 * phasor(0.1), 0.0, 0.9, 0.0);
  _out(f, f);
}
