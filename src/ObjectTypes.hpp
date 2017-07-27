#pragma once
#include <cstdint>

#define checkFlags(a,b) ((uint32_t)a & (uint32_t)b)

enum class ObjectType: uint8_t {
  NONE
  , FLOOR
  , WALL
  , ITEM
  , NPC
  , PLAYER
};

enum class ObjectFlags: uint32_t {
  NONE            = 0
  , CONTAINER     = (1 << 1)    // if this is a container
  , CONTAINED     = (2 << 1)    // if this object is contained
  , THINK         = (3 << 1)    // should doThink() be run
};
