#include "Server.h"

#include <unistd.h>

#include <functional>
#include <iostream>
#include <thread>

#include "ServerSharedMemoryMacOS.cpp"

struct Memory {
  char data[100];
};

int main(int argc, char* argv[]) {
  Server<Memory> server(argv[1]);

  server.run([&](Memory* memory) {
    printf("Server: Got \"%s\" from client\n", memory->data);

    if (strcmp(memory->data, "quit") == 0) {
      printf("Server: got quit message\n");
      server.stop();
      return;
    }

    say("Server: Sending \"goodbye\" to client");
    snprintf(memory->data, 10, "goodbye");
  });
}
