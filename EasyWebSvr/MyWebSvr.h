#pragma once

#include "websvr.h"
#include "svrlog.h"

class CMyWebServer : public CWebServer
{
public:
  CWebServerLog  SvrLog;

public:
  CMyWebServer();

public:
  BOOL          Init(const CWebServerConfig &Config);

protected:
  virtual void  OnRequestLog(const SOCKADDR_IN &ClientAddr, const CRequestData &RequestData, 
                             BOOL bExeCgi, BOOL bSuccess);
};