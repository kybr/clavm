// Client.h
#pragma once

#include "Shared.hpp"
#include "Helper.hpp"
#include <iostream>

class Client : public Shared {
public:
  Client(const char* completedWriteName, const char* completedReadName, const char* shmName)
    : Shared(completedWriteName, completedReadName, shmName) {

    // unlink the semaphores in case they already exist
    //
    sem_unlink(completedWriteName);
    sem_unlink(completedReadName);
    // XXX check for failure

    // create semaphores
    //
    completedWrite = sem_open(completedWriteName, O_CREAT, 0666, 0);
    completedRead = sem_open(completedReadName, O_CREAT, 0666, 0);
    // XXX check for failure

    // open shared memory read/write
    //
    shm_fd = shm_open(sharedMemoryName, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
      perror("Shared memory open");
      exit(1);
    }

    // set shared memory size
    //
    ftruncate(shm_fd, sizeof(SharedMemoryData));

    // map the shared memory read/write
    //
    data = (SharedMemoryData*)mmap(0, sizeof(SharedMemoryData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
      perror("Shared memory mapping");
      exit(1);
    }

    // clear the shared memory
    //
    memset(data, 0, sizeof(SharedMemoryData));
  }

  ~Client() {
    munmap(data, sizeof(SharedMemoryData));
    close(shm_fd);
    sem_close(completedWrite);
    sem_close(completedRead);
    sem_unlink(completedWriteName);
    sem_unlink(completedReadName);
  }

  bool sendRequest(const char* message) {
    say("Client: Writing message");
    strcpy(data->message, message);

    say("Client: Signaling write complete");
    sem_post(completedWrite);

    say("Client: Waiting for read complete");
    for (int i = 0; i < 5; i++) {
      if (sem_trywait(completedRead) == 0) {
        return true;
      }
      usleep(1000); // wait 1ms
    }
    return false;
  }
  void* memory() {
    return data->message;
  }
};
