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

  // make these fail if the semaphores are created.
  //
  auto* submit = new Semaphore("submit", true, true);
  auto* compile = new Semaphore("compile", true);
  auto* compiler = new Semaphore("tcc_a", true);
  auto* executer = new Semaphore("tcc_b", true);

  Process tcc_a;
  char const* const a[] = {"tcc", "tcc_a", nullptr};
  tcc_a.spawn(a);

  Process tcc_b;
  char const* const b[] = {"tcc", "tcc_b", nullptr};
  tcc_a.spawn(b);

  TRACE("CLAVM: Started CLAVM\n");

  for (int i = 0; i < 3000010; i++) {
    TRACE("CLAVM: Waiting on 'compile'\n");
    compile->wait();

    TRACE("CLAVM: Signaling 'compiler'\n");
    compiler->post();

    TRACE("CLAVM: Waiting on 'compiler'\n");
    compiler->wait();

    TRACE("CLAVM: Has compiled; Swapping\n");
    auto* tmp = compiler;
    compiler = executer;
    executer = tmp;

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
