#pragma once

/*enum Protocol {
    PING  = 0
  , OKAY   = 1
  , NOKAY = 2
  , USER  = 3
  , PASS  = 4
  , GAME  = 10
};*/

enum ClientProtocol: uint8_t {
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
};
