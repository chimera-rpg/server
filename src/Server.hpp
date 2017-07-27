#pragma once
#include <vector>
#include "TcpConnection.hpp"

class Server {
  public:
    Server();
    ~Server();
  private:
    std::vector<TcpConnection*> clients;
}
