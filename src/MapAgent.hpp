#pragma once
/*
MapAgent handles the creation, deletion, and accessing of Map(s) throughout
chimera.
*/
#include <unordered_map>
#include <string>
#include <memory>

#include "Map.hpp"

class MapAgent {
  std::unordered_map<std::string, std::shared_ptr<Map>> mMaps;

  public:
    std::shared_ptr<Map> acquireMap(std::string map);
};
