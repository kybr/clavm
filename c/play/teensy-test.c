double* _double(int);
float _rate();
void _out(float, float);

double accum(double frequence) {
  double* phase = _double(1);
  double return_value = *phase;
  *phase += frequence / _rate();
  return return_value;
}

void play() {
  float v = accum(2);
  v = v - (int)v;
  v = v * 2 - 1;
  v *= 0.3;
  _out(v, v);
}
