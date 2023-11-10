#pragma once

class _SharedMemory;
class SharedMemory {
  _SharedMemory* implementation;
  public:
  SharedMemory(const char* name, size_t size = 0, bool create = false);
  ~SharedMemory();
  void* memory();
};

#ifdef WIN32
#include "SharedMemoryWindows.cpp"
#else
#include "SharedMemoryMacOS.cpp"
#endif
