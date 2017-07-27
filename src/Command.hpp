#pragma once
#include <string>
#include <cstdint>
#include <vector>

struct Command {
  uint8_t mType;

  union {
    struct {
      char *mTo;
      size_t mTo_len;
      char *mMessage;
      size_t mMessage_len;

    } mChat;
  };
};
