// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//


#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


#define SEQUENCED_TIMERS

// Windows Header Files:
#if defined _MSVC_INTEL || defined _MSVC_CE2002ARM
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#pragma warning (disable:4996) // sprintf, etc deprecated
#include <d2d1_1.h>

#include <limits>
#include <type_traits>
#include <memory>
#endif

// C RunTime Header Files
#if defined _MSVC_INTEL || defined _MSVC_CE2002ARM
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <tchar.h>
#else
#ifdef _LINUX
#  include <SDL.h>
#  include <stdlib.h>
#  include <stdio.h>
#  include <memory.h>
#  include <glib.h>
#  include <sys/stat.h>
#  ifdef USE_OLD_GTK
#    include <gtk/gtk.h>
#  endif
# else
#  include <stdlib.h>
#  include <stdio.h>
# endif
#endif

// Local Header Files

// TODO: reference additional headers your program requires here

#include "Objects.h"
#include "CSBTypes.h"

#ifdef _MSVC_INTEL
#pragma warning(disable:4710)
#endif

#ifdef _MSVC_CE2002ARM
void ec(void);
#define EC ec();
#define CEtry _try {
#define CEexception(n) } _except(per=GetExceptionInformation(),CaptureExceptionInfo(), 1){PrintExceptionInfo();PostQuitMessage(1); };
#else
#define EC
#define CEtry
#define CEexception(n)
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

constexpr RECT g_rcAtari{0, 0, 320, 200};
extern POINT g_aspectRatio;


// Counting Pointers for automatic reference counting
template<typename T>
struct CntPtrTo
{
   CntPtrTo() = default;
   CntPtrTo(const CntPtrTo<T> &ptr) noexcept : m_p(ptr.m_p) { if(m_p) m_p->AddRef(); }
   CntPtrTo(CntPtrTo<T> &&ptr) noexcept : m_p(ptr.Extract()) { }
   template<typename TDerived> CntPtrTo(CntPtrTo<TDerived> &&ptr) noexcept : m_p(ptr.Extract()) { }
   CntPtrTo(T *pTNew) noexcept : m_p(pTNew) { if(m_p) m_p->AddRef(); }
   ~CntPtrTo() noexcept { if(m_p) m_p->Release(); }

   T **Address() noexcept { return &m_p; }

         T* operator ->()       noexcept { return m_p; }
   const T* operator ->() const noexcept { return m_p; }

   operator       T*()       noexcept { return m_p; }
   operator const T*() const noexcept { return m_p; }

   CntPtrTo<T> &operator=(const CntPtrTo<T> &ptr) noexcept { *this=ptr.m_p; return *this; }
   CntPtrTo<T> &operator=(CntPtrTo<T> &&ptr) noexcept { if(m_p) m_p->Release(); m_p=ptr.Extract(); return *this; }
   T *Extract() noexcept { T *p=m_p; m_p=nullptr; return p; } // Return pointer without releasing it

   CntPtrTo<T> &operator=(T *pTNew) noexcept
   {
      T *pT=m_p; m_p=pTNew; // To prevent release recursion problems
      if(m_p) m_p->AddRef();
      if(pT) pT->Release();
      return *this;
   }

   HRESULT CoCreateInstance(const CLSID &clsid, DWORD clsctx) { Assert(!m_p); return ::CoCreateInstance(clsid, nullptr, clsctx, __uuidof(T), IID_PPV_ARGS_Helper(&m_p)); }
private:
   T *m_p{};
};

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
