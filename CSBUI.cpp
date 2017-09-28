#include "stdafx.h"
#include "resource.h"

#ifdef _MSVC_INTEL //001
#pragma warning(disable:4201) //Allow nameless struct
#include <mmsystem.h>
#pragma warning(default:4201)
#endif //001

//#define WIN32_SOUNDMIXER

#include <stdio.h>

//typedef DWORD *DWORD_PTR;  // Needed by dsound.h

#if defined(_MSC_VER) //002
#include <dsound.h>
#include <io.h>
#include <FCNTL.h>
#include <SYS\STAT.h>

#else
// ROQUEN: Not reasonable
#include <unistd.h>
#include <string.h>
#endif //002

#include "UI.h"
//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

extern i32 updateScreenAreaEnterCount;
extern i32 updateScreenAreaLeaveCount;
extern i32 dsaFilterEnterCount;
extern i32 dsaFilterLeaveCount;


void DSAInstrumentation_Dump();
void ReadTranslationFile();
void RecordFile_Record(const char *line);

struct SNDHEAD
{
  i8  byte0[4];           //"RIFF"
  i32 Size;               //  6076  // #bytes after this integer
  i8  byte8[8];           //"WAVEfmt "
  i32 int16;              //    18
  i16 wFormatTag;         //     1
  i16 nChannels;          //     1
  i32 nSamplesPerSecond;  // 11025
  i32 nAvgBytesPerSec;    // 11025 // important
  i16 nBlockAlign;        //     1 // 2 for 16-bit
  i16 wBitsPerSample;     //     8
  //i16 cbSize;             //    40
  //i8  byte38[4];          // "fact"
  //i32 int42;              //     4
  //i32 numBytes46;         //
  i8  byte50[4];          // "data"
  i32 numSamples54;       //
  i8  sample58[1];
};


void Statistics();
void EnqueMouseClick(i32, i32, i32);
void  TAG001afe(i32, i32, i32);
//i32 AddSD(char *, i32, float, float, float);
void ItemsRemaining(i32 mode);
std::string parentFolder(char *folderName, char *endName);
bool PlayfileIsOpen();
bool GetVideoRectangle(i32, RECT *);
ui32 dumpWindow = 0;
bool annotationPlaced = false;
extern RECT g_rcClient;

#if defined(_MSC_VER) //003
WINDOWPLACEMENT annotationPlacement;
extern HINSTANCE hInst;
extern HWND hWnd;
#endif //003


extern i32 WindowHeight;
extern i32 WindowWidth;
extern i32 WindowX;
extern i32 WindowY;
extern bool fullscreenRequested;
extern bool virtualFullscreen;
//extern HINSTANCE hInst;

extern i32 VBLperTimer;
extern i32 VBLMultiplier;
extern i32 screenSize;
extern i32 keyboardMode;
extern i32 trace;
void PlayfileOpen(bool);
void RecordfileOpen(bool);
void RecordfilePreOpen();
extern bool BeginRecordOK;
extern bool TimerTraceActive;
extern bool AttackTraceActive;
extern bool RepeatGame;
extern i32 NoSpeedLimit;
bool OpenTraceFile();
void CloseTraceFile();
bool OpenGraphicTraceFile();
void CloseGraphicTraceFile();
extern bool g_cursorIsShowing;
extern bool RecordCommandOption;
extern bool NoRecordCommandOption;

extern HWND hWnd;

#define maxMessageStack 20
CSB_UI_MESSAGE msgStack[maxMessageStack];
i32 msgStackLen = 0;
i32 messageMask = 1; //Right now just 0 or 1

i32 latestScanp1 = 0;
i32 latestScanp2 = 0;
XLATETYPE latestScanType = TYPEIGNORED;
i32 latestScanXlate = 0;
XLATETYPE latestCharType = TYPEIGNORED;
i32 latestCharp1 = 0;
i32 latestCharXlate = 0;
char playFileName[80] = "Playfile.log";
char *PlayfileName = playFileName;
char dungeonname[80] = "dungeon.dat";
char *dungeonName = dungeonname;
bool PlaybackCommandOption = false;
bool RecordCommandOption = false;
bool NoRecordCommandOption = false;
bool RecordDesignOption = false;
bool DMRulesDesignOption = false;
bool RecordMenuOption = false;
bool NoRecordMenuOption = false;
bool extendedPortraits = false;

i32 keyQueueStart=0; // Next key to process
i32 keyQueueEnd=0;   // Next Empty entry
i32 keyQueueLen=3;
i32 keyQueue[3];

#ifdef _MSVC_INTEL //004
DSAListDialog::DSAListDialog()
{
  //m_initialText = NULL;
  //m_finalText = NULL;
}

DSAListDialog::~DSAListDialog()
{
}



LRESULT CALLBACK DSAListCallback(
                       HWND hDlg, 
                       UINT message, 
                       WPARAM wParam, 
                       LPARAM /*lParam*/)
{
//  i32 len;
	switch (message)
	{
    case WM_DESTROY:
    case WM_SETFONT:
    case WM_NCDESTROY:
    case WM_ACTIVATE:
    case WM_NOTIFYFORMAT:
    case 0x127:
    case 0x128:
    case 0x129:
    case WM_WINDOWPOSCHANGING:
    case WM_WINDOWPOSCHANGED:
    case WM_NCACTIVATE:
    case WM_USER:
    case WM_SHOWWINDOW:
    case WM_NCPAINT:
    case WM_GETTEXT:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORBTN:
    case WM_ERASEBKGND:
    case WM_PAINT:
    case WM_NOTIFY:
    case WM_SETCURSOR:
    case WM_NCHITTEST:
    case WM_MOUSEMOVE:
    case WM_NCMOUSEMOVE:
    case WM_MOUSEACTIVATE:
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
      return false;

		case WM_INITDIALOG:
      {
        HWND  hList;
        int i, idx;
        hList = GetDlgItem(hDlg, IDC_DSAList);
        //if (initialEditText == NULL)
        //initialEditText = "No Game Information available";
        //SetDlgItemText(hDlg, IDC_GameInformationEdit, initialEditText);
        idx = SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)"NONE");
        SendMessage(hList, LB_SETITEMDATA, idx, -1);
        idx = SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)"ALL");
        SendMessage(hList, LB_SETITEMDATA, idx, 256);
        if (DSAIndex.IsLoaded())
        {
          bool all = true;
          for (i=0; i<256; i++)
          {
            DSA *pDSA;
            if ((pDSA=DSAIndex.GetDSA(i)) != NULL)
            {
              char line[200];
              sprintf(line,"%03d - %s", i, pDSA->Description());
              idx = SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)line);
              SendMessage(hList, LB_SETITEMDATA, (WPARAM)idx, (LPARAM)i);
              if (DSAIndex.IsTracing(i))
              {
                SendMessage(hList,LB_SETSEL, (WPARAM)true, (LPARAM)idx);
              }
              else
              {
                all = false;
              };
            };
          };
          if (all)
          {
            SendMessage(hList, LB_SETSEL, (WPARAM)true, LPARAM(1));
          };
        }
        else
        {
          int override;
          override = DSAIndex.TraceOverride();
          if (override == 256)
          {
            SendMessage(hList, LB_SELITEMRANGE, (WPARAM)1, MAKELPARAM(1,1));                 
          }
          else
          {
            DSAIndex.TraceOverride(-1);
            SendMessage(hList, LB_SELITEMRANGE, (WPARAM)1, MAKELPARAM(0,0));                 
          };
        };
        SetFocus(hList);
      };
      DSAIndex.SaveTracing();
		  return TRUE;

		case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      case IDOK:
      case IDCANCEL:
			  {
          //HWND hEdit = GetDlgItem(hDlg, IDC_DSAList);
          //len = GetWindowTextLength(hEdit);
          //if (finalEditText == NULL)
          //{
          //  finalEditText = (char *)UI_malloc(len+1, MALLOC011);
          //};
          //GetWindowText(hEdit, finalEditText, len+1);
				  EndDialog(hDlg, LOWORD(wParam));
				  return 0; // Processed
        };
      case IDC_DSAList:
        {
          i32 focus;
          HWND  hList;
          hList = GetDlgItem(hDlg, IDC_DSAList);
          switch(HIWORD(wParam))
          {
          case LBN_SETFOCUS:
          case LBN_KILLFOCUS:
            return 1; //Not Processed.
          case LBN_SELCHANGE:
            focus = SendMessage(hList, LB_GETCARETINDEX, (WPARAM)0, (LPARAM)0);
            if (DSAIndex.IsLoaded())
            {
              i32 traceNums[258];
              i32 numSel;
              i32 i;
              numSel = SendMessage(hList, LB_GETSELITEMS, (WPARAM)258, (LPARAM)traceNums);
              if (focus == 0)
              {
                DSAIndex.NoTracing();
                SendMessage(hList, LB_SETSEL, (WPARAM)false, (LPARAM)-1);
                SendMessage(hList, LB_SETSEL, (WPARAM)true, (LPARAM)0);
                break;
              }
              if (focus == 1)
              {
                DSAIndex.AllTracing();
                SendMessage(hList, LB_SETSEL, (WPARAM)true, (LPARAM)-1);
                SendMessage(hList, LB_SETSEL, (WPARAM)false, (LPARAM)0);
                SendMessage(hList, LB_SETSEL, (WPARAM)true, (LPARAM)1);
                break;
              }
              SendMessage(hList, LB_SETSEL, (WPARAM)false, (LPARAM)0);
              SendMessage(hList, LB_SETSEL, (WPARAM)false, (LPARAM)1);
              DSAIndex.NoTracing();
              for (i=0; i<numSel; i++)
              {
                i32 dsaNum;
                dsaNum = SendMessage(hList, LB_GETITEMDATA, (WPARAM)traceNums[i], (LPARAM)0);
                DSAIndex.SetTracing(dsaNum);
              };
            }
            else
            {
              if (focus == 0)
              {
                DSAIndex.TraceOverride(-1);
                DSAIndex.NoTracing();
                SendMessage(hList, LB_SETSEL, (WPARAM)false, (LPARAM)1);
                SendMessage(hList, LB_SETSEL, (WPARAM)true, (LPARAM)0);
                break;
              }
              if (focus == 1)
              {
                DSAIndex.TraceOverride(256);
                DSAIndex.AllTracing();
                SendMessage(hList, LB_SETSEL, (WPARAM)false, (LPARAM)0);
                SendMessage(hList, LB_SETSEL, (WPARAM)true, (LPARAM)1);
                break;
              }

            };
            return 0;
          default:
            return 1;
          };
        };
        break;
      default:
        return 1; // Not Processed.
			}
			return 1;  // WM_COMMAND not processed.
	}
  return FALSE;
}





i32 DSAListDialog::DoModal()
{
  i32 result;
  bool saveCursorShowing;
  //initialEditText = m_initialText;
  saveCursorShowing = g_cursorIsShowing;
  if (!g_cursorIsShowing) ShowCursor(true);
  i32 mask = UI_DisableAllMessages();
  result = DialogBox(hInst, (LPCTSTR)IDD_TraceDSAList, hWnd, (DLGPROC)DSAListCallback);
//  if (result < 0)
//  {
//    DWORD errorWord = GetLastError();
//  };
  if (!saveCursorShowing) ShowCursor(false);
  UI_EnableMessages(mask);
  return result;
}


#endif //004


EditDialog::EditDialog()
{
  m_initialText = NULL;
  m_finalText = NULL;
}

EditDialog::~EditDialog()
{
}


static char *initialEditText;
char *finalEditText= NULL;
// Mesage handler for EditDialog

#ifdef _MSVC_INTEL //005
LRESULT CALLBACK EditTextCallback(
                       HWND hDlg, 
                       UINT message, 
                       WPARAM wParam, 
                       LPARAM /*lParam*/)
{
  i32 len;
	switch (message)
	{
		case WM_INITDIALOG:
    {
      if (initialEditText == NULL)
      initialEditText = "No Game Information available";
      SetDlgItemTextA(hDlg, IDC_GameInformationEdit, initialEditText);
 
      if (annotationPlaced)
      {
        SetWindowPlacement(hDlg, &annotationPlacement);
      };

      
//BOOL SetWindowPos(
//  HWND hWnd,             // handle to window
//  HWND hWndInsertAfter,  // placement-order handle
//  int X,                 // horizontal position
//  int Y,                 // vertical position
//  int cx,                // width
//  int cy,                // height
//  UINT uFlags            // window-positioning flags
//);
       
      
      
      };
		  return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
        HWND hEdit = GetDlgItem(hDlg, IDC_GameInformationEdit);
        len = GetWindowTextLength(hEdit);
        if (finalEditText == NULL)
        {
          finalEditText = (char *)UI_malloc(len+1, MALLOC011);
        };
        GetWindowTextA(hEdit, finalEditText, len+1);
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			return FALSE;
    case WM_SETFONT:
    case WM_ACTIVATE:
    case WM_WINDOWPOSCHANGING:
    case WM_WINDOWPOSCHANGED:
    case WM_NCACTIVATE:
    case WM_USER:
    case 0x0127:
    case 0x0128:
    case WM_SHOWWINDOW:
    case WM_NCPAINT:
    case WM_GETTEXT:
    case WM_ERASEBKGND:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLOREDIT:
    case WM_PAINT:
    case WM_SETCURSOR:
    case WM_NCHITTEST:
    case WM_MOUSEFIRST:
    case WM_MOUSEACTIVATE:
    case WM_NCMOUSEMOVE:
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
    case WM_NCDESTROY:
    case WM_NCLBUTTONDOWN:
    case WM_SYSCOMMAND:
    case WM_CAPTURECHANGED:
    case WM_GETMINMAXINFO:
    case WM_ENTERSIZEMOVE:
    case WM_MOVING:
    case WM_MOVE:
    case WM_EXITSIZEMOVE:
    case WM_TIMECHANGE:
    case WM_ACTIVATEAPP:
    case WM_ENTERMENULOOP:
      return FALSE;
    case WM_DESTROY:
      {
        if (GetWindowPlacement(
                hDlg,               // handle to window
                &annotationPlacement)) // address of structure for position data
        {
          annotationPlaced = true;
        };
      };
 

      return FALSE;
    default:
      return FALSE;
	};
}


i32 EditDialog::DoModal()
{
  i32 result;
  bool saveCursorShowing;
  initialEditText = m_initialText;
  saveCursorShowing = g_cursorIsShowing;
  if (!g_cursorIsShowing) ShowCursor(true);
  i32 mask = UI_DisableAllMessages();
  result = DialogBox(hInst, (LPCTSTR)IDD_GameInformation, hWnd, (DLGPROC)EditTextCallback);
  if (!saveCursorShowing) ShowCursor(false);
  UI_EnableMessages(mask);
  m_finalText = finalEditText;
  //if (finalEditText != NULL) UI_free(finalEditText);
  finalEditText = NULL;
  initialEditText = NULL;
  return result;
}
#endif //005

struct KEYXLATE 
{
   struct Key
   {
      i32 m_scan, m_mode;
      XLATETYPE m_type;
      bool operator==(const Key &key) const { return m_scan==key.m_scan && m_mode==key.m_mode && m_type==key.m_type; }
   };

  void addkey(i32 scan, i32 key, i32 mode, XLATETYPE type);
  i32 translate(i32 scan, i32 mode, XLATETYPE type);

private:

   struct Map
   {
      Key m_key;
      i32 m_result;
   };

   std::vector<Map> m_map;
};

void KEYXLATE::addkey(i32 scan, i32 key, i32 mode, XLATETYPE type)
{
   m_map.emplace_back(Map{Key{scan, mode, type}, key});
}

i32 KEYXLATE::translate(i32 scan, i32 mode, XLATETYPE type)
{
   Key key{scan, mode, type};
   for(auto &entry : m_map)
      if(entry.m_key==key)
         return entry.m_result;
   return 0;
}

KEYXLATE keyxlate;

char *getfield(const char *buf, i32& col)
{
  static char result[200];
  char term;
  i32 len = 0;
  term = ' ';
  while (buf[col] != '\n')
  {
    if ((buf[col]!=' ') && (buf[col]!='\t')) break;
    col++;
  };
  if (buf[col] == '"')
  {
    term = '"';
    col++;
  };
  while (buf[col] != '\n')
  {
    if (   (buf[col]==term) 
        || ((term==' ') && (buf[col]=='\t'))) break;
    result[len++] = buf[col];
    if (len>198) len--;
    col++;
  };
  result[len]=0;
  return result;
}

i32 gethex(char *buf, i32& col)
{
  i32 result;
  char *field;
  field = getfield(buf,col);
  sscanf(field,"%x", &result);
  return result;
}


// ROQUEN: massive temp hack
#ifndef _MSC_VER //006

#ifdef  TARGET_OS_MAC //007
#define strupr( a ) _strupr( a )
static void _strupr(char *str) {
    if( str ) {
	while(*str) {
	    *str = toupper( *str );
	    str++;
	}
    }
}
#else
#define _strupr(X) SDL_strupr(X)
#define strupr(X)  SDL_strupr(X)
#endif //007

#endif  //006

/*
i32 AddSmartDiscard(const char *buf)
{
  char fld[10][200];
  i32 i, j, col=0;
  i32 minimum;
  float level,y_intercept,slope;
  char *pfld;
  for (i=0; i<10; i++)
  {
    pfld=getfield(buf,col);
    if (*pfld == 0) break;
    _strupr(pfld);
    strcpy(fld[i],pfld);
  };
  j=1;
  while (i>5)
  {
    if ((fld[j][0]>='A') && (fld[j][0]<='Z'))
    {
      strcat(fld[0]," ");
      strcat(fld[0],fld[j]);
      j++;
      i--;
    }
    else break;
  };
  if (i<5) return 1; //error
  if (sscanf(fld[j],"%d",&minimum)!=1) return 1;
  if (sscanf(fld[j+1],"%f",&level)!=1) return 1;
  if (sscanf(fld[j+2],"%f",&y_intercept)!=1) return 1;
  if (sscanf(fld[j+3],"%f",&slope)!=1) return 1;
  return AddSD(fld[0],minimum,level,y_intercept,slope);
  //return 0;
}
*/
/*

//************************************************************************
// Smart Discard definitions
//
//Smart Discard only operates on databases which have been enlarged
// to size 1023.  Each item in a database is given a discard priority
// which is compared to other items in the same database.  The item with
// the highest priority is discarded first.
//
// The program operates as follows:  every clock tick initiates the
// "Smart Discard" process.  The databases are considered one at a time.
// Each clock tick causes the items in a single room to be evaluated and
// added to the list.  When all rooms have been evaluated and there are
// fewer than 900 items in the database then the next database is considered.
// If more than 900 items exist then on each tick of the clock the item
// with the highest discard priority has its priority recomputed.  If it
// is still at least as great as the next higher item then it is deleted.
// On the other hand, if its priority has declined (perhaps due to party
// movement, because we just discarded an item of this type, etc) then it 
// is put back in the list in its proper place and we wait for the next 
// clock tick.
//
// The priority calculation includes a random number between 0.0 and 1.0
// so that items with the same basic priority will be randomly selected.
//
// Format:
//
//   name  minimum  level  y-intercept  slope
//
//     name         item name - blanks are allowed  (or "default") (case-insensitive)
//     minimum      integer - If this many or fewer of the item exist then
//                            none will be considered for discard.
//     level        float - additional priority for not being on a level 
//                          adjacent to the party.
//     y-intercept  float - discard priority for minimum
//     slope        float - additional discard priority for each item over minimum
//
// The item's deletion priority is  y-intercept + slope*(num-minimum) + level
//     
// Let us provide an example:
//
// example   -->  Discard   Worm Round  20  10.0  0  1
//
// Worm Rounds are not considered for deletion unless there are more than
// 20 of them in the dungeon.  Assume there are 30 of them in the dungeon.
// The discard priority for each of the 30 Worm Rounds is equal to:
//         0 + 1*(30-20).
// Each Worm Round which is on a level not immediately adjacent to the party
// will have 10.0 added to its priority.
//
//
const char *SmartDiscards[] = {
"default       30  50  20.0   1.0",
"Wooden Shield 10  50  20.0   1.0", // We hardly need more than 10 of these 
"Rock          10  50  20.0   1.0",
"Sword         10  50  20.0   1.0",
"Dagger        10  50  20.0   1.0",
"Falchion      10  50  20.0   1.0",
"Club          10  50  20.0   1.0",
"Stone Club    10  50  20.0   1.0",
"Armet         10  50  20.0   1.0",
"Torso Plate   10  50  20.0   1.0",
"Leg Plate     10  50  20.0   1.0",
"Foot Plate    10  50  20.0   1.0",
//; Miscellaneous items
"Boulder       20  50  70.0   1.0", // Be sure Boulders go before food
"Dragon Steak 150   0  20.0   0.0", // Default goes before Dragon Steak.
 NULL
};
*/

// ROQUEN: humm...
#if !defined(_LINUX) //008
#define CONFIG_NAME "config.txt"
#else
#define CONFIG_NAME "config.linux"
#endif //008

#if defined TARGET_OS_MAC //009
#define strupr( a ) _strupr( a )
static void _strupr(char *str) {
    if( str ) {
	while(*str) {
	    *str = toupper( *str );
	    str++;
	}
    }
}
#endif //009
void ReadConfigFile()
{
  char buf[501];
  i16 f;
  i32 lineNumber = 0;
  i32 col, numerror=0;
  char *field;
  i32 scancode, key, mode;

  /*
  for (i32 i=0; SmartDiscards[i] != NULL; i++)
  {
    //_strupr(SmartDiscards[i]);
    if (AddSmartDiscard(SmartDiscards[i]) == 0) continue;
  };
  */
  f=OPEN(CONFIG_NAME, "r");
  if (f < 0) return;
  while (GETS(buf,500,f) != NULL)
  {
    lineNumber++;
    col=0;
    if (buf[0] == ';') continue;
    field=getfield(buf,col);
    _strupr(field);
    if (strcmp(field, ";")==0) continue;
    if (strcmp(field, "") ==0) continue;
    if (strcmp(field, "KEY")==0)
    {
      mode    =gethex(buf,col);
      scancode=gethex(buf,col);
      key     =gethex(buf,col);
      keyxlate.addkey(scancode, key, mode, TYPEKEY);
      continue;
    };
    if (strcmp(field,"SCAN")==0)
    {
      mode    =gethex(buf,col);
      scancode=gethex(buf,col);
      key     =gethex(buf,col);
      keyxlate.addkey(scancode, key, mode, TYPESCAN);
      continue;
    };
    if (strcmp(field,"MSCAN")==0)
    {
      i32 x, y;
      XLATETYPE type;
      char *button;
      mode    =gethex(buf,col);
      scancode=gethex(buf,col);
      x       =gethex(buf,col);
      y       =gethex(buf,col);
      button  =getfield(buf,col);
      _strupr(button);
      if (strcmp(button,"L")==0) type=TYPEMSCANL; else type=TYPEMSCANR;
      keyxlate.addkey(scancode, (x<<16)+y, mode, type);
      continue;
    };
    if (strcmp(field,"DIRECTORY")==0)
    {
      field = getfield(buf,col);
      if (g_folderName != NULL) UI_free(g_folderName);
      g_folderName = (char *) UI_malloc(strlen(field) + 1, MALLOC012);
      if (g_folderName!=NULL)
      {
        strcpy(g_folderName, field);
        g_folderParentName = parentFolder(g_folderName, g_folderName+strlen(g_folderName+1));
      };
      continue;
    };

#if defined(_LINUX) //010
extern ui32 TImER;
     if (strcmp(field,"TIMER")==0)
    {
      field = getfield(buf,col);
      if (TImER) continue; //cmd line has higher priority than config.linux
      //sscanf(TIMER,"%d", field);
      TImER=strtol( field,NULL, 10);
      continue;
    };
#endif //010

    if (strcmp(field,"PLACE") == 0)
    {
      i32 k, size, srcx, srcy, width, height, x, y;
      field = getfield(buf,col);
      if ( (field[0] < 'A')||(field[0] > 'E') ) continue;
      k = field[0] - 'A';
      size = gethex(buf,col);
      if ( (size < 0)||(size > 6) ) continue;
      srcx = gethex(buf,col);
      if ( (srcx<0) || (srcx>319) ) continue;
      srcy = gethex(buf,col);
      if ( (srcy<0) || (srcy>199) ) continue;
      width = gethex(buf,col);
      if ( (width<1) || (srcx+width>320) )continue;
      height = gethex(buf,col);
      if ( (height<1) || (srcy+height>200) )continue;
      x = gethex(buf,col);
      if ( (x < 0)||(x > 4096) ) continue;
      y = gethex(buf,col);
      if ( (y < 0)||(y >4096) ) continue;
      videoSegSize[k] = size;
      videoSegSrcX[k] = srcx;
      videoSegSrcY[k] = srcy;
      videoSegWidth[k] = width;
      videoSegHeight[k] = height;
      videoSegX[k] = x;
      videoSegY[k] = y;
      continue;
    };
    if (numerror==0)
    {
      char msg[200];
      if (strlen(buf)>0)
        if (buf[strlen(buf)-1] == '\n')
          buf[strlen(buf)-1] = 0;
      buf[100] = 0; //Just in case it is very long;
      sprintf(msg,"Line number = %d\n%s",lineNumber,buf);
      UI_MessageBox(msg,"Bad Config Line",MESSAGE_OK);
      numerror++;
    };
  };
  CLOSE(f);
}



i32 UI_DisableAllMessages()
{
  i32 oldmask = messageMask;
  messageMask = 0;
  return oldmask;
}

i32 UI_EnableMessages(i32 mask)
{
  messageMask |= mask;
  return messageMask;
}



i32 CSBUI(CSB_UI_MESSAGE *msg)
{
  static bool crash = false;
  i32 CallCheckVBL= 0;
  if (messageMask == 0) return UI_STATUS_NORMAL;
//  {
//    FILE *f;
//    f = fopen("debug","a");
//    fprintf(f,"CSBUI enter type = %d", msg->type);
//    fclose(f);
//  };
  try
  {
    switch (msg->type)
    {
    case UIM_INITIALIZE:
        ReadConfigFile();
        //Set the GameMode //0=quit
                           //1=dungeon
                           //2=utility
                           //3=hint
        ReadTranslationFile();
        DispatchCSB(st_AskWhatToDo);
        break;
    case UIM_SETOPTION:
      switch (msg->p1)
      {
      case OPT_DMRULES:
        DM_rules = !DM_rules;
        RecordFile_Record(DM_rules ? "#DMRules\n" : "#NoDMRules\n");
        break;
      case OPT_NORMAL:
        UI_ClearScreen();
        screenSize = 1;
        break;
      case OPT_DOUBLE:
        UI_ClearScreen();
        if (msg->p2 == 1) screenSize = 1;
        else screenSize = 2;
        break;
      case OPT_TRIPLE:
        UI_ClearScreen();
        if (msg->p2 == 1) screenSize = 1;
        else screenSize = 3;
        break;
      case OPT_QUADRUPLE:
        UI_ClearScreen();
        if (msg->p2 == 1) screenSize = 1;
        else screenSize = 4;
        break;
      case OPT_QUINTUPLE:
        UI_ClearScreen();
        if (msg->p2 == 1) screenSize = 1;
        else screenSize = 5;
        break;
      case OPT_SEXTUPLE:
        UI_ClearScreen();
        if (msg->p2 == 1) screenSize = 1;
        else screenSize = 6;
        break;
      case OPT_RECORD:
        if (msg->p2 == 1) 
        {
          RecordMenuOption = true;
          NoRecordMenuOption = false;
        }
        else 
        {
          RecordfileOpen(false); //This will override any other
                                 //Record options.
          RecordMenuOption = false;
          NoRecordMenuOption = true;
        };
        break;
      case OPT_PLAYBACK:
        if (msg->p2 == 1) PlayfileOpen(true);
        else PlayfileOpen(false);
        if (PlayfileIsOpen())
        {
          extraTicks = false;
        };
        break;
      case OPT_QUICKPLAY:
        if (msg->p2 == 1) NoSpeedLimit = 2000000000;
        else 
        {
          NoSpeedLimit = 0;
          VBLMultiplier = 1;
        };
        break;
      case OPT_CLOCK:
        gameSpeed = (SPEEDS)msg->p2;
        break;
      case OPT_VOLUME:
        gameVolume = (VOLUMES)msg->p2;
        break;
      case OPT_DIRECTX:
        usingDirectX = !usingDirectX;
        break;
      case OPT_PLAYERCLOCK:
        playerClock = !playerClock;
        break;
      case OPT_EXTRATICKS:
        extraTicks = !extraTicks;
        break;
      case OPT_ITEMSREMAINING:
        ItemsRemaining(0);
        break;
      case OPT_NONCSBITEMSREMAINING:
        ItemsRemaining(1);
        break;
      case OPT_TIMERTRACE:
        if (TimerTraceActive)
        {
          TimerTraceActive = false;
          if (!AttackTraceActive && !AITraceActive)
          {
            CloseTraceFile();
          };
        }
        else
        {
          if (AttackTraceActive || AITraceActive || OpenTraceFile())
          {
            TimerTraceActive = true;
          };
        };
        break;
      case OPT_DSATRACE:
#ifdef _MSVC_INTEL //011
        {
          DSAListDialog dlg;
          //dlg.m_initialText = gameInfo;
          if (dlg.DoModal() == IDOK)
          {
          //  if (gameInfo!=NULL) UI_free (gameInfo);
          //  gameInfo = dlg.m_finalText;
          }
          else
          {
          //  UI_free (dlg.m_finalText);
          };
        };
#else
        {          
          DSAInstrumentation_Dump();
          DSATraceActive = !DSATraceActive;
        };
#endif //011
        break;
      case OPT_GRAPHICTRACE:
        traceViewportDrawing = 1 - traceViewportDrawing;
        break;
      case OPT_ATTACKTRACE:
        if (AttackTraceActive)
        {
          AttackTraceActive = false;
          if (!TimerTraceActive && !AITraceActive)
          {
            CloseTraceFile();
          };
        }
        else
        {
          if (TimerTraceActive || AITraceActive || OpenTraceFile())
          {
            AttackTraceActive = true;
          };
        };
        break;
      case OPT_AITRACE:
        if (AITraceActive)
        {
          AITraceActive = false;
          if (!TimerTraceActive && !AttackTraceActive)
          {
            CloseTraceFile();
          };
        }
        else
        {
          if (TimerTraceActive || AttackTraceActive || OpenTraceFile())
          {
            AITraceActive = true;
          };
        };
        break;
      default:
        ASSERT(0,"attack");
      };
      break;
    case UIM_TERMINATE:
        return UI_STATUS_TERMINATE; 
    case UIM_CHAR:
        {
          //{
          //  char line[80];
          //  sprintf(line, "p1=%d; lastestCharp1=%d", msg->p1, latestCharp1);
          //  UI_MessageBox(line, "UIM_CHAR", MESSAGE_OK);
          //};
          if ((latestCharp1 == 3) && (msg->p1 == 3))
          {
            UI_Die(0x6d9a);
            break;
          };
          latestCharp1 = msg->p1;
          latestCharType = TYPEIGNORED;
          i32 key = keyxlate.translate(msg->p1, keyboardMode, TYPEKEY);
          if (key != 0)
          {
            if (key == 0x1b) // Escape
            {
              if (PlayfileIsOpen())
              {
                char message[80];
                sprintf(message,"Playback Paused at %d",d.Time);
                UI_MessageBox(message,"Pause",MESSAGE_OK);
                break;
              };
            };
            i32 next = keyQueueEnd+1;
#ifdef NoKeyQue
            if (keyQueueEnd != keyQueueStart) break;
#endif
            latestCharType = TYPEKEY;
            latestCharXlate = key;
            if (next >= keyQueueLen) next=0;
            if (next != keyQueueStart)
            {
              keyQueue[keyQueueEnd]= key;
              keyQueueEnd = next;
            };
          }
        }
        break;
    case UIM_KEYDOWN:
        {
          i32 key;
          latestScanp1 = msg->p1;
          latestScanp2 = msg->p2;
          //printf("CSBUI(UIM_KEYDOWN)@%d\n",(ui32)UI_GetSystemTime());
          if ((key = keyxlate.translate(msg->p2&0xff, 
                                        keyboardMode, 
                                        TYPESCAN))!= 0)
          {
            latestScanType = TYPESCAN;
            latestScanXlate = key;
            //printf("CSBUI->keyQueue\n");
            i32 next = keyQueueEnd+1;
            if (next >= keyQueueLen) next=0;
            if (next != keyQueueStart)
            {
              keyQueue[keyQueueEnd]= key;
              keyQueueEnd = next;
            };
          }
          else
          if ((key = keyxlate.translate(msg->p1, keyboardMode, TYPEMSCANL))!= 0)
          {
            //printf("CSBUI->TYPEMSCANL\n");
            latestScanType = TYPEMSCANL;
            latestScanXlate = key;
            OnMouseSwitchAction(2, key>>16, key & 0xffff);
            OnMouseSwitchAction(0);
          }
          else
          if ((key = keyxlate.translate(msg->p1, keyboardMode, TYPEMSCANR))!= 0)
          {
            latestScanType = TYPEMSCANR;
            //printf("CSBUI->TYPEMSCANR\n");
            latestScanXlate = key;
            OnMouseSwitchAction(1, key>>16, key & 0xffff);
            OnMouseSwitchAction(0);
          }
          else
          {
            latestScanType = TYPEIGNORED;
          };
        }
        break;
    case UIM_TIMER:
        //The UIM_TIMER messages come no more often than
        //20 per second on my machine.  So we can cause multiple
        //VBLs per UIM_TIMER.  Our stategy is to call CheckVBL.
        //Then dispatch until nothing is left to do.  Then
        //call CheckVBL again.  Repeat entire process 10 times
        CallCheckVBL = VBLperTimer*VBLMultiplier; // do it 10 times
        if (trace >= 0) 
        {
          fprintf(GETFILE(trace),"UIM_TIMER\n");
          fflush(GETFILE(trace));
        }
        break;
    case UIM_PAINT:
        display();
        break;
    case UIM_REDRAW_ENTIRE_SCREEN:
        ForceScreenDraw();
        UI_Invalidate();
        break;
    case UIM_LEFT_BUTTON_DOWN:
        OnMouseSwitchAction(0x2);

        if (virtualFullscreen)
        {
          i32 x,y,size;
          size = screenSize;
          TranslateFullscreen(msg->p1,msg->p2,x,y);
          EnqueMouseClick(x, y, 1);
          TAG001afe(x, y, 1);
        }
        else
        {
          EnqueMouseClick(msg->p1, msg->p2,1);//Chaos
          TAG001afe(msg->p1, msg->p2, 1);//Hint
        };
        UI_PushMessage(msg->type);
        break;
    case UIM_LEFT_BUTTON_UP:
        //OnMouseSwitchAction(0x0);
        OnMouseUnClick();
        break;
    case UIM_RIGHT_BUTTON_DOWN:
        OnMouseSwitchAction(0x1);
        if (virtualFullscreen)
        {
          i32 x, y, size;
          size = screenSize;
          TranslateFullscreen(msg->p1,msg->p2,x,y);
          EnqueMouseClick(x, y, 1);
          TAG001afe(x, y, 1); //[Erik]: Is this correct?
        }
        else
        {
          EnqueMouseClick(msg->p1, msg->p2,1);
          TAG001afe(msg->p2, msg->p1, 1); //[Erik]: Is this correct?
        };
        UI_PushMessage(msg->type);
        break;
    case UIM_RIGHT_BUTTON_UP:
        //OnMouseSwitchAction(0);
        OnMouseUnClick();
        break;
    case UIM_Statistics:
      Statistics();
      break;
    case UIM_EditGameInfo:
        {
#ifdef _MSVC_INTEL //012
          EditDialog dlg;
          dlg.m_initialText = gameInfo;
          if (dlg.DoModal() == IDOK)
          {
            if (gameInfo!=NULL) UI_free (gameInfo);
            gameInfo = dlg.m_finalText;
          }
          else
          {
            UI_free (dlg.m_finalText);
          };
#endif //012
        };
      break;
    default:
      ASSERT(0,"message");
    };
    do
    {
      if (CallCheckVBL > 0)
      {
        checkVBL();
        CallCheckVBL--;
      };
      while (msgStackLen != 0)
      {
        msgStackLen--;
        if (msgStack[msgStackLen].type == UIM_TERMINATE)
        { 
//          { 
//            FILE *f;
//            f = fopen("debug","a");
//            fprintf(f," Exit TERMINATE\n");
//            fclose(f);
//          };
          return UI_STATUS_TERMINATE;
        };
        //printf("CSBUI calls DispatchCSB @ %d\n", (ui32)UI_GetSystemTime());
        DispatchCSB(&msgStack[msgStackLen]);
      };
    } while (CallCheckVBL > 0);
//    { 
//      FILE *f;
//      f = fopen("debug","a");
//      fprintf(f," Exit NORMAL\n");
//      fclose(f);
//    };
    return UI_STATUS_NORMAL;
  }
  catch (i32 )
  {
    CloseTraceFile();
//    FILE *f;
//    f = fopen("debug", "a");
//    fprintf(f,"Caught 0x%x\n", v);
//    fclose(f);
    return UI_STATUS_TERMINATE;
  }
  catch(...)
  {
    if (!crash)
    {
//      {
//        FILE *f;
//        f = fopen("debug", "a");
//        fprintf(f,"Caught wild exception\n");
//        fclose(f);
//      };
      char message[200];
      strcpy(message, "Program Crashed.  ");
      if (dsaFilterEnterCount != dsaFilterLeaveCount)
      {
        strcat(message, " DSA Filter active.");
      };
      if (updateScreenAreaEnterCount != updateScreenAreaLeaveCount)
      {
        strcat(message,"  UpdateScreeenArea active");
      };
      CloseTraceFile();
      crash = true;
    };
    return UI_STATUS_TERMINATE;
  }  ;
};


void UI_PushMessage(MTYPE type,
                    i32 p1,i32 p2,i32 p3,i32 p4,i32 p5,i32 p6)
{
  if (msgStackLen >= maxMessageStack-1) UI_Die(0x7db6);
  msgStack[msgStackLen].type = type;
  msgStack[msgStackLen].p1 = p1;
  msgStack[msgStackLen].p2 = p2;
  msgStack[msgStackLen].p3 = p3;
  msgStack[msgStackLen].p4 = p4;
  msgStack[msgStackLen].p5 = p5;
  msgStack[msgStackLen].p6 = p6;
  msgStackLen++;
}
  

#if defined _MSVC_INTEL || defined TARGET_OS_MAC //013
void UI_Invalidate(bool erase)
{
    InvalidateRect(hWnd, NULL, erase);
}
#endif //013  _MSVC_INTEL

i32 UI_DeleteFile(const char *fname)
{
  return (remove(fname)==0) ? 1 : 0;;
}

#ifdef _MSVC_INTEL //014
i32 UI_MessageBox(const char *msg, const char *title, i32 flags)
{
  i32 i;
  i32 MB_FLAGS = MB_APPLMODAL | MB_TOPMOST;
  if (flags & MESSAGE_OK) MB_FLAGS |= MB_OK;
  if (flags & MESSAGE_YESNO) MB_FLAGS |= MB_YESNO;
  if (flags & MESSAGE_ICONERROR) MB_FLAGS |= MB_ICONERROR;
  if (flags & MESSAGE_ICONWARNING) MB_FLAGS |= MB_ICONWARNING;
  bool saveCursorShowing;
  saveCursorShowing = g_cursorIsShowing;
  if (!g_cursorIsShowing) ShowCursor(true);
  i32 mask = UI_DisableAllMessages();
  if (title == NULL) title = "Error";
  i = MessageBoxA(hWnd, msg, title, MB_FLAGS);
  UI_EnableMessages(mask);
  if (!saveCursorShowing) ShowCursor(false);
  if (i == IDYES) return MESSAGE_IDYES;
  if (i == IDNO)  return MESSAGE_IDNO;
  return MESSAGE_IDOK;
}
#endif //014 _MSVC_INTEL





i32 UI_CONSTAT()
{
  if (keyQueueStart != keyQueueEnd) return 1;
  return 0; // No console input available
}

i32 UI_DIRECT_CONIN()
{ //We should wait for a keystroke.  But we don't.  It is
  // up to you to be sure that input exists before
  // calling DIRECT_CONIN.
  ASSERT(keyQueueStart != keyQueueEnd,"keyQue");
  if (keyQueueStart == keyQueueEnd) return -1;
  i32 key = keyQueue[keyQueueStart];
  keyQueueStart++;
  if (keyQueueStart >= keyQueueLen) keyQueueStart=0;
  //printf("UI_DIRECT_CONIN @%d\n",(ui32)UI_GetSystemTime());
  return key;
}

void UI_Die(int v)
{
  throw  v;
}

void UI_Sleep(i32 milliseconds)
{
#ifdef _MSVC_INTEL //015
  if (!NoSleep) Sleep(milliseconds); //Windows sleep function
#endif //015 _MSVC_INTEL
#ifdef TARGET_OS_MAC //016
  Sleep(milliseconds); 
#endif //016
#ifdef xxxx //017
  xxx usleep(milliseconds*1000);
#endif //017
}

#ifdef _MSVC_INTEL //018


#ifdef WIN32_SOUNDMIXER //019
bool Win32_SoundMixer_Init(HWND hWnd);
void Win32_SoundMixer_Play(BYTE *pWave, DWORD dwBytes);
void Win32_SoundMixer_Shutdown();
#endif //019

void UI_StopSound()
{
  sndPlaySound(NULL,SND_SYNC);
}


#ifdef WIN32_SOUNDMIXER //020

class PlayDirectSound
{
  bool m_initialized;
  bool m_failed;
public:
  PlayDirectSound();
  ~PlayDirectSound();
  bool Play(char *wave);
  void Shutdown();
};

PlayDirectSound::PlayDirectSound()
{
  m_initialized = false;
  m_failed = false;
}

PlayDirectSound::~PlayDirectSound()
{
  Shutdown();
}

bool PlayDirectSound::Play(char *wave)
{
  if (!m_initialized)
  {
    if (!m_failed)
    {
      m_initialized = true;
      m_failed = !Win32_SoundMixer_Init(hWnd);
    };
  };
  if (m_failed) return false;
  Win32_SoundMixer_Play((unsigned char *)wave,
                        ((DWORD *)wave)[1]+8);
  return true;
}

void PlayDirectSound::Shutdown()
{
  if (m_initialized & !m_failed)
  {
    m_initialized = false;
    Win32_SoundMixer_Shutdown();
  };
  m_failed = false;
}

PlayDirectSound playDirectSound;

#else


ui8 CLSID_DS[] = 
{
  0x46, 0xD9, 0xD4, 0x47,
  0xE8, 0x62, 0xCF, 0x11,
  0x93, 0xBC, 0x44, 0x45,
  0x53, 0x54, 0x00, 0x00 
};

ui8 IID_DS[] =
{
  0x83, 0xfa, 0x9a, 0x27,
  0x81, 0x49, 0xce, 0x11,
  0xa5, 0x21, 0x00, 0x20,
  0xaf, 0x0b, 0xe5, 0x60
};


struct MYDIRECTSOUND
{
  ~MYDIRECTSOUND();

  bool Initialize(); //Allocate DirectSound object
  void Clean();      //Release completed buffers.
  void Shutdown();   //Release DirectSound object
  bool Play(const BYTE *wave, i32 attenuation); //Play a single sound.

private:
  bool                m_failed{};
  bool                m_comInitialized{};
  IDirectSound       *m_directSound{};
  IDirectSoundBuffer *m_soundBuffers[64];
  i32                 m_numBuffersActive;
};

MYDIRECTSOUND::~MYDIRECTSOUND()
{
  Shutdown();
}

bool MYDIRECTSOUND::Initialize()
{
  HRESULT result;
  WAVEFORMATEX fmt;
  DSBUFFERDESC dbd;
  IDirectSoundBuffer *pSoundBuf;
  if (m_failed) return false;
  if (m_directSound != NULL) return true;
  m_numBuffersActive = 0;
  result = CoInitialize(NULL);
  if (result != S_OK)
  {
    if (result == S_FALSE) CoUninitialize();
    m_failed = true;
    return false;
  };
  m_comInitialized = true;
  result = CoCreateInstance(
              *(_GUID *)CLSID_DS, 
              NULL, 
              CLSCTX_INPROC_SERVER, 
              *(_GUID *)IID_DS, 
              (void **)&m_directSound);
  if (result != S_OK)
  {
    m_directSound = NULL;
    Shutdown();
    m_failed = true;
    return false;
  };
  result = m_directSound->Initialize(NULL);
  if (result != DS_OK)
  {
    Shutdown();
    m_failed = true;
    return false;
  };
  if (m_directSound->SetCooperativeLevel(hWnd,DSSCL_PRIORITY) != DS_OK)
  {
    Shutdown();
    m_failed = true;
    return false;
  };
  fmt.cbSize = 40;
  fmt.nAvgBytesPerSec = 22050;
  fmt.nBlockAlign = 2;
  fmt.nChannels = 1;
  fmt.nSamplesPerSec = 11025;
  fmt.wBitsPerSample = 16;
  fmt.wFormatTag = 1;
  dbd.dwSize = sizeof (dbd);
  dbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
  dbd.dwBufferBytes = 0;
  dbd.lpwfxFormat = NULL;
  dbd.dwReserved = 0;
  if (m_directSound->CreateSoundBuffer(&dbd,&pSoundBuf,NULL) != DS_OK)
  {
    Shutdown();
    m_failed = true;
    return false;
  };
  result = pSoundBuf->SetFormat(&fmt);
  pSoundBuf->Release();
  return true;
}

void MYDIRECTSOUND::Shutdown()
{
  i32 i;
  if (m_directSound != NULL)
  {
    for (i=0; i<m_numBuffersActive; i++)
    {
      m_soundBuffers[i]->Release();
    };
    m_numBuffersActive = 0;
    m_directSound->Release();
    m_directSound = NULL;
  };
  if (m_comInitialized) 
  {
    CoUninitialize();
    m_comInitialized = false;
  };
}

void MYDIRECTSOUND::Clean()
{
  i32 i;
  i32 status;
  for (i=m_numBuffersActive-1; i>=0; i--)
  {
    m_soundBuffers[i]->GetStatus((DWORD *)&status);
    if (status & DSBSTATUS_PLAYING) continue;
    m_soundBuffers[i]->Release();
    m_soundBuffers[i] = m_soundBuffers[m_numBuffersActive-1];
    m_numBuffersActive--;
  };
}

bool MYDIRECTSOUND::Play(const BYTE *wave, i32 attenuation)
{
  //SNDHEAD sndhead;
  DSBUFFERDESC dbd;
  WAVEFORMATEX wavfmt;
  HRESULT result;
  SNDHEAD *pSndHead;
  char *pSecondaryBuffer;
  ui32 secondaryBufferSize;
  if (attenuation < 0) attenuation = 0;
  if (attenuation > 100) attenuation = 100;
  if (m_directSound == NULL) 
  {
    if (!Initialize()) return false;
  };
  Clean();  // Get rid of any finished buffers.
  if (wave == NULL) return true;
  if (m_numBuffersActive == 64) return true; //discard the wave
  pSndHead = (SNDHEAD *)wave;
  wavfmt.wFormatTag      = pSndHead->wFormatTag;
  wavfmt.nChannels       = pSndHead->nChannels;
  wavfmt.wBitsPerSample  = pSndHead->wBitsPerSample;
  wavfmt.nSamplesPerSec  = pSndHead->nSamplesPerSecond;
  wavfmt.nAvgBytesPerSec = pSndHead->nAvgBytesPerSec;
  wavfmt.nBlockAlign     = pSndHead->nBlockAlign;
  //wavfmt.cbSize          = pSndHead->cbSize;
  dbd.dwSize = sizeof (dbd);
  dbd.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLVOLUME;
  dbd.dwBufferBytes = pSndHead->numSamples54; // To be filled in after parsing 
  dbd.lpwfxFormat = &wavfmt;
  dbd.dwReserved = 0;
  result = m_directSound->CreateSoundBuffer(
                &dbd,
                &m_soundBuffers[m_numBuffersActive],
                NULL);
  if (result != DS_OK)
  {
    m_failed = true;
    Shutdown();
    return false;
  };
  m_numBuffersActive++;
  result = m_soundBuffers[m_numBuffersActive-1]->SetVolume(-100*attenuation);
  result = m_soundBuffers[m_numBuffersActive-1]->Lock(
                0,
                0,
                (void **)&pSecondaryBuffer, 
                (DWORD *)&secondaryBufferSize,
                NULL,
                NULL,
                DSBLOCK_ENTIREBUFFER);
  if (result != DS_OK)
  {
    Shutdown();
    m_failed = true;
    return false; 
  };
  memcpy(pSecondaryBuffer, pSndHead->sample58, pSndHead->numSamples54);
  result = m_soundBuffers[m_numBuffersActive-1]->Unlock(
                  pSecondaryBuffer, 
                  pSndHead->numSamples54, 
                  NULL, 
                  0);
  if (result != DS_OK)
  {
    Shutdown();
    m_failed = true;
    return false;
  };
  result = m_soundBuffers[m_numBuffersActive-1]->Play(0,0,0);
  if (result != DS_OK)
  {
    Shutdown();
    m_failed = true;
    return false;
  };
  return true;
}

MYDIRECTSOUND myDirectSound;
#endif //020


#ifdef _MIDI //021
// ****************************  Testing midi **********************************

char filename[] = "test.mid";
char mediaName[] = "media";

class MIDI
{
  bool isOpen;
public:
  void OpenMediaFile();
  void ClosePlayer();
  void PlayMIDI();
  void PlayMediaFile();
};

void MIDI::OpenMediaFile()
{
   HWND handle = 0;
   int status;
   char text[100];
   ClosePlayer();
   char playCommand[256];
   sprintf(playCommand,"open %stest.mid type sequencer alias %s", g_root,mediaName);
   status = mciSendStringA(playCommand, NULL, 0, handle);
   mciGetErrorStringA(status, text, 100);
   isOpen = true;
}

void MIDI::PlayMediaFile()
{
   if (isOpen)
   {
     char playCommand[80];
     sprintf(playCommand, "Play  %s ", mediaName);
     mciSendStringA(playCommand, NULL, 0, 0);
   }
}

void MIDI::ClosePlayer()
{
    if (isOpen)
    {
      char playCommand[80];
        sprintf(playCommand,"Close %s", mediaName);
        mciSendStringA(playCommand, NULL, 0, NULL);
        isOpen = false;
    }
}


void MIDI::PlayMIDI()
{
  OpenMediaFile();
  PlayMediaFile();
}

MIDI midi;

void playmidi()
{
  midi.PlayMIDI();
}

#endif //021

ui8 *(mostRecentSound[10]);
bool mostRecentResult[10];
//ui32 mostRecentTime[10];
static ui64 finishTime = 0;   //Time at which sound should be finished

bool UI_PlaySound(const BYTE *wave, i32 /*flags*/, i32 attenuation)
{
  //We will not stop any current sound.
  //Return true if new sound successfully started.
  i32 sndflags;
  ui64 currentTimeInt;
  FILETIME currentTime;
  bool result;
  static int soundCount = 0;
  if (soundCount == -1) return true;
#ifdef WIN32_SOUNDMIXER //022
  ASSERT (wave != NULL);
  if (usingDirectX && playDirectSound.Play(wave, attenuation)) return true;
  if (!usingDirectX) playDirectSound.Shutdown();
#else
  if (usingDirectX && myDirectSound.Play(wave, attenuation)) return true;
  myDirectSound.Shutdown();
  usingDirectX = false;
#endif //022
  GetSystemTimeAsFileTime(&currentTime);
  currentTimeInt = *(ui64 *)&currentTime;
  if (currentTimeInt < finishTime) return false; // Wait for previous sound to finish
  sndflags = 0;
  //if (flags & SOUND_SYNC)  sndflags |= SND_SYNC;
  //if (flags & SOUND_ASYNC) sndflags |= SND_ASYNC;
  //if (flags & SOUND_MEMORY)sndflags |= SND_MEMORY;
  sndflags |= SND_ASYNC | SND_NOSTOP | SND_MEMORY;
  mostRecentSound[9] = mostRecentSound[8];
  mostRecentSound[8] = mostRecentSound[7];
  mostRecentSound[7] = mostRecentSound[6];
  mostRecentSound[6] = mostRecentSound[5];
  mostRecentSound[5] = mostRecentSound[4];
  mostRecentSound[4] = mostRecentSound[3];
  mostRecentSound[3] = mostRecentSound[2];
  mostRecentSound[2] = mostRecentSound[1];
  mostRecentSound[1] = mostRecentSound[0];
  mostRecentSound[0] = (ui8 *)wave;
  result = sndPlaySoundA((const char *)wave, sndflags) != 0;
  mostRecentResult[9] = mostRecentResult[8];
  mostRecentResult[8] = mostRecentResult[7];
  mostRecentResult[7] = mostRecentResult[6];
  mostRecentResult[6] = mostRecentResult[5];
  mostRecentResult[5] = mostRecentResult[4];
  mostRecentResult[4] = mostRecentResult[3];
  mostRecentResult[3] = mostRecentResult[2];
  mostRecentResult[2] = mostRecentResult[1];
  mostRecentResult[1] = mostRecentResult[0];
  mostRecentResult[0] = result;
  if (result == false)
  {
    if (soundCount == 0)
    {
      UI_MessageBox("First attempt to play sound failed","error",MESSAGE_OK);
      soundCount = -1;
      return true;
    };
  }
  else
  {
    // Record the time at which this sound will be finished.
    //
    SNDHEAD *pSndHead;
    pSndHead = (SNDHEAD *)wave;
    finishTime = currentTimeInt + 10000000/pSndHead->nSamplesPerSecond * (INT64)pSndHead->numSamples54;
  };
  soundCount++;
  return result;
}
#endif //018 _MSVC_INTEL
#ifdef TARGET_OS_MAC //023
void UI_PlaySound(char *wave, i32 flags)
{
  i32 sndflags;
  sndflags = 0;
  if (flags & SOUND_SYNC)  sndflags |= SND_SYNC;
  if (flags & SOUND_ASYNC) sndflags |= SND_ASYNC;
  if (flags & SOUND_MEMORY)sndflags |= SND_MEMORY;
  sndflags |= SND_ASYNC;
  sndPlaySound(wave, sndflags);
}
#endif //023 TARGET_OS_MAC

#ifdef TARGET_OS_MAC //024 
void UI_ClearScreen()
{
}
#endif //024

#ifdef _LINUX //025
void UI_ClearScreen()
{
  //nothing;
  UI_Invalidate();
}
#endif //025


#ifdef _MSVC_INTEL //026
void UI_ClearScreen()
{
  RedrawWindow(hWnd,NULL,NULL,RDW_ERASE|RDW_INVALIDATE);
}
#endif //026

extern RECT g_rcClient;
POINT MapPoint(const RECT &rcDest, const RECT &rcSource, POINT point);

#ifndef TARGET_OS_MAC //028
#ifndef _LINUX //029
// The linux version of this function is defined in CSBlinux.cpp
void UI_GetCursorPos(i32 *x, i32 *y)
{
   POINT point;
   GetCursorPos(&point);
   ScreenToClient(hWnd, &point);

  point=MapPoint(g_rcAtari, g_rcClient, point);
  *x = point.x;
  *y = point.y;
}
#endif //029
#endif //028

#ifndef _LINUX //030

bool UI_ProcessOption(char *key, char *value)
{
  if (strcmp(key,"DIRECTORY")==0)
  {
    if (g_folderName != NULL) UI_free(g_folderName);
    g_folderName = (char *)UI_malloc(strlen(value)+2, MALLOC013);
    if (g_folderName != NULL) 
    {
      strcpy(g_folderName,value);
      if (g_folderName[strlen(g_folderName)-1] != '\\')
      {
        g_folderName[strlen(g_folderName)+1] = 0;
        g_folderName[strlen(g_folderName)] = '\\';
      };
      g_folderParentName = parentFolder(g_folderName, g_folderName+strlen(g_folderName));
    };
    return true;
  };
  if (strcmp(key,"REPEAT") == 0)
  {
    RepeatGame = true;
    NoRecordCommandOption = true;
    RecordCommandOption = false;
    return true;
  };
  if (strcmp(key,"HEIGHT") == 0)
  {
    sscanf(value,"%d", &WindowHeight);
    return true;
  };
  if (strcmp(key,"QUICK") == 0)
  {
    sscanf(value,"%d", &NoSpeedLimit);
    return true;
  };
  if (strcmp(key,"VBLMULTIPLIER") == 0)
  {
    sscanf(value,"%d", &VBLMultiplier);
    return true;
  };
  if (strcmp(key,"DUNGEON") == 0)
  {
    strcpy(dungeonName,value);
    return true;
  };
  if (strcmp(key,"WIDTH") == 0)
  {
    sscanf(value,"%d", &WindowWidth);
    return true;
  };
  if (strcmp(key,"X") == 0)
  {
    sscanf(value,"%d", &WindowX);
    return true;
  };
  if (strcmp(key,"Y") == 0)
  {
    sscanf(value,"%d", &WindowY);
    return true;
  };
  if (strcmp(key,"SIZE") == 0)
  {
    _strupr(value);
    if (strcmp(value,"FULL") == 0)
    {
      fullscreenRequested = true;
    };

    if (strcmp(value,"FSX6") == 0)
    {
      screenSize = 6;
      return true;
    };
    if (strcmp(value,"HUMONGOUS") == 0)
    {
      screenSize = 6;
      return true;
    };
    if (strcmp(value,"FSX5") == 0)
    {
      screenSize = 5;
      return true;
    };
    if (strcmp(value,"EXTREMELYLARGE") == 0)
    {
      screenSize = 5;
      return true;
    };
    if (strcmp(value,"EXTRALARGE") == 0)
    {
      screenSize = 4;
      return true;
    };
    if (strcmp(value,"VERYLARGE") == 0)
    {
      screenSize = 3;
      return true;
    };
    if (strcmp(value,"LARGE") == 0)
    {
      screenSize = 2;
      return true;
    };
    if (strcmp(value,"SMALL") == 0)
    {
      screenSize = 1;
      return true;
    };
    return false;
  };
  if (strcmp(key,"SPEED") == 0)
  {
    CSB_UI_MESSAGE csbMessage;
    _strupr(value);
    csbMessage.type = UIM_SETOPTION;
    csbMessage.p1 = OPT_CLOCK;
    csbMessage.p2 = -1;
    if (strcmp(value,"GLACIAL") == 0) csbMessage.p2 = SPEED_GLACIAL;
    if (strcmp(value,"MOLASSES") == 0) csbMessage.p2 = SPEED_MOLASSES;
    if (strcmp(value,"VERYSLOW") == 0) csbMessage.p2 = SPEED_VERYSLOW;
    if (strcmp(value,"SLOW") == 0) csbMessage.p2 = SPEED_SLOW;
    if (strcmp(value,"NORMAL") == 0) csbMessage.p2 = SPEED_NORMAL;
    if (strcmp(value,"FAST") == 0) csbMessage.p2 = SPEED_FAST;
    if (strcmp(value,"QUICK") == 0) csbMessage.p2 = SPEED_QUICK;
    if (csbMessage.p2 != -1) CSBUI(&csbMessage);
  };
  if (strcmp(key,"PLAY") == 0)
  {
    strcpy(PlayfileName,value);
    extraTicks = false;
    PlaybackCommandOption = true;
    return true;
  };
  if (strcmp(key,"RECORD") == 0)
  {
    RecordCommandOption = true;
    NoRecordCommandOption = false;
  };
  if (strcmp(key,"NORECORD") == 0)
  {
    NoRecordCommandOption = true;
    RecordCommandOption = false;
  };
  if (strcmp(key,"DIRECTX") == 0)
  {
    usingDirectX = true;
  };
  return false;
}

#endif //030

void UI_BeginRecordOK(bool ok)
{
  BeginRecordOK = ok;
  if (ok)
  {
    if (RecordCommandOption || RecordMenuOption)
    {
      RecordfileOpen(true);
    }
    else if (!NoRecordMenuOption)
    {
      RecordfilePreOpen();
    };
    if (PlaybackCommandOption)
    {
      PlayfileOpen(true);
      if (PlayfileIsOpen())
      {
        extraTicks = false;
      };
    };
#ifdef _DEBUG  //030
    /*
    if (!TimerTraceActive)
    {
      if (AttackTraceActive || AITraceActive || OpenTraceFile())
      {
        TimerTraceActive = true;
      };
    };
    */
#endif //030
  };
}

std::string parentFolder(char *fileName, char *endname)
{
  std::string result;
  // fileName points to first character; endname points to last+1;
  // Now we back up, looking for a '\'
#ifdef _MSVC_INTEL //031
  if (*(endname-1) == '\\') endname--;
  while (endname > fileName)
  {
    endname--;
    if (*endname == '\\') break;
  };
#endif //031
#if defined(TARGET_OS_MAC) || defined(_LINUX) //032
   if (*(endname-1) == '/') endname--;
  while (endname > fileName)
  {
    endname--;
    if (*endname == '/') break;
  };
#endif //032
  if (endname > fileName)
  {
    endname++; //include the backslash at the end
    result.append( fileName, endname-fileName); 
  }
  return result;
}

#ifdef _MSVC_INTEL //033
i64 UI_GetSystemTime()
{
  // Cumulative milliseconds since 1980 (or whenever..not important)
  SYSTEMTIME st;
  i64 result;
  i32 Day;
  GetSystemTime(&st);
  if (st.wMonth < 3)
  {
    st.wYear--;
    st.wMonth += 12;
  };
  Day = st.wDay;
  Day += 365 * st.wYear;
  Day += st.wYear/4;//We won't worry about -year/400 ;-)
  Day += 30 * st.wMonth;
  Day += (3 * (st.wMonth + 1))/5;
  result = Day;
  result *= 24;
  result += st.wHour;
  result *= 60;
  result += st.wMinute;
  result *= 60;
  result += st.wSecond;
  result *= 1000;
  result += st.wMilliseconds;
  return result;
}
#endif //033 _MSVC_INTEL

#if defined(_MSVC_INTEL)||defined(_LINUX) //034
FILE *UI_fopen(const char *name, const char *flags)
{
  return fopen(name, flags);
}
#endif //034 _MSVC_INTEL||_LINUX 

ui8 *allocatedMemoryList = NULL;
ui32 listLength = 0;

void AllocationError()
{
  UI_MessageBox("Cannot allocate memory","Fatal error",MESSAGE_OK);
  die(0xdab6);
};

#ifdef _DEBUG //035o
void CheckAllAllocated()
{
#if 0
  static i32 callcount = 0;
  ui8 *current, *end;
  callcount++;
  for (current = allocatedMemoryList;
       current != NULL;
       current = *(ui8 **)(current+0))
  {
#ifdef WIN98
    end = current + *(ui32 *)(current-16);
#else
    end = current + *(ui32 *)(current-12);
#endif
    if (    (*(ui32 *)(end+0) != 0xfdfdfdfd)
         || (*(ui32 *)(current-4) != 0xfdfdfdfd))
    {
      UI_MessageBox("Memory destruction","Disaster",MB_OK);
    };
  };
#endif
}
#endif //035

#ifdef _DEBUG //036
void *UI_malloc(i32 size, ui32 id)
#else
void *UI_malloc(i32 size, ui32 /*id*/)
#endif //036
{
  void *result;
#ifdef _DEBUG //037
  CheckAllAllocated();
  result = malloc(size+12);
#else
  result = malloc(size);
#endif //037
  if (result == NULL) AllocationError();
#ifdef _DEBUG //038
  *(ui8 **)((ui8 *)result+0) = allocatedMemoryList;
  *(ui8 **)((ui8 *)result+4) = NULL;
  *(i32 *)((ui8 *)result+8) = id;
  if (allocatedMemoryList != NULL)
  {
    *(void **)(allocatedMemoryList+4) = result;
  };
  allocatedMemoryList = (ui8 *)result;
  if (id != 0xffff) listLength++;
  return (void *)((ui8 *)result+12);
#else
  return result;
#endif //038
}

#ifdef _DEBUG //039
void *UI_realloc(void *buf, i32 size, ui32 id)
#else
void *UI_realloc(void *buf, i32 size, ui32 /*id*/)
#endif //039
{
  void *result;
#ifdef _DEBUG //040
  void **next, **prev;
  if (buf == NULL)
  {
    return UI_malloc(size, id);
  }
  else
  {
    buf = (void *)((ui8 *)buf - 12);
    size += 12;
    result = realloc(buf, size);
    if (result == NULL) AllocationError();
    prev = ((void ***)result)[1];
    if (prev == NULL)
    {
      allocatedMemoryList = (ui8 *)result;
    }
    else
    {
      prev[0] = result; //Fix forward link of previous.
    };
    next = ((void ***)result)[0];
    if (next != NULL)
    {
      next[1] = result;//Fix back link of next.
    };
  };
  return (void *)((char *)result + 12);
#else
  if (buf == NULL)
  {
    result = realloc(NULL, size);
  }
  else
  {
    result = realloc((char *)buf, size);
  };
  if (result == NULL) AllocationError();
  return (void *)((char *)result);
#endif //040
}

void UI_free(void *buf)
{
#ifdef _DEBUG //041
  void **next, **prev;
  CheckAllAllocated();
  buf = (void *)((pnt)buf-12);
  prev = ((void ***)buf)[1];
  next = ((void ***)buf)[0];
  if (prev == NULL)
  {
    allocatedMemoryList = (ui8 *)next;
  }
  else
  {
    prev[0] = next;
  };
  if (next != NULL)
  {
    next[1] = prev;
  };
  if ( ((ui32 *)buf)[2] != 0xffff) listLength--;
  free(buf);
#else
  free(buf);
#endif //041
}

void UI_CheckMemoryLeaks()
{
  if (listLength != 0)
  {
    char num[30];
    char result[300];
    int i;
    ui8 *pCurrent;
    result[0] = 0;
    for (i=0, pCurrent=allocatedMemoryList;
         (i<10) && (pCurrent!=NULL);
         pCurrent = ((ui8 **)pCurrent)[0])
    {
      if (((i32 *)pCurrent)[2] == 0xffff) continue;
      sprintf(num,"%d\n",((i32 *)pCurrent)[2]);
      strcat(result, num);
      i++;
    };

    // ROQUEN: bad me
#if defined(_MSC_VER) //042
    MessageBoxA(NULL,result,"Memory Leak Detected",MB_OK|MB_TASKMODAL);
#else
#endif //042
  };
}

#if !defined(_LINUX) //043

LISTING::LISTING()
{
  m_listing = (char *)malloc(1);
  *m_listing = 0;
}

void LISTING::Clear()
{
  free(m_listing);
  m_listing = NULL;
}

LISTING::~LISTING()
{
  Clear();
}

void LISTING::AddLine(const char *text)
{
  if ((m_listing != NULL) && (strlen(m_listing) != 0))
  {
    NewLine();
  };
  AddText(text);
}

void LISTING::AddText(const char *line)
{
  i32 len;
  len = (m_listing != NULL) ? len = strlen(m_listing) : 0;
  m_listing = (char *)realloc(m_listing, (len + strlen(line) + 200)/100*100);
  strcpy(m_listing+len, line);
}

void LISTING::NewLine()
{
  AddText("\015\012");
}

void LISTING::DisplayList(const char * /*title*/)
{
  EditDialog dlg;
  dlg.m_initialText = m_listing;
  dlg.DoModal();
  if (dlg.m_finalText != NULL) UI_free(dlg.m_finalText);
  dlg.m_finalText = NULL;
}

#endif //043
