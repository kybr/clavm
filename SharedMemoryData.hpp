#pragma once

#define SHARED_MEMORY_SIZE (65000)
struct SharedMemoryData {
  char message[SHARED_MEMORY_SIZE];
};
