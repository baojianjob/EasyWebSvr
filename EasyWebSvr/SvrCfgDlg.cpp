// 服务器设置界面

#include "stdafx.h"
#include "svrcfgdlg.h"
#include "dirdlg.h"
#include "filedlg.h"
#include "cmncode.h"
#include "resource.h"

static void GetDlgItemText(HWND hDlg, int CtrlID, MtSTRING &Text, int MaxCount = 128)
{
    TCHAR *pBuffer = Text.GetBuffer(MaxCount);
    ::GetDlgItemText(hDlg, CtrlID, pBuffer, MaxCount);
    pBuffer[MaxCount - 1] = 0;
    Text.ReleaseBuffer();
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CConfigMainPage, PropertyPAGE)
    ON_DLGCOMMAND(IDC_BTN_BROWSE, OnBtnBrowse)
END_MESSAGE_MAP()

CConfigMainPage::CConfigMainPage() : PropertyPAGE(IDD_CONFIG_MAIN)
{
    SetTitle("常规");
}

BOOL CConfigMainPage::OnInitDialog()
{
    ::SetDlgItemText(m_hWnd, IDC_EDIT_ROOTDIR, pSvrConfig->RootDirectory);
    ::SetDlgItemInt(m_hWnd, IDC_EDIT_PORT, pSvrConfig->ListenPort, FALSE);
    ::SetDlgItemInt(m_hWnd, IDC_EDIT_MAXTHREADNUM, pSvrConfig->MaxThreadNum, FALSE);
    ::SetDlgItemInt(m_hWnd, IDC_EDIT_TIMEOUT, pSvrConfig->NetTimeout, FALSE);
    ::CheckDlgButton(m_hWnd, IDC_CHECK_AUTORUN, pSvrConfig->bAutoRun ? BST_CHECKED : BST_UNCHECKED);
    return TRUE;
}

BOOL CConfigMainPage::OnBtnBrowse(WPARAM WParam, LPARAM LParam)
{
    MtSTRING InitDirName;
    GetDlgItemText(m_hWnd, IDC_EDIT_ROOTDIR, InitDirName, 300);

    CDirdlg Dlg;
    if(Dlg.DoModal(m_hWnd, InitDirName))
        ::SetDlgItemText(m_hWnd, IDC_EDIT_ROOTDIR, Dlg.GetDirPathName());
    return TRUE;
}

BOOL CConfigMainPage::OnOK(WPARAM WParam, LPARAM LParam)
{
    MtSTRING RootDir;
    GetDlgItemText(m_hWnd, IDC_EDIT_ROOTDIR, RootDir, 300);
    if(RootDir.IsEmpty() || !IsDirectoryExist(RootDir))
    {
        assert(pSheet != NULL);
        pSheet->SetActivePage(this);

        MtSTRING ErrText;
        if(RootDir.IsEmpty())
            ErrText = _T("主目录不能为空!");
        else
            ErrText.Format("主目录[%s]不存在!", (LPCTSTR)RootDir);
        MessageBox(m_hWnd, ErrText, APP_TITLE, MB_ICONWARNING);
        ::SetFocus(::GetDlgItem(m_hWnd, IDC_EDIT_ROOTDIR));
        SetWindowLong(m_hWnd, DWL_MSGRESULT, TRUE);  // 禁止关闭
        return TRUE;
    }
    pSvrConfig->RootDirectory = RootDir;

    pSvrConfig->ListenPort = ::GetDlgItemInt(m_hWnd, IDC_EDIT_PORT, NULL, FALSE);
    pSvrConfig->MaxThreadNum = ::GetDlgItemInt(m_hWnd, IDC_EDIT_MAXTHREADNUM, NULL, FALSE);
    pSvrConfig->NetTimeout = ::GetDlgItemInt(m_hWnd, IDC_EDIT_TIMEOUT, NULL, FALSE);
    pSvrConfig->bAutoRun = ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_AUTORUN) == BST_CHECKED;
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CConfigDocPage, PropertyPAGE)
    ON_DLGCOMMAND(IDC_CHECK_ENABLESHOWDIR, OnEnableShowDir)
END_MESSAGE_MAP()

CConfigDocPage::CConfigDocPage() : PropertyPAGE(IDD_CONFIG_DOC)
{
    SetTitle("文档");
}

BOOL CConfigDocPage::OnInitDialog()
{
    MtSTRING DefaultHtmlStr;
    pSvrConfig->GetDefaultHtmlStr(DefaultHtmlStr);
    ::SetDlgItemText(m_hWnd, IDC_EDIT_DEFAULTHTML, DefaultHtmlStr);
    ::CheckDlgButton(m_hWnd, IDC_CHECK_ENABLESHOWDIR, pSvrConfig->bEnableShowDir ? BST_CHECKED : BST_UNCHECKED);

    assert(IDC_RADIO_SHOWDIR1 + 1 == IDC_RADIO_SHOWDIR2);
    CheckRadioButton(m_hWnd, IDC_RADIO_SHOWDIR1, IDC_RADIO_SHOWDIR2, 
        pSvrConfig->bShowDirAlways ? IDC_RADIO_SHOWDIR1 : IDC_RADIO_SHOWDIR2);

    ::SendMessage(m_hWnd, WM_COMMAND, IDC_CHECK_ENABLESHOWDIR, 0);
    return TRUE;
}

BOOL CConfigDocPage::OnEnableShowDir(WPARAM WParam, LPARAM LParam)
{
    BOOL bEnableShowDir = ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_ENABLESHOWDIR) == BST_CHECKED;
    ::EnableWindow(::GetDlgItem(m_hWnd, IDC_RADIO_SHOWDIR1), bEnableShowDir);
    ::EnableWindow(::GetDlgItem(m_hWnd, IDC_RADIO_SHOWDIR2), bEnableShowDir);
    return TRUE;
}

BOOL CConfigDocPage::OnOK(WPARAM WParam, LPARAM LParam)
{
    MtSTRING DefaultHtmlStr;
    GetDlgItemText(m_hWnd, IDC_EDIT_DEFAULTHTML, DefaultHtmlStr, 300);
    pSvrConfig->SetDefaultHtmlList(DefaultHtmlStr);
    pSvrConfig->bEnableShowDir = ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_ENABLESHOWDIR) == BST_CHECKED;
    pSvrConfig->bShowDirAlways = ::IsDlgButtonChecked(m_hWnd, IDC_RADIO_SHOWDIR1) == BST_CHECKED;
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CConfigRightPage, PropertyPAGE)
    ON_DLGCOMMAND(IDC_CHECK_ENABLEISAPI, OnEnableIsapi)
END_MESSAGE_MAP()

CConfigRightPage::CConfigRightPage() : PropertyPAGE(IDD_CONFIG_RIGHTS)
{
    SetTitle("权限");
}

BOOL CConfigRightPage::OnInitDialog()
{
    ::CheckDlgButton(m_hWnd, IDC_CHECK_ENABLECGI, pSvrConfig->bEnableCgi ? BST_CHECKED : BST_UNCHECKED);
    ::CheckDlgButton(m_hWnd, IDC_CHECK_ENABLEISAPI, pSvrConfig->bEnableIsapi ? BST_CHECKED : BST_UNCHECKED);
    ::CheckDlgButton(m_hWnd, IDC_CHECK_ISAPICACHE, pSvrConfig->bIsapiCache ? BST_CHECKED : BST_UNCHECKED);
    ::CheckDlgButton(m_hWnd, IDC_CHECK_ENABLESCRIPT, pSvrConfig->bEnableScript ? BST_CHECKED : BST_UNCHECKED);
    ::SendMessage(m_hWnd, WM_COMMAND, IDC_CHECK_ENABLEISAPI, 0);
    return TRUE;
}

BOOL CConfigRightPage::OnEnableIsapi(WPARAM WParam, LPARAM LParam)
{
    BOOL bEnableIsapi = ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_ENABLEISAPI) == BST_CHECKED;
    ::EnableWindow(::GetDlgItem(m_hWnd, IDC_CHECK_ISAPICACHE), bEnableIsapi);
    return TRUE;
}

BOOL CConfigRightPage::OnOK(WPARAM WParam, LPARAM LParam)
{
    pSvrConfig->bEnableCgi = ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_ENABLECGI) == BST_CHECKED;
    pSvrConfig->bEnableIsapi = ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_ENABLEISAPI) == BST_CHECKED;
    pSvrConfig->bIsapiCache = ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_ISAPICACHE) == BST_CHECKED;
    pSvrConfig->bEnableScript = ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_ENABLESCRIPT) == BST_CHECKED;
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CConfigMapPage, PropertyPAGE)
    ON_DLGMESSAGE(WM_NOTIFY,    OnNotify)
    ON_DLGCOMMAND(IDC_BTN_ADD,  OnBtnAdd)
    ON_DLGCOMMAND(IDC_BTN_EDIT, OnBtnEdit)
    ON_DLGCOMMAND(IDC_BTN_DEL,  OnBtnDel)
END_MESSAGE_MAP()

CConfigMapPage::CConfigMapPage() : PropertyPAGE(IDD_CONFIG_MAP)
{
    SetTitle("映射");
    hWndListView    = NULL;
}

static int ListViewInsertItem(HWND hWndAttrListView, const TCHAR *pText, int Row)
{
    LVITEM Item;
    Item.mask     = LVIF_TEXT;// | LVIF_PARAM | LVIF_IMAGE;
    Item.iSubItem = 0;
    Item.pszText  = (TCHAR *)pText;  // maybe error
    Item.iItem    = Row;
    return ListView_InsertItem(hWndAttrListView, &Item);
}

static void AddListViewRow(HWND hWndListView, const TCHAR *pExtName, const TCHAR *pFileName)
{
    int Index = ListViewInsertItem(hWndListView, pExtName, 0xFF);
    ListView_SetItemText(hWndListView, Index, 1, (LPSTR)pFileName);
}

BOOL CConfigMapPage::OnInitDialog()
{
    INITCOMMONCONTROLSEX Icex;
    Icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    Icex.dwICC  = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&Icex); 

    hWndListView = ::GetDlgItem(m_hWnd, IDC_LIST_MAP);
    assert(hWndListView != NULL);

    ::SendMessage(hWndListView, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, 
        (LPARAM)(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP));

    struct
    {
        TCHAR  *pName;
        int     Width;
        int     Align;
    } ColArray[] =
    {
        { _T("扩展名"),             60,     LVCFMT_LEFT   },
        { _T("可执行文件路径"),     246,    LVCFMT_LEFT   },
    };

    for(int Col = 0; Col < sizeof(ColArray) / sizeof(ColArray[0]); Col++)
    {
        LVCOLUMN Lvc; 
        Lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        Lvc.iSubItem = Col;
        Lvc.pszText = ColArray[Col].pName;
        Lvc.cx = ColArray[Col].Width;
        Lvc.fmt = ColArray[Col].Align;

        ListView_InsertColumn(hWndListView, Col, &Lvc);
    }

    EngineLIST::POSITION EnginePos = pSvrConfig->ScriptEngineList.GetHeadPosition();
    for(; EnginePos != NULL; EnginePos = EnginePos->GetNext())
    {
        AddListViewRow(hWndListView, EnginePos->Data.ExtName, EnginePos->Data.CommandLine);
    }
    return TRUE;
}

void CConfigMapPage::_GetExistExtNameList(StrLIST &ExistNameList, EngineLIST::POSITION EditPos)
{
    ExistNameList.RemoveAll();

    EngineLIST::POSITION CurPos = pSvrConfig->ScriptEngineList.GetHeadPosition();
    for(; CurPos != NULL; CurPos = CurPos->GetNext())
    {
        if(CurPos != EditPos)
            ExistNameList.AddTail(CurPos->Data.ExtName);
    }
}

BOOL CConfigMapPage::OnBtnAdd(WPARAM WParam, LPARAM LParam)
{
    StrLIST ExistNameList;
    _GetExistExtNameList(ExistNameList, NULL);

    CScriptEngine NewEngine;
    CScriptEngineDlg EngineDlg(NewEngine, ExistNameList);
    if(EngineDlg.DoModal(m_hWnd) == IDOK)
    {
        pSvrConfig->ScriptEngineList.AddTail(NewEngine);
        AddListViewRow(hWndListView, NewEngine.ExtName, NewEngine.CommandLine);
    }
    return TRUE;
}

BOOL CConfigMapPage::OnBtnEdit(WPARAM WParam, LPARAM LParam)
{
    int SelIndex = ListView_GetNextItem(hWndListView, -1, LVNI_SELECTED);
    if(SelIndex != -1)
    {
        TCHAR ExtNameStr[MAX_ENGINE_EXTNAME];
        ListView_GetItemText(hWndListView, SelIndex, 0, ExtNameStr, ARRAY_SIZE(ExtNameStr));

        MtSTRING ExtName(ExtNameStr), CommandLine;
        CScriptEngine CurEngine(ExtNameStr, CommandLine);
        EngineLIST::POSITION CurPos = pSvrConfig->ScriptEngineList.Find(CurEngine);
        if(CurPos != NULL)
        {
            CurEngine = CurPos->Data;

            StrLIST ExistNameList;
            _GetExistExtNameList(ExistNameList, CurPos);

            CScriptEngineDlg EngineDlg(CurEngine, ExistNameList);
            if(EngineDlg.DoModal(m_hWnd) == IDOK)
            {
                CurPos->Data = CurEngine;
                ListView_SetItemText(hWndListView, SelIndex, 0, (LPSTR)(LPCTSTR)CurEngine.ExtName);
                ListView_SetItemText(hWndListView, SelIndex, 1, (LPSTR)(LPCTSTR)CurEngine.CommandLine);
            }
        }
    }

    return TRUE;
}

BOOL CConfigMapPage::OnNotify(WPARAM WParam, LPARAM LParam)
{
    NMHDR *pNmhdr = (NMHDR *)LParam;
    if(pNmhdr->hwndFrom == hWndListView)
    {
        NMLISTVIEW *pNmlv = (NMLISTVIEW *)LParam;
        if(pNmlv->hdr.code == NM_DBLCLK)
            ::SendMessage(m_hWnd, WM_COMMAND, IDC_BTN_EDIT, 0);
    }
    return TRUE;
}

BOOL CConfigMapPage::OnBtnDel(WPARAM WParam, LPARAM LParam)
{
    int SelIndex = ListView_GetNextItem(hWndListView, -1, LVNI_SELECTED);
    if(SelIndex != -1)
    {
        int Ret = MessageBox(m_hWnd, "确实要删除选定的脚本映射吗？", APP_TITLE, MB_YESNO | MB_ICONQUESTION);
        if(Ret == IDYES)
        {
            TCHAR ExtNameStr[MAX_ENGINE_EXTNAME];
            ListView_GetItemText(hWndListView, SelIndex, 0, ExtNameStr, ARRAY_SIZE(ExtNameStr));

            MtSTRING ExtName(ExtNameStr), CommandLine;
            CScriptEngine CurEngine(ExtNameStr, CommandLine);
            EngineLIST::POSITION CurPos = pSvrConfig->ScriptEngineList.Find(CurEngine);
            if(CurPos != NULL)
            {
                pSvrConfig->ScriptEngineList.RemoveAt(CurPos);
                ListView_DeleteItem(hWndListView, SelIndex);
            }
        }
    }
    return TRUE;
}

BOOL CConfigMapPage::OnOK(WPARAM WParam, LPARAM LParam)
{
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScriptEngineDlg, BaseDLG)
    ON_DLGCOMMAND(IDC_BTN_BROWSE,       OnBtnBrowse)
END_MESSAGE_MAP()

static BOOL IsValidEngineParam(const MtSTRING &EngineParam)
{
    // 参数中最多只能有一个%，且必须是%s
    int FormatPos = EngineParam.Find('%');
    if(FormatPos > 0)
    {
        if(EngineParam[FormatPos + 1] != 's')
            return FALSE;
        FormatPos = EngineParam.Find('%', FormatPos + 1);
        if(FormatPos > 0)
            return FALSE;
    }
    return TRUE;
}

static BOOL IsValidEnginePath(const MtSTRING &EngineCommandLine, MtSTRING &EnginePath, MtSTRING &EngineParam, 
                              MtSTRING &ErrInfo)
{
    // Example:
    //   c:\php5\php.exe
    //   "c:\php 5\php.exe" %s
    if(!ParseEngineCommandLine(EngineCommandLine, EnginePath, EngineParam))
    {
        ErrInfo = "错误的可执行文件名";
        return FALSE;
    }

    if(!IsFileExist(EnginePath))
    {
        ErrInfo.Format("文件[%s]不存在!", (LPCTSTR)EnginePath);
        return FALSE;
    }

    BOOL bExe = FALSE;
    MtSTRING ExtName = EnginePath.Right(4);
    if(ExtName.CompareNoCase(".exe") == 0)
        bExe = TRUE;
    else if(ExtName.CompareNoCase(".dll") == 0)
        bExe = FALSE;
    else
    {
        ErrInfo = "必须是exe或dll文件";
        return FALSE;
    }

    if((!bExe && !EngineParam.IsEmpty()) || !IsValidEngineParam(EngineParam))
    {
        ErrInfo = "错误的参数";
        return FALSE;
    }
    return TRUE;
}

static void BuildEngineCommandLine(const MtSTRING &EnginePath, const MtSTRING &EngineParam, MtSTRING &EngineCommandLine)
{
    EngineCommandLine = EnginePath;

    if(EngineCommandLine.Find(' ') > 0)
    {
        EngineCommandLine.Insert(0, "\"");
        EngineCommandLine += "\"";
    }

    if(EngineCommandLine.Right(4).CompareNoCase(".dll") == 0)
        return;

    if(!EngineParam.IsEmpty())
    {
        EngineCommandLine += " ";
        EngineCommandLine += EngineParam;
    }
}

BOOL AdjustEngineCommandLine(MtSTRING &EngineCommandLine)
{
    MtSTRING EnginePath, EngineParam, ErrInfo;
    if(!IsValidEnginePath(EngineCommandLine, EnginePath, EngineParam, ErrInfo))
        return FALSE;

    BuildEngineCommandLine(EnginePath, EngineParam, EngineCommandLine);
    return TRUE;
}

CScriptEngineDlg::CScriptEngineDlg(CScriptEngine &Engine, const StrLIST &ExistNameList) : 
    BaseDLG(IDD_ENGINE), ScriptEngine(Engine), ExistExtNameList(ExistNameList)
{
}

BOOL CScriptEngineDlg::OnInitDialog()
{
    ::SetDlgItemText(m_hWnd, IDC_EDIT_EXTNAME, ScriptEngine.ExtName);
    ::SetDlgItemText(m_hWnd, IDC_EDIT_ENGINEPATH, ScriptEngine.CommandLine);
    return TRUE;
}

BOOL CScriptEngineDlg::OnBtnBrowse(WPARAM WParam, LPARAM LParam)
{
    MtSTRING CommandLine, EnginePath, EngineParam;
    GetDlgItemText(m_hWnd, IDC_EDIT_ENGINEPATH, CommandLine, 300);
    ParseEngineCommandLine(CommandLine, EnginePath, EngineParam);

    const char *pFilter = "可执行程序(*.exe)\0*.exe\0动态链接库(*.dll)\0*.dll\0所有文件(*.*)\0*.*\0";
    DWORD FilterIndex = EnginePath.Right(3).CompareNoCase("dll") == 0 ? 2 : 1;
    CFileDlg FileDlg(true, m_hWnd, pFilter, "exe", FilterIndex, EnginePath);
    if(FileDlg.DoModal())
    {
        BuildEngineCommandLine(FileDlg.GetPathFileName(), EngineParam, CommandLine);
        ::SetDlgItemText(m_hWnd, IDC_EDIT_ENGINEPATH, CommandLine);
    }
    return TRUE;
}

BOOL CScriptEngineDlg::OnOK(WPARAM WParam, LPARAM LParam)
{
    MtSTRING CommandLine;
    GetDlgItemText(m_hWnd, IDC_EDIT_EXTNAME, ScriptEngine.ExtName, MAX_ENGINE_EXTNAME);
    GetDlgItemText(m_hWnd, IDC_EDIT_ENGINEPATH, CommandLine, 300);

    ScriptEngine.ExtName.TrimLeft(". \t");
    ScriptEngine.ExtName.TrimRight(" \t");
    if(ScriptEngine.ExtName.IsEmpty() || CommandLine.IsEmpty())
    {
        MessageBox(m_hWnd, _T("信息填写不完整"), APP_TITLE, MB_ICONWARNING);
        return TRUE;
    }
    ScriptEngine.ExtName.MakeLower();
    ScriptEngine.ExtName.Insert(0, ".");

    if(ExistExtNameList.Find(ScriptEngine.ExtName) != NULL)
    {
        MessageBox(m_hWnd, _T("该扩展名已经存在"), APP_TITLE, MB_ICONWARNING);
        return TRUE;
    }

    MtSTRING EnginePath, EngineParam, ErrInfo;
    if(!IsValidEnginePath(CommandLine, EnginePath, EngineParam, ErrInfo))
    {
        MessageBox(m_hWnd, ErrInfo, APP_TITLE, MB_ICONWARNING);
        ::SetFocus(::GetDlgItem(m_hWnd, IDC_EDIT_ENGINEPATH));
        return TRUE;
    }

    BuildEngineCommandLine(EnginePath, EngineParam, ScriptEngine.CommandLine);

    BaseDLG::OnOK(WParam, LParam);
    return TRUE;
}

BOOL CScriptEngineDlg::OnCancel(WPARAM WParam, LPARAM LParam)
{
    BaseDLG::OnCancel(WParam, LParam);
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CConfigLogPage, PropertyPAGE)
    ON_DLGCOMMAND(IDC_CHECK_LOGTOWINDOW, OnLogToWindow)
    ON_DLGCOMMAND(IDC_CHECK_LOGTOFILE,   OnLogToFile)
    ON_DLGCOMMAND(IDC_BTN_BROWSE,        OnBtnBrowse)
END_MESSAGE_MAP()

CConfigLogPage::CConfigLogPage() : PropertyPAGE(IDD_CONFIG_LOG)
{
    SetTitle("日志");
}

BOOL CConfigLogPage::OnInitDialog()
{
    ::CheckDlgButton(m_hWnd, IDC_CHECK_LOGTOWINDOW, pSvrConfig->bLogToWindow ? BST_CHECKED : BST_UNCHECKED);
    ::CheckDlgButton(m_hWnd, IDC_CHECK_LOGTOFILE, pSvrConfig->bLogToFile ? BST_CHECKED : BST_UNCHECKED);
    ::SetDlgItemInt(m_hWnd, IDC_EDIT_MAXLOGWNDLINE, pSvrConfig->LogWndMaxLine, FALSE);
    ::SetDlgItemText(m_hWnd, IDC_EDIT_LOGFILENAME, pSvrConfig->LogFileName);

    ::SendMessage(m_hWnd, WM_COMMAND, IDC_CHECK_LOGTOWINDOW, 0);
    ::SendMessage(m_hWnd, WM_COMMAND, IDC_CHECK_LOGTOFILE, 0);
    return TRUE;
}

BOOL CConfigLogPage::OnLogToWindow(WPARAM WParam, LPARAM LParam)
{
    BOOL bLogToWindow = ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_LOGTOWINDOW) == BST_CHECKED;
    ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_MAXLOGWNDLINE), bLogToWindow);
    return TRUE;
}

BOOL CConfigLogPage::OnLogToFile(WPARAM WParam, LPARAM LParam)
{
    BOOL bLogToFile = ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_LOGTOFILE) == BST_CHECKED;
    ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDIT_LOGFILENAME), bLogToFile);
    ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BTN_BROWSE), bLogToFile);
    return TRUE;
}

BOOL CConfigLogPage::OnBtnBrowse(WPARAM WParam, LPARAM LParam)
{
    MtSTRING LogFileName;
    GetDlgItemText(m_hWnd, IDC_EDIT_LOGFILENAME, LogFileName, 300);
    if(LogFileName.IsEmpty())
    {
        TCHAR *pBuf = LogFileName.GetBuffer(300);
        ::GetModuleFileName(NULL, pBuf, 300);
        TCHAR *pFind = _tcsrchr(pBuf, _T('.'));
        if(pFind == NULL)
            pFind = pBuf + _tcslen(pBuf);
        _tcscpy(pFind, _T(".log"));
        LogFileName.ReleaseBuffer();
    }

    const TCHAR *pFilter = _T("日志文件(*.log)\0*.log\0所有文件(*.*)\0*.*\0");
    CFileDlg FileDlg(false, m_hWnd, pFilter, _T("log"), 1, LogFileName);
    if(FileDlg.DoModal())
        ::SetDlgItemText(m_hWnd, IDC_EDIT_LOGFILENAME, FileDlg.GetPathFileName());
    return TRUE;
}

BOOL CConfigLogPage::OnOK(WPARAM WParam, LPARAM LParam)
{
    pSvrConfig->bLogToWindow = ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_LOGTOWINDOW) == BST_CHECKED;
    pSvrConfig->bLogToFile = ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_LOGTOFILE) == BST_CHECKED;
    pSvrConfig->LogWndMaxLine = ::GetDlgItemInt(m_hWnd, IDC_EDIT_MAXLOGWNDLINE, NULL, FALSE);
    GetDlgItemText(m_hWnd, IDC_EDIT_LOGFILENAME, pSvrConfig->LogFileName, 300);
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

CServerConfigDlg::CServerConfigDlg(CWebServerConfig &Config) : SvrConfig(Config)
{
    MainPage.pSvrConfig   = &SvrConfig;
    RightPage.pSvrConfig  = &SvrConfig;
    DocPage.pSvrConfig    = &SvrConfig;
    MapPage.pSvrConfig    = &SvrConfig;
    LogPage.pSvrConfig    = &SvrConfig;

    AddPage(&MainPage);
    AddPage(&DocPage);
    AddPage(&RightPage);
    AddPage(&MapPage);
    AddPage(&LogPage);
}

CServerConfigDlg::~CServerConfigDlg()
{
}

BOOL ServerConfigDialog(HWND hWndParent, CWebServerConfig &SvrConfig)
{
    CServerConfigDlg Dlg(SvrConfig);
    //Dlg.SetActivePage(0);
    return Dlg.DoModal(hWndParent) == IDOK;
}
