#include "utils.h"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <filesystem>

namespace ASI
{

  std::tm parseDateTime(const std::string & s)
  {
    std::istringstream ss(s);

    std::tm t = {};
    ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
    if (ss.fail())
    {
      throw std::runtime_error("Failed to parse " + s + " as time");
    }

    return t;
  }

  std::string getFilename(const std::string & folder, const size_t channel, const std::tm & date)
  {
    const std::filesystem::path rootPath(folder);

    std::stringstream filename;
    filename << "CCTV_" << channel << "_";
    filename << std::put_time(&date, "%Y-%m-%d_%H_%M");
    filename << ".mp4";

    const std::filesystem::path filePath = rootPath / filename.str();
    const std::string result = filePath;
    return filePath;
  }

  std::string getLiveFilename(const std::string & folder, const size_t channel)
  {
    const std::filesystem::path rootPath(folder);

    std::stringstream filename;
    filename << "CCTV_" << channel << "_live.mp4";

    const std::filesystem::path filePath = rootPath / filename.str();
    const std::string result = filePath;
    return filePath;
  }

  std::string getEnvVar(const std::string & var)
  {
     const char * val = std::getenv(var.c_str());
     if (val)
     {
         return val;
     }
     else
     {
         return std::string();
     }
  }

}
