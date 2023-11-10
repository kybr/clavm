// tick out seconds
//

int lastT = 0;
void process(double t, float* input, float* o) {
  int T = t;
  o[0] = o[1] = 0;
  if (T != lastT)  //
    o[0] = o[1] = 1;
  lastT = T;
}
