#pragma once
/* CommandParser
A static class for parsing a network buffer into commands.
*/
#include <deque>
#include <vector>
#include <cstdint>

#include "NetBuffer.hpp"
#include "Command.hpp"
#include "CommandBuffer.hpp"

class CommandParser {
  public:
    static int getCommand(NetBuffer&, Command& cmd);
    static int getCommands(NetBuffer&, CommandBuffer& cmds);
    enum rc {
      CMD_OKAY,
      CMD_EMPTY,
      CMD_INCOMPLETE,
      CMD_INVALID
    };
    static int readByte(NetBuffer&, size_t offset, uint8_t&);
    static int readInt16(NetBuffer&, size_t offset, uint16_t&);
    static int readInt32(NetBuffer&, size_t offset, uint32_t&);
    static int readBytes(NetBuffer&, size_t offset, uint8_t*&, size_t);
    static int readString(NetBuffer&, size_t offset, uint16_t& len, uint8_t*& chars);
  private:
    CommandParser() {};
};
