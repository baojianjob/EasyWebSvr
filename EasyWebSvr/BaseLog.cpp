#include "stdafx.h"
#include "baselog.h"

namespace minilib
{

BOOL BaseLOG::AddLog(const TCHAR *pLogLine, BOOL bAddTime)
{
  MtSTRING LogStr;

  if(bAddTime)
  {
    SYSTEMTIME Time;
    ::GetLocalTime(&Time);

    LogStr.Format(_T("[%04d-%02d-%02d %02d:%02d:%02d] %s"), Time.wYear, Time.wMonth, 
      Time.wDay, Time.wHour, Time.wMinute, Time.wSecond, pLogLine);
    return ImpAddLog(LogStr);
  }
  else
  {
    return ImpAddLog(pLogLine);
  }
}

///////////////////////////////////////////////////////////////////////////////

FileLOG::FileLOG()
{
  pFile = NULL;
}

FileLOG::FileLOG(const TCHAR *pLogFileName)
{
  pFile = NULL;
  SetLogFileName(pLogFileName);
}

FileLOG::~FileLOG()
{
  if(pFile != NULL)
    fclose(pFile);
}

BOOL FileLOG::SetLogFileName(const TCHAR *pLogFileName)
{
#ifdef _MT
    CritSectOwner Guard(Cs);
#endif // _MT

  if(pLogFileName && *pLogFileName)
  {
    LogFileName = pLogFileName;
  }
  else
  {
    TCHAR *pBuf = LogFileName.GetBuffer(300);
    ::GetModuleFileName(NULL, pBuf, 300);
    
    TCHAR *pFind = _tcsrchr(pBuf, _T('.'));
    if(pFind == NULL)
      pFind = pBuf + _tcslen(pBuf);
    _tcscpy(pFind, _T(".log"));

    LogFileName.ReleaseBuffer();
  }

  if(pFile != NULL)
    fclose(pFile);
  pFile = _tfopen(LogFileName, _T("ab"));
  return pFile != NULL;
}

BOOL FileLOG::ImpAddLog(const TCHAR *pLogStr)
{
#ifdef _MT
  CritSectOwner Guard(Cs);
#endif // _MT

  assert(pFile != NULL);
  if(pFile == NULL)
    return FALSE;

  fwrite(pLogStr, sizeof(TCHAR), _tcslen(pLogStr), pFile);
  fwrite(_T("\r\n"), sizeof(TCHAR), 2, pFile);
  fflush(pFile);

  return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

EditWndLOG::EditWndLOG() 
{ 
  hWndEdit      = NULL; 
  MaxTextLength = 0;
}

EditWndLOG::EditWndLOG(HWND hEdit, int MaxTextLen)
{
  hWndEdit      = hEdit;
  MaxTextLength = MaxTextLen;
}

void EditWndLOG::SetEditWnd(HWND hEdit, int MaxTextLen)
{
#ifdef _MT
  CritSectOwner Guard(Cs);
#endif // _MT

  hWndEdit = hEdit;
  MaxTextLength = MaxTextLen;
}

BOOL EditWndLOG::ImpAddLog(const TCHAR *pLogStr)
{
  assert(::IsWindow(hWndEdit));

#ifdef _MT
  CritSectOwner Guard(Cs);
#endif // _MT

  int Length = (int)SendMessage(hWndEdit, WM_GETTEXTLENGTH, 0, 0); 
  if(MaxTextLength > 0 && Length >= MaxTextLength)
  {
    ::SetWindowText(hWndEdit, _T(""));
    Length = ::GetWindowTextLength(hWndEdit); 
  }
  ::SendMessage(hWndEdit, EM_SETSEL, Length, Length);
  ::SendMessage(hWndEdit, EM_REPLACESEL, FALSE, (LPARAM)pLogStr);

  Length = (int)SendMessage(hWndEdit, WM_GETTEXTLENGTH, 0, 0); 
  ::SendMessage(hWndEdit, EM_SETSEL, Length, Length);
  ::SendMessage(hWndEdit, EM_REPLACESEL, FALSE, (LPARAM)_T("\r\n"));

  return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

ListBoxLOG::ListBoxLOG() 
{ 
  hWndListBox = NULL; 
  MaxRowCount = 0;
}

ListBoxLOG::ListBoxLOG(HWND hListBox, int MaxRow)
{
  hWndListBox = hListBox;
  MaxRowCount = MaxRow;
}

void ListBoxLOG::SetListBoxWnd(HWND hListBox, int MaxRow)
{
  hWndListBox = hListBox;
  MaxRowCount = MaxRow;
}

BOOL ListBoxLOG::ImpAddLog(const TCHAR *pLogStr)
{
  assert(::IsWindow(hWndListBox));

#ifdef _MT
  CritSectOwner Guard(Cs);
#endif // _MT

  LOG_INFO(GID_SERVERLOG, "%s", pLogStr);

  if(MaxRowCount > 0)
  {
    int RowCount = (int)::SendMessage(hWndListBox, LB_GETCOUNT, 0, 0);
    if(RowCount >= MaxRowCount)
      ::SendMessage(hWndListBox, LB_RESETCONTENT, 0, 0);
  }

  return (int)::SendMessage(hWndListBox, LB_ADDSTRING, 0, (LPARAM)pLogStr) >= 0;
}

}