//
// C•L•A•V•M
//

#include "HotSwap.hpp"

#include "al/app/al_App.hpp"
#include "al/system/al_Time.hpp"


struct Visual : al::App {
  HotSwap hotswap;
  void onCreate() override {
  }

  void onAnimate(double dt) override {
  }

  void onDraw(al::Graphics& g) override {
    g.clear(1, 0, 0);
  }
  bool onKeyDown(al::Keyboard const& k) override {
      if (hotswap.compile("void play() { return; }")) {
        printf(".");
        fflush(stdout);
        hotswap.process(1000, 0, 44100);
      } else {
        // this should not happen
        printf("%s\n", hotswap.error());
      }
  }

  void onMessage(al::osc::Message& m) override {
    m.print();
    // Check that the address and tags match what we expect
    if (m.addressPattern() == "/c" && m.typeTags() == "ib") {
      if (hotswap.compile("void play() {}")) {
        printf(".");
        fflush(stdout);
        hotswap.process(1000, 0, 44100);
      } else {
        // this should not happen
        printf("%s\n", hotswap.error());
      }
    }
  }
};

int main(int argc, char* argv[]) {
  Visual().start();
}

#include "HotSwap.cpp"
#include "TCC.cpp"
