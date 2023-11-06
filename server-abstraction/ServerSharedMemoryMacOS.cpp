#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>

#include <functional>

#include "Server.h"
#include "Utilities.h"

template <typename STRUCT>
class _Server {
  STRUCT* _shared{nullptr};
  int _file{0};
  sem_t* _clientDone{nullptr};
  sem_t* _serverDone{nullptr};
  bool _done{false};

 public:
  _Server(const std::string& prefix) {
    // open semaphores
    //
    if ((_clientDone = sem_open((prefix + "_client").c_str(), O_CREAT, 0666, 0)) ==
        SEM_FAILED) {
      perror("client semaphore failed");
      exit(1);
    }
    if ((_serverDone = sem_open((prefix + "_server").c_str(), O_CREAT, 0666, 0)) ==
        SEM_FAILED) {
      perror("client semaphore failed");
      exit(1);
    }

    // XXX
    // maybe trywait in a loop to eat all the signals?

    // open shared memory read/write
    //
    if ((_file = shm_open((prefix + "_shared").c_str(), O_RDWR, 0666)) == -1) {
      perror("Shared memory does not exist");
      exit(1);
    }

    // map the file to memory
    //
    _shared = (STRUCT*)mmap(0, sizeof(STRUCT), PROT_READ | PROT_WRITE,
                            MAP_SHARED, _file, 0);
    if (_shared == MAP_FAILED) {
      perror("Shared memory mapping");
      exit(1);
    }
  }

  ~_Server() {
    munmap(_shared, sizeof(STRUCT));
    close(_file);
    sem_close(_clientDone);
    sem_close(_serverDone);
    // XXX check for failure
  }

  void run(std::function<void(STRUCT*)> handler) {
    while (!_done) {
      say("Server: Waiting for client");
      sem_wait(_clientDone);

      say("Server: Calling handler");
      handler(_shared);

      say("Server: Signaling done");
      sem_post(_serverDone);
    }
  }

  void stop() { _done = true; }
};

//
//
//
//
//
template <typename STRUCT>
Server<STRUCT>::Server(const std::string& prefix)
    : _implementation(new _Server<STRUCT>(prefix)) {}
template <typename STRUCT>
Server<STRUCT>::~Server() {
  delete _implementation;
}
template <typename STRUCT>
void Server<STRUCT>::run(std::function<void(STRUCT*)> handler) {
  _implementation->run(handler);
}
template <typename STRUCT>
void Server<STRUCT>::stop() {
  _implementation->stop();
}
