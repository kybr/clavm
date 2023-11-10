float* _float(int);
float _rate();
float _out(float, float);

float saw(float t) {
  return 2 * t - 1;
}

float accum(float hz) {
  float* value = _float(1);
  *value += hz / _rate();
  return *value;
}
float reset() {
  float* value = _float(1);
  *value = 0;
  return *value;
}

float impulse_once() {
  float* value = _float(1);
  if (*value < 0.5) {
    *value = 1;
    return 1;
  }
  return 0;
}

void init() {
  // run once.. so as to initialize memory/state
}

void play() {
  // State g; // g.accum
  // float t = _link(); // https://github.com/Ableton/link
  // ^ future work
  //
  // float t = accum(1);
  // float f = 0;
  // if ((int)t % 2 == 0) {
  //   f = accum(1);
  // }
  // else {
  //   f = accum(2);
  // }

  // float p = phasor(&state->freq);
  float t = accum(1);
  float f = 0;
  if ((int)t % 2 == 0) {
    f = accum(2);
  }
  else {
    reset(); // interferes with the accum above; would a user expect this?
  }
  _out(f, f);
}

