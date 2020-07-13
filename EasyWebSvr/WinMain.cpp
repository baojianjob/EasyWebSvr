#include "stdafx.h"
#include "resource.h"
#include "MainWnd.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR CmdLine, int CmdShow)
{ 
    CMainWnd MainWnd;

    BOOL bSucess = MainWnd.CreateEx(0, _T("EasyWebSvrClass"), _T("EasyWebServer"), 
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 0, 0, 450, 300, NULL, (HMENU)NULL);

    if(!bSucess)
        return 0; 

    CenterWindow(MainWnd.m_hWnd);

    MainWnd.ShowWindow(SW_SHOW);
    MainWnd.UpdateWindow(); 

    MSG Msg;
    while(GetMessage(&Msg, NULL, 0, 0)) 
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return (int)Msg.wParam;
}
