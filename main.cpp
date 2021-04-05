#include "dvr.h"

#include "options.h"
#include "utils.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace
{

  void capturePictures(const ASI::NET_DVR & dvr, const ASI::ProgramOptions & options)
  {
    for (const size_t channel: options.channels)
    {
      const std::string filename = ASI::getPictureFilename(options.folder, ".jpeg", channel);
      dvr.capturePicture(channel, filename);
      std::cout << "Picture written to " << filename << std::endl;
    }
  }

  void downloadFiles(const ASI::NET_DVR & dvr, const ASI::ProgramOptions & options)
  {
    const std::tm start = ASI::parseDateTime(options.start);
    const std::tm end = ASI::parseDateTime(options.end);

    for (const size_t channel: options.channels)
    {
      const std::string filename = ASI::getFilename(options.folder, ".mp4", channel, start);
      dvr.downloadFiles(channel, start, end, filename);
      std::cout << "Video written to " << filename << std::endl;
    }
  }

  void live(const ASI::NET_DVR & dvr, const ASI::ProgramOptions & options)
  {
    for (const size_t channel: options.channels)
    {
      const std::string filename = ASI::getLiveFilename(options.folder, ".dat", channel);
      dvr.liveStream(channel, options.duration, filename);
      std::cout << "Video written to " << filename << std::endl;
    }
  }

  void info(ASI::NET_DVR & dvr)
  {
    const DWORD uiVersion = NET_DVR_GetSDKBuildVersion();

    std::cout << "HCNetSDK V" << ((0xff000000 & uiVersion) >> 24) << "." << ((0x00ff0000 & uiVersion) >> 16) << "." << ((0x0000ff00 & uiVersion) >> 8) << "." << (0x000000ff & uiVersion) << std::endl;

    dvr.info();
  }

  int cctv(int argc, char ** argv)
  {
    ASI::ProgramOptions options;
    const bool run = ASI::getProgramOptions(argc, argv, options);

    if (run)
    {
      ASI::NET_SDK sdk;

      if (!options.logfile.empty())
      {
        NET_DVR_SetLogToFile(options.loglevel, ASI::cast(options.logfile));
      }

      ASI::NET_DVR dvr("192.168.0.20", 8000, ASI::getEnvVar("HK_USERNAME"), ASI::getEnvVar("HK_PASSWORD"));

      switch (options.target) {
      case ASI::Live:
        live(dvr, options);
        break;
      case ASI::Replay:
        downloadFiles(dvr, options);
        break;
      case ASI::Pictures:
        capturePictures(dvr, options);
        break;
      default:
        info(dvr);
        break;
      };
    }
    return 0;
  }

}

int main(int argc, char ** argv)
{
  try
  {
    return cctv(argc, argv);
  }
  catch (const std::exception & e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
