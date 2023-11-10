// C•L•A•V•M
//
// stereo beats ~ play two pure tones slightly out of tune
//

// {{{ prelude
// this prelude contains function declarations and definitions
// that we will use later. you can hide it with your code 
// editor so you can focus on your sounds

//
// functions built into CLAVM
//
double* _double(int); // grabs one double of persistent memory
float* _float(int); // grabs one double of persistent memory
void _out(float, float); // outputs a sample, both left and right
float _rate(); // reminds us of the current sample rate

//
// functions defined here in the prelude
//

int floor(double x) {
  // rounds down to the nearest integer
  int xi = (int)x;
  return x < xi ? xi - 1 : xi;
}

double phasor(double hz) {
  // ramps up from 0 to 1 repeatedly at the given frequency
  // a "stateful" function; it remembers the previous phase
  double* phase = _double(1);
  *phase += hz / _rate();
  *phase -= floor(*phase);
  return *phase;
}

double sine(double x) {
  // Taylor series expansion of sin; expects x on (0, 1)
  x = 2 * x - 1;
  double xx = x * x;
  return x * (3.138982 + xx * (-5.133625 + xx * (2.428288 - xx * 0.433645f)));
}

// }}} end of prelude


// this is a crude approximation of tanh we might use for distortion
// or a "soft" squisher. it would not do to use it for anything that
// wants accuracy.
float tanhish(float v) {
  if (v > 1) return 1;
  if (v < -1) return -1;
  v = v / 2 - 1;
  v = 1 - v * v;
  return v;
}

float softclip(float x) {
  if (x <= -1) return -1;
  if (x >= 1) return 1;
  return (3 * x - x * x * x) / 2;
}

float ms20(float x, float f, float q) {
  float* z = _float(2);
  float t = exp(-2 * M_PI * f / SAMPLE_RATE);
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


void play() {
  // the play function is called thousands of times per second. each time we
  // provide two numbers each between -1 and 1 that are essentially command
  // for our speakers to move to forward or move back from their resting
  // position.

  // loudness         frequency
  //    |                 |
  _out(0.1 * (sine(phasor(220))),  // left output sample
       0.1 * (sine(phasor(221)))); // right output sample
  //        /           \
  //  sine function   phasor function

  // we hear "beats" at 1 Hz; read about this phenomenon here:
  // ...
}
