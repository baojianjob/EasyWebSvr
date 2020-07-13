// baojian 2007.11.15
#include "stdafx.h"
#include "httpparse.h"
#include <time.h>
#pragma warning(disable:4996)

CKeyList::~CKeyList()
{
    iterator Iter;
    for(Iter = begin(); Iter != end(); Iter++)
    {
        free(Iter->pName);
        free(Iter->pValue);
    }
}

const char *CKeyList::GetValueByName(const char *pName) const
{
    const_iterator Iter;
    for(Iter = begin(); Iter != end(); Iter++)
    {
        if(strcmp(Iter->pName, pName) == 0)
            return Iter->pValue;
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////

static int HexToInt(TCHAR ch)
{
    if((ch >= _T('0')) && (ch <= _T('9')))
        return ch - _T('0');

    if((ch >= _T('a')) && (ch <= _T('f')))
        return ch - _T('a') + 10;
    else if((ch >= _T('A')) && (ch <= _T('F')))
        return ch - _T('A') + 10;
    return -1;
}

static void UrlDecode(char *&pSrcBuf, char *&pDestBuf, bool &bUtf8)
{
    assert(!bUtf8);

    bool bValid = false;
    for(; *pSrcBuf; pSrcBuf++, pDestBuf++)
    {
        if(*pSrcBuf != '%')
        {
            if(*pSrcBuf == '+')
            {
                *pDestBuf = ' ';
                continue;
            }
        }
        else
        {
            if(pSrcBuf[1] && pSrcBuf[2])
            {
                int High = HexToInt(pSrcBuf[1]);
                int Low = HexToInt(pSrcBuf[2]);
                if(High != -1 && Low != -1)
                {
                    *pDestBuf = char((High << 4) + Low);
                    if(!bValid && ((BYTE)(*pDestBuf) > 0x7F))
                    {
                        bUtf8 = ((BYTE)(*pDestBuf)) > 0xE0;
                        bValid = true;
                    }

                    pSrcBuf += 2;
                    continue;
                }
            }
        }
        *pDestBuf = *pSrcBuf;
    }
    *pDestBuf = '\0';
}

static void DecodeUrl(char *pUrlStr)
{
    char *pSrcBuf = pUrlStr, *pDestBuf = pUrlStr;
    for(; *pSrcBuf; pSrcBuf++, pDestBuf++)
    {
        if(*pSrcBuf != '%')
        {
            if(*pSrcBuf == '+')
                *pDestBuf = ' ';
        }
        else
        {
            bool bUtf8 = false;
            UrlDecode(pSrcBuf, pDestBuf, bUtf8);
            if(bUtf8)
            {
                // UTF8 --> UNICODE
                int SrcLen = (int)strlen(pUrlStr);
                int WideLen = MultiByteToWideChar(CP_UTF8, 0, pUrlStr, SrcLen, NULL, NULL);
                assert(WideLen > 0);
                wchar_t *pWideStr = (wchar_t *)malloc(WideLen * sizeof(wchar_t));
                MultiByteToWideChar(CP_UTF8, 0, pUrlStr, SrcLen, pWideStr, WideLen);

                // UNICODE --> MultiByte
                int Len = WideCharToMultiByte(CP_ACP, 0, pWideStr, WideLen, pUrlStr, SrcLen, NULL, NULL);
                pUrlStr[Len] = 0;

                free(pWideStr);
            }
            break;
        }
    }
}

static void ParseKeyList(const char *pBuffer, CKeyList &KeyList)
{
    assert(KeyList.empty());
    char *pStr = strdup(pBuffer);
    char *pName = strtok(pStr, "&");
    while(pName != NULL)
    {
        // parse name=value
        char *pValue = strchr(pName, '=');
        if(pValue != NULL)
        {
            *pValue = '\0';
            pValue++;
        }
        else
        {
            pValue = "";
        }
        
        // new node
        CKeyNode NewNode;
        NewNode.pName = strdup(pName);
        NewNode.pValue = strdup(pValue);

        // decode
        DecodeUrl(NewNode.pName);
        DecodeUrl(NewNode.pValue);

        // add
        KeyList.push_back(NewNode);

        // next key
        pName = strtok(NULL, "&");
    }
    free(pStr);
}

//////////////////////////////////////////////////////////////////////////

CHttpContext::CHttpContext(EXTENSION_CONTROL_BLOCK *pECB)
{
    m_ContentType       = CTYPE_TEXT;
    m_SubmitMethod      = METHOD_GET;
    m_pECB              = pECB;

    assert(m_pECB != NULL);
}

void CHttpContext::ParseCookieList()
{
    char CookieBuffer[1024] = { 0 };
    DWORD CookieBufferLen = sizeof(CookieBuffer);
    if(GetServerVariable("HTTP_COOKIE", CookieBuffer, CookieBufferLen))
    {
        char *pName, *pValue;
        for(pName = strtok(CookieBuffer, ";"); pName && (pValue = strchr(pName, '=')); pName = strtok(NULL, ";"))
        {
            while(*pName == ' ' || *pName == '\t')
                pName++;
            *pValue = 0;
            pValue++;
            
            // new node
            CKeyNode NewNode;
            NewNode.pName = strdup(pName);
            NewNode.pValue = strdup(pValue);
            
            // decode
            DecodeUrl(NewNode.pName);
            DecodeUrl(NewNode.pValue);
            
            // add
            m_CookieList.push_back(NewNode);
        }
    }
}

BOOL CHttpContext::ParseHttpData()
{
    assert(m_GetList.empty());
    assert(m_PostList.empty());

    // parse submit method
    if(stricmp(m_pECB->lpszMethod, "GET") == 0)
        m_SubmitMethod = METHOD_GET;
    else if(stricmp(m_pECB->lpszMethod, "POST") == 0)
        m_SubmitMethod = METHOD_POST;
    else if(stricmp(m_pECB->lpszMethod, "HEAD") == 0)
        m_SubmitMethod = METHOD_HEAD;
    else
        return FALSE;

    // parse content type
    if(stricmp(m_pECB->lpszContentType, "text/xml") == 0)
        m_ContentType = CTYPE_SOAP;
    else if(strstr(m_pECB->lpszContentType, "boundary=") != NULL)
        m_ContentType = CTYPE_FILE;
    else
        m_ContentType = CTYPE_TEXT;

    // parse get list
    ParseKeyList(m_pECB->lpszQueryString, m_GetList);

    // parse cookie list
    ParseCookieList();

    // parse post list
    BOOL bRet = TRUE;
    if(m_SubmitMethod == METHOD_POST)
    {
        if(m_ContentType == CTYPE_TEXT)
        {
            char *pPostData = (char *)malloc(m_pECB->cbTotalBytes + 1);
            assert(pPostData != NULL);
            assert(m_pECB->cbAvailable <= m_pECB->cbTotalBytes);

            // copy available
            memcpy(pPostData, m_pECB->lpbData, m_pECB->cbAvailable);

            // read left data
            DWORD Left = m_pECB->cbTotalBytes - m_pECB->cbAvailable;
            while(Left > 0)
            {
                DWORD ReadBytes = Left;
                if(!m_pECB->ReadClient(m_pECB->ConnID, pPostData + m_pECB->cbTotalBytes - Left, &ReadBytes))
                {
                    bRet = FALSE;
                    break;
                }
                Left -= ReadBytes;
            }

            // parse
            if(bRet)
            {
                pPostData[m_pECB->cbTotalBytes] = '\0';

                ParseKeyList(pPostData, m_PostList);
            }

            // free
            free(pPostData);
        }
        else if(m_ContentType == CTYPE_SOAP)
        {
            //
            assert(!"not implement");
        }
        else if(m_ContentType == CTYPE_FILE)
        {
            //
            assert(!"not implement");
        }
        else
        {
            assert(false);
        }
    }
    return bRet;
}

void CHttpContext::AddHeader(const char *pStr)
{
    m_Header += pStr;
    m_Header += "\r\n";
}

void CHttpContext::SendResponseHeader(const char *pStatus)
{
    m_Header += "\r\n";
    m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER,
        (void *)pStatus, NULL, (DWORD *)m_Header.c_str());
}

static void TimeToStr(const time_t &Time, TCHAR *pBuffer, DWORD BufSize)
{
    static const TCHAR *MonthStr[] =  
    {
        _T("Jan"),
        _T("Feb"),
        _T("Mar"), 
        _T("Apr"),
        _T("May"),
        _T("Jun"),
        _T("Jul"),
        _T("Aug"),
        _T("Sep"),
        _T("Oct"),
        _T("Nov"),
        _T("Dec"),
    };

    static const TCHAR *DayStr[] =
    {
        _T("Sun"),
        _T("Mon"),
        _T("Tue"), 
        _T("Wed"),
        _T("Thu"),
        _T("Fri"),
        _T("Sat"),
    };

    tm *pT = gmtime(&Time);
    _sntprintf(pBuffer, BufSize, _T("%s, %02d-%s-%04d %02d:%02d:%02d GMT"), DayStr[pT->tm_wday], 
        pT->tm_mday, MonthStr[pT->tm_mon], pT->tm_year + 1900, pT->tm_hour, pT->tm_min, pT->tm_sec);
}

BOOL CHttpContext::SetCookie(const char *pName, const char *pValue, time_t Date,
                        const char *pPath, const char *pDoMain, BOOL bSecure)
{
    m_Header += "Set-Cookie: ";
    m_Header += pName;
    m_Header += '=';
    m_Header += pValue;

    if(Date != 0)
    {
        m_Header += "; ";
        m_Header += "expires";
        m_Header += '=';
        char DateStr[256];
        TimeToStr(Date, DateStr, sizeof(DateStr) / sizeof(DateStr[0]));
        m_Header += DateStr;
    }

    if(pPath && *pPath)
    {
        m_Header += "; ";
        m_Header += "path";
        m_Header += '=';
        m_Header += pPath;
    }
    
    if(pDoMain && *pDoMain)
    {
        m_Header += "; ";
        m_Header += "domain";
        m_Header += '=';
        m_Header += pDoMain;
    }

    if(bSecure)
    {
        m_Header += "; ";
        m_Header += "secure";
    }

    m_Header += "\r\n";
    return TRUE;
}
