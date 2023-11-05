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

// Quasi-bandlimited FM sawtooth emulation
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

// taylor series expansion of sine
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

// aka [scale] in Max
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

// for subdividing phasors into bunches of phasors
float subdiv(float p, float d) {
  p *= d;
  return p - (int)p;
}

// a way of creating complex rhythms out of hexidecimal numbers
// 0x8080808080808080 is "four on the floor"
// 0b10001000100010001000100010001000
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

// Korg MS20 filter emulation from "The Art of VA Filter Design"
// https://www.native-instruments.com/fileadmin/ni_media/downloads/pdf/VAFilterDesign_2.1.0.pdf
double MS20(double x, double f, double q) {
  double* z = _double(2);
  double t = exp(-2 * M_PI * f / _rate());
  double s = tanh(q * z[1]);
  z[0] = mix(x - s, z[0], t);
  z[1] = mix(z[0] + s, z[1], t);
  return z[1];
}

// Biquad Filter, like [biquad~] in Max
// https://www.w3.org/TR/audio-eq-cookbook/
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

double Uniform() {
  // From the FAUST noise generator...
  // https://ccrma.stanford.edu/~jos/aspf/White_Noise_Generator.html
  int* random = _int(1);
  *random += 12345;
  *random *= 1103515245;
  return *random / 2147483647.0;
}

// Filter-based drum things

double Kick(double t) {  // on (0, 1)
  return Notch(Highpass(1 - t, 42, 20), 279, 10);
}

double Clav(double t) {  // on (0, 1)
  return Notch(Highpass(1 - t, 1174, 20), 279, 10);
}

double Snare(double t) {  // on (0, 1)
  return Notch(Highpass(1 - t, 1174, 20), 279, 10) * Uniform();
}

// }}}

void play() {
  // output signals for left (f) and right (g) channels
  float f = 0, g = 0;

  // the main clock, counting the "time" that has passed
  float t = accum(115.0 / 60);

  // an up ramp that is a whole note (bar) long
  float whole = frac(t);

  float env = 1 - whole;

  float m = sine(saw(phasor(1))) / 2 + 0.5;
  float n = sine(saw(phasor(99))) / 2 + 0.5;
  f += 0.2 * subdiv(env, 2) * sine(saw(pd(phasor(50), 0.4 * m, 0.5)));
  g += 0.2 * subdiv(env, 4) * sine(saw(pd(phasor(50 + n * 48), 0.5, 0.3)));

  float asd = 400;

  float qu = quasi(asd, 0.9) + quasi(asd+1, 0.7); 
  qu *= env;
  float kick = Kick(whole) ;

  f += qu * 0.05;
  g += qu * 0.05;

  f += 1.1 * kick;
  g += 0.3 * Clav(subdiv(whole, 2));
  g += 0.8 * Snare(subdiv(whole, 1));

  f *= 0.5;
  g *= 0.5;

  _out(f, g);
}












// {{{ notes to self ~ a list of things to do
// - manual clicks (sound test)
// - stereo sines (literal incrementing)
// - modulate frequency (phasing)
// - phase distortion
// - tempo clock + envelope
// - Synth w/ glide
//   + array melody
//   + modulate filter
// - Kick (f with settings)
// - click hat (use hex gate)
// - duplicate synth and harmonize
// - MS20 everything
// - filter fade out
// }}}
//
