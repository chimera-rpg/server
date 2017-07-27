#include "MapAgent.hpp"
#include "globals.hpp"

#include "FileAgent.hpp"


/* acquireMap
Return a Map by creating it or returning the existing one from mMap.
*/
std::shared_ptr<Map> MapAgent::acquireMap(std::string map) {
  // Acquire our map
  std::unordered_map<std::string, std::shared_ptr<Map>>::iterator it = mMaps.find(map);
  // Map not found, attempt to create it
  if (it == mMaps.end()) {
    std::vector<uint8_t> buffer = gFileAgent->loadAsBuffer(map);
  }
  // it->second will be an existing map, a created map, or nullptr now
  return std::shared_ptr<Map>(it->second);
}
