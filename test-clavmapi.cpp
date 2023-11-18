#include "CLAVMAPI.h"
#include "Help.h"
#include "libtcc.h"

// https://evanw.github.io/float-toy
void showbyte(void* memory, size_t size) {
  const char* m = static_cast<char*>(memory);
  int i = 0;
  while (true) {
    for (int k = 0; k < 16; k++) {
      printf("%x%x ", 15 & (m[i] >> 4), 15 & m[i]);
      i++;
      if (i >= size) {
        printf("\n");
        return;
      }
    }
    printf("\n");
  }
}


const char* source = R"(
float _in(int);
void _lr(float, float);
float _p(int);
void _d(int, float);
float* _float(int);
double _time();

void play() {
  _d(0, _p(1));
  _d(1, _p(0));
  _time();
  float l = _in(0);
  float r = _in(1);
  _lr(r, l);
}
)";

void error_handler(void* code, const char* message) {
  TRACE("ERROR: %s\n", message);
}

int main() {
  TCCState* instance = tcc_new();
  if (instance == nullptr) {
    TRACE("no instance");
    exit(1);
  }

  tcc_set_error_func(instance, nullptr, error_handler);
  tcc_set_options(instance, "-Wall -Werror");
  // tcc_set_options(instance, "-Wall -Werror -nostdinc -nostdlib");

  tcc_set_output_type(instance, TCC_OUTPUT_MEMORY);
  if (0 != tcc_compile_string(instance, source)) {
    exit(1);
  }

  tcc_add_symbol(instance, "_samplerate", reinterpret_cast<void*>(_samplerate));
  tcc_add_symbol(instance, "_time", reinterpret_cast<void*>(_time));
  // tcc_add_symbol(instance, "_f", reinterpret_cast<void*>(_double));
  // tcc_add_symbol(instance, "_i", reinterpret_cast<void*>(_long));
  tcc_add_symbol(instance, "_in", reinterpret_cast<void*>(_in));
  tcc_add_symbol(instance, "_out", reinterpret_cast<void*>(_out));
  tcc_add_symbol(instance, "_lr", reinterpret_cast<void*>(_lr));
  tcc_add_symbol(instance, "_p", reinterpret_cast<void*>(_p));
  tcc_add_symbol(instance, "_d", reinterpret_cast<void*>(_d));

  if (-1 == tcc_relocate(instance, TCC_RELOCATE_AUTO)) {
    TRACE("relocate failed");
    exit(1);
  }
  auto* function =
      reinterpret_cast<void (*)(void)>(tcc_get_symbol(instance, "play"));
  if (function == nullptr) {
    TRACE("no play found\n");
    exit(1);
  }

  const int framecount = 10;
  void* state = calloc(10000, 4);
  void* audio = calloc(10000, 4);
  clavm_initialize(state, audio);
  clavm_update(1.0, 20.0, framecount, 2, 2, 2, 2);

  // configure memory on the input side
  //
  for (int i = 0; i < framecount; i++) {
    clavm_audio_in()[i * 2] = (float)i / framecount;
    clavm_audio_in()[i * 2 + 1] = 1.0 - (float)i / framecount;
  }
  clavm_parameter_in()[0] = 0.5;
  clavm_parameter_in()[1] = 2.0;

  // show memory on the input side
  //
  for (int i = 0; i < framecount; i++) {
    printf("%.2f ", clavm_audio_in()[i * 2]);
  }
  printf("\n");
  for (int i = 0; i < framecount; i++) {
    printf("%.2f ", clavm_audio_in()[1 + i * 2]);
  }
  printf("\n");

  for (int i = 0; i < 2; i++) {
    printf("%.2f\n", clavm_parameter_in()[i]);
  }

  // execute user code
  //
  clavm_process_block(function);

  // show memory on the input side
  //
  for (int i = 0; i < framecount; i++) {
    printf("%.2f ", clavm_audio_in()[i * 2]);
  }
  printf("\n");
  for (int i = 0; i < framecount; i++) {
    printf("%.2f ", clavm_audio_in()[1 + i * 2]);
  }
  printf("\n");

  for (int i = 0; i < 2; i++) {
    printf("%.2f\n", clavm_parameter_in()[i]);
  }

  // show memory on the output side
  //
  for (int i = 0; i < framecount; i++) {
    printf("%.2f ", clavm_audio_out()[i * 2]);
  }
  printf("\n");
  for (int i = 0; i < framecount; i++) {
    printf("%.2f ", clavm_audio_out()[1 + i * 2]);
  }
  printf("\n");
  for (int i = 0; i < 2; i++) {
    printf("%.2f\n", clavm_parameter_out()[i]);
  }


  printf("time: %lf\n", _time());
  printf("samplerate: %f\n", _samplerate());

  showbyte(audio, 50 * 4);
  //showbyte(audio, 4 * (framecount * (2 + 2 + 2 + 2) + 2 + 2) + 8 + 4 + 4 + 4 * 4);

  auto zero = audio;
  printf("pointer zero: %zx\n", (size_t)zero);
  printf("pointer out: %zx\n", (size_t)clavm_audio_out());
  printf("pointer in: %zx\n", (size_t)clavm_audio_in());
  printf("pointer parameter out: %zx\n", (size_t)clavm_parameter_out());
  printf("pointer parameter in: %zx\n", (size_t)clavm_parameter_in());
  printf("offset out: %zu\n", ((size_t)clavm_audio_out() - (size_t)zero) / 4);
  printf("offset in: %zu\n", ((size_t)clavm_audio_in() - (size_t)zero) / 4);
  printf("offset parameter out: %zu\n",( (size_t)clavm_parameter_out() - (size_t)zero) / 4);
  printf("offset parameter in: %zu\n", ((size_t)clavm_parameter_in() - (size_t)zero) / 4);
}

