#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

#include "Compiler.hpp"
#include "Utilities.hpp"

#include "globals.h"
float* __cc = nullptr;

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

//
// add logging here
//
//

int main(int argc, char* argv[]) {
  __cc = new float[100];
  std::string code;
  if (argc > 1)
    code = slurp(argv[1]);
  else
    code = slurp();

  Compiler c;
  if (!c.compile(code.c_str())) {
    fprintf(stderr, "%s\n", c.error());
    return 3;
  }

  std::string fileName = "out.wav";
  int sampleRate = 44100;
  int sampleCount = 20 * sampleRate;

  drwav_data_format format;
  format.container = drwav_container_riff;
  format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
  format.channels = 2;
  format.sampleRate = 44100;
  format.bitsPerSample = 32;

  drwav wav;
  assert(drwav_init_file_write(&wav, fileName.c_str(), &format, nullptr));

  for (int i = 0; i < sampleCount; i += 1024) {
    float* audio = c.process(1024, i, sampleRate);
    // for (int j = 0; j < 1024; j++) {
    //     //printf("%f,%f\n", audio[j]);
    // }
    drwav_uint64 framesWritten = drwav_write_pcm_frames(&wav, 1024, audio);
    assert(framesWritten == 1024);
  }

  drwav_uninit(&wav);
}
