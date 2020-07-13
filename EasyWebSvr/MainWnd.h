#ifndef __MAINWND_H__
#define __MAINWND_H__

#include "basewnd.h"
#include "systray.h"
#include "mywebsvr.h"
#include "flatbtn.h"
#include "BaseCtrl.h"

class CSvrStatusBar : public StatusBAR
{
protected:
  LONG          OnCommandParent(WPARAM WParam, LPARAM LParam);

  DECLARE_MESSAGE_MAP()
};

class CMainWnd : public BaseWND
{
public:
  CMainWnd();
  virtual ~CMainWnd();

public:

protected:
  LONG          OnCreate(WPARAM WParam, LPARAM LParam);
  LONG          OnDestroy(WPARAM WParam, LPARAM LParam);
  LONG          OnPaint(WPARAM WParam, LPARAM LParam);
  LONG          OnClose(WPARAM WParam, LPARAM LParam);
  LONG          OnInitMenuPop(WPARAM WParam, LPARAM LParam);
  LONG          OnSize(WPARAM WParam, LPARAM LParam);
  LONG          OnNotify(WPARAM WParam, LPARAM LParam);
  LONG          OnEndSession(WPARAM WParam, LPARAM LParam);

  LONG          OnTrayMainWnd(WPARAM WParam, LPARAM LParam);
  LONG          OnTrayBrowse(WPARAM WParam, LPARAM LParam);
  LONG          OnStartServer(WPARAM WParam, LPARAM LParam);
  LONG          OnStopServer(WPARAM WParam, LPARAM LParam);
  LONG          OnRestartServer(WPARAM WParam, LPARAM LParam);
  LONG          OnTrayConfig(WPARAM WParam, LPARAM LParam);
  LONG          OnTrayClearLog(WPARAM WParam, LPARAM LParam);
  LONG          OnTraySaveLog(WPARAM WParam, LPARAM LParam);
  LONG          OnTrayAbout(WPARAM WParam, LPARAM LParam);
  LONG          OnTrayExit(WPARAM WParam, LPARAM LParam);
  LONG          OnBtnStartStop(WPARAM WParam, LPARAM LParam);
  LONG          OnBtnShowMenu(WPARAM WParam, LPARAM LParam);

  LONG          OnSystrayNotify(WPARAM WParam, LPARAM LParam);
  LONG          OnTaskbarCreated(WPARAM WParam, LPARAM LParam);
  LONG          OnContextMenu(WPARAM WParam, LPARAM LParam);
  
  BOOL          LoadConfigInfo(CWebServerConfig &ServerConfig);
  BOOL          SaveConfigInfo(const CWebServerConfig &ServerConfig);
  BOOL          LoadStartParam(BOOL &bShowWnd, BOOL &bStartRun);
  BOOL          SaveStartParam(BOOL bShowWnd, BOOL bStartRun);
  BOOL          CloseApplication(BOOL bSure);
  void          AddSysTrayIcon(HWND hWnd);
  void          SetServerStatusView(BOOL bRunning);
  
  virtual void  PreRegisterClass(WNDCLASSEX *pWcex);

protected:
  CSvrStatusBar  StatusBar;      // 状态栏
  HWND          hListBoxLog;
  HWND          hTooltips;
  FlatBUTTON    BtnStartStop;
  FlatBUTTON    BtnShowMenu;
  minilib::CSystemTray SysTray;
  CWebServerConfig ServerConfig;
  CMyWebServer  WebServer;
  MtSTRING      IniFileName;
  HFONT         hFontList;

  DECLARE_MESSAGE_MAP()
};

#endif // __MAINWND_H__