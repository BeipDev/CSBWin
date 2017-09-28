#include "stdafx.h"

#include <stdio.h>
#ifdef _MSVC_CE2002ARM
ui32 time(void *);
#else
#include <time.h>
#endif

#include "UI.h"

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"


extern i32 VBLperTimer;
extern bool overlayActive;

i32 timerTypeModifier[3]; // set/clear/toggle

void info(const char *msg, unsigned int n);
void CleanupGraphics();
void CloseTraceFile();
void CustomBackgrounds(i32 x, i32 y, i32 facing, i32 roomNum);
void ClearOverlayPalette();
bool IsTextScrollArea(int, int);
i32  TextWidth();


void TraceTimer(TIMER *pTimer, i32 index, const char *msg);
void RecordfileOpen(bool open);
void RecordFile_Close();
void RecordFile_Record(MouseQueueEnt *MQ);
void RecordFile_Record(const char *line);
bool IsPlayFileOpen();
pnt GetExternalPortraitAddress(i32 index);
void QueueDSASwitchAction(
                   i32 delay,        //
                   i32 tmrAct,       //Action set/clear/toggle
                   i32 level,        //
                   i32 targetX,i32 targetY,
                   i32 targetPos,
                   char MorD);

//i32 D0,D1,D2,D3,D4,D5,D6,D7;
//pnt A0,A1,A2,A3,A4,A5,A6,A7;

extern bool PlaybackCommandOption;
extern bool DMRulesDesignOption;
extern bool invisibleMonsters;
extern bool drawAsSize4Monsters;
extern bool GameIsComplete;
extern bool RepeatGame;
extern bool fullscreenRequested;
extern bool virtualFullscreen;
extern bool overlappingText;
extern i32 VBLMultiplier;
extern char *helpMessage;
extern unsigned char *encipheredDataFile;
extern bool simpleEncipher;
extern i32 trace;
extern ui8 reincarnateAttributePenalty, reincarnateStatPenalty, randomPoints;
extern i32 numGlobalVariables;
extern ui32 *globalVariables;

#ifdef GraphicsDebug
FILE *GrphDbg = NULL;
#endif

pnt firstMemoryBlock=NULL; // Debug list of allocated blocks

DBank d;

void _MessageBox(const char *msg)
{
  UI_MessageBox(msg,NULL,MESSAGE_OK);
}

void Terminate()
{
  die(0);
};

void die(i32 errorNum, const char *msg) // TAG000efc
{
  char message[300];
//  {
//    FILE *f;
//    f = fopen("debug", "a");
//    fprintf(f, "die(%d)\n", errorNum);
//    fclose(f);
//  };
  if ((msg==NULL)||(strlen(msg)==0))
  {
    switch(errorNum)
    {
    case 41:  msg="Cannot open GRAPHICS.DAT"; break;
    case 141: msg="Too many files open"; break;
    default:  msg="SystemError";
    };
  };
  sprintf(message,"System Error  0x%08x\n%s",errorNum,msg);
  UI_MessageBox(message,NULL,MESSAGE_OK);
  if (errorNum == 41)
  {
    UI_MessageBox(helpMessage,"Help",MESSAGE_OK);
  };
  CloseTraceFile();
  RecordFile_Close();
  UI_Die(errorNum);
}

void _Assert(bool value, char *program, i32 line, const char *text)
{
  i32 n;
  if (value==0)
  {
    char msg[1000];
    if (text == NULL) text = "";
    sprintf(msg,
        "Assertion failure at time = %d\n"
        "Line %d\n"
        "Program %s\n"
        "%s\n"
        "Should we quit?",  d.Time, line, program, text);
    n = UI_MessageBox(msg,NULL,MESSAGE_YESNO);
    if (n == MESSAGE_IDYES)
    {
      if (TimerTraceActive)
      {
        CLOSE(TraceFile);
        die (0,NULL);
      };
    };
  };
}

struct PRINTENTRY
{
  PRINTENTRY *m_next;
  i32         m_color;
  i32         m_type;
  char        m_text[1];
};

class PRINTQUEUE
{
private:
  PRINTENTRY *m_firstEnt;
public:
  PRINTQUEUE() {m_firstEnt = NULL;};
  ~PRINTQUEUE()
  {
    while (m_firstEnt != NULL)
    {
      PRINTENTRY *temp = m_firstEnt;
      m_firstEnt = temp->m_next;
      UI_free (temp);
    };
  };
  void Queue(i32 color, i16 type, const char *text);
  PRINTENTRY *GetEntry();
};

void PRINTQUEUE::Queue(i32 color, i16 type, const char *text)
{
  i32 len;
  PRINTENTRY *temp, *next;
  len = strlen(text);
  len += sizeof (PRINTENTRY) + 10;// Just to feel safe.
  temp = (PRINTENTRY *)UI_malloc (len, MALLOC007);
  if (temp == NULL)
  {
    die(0x7dea3,"Cannot allocate Printline Memory");
  };
  temp->m_next = NULL;
  temp->m_color = color;
  temp->m_type = type;
  strcpy(temp->m_text,text);
  if (m_firstEnt ==NULL) m_firstEnt = temp;
  else
  {
    next = m_firstEnt;
    while (next->m_next != NULL) next = next->m_next;
    next->m_next = temp; // Add new entry at end of queue.
  };
}

PRINTENTRY *PRINTQUEUE::GetEntry()
{
  PRINTENTRY *temp;
  if (m_firstEnt == NULL) return NULL;
  temp = m_firstEnt;
  m_firstEnt = temp->m_next;
  return temp;
}

PRINTQUEUE printQueue;


bool verifyRectPos(RectPos *r)
{
  ui32 x1, x2, y1, y2;
  if (d.UseByteCoordinates)
  {
    x1=r->b.x1;
    x2=r->b.x2;
    y1=r->b.y1;
    y2=r->b.y2;
  }
  else
  {
    x1=r->w.x1;
    x2=r->w.x2;
    y1=r->w.y1;
    y2=r->w.y2;
  };
  if (x1>x2) return false;
  if (y1>y2) return false;
  if (x2>319) return false;
  if (y2>199) return false;
  return true;
}

#ifdef _littleEndian
i32 LE32(i32 lng)
{
  return   ((lng >> 24)&0xff)
         | ((lng >> 8)&0xff00)
         | ((lng << 8)&0xff0000)
         | ((lng <<24)&0xff000000);
}
#endif

#ifdef _bigEndian
i32 BE32(i32 lng)
{
  return   ((lng >> 24)&0xff)
         | ((lng >> 8)&0xff00)
         | ((lng << 8)&0xff0000)
         | ((lng <<24)&0xff000000);
}
#endif




RN& RNGear(pnt p)
{
  return *((RN *)(p));
}

#ifdef _DEBUG
i32& longGear(ui8 *p)
{
  ASSERT(((int)p & 3) == 0,"p");
  return *((i32 *)(p));
}
#endif

pnt& pntGear(pnt p)
{
  ASSERT(((int)p & 3) == 0,"p");
  return *((pnt *)(p));
}

upnt& upntGear(pnt p)
{
  return *((upnt *)(p));
}

void SwapWordsInButtonList(btn *pButton)
{
  for (;pButton->word0 != 0; pButton++)
  {
    pButton->word0  = LE16(pButton->word0);
    pButton->xMin   = LE16(pButton->xMin);
    pButton->xMax   = LE16(pButton->xMax);
    pButton->yMin   = LE16(pButton->yMin);
    pButton->yMax   = LE16(pButton->yMax);
    pButton->button = LE16(pButton->button);
  };
}

i32 programDescriptor[] = {
  0,      // Program Header Address
  0,      // Last Allocated address +1
  0,      // Start of program I-Bank
  0,      // Length of program I-Bank
  0,      // Start of initialized data
  0x05ec, // Length of initialized data
  0,      // Start of uninitialized data
  0x5b70  // Length of uninitialized data
};



ui8 LScreenBase[320][210]; // Extra room at end for ???

void SetButtonPointers()
{
  d.pButn18970[0][0] = d.Buttons18624;
  d.pButn18970[0][1] = d.Buttons18660;
  d.pButn18970[0][2] = d.Buttons18708;
  d.pButn18970[0][3] = d.Buttons18768;
  d.pButn18970[1][0] = d.Buttons18792;
  d.pButn18970[1][1] = d.Buttons18828;
  d.pButn18970[1][2] = d.Buttons18876;
  d.pButn18970[1][3] = d.Buttons18936;
}

DBank::DBank()
{
}

void DBank::Initialize() // TAG00332a
{
  //d.Word1834=512;
  Word1844=512;
  Word1870 = 1;
  Pointer2066[0] = &Byte1982;
  //Pointer2066[1] = (pnt)&Word1838;
  Pointer2066[2] = (pnt)&Word1870;
  Pointer2066[3] = (pnt)&Word1844;
  Pointer2526[0] = Word2050;
  Pointer2526[1] = Word2210;
  Pointer2526[2] = Word2318;
  GraphicHandle = -1;
  CurrentFloorAndCeilingGraphic = -1;
  CurrentWallGraphic  = -1;
  Word7254  = 1;
  LoadedLevel = -1;
  Word11684 = 1;
  Word11694 = 0;
  Time = 0;
  clockTick = 1;
  newPartyLevel = -1;
  LogicalScreenBase=&LScreenBase[0][0];
  srand((unsigned int)UI_GetSystemTime());
  RandomNumber=rand();
  TextScanlineScrollCount = -1;
  LastMonsterAttackTime = -200;
  Pointer12988[0] = &Byte23114;
  Pointer12988[1] = &Byte11850;
  Pointer12988[2] = &Byte22676;
  Pointer12988[3] = &Byte16550;
  Pointer12988[4] = &Byte13118;
  Pointer12988[5] = &Byte16702;
  ITEM16QueLen = 0;
  DelayedActuatorAction = -1; //Word13148
  HandChar = -1; // owner of cursor
  Pointer16596[0] = "FIGHTER";
  Pointer16596[1] = "NINJA";
  Pointer16596[2] = "PRIEST";
  Pointer16596[3] = "WIZARD";
  Long16600 = -505;
  DisplayResurrectChestOrScroll = 0;
  objOpenChest = RNnul;
  Pointer16770[2]  = "NEOPHYTE";
  Pointer16770[3]  = "NOVICE";
  Pointer16770[4]  = "APPRENTICE";
  Pointer16770[5]  = "JOURNEYMAN";
  Pointer16770[6]  = "CRAFTSMAN";
  Pointer16770[7]  = "ARTISAN";
  Pointer16770[8]  = "ADEPT";
  Pointer16770[9]  = "EXPERT";
  Pointer16770[10] = "` MASTER";
  Pointer16770[11] = "a MASTER";
  Pointer16770[12] = "b MASTER";
  Pointer16770[13] = "c MASTER";
  Pointer16770[14] = "d MASTER";
  Pointer16770[15] = "e MASTER";
  Pointer16770[16] = "ARCHMASTER";
  Pointer16778  = &Byte10638;
  //pwPointer16774  = &Word918;
  Pointer16770[0]  = (char *)&Long16600;
  //Pointer16770[1]  = (pnt)&Word1834;
  DirectionNames[0] = "NORTH";
  DirectionNames[1] = "EAST";
  DirectionNames[2] = "SOUTH";
  DirectionNames[3] = "WEST";
  Pointer16822[0] = "L";
  Pointer16822[1] = "STRENGTH";
  Pointer16822[2] = "DEXTERITY";
  Pointer16822[3] = "WISDOM";
  Pointer16822[4] = "VITALITY";
  Pointer16822[5] = "ANTI-MAGIC";
  Pointer16822[6] = "ANTI-FIRE";
  MouseQEnd = MOUSEQLEN-1;
  MouseInterlock = 1;
  SetButtonPointers();
  MagicCaster = -1;
  Pointer22812 = "GAME LOADED, READY TO PLAY.";
  Pointer22816 = "CAN'T MODIFY CHAOS STRIKES BACK DISK!";
  Pointer22820 = "THAT'S NOT THE MASTER DISK!";
  Pointer22824 = "CAN'T FIND SAVED GAME!";
  Pointer22828 = "UNABLE TO SAVE GAME!";
  Pointer22832 = "UNABLE TO FORMAT DISK!";
  Pointer22836 = "THAT'S THE CHAOS STRIKES BACK DISK!";
  Pointer22840 = "THAT'S A GAME SAVE DISK!";
  Pointer22844 = "THAT DISK IS WRITE-PROTECTED!";
  Pointer22848 = "THAT DISK IS UNREADABLE";
  Pointer22852 = "THAT'S NOT THE SAME GAME";
  Pointer22856 = "SAVED GAME DAMAGED!";
  Pointer22860 = "PUT THE GAME SAVE DISK IN ~";
  Pointer22864 = "PUT THE CHAOS STRIKES BACK DISK IN ~";
  Pointer22868 = "PUT A BLANK DISK IN ~";
  Pointer22872 = "SAVING GAME . . .";
  Pointer22876 = "LOADING GAME . . .";
  Pointer22880 = "FORMATTING DISK . . .";
  Pointer22884 = "FORMAT DISK ANYWAY?";
  Pointer22888 = "THERE IS NO DISK IN ~!";
  Pointer22892 = "LOAD SAVED GAME";
  Pointer22896 = "SAVE AND PLAY";
  Pointer22900 = "SAVE AND QUIT";
  Pointer22904 = "FORMAT FLOPPY";
  Pointer22908 = "OK";
  Pointer22912 = "CANCEL";
  strcpy(Byte23008,"A:\\csbgame.dat");
  strcpy(Byte23024,"A:\\csbgame.bak");
  strcpy(Byte23030,"A:\\F");
  UseByteCoordinates = 0;
  Pointer23040 = (pnt)0x484;
  CurrentSound = -1;
  Word23046 = 0;
  Word23118 = 0;
  STHideCursor(HC50);
  NewCursorShape = 1;
  Word23150 = 0;
  Byte23202[0] = 2;
  Byte23202[1] = 17;
  Byte23202[2] = 2;
  Byte23202[3] = 17;
  Byte23206[0] = 0;
  Byte23206[1] = 15;
  Byte23206[2] = 0;
  Byte23206[3] = 15;
  wRectPos23214.w.x1 = 2;
  wRectPos23214.w.x2 = 20;
  wRectPos23214.w.y1 = 2;
  wRectPos23214.w.y2 = 15;
  wRectPos23222.w.x1 = 0;
  wRectPos23222.w.x2 = 18;
  wRectPos23222.w.y1 = 0;
  wRectPos23222.w.y2 = 13;
  OnMouseSwitchActionLock = 0; //(23224)
  Word23226 = 0;
//  Word23228 = 0;
  ClusterInCache = -1; //(23276)
  Word23366 = 4096;
  RightZeroMask[0] = -1;
  RightZeroMask[1] = -2;
  RightZeroMask[2] = -4;
  RightZeroMask[3] = -8;
  RightZeroMask[4] = -16;
  RightZeroMask[5] = -32;
  RightZeroMask[6] = -64;
  RightZeroMask[7] = -128;
  RightZeroMask[8] = 0;
  RightOneMask[0] = 0;
  RightOneMask[1] = 1;
  RightOneMask[2] = 3;
  RightOneMask[3] = 7;
  RightOneMask[4] = 15;
  RightOneMask[5] = 31;
  RightOneMask[6] = 63;
  RightOneMask[7] = 127;
  RightOneMask[8] = -1;
  LZWBitNumber = 0;
  Word23406 = 0;
  ClearOverlayPalette();
}

ui16 DBank::GetDerivedGraphicSize(i32 graphicNum)
{
  return pwDerivedGraphicSizes[graphicNum];
}

void DBank::SetDerivedGraphicSize(i32 graphicNum, i32 size)
{
  if (   (pwDerivedGraphicSizes[graphicNum] != 0)
       &&(pDerivedGraphicCacheIndex[graphicNum] != -1) )
  {
    char msg[100];
    sprintf(msg,"Doubly defined graphic size\nGraphic=0x%x, oldSize=0x%x, newSize=0x%x",
                   graphicNum, pwDerivedGraphicSizes[graphicNum], size);
    UI_MessageBox(msg, "Warning", MESSAGE_OK);
    if (size > pwDerivedGraphicSizes[graphicNum]) pwDerivedGraphicSizes[graphicNum] = (i16)size;
    return;
  };
  pwDerivedGraphicSizes[graphicNum] = (i16)size;
}

void DBank::AllocateDerivedGraphicsSizesBuffer()
{
  pwDerivedGraphicSizes = (i16 *)allocateMemory(730*sizeof(i16), 1);
  memset(pwDerivedGraphicSizes, 0, 730*sizeof(i16));
}

void DBank::AllocateDerivedGraphicCacheIndex()
{
  pDerivedGraphicCacheIndex = (i16 *)allocateMemory(730*sizeof(i16), 1);
  memset(pDerivedGraphicCacheIndex, 0xff, 730*sizeof(i16));
}

i16 DBank::GetDerivedGraphicCacheIndex(i32 graphicNum)
{
  return pDerivedGraphicCacheIndex[graphicNum];
}

void DBank::SetDerivedGraphicCacheIndex(i32 graphicNum, i32 index)
{
  if ((index != -1) && (pDerivedGraphicCacheIndex[graphicNum] != -1))
  {
    char msg[100];
    sprintf(msg,"Doubly defined graphic Index\nGraphic=0x%x", graphicNum);
    UI_MessageBox(msg, "Warning", MESSAGE_OK);
  };
  pDerivedGraphicCacheIndex[graphicNum] = (i16)index;
  //The sizes of the first four graphics are wired into the code.
  //We must not forget their sizes even when the graphic is deleted.
  if ((index == -1) && (graphicNum > 3)) pwDerivedGraphicSizes[graphicNum] = 0;
}

void SetSupervisorMode() {}

void ClearSupervisorMode() {}

void HangIfZero(i16 P1)
{
  if (P1 == 0)
    die(0,"HangIfZero");
}

void DisableCursor() {}

i32 GetAvailMemory()
{
  return 2000000 - 400000;// 550000;//510000;//1000000;
  // We no longer need the 843144 bytes that used to be
  // used for graphics.  We now use malloc() to allocate
  // graphic buffers permanently rather than use the 
  // complicated graphics heap logic that was designed
  // for the memory-starved Atari 512ST.
}

void jdisint(i16)
{ // Disable some interrupts.
  HopefullyNotNeeded(0xa671);
}


ui32 GraphicRandom(ui32 n)
{
  static ui32 r = 12345;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  r = r * ui32(0xbb40e62d) + 11;
  return (r>>7) % n;
}

#ifdef _DEBUG
void DebugMain(char *msg)
{
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"%s\n",msg);
  };
}
#endif

//#define DEBUGMAIN(a) DebugMain(a);
#define DEBUGMAIN(a)

//   TAG00068e
RESTARTABLE _MainLoop(const CSB_UI_MESSAGE * /*msg*/)
{//()
  static dReg D0, D1;
  static PRINTENTRY *pPrintEntry;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(4)
   // RESTART(6)
    RESTART(7)
    RESTART(8)
    RESTART(9)
    //RESTART(10)
  END_RESTARTMAP
  // We discard any message information.  At least for now.
tag000698: //We get here for each tick of the clock.
  DEBUGMAIN("tag000698")
  if (videoMode != VM_ADVENTURE)
  {
    if ( (videoMode!=VM_INVENTORY) && (d.PartySleeping==0))
    {
      videoMode = VM_ADVENTURE;
      ForceScreenDraw();
      UI_Invalidate(true);
    };
  };
  if (d.PartySleeping == 0) 
  {
    scrollingText.SetNewSpeed(d.Speed, speedTable[gameSpeed].vblPerTick);
    d.Speed = sw(speedTable[gameSpeed].vblPerTick); // Delay vblOccurred for 10 VBLs
  };
  d.vblCount = 0;
  if (d.newPartyLevel == -1) goto tag0006d0;
tag0006a4:
  DEBUGMAIN("tag0006a4")
  LoadPartyLevel(d.newPartyLevel); // Load graphics for level
  MoveObject(RN(RNnul), -1, 0, d.partyX, d.partyY, NULL, NULL);
  d.newPartyLevel = -1;
  DiscardAllInput();
tag0006d0:
  DEBUGMAIN("tag0006d0")
  if (ProcessTimers())
  {
    // Gamme has been won.
    VBLDelay(_1_,300);  //about 5 seconds
    ShowCredits(_2_,1); // We won the game with 4 corbums in FulYa pit.
                        // Show Credits will never return
  };
  if (GameIsComplete) RETURN;
  if (d.newPartyLevel != -1) goto tag0006a4;
  if (d.SelectedCharacterOrdinal != 0) goto tag000730;
  if (d.PartySleeping != 0) goto tag000730;
  DrawViewport(d.partyFacing, d.partyX, d.partyY);
  if (d.ShowCursor16572 != 0)
  {
    d.ShowCursor16572=0;
    STHideCursor(HC19); //TAG002fd2
    CreateObjectCursor(d.Pointer16572);
    STShowCursor(HC19); //TAG003026
  };
  if (d.ShowCursor1 != 0)
  {
    d.ShowCursor1=0;
    d.NewCursorShape=1;
    STHideCursor(HC20); //TAG002fd2
    STShowCursor(HC20); //TAG003026
  };
tag000730:
  DEBUGMAIN("tag000730")
  WaitForButtonFlash(); // If any are active.
  StartQueuedSound(); //counted
  DisplayCharacterDamage(); //counted
  if (d.GameIsLost == 0)
  {
    d.Time++;
    if (TimerTraceActive)
    {
      i64 stime;
      static i64 prevstime;
      stime = UI_GetSystemTime();
#ifdef _LINUX
      //fprintf(GETFILE(TraceFile),"%lld %lld Increment virtual game Time\n", stime, stime-prevstime);
      fprintf(GETFILE(TraceFile), "Increment virtual game Time to %08x\n", d.Time);
#else
      //fprintf(GETFILE(TraceFile),"%I64d %I64d Increment virtual game Time\n", stime, stime-prevstime);
      fprintf(GETFILE(TraceFile), "Increment virtual game Timeto %08x\n", d.Time);
#endif
      prevstime = stime;
    };
    if ((currentOverlay.m_p3 != 0) && (currentOverlay.m_p3 < d.Time)) overlayActive = false;
    parameterMessageSequence = 0;
    scrollingText.ClockTick();
    SmartDiscard(false);
    GameTime = d.Time;
    if ((d.Time&511)==0) // about every 85 seconds
    {
      NinetySecondUpdate(); //TAG018124
    };
    if (d.freezeLifeTimer != 0) d.freezeLifeTimer--;
    TAG01b29a(_4_);
    D0L = d.Time;
    D0L &= d.PartySleeping ? 15 : 63;
    if (D0L==0) TenSecondUpdate();
    D0L=d.partyMoveDisableTimer;
    if (D0L!=0) 
    {
      d.partyMoveDisableTimer--;
    };
    D0L=d.Word11712;
    if (D0L != 0) d.Word11712--;
    //RemoveTimedOutText(_10_); // Remove text when its time is up.
    //RemoveTimedOutText(); // Remove text when its time is up.
    D0UL=d.WatchdogTime;
    D1UL=d.Time;
    if (D0UL < D1UL)
    {
      die(60, "Watchdog Timer failure"); //die (error number)
    };
    d.clockTick=0;
tag0007b4:
    DEBUGMAIN("tag0007b4")
    goto tag0007c8;
tag0007b6: // A UIM_KEYDOWN message was received.
    DEBUGMAIN("tag0007b6")
    D0L=UI_DIRECT_CONIN(); // Wait for key
    //D0L = msg->p1;
    ASSERT(D0L != -1,"D0L");
    TAG0196da(D0L);
tag0007c8:
    DEBUGMAIN("tag0007c8")
    pPrintEntry = printQueue.GetEntry();
    if (pPrintEntry != NULL)
    {
      PrintLines(pPrintEntry->m_color,pPrintEntry->m_text);
      UI_free (pPrintEntry);
    };
    WAITFORMESSAGE(_8_);//Any message!;//pumper();
    {
      //SYSTEMTIME systime;
      //GetSystemTime(&systime);
    };
    D0W=(i16)UI_CONSTAT();
    if (D0W != 0) goto tag0007b6;
/*
    if (d.QuitPressingEye != 0)
    {
      d.PressingEye=0;
      d.QuitPressingEye=0;
      QuitPressingEye();  //Remove object from eye????
    }
    else
    {
      if (d.QuitPressingMouth != 0)
      {
        d.PressingMouth=0;
        d.QuitPressingMouth=0;
        QuitPressingMouth();
      };
    };
*/
    do 
    {
      HandleMouseEvents(_7_,1);  //TAG01a7b2
      if (d.QuitPressingEye != 0)
      {
        d.PressingEye=0;
        d.QuitPressingEye=0;
        QuitPressingEye();  //Remove object from eye????
      }
      if (d.QuitPressingMouth != 0)
      {
        d.PressingMouth=0;
        d.QuitPressingMouth=0;
        QuitPressingMouth();
      };
    } while (intResult & 1);
    if (DiskMenuNeeded) DisplayDiskMenu(_9_);
    if (d.clockTick    == 0) WaitForButtonFlash(); // If any active
    if (d.clockTick    == 0) goto tag0007b4; // Usually 0
    if (d.ClockRunning == 0) goto tag0007b4; // Usually 1
    if ((d.Time&63)    == 0) // about every 10 seconds
    {
      D0W = (i16)(d.Word11722); //d.Word11724 always zero.  & (d.Word11724 ^ 0xffff));
      if (D0W != 0)
      {
        TAG02076e();
        TAG0207cc();
      };
    };
    goto tag000698;
  };
  DEBUGMAIN("RETURN")
  RETURN;
}

// *********************************************************
//
// *********************************************************
//   TAG000850
void LoadPartyLevel(const i32 level)
{//()
  dReg D0;
  //i32  LOCAL_1;
  //i8   LOCAL_2[6];
  TIMER LOCALa;
  //RESTARTMAP
    //RESTART(1)
    //RESTART(2)
  //END_RESTARTMAP
  TAG00bd40(); //Remove item16 entries for monsters from level.
  SetPartyLevel(level);
  ReadGraphicsForLevel();
  if (d.Time > d.Long16600+500)
  {
    d.Long16600=d.Time+1;
    LOCALa.Function(TT_22);
    D0L=d.Long16600+2;
    //D1L=level;
    //D1L >>= 24;
    //D0L |= D1L;
    //LOCALa.timerTime = D0L;

    // Notice that D1L was shifted right!!!! 20130115 PRS
    LOCALa.Time(D0L);
    LOCALa.Level((ui8)level);
    //Not needed, I think. PRS 17 Feb 2004....SetTimer(&LOCALa);
    InsertDisk(0,0);
    HopefullyNotNeeded(0xcdb7);
    //d.Word19218 = 12777; // Seems in middle of graphic data
    //d.Word918 = 136;
    //d.Word1834 = 555;
  };
  ProcessMonstersOnLevel(); //add item16 entries for monsters on level.
  SelectPaletteForLightLevel();
  return;
}


void MemMove(ui8 *src, ui8 *dest, i32 byteCount) // TAG0009dc
{
  memmove(dest, src, byteCount);
}

void ClearMemory(ui8 *dest,i32 numByte) // TAG000a84
{
  memset(dest,0,numByte);
}

void fillWithByte(ui8 *addr,i16 num,i8 value,i16 spacing) //TAG000ac0
{
  do
  {
    *addr = value;
    addr+=spacing;
  } while (--num!=0);
}

void fillMemory(i16 *pwAddress, i32 num, i16 value, i16 spacing) //TAG000af6
{ // num words,  spacing apart
  spacing /= 2; // Must be word spacing
  do
  {
    *pwAddress = value;
    pwAddress += spacing;
  } while ((--num) != 0);
}

void TAG000bb0(dReg& D0, dReg& D4, dReg& D5) // called by VBL handler
{  // Expects something in D0, D5, D4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// Always zero. I removed it.  if (d.Word23228 < 0)
  {
// Word11720 always zero.  I removed it.    d.Word11722 |= d.Word11720;
// Always zero.  I removed it.    d.Word11720 = 0;
//    d.Word23228 = 0;
  };
  if (d.Word23136 == 0) return;
  if (D0W != 0)goto tag000c32;
  if (d.TextScanlineScrollCount < 0) goto tag000bde;
  if ((ui16)D5W >= 151) goto tag000c32;
tag000bde:
  if (d.ViewportUpdated == 0) goto tag000c24;
  if (D5W >= 169) goto tag000c24;
  if (D4W >= 224) goto tag000c0c;
  if (d.Word23146 == 0) goto tag000c04;
  if (d.Word23150 == 0) goto tag000c04;
  if (D5W <= 23) goto tag000c24;
  goto tag000c32;
tag000c04:
  if (D5W < 16) goto tag000c24;
  goto tag000c32;
tag000c0c:
  if (d.Word23146 == 0) goto tag000c24;
  if (D5W > 28) goto tag000c24;
  if (D5W <= 23) goto tag000c24;
  if (D4W < 232) goto tag000c32;
tag000c24:
  if (d.FlashButnActive == 0) goto tag000c30;
  if (D4W >= 192) goto tag000c32;
tag000c30:
  return;
tag000c32:
  D0L = 1;
  RemoveCursor(); //TAG00289a();
}

ui32 VBLCount = 0;
ui32 VBLInterruptCount = 0;
void vblInterrupt() // TAG000c3a
{
// Vertical Blank Handler
  dReg D0, D1, D2, D3, D4, D5;
  //aReg A0;
  static i32 Count=0;
  VBLInterruptCount++;
  //SaveRegs(0xfc8c);
  //i32 *A4,*A5;
  //VBLCount++; // Counted by function that calls us.
  // A4 = data000b2c; // both start at zero
  // A5 = data000b30;
  if (d.ViewportUpdated)
  {
    d.DynamicPaletteSwitching = d.Word11740;
  }
  //if (d.DynamicPaletteSwitching != 0)
  //{
  //  d.Word11988 = 0;
  //  A0 = (pnt)0xfffffa00;
  //};

  if (d.VBLInterruptActive != 0)
  {
    return;
  };
  d.VBLInterruptActive++;
//                            Loop:  ; From below.
//loop:
  Instrumentation(icntVBLLoop);
  if (d.Word23138 == 0)
  {
    D0L = 0;
    D3H1 = d.NewMouseY;//Word23124;
    D3W = d.NewMouseX;//Word23122;
    if (!IsPlayFileOpen())
    {
      d.CurMouseY = D3H1;//Word23128 = D3U;
      d.CurMouseX = D3W;//Word23126 = D3W;
    };
    D2L = D3L;
    SWAP(D2);
    D4W = d.CurCursorX;
    D5W = d.CurCursorY;
    if (d.Word23144 != 0)
    {
      if ((D3W < 274) || (D2W > 28))
      {
        //SaveRegs(0xf0c0);
        TAG002818();
        //RestoreRegs(0x030f);
      };
    };
    D0W = d.Word23132;
    d.Word23132 = 0;
    if (   (d.Word23130 != 0)
        && (d.NewCursorShape == 0)
        && (D3W == D4W)
        && (D2W == D5W) )
    {
      D0L = 1;
      d.Word23130 = 0;
    };
    TAG000bb0(D0, D4, D5); // Expects something in D0, D5, D4//counted
    // May set D0 to 1;
    if ((d.Word1870 == 0) && (d.Word11750 == 0))
    {
      //SaveRegs(0xf0c0);
      TAG00456c();
      //RestoreRegs(0x030f);
        d.Word11750+=5;
    };
    if ((D0W != 0) && ((ui16)D2W <= 15))
    {
      DrawCursor();
      D0L = 0;
    };
    if (d.TextScanlineScrollCount >= 0)
    {
      //SaveRegs(0xf0c0);
      D1W = (i16)(27 - d.TextScanlineScrollCount);
      D1L = (ui16)D1W * (ui16)160;
      MemMove(d.LogicalScreenBase+172*160+160, // Up one line
              d.LogicalScreenBase+172*160,
              D1W);
      // Should not need this.  Unconditional Invalidate follows.  Invalidate();
      //RestoreRegs(0x030f);
    }
    else
    {
      if (d.PushTextUp == 0) goto tag000dbe;
      d.PushTextUp = 0;
    };
    D1L = 7;
    if (d.TextScanlineScrollCount >= 0)
    {
      d.TextScanlineScrollCount++;
      D1W = d.TextScanlineScrollCount;
      if (D1W == 7) d.TextScanlineScrollCount = -1;
    };
    //SaveRegs(0xf0c0);
    D2W = D1W;
    D1L = (ui16)D1W * (ui16)160;
    D2W = (i16)-D2W;
    D2W += 200;
    D2L = (ui16)D2W * (ui16)160;
    MemMove((ui8 *)d.newTextLine,   // Bitmap of new text line.
            d.LogicalScreenBase + D2L,
            D1W);
    Invalidate();
    //RestoreRegs(0x030f);
tag000dbe:
    if (d.ViewportUpdated != 0)
    {
      int i;
      pnt A0,A1;
      d.ViewportUpdated = 0;
      if (d.Word11778 != 0)
      {
        d.Word11778 = 0;
        A0 = (pnt)&d.Palette552[d.CurrentPalette]; // palette address
        memmove(&d.Palette11946,A0,32); // set current palette
      };
      A0 = (aReg)d.pViewportBMP;
      A1 = (aReg)d.LogicalScreenBase+5280;
      Instrumentation(icntCopyViewportToScreen);
      if ((VBLMultiplier==1) || (((Count++)%3)==0) || (VBLMultiplier==99))
      for (i=0; i <136; i++)
      {
        memmove(A1,A0,112);
        A0 += 112;
        A1 += 160;
      };
    };
    if (d.FlashButnActive != 0)
    {
      d.FlashButnActive = 0;
      d.Word11776 ^= 0xffff;
      //SaveRegs(0xf0c0);
      TraceFlash("vblFlashButn from 000e46");
      vblFlashButn(d.FlashButnX1,
                   d.FlashButnX2,
                   d.FlashButnY1,
                   d.FlashButnY2);
      //RestoreRegs(0x030f);
    };
    if (D0W != 0)
    {
      DrawCursor(); // put cursor in LogBase...save old pixels//counted
    };
    if (d.Word23130 != 0)
    {
      d.Word23130 = 0;
      d.Word23132 = 1;
      d.NewCursorShape = 0;
      //SaveRegs(0xf0c0);
      CreateCursorBitmap(); // TAG00295e
      //RestoreRegs(0x030f);
    };
  };
// Always zero.  I removed it.  if (d.Word23228 > 0)
//  {
//    //A0 = d.pFunc23232;
//  }
//  else
  {
    if (d.Word23226 != 0)
    {
      //A0 = d.pFunc23236;
    };
  };
  d.vblCount += 1;
  //D0W = d.vblCount;
  if ((d.vblCount >= d.Speed) || (NoSpeedLimit!=0))
  {
    if (NoSpeedLimit!=0)
    {
      if (d.Time >= NoSpeedLimit)
      {
        NoSpeedLimit = 0;
        VBLMultiplier = 1;
      };
    };
    d.clockTick = 1;
  }
  else
  {
    if (   playerClock
        && (d.vblCount > 10)
        && !mouseQueueIsEmpty()
        && !IsPlayFileOpen() )
    {
      d.clockTick = 1;
    };
  }
  d.VBLInterruptActive -= 1;
  //if (d.VBLInterruptActive != 0)
  //{
    // This seems impossible.
  //  goto loop;
  //};

  //RestoreRegs(0x313f);
  return;
}



void TAG000ec6()
{
//////;;;;;;;;;;;;;;;;;;;;;;;;;;;; Set some user interrupt vectors
////
////
////                                   TRAP #4 now goes to vertical blank code.
  verticalIntEnabled = true;
}

void SystemError(i16 errorNum)
{
  die(errorNum, "");
}

//   TAG000f7e
void BLT2Screen(ui8 *P1,RectPos *dstPos,i16 P3,i16 P4)
{   //             8       12     16     18
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//0f7e 4e56 0000                LINK     A6,#0
  d.UseByteCoordinates = 0;
  ASSERT(verifyRectPos(dstPos),"rectpos");
  TAG0088b2((ui8 *)P1,
            (ui8 *)d.LogicalScreenBase,
            dstPos,
            0,   // src x
            0,   // src y
            P3,  // src bytes/line
            160, // dst bytes/line
            P4);
}

// *********************************************************
//
// *********************************************************
//   TAG000fb2
RESTARTABLE _VBLDelay(const i32 n) //void
{ // Pause for n vertical blanks
  static i32 i;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  for (i=0; i<n; i++)
  {
    wvbl(_1_);
  };
  RETURN;
}

/*
i16 Abs(i32 x)
{
  if (x>=0) return (i16)x; else return (i16)-x;
}

i32 Min(i32 n, i32 m)
{
  return n<m?n:m;
}


//  TAG000ff8
i16 Max(i16 x, i16 y)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (x <= y) return y;
  else return x;
}

  */
//  TAG00100e
i16 ApplyLimits(i32 min,i32 x,i32 max)
{
  if      (x <= min) x = min;
  else if (x >  max) x = max;
  return (i16)x;
}

ui32 numRandomCalls = 0;

i32 STRandom() //TAG001030
{
  dReg D0;
  numRandomCalls++;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D0L = d.RandomNumber * ui32(0xbb40e62d) + 11;
  d.RandomNumber = D0L;
  D0L >>= 8;
  D0L &= 0xffffff;
  return D0L;
}

i32 STRandom(i32 num)
{
  ASSERT(num!=0,"num");
  if (num == 0) return 0;
  return (STRandom() & 0xffff) % num; // To be exactly like Atari
}

//  TAG001050
ui8 STRandomBool()
{ // Zero or One
  numRandomCalls++;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  d.RandomNumber = d.RandomNumber * (ui32)0xbb40e62d + 11;
  return (UI8)((d.RandomNumber>>8) & 1);
}

//  TAG001074
ui8 STRandom0_3()
{
  numRandomCalls++;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  d.RandomNumber = d.RandomNumber * (ui32)0xbb40e62d + 11;
  return (UI8)((d.RandomNumber>>8) & 3);
}

void TAG0010ae()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D7;
  aReg A2, A3;
  //i32 saveD7=D7;pnt saveA2=A2,saveA3=A3;
  d.Pointer16572 = (pnt)allocateMemory(128,1);
  A3 = (aReg)allocateMemory((GetGraphicDecompressedSize(0x22c)+199)&0xffff,1);
  A2 = (aReg)allocateMemory(2786,0);
  ReadAndExpandGraphic(0x822c,(ui8 *)A2,0,0);
  for (D7W=0;D7W<199;D7W++)
  {
    d.ObjectNames[D7W] = (char *)A3;
    while ((*A2 & 128)   == 0)
    {
      *(A3++) = *(A2++);
    };
    *(A3++) = (i8)(*(A2++)&0x7f);
    *(A3++) = 0;

  };
//  BuildSmartDiscardTable();
  FreeTempMemory(2786);
  //D7=saveD7;A2=saveA2;A3=saveA3;
}

OBJ_NAME_INDEX GetBasicObjectType(RN object)
{
  //dReg D7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (object == RNnul)
  {
    return objNI_NotAnObject;
  }
  else
  {
    OBJ_DESC_INDEX objDI;
    objDI = object.DescIndex();//TAG009308
    if (objDI == -1) return objNI_NotAnObject;
    return d.ObjDesc[objDI].objectType();
  };
}

//         TAG001534
OBJ_NAME_INDEX ObjectAtLocation(i32 bodyLocation)
{ // Object type in nth (0 to 7) hand at top of screen.
  // Or object in nth position of clothing/backpack (8 to 37)
  //dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D0W = 6*index;
  return d.IconDisplay[bodyLocation].objectType();
  //return D0W;
}
/*
//           TAG0018f6
void RemoveTimedOutText()
{//()
  //static dReg D0, D5, D6;
  //static RectPos LOCAL_8;
  dReg D0, D5, D6;
  RectPos LOCAL_8;
  //RESTARTMAP
  //  RESTART(1)
  //END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  d.UseByteCoordinates = 0;
  LOCAL_8.w.x1 = 0;
  LOCAL_8.w.x2 = 319;
  for (D6W=0; D6W<4; D6W++)
  {
    D5L = d.TextTimeout[D6W];
    if (D5L == -1) continue;
    if (D5L > d.Time) continue;
    D0W = (i16)(172 + D6W*7);
    LOCAL_8.w.y1 = D0W;
    LOCAL_8.w.y2 = (i16)(D0W +6);
    //  while (   (d.TextScanlineScrollCount >= 0)
    //         || (d.PushTextUp !=0) )
    //  {
    //    wvbl(_1_);//NotImplemented(0x194e);//pumper();
    //  };
      if (   (d.TextScanlineScrollCount >= 0)
             || (d.PushTextUp !=0) )
      {
        return;  //We'll get it next time.  //pumper();
      };
    STHideCursor();//TAG002fd2
    FillRectangle(d.LogicalScreenBase, (RectPos *)&LOCAL_8, 0, 160);
    STShowCursor(); //TAG003026
    d.TextTimeout[D6W] = -1;
//
  };
  //RETURN;
}
*/


/*
//           TAG001992
RESTARTABLE _CreateNewTextRow()
{//()
  static dReg D7;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.PrintRow == 3)
  {
    while (   (d.TextScanlineScrollCount>=0)
      || (d.PushTextUp != 0) )
    { //NotImplemented(0x19ae);};//pumper();};
      wvbl(_1_);
    };
    ClearMemory(d.newTextLine, 1120);
    d.TextScanlineScrollCount = 0; //Start new line into scroll area
    for (D7W=0; D7W<3; D7W++)
    {
      d.TextTimeout[D7W] = d.TextTimeout[D7W+1];
    };

    d.TextTimeout[3] = -1;
    RETURN;
  };
  d.PrintRow++;
  RETURN;
}
*/

// *********************************************************
//
// *********************************************************


SCROLLING_TEXT scrollingText;


LINEQUEUE::LINEQUEUE(i32 maxLines)
{
  m_maxLines = maxLines;
  m_firstText = NULL;
  m_lastText = NULL;
}

LINEQUEUE::~LINEQUEUE()
{
  Cleanup();
}

void LINEQUEUE::Cleanup()
{
  PIECE_OF_TEXT *next;
  while (m_firstText != NULL)
  {
    next = m_firstText;
    m_firstText = m_firstText->m_next;
    UI_free(next);
  };
  m_firstText = NULL;
}

i32 LINEQUEUE::MaxPrintLinesCount()
{
  if (m_firstText == NULL) return -1;
  // We assume that the printLinesCount cannot decrease.
  // Therefore, the entry with the highest printLineCount is the last entry.
  return m_lastText->m_printLinesCount;
}

void LINEQUEUE::AddText(i32 row, i32 column, i32 color, const char *text, i32 printLinesCount)
{
  i32 len;
  PIECE_OF_TEXT *pNewPiece;
  len = strlen(text);
  pNewPiece = (PIECE_OF_TEXT *)UI_malloc(sizeof (PIECE_OF_TEXT) + len + 1 , MALLOC103);
  pNewPiece->m_row = row;
  pNewPiece->m_col = column;
  pNewPiece->m_color = color;
  pNewPiece->m_next = NULL;
  pNewPiece->m_printLinesCount = printLinesCount;
  memcpy((char *)pNewPiece + sizeof(*pNewPiece), text, len+1);
  d.PrintColumn = (ui16)(d.PrintColumn + len);
  if (m_firstText == NULL)
  {
    m_firstText = pNewPiece;
    m_lastText  = pNewPiece;
  }
  else
  {
    m_lastText->m_next = pNewPiece;
    m_lastText = pNewPiece;
  };
}

void LINEQUEUE::DiscardFirst()
{
  PIECE_OF_TEXT *result;
  if (m_firstText == NULL) return;
  result = m_firstText;
  m_firstText = result->m_next;
  if (m_firstText == NULL) m_lastText = NULL;
  UI_free(result);
}

SCROLLING_TEXT::SCROLLING_TEXT()
    :m_pastLines(NUMOLDLINE),m_currentLines(4),m_futureLines(NUMFUTURELINE)
{
  i32 i;
  for (i=0; i<4; i++) m_printLinesCount[i] = -1;
}

SCROLLING_TEXT::~SCROLLING_TEXT()
{
  Cleanup();
}

void SCROLLING_TEXT::Cleanup()
{
  m_pastLines.Cleanup();
  m_currentLines.Cleanup();
  m_futureLines.Cleanup();
}

void SCROLLING_TEXT::DiscardText()
{
  i32 i, top;
  Cleanup();
  for (i=0; i<4; i++) m_printLinesCount[i] = -1;
  d.PrintRow = -1;
  d.TextScanlineScrollCount = -1;
  d.PushTextUp = 0;

  RectPos LOCAL_8;
  d.UseByteCoordinates = 0;
  LOCAL_8.w.x1 = 0;
  LOCAL_8.w.x2 = 319;
  for (i=0; i<4; i++)
  {
    top = (i16)(172 + i*7);
    LOCAL_8.w.y1 = (i16)top;
    LOCAL_8.w.y2 = (i16)(top +6);
    STHideCursor(HC20);//TAG002fd2
    FillRectangle(d.LogicalScreenBase, (RectPos *)&LOCAL_8, 0, 160);
    STShowCursor(HC20); //TAG003026
  };

}

//   TAG001836
void SCROLLING_TEXT::SetPrintPosition(i32 column, i32 row)
{
  if (column <   0) column = 0;
  if (column >= 53) column = 52;
  if (row <   0) row = 0;
  if (row >=  4) row = 3;
  d.PrintColumn = (i16)column;
  d.PrintRow = (i16)row;
}


//           TAG0018f6
void SCROLLING_TEXT::RemoveTimedOutText()
{//()
  //static dReg D0, D5, D6;
  //static RectPos LOCAL_8;
  dReg D0, D5, D6;
  RectPos LOCAL_8;
  //RESTARTMAP
  //  RESTART(1)
  //END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  d.UseByteCoordinates = 0;
  LOCAL_8.w.x1 = 0;
  LOCAL_8.w.x2 = 319;
  for (D6W=0; D6W<4; D6W++)
  {
    D5L = d.TextTimeout[D6W];
    if (D5L == -1) continue;
    if (D5L > d.Time) continue;
    D0W = (i16)(172 + D6W*7);
    LOCAL_8.w.y1 = D0W;
    LOCAL_8.w.y2 = (i16)(D0W +6);
    //  while (   (d.TextScanlineScrollCount >= 0)
    //         || (d.PushTextUp !=0) )
    //  {
    //    wvbl(_1_);//NotImplemented(0x194e);//pumper();
    //  };
      if (   (d.TextScanlineScrollCount >= 0)
             || (d.PushTextUp !=0) )
      {
        return;  //We'll get it next time.  //pumper();
      };
    STHideCursor(HC20);//TAG002fd2
    FillRectangle(d.LogicalScreenBase, (RectPos *)&LOCAL_8, 0, 160);
    STShowCursor(HC20); //TAG003026
    d.TextTimeout[D6W] = -1;
//
  };
  //RETURN;
}

void SCROLLING_TEXT::ClockTick()
{
  PIECE_OF_TEXT *pPiece;
  i32 row;
  RemoveTimedOutText();
  for (;;)
  {
    if (d.TextScanlineScrollCount >= 0) return; //wait for scrolling to cease
    if (d.PushTextUp != 0) return; //wait for line to scroll.
    if (m_futureLines.IsEmpty()) return;
    pPiece = m_futureLines.Peek();
    if (pPiece->m_row == -1)
    { // A command to create new row of text at bottom.
      if ((d.TextTimeout[0] != -1) && (m_futureLines.MaxPrintLinesCount() <= m_printLinesCount[0])) return;
      ScrollUp();
      m_printLinesCount[3] = pPiece->m_printLinesCount;
      m_futureLines.DiscardFirst();
      continue;
    };
    row = pPiece->m_row;
    STHideCursor(HC21);
    TextOutToScreen(pPiece->m_col*6, 
                    row*7+177, 
                    pPiece->m_color, 
                    0, 
                    (char *)pPiece+sizeof(*pPiece));
    STShowCursor(HC21);
    if (d.PartySleeping)
    {
      d.TextTimeout[row] = d.Time + 700;
    }
    else
    {
      d.TextTimeout[row] = d.Time + 70;
    };
    if (   (row > 0) 
        && (d.TextTimeout[row-1] != -1) 
        && (d.TextTimeout[row] <= d.TextTimeout[row-1]))
    {
      d.TextTimeout[row] = d.TextTimeout[row-1];
    };
    m_futureLines.DiscardFirst();    
  };
}

void SCROLLING_TEXT::ResetTime(i32 newTime)
{
  i32 i;
  for (i=0; i<4; i++)
  {
    if (d.TextTimeout[i] == -1) continue;
    d.TextTimeout[i] += newTime - d.Time;
  };
}

void SCROLLING_TEXT::CreateNewTextRow(i32 printLinesCount)
{
  if (d.PrintRow < 3)
  {
    d.PrintRow++;
    return;
  };
  m_futureLines.AddText(-1,0,0,"",printLinesCount); // Push lines 1, 2,and 3 up to 0, 1, and 2
}

void SCROLLING_TEXT::ScrollUp()
{
  i32 i;
  ClearMemory((ui8 *)d.newTextLine, 1120);
  d.PushTextUp = 1;
  d.TextScanlineScrollCount = 0; // Start the scrolling process
  for (i=0; i<3; i++)
  {
    d.TextTimeout[i] = d.TextTimeout[i+1];
    m_printLinesCount[i] = m_printLinesCount[i+1];
  };
  d.TextTimeout[3] = -1;
}

void SCROLLING_TEXT::SetNewSpeed(i32 oldSpeed, i32 newSpeed)
{
  i32 i, delta;
  if (oldSpeed == newSpeed) return;
  for (i=0; i<4; i++)
  {
    if (d.TextTimeout[i] < 0) continue;
    delta = d.TextTimeout[i] - d.Time;
    delta = (delta*(oldSpeed+1) + newSpeed)/(newSpeed+1);
    d.TextTimeout[i] = d.Time + delta;
  };
}

//   TAG0019f8
void SCROLLING_TEXT::Printf(i32 color, const char* text, i32 printLinesCount)
{
//  dReg D0, D7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
/*
  D7W = StrLen(text);
  if (d.TextScanlineScrollCount == -1)
  {
    while (d.PushTextUp != 0) {};
    STHideCursor();
    TextOutToScreen(d.PrintColumn*6, d.PrintRow*7+177, color, 0, text);
    STShowCursor();
  }
  else
  {
    TextOut_OneLine(d.newTextLine,
                    160,
                    d.PrintColumn*6,
                    5,
                    color,
                    0,
                    text);
    if (d.TextScanlineScrollCount == -1)
    {
      d.PushTextUp = 1;
    };
  };
  d.PrintColumn = (i16)(d.PrintColumn + D7W);
  D0L = d.Time + 70; // about 10 seconds
  d.TextTimeout[d.PrintRow] = D0L;
*/
  m_futureLines.AddText(d.PrintRow, d.PrintColumn, color, text, printLinesCount);
}

// *********************************************************
//PrintLines used to be restartable.  And that was very
//inconvenient.  So what did we do?  We changed everyone
//to call QuePrintLines instead!  Then when it is convenient
//for us to call a restartable function we will take the
//lines out of the queue and call PrintLines.  We will probably
//do this in MainLoop.
// *********************************************************
void QuePrintLines(i32 color, const char *text)
{
    printQueue.Queue(color,1,text); //That was easy!
}

int TextWidth()
{
#ifdef _MSVC_CE2002ARM
  if (overlappingText) return 240;
  else return 153;
#else
  return 320;
#endif
}


// *********************************************************
//
// *********************************************************
//   TAG001aa8
void PrintLines(const i32 color, const char *Text)
{//()
  i32 lastPrintCol;
  dReg D0, D6;
  i8  LOCAL_56[54];
  //static i16  LOCAL_2;
  pnt text;
  static i32 printLinesCount = 0;
  //RESTARTMAP
  //  RESTART(1)
  //  RESTART(2)
  //END_RESTARTMAP
  lastPrintCol = TextWidth()/6;  //53 for 320-wide screen; 39 for 240-wide.
  printLinesCount++;
  text = (pnt)Text;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7W = P1;
  //A3 = P2;

  while (*text != 0)
  {
    D0W = (i16)sb(*text);
    if (D0W == 10) // Linefeed
    {
      text++;
      if ( (d.PrintColumn!=0) || (d.PrintRow!=0) )
      {
        d.PrintColumn = 0;
        scrollingText.CreateNewTextRow(printLinesCount); // scroll timeouts for the 4 lines
      };
    }
    else
    {
      D0W = (i16)sb(*text);
      if (D0W == 32)
      {
        text++;
        if (d.PrintColumn != lastPrintCol)
        {
          //LOCAL_2 = ' '; // A single blank
          scrollingText.Printf(color, " ", printLinesCount); // A single space.
        };
      }
      else
      {
        D6W = 0;
        do
        {
          LOCAL_56[D6W++] = *(text++);


        } while ((*text!=0) && (*text!=' ') && (*text!=10));
        LOCAL_56[D6W] = 0;
        if (d.PrintColumn+D6W > lastPrintCol)
        {
          d.PrintColumn = 2;
          scrollingText.CreateNewTextRow(printLinesCount);
        };
        scrollingText.Printf(color, (char *)LOCAL_56, printLinesCount);
      };
    };
  };
  scrollingText.ClockTick(); //Get things on the screen right away
  //RETURN;
}

// *********************************************************
//
// *********************************************************
//   TAG001c02
void PrintLinefeed()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  QuePrintLines(0, (char *)d.Byte1830);
}

// *********************************************************
//
// *********************************************************
//   TAG001c42
void TextOutToScreen(i32 xPixel,i32 yPixel,i32 color,i32 P4, const char* P5, bool translate)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TextOut_OneLine(d.LogicalScreenBase,
                  160,
                  xPixel,
                  yPixel,
                  color,
                  P4,
                  P5,
                  999,
                  translate);
}

void TAG001c6e()
{
  dReg D7;
  //i16 saveD7 = D7W;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  scrollingText.SetPrintPosition(0, 0);
  d.newTextLine = allocateMemory(1120,1);
  d.Pointer12926 = (pnt)allocateMemory(768,1);
  ReadAndExpandGraphic(0x822d,(ui8 *)d.Pointer12926,0,0,768);
  for (D7W = 0; D7W<4; D7W++)
  {
    d.TextTimeout[D7W] = -1;

  };
  //D7W = saveD7;
}


/*
void TAG0020ca()
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  KeyclickOff();
  giaccess(0,0x80 + 11);
  giaccess(0, 0x80 + 12);
  D0W = (i16)(giaccess(0, 7) &0xff);
  d.Word23036 = D0W;
  giaccess(d.Word23036 | 0x3f, 0x80 + 7);
  TAG001e16(8);
  jdisint(13);
  xbtimer(0, 1, 112, TAG001e50);
  jdisint(13);
//0021604e5e                     UNLK     A6
}
*/

void KeyclickOff() //TAG002164
{
  aReg A0;
  pnt LOCAL_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  LOCAL_4 = (pnt)dosound(0); // Stop any existing sound
  A0 = d.Pointer23040;
   ////  d.Byte23042 = *A0; // save old keyclick state
   ////  *A0 &= 2; // keyclick off.
  dosound((ui8 *)LOCAL_4); // Restart sound
}

//   TAG00219a
void QueueSound(i32 soundNum,i32 mapX,i32 mapY,i32 deltaTime)
{
  dReg D0, D1, D7;
  SOUND *psA3;
  i16 LOCAL_12;
  TIMER LOCAL_10;
  LOCATIONREL soundLocr(d.LoadedLevel, mapX, mapY);
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (deltaTime)
  {
    if (d.LoadedLevel != d.partyLevel) return;
  };
  psA3 = &d.sound1772[soundNum];
  if (deltaTime > 1)
  {
    D0L = d.Time;
    D0L += deltaTime & 0xffff;
    D0L--;
    //D0L |= d.LoadedLevel<<24;
    //LOCAL_10.timerTime = D0L;
    LOCAL_10.Time(D0L);
    LOCAL_10.Level((ui8)d.LoadedLevel);
    LOCAL_10.Function(TT_20);
    LOCAL_10.timerUByte5(psA3->byte4);
    LOCAL_10.timerWord8() = sw(soundNum);
    LOCAL_10.timerUByte6((i8)mapX);
    LOCAL_10.timerUByte7((i8)mapY);
    gameTimers.SetTimer(&LOCAL_10);
    return;
  };
  D7W = (i16)(mapX - d.partyX);
  if (D7W < 0)
  {
    D0W = (i16)-D7W;
  }
  else
  {
    D0W = D7W;
  };
  D7W = (i16)(mapY- d.partyY);
  if (D7W < 0)
  {
    D1W = (i16)-D7W;
  }
  else
  {
    D1W = D7W;
  };
  D7W = (i16)(D0W + D1W); //Total orthoganal distance
  if (D7W <= (UI8)(psA3->byte6))
  {
    LOCAL_12 = 1;
  }
  else
  {
    LOCAL_12 = 0;
  };
  if (D7W >= (UI8)psA3->byte7) LOCAL_12 = -1;
  if (SoundFilter(soundNum, LOCAL_12, &soundLocr) == 0) return; // The Filter sill handle the sound.
  if (LOCAL_12 < 0) return;
  /*
  d.Word23116++;
  if ( (d.Word23116>=88) && (d.Word23228==0) )
  {
    d.Word23116 = 0;
    d.Word8892 += 3;
    d.CacheInvalid |= 2;
    TAG02134a(d.ClusterCache + 512);
    d.Word1588 = (i16)(d.Word8892<<2);
    //d.Word11752 &= d.Word1588;
  };
  */
  //if (deltaTime == 0)
  {
    //LOCATIONREL soundLocr(d.LoadedLevel, mapX, mapY);
    StartSound(GetBasicGraphicAddress(psA3->word0|0x8000),// Start Sound
               (UI8)(psA3->byte3),
               LOCAL_12);
    return;
  };
  //if ( (d.CurrentSound!=-1) && (LOCAL_12 <= d.Word23046) )
  //{
  //  if (LOCAL_12 == d.Word23046) return;
  //  D1W = (UI8)(d.sound1772[d.CurrentSound].byte4);
  //  D0W = (i16)(psA3->byte4 & 0xff);
  //  if (D0W <= D1W) return;
  //};
  //d.CurrentSound = sw(soundNum);
  //d.Word23046 = LOCAL_12;
}

//   TAG002336
void StartQueuedSound()
{
  SOUND *psA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  Instrumentation(icntTAG002336);
  CheckSoundQueue();
  if (d.CurrentSound != -1)
  {
    psA3 = &d.sound1772[d.CurrentSound];
    StartSound(GetBasicGraphicAddress(psA3->word0|0x8000),
               (UI8)(psA3->byte3),
               d.Word23046);
    //playSound(psA3->word0);
    /*
    if (d.Word8892 & 1)
    {
      if (d.Word23228 == 0)
      {
        if (d.Word11724 & 4)
        {
          d.Word11724 &= 0xfffb;
          d.Word11722 &= 0xfffb;
        };
        d.Word8892++;
      };
    };
    */
    d.CurrentSound = -1;
    d.Word23046 = 0;
  };
}

void TAG0023b0()
{
  //aReg A3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //pnt saveA3 = A3;
  d.Pointer23190 = (pnt)allocateMemory(288, 1);
  d.Pointer23186 = (pnt)allocateMemory(288,1);
  d.Pointer23182 = (pnt)allocateMemory(256, 1);
  d.Pointer23178 = (pnt)allocateMemory(256, 1);
  d.CursorBitmap = (ui8 *)allocateMemory(432, 1);
  d.SavedCursorPixels = allocateMemory(18*24, 1);
  d.NewMouseX = 251;
  d.NewMouseY = 51;
  // kbdvbase  A3 = TAG001c8(34);

  // Set Mouse Vector to 002ef4
  //A3 = saveA3;
}

//   TAG002456
void SetCursorShape(i16 P1)
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  STHideCursor(HC22);
  d.Word23146 = 0;
  D0W = (i16)((P1==1) ? 1 : 0);
  d.CursorShape = D0W;
  d.NewCursorShape = 1;
  STShowCursor(HC22);
}

//   TAG002480
void CreateObjectCursor(pnt P1)
{
  aReg A2, A3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A3 = P1;
  STHideCursor(HC23);
  d.Word23146 = 1;
  d.CursorShape = 0;
  d.NewCursorShape = 1;
  d.UseByteCoordinates = 1;
  A2 = d.Pointer23190;
  Flood((ui8 *)A2, 12, 36);
  ShrinkBLT((ui8 *)A3,
            d.SavedCursorPixels,
            16,
            16,
            16,
            16,
            d.Byte1382);
  TAG0088b2((ui8 *)d.SavedCursorPixels,
            (ui8 *)A2,
            (RectPos *)d.Byte23202,
            0,
            0,
            8,
            16,
            -1);
  MemMove((ui8 *)A2, (ui8 *)d.Pointer23186, 288);
  ShrinkBLT((ui8 *)A3,
            d.SavedCursorPixels,
            16,
            16,
            16,
            16,
            d.Byte1366);
  TAG0088b2((ui8 *)d.SavedCursorPixels,
            (ui8 *)A2,
            (RectPos *)d.Byte23206,
            0,
            0,
            8,
            16,
            12);
  TAG0088b2((ui8 *)A3,
            (ui8 *)d.Pointer23186,
            (RectPos *)d.Byte23206,
            0,
            0,
            8,
            16,
            12);
  STShowCursor(HC23);
}

void TAG002572()
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.EmptyHanded)
  {
    if (d.HandChar == -1) // owner of cursor
    {
      D0L = 0;
    }
    else
    {
      D0L = 1;
    };
    SetCursorShape(D0W);

  }
  else
  {
    CreateObjectCursor(d.Pointer16572);
  };
}

void TAG0025a6(i32 P1)
{//()
  dReg D0, D4, D5, D6, D7;
  aReg A2;
  RectPos *rectPosA3;
  CHARDESC *pcA0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(P1);
  if (d.Word23144 == 0)
  {
    D0W = (i16)CharacterAtPosition((d.partyFacing+D7W) & 3);
    if (D0W == -1) return;
    d.NewCursorShape = 1;
    STHideCursor(HC24);
    d.UseByteCoordinates = 0;
    A2 = (aReg)d.Pointer23182;
    Flood((ui8 *)A2, 12, 32);
    rectPosA3 = &d.wRectPos1454[D7W];
    TAG0088b2((ui8 *)d.LogicalScreenBase,
              (ui8 *)A2,
              &d.wRectPos23214,
              rectPosA3->w.x1,
              rectPosA3->w.y1,
              160,
              16,
              0);
    ShrinkBLT((ui8 *)A2, (ui8 *)A2, 32, 16,32,16,d.Byte1382);
    MemMove((ui8 *)A2, (ui8 *)d.Pointer23178, 256);
    TAG0088b2((ui8 *)d.LogicalScreenBase,
              (ui8 *)d.Pointer23178,
              &d.wRectPos23222,
              rectPosA3->w.x1,
              rectPosA3->w.y1,
              160,
              16,
              0);
    A2 = (aReg)d.SavedCursorPixels;
    Flood((ui8 *)A2, 12, 32);
    TAG0088b2((ui8 *)d.LogicalScreenBase,
              (ui8 *)A2,
              &d.wRectPos23222,
              rectPosA3->w.x1,
              rectPosA3->w.y1,
              160,
              16,
              0);
    ShrinkBLT((ui8 *)A2, (ui8 *)A2, 32, 14, 32, 14, d.Byte1366);
    TAG0088b2((ui8 *)A2,
              (ui8 *)d.Pointer23182,
              &d.wRectPos23222,
              0,
              0,
              16,
              16,
              12);
    FillRectangle(d.LogicalScreenBase,
                  rectPosA3,
                  0,
                  160);
    STShowCursor(HC24);
    d.Word23144 = (i16)(D7W + 1);
  }
  else
  {
    d.NewCursorShape = 1;
    D6W = (i16)(d.Word23144-1);
    d.Word23144 = 0;
    D4W = (i16)CharacterAtPosition((d.partyFacing + D6W) & 3);
    pcA0 = &d.CH16482[D4W];
    pcA0->facing = (i8)d.partyFacing;
    if (D6W == D7W)
    {
      pcA0 = &d.CH16482[D4W];
      pcA0->charFlags |= CHARFLAG_positionChanged;
      DrawCharacterState(D4W);
    }
    else
    {
      D5W = (i16)CharacterAtPosition((D7W + d.partyFacing) & 3);
      STHideCursor(HC25);
      if (D5W >= 0)
      {
        pcA0 = &d.CH16482[D5W];
        pcA0->charPosition = ub((D6W + d.partyFacing) & 3);
        d.CH16482[D5W].charFlags |= CHARFLAG_positionChanged;
        DrawCharacterState(D5W);
      }
      else
      {
        FillRectangle(d.LogicalScreenBase,
                      &d.wRectPos1454[D6W],
                      0,
                      160);
      };
      d.CH16482[D4W].charPosition = (UI8)((D7W+d.partyFacing)&3);
      d.CH16482[D4W].charFlags |= CHARFLAG_positionChanged;
      DrawCharacterState(D4W);
      STShowCursor(HC25);
    };
  };
}

void TAG002818() // called by VBL handler
{// Mouse cursor has moved out of the champion alignment area of the screen.
 // ( The upper right corner )
  dReg D6, D7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D6W = (i16)(d.Word23144-1);
  d.Word23144 = 0;
  if(!IsPlayFileOpen())
  {
    MouseQueueEnt ent;
    ent.x = 0xffff;
    ent.y = 0xffff;
    ent.num = 0xffff;
    RecordFile_Record(&ent);
  };
  d.NewCursorShape = 1;
  d.Word23118++;
  if (d.Word23118 == 1)
  {
    d.Word23134 = 0;
    d.Word23130 = 0;
    d.Word23132 = 0;
    if (d.Word23136)
    {
      d.Word23136 = 0;
      RemoveCursor();
    };
  };
  D7W = d.UseByteCoordinates;
  BLT2Screen((ui8 *)d.Pointer23178,
             &d.wRectPos1454[D6W],
             16,
             12);
  d.UseByteCoordinates = D7W;
  d.Word23118--;
  if (d.Word23118 == 0)
  {
    d.Word23130 = 1;
    d.Word23134 = 1;
  };
}

//   TAG00289a
void RemoveCursor() // called by VBL handler
{ // restore area under cursor
  Instrumentation(icntRemoveCursor);
  for (i32 i=0; i<18; i++)
  {
    memmove(d.ScreenCursorAddress+160*i,
            d.SavedCursorPixels+24*i,
            24);
  };
}

void CreateCursorBitmap() // TAG00295e
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D0, D1, D4, D5, D6, D7;
  aReg A2, A3;
  //i32 saveD4=D4, saveD5=D5, saveD6=D6, saveD7=D7;
  //pnt saveA2=A2, saveA3=A3;
  pnt LOCAL_12;
  i16 LOCAL_8;
  i16 LOCAL_6;
  RectPos rect;
//  i8  LOCAL_4;
//  i8  LOCAL_3;
//  i8  LOCAL_2;
//  i8  LOCAL_1;
  Instrumentation(icntCreateCursorBitmap);
  d.Pointer23170 = (pnt)d.CursorBitmap;
  LOCAL_6 = d.UseByteCoordinates;
  d.UseByteCoordinates = 1;
  D7H1 = d.CurMouseY;//Word23128;
  D7H2 = d.CurMouseX;//Word23126;
  D6H1 = D7H2;
  D6H2 = D7H1;
  d.NewCursorX = D7W;
  d.NewCursorY = D6W;
  if (d.Word23144 != 0) goto tag002ad4;
  A3 = d.Byte1222; // Arror cursor
  LOCAL_8 = 6;
  d.Word23150 = 0;
  if (D6W >= 169) goto tag0029f8;
  if (D7W >= 274) goto tag0029f8;
  if (D6W <= 28) goto tag0029ba;
  if (D7W >= 224) goto tag0029f8;
  goto tag0029e0;
tag0029ba:
  D4H2 = (i16)(D7UW / 69);
  D4H1 = (i16)(D7UW % 69);
  D5W = D4W;  // Which character
  if (D5W >= d.NumCharacter) goto tag0029e0;
  SWAP(D4); // pixel within character
  if (D4W > 42) goto tag0029e0;
  D5W++; // index to ordinal character number
  if (D5W == d.SelectedCharacterOrdinal) goto tag0029f8;
  if (D6W <= 6) goto tag0029f8;
tag0029e0:
  if (d.Word23146 != 0) goto tag002a8a;
  if (d.CursorShape == 0) goto tag0029f8;
  A3 = d.Byte1350;
  LOCAL_8 = 0;
tag0029f8:
  d.CursorHeight = 15;
  d.Word23156 = 0;
  d.Word23158 = 0;
  D7W &= 15; // x offset in word
  if (D7W == 0) goto tag002a20;
  d.CursorWidth = 2; // 32 pixels
  if (D7W > LOCAL_8) goto tag002a2e;
  d.CursorWidth = 1; // 16 pixels
  goto tag002a2e;
tag002a20:
  d.CursorWidth = 1; // 16 pixels
  d.Pointer23170 = A3;
  goto tag002c8c;
tag002a2e:
  D0W = (i16)(d.CursorWidth << 4); // convert to pixels
  Flood(d.CursorBitmap,12,D0W); // Flood with color 12
  D0 = D7; // 1st pixel offset within word
  rect.b.x1 = D0UB; // first pixel x
  D0W = (i16)((D0W & 0xff) + 15); // last pixel x
  rect.b.x2 = D0UB; // last pixel x
  rect.b.y1 = 0;
  rect.b.y2 = 15;
  TAG0088b2((ui8 *)A3,              // src
            (ui8 *)d.CursorBitmap,  // dst
            &rect,           // rectPos
            0,               // srcXOffset
            0,               // srcYOffset
            8,               // src bytes/line
            (d.CursorWidth<<3), // dst bytes/line = pixels/2
            -1);
  goto tag002c8c;
tag002a8a:
  d.Word23150 = 1;
  d.CursorHeight = 17;
  d.CursorWidth = 2; // 32 pixels
  d.Word23156 = 8;
  d.Word23158 = 8;
  D5L = 18;
  D4L = 17;
  A3 = d.Pointer23190;
  A2 = d.Pointer23186;
  LOCAL_12 = A2;
  D7W = (i16)((D7W - 8) & 15);
  if (D7W != 15) goto tag002aca;
  d.CursorWidth = 3; // 48 pixels
tag002aca:
  D6W -= 19;
  if (D6W <= 0) goto tag002c24;
  goto tag002b1c;
tag002ad4:
  d.CursorHeight = 15;
  d.CursorWidth = 2; // 16 pixels
  d.Word23156 = 9;
  d.Word23158 = 7;
  D5W = 16;
  D4W = 20;
  A3 = d.Pointer23182;
  A2 = d.Pointer23178;
  LOCAL_12 = A2;
  D7W = (I16)((D7W - 9) & 15);
  if (D7W < 12) goto tag002b14;
  d.CursorWidth = 3; // 48 pixels
tag002b14:
  D6W -= 20;
  if (D6W <= 0) goto tag002c24;
tag002b1c:
  if (d.DynamicPaletteSwitching == 0) goto tag002c24;
  if (D6W < D5W) goto tag002b30;
  LOCAL_12 = A3;
  goto tag002c24;
tag002b30:
  D0 = D7;
  if (D0W != 0) goto tag002b86;
  D7W = D5W;
  D1 = D6;
  D7W = sw((D7W - D1W) << 4);
  D7L &= 0xffff;
  D0 = D7;
  MemMove((ui8 *)A2, d.CursorBitmap, D0W);
  {
    i16 num = sw(D6W<<4);
    ui8 *dst = d.CursorBitmap + D7UW;
    ui8 *src = (ui8 *)A3 + D7UW;
    MemMove(src,dst,num);
  };
  goto tag002c22;
tag002b86:
  D0L = d.CursorWidth * D5UW;
  Flood(d.CursorBitmap,12,D0W); // Flood with color 12
  D0W = D5W;
  D1 = D6;
  D0W = sw(D0W - D1W);
  D0L &= 0xffff;
  D6 = D0;
  D0 = D7;
  rect.b.x1 = D0UB;
  D0W &= 0xff;
  D0W = sw(D0W + D4W);
  rect.b.x2 = D0B;
  rect.b.y1 = 0;
  D0 = D6;
  D0W--;
  rect.b.y2 = D0B;
  TAG0088b2((ui8 *)A2,(ui8 *)d.CursorBitmap,&rect,0,0,16,d.CursorWidth<<3,-1);
  D0 = D6;
  rect.b.y1 = D0B;
  D0W = D5W;
  D0W--;
  rect.b.y2 = D0B;
  TAG0088b2((ui8 *)A3,(ui8 *)d.CursorBitmap,&rect,0,D6W,16,d.CursorWidth<<3,-1);
tag002c22:
  goto tag002c8c;
tag002c24:
  D0 = D7;
  if (D0W != 0) goto tag002c32;
  d.Pointer23170 = LOCAL_12;
  goto tag002c8c;
tag002c32:
  D0 = D7;
  rect.b.x1 = D0B;
  D0W &= 0xff;
  D0W = sw(D0W + D4W);
  rect.b.x2 = D0B;
  rect.b.y1 = 0;
  D0W = sw(D5W - 1);
  rect.b.y2 = D0B;
  D0W = sw(d.CursorWidth * (ui16)D5W);
  Flood(d.CursorBitmap,12,D0W); // Flood with color 12 = transparent
  TAG0088b2((ui8 *)LOCAL_12,(ui8 *)d.CursorBitmap,&rect,0,0,16,d.CursorWidth<<3,-1);
tag002c8c:
  d.UseByteCoordinates = LOCAL_6;
  //D4=saveD4;D5=saveD5;D6=saveD6;D7=saveD7;
  //A2=saveA2;A3=saveA3;
}


// *********************************************************
//
// *********************************************************
//   TAG002c9a
void DrawCursor() // called by VBL handler
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D0,D1,D2,D3,D4,D5,D6,D7;
  pnt A0,A1,A2;
  Instrumentation(icntPlaceCursor);
  d.Word23136 = 1;

  D6W = d.NewCursorX;
  D7W = d.NewCursorY;
  if (!IsPlayFileOpen())
  {
    d.CurCursorX = D6W;
    d.CurCursorY = D7W;
  };
  D2L = 0; // Left of screen Flag
  D6W = sw(D6W - d.Word23156);
  if (D6W < 0)
  {
    D6L = 0;
    D1L = 0;
    D2L = 1; // Left of screen flag
    goto tag002cd6;
  };
  if (D6W >= 272) // Room for widest cursor = 48 pixels
  {
    D1L = 136;  // The last 24 bytes on the line
  }
  else
  {
    D1W = D6W;
tag002cd6:
    D1W = (I16)((D1W >> 1) & 0x7ff8);
  };
    SWAP(D2);
    D7W = sw(D7W - d.Word23158);
  if (D7W < 0)
  {
    D2W = sw(-D7W);
    D7W = 0;
  }
  else
  {
    if (D7W >= 182)
    {
      D1W +=29120;
    }
    else
    {
      D0W = sw(D7W << 5);
      D1W = sw(D1W + D0W);
      D0W = sw(D0W + D0W);
      D0W = sw(D0W + D0W);
      D1W = sw(D1W + D0W); // D1 += 160*D7W;
    };
  };
  A2 = (pnt)D2L; // save flag
  A0 = (pnt)d.LogicalScreenBase + D1W;
  d.ScreenCursorAddress = (ui8 *)A0;
  A1 = (aReg)d.SavedCursorPixels;
  for (i32 ii=0; ii<18; ii++)
  { // save pixels under cursor
    memmove(A1+24*ii,A0+160*ii,24);
  };
  D0W = d.CursorHeight;
  D1W = d.CursorWidth;
  A1 =  (aReg)d.Pointer23170; // CurCursorBitmap
  D2W = LOW_I16(A2);
  if (D2W != 0)
  {
    D0W = sw(D0W - D2W);
    D2L = D2UW * D1UW;
    D2W <<= 3;
    A1 += D2W;
  }
  else
  {
    // CursorHeight + cursorY - 200
    D3W = sw(D0W + D7W - 200); //
    if (D3W >= 0) // if partly offscreen
    {
      D0W = sw(D0W - D3W); // reduce height to fit
    };
  };
  D2L = (i32)A2;
  SWAP(D2);
  if (D2W != 0) // if left of screen
  {
tag002df8:
    D1W -= 2;
    if (D1W == 0)
    {
      A2 = (aReg)0x002eb2;
      D5W = 152;
      goto tag002e42;
    };
    A2 = (aReg)0x002e86;
    D5W = 144;
    goto tag002e42;
  }
  else
  {
    // cursorWidth + cursorX - 336
    // Assme offset might be as great as 15
    D3W = sw((D1W<<4)+D6W-336);
    // D3W = how far it spills over the right margin
    if (D3W >= 0)
    {
      A1 -= 8; //
      goto tag002df8;
    };
    D1W -= 2;
    if (D1W < 0)
    {
      A2 = (aReg)0x002eb4;
      D5W = 152;
      goto tag002e42;
    }
    else
    {
      if (D1W == 0)
      {
        A2 = (aReg)0x002e88;
        D5W = 144;
        goto tag002e42;
      };
      A2 = (pnt)0x002e5c;
      D5W = 136;
    };
  };
tag002e42:
  D6W = (I16)((D6W >> 1)&0x7ff8);
  D7W <<= 5;
  D6W = sw(D6W + D7W);
  D7W = sw(D7W + D7W);
  D7W = sw(D7W + D7W);
  D6W = sw(D6W + D7W); // D6W += 5*(D7W*32);
  A0 = (aReg)d.LogicalScreenBase + D6W;
  D7W = D0W;

tag002e5a:
  switch((i32)A2)
  {
  case 0x002e5c: goto tag002e5c;
  case 0x002e86: goto tag002e86;
  case 0x002e88: goto tag002e88;
  case 0x002eb4: goto tag002eb4;
  case 0x002eb2: goto tag002eb2;
  default: NotImplemented(0x2e5a);
  };
tag002e5c:
  D0W = LE16(wordGear(A1+0));
  D1W = LE16(wordGear(A1+2));
  D2W = LE16(wordGear(A1+4));
  D3W = LE16(wordGear(A1+6));
  D4W = (UI16)(((D2W & D3W) ^ 0xffff) | D0W | D1W);
  //SWAP(D0);
  D0H1 = D0H2;
  D0W = D1W;
  //SWAP(D2);
  D2H1 = D2H2;
  D2W = D3W;
  D3W = D4W;
  //SWAP(D4);
  D4H1 = D4H2;
  D4W = D3W;
  D0L &= D4L;
  D2L &= D4L;
  D4L ^= 0xffffffff;
  longGear((ui8 *)A0) &= LE32(D4L);
  longGear((ui8 *)A0) |= LE32(D0L);
  A0 += 4;
  longGear((ui8 *)A0) &= LE32(D4L);
  longGear((ui8 *)A0) |= LE32(D2L);
  A0 += 4;
tag002e86:
  A1 += 8;
tag002e88:
  D0W = LE16(wordGear(A1));
  D1W = LE16(wordGear(A1+2));
  D2W = LE16(wordGear(A1+4));
  D3W = LE16(wordGear(A1+6)); // 16 pixels colors D3->D0
  D4W = (UI16)(((D2W & D3W)^0xffff)|D0W|D1W); //0 where color=12 (transparent)
  //SWAP(D0); //D0 = (D0UW&0xffff)|((D0W&0xffff)<<16);
  D0H1 = D0H2;
  D0W = D1W;
  //SWAP(D2); //D2 = (D2U&0xffff)|((D2W&0xffff)<<16);
  D2H1 = D2H2;
  D2W = D3W;
  D3W = D4W;
  //SWAP(D4); //D4 = (D4U&0xffff)|((D4W&0xffff)<<16);
  D4H1 = D4H2;
  D4W = D3W;
  D0L &= D4L;
  D2L &= D4L;
  D4L ^= -1;
  wordGear(A0+0) &= LE16(D4H1);
  wordGear(A0+2) &= LE16(D4H2);
  wordGear(A0+0) |= LE16(D0H1);
  wordGear(A0+2) |= LE16(D0H2);
  A0 += 4;
  wordGear(A0+0) &= LE16(D4H1);
  wordGear(A0+2) &= LE16(D4H2);
  wordGear(A0+0) |= LE16(D2H1);
  wordGear(A0+2) |= LE16(D2H2);
  A0 +=4;
tag002eb2:
  A1 += 8;
tag002eb4:
  D0W = LE16(wordGear(A1));
  D1W = LE16(wordGear(A1+2));
  D2W = LE16(wordGear(A1+4));
  D3W = LE16(wordGear(A1+6));
  A1 += 8;
  D4W = (UI16)(((D2W & D3W) ^ 0xffff)|D0W|D1W);
  //SWAP(D0); // = (D0UW&0xffff)|((D0W&0xffff)<<16);
  D0H1 = D0H2;
  D0W = D1W;
  D2H1 = D2W;
  D2W = D3W;
  D3W = D4W;
  D4H1 = D4W;
  D4W = D3W;
  D2L &= D4L;
  D0L &= D4L;
  D4L ^= -1;
  wordGear(A0+0) &= LE16(D4H1);
  wordGear(A0+2) &= LE16(D4W);
  wordGear(A0+0) |= LE16(D0H1);
  wordGear(A0+2) |= LE16(D0W);
  A0 += 4;
  wordGear(A0+0) &= LE16(D4H1);
  wordGear(A0+2) &= LE16(D4W);
  wordGear(A0+0) |= LE16(D2H1);
  wordGear(A0+2) |= LE16(D2W);
  A0 += 4;
  A0 += D5W;
  D7W--;
  if (D7W >= 0)  goto tag002e5a;
}


void TAG00306c()
{
  NotImplemented(0x306c);
}

/*
void TAG0030e6(i32 divisor,
               i32 dividend,
               i32 *quotient,
               i32 *remainder)
{
  dReg D0, D1, D2, D3;
  i32 LOCAL_2;
  i32 carry;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D2L = divisor;
  if (divisor == 0)
  if (quotient != NULL) *quotient = 0;
  if (remainder != NULL) *remainder = 0;
  return;
  D1L = dividend;
  LOCAL_2 = 0;
  if (D1L < 0)
  {
    LOCAL_2 += 3;
    D1L = -D1L;
  };
//003110 4a2                     TST.L    D2
  if (D2L < 0)
  {
    LOCAL_2 += 1;
    D2L = -D2L;
  };
  D3L = 1;
  for (;;)
  {
    if (D2L >= D1L) break;
    D2L <<= 1;
    D3L <<= 1;
  };
  D0L = 0;
  do
  {
    if (D2L <= D1L)
    {
      D0L |= D3L;
      D1L -= D2L;
    };
    D2L >>= 1;
    carry = D3L & 1;
    D3L >>= 1;
  } while (carry==0);
  while (carry==0);
  if (LOCAL_2>=3)
  {
    D1L = -D1L;
  };
  if (D2L & 1)
  {
    D0L = -D0L;
  };
  if (quotient != NULL) *quotient = D0L;
  if (remainder != NULL) *remainder = D1L;
}
*/

void MoveWords(aReg& A0, aReg A1, i32 numM1) // 0031c4
{ // Uses A0, A1, and D0
  // Move D0+1 words from (A1) to (A0) and increment A0
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//  dReg D0;
//  aReg A0, A1;
  //pnt saveA1=A1;
  memcpy (A0, A1, 2*(numM1+1));
  A0 += 2*(numM1+1);

  //while (numM1 >= 0)
  //{
  //  wordGear(A0) = wordGear(A1);
  //  A0 += 2;
  //  A1 += 2;
  //  numM1--;
  //}
  //A1 = saveA1;
}


/*
//   TAG0031d6
void SearchWordNE(dReg *D0,i16 **A0,i16 *A1)
{ // Search two word strings for 'Not Equal'. Update D0, A0
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  do
  {
    if (*((*A0)++) != *(A1++)) break;
  } while (--(D0->W) != -1);
}
*/


//  TAG0031e8
pnt StrCat(char *msg1, const char *msg2)
{
  aReg A3;
  pnt LOCAL_4;
  const char *A2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A3 = (aReg)msg1;
  A2 = msg2;
  LOCAL_4 = A3;
  while ( *(A3++) != 0) {};
  A3--;
  while ( (*(A3++) = *(A2++)) != 0) {};
  return LOCAL_4;
}

//  TAG003222
i16 StrCmp(char *s1, char *s2)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  while ( (*s1!=0) && (*s1 ==*s2))
  {
    s1++;
    s2++;
  };
//
//

  return sw((UI8)(*s1) - (UI8)(*s2));
}


//  TAG00327c
pnt StrCpy(char *dest, const char *src, i16 maxChar)
{
  dReg D0, D7;
  aReg A3;
  const char *A2;
  pnt LOCAL_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A3 = (aReg)dest;
  A2 = src;
  D7W = maxChar;
  LOCAL_4 = A3;
  do
  {
    D0W = D7W;
    D7W--;
    if (D0B == 0) break;
    D0B = *(A2++);
    *(A3++) = D0B;
  } while (D0B != 0);
  return LOCAL_4;

}

//  TAG0032ba
i16 StrLen(const char *string)
{
  const char *A3, *A2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A3 = string;
  A2 = A3;
  while (*A3 != 0)
  {
    A3++;
  };
  return sw(A3-A2);
}

//  TAG0032e4
const char *StrChr(const char *P1,i16 P2)
{ // Find character in string.  Returns address or NULL
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D0, D6, D7;
  const char *A3;
  A3 = P1;
  D7W = P2;
  goto tag0032fa;
tag0032f8:
  A3 += 1;
tag0032fa:
  D0B = *A3;
  D6B = D0B;
  if (D6B == 0) goto tag00330c;
  D0W = D6UB;
  if (D0W != D7W) goto tag0032f8;
tag00330c:
  D0B = D6B;
  if (D0B == 0) goto tag003318;
  return A3;
tag003318:
  return NULL;
}
// *********************************************************
//
// *********************************************************
//   TAG003796
void ModifyPalettes(i32 whichColor,i32 newColor)
{
  dReg D0, D5;
  //D7W = whichColor;
  //D6W = sw(P2);
  for (D5W=0; D5W<6; D5W++)
  {
    D0W = LE16(d.specialColors[newColor][D5W]);
    ASSERT(D0W <= 0x777,"D0W");
    d.Palette552[D5W].color[whichColor] = D0W;
//
  };
  D0W = d.specialColors[newColor][6]; // first byte
  d.Byte6628[whichColor] = D0B;
  //D0B = d.Word6596[D6W][6]; // least significant byte
  d.Byte6612[whichColor] = sb(D0W>>8);

}

//   TAG00381a
void ReadFloorAndCeilingBitmaps(i16 P1)
{
  if (P1 != d.CurrentFloorAndCeilingGraphic)
  {
    d.CurrentFloorAndCeilingGraphic = P1;
    ReadAndExpandGraphic(P1 * 2 + 75, d.pFloorBitmap, 0,0, FLOOR_BITMAP_SIZE);
    ReadAndExpandGraphic(P1 * 2 + 76, d.pCeilingBitmap, 0,0, CEILING_BITMAP_SIZE);
  };
}

//   TAG00386a
void ReadWallBitmaps(i16 newWallGraphicSet)
{
  i32 i;
  i32 firstWallGraphic;
  if ( (newWallGraphicSet != d.CurrentWallGraphic) || (d.PartyHasDied != 0) )
  {
    d.CurrentWallGraphic = newWallGraphicSet;
    firstWallGraphic = sw(77 + 13 * newWallGraphicSet);
    for (i=0; i<7; i++)
    {
      ReadAndExpandGraphic(firstWallGraphic + i, d.pDoorBitmaps[i+1], 0, 0);
    };
    for (i=0; i<6; i++)
    {
      ReadAndExpandGraphic(firstWallGraphic + 7 + i, d.pWallBitmaps[i], 0, 0);
    };

    MakeMirror(d.pDoorBitmaps[2],
               d.pDoorBitmaps[0],                    //d.Pointer2142,
               //((RectPos *)(&d.Byte3138))->b.uByte4, //(UI8)(d.Byte3134),
               d.DoorFrameRect[0].b.uByte4, //(UI8)(d.Byte3134),
               //((RectPos *)(&d.Byte3138))->b.uByte5);//(UI8)(d.Byte3133));
               d.DoorFrameRect[0].b.uByte5);//(UI8)(d.Byte3133));
    MakeMirror(d.pWallBitmaps[5],
               d.pWallBitmaps[6],             //d.Pointer2086,
               d.wallRectangles[12].b.uByte4,  //(UI8)(d.Byte2978[4]),
               d.wallRectangles[12].b.uByte5); //(UI8)(d.Byte2978[5]));
  };
}
// *********************************************************
//
// *********************************************************
//   TAG0042da
void MarkViewportUpdated(i16 MultiplePalettes)
{ //()
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (MultiplePalettes == 2)
  {
    d.Word11740 = d.DynamicPaletteSwitching;
  }
  else
  {
    //if ((MultiplePalettes==0) && (d.Word11740!=0))
    //  memmove((pnt)palette,(pnt)&d.Palette11978,32);
    d.Word11740 = MultiplePalettes;
  };
  d.ViewportUpdated = 1; //Force VBL to copy Viewport to Screen
  vblInterrupt();// No increment of VBLCount!//wvbl(_1_);
}

void MakeMirror(ui8 *src, ui8 *dst, i32 width, i32 height) //TAG00434a
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D0L = (ui16)width * (ui16)height;
  MemMove((ui8 *)src,(ui8 *)dst,D0W);
  Mirror(dst,width,height);
}

//   TAG00437e
void BltShapeToViewport(ui8 *P1,RectPos *P2)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D0;
//  aReg A3;
  // A3 = P2;
  ASSERT(verifyRectPos(P2),"rectpos");
  D0W = (UI8)(P2->b.uByte4);
  if (D0B != 0)
  {
    TAG0088b2((ui8 *)P1,                   //src
              (ui8 *)d.pViewportBMP,       //dst
              P2,                   //destination RectPos
              (UI8)(P2->b.uByte6),  //srcX
              (UI8)(P2->b.uByte7),  //srcY
              (UI8)(P2->b.uByte4),  //srcWidth
              112,                  //dstWidth
              10);                  //transparent color
  };
}

//   TAG0043c4
void BltRectangleToViewport(ui8 *src,RectPos *P2)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //pnt A3;
  //A3 = P2;
  ASSERT(verifyRectPos(P2),"rectpos");
  if (P2->b.uByte4 != 0)
  {
    TAG0088b2((ui8 *)src,
              (ui8 *)d.pViewportBMP,
              P2,
              (UI8)(P2->b.uByte6),
              (UI8)(P2->b.uByte7),
              (UI8)(P2->b.uByte4),
              112,
              -1);
  };
}

// *********************************************************
//
// *********************************************************
//   TAG00440a
void BltTempToViewport(RectPos *rect)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (rect->b.uByte4 == 0) return;
  TAG0088b2(g_tempBitmap.get(),
            (ui8 *)d.pViewportBMP,
            rect,
            rect->b.uByte6,
            rect->b.uByte7,
            rect->b.uByte4,
            112,
            10);
}

// *********************************************************
//
// *********************************************************
//   TAG004450
void MirrorShapeBltToViewport(ui8 *P1,RectPos *P2)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //if (A3[4] == 0) return;
  if (P2->b.uByte4 == 0) return;
  //Mirror(P1,
  //       (UI8)(A3[4]),
  //       (UI8)(A3[5]) );
  Mirror(P1,
         P2->b.uByte4,
         P2->b.uByte5 );
  TAG0088b2((ui8 *)P1,
            (ui8 *)d.pViewportBMP,
            P2,
            P2->b.uByte6,//(UI8)(A3[6]),
            P2->b.uByte7,//(UI8)(A3[7]),
            P2->b.uByte4,//(UI8)(A3[4]),
            112,
            10);
}

//   TAG0044b0
void GraphicToViewport(i32 P1,RectPos *rect)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ASSERT(verifyRectPos(rect),"rectpos");
  if (rect->b.uByte4 == 0) return;
  TAG0088b2( GetBasicGraphicAddress(P1),
             (ui8 *)d.pViewportBMP,
             rect,
             rect->b.uByte6,
             rect->b.uByte7,
             rect->b.uByte4,
             112,
             10);
}

//   TAG0044fe
void MirrorGraphicToViewport(i32 graphic, RectPos *rectpos)
{
  ASSERT(verifyRectPos(rectpos),"rectpos");
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (rectpos->b.uByte4 == 0) return;
  MakeMirror(GetBasicGraphicAddress(graphic),
             g_tempBitmap.get(),
             rectpos->b.uByte4,
             rectpos->b.uByte5);
  TAG0088b2(g_tempBitmap.get(),
            (ui8 *)d.pViewportBMP,
            rectpos,
            rectpos->b.uByte6,
            rectpos->b.uByte7,
            rectpos->b.uByte4,
            112,
            10);
}

void TAG00456c()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if ((ui16)d.Word1842 < 5)
  {
    if (d.Word1870 == 0)
    {
//      if (d.Word23228 == 0)  // It was ALWAYS zero.  I removed it.
      {
        if ((d.Word11722 & 1) == 0)
        {
          d.Word11722 |= 1;
          d.Word1842++;
        };
      };
// Always zero.  I removed it.      d.Word11724 &= 0xfffe;
      d.Word1836 = 15;
      d.Word1870 = 1;
    };
  };
}


DECORATION_DESC *ReadWallDecoration(i32 id)
{
  ui32 actualSize;
  ui8 *result;
  result = ReadCSBgraphic(CGT_WallDecoration,
                          id,
                          92,
                          &actualSize,
                          true,
                          MALLOC120);
  if (actualSize == 0) 
  {
    if (result != NULL) UI_free(result);
    return NULL;
  };
  return (DECORATION_DESC *)result;
}


class WALL_DEC_CACHE
{
  ui32 m_index;
  ui8 *m_bitmap;
public:
  WALL_DEC_CACHE(){m_index=0xffffffff;m_bitmap=NULL;};
  ~WALL_DEC_CACHE(){if(m_bitmap!=NULL)UI_free(m_bitmap);};
  ui32 AllocateDerivedGraphic(ui32 index, DECORATION_DESC *pRect);
  ui8 *GetDerivedGraphicAddress(ui32 /*index*/){return m_bitmap;};
  void Cleanup(){if(m_bitmap!=NULL)UI_free(m_bitmap);m_bitmap=NULL;};
};



ui32 WALL_DEC_CACHE::AllocateDerivedGraphic(ui32 index, DECORATION_DESC *pRect)
{
  if (index == m_index) return 1;
  {
    i32 size;
    if (m_bitmap != NULL) UI_free(m_bitmap);
    m_index = index;
    size = pRect->width * pRect->height;
    m_bitmap = (ui8 *)UI_malloc(size, MALLOC109);
  };
  return 0;
}


WALL_DEC_CACHE wallDecCache;
void CleanupWallDecorations()
{
  wallDecCache.Cleanup();
}

// *************************************************
//
//  SEE DrawViewport for relative position definitions
//
//
// *************************************************
//  TAG0045ae
i16 DrawWallDecoration(i32 graphicOrdinal,
                       i32 relativePosition,
                       RN text,
                       i32 championPortraitOrdinal,
                       bool removeJoint)
{ //We also draw wall text.
  //  *****************************************************************
  //  ** We have modified this to handle graphics from CSBgraphics.dat.
  //  ** This function notices 'graphicOrdinal's that are greater than
  //  ** 0x10000.  In such a case, we do not use the ordinary means of
  //  ** translating this to a 'graphicID' (by looking in the table
  //  ** of wall decorations for the level) but instead use the
  //  ** (graphicOrdinal - 0x10000) directly as the graphic number
  //  ** of a graphic in CSBgraphics.dat.
  //  ** 
  //  ** The graphic in CSBgraphics,dat contains the DECORATION_DESC
  //  ** of the 13 possible placements of the decoration plus a 
  //  ** DECORATION_DESC of the two views of the wall decoration 
  //  ** (side and front) followed by the bitmaps themselves.
  //  *****************************************************************
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D0, D3;
  i32 graphicID;
  i32 graphicIndex;
  i32 derivedGraphicIndex;
  aReg A0, A3;
  ui8 *pFinalBitmap;
  char ExpandedText[1000]; //used to be [70]
  //ui8 LOCAL_22[8];
  RectPos LOCAL_22;
  i16 LOCAL_14;
  //i16 LOCAL_12;
  bool isAlcove;
  bool drawingText=false;// LOCAL_10;
  //i16 LOCAL_8;
  bool farRightSidewall; // Positions 1 or 6
  //i16 LOCAL_6;
  pnt LOCAL_4;
  DECORATION_DESC *pWDP_4;
  //bool bD0;
  i32 srcByteWidth;
  i32 finalBitmapWidth;
  i32 srcOffsetX;
  i32 graphicRectIndex; // Where to put bitmap on screen
  bool extendedWallDec;
  DECORATION_DESC *pWDP, *pSrcSizeDescriptor;
  DECORATION_DESC *pWidthDescriptor; // The rectangle used to determine destination graphic width.
  DECORATION_DESC *pExtWallRects;
  ui16 *pSourceRect;
  DECORATION_DESC *pExtSrcRect = NULL;
  ui8  *pExtendedBitmap;
  extendedWallDec = (graphicOrdinal & 0x10000) != 0;
  graphicIndex = graphicOrdinal; // Graphic index for this level.
  if (!extendedWallDec) graphicIndex--;
  // Otherwise, the graphic Index is equal to the graphic Ordinal.
  drawingText = (graphicIndex == d.NumWallDecoration);
  if (drawingText && (text == RNeof)) return 0;
  //D6W = P2;
  if ((graphicOrdinal==0) && (championPortraitOrdinal==0))return 0;
  pExtWallRects = NULL; //Keep compiler quiet.
  pExtendedBitmap = NULL; //Keep compiler quiet
  pSourceRect = NULL; // Keep compiler quiet
  if (graphicIndex >= 0)
  {
    if (extendedWallDec)
    {
      graphicID = graphicOrdinal & ~0x10000;
      graphicRectIndex = 0;  // There is only one!
      pExtWallRects = ReadWallDecoration(graphicID);
      if (pExtWallRects == NULL) 
      {
        return 0;
      };
      pWDP = &pExtWallRects[relativePosition];
      pExtendedBitmap = 2 + (ui8 *)&pExtWallRects[13];
      pExtSrcRect = &pExtWallRects[11];
      if ((relativePosition % 5) > 1)
      { // point to front view bitmap
        pExtendedBitmap += pExtWallRects[11].width * pExtWallRects[11].height;
        pExtSrcRect = &pExtWallRects[12];
      };
      isAlcove = false;
    }
    else
    {
      graphicID = d.WallDecorationsForLevel[graphicIndex][0]; // Assume front view
      graphicRectIndex = d.WallDecorationsForLevel[graphicIndex][1];
    //D1L = 78 * D4W;
    //A3 = d.Byte4836[0][0] + D1L + 6*relativePosition;
      pWDP = &d.wallDecorationPosition[graphicRectIndex][relativePosition];
      isAlcove = IsAlcove(graphicIndex);//if it one of three special??
    };
    if (drawingText)
    {
      DecodeText(ExpandedText,
                 //GetRecordAddressDB2(d.rn10540),
                 GetRecordAddressDB2(text),
                 0,
                 990);
    };
    TAG00456c();
  }
  else
  { //To keep the compiler quiet!
    graphicID = 0;
    pWDP = NULL;
    isAlcove = false;
    graphicRectIndex = 0x80000000;
  };
  if (relativePosition >= 10)
  { // Any of the F1 cells.
    srcOffsetX = 0;
    if (relativePosition == 12) //Wall immediately in front of party
    {
      if (drawingText)
      {
        if (removeJoint)
        {
          TAG0088b2((ui8 *)d.pWallBitmaps[2],  // src
                    (ui8 *)d.pViewportBMP, // dst
                    (RectPos *)d.Byte4192,     // 0x00,0x4a,0x14,0x5a
                    94,             // src x
                    28,             // src y
                    //d.uByte3026[4], // src byte width
                    d.wallRectangles[6].b.uByte4, // src byte width
                    112,            // dst byte width
                    -1);
        };
        A3 = (aReg)ExpandedText;
        LOCAL_4 = (pnt)GetBasicGraphicAddress(120);
        LOCAL_14 = 0;
        do
        {
          pnt A2;
          i32 i, lineLength;
          lineLength = 0;
          A2 = A3;
          while ((UI8)(*(A2++)) < 128) lineLength++; //Count characters in this line
          if (lineLength > 18) lineLength = 18;
          LOCAL_22.b.x1 = ub(112 - 4*lineLength);
          LOCAL_22.b.x2 = ub(LOCAL_22.b.x1 + 7);
          LOCAL_22.b.y2 = ub(*(d.Byte4196+(LOCAL_14++)));
          LOCAL_22.b.y1 = ub(LOCAL_22.b.y2 - 7);
          for (i=0; i<lineLength; i++)
          {
            TAG0088b2((ui8 *)LOCAL_4,
                      (ui8 *)d.pViewportBMP,
                      &LOCAL_22,
                      8*(UI8)(*(A3++)),
                      0,
                      144,
                      112,
                      10);
            LOCAL_22.b.x1+=8;
            LOCAL_22.b.x2+=8;
          };
          A3 = A2 - 1; //Go to terminator at end of line.
        } while ((UI8)(*(A3++)) != 129);
        if (pExtWallRects != NULL) UI_free(pExtWallRects);
        return isAlcove;
      }; //if (LOCAL_10 != 0) End of Drawing text.
      if (graphicIndex >= 0)
      {
        i32 n;
        graphicID++;
        //MemMove(A3, (pnt)d.ViewportObjectButtons[5], 4); // ?????
        MemMove((ui8 *)pWDP, (ui8 *)&d.ViewportObjectButtons[5], 4); // ?????
        d.FacingAlcove = isAlcove;
        d.FacingViAltar = (d.Word1870!=0) && (graphicIndex==d.OrnateAlcoveGraphicIndex);
        d.FacingWaterFountain = 0;
        for (n=0; n<1; n++)
        {
          D0W = d.FountainGraphicIndex[n];
          if (graphicIndex == D0W)
          {
            d.FacingWaterFountain = 1;
            break;
          };
        };
      };
    } // if (relativePosition == 12)
    else
    {
      if (graphicIndex < 0)
      {
        if (pExtWallRects != NULL) UI_free(pExtWallRects);
        return 0;
      };
    };
    pFinalBitmap = NULL;
    if (graphicIndex >= 0)
    {
      if (extendedWallDec)
      {
        pFinalBitmap = pExtendedBitmap;
      }
      else
      {
        pFinalBitmap = GetBasicGraphicAddress(graphicID); // Get graphic bitmap address
      };
      if (relativePosition == 11)
      {
        MakeMirror(pFinalBitmap,
                   g_tempBitmap.get(),
                  //(UI8)(*(A3+4)),
                  //(UI8)(*(A3+5)));
                   pWDP->width,
                   pWDP->height);
        pFinalBitmap = g_tempBitmap.get();
      };
      graphicRectIndex = 0;
    };
  }
  else
  { // relativePosition < 10 (F2 and farther)
    if (graphicIndex < 0) 
    {
      if (pExtWallRects != NULL) UI_free(pExtWallRects);
      return 0;
    };
    //LOCAL_6 = 0;
    pWidthDescriptor = pWDP;
    //bD0 = relativePosition == 6;
    //if (!bD0)
    //{
    //  bD0 = relativePosition == 1;
    //};
    farRightSidewall = (relativePosition==6)||(relativePosition==1);
    //  LOCAL_8 means sidewalls to our right
    if (farRightSidewall)
    {
      //A2 = d.Byte4776 + 78*D4W + 6;
      //A2 = d.Byte4836[0][0] + 60 + 78*D4W + 6;
      if (extendedWallDec)
      {
        pSrcSizeDescriptor = &pExtWallRects[11];
      }
      else
      {
        pSrcSizeDescriptor = &d.wallDecorationPosition[graphicRectIndex][11];
      };
    }
    else
    {
      if ( (relativePosition==5) || (relativePosition==0) )
      { //sidewalls to our left.
        //A2 = d.Byte4776 + 78*D4W;
        //A2 = d.Byte4836[0][0] + 60 + 78*D4W;
        if (extendedWallDec)
        {
          pSrcSizeDescriptor = &pExtWallRects[10];
        }
        else
        {
          pSrcSizeDescriptor = &d.wallDecorationPosition[graphicRectIndex][10];
        };
      }
      else
      { //front wall at F2 or F3 left, center, or right
        if (extendedWallDec)
        {
          //pExtendedBitmap += pExtWallRects[11].width*pExtWallRects[11].height;
          pSrcSizeDescriptor = &pExtWallRects[12];
          //pSourceRect = (ui16 *)&pExtWallRects[11];
        }
        else
        {
          graphicID++;
        //A2 = (d.Byte4776 + 12) + 78*D4W;
        //A2 = (d.Byte4836[0][0] + 60 + 12) + 78*D4W;
          pSrcSizeDescriptor = &d.wallDecorationPosition[graphicRectIndex][12];
        };
        if ((relativePosition == 7) || (relativePosition == 9))
        {
          //LOCAL_6 = 6;
          //LOCAL_6 = 1;
          if (extendedWallDec)
          {
            pWidthDescriptor = &pExtWallRects[8];
          }
          else
          {
            pWidthDescriptor = &d.wallDecorationPosition[graphicRectIndex][8];
          };
        }
        //else
        //{
        //  if (relativePosition == 9)
        //  {
        //    //LOCAL_6 = -6;
        //    //LOCAL_6 = -1;
        //    pWidthDescriptor = &d.wallDecorationPosition[graphicRectIndex][8];
        //  };
        //}; // not 7
      }; 
    };
    //A0 = A3 + LOCAL_6;
    //pWidthRect = pWDP+LOCAL_6;
    //D4W = ((ui8 *)(A0))[1]; //right edge
    //D4W = pWDP_A0->rect.x2;
    //A0 = A3 + LOCAL_6;
    //D3W = ((ui8 *)A0)[0];  //left edge
    //D3W = pWDP_A0->rect.x1;
    //D4W = sw(D4W - D3W);   //width
    finalBitmapWidth = pWidthDescriptor->rect.x2 - pWidthDescriptor->rect.x1;
    //D0L = 4*graphicIndex + 4;
    //A0 = d.Byte4024 + relativePosition;
    //D3W = (UI8)(A0[0]);
    //D3W = d.WallDecorationDerivedGraphicOffset[relativePosition]; // 0, 1, 2, 3, 4
    //D0W = sw(D0W + D3W);
    //
    //There are four derived graphic types.
    if (extendedWallDec)
    {
      derivedGraphicIndex = (graphicIndex & 0xffff) | (relativePosition << 16);
      D0L = wallDecCache.AllocateDerivedGraphic(derivedGraphicIndex, pWDP);
    }
    else
    {
      derivedGraphicIndex =   4 + 4*graphicIndex
                         + d.WallDecorationDerivedGraphicOffset[relativePosition];
                    // 0 = F3 (left and right sides)
                    // 1 = F3L, F3C, and F3R (front side)
                    // 2 = F2 (left and right sides)
                    // 3 = F2L, F2C, and F2R (front sides)
      D0W = AllocateDerivedGraphic(derivedGraphicIndex);
    };
    if (D0W == 0)
    {
      {
        ui8 *pBasicBitmap;
        ui8 *pColorMap;
        //LOCAL_4 = GetBasicGraphicAddress(graphicID);
        if (extendedWallDec)
        {
          pBasicBitmap = pExtendedBitmap;
        }
        else
        {
          pBasicBitmap = GetBasicGraphicAddress(graphicID);
        };
        if (relativePosition <= 4)
        {
          pColorMap = d.FarColorMapping;
        }
        else
        {
          pColorMap = d.MediumColorMapping;
        };
//
//
//
//
//
        {
          i32 width;
          i32 height;
          if (extendedWallDec)
          {
            width = pExtSrcRect->rect.x2 - pExtSrcRect->rect.x1 + 1;
            height = pExtSrcRect->height;
          }
          else
          {
            pSourceRect = (ui16 *)d.ppUnExpandedGraphics[graphicID];
            width = LE16(pSourceRect[0]);        //width in pixels
            height = LE16(pSourceRect[1]);
          };
          width = (width+15)&0xfff0; //Round up to multiple of 16
          width >>= 1;               //Width in bytes
          //if (width > A2[4])  width = A2[4];
          //if (height > A2[5]) height = A2[5];
          if (width > pSrcSizeDescriptor->width)  width = pSrcSizeDescriptor->width; //width in bytes
          if (height > pSrcSizeDescriptor->height) height = pSrcSizeDescriptor->height;
          //ShrinkBLT(LOCAL_4,                                 //Source
          ShrinkBLT(pBasicBitmap,                                 //Source
                    extendedWallDec
                      ?wallDecCache.GetDerivedGraphicAddress(derivedGraphicIndex)
                      :GetDerivedGraphicAddress(derivedGraphicIndex),  //Destination
                    2*width,      //2*(UI8)(A2[4]),          //source width in pixels
                    height,       //(UI8)(A2[5]),            //source height
                    //D4W+1,             
                    finalBitmapWidth+1,                      //destination width in pixels
                    //(UI8)(A3[5]),
                    (UI8)pWDP->height,                       //destination height
                    pColorMap);
        };
      };
      //TAG022d5e(graphicIndex);
    };
    if (extendedWallDec)
    {
      pFinalBitmap = (ui8 *)wallDecCache.GetDerivedGraphicAddress(derivedGraphicIndex);
      if (pFinalBitmap == NULL)
      {
        if (pExtWallRects != NULL) UI_free(pExtWallRects);
        return 0;
      };
    }
    else
    {
      pFinalBitmap = (ui8 *)GetDerivedGraphicAddress(derivedGraphicIndex); //Graphic address
    }
    if (farRightSidewall)
    {
      //MakeMirror(A2,d.Pointer1848,(UI8)(A3[4]),(UI8)(A3[5]));
      MakeMirror(pFinalBitmap,g_tempBitmap.get(),(UI8)pWDP->width,(UI8)pWDP->height);
      pFinalBitmap = g_tempBitmap.get();
      //D4W = (I16)(15 - (D4W&15));
      srcOffsetX = 15 - (finalBitmapWidth & 15);
    }
    else
    {

      // Jst ahead to the left.  Truncate graphic
      if (relativePosition == 7)
      {
        //D0W = (UI8)(A3[1]);
        //D3W = (UI8)(A3[0]);
        D0W = (UI8)(pWDP->rect.x2);
        D3W = (UI8)(pWDP->rect.x1);
        D0W = sw(D0W - D3W);
        //D4W = sw(D4W - D0W);
        srcOffsetX = finalBitmapWidth - D0W;
      }
      else
      {
        srcOffsetX = 0;
      };
    };
  }; //(relativePosition < 10)
  if (drawingText)
  {
    i32 numLine;
    //LOCAL_4 = A3;  //RECTPOS
    pWDP_4 = pWDP;
    A3 = (aReg)ExpandedText; //The text
    numLine = 0;
    do
    {
      while ((UI8)(*A3) < 128) A3++; //Find End-of-line


      numLine++; //Count lines
    } while ((UI8)(*(A3++)) != 129); //Find End-of-text
//    if (TimerTraceActive)
//    {
//      fprintf(GETFILE(TraceFile),
//        "A3->{%x,%x,%x,%x,%x,%x,%x,%x}"
//        "Byte4204=%x",
//        (UI8)
//
//    };
    //A3 = LOCAL_4; //RECTPOS
    pWDP = pWDP_4;
    if (numLine < 4)
    {
      MemMove((ui8 *)A3, (ui8 *)&LOCAL_22, 8); //Why 8?
      //A3 = &LOCAL_22; //Local copy of RECTPOS + 2 bytes
      A0 = d.WallDecorationDerivedGraphicOffset; //0,0,1,1,1,2,2,3,3,3,4,4 // Which of the 5 derived graphics to use
      D0L = 3*((UI8)(*(A0+relativePosition)));//Try adding P2. Seems Reasonable
      D0W = sw(D0W + numLine);
      //A0 = d.Byte4213 + D0W;
      A0 = d.Byte4212 - 1 + D0W;
      // *(A3+3) = *A0;
      LOCAL_22.b.y2 = *A0;
    };
  };
  //
  // We are going to attempt to fix a problem.
  // The wall decoration 'EVIL-OUTSIDE' appears to have
  // the wrong byteWidth in graphic 0x22e.  The table
  // entry says the graphic is 48 bytes wide but it
  // is not....it is 40 bytes wide.  So we will fix
  // that here in the runtime.  Yuk!
  if (graphicIndex >= 0)
  {

    //srcByteWidth = (UI8)(A3[4]);
    //if (   (A3 - d.Byte4836[0][0] == 0x234)
    //     &&((UI8)(A3[0]) == 0x4a)
    //     &&((UI8)(A3[1]) == 0x95)
    //     &&((UI8)(A3[2]) == 0x19)
    //     &&((UI8)(A3[3]) == 0x4b)
    //     &&(srcByteWidth == 48)
    //  )
    srcByteWidth = pWDP->width;
    if (   (pWDP - &d.wallDecorationPosition[0][0] == 0x5e)
         &&(pWDP->rect.x1 == 0x4a)
         &&(pWDP->rect.x2 == 0x95)
         &&(pWDP->rect.y1 == 0x19)
         &&(pWDP->rect.y2 == 0x4b)
         &&(srcByteWidth == 48)
      )
    {
      srcByteWidth = 40;
    };


    //if (   (A3 - d.Byte4836[0][0] == 0x5f)
    //     &&((UI8)(A3[0]) == 0x8b)
    //     &&((UI8)(A3[1]) == 0xdf)
    //     &&((UI8)(A3[2]) == 0x19)
    //     &&((UI8)(A3[3]) == 0x4b)
    //     &&(srcByteWidth == 48)
    //  )
    if (   (pWDP - &d.wallDecorationPosition[0][0] == 0x5f)
         &&(pWDP->rect.x1 == 0x8b)
         &&(pWDP->rect.x2 == 0xdf)
         &&(pWDP->rect.y1 == 0x19)
         &&(pWDP->rect.y2 == 0x4b)
         &&(srcByteWidth == 48)
      )
    {
      srcByteWidth = 40;
    };
  }
  else
  {//Keep compiler quiet.
    srcByteWidth = 0;
  };
  // Hopefully we have broken nothing.  We will see.
  //
  if (GraphicTraceActive)
  {
    if (graphicIndex >= 0)
    {
      fprintf(GETFILE(TraceFile),
        "Drawing decoration\n");
      fprintf(GETFILE(TraceFile),
        "A3 -> %x,%x,%x,%x srcOffsetX=%x srcByteWidth=%x\n",
        (UI8)(pWDP->rect.x1),(UI8)(pWDP->rect.x2),(UI8)(pWDP->rect.y1),(UI8)(pWDP->rect.y2),
        srcOffsetX, srcByteWidth);
    }
    else
    {
      fprintf(GETFILE(TraceFile),
        "Drawing portrait without graphic\n");
    };
  };
  if (graphicIndex >= 0)
  {
    //pWDP->rect.x2--;
    TAG0088b2(pFinalBitmap,  // Pictureframe appears
              (ui8 *)d.pViewportBMP,
              (RectPos *)pWDP, //0x50,0x8f,0x1d,0x47
              //D4W,
              srcOffsetX,
              0,
              srcByteWidth,
              112,
              10); // Transparent color
  };
  if (relativePosition == 12)
  {
    D0L = championPortraitOrdinal;
    championPortraitOrdinal--;
    if (D0W != 0)
    {
      if (championPortraitOrdinal <= 23)
      {
        TAG0088b2(GetBasicGraphicAddress(26),  // Draws portrait
                  (ui8 *)d.pViewportBMP,
                  (RectPos *)d.Byte2546, //0x60,0x7f,0x23,0x3f
                  (championPortraitOrdinal&7)<<5, //offset x
                  29 * BITS3_15(championPortraitOrdinal), //offsety
                  128,
                  112,
                  1);
      }
      else
      {
        TAG0088b2(
            (ui8 *)GetExternalPortraitAddress(championPortraitOrdinal),
            (ui8 *)d.pViewportBMP,
            (RectPos *)d.Byte2546,
            0, //Src offset X
            0, //Src offset Y
            16, //Src Byte Width
            112,//Dst Byte Width
            1);
      };
    };
    //d.ChampionPortraitOrdinal++;
  }
  else
  {
    // July 2010 PRS if (graphicIndex < 0) return 0;
    if (graphicIndex < 0) isAlcove = 0; // Allow pExtWallRects to be free'd
  };
  if (pExtWallRects != NULL) UI_free(pExtWallRects);
  return isAlcove;
}



DECORATION_DESC *ReadFloorDecoration(i32 id)
{
  ui32 actualSize;
  ui8 *result;
  result = ReadCSBgraphic(CGT_FloorDecoration,
                          id,
                          92,
                          &actualSize,
                          true,
                          MALLOC121);
  if (actualSize == 0) return NULL;
  return (DECORATION_DESC *)result;
}


//class FLOOR_DEC_CACHE
//{
//  ui32 m_index;
//  i8  *m_bitmap;
//public:
//  FLOOR_DEC_CACHE(){m_index=0xffffffff;m_bitmap=NULL;};
//  ~FLOOR_DEC_CACHE(){if(m_bitmap!=NULL)UI_free(m_bitmap);};
//  ui32 AllocateDerivedGraphic(ui32 index, DECORATION_DESC *pRect);
//  i8 *GetDerivedGraphicAddress(ui32 /*index*/){return m_bitmap;};
//  void Cleanup(){if(m_bitmap!=NULL)UI_free(m_bitmap);m_bitmap=NULL;};
//};



//ui32 FLOOR_DEC_CACHE::AllocateDerivedGraphic(ui32 index, DECORATION_DESC *pRect)
//{
//  if (index == m_index) return 1;
//  {
//    i32 size;
//    if (m_bitmap != NULL) UI_free(m_bitmap);
//    m_index = index;
//    size = pRect->width * pRect->height;
//    m_bitmap = (i8 *)UI_malloc(size, MALLOC109);
//  };
//  return 0;
//}


//FLOOR_DEC_CACHE floorDecCache;
//void CleanupFloorDecorations()
//{
//  floorDecCache.Cleanup();
//}


//   TAG004a1a
void DrawFloorDecorations(i32 graphicOrdinal,i32 relativeCell)
{
  //dReg D0;
  //aReg A0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D4;
  aReg A3;
  ui32 graphicIndex, graphicNumber, height;
  bool extFloorDec=false;
  int srcOffsetX;
  bool useMirror;
  ui8 *pGraphic;
  DECORATION_DESC *pFDD_A2=NULL, *pDesc=NULL;
  DECORATION_DESC *pGraphicDesc;  //Descriptor of bitmap pGraphic
  DECORATION_DESC *pA3Desc;       //Descriptor of bitmap A3
  DECORATION_DESC *pDisplayDesc;  //Where impage goes in viewport.
  //D7W = P1;
  //D6W = P2;
  if ((graphicOrdinal & 0x4000) != 0)
  {
    graphicOrdinal = graphicOrdinal & ~0x4000;
    pDesc = ReadFloorDecoration(graphicOrdinal & 0x7fff);
    if (pDesc == NULL)
    {
      graphicOrdinal &= 0x8000;
    }
    else
    {
      extFloorDec=true;
    };
  }
  if (!extFloorDec && (graphicOrdinal == 0)) return;
  D4W = (UI16)(graphicOrdinal & 0x8000);
  // This 0x8000 business is to force the drawing of footprints
  // even if the graphicOrdinal is zero.
  if (  (D4W == 0)
      || extFloorDec
      ||((graphicOrdinal = (I16)(graphicOrdinal&0x7fff)) !=0)) // Seems silly
  {
    ui8 *mirrorBitmap = NULL;
    if (extFloorDec)
    {
      ui8 *pColorMap;
      if (relativeCell < 3)
      {
        pColorMap = d.FarColorMapping;
      }
      else
      {
        pColorMap = d.MediumColorMapping;
      };
      graphicIndex = graphicOrdinal;
      graphicNumber = graphicOrdinal;
      pGraphic = (ui8 *)&pDesc[9]+2;  // +2 to get to multiple of 4
      pGraphicDesc = &pDesc[7];
      pFDD_A2 = &pDesc[relativeCell];
      pDisplayDesc = pFDD_A2;
      if (relativeCell < 6)
      {
        if (relativeCell < 3)
        {
          ShrinkBLT(pGraphic,
                    g_tempBitmap.get(),
                    pGraphicDesc->rect.x2 -  pDesc[7].rect.x1 + 1,
                    pGraphicDesc->height,
                    pFDD_A2->rect.x2 - pFDD_A2->rect.x1 + 1,
                    pFDD_A2->height,
                    pColorMap);
          pGraphicDesc = pFDD_A2;
        }
        else
        {
          ShrinkBLT(pGraphic,
                    g_tempBitmap.get(),
                    pGraphicDesc->rect.x2 -  pDesc[7].rect.x1 + 1,
                    pGraphicDesc->height,
                    pDesc[4].rect.x2 - pDesc[4].rect.x1 + 1,
                    pDesc[4].height,
                    pColorMap);
          pGraphicDesc = &pDesc[4];
        };
        pGraphic = g_tempBitmap.get();
      };
    }
    else
    {
      graphicIndex = graphicOrdinal - 1; //Convert ordinal to index
      //D5W = sw(P1 << 2);
      //A0 = &d.Byte2446 + D5W;
      //D5W = wordGear(A0);
      //D5W = d.Byte2446[4 * graphicIndex];
      graphicNumber =
            d.LoadedFloorDecorations[graphicIndex][0]
          + d.RelativeCellGraphicIncrement[relativeCell];
      //D1W = P2;
      //A0 = d.Byte4034 + D1W;
      //D3W = (UI8)(*A0);
      //D3W = d.Byte4034[P2];
      //D3W = d.RelativeCellGraphicIncrement[P2];
      //D5W = sw(D5W + D3W);
      //D0W = P2;
      //D1W = sw(graphicIndex << 2);
      //A0 = &d.Byte2446 + D1W + 2;
      //D1W = wordGear(A0);
      //D1W = d.Byte2446[4*graphicIndex + 2]
      //D1L = 54*D1W;
      //D1L =  54 * d.Byte2446[4*graphicIndex + 2]
      //A0 = d.Byte4998;
      //D1L += (i32)A0;
      //D0L = 6*D0W;
      //D0L = 6 * P2;
      //A2 = (pnt)(d.Byte4998 + 54*d.Word2446[graphicIndex][1] + 6 * P2);
      pFDD_A2 = &d.floorDecorDesc[d.LoadedFloorDecorations[graphicIndex][1]][relativeCell]; 
           //a2 points to rectpos + width + height
      //  if (P2 == 8) goto tag004ab6;
      //  if (P2 == 5) goto tag004ab6;
      //  if (P2 == 2) goto tag004ab6;
      //  if (graphicIndex != 15) goto tag004ae2;
      //  if (d.UseMirrorImages == 0) goto tag004ae2;
      //  if (P2 == 7) goto tag004ab6;
      //  if (P2 == 4) goto tag004ab6;
      //  if (P2 == 1) goto tag004ae2;
      pGraphic = GetBasicGraphicAddress(graphicNumber);
      pGraphicDesc = pFDD_A2;
      pDisplayDesc = pFDD_A2;
    };
    if (extFloorDec)
    {
      ui16 mirrorFlags;
      mirrorFlags = *(ui16 *)&pDesc[9];
      useMirror = (   (   (mirrorFlags >> (d.partyFacing + 9))
                        ^ (mirrorFlags >> relativeCell) 
                      )
                    & 1
                  ) != 0;
    }
    else
    {
      useMirror =
             (relativeCell==8) //F1R1
          || (relativeCell==5) //F2R1
          || (relativeCell==2) //F3R1
          || (    !extFloorDec
               && (graphicIndex==15)   // footprints
               && (d.UseMirrorImages!=0) 
               && ( (relativeCell==7) || (relativeCell==4) || (relativeCell!=1)));
              //               F1L1                 F2L1                 F3L1
    };
    if (useMirror)
    {
      int width;
//tag004ab6:
      //D0W = 0;
      //D0B = *(A2+5);
      //D0B = pFDD_A2->height;
      height = pGraphicDesc->height;//pFDD_A2->height;
      width =  pGraphicDesc->width; //pFDD_A2->width;
      mirrorBitmap = (ui8 *)UI_malloc(width*height, MALLOC113);
      //AllocateTempBitmap(height*width);
      A3 = (aReg)mirrorBitmap;
      MakeMirror(pGraphic,
                 (ui8 *)A3,      // Temp Viewport BMP
                 //(UI8)(A2[4]),               // Width
                 width,
                 height );                  // height
      pA3Desc = pGraphicDesc;
//    goto tag004aec;
    }
    else
    {
//tag004ae2:
      A3 = (aReg)pGraphic;
      pA3Desc = pGraphicDesc;
    };
//tag004aec:
    srcOffsetX = 0;
    if (extFloorDec)
    {
      int fullWidth, partialWidth;
      if (relativeCell == 6)
      {
        fullWidth = pDesc[7].rect.x2 - pDesc[7].rect.x1 + 1;
        partialWidth = pFDD_A2->rect.x2 - pFDD_A2->rect.x1 +1;
        srcOffsetX = fullWidth - partialWidth;
      }
      else if (relativeCell == 3)
      {
        fullWidth = pDesc[4].rect.x2 - pDesc[4].rect.x1 + 1;
        partialWidth = pDesc[3].rect.x2 - pDesc[3].rect.x1 +1;//pFDD_A2->rect.x2 - pFDD_A2->rect.x1 +1;
        srcOffsetX = fullWidth - partialWidth;
      };
    };
    TAG0088b2((ui8 *)A3,
              (ui8 *)d.pViewportBMP,
              (RectPos *)&pDisplayDesc->rect,//(RectPos *)&pDesc[relativeCell].rect, //&pFDD_A2->rect,
              srcOffsetX,
              0,
              //(UI8)(A2[4]),
              pA3Desc->width,//pFDD_A2->width,
              112,
              10);
    if (extFloorDec) UI_free(pDesc);
    if (mirrorBitmap!=NULL) UI_free(mirrorBitmap);
  };
  if (D4W != 0) //Draw footprints on top of everything else
  {
    DrawFloorDecorations(16, relativeCell);
  };
}

// *********************************************************
//
// *********************************************************
void TAG004b26(i32 P1, i32 P2)
{
  dReg D0, D1, D4, D5, D6, D7;
  aReg A2, A3;
  ui8 *pnt_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(P1);
  D6W = sw(P2);
  if (D7W == 0) return;
  D7W--;
  //D5W = wordGear(d.Byte2514 + 4*D7W);
  D5W = d.Word2514[D7W][0];
  //D4W = wordGear(d.Byte2512 + 4*D7W);
  D4W = d.Word2514[D7W][1];
  D1L = 18 * D4W;
  A3 = (aReg)d.Byte5070 + 18*D4W + 6*D6W;
  if (D6W == 2)
  {
    A2 = (aReg)GetBasicGraphicAddress(D5W);
    D6W = 48;
  }
  else
  {
    D7W = sw(2*D7W + 68 + D6W);
    D0W = AllocateDerivedGraphic(D7W);
    if (D0W == 0)
    {
      ui8 *pColorMap;
      ASSERT(D4W < 4,"D4W");
      A2 = (aReg)&d.Byte5058[18*D4W];
      pnt_4 = GetBasicGraphicAddress(D5W);
      if (D6W == 0)
      {
        pColorMap = d.Byte4172;
      }
      else
      {
        pColorMap = d.Byte4188;
      };
      ShrinkBLT(pnt_4,
                GetDerivedGraphicAddress(D7W),
                2*(UI8)(A2[4]),
                (UI8)(A2[5]),
                (UI8)(A3[1]) - (UI8)(A3[0]) + 1,
                (UI8)(A3[5]),
                pColorMap);
      //TAG022d5e(D7W);
    };
    A2 = (aReg)GetDerivedGraphicAddress(D7W);
    if (D6W == 0)
    {
      D6W = 24;
    }
    else
    {
      D6W = 32;
    };
  };
  TAG0088b2((ui8 *)A2,
            g_tempBitmap.get(),
            (RectPos *)A3,
            0,
            0,
            (UI8)(A3[4]),
            D6W,
            9);
}

// *********************************************************
//
// *********************************************************
//   TAG004c5e
void DrawDoorSwitch(i32 P1, i32 P2)
{
  dReg D0, D1, D4, D5, D6, D7;
  aReg A2, A3;
  ui8 *pnt_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = (i16)P1;
  D6W = (i16)P2;
  if (D7W == 0) return;
  D7W -= 1;
  D5W = sw(D7W + 315);
  D4W = (UI8)(d.Byte4124[D7W]);
  A3 = d.Byte5094 + 24*D4W + 6*D6W;
  if (D6W == 3)
  {
    A2 = (pnt)GetBasicGraphicAddress(D5W);
    MemMove((ui8 *)A3, (ui8 *)&d.ViewportObjectButtons[5], 4);
  }
  else
  {
    D0W = sw(2*D7W + 102);
    if (D6W == 0)
    {
      D1W = 0;
    }
    else
    {
      D1W = sw(D6W - 1);
    };
    D7W = sw(D0W + D1W);
    D0W = AllocateDerivedGraphic(D7W);
    if (D0W == 0)
    {
      ui8 *pColorMap;
      A2 = d.Byte5076 + 24*D4W;
      pnt_4 = GetBasicGraphicAddress(D5W);
      if (D6W == 2)
      {
        pColorMap = d.MediumColorMapping;
      }
      else
      {
        pColorMap = d.FarColorMapping;
      };
      ShrinkBLT(pnt_4,
                GetDerivedGraphicAddress(D7W),
                2*(UI8)(A2[4]),
                (UI8)A2[5],
                (UI8)A3[1]-(UI8)A3[0]+1,
                (UI8)A3[5],
                pColorMap);
      //TAG022d5e(D7W);
    };
    A2 = (aReg)GetDerivedGraphicAddress(D7W);
  };
  TAG0088b2((ui8 *)A2,
            (ui8 *)d.pViewportBMP,
            (RectPos *)A3,
            0,
            0,
            (UI8)(A3[4]),
            112,
            10);
}

// *********************************************************
// We got here when we saw a smashed door???
// *********************************************************
//   TAG004d9e
void DrawDoor(i32 DB0index,     //8
              i32 doorState,    //10 //Open. bashed, almost closed, etc.
              i16 *graphicNum,  //12 //Array of two graphic numbers for door types
              i32 graphicSize,  //16
              i32 nearness,     //18 //0=F3, 1=F2, 2=F1
              RectPos *rect)    //20 RectPos[10]
{
  dReg D0;
  i32 doorType;
  DB0  *DB0A2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ASSERT(DB0index < db.NumEntry(dbDOOR),"door");
  //D7W = (i16)P2;
  //A3 = P6;
  if (doorState == 0) return; //Fully open.
  DB0A2 = GetRecordAddressDB0(DB0index);
  doorType = DB0A2->doorType();
//
  MemMove((ui8 *)GetBasicGraphicAddress(graphicNum[doorType]),
          g_tempBitmap.get(), //temporary bitmap
          graphicSize);
  TAG004b26(DB0A2->ornateness(), nearness);
  D0W = d.DoorTOC[doorType];
  if (((D0W & 0x400)!=0) && (doorState!=5)) // Test most significant byte //'Lightning door' not bashed
  {
    if (STRandomBool())
    {
      Mirror(g_tempBitmap.get(), rect[0].b.uByte4, rect[0].b.uByte5);
    };
    if (STRandomBool())
    {
      TAG008840(g_tempBitmap.get(), rect[0].b.uByte4, rect[0].b.uByte5);
    };
  };
  //if (A3 == d.Byte3834)
  if (rect == d.DoorRectsF1)
  {
    if (d.SeeThruWalls != 0)
    {
      TAG004b26(17, 2);
    };
  };
  if (doorState == 4) //Closed
  {
    BltTempToViewport(rect);
  }
  else
  {
    if (doorState == 5)  //Bashed
    {
      TAG004b26(16, nearness);
      BltTempToViewport(rect);
    }
    else
    { //(0=open;  3=almost closed)
      //P2--; 
      if (DB0A2->mode())
      {
        //BltTempToViewport(A3+8+8*(P2-1));
        BltTempToViewport(&rect[1+doorState-1]);
      }
      else
      {
        //BltTempToViewport(A3+32+8*(P2-1));
        BltTempToViewport(&rect[4+doorState-1]);
        //BltTempToViewport(A3+56+8*(P2-1));
        BltTempToViewport(&rect[7+doorState-1]);
      };
    };
  };
}
// *********************************************************
//
// *********************************************************
//   TAG004f04
//void CheckCeilingPit(graphicNum,RectPos *rect,i32 mapX,i32 mapY,bool mirror)
void CheckCeilingPit(i32 relativeCellNumber,i32 mapX,i32 mapY)
{
  static ui8 cell2graphic[12] =
  {
    0,0,0,64,63,63,66,65,65,68,67,67
  };
  static ui8 cell2rect[12] =
  {
    0,0,0,7,8,6,4,5,3,1,2,0
  };
  static ui8 cell2mirror[12] =
  {
    0,0,1,0,0,1,0,0,1,0,0,1
  };
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D0, D7;
//  aReg A0;
  CELLFLAG **ppdA0;
  //CELLFLAG *pdA0;
  //i32 D7;
  D7W = IncrementLevel(d.LoadedLevel,-1,&mapX,&mapY);
  if (D7W >= 0)
  {
    D0W = sw(mapX);
    //D1W = D7W * 4;
    ppdA0 = d.pppdPointer10450[D7W];

    //D0W *= 4;
    D0W = ppdA0[mapX][mapY];
    //A0 = pntGear(A0) + mapY;
    //D0W = (UI8)(*A0);
  D7W = D0W;
  D0W >>= 5;
  D0W &= 0x7ff;
    if (D0W == roomPIT)
    {
      if ((D7W & 8) != 0)
      {
        if (cell2mirror[relativeCellNumber] != 0)
        {
          MirrorGraphicToViewport(
                       cell2graphic[relativeCellNumber],
                       &d.CeilingPit[cell2rect[relativeCellNumber]]);
        }
        else
        {
          GraphicToViewport(
                       cell2graphic[relativeCellNumber], 
                       &d.CeilingPit[cell2rect[relativeCellNumber]]);
        };
      };
    };
  };
}

//   TAG004f82
void DrawTeleporter(RectPos *P1,RectPos *rectpos)
{ // Draw teleporter???
  dReg D0;
  aReg A2;
  RectPos *uA3;
  ui8 *pnt_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  uA3 = P1;
  if (uA3->b.y2 == 0xff)
  {
    A2 = NULL;
  }
  else
  {
    D0W = sw(uA3->b.uByte4 * uA3->b.uByte5);
    A2 = (aReg)g_tempBitmap.get();
    MemMove((ui8 *)GetBasicGraphicAddress((uA3->b.y2 & 0x7f) + 69),
            (ui8 *)A2,
            D0W);
    if (uA3->b.y2 & 0x80)
    {
      Mirror((ui8 *)A2, uA3->b.uByte4, uA3->b.uByte5);
    };
  };
  AllocateDerivedGraphic(0);
  pnt_4 = GetBasicGraphicAddress(uA3->b.x1 + 73);
  TAG008c98(pnt_4,
            d.pViewportBMP,
            (ui8 *)A2,
            GetDerivedGraphicAddress(0),
            rectpos,
            sw(uA3->b.x2 + GraphicRandom(2)),
            sw(GraphicRandom(32)),
            112,
            uA3->b.y1,
            uA3->b.uByte6,
            0);
  //TAG022d5e(0);
  DeleteGraphic(0x8000);
}




ui8 data008734[256] = { //166 bytes
//008734
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
//008744
    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
//008754
    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
//008764
    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
    0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
//008774
    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
//008784
    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
    0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
//008794
    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
    0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
//0087a4
    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
//0087b4
    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
    0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
//0087c4
    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
    0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
//0087d4
    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
//0087e4
    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
    0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
//0087f4
    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
//008804
    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
//008814
    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
    0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
//008824
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
    0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
};

void Mirror(ui8 *bitmap, i32 width, i32 height) //TAG008642
{ // width is in bytes
  dReg D0, D1, D2, D3;
  aReg A1, A2, A3, A4;
  A2 = (aReg)data008734;
  D0L = 0;
  D1L = 0;
  D3W = sw(width);
  width >>= 3;
  A1 = (aReg)bitmap + D3W - 6;
  A3 = (aReg)bitmap;
  A4 = A1;
tag008670:
  D2W = sw(width);
tag008672:
  D2W -= 2;
  if (D2W < 0) goto tag0086e6;
  D0B     = *bitmap;
  D1B     = *(--A1);
  *A1     = *(A2 + D0L);
  *(bitmap++) = *(A2 + D1L);
  D0B     = *bitmap;
  D1B     = *(--A1);
  *A1     = *(A2 + D0L);
  *(bitmap++) = *(A2 + D1L);
  A1 += 4;
  D0B     = *bitmap;
  D1B     = *(--A1);
  *A1     = *(A2 + D0L);
  *(bitmap++) = *(A2 + D1L);
  D0B     = *bitmap;
  D1B     = *(--A1);
  *A1     = *(A2 + D0L);
  *(bitmap++) = *(A2 + D1L);
  A1 += 4;
  D0B     = *bitmap;
  D1B     = *(--A1);
  *A1     = *(A2 + D0L);
  *(bitmap++) = *(A2 + D1L);
  D0B     = *bitmap;
  D1B     = *(--A1);
  *A1     = *(A2 + D0L);
  *(bitmap++) = *(A2 + D1L);
  A1 += 4;
  D0B     = *bitmap;
  D1B     = *(--A1);
  *A1     = *(A2 + D0L);
  *(bitmap++) = *(A2 + D1L);
  D0B     = *bitmap;
  D1B     = *(--A1);
  *A1     = *(A2 + D0L);
  *(bitmap++) = *(A2 + D1L);
  if (D2W == 0) goto tag008722;
  A1 -= 12;
  goto tag008672;
tag0086e6:
  D0B  = *bitmap;
  D1B  = *(--A1);
  *A1 = *(A2 + D0L);
  *bitmap = *(A2 + D1L);
  bitmap += 2;
  A1 += 2;;
  D0B  = *bitmap;
  D1B  = *A1;
  *A1 = *(A2 + D0L);
  *bitmap = *(A2 + D1L);
  bitmap += 2;
  A1 += 2;;
  D0B = *bitmap;
  D1B = *A1;
  *A1 = *(A2 + D0L);
  *bitmap = *(A2 + D1L);
  bitmap += 2;
  A1 += 2;;
  D0B = *bitmap;
  D1B = *A1;
  *A1 = *(A2 + D0L);
  *bitmap = *(A2 + D1L);
tag008722:
  height--;
  if (height == 0) goto tag008834;
  A3 += D3W;
  bitmap = (ui8 *)A3;
  A4 += D3W;
  A1 = A4;
  goto tag008670;
tag008834:
  return;
}

// TAG008e1c
void Flood(ui8 *dst,i16 color,i16 num)
{ // number of 16-bit groups
  dReg D0, D1;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D0L = D1L = 0;
  if (color & 1) D0W = -1;
  if (color & 2) D0H1 = -1;
  if (color & 4) D1W = -1;
  if (color & 8) D1H1 = -1;
  do
  {
    longGear(dst) = D0L;
    longGear(dst+4) = D1L;
    dst += 8;
    num--;
  } while (num != 0);
};

/*
void TAG00907e(void (*  func)(i32,i32,i32), //8
               pnt (*  getGraphic)(i32),    //12
               i16    P3  ,                 //16
               pnt    P4  ,                 //18
               i16*   P5  ,                 //22
               i16**  P6  ,                 //26 Patch area pointers
               i16    P7  )                 //30 Number of patch areas
{
  // We appear to modify the front three words of
  // the function func to call a little code
  // patch that gets a different graphic to display.
  // After it executes, the patch puts the function
  // back as it was.  What sense does this make?
  // We are called upon to do this at strange times, IMHO.
//
  dReg D0;
  aReg A0;
  i16 *pwA2;
//
  HopefullyNotNeeded(0xddea);
//
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  do
  { // Look for empty entry at random
    D0L = Random() % P7;
    //A0 = P6 + 4 * D0L;
    pwA2 = P6[D0L];
    D0W = pwA2[30];
  } while (D0W != 0);
  // pwA2 will continue to point to the first word of the
  // patch area selected.
  pwA2[30] = 1;
  MemMove(TAG009110,pwA2,42); // 21 words.  Up to the
              // trailing jump without the address
  // A3 points to first byte of function to be patched.
  MemMove(func, pwA2+31, 6); // Save first 6 byes of func
  pntGear(pwA2+21) = func;  // Patch jumps back to function
  pntGear(pwA2+23) = getGraphic; // The function patch calls
//
}
*/
void TAG009110()
{
  HopefullyNotNeeded(0x6ef9);
  /*
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
009110 00 41fa 003a                LEA      $+60 (=0x00914c),A0
009114 02 4258                     CLR.W    (A0)+
  // Clear the patch in-use flag .. A0 <- pointer to saved words
009116 03 227a 0022                MOVE.L   $+36 (=0x00913a),A1   // put it back as it was
  // Get address of patched function.
00911a 05 22d8                     MOVE.L   (A0)+,(A1)+
00911c 06 3290                     MOVE.W   (A0),(A1)
  // Restore the function to its original state.
00911e 07 207a 001e                MOVE.L   $+32 (=0x00913e),A0
  // A0 <- extra function to call.
009122 09 3f3a 001e                MOVE.W   $+32 (=0x009142),-(A7)
  // Push the graphic number.
009126 11 4e90                     JSR      (A0)   ;getBasicGraphicAddress
  // Get the Graphic address.
009128 12 2040                     MOVE.L   D0,A0
  // A0 <- The graphic address.
00912a 13 2f3a 001c                MOVE.L   $+30 (=0x009148),-(A7)
  // Push the pointer P5.
00912e 15 2f3a 0014                MOVE.L   $+22 (=0x009144),-(A7)
  // Push the pointer P4
009132 17 4e71                     NOP
  // At this point we have on the stack ( P4, P5, graphicNum )
009134 18 4fef 000a                LEA      10(A7),A7
  // Get everything off the stack.
009138 20 4ef9 cdcd cdcd <         JMP      -842150451     DrawCellxxxx(facing,x,y)
  // Go perform the Drawing function as if we had never been here.

//00913e 23 0000 0000 // getGraphic function address goes here
//009142 25 0000      // The graphic number
//009144 26 0000 0000 // P4 (a pointer)
//009148 28 0000 0000 // P5 (a pointer)
//00914c 30 0000      // In-use flag
//00914e 31 0000 0000 0000  // Save first three words of function
*/
}



//   TAG0095e8
bool IsAlcove(i32 wallDecorationIndex)
{
  dReg D0, D7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (wallDecorationIndex < 0) return 0;
  for (D7W=0; D7W<3; D7W++)
  {
    D0W = d.AlcoveGraphicIndices[D7W];
    if (wallDecorationIndex == D0W) return 1;
//
  };
  return 0;
}

//   TAG009622
void RelativeXY(i32 facing,
                i32 stepsForward,
                i32 stepsRight,
                i32 *mapX,
                i32 *mapY)
{
  i16 *A0, *A1;
  dReg D1, D2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A0 = d.DeltaX;
  A1 = d.DeltaY;
  D1W = sw(stepsForward);
  D2W = D1W;
  D1L = D1UW * (ui16)A0[facing];
  *mapX = sw(*mapX + D1W);
  D2L = D2UW * (ui16)A1[facing];
  *mapY = sw(*mapY + D2W);
  facing = (I16)((facing+1)&3);
  D2W = D1W = sw(stepsRight);
  D1L = D1UW * (ui16)A0[facing];
  *mapX = sw(*mapX + D1W);
  D2L = D2UW * (ui16)A1[facing];
  *mapY = sw(*mapY + D2W);
}

void UpdateXY(i32 direction,
              i32 *newLevel,
              i32 *newX,
              i32 *newY)
{
  i32 deltaLevel;
  *newX = d.partyX;
  *newY = d.partyY;
  if (direction == 4) deltaLevel = 1; else deltaLevel = -1;
  *newLevel = IncrementLevel(d.partyLevel,deltaLevel,newX,newY);
}

//  TAG009672
CELLFLAG GetCellFlags(i32 mapX, i32 mapY)
{
  ROOMTYPE rtD4, rtD5;
  //D7W = P1;
  //D6W = P2;
  bool legalX, legalY;
  legalY = (mapY>=0) && (mapY<d.height);
  legalX = (mapX>=0) && (mapX<d.width);
  if (legalX && legalY)
  {
    return d.LevelCellFlags[mapX][mapY];
  }
  else
  {
    if (legalY)
    { // Then figure out what to do about illegal x.
      if (mapX == -1)
      {
        rtD4 = RoomType(d.LevelCellFlags[0][mapY]);
        if (rtD4 == roomOPEN) return 4; // Stone...Random decoration on east side
        if (rtD4 == roomPIT) return 4;
        return 0;
      };
      if (mapX == d.width)
      {
        rtD4 = RoomType(d.LevelCellFlags[d.width-1][mapY]);
        if (rtD4 == roomOPEN) return 1; // Stone...Random deoration on west side
        if (rtD4 == roomPIT) return 1;
        return 0;
      };
      return 0;
    };
    if (legalX)
    {
      // What to do if Y is illegal
      if (mapY == -1)
      {
        rtD5 = RoomType(d.LevelCellFlags[mapX][0]);
        if (rtD5 == roomOPEN) return 2;//Stone...Random decoration on south side
        if (rtD5 == roomPIT) return 2;
        return 0;
      };
      if (mapY == d.height)
      {
        rtD5 = RoomType(d.LevelCellFlags[mapX][mapY-1]);
        if (rtD5 == roomOPEN) return 8; //Stone ... Random decoration on north side
        if (rtD5 == roomPIT) return 8;
        return 0;
      };
      return 0;
    };
    return 0; // Both x and y illegal.
  };
}

CELLFLAG GetCellFlags(const LOCATIONREL& locr)
{
  return *GetCellFlagsAddress(locr.l, locr.x, locr.y);
}

//  TAG0097a0
i16 GetCellflagRelative(
              i32 facing,
              i32 stepsForward,
              i32 stepsRight,
              i32 x,
              i32 y)
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  RelativeXY(facing, stepsForward, stepsRight, &x, &y);
  D0W = GetCellFlags(x, y);
  return (I16)(D0W & 0xff);
}

//  TAG0097d6
i16 GetCelltypeRelative(
              i32 facing,
              i32 stepsForward,
              i32 stepsRight,
              i32 x,
              i32 y)
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D0W = GetCellflagRelative(
                   facing,
                   stepsForward,
                   stepsRight,
                   x,
                   y);
  D0W &= 0xff;
  D0W >>= 5;
  return D0W;
}

// *********************************************************
//
// *********************************************************
//  TAG009800
i16 IncrementLevel(i32 level,i32 delta,i32 *mapX,i32 *mapY)
//                 8     10      12      16
{ //returns new level or -1;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D1, D4;
//  aReg A3;
  LEVELDESC *pldA3;
  i32 absX, absY;
  i32 newLevel;
  i16 LOCAL_2;
  i16 LOCAL_12;
  if (d.partyLevel==255) return -1;

  pldA3 = &d.pLevelDescriptors[level];
  absX = pldA3->offsetX + (*mapX);
  absY = pldA3->offsetY + (*mapY);
  newLevel = pldA3->Level() + delta; // Old level + delta
  pldA3 = d.pLevelDescriptors;


  for (LOCAL_2 = 0; LOCAL_2 < d.dungeonDatIndex->NumLevel(); LOCAL_2++)
  { // 0 to #levels-1
    if (pldA3->Level() == newLevel)
      // search for new level among level descriptors
    {
      D4W = pldA3->offsetX;
      if (absX >= D4W)
      {
        D1W = pldA3->LastColumn(); // #columns-1 on new level
        LOCAL_12 = sw(D4W + D1W);
        if ((ui16)absX <= (ui16)LOCAL_12)
        {
          D4W = pldA3->offsetY;
          if(absY >= D4W)
          {
            D1W = pldA3->LastRow();//#rows-1 in new level
            LOCAL_12 =  sw(D4W + D1W);
            if ((ui16)absY <= (ui16)LOCAL_12)
            {
              *mapY = absY - D4W;
              *mapX = absX - pldA3->offsetX;
              return LOCAL_2;
            };
          };
        };
      };
    };
  pldA3 ++;
  };
//
  return -1;
}
// *********************************************************
//
// *********************************************************
DBCOMMON *GetCommonAddress(RN recDesc)
{
  // recDesc(bits 10-13) = database number
  // recDesc(bits 0-9)   = record number within database
//  dReg D0, D2, D3;
//  aReg A0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  // Some folks call to get the address before checking
  // to see if the object exists at all!
  if ((recDesc == RNnul) || (recDesc == RNeof))
  {
    HopefullyNotNeeded(0xea90);
    return NULL;
  };
  return db.GetCommonAddress(recDesc);
}

// *********************************************************
//
// *********************************************************
DB0 *GetRecordAddressDB0(RN object)
{
  ASSERT(object.dbType() == dbDOOR,"door");
  return (DB0 *) GetCommonAddress(object);
}

// *********************************************************
//
// *********************************************************
DB0 *GetRecordAddressDB0(i32 i)
{
  return (DB0 *) db.GetCommonAddress(dbDOOR, i);
}

// *********************************************************
//
// *********************************************************
DB1 *GetRecordAddressDB1(RN object)
{
  ASSERT(object.dbType() == dbTELEPORTER,"teleporter");
  return (DB1 *) GetCommonAddress(object);
}

// *********************************************************
//
// *********************************************************
DB2 *GetRecordAddressDB2(RN object)
{
  ASSERT(object.dbType() == dbTEXT,"text");
  return (DB2 *) GetCommonAddress(object);
}

// *********************************************************
//
// *********************************************************
DB2 *GetRecordAddressDB2(i32 i)
{
  return (DB2 *) db.GetCommonAddress(dbTEXT, i);
}

// *********************************************************
//
// *********************************************************
DB3 *GetRecordAddressDB3(i32 i)
{
  return (DB3 *) db.GetCommonAddress(dbACTUATOR, i);
}

// *********************************************************
//
// *********************************************************
DB3 *GetRecordAddressDB3(RN object)
{
  ASSERT(object.dbType() == dbACTUATOR,"actuator");
  return (DB3 *) GetCommonAddress(object);
}


// *********************************************************
//
// *********************************************************
DB4 *GetRecordAddressDB4(RN object)
{
  ASSERT(object.dbType() == dbMONSTER,"monster");
  return (DB4 *) GetCommonAddress(object);
}

// *********************************************************
//
// *********************************************************
DB4 *GetRecordAddressDB4(i32 i)
{
  return (DB4 *) db.GetCommonAddress(dbMONSTER, i);
}

// *********************************************************
//
// *********************************************************
DB5 *GetRecordAddressDB5(RN object)
{
  ASSERT(object.dbType() == dbWEAPON,"weapon");
  return (DB5 *) GetCommonAddress(object);
}

// *********************************************************
//
// *********************************************************
DB6 *GetRecordAddressDB6(RN object)
{
  ASSERT(object.dbType() == dbCLOTHING,"clothing");
  return (DB6 *) GetCommonAddress(object);
}

// *********************************************************
//
// *********************************************************
DB7 *GetRecordAddressDB7(RN object)
{
  ASSERT(object.dbType() == dbSCROLL,"scroll");
  return (DB7 *) GetCommonAddress(object);
}

// *********************************************************
//
// *********************************************************
DB8 *GetRecordAddressDB8(RN object)
{
  ASSERT(object.dbType() == dbPOTION,"potion");
  return (DB8 *) GetCommonAddress(object);
}

// *********************************************************
//
// *********************************************************
DB9 *GetRecordAddressDB9(RN object)
{
  ASSERT(object.dbType() == dbCHEST,"chest");
  return (DB9 *) GetCommonAddress(object);
}

// *********************************************************
//
// *********************************************************
DB10 *GetRecordAddressDB10(RN object)
{
  ASSERT(object.dbType() == dbMISC,"misc");
  return (DB10 *) GetCommonAddress(object);
}

// *********************************************************
//
// *********************************************************
DB10 *GetRecordAddressDB10(i32 i)
{
  return (DB10 *) db.GetCommonAddress(dbMISC, i);
}

// *********************************************************
//
// *********************************************************
DB14 *GetRecordAddressDB14(RN object)
{
  ASSERT(object.dbType() == dbMISSILE,"missile");
  return (DB14 *) GetCommonAddress(object);
}

// *********************************************************
//
// *********************************************************
DB15 *GetRecordAddressDB15(RN object)
{
  ASSERT(object.dbType() == dbCLOUD,"cloud");
  return (DB15 *) GetCommonAddress(object);
}

// *********************************************************
//
// *********************************************************
RN FindEmptyDB2Entry(bool important)
{//(RN)
  RN result;
  result = db.FindEmptyDBEntry(dbTEXT, important);
  if (result == RNnul)
  {
    result = MakeMoreRoomInDB(dbTEXT);
    if (result == RNnul) return RNnul;
  };
  GetRecordAddressDB2(result)->Clear();
  return result;
}

// *********************************************************
//
// *********************************************************
RN FindEmptyDB3Entry(bool important)
{//(RN)
  RN result;
  result = db.FindEmptyDBEntry(dbACTUATOR, important);
  if (result == RNnul)
  {
    result = MakeMoreRoomInDB(dbACTUATOR);
    if (result == RNnul) return RNnul;
  };
  GetRecordAddressDB3(result)->Clear();
  return result;
}

// *********************************************************
//
// *********************************************************
RN FindEmptyDB4Entry(bool important)
{//(RN)
  RN result;
  result = db.FindEmptyDBEntry(dbMONSTER, important);
  if (result == RNnul)
  {
    result = MakeMoreRoomInDB(dbMONSTER);
    if (result == RNnul) return RNnul;
  };
  GetRecordAddressDB4(result)->Clear();
  return result;
}

// *********************************************************
//
// *********************************************************
RN FindEmptyDB5Entry(bool important)
{//(RN)
  RN result;
  result = db.FindEmptyDBEntry(dbWEAPON, important);
  if (result == RNnul)
  {
    result = MakeMoreRoomInDB(dbWEAPON);
    if (result == RNnul) return RNnul;
  };
  GetRecordAddressDB5(result)->Clear();
  return result;
}


// *********************************************************
//
// *********************************************************
RN FindEmptyDB6Entry(bool important)
{//(RN)
  RN result;
  result = db.FindEmptyDBEntry(dbCLOTHING, important);
  if (result == RNnul)
  {
    result = MakeMoreRoomInDB(dbCLOTHING);
    if (result == RNnul) return RNnul;
  };
  GetRecordAddressDB6(result)->Clear();
  return result;
}

// *********************************************************
//
// *********************************************************
RN FindEmptyDB8Entry(bool important)
{//(RN)
  RN result;
  result = db.FindEmptyDBEntry(dbPOTION, important);
  if (result == RNnul)
  {
    result = MakeMoreRoomInDB(dbPOTION);
    if (result == RNnul) return RNnul;
  };
  GetRecordAddressDB8(result)->Clear();
  return result;
}


// *********************************************************
//
// *********************************************************
RN FindEmptyDB10Entry(bool important)
{//(RN)
  RN result;
  result = db.FindEmptyDBEntry(dbMISC, important);
  if (result == RNnul)
  {
    result = MakeMoreRoomInDB(dbMISC);
    if (result == RNnul) return RNnul;
  };
  GetRecordAddressDB10(result)->Clear();
  return result;
}

// *********************************************************
//
// *********************************************************
RN FindEmptyDB14Entry(bool important)
{//(RN)
  RN result;
  result = db.FindEmptyDBEntry(dbMISSILE, important);
  if (result == RNnul)
  {
    result = MakeMoreRoomInDB(dbMISSILE);
    if (result == RNnul) return RNnul;
  };
  GetRecordAddressDB14(result)->Clear();
  return result;
}


// *********************************************************
//
// *********************************************************
RN FindEmptyDB15Entry(bool important)
{//(RN)
  RN result;
  result = db.FindEmptyDBEntry(dbCLOUD, important);
  if (result == RNnul)
  {
    result = MakeMoreRoomInDB(dbCLOUD);
    if (result == RNnul) return RNnul;
  };
  GetRecordAddressDB15(result)->Clear();
  return result;
}

// *********************************************************
//
// *********************************************************
// TAG009a02
RN GetDBRecordLink(RN recName)
{
  // recDesc(bits 10-13) = database number
  // recDesc(bits 0-9)   = record number within database
  DBCOMMON *dbA0;
  ASSERT(recName != RNnul,"RNnul");
  ASSERT(recName != RNeof,"RNeof");
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dbA0 = GetCommonAddress(recName);
  return dbA0->link();
}

//  TAG009a1e
i16 GetObjectListIndex(i32 mapX,i32 mapY)
{ // Index in ??? of the object-list for this cell.
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D0, D4, D5;
  CELLFLAG *pdA3;
//  D7W = x;
//  D6W = y;
  if (mapX < 0) return -1;
  if (mapX >= d.width) return -1;
  if (mapY < 0) return -1;
  if (mapY >= d.height) return -1;
  pdA3 = d.LevelCellFlags[mapX];
  D0W = (I16)(pdA3[mapY] & 0x10);
  if (D0W == 0) return -1; // Nothing here!
  D4W = 0;
  D5W = d.pCurrentLevelObjectIndex[mapX];
  for (D4W=0; D4W<mapY; D4W++)
  {
    D0W = (I16)(pdA3[D4W]&0x10);
    if (D0W != 0) D5W++;

  };
  D0W = D5W;
  return D0W;
}

//  TAG009a94
RN FindFirstObject(i32 x,i32 y)
{

  i16 LOCAL_2;
  LOCAL_2 = GetObjectListIndex(x, y);
  if ( LOCAL_2 == -1) return RN(RNeof);
  return d.objectList[LOCAL_2];
}

// TAG009ac8
RN FindFirstDrawableObject(i32 x,i32 y)
{
  RN obj;
  obj = FindFirstObject(x, y);
  while ( (obj != RNeof) && (obj.dbType() < dbMONSTER))
  {
    obj = GetDBRecordLink(obj);
  };
  return obj;
}

RN FindFirstTeleporter(i32 level, i32 x, i32 y)
{
  RN obj;
  for (obj=FindFirstObject(level,x,y);
       obj!=RNeof;
       obj=GetDBRecordLink(obj))
  {
    if (obj.dbType() == dbTELEPORTER) break;
  };
  return obj;
}

// *********************************************************
//
// *********************************************************
//   TAG009c4e
void RemoveObjectFromRoom(RN object,i32 mapX,i32 mapY, MMRECORD *pmmr)
{
  dReg D0, D1, D5, D6;
  DBCOMMON *dbA3=NULL;
  RN objD4, objD7;
  RN *prnA2;
  RN P2;
  ui16 *puwA2;
  CELLFLAG *pdA0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"Removed %04x from %02x(%02x,%02x)\n",
             object.ConvertToInteger(), d.LoadedLevel, mapX, mapY);
  };
  objD7 = object;
  if (objD7 == RNeof) return;
  //05Nov2002 prs objD7.pos(0); // clear position leaving only record id
  if ((pmmr != NULL) && (pmmr->objMonster == object))
  {
    pmmr->finalLocr = LOCATIONREL(-1,0,0);
  };
  if (mapX >= 0)
  {
    dbA3 = GetCommonAddress(objD7);
    ASSERT(dbA3->link() != RNnul,"RNnul");
    D6W = GetObjectListIndex(mapX, mapY);
    prnA2 = &d.objectList[D6W];
    ASSERT(prnA2->ConvertToInteger() != 0xbadd,"prna2");
//
    // If specified object has no forward link
    //  and first object in room is the specified object
    //   then remove the entire room.
    if (    (dbA3->link() == RNeof)
         && ((*prnA2).IsSameObjectAs(objD7)) )
    {
      pdA0 = d.LevelCellFlags[mapX];
      ASSERT((pdA0[mapY] & 0x10)!=0,"pdA0");
      pdA0[mapY] &= 0xef; // clear 'room has object'
      D5W = sw(d.dungeonDatIndex->ObjectListLength() - 1);
      D0W = sw(2*(D5W - D6W));
      MemMove((ui8 *)(prnA2+1), (ui8 *)prnA2, D0W);
      d.objectList[D5W] = RN(RNnul); // clear last entry
      //Now we need to fix the column indexes.
      puwA2 = &d.pCurrentLevelObjectIndex[mapX+1];
      // Each column after the current column must
      // have its index decremented.  This includes
      // all the columns in higher numbered levels.
      D5W = d.numColumnPointers; // for all levels
      D1W = d.objectLevelIndex[d.LoadedLevel];
      // D1=index of first DUDAD for this level.
      D1W = sw(D1W + mapX);
      //D1=index of DUDAD for this column
      D5W = sw(D5W - D1W);
      D5W--; //#columns following this column in
              // this and following levels.
      while ((D5W--) != 0) (*(puwA2++))--;
//
      dbA3->link(RNeof);
      return;
    };
    if ((*prnA2).IsSameObjectAs(objD7))
    {
      *prnA2 = dbA3->link();
      dbA3->link(RNeof);
      return;
    }
    P2 = *prnA2;
  };
  objD4 = GetDBRecordLink(P2);
  while (!objD4.IsSameObjectAs(objD7))
  {
    if ((objD4 == RNeof) || (objD4 == RNnul))
    {
      dbA3->link(RNeof);
      return;
    };

    P2 = objD4;
    objD4 = GetDBRecordLink(objD4);
  };
  dbA3 = GetCommonAddress(P2);
  ASSERT(GetDBRecordLink(objD4) != RNnul,"RNnul");
  dbA3->link(GetDBRecordLink(objD4));
  dbA3 = GetCommonAddress(objD7);
  dbA3->link(RNeof);
}

void RemoveObjectFromRoom(RN object,const LOCATIONREL& locr)
{
  i32 oldLevel = d.LoadedLevel;
  LoadLevel(locr.l);
  RemoveObjectFromRoom(object,locr.x, locr.y, NULL);
  LoadLevel(oldLevel);
}



void TextTooLongMessage()
{
  static bool message = false;
  if (!message)
  {
    UI_MessageBox("Text Too Long", "Sorry", MB_OK);
    message = true;
  };
}

bool SubstituteCharacterName(char * pText, 
                             i32  *col,
                             i32  maxCol,
                             i32  characterIndex,
                             bool bcd)
{
  int i;
  CHARDESC *pCh;
  if (characterIndex >= d.NumCharacter) return false;
  pCh = &d.CH16482[characterIndex];
  for (i=0; i<8; i++)
  {
    if (*col >= maxCol) return true; //No more room.
    if (pCh->name[i] == 0) return true;//No more name.
    pText[*col] = (char)(pCh->name[i] - (bcd?'A':0));
    (*col)++;
  };
  return true;
}

void SetGlobalText(const char *text, ui32 index)
{
  int len;
  ui32 key;
  char temp[100];
  len = strlen(text);
  if (len > 99) len = 99;
  key = (EDT_GlobalText<<24) | (index&0xffff);
  memcpy(temp, text, len);
  temp[len] = 0;
  expool.Write(key, (ui32 *)temp, (len+4)/4);
}


bool SubstituteGlobalText(char * pText, 
                          i32  *col,
                          i32  maxCol,
                          i32  index,
                          bool bcd)
{
  int i;
  ui32 *pRecord;
  char *pSource;
  ui32 key;
  i32 size;
  key = (EDT_GlobalText<<24) | (index & 0xffff);
  size = expool.Locate(key, &pRecord);
  if (size < 0) return false;
  size *= 4;
  pSource = (char *)pRecord;
  for (i=0; i<size; i++)
  {
    if (*col >= maxCol) return true; //No more room.
    if (*pSource == 0) return true;//No more text.
    pText[*col] = (char)(*(pSource++) - (bcd?'A':0));
    (*col)++;
  };
  return true;
}

bool SubstituteGlobalVariable(char * pText, 
                              i32  *col,
                              i32  maxCol,
                              i32  index,
                              bool bcd)
{
  int i;
  char anum[20];
  if (index >= numGlobalVariables) return false;
  sprintf(anum,"%d", globalVariables[index]);
  for (i=0; anum[i] != 0; i++)
  {
    if (*col >= maxCol) return true; //No more room.
    pText[*col] = (char)(anum[i] - (bcd?'A':0));
    (*col)++;
  };
  return true;
}

i32 bcdstrstr(char *str, const char *pattern)
{
  i32 i, j, patLen, strLen;
  for (patLen=0; (pattern[patLen] & 0x80)==0; patLen++);
  for (strLen=0; (str[strLen]&0xff) != 0x81; strLen++);
  for (i=0; i<strLen-patLen; i++)
  {
    for (j=0; (j<patLen)&&(pattern[j]==str[i+j]); j++);
    if (j == patLen) return strLen;
  };
  return -1;
}

bool SubstituteCharacterNames(pnt pText, i32 max, bool bcd)
{
  i32 i, j, len=0;
  i32 offset, point, k, index;
  char cmd;
  if (    (bcd && ((len=bcdstrstr((char *)pText, "\033\020\033\200")) < 0))
       ||    (    !bcd 
               && (strstr((char *)pText, ".Q.") == NULL) 
               && (strstr((char *)pText, ".R.") == NULL)
               && (strstr((char *)pText, ".N.") == NULL)
             )
     ) return true;
  if (!bcd) len = strlen((char *)pText);
  if (len < 4) return true; 
  {
    bool result = true;
    char *pNewText;
    pNewText = (char *)UI_malloc(max+10, MALLOC008);
    j = 0; //Result column
    i = 0;
    offset = bcd ? 'A' : 0;  // What to add to get ASCII code.
    point  = bcd ? 27 : '.';
    while (result && (i < len))
    {
      if ( (pText[i]   == point) && (pText[i+2] == point) )
      {
        if (   (pText[i+1] + offset == 'Q')
            && (pText[i+3] + offset >= 'A') && (pText[i+3] + offset <= 'K')
          )
        {
          i += 3; // Skip over the ".Q."
          cmd = (char)(pText[i++] + (bcd?'A':0));
          switch (cmd)
          {
          case 'A': //Active character
            if (d.HandChar < 0)
            {
              result = false;
              break;
            };
            if (!SubstituteCharacterName(pNewText,&j,max,d.HandChar,bcd))
            {
              result = false;
            };
            break;
          case 'B':
          case 'C':
          case 'D':
          case 'E': //Character Number
            if (!SubstituteCharacterName(pNewText,&j,max,cmd-'B',bcd))
            {
              result = false;
            };
            break;
          case 'F': //Last character
            break;
          case 'G':
          case 'H':
          case 'I':
          case 'J': //Character at position NW, NE, SE, SW
            break;
          case 'K': //Character name.
            break;
          };
        }
        else if (    (pText[i+1] + offset == 'R')
                  && (pText[i+3] + offset >= 'A') && (pText[i+3] + offset <= 'A'+15)
                  && (pText[i+4] + offset >= 'A') && (pText[i+4] + offset <= 'A'+15)
                  && (pText[i+5] + offset >= 'A') && (pText[i+5] + offset <= 'A'+15)
                  && (pText[i+6] + offset >= 'A') && (pText[i+6] + offset <= 'A'+15)
                )
        {
          i += 3; // Skip over the ".R."
          k = i+4;
          for (index=0; i<k; i++)
          {
            index = index*16 + pText[i] + offset - 'A';
          };
          if (!SubstituteGlobalText(pNewText,&j,max,index,bcd))
          {
            result = false;
          };
        }  
        else if (    (pText[i+1] + offset == 'N')
                  && (pText[i+3] + offset >= 'A') && (pText[i+3] + offset <= 'A'+15)
                  && (pText[i+4] + offset >= 'A') && (pText[i+4] + offset <= 'A'+15)
                )
        {
          i += 3; // Skip over the ".N."
          k = i+2;
          for (index=0; i<k; i++)
          {
            index = index*16 + pText[i] + offset - 'A';
          };
          if (!SubstituteGlobalVariable(pNewText,&j,max,index,bcd))
          {
            result = false;
          };
        }  
        else
        {
          if (j < max-1) pNewText[j++] = pText[i];
          i++;
        };
      }
      else
      {
        if (j < max-1) pNewText[j++] = pText[i];
        i++;
      };
    };
    if (bcd) pNewText[j++] = -0x7f;
    else pNewText[j++] = 0;
    memcpy(pText, pNewText, j);
    UI_free(pNewText);
    return result;
  };
}   

//   TAG00a27c
i32 DecodeText(char *pDest,DB2 *pText,i16 typeOfText, i32 max)
{//Wall text P3 = 0;
  //Return: 0 if all is well
  //       -1 if Character name substitution fails.
  dReg D0, D4, D5, D6;
  aReg A2;
  char *result = pDest;
  DB2 *DB2A1;
  RN objD4;
  DB2 DB2_12;
  char *LOCAL_8;
  i16 LOCAL_4;
  i8  LOCAL_1;
  i32 slashCount = 0; // Consecutive '/'s
  i32 len = 0;
  D6L = 0xbaddadee;
  //A3 = P1;
  //D7W = typeOfText;
  //DB2A1 = GetRecordAddressDB2(textID);
  DB2A1 = pText;
  DB2_12 = *DB2A1;
  A2 = (aReg)(d.compressedText.data() + d.indirectTextIndex[DB2_12.index()]);
  if ((DB2_12.show() != 0) || ((typeOfText & 0x8000) != 0))
  {
    if ((typeOfText & 0x7fff) == 1)
    {
      *(result++) = 10;
      len++;
      LOCAL_1 = 32;
    }
    else
    {
      if (typeOfText == 0)
      {
        LOCAL_1 = -128;
      }
      else
      {
        LOCAL_1 = 10; //Probably D7W==2
      };
    };
    D5W = 0;
    LOCAL_4 = 0;
    for (;;)
    {
      if (D5W == 0)
      {
        D6W = LE16(wordGear(A2));
        A2 += 2;
        D4W = BITS10_14(D6W);
      }
      else
      {
        if (D5W == 1)
        {
          D4W = BITS5_9(D6W);
        }
        else
        {
          D4W = BITS0_4(D6W);
        };
      };
      D5W++;
      D5W = (I16)(D5W % 3);
      if (D4W == 28) slashCount++; else slashCount = 0;
      if (LOCAL_4 != 0)
      {
        *result = 0;
        if (LOCAL_4 == 30)
        {
          if (typeOfText != 0)
          {
            LOCAL_8 = d.Byte10016 + 8*D4W;
          }
          else
          {
            LOCAL_8 = d.Byte10336 + 8*D4W;
          };
        }
        else
        {
          if (typeOfText != 0)
          {
            LOCAL_8 = d.Byte10080 + 2*D4W;
          }
          else
          {
            LOCAL_8 = d.Byte10080 + 2*D4W;
          };
        };
        D0W = StrLen(LOCAL_8);
        len += D0W;
        if (len >= max-4)
        {
          TextTooLongMessage();
          len -= D0W; //Undo the damage.
        }
        else
        {
          StrCat((char *)result, LOCAL_8);
          result += D0W;
        };
        LOCAL_4 = 0;
      }
      else
      {
        if (D4W < 28)
        {
          if (typeOfText != 0)
          {
            if (D4W== 26)
            {
              D4W = ' ';
            }
            else
            {
              if (D4W ==27)
              {
                D4W = '.';
              }
              else
              {
                 D4W += 'A';
              };
            };
          };
          *(result++) = D4B;
          len++;
          if (len > max-4)
          {
            TextTooLongMessage();
            len--;
            result--;
          };
        }
        else
        {
          if (D4W == 28)
          {
            if ( (LOCAL_1 == ' ') && (slashCount == 2) )
            {
              *(result-1) = 10; // 2 slashes force linefeed.
            }
            else
            {
              *(result++) = LOCAL_1;
              len++;
              if (len > max-4)
              {
                UI_MessageBox("Text Too Long", "Sorry", MB_OK);
                UI_Die(0xcd32);
              };
            };
          }
          else
          {
            if (D4W <= 30)
            {
              LOCAL_4 = D4W;
            }
            else
            {
              break;
            };
          };
        };
      };
    };
  };
  if (typeOfText == 0)
  {
    *result = -127;
  }
  else
  {
    *result = 0;
  };
  //if (D7W != 0) 
  {
    if (!SubstituteCharacterNames((pnt)pDest,max,typeOfText==0)) return -1;
  };
  return 0;
}

// *********************************************************
//
// *********************************************************
//  TAG00a41c
i16 Hash(i16 P1,i16 P2,i16 P3)
{ // Called from only one place in program.
  // P1 is a function of mapX, mapY
  // P2 is a function of level, width, and height
  // P3 is a small integer like 30 used as arithmetic modulus.
  // Result is     hash(P1,P2,dungeonDatIndex[0]) % P3
  dReg D0, D1;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D0L = (ui16)P1 * 31417;
  D0W = (I16)((D0W >>= 1) & 0x7fff);
  D1L = (ui16)P2 * 11;
  D0W = sw(D0W + D1W);
  D0W = sw(D0W + d.dungeonDatIndex->Sentinel()); // = 13 in my game
  D0W = (I16)((D0W >> 2) & 0x3fff);
  D0L &= 0xffff;
  D0W = sw(D0L % (ui16)(P3));
  return D0W;
}


//  TAG00a44c
i16 RandomWallDecoration(i16 P1,i16 P2,i32 mapX,i32 P4,i16 probability)
{ //P4 = n*mapY  where n=facing
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  i16 LOCAL_2;
  if (P1 != 0)
  {
    LOCAL_2 = Hash(
                (I16)(2000 + (mapX<<5) + P4),
                (I16)(3000 + (d.LoadedLevel<<6) + d.width + d.height),
                probability);
    if (LOCAL_2 < P2)
    {
      return sw(LOCAL_2 + 1);
    };
  };
  return 0;
}

void TAG00a4a2(SUMMARIZEROOMDATA *rslt ,
               i16 P2,       //12
               i16 P3,       //14
               i16 P4,       //16
               i32 facing,   //18
               i32 mapX,     //20
               i32 mapY,     //22
               i32 P8)       //24
{
  i16 numDecoration, i;
  numDecoration = d.pCurLevelDesc->NumRandomWallDecoration();
  rslt->decorations[0] = RandomWallDecoration(
                             P2,
                             numDecoration,
                             mapX,
                             (mapY+1)*(((facing+1)&3)+1),
                             30);
  rslt->decorations[1] = RandomWallDecoration(
                             P3,
                             numDecoration,
                             mapX,
                             (mapY+1)*(((facing+2)&3)+1),
                             30);
  rslt->decorations[2] = RandomWallDecoration(
                             P4,
                             numDecoration,
                             mapX,
                             (mapY+1)*(((facing+3)&3)+1),
                             30);
  if (    (P8 != 0)
       || (mapX < 0)
       || (mapX >= d.width)
       || (mapY < 0)
       || (mapY >= d.height) )

  {
    for (i = 2; i <= 4; i++)
    {
      if (IsAlcove(sw(rslt->decorations[i-2]-1)))
      {
        rslt->decorations[i-2] = 0;
      };

    };
  };
}

// *********************************************************
//
// *********************************************************
//   TAG00a9d4
void LoadLevel(i32 newLevel)
{
  dReg D0;
  D0W = d.LoadedLevel;
  if (newLevel >= d.dungeonDatIndex->NumLevel())
  {
    die(0xd12b, "Illegal dungeon level");
  };
  if (D0W != newLevel)
  {
    d.LoadedLevel = sw(newLevel);
    d.LevelCellFlags = d.pppdPointer10450[newLevel];
    d.pCurLevelDesc = &d.pLevelDescriptors[newLevel];
    d.width = sw(d.pCurLevelDesc->LastColumn()+1);
    d.height = sw(d.pCurLevelDesc->LastRow()+1);
    D0W = d.pCurLevelDesc->Door1Type();
    d.DoorTOC[0] = d.DoorCharacteristics[D0W];
    D0W = d.pCurLevelDesc->Door2Type();
    d.DoorTOC[1] = d.DoorCharacteristics[D0W];
    D0W = d.objectLevelIndex[newLevel];
    d.pCurrentLevelObjectIndex = &d.objectListIndex[D0W];
  };
}

//   TAG00aa9c
void SetPartyLevel(i32 level)
{
  dReg D1;
  d.partyLevel = sw(level);
  LoadLevel(level);
  d.EndOfCELLFLAGS = d.LevelCellFlags[d.width-1] + d.height;//pdD0 = (CELLFLAG *)pD0;
  d.NumWallDecoration = d.pCurLevelDesc->NumWallDecoration();
  D1W=d.pCurLevelDesc->numMonsterType();
  memmove(d.WallDecorationTOC, //For this level
          d.EndOfCELLFLAGS + D1W,
          d.NumWallDecoration);
  d.WallDecorationTOC[d.NumWallDecoration] = 0;//Far Text decoration
  D1W = sw(D1W + d.NumWallDecoration);
  memmove(d.FloorDecorationTOC,
          d.EndOfCELLFLAGS + D1W,
          d.pCurLevelDesc->NumFloorDecorations());
  D1W = sw(D1W + d.pCurLevelDesc->NumFloorDecorations());
  memmove(d.DoorDecorationTOC,
          d.EndOfCELLFLAGS + D1W,
          d.pCurLevelDesc->NumDoorDecorations());
}

// *********************************************************
//
// *********************************************************
//  TAG00ad18
i16 SetMonsterPositionBits(i16 P1,i32 monsterNumber,i32 Pos)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  Pos &= 3; // Some folks forget!
  Pos <<= 2*monsterNumber; //locate position bits
  P1 &= ~(3 << (2*monsterNumber)); // mask out unwanted bits
  return sw(P1 | Pos);
}

// *********************************************************
//
// *********************************************************
//  TAG00ad18
i16 SetMonsterFacingBits(i16 P1,i32 monsterNumber,i32 facing)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  facing &= 3; // Some folks forget!
  facing <<= 2*monsterNumber; //locate position bits
  P1 &= ~(3 << (2*monsterNumber)); // mask out unwanted bits
  return sw(P1 | facing);
}

// *********************************************************
//
// *********************************************************
//  TAG00ad58
i32 NextMonsterUpdateTime(ITEM16 *P1,
                          i16 monsterIndex,
                          bool attacking)
{
  dReg D0, D4, D7;
  ITEM16 *pI16A3;
  DB4    *pDB4A2;
  MONSTERTYPE mtD6;
  MONSTERTYPE mt_4;
  MONSTERDESC_WORD4 mdw4;
  bool bool_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pI16A3 = P1;
  D7W = monsterIndex;
  pDB4A2 = GetRecordAddressDB4(pI16A3->word0);
  //pDB4A2 = & ((DB4 *)(d.misc1052eight[4]))[pI16A3->word0];
  mt_4 = pDB4A2->monsterType();
  mdw4 = d.MonsterDescriptor[mt_4].word4;
  bool_2 = D7W < 0;
  if (bool_2)
  {
    D7W = pDB4A2->numMonM1();
  };
  do  // while D7W >= 0
  {
    SINGLE_MONSTER_STATUS sms;
    sms = pI16A3->singleMonsterStatus[D7W];
    //D6W &= 0xc0;
    sms.Nibble0(0);
    sms.Nibble3(0);
    //D4W = BITS12_13(D5W);
    D4W = mdw4.Nibble12();
    if (D4W != 0)
    {
      D4W = (i16)STRandom(D4W);
      if (STRandomBool())
      {
        D4W = (I16)((-D4W) & 7);
      };
      //D6W |= D4W;
      sms.Nibble0(D4W);
    };
    //D4W = BITS14_15(D5W);
    D4W = mdw4.Nibble14();
    if (D4W != 0)
    {
      D4W = (i16)STRandom(D4W);
      if (STRandomBool())
      {
        D4W = (I16)((-D4W) & 7);
      };
      //D6W |= D4W << 3;
      sms.Nibble3(D4W);
    };
    if (attacking)
    {
      //if (D5W & 0x200)
      if (mdw4.AttackingMirrorOK())
      {
        if ( sms.TestAttacking() && (mt_4 == mon_DethKnight) )
        {
          if (STRandomBool())
          {
            sms.ToggleMirror();
            QueueSound(16, d.monsterX, d.monsterY, 1);
          };
        }
        else
        {
          if (   !sms.TestAttacking()
              //|| ((D5W & 0x400) == 0) )
              || !mdw4.HasBit10() )
          {
            if (STRandomBool())
            {
              //D6W |= 0x40;
              sms.SetMirror();
            }
            else
            {
              //D6W &= 0xffbf;
              sms.ClearMirror();
            };
          };
        };
      }
      else
      {
        //D6W &= 0xffbf;
        sms.ClearMirror();
      };
      //D6W |= 0x80;
      sms.SetAttacking();
    }
    else
    {
      //if (D5W & 4)
      if (mdw4.NonAttackingMirrorOK())
      {
        if (STRandomBool())
        {
          //D6W |= 0x40;
          sms.SetMirror();
        }
        else
        {
          //D6W &= 0xffbf;
          sms.ClearMirror();
        };
      }
      else
      {
        //D6W &= 0xffbf;
        sms.ClearMirror();
      };
      //D6W &= 0xff7f;
      sms.SetNotAttacking();
    };
    //pI16A3->ITEM16_uByte12[D7W] = D6B;
    pI16A3->singleMonsterStatus[D7W] = sms;
    if (!bool_2) break;
    D7W--;
  } while  (D7W >= 0);
  mtD6 = pDB4A2->monsterType();
//  D6W = d.MonsterDescriptor[mtD6].word20;
  //D0L = d.Time;
  {
	int minDelay;
    if (attacking)
	{
      //D1W = (I16)((D6W >> 8) & 15);
	  minDelay = d.MonsterDescriptor[mtD6].AdditionalDelayFromAttackToMove();
	}
    else
	{
      //D1W = (I16)((D6W >> 4) & 15);
	  minDelay = d.MonsterDescriptor[mtD6].AdditionalDelayFromMoveToAttack();
	};
    //D0L = d.Time + (D1L & 0xffff);
    D0L = d.Time + minDelay;
  };
  D0L += STRandomBool();
  return D0L;
}

//   TAG00af1a
void StartMonsterMovementTimers(i32 mapX,i32 mapY)
{
  DB4 *DB4A3;
  TIMER timer;
  RN obj;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  obj = FindFirstMonster(mapX, mapY);
  if ((obj != RNnul) && (obj != RNeof))
  {
    DB4A3 = GetRecordAddressDB4(obj);
    if (DB4A3->fear() >= StateOfFear4)
    {
      DB4A3->fear(StateOfFear0);
    };
  }
  else
  {
    ASSERT(0,"timer");
    HopefullyNotNeeded(0xcc8d);
    DB4A3 = NULL;
    timer.timerUByte5(255);
    // This happened in the prison when the only
    // character died from lack of food.
  };
  //timer.timerTime = (d.Time+1) | (d.LoadedLevel << 24);
  timer.Time(d.Time+1);
  timer.Level((ui8)d.LoadedLevel);
  timer.Function(TT_MONSTER_Bgroup);
  timer.timerUByte5(
        ub(255 - d.MonsterDescriptor[DB4A3->monsterType()].timePerMove()));
  timer.timerUByte8(0);
  timer.timerUByte6((i8)mapX);
  timer.timerUByte7((i8)mapY);
  gameTimers.SetTimer(&timer);
}

//   TAG00b062
void AttachItem16ToMonster(RN P1,i32 mapX,i32 mapY)
{
  dReg        D0, D1, D6;
  aReg        A0;
  MONSTERDESC *pmtDesc;
  ITEM16      *pi16A2;
  DB4         *pDB4A3;
  i16         item16Index;
  i16         LOCAL_6;
  static i32  message = 0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A0 = NULL;
  pi16A2 = d.Item16;
  item16Index = 0;
  while (pi16A2->word0 >= 0)
  {
    item16Index++;
    if (item16Index >= d.MaxITEM16)
    {
      if (!AutoEnlarge && !IsPlayFileOpen())
      {
        //UI_MessageBox("The Atari code did bad things at this point.\n"
        //              "I am going to try to increase the maximum.\n"
        //              "If you don't like this, close the program\n"
        //              "before saving the game.",
        //              "Too many monsters",
        //              MESSAGE_OK);
        if (item16Index > 500)
        {
          UI_MessageBox("Excessive number of monsters","WARNING",MESSAGE_OK);
        };
      };
      if (d.MaxITEM16 < 1050)
      {
        i32 i, newMax;
        newMax = d.MaxITEM16 + 50;
        d.Item16 = (ITEM16 *)UI_realloc(d.Item16, 16 * newMax,
                                        MALLOC060);
        memset(&d.Item16[d.MaxITEM16], 0, (newMax-d.MaxITEM16)*sizeof(ITEM16));
        for (i=d.MaxITEM16; i< newMax; i++)
        {
          d.Item16[i].word0 = -1;
        };
        pi16A2 = &d.Item16[d.MaxITEM16];
        d.MaxITEM16 = sw(newMax);
        break;
      }
      else
      {
        UI_MessageBox("I give up",
                      "Cannot reallocate monster data",
                      MESSAGE_OK);
        die (0);
      };
    };
    pi16A2++;
  };
  d.ITEM16QueLen++;
  pi16A2->word0 = D1W = P1.idx();
  pDB4A3 = GetRecordAddressDB4(D1W);
  //pDB4A3 = &((DB4 *)(d.misc1052eight[4]))[D1W];
  pi16A2->positions() = pDB4A3->groupIndex();
  pDB4A3->groupIndex(item16Index);
  pi16A2->uByte10 = (i8)mapX;
  pi16A2->ubPrevX = (i8)mapX;
  pi16A2->uByte11 = (i8)mapY;
  pi16A2->ubPrevY = (i8)mapY;
  pi16A2->uByte4 = (UI8)(d.Time - 127); // about 20 seconds
  pmtDesc = &d.MonsterDescriptor[pDB4A3->monsterType()];
  SET(D0B, pmtDesc->horizontalSize() == 1);
  LOCAL_6 = (I16)(D0W & 1);
  D6W = pDB4A3->numMonM1();
  do
  {
    // Set all members of group facing same direction.
    //D0W = pDB4A3->word14;
    D0W = SetMonsterPositionBits(
             pi16A2->facings(),  //Old facing byte
             D6W,               //MonsterNumber
             pDB4A3->facing());     //Monster's facing
    pi16A2->facings() = D0B;
    //pi16A2->ITEM16_uByte12[D6W] = 0;
    pi16A2->singleMonsterStatus[D6W].Clear();
    D0W = D6W--;
  } while (D0W != 0);
  NextMonsterUpdateTime(pi16A2, -1, false);
}

// *********************************************************
//
// *********************************************************
//   TAG00b152
void DetachItem16(i32 P1)
{
  ITEM16 *pi16A3;
  DB4 *pDB4A2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//
//
  if ( (P1 > d.MaxITEM16) || (d.Item16[P1].word0 < 0) ) return;
//
  ASSERT(P1 < d.MaxITEM16,"maxitem16");
  pi16A3 = &d.Item16[P1];
  pDB4A2 = GetRecordAddressDB4(pi16A3->word0);
  //pDB4A2 = &((DB4 *)d.misc1052eight[4])[pi16A3->word0];
  d.ITEM16QueLen--;
  pDB4A2->groupIndex(pi16A3->positions());
  // 30OCT02SETWBITS8_9(pDB4A2->word14, pi16A3->facings());
  pDB4A2->facing(pi16A3->facings());
  // 30OCT02if (BITS0_3(pDB4A2->word14) >=4)
  if (pDB4A2->fear() >= StateOfFear4)
  {
    //30OCT02SETWBITS0_3(pDB4A2->word14, 0);
    pDB4A2->fear(StateOfFear0);
  };
  pi16A3->word0 = -1;
}

void TAG00bd40()
{
  i32 i=0;
  while (d.ITEM16QueLen != 0)
  {
    ASSERT(i < d.MaxITEM16,"maxitem16");
    if ( d.Item16[i].word0 >= 0)
    {
      DetachItem16(i); //(item16Index)
    };
    i++;
  };
}

//   TAG00bd70
void ProcessMonstersOnLevel()
{ // I don't know what 'process' means.  :-(
  dReg D0;
  RN *prnA2;
  //DUDAD16 *pdA3;
  CELLFLAG *ubA3;
  RN objD5, nextObj;
  i32 mapX, mapY;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ubA3 = d.LevelCellFlags[0];
  D0W = *d.pCurrentLevelObjectIndex;
  prnA2 = &d.objectList[D0W];
  for (mapX=0; mapX<d.width; mapX++)
  {
    for (mapY=0; mapY<d.height; mapY++)
    {
      if (((*ubA3++)&16) != 0)
      {
        objD5 = *(prnA2++);
        while (objD5 != RNeof)
        {
          ASSERT(objD5.ConvertToInteger() != 0xfeff,"objD5");
          //D0W = ;
          if (objD5.dbType() == dbMONSTER) // Is it a monster?
          {
            DeleteMonsterMovementTimers(mapX, mapY); // type 29-41
            AttachItem16ToMonster(objD5, mapX, mapY); //add to item16
            StartMonsterMovementTimers(mapX, mapY); //Set timers for monster
            break;
          }
          nextObj = GetDBRecordLink(objD5);
          ASSERT(nextObj != RNnul,"nextObj");
          nextObj = GetDBRecordLink(objD5);
          objD5 = nextObj;
        }; //while
      };

    };
//
  };
}

// *********************************************************
//
// *********************************************************
//   TAG00be06
void InitializeItem16()
{
  i32 i;
  if (d.gameState != GAMESTATE_ResumeSavedGame) d.MaxITEM16 = 60;
  d.Item16 = (ITEM16 *)UI_malloc(16*d.MaxITEM16, MALLOC009);
  memset(d.Item16, 0, d.MaxITEM16*sizeof(ITEM16));
  for (i=0; i < d.MaxITEM16; i++) d.Item16[i].word0 = -1;
}


/*
// *********************************************************
//
// *********************************************************
i16 TAG00dcf2(pnt P1)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// All of this does nothing!
//
  d.Word8892+=4;
  return 4;
}
*/

// *********************************************************
//
// *********************************************************
i16 TAG011594(i16 chIdx,
              CHARDESC *pChar,
              i16 srcPossession,
              i16 dstPossession)
{//(i16)
  dReg D5, D6, D7;
  RN   objD0;
  CHARDESC *pcA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = chIdx;
  pcA3 = pChar;
  D6W = srcPossession;
  D5W = dstPossession;
  if (pChar->Possession(D6W) == RNnul) return 0;
  if (pChar->Possession(D6W).dbType() != dbWEAPON) return 0;
  objD0 = RemoveCharacterPossession(D7W, D6W);
  AddCharacterPossession(D7W, objD0, D5W);
  return (1);
}

void TAG0115ee(i32 chIdx, i32 byte6)
{ //()
  //Called by timer 11 when byte 6 not zero
  dReg D0, D5, D6, D7;
  CHARDESC *pcA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D6W = sw(byte6);
  D7W = sw(chIdx);
  pcA3 = &d.CH16482[chIdx];
  if (pcA3->Possession(D6W) != RNnul) return;
  D0W = TAG011594(D7W, pcA3, 12, D6W);
  if (D0W != 0) return;
  for (D5W = 7; D5W <= 9; D5W++)
  {
    D0W = TAG011594(D7W, pcA3, D5W, D6W);
    if (D0W != 0) return;
//
//
  };
}

// **********************************************************
//
// **********************************************************
//    TAG011664
void MarkAllPortraitsChanged()
{
  i32 i;
  for (i=0; i<d.NumCharacter; i++)
  {
    d.CH16482[i].charFlags |= CHARFLAG_portraitChanged;
  };
  DrawAllCharacterState();
}

// *********************************************************
//
// *********************************************************
void TAG0138ec()
{//()
  dReg D7;
  RN objD5;
  CHARDESC *pcA3;
  CURSORFILTER_PACKET cfp;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.gameState == GAMESTATE_ResumeSavedGame)
  {
    ASSERT(RememberToPutObjectInHand == -1,"objInHand");
    objD5 = d.objectInHand;
    if (objD5 == RNnul)
    {
      if (TimerTraceActive)
      {
        fprintf(GETFILE(TraceFile), " Set EmptyHanded in TAG0138ec-A\n");
      };
      d.EmptyHanded = 1;
      d.ObjectType16574 = -1;
      TAG002572();
    }
    else
    {
      cfp.object = objD5.ConvertToInteger();
      cfp.type   = CURSORFILTER_ResumeSavedGame;
      cfp.p1     = 0;
      cfp.p1     = 0;
      cfp.p1     = 0;
      cfp.p1     = 0;
      if (objD5 != RNnul)
      {
        CursorFilter(&cfp);
      };
      ObjectToCursor(objD5, 1);
    };
    pcA3 = d.CH16482;
    D7W = 0;
    while (D7W < d.NumCharacter)
    {
      pcA3->charFlags &= CHARFLAG_nonGraphicFlags;
      pcA3->charFlags |= CHARFLAG_weaponAttack | CHARFLAG_portraitChanged | CHARFLAG_positionChanged;
      D7W++;
      pcA3++; // +800

    };
    DrawAllCharacterState();
    D7W = d.HandChar; // owner of cursor
    if (D7W != -1)
    {
      // I have commented the following line.
      // What happened was that the Atari code
      // added the weight of the object in hand already.
      // So when we set d.HandChar to -1 and call
      // SetHandOwner the weight gets added again.
      // By leaving d.HandChar alone, SetHandOwner will
      // subtract the weight before adding it again.
      //d.HandChar = -1;
      // The following patch has been put into the
      // Atari code
      //  01395e  4e71  NOP   patch used to be 397c
      //  013960  4e71  NOP   patch used to be ffff
      //  013962  4e71  NOP   patch used to be bf48

      SetHandOwner(D7W);
    };
    D7W = d.MagicCaster;
    if (D7W != -1)
    {
      d.MagicCaster = -1;
      SelectMagicCaster(D7W);
    };
    return;
  };
  ASSERT(RememberToPutObjectInHand == -1,"objInHand");

  cfp.object = RN(RNnul).ConvertToInteger();
  cfp.type   = CURSORFILTER_EnteringPrison;
  cfp.p1     = 0;
  cfp.p1     = 0;
  cfp.p1     = 0;
  cfp.p1     = 0;
  CursorFilter(&cfp);
  d.objectInHand = RN(RNnul);

  d.ObjectType16574 = -1;
  d.EmptyHanded = 1;
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile), " Set EmptyHanded in TAG0138ec-B\n");
  };

}

//   TAG011692
bool ProcessTimers() // Parameter ignored, I think.
{//()
  dReg D0;
  TIMERTYPE function;
  i32 param;
  i32 chIdx;
  ui32 timerIndex;
  i16 *pwA3;
  CHARDESC *pcA0;
  TIMER  timeEnt;
  i16 cP4, cP3, cP2, cP1;
  i32 numberOfTimersProcessed;
//  RESTARTMAP
//    RESTART(1)
//    RESTART(2)
//  END_RESTARTMAP
  //i32 saveD7 = D7;
  //pnt saveA3 = A3;
  numberOfTimersProcessed = 0;
  function=TT_EMPTY;
  while (gameTimers.CheckForTimerEvent() != 0)
  {
    if (++numberOfTimersProcessed > 50000)
    {
      die(0,"50000 timer events in 1/6 second.\n"
            "That seems excessive.\n"
            "Please contact your friendly dungeon designer");
    };
    gameTimers.GetNextTimerEntry(&timeEnt, &timerIndex);//TAG01022c
    timerTypeModifier[0] = 0;
    timerTypeModifier[1] = 1;
    timerTypeModifier[2] = 2;
    //D0L = timeEnt.timerTime >> 24; // Level
    D0L = timeEnt.Level(); // Level
    LoadLevel(D0W);
    if ((ui32)d.Long11732 <= (ui32)d.Time)
    {
      function = timeEnt.Function();
    };
    if (   (function >= TT_FIRST_MONSTER_AI)
        && (function <= TT_LAST_MONSTER_AI) )
    {
      ProcessTimers29to41(
                timeEnt.timerUByte6(), //TAG00cbc6
                timeEnt.timerUByte7(),
                timeEnt.Function(),
                timeEnt.timerUByte8());
    }
    else
    {
      bool endOfGame;
      switch (function)
      {
      case TT_Missile0:
      case TT_Missile: MissileTimer(&timeEnt); break;
      case TT_1:  ProcessTT_1(&timeEnt); break;
      case TT_FALSEWALL:    ProcessTT_FALSEWALL(&timeEnt, timerIndex); break;
      case TT_BASH_DOOR:  ProcessTT_BASH_DOOR(&timeEnt); break;
      case TT_DOOR: ProcessTT_DOOR(&timeEnt); break;
      case TT_PITROOM:     ProcessTT_PITROOM(&timeEnt); break;
      case TT_TELEPORTER:  ProcessTT_TELEPORTER(&timeEnt); break;
      case TT_DESSAGE:     ProcessTT_OPENROOM(&timeEnt, timerIndex); break;
      case TT_ParameterMessage:
        {
          ROOMTYPE roomType;
          ui32 targetX, targetY;
          targetY = timeEnt.timerUByte7();
          targetX = timeEnt.timerUByte6();
          roomType = RoomType(d.LevelCellFlags[targetX][targetY]);
          if (roomType != roomSTONE)
          {
            ProcessTT_OPENROOM(&timeEnt, timerIndex); 
            break;
          };
        };
        // Fallthrough!!!
      case TT_STONEROOM:   
                  endOfGame = ProcessTT_STONEROOM(&timeEnt,timerIndex);
                  if (endOfGame)
                  {
                    return true;
                  };
                  break;
      case TT_11:
               chIdx=timeEnt.timerUByte5();
               EnableCharacterAction(chIdx);
               if (timeEnt.timerUByte6() != 0)
               {
                 TAG0115ee(timeEnt.timerUByte5(),
                           timeEnt.timerUByte6()-1);
               };
               D0B = timeEnt.timerUByte5();
               DrawCharacterState(D0W);//top box,pos,dir,weapon
               break;
      case TT_12: TAG011366(timeEnt.timerUByte5()); break;
      case TT_ViAltar: ProcessTT_ViAltar(&timeEnt); break;
      case TT_OPENROOM:     ProcessTT_OPENROOM(&timeEnt, timerIndex); break;
      case TT_60:
      case TT_61: ProcessTimer60and61(&timeEnt); break;//TAG011192
      case TT_ReactivateGenerator: 
               ProcessTT_ReactivateGenerator(&timeEnt); 
               break;
      case TT_20:
               cP4 = 1;
               D0B = timeEnt.timerUByte7();
               cP3 = D0W;
               D0B = timeEnt.timerUByte6();
               cP2 = D0W;
               cP1 = timeEnt.timerWord8();
               QueueSound(cP1, cP2, cP3, cP4);
               break;
      case TT_24:
               D0W = d.Word11694;
               if (D0W != 0) break;
               D0W = 0;
               D0B = timeEnt.timerUByte7();
               cP4 = D0W;
               D0B = timeEnt.timerUByte6();
               cP3 = D0W;
               //cP2 = 0;
               //cP1 = timeEnt.WB8.word;
               RemoveObjectFromRoom(timeEnt.timerObj8(), // Get word 8 as RN
                                    cP3,
                                    cP4,
                                    NULL);
               pwA3=(i16 *)GetCommonAddress(timeEnt.timerObj8());
               //A0 = A3;
               pwA3[0] = -1; //Frees the object
               break;
      case TT_25: ProcessTT_25(&timeEnt); break;
      case TT_53: SetWatchdogTimer(); break;
      case TT_LIGHT:
               LoadLevel(d.partyLevel);
               ProcessLightLevelTimer(&timeEnt);
               SelectPaletteForLightLevel();
               break;
      case TT_71: d.Invisible--; break;
      case TT_72:
               //D0 = (ui8)LOCAL_1[5] *800;
               pcA0 = &d.CH16482[timeEnt.timerUByte5()]; //d.Byte16208;
               //A0 += D0;
               //D0W = wordGear(A0)-wordGear(&LOCAL_1[6]);
               {
                 i32 oldShieldLevel, newShieldLevel;
                 oldShieldLevel = pcA0->shieldStrength/50;
                 pcA0->shieldStrength =
                       uw(pcA0->shieldStrength - timeEnt.timerUWord6());
                 newShieldLevel = pcA0->shieldStrength/50;
                 //if (oldShieldLevel != newShieldLevel)
                 //{
                 //  chIdx = timeEnt.timerUByte5;
                 //  PrintLinefeed();
                 //  QuePrintLines(d.Byte1386[chIdx],pcA0->name);
                 //  QuePrintLines(d.Byte1386[chIdx]," SHIELD DECREASED TO ");
                 //  sprintf(ascii, "%d", pcA0->shieldStrength);
                 //  QuePrintLines(d.Byte1386[chIdx], ascii);
                 //};
               };
               //wordGear(A0) = D0W;
               //D0 = (ui8)LOCAL_1[5] * 800;
               //A0 = d.Byte16434;
               //A0 += D0;
               pcA0->charFlags |= CHARFLAG_portraitChanged;
               //wordGear(A0) |= 0x1000;
               DrawCharacterState(timeEnt.timerUByte5()); //top box etc
               break;
      case TT_73: d.SeeThruWalls--; break;
      case TT_74:
               d.PartyShield = sw(d.PartyShield-timeEnt.timerWord6());
               MarkAllPortraitsChanged();
               break;
      case TT_75:
               //D0W = (ui8)LOCAL_1[5];
               //D7W = D0W;
               param = timeEnt.timerUByte5();
               //D0 = D0W * 800;
               //A0 = d.Byte16440 + D0;
               //(*A0)--;
               pcA0 = &d.CH16482[param];
               pcA0->poisonCount--;
               PoisonCharacter(param,timeEnt.timerWord6());
               break;
      case TT_77:
               d.SpellShield = sw(d.SpellShield-timeEnt.timerWord6());
               MarkAllPortraitsChanged();
               break;
      case TT_78: d.FireShield = sw(d.FireShield - timeEnt.timerWord6());
               MarkAllPortraitsChanged();
               break;
      case TT_79: d.MagicFootprintsActive--; break;
      case TT_22: // It appears that a timer of type 22 is created
               // when a game is restarted from a saved game.
               //TAG00dcf2(&d.Byte11678);  // This function NOP'ed out! Always returns 4.
                // Looks like something deleted here
                // Looks like  if (result==???) d.Word000554=136;
               break;
      default: die(0, "Illegal Timer Entry");
      }; // switch (function)
    };
    LoadLevel(d.partyLevel); //00a9d4
  }; // while()
  //D7=saveD7;
  //A1=saveA3;
  return false;
}

void PortraitTranslateDisable(
               DB3 *pPortrait,
               i32 level,
               i32 x,
               i32 y,
               i32 /*pos*/)
{
  i32 state, disableMessage, action;
  state = pPortrait->State();
  if ((state & PORTRAIT_InActive) != 0) return;
  disableMessage = pPortrait->DisableMessage();
  switch (disableMessage)
  {
  case PORTRAIT_ResponseSet:    action = 0; break;
  case PORTRAIT_ResponseClear:  action = 1; break;
  case PORTRAIT_ResponseToggle: action = 2; break;
  default: return;
  };
  QueueDSASwitchAction(  //Use the DSA function.
               0,     // delay,
               action,// Action set/clear/toggle
               level, x, y, pPortrait->Position(),'M');

}


//          TAG014368
RESTARTABLE _ResurrectReincarnateCancel(const i32 button)
{//()
  static dReg D0, D1, D5, D6, D7;
  static const char* A0;
  static DB3      *DB3A2;
  static RN objD4;
  static i32 mapX, mapY;
  static CHARDESC *pcA3;
  static RectPos LOCAL_12;
  static bool block;
  RESTARTMAP
    RESTART(2)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(d.NumCharacter-1);
  pcA3 = &d.CH16482[D7W];
  if (button == 162) //'Cancel' when selecting champion
  {
    ShowHideInventory(4); //Hide
    d.PotentialCharacterOrdinal = 0;
    if (d.NumCharacter == 1)
    {
      SetHandOwner(-1);
    };
    d.NumCharacter--;
    LOCAL_12.w.y1 = 0;
    LOCAL_12.w.y2 = 28;
    LOCAL_12.w.x1 = sw(69 * D7W);
    LOCAL_12.w.x2 = sw(LOCAL_12.w.x1 + 66);
    STHideCursor(HC26);
    FillRectangle(d.LogicalScreenBase,
                  &LOCAL_12,
                  0,
                  160);
    D0W = (UI8)(pcA3->charPosition+4-d.partyFacing);
    //D0L = (D0W & 3) << 3;
    //A0 = &d.wRectPos1454[D0W&3];
    FillRectangle(d.LogicalScreenBase,
                  &d.wRectPos1454[D0W&3],
                  0,
                  160);
    TAG0207cc();
    STShowCursor(HC26);
  }
  else
  {
    d.PotentialCharacterOrdinal = 0;
    mapX = d.partyX;
    mapY = d.partyY;
    D1W = d.partyFacing;
    D1W = d.DeltaX[D1W];
    mapX += D1W;
    D1W = d.partyFacing;
    D1W = d.DeltaY[D1W];
    mapY += D1W;

    for (D6W=0; D6W<30; D6W++)
    {
      objD4 = pcA3->Possession(D6W);
      if (objD4 != RNnul)
      {
        RemoveObjectFromRoom(objD4, mapX, mapY, NULL);
      };
    };
    block = false;
    for (objD4 = FindFirstObject(mapX, mapY);
         objD4 != RNeof;
         objD4 = GetDBRecordLink(objD4))
    {
      if (objD4.dbType() == dbACTUATOR)
      {
        i32 acType, state;
        DB3A2 = GetRecordAddressDB3(objD4);
        if (((objD4.pos() + 2) & 3) == d.partyFacing)
        {//Look for and disable the first active
         //Champion Portrait
         //Including AND-Gates, etc.
          acType = DB3A2->actuatorType();
          if (acType == 127)
          {
            state = DB3A2->State();
            if ((state & PORTRAIT_InActive) == 0)
            {
              PortraitTranslateDisable(
                        DB3A2,
                        d.LoadedLevel,
                        mapX, mapY, objD4.pos());
              DB3A2->actuatorType(0);
              if (DB3A2->BlockDisables()) block = true;
              break;
            };
          };
        };
      };
    };
    for (objD4 = FindFirstObject(mapX, mapY);
         (!block) && (objD4 != RNeof);
         objD4 = GetDBRecordLink(objD4))
    {
      //D0W = ;
      if (objD4.dbType() == dbACTUATOR)
      {
        i32 acType;
        DB3A2 = GetRecordAddressDB3(objD4);
        if (((objD4.pos() + 2) & 3) == d.partyFacing)
        {//Disable ALL actuators at this location.
        //Including AND-Gates, etc.
          acType = DB3A2->actuatorType();
          if (acType == 127)
          {
            PortraitTranslateDisable(
                      DB3A2,
                      d.LoadedLevel,
                      mapX, mapY, objD4.pos());
            block = DB3A2->BlockDisables();
          };
          DB3A2->actuatorType(0);
        };
      };
    };
    if (button == 161)
    {
      ReIncarnate(_2_, pcA3);
      ClearMemory((ui8 *)pcA3->skills92,120);
      if (!DM_rules)
      {
        for (D6W=1; D6W<=6; D6W++)
        {
          i32 v;
          //D5W = pcA3->Attributes[D6W].Current();
          //D5W >>= 3;
          //D0W = pcA3->Attributes[D6W].Current();
          //D0W = sw(D0W - D5W);
          v = pcA3->Attributes[D6W].Current();
          v -= v * reincarnateAttributePenalty / 16;
          v = Larger (v, pcA3->Attributes[D6W].Minimum());
          pcA3->Attributes[D6W].Current(v);
          pcA3->Attributes[D6W].Maximum(v);
          //D1W = pcA3->Attributes[D6W].Minimum();
          //D0W = sw(Larger(D1W, D0W));
          //pcA3->Attributes[D6W].Maximum(D0W);
          //pcA3->Attributes[D6W].Current(D0W);
//
        };
      };
      if (!DM_rules)
      {
        pcA3->HP(pcA3->HP() * (16 - reincarnateStatPenalty) / 16);
        pcA3->MaxHP(pcA3->MaxHP() * (16 - reincarnateStatPenalty) / 16);
        pcA3->Stamina(pcA3->Stamina() * (16 - reincarnateStatPenalty) / 16);
        pcA3->MaxStamina(pcA3->MaxStamina() * (16 - reincarnateStatPenalty) / 16);
        pcA3->Mana(pcA3->Mana() * (16 - reincarnateStatPenalty) / 16);
        pcA3->MaxMana(pcA3->MaxMana() * (16 - reincarnateStatPenalty) / 16);
        //pcA3->HP(pcA3->HP() >> 1);
        //pcA3->MaxHP(pcA3->MaxHP() >> 1);
        //pcA3->Stamina(pcA3->Stamina() >> 1);
        //pcA3->MaxStamina(pcA3->MaxStamina() >> 1);
        //pcA3->Mana(pcA3->Mana() >> 1);
        //pcA3->MaxMana(pcA3->MaxMana() >> 1);
      };
      //D6W = 0;
      for (D6W=0; D6W<12; D6W++)
      {
        D5W = sw((STRandom() & 0xffff) % 7);
        pcA3->Attributes[D5W].IncrCurrent(1);
        pcA3->Attributes[D5W].IncrMaximum(1);

//
      };
    };
    if (d.NumCharacter == 1)
    {
      d.LastPartyMoveTime = d.Time;
      SetHandOwner(0);
      SelectMagicCaster(0);
    }
    else
    {
      DrawSpellControls(d.MagicCaster);
    };
    PrintLinefeed();
    QuePrintLines(D6W = (UI8)(d.Byte1386[D7W]), pcA3->name);
    if (button == 160)
    {
      A0 = " RESURRECTED";
    }
    else
    {
      A0 = " REINCARNATED";
    };
      QuePrintLines(D6W, TranslateLanguage(A0));
      ShowHideInventory(4); //Hide
      TAG0207cc();
    if (d.HandChar == -1) // owner of cursor
    {
      D0W = 0;
    }
    else
    {
      D0W = 1;
    };
    SetCursorShape(D0W);
  };
  RETURN;
}

// *********************************************************
//
// *********************************************************
//   TAG015396
void DrawAllCharacterState()
{//()
  static i32 i;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //What if we simply don't do this????NotImplemented(0x99977a);//wvbl(_1_);
  for (i=0; i<d.NumCharacter; i++)
  {
    DrawCharacterState(i);//box at top, pos, dir, weapon
  };
  return;
}

//   TAG0156c4
void ObjectToCursor(RN object,i16 P2)
{//()
  	// If object is non-NULL then it is put into the
    // hand and its weight is accounted for.
    // The hand must be empty when you call this!!!  Else
    // I fear some weight will be lost.
  dReg D1;
  CHARDESC *pcA0;
  OBJ_NAME_INDEX objNI;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (object == RNnul) return;
  d.EmptyHanded = 0;
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile), " Clear EmptyHanded in ObjectToCursor\n");
  };
  ASSERT(RememberToPutObjectInHand == -1,"objInHand");

  d.objectInHand = object;

  objNI = d.objectInHand.NameIndex();
  d.ObjectType16574 = (I16)objNI;
  GetIconGraphic(objNI, (ui8 *)d.Pointer16572);
  STHideCursor(HC27);
  DrawNameOfHeldObject(object);
  if (P2)
  {
    d.ShowCursor16572 = 1;
  }
  else
  {
    CreateObjectCursor(d.Pointer16572);
  };
  STShowCursor(HC27);
  if (d.HandChar == -1) return; //owner of cursor
  D1L = GetObjectWeight(object);
  pcA0 = &d.CH16482[d.HandChar]; //owner of cursor
  DEBUGLOAD(pcA0, D1W, +1, object);
  pcA0->load = sw(pcA0->load + D1W);
  ASSERT(pcA0->load < MAXLOAD,"maxload");
  pcA0->charFlags |= CHARFLAG_possession;
  DrawCharacterState(d.HandChar); //owner of cursor
}

//  TAG01479c
i16 GetCharacterToDamage(i32 attackerX, i32 attackerY , i32 attackerAbsPos)
{ // We got here when we ran into a wall.
  // Direction appears to be 180 degrees from
  // direction party just moved.  Perhaps it is the
  // side of the cell we encountered.
  // We also got here when a Deth Knight attacked the
  // party.  In that case, direction was STRandom0_3().
  // Probably the position within the monster party
  // from which tha attack came???.
  dReg D6, D7;
  i32 xDist, yDist;
  ui8 attackedAbsPos[4];
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.NumCharacter == 0) return -1;
  xDist = abs(attackerX - d.partyX); // How far we moved?
  yDist = abs(attackerY - d.partyY);
  if (xDist + yDist > 1) return -1;
  //Pretend we are being attacked by the wall!  The
  //two people closest to the wall are affected.
  DetermineAttackOrder(attackedAbsPos,
                       d.partyX, d.partyY,
                       attackerX,attackerY,attackerAbsPos);
  for (D7W=0; D7W<4; D7W++)
  {
    D6L = CharacterAtPosition(attackedAbsPos[D7W]);
    if (D6L >= 0)
    {
      return D6W;
    };
//
  };
  return -1;
}

// *********************************************************
// 20 has already been subtracted from button number.
// It should be 0 to 7 for the 8 hands shown in
// in the 4 portrait areas.  It should be 8 to 20
// for the clothes on the 'paper doll'.  It should be
// 21 to 37 for the backpack items.  It will be 30 to
// 37 for items in the chest.
//
// *********************************************************
//   TAG015e0c
void HandleClothingClick(i32 button)
{//()
  dReg D0, D6, D7;
  RN objD4, objD5;
  //;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (button < 8)
  {
    if (d.PotentialCharacterOrdinal != 0) return;
    D7W = sw(button);
    D7W /= 2;
    if (D7W >= d.NumCharacter) return;
    if (D7W+1 == d.SelectedCharacterOrdinal) return;
    if (d.CH16482[D7W].HP() == 0) return;
    D6W = (I16)(button & 1); // Left or right hand.
  }
  else
  {
    D7W = sw(d.SelectedCharacterOrdinal-1);
    D6W = sw(button-8);
  };
  ASSERT(RememberToPutObjectInHand == -1,"objInHand");
  objD5 = d.objectInHand;
  if (D6W >= 30)
  {
    objD4 = d.rnChestContents[D6W-30];
  }
  else
  {
    objD4 = d.CH16482[D7W].Possession(D6W);
  };
  if ( (objD4==RNnul) && (objD5==RNnul) ) return;
  if (objD5 != RNnul)
  {
    //D0W = GetObjectDescIndex(objD5);
    D0W = d.ObjDesc[objD5.DescIndex()].word4;// Possible carry locations
    if ((D0W & d.CarryLocation[D6W]) == 0) return;
  };
  {
    CURSORFILTER_PACKET cfp;
    cfp.type = CURSORFILTER_PlaceOnCharacter;
    cfp.object = objD5.ConvertToInteger();
    cfp.p1 = D7W;
    cfp.p2 = D6W;
    CursorFilter(&cfp);
    if (cfp.type == CURSORFILTER_Cancel) return;
  };

  STHideCursor(HC28);
  if (objD5 != RNnul)
  {
    RemoveObjectFromHand();
  };
  if (objD4 != RNnul)
  {
    CURSORFILTER_PACKET cfp;
    RemoveCharacterPossession(D7W, D6W);
    cfp.object = objD4.ConvertToInteger();
    cfp.type   = CURSORFILTER_PickFromCharacter;
    cfp.p1     = D7W;
    cfp.p2     = D6W;
    if (objD4 != RNnul)
    {
      CursorFilter(&cfp);
    };
    ObjectToCursor(objD4, 0);
  };
  if (objD5 != RNnul)
  {
    AddCharacterPossession(D7W, objD5, D6W);
  };
  DrawCharacterState(D7W);
  STShowCursor(HC28);
}

//  TAG01657e
i16 TimeToMove(CHARDESC *pChar)
{ // Speed of character???
  dReg D0, D1, D6, D7;
  i16 time;
  OBJ_NAME_INDEX objNID0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //pcA3 = chIdx
  D6L = MaxLoad(pChar);
  D7W = pChar->load;
  if (D6L > D7W)
  {
    time = 2;
    D0L = 8 * (D7W & 0xffff);
    D1L = 5 * (D6L & 0xffff);
    //D1L = 5 * D1L;
    if (D0L > D1L)
    {
      time++;
    };
    D7W = 1;
  }
  else
  {
    time = 4;
    D1L = (4 * (D7W - D6L)) & 0xffff;
    time = sw(time + D1L / D6L);
    D7W = 2;
  };
  if (pChar->ouches & 32)
  {
    time = sw(time + D7W);
  };
  objNID0 = pChar->Possession(5).NameIndex(); // feet
  if (objNID0 == objNI_BootsOfSpeed)
  {
    time--;
  };
  return time;
}

// *********************************************************
//
// *********************************************************
//   TAG0169a8
void WakeUp()
{//()
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  d.clockTick = 1;
  d.PartySleeping = 0;
  VBLperTimer = 10;
  scrollingText.SetNewSpeed(d.Speed, speedTable[gameSpeed].vblPerTick);
  d.Speed = sw(speedTable[gameSpeed].vblPerTick);
  FloorAndCeilingOnly();
  d.PrimaryButtonList = &d.Buttons17196[0];
  d.SecondaryButtonList = &d.Buttons17304[0];
  d.pKeyXlate2 = (KeyXlate *)&d.Byte18400;
  d.pKeyXlate1 = (KeyXlate *)&d.Byte18428;
  DiscardAllInput();
  TAG0207cc();
  videoMode = VM_ADVENTURE;
}

//  TAG0169ec
i16 SearchFootprints(i32 mapX, i32 mapY)
{
  i32 index;
  i16 searchValue;
  searchValue = sw(  ((d.LoadedLevel & 63) << 10)
                   | (mapY << 5)
                   | (mapX & 31)  );
  for (index = d.NumFootprintEnt - 1; index >= 0; index--)  //entry count
  {
    if (d.PartyFootprints[index] == searchValue)
    {
      return sw(index + 1);
    };
  };
  return 0;
}

//   TAG016e54
void DisplayCharacterDamage()
{//()
  dReg D0, D4, D5, D6, D7;
  const char* A0;
  TIMER *ptA2;
  CHARDESC *pcA3;
  RectPos LOCAL_20;
  TIMER LOCAL_12;
  i16 LOCAL_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  Instrumentation(icntDisplayCharacterDamage);
  pcA3 = d.CH16482;
  for (D7W=0; D7W < d.NumCharacter; D7W++, pcA3++)
  {
    D4W = d.PendingOuches[D7W];
    pcA3->ouches |= D4W;
    d.PendingOuches[D7W] = 0;
    D6W = d.PendingDamage[D7W];
    if (D6W == 0) continue;
    d.PendingDamage[D7W] = 0;
    if (d.PotentialCharacterOrdinal == D7W+1) continue;
    D5W = pcA3->HP();
    if (D5W== 0) continue;
    D5W = sw(D5W - D6W);
    if (D5W <= 0)
    {
      KillCharacter(D7W);
      continue;
    };
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),
              " Character %d HP goes from %d to %d\n",
              D7W, pcA3->HP(), D5W);
    };
    pcA3->HP(D5W);
    pcA3->charFlags |= CHARFLAG_statsChanged;
    if (D4W)
    {
      pcA3->charFlags |= CHARFLAG_ouches;
    };
    D5L = 69 * D7W;
    LOCAL_20.w.y1 = 0;
    STHideCursor(HC29);
    if (D7W + 1 == d.SelectedCharacterOrdinal)
    {
      LOCAL_20.w.y2 = 28;
      LOCAL_20.w.x1 = sw(D5W + 7);
      LOCAL_20.w.x2 = sw(LOCAL_20.w.x1 + 31);
      BLT2Screen(GetBasicGraphicAddress(16), &LOCAL_20, 16, 10);
      if (D6W < 10)
      {
        D5W += 21;
      }
      else
      {
        if (D6W < 100)
        {
          D5W += 18;
        }
        else
        {
          D5W += 15;
        };
      };
      LOCAL_2 = 16;
    }
    else
    {
      LOCAL_20.w.y2 = 6;
      LOCAL_20.w.x1 = D5W;
      LOCAL_20.w.x2 = sw(LOCAL_20.w.x1 + 47);
      BLT2Screen(GetBasicGraphicAddress(15),&LOCAL_20,24,10);
      if (D6W < 10)
      {
        D5W += 19;
      }
      else
      {
        if (D6W < 100)
        {
          D5W += 16;
        }
        else
        {
          D5W += 13;
        };
      };
      LOCAL_2 = 5;
    };
    A0 = TAG014af6(D6W, 0, 3);
    TextOutToScreen(D5W, LOCAL_2, 15, 8, A0);
    D5W = pcA3->timerIndex;
    // Set a timer to erase the HP damage display.
    if (D5W == -1)
    {
      LOCAL_12.Function(TT_12);
      D0L = d.Time + 5;
      //D1L = d.partyLevel;
      //D3L = 24;
      //D1L <<= D3W;
      //D0L |= D1L;
      //LOCAL_12.timerTime = D0L;
      LOCAL_12.Time(D0L);
      LOCAL_12.Level((ui8)d.partyLevel);
      LOCAL_12.timerUByte5((i8)D7W);
      pcA3->timerIndex = gameTimers.SetTimer(&LOCAL_12);
    }
    else
    {
      ptA2 = gameTimers.pTimer(D5W);
      D0L = d.Time+5;
      //D1L = d.partyLevel << 24;
      //ptA2->timerTime = D0L | D1L;
      ptA2->Time(D0L);
      ptA2->Level((ui8)d.partyLevel);
      //AdjustTimerPosition(FindTimerPosition(D5W));
      gameTimers.AdjustTimerPriority(D5W);
    };
    DrawCharacterState(D7W);
    STShowCursor(HC29);
   //continue here

  };
}

//   TAG017274
void PoisonCharacter(i32 chIdx,i32 P2)
{//()
  CHARDESC  *pch_14;
  TIMER    timer;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (chIdx == -1) return;
  if (chIdx+1 == d.PotentialCharacterOrdinal) return;
  pch_14 = &d.CH16482[chIdx];
  DamageCharacter(chIdx, Larger(1, P2/64), 0, 0);
  pch_14->charFlags |= CHARFLAG_statsChanged;
  if (    (chIdx+1 == d.SelectedCharacterOrdinal)
       && (d.DisplayResurrectChestOrScroll == 0)   )
  {
    pch_14->charFlags |= CHARFLAG_chestOrScroll;
  };
  P2--;
  if (P2 != 0)
  {
    pch_14->poisonCount++;
    timer.Function(TT_75);
    timer.timerUByte5((i8)chIdx);
    //timer.timerTime = (d.Time+36)|(d.partyLevel<<24);
    timer.Time(d.Time+36);
    timer.Level((ui8)d.partyLevel);
    timer.timerWord6() = sw(P2);
    gameTimers.SetTimer(&timer);
  };
  DrawCharacterState(chIdx);
}

//   TAG017fa8
void SelectPaletteForLightLevel()
{
  dReg D0, D4, D5, D6;
  RN objD0;
  OBJ_NAME_INDEX objNID0;
  i16  LOCAL_26[8];
  RN   LOCAL_10;
  i16 temp;
  CHARDESC *pCharacter;
  i16 *pwA0, *pwA2, *pwA3;
  i32 totalLight;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D0W = d.pCurLevelDesc->experienceMultiplier();
  if (D0W == 0)
  {
    d.CurrentPalette = 0;
  }
  else
  {
    pCharacter = &d.CH16482[0];
    pwA3 = LOCAL_26;
    for (D5W=0; D5W<4; D5W++,pCharacter++)
    {
      for (D4W=2; (D4W--)!=0; pwA3++)
      {
        *pwA3 = 0;
        if (D5W >= d.NumCharacter) continue;
        objD0 = pCharacter->Possession(D4W);
        LOCAL_10 = objD0;
        objNID0 = GetBasicObjectType(objD0);
        if (   (objNID0 >= objNI_Torch_a)
            && (objNID0 <= objNI_Torch_d) )
        {
          pwA0 = (i16 *)GetCommonAddress(LOCAL_10);
          *pwA3 = ((DB5 *)pwA0)->charges();
          continue;
        };
      };
    };
    pwA3 = LOCAL_26;
    for (D4W=0; D4W!=4; D4W++, pwA3++)
    { // Find four brightest torches.
      D5W = sw(7 - D4W);
      for (pwA2=&LOCAL_26[D4W+1]; (D5W--)!=0; pwA2++)
      {
        if (*pwA2 > *pwA3)
        {
          temp = *pwA2;
          *pwA2 = *pwA3;
          *pwA3 = temp;
        };
      };
    };
    D6W = 6;
    D4W = 5;
    totalLight = 0;
    for (pwA3=LOCAL_26; (D4W--)!=0; pwA3++)
    {
      if (*pwA3 == 0) continue;
      D0W = d.Word1074[*pwA3];
      D0W <<= D6W;
      D0W = (I16)((D0W >> 6) & 0x3ff); // logical shift
      totalLight += D0W; // Sum of torches
      D6W = sw(Larger(0, D6W-1));

    };
    totalLight += d.Brightness;
    pwA3 = d.PaletteBrightness;
    if (totalLight > 0)
    {
      D4W = 0;
      while (*(pwA3++) > totalLight)
      {
        D4W++;
      };
    }
    else
    {
      D4W = 5;
    };
    d.CurrentPalette = D4W;
//    {
//      char comment[80];
//      int i;
//      sprintf(comment,"totalLight=%d CurrentPalette=%d",totalLight,d.CurrentPalette);
//      RecordFile.Comment(comment);
//      for (i=0; i<16; i++)
//      {
//        sprintf(comment,"   %d %04x", i, d.Palette552[d.CurrentPalette].color[i] & 0xffff);
//        RecordFile.Comment(comment);
//      };
//    };
  };
  d.Word11778 = 1;
}

//   TAG018124
void NinetySecondUpdate()
{ //()
  // Decrement bits 10-13 of weapon types 4, 5, 6, and 7
  dReg D0, D3, D5, D6, D7;
  OBJ_NAME_INDEX objNID4;
  DB5      *DB5A2;
  CHARDESC *pcA3;
//#ifdef _DEBUG
//  verifyppq();
//#endif
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D5W = 0;
  D7W = d.NumCharacter;
  if (d.PotentialCharacterOrdinal != 0) D7W--;
  pcA3 = d.CH16482;
  while ((D7W--) != 0)
  {
    for (D6W=2; (D6W--)!= 0;) // One and zero.  Hands.
    {
      objNID4 = pcA3->Possession(D6W).NameIndex();
      if (objNID4 < objNI_Torch_a) continue;
      if (objNID4 > objNI_Torch_d) continue;
      DB5A2 = GetRecordAddressDB5(pcA3->Possession(D6W));
      D0W = DB5A2->charges();
      if (D0W == 0) continue;
      D3W = (I16)((D0W-1) & 15);
      //D3W <<= 10;
      //dbA2->word(2) &= 0xc3ff;
      //dbA2->word(2) |= D3W;
      DB5A2->charges(D3W);
      if (D3W == 0)
      {
        DB5A2->important(false);
      };
      D5W = 1;
//
    };
    pcA3++;

  };
  //if (0)
  //{
  //};
  //if (D5W != 0)
  {
    SelectPaletteForLightLevel();
    DrawEightHands();
  };
}

//   TAG019018
void QuitPressingMouth()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TAG0189d4();
  MarkViewportUpdated(0);
  STShowCursor(HC5);
}

//   TAG019264
void QuitPressingEye()
{//()
  RN objD7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DrawSmallIcon(objNI_Special_d, 12, 13);//Eye back to normal.
  ASSERT(RememberToPutObjectInHand == -1,"objInHand");
  if (d.objectInHand == d.objOpenChest) RepackChest();
  TAG0189d4();
  MarkViewportUpdated(0);
  objD7 = d.objectInHand;
  if (objD7 != RNnul)
  {
    DrawNameOfHeldObject(objD7);
  };
  STShowCursor(HC47);
}
// *********************************************************
//
// *********************************************************
//   TAG0192f4
void ShowHideInventory(i32 chIdx)
{//()
  //
  // if (pressingEye or pressingMouth) return;
  // if (chIdx is dead) return;
  // if (chIdx inventory open) chidx = 4;
  // if (inventory open) close inventory;
  // if (party sleeping) return
  // if (chidx == 4) return;
  // open inventory for chIdx;
  //
  dReg D0, D6;
  CHARDESC *pcA3;
  i32 itemNum;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (chIdx != 4)
  {
    D0W = d.CH16482[chIdx].HP();
    if (D0W == 0) return; //Don't show inventory of dead character
  };
  if (d.PressingMouth != 0) return;
  if (d.PressingEye != 0) return;
  d.clockTick = 1;
//
//
  D6W = d.SelectedCharacterOrdinal;
  if ((chIdx+1) == D6W)
  { // If the inventory is already open for this character
    chIdx = 4;  //Change a 'Show' to a 'Hide' function
  };
  STHideCursor(HC30);
  if (D6W)  // If an inventory screen is open, close it.
  {
    d.SelectedCharacterOrdinal = 0;
    videoMode = VM_ADVENTURE;
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"%06x ShowHideInventory calling Repack Chest\n",d.Time);
    };
    RepackChest();
    pcA3 = &d.CH16482[D6W-1];
    if (pcA3->HP())
    {
      if (d.PotentialCharacterOrdinal == 0)
      {
        pcA3->charFlags |= CHARFLAG_portraitChanged;
        DrawCharacterState(D6W-1);//box at top, pos, dir,weapon
      };
    };
    if (d.PartySleeping)
    {
      STShowCursor(HC30);
      return;
    };
    if (chIdx == 4)
    {
      d.ShowCursor1 = 1;
      DrawMovementButtons();
      STShowCursor(HC30);
      d.SecondaryButtonList = d.Buttons17304;
      d.pKeyXlate1 = d.Byte18428;
      DiscardAllInput();
      FloorAndCeilingOnly(); //Clears viewport to floor and ceiling
      return;
    };
  };
  d.UseByteCoordinates = 0;
  d.SelectedCharacterOrdinal = sw(chIdx + 1);
  videoMode = VM_INVENTORY;
  if (D6W == 0)
  {
    ShadeRectangleInScreen((RectPos *)d.Word28, 0);
  };
  pcA3 = &d.CH16482[chIdx];
  TAG022a60(17, d.pViewportBMP);
  if (d.PotentialCharacterOrdinal)
  {
    FillRectangle(d.pViewportBMP, (RectPos *)d.Byte1094, 12, 112);
  };
  TextToViewport(5, 116, COLOR_13, "HEALTH", true); 
  TextToViewport(5, 124, COLOR_13, "STAMINA", true);
  TextToViewport(5, 132, COLOR_13, "MANA", true);   

  for (itemNum=0; itemNum<30; itemNum++)
  {
    DisplayBackpackItem(chIdx, itemNum);
  };
  pcA3->charFlags |=   CHARFLAG_viewportChanged
                     | CHARFLAG_portraitChanged
                     | CHARFLAG_chestOrScroll
                     | CHARFLAG_possession
                     | CHARFLAG_statsChanged
                     | CHARFLAG_cursor
                     ;
  //NotImplemented(0x99977b);//wvbl(_1_);
  DrawCharacterState(chIdx); // Box at top,pos,dir,weapon

  d.NewCursorShape = 1;
  STShowCursor(HC30);
  d.SecondaryButtonList = d.Buttons17760;
  d.pKeyXlate1 = NULL;
  DiscardAllInput();
  return;
}

i16 TAG0194c0(pnt)
{
  d.Word4012 = 3217;
  //d.Word19218 = 12777;
  return 1;
}

//   TAG01953a
void DiscardAllInput()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  while (UI_CONSTAT()!=0) UI_DIRECT_CONIN();
  {

  };
  d.MouseSwitches = 0;
  d.MouseInterlock = 1;
  d.MouseQStart = 0;
  d.MouseQEnd = MOUSEQLEN-1;
  d.MouseInterlock = 0;
}

i16 SearchButtonList(btn *ent,i32 x,i32 y,i32 buttons) //TAG019570
{
  dReg D0, D4, D5, D6, D7;
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //SaveRegs(0x0f10);
  //A3 = firstEnt;
  D7W = sw(x);
  if ( (D7W >= 512) && (D7W <= 515)) D7W = 0xee;
  D6W = sw(y);
  D5W = sw(buttons);
  if ((ent == NULL) || (d.Word16868 != 0))
  {
    D0L=0;
  }
  else
  {
    while ((D4W = ent->word0) != 0)
    {
      if (   (D7W <  ent->xMax)
          && (D7W >= ent->xMin)
          && (D6W >= ent->yMin)
          && (D6W <  ent->yMax) )
      {


        if (D5W & ent->button) break;
      }
      ent++;
    };
    D0W = D4W;
  };
  //RestoreRegs(0x08f0);
  return D0W;
}

void OnMouseUnClick()
{
  i32 numEnt;
  d.MouseInterlock = 1;
  d.MouseSwitches = 0;
  numEnt = d.MouseQEnd - d.MouseQStart + 1;
  numEnt = (numEnt + MOUSEQLEN) % MOUSEQLEN;
  if (numEnt < MOUSEQLEN-1) //There is room for only four entries.
  { 
      d.MouseQEnd++;
      if (d.MouseQEnd > MOUSEQLEN-1) d.MouseQEnd -= MOUSEQLEN;
      //d.MouseQEnd = D5W;
    //A0 = d.Pointer16848 + D5W * 6;
      pMouseQueue[d.MouseQEnd].num = 0xadd;  // Added type
    //A0 = d.Pointer16852 + D5W * 6;
      pMouseQueue[d.MouseQEnd].x = 0;
    //A0 = d.Pointer16850 + 6 * D5W;
      pMouseQueue[d.MouseQEnd].y =  0;
  };
  d.MouseInterlock = 0;
};




//   TAG0195da
void OnMouseClick(i32 x,i32 y,i32 buttons)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D0, D4, D6, D7;
  i32 numEnt;
  //SaveRegs(0x0f00);
  D7W = sw(x);
  D6W = sw(y);
  if (d.MouseInterlock != 0)
  {
    d.FakeMouseClick = 1;
    d.FakeMouseX = D7W;
    d.FakeMouseY = D6W;
    d.FakeMouseButton = sw(buttons);
    D0W = d.Word16868;
  }
  else
  {
    d.MouseInterlock = 1;
    numEnt = d.MouseQEnd - d.MouseQStart + 1;
    numEnt = (numEnt + MOUSEQLEN) % MOUSEQLEN;
    if (numEnt < MOUSEQLEN-2)
    { 
      //We cannot add a fourth entry because then there would be no
      //room for a matching 'UnClick' (button release).
      if (keyboardMode == 2) // Reincarnate mode
      {
        D4W = 0xade;
      }
      else
      {
        D4W = SearchButtonList(d.PrimaryButtonList,
                                D7W,
                                D6W,
                                buttons);
        if (D4W == 0)
        {
        D4W = SearchButtonList(d.SecondaryButtonList,
                                D7W,
                                D6W,
                                buttons);
        };
#ifdef MAEMO_NOKIA_770
        // The Nokia 770 has only a stylus as a mouse pointer.
	// There is therefore no concept of a "right mouse click".
	// As a result, for the 770, after checking to see if a
	// mouse click was a left mouse click, and finding that
	// it was not, we check to see if it may have really been
	// a right mouse click.
	// Note that if buttons == 0x00 then that's a mouse up,
	// so we don't do these checks
        if ((D4W == 0) && (buttons == 0x2) && (x < 550) && (y < 60))
        {
               D4W = SearchButtonList(d.PrimaryButtonList,
                               D7W,
                               D6W,
                               0x1);
               if (D4W == 0)
               {
                       D4W = SearchButtonList(d.SecondaryButtonList,
                                       D7W,
                                       D6W,
                                       0x1);
               }
        }
#endif
      };
       if ((D4W==0) && IsTextScrollArea(D7W, D6W)) 
      {
        D4W = 216;
        D7W = 0;
        D6W = 0;
      };     
      if (D4W != 0)
      {
        d.MouseQEnd++;
        if (d.MouseQEnd > MOUSEQLEN-1) d.MouseQEnd -= MOUSEQLEN;
        //d.MouseQEnd = D5W;
      //A0 = d.Pointer16848 + D5W * 6;
        pMouseQueue[d.MouseQEnd].num = D4W;
      //A0 = d.Pointer16852 + D5W * 6;
        pMouseQueue[d.MouseQEnd].x = D7W;
      //A0 = d.Pointer16850 + 6 * D5W;
        pMouseQueue[d.MouseQEnd].y =  D6W;
      };
      d.MouseInterlock = 0;
    };
  };
  //RestoreRegs(0x00f0);
}


//   TAG0196b6
void CauseFakeMouseClick()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.FakeMouseClick != 0)
  {
    d.FakeMouseClick = 0;
    OnMouseClick(d.FakeMouseX,
                 d.FakeMouseY,
                 d.FakeMouseButton);
  };
}

//   TAG01979c
RESTARTABLE _FlashButton(const i32 x1,
                         const i32 x2,
                         const i32 y1,
                         const i32 y2)
{//()
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
  while (d.FlashButnActive != 0) {};
  d.FlashButnX1 = sw(x1);
  d.FlashButnX2 = sw(x2);
  d.FlashButnY1 = sw(y1);
  d.FlashButnY2 = sw(y2);
  d.FlashButnActive = 1;
  wvbl(_1_);
  display();
  RETURN;
}

//   TAG0197f6
void TraverseStairway(i32 UpOrDown, bool setFacing)
{ //()
  // non-zero =decrement level = up
  i32 x, y;
  i16 newPartyLevel, newPartyFacing;
  PARTYMOVEDATA partyMoveData;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  // Compute relative x and y on the new level.
  x = d.partyX;
  y = d.partyY;
  newPartyLevel = IncrementLevel(
                               d.partyLevel,
                               UpOrDown == 0 ? 1 : -1,
                               &x,
                               &y);


  newPartyFacing = ExitStairsDirection(x, y, newPartyLevel);
  {
// *********Check the party move filter*****************
    LOCATIONREL locr;
    partyMoveData.moveType = PM_STAIRWAY;
    partyMoveData.relDirection = partyMoveData.absDirection = UpOrDown+2;
    partyMoveData.flags = 0;
    locr.l = d.partyLevel;
    locr.x = d.partyX;
    locr.y = d.partyY;
    locr.p = d.partyFacing;
    partyMoveData.fromLocation = locr.Integer();
    partyMoveData.fromLocationType = BITS5_15(GetCellFlags(d.partyX, d.partyY));
    locr.l = newPartyLevel;
    locr.x = x;
    locr.y = y;
    partyMoveData.toLocation = locr.Integer();
    partyMoveData.toLocationType = 3;

    CallPartyMoveFilter(&partyMoveData);
    if (partyMoveData.flags & PM_INHIBITMOVE) return;
  };


  MoveObject(RN(RNnul), d.partyX, d.partyY, -1, 0, NULL, NULL);
  d.newPartyLevel = newPartyLevel;
  d.partyX = sw(x);
  d.partyY = sw(y);
  LoadLevel(d.newPartyLevel);
  if (setFacing)
  {
    SetPartyFacing(newPartyFacing);
  };
  LoadLevel(d.partyLevel);  // Back to original level.
}



void CallPartyMoveFilter(PARTYMOVEDATA *pmd)
{
// See if there is a party move filter.
  ui32 key;
  RN partyMoveDSA;
  LOCATIONREL partyMoveFilterLocr;
  i32 len;
  ui32 *pRecord;
  key = (EDT_SpecialLocations<<24)|ESL_PARTYMOVEFILTER;
  len = expool.Locate(key,&pRecord);
  if (len > 0)
  //Let the DSA process the parameters
  {
    RN obj;
    TIMER timer;
    i32 objectDB, actuatorType;
    DB3 * pActuator;
    NEWDSAPARAMETERS dsaParameters;
    partyMoveFilterLocr.Integer(*pRecord);
    for (obj = FindFirstObject(
                    partyMoveFilterLocr.l,
                    partyMoveFilterLocr.x,
                    partyMoveFilterLocr.y);
        obj != RNeof;
        obj = GetDBRecordLink(obj))
    {
      objectDB = obj.dbType();
      if (objectDB == dbACTUATOR)
      {
        pActuator = GetRecordAddressDB3(obj);
        actuatorType = pActuator->actuatorType();
        if (actuatorType == 47)
        {
          partyMoveDSA = obj;
          {
            TIMER timerA;
            int currentLevel;
            DSAVARS dsaVars;
            currentLevel = d.LoadedLevel;
            LoadLevel(partyMoveFilterLocr.l);
            timerA.timerUByte9(0);//timerFunction
            timerA.timerUByte8(0);//timerPosition
            timerA.timerUByte7((ui8)partyMoveFilterLocr.y);
            timerA.timerUByte6((ui8)partyMoveFilterLocr.x);
            //timerA.timerTime   = partyMoveFilterLocr.l << 24;
            timerA.Time(0);
            timerA.Level((ui8)partyMoveFilterLocr.l);
            pDSAparameters[0] = sizeof(PARTYMOVEDATA)/4;
            memcpy(pDSAparameters+1, pmd, sizeof(*pmd));

            ProcessDSAFilter(partyMoveDSA, &timerA, partyMoveFilterLocr, NULL, &dsaVars);
            memcpy(pmd, pDSAparameters+1, sizeof(*pmd));
            LoadLevel(currentLevel);
          };
          break;
        };
      };
    };
  };
  return;
}




//   TAG01986c
RESTARTABLE _TurnParty(const i32 direction)
{ //()
  // 1 = left
  // 2 = right
  static dReg D0, D7;
  static i32 deltaFacing;
  static int cellFlags;
  RESTARTMAP
    RESTART(2)
    RESTART(3)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (extraTicks)
  {
    d.clockTick = 1;
  };
  {
    LOCATIONREL locr;
    cellFlags = GetCellFlags(d.partyX, d.partyY);
    locr.l = d.partyLevel;
    locr.x = d.partyX;
    locr.y = d.partyY;
    locr.p = d.partyFacing;
    PARTYMOVEDATA partyMoveData;
    partyMoveData.fromLocation = locr.Integer();
    locr.p = (locr.p + ((direction == 0)?1:-1)) & 3;
    partyMoveData.toLocation   = locr.Integer();
    partyMoveData.flags = 0;
    partyMoveData.moveType = PM_BEGINTURN;
    partyMoveData.relDirection = partyMoveData.absDirection = direction;
    partyMoveData.fromLocationType = BITS5_15(cellFlags);
    partyMoveData.toLocationType = partyMoveData.fromLocationType;
    if (partyMoveData.fromLocationType == 3)
    {
      partyMoveData.absDirection = partyMoveData.relDirection = (cellFlags&4)?3:2;
    };
    CallPartyMoveFilter(&partyMoveData);
    if (partyMoveData.flags & PM_INHIBITMOVE)
    {
      RETURN;
    };
  };
  if (direction == 1) // Turn left
  {
    FlashButton(_2_,0xea, 0x0105, 0x7d, 0x91);
  }
  else // Turn right
  {
    FlashButton(_3_,0x123, 0x13e, 0x7d, 0x91);
  };
  //D0W = GetCellFlags(d.partyX, d.partyY);
  D7W = (I16)(cellFlags & 0xff);
  if (BITS5_15(cellFlags) == 3)
  {
    TraverseStairway(D7W & 4, true); //non-zero = down
  }
  else
  {
    PlaceOrRemoveObjectInRoom(
                      d.partyX,
                      d.partyY,
                      RN(RNnul),
                      true,  // In current room (room unchanged)
                      0,     // 'remove' rather than 'place'
                      NULL);
    if (direction == 2)
    {
      deltaFacing = 1; // Increment to facing
    }
    else
    {
      deltaFacing = 3; // Increment to facing
    };
    SetPartyFacing((d.partyFacing + deltaFacing) & 3);
    PlaceOrRemoveObjectInRoom(
                      d.partyX,
                      d.partyY,
                      RN(RNnul),
                      true,       // In current room (room unchanged)
                      1,          // 'place' rather than 'remove'
                      NULL);
  };
  RETURN;
}


// *********************************************************
//
// *********************************************************
//   TAG01992e
MOVEBUTN *MoveParty(const i32 button)
{//()
  // 01 Turn Left
  // 02 Turn Right
  // 03 Move forward
  // 04 Slide right
  // 05 Move backward
  // 06 Slide Left
  // 0x1124 Move Down
  // 0x1125 Move Up
  dReg D0, D5;
//  ROOMTYPE rtD6;
  i32 cellFlagsFrom, cellFlagsTo;
  ROOMTYPE rtFrom, rtTo;
  RN objD0;
  i32 charIdx;
  i32 i;
  bool bD0, bD5;
  ui16 LOCAL_30;
  i32 maxLoad;
  i16 LOCAL_10;
  i16 LOCAL_8;
  bool startingInStairwell; //LOCAL_6
  i32 newY;
  i32 newX;
  CHARDESC *pCharacter;
  MOVEBUTN *result = NULL;
  PARTYMOVEDATA partyMoveData;
  i32 direction;
  i32 newLevel;
  i32 absdir=-1;
//  i32 staminaAdjustment[4];
  //RESTARTMAP
  //  RESTART(1)
  //END_RESTARTMAP
  direction = button;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (TimerTraceActive)
  {
    const char *pdir;
    //i64 stime = UI_GetSystemTime();
#ifdef _LINUX
    //fprintf(GETFILE(TraceFile),"%lldd MoveParty ", stime);
    fprintf(GETFILE(TraceFile), "%d MoveParty ", d.Time);
#else
    //fprintf(GETFILE(TraceFile),"%I64d MoveParty ", stime);
    fprintf(GETFILE(TraceFile), "%d MoveParty ", d.Time);
#endif
    switch(direction)
    {
    case 3: pdir="forward"; break;
    case 4: pdir="right";   break;
    case 5: pdir="backward";break;
    case 6: pdir="left";    break;
    default:pdir="????";
    };
    fprintf(GETFILE(TraceFile),"%s from ", pdir);
    fprintf(GETFILE(TraceFile),"%02x(%02x,%02x)\n",
            d.partyLevel,d.partyX,d.partyY);
  };
  if (direction == 0x1124) direction = 7;
  if (direction == 0x1125) direction = 8;
  direction -= 3;
  cellFlagsFrom = GetCellFlags(d.partyX, d.partyY);//TAG009672
  rtFrom = RoomType(cellFlagsFrom);
  startingInStairwell = rtFrom==roomSTAIRS;//Cell's type
  newX=d.partyX;
  newY=d.partyY;


  
  if (startingInStairwell && (direction==2)) // backward
  {
    rtTo = roomSTAIRS;
    cellFlagsTo = cellFlagsFrom;
  }
  else
  {
    if (direction <= 3)
    {
      RelativeXY( //TAG009622
            d.partyFacing,
            d.Word18560[direction], // #steps forward (-1, 0, or 1)
            d.Word18568[direction], // #steps right (-1, 0, or 1)
            &newX,   // original x to be updated
            &newY);  // original y to be updated
      cellFlagsTo = GetCellFlags(newX, newY);
      rtTo = RoomType(cellFlagsTo);
    }
    else
    {
      rtTo = roomSTONE;
      cellFlagsTo = 0;
    };
  };
  
  
  if (!gravityMove)
  {
    i32 axis=0;
    i32 delta=0;
    switch (direction)
    {
    case 0:
    case 1:
    case 2:
    case 3: absdir = (direction + d.partyFacing) & 3;
            axis = (absdir + 1) & 1;
            delta = ((absdir+1) & 2)>>1;
            break;
    //case 4:
    //case 5: 
    default:absdir = direction;
            axis = 2;
            delta = (absdir & 1) ^ 1;
            break;
    };
    //       n,e,s,w,u,d
    //absdir 0,1,2,3,4,5
    //axis   1,0,1,0,2,2
    //delta  0,1,1,0,0,1
    moveHistory[3] |= 1<<axis;
    if (delta==0) moveHistory[3] ^= 1<<axis;
    if (moveHistory[2] == axis)
    {
      moveHistory[2] = moveHistory[1];
      moveHistory[1] = (char)axis;
    };
    if (moveHistory[1] == axis)
    {
      moveHistory[1] = moveHistory[0];
      moveHistory[0] = (char)axis;
    };
  };

  if (GravityGame)
  {
    gravityMove = true;
    nextGravityMoveTime = d.Time + 2;
  };
  if (extraTicks)
  {
    d.clockTick = 1;
  };
  for (charIdx=0, pCharacter = &d.CH16482[0];
       charIdx < 4;
       charIdx++, pCharacter++)
  {
    if (charIdx < d.NumCharacter)
    {
      maxLoad = MaxLoad(pCharacter); //A multiple of 10. why??
      D0W = sw((3*pCharacter->load)/maxLoad + 1);
      partyMoveData.staminaAdjustment[charIdx] = D0W;
    }
    else
    {
      partyMoveData.staminaAdjustment[charIdx] = (unsigned)-1;
    };

  };
  {
    LOCATIONREL locr;
    locr.l = d.partyLevel;
    locr.x = d.partyX;
    locr.y = d.partyY;
    locr.p = d.partyFacing;
    partyMoveData.absDirection = absdir;
    partyMoveData.relDirection = button;
    partyMoveData.flags = 0;
    partyMoveData.delay = 0;
    partyMoveData.fromLocation = locr.Integer();
    partyMoveData.fromLocationType = rtFrom;
    partyMoveData.moveType = PM_ATTEMPTMOVE;
    partyMoveData.toLocation = locr.Integer();
    partyMoveData.toLocationType = rtTo;
    CallPartyMoveFilter(&partyMoveData);
    if (partyMoveData.flags & PM_INHIBITMOVE) return result;
  };

  if (     ((partyMoveData.flags & PM_SETDELAY) != 0)
        && ((partyMoveData.flags & PM_ADDDELAY) == 0))
  {
    d.partyMoveDisableTimer = (i16)partyMoveData.delay;
    d.Word11712 = 0;
  };
  for (charIdx=0, pCharacter = &d.CH16482[0];
       charIdx < d.NumCharacter;
       charIdx++, pCharacter++)
  {
    AdjustStamina(charIdx, partyMoveData.staminaAdjustment[charIdx]);

  };

  if ((direction>=0) && (direction<=3))
  {
    result = &d.MoveButn18496[direction];
    // The button will be flashed by the caller.  Makes this function
    // non-RESTARTABLE!!!  :-)
    //FlashButton(_1_,
    //            pmpA2->x1,   // TAG1979c
    //            pmpA2->x2,
    //            pmpA2->y1,
    //            pmpA2->y2);
  };
  if (startingInStairwell && (direction==2)) // backward
  {
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"Before TraverseStairway party is at ");
      fprintf(GETFILE(TraceFile),"%02x(%02x,%02x)\n",
             d.partyLevel,d.partyX,d.partyY);
    };
    TraverseStairway(cellFlagsFrom & 4, true);//non-zero = decrement level = UP
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"After TraverseStairway party is at ");
      fprintf(GETFILE(TraceFile),"%02x(%02x,%02x)\n",
               d.partyLevel,d.partyX,d.partyY);
    };
  }
  else
  {
//
//
//
    if (direction <= 3)
    {
    }
    else
    {
      UpdateXY(direction,&newLevel,&newX,&newY);
      if (  (newLevel > d.dungeonDatIndex->NumLevel()-1)
          ||(newLevel < 0)
         )
      {
        rtTo = roomSTONE;
        cellFlagsTo = 0;  // Not needed???
      }
      else
      {
        if (direction == 5)
        {
          cellFlagsTo = *GetCellFlagsAddress(newLevel,newX,newY);
        }
        else
        {
          cellFlagsTo = *GetCellFlagsAddress(d.partyLevel,d.partyX,d.partyY);
        };
        rtTo = RoomType(cellFlagsTo);
        switch (rtTo)
        {
        case roomPIT: break;
        default: rtTo = roomSTONE; break;
        };
      };
      if (    (rtTo != roomSTONE)
           && FindFirstMonster(newLevel, newX, newY) != RNeof
         )
      {
        return result;
      };
    };
    if (   (rtTo == roomSTAIRS)
        || ((rtTo != roomSTONE) && (direction>3)))
    {
      if (direction <= 3)
      {
        //First, move party into stairwell
        MoveObject(RN(RNnul),d.partyX,d.partyY,-1,0, NULL, NULL);
        d.partyX = sw(newX);
        d.partyY = sw(newY);
        //Then move party to new level.
        TraverseStairway(cellFlagsTo & 0x04, true);//non-zero = decrement level = UP
      }
      else
      {
        TraverseStairway(direction-4, false);
      };
    }
    else
    {
      D5W = 0;
      if (rtTo == roomSTONE) // Stone wall
      {
        D5W = 1; // Inhibit movement.
      }
      else
      {
        if (rtTo == roomDOOR)
        {
          i32 doorCondition = cellFlagsTo & 7;
          D5W = (   (doorCondition!=0)
                  && (doorCondition!=1)
                  && (doorCondition!=5));
          //D5W = D7W & 7;//Door type
          //bD0 = D5W!=0; //???
          //if (bD0)
          //{
            //bD0 = D5W!=1;
          //};
          //if (bD0)
          //{
            //bD0 = D5W!=5;
          //};
          //D5W = bD0;
        }
        else
        {
          if (rtTo == roomFALSEWALL)
          {
            bD5 = (cellFlagsTo&4)==0;
            if (bD5)
            {
              bD5 = (cellFlagsTo&1)==0;
            };
            D5W = bD5; // passable if bit 2 set or bit 0 set
          };
        };
      };
      if (d.NumCharacter != 0)
      {
        if (D5W != 0) // Inhibit movement
        {

          i16 k;
          // Find which side of new cell we are crossing
          direction = direction + (d.partyFacing + 2);
//
//
          LOCAL_8 = GetCharacterToDamage(
                       newX,
                       newY,
                       direction & 3);
          k = sw(DamageCharacter(LOCAL_8, 1, 24, 2));//TAG017068
          LOCAL_10 = GetCharacterToDamage(
                newX,
                newY,
                (direction+1) & 3);
          LOCAL_30 = LOCAL_10;
          if (LOCAL_8 != LOCAL_30)
          {
            k |= DamageCharacter(LOCAL_10,1,24,2);
          };
          if (k != 0)
          {
            QueueSound(18, d.partyX, d.partyY, 0);
          };
        }
        else
        {
          if (direction <= 3)
          {
            objD0 = FindFirstMonster(newX, newY);
            bD0 = objD0 != RNeof;
            D5W = bD0;
          };
          if (D5W != 0)
          {
            ProcessTimers29to41(newX, newY, TT_M1, 0);
          };
        };
      }
      else
      {
        if (direction > 3) return result;
      };
      if (D5W != 0)
      {
        DiscardAllInput();
        d.clockTick = 0;
      }
      else
      {
        i16 time2move;
        if (startingInStairwell)
        {
          MoveObject(RN(RNnul), -1, 0, newX, newY, NULL, NULL);
        }
        else
        {
          MoveObject(RN(RNnul),d.partyX,d.partyY,newX,newY,NULL, NULL);
        };
        time2move = 1;
        for (i=0, pCharacter=d.CH16482;
             i < d.NumCharacter;
             i++, pCharacter++)
             {  // Find slowest character
          if (pCharacter->HP() != 0)
          {
            D0W = TimeToMove(pCharacter); // Speed of character
            time2move = sw(Larger(time2move, D0W));
          };
//
        };
        if ((partyMoveData.flags & PM_ADDDELAY) != 0)
        {
          d.partyMoveDisableTimer = (i16)(d.partyMoveDisableTimer + partyMoveData.delay);
        }
        else
        {
          if ((partyMoveData.flags & PM_SETDELAY) == 0) // We may have already set the delay
          {
            d.partyMoveDisableTimer = time2move;
          };
        };
        d.Word11712 = 0;
      };
    };
  };
  return result;
}
// *********************************************************
//
// *********************************************************
void TAG019c34(i32 button,i32 clickX, i32 clickY)
{//()
  dReg D5, D6, D7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(clickX);
  D6W = sw(clickY);
  if (button+1 == d.SelectedCharacterOrdinal)
  {
    SetHandOwner(button);
  }
  else
  {
    D5W = SearchButtonList(d.Buttons18216,D7W, D6W,2);
//
    if ( (D5W>=16) && (D5W<=19) )
    {
      SetHandOwner(D5W-16);
    }
    else
    {
      if ( (D5W>=20) && (D5W<=27) )
      {
        HandleClothingClick(D5W-20);
      };
    };
  };
}

// *********************************************************
//
// *********************************************************
//   TAG019cb2
void SetHandOwner(i32 chIdx)
{//()
  dReg D1, D6;
  CHARDESC *pcA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7W = P1;
  if (chIdx == d.HandChar) return; //owner of cursor
  if ( (chIdx != -1) && (d.CH16482[chIdx].HP()==0) ) return;
  if (d.HandChar != -1) //owner of cursor
  {
    D6W = d.HandChar; //owner of cursor
    d.CH16482[D6W].charFlags |= CHARFLAG_possession | CHARFLAG_cursor;
    ASSERT(RememberToPutObjectInHand == -1,"objInHand");
    D1L = GetObjectWeight(d.objectInHand);
    DEBUGLOAD(d.CH16482+D6W, D1W, -1, d.objectInHand);
    d.CH16482[D6W].load = sw(d.CH16482[D6W].load - D1W);
    d.HandChar = -1; //owner of cursor
    DrawCharacterState(D6W);//box at top,pos,dir,weapon
  };
  if (chIdx == -1)
  {
    d.HandChar = -1;//owner of cursor
    return;
  };
  d.HandChar = sw(chIdx);//owner of cursor
  pcA3 = &d.CH16482[chIdx];
  pcA3->facing = (ui8)d.partyFacing;
  ASSERT(RememberToPutObjectInHand == -1,"objInHand");
  D1L = GetObjectWeight(d.objectInHand);
  DEBUGLOAD(d.CH16482+chIdx, D1W, +1, d.objectInHand);
  d.CH16482[chIdx].load = sw(d.CH16482[chIdx].load + D1W);
  if (chIdx+1 == d.PotentialCharacterOrdinal) return;
  pcA3->charFlags |= CHARFLAG_positionChanged | CHARFLAG_possession | CHARFLAG_cursor;
  DrawCharacterState(chIdx);//box at top,pos,dir,weapon
}

// *********************************************************
//
// *********************************************************
void TAG0196da(i32 key)
{
  dReg D0, D5, D6;
  KeyXlate *pxA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pxA3 = d.pKeyXlate2;
  if (pxA3 != NULL)
  {
    d.MouseInterlock = 1;
    if ((key&0xffff) == 0) key >>= 8; // Move scan code to lower word
    {
    };
    D0W = sw(d.MouseQEnd+2);
    D6W = D0W;
    if (D0W > MOUSEQLEN-1) D6W -= MOUSEQLEN;
    if (D6W != d.MouseQStart)
    {
      while ((D5W = pxA3->mouse) != 0)
      {
        if (pxA3->keyscan == (key&0xffff))
        {
          D0W = D6W;
          D6W--;
          if (D0W == 0) D6W = MOUSEQLEN-1;
          d.MouseQEnd = D6W;
          pMouseQueue[D6W].num = D5W;
          break;
        };
        pxA3++;

      };
      pxA3 = d.pKeyXlate1;
      if (pxA3 != NULL)
      {
        while ((D5W = pxA3->mouse) != 0)
        {
          if ((key&0xffff) == pxA3->keyscan)
          {
            D0W = D6W;
            D6W--;
            if (D0W == 0) D6W = MOUSEQLEN-1;
            d.MouseQEnd = D6W;
            pMouseQueue[D6W].num = D5W;
          };
          pxA3++;
        };
      };
      if (ThreeDMovements)
      {
        if (   (key == 0x4a00)
            || (key == 0x4900)
           )
        {
          if (key == 0x4a00) D5W = 0x1125; //Up
          if (key == 0x4900) D5W = 0x1124; //Down
          D0W = D6W;
          D6W--;
          if (D0W == 0) D6W = MOUSEQLEN-1;
          d.MouseQEnd = D6W;
          pMouseQueue[D6W].num = D5W;
        };
      };
    };
    d.MouseInterlock = 0;
    CauseFakeMouseClick();
  };
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG01a6ea(const i32 mouseX, const i32 mouseY)
{//()
  static dReg D0, D4, D5, D6, D7;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(mouseX);
  D6W = sw(mouseY);
  D4W = sw((d.HandChar == -1) ? 1: 0); //owner of cursor
  D0W = d.DisplayResurrectChestOrScroll;
  if (D0W == 4) //If displaying chest in weapon hand
  {
    if (D4W != 0) RETURN;
    D5W = SearchButtonList(d.Buttons18324,D7W, D6W,2);
    if (D5W)
    {
      //D0W = D5W - 20;
      HandleClothingClick(D5W-20);
    };
  }
  else
  {
    if (D0W == 5)
    {
      if (d.EmptyHanded)
      {
        D5W = SearchButtonList(d.Buttons18372, D7W, D6W, 2);
        if (D5W)
        {
          ResurrectReincarnateCancel(_1_, D5W);
        };
      };
    };
  };
  RETURN;
}

// *********************************************************
//
// *********************************************************
//   TAG01a77c
void DisplaySleepScreen()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  Flood(d.pViewportBMP, 0, 1904);
  TextOut_OneLine(d.pViewportBMP,
                  112,
                  93,
                  65,
                  4,
                  0,
                  "WAKE UP",
                  999,
                  false);
}

// *********************************************************
//
// *********************************************************
//   TAG0197d2
void WaitForButtonFlash()
{//()
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  Instrumentation(icntTAG0197d2);
  while (d.FlashButnActive != 0)
  {
    NotImplemented(0x197dc); //pumper();
  };



  /* I am going to attempt to write this differently
     so that WaitForButtonFlash does not have to
     be restartable.  God help me.
  if (d.Word11776 != 0)
  {
    d.FlashButnActive = 1;
    wvbl(_1_);
  };
*/
      //Here is the attempted rewrite.
      //We will call CheckVBL and sleep until
      //the vbl count is increased.
      if (d.Word11776 != 0)
      {
        ui32 oldCount;
        d.FlashButnActive = 1;
        oldCount = VBLCount+1;
        //checkVBL(); //Give it a chance to happen right now!
        //Make sure we wait at least 20 milliseconds.
        while (oldCount > VBLCount)
        {
          if (VBLMultiplier == 1) UI_Sleep(20);
          checkVBL();
        };
      };
      //OK...Let us see if that works.




  return;
}

// *********************************************************
//
// *********************************************************
//   TAG01ad6c
const char* GetAttackTypeName(i32 P1)
{
  dReg D7;
  aReg A3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = (I16)(P1 & 0xff);
  if (D7W == 255)
  {
    return "";
  }
  else
  {
    for (A3 = d.AttackNames; D7W-- != 0; )
    {
      while ( *(A3++) != 0) {}; // Skip one string
    };
    return (const char*)A3;
  };
}


LOCATIONREL actionNameFilterLocr;
RN actionNameDSA = RNnul;


const char *ActionNameFilter(const char *defaultName, i32 charIndex, i32 actionNumber, i32 numAttackOption)
{
// ****************************************************************************
//  See if there is aa Attack Option Name Filter to modify the description.
  if (actionNameDSA == RNnul)
  {
    ui32 key;
    i32 len;
    ui32 *pRecord;
    actionNameDSA = RNeof;
    key = (EDT_SpecialLocations<<24)|ESL_ACTIONNAMEFILTER;
    len = expool.Locate(key,&pRecord);
    if (len > 0)
    //Let the DSA process the parameters
    {
      RN obj;
      TIMER timer;
      i32 objectDB, actuatorType;
      DB3 * pActuator;
      NEWDSAPARAMETERS dsaParameters;
      actionNameFilterLocr.Integer(*pRecord);
      for (obj = FindFirstObject(
                      actionNameFilterLocr.l,
                      actionNameFilterLocr.x,
                      actionNameFilterLocr.y);
          obj != RNeof;
          obj = GetDBRecordLink(obj))
      {
        objectDB = obj.dbType();
        if (objectDB == dbACTUATOR)
        {
          pActuator = GetRecordAddressDB3(obj);
          actuatorType = pActuator->actuatorType();
          if (actuatorType == 47)
          {
            actionNameDSA = obj;
            break;
          };
        };
      };
    };
  };
  if (actionNameDSA!=RNeof)
  {
    TIMER timer;
    int currentLevel;
    DSAVARS dsaVars;
    currentLevel = d.LoadedLevel;
    LoadLevel(actionNameFilterLocr.l);
    timer.timerUByte9(0);//timerFunction
    timer.timerUByte8(0);//timerPosition
    timer.timerUByte7((ui8)actionNameFilterLocr.y);
    timer.timerUByte6((ui8)actionNameFilterLocr.x);
    //timer.timerTime   = actionNameFilterLocr.l << 24;
    timer.Time(0);
    timer.Level((ui8)actionNameFilterLocr.l);
  
    pDSAparameters[0] = 5;
    pDSAparameters[1] = charIndex;
    pDSAparameters[2] = actionNumber;
    pDSAparameters[3] = (ui32)-1; //The result
    pDSAparameters[4] = numAttackOption;
    pDSAparameters[5] = d.PossibleAttackTypes[actionNumber];
    ProcessDSAFilter(actionNameDSA, &timer, actionNameFilterLocr, NULL, &dsaVars);
    LoadLevel(currentLevel);
    d.PossibleAttackTypes[actionNumber] =  (i8)pDSAparameters[5];
    if (pDSAparameters[3] < 0x10000)
    {
      ui32 key, *pRecord;
      i32 size;
      key = (EDT_GlobalText<<24) | pDSAparameters[3];
      size = expool.Locate(key, &pRecord);
      if (size > 0)
      {
        defaultName = (char *)pRecord;
      };
    };
  };
  return defaultName;
}

// *********************************************************
//
// *********************************************************
//   TAG01b098
void DrawLegalAttackTypes()
{
	// This function just draws clicked item`s options in the place of
	// four hands.
  dReg D7;
  RectPos *prA3;
  const char *pName;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  STHideCursor(HC31);
  d.UseByteCoordinates = 0;
  FillRectangle(d.LogicalScreenBase,
                (RectPos *)d.Word20,
                0,
                160);
  if (d.Word20252)
  {
    for (D7W=0; D7W<d.NumCharacter; D7W++)
    {
      DrawAttackButton(D7W);

    };
  }
  else
  {
    if (d.AttackingCharacterOrdinal)
    {
      i32 numAttackOption = 3;
      prA3 = &d.wRectPos20202;
      if (d.PossibleAttackTypes[2] == -1)
      {
        prA3 = &d.wRectPos20210;
        numAttackOption = 2;
      };
      if (d.PossibleAttackTypes[1] == -1)
      {
        prA3 = &d.wRectPos20218;
        numAttackOption = 1;
      };
      BLT2Screen(GetBasicGraphicAddress(10),
                prA3,
                48,
                -1);
      DrawText(
               d.LogicalScreenBase,
               160,
               235,
               83,
               0,
               4,
               d.CH16482[d.AttackingCharacterOrdinal-1].name,
               7);


      for (D7W=0; D7W<3; D7W++)
      {
        pName = GetAttackTypeName(d.PossibleAttackTypes[D7W]);
        if (D7W < numAttackOption)
        {
          pName = ActionNameFilter(pName, d.AttackingCharacterOrdinal-1, D7W, numAttackOption);
        };
        DrawText(
                 d.LogicalScreenBase,
                 160,
                 241,
                 12 * D7W + 93,
                 4,
                 0,
                 pName,
                 12);

      };
    };
  };
  STShowCursor(HC31);
  d.Word20250 = 0;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG01b29a()
{//()
  static dReg D0, D1, D7;
  static CHARDESC *pcA3=NULL;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.NumCharacter == 0) RETURN;
  if ( (d.PartySleeping) || (d.PotentialCharacterOrdinal) )
  {
    if (d.AttackingCharacterOrdinal)
    {
      TAG01b1c0();
      RETURN;
    };
    if (d.PotentialCharacterOrdinal == 0) RETURN;
  }
  else
  {
    pcA3 = d.CH16482;
    for (D7W=0; D7W<d.NumCharacter; D7W++, pcA3++)
    {
      if (D7W != d.HandChar) //owner of cursro
      {
        if (D7W+1 != d.AttackingCharacterOrdinal)
        {
          if (pcA3->maxRecentDamage)
          {
            D0W = pcA3->facing;
            D1W = pcA3->facing3;
            if (D0W != D1W)
            {
              pcA3->facing = pcA3->facing3;
              pcA3->charFlags |= CHARFLAG_positionChanged;
              DrawCharacterState(D7W);//box at top,pos,dir.weapon
            };
          };
        };
      };
      pcA3->maxRecentDamage = 0;
    };
  };
  if (d.Word20250 == 0) RETURN;
  if (d.AttackingCharacterOrdinal == 0)
  {
    if (d.attackDamageToDisplay)
    {
      FlashAttackDamage(_1_,d.attackDamageToDisplay);
      d.attackDamageToDisplay = 0;
    }
    else
    {
      d.Word20252 = 1;
      DrawLegalAttackTypes();
    };
    RETURN;
  };
  d.Word20252 = 0;
  pcA3->charFlags |= CHARFLAG_weaponAttack;
  DrawCharacterState(d.AttackingCharacterOrdinal-1);//box at top, pos, dir,weapon
  DrawLegalAttackTypes();
  RETURN;
}

// *********************************************************
//
// *********************************************************
void TAG01b408(i16 P1)
{
  dReg D0, D1, D5, D6, D7;
  CHARDESC *pcA3;
  i16  LOCAL_12;
  char LOCAL_2[2];
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pcA3 = &d.CH16482[d.MagicCaster];
  if (P1 ==2)
  {
    MemMove((ui8 *)d.Byte21994, (ui8 *)d.Byte20842, 576);
    D6W = 1;
    D5W = 6;
    D1W = (UI8)(pcA3->byte33);
    LOCAL_12 = sw(D1W * D5W);
    D5W = 96;
    D5W = sw(D5W + LOCAL_12);
    LOCAL_2[1] = 0;
    for (D7W=0; D7W<6; D7W++)
    {

      LOCAL_2[0] = D5B;
      D5B++;
      D6W += 14;
      TextOut_OneLine((ui8 *)d.Byte20842,
                      48,
                      D6W,
                      8,
                      4,
                      0,
                      LOCAL_2,
                      999,
                      false);

//
    };
  }
  else
  {
    if (P1 == 3)
    {
      MemMove((ui8 *)d.Byte21418, (ui8 *)d.Byte20842, 576);
      LOCAL_2[1] = 0;
      D6W = 8;

      for (D7W=0; D7W<4; D7W++)
      {
        D0B = pcA3->incantation[D7W];
        LOCAL_2[0] = D0B;
        if (D0B == 0) break;
        D6W += 9;
        TextOut_OneLine((ui8 *)d.Byte20842,
                        48,
                        D6W,
                        8,
                        4,
                        0,
                        LOCAL_2,
                        999,
                        false);

//
      };
    };
  };
}

//   TAG01b8ae
void ReadSpellGraphic()
{
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ReadAndExpandGraphic(
          11,          // graphic number
          (ui8 *)d.Byte22570, // destination
          0,
          0);
}

//   TAG01b8ca
void DrawSpellMenuCharacters(i16 P1)
{
  dReg D5, D6, D7;
  char b_2[2];
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  b_2[1] = 0;
  D5W = sw(6 * P1 + 96);
  D6W = 225;
  for (D7W = 0; D7W < 6; D7W++)
  {
    b_2[0] = (i8)D5W++;
    TextOutToScreen(D6W += 14, 58, 4, 0, b_2);

//
  };
}

void TAG01b990(i16 button)
{//()
  dReg D0, D1, D5, D6, D7;
  aReg A0;
  CHARDESC *pcA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pcA3 = &d.CH16482[d.MagicCaster];
  D7W = (UI8)(pcA3->byte33);
  A0 = d.Byte19010 + 6*D7W;
  D6W = (UI8)(A0[button]);
  if (D7W != 0)
  {
    D5W = sw(pcA3->incantation[0] - 96);
    //A0 = d.Byte19016 + D5W;
    D1W = (UI8)(d.Byte19016[D5W]);
    D6W = sw((D6W * D1W) >> 3);
  };
  if (D6W > pcA3->Mana()) return;
  pcA3->IncrMana(-D6W);
  pcA3->charFlags |= CHARFLAG_statsChanged;
  D0W = sw(6 * D7W + 96 + button);
  //A0 = pcA3->byte34 + D7W;
  pcA3->incantation[D7W] = D0B;
  pcA3->incantation[D7W + 1] = 0;
  D7W = (I16)((D7W + 1) & 3);
  pcA3->byte33 = (i8)D7W;
  STHideCursor(HC32);
  DrawSpellMenuCharacters(D7W);//Zo, Rath, etc
  DisplayMagicSelection(pcA3);
  DrawCharacterState(d.MagicCaster);
  STShowCursor(HC32);
}




//   Tag01cefa
void SubstDiskLetter(char *dest,const char *src,i16 flag)
{
  aReg A0;
  i16 LOCAL_6;
  const char *LOCAL_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A0 = (aReg)dest;
  *A0 = 0;
  LOCAL_4 = StrChr(src, '~');
  if (LOCAL_4 != NULL)
  {
    LOCAL_6 = sw(LOCAL_4 - src); // index of tilde in P2
    StrCpy(dest,src,LOCAL_6); // Copy up to tilde
    A0 = (aReg)dest + LOCAL_6 ;
    *A0 = 0; // mark the end
    if ((d.SingleFloppyDrive) || (flag == 1))
    {
      StrCat(dest, (char *)d.Pointer22984);
    }
    else
    {
      StrCat(dest, (char *)d.Pointer22988);
    };
    StrCat(dest, LOCAL_4+1);
  };
}

i16 StreamInput(ui8 *dest,i32 num) //TAG01cf86
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  i32 count, result;
  count = num;
  if (count == 0) return 1;
  if (d.inStreamBuffered != 0)
  {
    if (count > d.inStreamLeft) return 0;
    MemMove((ui8 *)d.inStreamNext, dest, count);
    d.inStreamNext += count;
    d.inStreamLeft -= count;
    return 1;
  };
  result = READ(d.datafileHandle, count, dest);
  return (count == result) ? (ui16)1 : (ui16)0;
}

// TAG01d048

i16 Unscramble(ui8 *buf, i32 initialHash, i32 numword)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D5, D6, D7;
  aReg A3;
  A3 = (aReg)buf;
  D7W = (UI16)initialHash;
  D6W = sw(numword);
  D5W = (UI16)initialHash;
  do
  {
    D5W = (UI16)(D5W + LE16(wordGear(A3)));
    //D0W = D7W;
    wordGear(A3) ^= LE16(D7W);
    D5W = (UI16)(D5W + LE16(wordGear(A3)));
    A3 += 2;
    D7W = (UI16)(D7W + D6W);
    D6W--;
  } while (D6W != 0);
  return D5W;
}

// TAG01d0a4
i16 UnscrambleStream(ui8 *dest,i32 size, i16 initialHash, i16 Checksum)
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D0W = StreamInput(dest,size);
  if (D0W == 0) return 0;
  D0W = Unscramble(dest, initialHash, size/2);
  if (D0W == Checksum) return 1;
  return 0;
}

i16 FetchDataBytes(ui8 *P1,i16 *P2,i32 P3) //TAG01d138
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D0, D6;
  i32 count;
  i16 *uwA2;
  aReg A3;
  A3 = (aReg)P1;
  uwA2 = P2;
  count = P3;
  D6W = 0;
  D0W = StreamInput((ui8 *)A3, count);
  if (D0W == 0)
  {
    D0W = 0;
  }
  else
  {
    while (count != 0)
    {
      count--;
      D6W = (UI16)(D6W + *((ui8 *)A3));
      A3++;
    };
    *uwA2 = (UI16)(*uwA2 + D6W);
    D0W = 1;
  };
  return D0W;
}

// *********************************************************
//
// *********************************************************
i16 InsertDisk(const i32 P1,const i32 P2)
{//(i16)
  static dReg D0, D5, D6, D7;
  static aReg A3;
  static i8  LOCAL_80[80];
//  RESTARTMAP
//    RESTART(1)
//    RESTART(2)
//  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //SaveRegs(0x0710);
  D7W = sw(P1);
  D6W = sw(P2);
  D5W = 1;
  D0W = 0;
  if (D7W !=2)
  {
    D0W = D5W;
  }
  else
  {
    //A3 = NULL;
    //SubstDiskLetter(LOCAL_80, d.Pointer22864, 1);
    //do
    //{
    //  while (D7W != 0)
    //  {
    //    D7W = D5W = 0;
    //    DoMenu(_2_,
    //           A3, // TAG01d678
    //           LOCAL_80,
    //           d.Pointer22908,
    //           NULL,
    //           NULL,
    //           NULL,
    //           D6W,
    //           D6W,
    //           D6W );
    //    WaitForMenuSelect(_1_, 1, D6W, 1, 1);
    //    A3 = d.Pointer22820;
    //  };
    //  D0W = 0;
    //} while (D0W != 0);
    D0W = D5W;
  };
  //RestoreRegs(0x08e0);
  return D0W;
}

// *********************************************************
//
// *********************************************************
//  TAG01db46
i16 ReadUnscrambleBlock(ui8 *P1)
{
  dReg D0, D5, D6, D7;
  aReg A3;
  bool bD0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A3 = (aReg)P1;
  D0W = StreamInput((ui8 *)A3, 512); //TAG01cf86
  if (D0W == 0) return 0;
  for (D7W=32, D6W=0; D7W!=0; D7W--)
  {
    D6W = (UI16)(D6W + LE16(wordGear(A3)));
    A3 += 2;
    D6W ^= LE16(wordGear(A3));
    A3 += 2;
    D6W = (UI16)(D6W - LE16(wordGear(A3)));
    A3 += 2;
    D6W ^= LE16(wordGear(A3));
    A3 += 2;
  };
  Unscramble((ui8 *)A3, LE16(wordGear(P1+58)), 128);
  for (D7W=128, D5W = 0; D7W!=0; D7W--)
  {
    D5W = (UI16)(D5W + LE16(wordGear(A3)));
    A3 += 2;
  };
  bD0 = D6W == D5W;
  D0W = bD0;
  return D0W;
}




// *********************************************************
//
// *********************************************************
//   TAG01f140
RESTARTABLE _DisplayChaosStrikesBack()
{//()
  static dReg D0, D5, D6, D7;
  static aReg A2, A3;
  static PALETTE  LOCAL_340;
  static str10 LOCAL_308[18]; // 10-byte structures
  static ui8 *LOCAL_96;
  static ui8 *LOCAL_92;
  static ui8 *LOCAL_88[18];
  static ui8 *screenBuffers[2];
  static ui8 *pGraphic;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
    RESTART(4)
    RESTART(5)
    RESTART(6)
    RESTART(7)
    RESTART(8)
  END_RESTARTMAP
  d.UseByteCoordinates=0;
  if (d.iAvailableMemory < 138060) RETURN;
  pGraphic = (ui8 *)allocateMemory(133056,0); // Temporary allocation
  pGraphic = (ui8 *)((ui32)(pGraphic + 256) & 0xffffff00); // Round to 256-byte boundary
  // We have 132800 bytes remaining after rounding.
  A3 = (aReg)pGraphic;
#ifdef GraphicsDebug
  GrphDbg = GETFILE(CREATE("GraphicsDebug.txt","w", true));
#endif
  ReadAndExpandGraphic(1, (ui8 *)A3, 0, 0);
#ifdef GraphicsDebug
  fclose(GrphDbg);
  GrphDbg = NULL;
#endif
  pGraphic += 32000;
  // Now we have 100800 bytes left.
  A2 = (aReg)d.LogicalScreenBase;
  setscreen(NULL, (ui8 *)A2, 0);
  screenBuffers[1] = (ui8 *)A2;
  screenBuffers[0] = (ui8 *)A3;
  LOCAL_92 = (ui8 *)A3;
  fillMemory((i16 *)&LOCAL_340, 16, 2, 2); // 16 words  of 0x0002
  FadeToPalette(_1_,&LOCAL_340);  // darken screen
  ClearMemory((ui8 *)A2, 32000);
  TAG0088b2((ui8 *)A3,           // Move "Presents" to video memory
            (ui8 *)d.LogicalScreenBase,
            (RectPos *)d.Word52,
            0,      // src x
            137,    // src y offset
            160,    // src bytes/line
            160,    // dst bytes/line
            -1);
  LOCAL_340.color[15] = 0x777;
  videoMode = VM_PRESENTS;
  FadeToPalette(_3_,&LOCAL_340); // Display "Presents"
  LOCAL_96 = pGraphic;
  TAG0088b2((ui8 *)A3,       //src = graphic 1
	          (ui8 *)pGraphic,  //dst
			      (RectPos *)d.Word44, //dest offsets and sizes
			      0,        //src x offset
			      80,       //src y offset
			      160,      //src bytes/line
			      160,      //dst bytes/line
			      -1);           // "Strikes Back"
  pGraphic += 9120; // =57 * 160
  // We have 91680 bytes left
//  D6W = 12;
//  D5W = 48;
  {
    ui32 width, height;
    for (D7W=0, width=48, height=12;
         D7W<18; 
         D7W++, width+=16, height+=4)
    {
      //width      =  48   64   80   96  112  128   144   160   176   192   208   224   240   256   272   288   304    320
      //height     =  12   16   20   24   28   32    36    40    44    48    52    56    60    64    68    72    76     80
      //bytes/line =  24   32   40   48   56   64    72    80    88    96   104   112   120   128   136   144   152    160
      //totalBytes = 288  512  800 1152 1568 2048  2592  3200  3872  4608  5408  6272  7200  8192  9248 10368  11552 12800      
      //sum        = 288  800 1600 2752 4320 6368  8960 12160 16032 20540 26048 32320 39520 47712 56960 67328  78880 91680
      ui32 left, right, top, bottom;
      i32  bytesPerLine, totalBytes;

      LOCAL_88[D7W] = pGraphic;
      ShrinkBLT((ui8 *)A3, LOCAL_88[D7W], 320, 80, width, height, d.IdentityColorMap);
      //D0W = sw(320 - width);
      //D0L = D0W;
      //D0L /= 2;
      //LOCAL_308[D7W].w1 = D0W;
      left = (320-width)/2;
      LOCAL_308[D7W].w1 = (i16)left;

      //D0W = sw(D0W + width - 1);
      //LOCAL_308[D7W].w2 = D0W;
      right = left + width - 1;
      LOCAL_308[D7W].w2 = (i16)right;


      //D0W = sw((160 - height)/2);
      //LOCAL_308[D7W].w3 = D0W;
      top = (160 - height)/2;
      LOCAL_308[D7W].w3 = (i16)top;
 

      //D0W = sw(D0W + height - 1);
      //LOCAL_308[D7W].w4 = D0W;
      bottom = top + height - 1;
      LOCAL_308[D7W].w4 = (i16)bottom;

      //D0W = sw(((width+15)/16)*8);
      //LOCAL_308[D7W].w5 = D0W;
      bytesPerLine = ((width+15)/16)*8; // 8 bytes per 16 pixels
      LOCAL_308[D7W].w5 = (i16)bytesPerLine;

      //D0L = D0W*height;
      //D0L &= 0xffff;
      //D4L += D0L;
      totalBytes = height*bytesPerLine;
      pGraphic += totalBytes;
      //D6L +=4;
      //D5L +=16;
    };
  };
  LOCAL_340.color[15] = 2;
  FadeToPalette(_6_,&LOCAL_340); // Remove "Presents"
  ClearMemory(LOCAL_92, 32000);
  setscreen((ui8 *)-1, LOCAL_92, -1);
  ClearMemory((ui8 *)A2, 32000);
  setscreen((ui8 *)-1, (ui8 *)A2, -1);
  LOCAL_340.color[3] = 0x0541;
  LOCAL_340.color[4] = 0x0421;
  LOCAL_340.color[5] = 0x0651;
  LOCAL_340.color[6] = 0x0531;
  LOCAL_340.color[8] = 0x0772;
  LOCAL_340.color[15] =0x0700;
  LOCAL_340.color[10] = 0x0002;
  LOCAL_340.color[12] = 0x0002;
  FadeToPalette(_7_, &LOCAL_340); // Just delay.
  for (D7W=0; D7W<18; D7W++) // expanding "CHAOS"
  {
    VBLDelay(_2_,2);
    display();
    TAG0088b2((ui8 *)LOCAL_88[D7W],
              (ui8 *)screenBuffers[D7W&1],
              (RectPos *)&LOCAL_308[D7W],
              0,
              0,
              LOCAL_308[D7W].w5,
              160,
              -1);
    setscreen((ui8 *)-1, screenBuffers[D7W&1], -1); // Swap video buffers
  };
  VBLDelay(_4_,10);
  // Put "Strikes Back" on screen.
  TAG0088b2((ui8 *)LOCAL_96,
            (ui8 *)d.LogicalScreenBase,
            (RectPos *)d.Word36,
            0,
            0,
            160,
            160,
            0);
  LOCAL_340.color[10] = 0;  // Black print
  LOCAL_340.color[12] = 0x0700;  // Red background
  FadeToPalette(_8_,&LOCAL_340);
  VBLDelay(_5_,20);
  FreeTempMemory(133056); // Release temporary memory:
  //D4=saveD4;D5=saveD5;D6=saveD6;D7=saveD7;A2=saveA2;A3=saveA3;
  RETURN;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _OpenPrisonDoors() //TAG01f47a
{//()
  static dReg D7;
  static aReg A0, A1, A3;
  static ui8 *LOCAL_4, *LOCAL_8;
  static i32 i;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3);
  END_RESTARTMAP
  A3 = (pnt)&d.Pointer22952; // Prison door left
  LOCAL_4 = d.LogicalScreenBase + 4800;
  LOCAL_8 = (ui8 *)pntGear((pnt)A3 + 36);
  for (D7W = 1; D7W != 32; D7W++)
  {
    //UI_Sleep(60);
    if ((D7W % 3) ==1)
    {
      while (CheckSoundQueue()) wvbl(_2_);
      StartSound(d.Pointer22964, 145, 1); //Graphic #535 // Start sound
    };
    MemMove((ui8 *)pntGear((pnt)A3+32), (ui8 *)pntGear((pnt)A3+36), 20608); // Dungeon interior.
// I commented this out.. I could see no change.
    TAG0088b2((ui8 *)d.pViewportBMP,  // left part of interior.
              upntGear((pnt)A3+36),
              (RectPos *)d.Word60,
              0,
              0,
              112,
              128,
              -1);
    if (((RectPos *)d.Word68)->w.x2 >= 0)
    {
      TAG0088b2(upntGear(A3+(D7W&3)*4), // Left Door
                upntGear(A3+36),
                (RectPos *)d.Word68,
                (D7W&0xfc)*4,
                0,
                64,
                128,
                -1);
      ((RectPos *)d.Word68)->w.x2 -= 4;
    };
    TAG0088b2(upntGear(((D7W&3)+4)*4+A3), // Right side door
              upntGear(A3+36),
              (RectPos *)d.Word76,
              (D7W&3)*4,
              0,
              64,
              128,
              -1);
    ((RectPos *)d.Word76)->w.x1 += 4;
    for (i=0; i<6; i++) 
    {
      CheckSoundQueue();
      wvbl(_1_);
    };
    A0 = (aReg)LOCAL_4;
    A1 = (aReg)LOCAL_8;
    for (i32 k=0; k<161; k++)
    {
      memmove(A0, A1, 112); // Move image to physbase
      A1 += 112;
      *(A0+112) = *(A1+0);
      *(A0+114) = *(A1+2);
      *(A0+116) = *(A1+4);
      *(A0+118) = *(A1+6);
      A0 += 160;
      A1 += 16;
    };
  }; // for (D7W)
  while (CheckSoundQueue()) wvbl(_3_);
  //RestoreRegs(0x0880);
  RETURN;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG01f746()
{//()
  static aReg A0;
  static RectPos dstPos;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  static dReg D6,D7;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
    RESTART(4)
    RESTART(5)
    RESTART(6)
  END_RESTARTMAP
  //i32 D5;
  d.PrimaryButtonList = d.Buttons16932;
  d.SecondaryButtonList = NULL;
  d.pKeyXlate2 = NULL;
  d.pKeyXlate1 = NULL;
  for (D7W=0; D7W<8; D7W++)
  {
    A0 = (aReg)allocateMemory(10304, 0); // temporary memory
    (&d.Pointer22952)[D7W] = (ui8 *)A0;
//
  };
  d.Pointer22920 = allocateMemory(20608,0);
  d.Pointer22916 = allocateMemory(20608,0);
  ReadAndExpandGraphic(3,d.Pointer22936,0,0);
  ReadAndExpandGraphic(2,d.Pointer22952,0,0);
  D6L = AllocateExpandGraphic(4,(ui8 **)&d.Pointer22956);
  D6L += AllocateExpandGraphic(5, (ui8 **)&d.Pointer22960);
  D6L += AllocateExpandGraphic(535, (ui8 **)&d.Pointer22964);
        // Open prison door sound
  D6L =+ AllocateExpandGraphic(534, (ui8 **)&d.Pointer22968);
  //closeGraphicsFile();
  d.UseByteCoordinates = 0;
  dstPos.w.x1 = 0;
  dstPos.w.x2 = 100;
  dstPos.w.y1 = 0;
  dstPos.w.y2 = 160;
  for (D7W=1; D7W<4; D7W++)
  {
    TAG0088b2((ui8 *)d.Pointer22952,
              (ui8 *)(&d.Pointer22952)[D7W],
              &dstPos,
              D7W*4,0,64,64,-1);
    dstPos.w.x2 -= 4;

  };
  dstPos.w.x2 = 127;
  for (D7W=5; D7W<8; D7W++)
  {
    dstPos.w.x1 += 4;
  TAG0088b2((ui8 *)d.Pointer22936,
            (ui8 *)(&d.Pointer22952)[D7W],
            &dstPos,
            0,0,64,64,-1);

  };
  do
  {
    TAG01f5ea(_1_); //Remove "CHAOS" display, Show Prison door.
    STShowCursor(HC50); //TAG003026
    UI_BeginRecordOK(true);
    DiscardAllInput();
    d.gameState = GAMESTATE_AtPrisonDoor;
    do
    {
      wvbl(_2_);
      do
      {
        HandleMouseEvents(_4_,0); //TAG01a7b2();
      } while (intResult & 1);
      if (DiskMenuNeeded) DisplayDiskMenu(_6_);
    } while (d.gameState == GAMESTATE_AtPrisonDoor);
  } while (d.gameState == GAMESTATE_202);
  UI_BeginRecordOK(false);
  StartSound(d.Pointer22968, 112, 1); // Start sound
  VBLDelay(_3_,20); // 20 vertical blanks
  STHideCursor(HC33);//TAG002fd2
  if (d.gameState != GAMESTATE_ResumeSavedGame)
  {
    OpenPrisonDoors(_5_);
  };
  FreeTempMemory(D6L + 82432 + 41216);
  RETURN;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG01f5ea()
{//()
  static dReg D7;
  static aReg A0;
  static LEVELDESC LOCAL_170;
  static i8  LOCAL_154[5][5];
  static CELLFLAG *LOCAL_128[5];
  RESTARTMAP
    RESTART(1)
    RESTART(2)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //i32 saveD7=D7;
  d.partyLevel = 255;
  d.Word11684 = 1;
  d.width  = 5;
  d.height = 5;
  d.LevelCellFlags = LOCAL_128;
  d.pCurLevelDesc = &LOCAL_170;
  fillWithByte((ui8 *)LOCAL_154[0], 25, 0, 1);
  for (D7W=0; D7W<5; D7W++)
  {
    A0 = LOCAL_154[0] + 5*D7W;
    LOCAL_128[D7W] = (CELLFLAG *)A0;
    LOCAL_154[2][D7W] = 32;

  };
  LOCAL_154[1][2] = 32;
  FadeToPalette(_1_,&d.Palette11914);
  videoMode = VM_PRISONDOOR;
  //
  // Entrance to prison without door.
  ExpandGraphic((i8 *)d.Pointer22956, d.LogicalScreenBase, 0, 0);
  DrawViewport(2, 2, 0); //facing south, x=2; y=0.
  d.ViewportUpdated = 0;
  d.UseByteCoordinates = 0;
  //
  //
  TAG0088b2((ui8 *)d.LogicalScreenBase, // Save interior prison entrance
            (ui8 *)d.Pointer22920,
            (RectPos *)d.Word84,
            0,   // src x
            30,  // src y
            160, // src bytes/line
            128, // dst bytes/line
            -1);
  //
  //Put left half of door in opening
  BLT2Screen(d.Pointer22952, (RectPos *)d.Word92, 64, -1);
  //
  // Put right half of door in opening
  BLT2Screen(d.Pointer22936, (RectPos *)d.Word100, 64, -1);
  //
  // Display prison entrance.
  FadeToPalette(_2_,&d.Palette360);
  //D7=saveD7;
  RETURN;
}

// *********************************************************
//
// *********************************************************
i32 AllocateExpandGraphic(i16 graphicNum, ui8 **address) // TAG01f6fe
{ // Returns size of resulting graphic
  dReg D0;
  aReg A0;
  i32 graphicSize;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D0L = GetGraphicDecompressedSize(graphicNum);
  graphicSize = D0L;
  A0 = (aReg)allocateMemory(D0L, 0);
  *address = (ui8 *)A0;
  ReadAndExpandGraphic(graphicNum | 0x8000,(ui8 *)A0, 0, 0);
  return graphicSize;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG01f928()
{//()
  static dReg D0;
  static i16 LOCAL_2=0;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
  END_RESTARTMAP
  STHideCursor(HC34);//TAG002fd2
  FadeToPalette(_1_,&d.Palette11914); //
  ExpandGraphic((i8 *)d.Pointer22960,d.LogicalScreenBase,0,0);
  videoMode = VM_CREDITS;
  FadeToPalette(_2_,&d.Palette328);
  do
  {
    wvbl(_3_);
    D0W = --LOCAL_2;
    if (D0W == 0) break;
    if (d.MouseSwitches != 0) break;
  } while (UI_CONSTAT()==0); // keyboard
  d.gameState = GAMESTATE_202;
  RETURN;
}

struct ATARIMEM
{
  ui8 *mem(i32 size)
  {
    m_mem = std::make_unique<ui8[]>(size);
    return m_mem.get();
  };

  void Cleanup() { m_mem.reset(); }

private:
  // We put this in a class so that the destructor
  // will happily release the memory.  Keeps debug
  // aids quiet.
  std::unique_ptr<ui8[]> m_mem;
};


ATARIMEM AtariMem;

void AtariMemCleanup()
{
  AtariMem.Cleanup();
}


//   TAG020286
void InitializeHeap()
{
  dReg D0, D5,D6,D7;
  aReg A0, A1;
  // i32 D4;
  DisableCursor();
  firstMemoryBlock = NULL;
  D0L = GetAvailMemory();
  D7L = D0L-20;
  SetSupervisorMode();
  A1 = (aReg)0x5000; // System start address
  D6L = 0x19c00; // System end address;
  A0 = (aReg)0x19bf4; // ???
  D5L = 0x19c00; // What is 4(a0) // End of available memory
  D5L -= D6L; // say there is no space above OS
  ClearSupervisorMode();
  InitializeCaches(AtariMem.mem(D7L), D7L, NULL, D5L);
}

//   TAG020466
void CountFloppyDrives()
{
  dReg D7;
  //i32 saveD6=D6, saveD7=D7;
  SetSupervisorMode();
  D7W = sw(NumberFloppyDrives);
  ClearSupervisorMode();
  if (D7W == 2)
  {
    d.DualFloppyDrive = 1;
  }
  else
  {
    d.SingleFloppyDrive = 1;
  };
  d.Pointer22984 = "DRIVE.";
  d.Pointer22988 = "DRIVE.";
  //D6=saveD6; D7=saveD7;
}

i8 data2052e[] = {0x05, 0x05, 0x11, 0x1e, 0x2b};
i8 data20577[] = "CSBGAMEx.BAK";

#if !defined(_LINUX)
#include "winbase.h"
#endif

// *********************************************************
//
// *********************************************************
//   TAG0204bc
RESTARTABLE _SelectSaveGame(const i32 P1, const i32 checkExist, i32 alwaysDate)
{//()
  const ui64 unknownTime = ui64(-1); //0xffffffffffffffff;
  static dReg D0;
  static aReg A0, A1;
  int i;
  static char fNames[4][20];
  fNames[0][0] = ' ';
  fNames[0][1] = ' ';
  strcpy(fNames[0]+2,"CSBGAME.DAT");
  fNames[1][0] = ' ';
  fNames[1][1] = ' ';
  strcpy(fNames[1]+2,"CSBGAME2.DAT");
  fNames[2][0] = ' ';
  fNames[2][1] = ' ';
  strcpy(fNames[2]+2,"CSBGAME3.DAT");
  fNames[3][0] = ' ';
  fNames[3][1] = ' ';
  strcpy(fNames[3]+2,"CSBGAME4.DAT");
  RESTARTMAP
    RESTART(1)
    RESTART(2)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  ui64 modifiedTime[4];

  if (checkExist || alwaysDate)
  {

    for (i=0; i<4; i++)
    {
      int file;
      modifiedTime[i] = unknownTime;
      file = OPEN(fNames[i]+2,"rb");
      if (file >= 0)
      {
        modifiedTime[i] = MODIFIEDTIME(file);
        CLOSE(file);
      }
      else
      {
        if (checkExist)
        {
          strcpy(fNames[i],"   ");
        };
      };
    };
    
    for (i=0; i<4; i++)
    {
      int j, k;
      j=0;
      for (k=1; k<4; k++)
      {
        if (modifiedTime[k] < modifiedTime[j]) j = k;
      };
      // j=smallest time
      if (modifiedTime[j] != unknownTime)
      {
        fNames[j][0] = (char)('A'+i);
        modifiedTime[j] = unknownTime;
      };
    };
  };

  //GetFileTime((void *)3,NULL,NULL,NULL);
  STHideCursor(HC52);
  DoMenu(_2_,
         NULL, //TAG01d678
         TranslateLanguage("SELECT GAME FILE"),
         fNames[0]+((fNames[0][0]==' ')?2:0),
         fNames[1]+((fNames[1][0]==' ')?2:0),
         fNames[2]+((fNames[2][0]==' ')?2:0),
         fNames[3]+((fNames[3][0]==' ')?2:0),
         P1,
         P1,
         P1);
  STShowCursor(HC52);
  WaitForMenuSelect(_1_, 4,P1,2,1) ;
  STHideCursor(HC51);
  D0W = i16Result;
  switch (D0W)
  {
  case 1: A0 = (aReg)fNames[0]+2; break;
  case 2: A0 = (aReg)fNames[1]+2; break;
  case 3: A0 = (aReg)fNames[2]+2; break;
  case 4: A0 = (aReg)fNames[3]+2; break;
  default: A0 = (aReg)"Huh"; NotImplemented(0x20502);
  }
  d.SaveGameFilename = (char *)A0;
  A1 = data20577;
  do
  {
    *(A1++) = *A0;
  } while ( *(A0++) != 46); // until period
  *(A1++) = 66;
  *(A1++) = 65;
  *(A1++) = 75;
  *(A1) = 0;
  d.Pointer22980 = (char *)data20577;
  STShowCursor(HC51);
  RETURN;
}

// *********************************************************
//
// *********************************************************
void getBits(i32 numbits, dReg& D0, dReg& D1, aReg& A3)
// internal subroutine for TAG0206f8
{ // Bits added to D1U and swapped to D1W
  // D0 keeps track of number of bits remaining in D1W
  if (D0W < numbits)
  {
    numbits -= D0W;
    D1L <<= D0W;
    D1W = LE16(wordGear(A3));
    A3 += 2;
    D0L = 16; // Number of bits remaining in D1W
  };
  D0W = sw(D0W - numbits);
  D1L <<= numbits;
  // do this in calling routine SWAP(D1); //D1 = (D1U & 0xffff)|((D1W&0xffff)<<16);
}


void ExpandData(pnt src,pnt dst,i32 dstSize) //TAG0206f8
{
  // Data file consists of variable length fields.
  // There are 20 bytes at the beginning that are
  // lookup tables---the first is 4 bytes long
  // and is used to decode data of the form
  // ( in binary ) of 0xx.  xx is the index into the
  // table.
  // The next table is 16 bytes long and is used to
  // decode data of the form 10yyyy where yyyy is used
  // as an index into the table.
  // The only remaining case is data of the form
  // 11zzzzzzzz where zzzzzzzz is a literal byte.
  // Summary:
  // 0xx        index into small table
  // 10yyyy     index into large table
  // 11zzzzzzzz literal data
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D0, D1, D7;
  aReg A0, A2, A3;
  //SaveRegs(0x0130);
  A3 = src;
  A2 = dst;
  D7L = dstSize;
  A0 = A3;
  A3 = A3 + 20;
  D1H1 = 0;
tag020714: // Start new 16-bit input word.
  D1W = LE16(wordGear(A3));
  A3 += 2;
  D0L = 16; // # bits in D1W;
tag020718: // Is next bit a '1'?
  if ((D1W & 0x8000) == 0) goto tag02073c;
  getBits(2, D0, D1, A3);
  SWAP(D1);
  D1W -= 2;
  if (D1W != 0) goto tag020732;
  SWAP(D1);
  getBits(4, D0, D1, A3);
  SWAP(D1);
  *A2 = *(A0 + D1W + 4);
  A2++;
  goto tag020744;
tag020732:
  SWAP(D1);//D1L = D1UH[1] | (D1UH[0] << 16);
  getBits(8, D0, D1, A3);
  SWAP(D1);//D1L = D1UH[1] | (D1UH[0] << 16);
  *A2 = D1B;
  A2++;
  goto tag020744;
tag02073c:
  getBits(3, D0, D1, A3);
  SWAP(D1);//D1L = D1UH[1] | (D1UH[0] << 16);
  *A2 = *(A0 + D1W);
  A2++;
tag020744:
  D7W--;
  if (D7W == 0) goto tag020766;
  D1H2 = D1H1;
  D1H1 = 0;
  if (D0W != 0) goto tag020718;
  goto tag020714;
tag020766:
  //RestoreRegs(0x0c80);
  return;
}

// *********************************************************
//
// *********************************************************
void TAG0207cc()
{//()
  dReg D7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.PartySleeping)
  {
    DisplaySleepScreen();
    MarkViewportUpdated(0);
  }
  else
  {
    D7W = d.MagicCaster;
    d.MagicCaster = -1;
    SelectMagicCaster(D7W);
    if (d.AttackingCharacterOrdinal == 0)
    {
      d.Word20252 = 1;
    };
    DrawLegalAttackTypes();
    D7W = d.SelectedCharacterOrdinal;
    if (d.SelectedCharacterOrdinal)
    {
      d.SelectedCharacterOrdinal = 0;
      videoMode = VM_ADVENTURE;
      ShowHideInventory(D7W-1); //Show
    }
    else
    {
      FloorAndCeilingOnly();
      DrawMovementButtons();
    };
    TAG002572();
  };
  d.Word11750 = 0;
}

void TAG020836(i8 *P1)
{
  dReg D0, D7;
  //i32 D6;
  aReg A3;
  i16 index;
  A3=d.Pointer4+128;
  D7W=0; D7B=*A3; A3++;
  index=0;
  for (index=0; index< D7W; index++)
  {
    P1[index] = A3[index];
  };
  D0W=index;
  index++;
  D0W=index;
  P1[index]=0;
  D0W=D7W;
}

i16 TAG020880(i16 width,i16 height,i16 scale)
{ //width is in bytes (should be multiple of 8?)
  //scale is in 32ths.   eg:  16 means 1/2
  dReg D0, D1;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//  ui32 n;
    D0W = sw((width * scale + scale/2) / 32);
    D0W += 7; // Round to multiple of 8
    D0W &= 0xfff8;
    D1L = (height * scale + scale/2) / 32;
    D0L = D0UW * D1UW;
    return D0W;
}

void swap4words(i16 *words)
{
  words[0]=LE16(words[0]);
  words[1]=LE16(words[1]);
  words[2]=LE16(words[2]);
  words[3]=LE16(words[3]);
}

void swapNwords(i16 *words, i32 n)
{
  for (i32 i=0; i<n; i++)
  {
    words[i]=LE16(words[i]);
  };
}

void SwapKeyXlate(pnt p)
{
  while (wordGear(p)!=0)
  {
    wordGear(p) = LE16(wordGear(p));
    p+=2;
    wordGear(p) = LE16(wordGear(p));
    p+=2;
  };
}

void swapRectPos(RectPos *rect)
{
  swap4words((i16 *)rect);
}

void fixSpell(SPELL *p)
{
  p->spellID = LE32(p->spellID);
  p->word6 = LE16(p->word6);
}

void fixItem26(MONSTERDESC *pmtDesc)
{
  pmtDesc->word2  = LE16(pmtDesc->word2);
  pmtDesc->word4.Swap();//  = LE16(pmtDesc->word4);
  pmtDesc->word14 = LE16(pmtDesc->word14);
  pmtDesc->word16 = LE16(pmtDesc->word16);
  pmtDesc->word18 = LE16(pmtDesc->word18);
  pmtDesc->word20 = LE16(pmtDesc->word20);
};

void SwapGraphic0x231()
{
  SwapWordsInButtonList(d.Buttons17796); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons17760); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons16932); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons16956); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons17304); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons17196); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons17832); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons17892); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons17952); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons18060); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons18216); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons18324); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons18372); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons18660); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons18624); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons18792); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons18768); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons18708); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons18876); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons18828); // Fix littleEndian problem
  SwapWordsInButtonList(d.Buttons18936); // Fix littleEndian problem
  SwapKeyXlate(d.Byte18448);
  SwapKeyXlate(d.Byte18440);
  SwapKeyXlate((pnt)d.Byte18428);
  SwapKeyXlate(d.Byte18400);
  swapNwords((i16 *)d.MoveButn18496,4*4); // 4 directions
  swapNwords(d.Word18552,28);
  swapNwords(d.Word18568,4); // deltax
  swapNwords(d.Word18560,4); // deltay
  swapNwords((i16 *)d.Byte18600,4); // RectPos
  swapNwords((i16 *)d.Byte18592,4); // RectPos
  swapNwords((i16 *)d.Byte18584,4); // RectPos
  swapNwords((i16 *)d.Byte18576,4); // RectPos
};


//   TAG0208b4
void ReadTablesFromGraphicsFile()
{
  dReg           D0, D1, D3, D4, D5, D6, D7;
  aReg           A0;
  GRAPHIC_CLASS *psA3;
  //pnt            LOCAL_4;
  STRUCT5688    *pStruct5688;
  pnt            LOCAL_8;
  i16            LOCAL_10, LOCAL_12;
  i32 i;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //i32 saveD4=D4, saveD5=D5, saveD6=D6, saveD7=D7;
  //pnt saveA2=A2, saveA3=A3;
  d.pCeilingBitmap = allocateMemory(CEILING_BITMAP_SIZE, 1);
  d.pFloorBitmap = allocateMemory(FLOOR_BITMAP_SIZE, 1);
  d.pWallBitmaps[5] = allocateMemory(392, 1);
  d.pWallBitmaps[6] = allocateMemory(392, 1);
  d.pWallBitmaps[4] = allocateMemory(3264, 1);
  d.pWallBitmaps[3] = allocateMemory(5112, 1);
  d.pWallBitmaps[2] = allocateMemory(14208, 1);
  d.pWallBitmaps[1] = allocateMemory(2176, 1);
  d.pWallBitmaps[0] = allocateMemory(2176, 1);
  d.pDoorBitmaps[7] = allocateMemory(144, 1);
  d.pDoorBitmaps[6] = allocateMemory(256, 1);
  d.pDoorBitmaps[5] = allocateMemory(688, 1);
  d.pDoorBitmaps[4] = allocateMemory(704, 1);
  d.pDoorBitmaps[3] = allocateMemory(1560, 1);
  d.pDoorBitmaps[2] = allocateMemory(1504, 1);
  d.pDoorBitmaps[0] = allocateMemory(1504, 1);
  d.pDoorBitmaps[1] = allocateMemory(1968, 1);
  d.pViewportBMP = allocateMemory(15232, 1);
  d.pViewportBlack = d.pViewportBMP + 3248;
  d.pViewportFloor = d.pViewportBlack + 4144;
  ASSERT(d.GraphicDecompressedSizes[0x22e] == 0x1270,"graphicsize22e");
  ReadAndExpandGraphic(0x8000|0x22e, (ui8 *)d.Byte7248+2, 0,0); //TAG022b86
  for (i=0; i<27; i++)
  {
    d.Item6414[i].ITEM12_word0 = LE16(d.Item6414[i].ITEM12_word0);
  };
  d.Word4040[0] = LE16(d.Word4040[0]);
  swapNwords(d.Word7246, 12);
  swapNwords(d.Word7222, 30);
  swapNwords(d.Word7162, 60);
  swapNwords(d.Word7042, 30);
  btn missingButtons[4] =
  {
    {0x00c8,0x00f4,0x012a,0x002d,0x003a,0x0002},
    {0x00c9,0x00f4,0x012a,0x004c,0x005d,0x0002},
    {0x00ca,0x00f8,0x0125,0x00bb,0x00c7,0x0002},
    {0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}
  };
  ASSERT(    (d.GraphicDecompressedSizes[0x231] == 0x804)
          || (d.GraphicDecompressedSizes[0x231] == 0x7d4) ,"graphicSize231");
  ReadAndExpandGraphic(0x8000|0x231,(ui8 *)d.Byte18938+2, 0, 0);
  SwapGraphic0x231();
  if (d.GraphicDecompressedSizes[0x231] == 0x7d4)
  {
    d.Buttons16932[0] = missingButtons[0];
    d.Buttons16932[1] = missingButtons[1];
    d.Buttons16932[2] = missingButtons[2];
    d.Buttons16932[3] = missingButtons[3];
  };
  ASSERT(d.GraphicDecompressedSizes[0x230] == 0x4e8,"graphicSize230");
  ReadAndExpandGraphic(0x8000|0x230, (ui8 *)d.Byte20244+2, 0, 0);
  swapRectPos(&d.wRectPos20202);
  swapRectPos(&d.wRectPos20210);
  swapRectPos(&d.wRectPos20218);
  swapRectPos(&d.wRectPos20226);
  swapRectPos(&d.wRectPos20234);
  for (i=0; i<25; i++) fixSpell(&d.Spells[i]);
  ASSERT(d.GraphicDecompressedSizes[0x22f] == 0xc0e,"graphicsize22f");
  ReadAndExpandGraphic(0x8000|0x22f, (ui8 *)d.Byte10340, 0,0);
  for (i=0; i<27; i++)
  {
    bool msgPrinted = false;
    if (d.MonsterDescriptor[i].movementTicks06 == 0)
    {
      if (!msgPrinted)
           UI_MessageBox("Graphic File error\nMonster 'Time-per-move' == 0", "Warning", MESSAGE_OK);
      d.MonsterDescriptor[i].movementTicks06 = 1;
      msgPrinted = true;
    };
  };

  //d.DBEntrySize[dbSCROLL] = sizeof (DB7);
  //d.DBEntrySize[dbACTUATOR] = sizeof (DB3);
  swapNwords(d.DoorCharacteristics, 4);
  swapNwords(d.MonsterDroppings,40);
  for (i=0; i<4; i++)
  {
    d.DeltaX[i] = LE16(d.DeltaX[i]);
    d.DeltaY[i] = LE16(d.DeltaY[i]);
  };
  for (i=0; i<27; i++) fixItem26(&d.MonsterDescriptor[i]);
  for (i=0; i<8; i++)d.FoodValue[i]=LE16(d.FoodValue[i]);
  for (i=0; i<180; i++)
  {
    d.ObjDesc[i].littleEndian();
    //d.ObjDesc[i].objectType = LE16(d.ObjDesc[i].objectType);
    //d.ObjDesc[i].word4 = LE16(d.ObjDesc[i].word4);
  };
  for (i=0; i<46; i++)
  {
    d.weapons[i].word4 = LE16(d.weapons[i].word4);
  };
  ASSERT(d.GraphicDecompressedSizes[0x232] == 0x722,"graphicsize232");
  ReadAndExpandGraphic(0x8000|0x232, (ui8 *)d.Byte1832+2, 0,0);
  swap4words(d.Word20242);
  swap4words((i16 *)&d.wRectPos1780);
  swap4words((i16 *)&d.wRectPos1788);
  swap4words((i16 *)&d.wRectPos1796);
  for (i=0; i<22; i++)
  {
    d.sound1772[i].word0 = LE16(d.sound1772[i].word0);
  };
  swapNwords(d.DropOrder,30);
  swapNwords(d.Word1526[0],12);
  swapNwords(d.Word1502[0],24);
  for (i=0; i<4; i++) swapNwords((i16 *)&d.wRectPos1454[i],4);
  swapNwords(d.Word1406,8);
  swapNwords((i16 *)d.Byte1094,4);
  swapNwords((i16 *)d.PaletteBrightness,6);
  swapNwords((i16 *)d.Word1074,16);
  swapNwords((i16 *)&d.CarryLocation[0],38);
  swapNwords((i16 *)&d.wRectPos966,4);
  swapNwords((i16 *)&d.wRectPos958,4);
  swapNwords((i16 *)&d.wRectPos950,4);
  swapNwords((i16 *)&d.wRectPos942,4);
  swapNwords((i16 *)&d.wRectPos934,4);
  swapNwords((i16 *)&d.wRectPos926,4);
  swapNwords((i16 *)&d.IconDisplay, 3*46);
  swap4words((i16 *)&d.wRectPos624);
  swapNwords(d.Word612,7);
  //swapNwords(&d.Word554,1);
  for (i=0; i<6; i++) swapNwords((i16 *)&d.Palette552[i],16);
  swapNwords((i16 *)&d.Palette360,16);
  swapNwords((i16 *)&d.Palette328,16);
  swap4words(d.Word140);
  swap4words(d.Word132);
  swap4words(d.Word124);
  swap4words(d.Word116);
  swap4words(d.Word108);
  swap4words(d.Word100);
  swap4words(d.Word92);
  swap4words(d.Word84);
  swap4words(d.Word76);
  swap4words(d.Word68);
  swap4words(d.Word60);
  swap4words(d.Word52);
  swap4words(d.Word44);
  swap4words(d.Word36);
  swap4words(d.Word28);
  swap4words(d.Word20);
  swap4words(d.Word12);
  for (i32 j=0; j<70; j++)
    d.DefaultGraphicList[j] = LE16(d.DefaultGraphicList[j]);
  d.SetDerivedGraphicSize(0,15232);//d.pwDerivedGraphicSizes[0] = 15232;
  d.SetDerivedGraphicSize(1,4560);//d.pwDerivedGraphicSizes[1] = 4560;
  d.SetDerivedGraphicSize(2,1184);//d.pwDerivedGraphicSizes[2] = 1184;
  d.SetDerivedGraphicSize(3,888);//d.pwDerivedGraphicSizes[3] = 888;
  for (D7W=15; D7W<=16; D7W++)
  {
    D0W = sw(D7W + 286);
    //D1W = D7W << 2;
    //A0 = d.Byte2514 + D1W;
    //wordGear(A0) = D0W;
    d.Word2514[D7W][0] = D0W;
    //D0W = D7W << 2;
    //A0 = d.Byte2512 + D0W;
    //wordGear(A0) = 1;
    d.Word2514[D7W][1] = 1;
    //D0 = D7W*2;
    //A0 = d.pwPointer23274 + D7W;
    d.SetDerivedGraphicSize(68+2*D7W,984);//d.pwDerivedGraphicSizes[68 + D7W*2] = 984;
    //D0 = D7W * 2;
    //D0 <<= 1;
    //A0 = d.Pointer23274 + D0;
    d.SetDerivedGraphicSize(69+2*D7W, 1952);//d.pwDerivedGraphicSizes[69 + D7W*2] = 1952;
  };
  //d.Word2386 = 241;
  d.LoadedFloorDecorations[15][0] = 241;
  //d.Word2384 = 1;
  d.LoadedFloorDecorations[15][1] = 1;
  psA3 = d.s5604;
  for (D7W=0; D7W<85; D7W++)
  {
    D5W = sw(104 + psA3->uByte1);
//    D3W=(UI8)(psA3->byte1);
//    D5W += D3W
    D0W = TAG020880((UI8)(psA3->width), (UI8)(psA3->height), 16);
    d.SetDerivedGraphicSize(D5W++, D0W);//d.pwDerivedGraphicSizes[D5W++] = D0W;
    D0W = TAG020880((UI8)(psA3->width), (UI8)(psA3->height), 20);
    d.SetDerivedGraphicSize(D5W++, D0W);//d.pwDerivedGraphicSizes[D5W++] = D0W;
    if (psA3->byte4 & 1)  // Only three or four have this bit set
    {
      D0W = d.GetDerivedGraphicSize(D5W-2);//d.pwDerivedGraphicSizes[D5W - 2];
      d.SetDerivedGraphicSize(D5W, D0W);//d.pwDerivedGraphicSizes[D5W] = D0W;
      D5W++;
      D0W = d.GetDerivedGraphicSize(D5W-2);//d.pwDerivedGraphicSizes[D5W - 2];
      d.SetDerivedGraphicSize(D5W, D0W);//d.pwDerivedGraphicSizes[D5W] = D0W;
      D5W++;
    };
    if (psA3->byte4 & 16) // Only the first one has this bit
    {
      D0W = d.GetDerivedGraphicSize(D5W-2);//d.pwDerivedGraphicSizes[D5W - 2];
      d.SetDerivedGraphicSize(D5W, D0W);//d.pwDerivedGraphicSizes[D5W] = D0W;
      D5W++;
      D0W = d.GetDerivedGraphicSize(D5W-2);//d.pwDerivedGraphicSizes[D5W - 2];
      d.SetDerivedGraphicSize(D5W, D0W);//d.pwDerivedGraphicSizes[D5W] = D0W;
    };
    psA3++;
  };
  pStruct5688 = d.struct5688;
  for (D7W=0; D7W<14; D7W++) // I think these are missile or cloud
  {
    //if ((LE16(wordGear(LOCAL_4 + 4)) & 256) ==0)
    if ((LE16(pStruct5688->word4) & 256) == 0)
    {
      D5W = 282;
      //A0 = LOCAL_4;
      //D3W = ((ui8 *)A0)[1];
      D3W = pStruct5688->uByte1;
      D5W = sw(D5W + D3W);
      for (D6W=0; D6W<6; D6W++)
      {
        A0 = d.Byte5752+D6W;
        D4W = TAG020880(//(UI8)(LOCAL_4[2]),
                        pStruct5688->width,
                        //(UI8)(LOCAL_4[3]),
                        pStruct5688->height,
                         (UI8)(A0[0]));
//
        d.SetDerivedGraphicSize(D5W, D4W);//d.pwDerivedGraphicSizes[D5W] = D4W;
        //D0W = (I16)(LE16(wordGear(LOCAL_4 + 4)) & 3);
        D0W = (I16)(LE16(pStruct5688->word4) & 3);
        if (D0W != 3)
        {
          d.SetDerivedGraphicSize(D5W+6, D4W);//d.pwDerivedGraphicSizes[D5W + 6] = D4W;
          //D0W = (I16)(LE16(wordGear(LOCAL_4 + 4)) & 3);
          D0W = (I16)(LE16(pStruct5688->word4) & 3);
          if (D0W != 2)
          {
            d.SetDerivedGraphicSize(D5W+12, D4W);//d.pwDerivedGraphicSizes[D5W +12] = D4W;
          };
        };
        D5W++;
      };
    };
    //LOCAL_4 +=6;
    pStruct5688++;
  };

  ui8 *pCM;
  pCM = d.Byte5728; d.Pointer1864[0] = pCM;
  pCM = d.Byte5744; d.Pointer1860 = pCM;
  pCM = d.IdentityColorMap;  d.Pointer1852 = pCM; d.Pointer1856 = pCM;
  D5W = 438;
  LOCAL_8 = d.Byte5696;
  for (D7W=0; D7W<4; D7W++)
  {
    for (D6W=4; D6W<32; D6W+=2)
    {
      D0W = TAG020880((UI8)(LOCAL_8[0]),(UI8)(LOCAL_8[1]), D6W);
      d.SetDerivedGraphicSize(D5W, D0W);//d.pwDerivedGraphicSizes[D5W] = D0W;
      D5W++;
    };
    if (D7W == 3)
    {
      D0W = (UI8)(LOCAL_8[0]);
      D1W = (UI8)(LOCAL_8[1]);
      D0L = D0UW*D1UW;
      d.SetDerivedGraphicSize(D5W, D0W);//d.pwDerivedGraphicSizes[D5W] = D0W;
      D5W++;
    };
    LOCAL_8+=2;
  };
  D5W = 0x1ef;
  for (D6W=0; D6W<27; D6W++)
  {
    //A2 = d.Byte6414 + 12*D6W;
    //qA0 = d.Byte9660 + D6W*26;
    //D7W = d.MonsterDescriptor[D6W].word4;
    d.Item6414[D6W].ITEM12_word2 = D5W;
    D0W = TAG020880(d.Item6414[D6W].frontRearByteWidth,
                    d.Item6414[D6W].frontRearHeight,
                    16);
    LOCAL_12 = D0W;
    d.SetDerivedGraphicSize(D5W++, D0W);//d.pwDerivedGraphicSizes[D5W++] = D0W;
    D0W = TAG020880(d.Item6414[D6W].frontRearByteWidth,
                     d.Item6414[D6W].frontRearHeight,
                     20);
    LOCAL_10 = D0W;
    d.SetDerivedGraphicSize(D5W, D0W);//d.pwDerivedGraphicSizes[D5W] = D0W;
    D5W++;
    //if (D7W & 8)
    if(d.MonsterDescriptor[D6W].word4.HasSideGraphic())
    {
      D0W = TAG020880(d.Item6414[D6W].sideByteWidth,
                       d.Item6414[D6W].sideHeight,
                       16);
      d.SetDerivedGraphicSize(D5W, D0W);//d.pwDerivedGraphicSizes[D5W] = D0W;
      D5W++;
      D0W = TAG020880(d.Item6414[D6W].sideByteWidth,
                       d.Item6414[D6W].sideHeight,
                       20);
      d.SetDerivedGraphicSize(D5W, D0W);//d.pwDerivedGraphicSizes[D5W] = D0W;
      D5W++;
    };
    //if(D7W & 16)
    if(d.MonsterDescriptor[D6W].word4.HasBackGraphic())
    {
      d.SetDerivedGraphicSize(D5W, LOCAL_12);//d.pwDerivedGraphicSizes[D5W] = LOCAL_12;
      D5W++;
      d.SetDerivedGraphicSize(D5W, LOCAL_10);//d.pwDerivedGraphicSizes[D5W] = LOCAL_10;
      D5W++;
    };
    //if(D7W & 32)
    if(d.MonsterDescriptor[D6W].word4.HasAttackGraphic())
    {
      D0W = TAG020880(d.Item6414[D6W].attackByteWidth,
                       d.Item6414[D6W].attackHeight,
                       16);
      d.SetDerivedGraphicSize(D5W, D0W);//d.pwDerivedGraphicSizes[D5W] = D0W;
      D5W++;
      D0W = TAG020880(d.Item6414[D6W].attackByteWidth,
                       d.Item6414[D6W].attackHeight,
                       20);
      d.SetDerivedGraphicSize(D5W, D0W);//d.pwDerivedGraphicSizes[D5W] = D0W;
      D5W++;
    };
    //for (LOCAL_12=(I16)(D7W&3); LOCAL_12 != 0; LOCAL_12--)
    for (LOCAL_12=d.MonsterDescriptor[D6W].word4.Nibble0();
         LOCAL_12 != 0; 
         LOCAL_12--)
    {
      D0W = d.Item6414[D6W].frontRearByteWidth;
      D1W = d.Item6414[D6W].frontRearHeight;
      D0L = D0UW*D1UW;
      d.SetDerivedGraphicSize(D5W, D0W);//d.pwDerivedGraphicSizes[D5W] = D0W;
      D5W++;
      D0W = TAG020880(d.Item6414[D6W].frontRearByteWidth,
                     d.Item6414[D6W].frontRearHeight,
                     16);
      d.SetDerivedGraphicSize(D5W, D0W);//d.pwDerivedGraphicSizes[D5W] = D0W;
      D5W++;
      D0W = TAG020880(d.Item6414[D6W].frontRearByteWidth,
                     d.Item6414[D6W].frontRearHeight,
                     20);
      d.SetDerivedGraphicSize(D5W, D0W);//d.pwDerivedGraphicSizes[D5W] = D0W;
      D5W++;
    };
  };
  //D4=saveD4;D5=saveD5;D6=saveD6;D7=saveD7;A2=saveA2;A3=saveA3;
}

void TAG020fbc()
{
//  dReg D6;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  d.Pointer2214 = (pnt)allocateMemory(3264,1);
  d.Pointer2218 = (pnt)allocateMemory(5112, 1);
  d.Pointer2222 = (pnt)allocateMemory(14208,1);
  d.Pointer2226 = (pnt)allocateMemory(2176,1);
  d.Pointer2230 = (pnt)allocateMemory(2176,1);
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG021028()
{ //()
  static dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  static i16 LOCAL_2;
  RESTARTMAP
    //RESTART(1)
    RESTART(2)
    //RESTART(3)
    RESTART(4)
    RESTART(5)
    RESTART(6)
  END_RESTARTMAP
  d.PressingEye=0;//Word11756 = 0;
  d.QuitPressingEye = 0;
  d.PressingMouth = 0;
  d.QuitPressingMouth = 0;
  d.FlashButnActive = 0;
  d.Word11776 = 0;
  d.PartySleeping = 0;
  d.Word1836 = 15;
  //d.Word1838 = 0;
  d.Word1840 = 0;
  d.AttackingCharacterOrdinal = 0;
  d.Word20252 = 1;
  d.Word23144 = 0;
  d.WatchdogTime = 0xffffffff;
  d.PrimaryButtonList = d.Buttons17196; // characters, spells, movement
  d.SecondaryButtonList = d.Buttons17304;
  d.pKeyXlate2 = (KeyXlate *)d.Byte18400;
  d.pKeyXlate1 = d.Byte18428;
  LoadPartyLevel(d.partyLevel);
  D0W = d.gameState;
  if (d.gameState == GAMESTATE_ResumeSavedGame)
  {
    //ClearMemory(d.LogicalScreenBase, 32000);
    //FadeToPalette(&d.Palette11978);
    for (;;)
    {
      DoMenu(_6_,
             NULL,
             d.Pointer22812,
             "PLAY GAME",
             "ASCII DUMP",
             NULL,
             NULL,
             1,
             1,
             1);
      //if (1 == TAG01d1cc(2,1,0,0)) break;
      STShowCursor(HC33);
      WaitForMenuSelect(_2_, 2,1,0,0);
      STHideCursor(HC33);
      if (i16Result == 1) break;
      AsciiDump();
    } ;
#ifndef SDL20
    if (fullscreenRequested)
    {
      UI_ClearScreen();
      virtualFullscreen = true;
    };
#endif
    LOCAL_2 = d.Word23244;
    if (LOCAL_2 > 1)
    {
      d.Word23244 = 2;
      closeGraphicsFile();
    };
    InsertDisk(0,1);
    if (LOCAL_2 > 1)
    {
      openGraphicsFile();
      d.Word23244 = LOCAL_2;
    };
    wvbl(_4_);
    ClearMemory(d.LogicalScreenBase, 32000);
  }
  else
  {
    d.UseByteCoordinates = 0;
    wvbl(_5_);
    FillRectangle(d.LogicalScreenBase,&d.wRectPos1780,0,160); //Top of screen
    FillRectangle(d.LogicalScreenBase,&d.wRectPos1788,0,160);//Right side of screen
    FillRectangle(d.LogicalScreenBase,&d.wRectPos1796,0,160);//Scrolling text area
  };
  setpalette(&d.Palette552[0]);
  DrawMovementButtons();
  TAG0138ec();
  d.ClockRunning = 1;
  RETURN
}

void DisableTraceIfEnciphered()
{
  CSB_UI_MESSAGE csbMessage;
  if (  (encipheredDataFile == NULL)
       && !simpleEncipher ) return;
  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_TIMERTRACE;
  csbMessage.p2 = 0;
  if (TimerTraceActive)
  {
    CSBUI(&csbMessage);
  };

  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_ATTACKTRACE;
  csbMessage.p2 = 0;
  if (AttackTraceActive)
  {
    CSBUI(&csbMessage);
  };

  csbMessage.type = UIM_SETOPTION;
  csbMessage.p1 = OPT_AITRACE;
  csbMessage.p2 = 0;
  if (AITraceActive)
  {
    CSBUI(&csbMessage);
  };

  if (trace >= 0) 
  {
    CLOSE(trace);
    trace = -1;
  };
}

RESTARTABLE _GameSetup(i32 showPrisonDoor)
{
  static dReg D7;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
    RESTART(4)
  END_RESTARTMAP
  CountFloppyDrives();
  InitializeHeap();//TAG020286
  D7W = 1;
  ReadGraphicsIndex(); // TAG021d9a
  ReadTablesFromGraphicsFile();
  HangIfZero(D7W); // (=0x020276)
  //DisplayChaosStrikesBack(_1_);
  ReadFloorAndCeilingBitmaps(0);
  ReadWallBitmaps(0);
      HopefullyNotNeeded(0x669d);
  TAG001c6e();
  TAG0010ae();
    //D0 = GetAbsFuncAddr(276); // TAG00dd78(276);
    //D0 = TAG00306c;
  d.pFunc23232 = TAG00306c;
    //D0 = TAG00dd78(&DetachItem16)
  d.pFunc23236 = DetachItem16;
  //  MemMove((pnt)palette,(pnt)&d.Palette11946, 32);
  //  MemMove((pnt)palette,(pnt)&d.Palette11978, 32);
  TAG0023b0();
  TAG000ec6(); // In supervisor mode
  jdisint(5);  // TRAP 14 (#26,#5);
  //TAG0020ca();
  wvbl(_2_);
  MemMove((ui8 *)d.Palette552, (ui8 *)&d.Palette11946, 32); // Initialize palette
  MemMove((ui8 *)d.Palette552, (ui8 *)&d.Palette11978, 32); // Initialize palette
  if (showPrisonDoor == 1)
  {
    TAG01f746(_3_);
  }
  else
  {
    STShowCursor(HC50);
    STHideCursor(HC33);
  };
  for (;;)
  {
    ReadEntireGame(_4_);
    SmartDiscard(true);
    if (i16Result == 1) break;
    openGraphicsFile(); // TAG021d36
    TAG01f746(_1_);
  };
  DisableTraceIfEnciphered();
  openGraphicsFile();// TAG021d36
  RecordFile_Record(DM_rules ? "#DMRules\n" : "#NoDMRules\n");
  TAG021cd8();
  ReadSpellGraphic();
  RETURN;
}

#ifdef MIDI
void playmidi();
#endif
// *********************************************************
//
// *********************************************************
//   TAG0211a0
RESTARTABLE _ShowPrisonDoor()
{//()
  static dReg D7;
  RESTARTMAP
    RESTART(1)
    //RESTART(2)
    //RESTART(3)
    //RESTART(4)
//    RESTART(5)
    RESTART(6)
  END_RESTARTMAP
  //i16 saveD7 = D7W;
  //i8 LOCAL_130[130];
////  TAG001d0c(); // Set critical error handler.
  //TAG020836(LOCAL_130); //??Copy some part of Program Descriptor?? // Never used!
  GameSetup(_1_, 1);
  if (DMRulesDesignOption) 
  {
    DM_rules = true;
    RecordFile_Record("#DMRules\n");
  };
  TAG021028(_6_);
  closeGraphicsFile();
  if (d.gameState != GAMESTATE_ResumeSavedGame)
  {
    MoveObject(RN(RNnul), -1, 0, d.partyX, d.partyY, NULL, NULL);
  };
  STShowCursor(HC33); //TAG003026
  DiscardAllInput();
  //D7W = saveD7;
  RETURN;
}

// *********************************************************
//
// *********************************************************
void TAG02134a(pnt)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //d.Word918 = 136;
  //d.Word1838 = 136;
  //d.Word11752 = 0;
}

/*

// *********************************************************
//
// *********************************************************
//   TAG021822
void RemoveFromFreeList(ITEMQ *This)
{
  ITEMQ *Prev, *Next;
  VerifyGraphicFreeList();
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //pgA3 = pQ;
  Prev = This->pgPrev();
  Next = This->pgNext();
  if (Prev == NULL)
  {
    if (Next == NULL)
    {
      d.pgUnused = NULL;
    }
    else
    {
      d.pgUnused = Next;
      Next->pgPrev(NULL);
    };
  }
  else
  {
    Prev->pgNext(Next);
    if (Next != NULL)
    {
      Next->pgPrev(Prev);
    };
  };
  VerifyGraphicFreeList();
}

*/
/*
ui8 long02160a[] ALIGN4 = {
 0x00,0x01,0x00,0x01,
 0x00,0x03,0x00,0x03,
 0x00,0x07,0x00,0x07,
 0x00,0x0f,0x00,0x0f,
 0x00,0x1f,0x00,0x1f,
 0x00,0x3f,0x00,0x3f,
 0x00,0x7f,0x00,0x7f,
 0x00,0xff,0x00,0xff,
 0x01,0xff,0x01,0xff,
 0x03,0xff,0x03,0xff,
 0x07,0xff,0x07,0xff,
 0x0f,0xff,0x0f,0xff,
 0x1f,0xff,0x1f,0xff,
 0x3f,0xff,0x3f,0xff,
 0x7f,0xff,0x7f,0xff,
 0xff,0xff,0xff,0xff
};
*/

ui32 long02160a[]  = {
 0x00010001,
 0x00030003,
 0x00070007,
 0x000f000f,
 0x001f001f,
 0x003f003f,
 0x007f007f,
 0x00ff00ff,
 0x01ff01ff,
 0x03ff03ff,
 0x07ff07ff,
 0x0fff0fff,
 0x1fff1fff,
 0x3fff3fff,
 0x7fff7fff,
 0xffffffff
};


ui32 long02164a[] = {
  0xfffefffe,
  0xfffcfffc,
  0xfff8fff8,
  0xfff0fff0,
  0xffe0ffe0,
  0xffc0ffc0,
  0xff80ff80,
  0xff00ff00,
  0xfe00fe00,
  0xfc00fc00,
  0xf800f800,
  0xf000f000,
  0xe000e000,
  0xc000c000,
  0x80008000,
  0x00000000
};

ui8 long02168a[] ALIGN4 = {
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0xff,0xff,0x00,0x00,0x00,0x00,
 0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00,
 0xff,0xff,0x00,0x00,0xff,0xff,0x00,0x00,
 0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
 0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,
 0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff,
 0x00,0x00,0xff,0xff,0x00,0x00,0xff,0xff,
 0xff,0xff,0xff,0xff,0x00,0x00,0xff,0xff,
 0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,
 0xff,0xff,0x00,0x00,0xff,0xff,0xff,0xff,
 0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,
 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
};

enum BSRRETURNS {
  Tag02148a = 1,
  Tag0214aa,
  Tag0214e6,
  Tag0214f6,
  Tag02155e,
  Tag021564,
  Tag021568
};

#define BSR(to,from)              \
  retstack[--retindex]=Tag##from; \
  goto tag##to;                   \
tag##from:

#define RTS goto tagReturn;

#define POP retindex++;

void ExpandGraphic(i8 *src,ui8 *dest,i16 P3,i16 P4,i32 maxSize)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D0, D1, D2, D3, D4, D5, D6, D7;
  aReg A0, A1, A2=NULL, A3, A4, A5, A6;
  //i32 saveD4=D4,saveD5=D5,saveD6=D6,saveD7=D7;
  //pnt saveA2=A2,saveA3=A3,saveA4=A4,saveA5=A5,saveA6=A6;
  i32 retindex=10;
  i32 C;
  BSRRETURNS retstack[10];
  D1L = 0;
  D7L = 0;
  ASSERT(((int)src & 3) ==0,"src");
  ASSERT(((int)dest & 3) ==0,"dst");
  A0 = (aReg)src;
  A1 = (aReg)dest;
  D5W = P3;
  D4W = P4;
  D7W = LE16(wordGear(A0)); A0+=2; // GraphicWidth
  D6W = LE16(wordGear(A0)); A0+=2; // GraphicHeight
  D7W = (I16)(((D7W+15)>>1)&0x7ff8); // Number of bytes/GraphicLine
  if((A1==0) && (A1=(pnt)physbase(),D7W!=160)) // If destination not specified
  {
    D5W = (I16)((D5W>>1)&0x7ff8);
    A1 += D5W;
    D4L = D4UW * 160; //(ui16)D0W;
    A1 += D4W;
    A3 = A1 + D7W;
    D6W--;
    D6L = D6UW * 160; //(ui16)D0W; // Total number bytes
    if (D6L > maxSize) D6L = maxSize;
    D6L += (i32)A3;
    D4W = sw(160 - D7W);
//    D7W = D4W;
    D7W = 160;
  }
  else
  {
    D6L = D6UW * (ui16)D7W; // Total number bytes
    if (D6L > maxSize) D6L = maxSize;
    A3 = A1 + D6L;//Destination LWA+1
    D6L = (i32)A3;//Destination LWA+1
  };
  longGear((ui8 *)A1)   = 0;
  longGear((ui8 *)A1+4) = 0;
  D0L = 15; //#pixels in this group-1
  D4L = 0;  //pixel# in this group
  A4 = (pnt)long02160a;
  A5 = (pnt)long02164a;
  A6 = (pnt)long02168a;
tag021470:
  D2B = *(A0++);
  D3B = D2B;
  if (D2B < 0) goto tag0214be;
  D3B = (i8)((D3UB)>>4);
  if (D3B == 0) goto tag0214f2;
  D2W &= 0xf;
  D1L = 1;
  D1B = (i8)(D1B + D3B);
tag021482:
  D2W <<= 3;
  A2 = A6 + D2W;
tag021488:
  BSR (0214fa,02148a);//place D1 pixels of color *A2
  if (D1W == 0) goto tag021470;
  if ( (ui16)D1W < 16) goto tag021488;
  D3L = LE32(longGear((ui8 *)A2));
  D5L = LE32(longGear((ui8 *)A2+4));
  D1W -= 16;
tag02149e:
  longGear((ui8 *)A1) = LE32(D3L);
  A1+=4;
  longGear((ui8 *)A1) = LE32(D5L);
  A1+=4;
  if (A1 != A3) goto tag0214aa;
  BSR(02153c,0214aa);
  D1W -= 16;
  if ( (ui16)D1W < 65536-16) goto tag02149e;
  longGear((ui8 *)A1) = 0;
  longGear((ui8 *)A1+4) = 0;
  D1W +=16;
  if (D1W != 0) goto tag021488;
  goto tag021470;
tag0214be:
  D2W &= 15;
  D1L = 0;
  D1B = *(A0++);
  if ((D3W & 0x40)==0) goto tag0214d0;
  D1W <<= 8;
  D1B = *(A0++);
tag0214d0:
  D1W += 1;
  if ((D3W&0x10)==0) goto tag021482;
  if ((D3W&0x20)==0) goto tag021558;
  A2 = A1-D7W;
tag0214e4:
  BSR(0214fa,0214e6);
  if (D5L == 0) goto tag0214ec;
  A2 = (pnt)D5L;
tag0214ec:
  A2 += 8;
  if (D1W != 0) goto tag0214e4;
tag0214f2:
  BSR(021570,0214f6);
  goto tag021470;
tag0214fa: // D1=repeat, D0=#-1 pixels remain in group *A2 = color
  D4B = D0B;
  D4W = sw(D4W + D4W);
  D4W = sw(D4W + D4W);
  D3L = (longGear((ui8 *)A4+D4W)); //mask of remaining pixels in group
  if (D0L < D1L) goto tag021518;
  D0W = sw(D0W - D1W); //decrement pixels remaining in group
  D4B = D0B;
  D4W = sw(D4W + D4W + D4W + D4W);
  D3L &= (longGear((ui8 *)A5 + D4W)); //data mask
  D1L = 0;
  goto tag02151e;
tag021518:
  D1W = sw(D1W - D0W);
  D1W--;
  D0L = -1;
tag02151e:
  D5L = LE32(longGear((ui8 *)A2));
  D5L &= D3L;
  longGear((ui8 *)A1) |= LE32(D5L);

  //D5L = 0;
  A1 += 4;
  D5L = LE32(longGear((ui8 *)A2+4));
  D5L &= D3L;
  longGear((ui8 *)A1) |= LE32(D5L);
  A1 += 4;
  D5L = 0;
  if (D0W < 0) goto tag021538;
  A1 -= 8;
  RTS;
tag021536:
  A1 += 8;
tag021538:
  if (A1 != A3) goto tag02154e;
tag02153c:
  if (A1 == (pnt)D6L) goto tag02170a;
  A3 += D7W;
  SWAP(D7);//D7 = ((D7<<16)&0xffff0000) | ((D7>>16)&0xffff);
  A1 += D7W;
  D5L = (i32)A2;
  D5W = sw(D5W + D7W);
  SWAP(D7);// = ((D7<<16)&0xffff0000) | ((D7>>16)&0xffff);
tag02154e:
  D0L = 15;
  longGear((ui8 *)A1) = 0;
  longGear((ui8 *)A1+4) = 0;
  RTS;

tag021558:
  C = D1W&1;
  D1W >>= 1;
  if (C==0) goto tag02155e;
  BSR(021570,02155e)
  D2B = *(A0);
  D2B >>= 4;
  BSR(021570,021564);
  D2B = *(A0++);
  BSR(021570,021568);
  D1W--;
  if (D1W != 0) goto tag02155e;
  goto tag021470;
tag021570://D0=#pixels left in group -1; place one pixel color D2
  D3W = 0;
  D3L |= 1<<D0B;
//  D2W &= 15;
//  D2W += D2W
//  switch (D2W)
//  {
  if (D2W & 0x8) wordGear(A1+6) |= LE16((D3W));
  if (D2W & 0x4) wordGear(A1+4) |= LE16((D3W));
  if (D2W & 0x2) wordGear(A1+2) |= LE16((D3W));
  if (D2W & 0x1) wordGear(A1+0) |= LE16((D3W));
  D0W--;
  if (D0W < 0) goto tag021536;
  RTS;
//  case 0: goto tag0215aa;
//  case 1: goto tag0215a8;
//  case 2: goto tag0215cc;
//  case 3: goto tag0215a4;
//  case 4: goto tag0215dc;
//  case 5: goto tag0215b6;
//  case 6: goto tag0215c8;
//  case 7: goto tag0215a0;
//  case 8: goto tag0215ee;
//  case 9: goto tag0215ec;
//  case 10: goto tag0215fa;
//  case 11: goto tag0215e8;
//  case 12: goto tag0215d8;
//  case 13: goto tag0215b2;
//  case 14: goto tag0215c4;
//  case 15:
//    wordGear(A1+6) |= LE16(D3W);
//    wordGear(A1+4) |= LE16(D3W);
//    wordGear(A1+2) |= LE16(D3W);
//    wordGear(A1) |= LE16(D3W);
//    D0W--;\
//    if (D0W < 0) goto tag021536;
//    RTS;
//tag0215b2:
tag02170a:
  POP; // discard return address
  //D4=saveD4;D5=saveD5;D6=saveD6;D7=saveD7;
  //A2=saveA2;A3=saveA3;A4=saveA4;A5=saveA5;A6=saveA6;
  ASSERT(retindex==10,"retindex");
  return;
tagReturn:
  switch (retstack[retindex++])
  {
  case Tag02148a: goto tag02148a;
  case Tag0214e6: goto tag0214e6;
  case Tag0214f6: goto tag0214f6;
  case Tag02155e: goto tag02155e;
  case Tag021564: goto tag021564;
  case Tag021568: goto tag021568;
  default: NotImplemented(0x21712);
  };
}




void checkMemory()
{
  pnt prevblk=NULL;
  pnt blk = firstMemoryBlock;
  //return;
  while (blk != NULL)
  {
    ASSERT(longGear((ui8 *)blk+8) == memDebugFlag,"blk+8");
    prevblk=blk;
    blk = pntGear(blk);
  };
}



ui8 *allocateMemory(i32 sizeNeeded,i16 type) //TAG02175c
{ // Allocate memory.
  // Type =1 --> permanent allocation.
  // Type =0 --> temporary allocation
  // Type =2 --> temporary allocation of as much as possible
  //dReg D7;
  aReg A0, A3;
  ASSERT(sizeNeeded != 0,"sizeNeeded");
  //D7L = sizeNeeded;
  sizeNeeded = (sizeNeeded + 3) & 0xfffffffc; // Make the size a longword boundary
//  if ((type==1) && (d.Long23326 >= sizeNeeded+12))
//  {  // Allocate static memory from low addresses
//    A3 = d.Pointer23322;
//    d.Pointer23322 += sizeNeeded+12; // room for debug list
//    d.Long23326 -= sizeNeeded+12;
//    pntGear(A3) = firstMemoryBlock;
//    firstMemoryBlock = A3;
//    longGear(A3+8) = memDebugFlag;
//    longGear(A3+4) = sizeNeeded-12;
//    A3 += 12;
//  }
//  else
//  { // Not enough space or temporary allocation
    if (sizeNeeded+12 > d.iAvailableMemory)
    { // die, I think.
      die(40);
    };
    if (type == 2)
    {
      // Allocate all that's left from end
      A3 = (pnt)d.pEndOfAvailMemory - d.iAvailableMemory;
      d.iAvailableMemory -= sizeNeeded;
    }
    else if (type==1)
    {
      A3 = (pnt)d.pEndOfAvailMemory-sizeNeeded-12;
      ASSERT(((int)d.pEndOfAvailMemory & 3) == 0,"endOfMemory");
      ASSERT(((int)A3 & 3) == 0,"A3");
      d.pEndOfAvailMemory = (ui8 *)A3;
      pntGear(A3) = firstMemoryBlock;
      firstMemoryBlock = A3;
      longGear((ui8 *)A3+4) = sizeNeeded;
      longGear((ui8 *)A3+8) = memDebugFlag;
      A3 += 12;
      d.iAvailableMemory -= sizeNeeded+12;
    }
    else
    { // Temporary allocation
      A3 = (pnt)d.pEndOfAvailMemory-sizeNeeded;
      d.pEndOfAvailMemory = (ui8 *)A3;
      d.iAvailableMemory -= sizeNeeded;
    }
//  };
  A0 = A3;
  memset(A0, 0xba, sizeNeeded);
  //D7 = saveD7;
  //A3 = saveA3;
  return (ui8 *)A0;
}


//   TAG0217d8
void FreeTempMemory(i32 size)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  size = (size+3) & 0xfffffffc;
  d.iAvailableMemory += size;
  d.pEndOfAvailMemory += size;

}

void TAG021800(i32 num)
{
  if (num & 1) num++;
  d.iAvailableMemory += num;
}

//   TAG021c08
void InitializeCaches(ui8 *memStart, i32 memSize, ui8 *P3, i32 /*P4*/)
{ // P4 is (0x19bf8-endOS)=space above Operating System
  dReg D7;
  aReg A3;
  A3 = (pnt)memStart;
  D7L = memSize;
  d.pStartMemory = memStart;
  d.Pointer23318 = memStart;
  d.Pointer23294 = d.pEndOfAvailMemory = (ui8 *)A3+D7L;
  d.iAvailableMemory = D7L;
  d.Pointer23322 = P3;
//  d.Long23326 = P4;
  d.ClusterCache = (ui8 *)allocateMemory(1024,1); //TAG02175c
  if (d.pGraphicCachePointers != NULL) 
  {
    CleanupGraphics();
  }
  ClearMemory((ui8 *)d.pGraphicCachePointers, 4*NumExpandedGraphics); // TAG000a84

  d.AllocateDerivedGraphicCacheIndex();// = (i16 *)allocateMemory(1460, 1); //TAG02175c
  //fillMemory(d.DerivedGraphicIndex, 730, -1, 2);
  d.AllocateDerivedGraphicsSizesBuffer();//d.pwDerivedGraphicSizes = (i16 *)allocateMemory(1460, 1); //TAG02175c
  d.pgUnused = NULL;
//  d.pqFirstGraphic2 = NULL;
//  d.pqLastGraphic = NULL;
  d.pqFirstGraphic = NULL;
//  d.Long23358 = -1;
  //D7 = saveD7;
  //A3 = saveA3;
}

void TAG021cd8()
{
//  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D0L = d.Pointer23298 - d.pStartMemory;
  //D0L = D0L > 370000 ? 1 : 0;
  TAG020fbc();
  d.pStartAvailMemory = d.pEndOfAvailMemory - 10000;
  d.Word23328 = 1;
  d.Pointer23310 = d.Pointer23314 = (pnt)d.pStartAvailMemory;
  d.iAvailableMemory = 10000;
  //D0L = d.pStartAvailMemory - d.pStartMemory;
  d.iAvailableGraphicMemory = d.pStartAvailMemory - d.pStartMemory;
  if (d.iAvailableGraphicMemory < 0)
  {
    UI_MessageBox("Insufficient Memory Allocated","Error",MESSAGE_OK);
    die(0xd7d3);
  };
}


//  TAG022c90
bool AllocateDerivedGraphic(i32 graphicNum)
{// return true if graphic already exists
  //
  i32 cacheIndex;
  i32 bufferSize;
  ITEMQ *pqA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7L = graphicNum;
//  if (d.Time != d.Long23358)
//  {
//    ClearGraphicList2();
//  };
  if (d.GetDerivedGraphicCacheIndex(graphicNum) != -1)
  {
    //TAG02289a(D6W);
    return true;
  }
  else
  {
    bufferSize = d.GetDerivedGraphicSize(graphicNum) + 16;//(ui16)(d.pwDerivedGraphicSizes[graphicNum] + 16);
    pqA3 = GetExpandedGraphicBuffer(bufferSize);
    //==============
    //D6W = 0;
    //while (d.ppExpandedGraphics[D6W++] != 0) {};
    //D6W--;
    //if (D6W >= NumExpandedGraphics)
    //{
    //  die(0x6c7e,"No space for ExpandedGraphic");
    //};
    //===============
    for (cacheIndex=0; cacheIndex<NumExpandedGraphics; cacheIndex++)
    {
      if (d.pGraphicCachePointers[cacheIndex] == NULL) break;
    };
    if (cacheIndex >= NumExpandedGraphics)
    {
      die(0x6c7e,"No space for ExpandedGraphic");
    };
    d.SetDerivedGraphicCacheIndex(graphicNum, cacheIndex);
    ASSERT(d.pGraphicCachePointers[cacheIndex] == NULL,"expandedGraphic");
    d.pGraphicCachePointers[cacheIndex] = pqA3;
    pqA3->graphicNum = UI16(graphicNum | 0x8000);
  };
//#ifdef _VERIFYPPQ
//  verifyppq();
//#endif
  return false;
}

ui8 *GetDerivedGraphicAddress(i32 graphicNum)
{
  dReg D0;
  aReg A0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D0W = d.GetDerivedGraphicCacheIndex(graphicNum);
  A0 = (aReg)d.pGraphicCachePointers[D0W];
  return (ui8 *)A0+12;
}

/*
void TAG022d5e(i16 P1)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TAG022778(d.GraphicIndex8000[P1]);
}
*/

ui16 GetGraphicDecompressedSize(i32 P1) //(022d7a)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  return d.GraphicDecompressedSizes[P1];
}

// *********************************************************
//
#ifdef MIDI
void playmidi();
#endif
// *********************************************************
RESTARTABLE _StartCSB(const CSB_UI_MESSAGE * /*msg*/) //
{//()
  static aReg A4, A5;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
  END_RESTARTMAP
//  i32 dBankLen=programDescriptor[5]+programDescriptor[7];
  A4 =  (pnt)(&d);
  A4 += programDescriptor[7]; // uninitialized data,a4,initialized data
  A5=NULL; // Should stay that way.
  // Clear uninitialized memory.
  //memset (A4-programDescriptor[7],0,sizeof (d));
  Cleanup(false);
  d.Initialize(); // Pointers and initial values
  d.Pointer4 = NULL; // pointer to I-Bank (A5 in 68000 code).
#if defined _MSVC_CE2002ARM
  STHideCursor(HC50);
#endif
  ShowPrisonDoor(_1_);
  GameIsComplete = false;
#ifndef SDL20
  if (fullscreenRequested)
  {
    UI_ClearScreen();
    virtualFullscreen = true;
  };
#endif
  //To get dump of prison   AsciiDump();
  for (;;)
  {
    MainLoop(_2_); //TAG00068e
    d.DynamicPaletteSwitching = 0;
    {
      ShowCredits(_3_,d.GameIsLost);
    };
    if (GameIsComplete) break;
  };
#if defined _MSVC_CE2002ARM
  STShowCursor(HC50);
#endif
  RETURN;
}
#ifdef MIDI
void playmidi();
#endif

RESTARTABLE _AskWhatToDo()
{//(i32)
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
    RESTART(4)
    RESTART(5)
    RESTART(6)
    RESTART(7)
    RESTART(8)
    RESTART(9)
    RESTART(10)
  END_RESTARTMAP
  Cleanup(false);
#ifdef MIDI
  playmidi();
#endif
  d.Initialize(); // Pointers and initial values
  CountFloppyDrives();
  InitializeHeap();//TAG020286
  ReadGraphicsIndex(); // TAG021d9a
  ReadTablesFromGraphicsFile();
  DisplayChaosStrikesBack(_2_);
  ReadFloorAndCeilingBitmaps(0);
  ReadWallBitmaps(0);
      HopefullyNotNeeded(0x7486);
  TAG001c6e();
/*
  TAG0010ae();
    //D0 = GetAbsFuncAddr(276); // TAG00dd78(276);
    //D0 = TAG00306c;
  d.pFunc23232 = TAG00306c;
    //D0 = TAG00dd78(&DetachItem16)
  d.pFunc23236 = DetachItem16;
  //  MemMove((pnt)palette,(pnt)&d.Palette11946, 32);
  //  MemMove((pnt)palette,(pnt)&d.Palette11978, 32);
  TAG0023b0();
  TAG000ec6(); // In supervisor mode
  jdisint(5);  // TRAP 14 (#26,#5);
  TAG0020ca();
  wvbl(_2_);
  MemMove((pnt)d.Palette552, (pnt)&d.Palette11946, 32); // Initialize palette
  MemMove((pnt)d.Palette552, (pnt)&d.Palette11978, 32); // Initialize palette
  TAG01f746(_3_);
  */
  setscreen(d.LogicalScreenBase,d.LogicalScreenBase,0);
  MemMove((ui8 *)d.Palette552, (ui8 *)&d.Palette11978, 32); // Initialize palette
#ifdef TARGET_OS_MAC
  MacShowCursor();
#endif
  DoMenu(_1_,  //build the menu
         "I AM READY, MASTER",
         "WHAT IS YOUR WISH?",
         "QUIT",
         "DUNGEON",
         "UTILITY",
         "HINT",
         1,
         1,
         1);
  if (PlaybackCommandOption)
  {
    GameMode = 1;
  }
  else
  {
    STShowCursor(HC50);
    WaitForMenuSelect(_3_, 4,1,0,0);
    STHideCursor(HC53);
    GameMode = i16Result-1;
  };
  switch (GameMode)
  {
    case 0: //"Quit"
    UI_PushMessage(UIM_TERMINATE);
    WAITFORMESSAGE(_4_); //Should never come back!
    ASSERT(0,"gameMode");
    break;
  case 1://"Dungeon"
#ifdef TARGET_OS_MAC
    HideCursor();
#endif
    do
    {
      StartCSB(_5_);
      Cleanup(false);
    } while (RepeatGame);
    UI_PushMessage(UIM_TERMINATE);
    WAITFORMESSAGE(_6_); //Should never come back!
    ASSERT(0,"Repeat");
    break;
  case 2://"Utility"
    StartChaos(_7_);
    UI_PushMessage(UIM_TERMINATE);
    WAITFORMESSAGE(_8_); //Should never come back!
    ASSERT(0,"utility");
    break;
  case 3://"Hint"
    StartHint(_9_);
    UI_PushMessage(UIM_TERMINATE);
    WAITFORMESSAGE(_10_); //Should never come back!
    ASSERT(0,"hint");
    break;
  };
  RETURN;
}

