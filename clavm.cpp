#include <iostream>

#include "Configuration.h"
#include "Help.h"
#include "Process.h"
#include "Semaphore.h"
#include "SharedMemory.h"

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

  auto* code = new SharedMemory(NAME_MEMORY_CODE, SIZE_MEMORY_CODE, true);
  auto* state = new SharedMemory(NAME_MEMORY_STATE, SIZE_MEMORY_STATE, true);

  // make these fail if the semaphores are created.
  //
  auto* submit = new Semaphore(NAME_SEMAPHORE_SUBMIT, true, true);
  auto* compile = new Semaphore(NAME_SEMAPHORE_COMPILE, true);
  auto* compiler_a = new Semaphore(NAME_SEMAPHORE_COMPILER_A, true);
  auto* compiler_b = new Semaphore(NAME_SEMAPHORE_COMPILER_B, true);
  auto* executer_a = new Semaphore(NAME_SEMAPHORE_EXECUTER_A, true);
  auto* executer_b = new Semaphore(NAME_SEMAPHORE_EXECUTER_B, true);

  auto* compiler = compiler_a;
  auto* executer = executer_b;

  Process tcc_a;
  char const* const a[] = {"tcc", NAME_SEMAPHORE_COMPILER_A,
                           NAME_SEMAPHORE_EXECUTER_A, nullptr};
  tcc_a.spawn(a);

  Process tcc_b;
  char const* const b[] = {"tcc", NAME_SEMAPHORE_COMPILER_B,
                           NAME_SEMAPHORE_EXECUTER_B, nullptr};
  tcc_a.spawn(b);

  TRACE("CLAVM: Started CLAVM\n");

  for (int i = 0; i < 3000010; i++) {
    TRACE("CLAVM: Waiting on 'compile'\n");
    compile->wait();

    TRACE("CLAVM: Signaling 'compiler'\n");
    compiler->post();

    TRACE("CLAVM: Waiting on 'compiler'\n");
    compiler->wait();

    TRACE("CLAVM: Swapping\n");
    if (compiler == compiler_a) {
      compiler = compiler_b;
    } else if (compiler == compiler_b) {
      compiler = compiler_a;
    } else {
      TRACE("BAD\n");
      exit(1);
    }
    if (executer == executer_a) {
      executer = executer_b;
    } else if (executer == executer_b) {
      executer = executer_a;
    } else {
      TRACE("BAD\n");
      exit(1);
    }

    TRACE("CLAVM: Signaling 'compile'\n");
    compile->post();
  }

  tcc_a.kill();
  tcc_b.kill();

  delete submit;
  delete compile;
  delete compiler;
  delete executer;

  return 0;
}
