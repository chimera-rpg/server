#pragma once
/**
TcpClient represents our TCP connection to a client. All incoming data is placed into mBuffer for parsing via the CommandParser into a CommandBuffer structure.

A TcpClient is generally associated with a Client, providing the underlying communication layer for a connected player.
*/
#include <sys/types.h>
#include <string>
#include <ctime>

#include <memory>

#include "NetBuffer.hpp"

class Client;

class TcpClient {
  friend int main(int argc, char* argv[]); // I'm a verrry bad boy
  friend class TcpServer;
  int         mSocket = -1;         // socket descriptor
  std::string mAddress;             // IP address of connection
  int         mPort;                // port of connection
  std::time_t mStartTime,           // connection start time
              mLastTime;            // last packet receive time

  NetBuffer   mBuffer;              // our network received buffer
  bool        mHasNewData = false;  // set when mBuffer is added to

  std::shared_ptr<Client> mClient;  // associated game client

  public:
    TcpClient(int sd, struct sockaddr_storage* addr);
    ~TcpClient();
    ssize_t send(const void *buf, size_t len);
};
