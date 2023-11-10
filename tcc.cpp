#include <libtcc.h>
#include <unistd.h>

#include <iostream>
#include <thread>

#include "Configuration.h"
#include "Help.h"
#include "Semaphore.h"
#include "SharedMemory.h"

void error_handler(void* code, const char* message) {
  snprintf(static_cast<char*>(code), 100, "%s", message);
}

int main(int argc, char* argv[]) {
  if (argc != 3) return 1;
  TRACE("%s: Starting...\n", argv[1]);

  auto* audio = new SharedMemory(NAME_MEMORY_AUDIO, SIZE_MEMORY_AUDIO);
  auto* code = new SharedMemory(NAME_MEMORY_CODE, SIZE_MEMORY_CODE);
  auto* state = new SharedMemory(NAME_MEMORY_STATE, SIZE_MEMORY_STATE);
  auto* compile = new Semaphore(argv[1]);
  auto* execute = new Semaphore(argv[2]);

  // XXX set this in the compile step or the audio step?
  // relocate? find symbol? setting memory symbols?
  //
  void (*play)(void) = nullptr;

  std::thread thread([&]() {
    while (true) {
      execute->wait();
      usleep(800000);
      auto N = static_cast<int*>(audio->memory())[0];
      auto* block = static_cast<float*>(audio->memory());
      // call play N times, filling audio memory with samples
      for (int i = 0; i < N; i++) {
        block[i] = float(i);
      }
      execute->post();
    }
  });

  while (true) {
    TCCState* instance = tcc_new();
    if (instance == nullptr) {
      exit(1);
    }
    tcc_set_error_func(instance, code->memory(), error_handler);
    // tcc_set_options(instance, "-Wall -Werror");
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

  delete audio;
  delete code;
  delete state;
  delete compile;
  delete execute;

  return 0;
}
