//***********************************************
// 文件名称：basedlg.h
// 功　　能：对话框基类
// 作　　者：wbj
// 日　　期：2004-3-29
// 更新日期: 2006-2-23
//***********************************************
#ifndef __BASEDLG_H__
#define __BASEDLG_H__
#pragma once

#include "basewnd.h"

namespace minilib
{
class BaseDLG : public BaseWND
{
public:
  BaseDLG(UINT DlgIDD);
  virtual ~BaseDLG();

public:
  BOOL          Create(HWND hWndParent, BOOL bShow = FALSE);
  int           DoModal(HWND hWndParent);
  BOOL          EndDialog(int Result);

protected:
  enum MODE { MODE_NORMAL = 0, MODE_MODAL = 1, MODE_MODALLESS = 2};

  UINT          IDD;
  MODE          Mode;

protected:
  virtual BOOL  OnInitDialog();
  virtual BOOL  OnOK(WPARAM WParam, LPARAM LParam);
  virtual BOOL  OnCancel(WPARAM WParam, LPARAM LParam);

  //virtual BOOL CALLBACK DefDialogProc(UINT Message, WPARAM WParam, LPARAM LParam);
  virtual BOOL CALLBACK DialogProc(UINT Message, WPARAM WParam, LPARAM LParam);
  virtual LRESULT CALLBACK DefWindowProc(UINT Message, WPARAM WParam, LPARAM LParam);

  static BOOL CALLBACK StaticDlgProc(HWND hWnd, UINT Message, WPARAM WParam, LPARAM LParam);

  DECLARE_MESSAGE_MAP()
};
}
#endif // __BASEDLG_H__