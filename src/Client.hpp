#pragma once
/**
A Client represents a connected user. It contains the NetClient being used for transferring data, a CommandBuffer, and the Object the client controls.
*/
#include "CommandBuffer.hpp"

#include <memory>

/* Forward declarations */
class Object;
class TcpClient;

class Client {
  std::shared_ptr<TcpClient>  mNetClient; // the network client this client is associated with
  CommandBuffer               mCommands;  // commands queued from the netclient
  std::shared_ptr<Object>     mObject;    // object this client is associated with

};
