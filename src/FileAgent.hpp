#pragma once
#include <fstream>
#include <cstdint>
#include <vector>

/*
FileAgent handles the reading and writing of data on the filesystem.
*/

class FileAgent {
  std::string mDirectory;   // location on the fs to read from
  public:

    void setDirectory(std::string directory) { mDirectory = directory; }

    std::vector<uint8_t> loadAsBuffer(std::string file) {};
    std::fstream loadAsStream(std::string file) {};
};
