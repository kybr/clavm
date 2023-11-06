#pragma once

#include <string>
#include <functional>

template <typename STRUCT>
class _Server;

template <typename STRUCT>
class Server {
  _Server<STRUCT>* _implementation;
 public:
  Server(const std::string& prefix);
  ~Server();
  void run(std::function<void(STRUCT*)> handler);
  void stop();
};

