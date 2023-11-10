#include "compiler.h"

#include <math.h>

#include <mutex>
#include <regex>
#include <string>

#include "libtcc.h"

#include "../HostInterface.h"

using std::string;

extern unsigned SAMPLE_RATE;

//////////////////////////////////////////////////////////////////////////////
// Tiny C Compiler ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void tcc_error_handler(void* str, const char* msg) {
  static_cast<string*>(str)->append(msg);
}

struct TCC {
  TCCState* instance{nullptr};
  ProcessFunc function{nullptr};
  size_t size{0};

  ~TCC() { maybe_destroy(); }
  void maybe_destroy() {
    if (instance) {
      tcc_delete(instance);

      // tcc_delete does not null the pointer
      instance = nullptr;
    }
  }

  bool compile(const string& code, string* err) {
    maybe_destroy();
    instance = tcc_new();
    if (instance == nullptr) {
      err->append("Null Instance");
      return false;
    }

    tcc_set_error_func(instance, (void*)err, tcc_error_handler);

    // treat warnings as errors
    tcc_set_options(instance, "-Wall -Werror -nostdinc -nostdlib");
    // tcc_set_options(instance, "-Wall -Werror");

    tcc_set_output_type(instance, TCC_OUTPUT_MEMORY);

    // Define Pre-Processor Symbols
    //
    char buffer[10];
    sprintf(buffer, "%u", SAMPLE_RATE);
    tcc_define_symbol(instance, "SAMPLE_RATE", buffer);

    // Do the compile step
    //
    if (0 != tcc_compile_string(instance, code.c_str())) {
      // TODO: move all this to the client. the server should never fail to
      // compile now because the client does this check

      // Given error string like this:
      //   <string>:5: error: declaration expected
      //   $FILE ':' $LINE ':' $MESSAGE
      //
      std::regex re("^([^:]+):(\\d+):(.*)$");
      std::smatch m;
      if (std::regex_match(*err, m, re)) {
        // int line = stoi(m[2]);

        // XXX
        //
        // given the line number, extract the actual line

        err->clear();
        err->append("line:");
        err->append(std::to_string(stoi(m[2])));
        err->append(m[3].str());

      } else {
        err->append(" | failed to match error");
      }

      return false;
    }
    // add symbols
    tcc_add_symbol(instance, "host_reset", (void*)host_reset);
    tcc_add_symbol(instance, "host_float", (void*)host_float);
    tcc_add_symbol(instance, "host_int", (void*)host_int);
    tcc_add_symbol(instance, "host_char", (void*)host_char);

    // tcc_add_symbol(instance, "log", (void*)logf);
    // tcc_add_symbol(instance, "log", (void*)logf);
    //tcc_add_library(instance, "m");

    // XXX broken on linux
    // size = tcc_relocate(instance, nullptr);

    if (-1 == tcc_relocate(instance, TCC_RELOCATE_AUTO)) {
      err->append("Relocate Failed");
      return false;
    }

    function = (ProcessFunc)tcc_get_symbol(instance, "process");
    if (function == nullptr) {
      err->append("No 'process' Symbol");
      return false;
    }

    using InitFunc = void (*)(void);
    InitFunc init = (InitFunc)tcc_get_symbol(instance, "begin");
    if (init) init();

    return true;
  }
};

//////////////////////////////////////////////////////////////////////////////
// Private Implementation ////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

struct SwappingCompilerImplementation {
  TCC tcc[2];
  std::mutex lock;
  int active{0};
  bool shouldSwap{false};
  int size;

  bool compile(const string& code, string* err) {
#if _USE_TRY_LOCK
    if (tryLock && !lock.try_lock()) {
      string("No Lock");
      return false;
    }
#else
    lock.lock();  // blocking call
#endif

    bool compileSucceeded = false;

    if (tcc[1 - active].compile(code, err)) {
      this->size = tcc[1 - active].size;
      shouldSwap = true;
      compileSucceeded = true;
    }

    lock.unlock();
    return compileSucceeded;
  }

  ProcessFunc getFunctionMaybeSwap() {
    if (lock.try_lock()) {
      if (shouldSwap) active = 1 - active;
      shouldSwap = false;
      lock.unlock();
    }
    return tcc[active].function;
  }
};

//////////////////////////////////////////////////////////////////////////////
// Public Interface //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

SwappingCompiler::SwappingCompiler()
    : implementation(new SwappingCompilerImplementation) {}
SwappingCompiler::~SwappingCompiler() { delete implementation; }

int SwappingCompiler::size() { return implementation->size; }

bool SwappingCompiler::operator()(const string& code, string* err) {
  return implementation->compile(code, err);
}

ProcessFunc SwappingCompiler::operator()(void) {
  return implementation->getFunctionMaybeSwap();
}
