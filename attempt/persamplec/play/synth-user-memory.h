#ifndef __SYNTH__
#define __SYNTH__

#include "math.h"

#define N (200000)
//#define N (100000)
//#define N (1000000)

static float _memory[N];
static float _table[N];
static unsigned _index = 0;

// call this at the start of each process to service the system
void synth_process() {  //
  _index = 0;
}

void synth_begin() {
  for (int i = 0; i < N; i++) {
    _table[i] = sin(2 * M_PI * i / N);
  }
}

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

// float tri(float phase) {
//   unsigned i = phase * 4;
//   float f = phase * 4 - i;
//   switch (i) {
//     case 0:
//       return f;  // noop
//     case 1:
//       return 1 - f;  // flip
//     case 2:
//       return -f;  // negate
//     case 3:
//       return f - 1;  // lower
//   }
// }

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

float sine(float phase) {
  //
  return _table[(int)(phase * (N - 1.0))];
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
#define PHASE (_memory[_index])
  float value = PHASE;
  PHASE += hertz / SAMPLE_RATE;
  if (PHASE > 1)  //
    PHASE -= 1;
  _index++;
  return value;
#undef PHASE
}

int edge(float value) {
  int condition = value < _memory[_index];
  _memory[_index] = value;
  _index++;
  return condition;
}

float onepole(float x, float a) {
#define HISTORY (_memory[_index])
  HISTORY = (1 - a) * x + a * HISTORY;
  float value = HISTORY;
  _index++;
  return value;
#undef HISTORY
}

// float onepole(float x0, float a1) {
//  float y1 = _memory[_index];
//  y1 = (1 - a1) * x0 + a1 * y1;
//  _memory[_index] = y1;
//  _index++;
//  return y1;
//}

// http://doc.sccode.org/Classes/OneZero.html
// https://www.dsprelated.com/freebooks/filters/Elementary_Filter_Sections.html
float onezero(float x, float b) {
  float* history = &_memory[_index];
  _index += 1;
  if (b < 0)  // abs(b)
    b = -b;
  float f = mix(x, history[0], b);
  history[0] = x;
  return f;
}

float ms20(float x, float f, float q) {
  float* z = &_memory[_index];
  _index += 2;
  float t = exp(-2 * M_PI * f / SAMPLE_RATE);
  float s = tanh(q * z[1]);
  z[0] = mix(x - s, z[0], t);
  z[1] = mix(z[0] + s, z[1], t);
  return z[1];
}

float quasi(float frequency, float filter) {
  float* z = &_memory[_index];
  _index += 2;
  float w = frequency / SAMPLE_RATE;
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
  float* z = &_memory[_index];
  _index += 4;
  // x[n-1], x[n-2], y[n-1], y[n-2]
  float y0 = b0 * x0 + b1 * z[0] + b2 * z[1] - a1 * z[2] - a2 * z[3];
  z[3] = z[2];  // y2 = y1;
  z[2] = y0;    // y1 = y0;
  z[1] = z[0];  // x2 = x1;
  z[0] = x0;    // x1 = x0;
  return y0;
}

float lpf(float x, float frequency, float quality) {
  float w0 = 2 * M_PI * frequency / SAMPLE_RATE;
  float alpha = sin(w0) / (2 * quality);
  float b0 = (1 - cos(w0)) / 2;
  float b1 = 1 - cos(w0);
  float b2 = (1 - cos(w0)) / 2;
  float a0 = 1 + alpha;
  float a1 = -2 * cos(w0);
  float a2 = 1 - alpha;
  b0 /= a0;
  b1 /= a0;
  b2 /= a0;
  a1 /= a0;
  a2 /= a0;
  return biquad(x, b0, b1, b2, a1, a2);
}

#endif
