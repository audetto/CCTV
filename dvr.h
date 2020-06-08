#pragma once

#include "HCNetSDK.h"

#include <string>

namespace ASI
{

  template <typename T>
  T * cast(const T * c)
  {
    return const_cast<T*>(c);
  }

  class NET_SDK
  {
  public:
    NET_SDK();

    ~NET_SDK();
  };

  class NET_DVR
  {
  public:
    NET_DVR(const char *ip, const WORD port, const std::string & username, const std::string & password);

    const NET_DVR_DEVICEINFO_V40 & getDeviceInfo() const;

    void capturePicture(const LONG channel, NET_DVR_JPEGPARA & parameters, const std::string & filename) const;
    void downloadFiles(const LONG channel, const std::string & filename) const;
    void liveStream(const LONG channel, const int seconds, const std::string & filename) const;

    ~NET_DVR();

  private:

    [[ noreturn ]] void error(const char * msg) const;
    void debug(const char * msg) const;

    LONG myUserID;
    NET_DVR_DEVICEINFO_V40 myDeviceInfo;
  };
}