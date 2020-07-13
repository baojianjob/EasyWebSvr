#include "stdafx.h"
#include "baseio.h"

//***********************************************
// 文件名称：baseio.cpp
// 功　　能：数据流输入/输出
// 作　　者：wbj
// 创建日期：2003-6-6
// 更新日期：2003-11-12
//***********************************************

/*
History:
  2003-6-6
    create 
  2003-6-17
    Add function ReadValue, WriteValue: MtSTRING
  2003-6-24
    Add function ReadValue, WriteValue: BYTE, bool, int, __int64, WORD, DWORD
  2003-9-2
    Add function ReadValue, WriteValue: BINARY
  2003-9-24
    Add template ReadValue, WriteValue
  2003-11-12
    Modify ReadValue(MtSTRING &StrValue), Set MAX_IO_STRLEN
*/

#define MAX_IO_STRLEN   (1024 * 128)

namespace minilib
{

#ifdef __MTSTRING_H__

bool BaseIO::ReadValue(MtSTRING &StrValue)
{
  int Len;
  if(!ReadValue(Len))
    return false;

  assert(Len >= 0 && Len <= MAX_IO_STRLEN);
  if(Len < 0 || Len > MAX_IO_STRLEN)
    return false;

  TCHAR *pBuf = StrValue.GetBuffer(Len + 1);
  if(pBuf == NULL)
    assert(pBuf != NULL);
  int ReadSize = Len > 0 ? Read(pBuf, Len * sizeof(TCHAR)) : 0;
  pBuf[Len] = 0;
  StrValue.ReleaseBuffer();
  return ReadSize == Len * sizeof(TCHAR);
}

bool BaseIO::WriteValue(const MtSTRING &StrValue)
{
  int Len = StrValue.GetLength();
  if(!WriteValue(Len))
    return false;

  return Write((LPCTSTR)StrValue, Len * sizeof(TCHAR)) == Len * sizeof(TCHAR);
}

#endif // __MTSTRING_H__

#ifdef __MTBINARY_H__

bool BaseIO::ReadValue(BINARY &BinValue)
{
  int Len;
  if(!ReadValue(Len))
    return false;

  assert(Len >= 0);
  BinValue.Resize(Len);

  return Read(BinValue.Ptr, Len) == Len;
}

bool BaseIO::WriteValue(const BINARY &BinValue)
{
  int Len = BinValue.Size;
  if(!WriteValue(Len))
    return false;

  return Write(BinValue.Ptr, Len) == Len;
}

#endif // __MTBINARY_H__

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

MemFILE::MemFILE()
{
  pValue        = NULL;
  Size          = 0;
  Pos           = -1;
  AllocSize     = 0;
}

MemFILE::~MemFILE()
{
  if(pValue != NULL)
  {
    free(pValue);
    pValue = NULL;
    Size = 0;
  }
}

int MemFILE::Read(void *pBuf, int BufSize)
{
  if(Pos == -1 || bIsModeWrite)
    return 0;

  int CopySize = Size - Pos;
  CopySize = CopySize > BufSize ? BufSize : CopySize;

  memcpy(pBuf, (BYTE *)pValue + Pos, CopySize);
  Pos += CopySize;
  return CopySize;
}

void MemFILE::Resize(int NewSize)
{
  if(NewSize <= AllocSize)
  {

  }
  else
  {
    int Tmp = NewSize - AllocSize;
    Tmp = Tmp < 1024 ? 1024 : Tmp;
    AllocSize += Tmp;
    pValue = (void *)realloc(pValue, AllocSize);
  }
}

int MemFILE::Write(const void *pBuf, int BufSize)
{
  if(Pos == -1 || !bIsModeWrite)
    return 0;
/*
  int tmp = Size + BufSize - AllocSize;

  if(tmp>0)
  {
    tmp = tmp > 1024 ? bufSize : 1024;
    Resize(AllocSize+tmp);
  }
*/
  pValue = (void *)realloc(pValue, Pos + BufSize);
  memcpy((BYTE *)pValue + Pos, pBuf, BufSize);
  Pos += BufSize;
  return BufSize;
}

bool MemFILE::Open(bool bModeWrite, bool bWriteAdd)
{
  bIsModeWrite = bModeWrite;
  if(bModeWrite && bWriteAdd)
    Pos = Size;
  else
    Pos = 0;
  return true;
}

bool MemFILE::Close()
{
  if(bIsModeWrite && Pos != -1)
  {
    Size = Pos;
    pValue = (void *)realloc(pValue, Size);
  }
  Pos = -1;
  return true;
}

void MemFILE::Attach(void *pOtherValue, int OtherSize)
{

}
void *MemFILE::Detach(int &FileSize)
{
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////

MemFILE2::MemFILE2()
{
  pValue        = NULL;
  Size          = 0;
  Pos           = 0;
  AllocSize     = 0;
}

MemFILE2::~MemFILE2()
{
  if(pValue != NULL)
  {
    free(pValue);
    pValue = NULL;
    Size = 0;
  }
}

int MemFILE2::Read(void *pBuf, int BufSize)
{
  int CopySize = Size - Pos;
  CopySize = CopySize > BufSize ? BufSize : CopySize;

  memcpy(pBuf, (BYTE *)pValue + Pos, CopySize);
  Pos += CopySize;
  return CopySize;
}

// 考虑增加FreeExtra函数 ----wbj

void MemFILE2::Resize(int NewSize)
{
  if(NewSize <= AllocSize)
  {
    if(NewSize < Pos)
      Pos = NewSize; //----
  }
  else
  {
    int Tmp = NewSize - AllocSize;
    Tmp = Tmp < 1024 ? 1024 : Tmp;
    AllocSize += Tmp;
    pValue = (void *)realloc(pValue, AllocSize);
  }
  Size = NewSize;
}

int MemFILE2::Write(const void *pBuf, int BufSize)
{
/*
  int tmp = Size + BufSize - AllocSize;

  if(tmp>0)
  {
    tmp = tmp > 1024 ? bufSize : 1024;
    Resize(AllocSize+tmp);
  }
*/
  //pValue = (void *)realloc(pValue, Pos + BufSize);
  if(Pos + BufSize > Size)
    Resize(Pos + BufSize);

  memcpy((BYTE *)pValue + Pos, pBuf, BufSize);
  Pos += BufSize;
  return BufSize;
}

int MemFILE2::Seek(int Len, int PosFlag)
{
  if(PosFlag == SEEK_SET)
  {
    Pos = Len;
  }
  else if(PosFlag == SEEK_CUR)
  {
    Pos += Len;
  }
  else if(PosFlag == SEEK_END)
  {
    Pos = Size + Len;
  }
  else
  {
  }

  if(Pos < 0)
    Pos = 0;
  else if(Pos > Size)
    Pos = Size;
  return Pos;
}

bool MemFILE2::Attach(void *pOtherValue, int OtherSize)
{
  if(pOtherValue == NULL)
    return false;

  free(pValue);
  pValue = pOtherValue;
  Size = AllocSize = OtherSize;
  Pos = 0;
  return true;
}
void *MemFILE2::Detach(int &FileSize)
{
  FileSize = Size;
  void *pOtherValue = pValue;
  pValue = NULL;
  Size = AllocSize = 0;
  Pos = 0;
  return pOtherValue;
}

}