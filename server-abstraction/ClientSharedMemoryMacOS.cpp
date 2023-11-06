#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>

#include <functional>
#include <string>

#include "Client.h"
#include "Utilities.h"

template <typename STRUCT>
class _Client {
  STRUCT* _shared{nullptr};
  int _file{0};
  sem_t* _clientDone{nullptr};
  sem_t* _serverDone{nullptr};
  bool _done{false};
  std::string _clientDoneName;
  std::string _serverDoneName;
  std::string _sharedName;

 public:
  _Client(const std::string& prefix) {
    // open semaphores
    //
    _clientDoneName = prefix + "_client";
    _serverDoneName = prefix + "_server";
    _sharedName = prefix + "_shared";

    // unlink the semaphores in case they already exist
    //
    sem_unlink(_clientDoneName.c_str());
    sem_unlink(_serverDoneName.c_str());
    // XXX check for failure

    // create semaphores
    //
    _clientDone = sem_open(_clientDoneName.c_str(), O_CREAT, 0666, 0);
    _serverDone = sem_open(_serverDoneName.c_str(), O_CREAT, 0666, 0);
    // XXX check for failure

    // open shared memory read/write
    //
    _file = shm_open(_sharedName.c_str(), O_CREAT | O_RDWR, 0666);
    if (_file == -1) {
      perror("Shared memory open");
      exit(1);
    }

    // set shared memory size
    //
    ftruncate(_file, sizeof(STRUCT));

    // map the shared memory read/write
    //
    _shared = (STRUCT*)mmap(0, sizeof(STRUCT), PROT_READ | PROT_WRITE, MAP_SHARED, _file, 0);
    if (_shared == MAP_FAILED) {
      perror("Shared memory mapping");
      exit(1);
    }

    // clear the shared memory
    //
    memset(_shared, 0, sizeof(STRUCT));
  }

  ~_Client() {
    munmap(_shared, sizeof(STRUCT));
    // XXX have to unlink/close shared memory?
    close(_file);
    sem_close(_clientDone);
    sem_close(_serverDone);
    sem_unlink(_clientDoneName.c_str());
    sem_unlink(_serverDoneName.c_str());
  }

  bool swap() {
    say("Client: Signaling write complete");
    sem_post(_clientDone);

    say("Client: Waiting for read complete");
    for (int i = 0; i < 5; i++) {
      if (sem_trywait(_serverDone) == 0) {
        return true;
      }
      usleep(1000); // wait 1ms
    }
    return false;
  }

  STRUCT* memory() { return _shared; }
};

//
//
//
//
//
template <typename STRUCT>
Client<STRUCT>::Client(const std::string& prefix)
    : _implementation(new _Client<STRUCT>(prefix)) {}
template <typename STRUCT>
Client<STRUCT>::~Client() {
  delete _implementation;
}
template <typename STRUCT>
bool Client<STRUCT>::swap() {
  return _implementation->swap();
}
template <typename STRUCT>
STRUCT* Client<STRUCT>::memory() {
  return _implementation->memory();
}
