#pragma once

#define ITEM_COUNT (1000000)

static float __float[ITEM_COUNT];
static int __int[ITEM_COUNT];
static char __char[ITEM_COUNT];

static unsigned __float_index;
static unsigned __int_index;
static unsigned __char_index;

void host_reset() {
  __float_index = 0;
  __int_index = 0;
  __char_index = 0;
}

float *host_float(unsigned advance) {
  float *value = &__float[__float_index];
  __float_index += advance;
  return value;
}

int *host_int(unsigned advance) {
  int *value = &__int[__int_index];
  __int_index += advance;
  return value;
}

char *host_char(unsigned advance) {
  char *value = &__char[__char_index];
  __char_index += advance;
  return value;
}
