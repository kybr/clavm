// {{{ C•L•A•V•M

// Built-in Functions


void _out(float, float);
float _rate();
double _time();
float* _float(int);
int* _int(int);
double* _double(int);

double exp(double);
double log2(double);
double log10(double);
double tanh(double);
double sin(double);
double cos(double);
double pow(double, double);
double M_PI = 3.1415926535;

void printf(const char*, ...);
void exit();

// Stateless Functions

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

// take a phasor/ramp on (0, 1) and divide it into integer pieces
// ...but what if it's not linear? ooooooo.
float subdiv(float p, float d) {
  p *= d;
  return p - (int)p;
}

int steps(float p, float d) {
  p *= d;
  return p;
}

int hex(double phase, int hex) {
  int index = phase * 32;
  int value = 1 & (hex >> (31 - index));
  return value;
}

int gate(double phase, const char* pattern) {
  int* length = _int(1);
  int* has_calculated_length = _int(1);
  if (*has_calculated_length != 1000000) {
    *has_calculated_length = 1000000;
    *length = 0;
    while (pattern[(int)*length] != '\0') *length++;
  }
  int index = phase * *length;
  return pattern[index] == '-' ? 1.0 : 0.0;
}

// get the fractional part of the double
//
double frac(double d) {  //
  return d - (int)d;
}

int floor(double x) {
  int xi = (int)x;
  return x < xi ? xi - 1 : xi;
}

double saw(double t) { return 2.0 * t - 1.0; }

double sin(double x) {  // expects x on (-1, 1) !!!!!
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

double tri(double phase) {
  int i = phase * 4.0;
  double f = phase * 4.0 - i;
  f = (i == 1) ? 1.0 - f : f;
  f = (i == 2) ? 1.0 - f : f;
  f = (i > 1) ? f - 1.0 : f;
  return f;
}

float imp(float phase) {
  unsigned i = phase * 4;
  float f = phase * 4 - i;
  if (i == 1 || i == 3) f = 1 - f;

  f = f * f;

  if (i > 1)  //
    f = -f;

  return f;
}

double pd(double phase, double x, double y) {
  if (phase < x) return phase * y / x;
  return y + (phase - x) * x / y;
}

/// Stateful Functions ////////////////////////////////////////////////////////

double Accum(double frequency) {
  double* phase = _double(1);
  double return_value = *phase;
  *phase += frequency / _rate();
  return return_value;
}

double Phasor(double hz) {
  double* phase = _double(1);
  *phase += hz / _rate();
  *phase -= floor(*phase);
  return *phase;
}

double Cycle(double hz) { return sin(saw(Phasor(hz))); }

double Tone(double hz) { return tone(Phasor(hz)); }

double OnePole(double x, double a) {
  double* history = _double(1);
  *history = (1.0 - a) * x + a * *history;
  return *history;
}

// http://doc.sccode.org/Classes/OneZero.html
// https://www.dsprelated.com/freebooks/filters/Elementary_Filter_Sections.html
float OneZero(float x, float b) {
  float* history = _float(1);
  if (b < 0)  // abs(b)
    b = -b;
  float f = mix(x, *history, b);
  *history = x;
  return f;
}

// Filters

double MS20(double x, double f, double q) {
  double* z = _double(2);
  double t = exp(-2 * M_PI * f / _rate());
  double s = tanh(q * z[1]);
  z[0] = mix(x - s, z[0], t);
  z[1] = mix(z[0] + s, z[1], t);
  return z[1];
}

// k: (0, -16)
// f: (0, 1)
double Diode(double x, double f, double k) {
  double* y = _double(4);

  // feedback
  x -= tanh(k * y[3]);

  // magic!
  double dy1 = f * ((x + y[1]) / 2 - y[0]);
  double dy2 = f * ((y[0] + y[2]) / 2 - y[1]);
  double dy3 = f * ((y[1] + y[3]) / 2 - y[2]);
  double dy4 = f * (y[2] - y[3]);

  // integration
  y[0] += dy1;
  y[1] += dy2;
  y[2] += dy3;
  y[3] += dy4;

  return y[3];
}

// filter coefficients
// Direct Form 1, normalized...
double Biquad(double x0, double b0, double b1, double b2, double a1,
              double a2) {
  double* z = _double(4);
  // x[n-1], x[n-2], y[n-1], y[n-2]
  double y0 = b0 * x0 + b1 * z[0] + b2 * z[1] - a1 * z[2] - a2 * z[3];
  z[3] = z[2];  // y2 = y1;
  z[2] = y0;    // y1 = y0;
  z[1] = z[0];  // x2 = x1;
  z[0] = x0;    // x1 = x0;
  return y0;
}

double Lowpass(double x, double frequency, double quality) {
  double w0 = 2 * M_PI * frequency / _rate();
  double alpha = sin(w0) / (2 * quality);
  double b0 = (1 - cos(w0)) / 2;
  double b1 = 1 - cos(w0);
  double b2 = (1 - cos(w0)) / 2;
  double a0 = 1 + alpha;
  double a1 = -2 * cos(w0);
  double a2 = 1 - alpha;
  return Biquad(x, b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
}

double Highpass(double x, double frequency, double quality) {
  double w0 = 2 * M_PI * frequency / _rate();
  double alpha = sin(w0) / (2 * quality);
  double b0 = (1 + cos(w0)) / 2;
  double b1 = -(1 + cos(w0));
  double b2 = (1 + cos(w0)) / 2;
  double a0 = 1 + alpha;
  double a1 = -2 * cos(w0);
  double a2 = 1 - alpha;
  return Biquad(x, b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
}

double Notch(double x0, double f0, double Q) {
  double omega = 2 * M_PI * f0 / 44100;
  double alpha = sin(omega) / (2 * Q);

  double a0 = 1 + alpha;
  double b0 = 1;
  double b1 = -2 * cos(omega);
  double b2 = 1;
  double a1 = -2 * cos(omega);
  double a2 = 1 - alpha;

  return Biquad(x0, b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
}

// https://ccrma.stanford.edu/~jos/filters/DC_Blocker.html
float BlockDC(float x0) {
  float* history = _float(2);
  float y0 = x0 - history[0] + history[1] * 0.9997;
  // float y0 = x0 - history[0] + history[1] * 0.995;
  history[0] = x0;
  history[1] = y0;
  return y0;
}

// Synths

// http://scp.web.elte.hu/papers/synthesis1.pdf
float Quasi(float frequency, float filter) {
  float* z = _float(2);
  float w = frequency / _rate();
  float b = 13.0 * pow(0.5 - w, 4.0) * filter;
  float phase = 2 * Phasor(frequency) - 1;
  z[0] = (z[0] + sin(M_PI * (phase + b * z[0]))) / 2;
  float o = 2.5 * z[0] - 1.5 * z[1];
  z[1] = z[0];
  return (o + (0.376 - w * 0.752)) * (1.0 - 2.0 * w);
}

double Uniform() {
  // From the FAUST noise generator...
  // https://ccrma.stanford.edu/~jos/aspf/White_Noise_Generator.html
  int* random = _int(1);
  *random += 12345;
  *random *= 1103515245;
  return *random / 2147483647.0;
}

double Kick(double t) {  // on (0, 1)
  return Notch(Highpass(1 - t, 42, 40), 200, 10);
}

double Clav(double t) {  // on (0, 1)
  return Notch(Highpass(1 - t, 1800, 40), 800, 10);
}

double Chit(double t) {  // on (0, 1)
  return Notch(Highpass(1 - t, 1800, 40), 800, 10) * Uniform();
}

// }}}

void play() {
  //double t = Accum(0.30);

  //double whole = frac(t);
  //double kick = Kick(hex(whole, 0x80208020) * subdiv(whole, 32));
  //double clav = Clav(hex(whole, 0x08080808) * subdiv(whole, 32));
  //double chit = Chit(hex(whole, 0x08080808) * subdiv(whole, 32));
  //double clav = Clav(hex(whole, 0x28282828) * subdiv(whole, 32));
  //float hz = 50; // mtof(float[]{}[steps(0)]);
  double quasi = Quasi(50, 0.1);
  float left = 0, right = 0;
  //left += 0.3 * kick;
  //right += 0.3 * clav;
  //left += 0.05 * snare;
  //right += 0.05 * snare;
   left += 0.4 * quasi;

  _out(left, right);
}


