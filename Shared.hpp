#pragma once

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>

#include "SharedMemoryData.hpp"

struct Shared {
  const char* clientDoneName;
  const char* serverDoneName;
  const char* sharedMemoryName;
  int shm_fd;
  sem_t* clientDone;
  sem_t* serverDone;
  SharedMemoryData* data;
  Shared(const char* clientDoneName, const char* serverDoneName,
         const char* sharedMemoryName)
      : clientDoneName(clientDoneName),
        serverDoneName(serverDoneName),
        sharedMemoryName(sharedMemoryName) {}
};

