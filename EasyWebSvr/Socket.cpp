// SOCKET 基类
// wbj 2003.4.4
// SOCKET 基类
// wbj 2003.4.4
// wbj 2003.6.24  add SendTo, ReceiveFrom
// wbj 2003.10    add SendAll, RecvAll
// wbj 2003.10.27 
//     add SetSendBufSize, SetRecvBufSize, GetSendBufSize, GetRecvBufSize
// wbj 2004.11.23 Modify RecvAllStr

//#include <winsock2.h>
#include "stdafx.h"
#include "socket.h"

#pragma comment(lib, "Ws2_32.lib")

namespace minilib
{

bool BaseSOCKET::SocketInit(int MajorVersion, int MinorVersion)  // static
{
  WORD Version = MAKEWORD(MajorVersion, MinorVersion);;
  WSADATA WSData;

  if(WSAStartup(Version, &WSData) != 0)
    return false;
  if(LOBYTE(WSData.wVersion ) != MajorVersion || HIBYTE(WSData.wVersion) != MinorVersion)
  {
    WSACleanup();
    return false; 
  }
  return true;
}

bool BaseSOCKET::IsReadable()
{
  assert(hSocket != INVALID_SOCKET);

  timeval TimeOut = { 0, 0 };
  
  fd_set ReadSet;
  FD_ZERO(&ReadSet);
  FD_SET(hSocket,&ReadSet);
  int Status = select(0, &ReadSet, 0, 0, &TimeOut);

  if(Status == 0 || Status == SOCKET_ERROR)
    return false;
  
  return true;
}

bool BaseSOCKET::IsWriteable()
{
  return WaitData(false, 0);
}

int BaseSOCKET::SendTo(const void *pBuf, int BufLen, UINT HostPort, const char *pHostAddress, int Flags)
{
  SOCKADDR_IN sockAddr;

  memset(&sockAddr, 0, sizeof(sockAddr));

  //LPSTR lpszAscii = T2A((LPTSTR)pHostAddress);
  sockAddr.sin_family = AF_INET;

  if (pHostAddress == NULL)
    sockAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
  else
  {
    sockAddr.sin_addr.s_addr = inet_addr(pHostAddress);
    if (sockAddr.sin_addr.s_addr == INADDR_NONE)
    {
      LPHOSTENT lphost;
      lphost = gethostbyname(pHostAddress);
      if(lphost != NULL)
        sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
      else
      {
        WSASetLastError(WSAEINVAL);
        return SOCKET_ERROR;
      }
    }
  }

  sockAddr.sin_port = htons((u_short)HostPort);

  return sendto(hSocket, (const char *)pBuf, BufLen, Flags, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
}

int BaseSOCKET::ReceiveFrom(void *pBuf, int BufLen, char *pSocketAddress, int AddrBufSize, UINT &rSocketPort, int Flags)
{
  SOCKADDR_IN sockAddr;
  memset(&sockAddr, 0, sizeof(sockAddr));

  int nSockAddrLen = sizeof(sockAddr);
  int nResult = recvfrom(hSocket, (char *)pBuf, BufLen, Flags, (SOCKADDR*)&sockAddr, &nSockAddrLen);
  if(nResult != SOCKET_ERROR)
  {
    rSocketPort = ntohs(sockAddr.sin_port);
    lstrcpyn(pSocketAddress, inet_ntoa(sockAddr.sin_addr), AddrBufSize);
  }
  return nResult;
}

int BaseSOCKET::SendAll(const BYTE *pBuf, int Size, int TimeOutSecond /* = -1*/)
{
#if 0 && defined _DEBUG
  FILE *pFile = fopen("d:\\http.txt", "ab");
  if(pFile != NULL)
  {
    fwrite(pBuf, 1, Size, pFile);
    fclose(pFile);
  }
#endif

  if(TimeOutSecond > 0)
    SetSendTimeout(TimeOutSecond * 1000);

  int Left = Size;
  while(Left > 0)
  {
    //Sleep(500);
    int Ret = send(hSocket, (const char *)pBuf, Left, 0);
    if(Ret <= 0)
    {
      if(Ret == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
      {
        int MilliSeconds = GetSendTimeout();
        if(MilliSeconds <= 0)
          MilliSeconds = 60 * 1000;

        if(!WaitData(false, MilliSeconds))
          break;//return false;
        continue;
      }
      else
      {
        //int Err = WSAGetLastError();
        break;//return false; // 其他错误
      }
    }
    Left -= Ret;
    pBuf += Ret;
  }
  return Size - Left;
}

bool BaseSOCKET::WaitData(bool bIsForRead, DWORD MilliSeconds)
{
  assert(hSocket != INVALID_SOCKET);

  timeval TimeOut;
  TimeOut.tv_sec = MilliSeconds / 1000;
  TimeOut.tv_usec = MilliSeconds % 1000 * 1000;

  fd_set SocketSet;
  FD_ZERO(&SocketSet);
  FD_SET(hSocket, &SocketSet);

  if(bIsForRead)
    return select(0, &SocketSet, 0, 0, &TimeOut) > 0;
  else
    return select(0, 0, &SocketSet, 0, &TimeOut) > 0;
}

int BaseSOCKET::RecvAll(BYTE *pBuf, int Size, int TimeOutSecond/* = -1*/)
{ // 本函数对阻塞和非阻塞的socket都适用
  if(TimeOutSecond > 0)
    SetRecvTimeout(TimeOutSecond * 1000);

  int Left = Size;
  while(Left > 0)
  {
    int Ret = recv(hSocket, (char *)pBuf, Left, 0);
    if(Ret <= 0)
    {
      if(Ret == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
      {
        int MilliSeconds = GetRecvTimeout();
        if(MilliSeconds <= 0)
          MilliSeconds = 60 * 1000;
          
        if(!WaitData(true, MilliSeconds))
          break;//return false;
        continue;
      }
      else
        break;//return false; // 其他错误
    }

    Left -= Ret;
    pBuf += Ret;
  }
  return Size - Left;
}

bool BaseSOCKET::RecvAllStr(char *pBuf, int Size, const char *pEndFlag)
{ // 本函数对阻塞和非阻塞的socket都适用
  int Total = 0;
  int EndFlagLen = lstrlen(pEndFlag);
  Size--;
  while(Size - Total > 0)
  {
    int Ret = recv(hSocket, pBuf + Total, Size - Total, 0);
    
    if(Ret <= 0)
    {
      if(Ret == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
      {
        int MilliSeconds = GetRecvTimeout();
        if(MilliSeconds <= 0)
          MilliSeconds = 60 * 1000;
          
        if(!WaitData(true, MilliSeconds))
          return false;  // 非阻塞模式的超时
        continue;
      }
      else
        return false; // 其他错误
    }

    Total += Ret;
    pBuf[Total] = '\0';
    if(strcmp(pBuf + Total - EndFlagLen, pEndFlag) == 0)
      return true;
  }
  return false;  // 空间满
}

bool BaseSOCKET::RecvAllStr2(char *pBuf, int Size, const char *pEndFlag)
{ // 本函数对阻塞和非阻塞的socket都适用
  int Total = 0;
  int EndFlagLen = lstrlen(pEndFlag);
  Size--;
  while(Size - Total > 0)
  {
    int Ret = recv(hSocket, pBuf + Total, Size - Total, 0);
    
    if(Ret == 0)
    {
      // 客户端正常断开
      return false; 
    }
    else if(Ret < 0)
    {
      if(Ret == SOCKET_ERROR)
      {
        int Err = WSAGetLastError();
        if(Err == WSAEWOULDBLOCK)
        {
          int MilliSeconds = GetRecvTimeout();
          if(MilliSeconds <= 0)
            MilliSeconds = 60 * 1000;

          if(!WaitData(true, MilliSeconds))
          {
            // 非阻塞模式的超时
            return false;
          }
          continue;
        }
        else if(Err == WSAETIMEDOUT)
        {
          // 阻塞模式的超时
          return false;
        }
        else
        {
          // 其他错误
          return false;
        }
      }
      else
      {
        assert(false);
        return false; 
      }
    }

    Total += Ret;
    pBuf[Total] = '\0';
    if(strcmp(pBuf + Total - EndFlagLen, pEndFlag) == 0)
      return true;
  }
  return false;  // 空间满
}

/*
// 返回实际收到的数据字节数
int BaseSOCKET::ReceiveEx(BYTE *pRecvBuffer, int BufSize, UINT TimeOutSecond)
{
  int CurGetSize = 0;
  DWORD BeginTime = ::GetTickCount();
  while(1)
  {
    if(::GetTickCount() - BeginTime > TimeOutSecond * 1000L)  // 判断是否超时
      break;

    bool bIsReadable = IsReadable();  // 判断是否可读
    if(!bIsReadable)
    {
      Sleep(100);
      continue;
    }
    else
    {
      int TempSize = recv(hSocket, (char *)(pRecvBuffer + CurGetSize), BufSize - CurGetSize, 0);
      if(TempSize > 0)
      {
        CurGetSize += TempSize;
        if(CurGetSize >= BufSize)
          break;
        BeginTime = ::GetTickCount();    // 更新起始时间
      }
    }
  }

  return CurGetSize;
}

// 注意用 free 函数释放返回的内存
char *BaseSOCKET::ReceiveEx(const char *pEndFlagStr, UINT TimeOutSecond, UINT *pRecvSize)
{
  int BufSize    = 1000;  // 初始分配内存大小
  int CurGetSize = 0;     // 已收到的字节数
  
  char *pRecvBuffer = (char *)malloc(BufSize);
  assert(pRecvBuffer != NULL);
  memset(pRecvBuffer, 0, BufSize);

  DWORD BeginTime = ::GetTickCount();
  while(1)
  {
    if(::GetTickCount() - BeginTime > TimeOutSecond * 1000L)  // 判断是否超时
    {
      free(pRecvBuffer);
      return NULL;
    }

    bool bIsReadable = IsReadable();  // 判断是否可读
    if(!bIsReadable)
    {
      Sleep(100);
      continue;
    }
    else
    {
      if(BufSize - CurGetSize - 1 <= 0)  // 当前分配的内存不足
      {
        BufSize += 2000;
        pRecvBuffer = (char *)realloc(pRecvBuffer, BufSize);  // 增加内存分配
        assert(pRecvBuffer);
        memset(pRecvBuffer + CurGetSize, 0, BufSize - CurGetSize);
      }
      
      int TempSize = recv(hSocket, pRecvBuffer + CurGetSize, BufSize - CurGetSize - 1, 0);
      if(TempSize > 0)
      {
        CurGetSize += TempSize;
        if(strstr(pRecvBuffer, pEndFlagStr) != NULL)  // 查找接收的结束标志
          break;
        BeginTime = ::GetTickCount();    // 更新起始时间
      }
    }
  }

  if(pRecvSize != NULL)
    *pRecvSize = CurGetSize;

  return pRecvBuffer;
}
*/
bool BaseSOCKET::Connect(DWORD Ip, int Port)
{
  assert(hSocket != INVALID_SOCKET);

  SOCKADDR_IN RemoteSockAddr;
  RemoteSockAddr.sin_family = AF_INET;
  RemoteSockAddr.sin_port = htons((u_short)Port);
  RemoteSockAddr.sin_addr.s_addr = Ip;

  return ::connect(hSocket, (sockaddr *)&RemoteSockAddr, sizeof(RemoteSockAddr)) != SOCKET_ERROR;
}

bool BaseSOCKET::Connect(const char *pServerName, int Port)
{
  assert(hSocket != INVALID_SOCKET);

  DWORD Ip;
  if(!GetIpAddr(pServerName, &Ip))
    return false;
  return Connect(Ip, Port);
}

bool BaseSOCKET::GetIpAddr(const char *pName, DWORD *pIp)
{
  hostent *pHostent = ::gethostbyname(pName);
  if(pHostent == NULL)
    return false;
  *pIp = ((LPIN_ADDR)pHostent->h_addr)->s_addr;
  return true;
}

bool BaseSOCKET::ConnectEx(DWORD Ip, int Port, int TimeoutMilliSeconds)
{
  if(!SetBlockingMode(false))
    return false;

  if(!Connect(Ip, Port) && WSAGetLastError() != WSAEWOULDBLOCK)
    return false;

  if(!WaitData(false, TimeoutMilliSeconds))
    return false;

  if(!SetBlockingMode(true))
    return false;
  return true;
}

bool BaseSOCKET::ConnectEx(const char *pServerName, int Port, int TimeoutMilliSeconds)
{
  assert(hSocket != INVALID_SOCKET);

  DWORD Ip;
  if(!GetIpAddr(pServerName, &Ip))
    return false;
  return ConnectEx(Ip, Port, TimeoutMilliSeconds);
}

bool BaseSOCKET::Bind(int Port, const char *pSocketAddress)
{
  assert(hSocket != INVALID_SOCKET);

  SOCKADDR_IN LocalSockAddr;
  memset(&LocalSockAddr, 0, sizeof(LocalSockAddr));

  LocalSockAddr.sin_family = AF_INET;
  if(pSocketAddress == NULL)
    LocalSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  else
  {
    DWORD Result = inet_addr(pSocketAddress);
    if (Result == INADDR_NONE)
      return false;
    LocalSockAddr.sin_addr.s_addr = Result;
  }
  LocalSockAddr.sin_port = htons(Port);
  return ::bind(hSocket, (struct sockaddr FAR *)&LocalSockAddr, sizeof(LocalSockAddr)) != SOCKET_ERROR;
}

// 异步选择
bool BaseSOCKET::AsyncSelect(HWND hWnd, UINT Msg, long Event)
{
  assert(hSocket != INVALID_SOCKET);
  if(!::IsWindow(hWnd) || Msg == 0)
    return false;

  return ::WSAAsyncSelect(hSocket, hWnd, Msg, Event) != SOCKET_ERROR;
}

// 事件选择
bool BaseSOCKET::EventSelect(WSAEVENT hEvent, long NetworkEvents)
{
  assert(hSocket != INVALID_SOCKET);
  return ::WSAEventSelect(hSocket, hEvent, NetworkEvents) != SOCKET_ERROR;
}

// 返回本地端口号。 如果错误，则返回 -1
int BaseSOCKET::GetLocalPort()
{
  assert(hSocket != INVALID_SOCKET);

  SOCKADDR_IN LocalSockAddr;
  int Len = sizeof(LocalSockAddr);
  if(getsockname(hSocket, (sockaddr *)&LocalSockAddr, &Len) == SOCKET_ERROR)
    return -1;   // Error

  return ntohs(LocalSockAddr.sin_port);
}

// 返回远端端口号。 如果错误，则返回 -1
int BaseSOCKET::GetRemotePort()
{
  assert(hSocket != INVALID_SOCKET);

  SOCKADDR_IN RemoteSockAddr;
  int Len = sizeof(RemoteSockAddr);
  if(getpeername(hSocket, (sockaddr *)&RemoteSockAddr, &Len) == SOCKET_ERROR)
    return -1;   // Error

  return ntohs(RemoteSockAddr.sin_port);
}

// 取得本地主机名
bool BaseSOCKET::GetLocalHostName(char *pHostNameBuf, int BufSize) // static
{
  assert(pHostNameBuf != NULL);
  if(pHostNameBuf == NULL)
    return false;

  return ::gethostname(pHostNameBuf, BufSize) != SOCKET_ERROR;
}

// 取得远端主机名
bool BaseSOCKET::GetRemoteHostName(char *pHostNameBuf, int BufSize)
{
  assert(hSocket != INVALID_SOCKET);
  assert(pHostNameBuf != NULL);
  if(pHostNameBuf == NULL)
    return false;

  SOCKADDR_IN RemoteSockAddr;
  int Len = sizeof(RemoteSockAddr);
  if(getpeername(hSocket, (sockaddr *)&RemoteSockAddr, &Len) == SOCKET_ERROR)
    return false;   // Error

  hostent *pHostent = gethostbyaddr((char *)&RemoteSockAddr.sin_addr.s_addr, 4 ,PF_INET);
  if(pHostent == NULL)
    return false;

  lstrcpyn(pHostNameBuf, pHostent->h_name, BufSize);
  return true;
}

bool BaseSOCKET::GetLocalIpStatic(char *pStrIpBuf, int BufSize) // static 
{
  char  HostName[128];
  if(gethostname(HostName, sizeof(HostName)) != 0)
    return false;

  HOSTENT *pHost = gethostbyname(HostName);
  if(pHost == NULL)
    return false;
   
  char *pIpStr = inet_ntoa(*(in_addr*)pHost->h_addr_list[0]);
  if(pIpStr == NULL)
    return false;

  lstrcpyn(pStrIpBuf, pIpStr, BufSize);
  return true;
}

// 取得本地 IP 地址
bool BaseSOCKET::GetLocalIp(char *pStrIpBuf, int BufSize)
{
  assert(hSocket != INVALID_SOCKET);
  assert(pStrIpBuf != NULL);
  if(pStrIpBuf == NULL)
    return false;

  SOCKADDR_IN LocalSockAddr;
  int Len = sizeof(LocalSockAddr);
  if(getsockname(hSocket, (sockaddr *)&LocalSockAddr, &Len) == SOCKET_ERROR)
    return false;   // Error

  LongToDotIp(LocalSockAddr.sin_addr.s_addr, pStrIpBuf, BufSize);
  return true;
}

// 取得远端 IP 地址
bool BaseSOCKET::GetRemoteIp(char *pStrIpBuf, int BufSize)
{
  assert(pStrIpBuf != NULL);
  assert(hSocket != INVALID_SOCKET);
  if(pStrIpBuf == NULL)
    return false;

  SOCKADDR_IN RemoteSockAddr;
  int Len = sizeof(RemoteSockAddr);
  if(getpeername(hSocket, (sockaddr *)&RemoteSockAddr, &Len) == SOCKET_ERROR)
    return false;   // Error

  LongToDotIp(RemoteSockAddr.sin_addr.s_addr, pStrIpBuf, BufSize);
  return true;
}

}

