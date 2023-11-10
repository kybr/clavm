#pragma once

#include <string>

using std::string;

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
