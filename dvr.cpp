#include "dvr.h"

#include "unistd.h"

#include <cstring>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <chrono>
#include <thread>

namespace
{
  void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
  {
    std::cerr << "Got some exception: " << dwType << " for user: " << lUserID << " and handle: " << lHandle << std::endl;
  }

  void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* dwUser)
  {
    std::cout << "Got " << dwBufSize << " bytes: handle = " << lRealHandle << ", type = " << dwDataType << std::endl;
    std::ofstream * out = static_cast<std::ofstream *>(dwUser);

    if (dwDataType == NET_DVR_STREAMDATA)
    {
      const char * buf = reinterpret_cast<char *>(pBuffer);
      out->write(buf, dwBufSize);
    }
  }

}

namespace ASI
{

  NET_DVR::NET_DVR(const char *ip, const WORD port, const std::string & username, const std::string & password) : myUserID(0), myDeviceInfo({0})
  {
    NET_DVR_USER_LOGIN_INFO loginInfo = {};
    myDeviceInfo = {};
    loginInfo.bUseAsynLogin = FALSE;

    loginInfo.wPort = port;
    strncpy(loginInfo.sDeviceAddress, ip, sizeof(loginInfo.sDeviceAddress) - 1);
    strncpy(loginInfo.sUserName, username.c_str(), sizeof(loginInfo.sUserName) - 1);
    strncpy(loginInfo.sPassword, password.c_str(), sizeof(loginInfo.sPassword) - 1);

    myUserID = NET_DVR_Login_V40(&loginInfo, &myDeviceInfo);

    if (myUserID < 0)
    {
      error("Login error");
    }

    const int firstChannel = myDeviceInfo.struDeviceV30.byStartDChan;
    const int numberOfChannels = myDeviceInfo.struDeviceV30.byIPChanNum + myDeviceInfo.struDeviceV30.byHighDChanNum * 256;
    std::cout << "First digital channel: " << firstChannel << std::endl;
    std::cout << "Number of digital channels: " << numberOfChannels << std::endl;
  }

  NET_DVR::~NET_DVR()
  {
    const BOOL ok = NET_DVR_Logout(myUserID);
    myUserID = 0;
    if (!ok)
    {
      debug("NET_DVR_Logout");
    }
  }

  [[ noreturn ]] void NET_DVR::error(const char * msg) const
  {
    LONG err = NET_DVR_GetLastError();
    std::ostringstream ss;

    ss << msg << ": " << err;
    ss << " = " << NET_DVR_GetErrorMsg(&err);
    throw std::runtime_error(ss.str());
    // std::cerr << ss.str() << std::endl;
  }

  void NET_DVR::debug(const char * msg) const
  {
    LONG err = NET_DVR_GetLastError();
    std::ostringstream ss;

    ss << msg << ": " << err;
    ss << " = " << NET_DVR_GetErrorMsg(&err);
    std::cerr << ss.str() << std::endl;
  }

  const NET_DVR_DEVICEINFO_V40 & NET_DVR::getDeviceInfo() const
  {
    return myDeviceInfo;
  }

  void NET_DVR::capturePicture(LONG channel, NET_DVR_JPEGPARA & parameters, const std::string & filename) const
  {
    const LONG dChannel = myDeviceInfo.struDeviceV30.byStartDChan + channel;
#if 1
    const BOOL ok = NET_DVR_CaptureJPEGPicture(myUserID, dChannel, &parameters, cast(filename.c_str()));
#else
    std::vector<char> buffer(100000);
    DWORD size = 0;
    const BOOL ok = NET_DVR_CaptureJPEGPicture_NEW(myUserID, dChannel, &parameters, buffer.data(), buffer.size(), &size);
#endif

    if (!ok)
    {
      error("NET_DVR_CaptureJPEGPicture");
    }
    else
    {
      std::cout << channel << " to " << filename << std::endl;
    }
  }

  void NET_DVR::downloadFiles(const LONG channel, const std::string & filename) const
  {
    const LONG dChannel = myDeviceInfo.struDeviceV30.byStartDChan + channel;

    NET_DVR_PLAYCOND downloadCond = {};
    downloadCond.dwChannel = dChannel;
    
    downloadCond.struStartTime.dwYear   = 2020;
    downloadCond.struStartTime.dwMonth  = 6;
    downloadCond.struStartTime.dwDay    = 1;
    downloadCond.struStartTime.dwHour   = 9;
    downloadCond.struStartTime.dwMinute = 50;
    downloadCond.struStartTime.dwSecond = 0;
    downloadCond.struStopTime.dwYear    = 2020;
    downloadCond.struStopTime.dwMonth   = 6;
    downloadCond.struStopTime.dwDay     = 1;
    downloadCond.struStopTime.dwHour    = 9;
    downloadCond.struStopTime.dwMinute  = 59;
    downloadCond.struStopTime.dwSecond  = 0;
    
    const LONG hPlayback = NET_DVR_GetFileByTime_V40(myUserID, cast(filename.c_str()), &downloadCond);
    
    if (hPlayback < 0)
    {
        error("NET_DVR_GetFileByTime_V40");
    }

    if (!NET_DVR_PlayBackControl_V40(hPlayback, NET_DVR_PLAYSTART, NULL, 0, NULL, NULL))
    {
        error("NET_DVR_PlayBackControl_V40");
    }

    LONG pos = 0;
    for (pos = 0; pos < 100 && pos >= 0; pos = NET_DVR_GetDownloadPos(hPlayback))
    {
        std::cout << "Downloading... " << pos << " %" << std::endl;
        sleep(1);
    }
    
    if (!NET_DVR_StopGetFile(hPlayback))
    {
        error("NET_DVR_StopGetFile");
    }

    std::cout << "Downloaded... " << pos << " %" << std::endl;
  }

  void NET_DVR::liveStream(const LONG channel, const int seconds, const std::string & filename) const
  {
    const LONG dChannel = myDeviceInfo.struDeviceV30.byStartDChan + channel;

    NET_DVR_PREVIEWINFO struPlayInfo = {};
    struPlayInfo.hPlayWnd     = 0;
    struPlayInfo.lChannel     = dChannel;
    struPlayInfo.dwStreamType = 0;       //0-Main Stream, 1-Sub Stream, 2-Stream 3, 3-Stream 4, and so on
    struPlayInfo.dwLinkMode   = 0;       //0- TCP Mode, 1- UDP Mode, 2- Multicast Mode, 3- RTP Mode, 4-RTP/RTSP, 5-RSTP/HTTP
    struPlayInfo.bBlocked     = 1;       //0- Non-blocking Streaming, 1- Blocking Streaming

    std::ofstream out(filename.c_str());

    const LONG realPlayHandle = NET_DVR_RealPlay_V40(myUserID, &struPlayInfo, g_RealDataCallBack_V30, &out);
    if (realPlayHandle < 0)
    {
        error("NET_DVR_RealPlay_V40");
    }

    std::this_thread::sleep_for(std::chrono::seconds(seconds));

    NET_DVR_StopRealPlay(realPlayHandle);

    out.flush();
  }

  NET_SDK::NET_SDK()
  {
    NET_DVR_Init();
    NET_DVR_SetExceptionCallBack_V30(0, NULL, g_ExceptionCallBack, NULL);
  }

  NET_SDK::~NET_SDK()
  {
    NET_DVR_Cleanup();
  }

}
