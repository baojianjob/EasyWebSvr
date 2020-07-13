// 多线程封装类
// wbj 2004.1.15
#ifndef __BASETHREAD_H__
#define __BASETHREAD_H__

//#include <process.h>

#ifndef _MT
  #error "必须设置为多线程模式"
#endif

namespace minilib
{

class CBaseThread 
{
public :
  CBaseThread();
  virtual ~CBaseThread();

  operator HANDLE() const;
  BOOL          Start();
  BOOL          Terminate(DWORD ExitCode = 0);
  BOOL          Wait(DWORD TimeoutMillis = INFINITE) const;

private:
  HANDLE        hThread;

private:
  virtual int   RunProc() = 0; 
  static unsigned int __stdcall ThreadFunction(void *pData);

  // No copies do not implement
  CBaseThread(const CBaseThread &Rhs);
  CBaseThread &operator=(const CBaseThread &Rhs);
};

inline CBaseThread::CBaseThread()
{
  hThread = INVALID_HANDLE_VALUE;
}

inline CBaseThread::~CBaseThread()
{
  if(hThread != INVALID_HANDLE_VALUE)
    ::CloseHandle(hThread);
}

inline CBaseThread::operator HANDLE() const
{
  return hThread;
}

inline BOOL CBaseThread::Terminate(DWORD ExitCode /* = 0 */)
{
  return ::TerminateThread(hThread, ExitCode);
}

}

#endif // __BASETHREAD_H__