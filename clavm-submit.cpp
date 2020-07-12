//
// C.A.V.M.
//

#include <lo/lo.h>
#include <lo/lo_cpp.h>

#include <chrono>
#include <iostream>
#include <string>

#include "Compiler.hpp"
#include "Utilities.hpp"

//
// cavm-submit
//

int main(int argc, char* argv[]) {
  //
  // load code from file or stdin
  //

  Timer timer;

  std::string code;
  if (argc > 1)
    code = slurp(argv[1]);
  else
    code = slurp();

  timer.check("load");

  //
  // compile code or print error
  //

  Compiler c;

  if (!c.compile(code.c_str())) {
    fprintf(stderr, "%s\n", c.error());
    return 3;
  }

  timer.check("compile");

  //
  // test code by running. this might crash this process.
  //

  float* audio = c.process(1024, 0, 44100);
  audio[0] = 0;

  timer.check("test");

  //
  // send code on to audio virtual machine
  //

  lo::Address address("localhost", "9000");
  lo::Blob blob(code.size(), code.data());
  lo::Message message;
  message.add(blob);
  address.send("code", message);
  timer.check("send");

  //
  // show elapsed time for each stage
  //

  timer.delta();
  // timer.percent();
  timer.print("%s:%3.1lf ");
}
