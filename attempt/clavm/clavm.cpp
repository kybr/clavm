#include <lo/lo.h>
#include <lo/lo_cpp.h>

#include <iostream>
#include <thread>

#include "HotSwap.hpp"
#include "RtAudio.h"
#include "RtMidi.h"
#include "globals.h"

const int FRAME_COUNT = 1024;
const int SAMPLE_RATE = 48000;
const int CHANNELS = 2;

float* __cc = nullptr;

void midi_callback(double timeStamp, std::vector<unsigned char> *message, void *userData) {
  if (message->size() < 1) return; // XXX FAIL

  if (message->at(0) != 176) return; // IGNORE all but CC
  unsigned char control = message->at(1);
  unsigned char value = message->at(2);

  // course control; most significant bits
  if (control >= 20 && control <= 31) {
    __cc[control - 20] = float(value) / 128.0;
  }

  // fine control; least significant bits
  if (control >= 52 && control <= 62) {
    __cc[control - 52] += float(value) / (128 * 128);
    //printf("_cc(%d) -> %f\n", control - 52, __cc[control - 52]);
  }

  //printf("CC %d ~ %d\n", control, value);
}

int main(int argc, char* argv[]) {
  __cc = new float[100];

  lo::ServerThread liblo(9000);
  if (!liblo.is_valid()) {
    std::cout << "Nope." << std::endl;
    return 1;
  }

  RtMidiIn *midi = nullptr;
//  bool found_port = false;
//  unsigned port;
//  for (unsigned i = 0; i < midi->getPortCount(); i++) {
//    auto name = midi->getPortName(i);
//    printf("MIDI port: %s\n", name.c_str());
//    if (name == "CLAVM") {
//      port = i;
//      found_port = true;
//      break;
//    }
//  }
//  if (found_port) {
//    midi->openPort(port);
//  }
//  else {
  try {
    midi = new RtMidiIn();
    midi->openVirtualPort("CLAVM");
    midi->openPort(0);
  }
  catch (...) {
  }
//  }

  printf("opening midi port '%s'\n", midi->getPortName(0).c_str());

  HotSwap hotswap;

  if (midi) 
  midi->setCallback(midi_callback, &hotswap);

  liblo.add_method("/c", "ib", [&hotswap](lo_arg** argv, int) {
    int version = argv[0]->i;

    unsigned char* data = (unsigned char*)lo_blob_dataptr((lo_blob)argv[1]);
    int size = lo_blob_datasize((lo_blob)argv[1]);
    std::string code(data, data + size);
    // printf("%s", code.c_str());

    if (hotswap.compile(code.c_str())) {
      printf(".");
      fflush(stdout);
    } else {
      // this should not happen
      printf("%s\n", hotswap.error());
    }
    return 0;
  });

  RtAudioCallback process = [](void* outputBuffer, void* inputBuffer,
                               unsigned frameCount, double streamTime,
                               RtAudioStreamStatus status, void* data) -> int {
    static int sample = 0;

    auto hotswap = static_cast<HotSwap*>(data);
    auto o = static_cast<float*>(outputBuffer);
    // auto i = static_cast<float*>(inputBuffer);

    float* block = hotswap->process(frameCount, sample, SAMPLE_RATE);

    if (block == nullptr) {
      printf("block is null\n");
      exit(7);
    }

    for (unsigned i = 0; i < 2 * frameCount; ++i)  //
      o[i] = block[i];

    sample += frameCount;

    return 0;
  };

  //
  //
  //

  RtAudio dac;

  RtAudio::StreamParameters oParams;
  oParams.deviceId = dac.getDefaultOutputDevice();
  oParams.nChannels = CHANNELS;

  RtAudio::StreamParameters iParams;
  iParams.deviceId = dac.getDefaultInputDevice();
  iParams.nChannels = CHANNELS;

  RtAudio::DeviceInfo info = dac.getDeviceInfo(dac.getDefaultOutputDevice());
  printf("device: %s\n", info.name.c_str());

  try {
    unsigned frameCount = FRAME_COUNT;
    unsigned sampleRate = SAMPLE_RATE;

#ifdef __PI__
    dac.openStream(&oParams, nullptr, RTAUDIO_FLOAT32, sampleRate, &frameCount,
                   process, &hotswap);
#else
    dac.openStream(&oParams, nullptr /* &iParams */, RTAUDIO_FLOAT32,
                   sampleRate, &frameCount, process, &hotswap);
#endif

    if (FRAME_COUNT != frameCount) {
      // die
    }
    dac.startStream();
  } catch (RtAudioError& e) {
    printf("ERROR: %s\n", e.getMessage().c_str());
    fflush(stdout);
    exit(1);
  }

  liblo.start();

  //
  //
  //

  printf("Hit enter to quit...\n");
  fflush(stdout);

  getchar();

  //
  //
  //

  liblo.stop();

  try {
    dac.stopStream();
    dac.closeStream();
  } catch (RtAudioError& error) {
    error.printMessage();
  }
}
