#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

class _SharedMemory;
class _SharedMemory {
  void* _shared;
  size_t _size;
  const char* _name;
  int _file;

 public:
  _SharedMemory(const char* name, size_t size, bool create) : _shared(nullptr), _size(size), _name(name) {
    // XXX possibly unlink here

    if (create) {
      _file = shm_open(name, O_CREAT | O_RDWR, 0666);
    } else {
      _file = shm_open(name, O_RDWR, 0666);
    }

    if (_file == -1) {
      perror("Opening shared memory");
      exit(1);
    }

    // XXX just on create?
    //
    if (create) {
      // set size and fill with zeros
      ftruncate(_file, _size);
    }

    _shared = mmap(0, _size, PROT_READ | PROT_WRITE, MAP_SHARED, _file, 0);
    if (_shared == MAP_FAILED) {
      perror("Shared memory mapping");
      exit(1);
    }
  }

  ~_SharedMemory() {
    unlink(_name);
    munmap(_shared, _size);
    close(_file);
  }

  void* memory() { return _shared; }
};

SharedMemory::SharedMemory(const char* name, size_t size, bool create)
    : implementation(new _SharedMemory(name, size, create)) {}
SharedMemory::~SharedMemory() { delete implementation; }
void* SharedMemory::memory() { return implementation->memory(); }
