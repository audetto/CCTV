#include "dvr.h"

#include <iostream>
#include <sstream>

namespace
{

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

  void Demo_SDK_Version()
  {
    const DWORD uiVersion = NET_DVR_GetSDKBuildVersion();

    std::cout << "HCNetSDK V" << ((0xff000000 & uiVersion) >> 24) << "." << ((0x00ff0000 & uiVersion) >> 16) << "." << ((0x0000ff00 & uiVersion) >> 8) << "." << (0x000000ff & uiVersion) << std::endl;
  }

  void capturePictures(ASI::NET_DVR & dvr)
  {
    NET_DVR_JPEGPARA strPicPara = {0};
    strPicPara.wPicQuality = 2;

    for (size_t i = 0; i < 2; ++i)
    {
      std::ostringstream name;
      name << "channel_" << i << ".jpeg";
      dvr.capturePicture(i, strPicPara, name.str());
    }
  }

  void downloadFiles(ASI::NET_DVR & dvr)
  {
    dvr.downloadFiles(0, "/tmp/test.mp4");
  }

  void live(ASI::NET_DVR & dvr)
  {
    dvr.liveStream(0, 5, "/tmp/live.mp4");
  }

  int cctv()
  {
    ASI::NET_SDK sdk;

    Demo_SDK_Version();
    NET_DVR_SetLogToFile(5, ASI::cast("/tmp/sdkLog"));

    ASI::NET_DVR dvr("192.168.0.20", 8000, getEnvVar("HK_USERNAME"), getEnvVar("HK_PASSWORD"));

    // capturePictures(dvr);
    // downloadFiles(dvr);
    live(dvr);

    return 0;
  }

}

int main()
{
  try
  {
    cctv();
  }
  catch (const std::exception & e)
  {
    std::cerr << e.what() << std::endl;
  }
}
