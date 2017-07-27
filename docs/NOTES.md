# kts - 2017-03-16 10:26 - Notes on Client Commands buffering
Following is a way to handle acquiring Commands from a dynamic buffer. This buffer may contain incomplete messages.

processCommands:
  int err = 0;
  if (mHasNewData) {
    bool loop = true;
    while(loop) {
      // OPTION A: an EOC is found (?)
      // OPTION B: First byte in buffer is the COMMAND byte. Reading is then delineated to different functions that read the buffer into their respective command structures. If during reading the buffer is empty, the functions return and the main while loop breaks, leaving the remaining data in the buffer. If during reading the COMMAND byte does not exist
      Command cmd;
      int rc

      // load Command from buffer into cmd. buffer is modified
      rc = getCommand(&buffer, &cmd);
      if (rc == 1) { // incomplete command, wait for more data
        loop = false;
      } else if (rc == -1) { // invalid command, clear buffer or kick client
        loop = false;
        buffer.empty();
        err = 1;
      }

      if (bufferIsEmpty) {
        mHasData = false;
        loop = false;
      }
    }
    mHasNewData = false;
  }
  return err;

