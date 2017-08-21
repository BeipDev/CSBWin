#ifdef _LINUX
extern SDL_Surface *SCR;
extern SDL_Surface *WND;
extern float st_Y;
extern float st_X;
//void LIN_PlaySound(i8* audio, ui32 size, i32 volume);
void UI_Initialize_sounds(void);
//void LIN_KillSound(i8*);
#define MB_OK MESSAGE_OK
#endif
#ifdef _DYN_WINSIZE
# include <Hermes/Hermes.h>
extern HermesHandle from_palette;
extern HermesHandle to_palette;
extern HermesHandle converter;
extern HermesFormat* from_format;
# define Y_TO_CSB( y,size ) (i32)( ((float)( y )) * st_Y )
# define X_TO_CSB( x,size ) (i32)( ((float)( x )) * st_X )
#else /* _DYN_WINSIZE */
# define Y_TO_CSB( y,size ) (i32)( ( y ) / (size) )
# define X_TO_CSB( x,size ) (i32)( ( x ) / (size) )
#endif /* _DYN_WINSIZE */


enum XLATETYPE
{ 
  TYPEIGNORED,
  TYPEKEY,
  TYPESCAN,
  TYPEMSCANL,
  TYPEMSCANR
};



enum MTYPE
{
  UIM_NULL = 0,
  UIM_INITIALFUNCTIONENTRY,
  UIM_TERMINATE, //Internal message to terminate program.
  UIM_RETURN, //Function return
  UIM_INITIALIZE,
  UIM_SETOPTION, // p1 = option  (see enum OPTIONS)
                 // p2 = new value
  UIM_KEYDOWN,     // P1=virtual key #
  UIM_CHAR,        // Ascii character
  UIM_LEFT_BUTTON_DOWN,
  UIM_LEFT_BUTTON_UP,
  UIM_RIGHT_BUTTON_DOWN,
  UIM_RIGHT_BUTTON_UP,
  UIM_VBL,   //created by vertical blank handler
  UIM_TIMER,
  UIM_PAINT, //Redraw changed portions of screen
  UIM_REDRAW_ENTIRE_SCREEN,
             // The next UIM_PAINT message should cause the
             // entire screen to be repainted unconditionally.
  UIM_EditGameInfo, // No parameters....just do it.
  UIM_Statistics,    // No parameters....just do it
  UIM_NOOP           // No Parameters....Do nothing
};

enum STYPE // status return from CSBUI()
{
  UI_STATUS_NORMAL = 1,
  UI_STATUS_TERMINATE
};

enum OPTIONS
{
  OPT_NORMAL,
  OPT_DOUBLE,
  OPT_TRIPLE,
  OPT_QUADRUPLE,
  OPT_QUINTUPLE,
  OPT_SEXTUPLE,
  OPT_RECORD,
  OPT_PLAYBACK,
  OPT_QUICKPLAY,
  OPT_TIMERTRACE,
  OPT_ATTACKTRACE,
  OPT_CLOCK,
  OPT_ITEMSREMAINING,
  OPT_NONCSBITEMSREMAINING,
  OPT_PLAYERCLOCK,
  OPT_EXTRATICKS,
  OPT_DMRULES,
  OPT_AITRACE,
  OPT_VOLUME,
  OPT_DIRECTX,
  OPT_GRAPHICTRACE,
  OPT_DSATRACE,
};

class CSB_UI_MESSAGE
{
public:
  MTYPE type;
  i32   p1;
  i32   p2;
  i32   p3;
  i32   p4;
  i32   p5;
  i32   p6;
};

i32 CSBUI(CSB_UI_MESSAGE *);

void UI_Die(int v); //Called by game to terminate abnormally.
                    //The game does not expect this function
                    //to return !!!!
                    //In windows version this is implemented
                    //with try .. catch  code structure.
void UI_Sleep(i32 milliseconds);
i64  UI_GetSystemTime(void); //cumulative milliseconds
const i32 MESSAGE_OK          = 1;
const i32 MESSAGE_YESNO       = 2;
const i32 MESSAGE_ICONERROR   = 4;
const i32 MESSAGE_ICONWARNING = 8;
const i32 MESSAGE_IDOK  = 0;
const i32 MESSAGE_IDYES = 1;
const i32 MESSAGE_IDNO  = 2;
i32  UI_MessageBox(const char *msg, const char *title, i32 flags);
i32  UI_DeleteFile(const char *fname);
void UI_Invalidate(bool erase = false); // Send UIM_Paint message when
                          //all other messages are done.
const i32 SOUND_SYNC   =1;
const i32 SOUND_ASYNC  =2;
const i32 SOUND_MEMORY =4;
bool UI_PlaySound(const char *wave, i32 flags, i32 attenuation);
void UI_StopSound(void);
void UI_PushMessage(MTYPE,i32=0,i32=0,i32=0,i32=0,i32=0,i32=0);
void UI_GetCursorPos(i32 *x, i32 *y);
i32  UI_CONSTAT(void);// return non-zero if key available
i32  UI_DIRECT_CONIN(void); //Get next key...no waiting.
void UI_ClearScreen(void);
i32  UI_DisableAllMessages(void);
i32 UI_EnableMessages(i32);
void UI_SetDIBitsToDevice(
            i32,
            i32,
            i32,
            i32,
            i32,
            i32,
            i32,
            i32,
            char *,
            void *,//BITMAPINFO *,
            i32);
#ifdef _LINUX
bool UI_ProcessOption(char **argv,i32 &argc);
#else
bool UI_ProcessOption(char *key, char *value);
#endif

void UI_BeginRecordOK(bool ok);

#if defined _MSVC_INTEL || defined _LINUX || defined _MSVC_CE2002ARM
FILE *UI_fopen(const char *name, const char *flags);
#endif

void *UI_malloc(i32 size, ui32 id);
void *UI_realloc(void *buf, i32 size, ui32 id);
void  UI_CheckMemoryLeaks(void);

#if defined _MSVC_INTEL || defined _LINUX || defined _MSVC_CE2002ARM
void UI_free(void *buf);
#endif

#if defined _MSVC_CE2002ARM
struct BMI_CE2002
{
  BITMAPINFOHEADER bmih;
  int colors[3];
};

class VIDEOCE
{
private:
  HDC hDC;
  HDC hDCmem;
  HBITMAP hBitmap;
  HBITMAP hOldBitmap;
  BMI_CE2002 bmi;
  unsigned short *bitmap;
  void ReleaseBitmap(void);
public:
  VIDEOCE(void){hDC=0;hDCmem=0;hBitmap=0;hOldBitmap=0;bitmap=NULL;};
  ~VIDEOCE(void){ReleaseBitmap();};
  unsigned short *GetBitmap(int width, int height);
  unsigned short *GetBitmap(void){return bitmap;};
  void SetPalette(i32 *palette);
  void DrawBitmap(int srcx, int srcy, int dstx, int dsty, int width, int height);
  void ClearScreen(void);
};

#endif


class EditDialog
{
  //Text strings are nul-terminated.
public:
  char *m_initialText;//This will not be free'd.  You are
                      //responsible for it.
  char *m_finalText;  //We will allocate this from the
                      //heap. You are responsible for it.
  EditDialog(void);
  ~EditDialog(void);
  i32 DoModal(void);
};


class DSAListDialog
{
  //Text strings are nul-terminated.
public:
  //char *m_initialText;//This will not be free'd.  You are
                      //responsible for it.
  //char *m_finalText;  //We will allocate this from the
                      //heap. You are responsible for it.
  DSAListDialog(void);
  ~DSAListDialog(void);
  i32 DoModal(void);
};



class LISTING
{
  char *m_listing;
public:
  LISTING(void);
  ~LISTING(void);
  void Clear(void);  // Remove all text
  void DisplayList(const char *title);
  void AddLine(const char *line);  // Add an endline sequence followed by text.
  void NewLine(void);        // Add newline sequence.
  void AddText(const char *text);  // Add text with no endline sequence.
};
