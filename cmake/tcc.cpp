#include <semaphore.h>
#include <unistd.h> // usleep
#include <iostream>
int main(int argc, char* argv[]) {
  if (argc != 2) return 1;
  // create shared memory (CODE)
  // open semaphores
  // wait(tcc_a)
  // [read] CODE
  // compile
  // [write] CODE
  // post(tcc_a)

  sem_t* semaphore = sem_open(argv[1], 0);
  if (semaphore == SEM_FAILED) {
    printf("Could not open 'tcc_*'; Probably CLAVM is not running\n");
    exit(1);
  }

  for (int i = 0; i < 5; i++) {
    printf("Waiting on '%s'\n", argv[1]);
    sem_wait(semaphore);

    printf("Compiling...\n");
    usleep(1000000);

    printf("Signaling '%s'\n", argv[1]);
    sem_post(semaphore);
  }

  return 0;
}
