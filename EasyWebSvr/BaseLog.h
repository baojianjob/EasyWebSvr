// 日志类
// wbj 2005.9.21
// 2005.10.10 增加 EditWndLOG
// 2005.12.29 增加 ListBoxLOG
//            修改FileLOG和EditWndLOG，多线程保护

#ifndef __BASELOG_H__
#define __BASELOG_H__
#pragma once

#include "mtstring.h"
#include "critsect.h"

namespace minilib
{

class BaseLOG
{
public:
  virtual ~BaseLOG() {};

public:
  BOOL          AddLog(const TCHAR *pLogLine, BOOL bAddTime = TRUE);

protected:
  virtual BOOL  ImpAddLog(const TCHAR *pLogStr) = 0;
};

class FileLOG : public BaseLOG
{
public:
  FileLOG();
  FileLOG(const TCHAR *pLogFileName);
  ~FileLOG();

public:
  BOOL          SetLogFileName(const TCHAR *pLogFileName = NULL);

protected:
  virtual BOOL  ImpAddLog(const TCHAR *pLogStr);

protected:
  MtSTRING      LogFileName;
  FILE         *pFile;

#ifdef _MT
  CCriticalSect  Cs;
#endif // _MT
};

class EditWndLOG : public BaseLOG
{
public:
  EditWndLOG();
  EditWndLOG(HWND hEdit, int MaxTextLen = 0);

public:
  void SetEditWnd(HWND hEdit, int MaxTextLen = 0);

protected:
  virtual BOOL  ImpAddLog(const TCHAR *pLogStr);

protected:
  HWND          hWndEdit;
  int           MaxTextLength;

#ifdef _MT
  CCriticalSect  Cs;
#endif // _MT
};

class ListBoxLOG : public BaseLOG
{
public:
  ListBoxLOG();
  ListBoxLOG(HWND hListBox, int MaxRow = 0);
  ~ListBoxLOG() {};

public:
  void          SetListBoxWnd(HWND hListBox, int MaxRow = 0);
  HWND          GetListBoxWnd();

protected:
  virtual BOOL  ImpAddLog(const TCHAR *pLogStr);

protected:
  HWND          hWndListBox;
  int           MaxRowCount;
#ifdef _MT
  CCriticalSect  Cs;
#endif // _MT
};

inline HWND ListBoxLOG::GetListBoxWnd()
{
  return hWndListBox;
}

}

#endif // __BASELOG_H__