#include <lo/lo.h>
#include <lo/lo_cpp.h>

#include <cassert>
#include <iostream>
#include <thread>

#include "HotSwap.hpp"
#include "RtAudio.h"

const int FRAME_COUNT = 1024;
const int SAMPLE_RATE = 48000;
const int CHANNELS = 2;

int main(int argc, char* argv[]) {
  lo::ServerThread liblo(9000);
  if (!liblo.is_valid()) {
    std::cout << "Nope." << std::endl;
    return 1;
  }

  HotSwap hotswap;

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

    assert(block != nullptr);

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
