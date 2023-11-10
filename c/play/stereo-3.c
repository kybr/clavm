// Karl Yerkes
//
// 2022-04-05
//
// Thinking through some design.
//
// Question: Would using doubles be better? Check the ASM to see.
//

/// C•L•A•V•M Functions ///////////////////////////////////////////////////////

float* _float(int); // get the next float from memory
int* _int(int); // get the next int from memory
double _time(); // the time since the server started
float _rate(); // get the sample rate of the audio device
void _out(float, float); // output a signal to the audio device

float _knob(int); // XXXDNE get the value of a particular knob
float _show(int, float); // XXXDNE present a signal visually
float _link(int); // XXXDNE get the global transport time
void _zero(); // XXXDNE clears all state/memory

void setup() {
  _param(4, 0, 0, 4, "foo");
}

void block() { }

/// Stateless Functions ///////////////////////////////////////////////////////

int floor(double x) {
  int xi = (int)x;
  return x < xi ? xi - 1 : xi;
}

float saw(float t) {
  return 2 * t - 1;
}

float sin(float x) { // expects x on (-1, 1) !!!!!
  float xx = x * x;
  return x * (3.138982f + xx * (-5.133625f + xx * (2.428288f - xx * 0.433645f)));
}

// piecewise parabola mimicking sine
float tone(float phase) {
  int i = phase * 4;
  float f = phase * 4 - i;
  f = (i % 2 == 0) ? 1 - f : f; // flip, maybe
  f = f * f;
  f = (i < 2) ? 1 - f : f - 1; // flip or lower
  return f;
}

float tri(float phase) {
  unsigned i = phase * 4;
  float f = phase * 4 - i;
  f = (i == 1) ? 1 - f : f;
  f = (i == 2) ? 1 - f : f;
  f = (i > 1) ? f - 1 : f;
  return f;
}

float pd(float phase, float x, float y) {
  if (phase < x) return phase * y / x;
  return y + (phase - x) * x / y;
}

/// Stateful Functions ////////////////////////////////////////////////////////

float Phasor(float hz) {
  float* phase = _float(1);
  *phase += hz / 44100;
  *phase -= floor(*phase);
  return *phase;
}

float Cycle(float hz) {
  return sin(saw(Phasor(hz)));
}

float Tone(float hz) {
  return tone(Phasor(hz));
}

float OnePole(float x, float a) {
  float* history = _float(1);
  *history = (1 - a) * x + a * *history;
  return *history;
}

/// Stateful Functions ////////////////////////////////////////////////////////

void play() {
  //float g = OnePole(1 - Phasor(4), 0.99);
  //float e = OnePole(1 - Phasor(2), 0.99);
  //float f = 1 - OnePole(Phasor(3), 0.99);
  //float l = e * Cycle(110 * Cycle(g * 110) + 110);
  //float r = f * Cycle(110 * Cycle(110) + 110);

  float l = 0, r = 0;

  float g = OnePole(1 - Phasor(1), 0.99);
  
  { // pd synth left
    float p = tone(pd(Phasor(111), g, 0.5));
    l += p * g * 0.6;
  }
  { // pd synth right
    float p = tone(pd(Phasor(110), 0.5, g));
    r += p * g * 0.6;
  }

  float h = 0.3 * Cycle(1./16) * Cycle(56 + Cycle(666) * 440);
  float u = 0.3 * Cycle(1./8) * Cycle(55 + Cycle(550) * 770);
  // l += h;
  // r += u;

  //_out(l * 0.3, 0.3 * r);
}
