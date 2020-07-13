#ifndef __BASEIO_H__
#define __BASEIO_H__
#pragma once
//***********************************************
// 文件名称：baseio.h
// 功　　能：数据流输入/输出
// 作　　者：wbj
// 创建日期：2003-6-6
// 更新日期：2005-12-19
//***********************************************

//#include "mtstring.h"
#include "socket.h"

namespace minilib
{

class BaseIO
{
public:
  BaseIO() {};
  virtual ~BaseIO() {};

  virtual int Read(void *pBuf, int BufSize) = 0;
  virtual int Write(const void *pBuf, int BufSize) = 0;

  bool ReadValue(BYTE &ByteValue);
  bool WriteValue(BYTE ByteValue);

  bool ReadValue(bool &BoolValue);
  bool WriteValue(bool BoolValue);

  bool ReadValue(int &IntValue);
  bool WriteValue(int IntValue);

  bool ReadValue(__int64 &Int64Value);
  bool WriteValue(__int64 Int64Value);

  bool ReadValue(float &FloatValue);
  bool WriteValue(float FloatValue);

  bool ReadValue(short &ShortValue);
  bool WriteValue(short ShortValue);

  bool ReadValue(UINT &UintValue);
  bool WriteValue(UINT UintValue);

  bool ReadValue(WORD &WordValue);
  bool WriteValue(WORD WordValue);

  bool ReadValue(DWORD &DWordValue);
  bool WriteValue(DWORD DWordValue);

#ifdef __MTSTRING_H__
  bool ReadValue(MtSTRING &StrValue);
  bool WriteValue(const MtSTRING &StrValue);
#endif // __MTSTRING_H__

#ifdef __MTBINARY_H__
  bool ReadValue(BINARY &BinValue);
  bool WriteValue(const BINARY &BinValue);
#endif // __MTBINARY_H__
/*
  template <typename NewType>
  bool ReadValue(NewType &Value)
  {
    return Value.Load(this);
  }

  template <typename NewType>
  bool WriteValue(const NewType &Value)
  {
    return Value.Save(this);
  }*/
};

inline bool BaseIO::ReadValue(BYTE &ByteValue)
{
  return Read(&ByteValue, sizeof(BYTE)) == sizeof(BYTE);
}

inline bool BaseIO::WriteValue(BYTE ByteValue)
{
  return Write(&ByteValue, sizeof(BYTE)) == sizeof(BYTE);
}

inline bool BaseIO::ReadValue(bool &BoolValue)
{
  return Read(&BoolValue, sizeof(bool)) == sizeof(bool);
}

inline bool BaseIO::WriteValue(bool BoolValue)
{
  return Write(&BoolValue, sizeof(bool)) == sizeof(bool);
}

inline bool BaseIO::ReadValue(int &IntValue)
{
  return Read(&IntValue, sizeof(int)) == sizeof(int);
}

inline bool BaseIO::WriteValue(int IntValue)
{
  return Write(&IntValue, sizeof(int)) == sizeof(int);
}

inline bool BaseIO::ReadValue(__int64 &Int64Value)
{
  return Read(&Int64Value, sizeof(__int64)) == sizeof(__int64);
}

inline bool BaseIO::WriteValue(__int64 Int64Value)
{
  return Write(&Int64Value, sizeof(__int64)) == sizeof(__int64);
}

inline bool BaseIO::ReadValue(float &FloatValue)
{
  return Read(&FloatValue, sizeof(float)) == sizeof(float);
}

inline bool BaseIO::WriteValue(float FloatValue)
{
  return Write(&FloatValue, sizeof(float)) == sizeof(float);
}

inline bool BaseIO::ReadValue(short &ShortValue)
{
  return Read(&ShortValue, sizeof(short)) == sizeof(short);
}

inline bool BaseIO::WriteValue(short ShortValue)
{
  return Write(&ShortValue, sizeof(short)) == sizeof(short);
}

inline bool BaseIO::ReadValue(UINT &UintValue)
{
  return Read(&UintValue, sizeof(UINT)) == sizeof(UINT);
}

inline bool BaseIO::WriteValue(UINT UintValue)
{
  return Write(&UintValue, sizeof(UINT)) == sizeof(UINT);
}

inline bool BaseIO::ReadValue(WORD &WordValue)
{
  return Read(&WordValue, sizeof(WORD)) == sizeof(WORD);
}

inline bool BaseIO::WriteValue(WORD WordValue)
{
  return Write(&WordValue, sizeof(WORD)) == sizeof(WORD);
}

inline bool BaseIO::ReadValue(DWORD &DWordValue)
{
  return Read(&DWordValue, sizeof(DWORD)) == sizeof(DWORD);
}

inline bool BaseIO::WriteValue(DWORD DWordValue)
{
  return Write(&DWordValue, sizeof(DWORD)) == sizeof(DWORD);
}

///////////////////////////////////////////////////////////////////////////////

class DosFILE : public BaseIO
{
public:
  DosFILE();
  virtual ~DosFILE();

  virtual int Read(void *pBuf, int BufSize);
  virtual int Write(const void *pBuf, int BufSize);

  bool Open(const TCHAR *pFileName, const TCHAR *pMode);
  bool Close();
  int  Seek(long Len, int PosFlag);
  void SeekToBegin();
  void SeekToEnd();
  int  Flush();
  long GetCurPos();
  long GetFileLength();
  bool SetFileLength(long NewLen); 

protected:
  FILE *pFile;
};

inline DosFILE::DosFILE()
{
  pFile = NULL;
}

inline DosFILE::~DosFILE()
{
  Close();
}

inline bool DosFILE::Open(const TCHAR *pFileName, const TCHAR *pMode)
{
  assert(pFile == NULL);
  pFile = _tfopen(pFileName, pMode);
  return pFile != NULL;
}

inline bool DosFILE::Close()
{
  if(pFile != NULL)
  {
    fclose(pFile);
    pFile = NULL;
  }
  return true;
}

inline int DosFILE::Read(void *pBuf, int BufSize)
{
  assert(pFile != NULL);
  return (int)fread(pBuf, 1, BufSize, pFile); 
}

inline int DosFILE::Write(const void *pBuf, int BufSize)
{
  assert(pFile != NULL);
  return (int)fwrite(pBuf, 1, BufSize, pFile); 
}

inline int DosFILE::Seek(long Len, int PosFlag)
{
  assert(pFile != NULL);
  return fseek(pFile, Len, PosFlag);
}

inline void DosFILE::SeekToBegin()
{
  Seek(0, SEEK_SET);
}

inline void DosFILE::SeekToEnd()
{
  Seek(0, SEEK_END);
}

inline int DosFILE::Flush()
{
  assert(pFile != NULL);
  return fflush(pFile);
}

inline long DosFILE::GetCurPos()
{
  assert(pFile != NULL);
  return ftell(pFile);
}

inline long DosFILE::GetFileLength()
{
  assert(pFile != NULL);
  long CurPos = ftell(pFile);
  fseek(pFile, 0, SEEK_END);
  long Length = ftell(pFile);
  fseek(pFile, CurPos, SEEK_SET);
  return Length;
}

#include <io.h>
inline bool DosFILE::SetFileLength(long NewLen)
{
  assert(pFile != NULL);
  Flush();
  return _chsize(_fileno(pFile), NewLen) == 0;
}
///////////////////////////////////////////////////////////////////////////////

class MemFILE : public BaseIO
{
public:
  MemFILE();
  virtual ~MemFILE();

  virtual int   Read(void *pBuf, int BufSize);
  virtual int   Write(const void *pBuf, int BufSize);

  bool          Open(bool bModeWrite, bool bWriteAdd);
  bool          Close();

  void          Resize(int NewSize);
  int           GetSize() const { return Size; };
  const void   *GetPtr() const { return pValue; };

  void          Attach(void *pOtherValue, int OtherSize);
  void         *Detach(int &FileSize);

protected:
  void *pValue;
  int   Size;
  int   Pos;

  bool  bIsModeWrite;
  int   AllocSize;
};

class MemFILE2 : public BaseIO
{
public:
  MemFILE2();
  virtual ~MemFILE2();

  virtual int   Read(void *pBuf, int BufSize);
  virtual int   Write(const void *pBuf, int BufSize);

  void          Resize(int NewSize);
  int           GetSize() const { return Size; };
  const void   *GetPtr() const { return pValue; };

  int           Seek(int Len, int PosFlag);
  void          SeekToBegin();
  void          SeekToEnd();

  bool          Attach(void *pOtherValue, int OtherSize);
  void         *Detach(int &FileSize);

protected:
  void *pValue;
  int   Size;
  int   Pos;

  int   AllocSize;
};

inline void MemFILE2::SeekToBegin()
{
  Seek(0, SEEK_SET);
}

inline void MemFILE2::SeekToEnd()
{
  Seek(0, SEEK_END);
}

///////////////////////////////////////////////////////////////////////////////

#ifdef __BASESOCKET_H__
class SocketFILE : public BaseIO
{
public:
  SocketFILE(BaseSOCKET *pSocket);
  virtual ~SocketFILE();

public:
  virtual int Read(void *pBuf, int BufSize);
  virtual int Write(const void *pBuf, int BufSize);

protected:
  BaseSOCKET *pSocket;
};

inline SocketFILE::SocketFILE(BaseSOCKET *pSocket)
{
  assert(pSocket != NULL);
  this->pSocket = pSocket;
}

inline SocketFILE::~SocketFILE()
{
}

inline int SocketFILE::Read(void *pBuf, int BufSize)
{
  assert(pSocket != NULL);
  return pSocket->RecvAll((BYTE *)pBuf, BufSize);
}

inline int SocketFILE::Write(const void *pBuf, int BufSize)
{
  assert(pSocket != NULL);
  return pSocket->SendAll((const BYTE *)pBuf, BufSize);
}

#endif // __BASESOCKET_H__

}

#endif // __BASEIO_H__