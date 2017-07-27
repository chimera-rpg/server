#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>

class Cfg {
  std::string mName;
  std::unordered_map<std::string, std::string> mTable;

  public:
    Cfg();
    Cfg(std::string name);

    int readFromStream(std::ifstream &in);
    int readFromFilename(std::string name);

    std::string get(std::string key);
    void set(std::string key, std::string value);
};
