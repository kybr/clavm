// external functions
//
float* _float(int);       // host memory access
float _rate(void);        // current sample rate
void _out(float, float);  // output audio


void play() {
  float f = 0;
  _out(f, f);
}
