//***********************************************
// 文件名称：mtstring.h
// 功　　能：字符串类
// 作　　者：wbj
// 创建日期：2002-10-18
// 更新日期：2007-02-05
//***********************************************
#ifndef __MTSTRING_H__
#define __MTSTRING_H__
#pragma once

namespace minilib
{

class MtSTRING
{
protected:
  TCHAR        *pString;
  int           Length;

public:
  MtSTRING(const TCHAR *pStr = _T(""));
  MtSTRING(const MtSTRING &OtherStr);
  MtSTRING(TCHAR Ch, int Repeat = 1);
  MtSTRING(const TCHAR *pStr, int Len);
  //MtSTRING(int IdStr);
#ifdef _UNICODE
#else
  MtSTRING(const WCHAR *pStr);
#endif

  ~MtSTRING();

  const MtSTRING &operator =(const TCHAR *pStr);
  const MtSTRING &operator =(const MtSTRING &Str);
  
  const MtSTRING &operator +=(const TCHAR *pStr);
  const MtSTRING &operator +=(const MtSTRING &Str);
  const MtSTRING &operator +=(TCHAR Ch);

  friend MtSTRING operator +(const MtSTRING &Str1, const MtSTRING &Str2);
  friend MtSTRING operator +(const MtSTRING &Str1, const TCHAR *pStr2);
  friend MtSTRING operator +(const TCHAR *pStr1, const MtSTRING &Str2);
  friend MtSTRING operator +(const MtSTRING &Str, TCHAR Ch);
  friend MtSTRING operator +(TCHAR Ch, const MtSTRING &Str);
  
  bool          operator ==(const TCHAR *pOtherStr) const;
  bool          operator !=(const TCHAR *pOtherStr) const;
  bool          operator ==(const MtSTRING &OtherStr) const;
  bool          operator !=(const MtSTRING &OtherStr) const;
  bool          operator <(const MtSTRING &OtherStr) const;

  operator LPCTSTR() const;
  TCHAR        &operator[](int Index); // 若使用此函数截断了字符串, 一定要调用 ReleaseBuffer 函数
  TCHAR         operator[](int Index) const;

  bool          Attach(TCHAR *pOtherStr, int StrLen = -1);
  TCHAR        *Detach(int *pStrLen = NULL);

  TCHAR         GetAt(int Index) const;
  void          SetAt(int Index, TCHAR Ch);

  int           Compare(const TCHAR *pStr) const;
  int           CompareNoCase(const TCHAR *pStr) const;

  int           Find(TCHAR Ch, int StartIndex = 0) const;
  int           Find(const TCHAR *pSubStr, int StartIndex = 0) const;
  int           FindNoCase(const TCHAR *pSubStr, int StartIndex = 0) const;
  int           ReverseFind(TCHAR Ch) const;
  int           ReverseFind(const TCHAR *pSubStr, int StartIndex = -1) const;
  int           ReverseFindNoCase(const TCHAR *pSubStr, int StartIndex = -1) const;

  void          Format(const TCHAR *pFormat, ...);
  void          FormatV(const TCHAR *pFormat, va_list ArgList);

  int           GetLength() const { return Length; }
  TCHAR        *GetBuffer(int BufferLength);          // 使用此函数可以直接访问内部内存空间
  void          ReleaseBuffer(int NewLength = -1);    // 用 GetBuffer 函数后, 一定要调用 ReleaseBuffer 函数
  void          FreeExtra();

  void          Empty();
  bool          IsEmpty() const { return Length == 0 ? true : false; }

  int           Insert(int Index, TCHAR Ch);
  int           Insert(int Index, const TCHAR *pStr);
  int           Delete(int Index, int Count = 1);
  int           Replace(int BeginIndex, int Count, const TCHAR *pNewStr);
  int           Replace(const TCHAR *pOldStr, const TCHAR *pNewStr);

  bool          LoadString(UINT IdStr, int MaxLen = 500);

  void          MakeLower();
  void          MakeUpper();
  void          MakeReverse();

  MtSTRING      Left(int Count) const;
  MtSTRING      Right(int Count) const;
  MtSTRING      Mid(int BeginIndex, int Count) const;

  void          TrimLeft(TCHAR TargetCh);
  void          TrimLeft(const TCHAR *pTargetStr = _T("\t\r\n "));
  void          TrimRight(TCHAR TargetCh);
  void          TrimRight(const TCHAR *pTargetStr = _T("\t\r\n "));

  int           GetLeftSameCount(const TCHAR *pOtherStr) const;
  int           GetLeftSameCountNoCase(const TCHAR *pOtherStr) const;

protected:
  void          _CopyString(const TCHAR *pStr, int Count = -1);
};

inline void MtSTRING::Empty()
{
  _CopyString(_T(""));
}

inline const MtSTRING &MtSTRING::operator +=(const MtSTRING &Str)
{
  return operator +=(Str.pString);
}

inline bool MtSTRING::operator ==(const TCHAR *pOtherStr) const
{
  return ::_tcscmp(pString, pOtherStr) == 0;
}

inline bool MtSTRING::operator !=(const TCHAR *pOtherStr) const
{
  return !operator ==(pOtherStr);
}

inline bool MtSTRING::operator ==(const MtSTRING &OtherStr) const
{
  return operator ==(OtherStr.pString);
}

inline bool MtSTRING::operator !=(const MtSTRING &OtherStr) const
{
  return !operator ==(OtherStr);
}

inline bool MtSTRING::operator <(const MtSTRING &OtherStr) const
{
  return Compare(OtherStr.pString) < 0;
}

inline MtSTRING::operator LPCTSTR() const
{
  return pString;
}

inline MtSTRING operator +(const MtSTRING &Str1, const MtSTRING &Str2)
{
  return MtSTRING(Str1) += Str2;
}

inline MtSTRING operator +(const MtSTRING &Str1, const TCHAR *pStr2)
{
  return MtSTRING(Str1) += pStr2;
}

inline MtSTRING operator +(const TCHAR *pStr1, const MtSTRING &Str2)
{
  return MtSTRING(pStr1) += Str2;
}

inline MtSTRING operator +(const MtSTRING &Str, TCHAR Ch)
{
  return MtSTRING(Str) += Ch;
}

inline MtSTRING operator +(TCHAR Ch, const MtSTRING &Str)
{
  return MtSTRING(Ch) += Str;
}

// 直接访问内部缓冲区
// 若使用此函数截断了字符串, 一定要调用 ReleaseBuffer 函数, 以取得正确的字符串长度
inline TCHAR &MtSTRING::operator[](int Index)
{
  assert(Index >= 0 && Index < Length);
  return pString[Index];
}

inline TCHAR MtSTRING::operator[](int Index) const
{
  assert(Index >= 0 && Index < Length);
  return pString[Index];
}

inline TCHAR MtSTRING::GetAt(int Index) const
{
  assert(Index >= 0 && Index < Length);
  return pString[Index];
}

inline int MtSTRING::Compare(const TCHAR *pStr) const
{
  return ::_tcscmp(pString, pStr);
}

inline int MtSTRING::CompareNoCase(const TCHAR *pStr) const 
{
  return ::_tcsicmp(pString, pStr);
}

inline int MtSTRING::Find(TCHAR Ch, int StartIndex) const
{
  assert(StartIndex >= 0);
  return Find((LPCTSTR)MtSTRING(Ch), StartIndex);
}

inline int MtSTRING::Insert(int Index, TCHAR Ch) 
{
  assert(Index >= 0);
  return Insert(Index, (LPCTSTR)MtSTRING(Ch));
}

inline int MtSTRING::Insert(int Index, const TCHAR *pStr)
{
  assert(Index >= 0 && Index <= Length && pStr != NULL);
  return Replace(Index, 0, pStr);
}

inline int MtSTRING::Delete(int Index, int Count)
{
  assert(Index >= 0 && Index <= Length);
  assert(Count > 0);
  return Replace(Index, Count, _T(""));
}

inline void MtSTRING::MakeLower()
{
  ::_tcslwr(pString);
}

inline void MtSTRING::MakeUpper()
{
  ::_tcsupr(pString);
}

inline void MtSTRING::MakeReverse()
{
  ::_tcsrev(pString);
}

inline void MtSTRING::TrimLeft(TCHAR TargetCh)
{
  TrimLeft((LPCTSTR)MtSTRING(TargetCh));
}

inline void MtSTRING::TrimRight(TCHAR TargetCh)
{
  TrimRight((LPCTSTR)MtSTRING(TargetCh));
}

inline void MtSTRING::FreeExtra()
{
  pString = (TCHAR *)realloc(pString, (Length + 1) * sizeof(TCHAR));
  assert(pString != NULL);
}

}
#endif // __MTSTRING_H__