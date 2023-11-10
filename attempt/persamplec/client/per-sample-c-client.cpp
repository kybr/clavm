#include <lo/lo.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "libtcc.h"

#include "../HostInterface.h"

using ProcessFunc = void (*)(double time, float *input, float *output);
using std::string;
std::string slurp(const std::string &fileName);
std::string slurp();

unsigned SAMPLE_RATE = 44100;

int main(int argc, char *argv[]) {
  // use command line arguments to...
  // set destination address and port (defaults 127.0.0.1:9011)
  // disable compile checking or other processes

  std::string sourceCode;
  if (argc > 1)
    sourceCode = slurp(argv[1]);
  else
    sourceCode = slurp();

  // try to compile the program immediately here in the client so we might
  // report that to the user as soon as possible. don't send failing programs
  // to the server.
  //
  TCCState *instance = tcc_new();
  // TODO: determine these from a modeline
  // tcc_set_options(instance, "-Wall -Werror");
  tcc_set_options(instance, "-Wall -Werror");
  // tcc_set_options(instance, "-Wall -Werror -nostdinc");
  // tcc_set_options(instance, "-Wall -Werror -nostdinc -nostdlib");
  tcc_set_output_type(instance, TCC_OUTPUT_MEMORY);

  // XXX sample rate
  //
  // making sample rate a function call, a global variable, or a
  // parameter of the process callback might be better
  //
  char buffer[10];
  // TODO: determine these from a modeline
  sprintf(buffer, "%u", SAMPLE_RATE);
  tcc_define_symbol(instance, "SAMPLE_RATE", buffer);

  if (0 != tcc_compile_string(instance, sourceCode.c_str())) {
    // TCC already printed the error on the standard out
    exit(1);
  }
  // add symbols
  tcc_add_symbol(instance, "host_reset", (void *)host_reset);
  tcc_add_symbol(instance, "host_float", (void *)host_float);
  tcc_add_symbol(instance, "host_int", (void *)host_int);
  tcc_add_symbol(instance, "host_char", (void *)host_char);

  // XXX for now, we leave this here, but eventually, we should
  // get this linker information from a modeline
  //
  //tcc_add_library(instance, "m");

  // clang-format off
  // XXX this breaks on linux; we get this kind of stuff:
  //   tcc: error: '_etext' defined twice... may be -fcommon is needed?
  //   tcc: error: '_edata' defined twice... may be -fcommon is needed?
  //   tcc: error: '_end' defined twice... may be -fcommon is needed?
  //   tcc: error: '__preinit_array_start' defined twice... may be -fcommon is needed?
  //   tcc: error: '__preinit_array_end' defined twice... may be -fcommon is needed?
  //   tcc: error: '__init_array_start' defined twice... may be -fcommon is needed?
  //   tcc: error: '__init_array_end' defined twice... may be -fcommon is needed?
  //   tcc: error: '__fini_array_start' defined twice... may be -fcommon is needed?
  //   tcc: error: '__fini_array_end' defined twice... may be -fcommon is needed?
  // clang-format on
  // int size = tcc_relocate(instance, nullptr);

  if (-1 == tcc_relocate(instance, TCC_RELOCATE_AUTO)) {
    printf("Relocate failed! Linking problem?\n");
    exit(1);
  }

  ProcessFunc function = (ProcessFunc)tcc_get_symbol(instance, "process");
  if (function == nullptr) {
    printf("No process callback found.\n");
    exit(1);
  }

  // TODO: allow block processing by looking for a block() function
  // BlockProcessFunc function = (BlockFunc)tcc_get_symbol(instance, "block");
  // if (function == nullptr) {
  //  printf("No block callback found.\n");
  //  exit(1);
  //}

  using InitFunc = void (*)(void);
  InitFunc init = (InitFunc)tcc_get_symbol(instance, "begin");
  if (init) init();

  // try to run the program here in the client so we might find out if the
  // program crashes. report the crash to the user; don't send crashing
  // programs to the server.
  //
  // how many times should we call process? time this?
  //
  float i[8] = {0};
  float o[8] = {0};
  float maximum = 0;
  for (int n = 0; n < 44100 * 0.5; n++) {
    host_reset();
    function((double)n / SAMPLE_RATE, i, o);
    if (fabs(o[0]) > maximum)  //
      maximum = abs(o[0]);
    if (fabs(o[1]) > maximum)  //
      maximum = abs(o[1]);
  }

  //
  // of course, it fails on this program:
  //
  // void process(double d, float* i, float* o) {  //
  //   o[0] = d;
  //   o[1] = d;
  // }
  //
  // because d is below 1 for the first second!
  //

  if (maximum > 1) {
    printf("Clipping would occur.\n");
    exit(1);
  }

  //
  // at this point, we are either crashed or we are ready to send the code on
  //

  //
  // run pre-processor to remove comments and include files
  //
  // libtcc lets us run the preprocessor when we pass TCC_OUTPUT_PREPROCESS to
  // tcc_set_output_type(), but this spits the code out the standard output. we
  // must intercept this output to get it into a string. also, we might want to
  // do this step before anything else?
  //
  // https://stackoverflow.com/questions/5419356/redirect-stdout-stderr-to-a-string
  //

  //
  // maybe...
  //   minify (instead of clang-format)
  //   extract "modeline" commands
  //

  // TODO: switch back to oscpkt OR go TCP
  lo_blob b = lo_blob_new(sourceCode.size(), sourceCode.data());
  lo_address t = lo_address_new(nullptr, "9011");
  if (!lo_send(t, "/code", "b", b)) {
    printf("failed to send packet.");
    fflush(stdout);
    exit(1);
  }

  // send also to the visualizer
  lo_address T = lo_address_new(nullptr, "9010");
  if (!lo_send(T, "/code", "b", b)) {
    printf("failed to send packet.");
    fflush(stdout);
    exit(1);
  }

  return 0;
}

std::string slurp(const std::string &fileName) {
  using namespace std;
  ifstream ifs(fileName.c_str(), ios::in | ios::binary | ios::ate);

  ifstream::pos_type fileSize = ifs.tellg();
  ifs.seekg(0, ios::beg);

  vector<char> bytes(fileSize);
  ifs.read(bytes.data(), fileSize);

  return string(bytes.data(), fileSize);
}

std::string slurp() {
  using namespace std;
  vector<char> cin_str;
  // 64k buffer seems sufficient
  streamsize buffer_sz = 65536;
  vector<char> buffer(buffer_sz);
  cin_str.reserve(buffer_sz);

  auto rdbuf = cin.rdbuf();
  while (auto cnt_char = rdbuf->sgetn(buffer.data(), buffer_sz))
    cin_str.insert(cin_str.end(), buffer.data(), buffer.data() + cnt_char);
  return string(cin_str.data(), cin_str.size());
}
