#include "Help.h"
#include "Semaphore.h"

#include <windows.h>
#include <iostream>

class _Semaphore {
  HANDLE semaphore;

 public:
  _Semaphore(const char* name, bool create, bool unlink, bool state)
      : name(name), unlink(unlink) {
    if (unlink) {
      sem_unlink(name);
    }

    // XXX
    // - how to unlink a windows semaphore?
    // - how do we fail if the semaphore does not exists?
    // - how do we set the initial state?

    // XXX
    // - can we use a simple const char* or do we need a L"" ?
    semaphore = OpenSemaphore(
        SEMAPHORE_ALL_ACCESS,    // Request full access
        FALSE,                   // Handle not inheritable
        name);          // Name of semaphore

    if (semaphore == NULL) {
      TRACE("%s open failed; Probably CLAVM is not running\n", name);
      exit(1);
    }
  }

  _Semaphore() {
    // XXX
    // - check result
    CloseHandle(semaphore);
  }

  bool wait(double timeout) {
    // XXX
    // - check result
    // - use timeout
    WaitForSingleObject(semaphore, INFINITE);
    return true;
  }

  void post() {
    // XXX
    // - what do the arguments mean?
    // - check result
    ReleaseSemaphore(semaphore, 1, NULL);
  }
};

Semaphore::Semaphore(const char* name, bool create, bool unlink, bool state)
    : implementation(new _Semaphore(name, create, unlink, state)) {}

Semaphore::~Semaphore() { delete implementation; }

bool Semaphore::wait(double timeout) { return implementation->wait(timeout); }
void Semaphore::post() { implementation->post(); }
