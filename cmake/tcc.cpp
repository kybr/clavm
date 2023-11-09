#include <semaphore.h>
#include <unistd.h>  // usleep

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

  TRACE("%s: Opening '%s'\n", argv[1], argv[1]);
  sem_t* tcc_ = sem_open(argv[1], 0);
  if (tcc_ == SEM_FAILED) {
    TRACE("%s: Could not open 'tcc_*'; Probably CLAVM is not running\n",
          argv[1]);
    exit(1);
  }

  while (true) {
    TRACE("%s: Waiting on '%s'\n", argv[1], argv[1]);
    sem_wait(tcc_);

    TRACE("%s: Compiling...\n", argv[1]);
    // usleep(1000000);

    TRACE("%s: Signaling '%s'\n", argv[1], argv[1]);
    sem_post(tcc_);
  }

  return 0;
}
