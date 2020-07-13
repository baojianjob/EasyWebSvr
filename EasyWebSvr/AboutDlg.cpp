#include "stdafx.h"
#include "resource.h"
#include "mtstring.h"
#include "aboutdlg.h"

#define ID_TIMER_SCROLL         100
#define WM_STARTSCROLL          WM_USER + 100
#define WM_STOPSCROLL           WM_USER + 101

#pragma warning(disable:4311)
#pragma warning(disable:4312)

using namespace minilib;

// 单向颜色渐变函数
COLORREF GetGradualColor(COLORREF Color1, COLORREF Color2, int TotalLen, int CurPos)
{
    assert(CurPos >= 0 && CurPos <= TotalLen);
    int R =  GetRValue(Color1) + (GetRValue(Color2) - GetRValue(Color1)) * CurPos / TotalLen;
    int G =  GetGValue(Color1) + (GetGValue(Color2) - GetGValue(Color1)) * CurPos / TotalLen;
    int B =  GetBValue(Color1) + (GetBValue(Color2) - GetBValue(Color1)) * CurPos / TotalLen;
    return RGB(R, G, B);
}

// 把一字符串复制到剪贴板
static bool CopyToClipboard(const char *pText)
{
    assert(pText != NULL);

    HANDLE hClipboard = GlobalAlloc(GHND | GMEM_SHARE, lstrlen(pText) + 1);
    if(!hClipboard)
        return false;
    char *pBuffer = (char *)GlobalLock(hClipboard);
    lstrcpy(pBuffer, pText);
    GlobalUnlock(hClipboard);

    if(!OpenClipboard(NULL)) // maybe this paramer is hWnd
        return false;
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hClipboard);
    CloseClipboard();
    return true;
}

LRESULT APIENTRY StaticSubclassProc(HWND hWnd, UINT Message, WPARAM WParam, LPARAM LParam)
{
    static HDC hMemDc, hMemDc2;
    static HBITMAP hMemBmp, hMemBmp2;
    static LOGFONT LogFont;
    static int PosY, Height;
    static RECT ClientRect;
    static char *pText[] =
    {
        "《EasyWebServer网站服务器》V1.92",
        "",
        "这是一款小型的Web服务器软件。它可以很快速地在您",
        "的PC上创建一个站点，而无需IIS等庞大复杂的工具。",
        "它的主要功能特点如下：",
        "",
        "◇ 遵循HTTP/1.1协议标准",
        "◇ 支持CGI/ISAPI，支持PHP/ASP/Perl脚本",
        "◇ 支持自定义端口、最大连接数、网络超时",
        "◇ 支持自定义缺省文档列表，支持目录浏览",
        "◇ 支持中文UTF-8编码",
        "◇ 支持Cookie，支持SOAP等",
        "◇ 支持多线程文件下载、断点续传",
        "◇ 支持访问日志记录",
        "◇ 极少的资源占用，极快的访问速度，极简单的设置",
        "◇ 绿色软件，无须安装，不写注册表",
        "◇ VC++ SDK 编写，体积超小",
        "",
        "欢迎测试！",//，并提出您的宝贵意见和建议",
        "如有Bug或建议请来信 baojianjob@tom.com",
        "Copyright (C) 2007.11 王宝剑"
    };

    /*
    考虑改成在WM_ERASEBKGND中画渐变背景
    不用memdc，而采用单行刷新的方法
    */
    switch(Message)
    {
        case WM_STARTSCROLL:
        {
            ::GetClientRect(hWnd, &ClientRect);

            if(hMemDc != NULL)
                return 0;
            HDC hDc = ::GetDC(hWnd);
            hMemDc = CreateCompatibleDC(hDc);
            hMemBmp = CreateCompatibleBitmap(hDc, ClientRect.right, ClientRect.bottom);
            SelectObject(hMemDc, hMemBmp);
            for(int i = 0; i < ClientRect.bottom; i++)
            {
                MoveToEx(hMemDc, 0, i, NULL);
                COLORREF Color1 = ::GetSysColor(COLOR_BTNFACE);
                //COLORREF Color1 = RGB(0, 0, 0);
                COLORREF Color2 = RGB(255, 255, 255);
                //COLORREF Color2 = ::GetSysColor(COLOR_BTNFACE);

                bool bUp = i < ClientRect.bottom / 2;
                COLORREF Color = GetGradualColor(bUp ? Color1 : Color2, bUp ? Color2 : Color1, ClientRect.bottom, i);

                HPEN hPen = ::CreatePen(PS_SOLID, 1, Color);
                HPEN hPenOld = (HPEN)::SelectObject(hMemDc, hPen);
                LineTo(hMemDc, ClientRect.right, i);
                ::DeleteObject(::SelectObject(hMemDc, hPenOld));
            }

            hMemDc2 = CreateCompatibleDC(hDc);
            hMemBmp2 = CreateCompatibleBitmap(hDc, ClientRect.right, ClientRect.bottom);
            SelectObject(hMemDc2, hMemBmp2);
            ::ReleaseDC(hWnd, hDc);

            ::memset(&LogFont, 0, sizeof(LOGFONT));
            lstrcpy(LogFont.lfFaceName, "宋体");
            LogFont.lfWeight = 400;
            LogFont.lfHeight = -12;
            LogFont.lfCharSet = 134;
            LogFont.lfOutPrecision = 3;
            LogFont.lfClipPrecision = 2;
            LogFont.lfQuality = 1;
            LogFont.lfPitchAndFamily = 2;

            Height = sizeof(pText) / sizeof(pText[0]) * 16;
            PosY = -Height;

            ::SetTimer(hWnd, ID_TIMER_SCROLL, (UINT)WParam, NULL);
            return 0;
        }
        case WM_STOPSCROLL:
        {
            if(hMemDc != NULL)
            {
                ::KillTimer(hWnd, ID_TIMER_SCROLL);
                ::DeleteDC(hMemDc);
                ::DeleteDC(hMemDc2);
                DeleteObject(hMemBmp);
                DeleteObject(hMemBmp2);
                hMemDc = hMemDc2 = NULL;
                hMemBmp = hMemBmp2 = NULL;
            }
            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT Ps;
            HDC hDc = ::BeginPaint(hWnd, &Ps);

            RECT ClientRect;
            ::GetClientRect(hWnd, &ClientRect);

            assert(hMemDc != NULL);
            ::BitBlt(hMemDc2, 0, 0, ClientRect.right, ClientRect.bottom, hMemDc, 0, 0, SRCCOPY);
            ::SetBkMode(hMemDc2, TRANSPARENT);
            ::SetTextColor(hMemDc2, RGB(0, 0, 255));
            HFONT hFont = ::CreateFontIndirect(&LogFont);
            HFONT hFontOld = (HFONT)::SelectObject(hMemDc2, hFont);

            for(int i = 0; i < sizeof(pText) / sizeof(pText[0]); i++)
            {
                int CurPosY = PosY + i * 16;
                if(CurPosY >= -16 && CurPosY <= ClientRect.bottom)
                    TextOut(hMemDc2, 5, CurPosY, pText[i], lstrlen(pText[i]));
            }
            ::DeleteObject(::SelectObject(hMemDc2, hFontOld));

            ::BitBlt(hDc, 0, 0, ClientRect.right, ClientRect.bottom, hMemDc2, 0, 0, SRCCOPY);

            ::EndPaint(hWnd, &Ps);
            return 0;
        }
        case WM_TIMER:
        {
            POINT CursorPos;
            ::GetCursorPos(&CursorPos);
            ::ScreenToClient(hWnd, &CursorPos);
            if(::PtInRect(&ClientRect, CursorPos))
                return 0;

            PosY--;
            if(PosY < -Height)
                PosY = ClientRect.bottom;

            ::InvalidateRect(hWnd, NULL, FALSE);

            return 0;
        }
        case WM_CONTEXTMENU:
        {
            POINT Pt;
            ::GetCursorPos(&Pt);

            POINT PtTemp = Pt;
            ::ScreenToClient(hWnd, &PtTemp);
            if(!::PtInRect(&ClientRect, PtTemp))
                return 0;

            HMENU hMenu = ::CreateMenu();
            HMENU hMenuPop = ::CreateMenu();
            ::AppendMenu(hMenuPop, MF_STRING, 100, "复制(&C)");
            ::AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hMenuPop, "");

            TrackPopupMenu(hMenuPop, TPM_LEFTALIGN | TPM_RIGHTBUTTON, Pt.x, Pt.y, 0, hWnd, NULL);

            ::DestroyMenu(hMenuPop);
            ::DestroyMenu(hMenu);

            return 0;
        }
        case WM_COMMAND:
        {
            if(WParam == 100) // 复制说明内容到内存
            {
                MtSTRING Text;
                for(int i = 0; i < sizeof(pText) / sizeof(pText[0]); i++)
                {
                    Text += pText[i];
                    Text += "\r\n";
                }
                CopyToClipboard(Text);
            }
            return 0;
        }
    }

    WNDPROC pDefStaticProc = (WNDPROC)::GetWindowLong(hWnd, GWL_USERDATA);
    return CallWindowProc(pDefStaticProc, hWnd, Message, WParam, LParam);
}

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT Message, WPARAM WParam, LPARAM LParam)
{
    static HWND hWndAuthor;
    switch(Message)
    {
        case WM_INITDIALOG:
        {
            hWndAuthor = GetDlgItem(hDlg, IDC_STATIC_ABOUT);
            LONG DefStaticProc = SetWindowLong(hWndAuthor, GWL_WNDPROC, (LONG)StaticSubclassProc);
            SetWindowLong(hWndAuthor, GWL_USERDATA, DefStaticProc);

            ::SendMessage(hWndAuthor, WM_STARTSCROLL, 30, 0);

            ::SetWindowPos(hDlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            return TRUE;
        }
        case WM_COMMAND:
        {
            if(LOWORD(WParam) == IDOK || LOWORD(WParam) == IDCANCEL)
            {
                ::SendMessage(hWndAuthor, WM_STOPSCROLL, 0, 0);
                LONG DefStaticProc = GetWindowLong(hWndAuthor, GWL_USERDATA);
                SetWindowLong(hWndAuthor, GWL_WNDPROC, DefStaticProc);
                EndDialog(hDlg, LOWORD(WParam));
                return TRUE;
            }
            break;
        }
        case WM_CONTEXTMENU:
        {
            ::SendMessage(hWndAuthor, WM_CONTEXTMENU, WParam, LParam);
            return TRUE;
        }
    }
    return FALSE;
}

void AboutDlgModal(HWND hWndParent)
{
    static BOOL bPop = FALSE;
    if(!bPop)
    {
        bPop = TRUE;
        DialogBox(::GetModuleHandle(NULL), (LPCTSTR)IDD_ABOUTBOX, hWndParent, (DLGPROC)AboutDlgProc);
        bPop = FALSE;
    }
}