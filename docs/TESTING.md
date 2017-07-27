# Network Protocol Testing via Shell
To send bytes to the server to test communication without a client, you can time and emulate a connection with the following:

    { echo -ne '\x78'; sleep 1; echo -ne '\x02\x00'; sleep 1; echo -ne 'hi'; sleep 1; echo -ne '\x03\x00bye'; sleep 5; echo -ne '\x78\x05\x00hello\x05\x00world'; sleep 2; } | nc 127.0.0.1 9999

What this does is:
  * send 0x78 (CHAT)
  * wait 1
  * send 0x02 0x00 (length of bytes to send)
  * wait 1
  * send 'hi' (recipient of chat)
  * wait 1
  * send 0x03 0x00 b y e (3 character string length, then string itself)
  * wait 5
  * send 0x78 0x05 0x00 h e l l o 0x05 0x00 w o r l d (CHAT, to length, to, msg length, msg)
  * wait 2

As such, the simply form of `{ echo -ne '\xCMDCODE} | nc 127.0.0.1 9999` can be used to test low-level networking.
