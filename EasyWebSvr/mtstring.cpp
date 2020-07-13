//***********************************************
// 文件名称：mtstring.cpp
// 功　　能：字符串类
// 作　　者：wbj
// 创建日期：2002-10-18
// 更新日期：2007-02-05
//***********************************************
/*
更新历史:
2004-06-22 Add Func "TCHAR operator[](int Index) const;"
2006-12-18 Modify ReleaseBuffer()
2007-01-03 Add Func ReverseFindNoCase()
2007-02-05 Add Func FreeExtra()
*/
#include "stdafx.h"
#include "mtstring.h"

#pragma warning(disable:4267)

namespace minilib
{

MtSTRING::MtSTRING(const TCHAR *pStr)
{
  pString = NULL;
  Length  = 0;
  _CopyString(pStr);
}

#ifdef _UNICODE
#else
MtSTRING::MtSTRING(const WCHAR *pStr)
{
  int WideLen = wcslen(pStr);
  int Length = WideCharToMultiByte(CP_ACP, 0, pStr, WideLen, NULL, 0, NULL, NULL);
  pString = (char *)malloc(Length + 1);
  WideCharToMultiByte(CP_ACP, 0, pStr, WideLen, pString, Length, NULL, NULL);
  pString[Length] = 0;

  assert((int)strlen(pString) == Length);
}
#endif

MtSTRING::MtSTRING(const MtSTRING &OtherStr)
{
  pString = NULL;
  Length  = 0;
  _CopyString(OtherStr.pString);
}

MtSTRING::MtSTRING(TCHAR Ch, int Repeat)
{
  assert(Repeat >= 0);
  if(Repeat < 0)
    return;

  pString = (TCHAR *)malloc((Repeat + 1) * sizeof(TCHAR));
  assert(pString);
  if(pString == NULL)
    return;

  int i;
  for(i = 0; i < Repeat; i++)
    pString[i] = Ch;
  pString[i] = '\0';

  Length  = Repeat;
}

MtSTRING::MtSTRING(const TCHAR *pStr, int Len)
{
  pString = NULL;
  Length  = 0;
  _CopyString(pStr, Len);
}
/*
MtSTRING::MtSTRING(int IdStr)
{
  pString = NULL;
  Length  = 0;
  if(!LoadString(IdStr))
    _CopyString(_T(""));
}
*/
MtSTRING::~MtSTRING()
{
  if(pString)
  {
    free(pString);
    Length = 0;
  }
}

// 使用此函数可以直接访问内部内存空间
// 使用此函数得到长度为 BufferLength 的内存缓冲区, 直接访问一定不能超出此缓冲区
// 不再直接访问时, 一定要调用 ReleaseBuffer() 函数
TCHAR *MtSTRING::GetBuffer(int BufferLength)
{
  assert(BufferLength >= 1);
  if(BufferLength > Length + 1)
  {
    pString = (TCHAR *)realloc(pString, BufferLength * sizeof(TCHAR));
    assert(pString != NULL);
  }
  return pString;
}

// 函数作用: 取得新的字符串长度, 重新分配内存空间
// 用 GetBuffer 函数后, 一定要调用 ReleaseBuffer 函数
// 使用operator[]函数截断了字符串后, 一定要调用 ReleaseBuffer 函数
void MtSTRING::ReleaseBuffer(int NewLength/* = -1*/)
{
  if(NewLength == -1)
    NewLength = _tcslen(pString);
  else
  {
    pString[NewLength] = _T('\0');
    assert((int)_tcslen(pString) == NewLength);
  }

  Length = NewLength;
}

void MtSTRING::_CopyString(const TCHAR *pStr, int Count)
{
  if(pString)
    free(pString);

  if(pStr == NULL)
    pStr = _T("");

  Length = _tcslen(pStr);
  if(Count >= 0 && Count < Length)
    Length = Count;
  pString = (TCHAR *)malloc((Length + 1) * sizeof(TCHAR));
  assert(pString);
  
  if(pString)
  {
    _tcsncpy(pString, pStr, Length);
    pString[Length] = 0;
  }
  else
    Length = 0;
}

const MtSTRING &MtSTRING::operator =(const TCHAR *pStr)
{
  if(pStr != pString)
    _CopyString(pStr);
  return *this;
}

const MtSTRING &MtSTRING::operator =(const MtSTRING &Str)
{
  if(&Str != this)
    _CopyString(Str.pString);
  return *this;
}

const MtSTRING &MtSTRING::operator +=(const TCHAR *pStr)
{
  if(pStr != NULL)
  {
    Length += _tcslen(pStr);
    pString = (TCHAR *)realloc(pString,  (Length + 1) * sizeof(TCHAR));
    assert(pString);

    _tcscat(pString, pStr);
  }
  return *this;
}

const MtSTRING &MtSTRING::operator +=(TCHAR Ch)
{
  TCHAR S[2];
  S[0] = Ch;
  S[1] = '\0';
  return operator +=(S);
}

// pOtherStr 必须是用 malloc 或 realloc 分配的空间
// 调用此函数后, 不能再用 free 释放 pOtherStr 的内存
bool MtSTRING::Attach(TCHAR *pOtherStr, int StrLen)
{
  assert(pOtherStr != NULL);

  if(pOtherStr == NULL)
    return false;
  assert(StrLen < 0 || StrLen == (int)_tcslen(pOtherStr));

  free(pString);

  pString = pOtherStr;
  Length = (StrLen < 0 ? _tcslen(pString) : StrLen);
  return true;
}

// 此函数的返回值一定要记得用 free 函数释放空间
TCHAR *MtSTRING::Detach(int *pStrLen)
{
  TCHAR *pStrRet = pString;
  if(pStrLen != NULL)
    *pStrLen = Length;

  pString = NULL;
  Length = 0;
  _CopyString(_T(""));  

  return pStrRet;
}

void MtSTRING::SetAt(int Index, TCHAR Ch)
{
  assert(Index >= 0 && Index < Length);
  pString[Index] = Ch;
  if(Ch == '\0')
    ReleaseBuffer();
}

int MtSTRING::Find(const TCHAR *pSubStr, int StartIndex) const
{
  assert(pSubStr != NULL && StartIndex >= 0);

  if(StartIndex >= Length)
    return -1;

  TCHAR *pResult = ::_tcsstr(pString + StartIndex, pSubStr);
  if(pResult == NULL)
    return -1;
  return (int)(pResult - pString);
}

int MtSTRING::FindNoCase(const TCHAR *pSubStr, int StartIndex) const
{
  assert(pSubStr != NULL && StartIndex >= 0);

  int SubLen = _tcslen(pSubStr);
  for(int j = StartIndex; j <= Length - SubLen; j++)
  {
    int i;
    for(i = 0; i < SubLen && tolower(pString[j + i]) == tolower(pSubStr[i]); i++)
      ;
    if(i == SubLen)
      return j;
  }
  return -1;
}

int MtSTRING::ReverseFind(TCHAR Ch) const
{
  TCHAR *pResult = ::_tcsrchr(pString, Ch);
  if(pResult == NULL)
    return -1;
  return (int)(pResult - pString);
}

int MtSTRING::ReverseFind(const TCHAR *pSubStr, int StartIndex/* = -1*/) const
{
  assert(pSubStr && *pSubStr);
  if(StartIndex < 0 || StartIndex > Length)
    StartIndex = Length;

  int i, j, SubLen = _tcslen(pSubStr);
  for(j = StartIndex - 1; j >= SubLen - 1; j--)
  {
    for(i = 0; i < SubLen && (pString[j - i] == pSubStr[SubLen - i - 1]); i++)
      ;
    if(i == SubLen)
      return j - SubLen + 1;
  }
  return -1;
}

int MtSTRING::ReverseFindNoCase(const TCHAR *pSubStr, int StartIndex/* = -1*/) const
{
  assert(pSubStr && *pSubStr);
  if(StartIndex < 0 || StartIndex > Length)
    StartIndex = Length;

  int i, j, SubLen = _tcslen(pSubStr);
  for(j = StartIndex - 1; j >= SubLen - 1; j--)
  {
    for(i = 0; i < SubLen && (tolower(pString[j - i]) == tolower(pSubStr[SubLen - i - 1])); i++)
      ;
    if(i == SubLen)
      return j - SubLen + 1;
  }
  return -1;
}

void MtSTRING::Format(const TCHAR *pFormat, ...)
{
  assert(pFormat);

  va_list ArgList;            // 等价于 char *ArgList;
  va_start(ArgList, pFormat); // 给 ArgList 赋值，等价于 ArgList = (char *)(&pFormat) + sizeof(void *);

  FormatV(pFormat, ArgList);
  
  va_end(ArgList);            // 等价于 ArgList = NULL;
}

void MtSTRING::FormatV(const TCHAR *pFormat, va_list ArgList)
{
  assert(pFormat);

#if _MSC_VER >= 1300  // VC++.NET 或以上版本

  TCHAR *pBuf = GetBuffer(_vsctprintf(pFormat, ArgList) + 1);
  _vstprintf(pBuf, pFormat, ArgList);

#else

  TCHAR *pBuf = GetBuffer(1024);
  _vsntprintf(pBuf, 1024, pFormat, ArgList); //  Format 后字符串最大长度为 1023

#endif

  ReleaseBuffer();
}

// 把从下标 BeginIndex 开始的 Count 个字符替换成 pNewStr
int MtSTRING::Replace(int BeginIndex, int Count, const TCHAR *pNewStr)
{
  assert(BeginIndex >= 0 && BeginIndex <= Length);
  assert(Count >= 0 && pNewStr != NULL);

  if(BeginIndex < 0 || BeginIndex > Length || Count < 0 || pNewStr == NULL)
    return Length;

  if(BeginIndex + Count > Length)
    Count = Length - BeginIndex;
  
  int NewSubLen = _tcslen(pNewStr);
  if(NewSubLen > Count)
  {
    pString = (TCHAR *)realloc(pString, (Length + NewSubLen - Count + 1) * sizeof(TCHAR));
    assert(pString);

    int i;
    for(i = Length; i >= BeginIndex + Count; i--)
      pString[i + NewSubLen - Count] = pString[i];

    for(i = 0; i < NewSubLen; i++)
      pString[BeginIndex + i] = pNewStr[i];

    Length += NewSubLen - Count;
  }
  else if(NewSubLen == Count)
  {
    for(int i = 0; i < Count; i++)
      pString[BeginIndex + i] = pNewStr[i];
  }
  else
  {
    int i;
    for(i = 0; i < NewSubLen; i++)
      pString[BeginIndex + i] = pNewStr[i];

    for(i = BeginIndex + Count; i <= Length; i++)
      pString[i - Count + NewSubLen] = pString[i];

    pString = (TCHAR *)realloc(pString, (Length + NewSubLen - Count + 1) * sizeof(TCHAR));
    assert(pString);

    Length -= Count - NewSubLen;
  }

  return Length;
}

int MtSTRING::Replace(const TCHAR *pOldStr, const TCHAR *pNewStr)
{
  assert(pOldStr != NULL && pNewStr != NULL);
  if(pOldStr == NULL || pNewStr == NULL)
    return Length;

  int OldLen = _tcslen(pOldStr);
  int NewLen = _tcslen(pNewStr);
  if(OldLen == 0)
    return Length;

  int Pos = 0;
  while(1)
  {
    Pos = Find(pOldStr, Pos);
    if(Pos == -1)
      break;
    Replace(Pos, OldLen, pNewStr);
    Pos += NewLen;
  }
  return Length;
}

bool MtSTRING::LoadString(UINT IdStr, int MaxLen/* = 500 */)
{
  TCHAR *pBuf = GetBuffer(MaxLen + 1);
  int Num = ::LoadString(::GetModuleHandle(NULL), IdStr, pBuf, MaxLen + 1);
  ReleaseBuffer();

  assert(Num > 0);
  if(Num == 0)
  {
    *this = _T("");
    return false;
  }
  return true;
}

MtSTRING MtSTRING::Left(int Count) const
{
  assert(Count >= 0);
  if(Count < 0)
    Count = 0;

  return MtSTRING(pString, Count);
}

MtSTRING MtSTRING::Right(int Count) const
{
  assert(Count >= 0);
  if(Count < 0)
    Count = 0;

  if(Count > Length)
    Count = Length;

  return MtSTRING(pString + Length - Count);
}

MtSTRING MtSTRING::Mid(int BeginIndex, int Count) const
{
  assert(BeginIndex >= 0 && BeginIndex < Length);
  assert(Count >= 0 && BeginIndex + Count <= Length);

  BeginIndex = min(max(BeginIndex, 0), Length - 1);
  Count = min(max(Count, 0), Length - BeginIndex);

  return MtSTRING(pString + BeginIndex, Count);
}

void MtSTRING::TrimLeft(const TCHAR *pTargetStr)
{
  assert(pTargetStr);
  if(pTargetStr == NULL)
    return;

  int Start;
  for(Start = 0; Start < Length; Start++)
    if(_tcschr(pTargetStr, pString[Start]) == NULL)
      break;
  if(Start > 0)
  {
    for(int i = Start; i <= Length; i++)
      pString[i - Start] = pString[i];

    Length -= Start;
  }
}

void MtSTRING::TrimRight(const TCHAR *pTargetStr)
{
  assert(pTargetStr);
  if(pTargetStr == NULL)
    return;

  int i;
  for(i = Length - 1; i >= 0; i--)
    if(_tcschr(pTargetStr, pString[i]) == NULL)
      break;
  if(i != Length - 1)
  {
    pString[i + 1] = '\0';
    Length = i + 1;
  }
}

int MtSTRING::GetLeftSameCount(const TCHAR *pOtherStr) const 
{
  int SameCount = 0;
  const TCHAR *pStr1 = pString;
  const TCHAR *pStr2 = pOtherStr;
  while(1)
  {
    if(*pStr1 != *pStr2)
      break;
    SameCount++;
    pStr1++;
    pStr2++;
  }
  return SameCount;
}

int MtSTRING::GetLeftSameCountNoCase(const TCHAR *pOtherStr) const
{
  int SameCount = 0;
  const TCHAR *pStr1 = pString;
  const TCHAR *pStr2 = pOtherStr;
  while(1)
  {
    if(::tolower(*pStr1) != ::tolower(*pStr2))
      break;
    SameCount++;
    pStr1++;
    pStr2++;
  }
  return SameCount;
}

}