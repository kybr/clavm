#include "Help.h"

#include <semaphore.h>
#include <signal.h>
#include <spawn.h>
#include <unistd.h>

#include <iostream>
#include <thread>

void spawn(pid_t*, const char*, const char*);

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
  sem_t* tcc_compiler = sem_open("tcc_a", O_CREAT, 0666, 0);
  sem_t* tcc_executer = sem_open("tcc_b", O_CREAT, 0666, 0);

  pid_t tcc_a;
  spawn(&tcc_a, "tcc", "tcc_a");
  pid_t tcc_b;
  spawn(&tcc_b, "tcc", "tcc_b");

  TRACE("CLAVM: Started CLAVM\n");

  for (int i = 0; i < 3000010; i++) {
    TRACE("CLAVM: Waiting on 'compile'\n");
    sem_wait(compile);

    TRACE("CLAVM: Signaling 'compiler'\n");
    sem_post(tcc_compiler);

    TRACE("CLAVM: Waiting on 'compiler'\n");
    sem_wait(tcc_compiler);

    TRACE("CLAVM: Has compiled; Swapping\n");
    sem_t* tmp = tcc_compiler;
    tcc_compiler = tcc_executer;
    tcc_executer = tmp;

    TRACE("CLAVM: Signaling 'compile'\n");
    sem_post(compile);
  }

  sem_unlink("submit");
  sem_unlink("compile");
  sem_unlink("tcc_a");
  sem_unlink("tcc_b");

  kill(tcc_a, SIGINT);
  kill(tcc_b, SIGINT);

  return 0;
}

void spawn(pid_t* pid, const char* tcc, const char* semaphore) {
  char* argv[3];
  argv[0] = strdup(tcc);
  argv[1] = strdup(semaphore);
  argv[2] = nullptr;

  posix_spawn_file_actions_t action;
  posix_spawn_file_actions_init(&action);
  posix_spawn_file_actions_addinherit_np(&action, STDIN_FILENO);
  posix_spawn_file_actions_addinherit_np(&action, STDOUT_FILENO);
  posix_spawn_file_actions_addinherit_np(&action, STDERR_FILENO);
  if (posix_spawn(pid, argv[0], &action, nullptr, argv, nullptr) != 0) {
    perror("posix_spawn failed");
    exit(1);
  }
  posix_spawn_file_actions_destroy(&action);
}
