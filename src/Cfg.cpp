#include "Log.hpp"
#include "Cfg.hpp"
#include <iostream>

Cfg::Cfg() {
}

Cfg::Cfg(std::string name) {
  mName = name;
  readFromFilename(mName);
}

int Cfg::readFromFilename(std::string name) {
  std::ifstream in(name);
  if (in.is_open()) {
    readFromStream(in);
  }
}

int Cfg::readFromStream(std::ifstream &in) {
  std::string line;
  std::string key, value;
  std::istringstream sin;

  while (std::getline(in, line)) {
    std::string::iterator it;
    std::size_t pos, start;

    // strip leading white-space
    start = line.find_first_not_of("\t ");
    if (start != std::string::npos) {
      line = line.substr(start);
    }
    // if empty or starts with ';', continue to next line
    if (line.size() == 0 || line[0] == ';') {
      continue;
    }
    // get our key
    pos = line.find_first_of("\t\n ");
    key = line.substr(start, pos);
    if (pos != std::string::npos) {
      line = line.substr(pos);
    }
    // strip leading white-space
    start = line.find_first_not_of("\t ");
    if (start != std::string::npos) {
      line = line.substr(start);
    }
    value = line;
    set(key, value);
  }
  return 0;
}

std::string Cfg::get(std::string key) {
  auto it = mTable.find(key);
  if (it != mTable.end()) {
    return it->second;
  }
  return "";
}

void Cfg::set(std::string key, std::string value) {
  mTable[key] = value;
}
