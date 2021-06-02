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
  lo::ServerThread liblo(11000);
  if (!liblo.is_valid()) {
    std::cout << "Nope." << std::endl;
    return 1;
  }

  // XXX send a message to Vim explaining that this clavm-submit-listener was
  // restarted (perhaps after an error).
  //

  liblo.add_method("code", "b", [](lo_arg** argv, int) {
    Timer timer;

    unsigned char* data = (unsigned char*)lo_blob_dataptr((lo_blob)argv[0]);
    int size = lo_blob_datasize((lo_blob)argv[0]);
    std::string code(data, data + size);
    // printf("%s", code.c_str());

    // XXX this is a lie, right? because it does not count the time it took for
    // the message to get to us. consider adding a timestamp to the 'code'
    // message and checking it here?
    //
    timer.check("load");

    // XXX change the error reporting / time stuff to fill an OSC message and
    // send instead of using blocking console i/o here.

    //
    // compile code or print error
    //

    Compiler c;

    if (!c.compile(code.c_str())) {
      // XXX this does not get sent back to Vim like it needs to. FIXME
      //
      fprintf(stderr, "%s\n", c.error());
      fflush(stdout);
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

    // XXX we should not have to repackage the code here since it is already
    // exactly the code blob that CLAVM expects; FIXME
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
    printf("\n");
    fflush(stdout);
    return 0;
  });

  liblo.start();
  printf("Hit enter to quit...\n");
  fflush(stdout);
  getchar();
  liblo.stop();
}
