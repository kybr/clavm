#include <iostream>

#include "Help.h"
#include "Process.h"
#include "Semaphore.h"

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

  Process tcc_a;
  char const* const a[3] = {"tcc", "tcc_a", nullptr};
  tcc_a.spawn(a);

  Process tcc_b;
  char const* const b[3] = {"tcc", "tcc_b", nullptr};
  tcc_a.spawn(b);

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

  tcc_a.kill();
  tcc_b.kill();

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
