#include "stdafx.h"
#include "systray.h"

namespace minilib
{

CSystemTray::CSystemTray()
{
  m_nDbClickMenuItem       = 0;
  m_nLeftClickMenuItem     = 0;
  m_nIconCount             = 0;
  m_nTimerId               = 0;
  pIconList             = NULL;
}

CSystemTray::~CSystemTray()
{
  if(pIconList != NULL)
    delete pIconList;
}

bool CSystemTray::Create(HWND hWndParent, UINT CallbackMessage, const TCHAR *pToolTip, 
                        HICON hIcon, UINT MenuIDResource)
{
  assert(CallbackMessage >= WM_USER);
  assert(lstrlen(pToolTip) < 64);
  assert(::IsWindow(hWndParent));

  m_Nid.cbSize            = sizeof(m_Nid);
  m_Nid.uFlags            = NIF_ICON | NIF_TIP | NIF_MESSAGE;
  m_Nid.uCallbackMessage  = CallbackMessage;
  m_Nid.hIcon             = hIcon;
  m_Nid.hWnd              = hWndParent;
  m_Nid.uID               = MenuIDResource;

  lstrcpyn(m_Nid.szTip, pToolTip, 64);
  return Shell_NotifyIcon(NIM_ADD, &m_Nid) ? true : false;
}

void CSystemTray::RemoveIcon()
{
  Shell_NotifyIcon(NIM_DELETE, &m_Nid);
}

bool CSystemTray::SetTooltipText(const TCHAR *pToolTips)
{
  m_Nid.uFlags = NIF_TIP;
  lstrcpyn(m_Nid.szTip, pToolTips, 64);

  return Shell_NotifyIcon(NIM_MODIFY, &m_Nid) ? true : false;
}

bool CSystemTray::SetIcon(HICON hIcon)
{
  m_Nid.uFlags = NIF_ICON;
  m_Nid.hIcon = hIcon;

  return Shell_NotifyIcon(NIM_MODIFY, &m_Nid) ? true : false;
}

bool CSystemTray::SetIcon(UINT IconIDResource)
{
  HICON hIcon = ::LoadIcon(::GetModuleHandle(NULL), MAKEINTRESOURCE(IconIDResource));
  assert(hIcon != NULL);

  return SetIcon(hIcon);
}

bool CSystemTray::CreateIconList(UINT FirstIconID, UINT LastIconID)
{
  m_nIconCount = LastIconID - FirstIconID + 1;
  assert(m_nIconCount > 1 && m_nIconCount <= 20);

  if(pIconList != NULL)
    delete pIconList;

  pIconList = new HICON[m_nIconCount];
  assert(pIconList != NULL);

  HINSTANCE hInstance = ::GetModuleHandle(NULL);
  for(UINT CurIconID = FirstIconID; CurIconID <= LastIconID; CurIconID++)
  {
    HICON hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(CurIconID));
    assert(hIcon != NULL);

    pIconList[CurIconID - FirstIconID] = hIcon;
  }

  return true;
}

CSystemTray *CSystemTray::pThis = NULL;

void CALLBACK CSystemTray::AnimateTimerProc(HWND hWnd, UINT Msg, UINT_PTR IdEvent, DWORD Time)
{
  if(pThis == NULL)
    return;

  static int Steps = 0;
  pThis->SetIcon(pThis->pIconList[Steps]);

  Steps++;
  if(Steps >= pThis->m_nIconCount)
    Steps = 0;
}

bool CSystemTray::BeginAnimate(UINT DelayMilliSeconds)
{
  assert(pIconList != NULL);
  if(pIconList == NULL)
    return false;

  pThis = this;

  m_nTimerId = ::SetTimer(NULL, 0, DelayMilliSeconds, (TIMERPROC)AnimateTimerProc);
  return m_nTimerId != 0;
}

void CSystemTray::StopAnimate()
{
  assert(m_nTimerId != 0);
  if(m_nTimerId == 0)
    return;

  ::KillTimer(NULL, m_nTimerId);
  m_nTimerId = 0;
}

void CSystemTray::OnTrayNotification(WPARAM WParam, LPARAM LParam)
{
  if(WParam != m_Nid.uID)
    return;
  
  switch(LOWORD(LParam))
  {
    case WM_RBUTTONUP:
    {
      ::SetForegroundWindow(m_Nid.hWnd);
      HMENU hMenuMain = ::LoadMenu(::GetModuleHandle(NULL), MAKEINTRESOURCE(m_Nid.uID));
      if(hMenuMain == NULL)
        return;
      HMENU hMenuTray = ::GetSubMenu(hMenuMain, 0);
      if(hMenuTray == NULL)
        return;
      
      POINT MousePos;
      ::GetCursorPos(&MousePos);

      TrackPopupMenu(hMenuTray, 0, //TPM_CENTERALIGN | TPM_RIGHTBUTTON, 
        MousePos.x, MousePos.y, 0, m_Nid.hWnd, NULL); 

      DestroyMenu(hMenuMain);
      break;
    }

    case WM_LBUTTONDBLCLK:
      if(m_nDbClickMenuItem != 0)
        SendMessage(m_Nid.hWnd, WM_COMMAND, m_nDbClickMenuItem, 0);
      break;

    case WM_LBUTTONUP:
      if(m_nLeftClickMenuItem != 0)
        SendMessage(m_Nid.hWnd, WM_COMMAND, m_nLeftClickMenuItem, 0);
      break;
  }
}

}