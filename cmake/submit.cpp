#include <semaphore.h>
#include <unistd.h>
#include <iostream>

extern "C" const char* submit(const char* code) {
  // open code shared memory
  // open semaphores
  // wait(submit)
  // [write] CODE
  // post(compile)
  // wait(compile)
  // [read] CODE
  // post(submit)

  sem_t* submit = sem_open("submit", 0);
  if (submit == SEM_FAILED) {
    printf("Could not open 'submit'; Probably CLAVM is not running\n");
    return "FAILED\n";
  }
  sem_t* compile = sem_open("compile", 0);
  if (compile == SEM_FAILED) {
    printf("Could not open 'compile'; Probably CLAVM is not running\n");
    return "FAILED\n";
  }

  printf("Started submit\n");
  printf("Waiting for 'submit'\n");
  sem_wait(submit);
  usleep(1000000);
  printf("Signaling 'compile'\n");
  sem_post(compile);
  printf("Waiting on 'compile'\n");
  sem_wait(compile);
  printf("Signaling 'submit'\n");
  sem_post(submit);
  printf("Submit done\n");

  return "got here";
}

int main() {
  printf("Got %s\n", submit("void play() {}"));
}
