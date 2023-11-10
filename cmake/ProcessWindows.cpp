#include <windows.h>

#include <iostream>

#include "Process.h"

class _Process;
class _Process {
  PROCESS_INFORMATION pi;

 public:
  bool spawn(const char* filename, char* argument[]) {
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    if (!CreateProcess(filename,  // No module name (use command line)
                       NULL,      // Command line
                       NULL,      // Process handle not inheritable
                       NULL,      // Thread handle not inheritable
                       FALSE,     // Set handle inheritance to FALSE
                       0,         // No creation flags
                       NULL,      // Use parent's environment block
                       NULL,      // Use parent's starting directory
                       &si,       // Pointer to STARTUPINFO structure
                       &pi)       // Pointer to PROCESS_INFORMATION structure
    ) {
      std::cerr << "CreateProcess failed (" << GetLastError() << ").\n";
      return false;
    }

    return true;
  }

  ~_Process() { kill(); }

  void kill() {
    if (!TerminateProcess(pi.hProcess, 1)) {
      std::cerr << "TerminateProcess failed (" << GetLastError() << ").\n";
    }
    WaitForSingleObject(pi.hProcess, INFINITE);  // XXX ?
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
  }
};

Process::Process() : implementation(new _Process()) {}
Process::~Process() { delete implementation; }

bool Process::spawn(char const * argument[]) {
  implementation->spawn(argument);
}

void Process::kill() { implementation->kill(); }
