#include <iostream>
#include <thread>

#include "Configuration.h"
#include "Help.h"
#include "Process.h"
#include "Semaphore.h"
#include "SharedMemory.h"

int main() {
  auto* code = new SharedMemory(NAME_MEMORY_CODE, SIZE_MEMORY_CODE, true);
  auto* state = new SharedMemory(NAME_MEMORY_STATE, SIZE_MEMORY_STATE, true);
  auto* audio = new SharedMemory(NAME_MEMORY_AUDIO, SIZE_MEMORY_AUDIO, true);

  // make these fail if the semaphores are created.
  //
  auto* submit = new Semaphore(NAME_SEMAPHORE_SUBMIT, true, true);
  auto* compile = new Semaphore(NAME_SEMAPHORE_COMPILE, true);
  auto* compiler_a = new Semaphore(NAME_SEMAPHORE_COMPILER_A, true);
  auto* compiler_b = new Semaphore(NAME_SEMAPHORE_COMPILER_B, true);
  auto* executer_a = new Semaphore(NAME_SEMAPHORE_EXECUTER_A, true, true);
  auto* executer_b = new Semaphore(NAME_SEMAPHORE_EXECUTER_B, true, true);
  auto* swap = new Semaphore(NAME_SEMAPHORE_SWAP, true);

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

  // simulating the audio thread
  //
  std::thread thread([&]() {
    while (true) {
      static_cast<int*>(audio->memory())[0] = AUDIO_BLOCK_SIZE;
      executer->post();
      executer->wait();
      if (swap->trywait()) {
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
        swap->post();
      }
      auto* block = static_cast<float*>(audio->memory());
      for (int i = 0; i < AUDIO_BLOCK_SIZE; i++) {
      }
    }
  });

  TRACE("CLAVM: Started CLAVM\n");

  for (int i = 0; i < 3000010; i++) {
    TRACE("CLAVM: Waiting on 'compile'\n");
    compile->wait();

    TRACE("CLAVM: Signaling 'compiler'\n");
    compiler->post();

    TRACE("CLAVM: Waiting on 'compiler'\n");
    compiler->wait();

    // XXX check if compiler succeeded!
    swap->post();
    swap->wait();

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
