// wbj 2006.1.5

#include "stdafx.h"
#include "propsheet.h"
#include "resource.h"

#define IDD_PROPERTYSHEET       IDD_CONFIG
#define IDC_TAB_PROPERTYSHEET   IDC_TAB_CONFIG

namespace minilib
{

PropertyPAGE::PropertyPAGE(UINT DlgIDD) : BaseDLG(DlgIDD)
{
  Title[0]      = 0;
  pSheet        = NULL;
}

const TCHAR *PropertyPAGE::GetTitle()
{
  if(Title[0] == 0 && m_hWnd != NULL)
    ::GetWindowText(m_hWnd, Title, PROPERTYPAGE_TITLESIZE);
  return (LPCTSTR)Title;
}

void PropertyPAGE::SetTitle(const TCHAR *pTitle)
{
  lstrcpyn(Title, pTitle, PROPERTYPAGE_TITLESIZE);
}

BOOL PropertyPAGE::OnOK(WPARAM WParam, LPARAM LParam)
{
  return TRUE;
}

BOOL PropertyPAGE::OnCancel(WPARAM WParam, LPARAM LParam)
{
  return TRUE;
}
/*
BOOL PropertyPAGE::OnCommand(WPARAM WParam, LPARAM LParam)
{
  return TRUE;
}*/


///////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(PropertySHEET, BaseDLG)
  ON_DLGMESSAGE(WM_NOTIFY, OnNotify)
END_MESSAGE_MAP()

PropertySHEET::PropertySHEET() : BaseDLG(IDD_PROPERTYSHEET)
{
  hWndTabCtrl   = NULL;
  pPageArray    = NULL;
  PageNum       = 0;
  CurPageIndex  = 0;
}

PropertySHEET::~PropertySHEET()
{
  free(pPageArray);
}

BOOL PropertySHEET::AddPage(PropertyPAGE *pPage)
{
  assert(pPage != NULL);
  if(pPage == NULL)
    return FALSE;

  pPageArray = (PropertyPAGE **)realloc(pPageArray, (PageNum + 1) * sizeof(PropertyPAGE *));
  pPageArray[PageNum] = pPage;
  PageNum++;

  pPage->pSheet = this;
  if(this->m_hWnd != NULL)
    return ImpAddPage(pPage);

  return TRUE;
}

BOOL PropertySHEET::OnInitDialog()
{
  hWndTabCtrl = ::GetDlgItem(m_hWnd, IDC_TAB_PROPERTYSHEET);

  int i;
  for(i = 0; i < PageNum; i++)
    ImpAddPage(pPageArray[i]);

  if(PageNum > 0)
  {
    assert(CurPageIndex >= 0 && CurPageIndex < PageNum);
    ::ShowWindow(pPageArray[CurPageIndex]->m_hWnd, SW_SHOW);
    TabCtrl_SetCurSel(hWndTabCtrl, CurPageIndex);
  }
  return TRUE;
}

BOOL PropertySHEET::ImpAddPage(PropertyPAGE *pPage)
{
  assert(hWndTabCtrl != NULL);

  if(pPage->m_hWnd == NULL && !pPage->Create(m_hWnd, FALSE))
    return FALSE;

  int Count = TabCtrl_GetItemCount(hWndTabCtrl);

  TCITEM Tie;
  Tie.mask = TCIF_TEXT;
  Tie.pszText = (LPSTR)(pPage->GetTitle());
  TabCtrl_InsertItem(hWndTabCtrl, Count, &Tie);

  RECT HeadRect;
  TabCtrl_GetItemRect(hWndTabCtrl, 0, &HeadRect);
  RECT Rect;
  GetWindowRect(hWndTabCtrl, &Rect);
  ::ScreenToClient(m_hWnd, &(((POINT *)(&Rect))[0]));
  ::ScreenToClient(m_hWnd, &(((POINT *)(&Rect))[1]));
  MoveWindow(pPage->m_hWnd, Rect.left + 2, Rect.top + HeadRect.bottom + 2, 
    Rect.right - Rect.left - 4, Rect.bottom - Rect.top - HeadRect.bottom - 4, 0);
  SetWindowPos(pPage->m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // 让Spy++可以选中该对话框子窗口
  return TRUE;
}

BOOL PropertySHEET::SetActivePage(int Index)
{
  if(Index < 0 || Index >= PageNum)
    return FALSE;

  if(this->m_hWnd != NULL)
  {
    assert(::IsWindow(pPageArray[Index]->m_hWnd));
    ::ShowWindow(pPageArray[CurPageIndex]->m_hWnd, SW_HIDE);
    ::ShowWindow(pPageArray[Index]->m_hWnd, SW_SHOW);

    TabCtrl_SetCurSel(hWndTabCtrl, Index); // 不会激发TCN_SELCHANGE
  }
  CurPageIndex = Index;
  return TRUE;
}

BOOL PropertySHEET::SetActivePage(const PropertyPAGE *pPage)
{
  return SetActivePage(GetPageIndex(pPage));
}

int PropertySHEET::GetPageIndex(const PropertyPAGE *pPage)
{
  for(int i = 0; i < PageNum; i++)
  {
    if(pPageArray[i] == pPage)
      return i;
  }
  assert(false);
  return -1;
}

BOOL PropertySHEET::OnNotify(WPARAM WParam, LPARAM LParam)
{
  NMHDR *pNmhdr = (NMHDR *)LParam;
  if(pNmhdr->idFrom != IDC_TAB_PROPERTYSHEET)
    return FALSE;

  assert(hWndTabCtrl != NULL);

  if(pNmhdr->code == TCN_SELCHANGE)
  {
    int Index = TabCtrl_GetCurSel(hWndTabCtrl);
    assert(Index >= 0 && Index < PageNum);
    ::ShowWindow(pPageArray[CurPageIndex]->m_hWnd, SW_HIDE);
    CurPageIndex = Index;
    ::ShowWindow(pPageArray[CurPageIndex]->m_hWnd, SW_SHOW);
    return TRUE;
  }
  return FALSE;
}
/*
BOOL PropertySHEET::OnCommand(WPARAM WParam, LPARAM LParam)
{
  UINT CmdID = LOWORD(WParam);
  switch(CmdID)
  {
    case IDOK:
    case IDCANCEL:
    {
      int i;
      for(i = 0; i < PageNum; i++)
      {
        int Ret = (int)::SendMessage(pPageArray[i]->m_hWnd, WM_COMMAND, CmdID, 0);
        if(Ret)
          return TRUE;
      }
      for(i = 0; i < PageNum; i++)
        pPageArray[i]->EndDialog(CmdID);

      EndDialog(CmdID);
      return TRUE;
    }
  }
  return FALSE;
}
*/
void PropertySHEET::ClosePropertySheet(UINT CmdId)
{
  int i;
  for(i = 0; i < PageNum; i++)
  {
    int Ret = (int)::SendMessage(pPageArray[i]->m_hWnd, WM_COMMAND, CmdId, 0);
    if(Ret)
      return;
  }
  for(i = 0; i < PageNum; i++)
    pPageArray[i]->EndDialog(CmdId);

  EndDialog(CmdId);
}

BOOL PropertySHEET::OnOK(WPARAM WParam, LPARAM LParam)
{
  ClosePropertySheet(IDOK);
  return TRUE;
}

BOOL PropertySHEET::OnCancel(WPARAM WParam, LPARAM LParam)
{
  ClosePropertySheet(IDCANCEL);
  return TRUE;
}


}