// baojian
// 2004.3.18
// 2006.1.13
#ifndef __CRITICALSECTION_H__
#define __CRITICALSECTION_H__
#pragma once

class CCriticalSect
{
public:
  CCriticalSect();
  ~CCriticalSect();

public:
  void          Enter();
  void          Leave();

protected:
  CRITICAL_SECTION      m_CriticalSection;
};

inline CCriticalSect::CCriticalSect()
{
  ::InitializeCriticalSection(&m_CriticalSection);
}

inline CCriticalSect::~CCriticalSect()
{
  ::DeleteCriticalSection(&m_CriticalSection);
}

inline void CCriticalSect::Enter()
{
  ::EnterCriticalSection(&m_CriticalSection); 
}

inline void CCriticalSect::Leave()
{
  ::LeaveCriticalSection(&m_CriticalSection);
}

///////////////////////////////////////////////////////////////////////////////

class CritSectOwner
{
public:
  CritSectOwner(CCriticalSect &rCriticalSection);
  ~CritSectOwner();

  void          Enough();

protected:
  CCriticalSect &m_Cs;
  BOOL          m_bStillIn;
};

inline CritSectOwner::CritSectOwner(CCriticalSect &rCriticalSection) : 
  m_Cs(rCriticalSection), m_bStillIn(TRUE)
{
  m_Cs.Enter();
}

inline CritSectOwner::~CritSectOwner()
{
  if(m_bStillIn)
    m_Cs.Leave();
}

inline void CritSectOwner::Enough()
{
  if(m_bStillIn)
    m_Cs.Leave();
  m_bStillIn = FALSE;
}

#endif // __CRITICALSECTION_H__