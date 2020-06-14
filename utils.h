#pragma once

#include <string>
#include <ctime>

namespace ASI
{

  inline char * cast(const char * c)
  {
    return const_cast<char*>(c);
  }

  inline char * cast(const std::string & s)
  {
    return cast(s.c_str());
  }

  // Format: "%Y-%m-%d %H:%M:%S"
  std::tm parseDateTime(const std::string & s);

  std::string getEnvVar(const std::string & var);

  std::string getFilename(const std::string & folder, const size_t channel, const std::tm & date);
  std::string getLiveFilename(const std::string & folder, const size_t channel);

}
