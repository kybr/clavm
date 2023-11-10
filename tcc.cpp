#include <libtcc.h>

#include <iostream>

#include "Help.h"
#include "Semaphore.h"
#include "SharedMemory.h"
#include "Configuration.h"

void error_handler(void* code, const char* message) {
  snprintf(static_cast<char*>(code), 100, "%s", message);
}

int main(int argc, char* argv[]) {
  if (argc != 3) return 1;
  // create shared memory (CODE)
  // open semaphores
  // wait(tcc_a)
  // [read] CODE
  // compile
  // [write] CODE
  // post(tcc_a)

  TRACE("%s: Starting...\n", argv[1]);

  auto* code = new SharedMemory(NAME_MEMORY_CODE, SIZE_MEMORY_CODE);
  auto* state = new SharedMemory(NAME_MEMORY_STATE, SIZE_MEMORY_STATE);

  auto* compile = new Semaphore(argv[1]);
  auto* execute = new Semaphore(argv[2]);

  while (true) {
    TCCState* instance = tcc_new();
    if (instance == nullptr) {
      exit(1);
    }
    tcc_set_error_func(instance, code->memory(), error_handler);
    //tcc_set_options(instance, "-Wall -Werror");
    tcc_set_options(instance, "-Wall -Werror -nostdinc -nostdlib");
    tcc_set_output_type(instance, TCC_OUTPUT_MEMORY);

    TRACE("%s: Waiting on '%s'\n", argv[1], argv[1]);
    compile->wait();

    TRACE("%s: Compiling...\n", argv[1]);

    StopWatch clock;
    clock.tic();
    if (0 != tcc_compile_string(instance, static_cast<char*>(code->memory()))) {
      // error is handled with a callback
      compile->post();
      continue;
    }
    tcc_delete(instance);
    printf("Compiler took %lf seconds\n", clock.toc());

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
        compile->post();
        continue;
      }

      TRACE("%s: Finding 'play' function...\n", argv[1]);
      auto* function =
          reinterpret_cast<void (*)(void)>(tcc_get_symbol(instance, "play"));
      if (function == nullptr) {
        snprintf(static_cast<char*>(code->memory()), 100, "no 'play' function");
        compile->post();
        continue;
      }

      TRACE("%s: Calling 'play' function...\n", argv[1]);
      function();
    }

    snprintf(static_cast<char*>(code->memory()), 100, "success");

    TRACE("%s: Signaling\n", argv[1]);
    compile->post();
  }

  delete compile;
  delete code;

  return 0;
}
