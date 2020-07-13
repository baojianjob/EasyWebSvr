// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

//#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件：
#include <winsock2.h>
#include <windows.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: 在此处引用程序要求的附加头
#include <stdio.h>
#include <commctrl.h>
#include <assert.h>
#include <process.h>

#include "core_log.h"

#define  ARRAY_SIZE(x)  (sizeof(x) / sizeof(x[0]))

#ifdef _DEBUG
  #define VERIFY(f)     assert(f)
#else
  #define VERIFY(f)     ((void)(f))
#endif

#define APP_TITLE       _T("EasyWebServer") 
