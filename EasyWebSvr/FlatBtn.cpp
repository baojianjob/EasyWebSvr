//***********************************************
// 文件名称：flatbtn.cpp
// 功　　能：平面按钮
// 作　　者：wbj
// 日　　期：2006-2-10
// 更新日期: 2007-11-7
//***********************************************
// 2006.2.11 修正了几个Bug
// 2007.11.7 修正了在使用XP风格（manifest文件）时，对于新创建的按钮无效的Bug
#include "stdafx.h"
#include "flatbtn.h"

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

namespace minilib
{
BEGIN_MESSAGE_MAP(FlatBUTTON, BaseBUTTON)
    ON_MESSAGE(WM_CREATE,        OnCreate)
    ON_MESSAGE(WM_MOUSEMOVE,     OnMouseMove)
    ON_MESSAGE(WM_MOUSELEAVE,    OnMouseLeave)
    ON_MESSAGE(WM_LBUTTONUP,     OnLeftButtonUp)
    ON_MESSAGE(BM_SETCHECK,      OnSetCheck)
    ON_MESSAGE(BM_GETCHECK,      OnGetCheck)
    ON_MESSAGE(WM_ENABLE,        OnEnable)
END_MESSAGE_MAP()

FlatBUTTON::FlatBUTTON()
{
    bIsFlat       = TRUE; 
    bDrawBorder   = TRUE;
    bDrawFlatFocus= FALSE;
    bMouseOnButton= FALSE;
    bIsCheckBox   = FALSE;
    CheckState    = BST_UNCHECKED;

    ::ZeroMemory(&BtnIcons, sizeof(BtnIcons));

    SetDefaultColors(FALSE);
}

FlatBUTTON::~FlatBUTTON()
{
    FreeResources();
}

void FlatBUTTON::PreSubclassWindow()
{
    _SetOwnerDrawStyle();

    BaseBUTTON::PreSubclassWindow();
}

LONG FlatBUTTON::OnCreate(WPARAM WParam, LPARAM LParam)
{
    _SetOwnerDrawStyle();
    return 0;
}

void FlatBUTTON::_SetOwnerDrawStyle()
{
    UINT Style = GetButtonStyle();
    if(Style & BS_CHECKBOX) 
    {
        bIsCheckBox = TRUE;
        CheckState = (UINT)::SendMessage(m_hWnd, BM_GETCHECK, 0, 0);
    }

    SetButtonStyle(Style | BS_OWNERDRAW);
}

void FlatBUTTON::SetFlat(BOOL bFlat, BOOL bRepaint)
{
    bIsFlat = bFlat;
    if(bRepaint)
        ::InvalidateRect(m_hWnd, NULL, TRUE);
}

BOOL FlatBUTTON::GetFlat()
{
    return bIsFlat;
}

void FlatBUTTON::SetBorder(BOOL bDrawBorder, BOOL bRepaint)
{
    this->bDrawBorder = bDrawBorder;
    if(bRepaint)
        ::InvalidateRect(m_hWnd, NULL, TRUE);
}

BOOL FlatBUTTON::GetBorder()
{
    return bDrawBorder;
}

void FlatBUTTON::SetFlatFocus(BOOL bDrawFlatFocus, BOOL bRepaint)
{
    this->bDrawFlatFocus = bDrawFlatFocus;
    if(bRepaint)
        ::InvalidateRect(m_hWnd, NULL, TRUE);
}

BOOL FlatBUTTON::GetFlatFocus()
{
    return bDrawFlatFocus;
}

void FlatBUTTON::SetColor(UINT ColorIndex, COLORREF NewColor, BOOL bRepaint)
{
    assert(ColorIndex >= 0 && ColorIndex < 4);
    BtnColor[ColorIndex] = NewColor;
    if(bRepaint)
        ::InvalidateRect(m_hWnd, NULL, TRUE);
}

void FlatBUTTON::SetDefaultColors(BOOL bRepaint)
{
    BtnColor[FLATBTN_COLOR_ACTIVE_FG]     = ::GetSysColor(COLOR_BTNTEXT);
    BtnColor[FLATBTN_COLOR_ACTIVE_BK]     = ::GetSysColor(COLOR_BTNFACE);
    BtnColor[FLATBTN_COLOR_INACTIVE_FG]   = ::GetSysColor(COLOR_BTNTEXT);
    BtnColor[FLATBTN_COLOR_INACTIVE_BK]   = ::GetSysColor(COLOR_BTNFACE);

    if(bRepaint)
        ::InvalidateRect(m_hWnd, NULL, TRUE);
}

COLORREF FlatBUTTON::GetColor(UINT ColorIndex)
{
    assert(ColorIndex >= 0 && ColorIndex < 4);
    return BtnColor[ColorIndex];
}

BOOL FlatBUTTON::SetIcon(int IconInId, int IconOutId, int IconDisableId, HINSTANCE hInstRes)
{
    HICON hIconIn = NULL, hIconOut = NULL, hIconDisable = NULL;

    if(hInstRes == (HINSTANCE)-1)
        hInstRes = ::GetModuleHandle(NULL);

    hIconIn = (HICON)::LoadImage(hInstRes, MAKEINTRESOURCE(IconInId), IMAGE_ICON, 0, 0, 0);
    assert(hIconIn != NULL);

    if(IconOutId != NULL)
    {
        hIconOut = (HICON)::LoadImage(hInstRes, MAKEINTRESOURCE(IconOutId), IMAGE_ICON, 0, 0, 0);
        assert(hIconOut != NULL);
    }

    if(IconDisableId != NULL)
    {
        hIconDisable = (HICON)::LoadImage(hInstRes, MAKEINTRESOURCE(IconDisableId), IMAGE_ICON, 0, 0, 0);
        assert(hIconDisable != NULL);
    }

    return SetIcon(hIconIn, hIconOut, hIconDisable);
}

void FlatBUTTON::FreeResources()
{
    int i;
    for(i = 0; i < (sizeof(BtnIcons) / sizeof(BtnIcons[0])); i++)
    {
        if(BtnIcons[i].hIcon != NULL)
            DestroyIcon(BtnIcons[i].hIcon);
    }

    ::ZeroMemory(&BtnIcons, sizeof(BtnIcons));
}

BOOL FlatBUTTON::GetIconInfo(STRUCT_ICONS &IconInfo, HICON hIcon)
{
    assert(hIcon != NULL);
    IconInfo.hIcon = hIcon;

    ICONINFO Ii;
    ::ZeroMemory(&Ii, sizeof(ICONINFO));
    if(!::GetIconInfo(hIcon, &Ii))
        return FALSE;

    IconInfo.Width = Ii.xHotspot * 2;
    IconInfo.Height = Ii.yHotspot * 2;
    ::DeleteObject(Ii.hbmMask);
    ::DeleteObject(Ii.hbmColor);
    return TRUE;
}

// 外部传入的hIcon是否需要释放？ -------
BOOL FlatBUTTON::SetIcon(HICON hIconIn, HICON hIconOut, HICON hIconDisable)
{
    FreeResources();

    if(hIconIn != NULL)
    {
        if(!GetIconInfo(BtnIcons[0], hIconIn))
        {
            FreeResources();
            return FALSE;
        }

        if(hIconOut != NULL)
        {
            if(!GetIconInfo(BtnIcons[1], hIconOut))
            {
                FreeResources();
                return FALSE;
            }
        }

        if(hIconDisable != NULL)
        {
            if(!GetIconInfo(BtnIcons[2], hIconDisable))
            {
                FreeResources();
                return FALSE;
            }
        }
    }

    ::InvalidateRect(m_hWnd, NULL, TRUE);
    return TRUE;
}

/*
// 填充矩形
static void FillRect(HDC hDc, const RECT *pRect, COLORREF Color)
{
    HBRUSH hBrush = ::CreateSolidBrush(Color);
    ::FillRect(hDc, pRect, hBrush);
    ::DeleteObject(hBrush);
}
*/
void FlatBUTTON::DrawItem(DRAWITEMSTRUCT *pDis)
{
    assert(pDis->CtlType == ODT_BUTTON);

    BOOL bIsPressed  = (pDis->itemState & ODS_SELECTED);
    BOOL bIsFocused  = (pDis->itemState & ODS_FOCUS);
    BOOL bIsDisabled = (pDis->itemState & ODS_DISABLED);
    if(bIsCheckBox)
    {
        if(CheckState != BST_UNCHECKED)
            bIsPressed = TRUE;
    }

    HDC hDc = pDis->hDC;
    RECT ItemRect = pDis->rcItem;

    if(!bIsFlat)
    {
        if(bIsFocused)
        {
            HBRUSH hBrush = ::CreateSolidBrush(RGB(0, 0, 0));
            ::FrameRect(hDc, &ItemRect, hBrush);
            ::DeleteObject(hBrush);
            ::InflateRect(&ItemRect, -1, -1);
        }
    }

    COLORREF BkColor;
    //if(bMouseOnButton || bIsPressed)
    if((bMouseOnButton && !bIsCheckBox) || bIsPressed)
        BkColor = GetColor(FLATBTN_COLOR_ACTIVE_BK);
    else
        BkColor = GetColor(FLATBTN_COLOR_INACTIVE_BK);

    //FillRect(hDc, &(ItemRect), BkColor);
    HBRUSH hBrush = ::CreateSolidBrush(BkColor);
    ::FillRect(hDc, &(ItemRect), hBrush);
    ::DeleteObject(hBrush);

    if(bIsPressed)
    {
        if(bIsFlat)
        {
            if(bDrawBorder)
            {
                // 画凹陷边框
                HPEN bPenBtnHilight = ::CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT));
                HPEN hPenBtnShadow = ::CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));

                // 左上暗边
                HPEN hPenOld = (HPEN)::SelectObject(hDc, hPenBtnShadow);
                ::MoveToEx(hDc, ItemRect.left, ItemRect.bottom - 1, NULL);
                ::LineTo(hDc, ItemRect.left, ItemRect.top);
                ::LineTo(hDc, ItemRect.right, ItemRect.top);

                // 右下亮边
                ::SelectObject(hDc, bPenBtnHilight);
                ::MoveToEx(hDc, ItemRect.left, ItemRect.bottom - 1, NULL);
                ::LineTo(hDc, ItemRect.right - 1, ItemRect.bottom - 1);
                ::LineTo(hDc, ItemRect.right - 1, ItemRect.top - 1);

                ::SelectObject(hDc, hPenOld);

                ::DeleteObject(bPenBtnHilight);
                ::DeleteObject(hPenBtnShadow);
            }
        }
        else
        {
            HBRUSH hBrush = ::CreateSolidBrush(::GetSysColor(COLOR_BTNSHADOW));
            ::FrameRect(hDc, &(ItemRect), hBrush);
            ::DeleteObject(hBrush);
        }
    }
    else // ...else draw non pressed button
    {
        HPEN bPenBtnHilight = ::CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT));
        HPEN bPen3DLight = ::CreatePen(PS_SOLID, 0, GetSysColor(COLOR_3DLIGHT));    // Light gray
        HPEN hPenBtnShadow = ::CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW)); // Dark gray
        HPEN bPen3DDKShadow = ::CreatePen(PS_SOLID, 0, GetSysColor(COLOR_3DDKSHADOW)); // Black

        if(bIsFlat)
        {
            if(bMouseOnButton && bDrawBorder)
            {
                // 画凸起边框
                // 左上亮边
                HPEN hPenOld = (HPEN)::SelectObject(hDc, bPenBtnHilight);
                ::MoveToEx(hDc, ItemRect.left, ItemRect.bottom - 1, NULL);
                ::LineTo(hDc, ItemRect.left, ItemRect.top);
                ::LineTo(hDc, ItemRect.right, ItemRect.top);

                // 右下暗边
                ::SelectObject(hDc, hPenBtnShadow);
                ::MoveToEx(hDc, ItemRect.left, ItemRect.bottom - 1, NULL);
                ::LineTo(hDc, ItemRect.right - 1, ItemRect.bottom - 1);
                ::LineTo(hDc, ItemRect.right - 1, ItemRect.top - 1);

                ::SelectObject(hDc, hPenOld);
            }
        }
        else
        {
            // White line
            HPEN hPenOld = (HPEN)::SelectObject(hDc, bPenBtnHilight);
            ::MoveToEx(hDc, ItemRect.left, ItemRect.bottom - 1, NULL);
            ::LineTo(hDc, ItemRect.left, ItemRect.top);
            ::LineTo(hDc, ItemRect.right, ItemRect.top);

            // Light gray line
            ::SelectObject(hDc, bPen3DLight);
            ::MoveToEx(hDc, ItemRect.left + 1, ItemRect.bottom - 1, NULL);
            ::LineTo(hDc, ItemRect.left + 1, ItemRect.top + 1);
            ::LineTo(hDc, ItemRect.right, ItemRect.top + 1);

            // Black line
            ::SelectObject(hDc, bPen3DDKShadow);
            ::MoveToEx(hDc, ItemRect.left, ItemRect.bottom - 1, NULL);
            ::LineTo(hDc, ItemRect.right - 1, ItemRect.bottom - 1);
            ::LineTo(hDc, ItemRect.right - 1, ItemRect.top - 1);

            // Dark gray line
            ::SelectObject(hDc, hPenBtnShadow);
            ::MoveToEx(hDc, ItemRect.left + 1, ItemRect.bottom - 2, NULL);
            ::LineTo(hDc, ItemRect.right - 2, ItemRect.bottom - 2);
            ::LineTo(hDc, ItemRect.right - 2, ItemRect.top);

            ::SelectObject(hDc, hPenOld);
        }
        ::DeleteObject(bPenBtnHilight);
        ::DeleteObject(bPen3DLight);
        ::DeleteObject(hPenBtnShadow);
        ::DeleteObject(bPen3DDKShadow);
    }

    TCHAR TitleText[256] = _T("");
    ::GetWindowText(m_hWnd, TitleText, 256);

    RECT CaptionRect = pDis->rcItem;

    if(BtnIcons[0].hIcon != NULL)
    {
        DrawTheIcon(hDc, TitleText[0] != 0, pDis->rcItem, CaptionRect, bIsPressed, bIsDisabled);
    }

    if(TitleText[0])
    {
        if(bIsPressed)
            ::OffsetRect(&CaptionRect, 1, 1);

        //if(bMouseOnButton || bIsPressed)
        if((bMouseOnButton && !bIsCheckBox) || bIsPressed)
        {
            ::SetTextColor(hDc, GetColor(FLATBTN_COLOR_ACTIVE_FG));
            ::SetBkColor(hDc, GetColor(FLATBTN_COLOR_ACTIVE_BK));
        } 
        else 
        {
            ::SetTextColor(hDc, GetColor(FLATBTN_COLOR_INACTIVE_FG));
            ::SetBkColor(hDc, GetColor(FLATBTN_COLOR_INACTIVE_BK));
        }

//#define DISABLED_GRAYTEXT // 按钮Disable时，是否使用灰色文本

#ifdef DISABLED_GRAYTEXT
        if(bIsDisabled)
            ::SetTextColor(hDc, ::GetSysColor(COLOR_GRAYTEXT));
#endif

        RECT CenterRect = CaptionRect;
        ::DrawText(hDc, TitleText, -1, &CaptionRect, DT_SINGLELINE | DT_CALCRECT);
        ::OffsetRect(&CaptionRect, 
            (CenterRect.right - CenterRect.left - CaptionRect.right + CaptionRect.left) / 2, 
            (CenterRect.bottom - CenterRect.top - CaptionRect.bottom + CaptionRect.top) / 2);

        ::DrawState(hDc, NULL, NULL, (LPARAM)TitleText, 0, CaptionRect.left, CaptionRect.top,
            CaptionRect.right - CaptionRect.left, CaptionRect.bottom - CaptionRect.top,
#ifdef DISABLED_GRAYTEXT 
            DSS_NORMAL | DST_PREFIXTEXT);
#else
            (bIsDisabled ? DSS_DISABLED : DSS_NORMAL) | DST_PREFIXTEXT);
#endif

        //::DrawText(hDc, TitleText, -1, &CaptionRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    }

    if(!bIsFlat || (bIsFlat && bDrawFlatFocus))
    {
        if(bIsFocused)
        {
            ::InflateRect(&ItemRect, -3, -3);
            ::DrawFocusRect(hDc, &ItemRect);
        }
    }
}

void FlatBUTTON::DrawTheIcon(HDC hDc, BOOL bHasTitle, const RECT &ItemRect, 
    RECT &CaptionRect, BOOL bIsPressed, BOOL bIsDisabled)
{
    assert(BtnIcons[0].hIcon != NULL);

    int IconIndex;
    if(bIsDisabled)
        IconIndex = 2;
    else if((bIsCheckBox && bIsPressed) || (!bIsCheckBox && (bIsPressed || bMouseOnButton)))
        IconIndex = 0;
    else
        IconIndex = 1;
    if(BtnIcons[IconIndex].hIcon == NULL)
        IconIndex = 0;

    RECT IconRect = ItemRect;
    if(!bHasTitle)
    {
        IconRect.left += (IconRect.right - IconRect.left - (long)BtnIcons[IconIndex].Width) / 2; 
    }
    else
    {
        IconRect.left += 3;
        CaptionRect.left += BtnIcons[IconIndex].Width + 3;
    }
    // 加long转换，避免当按钮高度小于图标高度，且Disable时，图标不能显示的Bug
    IconRect.top += (IconRect.bottom - IconRect.top - (long)BtnIcons[IconIndex].Height) / 2;

    if(bIsPressed) 
        ::OffsetRect(&IconRect, 1, 1);

    ::DrawState(hDc, NULL, NULL, (LPARAM)BtnIcons[IconIndex].hIcon, 0, IconRect.left, IconRect.top, 
        IconRect.right - IconRect.left, IconRect.bottom - IconRect.top,
        ((bIsDisabled && IconIndex == 0) ? DSS_DISABLED : DSS_NORMAL) | DST_ICON);
}

LONG FlatBUTTON::OnMouseMove(WPARAM WParam, LPARAM LParam)
{
    DefWindowProc(WM_MOUSEMOVE, WParam, LParam);

    if(!bIsFlat)
        return 0;

    POINT Point;
    Point.x = (short)LOWORD(LParam);
    Point.y = (short)HIWORD(LParam);
    ::ClientToScreen(m_hWnd, &Point);

    if(::WindowFromPoint(Point) == m_hWnd)
    {
        if(!bMouseOnButton)
        {
            TRACKMOUSEEVENT Tme;
            Tme.cbSize    = sizeof(Tme);
            Tme.hwndTrack = m_hWnd;
            Tme.dwFlags   = TME_LEAVE;
            _TrackMouseEvent(&Tme);

            bMouseOnButton = TRUE;
            InvalidateRect(m_hWnd, NULL, TRUE);
        }
    }
    else
        CancelHover();

    return 0;
}

LONG FlatBUTTON::OnMouseLeave(WPARAM WParam, LPARAM LParam)
{
    CancelHover();
    return 0;
}

void FlatBUTTON::CancelHover()
{
    if(bIsFlat)
    {
        if(bMouseOnButton)
        {
            bMouseOnButton = FALSE;
            InvalidateRect(m_hWnd, NULL, TRUE);
        }
    }
}

LONG FlatBUTTON::OnLeftButtonUp(WPARAM WParam, LPARAM LParam)
{
    BOOL bCaptureThis = (::GetCapture() == m_hWnd);

    if(bIsCheckBox && bCaptureThis)
    {
        if(bMouseOnButton)
        {
            CheckState = (CheckState == BST_UNCHECKED ? BST_CHECKED : BST_UNCHECKED);
            InvalidateRect(m_hWnd, NULL, TRUE);
        }
    }

    DefWindowProc(WM_LBUTTONUP, WParam, LParam);
    return 0;
}

LONG FlatBUTTON::OnGetCheck(WPARAM WParam, LPARAM LParam)
{
    assert(bIsCheckBox);
    return CheckState;
}

LONG FlatBUTTON::OnSetCheck(WPARAM WParam, LPARAM LParam)
{
    assert(bIsCheckBox);
    CheckState = (UINT)WParam;
    InvalidateRect(m_hWnd, NULL, TRUE);
    return 0;
}

LONG FlatBUTTON::OnEnable(WPARAM WParam, LPARAM LParam)
{
    DefWindowProc(WM_ENABLE, WParam, LParam);

    if(!((BOOL)WParam))
    {
        CancelHover();
    }

    return 0;
}
/*
BOOL FlatBUTTON::SetWindowRgn(HRGN hRgn, BOOL bRepaint)
{
    return ::SetWindowRgn(m_hWnd, hRgn, bRepaint);
}*/
}