#include "TcpServer.hpp"
#include "Log.hpp"

#include "TcpClient.hpp"

#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

TcpServer::TcpServer(unsigned long int addr, int port) {
  //mAddress = address;
  mPort = port;
}
TcpServer::TcpServer(std::string address, int port) {
  /* Figure out our ipv4 or ipv6 address */
  struct addrinfo *res;
  memset(&mAddrInfo, 0, sizeof(mAddrInfo));
  mAddrInfo.ai_family  = PF_UNSPEC;
  mAddrInfo.ai_flags   = AI_NUMERICHOST;

  if (getaddrinfo(address.c_str(), NULL, &mAddrInfo, &res)) {
    ERR << "Invalid address, defaulting to ipv4 any and continuing";
    mAddrInfo.ai_family = AF_INET;
    mAddrInfo.ai_addr   = INADDR_ANY;
  } else {
    if (res->ai_family == AF_INET) {
      LOG << "ipv4: " << address.c_str();
    } else if (res->ai_family == AF_INET6) {
      LOG << "ipv6: " << address.c_str();
    } else {
      LOG << "unkn, defaulting to ipv4: " << address.c_str() << " - " << res->ai_family;
      res->ai_family = AF_INET;
    }
    // set our addr info to res
    mAddrInfo = *res;
    freeaddrinfo(res);
  }

  mAddress = address;
  mPort = port;
}
TcpServer::~TcpServer() {
  if (!isListening) return;

  for (int i = 0; i < mMaxFd; i++) {
    if (mPollFds[i].fd >= 0) {
      /* Delete our Client */
      std::unordered_map<int, std::shared_ptr<TcpClient>>::iterator it;
      if ((it = mClients.find(mPollFds[i].fd)) != mClients.end()) {
        removeClientByIterator(it);
      } else {
        close(mPollFds[i].fd);
      }
    }
  }
}

int TcpServer::listen() {
  int on = 1;
  int no = 0;
  if (isListening) {
    throw NetException("Already listening");
  }
  /* Create our stream socket for listening. */
  mListenSd = socket(mAddrInfo.ai_family, SOCK_STREAM, 0);
  if (mListenSd < 0) {
    throw NetException("socket() failed");
  }
  /* Allow socket descriptor reuse. */
  if (setsockopt(mListenSd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0) {
    close(mListenSd);
    throw NetException("setsockopt() failed");
  }
  /* Set the socket to be non-blocking -- all new connections will be non-blocking as well thru inheritance. */
  if (ioctl(mListenSd, FIONBIO, (char*)&on) < 0) {
    close(mListenSd);
    throw NetException("ioctl() failed");
  }
  /* Set the socket to allow ipv4 thru ipv6 */
  if (mAddrInfo.ai_family == AF_INET6) {
    if (setsockopt(mListenSd, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&no, sizeof(no)) < 0) {
      close(mListenSd);
      throw NetException("setsockopt() failed");
    }
  }
  /* Set up our listening socket */
  struct sockaddr_in*  sa   = (sockaddr_in*)&mAddr;
  struct sockaddr_in6* sa6  = (sockaddr_in6*)&mAddr;
  memset(&mAddr, 0, sizeof(mAddr));
  if (mAddrInfo.ai_family == AF_INET) { // ipv4 address specified
    sa->sin_family = AF_INET;
    sa->sin_port = htons(mPort);
    inet_pton(AF_INET, mAddress.c_str(), &(sa->sin_addr));
  } else if (mAddrInfo.ai_family == AF_INET6) { // ipv6 address specified
    sa6->sin6_family = AF_INET6;
    sa6->sin6_port = htons(mPort);
    inet_pton(AF_INET6, mAddress.c_str(), &(sa6->sin6_addr));
  } else { // no address specified, listen on any
    sa->sin_addr.s_addr = htonl(INADDR_ANY);
  }
  /* Bind the listening socket. */
  if (bind(mListenSd, (struct sockaddr*)&mAddr, sizeof(struct sockaddr)) < 0) {
    close(mListenSd);
    throw NetException("bind() failed");
  }
  /* Set the listen back log */
  if (::listen(mListenSd, 128) < 0) {
    close(mListenSd);
    throw NetException("listen() failed");
  }
  /* Initialize our poll fds */
  memset(mPollFds, 0, sizeof(mPollFds));
  /* Setup and add our listening socket to our poll fds */
  mPollFds[0].fd = mListenSd;
  mPollFds[0].events = POLLIN;
  /* Set our listening flag for future operations */
  isListening = true;
  mLastCheckTime = mStartTime = (uintmax_t)std::time(nullptr);
  return 0;
}

int TcpServer::readPackets() {
  mCurrentTime = (uintmax_t)std::time(nullptr);
  int on = 1;
  int new_size;
  uint8_t buffer[1024]; // FIXME

  if (poll(mPollFds, mMaxFd, 0) < 0) {
    throw NetException("poll() failed");
  }
  /* At least one descriptor is readable, let's do something with 'em */
  new_size = mMaxFd;
  for (int i = 0; i < new_size; i++) {
    /* no event, continue */
    if (mPollFds[i].revents == 0) continue;
    /* New fd is our listening socket, therefore we have a new connection */
    if (mPollFds[i].fd == mListenSd) {
      /* Loop over our new connections, adding them to mPollFds */
      int new_sd = -1;
      do {
        /* Accept new connections. EWOULDBLOCK signifies we have accepted all -- other errors cause the server to quit */
        struct sockaddr_storage new_addr;
        socklen_t new_addr_len = sizeof(new_addr);
        memset(&new_addr, 0, sizeof(new_addr));

        new_sd = accept(mListenSd, (struct sockaddr*)&new_addr, &new_addr_len);
        if (new_sd < 0) {
          if (errno != EWOULDBLOCK) {
            mQuit = true;
            throw NetException("accept() failed");
          }
          break;
        }
        /* Set the new connection to non-blocking */
        // FIXME: I thought the listening non-blocking chained to this
        if (ioctl(new_sd, FIONBIO, (char*)&on) < 0) {
          ERR << "ioctl() failed, client will block";
        }
        /* Add the connection to our poll fds */
        LOG << "New connection on " << new_sd;
        mPollFds[mMaxFd].fd = new_sd;
        mPollFds[mMaxFd].events = POLLIN;
        mMaxFd++;
        //
        mClients.insert({new_sd, std::shared_ptr<TcpClient>(new TcpClient(new_sd, &new_addr))});
        mClients[new_sd]->mLastTime = mCurrentTime;
      } while (new_sd != -1);
    /* New fd is an existing socket, read it */
    } else {
      bool close_fd = false;
      int status    = 0;
      /* Receive all data on fd */
      while (true) {
        /* Receive up to buffer and append to TcpClient's mBuffer */
        status = recv(mPollFds[i].fd, buffer, sizeof(buffer), 0);
        if (status < 0) {
          if (errno != EWOULDBLOCK) {
            ERR << "recv() failed, closing connection to " << mPollFds[i].fd;
            close_fd = true;
          }
          break;
        /* Has the client closed the connection?  */
        } else if (status == 0) {
          LOG << "connection " << mPollFds[i].fd << " closed";
          close_fd = true;
          break;
        }
        // copy into mClient's buffer
        mClients[mPollFds[i].fd]->mBuffer.insert(mClients[mPollFds[i].fd]->mBuffer.end(), buffer, buffer+status);
        mClients[mPollFds[i].fd]->mLastTime = mCurrentTime;
        mClients[mPollFds[i].fd]->mHasNewData = true;
      }
      /* Close the fd if told to */
      if (close_fd) {
        std::unordered_map<int, std::shared_ptr<TcpClient>>::iterator it;
        if ((it = mClients.find(mPollFds[i].fd)) != mClients.end()) {
          removeClientByIterator(it);
        } else {
          close(mPollFds[i].fd);
        }
        mPollFds[i].fd = -1;
        mCleanFds = true;
      }
    }
  }
  /* Check our clients for a lack of updates every minute */
  if (mCurrentTime - mLastCheckTime >= 10) {
    if (clearDeadClients() > 0) mCleanFds = true;
    mLastCheckTime = mCurrentTime;
  }
  /* Clean up our mPollFds of destroyed fds */
  if (mCleanFds) {
    mCleanFds = false;
    for (int i = 0; i < mMaxFd; i++) {
      if (mPollFds[i].fd == -1) {
        for (int j = i; j < mMaxFd; j++) {
          mPollFds[j].fd = mPollFds[j+1].fd;
        }
        i--;
        mMaxFd--;
      }
    }
  }
  return 0;
}

void TcpServer::removeClientByFd(int fd) {
  std::unordered_map<int, std::shared_ptr<TcpClient>>::iterator it;
  if ((it = mClients.find(fd)) != mClients.end()) removeClientByIterator(it);
}

void TcpServer::removeClientByIterator(std::unordered_map<int, std::shared_ptr<TcpClient>>::iterator it) {
  for (int i = 0; i < mMaxFd; i++) {
    if (mPollFds[i].fd == it->first) {
      mPollFds[i].fd = -1;
      break;
    }
  }
  mCleanFds = true;

  //delete it->second;
  mClients.erase(it);
}

int TcpServer::broadcast(const void* buffer, size_t len) {
  std::unordered_map<int, std::shared_ptr<TcpClient>>::iterator it;
  ssize_t sent;

  it = mClients.begin();
  while (it != mClients.end()) {
    sent = it->second->send(buffer, len);
    if (sent == -1) {
      removeClientByIterator(it++);
    } else {
      ++it;
    }
  }
}

int TcpServer::clearDeadClients() {
  std::unordered_map<int, std::shared_ptr<TcpClient>>::iterator it;
  int i = 0;

  it = mClients.begin();
  while (it != mClients.end()) {
    // kick clients that haven't responded for 60 seconds
    if (mCurrentTime - (uintmax_t)it->second->mLastTime >= 60) {
      removeClientByIterator(it++);
      i++;
    } else {
      ++it;
    }
  }
  return i;
}
