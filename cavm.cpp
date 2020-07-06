#include <iostream>
#include <thread>

#include <lo/lo.h>
#include <lo/lo_cpp.h>

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

  liblo.add_method("code", "b", [&hotswap](lo_arg** argv, int) {
    unsigned char* data = (unsigned char*)lo_blob_dataptr((lo_blob)argv[0]);
    int size = lo_blob_datasize((lo_blob)argv[0]);
    std::string code(data, data + size);
    // printf("%s", code.c_str());

    if (hotswap.compile(code.c_str())) {
      // printf("~~~ compiled ~~~\n");
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
    auto i = static_cast<float*>(inputBuffer);

    float* block = hotswap->process(frameCount, sample, SAMPLE_RATE);

    // int n = 0;
    // for (int i = 0; i < frameCount; ++i) {
    //   o[n++] = block[i];
    //   o[n++] = block[i];
    // }

    for (int i = 0; i < 2 * frameCount; ++i)  //
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

  try {
    unsigned frameCount = FRAME_COUNT;
    unsigned sampleRate = SAMPLE_RATE;

    // dac.openStream(&oParams, &iParams, RTAUDIO_FLOAT32, sampleRate,
    // &frameCount, &process, &compiler);
    //
    dac.openStream(&oParams, &iParams, RTAUDIO_FLOAT32, sampleRate, &frameCount,
                   process, &hotswap);

    if (FRAME_COUNT != frameCount) {
      // die
    }
    dac.startStream();
  } catch (RtAudioError& e) {
    printf("ERROR: %s\n", e.getMessage().c_str());
    fflush(stdout);
    exit(1);
  }

  ////////////////////////////////////////////////////////////////////////////
  // WAIT FOR QUIT
  ////////////////////////////////////////////////////////////////////////////

  liblo.start();

  printf("Hit enter to quit...\n");
  fflush(stdout);

  getchar();

  try {
    dac.stopStream();
    dac.closeStream();
  } catch (RtAudioError& error) {
    error.printMessage();
  }

  liblo.stop();

  //
  //
  //

  /*
  using namespace std::chrono;

  int k = 0;
  while (true) {
    float* block = hotswap.process(1024, k * 1024, 44100);
    k += 1024;

    std::this_thread::sleep_for(milliseconds(23));
  }
  */
}
