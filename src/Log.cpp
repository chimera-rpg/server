/* ================================================================
Log
----------------
This file defines the class for Log.
================================================================ */
#include "Log.hpp"
#include <stdexcept>

/*void *logContexts[4] = { nullptr, nullptr, nullptr, nullptr };

void (*gLogHooks[4])(void *, const char*, const char*) = { nullptr, nullptr, nullptr, nullptr };*/

std::map<LogLevel, std::function<void(const std::string, const std::string)>> gLogHooks;

Log::Log() {}

std::ostringstream& Log::Get(LogLevel level) {
  l = level;
  time_t current_time = time(0);
  struct tm *ltm;
#ifdef _MSC_VER
  struct tm lltm;
  localtime_s(&lltm, &current_time);
  ltm = &lltm;
#else
  ltm = localtime(&current_time);
#endif
  os << ltm->tm_hour << ":" << ltm->tm_min << ':' << ltm->tm_sec << " ";
  //os << 1900+ltm->tm_year << "/" << 1+ltm->tm_mon << "/" << ltm->tm_mday << " " << ltm->tm_hour << ":" << ltm->tm_min << ':' << ltm->tm_sec << " ";
  return os;
}
Log::~Log() {
  os << std::endl;
  char const *title = "Undefined";
  switch(l) {
  case LOG_INFO:
    title = "I";
    break;
  case LOG_ERROR:
    title = "E";
    break;
  case LOG_FAIL:
    title = "F";
    break;
  case LOG_WARNING:
    title = "W";
    break;
  case LOG_DEBUG:
    title = "D";
    break;
  }
  if (gLogHooks.count((LogLevel)l) != 0) {
    gLogHooks[(LogLevel)l](title, os.str());
  } else {
    fprintf(stderr, "%s: %s\n", title, os.str().c_str());
    fflush(stderr);
  }
}
