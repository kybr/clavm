void process(double d, float* input, float* o) {
  int D = d;
  d -= D;
  // XXX take contol of your volume knob; turn down!
  o[0] = o[1] = d;
}
