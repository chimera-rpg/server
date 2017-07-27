#include "TcpClient.hpp"
#include <arpa/inet.h>
#include <unistd.h>

#include "Log.hpp"

#include <sstream>
#include <iomanip>

TcpClient::TcpClient(int sd, struct sockaddr_storage* addr) {
  char ip[100];

  // convert addr to ipv4 or ipv6 char address
  struct sockaddr_in  *sa   = (sockaddr_in*)addr;
  struct sockaddr_in6 *sa6  = (sockaddr_in6*)addr;
  if (((sockaddr*)addr)->sa_family == AF_INET) {
    inet_ntop(AF_INET, &(sa->sin_addr), ip, sizeof(ip)-1);
  } else if (((sockaddr*)addr)->sa_family == AF_INET6) {
    inet_ntop(AF_INET6, &(sa6->sin6_addr), ip, sizeof(ip)-1);
  }

  mSocket     = sd;
  mAddress    = ip;
  mPort       = ntohs(sa->sin_port);
  mStartTime  = std::time(nullptr);
  mLastTime   = mStartTime;
  DBG << mAddress << ":" << mPort << "@" << mSocket << ", " << mStartTime;
}
TcpClient::~TcpClient() {
  if (mSocket != -1) close(mSocket);
  DBG << mAddress << ":" << mPort << "@" << mSocket << ", for " << ((uintmax_t)time(nullptr) - (uintmax_t)mStartTime);
}

ssize_t TcpClient::send(const void *buf, size_t len) {
  if (mSocket == -1) return -1;

#ifdef MSG_NOSIGNAL
  ssize_t rc = ::send(mSocket, buf, len, MSG_NOSIGNAL);
#else // os x?
  ssize_t rc = ::send(mSocket, buf, len, MSG_OOB);
#endif
  return rc;
}
