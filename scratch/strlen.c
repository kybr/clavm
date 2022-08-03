#include <stdio.h>
#include <string.h>

int size(const char* pattern) {
  int size = 0;
  while (pattern[size] != '\0') size++;
  return size;
}

int main() {
  printf("size:%d\n", size("12345"));
}
