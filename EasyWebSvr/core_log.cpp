#include <windows.h>
#include <stdio.h>
#include <crtdbg.h>
#include "core_log.h"
//#include "Version.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)  (sizeof(x) / sizeof(x[0]))
#endif

class base_log_device
{
public:
    virtual void put(const char *pLog, int Len) = 0;
};

//////////////////////////////////////////////////////////////////////////

class log_device_file : public base_log_device
{
public:
    log_device_file();
    ~log_device_file();

    virtual void put(const char *pLog, int Len);

private:
    FILE *pFile;
};

log_device_file::log_device_file()
{
    SYSTEMTIME T;
    ::GetLocalTime(&T);

    char FileName[300];
    ::GetModuleFileNameA(::GetModuleHandle(NULL), FileName, ARRAY_SIZE(FileName));
    char *pFind = strrchr(FileName, '\\');
    _ASSERTE(pFind != NULL);
    _snprintf(pFind + 1, FileName + ARRAY_SIZE(FileName) - pFind - 2, "EasyWebSvr%d%02d%02d%02d%02d%02d.log", T.wYear, T.wMonth, T.wDay, 
        T.wHour, T.wMinute, T.wSecond);
    pFile = fopen(FileName, "ab");
    if(pFile != NULL)
    {
        const char *pVersion = "(0000) 00:00:00.000000 Version: EasyWebServer 2.0";
        this->put(pVersion, (int)strlen(pVersion));
    }
}

log_device_file::~log_device_file()
{
    if(pFile)
        fclose(pFile);
}

void log_device_file::put(const char *pLog, int Len)
{
    if(pFile != NULL)
    {
        fwrite(pLog, 1, Len, pFile);
        fwrite("\r\n", 1, 2, pFile);
        fflush(pFile);
    }
}
//////////////////////////////////////////////////////////////////////////

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
    m_hWndLogView = ::FindWindowA("Coobol_LogView", NULL);
    if(m_hWndLogView != NULL)
    {
        if(::IsIconic(m_hWndLogView)) 
            ::ShowWindow(m_hWndLogView, SW_RESTORE);
        //SetForegroundWindow(m_hWndLogView);
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

//////////////////////////////////////////////////////////////////////////

class core_critical_section
{ 
public: 
    core_critical_section()  { ::InitializeCriticalSection(&m_cs); } 
    ~core_critical_section() { ::DeleteCriticalSection(&m_cs); } 
    void Enter() { ::EnterCriticalSection(&m_cs); } 
    void Leave() { ::LeaveCriticalSection(&m_cs); } 

private: 
    CRITICAL_SECTION m_cs; 
}; 

class core_guard
{
public:
    core_guard(core_critical_section &cs) : m_cs(cs)
    {
        m_cs.Enter();
    }
    ~core_guard()
    {
        m_cs.Leave();
    }

private:
    core_critical_section &m_cs;
};

//////////////////////////////////////////////////////////////////////////

class log_device_manager : public base_log_device
{
public:
    static log_device_manager &instance();

    void put(int module, const char *pLog);

private:
    virtual void put(const char *pLog, int Len);

private:
    //log_device_file m_logfile;
    log_device_logview m_logview;
    core_critical_section m_cs;
};

log_device_manager &log_device_manager::instance()
{
    static log_device_manager _instance;
    return _instance;
}

void log_device_manager::put(const char *pLog, int Len)
{
    core_guard guard(m_cs);
    //m_logfile.put(pLog, Len);
    m_logview.put(pLog, Len);
}

const char *GetModuleName(int ModuleID)
{
    static struct
    {
        int ModuleID;
        const char *pModuleName;
    } ModuleMap[] =
    {
        GID_NETSERVER,           "NetServer",
        GID_WEBSERVER,           "WebServer",
        GID_DECODEURL,           "DecodeUrl",
        GID_SERVERLOG,           "ServerLog",
    };

    for(int i = 0; i < (sizeof(ModuleMap) / sizeof(ModuleMap[0])); i++)
    {
        if(ModuleMap[i].ModuleID == ModuleID)
            return ModuleMap[i].pModuleName;
    }
    return "Unkonwn";
}

void log_device_manager::put(int module, const char *pLog)
{
    SYSTEMTIME T;
    ::GetLocalTime(&T);

    char Buffer[8192] = { 0 };
    _ASSERTE(strlen(pLog) <= 8000);
    int Len = _snprintf(Buffer, ARRAY_SIZE(Buffer) - 1, "(%d) %02d:%02d:%02d.%03d000 %s: %s", ::GetCurrentThreadId(), 
        T.wHour, T.wMinute, T.wSecond, T.wMilliseconds,
        GetModuleName(module), pLog);
    this->put(Buffer, Len);
}

//////////////////////////////////////////////////////////////////////////

void OutputDebugInfo(int ModuleID, const char *pFormat, ...)
{
    va_list ArgList;            // 等价于 char *ArgList;
    va_start(ArgList, pFormat); // 给 ArgList 赋值，等价于 ArgList = (char *)(&pFormat) + sizeof(void *);

    char Buffer[1024];
    int Ret = _vsnprintf(Buffer, 1024, pFormat, ArgList); //  Format 后字符串最大长度为 1023
    if(Ret < 0)
        Buffer[1023] = '\0';

    va_end(ArgList);            // 等价于 ArgList = NULL;

    log_device_manager::instance().put(ModuleID, Buffer);
}

void OutputDebugInfo(int ModuleID, const wchar_t *pFormat, ...)
{
    va_list ArgList;
    va_start(ArgList, pFormat);

    wchar_t Buffer[1024];
    int WideLen = _vsnwprintf(Buffer, 1024, pFormat, ArgList);
    if(WideLen < 0)
    {
        Buffer[1023] = 0;
        WideLen = (int)wcslen(Buffer);
    }
    va_end(ArgList);

    char Buffer2[1024];
    int Length = WideCharToMultiByte(CP_ACP, 0, Buffer, WideLen, NULL, 0, NULL, NULL);
    _ASSERTE(Length < sizeof(Buffer2));
    WideCharToMultiByte(CP_ACP, 0, Buffer, WideLen, Buffer2, Length, NULL, NULL);
    Buffer2[Length] = 0;

    log_device_manager::instance().put(ModuleID, Buffer2);
}

void OutputHex1(int ModuleID, const char *pHeader, const unsigned char *pData, unsigned int Len)
{
    const int LINE_LEN = 16;
    const int MAX_HEX_LEN = 2048;
    char ResultBuffer[MAX_HEX_LEN * 3 + MAX_HEX_LEN / LINE_LEN * 2 + 300]; 

    if(pData == NULL)
        return;

    if(Len > MAX_HEX_LEN)
    {
        _snprintf(ResultBuffer, ARRAY_SIZE(ResultBuffer) - 1, "%s Len=%d (RealLen=%d, too long)\r\n", pHeader, MAX_HEX_LEN, Len);
        Len = MAX_HEX_LEN;
    }
    else
    {
        _snprintf(ResultBuffer, ARRAY_SIZE(ResultBuffer) - 1, "%s Len=%d\r\n", pHeader, Len);
    }

    char *pBuf = ResultBuffer + strlen(ResultBuffer);
    for(unsigned int i = 0; i < Len; i++)
    {
        _snprintf(pBuf, 3, " %02X", (BYTE)pData[i]);
        pBuf += 3;
        if(((i + 1) % LINE_LEN) == 0 && i != Len - 1)
        {
            strncpy(pBuf, "\r\n", 2);
            pBuf += 2;
        }
    }
    *pBuf = 0;

    log_device_manager::instance().put(ModuleID, ResultBuffer);
}

const char *HexDumpStr(char *sHexBuf, const unsigned char *data, long size)
{
    bool fin=false;
    int srcpos=0, dstpos=0;
    int linePos;
    char charBuf[16];
    unsigned char lineBuf[32];

    for(srcpos=0, linePos=0; ; srcpos++)
    {
        if(srcpos < size)
        {
            if(linePos == 0)
            {
                dstpos += sprintf(&(sHexBuf[dstpos]),"%04x: ", srcpos);
            }
            sprintf(charBuf, "%08x", data[srcpos]);
            dstpos += sprintf(&(sHexBuf[dstpos]), "%c%c ", charBuf[6], charBuf[7]);
            lineBuf[linePos] = data[srcpos];
            if(lineBuf[linePos] < 0x20)// || lineBuf[linePos] > 0x7f)
                lineBuf[linePos]='.';
        }
        else
        {
            if(linePos == 0)
                break;
            else
            {
                dstpos += sprintf(&(sHexBuf[dstpos]),"   ");
                lineBuf[linePos]=' ';
                fin = true;
            }
        }
        linePos++;
        if(!(linePos < 16))
        {
            lineBuf[linePos]=0;
            dstpos += sprintf(&(sHexBuf[dstpos]), "%s\n", lineBuf);
            linePos = 0;
            if(fin)
                break;
        }
    }
    sHexBuf[dstpos] = '\0';
    return sHexBuf;
}

void OutputHex2(int ModuleID, const char *pHeader, const unsigned char *pData, unsigned int Len)
{
    const int LINE_LEN = 16;
    const int MAX_HEX_LEN = 1536;
    char ResultBuffer[MAX_HEX_LEN * 4 + MAX_HEX_LEN / LINE_LEN * 8 + 300]; 

    if(pData == NULL)
        return;

    if(Len > MAX_HEX_LEN)
    {
        _snprintf(ResultBuffer, ARRAY_SIZE(ResultBuffer) - 1, "%s Len=%d (RealLen=%d, too long)\r\n", pHeader, MAX_HEX_LEN, Len);
        Len = MAX_HEX_LEN;
    }
    else
    {
        _snprintf(ResultBuffer, ARRAY_SIZE(ResultBuffer) - 1, "%s Len=%d\r\n", pHeader, Len);
    }

    char *pBuf = ResultBuffer + strlen(ResultBuffer);
    HexDumpStr(pBuf, pData, Len);

    log_device_manager::instance().put(ModuleID, ResultBuffer);
}

log_trace::log_trace(int ModuleID, const char *pFuncName, const char *pFormat, ...)
{
    char Buffer[1024] = { 0 };
    if(pFormat && *pFormat)
    {
        Buffer[0] = ',';
        Buffer[1] = ' ';

        va_list ArgList;
        va_start(ArgList, pFormat);

        int Ret = _vsnprintf(Buffer + 2, 1024 - 2, pFormat, ArgList);
        if(Ret < 0)
            Buffer[1023] = '\0';
        va_end(ArgList);  
    }

    m_nModuleID = ModuleID;
    lstrcpyn(m_szFuncName, pFuncName, ARRAY_SIZE(m_szFuncName));
    LOG_INFO(m_nModuleID, "come in, %s%s", m_szFuncName, Buffer);
}

log_trace::~log_trace()
{
    LOG_INFO(m_nModuleID, "exit, %s", m_szFuncName);
}