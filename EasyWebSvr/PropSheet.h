// wbj 2006.1.5

#pragma once
#include "basedlg.h"

namespace minilib
{

#define PROPERTYPAGE_TITLESIZE  256

class PropertySHEET;
class PropertyPAGE : public BaseDLG
{
public:
  PropertyPAGE(UINT DlgIDD);

public:
  const TCHAR  *GetTitle();
  void          SetTitle(const TCHAR *pTitle);

protected:
  virtual BOOL  OnOK(WPARAM WParam, LPARAM LParam);
  virtual BOOL  OnCancel(WPARAM WParam, LPARAM LParam);
//  virtual BOOL  OnCommand(WPARAM WParam, LPARAM LParam);

protected:
  TCHAR         Title[PROPERTYPAGE_TITLESIZE];
  PropertySHEET *pSheet;

  friend class PropertySHEET;
};

class PropertySHEET : public BaseDLG
{
public:
  PropertySHEET();
  virtual ~PropertySHEET();

public:
  BOOL          AddPage(PropertyPAGE *pPage);
  BOOL          SetActivePage(int Index);
  BOOL          SetActivePage(const PropertyPAGE *pPage);
  int           GetPageIndex(const PropertyPAGE *pPage);
  int           GetPageCount();

protected:
  virtual BOOL  OnInitDialog();
  //virtual BOOL  OnCommand(WPARAM WParam, LPARAM LParam);
  virtual BOOL  OnOK(WPARAM WParam, LPARAM LParam);
  virtual BOOL  OnCancel(WPARAM WParam, LPARAM LParam);

  virtual BOOL  OnNotify(WPARAM WParam, LPARAM LParam);
  void          ClosePropertySheet(UINT CmdId);

  BOOL          ImpAddPage(PropertyPAGE *pPage);

protected:
  HWND          hWndTabCtrl;
  PropertyPAGE**pPageArray;
  int           PageNum;
  int           CurPageIndex;

  DECLARE_MESSAGE_MAP()
};

inline int PropertySHEET::GetPageCount()
{
  return PageNum;
}
}