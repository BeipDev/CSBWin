// CSBwin.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "commctrl.h"
#include "resource.h"
#include <stdio.h>
#include "UI.h"
#include "dispatch.h"
//#include "objects.h"
#include "CSB.h"
#include "data.h"

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "D2D1.lib")

bool GetVideoRectangle(i32, RECT *);
void FreeIfNonNULL(void **pointer);
void DumpImages(void);


i32 trace = -1;
CSB_UI_MESSAGE csbMessage;
bool overlappingText = false;
extern i32 screenSize;
extern bool BeginRecordOK;
extern bool ItemsRemainingOK;
extern bool PlayfileIsOpen(void);
extern bool RecordfileIsOpen(void);
extern bool TimerTraceActive;
extern bool AttackTraceActive;
extern bool AITraceActive;
extern bool RecordMenuOption;
extern i32 NoSpeedLimit;
extern i32 GameMode;
extern i32 MostRecentlyAdjustedSkills[2];
extern i32 LatestSkillValues[2];
extern XLATETYPE latestScanType;
extern XLATETYPE latestCharType;
extern i32 latestCharp1;
extern i32 latestScanp1;
extern i32 latestScanp2;
extern i32 latestScanXlate;
extern i32 latestCharXlate;
extern unsigned char *encipheredDataFile;
extern bool simpleEncipher;
extern ui32 dumpWindow;
extern RECT g_rcClient;

CntPtrTo<ID2D1Factory1> g_pID2D1Factory1;
CntPtrTo<ID2D1HwndRenderTarget> g_pID2DRenderTarget;
CntPtrTo<ID2D1Bitmap> g_pID2DBitmap;

POINT g_aspectRatio{320, 240};

char *parentFolder(char *folderName, char *endName);

char *helpMessage = "CSBWin looks in three places for files:\n"
                    " 1) The default directory\n"
                    " 2) The parent of the default directory\n"
                    " 3) The directory of CSBWin.exe\n"
                    "      Searched in order 1, 2, 3\n\n."
                    "The default directory can be spcified in two ways:\n"
                    " 1) The command line   directory=\"c:\\My Games\\DM\\\"\n"
                    " 2) The config.txt file   directory \"c:\\My Games\\DM\\\"\n\n"
                    "      The quotes are necessary if the name contains spaces.\n"
                    "      The config.txt file overrides the command line\n"
                    "      except (of course) when searching for config.txt\n\n"
                    "If no default directory is specified then the only\n"
                    "directory searched will be the directory containing CSBWin.exe.\n\n"
                    "Whenever a file is created it will be created in the first of\n"
                    "the three directories that is specified.  So new files will go\n"
                    "into the default directory unless none is specified, in which\n"
                    "case they will go into the CSBWin.exe directory";

i32 WindowWidth = 960;
i32 WindowHeight = 0;
i32 WindowX = 0;
i32 WindowY = 0;
bool fullscreenRequested = false;
extern bool virtualFullscreen;

i32 line = 0;

// Global Variables:
HINSTANCE hInst;								// current instance
HWND hWnd; // Global
char szTitle[MAX_LOADSTRING];								// The title bar text
char szWindowClass[MAX_LOADSTRING];								// The title bar text
char szCSBVersion[MAX_LOADSTRING];

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, i32);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


void MTRACE(char *msg)
{
  if (trace < 0) return;
  FILE *f = GETFILE(trace);
  fprintf(f, msg);
  fflush(f);
}


char *GetField(char *pCol, char *field, char term)
{
  i32 col;
  while (*pCol == ' ') pCol++;
  col = 0;
  field[col] = 0;
  while ( col<200 )
  {
    if (*pCol == 0) return pCol;
    if (*pCol == term) return pCol;
    if ( (term == '=') && (*pCol == ' ') ) return pCol;
    field[col] = *pCol;
    pCol++;
    col++;
    field[col] = 0;
  };
  return pCol;
}

char *ParseOption(char *pCol, char *key, char *value)
{
  pCol = GetField(pCol, key, '=');
  _strupr(key);
  value[0] = 0;
  if (*pCol == '=') 
  {
    if (pCol[1] == '"')
    {
      pCol = GetField(pCol+2, value, '"');
    }
    else
    {
      pCol = GetField(pCol+1, value, ' ');
    };
  }
  return pCol+1;
}

i32 WINAPI WinMain(HINSTANCE hInstance ,
	HINSTANCE /*hPrevInstance*/,
	LPSTR     /*lpCmdLine*/,
	int       nCmdShow)
//#endif
{
   D2D1_FACTORY_OPTIONS d2dOptions;
#if defined(_DEBUG)
   d2dOptions.debugLevel=D2D1_DEBUG_LEVEL_INFORMATION;
#else
   d2dOptions.debugLevel=D2D1_DEBUG_LEVEL_NONE;
#endif

   D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dOptions, g_pID2D1Factory1.Address());

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;


  if (encipheredDataFile != NULL)
  memcpy(encipheredDataFile, WinMain, 64);

	// Initialize global strings
	LoadStringA(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringA(hInstance, IDC_CSBWIN, szWindowClass, MAX_LOADSTRING);
	LoadStringA(hInstance, IDS_VERSION, szCSBVersion, MAX_LOADSTRING);
    versionSignature = Signature(szCSBVersion);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

  speedTable[SPEED_GLACIAL].vblPerTick = 1000;
  speedTable[SPEED_MOLASSES].vblPerTick = 55;
  speedTable[SPEED_VERYSLOW].vblPerTick = 33;
  speedTable[SPEED_SLOW].vblPerTick = 22;
  speedTable[SPEED_NORMAL].vblPerTick = 15;
  speedTable[SPEED_FAST].vblPerTick = 11;
  speedTable[SPEED_QUICK].vblPerTick = 7;

  volumeTable[VOLUME_FULL].attenuation = 0;
  volumeTable[VOLUME_HALF].attenuation = 6;
  volumeTable[VOLUME_QUARTER].attenuation = 12;
  volumeTable[VOLUME_EIGHTH].attenuation = 18;
  volumeTable[VOLUME_OFF].attenuation = 100;

  volumeTable[VOLUME_FULL].divisor = 1;
  volumeTable[VOLUME_HALF].divisor = 2;
  volumeTable[VOLUME_QUARTER].divisor = 4;
  volumeTable[VOLUME_EIGHTH].divisor = 8;
  volumeTable[VOLUME_OFF].divisor = 65535;

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_CSBWIN);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
  Cleanup(true); //Program termination
  UI_CheckMemoryLeaks();
	return msg.wParam;
}

void ProcessCommandLine(void)
{
  char *lpCmdLine;
  char *pCol, *command;
  char term;
  char key[201], value[201];
  lpCmdLine = GetCommandLineA();
  term = ' ';
  pCol = lpCmdLine;
  if (*pCol == '"')
  {
    pCol++;
    term = '"';
  };
  command = pCol;
  while (*pCol != 0)
  {
    if (*pCol == term) break;
    pCol++;
  };
  root = parentFolder(command, pCol);
  pCol = lpCmdLine;
  if (pCol[0] == '"')
  {
    pCol = GetField(lpCmdLine+1, key, '"')+1;
  }
  else
  {
   pCol = GetField(lpCmdLine, key, ' ');
  };
  while (*pCol != 0)
  {
    pCol = ParseOption(pCol, key, value);
    if (key[0] != 0) UI_ProcessOption(key, value);
  };
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXA wcex;

  ProcessCommandLine();
//  if (!fullscreenRequested)
  {
	  wcex.cbSize = sizeof(WNDCLASSEX); 

	  wcex.style			= CS_HREDRAW | CS_VREDRAW;
	  wcex.lpfnWndProc	= (WNDPROC)WndProc;
	  wcex.cbClsExtra		= 0;
	  wcex.cbWndExtra		= 0;
	  wcex.hInstance		= hInstance;
	  wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_CSBWIN);
	  wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	  wcex.hbrBackground	= (HBRUSH)::GetStockObject(BLACK_BRUSH);
	  wcex.lpszMenuName	= (LPCSTR)IDC_CSBWIN;
	  wcex.lpszClassName	= szWindowClass;
	  wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
  }
#if 0
  else
  {

	  wcex.cbSize = sizeof(WNDCLASSEX); 

  	wcex.style			= 0;
  	wcex.lpfnWndProc	= (WNDPROC)WndProc;
  	wcex.cbClsExtra		= 0;
  	wcex.cbWndExtra		= 0;
  	wcex.hInstance		= hInstance;
  	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_CSBWIN);
  	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
  	wcex.hbrBackground	= hBrush;
  	wcex.lpszMenuName	= NULL;
  	wcex.lpszClassName	= szWindowClass;
  	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
  };
#endif
	return RegisterClassExA(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, i32)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, i32 nCmdShow)
{
   //HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   RECT rc{0, 0, WindowWidth, WindowWidth*240/320};
   AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, true);

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      WindowX, WindowY, rc.right-rc.left, rc.bottom-rc.top, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
//   UpdateWindow(hWnd);
   SetTimer(hWnd, 1, 10, NULL);
   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
i32 delta[60];
i32 prevms = -1;
i32 slot;
void display(void);
extern ui32 VBLInterruptCount;
extern ui32 CheckVBLCount;
extern ui32 STBLTCount;
extern i32 GameTime;

bool g_cursorIsShowing = true;

// Converted from code that used rich vector types, so these are a little more cumbersome than they should be
POINT TouchFromInside(POINT sizeContainer, POINT sizeObject)
{
   float minRatio=min(sizeContainer.x/float(sizeObject.x), sizeContainer.y/float(sizeObject.y));
   return POINT{LONG(sizeObject.x*minRatio), LONG(sizeObject.y*minRatio)};
}

RECT TouchFromInside(const RECT &rcArea, POINT size)
{
   POINT center{(rcArea.left+rcArea.right)/2, (rcArea.top+rcArea.bottom)/2};
   POINT radius=TouchFromInside(POINT{rcArea.right-rcArea.left, rcArea.bottom-rcArea.top}, size);
   radius.x/=2;
   radius.y/=2;
   return RECT{center.x-radius.x, center.y-radius.y, center.x+radius.x, center.y+radius.y};
}

// Translates a point relative to rcSource to one relative to rcDest
POINT MapPoint(const RECT &rcDest, const RECT &rcSource, POINT point)
{
   return POINT{(point.x-rcSource.left)*(rcDest.right-rcDest.left)/(rcSource.right-rcSource.left)+rcDest.left,
               (point.y-rcSource.top)*(rcDest.bottom-rcDest.top)/(rcSource.bottom-rcSource.top)+rcDest.top};
}

LRESULT CALLBACK WndProc(HWND _hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   i32 wmId, wmEvent;
   ASSERT( (hWnd==0) || (hWnd==_hWnd) , "hWnd");
   hWnd = _hWnd;
   char msg[100];

  MTRACE("msg=");
   switch(message) 
   {
      case WM_SIZE:
      {
         RECT rcClient; ::GetClientRect(hWnd, &rcClient);

         g_rcClient=TouchFromInside(rcClient, g_aspectRatio);
         g_pID2DRenderTarget->Resize(D2D1_SIZE_U{uint32_t(rcClient.right-rcClient.left), uint32_t(rcClient.bottom-rcClient.top)});
         break;
      }
		case WM_COMMAND:
      MTRACE("WM_COMMAND\n");
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
        case 0:
            MTRACE("0\n");
            break;
				case IDM_ABOUT:
            MTRACE("IDM_ABOUT\n");
				    DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				    break;
        case IDM_HELP:
            MTRACE("IDM_HELP\n");
            UI_MessageBox(helpMessage, "Help", MESSAGE_OK);
            break;
       case ID_4X3ASPECTRATIO:
         {
            bool fChecked=(::GetMenuState(GetMenu(hWnd), ID_4X3ASPECTRATIO, MF_BYCOMMAND)&MF_CHECKED)!=0;
            fChecked^=true;
            ::CheckMenuItem(GetMenu(hWnd), ID_4X3ASPECTRATIO, MF_BYCOMMAND|(fChecked ? MF_CHECKED : 0));
            
            g_aspectRatio=fChecked ? POINT{320, 240} : POINT{320, 200};
            RECT rcClient; ::GetClientRect(hWnd, &rcClient);
            g_rcClient=TouchFromInside(rcClient, g_aspectRatio);
            InvalidateRect(hWnd, nullptr, true);
            break;
         }
        case IDM_Statistics:
            MTRACE("IDM_Statistics\n");
            csbMessage.type = UIM_Statistics;
            csbMessage.p1 = 0;
            csbMessage.p2 = 0;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            }; 
            break; 
        case IDM_GameVersion:
            MTRACE("IDM_GameVersion\n");
            csbMessage.type = UIM_EditGameInfo;
            csbMessage.p1 = 0;
            csbMessage.p2 = 2; //2-(screenSize==2); //new value
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            }; 
            break; 

				case IDM_EXIT:
            MTRACE("IDM_EXIT\n");
            if (trace >= 0) CLOSE(trace);
            trace = -1;
				    DestroyWindow(hWnd);
				    break;
        case IDC_Normal:
            MTRACE("IDC_Normal\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_NORMAL;
            csbMessage.p2 = 2; //2-(screenSize==1); //new value
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_Double:
            MTRACE("IDC_Double\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_DOUBLE;
            csbMessage.p2 = 2; //2-(screenSize==2); //new value
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_Triple:
            MTRACE("IDC_Triple\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_TRIPLE;
            csbMessage.p2 = 2; //2-(screenSize==3); //new value
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_Quadruple:
            MTRACE("IDC_Quadruple\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_QUADRUPLE;
            csbMessage.p2 = 2; //2-(screenSize==4); //new value
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_Quintuple:
            MTRACE("IDC_Quintuple\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_QUINTUPLE;
            csbMessage.p2 = 2; //2-(screenSize==4); //new value
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_Sextuple:
            MTRACE("IDC_SecTuple\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_SEXTUPLE;
            csbMessage.p2 = 2; //2-(screenSize==4); //new value
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_QuickPlay:
            MTRACE("IDC_QuickPlay\n");
            if (!PlayfileIsOpen()) break;
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_QUICKPLAY;
            csbMessage.p2 = (NoSpeedLimit!=0) ? 0 : 1;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_Glacial:
            MTRACE("IDM_Glacial\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_CLOCK;
            csbMessage.p2 = SPEED_GLACIAL;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_Molasses:
            MTRACE("IDM_Molasses\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_CLOCK;
            csbMessage.p2 = SPEED_MOLASSES;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_VerySlow:
            MTRACE("IDM_VerySlow\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_CLOCK;
            csbMessage.p2 = SPEED_VERYSLOW;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_Slow:
            MTRACE("IDM_Slow\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_CLOCK;
            csbMessage.p2 = SPEED_SLOW;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_Normal:
            MTRACE("IDM_Normal\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_CLOCK;
            csbMessage.p2 = SPEED_NORMAL;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_Fast:
            MTRACE("IDM_Fast\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_CLOCK;
            csbMessage.p2 = SPEED_FAST;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_Quick:
            MTRACE("IDM_Quick\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_CLOCK;
            csbMessage.p2 = SPEED_QUICK;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_PlayerClock:
            MTRACE("IDM_PlayerClock\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_PLAYERCLOCK;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_DMRULES:
            MTRACE("IDM_DMRULES\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_DMRULES;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_ExtraTicks:
            MTRACE("IDM_ExtraTicks\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_EXTRATICKS;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_Record:
            MTRACE("IDC_Record\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_RECORD;
            csbMessage.p2 = 1;
            if (RecordMenuOption) csbMessage.p2 = 0;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_TimerTrace:
            MTRACE("IDC_TimerTrace\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_TIMERTRACE;
            csbMessage.p2 = 1;
            if (TimerTraceActive) csbMessage.p2 = 0;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_GraphicTrace:
            MTRACE("IDC_GraphicTrace\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_GRAPHICTRACE;
            csbMessage.p2 = 1;
            if (TimerTraceActive) csbMessage.p2 = 0;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
         case IDC_AttackTrace:
            MTRACE("IDC_AttackTrace\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_ATTACKTRACE;
            csbMessage.p2 = 1;
            if (AttackTraceActive) csbMessage.p2 = 0;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
         case IDC_DSATrace:
            MTRACE("IDC_DSATrace\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_DSATRACE;
            csbMessage.p2 = 1;
#ifndef _MSVC_INTEL
            if (DSATraceActive) csbMessage.p2 = 0;
#endif
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_AITrace:
            MTRACE("IDC_AITrace\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_AITRACE;
            csbMessage.p2 = 1;
            if (AITraceActive) csbMessage.p2 = 0;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_ItemsRemaining:
            MTRACE("IDC_ItemsRemaining\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_ITEMSREMAINING;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_NonCSBItemsRemaining:
            MTRACE("IDC_NonCSBItemsRemaining\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_NONCSBITEMSREMAINING;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDC_Playback:
            MTRACE("IDC_Playback\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_PLAYBACK;
            csbMessage.p2 = 1;
            if (PlayfileIsOpen()) csbMessage.p2 = 0;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_WindowDump:
            dumpWindow = 1;
            break;
        case IDC_DispatchTrace:
            MTRACE("IDC_DispatchTrace\n");
            if (trace >= 0) 
            {
              CLOSE(trace);
              trace = -1;
              break;
            };
            trace = CREATE("trace.log","w", true);
            break;
        case IDM_VOLUME_OFF:
            MTRACE("IDM_VOLUME_OFF\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_VOLUME;
            csbMessage.p2 = VOLUME_OFF;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_VOLUME_QUARTER:
            MTRACE("IDM_VOLUME_QUARTER\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_VOLUME;
            csbMessage.p2 = VOLUME_QUARTER;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_VOLUME_EIGHTH:
            MTRACE("IDM_VOLUME_EIGHTH\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_VOLUME;
            csbMessage.p2 = VOLUME_EIGHTH;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_VOLUME_HALF:
            MTRACE("IDM_VOLUME_HALF\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_VOLUME;
            csbMessage.p2 = VOLUME_HALF;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_VOLUME_FULL:
            MTRACE("IDM_VOLUME_FULL\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_VOLUME;
            csbMessage.p2 = VOLUME_FULL;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
        case IDM_AttemptDirectX:
            MTRACE("IDM_AttemptDirectX\n");
            csbMessage.type = UIM_SETOPTION;
            csbMessage.p1 = OPT_DIRECTX;
            csbMessage.p2 = 1;
            if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
            {
              PostQuitMessage(0);
              break;
            };
            break;
				default:
            MTRACE("Unknown WM_COMMAND\n");
				    return DefWindowProc(hWnd, message, wParam, lParam);
			};
			break;
    case WM_CTLCOLOREDIT:
      MTRACE("WM_CTLCOLOREDIT\n");
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_POWERBROADCAST:
      MTRACE("WM_POWERBROADCAST\n");
      break;
		case WM_PAINT:
      MTRACE("WM_PAINT\n");
      {
         PAINTSTRUCT ps;
         HDC hdc=BeginPaint(hWnd, &ps);
      if (  (!virtualFullscreen) && (screenSize == 1)
          ||(virtualFullscreen && (videoSegSize[4]!=0)) )
      {
        line++;
        line = line%10;
        switch(line)
        {
        case 0:
          sprintf(msg,"vblint = %u",VBLInterruptCount);
          break;
        case 1:
          sprintf(msg,"chkvbl = %u",CheckVBLCount);
          break;
        case 2:
          sprintf(msg,"STblt = %u",STBLTCount);
          break;
        case 3:
          sprintf(msg,"Time = %u",GameTime);
          break;
        case 4:
          sprintf(msg,
                  "Skill[%d]=0x%08x      ",
                  MostRecentlyAdjustedSkills[0],
                  LatestSkillValues[0]);
          break;
        case 5:
          sprintf(msg,
                  "Skill[%d]=0x%08x      ",
                  MostRecentlyAdjustedSkills[1],
                  LatestSkillValues[1]);
          break;
        case 6:
          switch (latestCharType)
          {
          case TYPEIGNORED: 
            sprintf(msg, "%04x key --> Ignored                         ", latestCharp1);
            break;
          case TYPEKEY:
            sprintf(msg, "%04x key --> Translated %08x", latestCharp1, latestCharXlate);
            break;
          };
          break;
        case 7:
          switch (latestScanType)
          {
          case TYPESCAN:
          case TYPEIGNORED:
            sprintf(msg, "%08x mscan --> Ignored                        ",latestScanp1);
            break;
          case TYPEMSCANL:
            sprintf(msg, "%08x mscan --> Translated to %08x L",latestScanp1,latestScanXlate);
            break;
          case TYPEMSCANR:
            sprintf(msg, "%08x mscan --> Translated to %08x R",latestScanp1,latestScanXlate);
            break;
          default:
            sprintf(msg,"                                              ");
          };
          break;    
        case 8:
          switch (latestScanType)
          {
          case TYPEMSCANL:
          case TYPEIGNORED:
          case TYPEMSCANR:
            sprintf(msg, "%08x scan --> Ignored                        ",latestScanp2);
            break;
          case TYPESCAN:
            sprintf(msg, "%08x scan --> Translated to %08x", latestScanp2,latestScanXlate);
            break;
          default:
            sprintf(msg,"                                              ");
          };
          break;    
        case 9:
          sprintf(msg,"Total Moves = %d",totalMoveCount);
          break;
        };
        {
          i32 X, Y;
          X = 325;
          Y = 25;
          if (virtualFullscreen)
          {
            X = videoSegX[4];
            Y = videoSegY[4];
          };
          Y += 15 *line;
          TextOutA(hdc,X,Y,msg,strlen(msg));
        };
      };
      ::EndPaint(hWnd, &ps);
      csbMessage.type=UIM_PAINT;
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
			break;
      }
      case WM_MOUSEMOVE:
    {
//      auto &move=message.Cast<Msg::MouseMove>();
//      if(g_rcClient.IsInside(move.position()))
      if(GameMode == 1)
      {
         if(g_cursorIsShowing)
         {
            ShowCursor(false);
            g_cursorIsShowing=false;
         }
      }
      else
      {
         if (!g_cursorIsShowing)
         {
            ShowCursor(true);
            g_cursorIsShowing = true;
         };
      }
      __fallthrough;
    }
    case WM_TIMER:
      MTRACE("WM_TIMER\n");
      csbMessage.type=UIM_TIMER;
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
      MTRACE("   Returned\n");
      break;
		case WM_DESTROY:
      MTRACE("WM_DESTROY\n");
			PostQuitMessage(0);
			break;
    case WM_GETMINMAXINFO:
      MTRACE("WM_GETMINMAXINFO\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_NCCREATE:
      MTRACE("WM_NCCREATE\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_NCCALCSIZE:
      MTRACE("WM_NCCALCSIZE\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_CREATE:
    {
      MTRACE("WM_CREATE\n"); 
      csbMessage.type=UIM_INITIALIZE;
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      }
      {
         D2D1_RENDER_TARGET_PROPERTIES properties;
         properties.type=D2D1_RENDER_TARGET_TYPE_DEFAULT;
         properties.pixelFormat.format=DXGI_FORMAT_B8G8R8A8_UNORM;
         properties.pixelFormat.alphaMode=D2D1_ALPHA_MODE_IGNORE;
         properties.dpiX=0;
         properties.dpiY=0;
         properties.usage=D2D1_RENDER_TARGET_USAGE_NONE;
         properties.minLevel=D2D1_FEATURE_LEVEL_DEFAULT;

         D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProperties;
         hwndProperties.hwnd=hWnd;
         hwndProperties.pixelSize.width=1;
         hwndProperties.pixelSize.height=1;
         hwndProperties.presentOptions=D2D1_PRESENT_OPTIONS_IMMEDIATELY; // D2D1_PRESENT_OPTIONS_NONE;

         g_pID2D1Factory1->CreateHwndRenderTarget(&properties, &hwndProperties, g_pID2DRenderTarget.Address());
         g_pID2DRenderTarget->SetDpi(96, 96);
      }
      {
         D2D1_SIZE_U size{uint32_t(g_rcAtari.right-g_rcAtari.left), uint32_t(g_rcAtari.bottom-g_rcAtari.top)};

         D2D1_BITMAP_PROPERTIES properties;
         properties.dpiX=0;
         properties.dpiY=0;
         properties.pixelFormat.format=DXGI_FORMAT_B8G8R8A8_UNORM;
         properties.pixelFormat.alphaMode=D2D1_ALPHA_MODE_IGNORE;

         g_pID2DRenderTarget->CreateBitmap(size, properties, g_pID2DBitmap.Address());
      }

		return DefWindowProc(hWnd, message, wParam, lParam);
   }
    case WM_SHOWWINDOW:
      MTRACE("WM_SHOWWINDOW\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_NCDESTROY:
      MTRACE("WM_NCDESTROY\n"); 
      if (trace >= 0) CLOSE(trace);
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_WINDOWPOSCHANGING:
      MTRACE("WM_WINDOWPOSCHANGING\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_ACTIVATEAPP:
      if (wParam) MTRACE("WM_ACTIVATEAPP(true)\n"); 
      else MTRACE("WM_ACTIVATEAPP(false)\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_NCACTIVATE:
      MTRACE("WM_NCACTIVATE\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_GETTEXT:
      MTRACE("WM_GETTEXT\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_ACTIVATE:
      MTRACE("WM_ACTIVATE\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_SETFOCUS:
      MTRACE("WM_SETFOCUS\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_NCPAINT:
      MTRACE("WM_NCPAINT\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_ERASEBKGND:
      MTRACE("WM_ERASEBKGND\n"); 
      csbMessage.type=UIM_REDRAW_ENTIRE_SCREEN;
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_WINDOWPOSCHANGED:
      MTRACE("WM_WINDOWPOSCHANGED\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_MOVE:
      MTRACE("WM_MOVE\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_NCHITTEST:
      MTRACE("WM_NCHITTEST\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_SETCURSOR:
      MTRACE("WM_SETCURSOR\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_KEYDOWN:
      MTRACE("WM_KEYDOWN\n"); 
      csbMessage.type=UIM_KEYDOWN;
      csbMessage.p1 = wParam; //virtual key
      csbMessage.p2 = (lParam>>16)&0xff; //scancode
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_KEYUP:
      MTRACE("WM_KEYUP\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_CHAR:
      MTRACE("WM_CHAR\n"); 
      csbMessage.type=UIM_CHAR;
      csbMessage.p1 = wParam;
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
      {
        PostQuitMessage(0);
        break;
      };
			return 0;// DefWindowProc(hWnd, message, wParam, lParam);
    case WM_KILLFOCUS:
      MTRACE("WM_KILLFOCUS\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_NCMOUSEMOVE:
      MTRACE("WM_NCMOUSEMOVE\n"); 
      if (g_cursorIsShowing)
      {
      }
      else
      {
        ShowCursor(true);
        g_cursorIsShowing = true;
      };
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_SYSKEYDOWN:
      MTRACE("WM_SYSKEYDOWN\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_SYSCHAR:
      MTRACE("WM_SYSCHAR\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_SYSCOMMAND:
      MTRACE("WM_SYSCOMMAND\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_ENTERMENULOOP:
      MTRACE("WM_ENTERMENULOOP\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_INITMENU:
      MTRACE("WM_INITMENU\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_MENUSELECT:
      MTRACE("WM_MENUSELECT\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_INITMENUPOPUP:
      MTRACE("WM_INITMMENUPOPUP\n"); 
      switch (LOWORD(lParam))
      {
      case 0: //File menu
        break;
      case 1: //Misc menu
        {
          i32 flag;
          if (screenSize==1)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDC_Normal,
                         MF_BYCOMMAND|flag);

          if (screenSize==2)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDC_Double,
                         MF_BYCOMMAND|flag);

          if (screenSize==3)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDC_Triple,
                         MF_BYCOMMAND|flag);

          if (screenSize==4)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDC_Quadruple,
                         MF_BYCOMMAND|flag);

          if (screenSize==5)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDC_Quintuple,
                         MF_BYCOMMAND|flag);

          if (screenSize==6)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDC_Sextuple,
                         MF_BYCOMMAND|flag);

          
          if (DM_rules)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_DMRULES,
                         MF_BYCOMMAND|flag);

          flag = MF_GRAYED;
          if (BeginRecordOK) flag = MF_ENABLED;
          EnableMenuItem((HMENU)wParam,
                         IDC_Playback,
                         MF_BYCOMMAND | flag);
          flag = MF_GRAYED;
          if (ItemsRemainingOK
              && (encipheredDataFile==NULL)
              && !simpleEncipher) flag = MF_ENABLED;
          EnableMenuItem((HMENU)wParam,
                         IDC_ItemsRemaining,
                         MF_BYCOMMAND | flag);
          flag = MF_GRAYED;
          if (ItemsRemainingOK
              && (encipheredDataFile==NULL)
              && !simpleEncipher) flag = MF_ENABLED;
          EnableMenuItem((HMENU)wParam,
                         IDC_NonCSBItemsRemaining,
                         MF_BYCOMMAND | flag);
          flag = MF_UNCHECKED;
          if (PlayfileIsOpen()) flag = MF_CHECKED;
          CheckMenuItem((HMENU)wParam,
                        IDC_Playback,
                        MF_BYCOMMAND | flag);

          flag = MF_GRAYED;
          if (BeginRecordOK) flag = MF_ENABLED;
          EnableMenuItem((HMENU)wParam,
                         IDC_Record,
                         MF_BYCOMMAND | flag);
          flag = MF_UNCHECKED;
          if (RecordMenuOption) flag = MF_CHECKED;
          CheckMenuItem((HMENU)wParam,
                        IDC_Record,
                        MF_BYCOMMAND | flag);
          flag = MF_GRAYED;
          if (PlayfileIsOpen()) flag = MF_ENABLED;
          EnableMenuItem((HMENU)wParam,
                         IDC_QuickPlay,
                         MF_BYCOMMAND | flag);
          flag = MF_UNCHECKED;
          if (NoSpeedLimit!=0) flag = MF_CHECKED;
          CheckMenuItem((HMENU)wParam,
                        IDC_QuickPlay,
                        MF_BYCOMMAND | flag);

        };
        return 0;
      case 2: //Speed Menu
        {
          i32 flag;
          if (gameSpeed==SPEED_GLACIAL)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_Glacial,
                         MF_BYCOMMAND|flag);

          if (gameSpeed==SPEED_MOLASSES)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_Molasses,
                         MF_BYCOMMAND|flag);

          if (gameSpeed==SPEED_VERYSLOW)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_VerySlow,
                         MF_BYCOMMAND|flag);

          if (gameSpeed==SPEED_SLOW)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_Slow,
                         MF_BYCOMMAND|flag);

          if (gameSpeed==SPEED_NORMAL)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_Normal,
                         MF_BYCOMMAND|flag);

          if (gameSpeed==SPEED_QUICK)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_Quick,
                         MF_BYCOMMAND|flag);

          if (gameSpeed==SPEED_FAST)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_Fast,
                         MF_BYCOMMAND|flag);

          if (playerClock)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_PlayerClock,
                         MF_BYCOMMAND|flag);

          if (extraTicks)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_ExtraTicks,
                         MF_BYCOMMAND|flag);
        };
      case 3: //Volume Menu
        {
          i32 flag;

          if (gameVolume == VOLUME_FULL)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem((HMENU)wParam,
            IDM_VOLUME_FULL,
            MF_BYCOMMAND | flag);

          if (gameVolume == VOLUME_HALF)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem((HMENU)wParam,
            IDM_VOLUME_HALF,
            MF_BYCOMMAND | flag);

          if (gameVolume == VOLUME_QUARTER)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem((HMENU)wParam,
            IDM_VOLUME_QUARTER,
            MF_BYCOMMAND | flag);

          if (gameVolume == VOLUME_EIGHTH)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem((HMENU)wParam,
            IDM_VOLUME_EIGHTH,
            MF_BYCOMMAND | flag);

          if (gameVolume == VOLUME_OFF)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem((HMENU)wParam,
            IDM_VOLUME_OFF,
            MF_BYCOMMAND | flag);

          flag = MF_UNCHECKED;
          if (usingDirectX) flag = MF_CHECKED;
          CheckMenuItem((HMENU)wParam,
            IDM_AttemptDirectX,
            MF_BYCOMMAND | flag);
          };
        break;
      case 4: // Tracing Menu
        {
          i32 flag;

          if (   (encipheredDataFile == NULL) 
              && !simpleEncipher ) flag = MF_ENABLED;
          else flag = MF_GRAYED;
          EnableMenuItem((HMENU)wParam,
                         IDC_AttackTrace,
                         MF_BYCOMMAND|flag);

          if (   (encipheredDataFile == NULL) 
              && !simpleEncipher ) flag = MF_ENABLED;
          else flag = MF_GRAYED;
          EnableMenuItem((HMENU)wParam,
                         IDC_AITrace,
                         MF_BYCOMMAND|flag);

          if (   (encipheredDataFile == NULL) 
              && !simpleEncipher ) flag = MF_ENABLED;
          else flag = MF_GRAYED;
          EnableMenuItem((HMENU)wParam,
                         IDC_TimerTrace,
                         MF_BYCOMMAND|flag);

          if (TimerTraceActive)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDC_TimerTrace,
                         MF_BYCOMMAND|flag);

          if (traceViewportDrawing != 0)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDM_GraphicTrace,
                         MF_BYCOMMAND|flag);

          if (AttackTraceActive)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDC_AttackTrace,
                         MF_BYCOMMAND|flag);

#ifdef _MSVC_INTEL
          if (DSAIndex.AnyTraceActive()) flag = MF_CHECKED;
#else
          if (DSATraceActive)flag = MF_CHECKED;
#endif
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDC_DSATrace,
                         MF_BYCOMMAND|flag);

          if (AITraceActive)flag = MF_CHECKED;
          else flag = MF_UNCHECKED;
          CheckMenuItem ((HMENU)wParam,
                         IDC_AITrace,
                         MF_BYCOMMAND|flag);

          if (   (encipheredDataFile == NULL) 
              && !simpleEncipher ) flag = MF_ENABLED;
          else flag = MF_GRAYED;
          EnableMenuItem((HMENU)wParam,
                         IDC_DispatchTrace,
                         MF_BYCOMMAND|flag);

          if (   (encipheredDataFile == NULL) 
              && !simpleEncipher ) flag = MF_ENABLED;
          else flag = MF_GRAYED;
          EnableMenuItem((HMENU)wParam,
                         IDC_DispatchTrace,
                         MF_BYCOMMAND|flag);

        };
      case 5: 
        break;//Help menu
      };
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_ENTERIDLE:
      MTRACE("WM_ENTERIDLE\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_CAPTURECHANGED:
      MTRACE("WM_CAPTURECHANGED\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_EXITMENULOOP:
      MTRACE("WM_EXITMENULOOP\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_NCLBUTTONDOWN:
      MTRACE("WM_NCLBUTTONDOWN\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_CLOSE:
      MTRACE("WM_CLOSE\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_SIZING:
      MTRACE("WM_SIZING\n");
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_ENTERSIZEMOVE:
      MTRACE("WM_ENTERSIZEMOVE\n");
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_MOUSEACTIVATE:
      MTRACE("WM_MOUSEACTIVATE\n");
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_LBUTTONDOWN:
    {
      POINT mouse{(short)LOWORD(lParam), (short)HIWORD(lParam)};
      POINT point=MapPoint(g_rcAtari, g_rcClient, mouse);

      csbMessage.type=UIM_LEFT_BUTTON_DOWN;
      csbMessage.p1 = point.x;  // horizontal position of cursor 
      csbMessage.p2 = point.y;  // vertical position of cursor 
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
         PostQuitMessage(0);
      break;
    }
    case WM_LBUTTONUP:
    {
      POINT mouse{(short)LOWORD(lParam), (short)HIWORD(lParam)};
      POINT point=MapPoint(g_rcAtari, g_rcClient, mouse);

      csbMessage.type=UIM_LEFT_BUTTON_UP;
      csbMessage.p1 = point.x;  // horizontal position of cursor 
      csbMessage.p2 = point.y;  // vertical position of cursor 
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
        PostQuitMessage(0);
      break;
    }
    case WM_RBUTTONDOWN:
    {
      POINT mouse{(short)LOWORD(lParam), (short)HIWORD(lParam)};
      POINT point=MapPoint(g_rcAtari, g_rcClient, mouse);

      csbMessage.type=UIM_RIGHT_BUTTON_DOWN;
      csbMessage.p1 = LOWORD(lParam);  // horizontal position of cursor 
      csbMessage.p2 = HIWORD(lParam);  // vertical position of cursor 
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
         PostQuitMessage(0);
      break;
    }
    case WM_RBUTTONUP:
    {
      POINT mouse{(short)LOWORD(lParam), (short)HIWORD(lParam)};
      POINT point=MapPoint(g_rcAtari, g_rcClient, mouse);

      csbMessage.type=UIM_RIGHT_BUTTON_UP;
      csbMessage.p1 = point.x;  // horizontal position of cursor 
      csbMessage.p2 = point.y;  // vertical position of cursor 
      if (CSBUI(&csbMessage) != UI_STATUS_NORMAL)
        PostQuitMessage(0);
      break;
   }
    case WM_EXITSIZEMOVE:
      MTRACE("WM_EXITSIZEMOVE\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_MOVING:
      MTRACE("WM_MOVING\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_SYNCPAINT:
      MTRACE("WM_SYNCPAINT\n"); 
			return DefWindowProc(hWnd, message, wParam, lParam);
		default:
      if (trace >= 0)
      {
        char text[30];
        sprintf(text,"0x%02x\n",message);
        MTRACE(text);
      };
			return DefWindowProc(hWnd, message, wParam, lParam);
   };
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, 
                       UINT message, 
                       WPARAM wParam, 
                       LPARAM /*lParam*/)
{
	switch (message)
	{
		case WM_INITDIALOG:
      {
        SetDlgItemTextA(hDlg, IDC_Version, szCSBVersion);
      };
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
