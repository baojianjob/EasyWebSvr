#include "stdafx.h"
#include <httpext.h>

static HANDLE g_hModule;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID lpReserved)
{
    switch(dwReason)
    {
    case DLL_PROCESS_ATTACH:
        g_hModule = hModule;
        break;
    case DLL_PROCESS_DETACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}

BOOL WINAPI GetExtensionVersion(OUT HSE_VERSION_INFO * pVer)
{
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

//#define malloc(x) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, x)
//#define free(x)   HeapFree(GetProcessHeap(), 0, x)

DWORD WINAPI HttpExtensionProc(IN EXTENSION_CONTROL_BLOCK *pECB)
{
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
    pECB->WriteClient(pECB->ConnID, (void *)pResultStr, &Len, 0);

    free(pDataBuf);
    return HSE_STATUS_SUCCESS;
}

BOOL WINAPI TerminateExtension(IN DWORD dwFlags)
{
    return TRUE;
}
