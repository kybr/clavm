#pragma once

struct C;
class Compiler {
  C *implementation;

 public:
  Compiler();
  ~Compiler();

  bool load_dynamic(const char *path);
  bool compile(const char *code);
  const char *error();
  float *process(int N, int sample, int samplerate);
};
