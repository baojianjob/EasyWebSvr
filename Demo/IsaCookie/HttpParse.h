// baojian 2007.11.15
#ifndef __HTTPPARSE_H__
#define __HTTPPARSE_H__
#pragma once

struct CKeyNode
{
    char *pName;
    char *pValue;
};

class CKeyList : public std::vector<CKeyNode>
{
public:
    ~CKeyList();

public:
    const char *GetValueByName(const char *pName) const;
};

enum SubmitMETHOD
{
    METHOD_GET,
    METHOD_POST,
    METHOD_HEAD,
};

enum ContentTYPE
{
    CTYPE_TEXT,
    CTYPE_SOAP,
    CTYPE_FILE,
};

class CHttpContext
{
public:
    CHttpContext(EXTENSION_CONTROL_BLOCK *pECB);

public:
    BOOL                ParseHttpData();

    SubmitMETHOD        GetSubmitMethod() const; 
    ContentTYPE         GetContentType() const; 

    const CKeyList     &GetGetList() const;
    const CKeyList     &GetPostList() const;
    const CKeyList     &GetCookieList() const;

    BOOL                GetServerVariable(const char *pVarName, char *pValueBuf, DWORD &ValueLen);

    void                SendResponseHeader(const char *pStatus = "200 OK");
    void                AddHeader(const char *pStr);
    void                ResetHeader();
    BOOL                SetCookie(const char *pName, const char *pValue, time_t Date = 0, const char *pPath = NULL, 
                            const char *pDoMain = NULL, BOOL bSecure = FALSE);

    BOOL                WriteClient(const char *pData, DWORD DataLen);

private:
    void                ParseCookieList();

private:
    CKeyList            m_GetList;
    CKeyList            m_PostList;
    CKeyList            m_CookieList;
    ContentTYPE         m_ContentType;
    SubmitMETHOD        m_SubmitMethod;
    std::string         m_Header;
    EXTENSION_CONTROL_BLOCK *m_pECB;
};

inline const CKeyList &CHttpContext::GetGetList() const
{
    return m_GetList;
}

inline const CKeyList &CHttpContext::GetPostList() const
{
    return m_PostList;
}

inline const CKeyList &CHttpContext::GetCookieList() const
{
    return m_CookieList;
}

inline SubmitMETHOD CHttpContext::GetSubmitMethod() const
{
    return m_SubmitMethod;
}

inline ContentTYPE CHttpContext::GetContentType() const
{
    return m_ContentType;
}

inline BOOL CHttpContext::GetServerVariable(const char *pVarName, char *pValueBuf, DWORD &ValueLen)
{
    return m_pECB->GetServerVariable(m_pECB->ConnID, (char *)pVarName, pValueBuf, &ValueLen);
}

inline void CHttpContext::ResetHeader()
{
    m_Header.c_str();
}

inline BOOL CHttpContext::WriteClient(const char *pData, DWORD DataLen)
{
    assert(m_pECB != NULL);
    return m_pECB->WriteClient(m_pECB->ConnID, (void *)pData, &DataLen, HSE_IO_SYNC);
}

#endif // __HTTPPARSE_H__