//***********************************************
// 文件名称：basectrl.cpp
// 功　　能：标准控件封装
// 作　　者：wbj
// 日　　期：2003-4-19
// 更新日期: 2006-8-29
//***********************************************
#include "stdafx.h"
#include "basectrl.h"

#pragma warning(disable:4312)

namespace minilib
{
BOOL BaseBUTTON::CreateEx(HWND hWndParent, DWORD ExStyle, DWORD Style, LPCTSTR WindowName, 
                          int Left, int Top, int Width, int Height, UINT ID)
{
  return BaseWND::CreateEx(ExStyle, _T("BUTTON"), WindowName, Style, Left, Top, Width, Height, hWndParent, (HMENU)ID);
}

BOOL BaseBUTTON::OnChildNotify(UINT Message, WPARAM WParam, LPARAM LParam, LRESULT *pResult)
{
  if(Message != WM_DRAWITEM)
    return BaseWND::OnChildNotify(Message, WParam, LParam, pResult);

  assert(pResult == NULL);       // no return value expected
  DrawItem((DRAWITEMSTRUCT *)LParam);
  return TRUE;
}

void BaseBUTTON::DrawItem(DRAWITEMSTRUCT *pDis)
{
}

///////////////////////////////////////////////////////////////////////////////

BOOL BaseEDIT::CreateEx(HWND hWndParent, DWORD ExStyle, DWORD Style, LPCTSTR WindowName, 
                          int Left, int Top, int Width, int Height, UINT ID)
{
  return BaseWND::CreateEx(ExStyle, _T("EDIT"), WindowName, Style, Left, Top, Width, Height, hWndParent, (HMENU)ID);
}

///////////////////////////////////////////////////////////////////////////////

SIZE StatusBAR::GetSize()
{
  RECT Rect;
  GetClientRect(m_hWnd, &Rect);

  SIZE Size;
  Size.cx = Rect.right;
  Size.cy = Rect.bottom;
  return Size;
}
}