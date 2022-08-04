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


void play() {
  // the play function is called thousands of times per second. each time we
  // provide two numbers each between -1 and 1 that are essentially command
  // for our speakers to move to forward or move back from their resting
  // position.

  // loudness         frequency
  //    |                 |
  _out(0.1 * sine(phasor(220)),  // left output sample
       0.1 * sine(phasor(221))); // right output sample
  //        /           \
  //  sine function   phasor function

  // we hear "beats" at 1 Hz; read about this phenomenon here:
  // ...
}
