//***********************************************
// 文件名称：BaseWnd.cpp
// 功　　能：窗口基类
// 作　　者：wbj
// 日　　期：2003-4-19
// 更新日期: 2006-2-10
//***********************************************
#include "stdafx.h"
#include "basewnd.h"

#pragma warning(disable:4311)
#pragma warning(disable:4312)

namespace minilib
{
const MSGMAP* CmdTARGET::GetMessageMap() const
{ 
  return GetThisMessageMap(); 
}

const MSGMAP *__stdcall CmdTARGET::GetThisMessageMap()
{
  static const MSGMAP_ENTRY _MessageEntries[] = 
  {
    { 0, 0, NULL }
  };
  static const MSGMAP MessageMap =
  { 
    NULL, 
    &_MessageEntries[0] 
  };
  return &MessageMap;
}

///////////////////////////////////////////////////////////////////////////////

static __declspec(thread) HHOOK hOldHook;
static __declspec(thread) BaseWND *pWndInit;

BEGIN_MESSAGE_MAP(BaseWND, CmdTARGET)
  ON_MESSAGE(WM_DRAWITEM, OnDrawItem)
END_MESSAGE_MAP()

// static
LRESULT CALLBACK BaseWND::StaticWndProc(HWND hWnd, UINT Message, WPARAM WParam, LPARAM LParam)
{
  BaseWND *pThis = BaseWND::GetWndFromHandle(hWnd);
  assert(pThis != NULL);
  assert(pThis->m_hWnd == hWnd);

  LRESULT Result = pThis->WindowProc(Message, WParam, LParam);
  if(Message == WM_NCDESTROY && pThis->bDeleteThis)
    delete pThis;
  return Result;
}

BaseWND::BaseWND()
{
  m_hWnd        = NULL;
  OrgWndProc   = NULL;
  bDeleteThis   = FALSE;
}

BaseWND::~BaseWND()
{
}

static LRESULT CALLBACK CbtFilterHook(int Code, WPARAM WParam, LPARAM LParam)
{
  if(Code != HCBT_CREATEWND)
    return CallNextHookEx(hOldHook, Code, WParam, LParam);

  if(pWndInit != NULL)
  {
    pWndInit->SubclassWindow((HWND)WParam);
    pWndInit = NULL;
  }

  return CallNextHookEx(hOldHook, Code, WParam, LParam);
}

BOOL BaseWND::CreateEx(DWORD ExStyle, LPCTSTR ClassName, LPCTSTR WindowName, DWORD Style,
  int Left, int Top, int Width, int Height, HWND hWndParent, HMENU IDorHMenu)
{
  HINSTANCE hInst = (HINSTANCE)::GetModuleHandle(NULL);

  WNDCLASS Wc;
  if(!GetClassInfo(hInst, ClassName, &Wc))
    RegisterWndClass(hInst, ClassName);

  if(hOldHook == NULL)
  {
    hOldHook = SetWindowsHookEx(WH_CBT, CbtFilterHook, NULL, ::GetCurrentThreadId());
  }
  assert(hOldHook != NULL);
  assert(m_hWnd == NULL);
  pWndInit = this;

  HWND hWnd = ::CreateWindowEx(ExStyle, ClassName, WindowName, Style, Left, Top,
    Width, Height, hWndParent, IDorHMenu, hInst, NULL);

  assert(pWndInit == NULL);
  ::UnhookWindowsHookEx(hOldHook);
  hOldHook = NULL;

  if(hWnd == NULL)
    return FALSE;
  return TRUE;
}

BOOL BaseWND::Attach(HWND hWndNew)
{
  assert(m_hWnd == NULL);
  assert(GetWndFromHandle(hWndNew) == NULL);

  if(hWndNew == NULL)
    return FALSE;

  m_hWnd = hWndNew;
  ::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
  return TRUE;
}

HWND BaseWND::Detach()
{
  HWND hWndRet = m_hWnd;
  if(m_hWnd != NULL)
  {
    ::SetWindowLong(m_hWnd, GWL_USERDATA, 0);
    m_hWnd = NULL; 
  }
  return hWndRet;
}

BOOL BaseWND::SubclassWindow(HWND hWnd)
{
  if(!Attach(hWnd))
    return FALSE;

  PreSubclassWindow();

  assert(OrgWndProc == NULL);

  OrgWndProc = (WNDPROC)::SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)StaticWndProc);
  assert(OrgWndProc != StaticWndProc);
  return TRUE;
}

BOOL BaseWND::SubclassDlgItem(HWND hDlg, UINT DlgItemID)
{
  return SubclassWindow(::GetDlgItem(hDlg, DlgItemID));
}

HWND BaseWND::UnsubclassWindow()
{
  assert(::IsWindow(m_hWnd));

  SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)OrgWndProc);
  OrgWndProc = NULL;

  return Detach();
}

const MSGMAP_ENTRY *BaseWND::FindMessageEntry(UINT Message, WPARAM WParam)
{
  UINT FuncType = (Message == WM_COMMAND) ? WFT_COMMAND : WFT_MESSAGE;
  UINT MsgId = ((FuncType == WFT_COMMAND) ? LOWORD(WParam) : Message);

  const MSGMAP *pMsgMap = GetMessageMap();
  for(; pMsgMap != NULL; pMsgMap = pMsgMap->pfnGetBaseMap ? pMsgMap->pfnGetBaseMap() : NULL)
  {
    int i;
    for(i = 0; pMsgMap->pEntries[i].Message != 0; i++)
    {
      if(FuncType == pMsgMap->pEntries[i].FuncType && pMsgMap->pEntries[i].Message == MsgId)
        return &(pMsgMap->pEntries[i]);
    }
  }
  return NULL;
}

BOOL BaseWND::OnWndMsg(UINT Message, WPARAM WParam, LPARAM LParam, LRESULT *pResult)
{
  const MSGMAP_ENTRY *pEntry = FindMessageEntry(Message, WParam);
  if(pEntry != NULL)
  {
    LRESULT Result = (this->*((WndMsgFUNC)pEntry->MsgFunc))(WParam, LParam);
    if(pResult != NULL)
      *pResult = Result;
    return TRUE;
  }
  return FALSE;
}

LRESULT CALLBACK BaseWND::WindowProc(UINT Message, WPARAM WParam, LPARAM LParam)
{
  LRESULT Result = 0;
  if(!OnWndMsg(Message, WParam, LParam, &Result))
    Result = this->DefWindowProc(Message, WParam, LParam);
  return Result;
}

void BaseWND::PreSubclassWindow()
{
}

void BaseWND::PreRegisterClass(WNDCLASSEX *pWcex) 
{
}

ATOM BaseWND::RegisterWndClass(HINSTANCE hInst, LPCTSTR ClassName)
{
  WNDCLASSEX Wcex;

  Wcex.cbSize           = sizeof(WNDCLASSEX);
  Wcex.style            = 0;
  Wcex.lpfnWndProc      = ::DefWindowProc;//(WNDPROC)StaticWndProc;
  Wcex.cbClsExtra       = 0;
  Wcex.cbWndExtra       = 0;
  Wcex.hInstance        = hInst;
  Wcex.hIcon            = NULL;
  Wcex.hCursor          = LoadCursor(NULL, IDC_ARROW);
  Wcex.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);
  Wcex.lpszMenuName     = (LPCTSTR)NULL;
  Wcex.lpszClassName    = ClassName;
  Wcex.hIconSm          = NULL;

  PreRegisterClass(&Wcex);
  
  return ::RegisterClassEx(&Wcex);
}

LONG BaseWND::OnDrawItem(WPARAM WParam, LPARAM LParam)
{
  DRAWITEMSTRUCT *pDrawItem = (DRAWITEMSTRUCT *)LParam;

  if(pDrawItem->CtlType == ODT_MENU)
  {
    // Menu 暂未实现
    assert(false);
    return FALSE;
  }

  BaseWND *pWndChild = BaseWND::GetWndFromHandle(pDrawItem->hwndItem);
  assert(pWndChild == NULL || pWndChild->m_hWnd == pDrawItem->hwndItem);
  //if(pWndChild == NULL)
  //  return 0;
  if(pWndChild != NULL && pWndChild->OnChildNotify(WM_DRAWITEM, WParam, LParam, NULL))
    return TRUE;

  return (LONG)this->DefWindowProc(WM_DRAWITEM, WParam, LParam);
}

BOOL BaseWND::OnChildNotify(UINT Message, WPARAM WParam, LPARAM LParam, LRESULT *pResult)
{
  // 待实现
  return FALSE;
}

///////////////////////////////////////////////////////////////////////////////

void MakePoint(POINT *pPt, LPARAM LParam)
{
  int PosX = LOWORD(LParam);
  int PosY = HIWORD(LParam);

  if(PosX & (1 << 15))
    PosX = (PosX | (0xffff << 16));
  if(PosY & (1 << 15))
    PosY = (PosY | (0xffff << 16));

  (*pPt).x = PosX;
  (*pPt).y = PosY;
}

// 把当前客户区坐标转换成屏幕坐标
void ClientToScreen(HWND hWnd, RECT *pRect)
{
  ::ClientToScreen(hWnd, (LPPOINT)pRect);
  ::ClientToScreen(hWnd, ((LPPOINT)pRect) + 1);
}

// 把屏幕坐标转换成当前客户区坐标
void ScreenToClient(HWND hWnd, RECT *pRect)
{
  ::ScreenToClient(hWnd, (LPPOINT)pRect);
  ::ScreenToClient(hWnd, ((LPPOINT)pRect) + 1);
}

// 把当前客户区坐标转换成父窗口客户区坐标
void ClientToParent(HWND hWnd, POINT *pPos)
{
  ::ClientToScreen(hWnd, pPos);
  ::ScreenToClient(::GetParent(hWnd), pPos);
}

// 把当前客户区坐标转换成父窗口客户区坐标
void ClientToParent(HWND hWnd, RECT *pRect)
{
  ClientToParent(hWnd, (LPPOINT)pRect);
  ClientToParent(hWnd, ((LPPOINT)pRect) + 1);
}

// 填充矩形
void FillRect(HDC hDc, const RECT *pRect, COLORREF Color)
{
  HBRUSH hBrush = ::CreateSolidBrush(Color);
  ::FillRect(hDc, pRect, hBrush);
  ::DeleteObject(hBrush);
}

// 用 XOR 方式填充矩形
void FillFocusRect(HDC hDc, CONST RECT *pRect)
{
  RECT Rect;
  memcpy(&Rect, pRect, sizeof(RECT));

  if((Rect.right - Rect.left) % 2 != 0)
    Rect.right++;
  if((Rect.bottom - Rect.top) % 2 != 0)
    Rect.bottom++;
  
  while(Rect.right - Rect.left > 0 && Rect.bottom - Rect.top > 0)
  {
    ::DrawFocusRect(hDc, &Rect);
    ::InflateRect(&Rect, -1, -1);
  }
}

// 居中窗口
BOOL CenterWindow(HWND hWnd, HWND hWndCenter)
{
    assert(::IsWindow(hWnd));

    // determine owner window to center against
    DWORD dwStyle = ::GetWindowLong(hWnd, GWL_STYLE);
    if(hWndCenter == NULL)
    {
        if(dwStyle & WS_CHILD)
            hWndCenter = ::GetParent(hWnd);
        else
            hWndCenter = ::GetWindow(hWnd, GW_OWNER);
    }

    // get coordinates of the window relative to its parent
    RECT rcDlg;
    ::GetWindowRect(hWnd, &rcDlg);
    RECT rcArea;
    RECT rcCenter;
    HWND hWndParent;
    if(!(dwStyle & WS_CHILD))
    {
        // don't center against invisible or minimized windows
        if(hWndCenter != NULL)
        {
            DWORD dwStyleCenter = ::GetWindowLong(hWndCenter, GWL_STYLE);
            if(!(dwStyleCenter & WS_VISIBLE) || (dwStyleCenter & WS_MINIMIZE))
                hWndCenter = NULL;
        }

        // center within screen coordinates
        ::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
        if(hWndCenter == NULL)
            rcCenter = rcArea;
        else
            ::GetWindowRect(hWndCenter, &rcCenter);
    }
    else
    {
        // center within parent client coordinates
        hWndParent = ::GetParent(hWnd);
        assert(::IsWindow(hWndParent));

        ::GetClientRect(hWndParent, &rcArea);
        assert(::IsWindow(hWndCenter));
        ::GetClientRect(hWndCenter, &rcCenter);
        ::MapWindowPoints(hWndCenter, hWndParent, (POINT*)&rcCenter, 2);
    }

    int DlgWidth = rcDlg.right - rcDlg.left;
    int DlgHeight = rcDlg.bottom - rcDlg.top;

    // find dialog's upper left based on rcCenter
    int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
    int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

    // if the dialog is outside the screen, move it inside
    if(xLeft < rcArea.left)
        xLeft = rcArea.left;
    else if(xLeft + DlgWidth > rcArea.right)
        xLeft = rcArea.right - DlgWidth;

    if(yTop < rcArea.top)
        yTop = rcArea.top;
    else if(yTop + DlgHeight > rcArea.bottom)
        yTop = rcArea.bottom - DlgHeight;

    // map screen coordinates to child coordinates
    return ::SetWindowPos(hWnd, NULL, xLeft, yTop, -1, -1,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}
}