#pragma once

#include <iostream>

void say(const char* message) { printf("%s\n", message); }

#define DEBUG

#ifdef DEBUG
#define TRACE(fmt, ...) \
  printf(fmt, ##__VA_ARGS__)
#else
#define TRACE(fmt, ...)
#endif

#define CODE_SIZE (65536)

