#include "CommandParser.hpp"
#include "ClientProtocol.hpp"

#include <arpa/inet.h>

#include "Log.hpp"

/**
  Reads a single byte from the NetBuffer
  
  @param buffer a NetBuffer reference to read from
  @param offset byte offset into the buffer
  @param out a uint8_t reference to write to
  @return bytes read or 0 on end of buffer
*/
int CommandParser::readByte(NetBuffer& buffer, size_t offset, uint8_t& out) {
  if (buffer.size() < offset+1) return 0;
  out = buffer[offset];
  return 1;
}
/**
  Reads a big-endian 16-bit integer from the NetBuffer, calling ntohs in the process.

  @param buffer a NetBuffer reference to read from
  @param offset byte offset into the buffer
  @param out a uint16_t reference to write to
  @return bytes read, 0 if buffer was too short
*/
int CommandParser::readInt16(NetBuffer& buffer, size_t offset, uint16_t& out) {
  if (buffer.size() < offset+2) return 0;
  out = ntohs((((uint16_t)buffer[offset]) << 8) | (((uint16_t)buffer[offset+1]) << 0));
  return 2;
}
/**
  Reads a big-endian 32-bit integer from the NetBuffer, calling ntohl.

  @param buffer a NetBuffer reference to read from
  @param offset byte offset into the buffer
  @param out a int32_t reference to write to
  @return bytes read, 0 if buffer was too short
*/
int CommandParser::readInt32(NetBuffer& buffer, size_t offset, uint32_t& out) {
  if (buffer.size() < offset+4) return 0;
  out = (((uint32_t)buffer[offset+0]) << 24)
      | (((uint32_t)buffer[offset+1]) << 16)
      | (((uint32_t)buffer[offset+2]) << 8)
      | (((uint32_t)buffer[offset+3]) << 0);
  return 4;
}
/**
  Reads bytes from a NetBuffer.

  @param buffer a NetBuffer reference to read from
  @param offset byte offset into the buffer
  @param out a reference to a uint8_t pointer that MUST be nullptr
  @param len an amount of bytes to read
  @return bytes read, 0 if buffer was too short
*/
int CommandParser::readBytes(NetBuffer& buffer, size_t offset, uint8_t*& out, size_t length) {
  if (buffer.size() < offset+length || out != nullptr) return 0;
  out = new uint8_t[length];
  for (size_t i = 0; i < length; i++) {
    out[i] = buffer[offset+i];
  }
  return length;
}
/**
  Reads a short and up to that short in bytes from a NetBuffer

  @param buffer a NetBuffer reference to read from
  @param offset byte offset into the buffer
  @param chars a reference to a char pointer to a nullptr
  @param len a reference to a uint16_t
  @return bytes read, 0 if buffer was too short
*/
int CommandParser::readString(NetBuffer& buffer, size_t offset, uint16_t& len, uint8_t*& chars) {
  int bytes = 0;
  bytes = readInt16(buffer, offset, len);
  if (bytes <= 0) return 0;
  bytes += readBytes(buffer, offset+bytes, chars, len);
  if (bytes <= 0) return 0;
  return bytes;
}

int CommandParser::getCommand(std::deque<uint8_t>& buffer, Command& cmd) {
  int rc      = 0;
  size_t pos  = 0;
  size_t len  = buffer.size();
  // our structures to use for reading
  int      rc_in = 0;
  uint32_t in_int32;
  uint16_t in_int16;
  uint8_t  in_int8;

  if (len == 0) return CMD_EMPTY;
  // read our first byte
  cmd.mType = buffer[pos++];
  switch((ClientProtocol)cmd.mType) {
    case ClientProtocol::OKAY:
    case ClientProtocol::NOKAY:
    case ClientProtocol::NORTH:
    case ClientProtocol::NORTHEAST:
    case ClientProtocol::EAST:
    case ClientProtocol::SOUTHEAST:
    case ClientProtocol::SOUTH:
    case ClientProtocol::SOUTHWEST:
    case ClientProtocol::WEST:
    case ClientProtocol::NORTHWEST:
    break;
    case ClientProtocol::CHAT:
      cmd.mChat.mTo       = nullptr;
      cmd.mChat.mMessage  = nullptr;
      // read in our "to" string
      rc_in = readString(buffer, pos, (uint16_t&)cmd.mChat.mTo_len, (uint8_t*&)cmd.mChat.mTo);
      if (!rc_in) return CMD_INCOMPLETE;
      pos += rc_in;
      // read in our "message" string
      rc_in = readString(buffer, pos, (uint16_t&)cmd.mChat.mMessage_len, (uint8_t*&)cmd.mChat.mMessage);
      if (!rc_in) return CMD_INCOMPLETE;
      pos += rc_in;
    break;
    default: // unhandled command
      return CMD_INVALID;
    break;
  }

  buffer.erase(buffer.begin(), buffer.begin() + (len > pos ? pos : len));

  return CMD_OKAY;
}

int CommandParser::getCommands(std::deque<uint8_t>& buffer, std::vector<Command>& cmds) {
  int rc    = 0;
  bool loop = true;

  while (loop) {
    Command cmd;
    // retrieve a command from the buffer, modifying the buffer and populating cmd
    rc = getCommand(buffer, cmd);
    if (rc == CMD_OKAY) {
      cmds.push_back(cmd);
    } else { // empty, incomplete, or invalid
      loop = false;
    }
  }
  return rc;
}

