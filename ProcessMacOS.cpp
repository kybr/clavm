#include <signal.h>  // kill
#include <spawn.h>   // posix_spawn
#include <unistd.h>  // STDIN_FILENO

#include <iostream>

#include "Help.h"
#include "Process.h"

class _Process;
class _Process {
  pid_t pid{0};

 public:
  bool spawn(char const* const argument[]) {
    posix_spawn_file_actions_t action;
    posix_spawn_file_actions_init(&action);
    posix_spawn_file_actions_addinherit_np(&action, STDIN_FILENO);
    posix_spawn_file_actions_addinherit_np(&action, STDOUT_FILENO);
    posix_spawn_file_actions_addinherit_np(&action, STDERR_FILENO);
    if (posix_spawn(&pid, argument[0], &action, nullptr,
                    const_cast<char* const*>(argument), nullptr) != 0) {
      perror("posix_spawn failed");
      return false;
    }
    posix_spawn_file_actions_destroy(&action);
    return true;
  }
  void kill() { ::kill(pid, SIGINT); }
};

bool Process::spawn(char const* const argument[]) {
  return implementation->spawn(argument);
}
void Process::kill() { implementation->kill(); }
