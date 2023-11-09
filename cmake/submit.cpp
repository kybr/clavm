#include "Help.h"

#include <semaphore.h>
#include <unistd.h>

#include <iostream>
#include <chrono>
#include <cmath>

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
    TRACE("Could not open 'submit'; Probably CLAVM is not running\n");
    return "FAILED\n";
  }
  sem_t* compile = sem_open("compile", 0);
  if (compile == SEM_FAILED) {
    TRACE("Could not open 'compile'; Probably CLAVM is not running\n");
    return "FAILED\n";
  }
  using std::chrono::time_point;
  using std::chrono::duration;
  using std::chrono::high_resolution_clock;
  time_point<high_resolution_clock> then = high_resolution_clock::now();

  for (int i = 0; i < 1000000; i++) {

  TRACE("Started submit\n");
  TRACE("Waiting for 'submit'\n");
  sem_wait(submit);
  TRACE("Signaling 'compile'\n");
  sem_post(compile);
  TRACE("Waiting on 'compile'\n");
  sem_wait(compile);
  TRACE("Signaling 'submit'\n");
  sem_post(submit);
  TRACE("Submit done\n");

  }

  duration<double> time = high_resolution_clock::now() - then;

  printf("time: %lf mean: %lf\n", time.count(), time.count() / 1000000.0);
  fflush(stdout);

  std::vector<double> list;
  for (int i = 0; i < 1000000; i++) {
  using std::chrono::time_point;
  using std::chrono::duration;
  using std::chrono::high_resolution_clock;
  time_point<high_resolution_clock> then = high_resolution_clock::now();

  TRACE("Started submit\n");
  TRACE("Waiting for 'submit'\n");
  sem_wait(submit);
  TRACE("Signaling 'compile'\n");
  sem_post(compile);
  TRACE("Waiting on 'compile'\n");
  sem_wait(compile);
  TRACE("Signaling 'submit'\n");
  sem_post(submit);
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

int main() { printf("Got %s\n", submit("void play() {}")); }
