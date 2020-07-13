#include "stdafx.h"
#include "resource.h"
#include "FileDlg.h"
#include "cmncode.h"
#include "svrcfgdlg.h"
#include "aboutdlg.h"
#include "MainWnd.h"

#define WM_TRAYNOTIFY           WM_USER + 100

static const UINT WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));

BEGIN_MESSAGE_MAP(CSvrStatusBar, StatusBAR)
    ON_COMMAND(IDC_BTN_STARTSTOP,     OnCommandParent)
    ON_COMMAND(IDC_BTN_SHOWMENU,      OnCommandParent)
END_MESSAGE_MAP()

LONG CSvrStatusBar::OnCommandParent(WPARAM WParam, LPARAM LParam)
{
    return (LONG)::SendMessage(::GetParent(m_hWnd), WM_COMMAND, WParam, LParam);
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMainWnd, BaseWND)
    ON_MESSAGE(WM_CREATE,             OnCreate)
    ON_MESSAGE(WM_PAINT,              OnPaint)
    ON_MESSAGE(WM_DESTROY,            OnDestroy)
    ON_MESSAGE(WM_CLOSE,              OnClose)
    ON_MESSAGE(WM_SIZE,               OnSize)
    ON_MESSAGE(WM_INITMENUPOPUP,      OnInitMenuPop)
    ON_MESSAGE(WM_TRAYNOTIFY,         OnSystrayNotify)
    ON_MESSAGE(WM_TASKBARCREATED,     OnTaskbarCreated)
    ON_MESSAGE(WM_CONTEXTMENU,        OnContextMenu)
    ON_MESSAGE(WM_NOTIFY,             OnNotify)
    ON_MESSAGE(WM_ENDSESSION,         OnEndSession)

    ON_COMMAND(IDM_TRAY_MAINWND,      OnTrayMainWnd)
    ON_COMMAND(IDM_TRAY_BROWSE,       OnTrayBrowse)
    ON_COMMAND(IDM_TRAY_STARTSERVER,  OnStartServer)
    ON_COMMAND(IDM_TRAY_STOPSERVER,   OnStopServer)
    ON_COMMAND(IDM_TRAY_RESTARTSERVER,OnRestartServer)
    ON_COMMAND(IDM_TRAY_CONFIG,       OnTrayConfig)
    ON_COMMAND(IDM_TRAY_CLEARLOG,     OnTrayClearLog)
    ON_COMMAND(IDM_TRAY_SAVELOG,      OnTraySaveLog)
    ON_COMMAND(IDM_TRAY_ABOUT,        OnTrayAbout)
    ON_COMMAND(IDM_TRAY_EXIT,         OnTrayExit)
    ON_COMMAND(IDC_BTN_STARTSTOP,     OnBtnStartStop)
    ON_COMMAND(IDC_BTN_SHOWMENU,      OnBtnShowMenu)
END_MESSAGE_MAP()


static DWORD GetPrivateProfileString(LPCTSTR pAppName, LPCTSTR pKeyName, LPCTSTR pDefault,
                                     MtSTRING &RetString, DWORD MaxSize, LPCTSTR pFileName)
{
    assert(pDefault != (LPCTSTR)RetString);
    DWORD Ret = ::GetPrivateProfileString(pAppName, pKeyName, pDefault,
        RetString.GetBuffer(MaxSize + 1), MaxSize, pFileName);
    RetString.ReleaseBuffer();
    return Ret;
}

static BOOL WritePrivateProfileInt(LPCTSTR pAppName, LPCTSTR pKeyName, int IntValue, LPCTSTR pFileName)
{
    TCHAR TempStr[128];
    wsprintf(TempStr, _T("%d"), IntValue);
    return ::WritePrivateProfileString(pAppName, pKeyName, TempStr, pFileName);
}

// 写到注册表的启动组
static BOOL WriteRegStartRun(BOOL bStartRun)
{
    char RegKeyPos[] = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    char KeyName[] = "EasyWebServer";

    if(bStartRun)
    {
        char SelfExeFileName[300];
        ::GetModuleFileName(::GetModuleHandle(NULL), SelfExeFileName, sizeof(SelfExeFileName));

        HKEY hKey;
        if(RegCreateKey(HKEY_LOCAL_MACHINE, (LPCTSTR)RegKeyPos, &hKey) != ERROR_SUCCESS)
            return FALSE;

        BOOL bResult = TRUE;
        if(RegSetValueEx(hKey, KeyName, 0, REG_SZ, (LPBYTE)SelfExeFileName, 
            lstrlen(SelfExeFileName)) !=  ERROR_SUCCESS)
            bResult = FALSE;

        RegCloseKey(hKey);
        return bResult;
    }
    else
    {
        HKEY hKey;
        ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, RegKeyPos, 0, KEY_WRITE, &hKey);

        DWORD Result = ::RegDeleteValue(hKey, KeyName);
        RegCloseKey(hKey);

        return (Result == ERROR_SUCCESS);
    }
}

static HWND CreateTooltips(HWND hWndParent)
{
    INITCOMMONCONTROLSEX Iccex;
    Iccex.dwICC = ICC_BAR_CLASSES ;
    Iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    InitCommonControlsEx(&Iccex);

    HWND hWndTips = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT, hWndParent, NULL, ::GetModuleHandle(NULL), NULL);

    SetWindowPos(hWndTips, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    return hWndTips;
}

static void AddTooltips(HWND hWndTips, HWND hWndParent, HWND hWndChild, const char *pTipsText, BOOL bCallBack = FALSE)
{
    char TempText[500];
    if(!bCallBack)
        lstrcpyn(TempText, pTipsText, 499);

    TOOLINFO ToolInfo;
    ToolInfo.cbSize = sizeof(TOOLINFO);
    ToolInfo.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ToolInfo.hwnd = hWndParent;
    ToolInfo.uId = (WPARAM)hWndChild;
    ToolInfo.lpszText = bCallBack ? LPSTR_TEXTCALLBACK : TempText;

    SendMessage(hWndTips, TTM_ADDTOOL, 0, (LPARAM)(&ToolInfo));
}

CMainWnd::CMainWnd()
{
    hListBoxLog     = NULL;
    hTooltips       = NULL;

    // Ini
    TCHAR *pBuffer = IniFileName.GetBuffer(300);
    ::GetModuleFileName(::GetModuleHandle(NULL), pBuffer, 300);
    lstrcpy(_tcsrchr(pBuffer, '\\') + 1, _T("EasyWebSvr.ini"));
    IniFileName.ReleaseBuffer();

    // Create Font
    LOGFONT LogFont;
    ::memset(&LogFont, 0, sizeof(LOGFONT));
    lstrcpy(LogFont.lfFaceName, "宋体");
    LogFont.lfWeight = 400;
    LogFont.lfHeight = -12;
    LogFont.lfCharSet = 134;
    LogFont.lfOutPrecision = 1;
    LogFont.lfClipPrecision = 2;
    LogFont.lfQuality = 1;
    LogFont.lfPitchAndFamily = 49;
    hFontList = ::CreateFontIndirect(&LogFont);
}

CMainWnd::~CMainWnd()
{
    ::DeleteObject(hFontList);
}

void CMainWnd::PreRegisterClass(WNDCLASSEX *pWcex)
{
    pWcex->hIcon = ::LoadIcon(::GetModuleHandle(NULL), (LPCTSTR)IDI_ICON_MAIN);
    pWcex->hCursor = ::LoadCursor(NULL, IDC_ARROW);
    pWcex->lpszMenuName = NULL;//(LPCTSTR)IDR_MAIN_MENU;
    pWcex->hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);//GetStockObject(WHITE_BRUSH);
}

void CMainWnd::AddSysTrayIcon(HWND hWnd)
{
    HICON hIconStop = ::LoadIcon(::GetModuleHandle(NULL), (LPCTSTR)IDI_TRAYSTOP);
    SysTray.Create(hWnd, WM_TRAYNOTIFY, _T(""), hIconStop, IDR_MENU_TRAY);
    SysTray.SetMenuItemDoubleClick(IDM_TRAY_MAINWND);
    if(ServerConfig.bIconStartStop)
        SysTray.SetMenuItemLeftClick(IDC_BTN_STARTSTOP);
}

LONG CMainWnd::OnDestroy(WPARAM WParam, LPARAM LParam)
{ 
    PostQuitMessage(0);
    return 0;
}

LONG CMainWnd::OnInitMenuPop(WPARAM WParam, LPARAM LParam)
{
    HMENU hMenuTray = (HMENU)WParam;
    BOOL bRunning = WebServer.IsServerRunning();
    EnableMenuItem(hMenuTray, IDM_TRAY_STARTSERVER, (bRunning ? MF_GRAYED : MF_ENABLED) | MF_BYCOMMAND);
    EnableMenuItem(hMenuTray, IDM_TRAY_STOPSERVER, (bRunning ? MF_ENABLED : MF_GRAYED) | MF_BYCOMMAND);
    EnableMenuItem(hMenuTray, IDM_TRAY_RESTARTSERVER, (bRunning ? MF_ENABLED : MF_GRAYED) | MF_BYCOMMAND);
    EnableMenuItem(hMenuTray, IDM_TRAY_BROWSE, (bRunning ? MF_ENABLED : MF_GRAYED) | MF_BYCOMMAND);

    const TCHAR *pMenuItemName = IsWindowVisible(m_hWnd) ? "隐藏主窗口" : "显示主窗口";
    ModifyMenu(hMenuTray, IDM_TRAY_MAINWND, MF_BYCOMMAND | MF_STRING, IDM_TRAY_MAINWND, pMenuItemName);
    return 0;
}

LONG CMainWnd::OnPaint(WPARAM WParam, LPARAM LParam)
{
    PAINTSTRUCT Ps;
    HDC hDc = BeginPaint(m_hWnd, &Ps);

    ::SetBkMode(hDc, TRANSPARENT);
    HFONT hFontOld = (HFONT)::SelectObject(hDc, hFontList);
    ::TextOut(hDc, 0, 8, "访问日志:", 9);
    ::SelectObject(hDc, hFontOld);

    EndPaint(m_hWnd, &Ps);
    return 0;
}

LONG CMainWnd::OnCreate(WPARAM WParam, LPARAM LParam)
{ 
    hListBoxLog = CreateWindowEx(WS_EX_CLIENTEDGE, _T("ListBox"), _T(""), 
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | LBS_NOINTEGRALHEIGHT, //LBS_NOTIFY | 
        0, 0, 100, 100, m_hWnd, (HMENU)0, (HINSTANCE)::GetModuleHandle(NULL), NULL); 
    assert(hListBoxLog != NULL);
    ::SendMessage(hListBoxLog, WM_SETFONT, (WPARAM)hFontList, 0); 

    StatusBar.CreateEx(m_hWnd);
    UINT Length[] = {10, 20, 30, -1}; 
    StatusBar.SetParts(Length, sizeof(Length) / sizeof(Length[0]));

    BtnStartStop.CreateEx(StatusBar, 0, WS_CHILD | WS_VISIBLE, _T(""), 0, 0, 10, 10, IDC_BTN_STARTSTOP);
    BtnStartStop.SetIcon(IDI_LEDSTOP);
    BtnShowMenu.CreateEx(StatusBar, 0, WS_CHILD | WS_VISIBLE, _T(""), 0, 0, 10, 10, IDC_BTN_SHOWMENU);
    BtnShowMenu.SetIcon(IDI_SHOWMENU);

    LoadConfigInfo(ServerConfig);
    WebServer.SvrLog.ListBoxLog.SetListBoxWnd(hListBoxLog, 5000);
    WebServer.SvrLog.ListBoxLog.AddLog("EasyWebServer V1.92", TRUE);

    AddSysTrayIcon(m_hWnd);
    SetServerStatusView(FALSE);

    hTooltips = CreateTooltips(m_hWnd);
    AddTooltips(hTooltips, m_hWnd, BtnStartStop.m_hWnd, LPSTR_TEXTCALLBACK, TRUE);
    AddTooltips(hTooltips, m_hWnd, BtnShowMenu.m_hWnd, "显示菜单");

    BOOL bShowWnd = TRUE, bStartRun = FALSE;
    LoadStartParam(bShowWnd, bStartRun);
    if(bStartRun)
        ::PostMessage(m_hWnd, WM_COMMAND, IDM_TRAY_STARTSERVER, 0);
    if(!bShowWnd)
        ::PostMessage(m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
    return 0;
}

LONG CMainWnd::OnSize(WPARAM WParam, LPARAM LParam)
{
    if(WParam == SIZE_MINIMIZED)
    {
        ::ShowWindow(m_hWnd, SW_HIDE);
        return 0;
    }

    StatusBar.OnSize(WParam, LParam);

    RECT ClientRect;
    GetClientRect(m_hWnd, &ClientRect);
    ClientRect.bottom -= StatusBar.GetSize().cy;

    UINT Length[] = {ClientRect.right - 146, ClientRect.right - 124, ClientRect.right - 102, -1}; 
    StatusBar.SetParts(Length, sizeof(Length) / sizeof(Length[0]));

    RECT BtnRect;
    ::SendMessage(StatusBar, SB_GETRECT, 1, (LPARAM)&BtnRect);
    ::InflateRect(&BtnRect, -1, -1);
    ::MoveWindow(BtnStartStop, BtnRect.left, BtnRect.top, BtnRect.right - BtnRect.left, 
        BtnRect.bottom - BtnRect.top, TRUE);

    ::SendMessage(StatusBar, SB_GETRECT, 2, (LPARAM)&BtnRect);
    ::InflateRect(&BtnRect, -1, -1);
    ::MoveWindow(BtnShowMenu, BtnRect.left, BtnRect.top, BtnRect.right - BtnRect.left, 
        BtnRect.bottom - BtnRect.top, TRUE);

    ::MoveWindow(hListBoxLog, 0, 25, ClientRect.right, ClientRect.bottom - 25, TRUE);
    return 0;
}

LONG CMainWnd::OnClose(WPARAM WParam, LPARAM LParam)
{
#ifdef _DEBUG
    ::SendMessage(m_hWnd, WM_COMMAND, IDM_TRAY_EXIT, 0);
#else
    ::ShowWindow(m_hWnd, SW_MINIMIZE);
#endif
    return 0;
}

LONG CMainWnd::OnNotify(WPARAM WParam, LPARAM LParam)
{
    LPNMHDR pNmHdr=(LPNMHDR)LParam;
    if(pNmHdr->hwndFrom == hTooltips && pNmHdr->code == TTN_GETDISPINFO)
    {
        LPNMTTDISPINFO pDispInfo = (LPNMTTDISPINFO)LParam;
        char TipsText[100];
        if(WebServer.IsServerRunning())
            lstrcpy(TipsText, "停止服务器");
        else
            lstrcpy(TipsText, "启动服务器");
        pDispInfo->lpszText = TipsText;
    }
    return 0;
}

// 系统关机
LONG CMainWnd::OnEndSession(WPARAM WParam, LPARAM LParam)
{
    CloseApplication(FALSE);
    return 0;
}

LONG CMainWnd::OnTrayMainWnd(WPARAM WParam, LPARAM LParam)
{
    if(IsWindowVisible(m_hWnd))
    {
        ::ShowWindow(m_hWnd, SW_MINIMIZE);
    }
    else
    {
        ::ShowWindow(m_hWnd, SW_SHOWNORMAL);
        ::SetForegroundWindow(m_hWnd);
    }
    return 0;
}

LONG CMainWnd::OnTrayBrowse(WPARAM WParam, LPARAM LParam)
{
    if(!WebServer.IsServerRunning())
    {
        MessageBox(m_hWnd, "请先启动服务器", APP_TITLE, MB_ICONWARNING);
        return 0;
    }
    else
    {
        char HostName[64];
        if(gethostname(HostName, 64) != 0)
            strcpy(HostName, "localhost");

        int Port = WebServer.GetServerConfig().ListenPort;
        MtSTRING Url;
        if(Port == 80)
            Url.Format("http://%s", HostName);
        else
            Url.Format("http://%s:%d", HostName, Port);
        ShellExecute(NULL, NULL, Url, NULL, NULL, SW_SHOWNORMAL);
        return 0;
    }
}

LONG CMainWnd::OnStartServer(WPARAM WParam, LPARAM LParam)
{
    if(WebServer.IsServerRunning())
    {
        MessageBox(m_hWnd, "服务器已经启动!", APP_TITLE, MB_ICONWARNING);
        return TRUE;
    }

    if(!WebServer.Init(ServerConfig) || !WebServer.StartServer())
    {
        MtSTRING ErrText;
        ErrText.Format(_T("启动服务器失败! (%s)"), (LPCTSTR)WebServer.GetLastErrStr());
        WebServer.SvrLog.ListBoxLog.AddLog(ErrText, TRUE);
        return TRUE;
    }

    MtSTRING LogText;
    LogText.Format("服务器成功启动! (%s:%d)", 
        (LPCTSTR)ServerConfig.RootDirectory, ServerConfig.ListenPort);
    WebServer.SvrLog.ListBoxLog.AddLog(LogText, TRUE);

    SetServerStatusView(TRUE);
    return 0;
}

void CMainWnd::SetServerStatusView(BOOL bRunning)
{
    if(bRunning)
    {
        MtSTRING StatusText;
        StatusText.Format("服务器运行中... (%s)", 
            (LPCTSTR)ServerConfig.RootDirectory);
        StatusBar.SetPaneText(0, StatusText);
        StatusText.Format("127.0.0.1:%d", ServerConfig.ListenPort);
        StatusBar.SetPaneText(3, StatusText);

        BtnStartStop.SetIcon(IDI_LEDRUN);

        SysTray.SetIcon(IDI_TRAYRUN);
        SysTray.SetTooltipText("EasyWebServer - 运行中");
    }
    else
    {
        StatusBar.SetPaneText(0, "服务器已停止");
        StatusBar.SetPaneText(3, _T("0.0.0.0:0"));

        BtnStartStop.SetIcon(IDI_LEDSTOP);

        SysTray.SetIcon(IDI_TRAYSTOP);
        SysTray.SetTooltipText("EasyWebServer - 已停止");
    }
}

LONG CMainWnd::OnStopServer(WPARAM WParam, LPARAM LParam)
{
    if(!WebServer.StopServer())
    {
        MessageBox(m_hWnd, WebServer.GetLastErrStr(), APP_TITLE, MB_ICONWARNING);
        return TRUE;
    }

    WebServer.SvrLog.ListBoxLog.AddLog("服务器成功停止!", TRUE);
    SetServerStatusView(FALSE);
    return 0;
}

LONG CMainWnd::OnRestartServer(WPARAM WParam, LPARAM LParam)
{
    if(WebServer.IsServerRunning())
        SendMessage(m_hWnd, WM_COMMAND, IDM_TRAY_STOPSERVER, 0);
    SendMessage(m_hWnd, WM_COMMAND, IDM_TRAY_STARTSERVER, 0);
    return 0;
}

LONG CMainWnd::OnBtnStartStop(WPARAM WParam, LPARAM LParam)
{
    if(WebServer.IsServerRunning())
        SendMessage(m_hWnd, WM_COMMAND, IDM_TRAY_STOPSERVER, 0);
    else
        SendMessage(m_hWnd, WM_COMMAND, IDM_TRAY_STARTSERVER, 0);
    return 0;
}

LONG CMainWnd::OnBtnShowMenu(WPARAM WParam, LPARAM LParam)
{
    ::SendMessage(m_hWnd, WM_CONTEXTMENU, (WPARAM)m_hWnd, 0);
    return 0;
}

LONG CMainWnd::OnTrayConfig(WPARAM WParam, LPARAM LParam)
{
    static BOOL bPop = FALSE;
    if(!bPop)
    {
        bPop = TRUE;
        CWebServerConfig TempConfig(ServerConfig);
        if(ServerConfigDialog(m_hWnd, TempConfig))
        {
            ServerConfig = TempConfig;

            SaveConfigInfo(ServerConfig);
            WriteRegStartRun(ServerConfig.bAutoRun);

            if(WebServer.IsServerRunning())
            {
                int Ret = MessageBox(m_hWnd, "要想设置生效，必须重启服务器，要重启吗？", APP_TITLE, MB_YESNO | MB_ICONWARNING);
                if(Ret == IDYES)
                    ::SendMessage(m_hWnd, WM_COMMAND, IDM_TRAY_RESTARTSERVER, 0);
            }
        }
        bPop = FALSE;
    }
    return 0;
}

LONG CMainWnd::OnTrayClearLog(WPARAM WParam, LPARAM LParam)
{
    ::SendMessage(hListBoxLog, LB_RESETCONTENT, 0, 0);
    ::SendMessage(hListBoxLog, LB_SETHORIZONTALEXTENT, 0, 0);
    WebServer.SvrLog.MaxTextLen = 0;
    return 0;
}

LONG CMainWnd::OnTraySaveLog(WPARAM WParam, LPARAM LParam)
{
    const char *pFilter = "文本文件(*.txt)\0*.txt\0所有文件(*.*)\0*.*\0";
    CFileDlg FileDlg(false, m_hWnd, pFilter, "txt");
    if(!FileDlg.DoModal())
        return 0;

    FILE *pFile = fopen(FileDlg.GetPathFileName(), "w");
    if(pFile == NULL)
    {
        MtSTRING ErrInfo;
        ErrInfo.Format("写文件[%s]失败！", (LPCTSTR)(FileDlg.GetPathFileName()));
        MessageBox(m_hWnd, ErrInfo, APP_TITLE, MB_ICONWARNING);
        return 0;
    }

    char TextBuffer[2048];
    int Count = (int)::SendMessage(hListBoxLog, LB_GETCOUNT, 0, 0);
    for(int i = 0; i < Count; i++)
    {
        ::SendMessage(hListBoxLog, LB_GETTEXT, i, (LPARAM)TextBuffer);
        fprintf(pFile, "%s\n", TextBuffer);
    }
    fclose(pFile);
    return 0;
}

LONG CMainWnd::OnTrayAbout(WPARAM WParam, LPARAM LParam)
{
    AboutDlgModal(m_hWnd);
    return 0;
}

BOOL CMainWnd::CloseApplication(BOOL bSure)
{
    BOOL bShowWnd = IsWindowVisible(m_hWnd);
    BOOL bStartRun = FALSE;
    if(WebServer.IsServerRunning())
    {
        bStartRun = TRUE;
        if(bSure)
        {
            int Ret = MessageBox(m_hWnd, "服务器正在运行，确定退出？", "退出确认", MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING);
            if(Ret != IDYES)
                return FALSE;
        }
        SendMessage(m_hWnd, WM_COMMAND, IDM_TRAY_STOPSERVER, 0);
    }

    SaveStartParam(bShowWnd, bStartRun);
    //SaveConfigInfo(ServerConfig);

    SysTray.RemoveIcon();
    ::DestroyWindow(m_hWnd);
    return TRUE;
}

LONG CMainWnd::OnTrayExit(WPARAM WParam, LPARAM LParam)
{
    CloseApplication(TRUE);
    return 0;
}

static void StringToEngineList(TCHAR *pEngineListStr, EngineLIST &EngineList)
{
    EngineList.RemoveAll();

    TCHAR *pEngineStr = pEngineListStr;
    while(pEngineStr && *pEngineStr)
    {
        int StrLen = (int)_tcslen(pEngineStr);
        TCHAR *pCommandLine = _tcschr(pEngineStr, _T('='));
        if(pCommandLine != NULL)
        {
            *pCommandLine = 0;
            pCommandLine++;

            CScriptEngine NewEngine;
            NewEngine.ExtName = pEngineStr;
            NewEngine.CommandLine = pCommandLine;
            NewEngine.ExtName.TrimLeft(".");
            if(!NewEngine.ExtName.IsEmpty() && AdjustEngineCommandLine(NewEngine.CommandLine))
            {
                NewEngine.ExtName.Insert(0, _T("."));
                if(EngineList.Find(NewEngine) == NULL)
                    EngineList.AddTail(NewEngine);
            }
        }

        pEngineStr += StrLen + 1;
    }
}

static void EngineListToString(const EngineLIST &EngineList, MemFILE2 &EngineString)
{
    EngineString.Resize(0);

    MtSTRING TempStr;
    EngineLIST::POSITION EnginePos = EngineList.GetHeadPosition();
    for(; EnginePos != NULL; EnginePos = EnginePos->GetNext())
    {
        TempStr.Format("%s=%s", (LPCTSTR)EnginePos->Data.ExtName, (LPCTSTR)EnginePos->Data.CommandLine);
        TempStr.TrimLeft(".");
        EngineString.Write((LPCTSTR)TempStr, TempStr.GetLength() + 1);
    }
    EngineString.Write("", 1);
}

BOOL CMainWnd::LoadConfigInfo(CWebServerConfig &ServerConfig)
{
    // Common
    GetPrivateProfileString("WebServer", "RootDir", "", ServerConfig.RootDirectory, 300, IniFileName);
    ServerConfig.ListenPort = ::GetPrivateProfileInt("WebServer", "ListenPort", ServerConfig.ListenPort, IniFileName);
    ServerConfig.MaxThreadNum = ::GetPrivateProfileInt("WebServer", "MaxThreadNum", ServerConfig.MaxThreadNum, IniFileName);
    ServerConfig.NetTimeout = ::GetPrivateProfileInt("WebServer", "Timeout", ServerConfig.NetTimeout, IniFileName);
    ServerConfig.bAutoRun = ::GetPrivateProfileInt("WebServer", "bAutoRun", ServerConfig.bAutoRun, IniFileName);
    ServerConfig.bIconStartStop = ::GetPrivateProfileInt("WebServer", "bIconStartStop", ServerConfig.bIconStartStop, IniFileName);
    ServerConfig.MaxContentLength = ::GetPrivateProfileInt("WebServer", "MaxContentLength", ServerConfig.MaxContentLength, IniFileName);
    ServerConfig.MaxHeaderLength = ::GetPrivateProfileInt("WebServer", "MaxHeaderLength", ServerConfig.MaxHeaderLength, IniFileName);

    // Cgi
    ServerConfig.bEnableCgi = ::GetPrivateProfileInt("Cgi", "EnableCgi", ServerConfig.bEnableCgi, IniFileName);
    ServerConfig.bEnableIsapi = ::GetPrivateProfileInt("Cgi", "EnableIsapi", ServerConfig.bEnableIsapi, IniFileName);
    ServerConfig.bIsapiCache = ::GetPrivateProfileInt("Cgi", "IsapiCache", ServerConfig.bIsapiCache, IniFileName);
    ServerConfig.bEnableScript = ::GetPrivateProfileInt("Cgi", "EnablePhp", ServerConfig.bEnableScript, IniFileName);

    // Docs
    MtSTRING DefaultHtmlStr;
    GetPrivateProfileString("Docs", "DefaultHtml", "index.htm,index.html,default.htm,index.php", DefaultHtmlStr, 300, IniFileName);
    ServerConfig.SetDefaultHtmlList(DefaultHtmlStr);
    ServerConfig.bEnableShowDir = ::GetPrivateProfileInt("Docs", "EnableDirBrowse", ServerConfig.bEnableShowDir, IniFileName);
    ServerConfig.bShowDirAlways = ::GetPrivateProfileInt("Docs", "ShowDirAlways", ServerConfig.bShowDirAlways, IniFileName);

    // ScriptEngine
    TCHAR EngineListStr[32767];
    GetPrivateProfileSection(_T("ScriptEngine"), EngineListStr, ARRAY_SIZE(EngineListStr), IniFileName);
    StringToEngineList(EngineListStr, ServerConfig.ScriptEngineList);

    // Log
    ServerConfig.bLogToWindow = ::GetPrivateProfileInt("Log", "LogToWindow", ServerConfig.bLogToWindow, IniFileName);
    ServerConfig.LogWndMaxLine = ::GetPrivateProfileInt("Log", "LogWndMaxLine", ServerConfig.LogWndMaxLine, IniFileName);
    ServerConfig.bLogToFile = ::GetPrivateProfileInt("Log", "LogToFile", ServerConfig.bLogToFile, IniFileName);
    GetPrivateProfileString("Log", "LogFileName", "", ServerConfig.LogFileName, 300, IniFileName);
    return TRUE;
}

BOOL CMainWnd::SaveConfigInfo(const CWebServerConfig &ServerConfig)
{
    // Common
    ::WritePrivateProfileString("WebServer", "RootDir", ServerConfig.RootDirectory, IniFileName);
    ::WritePrivateProfileInt("WebServer", "ListenPort", ServerConfig.ListenPort, IniFileName);
    ::WritePrivateProfileInt("WebServer", "MaxThreadNum", ServerConfig.MaxThreadNum, IniFileName);
    ::WritePrivateProfileInt("WebServer", "Timeout", ServerConfig.NetTimeout, IniFileName);
    ::WritePrivateProfileInt("WebServer", "bAutoRun", ServerConfig.bAutoRun, IniFileName);
    ::WritePrivateProfileInt("WebServer", "bIconStartStop", ServerConfig.bIconStartStop, IniFileName);
    ::WritePrivateProfileInt("WebServer", "MaxContentLength", ServerConfig.MaxContentLength, IniFileName);
    ::WritePrivateProfileInt("WebServer", "MaxHeaderLength", ServerConfig.MaxHeaderLength, IniFileName);

    // Cgi
    ::WritePrivateProfileInt("Cgi", "EnableCgi", ServerConfig.bEnableCgi ? 1 : 0, IniFileName);
    ::WritePrivateProfileInt("Cgi", "EnableIsapi", ServerConfig.bEnableIsapi ? 1 : 0, IniFileName);
    ::WritePrivateProfileInt("Cgi", "IsapiCache", ServerConfig.bIsapiCache ? 1 : 0, IniFileName);
    ::WritePrivateProfileInt("Cgi", "EnablePhp", ServerConfig.bEnableScript ? 1 : 0, IniFileName);

    // Docs
    MtSTRING DefaultHtmlStr;
    ServerConfig.GetDefaultHtmlStr(DefaultHtmlStr);
    ::WritePrivateProfileString("Docs", "DefaultHtml", DefaultHtmlStr, IniFileName);
    ::WritePrivateProfileInt("Docs", "EnableDirBrowse", ServerConfig.bEnableShowDir ? 1 : 0, IniFileName);
    ::WritePrivateProfileInt("Docs", "ShowDirAlways", ServerConfig.bShowDirAlways ? 1 : 0, IniFileName);

    // ScriptEngine
    MemFILE2 EngineListStr;
    EngineListToString(ServerConfig.ScriptEngineList, EngineListStr);
    WritePrivateProfileSection(_T("ScriptEngine"), (LPCSTR)EngineListStr.GetPtr(), IniFileName);

    // Log
    ::WritePrivateProfileInt("Log", "LogToWindow", ServerConfig.bLogToWindow ? 1 : 0, IniFileName);
    ::WritePrivateProfileInt("Log", "LogWndMaxLine", ServerConfig.LogWndMaxLine, IniFileName);
    ::WritePrivateProfileInt("Log", "LogToFile", ServerConfig.bLogToFile ? 1 : 0, IniFileName);
    ::WritePrivateProfileString("Log", "LogFileName", ServerConfig.LogFileName, IniFileName);
    return TRUE;
}

BOOL CMainWnd::LoadStartParam(BOOL &bShowWnd, BOOL &bStartRun)
{
    bShowWnd = ::GetPrivateProfileInt("StartParam", "bShowWnd", 1, IniFileName);
    bStartRun = ::GetPrivateProfileInt("StartParam", "bStartRun", 0, IniFileName);
    return TRUE;
}

BOOL CMainWnd::SaveStartParam(BOOL bShowWnd, BOOL bStartRun)
{
    ::WritePrivateProfileInt("StartParam", "bShowWnd", bShowWnd ? 1 : 0, IniFileName);
    ::WritePrivateProfileInt("StartParam", "bStartRun", bStartRun ? 1 : 0, IniFileName);
    return TRUE;
}

LONG CMainWnd::OnSystrayNotify(WPARAM WParam, LPARAM LParam)
{
    SysTray.OnTrayNotification(WParam, LParam);
    return 0;
}

LONG CMainWnd::OnTaskbarCreated(WPARAM WParam, LPARAM LParam)
{
    SysTray.RemoveIcon();
    AddSysTrayIcon(m_hWnd);
    SetServerStatusView(WebServer.IsServerRunning());
    return 0;
}

LONG CMainWnd::OnContextMenu(WPARAM WParam, LPARAM LParam)
{
    POINT MousePos;
    ::GetCursorPos(&MousePos);

    HMENU hMenuMain = ::LoadMenu(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU_TRAY));
    if(hMenuMain == NULL)
        return FALSE;
    HMENU hMenuTray = ::GetSubMenu(hMenuMain, 0);
    if(hMenuTray == NULL)
        return FALSE;

    TrackPopupMenu(hMenuTray, 0, 
        MousePos.x, MousePos.y, 0, m_hWnd, NULL); 

    DestroyMenu(hMenuMain);
    return 0;
}