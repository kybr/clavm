float* _float(int); // grab one float of persistent state
void _out(float, float); // output sample (left, right)
float _rate(); // what is the sample rate?
double sin(double); // trigonometry!!

// round down
int floor(double x) {
  int xi = (int)x;
  return x < xi ? xi - 1 : xi;
}

// a ramp up from 0 to 1 repeatedly at the given frequency
// a "stateful" function; it remembers the previous value
float phasor(float hz) {
  float* phase = _float(1);
  *phase += hz / _rate();
  *phase -= floor(*phase);
  return *phase;
}

double pi = 6.28318530718;
void play() {
  // play two sine waves, slightly out of tune
  _out(0.1 * sin(pi * phasor(220)),
       0.1 * sin(pi * phasor(221)));
}
