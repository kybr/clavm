#include "HotSwap.hpp"
#include <unistd.h>
#include <iostream>
HotSwap hotswap;
int main() {
  while (true) {
    printf("compile...\n");
    if (!hotswap.compile("void play(){}")) {
      printf("FAIL\n");
      return 2;
    }
    float* block = hotswap.process(1000, 0, 44100);
    sleep(1);
  }
}
