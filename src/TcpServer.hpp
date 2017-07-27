#pragma once
#include <stdexcept>

#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <unordered_map>
#include <memory>

//#include "TcpClient.hpp"
class TcpClient;

class NetException : public std::runtime_error {
  public:
    NetException(std::string wat) : std::runtime_error(wat) { }
};

class TcpServer {
  friend int main(int argc, char* argv[]); // I'm a verrry bad boy
  bool isListening  = false;
  bool mQuit        = false;
  bool mCleanFds    = false;

  uintmax_t mStartTime;       // time we started
  uintmax_t mLastCheckTime;   // time we last checked clients for dead
  uintmax_t mCurrentTime;     // time at beginning of readPackets()

  int mListenSd;
  struct sockaddr_storage mAddr;
  struct addrinfo         mAddrInfo;

  struct pollfd mPollFds[128];
  int    mMaxFd = 1;

  std::string mAddress;
  int         mPort;

  std::unordered_map<int, std::shared_ptr<TcpClient>> mClients;

  public:
    TcpServer(std::string address, int port);
    TcpServer(unsigned long int address, int port);
    ~TcpServer();

    int listen();
    int readPackets();
    int broadcast(const void* buffer, size_t len);
    void removeClientByFd(int fd);
    void removeClientByIterator(std::unordered_map<int, std::shared_ptr<TcpClient>>::iterator it);
    int clearDeadClients();
};
