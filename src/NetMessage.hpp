#pragma once
/* Types of Messages
````````````````````````
TRAVEL              1 byte (signifies the client to wipe the map)
MSG                 1 byte
  fmt_message       2 byte len + payload
SAY                 1 byte
  from              2 byte len + payload
  fmt_message       2 byte len + payload
SHOUT               1 byte
  from              2 byte len + payload
  fmt_message       2 byte len + payload
WHISPER             1 byte
  from              2 byte len + payload
  fmt_message       2 byte len + payload

STATUS              1 byte (signifies the player's status)
  which             1 byte (the stat, such as: str,dex,con,wis,etc)
  value             2 byte len + payload (dynamic, could be 1byte or string)

ENT_UPDATE          1 byte
  id                2 byte
  commands count    1 byte
  command type      1 byte
    MOVE
      x             2 byte
      y             2 byte
      z             1 byte
    DIE
    IMAGE
      name (string) 2 byte len + payload
    SOUND
      name (string) 2 byte len + payload

ENT_CREATE          1 byte
  id                2 byte
  image (string)    2 byte len + payload

DATA
  type (enum:image,sound,etc.)  1 byte
  name (string)                 2 byte len + payload
  size (size_t)                 4 byte len + payload
  data (void*)

*/
class NetMessage {
  uint8_t   cmd;
  void      *payload;
  size_t    size;
};
