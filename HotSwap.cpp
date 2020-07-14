#include "HotSwap.hpp"

#include <cassert>
#include <mutex>
#include <thread>

#include "Compiler.hpp"

HotSwap::HotSwap() {
  active = new Compiler;
  available = new Compiler;
#ifdef __PI__
  assert(available->load_dynamic("dynamic/lib/libtcc.so"));
#else
  assert(available->load_dynamic("dynamic/lib/libtcc.dylib"));
#endif

  active->compile("void play(void) {}");
  new_code_ready.store(false);
}

void HotSwap::info() {
  fprintf(stderr, "atomic %s\n",
          new_code_ready.is_lock_free() ? "is lock-free" : "uses lock");
  fprintf(stderr, "%d swaps\n", count_swap);
  fprintf(stderr, "%d microseconds, total\n", total_microsecond_wait);
  fprintf(stderr, "%d microseconds, maximum\n", microsecond_max_wait);
}

const char* HotSwap::error() {
  //
  return available->error();
}

bool HotSwap::compile(const char* code) {
  int wait = 0;
  int waitTime = 10;
  while (new_code_ready.load()) {
    std::this_thread::sleep_for(std::chrono::microseconds(waitTime));
    wait += waitTime;
  }
  if (wait > microsecond_max_wait)  //
    microsecond_max_wait = wait;
  total_microsecond_wait += wait;

  if (available->compile(code)) {
    new_code_ready.store(true);
    return true;
  }

  return false;
}

float* HotSwap::process(int N, int sample, int samplerate) {
  //
  //
  float* audio = active->process(N, sample, samplerate);

  // maybe swap compilers
  //
  if (new_code_ready.load()) {
    //
    // pointer swap
    auto tmp = active;
    active = available;
    available = tmp;

    count_swap++;

    // signal compiler thread
    new_code_ready.store(false);
  }

  return audio;
}
