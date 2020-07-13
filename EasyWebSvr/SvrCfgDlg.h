#pragma once
#include "websvr.h"
#include "propsheet.h"

using namespace minilib;

class CConfigMainPage : public PropertyPAGE
{
public:
    CWebServerConfig    *pSvrConfig;

public:
    CConfigMainPage();

protected:
    virtual BOOL        OnInitDialog();
    virtual BOOL        OnOK(WPARAM WParam, LPARAM LParam);
    BOOL                OnBtnBrowse(WPARAM WParam, LPARAM LParam);

    DECLARE_MESSAGE_MAP()
};

class CConfigDocPage : public PropertyPAGE
{
public:
    CWebServerConfig      *pSvrConfig;

public:
    CConfigDocPage();

protected:
    virtual BOOL        OnInitDialog();
    virtual BOOL        OnOK(WPARAM WParam, LPARAM LParam);
    BOOL                OnEnableShowDir(WPARAM WParam, LPARAM LParam);

    DECLARE_MESSAGE_MAP()
};

class CConfigRightPage : public PropertyPAGE
{
public:
    CWebServerConfig    *pSvrConfig;

public:
    CConfigRightPage();

protected:
    virtual BOOL        OnInitDialog();
    virtual BOOL        OnOK(WPARAM WParam, LPARAM LParam);
    BOOL                OnEnableIsapi(WPARAM WParam, LPARAM LParam);

    DECLARE_MESSAGE_MAP()
};

class CConfigMapPage : public PropertyPAGE
{
public:
    CWebServerConfig      *pSvrConfig;

public:
    CConfigMapPage();

protected:
    virtual BOOL        OnInitDialog();
    virtual BOOL        OnOK(WPARAM WParam, LPARAM LParam);
    BOOL                OnBtnAdd(WPARAM WParam, LPARAM LParam);
    BOOL                OnBtnEdit(WPARAM WParam, LPARAM LParam);
    BOOL                OnBtnDel(WPARAM WParam, LPARAM LParam);
    BOOL                OnNotify(WPARAM WParam, LPARAM LParam);

private:
    void                _GetExistExtNameList(StrLIST &ExistNameList, EngineLIST::POSITION EditPos);

protected:
    HWND                hWndListView;

    DECLARE_MESSAGE_MAP()
};

class CScriptEngineDlg : public BaseDLG
{
public:
    CScriptEngineDlg(CScriptEngine &Engine, const StrLIST &ExistNameList);

protected:
    virtual BOOL        OnInitDialog();
    virtual BOOL        OnOK(WPARAM WParam, LPARAM LParam);
    virtual BOOL        OnCancel(WPARAM WParam, LPARAM LParam);
    BOOL                OnBtnBrowse(WPARAM WParam, LPARAM LParam);

protected:
    CScriptEngine      &ScriptEngine;
    const StrLIST      &ExistExtNameList;

    DECLARE_MESSAGE_MAP()
};

class CConfigLogPage : public PropertyPAGE
{
public:
    CWebServerConfig    *pSvrConfig;

public:
    CConfigLogPage();

protected:
    virtual BOOL        OnInitDialog();
    virtual BOOL        OnOK(WPARAM WParam, LPARAM LParam);
    BOOL                OnLogToWindow(WPARAM WParam, LPARAM LParam);
    BOOL                OnLogToFile(WPARAM WParam, LPARAM LParam);
    BOOL                OnBtnBrowse(WPARAM WParam, LPARAM LParam);

    DECLARE_MESSAGE_MAP()
};

class CServerConfigDlg : public PropertySHEET
{
public:
    CConfigMainPage     MainPage;
    CConfigDocPage      DocPage;
    CConfigRightPage    RightPage;
    CConfigLogPage      LogPage;
    CConfigMapPage      MapPage;
    CWebServerConfig   &SvrConfig;

public:
    CServerConfigDlg(CWebServerConfig &Config);
    ~CServerConfigDlg();
};

BOOL AdjustEngineCommandLine(MtSTRING &EngineCommandLine);

BOOL ServerConfigDialog(HWND hWndParent, CWebServerConfig &SvrConfig);