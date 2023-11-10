#include <chrono>
#include <cmath>
#include <iostream>

#include "Help.h"
#include "Semaphore.h"
#include "SharedMemory.h"

extern "C" void submit(char* reply, const char* source) {
  // open code shared memory
  // open semaphores
  // wait(submit)
  // [write] CODE
  // post(compile)
  // wait(compile)
  // [read] CODE
  // post(submit)

  auto* code = new SharedMemory("/code", CODE_SIZE);

  auto* submit = new Semaphore("submit");
  auto* compile = new Semaphore("compile");

  using std::chrono::duration;
  using std::chrono::high_resolution_clock;
  using std::chrono::time_point;
  time_point<high_resolution_clock> then = high_resolution_clock::now();

  //TRACE("source: %s\n", source);

  submit->wait();
  snprintf(static_cast<char*>(code->memory()), CODE_SIZE, "%s", source);
  compile->post();

  compile->wait();
  snprintf(reply, 100, "%s", static_cast<char*>(code->memory()));
  submit->post();
  //TRACE("reply: %s\n", reply);

  duration<double> time = high_resolution_clock::now() - then;

  delete submit;
  delete compile;
  delete code;
}

//
// There was benchmarking code here once. It tested the semaphores
// only from submit to clavm to tcc and back. It showed something
// like this:
//
// 1000000 round trip. 8s ellapsed. mean: 0.000009s
// 1000000 individual. mean: 0.000009s stddev: 0.000002
//

