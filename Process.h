#pragma once

class _Process;
class Process {
  _Process* implementation;
  public:
  bool spawn(char const * const argument[]);
  void kill();
};

#ifdef WIN32
#include "ProcessWindows.cpp"
#else
#include "ProcessMacOS.cpp"
#endif
