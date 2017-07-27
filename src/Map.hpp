#pragma once
#include "Object.hpp"
#include <vector>
#include <memory>

class Map {
  std::vector<std::shared_ptr<Object>> mObjects;

  public:
    void doProcess(int32_t elapsed);
};
