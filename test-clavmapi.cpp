#include "CLAVMAPI.h"
#include "Help.h"
#include "libtcc.h"

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
  //tcc_add_symbol(instance, "_f", reinterpret_cast<void*>(_double));
  //tcc_add_symbol(instance, "_i", reinterpret_cast<void*>(_long));
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

  void* state = calloc(10000, 4);
  void* audio = calloc(10000, 4);
  clavm_initialize(state, audio);
  clavm_update(1.0, 48000, 16, 2, 2, 2, 2);

  // configure memory on the input side
  //
  for (int i = 0; i < 16; i++) {
    clavm_audio_in()[i * 2] = (float)i / 16;
    clavm_audio_in()[i * 2 + 1] = 1.0 - (float)i / 16;
  }
  clavm_parameter_in()[0] = 0.5;
  clavm_parameter_in()[1] = 2.0;

  // show memory on the input side
  //
  for (int i = 0; i < 16; i++) {
    printf("%.2f ", clavm_audio_in()[i * 2]);
  }
  printf("\n");
  for (int i = 0; i < 16; i++) {
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
  for (int i = 0; i < 16; i++) {
    printf("%.2f ", clavm_audio_in()[i * 2]);
  }
  printf("\n");
  for (int i = 0; i < 16; i++) {
    printf("%.2f ", clavm_audio_in()[1 + i * 2]);
  }
  printf("\n");

  for (int i = 0; i < 2; i++) {
    printf("%.2f\n", clavm_parameter_in()[i]);
  }

  // show memory on the output side
  //
  for (int i = 0; i < 16; i++) {
    printf("%.2f ", clavm_audio_out()[i * 2]);
  }
  printf("\n");
  for (int i = 0; i < 16; i++) {
    printf("%.2f ", clavm_audio_out()[1 + i * 2]);
  }
  printf("\n");
  for (int i = 0; i < 2; i++) {
    printf("%.2f\n", clavm_parameter_out()[i]);
  }
}

