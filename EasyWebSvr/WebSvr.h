#ifndef __WEBSVR_H__
#define __WEBSVR_H__
#pragma once

#include <httpext.h>
#include "baseio.h"
#include "tlist.h"
#include "strlist.h"
#include "CritSect.h"
#include "netsvr.h"

using namespace minilib;
/*
http://wbj/cgi1.exe/abc?def
/cgi1.exe/abc?def
SCRIPT_NAME=/cgi1.exe
PATH_INFO=/abc
PATH_TRANSLATED=e:\inetpub\wwwroot\abc
QUERY_STRING=def
*/
class CRequestData
{
public:
    BOOL        bValidate;              // http请求是否有效(是否解析成功)
    MtSTRING    RequestMethod;          // 请求方法 GET/POST/HEAD
    MtSTRING    WebFileName;            // 网页文件名
    MtSTRING    ScriptName;
    MtSTRING    LocalFileName;          // 转换后的本地文件名
    MtSTRING    PathInfo;
    DWORD       ContentLength;  
    MtSTRING    QueryString;
    MtSTRING    CookieString;
    MtSTRING    ContentType;
    MtSTRING    SoapAction;
    MtSTRING    HostName;
    MtSTRING    UserAgent;
    MtSTRING    Referer;
    MtSTRING    Authorization;
    BYTE       *pRawData;
    DWORD       RawDataLen;
    BYTE       *pContentData;

    SYSTEMTIME  IfModifiedSince;        // If-Modified-Since
    __int64     RangeBegin;
    __int64     RangeEnd;
    BYTE        bIfModifiedSinceValid;  // 指定IfModifiedSince是否有效
    BYTE        bRangeValid;            // 指定RangeBegin和RangeEnd是否有效
    BYTE        bKeepAlive;             // KeepAlive
    BYTE        Reserve;                // 保留

public:
    CRequestData();
    ~CRequestData();

public:
    void        SetEmpty();
};

enum HttpACT
{
    HTTP_ACT_SENDFILE,
    HTTP_ACT_EXECUTE,
    HTTP_ACT_BROWSEDIR,
};

#define MAX_ENGINE_EXTNAME 32

class CScriptEngine
{
public:
    MtSTRING    ExtName;
    MtSTRING    CommandLine;

public:
    CScriptEngine() {};
    CScriptEngine(const MtSTRING &Name, const MtSTRING &Path) : ExtName(Name), CommandLine(Path) {}
    bool operator==(const CScriptEngine &Other) const { return ExtName.CompareNoCase(Other.ExtName) == 0; }
};
typedef TLIST<CScriptEngine> EngineLIST;

class CWebServerConfig
{
public:
    MtSTRING    RootDirectory;    // 主目录
    StrLIST     DefaultDoclList;  // 缺省文档列表
    int         ListenPort;       // 端口号
    int         MaxThreadNum;     // 最大连接数（0表示不限制）
    BOOL        bEnableCgi;       // 是否允许执行CGI
    BOOL        bEnableIsapi;     // 是否允许执行ISAPI
    BOOL        bEnableScript;    // 是否允许执行脚本
    BOOL        bIsapiCache;      // 缓存ISAPI(仅在bEnableIsapi为TRUE时有效)
    int         NetTimeout;       // 超时时间（单位1/1000秒）
    BOOL        bAutoRun;         // 是否开机自动运行
    BOOL        bIconStartStop;   // 是否点托盘图标启动停止服务器
    BOOL        bEnableShowDir;   // 是否允许目录浏览
    BOOL        bShowDirAlways;   // 是否总是显示目录内容(仅在bEnableShowDir为TRUE时有效)
    BOOL        bLogToWindow;     // 记录日志到窗口
    int         LogWndMaxLine;    // 日志窗口最大显示行数(仅在bLogToWindow为TRUE时有效)
    BOOL        bLogToFile;       // 记录日志到文件
    MtSTRING    LogFileName;      // 日志文件名(仅在bLogToFile为TRUE时有效)
    MtSTRING    ServerName;       // 服务器名称/版本
    DWORD       MaxContentLength; // 接收的最大ContentLength
    DWORD       MaxHeaderLength;  // 接收的最大HeaderLength（\r\n\r\n之前的）
    EngineLIST  ScriptEngineList; // 脚本引擎列表

public:
    CWebServerConfig();

public:
    void        SetDefaultHtmlList(const TCHAR *pDefaultHtmlStr);
    void        GetDefaultHtmlStr(MtSTRING &DefaultHtmlStr) const;
};

typedef BOOL (WINAPI *GetExtensionVersionFUNC)(HSE_VERSION_INFO *pVersion);
typedef DWORD (WINAPI *HttpExtensionProcFUNC)(EXTENSION_CONTROL_BLOCK *pECB);
typedef BOOL (WINAPI *TerminateExtensionFUNC)(DWORD dwFlags);

struct CCacheDllData
{
    MtSTRING                    DllFileName;
    HMODULE                     hDll;
    GetExtensionVersionFUNC     pGetExtensionVersion;
    HttpExtensionProcFUNC       pHttpExtensionProc;
    TerminateExtensionFUNC      pTerminateExtension;
};

typedef TLIST<CCacheDllData> CacheDllLIST;
typedef CacheDllLIST::POSITION CacheDllListPOS;

enum HTTP_ERROR
{
    HERROR_SUCCESS,
    HERROR_NETWORK,         // 读取Http请求时网络错误
    HERROR_CONTENTLENGTH,   // 读取Http请求时ContentLength太大
    HERROR_HEADERLENGTH,    // 读取Http请求时Header太长
    HERROR_BADREQUEST,      // 无效的Http请求
    HERROR_FORBIDDEN,       // Http请求的Url中含有..等
    HERROR_BADISAPI,        // 错误的ISAPI dll
    HERROR_EXCEPTION,       // ISAPI执行抛出异常或返回失败
};

class CWebServer;
class CWebServerThread : public CServerThread
{
public:
    CRequestData    RequestData;
    BOOL            bResponseKeepAlive;
    EXTENSION_CONTROL_BLOCK *pECB;

public:
    CWebServerThread() { bResponseKeepAlive = FALSE; pECB = NULL; };
    ~CWebServerThread() { delete pECB; };
    BaseSOCKET   &GetSocket();
    CWebServer    *GetWebServer();
    const SOCKADDR_IN &GetClientAddr();

protected:
    virtual int   ServerProc();

    HTTP_ERROR      ReadRequest(CRequestData &RequestData, int TimeOutMilliSeconds, int GrowBy);
    HTTP_ERROR      ParseRequest(CRequestData &RequestData);
    BOOL            ProcessRequest(CRequestData &RequestData, const MtSTRING &WebFileName, BOOL &bExecute);
    BOOL            SendHttpFile();
    BOOL            SendHttpErrorStr(const TCHAR *pResponseCmd, const TCHAR *pResponseText);
    const TCHAR    *GetFileContentType(const TCHAR *pFileExtName);
    void            BuildCgiEnvironment(const CRequestData &RequestData, MemFILE2 &MemFile);
    BOOL            ExecuteCgiProcess(const CRequestData &RequestData);
    HTTP_ERROR      ExecuteIsapiDll(const CRequestData &RequestData);
    const TCHAR    *GetExistDefaultHtml(MtSTRING &LocalFilePath, const StrLIST &DefaultDoclList);
    BOOL            BrowseDirectory(const TCHAR *pWebDirName, const TCHAR *pLocalDirName);
    BOOL            ParseDateTime(TCHAR *pTimeStr, SYSTEMTIME &Time);
    BOOL            ParseWeekDay(const TCHAR *pToken, WORD &nWeekDay);
    BOOL            ParseMonth(const TCHAR *pToken, WORD &nMonth);
    BOOL            ParseRange(TCHAR *pRangeStr, __int64 &RangeBegin, __int64 &RangeEnd);

    friend void ReadPipeFunc(HANDLE hPipe, LPARAM FuncParam);
};

inline BaseSOCKET &CWebServerThread::GetSocket()
{
    return TheSocket;
}

inline const SOCKADDR_IN &CWebServerThread::GetClientAddr()
{
    return ClientAddr;
}

//////////////////////////////////////////////////////////////////////////

class CWebServer : public CNetServer
{
public:
    CWebServer();
    ~CWebServer();

public:
    BOOL          Init(const CWebServerConfig &Config);
    const CWebServerConfig &GetServerConfig();

protected:
    virtual CServerThread *CreateServerThread();
    virtual void  OnServerStarted();
    virtual void  OnServerClosed();
    virtual void  OnServerTimestep();
    virtual void  OnRequestLog(const SOCKADDR_IN &ClientAddr, const CRequestData &RequestData, BOOL bExecute, BOOL bSuccess);

    const CCacheDllData *FindCacheDll(const TCHAR *pDllFileName);
    void          AddOneCacheDll(const CCacheDllData &CacheDllData);

protected:
    CWebServerConfig ServerConfig;  // 服务器配置
    CacheDllLIST  CacheDllList;     // ISAPI的dll缓存
    CCriticalSect CacheDllListCs;

    friend class CWebServerThread;
};

inline CServerThread *CWebServer::CreateServerThread()
{
    return new CWebServerThread;
}

inline const CWebServerConfig &CWebServer::GetServerConfig()
{
    return ServerConfig;
}

inline CWebServer *CWebServerThread::GetWebServer()
{
    return (CWebServer *)pServer;
}

//////////////////////////////////////////////////////////////////////////

BOOL ParseEngineCommandLine(const MtSTRING &CommandLine, MtSTRING &EnginePath, MtSTRING &EngineParam);

#endif // __WEBSVR_H__