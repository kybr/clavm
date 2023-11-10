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
    }
    else {
      snprintf(static_cast<char*>(code->memory()), 100, "compiled");
    }

    TRACE("%s: Signaling '%s'\n", argv[1], argv[1]);
    tcc->post();
  }

  delete tcc;
  delete code;

  return 0;
}
