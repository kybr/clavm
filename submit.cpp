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
//
//
//
//
//
//
//
//
//
//
//

const char* test(const char* code) {
  // open code shared memory
  // open semaphores
  // wait(submit)
  // [write] CODE
  // post(compile)
  // wait(compile)
  // [read] CODE
  // post(submit)

  auto* submit = new Semaphore("submit");
  auto* compile = new Semaphore("compile");

  using std::chrono::duration;
  using std::chrono::high_resolution_clock;
  using std::chrono::time_point;
  time_point<high_resolution_clock> then = high_resolution_clock::now();

  for (int i = 0; i < 1000000; i++) {
    TRACE("Started submit\n");
    TRACE("Waiting for 'submit'\n");
    submit->wait();
    TRACE("Signaling 'compile'\n");
    compile->post();
    TRACE("Waiting on 'compile'\n");
    compile->wait();
    TRACE("Signaling 'submit'\n");
    submit->post();
    TRACE("Submit done\n");
  }

  duration<double> time = high_resolution_clock::now() - then;

  printf("time: %lf mean: %lf\n", time.count(), time.count() / 1000000.0);
  fflush(stdout);

  std::vector<double> list;
  for (int i = 0; i < 1000000; i++) {
    using std::chrono::duration;
    using std::chrono::high_resolution_clock;
    using std::chrono::time_point;
    time_point<high_resolution_clock> then = high_resolution_clock::now();

    TRACE("Started submit\n");
    TRACE("Waiting for 'submit'\n");
    submit->wait();
    TRACE("Signaling 'compile'\n");
    compile->post();
    TRACE("Waiting on 'compile'\n");
    compile->wait();
    TRACE("Signaling 'submit'\n");
    submit->post();
    TRACE("Submit done\n");

    duration<double> time = high_resolution_clock::now() - then;

    list.push_back(time.count());
  }

  double sum = 0;
  for (int i = 0; i < list.size(); i++) {
    sum += list[i];
  }
  double mean = sum / list.size();

  sum = 0;
  for (int i = 0; i < list.size(); i++) {
    double v = list[i] - mean;
    sum += v * v;
  }
  double stddev = sqrt(sum / list.size());
  printf("mean: %lf stddev: %lf\n", mean, stddev);

  return "got here";
}

int main() { printf("Got %s\n", test("void play() {}")); }

