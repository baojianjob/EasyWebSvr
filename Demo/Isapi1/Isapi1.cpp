#include "stdafx.h"
#include <httpext.h>
#include <process.h>

//#ifdef __cplusplus
//#define EXPORT extern "C" __declspec(dllexport)
//#else
//#define EXPORT __declspec(dllexport)
//#endif 

//////////////////////////////////////////////////////////////////////////

class base_log_device
{
public:
    virtual void put(const char *pLog, int Len) = 0;
};

class log_device_logview : public base_log_device
{
public:
    log_device_logview();
    ~log_device_logview();

    virtual void put(const char *pLog, int Len);

private:
    HWND m_hWndLogView;
};

log_device_logview::log_device_logview()
{
    m_hWndLogView = ::FindWindow("Coobol_LogView", NULL);
    if(m_hWndLogView != NULL)
    {
        if(::IsIconic(m_hWndLogView)) 
            ::ShowWindow(m_hWndLogView, SW_RESTORE);
        SetForegroundWindow(m_hWndLogView);
    }
}

log_device_logview::~log_device_logview()
{
}

void log_device_logview::put(const char *pLog, int Len)
{
    if(m_hWndLogView == NULL)
        return;

    COPYDATASTRUCT Data;
    Data.dwData = 0;
    Data.lpData = (void *)pLog;
    Data.cbData = Len;
    ::SendMessage(m_hWndLogView, WM_COPYDATA, 0, (LPARAM)&Data);
}

class log_device_manager : public base_log_device
{
public:
    static log_device_manager &instance();

    void put(int module, const char *pLog);

private:
    virtual void put(const char *pLog, int Len);

private:
    log_device_logview m_logview;
};

log_device_manager &log_device_manager::instance()
{
    static log_device_manager _instance;
    return _instance;
}

// (3628) 10:31:35.234000 RelayPool: echo from relay node 62.195.191.175:1329 for 1859
void log_device_manager::put(const char *pLog, int Len)
{
    m_logview.put(pLog, Len);
}

const char *GetModuleName(int module)
{
    //if(module == GID_WEBSERVER)
    //    return "VuiSDK";
    return "LogView";
}

void log_device_manager::put(int module, const char *pLog)
{
    SYSTEMTIME T;
    ::GetLocalTime(&T);

    char Buffer[4096];
    assert(strlen(pLog) <= 4000);
    int Len = wsprintf(Buffer, "(%d) %02d:%02d:%02d.%03d000 %s: %s", ::GetCurrentThreadId(), 
        T.wHour, T.wMinute, T.wSecond, T.wMilliseconds,
        GetModuleName(module), pLog);
    this->put(Buffer, Len);
}

void OutputDebugInfo(int module, const TCHAR *pFormat, ...)
{
    va_list ArgList;            // 等价于 char *ArgList;
    va_start(ArgList, pFormat); // 给 ArgList 赋值，等价于 ArgList = (char *)(&pFormat) + sizeof(void *);

    char Buffer[1024];
    _vsntprintf(Buffer, 1024, pFormat, ArgList); //  Format 后字符串最大长度为 1023

    va_end(ArgList);            // 等价于 ArgList = NULL;

    log_device_manager::instance().put(module, Buffer);
}

//////////////////////////////////////////////////////////////////////////

//#define _LOGVIEW
#define _LOGDEBUG

#if defined _LOGVIEW
  #define LOG_INFO(x) OutputDebugInfo(0, x)
#elif defined _LOGDEBUG
  #define LOG_INFO(x) OutputDebugString(x)
#else
  #if _MSC_VER >= 1300
    #define LOG_INFO(x) __noop  
  #else
    #define LOG_INFO(x) ((void)0)
  #endif
#endif

HANDLE g_hModule;
BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  dwReason, 
                      LPVOID lpReserved)
{
    switch(dwReason)
    {
    case DLL_PROCESS_ATTACH:
        g_hModule = hModule;
        LOG_INFO("----DLL_PROCESS_ATTACH");
        break;
    case DLL_PROCESS_DETACH:
        LOG_INFO("----DLL_PROCESS_DETACH");
        break;
    case DLL_THREAD_ATTACH:
        LOG_INFO("----DLL_THREAD_ATTACH");
        break;
    case DLL_THREAD_DETACH:
        LOG_INFO("----DLL_THREAD_DETACH");
        break;
    }
    return TRUE;
}

BOOL WINAPI GetExtensionVersion(OUT HSE_VERSION_INFO * pVer)
{
    LOG_INFO("----GetExtensionVersion");

    pVer->dwExtensionVersion = MAKELONG(HSE_VERSION_MINOR, HSE_VERSION_MAJOR);
    strcpy(pVer->lpszExtensionDesc, "ISAPI Hello World");
    return TRUE;
}

void GetCalcParam(DWORD &MaxValue, BOOL &bFixLen)
{
    char FileName[300];
    ::GetModuleFileName((HMODULE)g_hModule, FileName, 300);
    strcpy(strrchr(FileName, '\\') + 1, "param.txt");

    MaxValue = 100;
    bFixLen = FALSE;

    FILE *pFile = fopen(FileName, "r");
    if(pFile == NULL)
        return;
    fscanf(pFile, "%d%d", &MaxValue, &bFixLen);
    if(MaxValue > 100000)
        MaxValue = 100000;
    fclose(pFile);
}

#define SHOW_VAR(x) \
    { \
        pVarName = x; \
        BufSize = sizeof(TempStr) - 1; \
        TempStr[0] = 0; \
        BOOL bRet = pECB->GetServerVariable(pECB->ConnID, pVarName, TempStr, &BufSize); \
        Len += wsprintf(ResultStr + Len, "<pre>%s = (%d)%s</pre>\r\n", pVarName, bRet, TempStr); \
    }


unsigned int __stdcall IsapiThreadFunc(void *pData)
{
    EXTENSION_CONTROL_BLOCK *pECB = (EXTENSION_CONTROL_BLOCK *)pData;
    assert(pECB != NULL);

    char TempStr[1024];
    for(int i = 0; i < 100; i++)
    {
        Sleep(20);
        LOG_INFO("IsapiThreadFunc...");
        DWORD Len = sprintf(TempStr, "test %d<br>\r\n", i);
        pECB->WriteClient(pECB->ConnID, (void *)TempStr, &Len, 0);
    }
    DWORD Status = HSE_STATUS_SUCCESS;
    pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &Status, NULL, NULL);
    LOG_INFO("IsapiThreadFunc over");
    return 0;
}

#define TEST_FUNC  1

DWORD WINAPI HttpExtensionProc(IN EXTENSION_CONTROL_BLOCK *pECB)
{
#if (TEST_FUNC == 1)

    const char *pBodyStr = 
        "<html><head><title>ISAPI Demo</title></head>\r\n"
        "<body bgcolor=\"#000000\" text=\"#FFFFFF\">\r\n"
        "<br><center><h1>Hello World, ISAPI!</h1></center>\r\n"
        "</body></html>";
    DWORD BodyLen = strlen(pBodyStr);

    // HeadStr
    char HeadStr[256];
    wsprintf(HeadStr, "Content-Type: text/html\r\nContent-Length: %d\r\n\r\n", BodyLen);
    pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER,
        "200 OK", NULL, (DWORD *)HeadStr);

    // Body
    if(stricmp(pECB->lpszMethod, "HEAD") != 0)
        pECB->WriteClient(pECB->ConnID, (void *)pBodyStr, &BodyLen, HSE_IO_SYNC);
    return HSE_STATUS_SUCCESS;

#elif (TEST_FUNC == 2)

    pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_SEND_URL_REDIRECT_RESP,
        "http://www.baidu.com", NULL, NULL);

    return HSE_STATUS_SUCCESS;

#elif (TEST_FUNC == 3)

    pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER,
        "200 OK", NULL, (DWORD *)"Content-Type: text/html\r\n\r\n");

    const char *pFormatStr = "<html><body>threadid = %05d</body></html>";
    char ResultStr[1024];
    DWORD ThreadId = GetCurrentThreadId();
    DWORD Len = wsprintf(ResultStr, pFormatStr, ThreadId);
    pECB->WriteClient(pECB->ConnID, (void *)ResultStr, &Len, HSE_IO_SYNC);
    return HSE_STATUS_SUCCESS;

#elif (TEST_FUNC == 4)

    LOG_INFO("----HttpExtensionProc");

    pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER,
        "200 OK", NULL, (DWORD *)"Content-Type: text/html\r\n\r\n");

    char ResultStr[81920], TempStr[8192], *pVarName;
    DWORD BufSize, Len;
    strcpy(ResultStr, "<html><body>\r\n");
    Len = strlen(ResultStr);

    SHOW_VAR("SERVER_SOFTWARE");
    SHOW_VAR("QUERY_STRING");
    SHOW_VAR("CONTENT_LENGTH");
    {
        Len += wsprintf(ResultStr + Len, "<pre>cbTotalBytes = %d\r\n"
            "cbAvailable = %d\r\n"
            "lpbData = %s </pre>\r\n", 
            pECB->cbTotalBytes, pECB->cbAvailable, pECB->lpbData);
    }

    SHOW_VAR("PATH_INFO");
    SHOW_VAR("SCRIPT_NAME");

    SHOW_VAR("REMOTE_USER");
    SHOW_VAR("LOGON_USER");

    SHOW_VAR("ALL_HTTP");
    SHOW_VAR("PATH_TRANSLATED");
    SHOW_VAR("REQUEST_URI");
    SHOW_VAR("URL");
    SHOW_VAR("HTTP_AUTHORIZATION");
    SHOW_VAR("SERVER_PORT_SECURE");
    SHOW_VAR("GATEWAY_INTERFACE");

    SHOW_VAR("APPL_MD_PATH");
    SHOW_VAR("APPL_PHYSICAL_PATH");

    SHOW_VAR("INSTANCE_ID");
    SHOW_VAR("INSTANCE_META_PATH");

    SHOW_VAR("AUTH_PASSWORD");
    SHOW_VAR("AUTH_TYPE");
    SHOW_VAR("AUTH_USER");

    SHOW_VAR("CERT_COOKIE");
    SHOW_VAR("CERT_FLAGS");
    SHOW_VAR("CERT_ISSUER");
    SHOW_VAR("CERT_KEYSIZE");
    SHOW_VAR("CERT_SECRETKEYSIZE");
    SHOW_VAR("CERT_SERIALNUMBER");
    SHOW_VAR("CERT_SERVER_ISSUER");
    SHOW_VAR("CERT_SERVER_SUBJECT");
    SHOW_VAR("CERT_SUBJECT");

    SHOW_VAR("HTTPS");
    SHOW_VAR("HTTPS_KEYSIZE");
    SHOW_VAR("HTTPS_SECRETKEYSIZE");
    SHOW_VAR("HTTPS_SERVER_ISSUER");
    SHOW_VAR("HTTPS_SERVER_SUBJECT");

    Len += wsprintf(ResultStr + Len, "</body></html>");
    assert(Len < sizeof(ResultStr));
    pECB->WriteClient(pECB->ConnID, (void *)ResultStr, &Len, HSE_IO_SYNC);
    return HSE_STATUS_SUCCESS;

#elif (TEST_FUNC == 5)

    DWORD MaxValue;
    BOOL bFixLen;
    GetCalcParam(MaxValue, bFixLen);
    const char *pTimeFormat = bFixLen ? "共耗时%05d毫秒<hr>\r\n" : "共耗时%d毫秒<hr>\r\n";

    const DWORD BufSize = MaxValue * 30 + 1024;
    char *pDataBuf = (char *)malloc(BufSize);

    const DWORD PreBufLen = 100;
    char *pResultStr = pDataBuf + PreBufLen;

    // Data
    DWORD Len = 0, Count = 0;
    DWORD T1 = GetTickCount();
    for(DWORD i = 1; i <= MaxValue; i++)
    {
        Count += i;
        Len += wsprintf(pResultStr + Len, "从1加到%d=%d<br>\r\n", i, Count);
    }
    DWORD T2 = GetTickCount();

    Len += wsprintf(pResultStr + Len, "</body></html>");
    assert(Len < BufSize - PreBufLen - 1);

    // PreData
    DWORD PreDataLen = wsprintf(pDataBuf, "<html><body>");
    PreDataLen += wsprintf(pDataBuf + PreDataLen, pTimeFormat, T2 - T1);
    assert(PreDataLen < PreBufLen);
    Len += PreDataLen;
    pResultStr = pDataBuf + PreBufLen - PreDataLen;
    memmove(pResultStr, pDataBuf, PreDataLen);

    // Header
    char HeadStr[256];
    wsprintf(HeadStr, "Content-Type: text/html\r\nContent-Length: %d\r\n\r\n", Len);
    pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER,
        "200 OK", NULL, (DWORD *)HeadStr);

    assert(strlen(pResultStr) == Len);
    pECB->WriteClient(pECB->ConnID, (void *)pResultStr, &Len, HSE_IO_SYNC);

    free(pDataBuf);
    return HSE_STATUS_SUCCESS;

#elif (TEST_FUNC == 6)

    //const char *pBodyStr = 
    //"<html><head><title>ISAPI Demo</title></head>\r\n"
    //"<body bgcolor=\"#000000\" text=\"#FFFFFF\">\r\n"
    //"<br><center><h1>Hello World ISAPI!</h1></center>\r\n"
    //"</body></html>";
    //DWORD BodyLen = strlen(pBodyStr);

    LOG_INFO("HttpExtensionProc1");
    if(strnicmp(pECB->lpszQueryString, "func=recv", 9) == 0)
    {
    }
    else if(strnicmp(pECB->lpszQueryString, "func=send", 9) == 0)
    {
    }
    // HeadStr
    pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER,
                                "200 OK", NULL, (DWORD *)"Content-Type: text/html\r\n\r\n");

    //unsigned int ThreadID = 0;
    //HANDLE hThread = (HANDLE)::_beginthreadex(0, 0, IsapiThreadFunc, (void*)pECB, 0, &ThreadID);
    //::CloseHandle(hThread);
    char TempStr[1024];
    DWORD Len = sprintf(TempStr, "%s", pECB->lpszQueryString);

    pECB->WriteClient(pECB->ConnID, (void *)TempStr, &Len, HSE_IO_SYNC);

    LOG_INFO("HttpExtensionProc2");
    return HSE_STATUS_SUCCESS;
    //return HSE_STATUS_PENDING;

#elif (TEST_FUNC == 7)

    // HeadStr
    pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER,
                            "200 OK", NULL, (DWORD *)"Content-Type: text/html\r\n\r\n");

    {
        DWORD Size = 261;
        char UrlStr[300] = "/hello.pl2";
        BOOL bRet = pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_MAP_URL_TO_PATH, UrlStr, &Size, NULL);
    }

    {
        HSE_URL_MAPEX_INFO UMI;
        DWORD Size = 300;
        char UrlStr[300] = "/webtest/hello123.pl2";
        BOOL bRet = pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_MAP_URL_TO_PATH_EX, UrlStr, &Size, (DWORD *)&UMI);

        char TempStr[1024];
        DWORD Len = wsprintf(TempStr, "bRet=%d\r\nPath=%s\r\nPathLen=%d\r\nUrlLen=%d\r\nFlags=%d\r\n\r\n", 
            bRet, UMI.lpszPath, UMI.cchMatchingPath, UMI.cchMatchingURL, UMI.dwFlags);
        pECB->WriteClient(pECB->ConnID, (void *)TempStr, &Len, HSE_IO_SYNC);
    }

    {
        DWORD ScriptSize=300;
        char ScriptName[300];
        BOOL bRet = pECB->GetServerVariable(pECB->ConnID, "SCRIPT_NAME", ScriptName, &ScriptSize);

        char TempStr[1024];
        DWORD Len = wsprintf(TempStr, "bRet=%d\r\nScriptName=%s\r\nScriptLen=%d\r\n\r\n", 
            bRet, ScriptName, ScriptSize);
        pECB->WriteClient(pECB->ConnID, (void *)TempStr, &Len, HSE_IO_SYNC);
    }

    {
        char CurrentDir[300];
        ::GetCurrentDirectory(300, CurrentDir);

        char TempStr[1024];
        DWORD Len = wsprintf(TempStr, "CurrentDir=%s", CurrentDir);
        pECB->WriteClient(pECB->ConnID, (void *)TempStr, &Len, HSE_IO_SYNC);
    }
    return HSE_STATUS_SUCCESS;

#else
    #error not code.
#endif;
}

BOOL WINAPI TerminateExtension(IN DWORD dwFlags)
{
    LOG_INFO("----TerminateExtension");
    return TRUE;
}
