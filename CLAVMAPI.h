//
// C•L•A•V•M © Karl Yerkes 2019-2023
//

#pragma once

// Who needs to access audio/state memory?
//
//                audio     state
// usercode        r/w       r/w
// audio           r/w
//

extern "C" {

//
// Platform API
//

// call once on setup
//
void clavm_initialize(void* state_memory, void* audio_memory);

// call each time any of these settings change
//
void clavm_update(double time, float samplerate, int framecount,
                  int channel_count_out, int channel_count_in,
                  int parameter_count_out, int parameter_count_in);

// valid only after the above two functions are called
//
float* clavm_audio_out();
float* clavm_audio_in();
float* clavm_parameter_out();
float* clavm_parameter_in();

// execute user code many times
//
void clavm_process_block(void (*play)(void));

//
// User code API
//

//
float _samplerate();
double _time();

// get/use state memory
//
float* _float(int howmany);
int* _int(int howmany);

// accept audio input on a given channel
float _in(int channel);

// output audio to a given channel
//
void _out(int channel, float value);

// output stereo audio
//
void _lr(float left, float right);

// read an input parameter
//
float _p(int number);

// write and output (display) parameter
//
void _d(int number, float value);
}
