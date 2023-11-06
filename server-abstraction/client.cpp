#include "Client.h"

#include <thread>
#include <signal.h>

#include "ClientSharedMemoryMacOS.cpp"
#include "Server.h"
#include "ServerSharedMemoryMacOS.cpp"
#include "Utilities.h"

struct Memory {
  char data[100];
};

int main(int argc, char* argv[]) {
  Client<Memory> client(argv[1]);

  pid_t pid;
  if ((pid = fork()) == 0) {
    // we are the child process

    Server<Memory> server(argv[1]);

    server.run([&](Memory* memory) {
      printf("Server: Got \"%s\" from client\n", memory->data);

      if (strcmp(memory->data, "quit") == 0) {
        server.stop();
        return;
      }

      say("Server: Sending \"goodbye\" to client");
      snprintf(memory->data, 10, "goodbye");
    });
    return 0;
  }

  Memory* memory = client.memory();
  snprintf(memory->data, 10, "hello");
  say("Client: Sending \"hello\" to server");
  if (!client.swap()) {
    say("Client: Server too slow or crashed");
    kill(pid, SIGINT);
    return 1;
  }
  printf("Client: Got back \"%s\" from server\n", memory->data);

  snprintf(memory->data, 10, "quit");
  say("Client: Sending quit message");
  if (!client.swap()) {
    say("Client: Server too slow or crashed");
    kill(pid, SIGINT);
    return 1;
  }
}
