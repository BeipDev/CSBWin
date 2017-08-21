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
#include <windows.h>
#pragma warning (disable:4996) // sprintf, etc deprecated
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

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)

