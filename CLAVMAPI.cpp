//
// C•L•A•V•M © Karl Yerkes 2019-2023
//

#include "CLAVMAPI.h"

#include "Help.h"

extern "C" {

// Memory/Data layout
//
// The first slots of the audio memory holds this class:
//
struct Audio {
  double time;
  float samplerate;
  int framecount;
  int channel_count_out;
  int channel_count_in;
  int parameter_count_out;
  int parameter_count_in;
};
//
// Beyond that information are several contiguous arrays of floats.
// First comes the audio output data, followed by the audio input.
// Then the set of output parameters, followed by the inputs. The
// information in the structure above tells us how many outputs,
// intputs, and parameters to expect.
//

// XXX do these need to be static?
void* __state_memory{nullptr};
void* __audio_memory{nullptr};
Audio* __audio{nullptr};
int __state_memory_index{0};  // units of 32-bits
int __channel_block_offset_in{0};
int __channel_block_offset_out{0};
float* __audio_out{nullptr};
float* __audio_in{nullptr};
float* __parameter_out{nullptr};
float* __parameter_in{nullptr};

void clavm_initialize(void* state_memory, void* audio_memory) {
  __state_memory = state_memory;
  __audio_memory = audio_memory;
  __audio = static_cast<Audio*>(__audio_memory);
}

void clavm_update(double time, float samplerate, int framecount,
                  int channel_count_out, int channel_count_in,
                  int parameter_count_out, int parameter_count_in) {
  __audio->time = time;
  __audio->samplerate = samplerate;
  __audio->framecount = framecount;
  __audio->channel_count_out = channel_count_out;
  __audio->channel_count_in = channel_count_out;
  __audio->parameter_count_out = parameter_count_out;
  __audio->parameter_count_in = parameter_count_out;

  int __offset_audio_out = sizeof(Audio) / sizeof(float);
  int __offset_audio_in =
      __offset_audio_out + __audio->channel_count_out * __audio->framecount;
  int __offset_parameter_out =
      __offset_audio_in + __audio->channel_count_in * __audio->framecount;
  int __offset_parameter_in =
      __offset_parameter_out + __audio->parameter_count_out;

  auto* memory = static_cast<float*>(__audio_memory);
  __audio_out = &memory[__offset_audio_out];
  __audio_in = &memory[__offset_audio_in];
  __parameter_out = &memory[__offset_parameter_out];
  __parameter_in = &memory[__offset_parameter_in];
}

void clavm_process_block(void (*play)(void)) {
  if (play == nullptr) {
    return;
  }

  __channel_block_offset_in = 0;
  __channel_block_offset_out = 0;
  for (int i = 0; i < __audio->framecount; i++) {
    TRACE("play() // %lf\n", __audio->time);
    __state_memory_index = 0;
    play();
    __channel_block_offset_in += __audio->channel_count_in;
    __channel_block_offset_out += __audio->channel_count_out;
    __audio->time += 1.0 / __audio->samplerate;
  }

  // XXX maybe capture the __state_memory_index because
  // that's how much state memory we are using in our
  // user code.
}

float* clavm_audio_out() { return __audio_out; }
float* clavm_audio_in() { return __audio_in; }
float* clavm_parameter_out() { return __parameter_out; }
float* clavm_parameter_in() { return __parameter_in; }

//
// User code API
//

float _samplerate() {
  TRACE("_samplerate\n");
  return __audio->samplerate;
}

double _time() {
  return __audio->time;
}

float* _float(int howmany) {
  TRACE("_float(%d)\n", howmany);
  float* pointer = &static_cast<float*>(__state_memory)[__state_memory_index];
  __state_memory_index += howmany;
  return pointer;
}

// double* _double(int howmany) {
//   TRACE("_double(%d)\n", howmany);
//   double* pointer =
//   reinterpret_cast<double*>(&static_cast<float*>(__state_memory)[__state_memory_index]);
//   __state_memory_index += howmany * 2;
//   return pointer;
// }

int* _int(int howmany) {
  TRACE("_int(%d)\n", howmany);
  int* pointer = &static_cast<int*>(__state_memory)[__state_memory_index];
  __state_memory_index += howmany;
  return pointer;
}

// long* _long(int howmany) {
//   TRACE("_long(%d)\n", howmany);
//   long* pointer =
//   reinterpret_cast<long*>(&static_cast<int*>(__state_memory)[__state_memory_index]);
//   __state_memory_index += howmany * 2;
//   return pointer;
// }

float _in(int channel) {
  TRACE("_in(%d)\n", channel);
  if (channel >= __audio->channel_count_in || channel < 0) {
    return 0.0f;
  }
  return __audio_in[__channel_block_offset_in + channel];
}

void _out(int channel, float value) {
  TRACE("_out(%d, %lf)\n", channel, value);
  if (channel >= __audio->channel_count_out || channel < 0) {
    return;
  }
  __audio_out[__channel_block_offset_out + channel] = value;
}

void _lr(float left, float right) {
  TRACE("_lr(%lf, %lf)\n", left, right);
  __audio_out[__channel_block_offset_out + 0] = left;
  __audio_out[__channel_block_offset_out + 1] = right;
}

float _p(int number) {
  TRACE("_p(%d)\n", number);
  if (number < 0 || number >= __audio->parameter_count_in) {
    return 0;
  }
  return __parameter_in[number];
}

void _d(int number, float value) {
  TRACE("_d(%d, %lf)\n", number, value);
  if (number < 0 || number >= __audio->parameter_count_out) {
    return;
  }
  __parameter_out[number] = value;
}
}

