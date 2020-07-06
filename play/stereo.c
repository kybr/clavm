
float* _float(int);
int* _int(int);
double _time();
float _rate();
void _out(float, float);
double sin(double);

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

void play() {
  _out(                                         //
      0.1 * sin(6.28318530718 * phasor(220)),   //
      0.1 * sin(6.28318530718 * phasor(221)));  //
}
