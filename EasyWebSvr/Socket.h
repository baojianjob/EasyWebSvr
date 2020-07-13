#ifndef __BASESOCKET_H__
#define __BASESOCKET_H__
#pragma once

namespace minilib
{

#ifndef SD_BOTH
  #define SD_BOTH       0x02
#endif 

#ifndef WSAEVENT
  #define WSAEVENT      HANDLE
#endif

// SOCKET 基类
// wbj 2003.4.4
// wbj 2003.6.24  add SendTo, ReceiveFrom
// wbj 2003.10    add SendAll, RecvAll
// wbj 2004.2.26  Modify Function: Close()

class BaseSOCKET
{
public:
  BaseSOCKET();
  virtual ~BaseSOCKET();

  static bool   SocketInit(int MajorVersion = 1, int MinorVersion = 1);
  static bool   SocketTerm();

  bool          Create(int Type = SOCK_STREAM, int Protocol = 0);
  void          Close();

  bool          SetOptNoDelay(BOOL bNoDelay);
  bool          SetReuseAddr(BOOL bReuse);

//protected:
  int           Send(const char *pBuf, int BufLen, int Flags = 0);
  int           SendAll(const BYTE *pBuf, int Size, int TimeOutSecond = -1);
  int           RecvAll(BYTE *pBuf, int Size, int TimeOutSecond = -1);
  int           SendStr(const char *pBuf);

public:
  int           Receive(char *pBuf, int BufLen, int Flags = 0);
  
  bool          RecvAllStr(char *pBuf, int Size, const char *pEndFlag);
  bool          RecvAllStr2(char *pBuf, int Size, const char *pEndFlag);
  //int           ReceiveEx(BYTE *pRecvBuffer, int BufSize, UINT TimeOutSecond);
  //char         *ReceiveEx(const char *pEndFlagStr, UINT TimeOutSecond, UINT *pRecvSize = NULL);
  bool          IsReadable();
  bool          IsWriteable();
  bool          WaitData(bool bIsForRead, DWORD MilliSeconds);

  int           SendTo(const void *pBuf, int BufLen, UINT HostPort, const char *pHostAddress = NULL, int Flags = 0);
  int           ReceiveFrom(void *pBuf, int BufLen, char *pSocketAddress, int AddrBufSize, UINT &rSocketPort, int Flags = 0);

  bool          Connect(DWORD Ip, int Port);
  bool          Connect(const char *pServerName, int Port);
  bool          ConnectEx(DWORD Ip, int Port, int TimeoutMilliSeconds);
  bool          ConnectEx(const char *pServerName, int Port, int TimeoutMilliSeconds);

  bool          Bind(int Port, const char *pSocketAddress = NULL);

  bool          Listen(int Backlog = 5);
  bool          Accept(BaseSOCKET &ConnectedSocket, SOCKADDR *pSockAddr = NULL, int *pSockAddrLen = NULL);

  bool          SetBlockingMode(bool bIsBlocking);

  bool          SetSendTimeout(int MilliSeconds);                   // Sets send timeout, in milliseconds
  bool          SetRecvTimeout(int MilliSeconds);               // Sets recv timeout, in milliseconds
  int           GetSendTimeout();
  int           GetRecvTimeout();
  
  bool          SetSendBufSize(int NewBufSize);
  bool          SetRecvBufSize(int NewBufSize);
  int           GetSendBufSize();
  int           GetRecvBufSize();

  bool          AsyncSelect(HWND hWnd, UINT Msg, long Event);
  bool          EventSelect(WSAEVENT hEvent, long NetworkEvents);

  bool          Attach(SOCKET hSocketOther);
  SOCKET        Detach();

  SOCKET        GetHandle() { return hSocket; }
  bool          IsValidSocket() { return hSocket != INVALID_SOCKET; }

  int           GetLocalPort();                                     // Returns local Port number
  int           GetRemotePort();                                    // Returns remote side Port number

  bool          GetLocalIp(char *pStrIpBuf, int BufSize);           // Returns local IP address
  bool          GetRemoteIp(char *pStrIpBuf, int BufSize);          // Returns remote side IP

  static bool   GetLocalIpStatic(char *pStrIpBuf, int BufSize);

  static bool   GetLocalHostName(char *pHostNameBuf, int BufSize);  // Returns local host name
  bool          GetRemoteHostName(char *pHostNameBuf, int BufSize); // Returns remote host name

  static bool   GetIpAddr(const char *pName, DWORD *pIp);
  static void   LongToDotIp(unsigned long LongIp, char *pStrIpBuf, int BufSize);

protected:
  SOCKET        hSocket;
};

///////////////////////////////////////////////////////////////////////////////

inline BaseSOCKET::BaseSOCKET()
{
  hSocket = INVALID_SOCKET;
}

inline BaseSOCKET::~BaseSOCKET()
{
  if(hSocket != INVALID_SOCKET)
    Close();
}

inline bool BaseSOCKET::SocketTerm()  // static
{
  return WSACleanup() != SOCKET_ERROR;
}

inline bool BaseSOCKET::Create(int Type, int Protocol)
{
  assert(hSocket == INVALID_SOCKET);

  hSocket = socket(PF_INET, Type, Protocol); 
  return (hSocket != INVALID_SOCKET);
}

inline void BaseSOCKET::Close()
{
  if(hSocket != INVALID_SOCKET)
  {
    shutdown(hSocket, SD_BOTH);  // wbj add  2004.2.26
    closesocket(hSocket);
    hSocket = INVALID_SOCKET;
  }
}

/*inline int BaseSOCKET::Read(void *pBuf, int BufSize)
{
  return RecvAll((BYTE *)pBuf, BufSize);
}

inline int BaseSOCKET::Write(const void *pBuf, int BufSize)
{
  return SendAll((const BYTE *)pBuf, BufSize);
}
*/
inline int BaseSOCKET::Send(const char *pBuf, int BufLen, int Flags)
{
  assert(hSocket != INVALID_SOCKET);
  return send(hSocket, pBuf, BufLen, Flags);
}

inline int BaseSOCKET::Receive(char *pBuf, int BufLen, int Flags)
{
  assert(hSocket != INVALID_SOCKET);
  return recv(hSocket, pBuf, BufLen, Flags);
}

inline int BaseSOCKET::SendStr(const char *pBuf)
{
  assert(hSocket != INVALID_SOCKET);
  return SendAll((const BYTE *)pBuf, (int)strlen(pBuf));
}

inline bool BaseSOCKET::Listen(int Backlog)
{
  assert(hSocket != INVALID_SOCKET);

  return ::listen(hSocket, Backlog) != SOCKET_ERROR;
}

inline bool BaseSOCKET::Accept(BaseSOCKET &ConnectedSocket, SOCKADDR *pSockAddr, int *pSockAddrLen)
{
  assert(hSocket != INVALID_SOCKET);
  assert(ConnectedSocket.hSocket == INVALID_SOCKET);

  ConnectedSocket.hSocket = ::accept(hSocket, pSockAddr, pSockAddrLen);

  return ConnectedSocket.hSocket != SOCKET_ERROR;
}

inline bool BaseSOCKET::SetBlockingMode(bool bIsBlocking)
{
  assert(hSocket != INVALID_SOCKET);
  unsigned long Mode = bIsBlocking ? 0 : 1;
  return ioctlsocket(hSocket, FIONBIO, (unsigned long*)&Mode) != SOCKET_ERROR;
}

inline bool BaseSOCKET::SetSendTimeout(int MilliSeconds)
{
  assert(MilliSeconds >= 0);
  return setsockopt(hSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&MilliSeconds, sizeof(MilliSeconds)) != SOCKET_ERROR;
}

inline bool BaseSOCKET::SetRecvTimeout(int MilliSeconds)
{
  assert(MilliSeconds >= 0);
  return setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&MilliSeconds, sizeof(MilliSeconds)) != SOCKET_ERROR;
}

inline int BaseSOCKET::GetSendTimeout()
{
  int MilliSeconds, Size = sizeof(int);
  if(getsockopt(hSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&MilliSeconds, &Size) == SOCKET_ERROR)
    return -1;
  return MilliSeconds;
}

inline int BaseSOCKET::GetRecvTimeout()
{
  int MilliSeconds, Size = sizeof(int);
  if(getsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&MilliSeconds, &Size) == SOCKET_ERROR)
    return -1;
  return MilliSeconds;
}

inline bool BaseSOCKET::SetSendBufSize(int NewBufSize)
{
  assert(NewBufSize >= 0);
  return setsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, (char *)&NewBufSize, sizeof(NewBufSize)) != SOCKET_ERROR;
}

inline bool BaseSOCKET::SetRecvBufSize(int NewBufSize)
{
  assert(NewBufSize >= 0);
  return setsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, (char *)&NewBufSize, sizeof(NewBufSize)) != SOCKET_ERROR;
}

inline int BaseSOCKET::GetSendBufSize()
{
  int BufSize, Size = sizeof(int);
  if(getsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, (char *)&BufSize, &Size) == SOCKET_ERROR)
    return -1;
  return BufSize;
}

inline int BaseSOCKET::GetRecvBufSize()
{
  int BufSize, Size = sizeof(int);
  if(getsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, (char *)&BufSize, &Size) == SOCKET_ERROR)
    return -1;
  return BufSize;
}

inline bool BaseSOCKET::Attach(SOCKET hSocketOther)
{
  assert(hSocketOther != INVALID_SOCKET);
  assert(hSocket == INVALID_SOCKET);

  if(hSocketOther == INVALID_SOCKET || hSocket != INVALID_SOCKET)
    return false;

  hSocket = hSocketOther;
  return true;
}

inline SOCKET BaseSOCKET::Detach()
{
  assert(hSocket != INVALID_SOCKET);

  SOCKET hSocketRet = hSocket;
  hSocket = INVALID_SOCKET;

  return hSocketRet;
}

inline void BaseSOCKET::LongToDotIp(unsigned long LongIp, char *pStrIpBuf, int BufSize)
{
  _snprintf(pStrIpBuf, BufSize, "%d.%d.%d.%d", (int)((BYTE *)&LongIp)[0],
    (int)((BYTE *)&LongIp)[1], (int)((BYTE *)&LongIp)[2], (int)((BYTE *)&LongIp)[3]);
}

// 是否采用延时算法 
inline bool BaseSOCKET::SetOptNoDelay(BOOL bNoDelay)
{
  assert(hSocket != INVALID_SOCKET);
  return setsockopt(hSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&bNoDelay, sizeof(bNoDelay)) != SOCKET_ERROR;
}

inline bool BaseSOCKET::SetReuseAddr(BOOL bReuse)
{
  assert(hSocket != INVALID_SOCKET);
  return setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&bReuse, sizeof(bReuse)) != SOCKET_ERROR;
}

}

#endif // __BASESOCKET_H__