//***********************************************
// 功　　能: ISAPI推技术聊天室演示
// 创建日期: baojian 2007.11.15
// 更新历史: 
//    2007.11.19 
//      1. 修正了浏览器连续刷新可能会导致死锁的Bug
//      2. 修正了g_hEventCmd资源泄露
//***********************************************

#include "stdafx.h"
#include "HttpParse.h"
#include "critsect.H"

enum CmdTYPE
{
    CT_CHAT,
    CT_SHUTDOWN,
};

struct ChatCMD
{
    CmdTYPE CmdType;
    string  ChatData;
};
typedef list<ChatCMD> ChatCmdLIST;

struct UserINFO
{
    EXTENSION_CONTROL_BLOCK *pECB;
    string          UserName;
};
typedef list<UserINFO> UserLIST;

static UserLIST     g_UserList;     // 在线用户列表
static ChatCmdLIST  g_ChatCmdList;  // 聊天消息队列
static CCriticalSect g_UserLock;     // g_UserList的锁
static CCriticalSect g_ChatCmdLock;  // g_ChatCmdList的锁
static HANDLE       g_hEventCmd;    // 命令事件
static HANDLE       g_hThreadChat;  // 主聊天线程
static HANDLE       g_hModule;      // dll instance

BOOL WINAPI GetExtensionVersion(OUT HSE_VERSION_INFO *pVer)
{
    pVer->dwExtensionVersion = MAKELONG(HSE_VERSION_MINOR, HSE_VERSION_MAJOR);
    strcpy(pVer->lpszExtensionDesc, "IsaChat");
    return TRUE;
}

static void SendDataToAllUserImpl(const char *pStr)
{
    CritSectOwner Lock(g_UserLock);

    DWORD Len = (DWORD)strlen(pStr);
    UserLIST::iterator Iter = g_UserList.begin(); 
    while(Iter != g_UserList.end())
    {
        EXTENSION_CONTROL_BLOCK *pECB = (*Iter).pECB;
        if(!pECB->WriteClient(pECB->ConnID, (void *)pStr, &Len, HSE_IO_SYNC))
        {
            pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_DONE_WITH_SESSION, NULL, NULL, NULL);
            Iter = g_UserList.erase(Iter);
        }
        else
        {
            Iter++;
        }
    }
}

static void AddChatCmd(const ChatCMD &ChatCmd)
{
    CritSectOwner Lock(g_ChatCmdLock);
    g_ChatCmdList.push_back(ChatCmd);
    SetEvent(g_hEventCmd);
}

static void SendDataToAllUser(const char *pUser, const char *pMsg)
{
    char DataStr[1024] = { 0 };

    SYSTEMTIME T;
    ::GetLocalTime(&T);
    _snprintf(DataStr, 1023, "(%02d:%02d:%02d) <font color=#0000ff>%s</font>说: %s<br>\r\n", 
        T.wHour, T.wMinute, T.wSecond, pUser, pMsg);

    ChatCMD ChatCmd;
    ChatCmd.CmdType = CT_CHAT;
    ChatCmd.ChatData = DataStr;
    AddChatCmd(ChatCmd);
}

static void CloseAllUser()
{
    CritSectOwner Lock(g_UserLock);

    UserLIST::iterator Iter;
    for(Iter = g_UserList.begin(); Iter != g_UserList.end(); Iter++)
    {
        EXTENSION_CONTROL_BLOCK *pECB = (*Iter).pECB;
        pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_DONE_WITH_SESSION, NULL, NULL, NULL);
    }
    g_UserList.clear();
}

static DWORD GetUserCount()
{
    CritSectOwner Lock(g_UserLock);
    return (DWORD)g_UserList.size();
}

static BOOL GetOneChatCmd(ChatCMD &ChatCmd)
{
    CritSectOwner Lock(g_ChatCmdLock);
    if(g_ChatCmdList.empty())
        return FALSE;

    ChatCmd = g_ChatCmdList.front();
    g_ChatCmdList.pop_front();
    return TRUE;
}

static unsigned int __stdcall IsaChatThreadFunc(void *pData)
{
    while(1)
    {
        DWORD Result = ::WaitForSingleObject(g_hEventCmd, 2000);
        if(Result == WAIT_OBJECT_0)
        {
            while(1)
            {
                ChatCMD ChatCmd;
                
                if(!GetOneChatCmd(ChatCmd))
                    break;

                if(ChatCmd.CmdType == CT_SHUTDOWN)
                {
                    CloseAllUser();
                    return 0;
                }
                else if(ChatCmd.CmdType == CT_CHAT)
                {
                    SendDataToAllUserImpl(ChatCmd.ChatData.c_str());
                }
                else
                {
                    assert(false);
                }
            }
        }
        else if(Result == WAIT_TIMEOUT)
        {
            static int i = 0;
            i++;
            if(i > 1000)
                i = 0;
            char TempStr[1024];
            sprintf(TempStr, "Hello IsaChat %d...(UserCount=%d)", i, GetUserCount());
            SendDataToAllUser("系统", TempStr);
        }
        else
        {
            assert(false);
            return 0;
        }
    }
}

static void AddNewUser(EXTENSION_CONTROL_BLOCK *pECB, const string &UserName)
{
    do 
    {
        CritSectOwner Lock(g_UserLock);
        UserINFO NewUser;
        NewUser.pECB = pECB;
        NewUser.UserName = UserName;
        g_UserList.push_back(NewUser);
    } while(0);

    char TempStr[1024];
    sprintf(TempStr, "%s进入了聊天室，当前用户数%d个", UserName.c_str(), GetUserCount());
    SendDataToAllUser("系统", TempStr);
}

// 读取一个文件内容，注意用 free 函数释放返回的内存
static BYTE *GetFileData(const TCHAR *pFileName, DWORD &FileSize)
{
    assert(pFileName != NULL);
    HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, 
        OPEN_EXISTING, 0, NULL);
    if(hFile == INVALID_HANDLE_VALUE) 
        return NULL;

    FileSize = ::GetFileSize(hFile, NULL);
    BYTE *pFileData = (BYTE *)malloc(FileSize + 1);
    assert(pFileData != NULL);
    ::ReadFile(hFile, pFileData, FileSize, &FileSize, NULL);
    ::CloseHandle(hFile);

    *(pFileData + FileSize) = '\0';
    return pFileData;
}

static BYTE *GetChatResFileData(const TCHAR *pFileName, DWORD &FileSize)
{
    char ResFileName[300];
    ::GetModuleFileName((HMODULE)g_hModule, ResFileName, 300);
    strcpy(strrchr(ResFileName, '\\') + 1, pFileName);
    return GetFileData(ResFileName, FileSize);
}

DWORD WINAPI HttpExtensionProc(IN EXTENSION_CONTROL_BLOCK *pECB)
{
    // Parse
    CHttpContext HttpCtx(pECB);
    if(!HttpCtx.ParseHttpData())
        return HSE_STATUS_ERROR;

    char ResultStr[1024];
    DWORD Len = 0;

    // get func
    const char *pFunc = HttpCtx.GetGetList().GetValueByName("func");
    if(pFunc == NULL)
    {
        Len = sprintf(ResultStr, "not find func");
    }
    else if(strcmp(pFunc, "recvmsg") == 0)
    {
        pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER,
            "200 OK", NULL, (DWORD *)"Content-Type: text/html\r\n\r\n");

        DWORD FileSize;
        BYTE *pChatHead = GetChatResFileData("chathead.htm", FileSize);
        assert(pChatHead != NULL);
        if(pChatHead != NULL)
        {
            pECB->WriteClient(pECB->ConnID, (void *)pChatHead, &FileSize, HSE_IO_SYNC);
            free(pChatHead);
        }

        const char *pUser = HttpCtx.GetGetList().GetValueByName("user");
        if(pUser == NULL || *pUser == 0)
            pUser = "Guest";

        AddNewUser(pECB, pUser);
        return HSE_STATUS_PENDING;
    }
    else if(strcmp(pFunc, "sendmsg") == 0)
    {
        const char *pUser = HttpCtx.GetPostList().GetValueByName("user");
        const char *pMsg = HttpCtx.GetPostList().GetValueByName("msg");
        if(pUser && *pUser && pMsg && *pMsg)
        {
            SendDataToAllUser(pUser, pMsg);

            sprintf(ResultStr, "isachat.dll?func=sendpage&user=%s", pUser);
            pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_SEND_URL_REDIRECT_RESP,
                ResultStr, NULL, NULL);
            return HSE_STATUS_SUCCESS;
        }
        else
        {
            Len = sprintf(ResultStr, "no data");
        }
    }
    else if(strcmp(pFunc, "index") == 0)
    {
        const char *pUser = HttpCtx.GetGetList().GetValueByName("user");
        if(pUser == NULL || *pUser == 0)
            pUser = "Guest";
        Len = sprintf(ResultStr, 
            "<html><head><title>IsaChat聊天室</title></head>\r\n"
            "<frameset rows=\"*,100\">\r\n"
            "<frame name=\"top\" src=\"isachat.dll?func=recvmsg&user=%s\">\r\n"
            "<frame name=\"bottom\" src=\"isachat.dll?func=sendpage&user=%s\">\r\n"
            "</frameset><html>\r\n", pUser, pUser);
    }
    else if(strcmp(pFunc, "sendpage") == 0)
    {
        const char *pUser = HttpCtx.GetGetList().GetValueByName("user");
        if(pUser == NULL || *pUser == 0)
            pUser = "Guest";
        Len = sprintf(ResultStr, 
            "<html><body onLoad=\"document.chatform.msg.focus();\">\r\n"
            "<form name=\"chatform\" action=\"isachat.dll?func=sendmsg\" method=POST>\r\n"
            "<input type=\"hidden\" name=\"user\" value=\"%s\">\r\n"
            "<input type=\"text\" name=\"msg\" size=45>\r\n"
            "<input type=\"submit\" value=\"发送\">\r\n"
            "</form></body></html>\r\n", pUser);
    }
    else
    {
        Len = sprintf(ResultStr, "invalid func: %s", pFunc);
    }

    pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER,
        "200 OK", NULL, (DWORD *)"Content-Type: text/html\r\n\r\n");

    pECB->WriteClient(pECB->ConnID, (void *)ResultStr, &Len, HSE_IO_SYNC);
    return HSE_STATUS_SUCCESS;
}

BOOL WINAPI TerminateExtension(IN DWORD dwFlags)
{
    ChatCMD ChatCmd;
    ChatCmd.CmdType = CT_SHUTDOWN;
    AddChatCmd(ChatCmd);

    DWORD Result = ::WaitForSingleObject(g_hThreadChat, 1000);
    if(Result != WAIT_OBJECT_0)
        ::TerminateThread(g_hThreadChat, 0);
    ::CloseHandle(g_hThreadChat);
    return TRUE;
}

static void OnProcessAttach(HANDLE hModule)
{
    g_hModule = hModule;
    g_hEventCmd = ::CreateEvent(NULL, FALSE, FALSE, NULL);

    unsigned int ThreadID = 0;
    g_hThreadChat = (HANDLE)::_beginthreadex(0, 0, IsaChatThreadFunc, (void*)NULL, 0, &ThreadID);
}

static void OnProcessDetach()
{
    ::CloseHandle(g_hEventCmd);
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch(dwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            LOG_INFO("----DLL_PROCESS_ATTACH");
            OnProcessAttach(hModule);
            break;
        }
        case DLL_PROCESS_DETACH:
        {
            LOG_INFO("----DLL_PROCESS_DETACH");
            OnProcessDetach();
            break;
        }
        case DLL_THREAD_ATTACH:
        {
            LOG_INFO("----DLL_THREAD_ATTACH");
            break;
        }
        case DLL_THREAD_DETACH:
        {
            LOG_INFO("----DLL_THREAD_DETACH");
            break;
        }
    }
    return TRUE;
}