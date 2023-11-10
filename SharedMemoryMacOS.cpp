#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <iostream>

class _SharedMemory;
class _SharedMemory {
  void* _shared;
  size_t _size;
  const char* _name;
  bool _create;
  int _file;

 public:
  _SharedMemory(const char* name, size_t size, bool create)
      : _shared(nullptr), _size(size), _name(name), _create(create) {
    // XXX possibly unlink here

    if (_create) {
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
    if (_create) {
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
    if (_create) {
      unlink(_name);
    }
    munmap(_shared, _size);
    close(_file);
  }

  void* memory() { return _shared; }
};

SharedMemory::SharedMemory(const char* name, size_t size, bool create)
    : implementation(new _SharedMemory(name, size, create)) {}
SharedMemory::~SharedMemory() { delete implementation; }
void* SharedMemory::memory() { return implementation->memory(); }
