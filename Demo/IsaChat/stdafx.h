// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once
#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
#include <tchar.h>
#include <process.h>
#include <httpext.h>
#include <assert.h>
#include <vector>
#include <list>
using namespace std;

//#define _LOGDEBUG
#if defined _LOGDEBUG
    #define LOG_INFO(x) OutputDebugString(x)
#else
    #if _MSC_VER >= 1300
    #define LOG_INFO(x) __noop  
    #else
    #define LOG_INFO(x) ((void)0)
    #endif
#endif