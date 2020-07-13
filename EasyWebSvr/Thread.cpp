// 多线程封装类
// wbj 2004.1.15
// wbj 2004.6.15

#include "stdafx.h"
#include "thread.h"

namespace minilib
{

BOOL CBaseThread::Start()
{
  if(hThread != INVALID_HANDLE_VALUE)
  {
    // assert(false);
    ::CloseHandle(hThread);
  }

  unsigned int ThreadID = 0;
  hThread = (HANDLE)::_beginthreadex(0, 0, ThreadFunction, (void*)this, 0, &ThreadID);

  return hThread != INVALID_HANDLE_VALUE;
}

BOOL CBaseThread::Wait(DWORD TimeoutMillis /* = INFINITE */) const
{
  DWORD Result = ::WaitForSingleObject(hThread, TimeoutMillis);

  if(Result == WAIT_OBJECT_0)
    return TRUE;
  else if(Result == WAIT_TIMEOUT)
    return FALSE;  // timeout
  else
    return FALSE;  // other error
}

unsigned int __stdcall CBaseThread::ThreadFunction(void *pData)
{
  CBaseThread *pThis = (CBaseThread *)pData;
  assert(pThis != NULL);

  int Result = 0;
  if(pThis)
  {
    Result = pThis->RunProc();
    //_endthreadex(Result);
  }

  return Result;
}

}
/*
线程的基础知识：

1. 线程是由两个部分组成的：一个是线程的内核对象，另一个是线程堆栈
2. 进程时不活泼的，它从来不执行任何东西，它只是线程的容器。线程的开销比进程少得多
3. 通常情况下，一个应用程序拥有一个用户界面线程，用于创建所有窗口，并且有一个
   GetMessage循环。进程中的所有其它线程都是工作线程，它们从不创建窗口
4. CreateThread函数是用来创建线程的Windows函数。不过，如果你正在编写C/C++代码，
   决不应该调用CreateThread。相反，应该使用Visual C++运行期库函数_beginthreadex
5. 调用CreateThread后，返回线程句柄，如果不再使用，应该用CloseHandle函数关闭
6. CreateThread的最后一个参数pdwThreadID，在Windows2000/NT下可以传递NULL，但是在
   Windows95/98下，必须传递一个有效的DWORD地址值
7. 要终止线程的运行，最好的方法是：线程函数返回。这是确保所有线程资源被正确地清
   除的唯一办法
8. 终止线程的运行，也可以用ExitThread函数。这种方法终止线程时，操作系统可以清除
   该线程使用的所有操作系统资源，但是C++资源（如C++对象）将不被撤消。ExitThread
   函数是Windows用来撤消线程的函数，如果编写C/C++代码，决不应该调用ExitThread，
   应该使用Visual C++运行期库函数_endthreadex
9. 终止线程的运行，也可以用TerminateThread函数。这个函数能够撤消任何线程。注意它
   是一个异步运行的函数，也就是说，它告诉系统你想要线程终止运行，但是当函数返回
   时，不能保证线程被撤消。如果需要确切地知道该线程已经终止运行，必须调用
   WaitForSingleObject或者类似的函数，传递线程的句柄。良好的设计应该从来不使用这
   个函数，因为被终止运行的线程收不到它被撤消的通知。线程不能正确地清除，并且不
   能防止自己被撤消
10.当使用返回或调用ExitThread的方法撤销线程时，该线程的内存堆栈也被撤消。但是，
   如果使用TerminateThread，那么在拥有线程的进程终止运行之前，系统不撤销该线程的
   堆栈。此外，当线程终止运行时，DLL通常收到通知，如果用TerminateThread强迫线程
   终止，DLL就不能收到通知
11.当线程终止运行时发生的操作：线程拥有的所有用户对象均被释放，系统自动撤消该线
   程的任何窗口，并且卸载该线程创建的或安装的任何挂钩；线程的退出代码从
   STILL_ACTIVE改为传递给ExitThread或TerminateThread的代码；线程的内核对象状态变
   为已通知；如果线程是进程中最后一个活动线程，系统也将进程视为已经终止运行；线
   程的内核对象使用计数递减 1
12.一旦线程终止运行，线程的句柄将不再有效。然而别的线程可以调用GetExitcodeThread
   来检查由hTread标识的线程是否已经终止运行
*/