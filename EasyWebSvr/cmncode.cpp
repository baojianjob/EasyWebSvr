#include "stdafx.h"
#include "cmncode.h"

#ifndef INVALID_FILE_ATTRIBUTES
  #define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

// 判断一个文件是否存在
BOOL IsFileExist(const TCHAR *pFileName)
{
  DWORD Attr = GetFileAttributes(pFileName);
  if(Attr == INVALID_FILE_ATTRIBUTES)
    return FALSE;
  return (Attr & FILE_ATTRIBUTE_DIRECTORY) ? FALSE : TRUE;
}

// 判断一个目录是否存在
BOOL IsDirectoryExist(const TCHAR *pDirName)  
{
  DWORD Attr = GetFileAttributes(pDirName);
  if(Attr == INVALID_FILE_ATTRIBUTES)
    return FALSE;
  return (Attr & FILE_ATTRIBUTE_DIRECTORY) ? TRUE : FALSE;
}
