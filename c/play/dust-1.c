int rand(void); // from the C standard library
void _out(float, float); // CLAVM: output left and right


void play() {
  float f = 0;
  if (0.0001f < rand() / 2147483647.0) {
    f = 0.2;
  }
  _out(f, f);
}

