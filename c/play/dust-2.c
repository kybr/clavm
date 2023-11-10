void _out(float, float); // CLAVM: output left and right
int* _int(int); // get one 32-bit int of persistent memory

void play() {

  // From the FAUST noise generator...
  // https://ccrma.stanford.edu/~jos/aspf/White_Noise_Generator.html
  int *random = _int(1);
  *random += 12345;
  *random *= 1103515245;
  float r = *random / 2147483647.0f;
  // r is on (-1, 1)

  float f = 0;
  if (r < -0.999) {
    f = 0.5;
  }

  _out(f, f);
}
