void process(double s, float* i, float* o) {
  float tempo = 150;
  float q = s * tempo / 60;
  int Q = q;
  q -= Q;
  q = 1 - q;
  q = q * q;

  float f = ((float[]){55, 0, 110, 55, 0, 55, 220, 0})[Q & 7];

  // make a simple aliasing sawtooth signal
  // at 110 Hertz
  float t = s * f;
  t -= (int)t;
  t = 2 * t - 1;

  // make an envelope for filter control,
  // an up-ramp over one whole note
  float c = s * tempo / 60 / 8;
  c -= (int)c;
  c = 0.2 * c + 0.01;

  // apply a one pole filter using the
  // control envelope above
  static float z = 0;
  t = c * t + (1 - c) * z;
  z = t;

  float h = ((float[]){55 * 5 / 4.0, 55 * 3 / 2, 110, 220})[(Q >> 3) & 3];

  {
    static float _ = 0;
    static float p = 0.9999;
    h = (1 - p) * h + p * _;
    _ = h;
  }

  float m = s * h;
  m -= (int)m;
  // for (int i = 0; i < (Q & 7); i++)  //
  m = m * m;
  m = (m > 0.5) ? 1 - m : m;  // triangle
  m = 2 * m - 1;

  static float x = 0;
  m = 0.1 * m + 0.9 * x;
  x = m;

  // output the signal
  o[0] = o[1] = 0.1 * q * t + 0.5 * m;
}
