//***********************************************
// 文件名称：strlist.cpp
// 功　　能：字符串链表
// 作　　者：wbj
// 创建日期：2002-12-22
// 更新日期：2003-11-26
//***********************************************

// wbj 2006.1.20
// add: FromStringToList(const TCHAR *pString, const TCHAR *pSeparator)

#include "stdafx.h"
#include "strlist.h"

namespace minilib
{

// 把字符串转换成字符串链表, 以 pSeparator 中的字符做分隔
void StrLIST::FromStringToList(const TCHAR *pString, const TCHAR *pSeparator)
{
  if(pString == NULL)
    return;

  RemoveAll();

  TCHAR *pTempStr = _tcsdup(pString);
  TCHAR *pToken = _tcstok(pTempStr, pSeparator);
  while(pToken != NULL)
  {
    AddTail(MtSTRING(pToken));
    pToken = _tcstok(NULL, pSeparator);
  }

  free(pTempStr);
}

// 把字符串转换成字符串链表, 以 \r\n 做分隔
void StrLIST::FromStringToList(const TCHAR *pString)
{
  if(pString == NULL)
    return;

  RemoveAll();

  MtSTRING RowStr;
  const TCHAR *pStr = pString;
  while(*pStr)
  {
    int MemLen = 200;   // 每一行的初始内存大小(最小值可设为1)
    TCHAR *pRowStr = RowStr.GetBuffer(MemLen); 
    TCHAR *pDest = pRowStr;
    for(int RowLen = 0; *pStr != '\0' && *pStr != '\r' && *pStr != '\n'; pDest++, pStr++)
    {
      *pDest = *pStr;
      RowLen++;
      if(RowLen >= MemLen - 1)
      {
        MemLen += 200;  // 一行内存不足时, 每次多分配200个(最小值可设为1)
        RowStr.ReleaseBuffer();
        pRowStr = RowStr.GetBuffer(MemLen);
        pDest = pRowStr + RowLen - 1;
      }
    }
    *pDest = '\0';

    RowStr.ReleaseBuffer();
    //RowStr.TrimRight();

#ifdef TAB_TO_SPACE  // 把 tab 转换成 8 个空格
    while(1)
    {
      int Index = RowStr.Find('\t');
      if(Index == -1)
        break;
      RowStr.Replace(Index, 1, "        ");
    }    
#endif

    AddTail(RowStr);

    if(*pStr == '\r')
      pStr++;
    if(*pStr == '\n')
      pStr++;
  }
}

// 把字符串链表转换成字符串, 以 pSeparator 做分隔
void StrLIST::FromListToString(MtSTRING &Str, const TCHAR *pSeparator) const
{
  int Length = 0, SepLength = lstrlen(pSeparator);
  StrListPOS ListPos = GetHeadPosition();
  for(; ListPos != NULL; ListPos = ListPos->GetNext())
    Length += ListPos->Data.GetLength() + SepLength;

  TCHAR *pBuffer = Str.GetBuffer(Length + 1);

  *pBuffer = 0;
  ListPos = GetHeadPosition();
  while(ListPos != NULL)
  {
    const TCHAR *pStr = (LPCTSTR)ListPos->Data;
    for(; *pStr != '\0'; pBuffer++, pStr++)  // 复制正常串
      *pBuffer = *pStr;

    ListPos = ListPos->GetNext();
    if(ListPos == NULL)
      break;

    const TCHAR *pSep = pSeparator;
    for(; *pSep != '\0'; pBuffer++, pSep++)  // 复制分隔符
      *pBuffer = *pSep;
  }
  *pBuffer = '\0';
  Str.ReleaseBuffer();
}

// 读取文件到字符串链表
bool StrLIST::LoadFromFile(const TCHAR *pFileName)
{
  assert(pFileName != NULL);
  HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if(hFile == INVALID_HANDLE_VALUE)
    return false;

  DWORD FileLength = GetFileSize(hFile, NULL); 
  TCHAR *pBuffer = (TCHAR *)malloc(FileLength + sizeof(TCHAR));
  
  ::ReadFile(hFile, pBuffer, FileLength, &FileLength, NULL);
  ::CloseHandle(hFile);
  pBuffer[FileLength] = '\0' ;
  
  FromStringToList(pBuffer);

  free(pBuffer); 

  return true;
}

// 把字符串链表的内容保存到文件
bool StrLIST::SaveToFile(const TCHAR *pFileName) const
{
  assert(pFileName != NULL);
  HANDLE hFile = ::CreateFile(pFileName, GENERIC_WRITE, 0, 
                              NULL, CREATE_ALWAYS, 0, NULL);
  if(hFile == INVALID_HANDLE_VALUE)
    return false;

  StrListPOS Pos = GetHeadPosition();
  while(Pos)
  {
    DWORD FileLength = (Pos->Data.GetLength() + 2) * sizeof(TCHAR);
    if(!::WriteFile(hFile, (LPCTSTR)(Pos->Data + _T("\r\n")), FileLength, &FileLength, NULL))
      return false;
    Pos = Pos->GetNext();
  }
  ::CloseHandle(hFile);

  return true;
}

// 删除链表中所有的空字符串节点
void StrLIST::RemoveNullString()
{
  StrListPOS ListPos = GetHeadPosition();
  while(ListPos != NULL)
  {
    if(ListPos->Data.IsEmpty())
    {
      RemoveAt(ListPos, 1);
      if(ListPos == NULL)
        break;
    }
    else
      ListPos = ListPos->GetNext();
  }
}

// 自动换行
// 暂只支持单行字符串的自动换行
// 注意：对 Unicode 版本可能有问题，有待改进
void StrLIST::AutoBreakLineStr(const TCHAR *pString, int MaxLineLength)
{
  assert(pString != NULL && MaxLineLength >= 2);
  assert(_tcschr(pString, _T('\r')) == NULL && _tcschr(pString, _T('\n')) == NULL);

  bool bInHan = (BYTE)(*pString) > 127;  // 避免半个汉字问题
  const TCHAR *pStartPos = pString;
  for(; *pString != '\0'; pString++)
  {
    if((BYTE)(*(pString + 1)) > 127)
      bInHan = !bInHan;

    if(pString - pStartPos >= MaxLineLength - 1 ||
      bInHan && pString - pStartPos >= MaxLineLength - 2 ||
      *(pString + 1) == '\0')
    {
      MtSTRING RowStr;
      TCHAR *pDest = RowStr.GetBuffer(MaxLineLength + 1);
      memcpy(pDest, pStartPos, pString - pStartPos + 1);
      pDest[pString - pStartPos + 1] = 0;
      RowStr.ReleaseBuffer();

      AddTail(RowStr);
      pStartPos = pString +1;
    }
  }
}

}