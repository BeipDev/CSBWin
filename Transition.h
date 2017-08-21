#ifndef __TRANSITION__
#define __TRANSITION__

#define TARGET_RT_MAC_MACH_O    1

#include <CarbonCore/MacTypes.h>
#include <AvailabilityMacros.h>


#ifndef _MSVC_INTEL
enum {
	WM_PAINT,
	WM_DESTROY,
	WM_CREATE,
	WM_TIMER,
	WM_LBUTTONDOWN,
	WM_LBUTTONUP,
	WM_RBUTTONDOWN,
	WM_RBUTTONUP,
	WM_NCMOUSEMOVE,
	WM_MOUSEMOVE,
	WM_ERASEBKGND,
	WM_KEYDOWN,
	WM_CHAR
};
#endif

//#include "Dispatch.h"
#include <string.h>
#include <stdio.h>
#include "CSBTypes.h"
#define 	BITMAPINFO	PixMapHandle

typedef WindowRef       HWND;
typedef i32             HDC;
typedef void*           HINSTANCE;
typedef UInt32          DWORD;
typedef SInt32          LONG;
typedef SInt16          WORD;
typedef char            BYTE;
typedef unsigned char   UBYTE;
typedef unsigned short  UWORD;

#define		MB_OK			1
#define		MB_YESNO		2

#define 	IDYES			1
#define		IDNO			2

#define		MB_TOPMOST		0
#define		MB_TASKMODAL	0
#define		MB_ICONWARNING	0

#define		SND_ASYNC		0x0000
#define		SND_SYNC		0x0001

#define		SND_MEMORY		0x0002

#define		max(a,b) (((a)>(b))?(a):(b))
#define		min(a,b) (((a)<(b))?(a):(b))

//#define		LOWORD(a) LoWord(a)
//#define		HIWORD(a) HiWord(a)

typedef struct POINT {
	int x;
	int y;
} POINT;


typedef struct SYSTEMTIME {
	unsigned short wSecond;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wMilliseconds;
	unsigned short wMonth;
	unsigned short wYear;
	unsigned short wDay;
} SYSTEMTIME;


void 	Sleep(long secs);
//void 	Delay(RETURNS r, int par2);
short 	MessageBox(long par1, const char* text, const char* par2, long mbtype);

#ifndef strupr
	void 	strupr(char *t);
#endif
#define _strupr(c) strupr(c)

void XShowCursor(ui8 flag);
void XHideCursor();

void sndPlaySound(char *wave, long flags);
void ScreenToClient(WindowPtr window, POINT* point);
void GetCursorPos(POINT* point);
void GetSystemTime(SYSTEMTIME *s);

i64  UI_GetSystemTime();
void UI_GetCursorPos(i32 *x, i32 *y);

FILE* UI_fopen(char* name, const char* mode);
void InvalidateRect(HWND w, i32, i32);
void DebugDisplay(bool viewport);

void UI_free(void*);
void* UI_malloc(i32 size, ui32 id);
#endif