#pragma once

#include <string>

template <typename STRUCT>
class _Client;

template <typename STRUCT>
class Client {
  _Client<STRUCT>* _implementation;
 public:
  Client(const std::string& prefix);
  ~Client();
  bool swap();
  STRUCT* memory();
};

