//
// C•L•A•V•M
//

#include "HotSwap.hpp"
#include "al/app/al_App.hpp"
#include "al/system/al_Time.hpp"

struct Visual : al::App {
  HotSwap hotswap;
  double time{0};

  void onCreate() override {}

  void onAnimate(double dt) override {}

  void onDraw(al::Graphics& g) override {
    g.clear(0, 0, 0);
    auto block = hotswap.process(width(), (int)(time * 44100), 44100);
    left.clear();
    right.clear();
    for (int i = 0; i < width(); i++) {
      left.vertex(2.0 * i / width() - 1, block[i * 2]);
      right.vertex(2.0 * i / width() - 1, block[i * 2 + 1]);
    }
  }

  bool onKeyDown(al::Keyboard const& k) override {
    return false;
  }

  void onMessage(al::osc::Message& m) override {
    m.print();
    // Check that the address and tags match what we expect
    if (m.addressPattern() == "/c" && m.typeTags() == "ib") {
      if (hotswap.compile("void play() {}")) {
        printf(".");
        fflush(stdout);

      } else {
        // this should not happen
        printf("%s\n", hotswap.error());
      }
    }
    if (m.addressPattern() == "/t" && m.typeTags() == "d") {
      m >> time;
    }
  }
};

int main(int argc, char* argv[]) { Visual().start(); }

#include "HotSwap.cpp"
#include "TCC.cpp"
