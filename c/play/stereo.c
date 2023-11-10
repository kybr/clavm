float* _float(int); // grab one float of persistent state
                    //
void _out(float, float); // output sample (left, right)
float _rate(); // what is the sample rate?

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

float saw(float p) {
  return 2 * p - 1;
}

float onepole(float x, float a) {
  float* history = _float(1);
  *history = (1.0 - a) * x + a * *history;
  return *history;
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

// taylor series expansion of sine
double sine(double x) {  // expects x on (-1, 1) !!!!!
  double xx = x * x;
  return x * (3.138982 + xx * (-5.133625 + xx * (2.428288 - xx * 0.433645f)));
}

float tri(float p) { return 2 * p - 1; }

double pi = 6.28318530718;
void play() {
  //return;
  float p1 = phasor(220);
  float left = 0.5 * sine(tri(phasor(220)));
  float right = 0.5 * sine(tri(phasor(221)));

  float main = phasor(0.1);

  float env2 = hex(main, 0xFeedDadE) * (1 - subdiv(main, 32));
  left = onepole(env2, 0.99) * left;
  float env = hex(main, 0xDeadBeef) * (1 - subdiv(main, 32));
  right = onepole(env, 0.99) * right;

  left *= 0.5;
  right *= 0.5;

  _out(left, right);
}
