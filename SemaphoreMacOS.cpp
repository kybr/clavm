#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#include "Help.h"
#include "Semaphore.h"

class _Semaphore {
  const char* name;
  bool unlink;
  sem_t* semaphore;

 public:
  _Semaphore(const char* name, bool create, bool state)
      : name(name), unlink(create) {
    if (unlink) {
      sem_unlink(name);
    }

    if (create) {
      semaphore = sem_open(name, O_CREAT, 0666, state ? 1 : 0);
    } else {
      semaphore = sem_open(name, 0);
    }
    if (semaphore == SEM_FAILED) {
      TRACE("%s open failed; Probably CLAVM is not running\n", name);
      exit(1);
    }
  }
  _Semaphore() {
    sem_close(semaphore);
    if (unlink) {
      sem_unlink(name);
    }
    sem_unlink(name);
  }

  bool wait(double timeout) {
    if (timeout == 0.0) {
      sem_wait(semaphore);
      return true;
    }

    for (int i = 0; i < 10; i++) {
      if (0 == sem_trywait(semaphore)) {
        return true;
      }
      usleep(static_cast<int>(timeout / 10000000));
    }
    return false;
  }

  bool trywait() {
    return 0 == sem_trywait(semaphore);
  }

  void post() { sem_post(semaphore); }
};

Semaphore::Semaphore(const char* name, bool create, bool state)
    : implementation(new _Semaphore(name, create, state)) {}

Semaphore::~Semaphore() { delete implementation; }

bool Semaphore::wait(double timeout) { return implementation->wait(timeout); }
bool Semaphore::trywait() { return implementation->trywait(); }
void Semaphore::post() { implementation->post(); }
