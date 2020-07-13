#ifndef __NETSVR_H__
#define __NETSVR_H__
#pragma once

#include "socket.h"
#include "thread.h"
#include "mtstring.h"

using namespace minilib;

class CNetServer;

class CServerThread : public CBaseThread
{
protected:
    BaseSOCKET    TheSocket;
    SOCKADDR_IN   ClientAddr;
    int           ClientAddrLen;
    CNetServer   *pServer;
    BOOL          bDeleteThis;

public:
    CServerThread();
    ~CServerThread();

protected:
    virtual int   ServerProc();

private:
    virtual int   RunProc(); 

    friend class CNetServer;//::RunProc(); 
};

class CNetServer : protected CBaseThread
{
public:
    CNetServer();
    virtual ~CNetServer();

public:
    BOOL                Init(int Port, int MaxThreadNum = 50);
    BOOL                StartServer();
    BOOL                StopServer();
    BOOL                IsServerRunning();

    LONG                GetClientThreadNum();
    const TCHAR        *GetLastErrStr();

protected:
    virtual CServerThread *CreateServerThread();
    virtual void        OnServerStarted() {};
    virtual void        OnServerClosed() {};
    virtual void        OnServerTimestep() {};

protected:
    BaseSOCKET          ListenSocket;   // 监听的Socket
    UINT                ListenPort;     // 监听的端口
    WSAEVENT            hEventSocket;   // socket事件
    WSAEVENT            hEventExit;     // 退出事件
    MtSTRING            LastErrStr;     // 错误原因描述
    volatile LONG       ThreadNum;      // 线程数
    LONG                MaxThreadNum;   // 最大线程数
    UINT                WaitTimestep;   // 

private:
    virtual int   RunProc(); 

    friend class CServerThread;
};

inline const TCHAR *CNetServer::GetLastErrStr()
{
    return (LPCTSTR)LastErrStr;
}

inline LONG CNetServer::GetClientThreadNum()
{
    return ThreadNum;
}

inline BOOL CNetServer::IsServerRunning()
{
    return ListenSocket.IsValidSocket();
}

#endif // __NETSVR_H__