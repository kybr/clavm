#include <dlfcn.h>
#include <libtcc.h>

#include <string>

#include "Compiler.hpp"
#include "globals.h"

typedef void (*TCCErrorFunc)(void *opaque, const char *msg);
typedef TCCState *(*type_tcc_new)(void);
typedef void (*type_tcc_delete)(TCCState *);
typedef void (*type_tcc_set_error_func)(TCCState *, void *, TCCErrorFunc);
typedef void (*type_tcc_set_options)(TCCState *, const char *);
typedef int (*type_tcc_set_output_type)(TCCState *, int);
typedef void (*type_tcc_define_symbol)(TCCState *, const char *, const char *);
typedef int (*type_tcc_compile_string)(TCCState *, const char *);
typedef int (*type_tcc_add_symbol)(TCCState *, const char *, const void *);
typedef int (*type_tcc_add_library)(TCCState *, const char *);
typedef int (*type_tcc_relocate)(TCCState *, void *);
typedef void *(*type_tcc_get_symbol)(TCCState *, const char *);

typedef void (*PlayFunc)(void);

struct C {
  type_tcc_new _new{tcc_new};
  type_tcc_delete _delete{tcc_delete};
  type_tcc_set_error_func _set_error_func{tcc_set_error_func};
  type_tcc_set_options _set_options{tcc_set_options};
  type_tcc_set_output_type _set_output_type{tcc_set_output_type};
  type_tcc_define_symbol _define_symbol{tcc_define_symbol};
  type_tcc_compile_string _compile_string{tcc_compile_string};
  type_tcc_add_symbol _add_symbol{tcc_add_symbol};
  type_tcc_add_library _add_library{tcc_add_library};
  type_tcc_relocate _relocate{tcc_relocate};
  type_tcc_get_symbol _get_symbol{tcc_get_symbol};

  TCCState *instance{nullptr};

  PlayFunc function{nullptr};
  std::string error;
  size_t size{0};
  void *where{nullptr};

  float *process(int N, int sample, int samplerate);
  bool compile(const std::string &code);
  bool load_dynamic(const char *path);
};

//
//
//

#define MEM_SIZE (10000)

static float _float_memory[MEM_SIZE];
static int _float_memory_index = 0;

extern "C" float *_float(int many) {
  float *pointer = &_float_memory[_float_memory_index];
  _float_memory_index += many;
  return pointer;
}

static int _int_memory[MEM_SIZE];
static int _int_memory_index = 0;

extern "C" int *_int(int many) {
  int *pointer = &_int_memory[_int_memory_index];
  _int_memory_index += many;
  return pointer;
}

static double _double_memory[MEM_SIZE];
static int _double_memory_index = 0;

extern "C" double *_double(int many) {
  double *pointer = &_double_memory[_double_memory_index];
  _double_memory_index += many;
  return pointer;
}

static float _output_memory[256 * 4096];
static int _output_memory_index = 0;

extern "C" void _out(float left, float right) {
  _output_memory[_output_memory_index] = left;
  _output_memory[_output_memory_index + 1] = right;
}

void zero_memory() {
  for (int i = 0; i < MEM_SIZE; i++) {
    _float_memory[i] = 0.0f;
    _int_memory[i] = 0;
    _double_memory[i] = 0.0;
  }
  // memset(_float_memory, 0, MEM_SIZE);
  // memset(_int_memory, 0, MEM_SIZE);
  // memset(_double_memory, 0, MEM_SIZE);
}

extern "C" float _rate(void) { return 44100; }


extern "C" float _cc(int i) { return __cc[i]; }

static double _t = 0;
extern "C" double _time(void) { return _t; }

// this is okay if only one TCC calls these functions at a time.
//
float *C::process(int N, int sample, int samplerate) {
  _output_memory_index = 0;
  double sampletime = 1.0 / samplerate;
  _t = sample * sampletime;
  for (int n = 0; n < N; n++) {
    //
    // preparation
    //
    _float_memory_index = 0;
    _int_memory_index = 0;
    _double_memory_index = 0;
    _out(0.0, 0.0);  // clear the current sample (left, right)

    // call the 'play' function
    //
    function();

    //
    //
    _output_memory_index += 2;
    _t += sampletime;
  }

  /*
  // replace bad stuff w/ zero
  for (int n = 0; n < 2 * N; n++) {
    switch (fpclassify(_output_memory[n])) {
      default:
        break;
      case FP_INFINITE:
      case FP_NAN:
      case FP_SUBNORMAL:
        _output_memory[n] = 0.0f;
        // XXX should count and notify
    }
  }
  */
  return _output_memory;
}

//
//
//

void error_handler(void *opaque, const char *msg) {
  static_cast<std::string *>(opaque)->append(msg);
}

bool C::compile(const std::string &code) {
  // make a new compiler instance, perhaps deleting the old one
  //
  if (instance)  //
    _delete(instance);
  instance = _new();
  if (instance == nullptr) {
    error = "Null Instance";
    return false;
  }

  // zero_memory(); // NO

  _set_error_func(instance, (void *)&error, error_handler);

  // treat warnings as errors
  _set_options(instance, "-Wall -Werror");
  //_set_options(instance, "-Wall -Werror -nostdinc -nostdlib");

  _set_output_type(instance, TCC_OUTPUT_MEMORY);

  // Define Pre-Processor Symbols
  //
  //  defining floats crashes
  //  char buffer[10];
  //  sprintf(buffer, "%f", samplerate);
  //  _define_symbol(instance, "samplerate", buffer);
  //

  // Do the compile step
  //
  if (0 != _compile_string(instance, code.c_str())) {
    // error is handled with a callback
    return false;
  }

  // add symbols; these must be extern "C"
  // (but are theses strictly necessary?)
  _add_symbol(instance, "_rate", (void *)_rate);
  _add_symbol(instance, "_float", (void *)_float);
  _add_symbol(instance, "_double", (void *)_double);
  _add_symbol(instance, "_int", (void *)_int);
  _add_symbol(instance, "_cc", (void *)_cc);
  _add_symbol(instance, "_out", (void *)_out);
  _add_symbol(instance, "_time", (void *)_time);

  // tcc_add_symbol(instance, "log", (void*)logf);
  // tcc_add_symbol(instance, "log", (void*)logf);

  // the answer was NO on macOS
  // if (!_add_library(instance, "System")) {
  //   printf("did not load 'System'\n");
  // }
  // if (!_add_library(instance, "m")) {
  //   printf("did not load 'm'\n");
  // }

  // XXX broken on linux
  // size = _relocate(instance, nullptr);
  // printf("%d bytes\n", size);
  // if (where != nullptr)  //
  //   free(where);
  // where = malloc(size);
  // if (-1 == _relocate(instance, where)) {

  if (-1 == _relocate(instance, TCC_RELOCATE_AUTO)) {
    error = "Relocate Failed";
    printf("Relocate Failed\n");
    fflush(stdout);
    exit(1);
    return false;
  }

  function = (PlayFunc)_get_symbol(instance, "play");
  if (function == nullptr) {
    error = "No 'play' Symbol";
    return false;
  }

  using InitFunc = void (*)(void);
  InitFunc init = (InitFunc)_get_symbol(instance, "init");
  if (init) init();  // called immediately after a compile

  return true;
}

bool C::load_dynamic(const char *path) {
  void *handle = dlopen(path, RTLD_NOW);

  if (handle == nullptr) {
    printf("DLOPEN ERROR:%s\n", dlerror());
    return false;
  }

  _new = (type_tcc_new)dlsym(handle, "tcc_new");
  _delete = (type_tcc_delete)dlsym(handle, "tcc_delete");
  _set_error_func =
      (type_tcc_set_error_func)dlsym(handle, "tcc_set_error_func");
  _set_options = (type_tcc_set_options)dlsym(handle, "tcc_set_options");
  _set_output_type =
      (type_tcc_set_output_type)dlsym(handle, "tcc_set_output_type");
  _define_symbol = (type_tcc_define_symbol)dlsym(handle, "tcc_define_symbol");
  _compile_string =
      (type_tcc_compile_string)dlsym(handle, "tcc_compile_string");
  _add_symbol = (type_tcc_add_symbol)dlsym(handle, "tcc_add_symbol");
  _add_library = (type_tcc_add_library)dlsym(handle, "tcc_add_library");
  _relocate = (type_tcc_relocate)dlsym(handle, "tcc_relocate");
  _get_symbol = (type_tcc_get_symbol)dlsym(handle, "tcc_get_symbol");
  return true;
}

//
//
//

Compiler::Compiler() : implementation(new C) {}
Compiler::~Compiler() { delete implementation; }

bool Compiler::compile(const char *code) {
  return implementation->compile(code);
}

const char *Compiler::error() {  //
  return implementation->error.c_str();
}

bool Compiler::load_dynamic(const char *path) {
  return implementation->load_dynamic(path);
}

float *Compiler::process(int N, int sample, int samplerate) {
  return implementation->process(N, sample, samplerate);
}
