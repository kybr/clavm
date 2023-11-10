#ifndef __SYNTH__
#define __SYNTH__

float* _float(unsigned advance);
float powf(float, float)
float log2f(float);
float log10f(float);
double exp(double);
double tanh(double);
double sin(double);
double cos(double);

///////////////////////////////////////////////////////////////////////////////
// stateless
///////////////////////////////////////////////////////////////////////////////

float mtof(float m) {
  return 8.175799f * powf(2.0f, m / 12.0f);
  // return 440.0f * powf(2.0f, (m - 69.0f) / 12.0f);
}

float ftom(float f) {
  return 12.0f * log2f(f / 8.175799f);
  // return 12.0f * log2f(f / 440.0f) - 69.0f;
}

float dbtoa(float db) {
  return powf(10.0f, db / 20.0f);
  //
}

float atodb(float a) {
  return 20.0f * log10f(a / 1.0f);
  //
}

// aka lerp
float mix(float a, float b, float t) {  //
  return (1 - t) * a + t * b;
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

///////////////////////////////////////////////////////////////////////////////
// accepts phase (0, 1)
///////////////////////////////////////////////////////////////////////////////

float saw(float phase) {  //
  return 2 * phase - 1;
}

// a sine-like waveform without any trig
//
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

float tri(float phase) {
  unsigned i = phase * 4;
  float f = phase * 4 - i;
  // flip
  if (i == 1 || i == 2)  //
    f = 1 - f;
  // lower
  if (i > 1)  //
    f = f - 1;
  return f;
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

// phase distortions
//
float pd(float phase, float x, float y) {
  if (phase < x) return phase * y / x;
  return y + (phase - x) * x / y;
  // return y + (phase - x) * (1 - y) / (1 - x);
}

float pda(float phase, float a) {
  a = 0.5 + a / 2;  // accept normalized value
  float slope = 1 / a - 1;
  if (phase < a) return phase * slope;
  return (1 - a) + (phase - a) / slope;
  // if (phase < a) return phase * (1 - a) / a;
  // return (1 - a) + (phase - a) * a / (1 - a);
}

float pdb(float phase, float a) {  //
  return pd(phase, a, 1 - a);
}

// take a phasor/ramp on (0, 1) and divide it into integer pieces
// ...but what if it's not linear? ooooooo.
float subdiv(float p, float d) {
  p *= d;
  return p - (int)p;
}

// get the fractional part of the double
//
float frac(double d) {  //
  return d - (int)d;
}

///////////////////////////////////////////////////////////////////////////////
// stateful
///////////////////////////////////////////////////////////////////////////////

float phasor(float hertz) {
  float* phase = _float(1);
  *phase += hertz / _rate();
  *phase -= floor(*phase);
  // if (*phase > 1)  //
  //  *phase -= 1;
  return *phase;
}

int edge(float value) {
  float* history = _float(1);
  int condition = value < *history;
  *history = value;
  return condition;
}

float onepole(float x, float a) {
  float* history = _float(1);
  *history = (1 - a) * x + a * *history;
  return *history;
}

// http://doc.sccode.org/Classes/OneZero.html
// https://www.dsprelated.com/freebooks/filters/Elementary_Filter_Sections.html
float onezero(float x, float b) {
  float* history = _float(1);
  if (b < 0)  // abs(b)
    b = -b;
  float f = mix(x, *history, b);
  *history = x;
  return f;
}

float ms20(float x, float f, float q) {
  float* z = _float(2);
  float t = exp(-2 * M_PI * f / _rate());
  float s = tanh(q * z[1]);
  z[0] = mix(x - s, z[0], t);
  z[1] = mix(z[0] + s, z[1], t);
  return z[1];
}

// k: (0, -16)
// f: (0, 1)
float diode(float x, float f, float k) {
  float* y = _float(4);

  // feedback
  x -= tanh(k * y[3]);

  // magic!
  float dy1 = f * ((x + y[1]) / 2 - y[0]);
  float dy2 = f * ((y[0] + y[2]) / 2 - y[1]);
  float dy3 = f * ((y[1] + y[3]) / 2 - y[2]);
  float dy4 = f * (y[2] - y[3]);

  // integration
  y[0] += dy1;
  y[1] += dy2;
  y[2] += dy3;
  y[3] += dy4;

  return y[3];
}

// http://scp.web.elte.hu/papers/synthesis1.pdf
float quasi(float frequency, float filter) {
  float* z = _float(2);
  float w = frequency / _rate();
  float b = 13 * pow(0.5 - w, 4.0) * filter;
  float phase = 2 * phasor(frequency) - 1;
  z[0] = (z[0] + sin(M_PI * (phase + b * z[0]))) / 2;
  float o = 2.5 * z[0] - 1.5 * z[1];
  z[1] = z[0];
  return (o + (0.376 - w * 0.752)) * (1.0 - 2.0 * w);
}

// filter coefficients
// Direct Form 1, normalized...
float biquad(float x0, float b0, float b1, float b2, float a1, float a2) {
  float* z = _float(4);
  // x[n-1], x[n-2], y[n-1], y[n-2]
  float y0 = b0 * x0 + b1 * z[0] + b2 * z[1] - a1 * z[2] - a2 * z[3];
  z[3] = z[2];  // y2 = y1;
  z[2] = y0;    // y1 = y0;
  z[1] = z[0];  // x2 = x1;
  z[0] = x0;    // x1 = x0;
  return y0;
}

float lpf(float x, float frequency, float quality) {
  float w0 = 2 * M_PI * frequency / _rate();
  float alpha = sin(w0) / (2 * quality);
  float b0 = (1 - cos(w0)) / 2;
  float b1 = 1 - cos(w0);
  float b2 = (1 - cos(w0)) / 2;
  float a0 = 1 + alpha;
  float a1 = -2 * cos(w0);
  float a2 = 1 - alpha;
  return biquad(x, b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
}

float notch(float x0, float f0, float Q) {
  float omega = 2 * M_PI * f0 / 44100;
  float alpha = sin(omega) / (2 * Q);

  float a0 = 1 + alpha;
  float b0 = 1;
  float b1 = -2 * cos(omega);
  float b2 = 1;
  float a1 = -2 * cos(omega);
  float a2 = 1 - alpha;

  return biquad(x0, b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
}

//  multitap delay
void multitap(float x, float maximumTime, float* tap, int size) {
  int maximum = 1 + (int)(44100 * maximumTime);
  int* n = host_int(1);
  float* buffer = _float(maximum);

  // circular buffer
  buffer[*n] = x;
  *n = 1 + *n;
  if (*n >= maximum)  //
    *n = 0;

  for (int i = 0; i < size; i++) {
    float t = *n - tap[i] * 44100;
    if (t < 0)  //
      t += maximum;
    int a = t;
    t -= a;
    int b = 1 + a;
    if (b >= maximum)  //
      b = 0;
    tap[i] = (1 - t) * buffer[a] + t * buffer[b];
  }
}

// https://ccrma.stanford.edu/~jos/filters/DC_Blocker.html
float dcblock(float x0) {
  float* history = _float(2);
  float y0 = x0 - history[0] + history[1] * 0.9997;
  // float y0 = x0 - history[0] + history[1] * 0.995;
  history[0] = x0;
  history[1] = y0;
  return y0;
}

#endif
