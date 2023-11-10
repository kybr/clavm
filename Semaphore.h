#pragma once

class _Semaphore;
class Semaphore {
  _Semaphore* implementation;
  public:
  Semaphore(const char* name, bool create = false, bool state = false);
  ~Semaphore();
  bool wait(double timeout = 0.0);
  void post();
};

#ifdef WIN32
#include "SemaphoreWin32.cpp"
#else
#include "SemaphoreMacOS.cpp"
#endif
