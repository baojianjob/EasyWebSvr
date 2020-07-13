#include "stdafx.h"
#include "resphdr.h"

void CResponseHeader::AddStatusCode(int StatusCode)
{
    assert(false); //------
}

void CResponseHeader::AddStatus(const TCHAR *pStatusString)
{
    assert(ResponseHeader.IsEmpty());
    ResponseHeader.Format(_T("HTTP/1.1 %s\r\n"), pStatusString);
}

void CResponseHeader::AddServer(const TCHAR *pServerName)
{
    assert(!ResponseHeader.IsEmpty());

    MtSTRING LineStr;
    LineStr.Format(_T("Server: %s\r\n"), pServerName);
    ResponseHeader += LineStr;
}

void CResponseHeader::AcceptRanges()
{
    assert(!ResponseHeader.IsEmpty());

    ResponseHeader += _T("Accept-Ranges: bytes\r\n");
}

void CResponseHeader::AddContentLength(unsigned __int64 ContentLength)
{
    assert(!ResponseHeader.IsEmpty());

    MtSTRING LineStr;
    LineStr.Format(_T("Content-Length: %I64u\r\n"), ContentLength);
    ResponseHeader += LineStr;
}

void CResponseHeader::AddContentType(const TCHAR *pContentType)
{
    assert(!ResponseHeader.IsEmpty());

    MtSTRING LineStr;
    LineStr.Format(_T("Content-Type: %s\r\n"), pContentType);
    ResponseHeader += LineStr;
}

void CResponseHeader::AddLocation(const TCHAR *pUrl)
{
    assert(!ResponseHeader.IsEmpty());

    MtSTRING LineStr;
    LineStr.Format(_T("Location: %s\r\n"), pUrl);
    ResponseHeader += LineStr;
}

void CResponseHeader::AddConnection(BOOL bKeepAlive)
{
    assert(!ResponseHeader.IsEmpty());

    MtSTRING LineStr;
    LineStr.Format(_T("Connection: %s\r\n"), bKeepAlive ? _T("Keep-Alive") : _T("close"));
    ResponseHeader += LineStr;
}

void CResponseHeader::AddContentRange(unsigned __int64 Begin, unsigned __int64 End, unsigned __int64 Total)
{
    assert(!ResponseHeader.IsEmpty());

    MtSTRING LineStr;
    LineStr.Format(_T("Content-Range: bytes %I64u-%I64u/%I64u\r\n"), Begin, End, Total);
    ResponseHeader += LineStr;
}

void CResponseHeader::AddContentRange(unsigned __int64 Total)
{
    assert(!ResponseHeader.IsEmpty());

    MtSTRING LineStr;
    LineStr.Format(_T("Content-Range: bytes */%I64u\r\n"), Total);
    ResponseHeader += LineStr;
}

/*
void CResponseHeader::AddCurrentTime()
{
  char szDT[128];

  long ltime;
  time(&ltime);
  struct tm *newtime = gmtime(&ltime);
  strftime(szDT, 128,
    "Date: %a, %d %b %Y %H:%M:%S GMT\r\n", newtime);

  ResponseHeader += szDT;
}
*/
void CResponseHeader::AddLastModified(const SYSTEMTIME &T)
{
    static TCHAR *MonthStr[] =  
    {
        _T(""),
        _T("Jan"),
        _T("Feb"),
        _T("Mar"), 
        _T("Apr"),
        _T("May"),
        _T("Jun"),
        _T("Jul"),
        _T("Aug"),
        _T("Sep"),
        _T("Oct"),
        _T("Nov"),
        _T("Dec"),
    };

    static TCHAR *DayStr[] =
    {
        _T("Sun"),
        _T("Mon"),
        _T("Tue"), 
        _T("Wed"),
        _T("Thu"),
        _T("Fri"),
        _T("Sat"),
    };

    assert(!ResponseHeader.IsEmpty());

    MtSTRING LineStr;
    LineStr.Format(_T("Last-Modified: %s, %02d %s %04d %02d:%02d:%02d GMT\r\n"), DayStr[T.wDayOfWeek], 
        T.wDay, MonthStr[T.wMonth], T.wYear, T.wHour, T.wMinute, T.wSecond);
    ResponseHeader += LineStr;
}

