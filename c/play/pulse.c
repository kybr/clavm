// external functions
//
float* _float(int);       // host memory access
int* _int(int);       // host memory access
float _rate(void);        // current sample rate
void _out(float, float);  // output audio
double* _double(int);       // host memory access

double uniform() {
  // from the FAUST:
  // rand  = +(12345)~*(1103515245);
  // w   = rand/2147483647.0;
  int* history = _int(1);
  *history *= 1103515245;
  *history += 12345;
  return *history / 2147483647.0;
}

double onepole(double x, double a) {
  double* history = _double(1);
  *history = (1.0 - a) * x + a * *history;
  return *history;
}

float pulse() {
  return 0;
}

void play() {
  float f = 0;
  if (uniform() > 0.99997) f = 1;
  _out(f, f);
}
