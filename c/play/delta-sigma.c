// external functions{{{
//
double _time();
double* _double(int);
float _rate(void);        // current sample rate
float* _float(int);       // host memory access
int* _int(int);       // host memory access
void _out(float, float);  // output audio

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

// taylor series expansion of sine
double sine(double x) {  // expects x on (-1, 1) !!!!!
  double xx = x * x;
  return x * (3.138982 + xx * (-5.133625 + xx * (2.428288 - xx * 0.433645f)));
}

float delta(float signal) {
  float* accumulation = _float(1);
  float* bit = _float(1);
  *accumulation += signal - *bit;
  *bit = (*accumulation >= 0) ? 1 : -1;
  return *bit;
}

// second order
float delta2(float signal) {
  float* accum1 = _float(1);
  float* accum2 = _float(1);
  float* bit = _float(1);
  *accum1 += signal - *bit;
  *accum2 += *accum1 - *bit;
  *bit = (*accum2 >= 0) ? 1 : -1;
  return *bit;
}

float saw(float in) { return 2 * in - 1; }

void play() {
  float f = sine(saw(phasor(106))); // 0.1 * uniform();
  f = delta2(f); 
  f = delta(f); 
  f = delta2(f); 
  f = delta(f); 
  _out(f, f);
}
