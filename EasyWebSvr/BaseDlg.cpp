//***********************************************
// 文件名称：basedlg.cpp
// 功　　能：对话框基类
// 作　　者：wbj
// 日　　期：2004-3-29
// 更新日期: 2006-2-23
//***********************************************
#include "stdafx.h"
#include "basedlg.h"

namespace minilib
{
BEGIN_MESSAGE_MAP(BaseDLG, BaseWND)
  ON_DLGCOMMAND(IDOK,      OnOK)
  ON_DLGCOMMAND(IDCANCEL,  OnCancel)
END_MESSAGE_MAP()

BOOL BaseDLG::OnOK(WPARAM WParam, LPARAM LParam)
{
  EndDialog(IDOK);
  return TRUE;
}

BOOL BaseDLG::OnCancel(WPARAM WParam, LPARAM LParam)
{
  EndDialog(IDCANCEL);
  return TRUE;
}

BaseDLG::BaseDLG(UINT DlgIDD)
{
  Mode          = MODE_NORMAL;
  IDD           = DlgIDD;
}

BaseDLG::~BaseDLG()
{
}

BOOL BaseDLG::Create(HWND hWndParent, BOOL bShow)
{
  HWND hDlg = CreateDialogParam(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD), 
    hWndParent, StaticDlgProc, (LPARAM)this);
  if(hDlg == NULL)
    return FALSE;

  assert(hDlg == m_hWnd);
  if(bShow)
    ::ShowWindow(m_hWnd, SW_SHOW);

  Mode = MODE_MODALLESS;
  return TRUE;
}

int BaseDLG::DoModal(HWND hWndParent)
{
  Mode = MODE_MODAL;
  return (int)::DialogBoxParam(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD), 
    hWndParent, StaticDlgProc, (LPARAM)this);
}

BOOL CALLBACK BaseDLG::DialogProc(UINT Message, WPARAM WParam, LPARAM LParam)
{
  const MSGMAP_ENTRY *pEntry = FindMessageEntry(Message, WParam);
  if(pEntry != NULL)
    return (this->*((DlgMsgFUNC)pEntry->MsgFunc))(WParam, LParam);

  return (BOOL)this->DefWindowProc(Message, WParam, LParam);
}

/*
BOOL CALLBACK BaseDLG::DefDialogProc(UINT Message, WPARAM WParam, LPARAM LParam)
{
  return FALSE;
}
*/
LRESULT CALLBACK BaseDLG::DefWindowProc(UINT Message, WPARAM WParam, LPARAM LParam)
{
  return FALSE;
}

// static
BOOL CALLBACK BaseDLG::StaticDlgProc(HWND hWnd, UINT Message, WPARAM WParam, LPARAM LParam)
{
  BaseDLG *pDlg = NULL;

  if(Message == WM_INITDIALOG)
  {
    pDlg = (BaseDLG *)LParam;

    pDlg->Attach(hWnd);
    return pDlg->OnInitDialog();
  }
  else
  {
    pDlg = (BaseDLG *)BaseWND::GetWndFromHandle(hWnd);
  }

  if(pDlg != NULL)
  {
    BOOL bRet = pDlg->DialogProc(Message, WParam, LParam);
    if(Message == WM_NCDESTROY && pDlg->bDeleteThis)
      delete pDlg;
    return bRet;
  }

  return FALSE;
}

BOOL BaseDLG::OnInitDialog()
{
  return TRUE;
}

BOOL BaseDLG::EndDialog(int Result)
{
  if(Mode == MODE_MODAL)
  {
    return ::EndDialog(m_hWnd, Result);
  }
  else
  {
    return ::DestroyWindow(m_hWnd);
  }
}

}