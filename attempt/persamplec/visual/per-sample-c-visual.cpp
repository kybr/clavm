#include <cmath>
#include <mutex>
#include <regex>
#include <string>

#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
#include "libtcc.h"

#include "../HostInterface.h"

using std::string;

unsigned CHANNELS_OUT = 2;
unsigned CHANNELS_IN = 0;
unsigned SAMPLE_RATE = 44100;
unsigned FRAME_COUNT = 1024;

using namespace al;

// https://mattdesl.svbtle.com/drawing-lines-is-hard

const char* vertexCode = R"(
#version 330

uniform mat4 al_ModelViewMatrix;
uniform mat4 al_ProjectionMatrix;

uniform float cursor;
uniform float moveMode;

layout (location = 0) in vec3 position;

out float closeness;

void main() {
  vec3 p = position;
  if (p.x < cursor) {
    p.x += 1 - cursor;
  }
  else {
    p.x -= cursor + 1;
  }
  if (moveMode > 0.5)
    gl_Position = al_ProjectionMatrix * al_ModelViewMatrix * vec4(p, 1.0);
  else 
    gl_Position = al_ProjectionMatrix * al_ModelViewMatrix * vec4(position, 1.0);
  closeness = (p.x + 1) / 2;
}
)";

const char* fragmentCode = R"(
#version 330

uniform float color;

layout(location = 0) out vec4 fragmentColor;

in float closeness;

void main() {
  fragmentColor = vec4(color, color, color, closeness * closeness);
}
)";

// signature each c sketch
//
using ProcessFunc = void (*)(double time, float* input, float* output);

// "pimpl" pattern hides implementation
//
struct SwappingCompilerImplementation;

//
//
class SwappingCompiler {
  SwappingCompilerImplementation* implementation;

 public:
  int size();

  SwappingCompiler();
  ~SwappingCompiler();

  // call from the server thread
  //
  bool operator()(const string& code, string* err);

  // call from the audio thread
  //
  ProcessFunc operator()();
};

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
    // tcc_set_options(instance, "-Wall -Werror -nostdinc -nostdlib");
    tcc_set_options(instance, "-Wall -Werror");

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

    // tcc_add_symbol(instance, "log", (void*)logf);
    // tcc_add_symbol(instance, "log", (void*)logf);
    tcc_add_library(instance, "m");
    tcc_add_symbol(instance, "host_reset", (void*)host_reset);
    tcc_add_symbol(instance, "host_float", (void*)host_float);
    tcc_add_symbol(instance, "host_int", (void*)host_int);
    tcc_add_symbol(instance, "host_char", (void*)host_char);

    //  size = tcc_relocate(instance, nullptr);

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

inline float map(float value, float low, float high, float Low, float High) {
  return Low + (High - Low) * ((value - low) / (high - low));
}

const int N = 8;

struct MyApp : App {
  SwappingCompiler compiler;
  double streamTime{0};

  ShaderProgram shader;

  Mesh wave[N];

  Mesh horizontal, vertical;

  void onCreate() override {  //

    shader.compile(vertexCode, fragmentCode);

    vertical.primitive(Mesh::LINES);
    vertical.vertex(0, -1);
    vertical.vertex(0, 1);
    onResize();
  }

  float cursor{0};

  void onResize() {
    horizontal.primitive(Mesh::LINE_STRIP);
    float x = -1;
    for (int i = 0; i < width(); i++) {
      horizontal.vertex(x, 0);
      x += 2.0 / width();
    }

    for (int j = 0; j < N; j++) {
      wave[j].primitive(Mesh::LINES);
      // wave[j].primitive(Mesh::LINE_STRIP);
      wave[j].reset();
      float x = -1;
      for (int i = 0; i < width(); i++) {
        wave[j].vertex(x, 0);
        wave[j].vertex(x, 0);

        x += 2.0 / width();
      }
    }
    cursor = 1;
  }

  int index{0};
  void update(float* low, float* high) {
    for (int j = 0; j < N; j++) {
      wave[j].vertices()[index].y = low[j];
    }
    index++;
    for (int j = 0; j < N; j++) {
      wave[j].vertices()[index].y = high[j];
    }
    index++;
    if (index >= wave[0].vertices().size())  //
      index = 0;

    cursor = 2.0 * index / wave[0].vertices().size() - 1;
  }

  double time{0};
  float ins[8]{0};
  float outs[8]{0};
  float lows[8]{0};
  float highs[8]{0};

  double seconds{7};
  double samplesPerPixel{1};

  void onAnimate(double dt) override {
    static int Width;
    static int Height;
    if (Width != width() || Height != height()) {
      Width = width();
      Height = height();
      onResize();
    }

    samplesPerPixel = seconds * 44100 / width();
    if (time < streamTime - seconds) {
      time = streamTime - seconds;
    }

    auto func = compiler();
    if (func != nullptr) {
      double t = streamTime;
      while (time < t) {
        for (int j = 0; j < N; j++) {
          lows[j] = 1e35;    // was 1
          highs[j] = -1e35;  // was -1
        }
        for (int i = 0; i < (int)samplesPerPixel; i++) {
          host_reset();
          func(time, ins, outs);
          for (int j = 0; j < N; j++) {
            if (outs[j] < lows[j])  //
              lows[j] = outs[j];
            if (outs[j] > highs[j])  //
              highs[j] = outs[j];
          }
          time += 1.0 / 44100;
        }

        update(lows, highs);
      }
    }

    if (moveMode > 0.5)
      for (int j = 0; j < N; j++)  //
        wave[j].primitive(Mesh::LINES);
    else
      for (int j = 0; j < N; j++)  //
        wave[j].primitive(Mesh::LINE_STRIP);

    vertical.vertices()[0].x = cursor;
    vertical.vertices()[1].x = cursor;

    if (foreground == foreground_target) {
    } else {
      foreground = foreground * 0.9 + foreground_target * 0.1;
    }
  }

  float moveMode{0};

  bool onKeyDown(Keyboard const& k) override {
    if (k.key() == 27) {
      // "capture" the ESC key
      return false;
    }
  }
  bool onKeyUp(Keyboard const& k) override {
    if (k.key() == ' ') {
      foreground_target = 1 - foreground;
    }

    if (k.key() == '\\') {
      moveMode = 1 - moveMode;
    }

    if (k.key() == '-' || k.key() == '_') {
      seconds *= 1 / 1.23;
      // seconds -= 0.5;
      if (seconds < 0.25)  //
        seconds = 0.25;
    }
    if (k.key() == '+' || k.key() == '=') {
      // seconds += 0.5;
      seconds *= 1.23;
      if (seconds > 10)  //
        seconds = 10;
    }
    // std::cerr << samplesPerPixel << std::endl;
    return true;
  }

  float foreground{0};
  float foreground_target{0};

  void onDraw(Graphics& g) override {
    float grey = -0.2 * (foreground - 0.5) + 0.5;
    g.clear(1 - foreground);
    g.blending(true);
    g.blendModeTrans();
    g.camera(Viewpoint::IDENTITY);

    g.shader(shader);
    g.shader().uniform("cursor", (float)cursor);
    g.shader().uniform("moveMode", moveMode);
    g.update();

    g.shader().uniform("color", grey);
    g.draw(vertical);

    g.scale(1, 1.0 / 8, 1);
    g.translate(0, 1 + 6, 0);
    g.shader().uniform("color", grey);
    g.draw(horizontal);
    g.shader().uniform("color", foreground);
    g.draw(wave[0]);

    for (int n = 1; n < N; n++) {
      g.translate(0, -2, 0);
      g.shader().uniform("color", grey);
      g.draw(horizontal);
      g.shader().uniform("color", foreground);
      g.draw(wave[n]);
    }
  }

  void onMessage(osc::Message& m) override {
    if (m.addressPattern() == "/time") {
      double temp;
      m >> temp;
      if (temp < streamTime) {
        // the server restarted; we should restart
        time = temp - N;
      }
      streamTime = temp;
      // printf("time=%lf\n", streamTime);
      return;
    } else if (m.addressPattern() != "/code")
      return;

    osc::Blob blob;
    m >> blob;

    std::string sourceCode((const char*)blob.data,
                           ((const char*)blob.data) + blob.size);

    std::string err;
    if (compiler(sourceCode, &err)) {
      printf("SUCCESS!\n");
      printf("code size: %u\n", compiler.size());
      printf("string length: %zu\n", sourceCode.size());
    } else {
      printf("ERROR: %s\n", err.c_str());
    }
  }
};

int main() {
  MyApp app;
  app.vsync(true);
  app.fps(60);
  app.start();
}
