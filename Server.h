// Server.h
#pragma once

#include <iostream>
#include "Shared.hpp"
#include "Helper.hpp"
#include "HotSwap.hpp"

class Server : public Shared {
  HotSwap hotswap;
public:
  Server(const char* completedWriteName, const char* completedReadName, const char* shmName)
    : Shared(completedWriteName, completedReadName, shmName) {

    // open semaphores
    //
    completedWrite = sem_open(completedWriteName, 0);
    completedRead = sem_open(completedReadName, 0);
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

    data = (SharedMemoryData*)mmap(0, sizeof(SharedMemoryData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
      perror("Shared memory mapping");
      exit(1);
    }

  }

  ~Server() {
    munmap(data, sizeof(SharedMemoryData));
    close(shm_fd);
    sem_close(completedWrite);
    sem_close(completedRead);
    // XXX check for failure
  }

  void run() {
    while (true) {
      say("Server: Waiting for write to complete");
      sem_wait(completedWrite);

      if (strcmp(data->message, "quit") == 0) {
        say("Server: Received quit message");
        sem_post(completedRead);
        break;
      }

      if (strcmp(data->message, "abort") == 0) {
        say("Server: Received abort message");
        fflush(stdout);
        abort();
      }

      say("Server: Received message");
      std::cout << "Server: " << data->message << std::endl;

      say("Server: Signaling read complete");
      sem_post(completedRead);
    }
  }
};
