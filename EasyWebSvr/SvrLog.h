#pragma once

#include "baselog.h"

using namespace minilib;

class CWebServerLog : public BaseLOG
{
public:
  CWebServerLog();

public:
  void          Init(BOOL bLogToWnd, BOOL bLogToFile, HWND hWndList, const TCHAR *pLogFileName = NULL);
  void          SetListBoxMaxTextLen(int Len);

protected:
  virtual BOOL  ImpAddLog(const TCHAR *pLogStr);

public:
  BOOL          bLogToWnd;
  BOOL          bLogToFile;
  FileLOG       FileLog;
  ListBoxLOG    ListBoxLog;
  int           MaxTextLen;
};