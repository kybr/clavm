void printf(const char*, ...);
void exit(int);
int* _int(int);
float* _float(int);
double* _double(int);

void play() {
  for (int i = 0; i < 1000; i++) {
    int* k = _int(1);
    float* f = _float(1);
    double* d = _double(1);
    printf("%i %f %lf\n", *k, *f, *d);
  }
  exit(1);
}
