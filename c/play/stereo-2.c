
float* _float(int);
int* _int(int);
double _time();
float _rate();
void _out(float, float);
void _zero(); // clears all state/memory ~ does not exists yet

int floor(double x) {
  int xi = (int)x;
  return x < xi ? xi - 1 : xi;
}

float saw(float t) {
  return 2 * t - 1;
}

float sin(float n){
  float nn = n * n;
  return n * (3.138982f + nn * (-5.133625f + nn * (2.428288f - nn * 0.433645f)));
}

float pd(float p, float d){
  return 0;
}

float Phasor(float hz) {
  float* phase = _float(1);
  *phase += hz / 44100;
  *phase -= floor(*phase);
  return *phase;
}

float Cycle(float hz) {
  return sin(saw(Phasor(hz)));
}

float OnePole(float x, float a) {
  float* history = _float(1);
  *history = (1 - a) * x + a * *history;
  return *history;
}

void play() {
  float g = OnePole(1 - Phasor(4), 0.99);
  float e = OnePole(1 - Phasor(2), 0.99);
  float f = 1 - OnePole(Phasor(3), 0.99);
  float l = e * Cycle(110 * Cycle(g * 110) + 110);
  float r = f * Cycle(110 * Cycle(110) + 110);

  float h = 0.1 * Cycle(1./16) * Cycle(56 + Cycle(666) * 450);
  float u = 0.1 * Cycle(1./8) * Cycle(55 + Cycle(550) * 770);
  l += h;
  r += u;
  _out(l * 0.3, 0.3 * r);
}
