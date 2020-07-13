#include "stdafx.h"
#include "svrlog.h"

CWebServerLog::CWebServerLog()
{
  bLogToWnd  = FALSE;
  bLogToFile = FALSE;
  MaxTextLen = 0;
}

void CWebServerLog::Init(BOOL bLogToWnd, BOOL bLogToFile, HWND hWndList, const TCHAR *pLogFileName)
{
  this->bLogToWnd = bLogToWnd;
  this->bLogToFile = bLogToFile;
  ListBoxLog.SetListBoxWnd(hWndList);
  FileLog.SetLogFileName(pLogFileName);
}

void CWebServerLog::SetListBoxMaxTextLen(int Len)
{
  if(Len > MaxTextLen)
  {
    MaxTextLen = Len;
    ::SendMessage(ListBoxLog.GetListBoxWnd(), LB_SETHORIZONTALEXTENT, (MaxTextLen + 22) * 6 + 10, 0);
  }
}

BOOL CWebServerLog::ImpAddLog(const TCHAR *pLogStr)
{
  if(bLogToWnd && !ListBoxLog.AddLog(pLogStr, FALSE))
    return FALSE;
  if(bLogToFile && !FileLog.AddLog(pLogStr, FALSE))
    return FALSE;
  return TRUE;
}
