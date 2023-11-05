#pragma once

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>

#include "SharedMemoryData.hpp"

struct Shared {
  const char* completedWriteName;
  const char* completedReadName;
  const char* sharedMemoryName;
  int shm_fd;
  sem_t* completedWrite;
  sem_t* completedRead;
  SharedMemoryData* data;
  Shared(const char* completedWriteName, const char* completedReadName,
         const char* sharedMemoryName)
      : completedWriteName(completedWriteName),
        completedReadName(completedReadName),
        sharedMemoryName(sharedMemoryName) {}
};

