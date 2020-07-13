#pragma once

#include "mtstring.h"

using namespace minilib;

class CResponseHeader
{
public:
  void          AddStatusCode(int StatusCode);
  void          AddStatus(const TCHAR *pStatusString);
  void          AcceptRanges();
  void          AddLastModified(const SYSTEMTIME &T);
  void          AddServer(const TCHAR *pServerName);
  void          AddConnection(BOOL bKeepAlive);
  void          AddContentLength(unsigned __int64 ContentLength);
  void          AddContentType(const TCHAR *pContentType);
  void          AddLocation(const TCHAR *pUrl);
  void          AddContentRange(unsigned __int64 Begin, unsigned __int64 End, unsigned __int64 Total);
  void          AddContentRange(unsigned __int64 Total);
  void          AddExtraHeaders(const TCHAR *pExtraHeader);

  //const MtSTRING &GetResponseHeader();

public:
  MtSTRING      ResponseHeader;
};

inline void CResponseHeader::AddExtraHeaders(const TCHAR *pExtraHeader)
{
    ResponseHeader += pExtraHeader;
}