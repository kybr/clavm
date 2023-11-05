#include "Client.hpp"

//#include <iostream>
#include <thread>

int main() {
  const char* completeWriteName = "/completeWriteSemaphore";
  const char* completedReadName = "/completedReadSemaphore";
  const char* sharedMemoryName = "/mySharedMemory";

  Client client(completeWriteName, completedReadName, sharedMemoryName);

  // Build server invocation command; Start server
  //
  char command[256];
  snprintf(command, sizeof(command), "./server %s %s %s", completeWriteName,
           completedReadName, sharedMemoryName);
  say(command);
  bool done = false;
  std::thread server_thread([&]() {
    // restart server if it crashes
    //
    while (!done) {
      if (system(command)) {
        say("Server: Crash");
      }
      else {
        say("Server: Clean exit");
      }
    }
    usleep(1000); // wait 1ms
  });


  getchar();

  if (client.sendRequest("void play() {}")) {
    printf("Client: Received \"%s\" message\n", (const char*)client.memory());
  } else {
    say("Client: Server crashed or its too slow to be useful");
  }

  done = true;
  client.sendRequest("quit");
  server_thread.join();

  getchar();

  return 0;
}

