//
// C•L•A•V•M © Karl Yerkes 2019-2023
//

#pragma once

extern "C" {

struct Audio {
  double time;
  float samplerate;
  int blocksize;
  int channel_in;
  int channel_out;
  int parameter_in;
  int parameter_out;
};

//
// Platform API
//

void clavm_initialize(void* state_memory, void* audio_memory);
void clavm_process_block(void (*play)(void));

//
// User code API
//

float _samplerate();
double _time();
float* _float(int howmany);
int* _int(int howmany);
//double* _double(int howmany);
//long* _long(int howmany);
float _in(int channel);
void _out(int channel, float value);
void _lr(float left, float right);
float _p(int number);
void _d(int number, float value);

}
