#pragma once

#include "Configuration.h"

template <typename T>
T* cast(float* in) {
  return static_cast<T*>(static_cast<void*>(in));
  //return static_cast<T*>(static_cast<void*>(in));
}

extern "C" {

float* __state_memory{nullptr};   // static?
float* __audio_memory{nullptr};  // static?
int __state_memory_index{0};
int __audio_memory_index{0};
int __index_in{0};
int __index_out{0};
int __channel_in{0};
int __channel_out{0};
int __blocksize{0};
float __samplerate{0};
float __time{0};
void (*_play)(void) = nullptr;

void clavm_initialize(void* state_memory, void* audio_memory) {
  __state_memory = static_cast<float*>(state_memory);
  __audio_memory = static_cast<float*>(audio_memory);
}

void clavm_clear_state_memory() {
  if (__state_memory) {
    for (int i = 0; i < SIZE_MEMORY_STATE; i++) {
      __state_memory[i] = 0;
    }
  }
}

void clavm_channel_count(int out, int in) {
  __channel_out = out;
  __channel_in = in;
}

void clavm_process_block() {
  if (_play == nullptr) {
    return;
  }
  // XXX this will make it aligned, right?
  auto time_offset = (__channel_in + __channel_out) * __blocksize;
  //__time = *static_cast<double*>(static_cast<void*>(&__audio_memory[time_offset]));
  __time = *cast<double>(&__audio_memory[time_offset]);

  // XXX also get blocksize and samplerate this way

  __index_in = 0;
  __index_out = 0;
  for (int i = 0; i < __blocksize; i++) {
    __state_memory_index = 0;
    _play();
    __index_in += __channel_in;
    __index_out += __channel_out;
    __time += 1.0 / __samplerate;
  }

  // XXX maybe capture the __state_memory_index because
  // that's how much state memory we are using.
}

float _samplerate() { return __samplerate; }

double _time() { return __time; }

void clavm_blocksize(int blocksize) { __blocksize = blocksize; }

int _blocksize() { return __blocksize; }

float* _float(int howmany) {
  float* pointer = static_cast<float*>(&__state_memory[__state_memory_index]);
  __state_memory_index += howmany;
  return pointer;
}

double* _double(int howmany) {
  double* pointer = cast<double>(&__state_memory[__state_memory_index]);
  __state_memory_index += howmany * 2;
  return pointer;
}

int* _int(int howmany) {
  int* pointer = cast<int>(&__state_memory[__state_memory_index]);
  __state_memory_index += howmany;
  return pointer;
}

long* _long(int howmany) {
  long* pointer = cast<long>(&__state_memory[__state_memory_index]);
  __state_memory_index += howmany * 2;
  return pointer;
}

void _out(int channel, float sample) {
  if (channel >= AUDIO_CHANNEL_OUT || channel < 0 || __index_out <= 0) {
    return;
  }
  static_cast<float*>(&__audio_memory[__index_out])[channel] = sample;
}

void _stereo(float left, float right) {
  _out(0, left);
  _out(1, right);
}

float _in(int channel) {
  if (channel >= AUDIO_CHANNEL_IN || channel < 0 || __index_in <= 0) {
    return 0.0f;
  }
  return static_cast<float*>(
      &__audio_memory[AUDIO_OFFSET_IN + __index_in])[channel];
}

// XXX add more built-in here
}

