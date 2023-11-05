// Server.h
#pragma once

#include <iostream>

#include "Helper.hpp"
#include "HotSwap.hpp"
#include "Shared.hpp"

class Server : public Shared {
  HotSwap hotswap;

 public:
  Server(const char* clientDoneName, const char* serverDoneName,
         const char* sharedMemory)
      : Shared(clientDoneName, serverDoneName, sharedMemory) {
    // open semaphores
    //
    clientDone = sem_open(clientDoneName, 0);
    serverDone = sem_open(serverDoneName, 0);
    // XXX check for failure

    // XXX
    // maybe trywait in a loop to eat all the signals?

    // open shared memory read/write
    //
    shm_fd = shm_open(sharedMemoryName, O_RDWR, 0666);
    if (shm_fd == -1) {
      perror("Shared memory does not exist");
      exit(1);
    }

    data =
        (SharedMemoryData*)mmap(0, sizeof(SharedMemoryData),
                                PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
      perror("Shared memory mapping");
      exit(1);
    }
  }

  ~Server() {
    munmap(data, sizeof(SharedMemoryData));
    close(shm_fd);
    sem_close(clientDone);
    sem_close(serverDone);
    // XXX check for failure
  }

  void run() {
    // XXX start function execution thread
    //

    while (true) {
      say("Server: Waiting for write to complete");
      sem_wait(clientDone);

      if (strcmp(data->message, "quit") == 0) {
        say("Server: Received quit message");
        sem_post(serverDone);
        break;
      }

      if (strcmp(data->message, "abort") == 0) {
        say("Server: Received abort message");
        fflush(stdout);
        abort();
      }

      say("Server: Received message");
      std::cout << "Server: " << data->message << std::endl;

      say("Server: Compiling source code");
      if (hotswap.compile(data->message)) {
        snprintf(data->message, SHARED_MEMORY_SIZE, "compiled");
        say("Server: Compile successful");
      } else {
        snprintf(data->message, SHARED_MEMORY_SIZE, "%s", hotswap.error());
        say("Server: Compile failed");
      }

      say("Server: Signaling read complete");
      sem_post(serverDone);
    }
  }
};
