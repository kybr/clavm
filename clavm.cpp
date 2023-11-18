#include <iostream>
#include <thread>

#include "CLAVMAPI.h"
#include "Configuration.h"
#include "Help.h"
#include "Process.h"
#include "RtAudio.h"
#include "RtMidi.h"
#include "Semaphore.h"
#include "SharedMemory.h"

SharedMemory* code;
SharedMemory* state;
SharedMemory* audio;
Semaphore* submit;
Semaphore* compile;
Semaphore* compiler_a;
Semaphore* compiler_b;
Semaphore* executer_a;
Semaphore* executer_b;
Semaphore* swap;
Semaphore* compiler;
Semaphore* executer;

RtMidiIn* midi;

void execute() {
  executer->post();
  executer->wait(0.001);
}

void tryswap() {
  if (swap->trywait()) {
    if (compiler == compiler_a) {
      compiler = compiler_b;
    } else if (compiler == compiler_b) {
      compiler = compiler_a;
    }
    if (executer == executer_a) {
      executer = executer_b;
    } else if (executer == executer_b) {
      executer = executer_a;
    }
    swap->post();
  }
}

int process(void* output_buffer, void* input_buffer, unsigned framecount,
            double time, RtAudioStreamStatus status, void* userdata) {
  auto* output = static_cast<float*>(output_buffer);
  auto* input = static_cast<float*>(input_buffer);

  tryswap();

  // XXX refactor audio to allow for changes in settings: samplerate,
  // framecount, i/o channels, and parameters.

  clavm_update(time, AUDIO_SAMPLERATE, AUDIO_FRAMECOUNT, AUDIO_CHANNEL_OUT,
               AUDIO_CHANNEL_IN, AUDIO_PARAMETER_OUT, AUDIO_PARAMETER_IN);

  // copy inputs and parameters into memory
  // a
  for (int i = 0; i < AUDIO_FRAMECOUNT * AUDIO_CHANNEL_IN; i++) {
    // assumes interleaved
    clavm_audio_in()[i] = input[i];
  }
  for (int i = 0; i < AUDIO_PARAMETER_IN; i++) {
    clavm_parameter_in()[i] = 0;
  }

  execute();

  // copy outputs and parameters from memory
  //
  for (int i = 0; i < AUDIO_FRAMECOUNT * AUDIO_CHANNEL_OUT; i++) {
    // assumes interleaved
    output[i] = clavm_audio_out()[i];
  }
  for (int i = 0; i < AUDIO_PARAMETER_OUT; i++) {
    float _ = clavm_parameter_out()[i];
  }

  return 0;
}

int main() {
  ///////////////////////////////////////////////////////////////////////////
  // MIDI CONFIGURATION AND HANDLER /////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////

  RtMidiIn* midi = nullptr;
  try {
    midi = new RtMidiIn();
    midi->openVirtualPort("CLAVM");
    midi->openPort(0);
  } catch (...) {
  }

  ///////////////////////////////////////////////////////////////////////////
  // SHARED MEMORY AND SEMAPHORES ///////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////

  code = new SharedMemory(NAME_MEMORY_CODE, SIZE_MEMORY_CODE, true);
  state = new SharedMemory(NAME_MEMORY_STATE, SIZE_MEMORY_STATE, true);
  audio = new SharedMemory(NAME_MEMORY_AUDIO, SIZE_MEMORY_AUDIO, true);
  submit = new Semaphore(NAME_SEMAPHORE_SUBMIT, true, true);
  compile = new Semaphore(NAME_SEMAPHORE_COMPILE, true);
  compiler_a = new Semaphore(NAME_SEMAPHORE_COMPILER_A, true);
  compiler_b = new Semaphore(NAME_SEMAPHORE_COMPILER_B, true);
  executer_a = new Semaphore(NAME_SEMAPHORE_EXECUTER_A, true, true);
  executer_b = new Semaphore(NAME_SEMAPHORE_EXECUTER_B, true, true);
  swap = new Semaphore(NAME_SEMAPHORE_SWAP, true);
  compiler = compiler_a;
  executer = executer_b;

  ///////////////////////////////////////////////////////////////////////////
  // COMPILER PROCESSES /////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////

  Process tcc_a;
  char const* const a[] = {"tcc", NAME_SEMAPHORE_COMPILER_A,
                           NAME_SEMAPHORE_EXECUTER_A, nullptr};
  tcc_a.spawn(a);

  Process tcc_b;
  char const* const b[] = {"tcc", NAME_SEMAPHORE_COMPILER_B,
                           NAME_SEMAPHORE_EXECUTER_B, nullptr};
  tcc_a.spawn(b);

  ///////////////////////////////////////////////////////////////////////////
  // AUDIO THREAD AND CONFIGURATION /////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////

  //
  //
  //

  RtAudio dac;

  RtAudio::StreamParameters oParams;
  oParams.deviceId = dac.getDefaultOutputDevice();
  oParams.nChannels = AUDIO_CHANNEL_OUT;

  RtAudio::StreamParameters iParams;
  iParams.deviceId = dac.getDefaultInputDevice();
  iParams.nChannels = AUDIO_CHANNEL_IN;

  RtAudio::DeviceInfo info = dac.getDeviceInfo(dac.getDefaultOutputDevice());
  printf("device: %s\n", info.name.c_str());

  try {
    unsigned framecount = AUDIO_FRAMECOUNT;
    unsigned samplerate = AUDIO_SAMPLERATE;

    clavm_initialize(state, audio);

    dac.openStream(&oParams, &iParams, RTAUDIO_FLOAT32, samplerate, &framecount,
                   process, nullptr);

    if (AUDIO_FRAMECOUNT != framecount) {
      exit(1);
    }
    dac.startStream();
  } catch (...) {
    exit(1);
  }

  ///////////////////////////////////////////////////////////////////////////
  // COMPILER DISPATCHER ////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////

  for (int i = 0; i < 3000010; i++) {
    TRACE("CLAVM: Waiting on 'compile'\n");
    compile->wait();

    TRACE("CLAVM: Signaling 'compiler'\n");
    compiler->post();

    TRACE("CLAVM: Waiting on 'compiler'\n");
    compiler->wait();

    // XXX check if compiler succeeded!
    swap->post();
    swap->wait();

    TRACE("CLAVM: Signaling 'compile'\n");
    compile->post();
  }

  ///////////////////////////////////////////////////////////////////////////
  // SHUTDOWN ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////

  tcc_a.kill();
  tcc_b.kill();

  delete code;
  delete state;
  delete audio;
  delete submit;
  delete compile;
  delete compiler_a;
  delete compiler_b;
  delete executer_a;
  delete executer_b;
  delete swap;
  delete compiler;
  delete executer;
  delete midi;

  return 0;
}
