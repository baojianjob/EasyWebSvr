//***********************************************
// 文件名称：flatbtn.h
// 功　　能：平面按钮
// 作　　者：wbj
// 日　　期：2006-2-10
// 更新日期: 2007-11-7
//***********************************************
#ifndef __FLATBTN_H__
#define __FLATBTN_H__
#pragma once

#include "basectrl.h"

namespace minilib
{
class FlatBUTTON : public BaseBUTTON
{
public:
    FlatBUTTON();
    ~FlatBUTTON();

    enum
    {
        FLATBTN_COLOR_ACTIVE_FG   = 0,
        FLATBTN_COLOR_ACTIVE_BK   = 1,
        FLATBTN_COLOR_INACTIVE_FG = 2,
        FLATBTN_COLOR_INACTIVE_BK = 3,
    };

public:
    void          SetColor(UINT ColorIndex, COLORREF NewColor, BOOL bRepaint = TRUE);
    void          SetDefaultColors(BOOL bRepaint = TRUE);
    COLORREF      GetColor(UINT ColorIndex);

    void          SetFlat(BOOL bFlat = TRUE, BOOL bRepaint = TRUE);
    BOOL          GetFlat();

    BOOL          SetIcon(int IconInId, int IconOutId = 0, int IconDisableId = 0, HINSTANCE hInstRes = (HINSTANCE)-1);
    BOOL          SetIcon(HICON hIconIn, HICON hIconOut, HICON hIconDisable);

    void          SetBorder(BOOL bDrawBorder = TRUE, BOOL bRepaint = TRUE);
    BOOL          GetBorder();

    void          SetFlatFocus(BOOL bDrawFlatFocus, BOOL bRepaint = TRUE);
    BOOL          GetFlatFocus();

    //BOOL          SetWindowRgn(HRGN hRgn, BOOL bRepaint = TRUE);

protected:
    struct STRUCT_ICONS
    {
        HICON       hIcon; 
        DWORD       Width;
        DWORD       Height;
    };

    LONG          OnCreate(WPARAM WParam, LPARAM LParam);
    LONG          OnMouseMove(WPARAM WParam, LPARAM LParam);
    LONG          OnMouseLeave(WPARAM WParam, LPARAM LParam);
    LONG          OnLeftButtonUp(WPARAM WParam, LPARAM LParam);
    LONG          OnGetCheck(WPARAM WParam, LPARAM LParam);
    LONG          OnSetCheck(WPARAM WParam, LPARAM LParam);
    LONG          OnEnable(WPARAM WParam, LPARAM LParam);

    virtual void  PreSubclassWindow();
    virtual void  DrawItem(DRAWITEMSTRUCT *pDis);

    void          DrawTheIcon(HDC hDc, BOOL bHasTitle, const RECT &ItemRect, RECT &CaptionRect, BOOL bIsPressed, BOOL bIsDisabled);
    void          CancelHover();
    void          FreeResources();
    BOOL          GetIconInfo(STRUCT_ICONS &IconInfo, HICON hIcon);

private:
    void          _SetOwnerDrawStyle();

private:
    STRUCT_ICONS  BtnIcons[3];    // 按钮图标数组
    COLORREF      BtnColor[4];    // 按钮颜色数组
    BOOL          bIsFlat;        // 是否为平面按钮
    BOOL          bIsCheckBox;    // 是否为CheckBox
    UINT          CheckState;     // CheckBox的选中状态（仅在bIsCheckBox为TRUE时有效）
    BOOL          bDrawBorder;    // 平面时是否显示悬浮框（仅在bIsFlat为TRUE时有效）
    BOOL          bDrawFlatFocus; // 平面时是否显示焦点框（仅在bIsFlat为TRUE时有效）
    BOOL          bMouseOnButton; // 鼠标是否在按钮内

    DECLARE_MESSAGE_MAP()
};
}
#endif // __FLATBTN_H__