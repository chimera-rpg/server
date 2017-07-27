#pragma once
#include <cstdint>

enum class ClientProtocol: uint8_t {
    PING  = 200
  , OKAY  = 1
  , NOKAY = 2
  , NORTH     = 100
  , NORTHEAST = 101
  , EAST      = 102
  , SOUTHEAST = 103
  , SOUTH     = 104
  , SOUTHWEST = 105
  , WEST      = 106
  , NORTHWEST = 107
  , USE       = 108
  , CHAT      = 120
  , SAY       = 121
  , TELL      = 122
};
