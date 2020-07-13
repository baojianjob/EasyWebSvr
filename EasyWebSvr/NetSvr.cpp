#include "stdafx.h"
#include "netsvr.h"

#define DEFAULT_SERVER_PORT 5050

CNetServer::CNetServer()
{
    BaseSOCKET::SocketInit();

    ThreadNum     = 0;
    MaxThreadNum  = 64;
    ListenPort    = DEFAULT_SERVER_PORT;
    WaitTimestep  = 5000;
    hEventSocket  = ::WSACreateEvent();
    hEventExit    = ::WSACreateEvent();
}

CNetServer::~CNetServer()
{
    ::WSACloseEvent(hEventSocket);
    ::WSACloseEvent(hEventExit);

    BaseSOCKET::SocketTerm();
}

BOOL CNetServer::Init(int Port, int MaxThreadNum)
{
    assert(Port >= 1 && Port <= 65535);
    if(Port <= 0 || Port > 65535)
    {
        LastErrStr = _T("端口必须在1--65535之间");
        return FALSE;
    }
    assert(MaxThreadNum >= 0);

    ListenPort = Port;
    this->MaxThreadNum = MaxThreadNum;
    return TRUE;
}

BOOL CNetServer::StartServer()
{
    if(ListenSocket.IsValidSocket())
    {
        LastErrStr = _T("服务器已经启动");
        return FALSE;
    }

    if(!ListenSocket.Create())
    {
        LastErrStr = _T("Socket Create Error");
        return FALSE;
    }

    if(!ListenSocket.Bind(ListenPort))
    {
        //LastErrStr = _T("Bind Port Error");
        //LastErrStr.Format(_T("绑定到%d端口失败"), ListenPort);
        LastErrStr.Format(_T("请检查%d端口是否被占用"), ListenPort);
        ListenSocket.Close();
        return FALSE;
    }

    if(!ListenSocket.Listen())
    {
        LastErrStr = _T("Listen Error");
        ListenSocket.Close();
        return FALSE;
    }

    if(!ListenSocket.EventSelect(hEventSocket, FD_ACCEPT | FD_CLOSE))
    {
        LastErrStr = _T("EventSelect Error");
        ListenSocket.Close();
        return FALSE;
    }

    OnServerStarted();

    if(!Start())
        return FALSE;

    return TRUE;
}

int CNetServer::RunProc()
{
    while(1)
    {
        WSAEVENT EventArray[2] = { hEventExit, hEventSocket};
        DWORD Ret = ::WSAWaitForMultipleEvents(2, EventArray, FALSE, WaitTimestep, FALSE);
        if(Ret == WSA_WAIT_FAILED)
        {
            assert(false);
            break;
        }
        else if(Ret == WSA_WAIT_TIMEOUT)
        {
            OnServerTimestep();
            continue;
        }

        int Index = Ret - WSA_WAIT_EVENT_0;
        if(Index == 0)
            break;

        assert(Index == 1);
        WSANETWORKEVENTS NetEvents;
        Ret = ::WSAEnumNetworkEvents(ListenSocket.GetHandle(), EventArray[Index], &NetEvents);
        if(Ret == SOCKET_ERROR)
        {
            assert(false);
            break;
        }
        else if(NetEvents.lNetworkEvents & FD_ACCEPT)
        {
            if(NetEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
            {
                assert(false);
                continue;
            }

            CServerThread *pServerThread = CreateServerThread(); // new connected thread
            if(!ListenSocket.Accept(pServerThread->TheSocket, 
                (SOCKADDR *)&(pServerThread->ClientAddr), &(pServerThread->ClientAddrLen)))
            {
                assert(false);
                delete pServerThread;
                continue;
            }

            if(MaxThreadNum > 0 && ThreadNum >= MaxThreadNum)
            {
                delete pServerThread;
                continue;
            }

            pServerThread->pServer = this;
            pServerThread->Start();
        }
        else if(NetEvents.lNetworkEvents & FD_CLOSE)
        {
        }
    }

    OnServerClosed();
    return 0;
}

BOOL CNetServer::StopServer()
{
    if(ListenSocket.GetHandle() == INVALID_SOCKET)
    {
        LastErrStr = "服务器没有启动";
        return FALSE;
    }

    ::SetEvent(hEventExit);
    if(!Wait(2000))
    {
        assert(false);
        Terminate();
    }
    ResetEvent(hEventExit);

    ListenSocket.Close();
    return TRUE;
}

CServerThread *CNetServer::CreateServerThread()
{
    return new CServerThread;
}

///////////////////////////////////////////////////////////////////////////////

CServerThread::CServerThread()
{
    pServer       = NULL;
    bDeleteThis   = TRUE;
    ClientAddrLen = sizeof(ClientAddr);
    memset(&ClientAddr, 0, sizeof(ClientAddr));
}

CServerThread::~CServerThread()
{
}

int CServerThread::ServerProc()
{
    return 0;
}

int CServerThread::RunProc()
{
#if _MSC_VER < 1300
    InterlockedIncrement((LONG *)(&(pServer->ThreadNum)));
#else
    InterlockedIncrement(&(pServer->ThreadNum));
#endif

    LOG_INFO(GID_NETSERVER, "CServerThread::RunProc() Begin.");
    int Ret = ServerProc();
    LOG_INFO(GID_NETSERVER, "CServerThread::RunProc() End.");

#if _MSC_VER < 1300
    InterlockedDecrement((LONG *)(&(pServer->ThreadNum)));
#else
    InterlockedDecrement(&(pServer->ThreadNum));
#endif
    if(bDeleteThis)
        delete this;
    return Ret;
}
