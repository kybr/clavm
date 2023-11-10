// external functions
//
float* _float(int);       // host memory access
float _rate(void);        // current sample rate
void _out(float, float);  // output audio
float _cc(int);            // MIDI CC messages
double sin(double);       // math.h
double pow(double, double);

float phasor(float hz) {
  float* phase = _float(1);
  *phase += ((hz < 0) ? -hz : hz) / _rate();
  *phase -= (int)(*phase);
  return *phase;
}

float onepole(float x, float a) {
  float* history = _float(1);
  *history = (1 - a) * x + a * *history;
  return *history;
}

float mtof(float m) {  //
  return 440.0 * pow(2.0, (m - 69.0) / 12.0);
}

// void __init() {}

float oneminus(float v) {  //
  return 1 - v;
}

void abort();
int exit();

void play() {
  // *((float*)0) = 1;
  // 0 / 0;
  // abort();
  // exit(42);
  float hz = onepole(mtof(_cc(0) + _cc(1) * phasor(1.8)), 0.999);
  float phase = phasor(hz);
  float out = sin(phase * 6.283185307179586);
  _out(0.02 * out, 0.03 * out);
}
