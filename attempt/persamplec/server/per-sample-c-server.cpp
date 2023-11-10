#include <lo/lo.h>

#include <iostream>

#include "RtAudio.h"
#include "compiler.h"

void host_reset();

unsigned CHANNELS_OUT = 2;
unsigned CHANNELS_IN = 0;
unsigned SAMPLE_RATE = 44100;
unsigned FRAME_COUNT = 512;

uint16_t fletcher16(const uint8_t *data, size_t len);
//////////////////////////////////////////////////////////////////////////////
// AUDIO THREAD //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void clamp(float *data, int size) {
  for (int i = 0; i < size; i++) {
    if (data[i] > 1) data[i] = 1;
    if (data[i] < -1) data[i] = -1;
  }
}

lo_address t = lo_address_new(nullptr, "9010");

// TODO:
// - add Ableton Link support
// - cross-fade content on change
//   + detect changes by looking at the pointer
//   + use both functions for this frame
//   + but this doubles (or more) the amount of work this callback might
//   do??
//     * justification: we only support small programs
// - add control inputs
int process(void *outputBuffer, void *inputBuffer, unsigned frameCount,
            double streamTime, RtAudioStreamStatus status, void *data) {
  //
  //
  SwappingCompiler &compiler(*static_cast<SwappingCompiler *>(data));
  float *input = static_cast<float *>(inputBuffer);
  float *output = static_cast<float *>(outputBuffer);

  lo_send(t, "/time", "d", streamTime);

  for (unsigned _ = 0; _ < frameCount * CHANNELS_OUT; _ = 1 + _)  //
    output[_] = 0;

  ProcessFunc f = compiler();

  if (f == nullptr) {
    return 0;
  }

  unsigned i = 0;
  unsigned o = 0;
  for (unsigned _ = 0; _ < frameCount; _ = 1 + _) {
    host_reset();
    f(streamTime, &input[i], &output[o]);
    streamTime += 1.0 / SAMPLE_RATE;
    i = i + CHANNELS_IN;
    o = o + CHANNELS_OUT;
  }

  // XXX clip check? because macOS volume control seems to just multiply by
  // a gain value---we should test this. if the number is very large even a
  // low setting may be very loud when the volume setting is low but not
  // mute.
  clamp(output, frameCount * CHANNELS_OUT);

  return 0;
}

//////////////////////////////////////////////////////////////////////////////
// COMPILER THREAD ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

std::string lastCode = "";
int handle_code(const char *path, const char *types, lo_arg **argv, int argc,
                void *data, void *user_data) {
  unsigned char *blobdata = (unsigned char *)lo_blob_dataptr((lo_blob)argv[0]);
  int blobsize = lo_blob_datasize((lo_blob)argv[0]);

  std::string sourceCode(blobdata, blobdata + blobsize);

  SwappingCompiler &compiler(*static_cast<SwappingCompiler *>(user_data));

  // printf("%s\n", sourceCode.c_str());

  // XXX
  //
  // - compute the hash of the code?
  // - increment a "build number" and store with the compiler?
  // - check if the code is exactly the same as last time

  // when used with a clang-format filter, this seems to ignore many minor
  // edits
  //
  if ((lastCode.size() == sourceCode.size()) && (lastCode == sourceCode)) {
    // skip the compile
    printf("ignoring repeated code\n");
    return 0;
  }
  lastCode = sourceCode;

  // Compile the code and maybe print stats
  //
  std::string err;
  printf("\n/////////////////////////////////////////////////////////////\n");
  if (compiler(sourceCode, &err)) {
    printf("SUCCESS!\n");
    printf("code size: %u\n", compiler.size());
    printf("string length: %zu\n", sourceCode.size());
    printf("checksum: %04x\n",
           fletcher16((const uint8_t *)sourceCode.c_str(), sourceCode.size()));
  } else {
    printf("%s\n", err.c_str());
  }
  printf("\n/////////////////////////////////////////////////////////////\n");
  fflush(stdout);

  return 0;
}

void handle_error(int num, const char *msg, const char *path) {
  printf("liblo server error %d in path %s: %s\n", num, path, msg);
  fflush(stdout);
}

//////////////////////////////////////////////////////////////////////////////
// MAIN THREAD ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// TODO:
//
// - gather and process command line settings
// - settings
//   + sample rate
//   + frame count
//   + channels in/out
//   + server port
//
int main(int argc, char *argv[]) {
  SwappingCompiler compiler;

  // SERVER/COMPILER THREAD
  //
  lo_server_thread s = lo_server_thread_new("9011", handle_error);
  lo_server_thread_add_method(s, "/code", "b", handle_code, &compiler);
  lo_server_thread_start(s);

  // AUDIO THREAD
  //
  RtAudio dac;

  RtAudio::StreamParameters oParams;
  oParams.deviceId = dac.getDefaultOutputDevice();
  oParams.nChannels = CHANNELS_OUT;

  RtAudio::StreamParameters iParams;
  iParams.deviceId = dac.getDefaultInputDevice();
  iParams.nChannels = CHANNELS_IN;

  try {
    unsigned frameCount = FRAME_COUNT;
    unsigned sampleRate = SAMPLE_RATE;

    dac.openStream(&oParams, CHANNELS_IN ? &iParams : nullptr, RTAUDIO_FLOAT32,
                   sampleRate, &frameCount, &process, &compiler);

    if (FRAME_COUNT != frameCount) {
      // die
    }
    dac.startStream();
  } catch (RtAudioError &e) {
    printf("ERROR: %s\n", e.getMessage().c_str());
    fflush(stdout);
    exit(1);
  }

  ////////////////////////////////////////////////////////////////////////////
  // WAIT FOR QUIT
  ////////////////////////////////////////////////////////////////////////////

  printf("Hit enter to quit...\n");
  fflush(stdout);
  getchar();

  try {
    dac.stopStream();
    dac.closeStream();
  } catch (RtAudioError &error) {
    error.printMessage();
  }

  lo_server_thread_free(s);
  return 0;
}

uint16_t fletcher16(const uint8_t *data, size_t len) {
  uint32_t c0, c1;
  unsigned int i;

  for (c0 = c1 = 0; len >= 5802; len -= 5802) {
    for (i = 0; i < 5802; ++i) {
      c0 = c0 + *data++;
      c1 = c1 + c0;
    }
    c0 = c0 % 255;
    c1 = c1 % 255;
  }
  for (i = 0; i < len; ++i) {
    c0 = c0 + *data++;
    c1 = c1 + c0;
  }
  c0 = c0 % 255;
  c1 = c1 % 255;
  return (c1 << 8 | c0);
}
