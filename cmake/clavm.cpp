#include <semaphore.h>
#include <unistd.h>
#include <iostream>

int main() {
  // create shared memory (CODE, AUDIO)
  // open semaphores
  // wait(compile)
  // post(tcc_a)
  // wait(tcc_a)
  // [read] CODE
  // maybe swap
  // [write] CODE
  // post(done)

  sem_unlink("submit");
  sem_unlink("compile");
  sem_unlink("tcc_a");
  sem_unlink("tcc_b");

  sem_t* submit = sem_open("submit", O_CREAT, 0666, 1);
  sem_t* compile = sem_open("compile", O_CREAT, 0666, 0);
  sem_t* tcc_a = sem_open("tcc_a", O_CREAT, 0666, 0);
  sem_t* tcc_b = sem_open("tcc_b", O_CREAT, 0666, 0);

  printf("Started CLAVM\n");
  for (int i = 0; i < 5; i++) {
    printf("Waiting on 'compile'\n");
    sem_wait(compile);
    printf("Compiling...\n");
    usleep(1000000);
    printf("Signaling 'compile'\n");
    sem_post(compile);
  }
  sem_post(submit);

  sem_unlink("submit");
  sem_unlink("compile");
  sem_unlink("tcc_a");
  sem_unlink("tcc_b");

  return 0;
}
