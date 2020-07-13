#pragma once

#define GID_NETSERVER          1  
#define GID_WEBSERVER          2
#define GID_DECODEURL          3
#define GID_SERVERLOG          4


void OutputDebugInfo(int ModuleID, const char *pFormat, ...);
void OutputDebugInfo(int ModuleID, const wchar_t *pFormat, ...);

void OutputHex1(int ModuleID, const char *pHeader, const unsigned char *pData, unsigned int Len);
void OutputHex2(int ModuleID, const char *pHeader, const unsigned char *pData, unsigned int Len);

class log_trace
{
public:
    log_trace(int ModuleID, const char *pFuncName, const char *pFormat = "", ...);
    ~log_trace();

private:
    char m_szFuncName[256];
    int  m_nModuleID;
};

//////////////////////////////////////////////////////////////////////////

//#ifdef _DEBUG
    #define _ENABLE_LOG       // 是否启用日志输出
//#endif

#if _MSC_VER >= 1300
    #define __NOOP       __noop  
#else
    #define __NOOP       ((void)0)
#endif

#ifdef _ENABLE_LOG
    #define LOG_INFO            OutputDebugInfo
    #define LOG_HEX             OutputHex1
    #define LOG_TRACE           log_trace __log_trace
#else
    #define LOG_INFO            __NOOP 
    #define LOG_HEX             __NOOP
    #define LOG_TRACE           __NOOP
#endif
