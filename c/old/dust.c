// tick randomly
//

// Per-Sample C always links the math library with the equivalent of
// -lm and macOS seems to pull in the whole c runtime including rand
// from stdlib when we do this. so, we did 'man rand' to check the
// signature and put it here.
int rand();

void process(double t, float* input, float* o) {
  o[0] = o[1] = 0;
  if (rand() < 1000000)  //
    o[0] = o[1] = 1;
}
