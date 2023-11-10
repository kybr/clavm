#include <libtcc.h>

#include <iostream>

#include "Help.h"
#include "Semaphore.h"
#include "SharedMemory.h"

void error_handler(void* code, const char* message) {
  snprintf(static_cast<char*>(code), 100, "%s", message);
}

int main(int argc, char* argv[]) {
  if (argc != 2) return 1;
  // create shared memory (CODE)
  // open semaphores
  // wait(tcc_a)
  // [read] CODE
  // compile
  // [write] CODE
  // post(tcc_a)

  TRACE("%s: Starting...\n", argv[1]);

  auto* code = new SharedMemory("/code", CODE_SIZE);
  auto* tcc = new Semaphore(argv[1]);

  while (true) {
    TCCState* instance = tcc_new();
    if (instance == nullptr) {
      exit(1);
    }
    tcc_set_error_func(instance, code->memory(), error_handler);
    tcc_set_options(instance, "-Wall -Werror");
    // tcc_set_options(instance, "-Wall -Werror -nostdinc -nostdlib");
    tcc_set_output_type(instance, TCC_OUTPUT_MEMORY);

    TRACE("%s: Waiting on '%s'\n", argv[1], argv[1]);
    tcc->wait();

    TRACE("%s: Compiling...\n", argv[1]);
    if (0 != tcc_compile_string(instance, static_cast<char*>(code->memory()))) {
      // error is handled with a callback
      tcc->post();
      continue;
    }

    // XXX
    //
    // do we test the code right after we compile it?
    //
    // what we get:
    //   most crashy code does not get to the audio thread
    //   so the audio is less likely to go silent because
    //   of a bit of bad code. we can also time the execution
    //   to get an idea of the current cost of the code
    // what we lose:
    //   latency. running the code takes time.
    //
    // we will make it an option
    //
    if (0) {
      // XXX run the code once
      TRACE("%s: Relocating...\n", argv[1]);
      if (-1 == tcc_relocate(instance, TCC_RELOCATE_AUTO)) {
        snprintf(static_cast<char*>(code->memory()), 100, "relocate failed");
        tcc->post();
        continue;
      }

      TRACE("%s: Finding 'play' function...\n", argv[1]);
      auto* function =
          reinterpret_cast<void (*)(void)>(tcc_get_symbol(instance, "play"));
      if (function == nullptr) {
        snprintf(static_cast<char*>(code->memory()), 100, "no 'play' function");
        tcc->post();
        continue;
      }

      TRACE("%s: Calling 'play' function...\n", argv[1]);
      function();
    }

    snprintf(static_cast<char*>(code->memory()), 100, "success");

    TRACE("%s: Signaling\n", argv[1]);
    tcc->post();
  }

  delete tcc;
  delete code;

  return 0;
}
