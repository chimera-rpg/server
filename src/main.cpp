#include "Log.hpp"
#include "TcpServer.hpp"
#include "Cfg.hpp"

#include <chrono>
#include <thread>

#include <cstdlib>
#include <signal.h>

#include "CommandParser.hpp"
#include "ClientProtocol.hpp"

#include "TcpClient.hpp"
#include "Client.hpp" // TODO: Remove this

bool doQuit = false;

void handleSigInt(int s) {
  LOG << "SIGINT, shutting down";
  doQuit = true;
}

int main(int argc, char* argv[]) {
  TcpServer* tcp_server;
  // initialize our log hooks
  gLogHooks[LOG_DEBUG] = [](const std::string type, const std::string msg) {
    std::cout << type << " " << msg;
  };
  gLogHooks[LOG_INFO] = [](const std::string type, const std::string msg) {
    std::cout << type << " " << msg;
  };
  gLogHooks[LOG_ERROR] = [](const std::string type, const std::string msg) {
    std::cout << type << " " << msg;
  };
  // initialize our sigint handler for clean exit
  struct sigaction sigHandler;
  sigHandler.sa_handler = handleSigInt;
  sigemptyset(&sigHandler.sa_mask);
  sigHandler.sa_flags = 0;
  sigaction(SIGINT, &sigHandler, NULL);
  // load conf
  Cfg cfg("server.cfg");

  tcp_server = new TcpServer(cfg.get("ip"), atoi(cfg.get("port").c_str()));

  if (tcp_server->listen()) {
    ERR << "err while listening";
    doQuit = true;
  }
  std::chrono::duration<long, std::ratio<1,1000>> tickrate(33); // 30fps in ms
  while (!doQuit) {
    auto start = std::chrono::high_resolution_clock::now();
    /* read our TCP sockets */
    tcp_server->readPackets();
    /* read our client commands */
    for (auto it = tcp_server->mClients.begin(); it != tcp_server->mClients.end();) {
      int rc            = 0;
      TcpClient* client = &*it->second;

      if (client->mHasNewData) {
        std::vector<Command> cmds;
        rc = CommandParser::getCommands(client->mBuffer, cmds);
        if (rc == CommandParser::CMD_INVALID) {
          DBG << "got an invalid cmd, kicking " << client->mAddress;
          tcp_server->removeClientByIterator(it++);
        } else if (rc == CommandParser::CMD_INCOMPLETE) {
          DBG << "incomplete message, waiting for more.";
          ++it;
        } else {
          DBG << "got a good cmd";
          ++it;
        }
        client->mHasNewData = false;
        for (int i = 0; i < cmds.size(); i++) {
          if (cmds[i].mType == (uint8_t)ClientProtocol::CHAT) {
            LOG << "to: " << cmds[i].mChat.mTo << ", msg: " << cmds[i].mChat.mMessage;
          }
        }
      } else {
        it++;
      }
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end-start;

    //tcp_server->broadcast((void*)PING, sizeof(uint8_t));

    std::this_thread::sleep_for(tickrate - elapsed);
  }

  delete tcp_server;

  return 0;
}
