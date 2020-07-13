//***********************************************
// 文件名称：basectrl.h
// 功　　能：标准控件封装
// 作　　者：wbj
// 日　　期：2003-4-19
// 更新日期: 2006-8-29
//***********************************************
#ifndef __BASECTRL_H__
#define __BASECTRL_H__
#pragma once

#include "basewnd.h"
#pragma warning(disable:4312)

namespace minilib
{
class BaseBUTTON : public BaseWND
{
public:
  BOOL          CreateEx(HWND hWndParent, DWORD ExStyle, DWORD Style, LPCTSTR WindowName, 
                         int Left, int Top, int Width, int Height, UINT ID);

  UINT          GetButtonStyle() const;
  void          SetButtonStyle(UINT Style, BOOL bRedraw = TRUE);

protected:
  virtual BOOL  OnChildNotify(UINT Message, WPARAM WParam, LPARAM LParam, LRESULT *pResult);
  virtual void  DrawItem(DRAWITEMSTRUCT *pDis);
};

inline UINT BaseBUTTON::GetButtonStyle() const
{
  assert(::IsWindow(m_hWnd)); 
  return (UINT)GetWindowLong(m_hWnd, GWL_STYLE) & 0xff;
}

inline void BaseBUTTON::SetButtonStyle(UINT Style, BOOL bRedraw)
{
  assert(::IsWindow(m_hWnd)); 
  ::SendMessage(m_hWnd, BM_SETSTYLE, Style, (LPARAM)bRedraw);
}

///////////////////////////////////////////////////////////////////////////////

class BaseEDIT : public BaseWND
{
public:
  BOOL          CreateEx(HWND hWndParent, DWORD ExStyle, DWORD Style, LPCTSTR WindowName, 
                         int Left, int Top, int Width, int Height, UINT ID);

//protected:
//  virtual BOOL  OnChildNotify(UINT Message, WPARAM WParam, LPARAM LParam, LRESULT *pResult);
//  virtual void  DrawItem(DRAWITEMSTRUCT *pDis);
};

///////////////////////////////////////////////////////////////////////////////

class StatusBAR : public BaseWND
{
public:
  StatusBAR() {};     
  ~StatusBAR() {};

  BOOL          CreateEx(HWND hWndParent, DWORD ExStyle = 0, DWORD Style = WS_CHILD | WS_VISIBLE | CCS_ADJUSTABLE | SBARS_SIZEGRIP,
                         UINT ID = 0);

  BOOL          SetParts(const UINT *pLengthArray, int LengthCount);
  void          OnSize(WPARAM WParam, LPARAM LParam);
  BOOL          SetPaneText(UINT Index, const TCHAR *pString);
  SIZE          GetSize();
};

inline BOOL StatusBAR::CreateEx(HWND hWndParent, DWORD ExStyle, DWORD Style, UINT ID)
{
  return BaseWND::CreateEx(ExStyle, STATUSCLASSNAME, _T(""), Style, 0, 0, 0, 0, hWndParent, (HMENU)ID);
}

inline BOOL StatusBAR::SetParts(const UINT *pLengthArray, int LengthCount)
{
  return (BOOL)SendMessage(m_hWnd, SB_SETPARTS, (WPARAM)LengthCount, (LPARAM)pLengthArray);
}

inline void StatusBAR::OnSize(WPARAM WParam, LPARAM LParam)
{
  SendMessage(m_hWnd, WM_SIZE, WParam, LParam);
}

inline BOOL StatusBAR::SetPaneText(UINT Index, const TCHAR *pString)
{
  return (BOOL)SendMessage(m_hWnd, SB_SETTEXT, Index, (LPARAM)pString);
}
}
#pragma warning(default:4312)

#endif // __BASECTRL_H__