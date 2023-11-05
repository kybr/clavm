#include <iostream>
#include "Server.hpp"
float* __cc{nullptr};
int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <complete_write_semaphore_name> <completed_read_semaphore_name> <shared_memory_name>" << std::endl;
    return 1;
  }

  const char* completeWriteName = argv[1];
  const char* completedReadName = argv[2];
  const char* sharedMemoryName = argv[3];

  Server server(completeWriteName, completedReadName, sharedMemoryName);
  server.run();

  return 0;
}

