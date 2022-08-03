void printf(const char*, ...);
void exit(int);
float* _float(int);

int gate(double phase, const char* pattern) {
  float* size = _float(1);
  printf("size:%f\n", *size);

  if (*size < 0.5) {
    printf("GOT HERE\n");
    while (pattern[(int)*size] != '\0') *size = *size + 1;
  }
  printf("size:%f\n", *size);
  int index = phase * *size;

  printf("phase:%lf index:%d\n", phase, index);
  return pattern[index] == '-' ? 1.0 : 0.0;
}

void play() {
  for (double f = 0.0; f < 1.0; f += 0.1) {
    printf("value:%lf\n", gate(f, "..."));
  }
  exit(1);
}
