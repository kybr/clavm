#include <unistd.h>  // usleep
#include "SharedMemory.h"
#include "Semaphore.h"

#include <iostream>

#include "Help.h"
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
    TRACE("%s: Waiting on '%s'\n", argv[1], argv[1]);
    tcc->wait();

    TRACE("%s: Compiling...\n", argv[1]);
    // usleep(1000000);

    TRACE("%s: Signaling '%s'\n", argv[1], argv[1]);
    tcc->post();
  }

  delete tcc;
  delete code;

  return 0;
}
