// 打开/保存文件对话框
// wbj 2002.12.10
// wbj 2002.6.12

#include "stdafx.h"
#include "filedlg.h"

namespace minilib
{

CFileDlg::CFileDlg(bool bOpenFile,  // TRUE for file open, FALSE for file save
  HWND hParentWnd, const TCHAR *pFilter, const TCHAR *pDefExt, DWORD FilterIndex, 
  const TCHAR *pInitFileName, DWORD Flags)
{
  bOpenFileDlg = bOpenFile;
  lstrcpyn(FilePathName, pInitFileName, sizeof(FilePathName) - 1);

  ZeroMemory(&Ofn, sizeof(OPENFILENAME));
  Ofn.lStructSize       = sizeof (OPENFILENAME);
  Ofn.hwndOwner         = hParentWnd;
  Ofn.lpstrFile         = FilePathName;
  Ofn.nMaxFile          = sizeof(FilePathName);

  Ofn.lpstrFilter       = pFilter; 
  Ofn.nFilterIndex      = FilterIndex;
  Ofn.lpstrFileTitle    = FileTitle;
  Ofn.lpstrDefExt       = pDefExt;
  Ofn.nMaxFileTitle     = sizeof(FileTitle);

  Ofn.lpstrInitialDir   = NULL; 

  if(bOpenFile)
    Ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | Flags;
  else
    Ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | Flags;
}

CFileDlg::~CFileDlg()
{
}

bool CFileDlg::DoModal()
{
  BOOL bRet;
  bRet = (bOpenFileDlg ? GetOpenFileName(&Ofn) : GetSaveFileName(&Ofn));
  return (bRet ? true : false);
}

const TCHAR *CFileDlg::GetFileName() const
{
  return Ofn.lpstrFileTitle;
}

const TCHAR *CFileDlg::GetPathFileName() const
{
  return Ofn.lpstrFile;
}

}