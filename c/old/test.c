void process(double s, float* i, float* o) {
  float tempo = 60;
  float q = s * tempo / 60;
  int Q = q;
  q -= Q;
  q = 1 - q;
  q = q * q;

  float f = ((float[]){55, 0, 110, 55, 0, 55, 220, 0})[Q & 7];

  {  // LFO on frequency
    float t = s * 7;
    t -= (int)t;
    t = (t > 0.5) ? 1 - t : t;  // triangle
    t = 4 * t - 1;
    f += t * 0.0007;
  }

  // make a simple aliasing sawtooth signal
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
  {
    static float _ = 0;
    t = c * t + (1 - c) * _;
    _ = t;
  }

  // make the envelope less sharp with a one pole
  {
    static float _ = 0;
    float p = 0.992;
    q = (1 - p) * q + p * _;
    _ = q;
  }

  // output the signal
  o[0] = o[1] = 0.8 * q * t;
}
