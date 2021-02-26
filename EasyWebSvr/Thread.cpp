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
