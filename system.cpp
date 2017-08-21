#include "stdafx.h"

#include <stdio.h>
#include "UI.h"



//#include "Objects.h"
#include "Dispatch.h"


#include "CSB.h"
#include "Data.h"

#if defined _LINUX || defined TARGET_OS_MAC
#include "LinuxFileName.inl" // class FILENAME
#endif

#ifdef _MSVC_INTEL
#include "WindowsFileName.inl" //class FILENAME
#endif // _LINUX

#ifdef _MSVC_CE2002ARM
#include "WinFileNameCE2002.inl"
#endif

void info(char *, unsigned int);
bool IsPlayFileOpen(void);

extern unsigned char *encipheredDataFile;
void RC4_prepare_key(unsigned char *key_data_ptr, 
                     i32 key_data_len);
void RC4_encipher(unsigned char *buffer_ptr, 
                  i32 buffer_len,
                  i32 position);


extern ui32 VBLCount;
extern i32 millisecondsPerVBL;



FILETABLE *GETFILETABLE(i16 f)
{
  return fileTable + f;
}

void DebugCast(i32 i, i32 line, char *file)
{
  char msg[1000];
  sprintf(msg,"A debug error has occurred.  It\n"
              "will probably do no harm.  Please\n"
              "let me know, especially if you can\n"
              "reproduce the problem and can ship me\n"
              "a save game and Record file.\n"
              "\n"
              "Value = 0x%08x\n"
              "Error at line %d\n"
              "in file %s\n"
              "\n"
              "prsteven@facstaff.wisc.edu\n"
              "\n"
              "Should we ignore this error?",
              i, line, file);
  if (UI_MessageBox(msg,NULL,MESSAGE_YESNO) == MESSAGE_IDNO) die(0);
}

#ifdef _DEBUGCAST
ui8 _ub(i32 i, i32 line, char *file)
{
  if ( (i&0xffffff00)!=0)
    DebugCast(i,line,file);
  return (ui8) i;
}

i8 _sb(i32 i, i32 line, char *file)
{
  if (!(((i&0xffffff80)==0)||((i&0xffffff80)==0xffffff80)))
    DebugCast(i,line,file);
  return (i8) i;
}

i16 _sw(i32 i, i32 line, char *file)
{
  if (!(((i&0xffff8000)==0)||((i&0xffff8000)==0xffff8000)))
    DebugCast(i,line,file);
  return (i16) i;
}

ui16 _uw(i32 i, i32 line, char *file)
{
  if ( (i&0xffff0000)!=0 )
    DebugCast(i,line,file);
  return (ui16) i;
}

#endif

FILETABLE fileTable[maxFilesOpen];

void FILETABLECleanup(void)
{
  int i;
  for (i=0; i<maxFilesOpen; i++)
  {
    fileTable[i].Cleanup();
  };
}


FILETABLE::FILETABLE(void)
{
  m_enciphered = false;
  m_file = NULL;
  m_fileName = NULL;
}

void FILETABLE::Cleanup(void)
{
  if (m_file != NULL) fclose(m_file);
  m_file = NULL;
  if (m_fileName != NULL) UI_free(m_fileName);
  m_fileName = NULL;
}


FILETABLE::~FILETABLE(void)
{
  Cleanup();
}

void FILETABLE::SetFile(FILE *f, const char *fileName)
{
  ASSERT((f==NULL) || (m_file == NULL),"f");
  m_file = f;
  if (m_fileName != NULL)
  {
    UI_free(m_fileName);
  };
  m_fileName = NULL;
  if (m_file != NULL)
  {
    if (fileName != NULL)
    {
      m_fileName = (char *)UI_malloc(strlen(fileName)+1, MALLOC107);
      strcpy(m_fileName, fileName);
    };
  }

}

ICOUNTS::ICOUNTS(void)
{
  for (i32 i=0; i<numInstrumentation; i++)
    m_InstrumentationCounts[i] = 0;
}

ICOUNTS InstrumentationCounts;

extern i16 globalPalette[];

i32 Smaller(i32 x, i32 y)
{
  return x<y ? x : y;
}

i32 Larger(i32 x, i32 y)
{
  return x>y ? x : y;
}

i32 TwoBitNibble(i32 packedValue, i32 nibbleNum)
{
  return (packedValue>>(2*nibbleNum))&3;
}

void NotImplemented(i32 n)
{
  char msg[1000];
  sprintf(msg,
             "You have encountered code at\n"
             "relative address 0x%06x that I\n"
             "thought was unused.  I guess I\n"
             "was wrong.  Please let me know\n"
             "about it and the circumstances in\n"
             "which it happened.\n"
             "prsteven@facstaff.wisc.edu", n);
  UI_MessageBox(msg, NULL, MESSAGE_OK);
  UI_Die(n);
}

void SorryNotImplemented(i32 n)
{
  char msg[1000];
  sprintf(msg,
             "You have encountered code at\n"
             "relative address 0x%06x that I\n"
             "have not implemented.\n"
             "It is unlikely that I will implement\n"
             "in the near future.  Maybe even in \n"
             "the far future.  I am more interested\n"
             "in improving the readability of the\n"
             "code needed to actually play the game.\n"
             "Perhaps you would like to implement it!\n"
             "Let me know....I have the Atari code you\n"
             "can look at.\n"
             "prsteven@facstaff.wisc.edu", n);

  UI_MessageBox(msg, NULL, MESSAGE_OK);
}

void HopefullyNotNeeded(i32)
{
  ASSERT(1,"notNeeded");
}

void PRINTLINE(const char *message)
{
  char msg[300];
  sprintf(msg,"PRINTLINE\n%s",message);
  UI_MessageBox(msg,NULL,MESSAGE_OK);
}

i32 RENAME(i32,const char *oldname, const char *newname)
{
  FILENAME file;
  return file.Rename(oldname,newname);
}

FILE *GETFILE(i32 f)
{
  if (f < 0) return NULL;
  return fileTable[f].GetFile();
}

char *GETFILENAME(i32 f)
{
  return fileTable[f].GetFileName();
}

i16 CREATE(const char *fname, const char *flags, bool dieIfError)
{
  i32 i;
  FILENAME file;
  for (i=1; i<maxFilesOpen; i++)
  {
    if (fileTable[i].GetFile()==NULL) break;
  };
  if (i==maxFilesOpen) die(141); // i=fileTable index
  fileTable[i].SetFile(file.Create(fname, flags), fname);
  if (fileTable[i].GetFile()==NULL)
  {
    if (dieIfError)
    {
      char msg[100];
      sprintf(msg,"Cannot create %s",fname);
      die(0,msg);
    }
    else
    {
      return -1;
    };
  };
  return sw(i);
}

i16 UNLINK(char *fname)
{
  FILENAME file;
  if (file.Unlink(fname)) return 0;
  return 1;
}

void SETENCIPHERED(i16 file, unsigned char *key, i32 keylen)
{
  unsigned char key5[5] = {112,32,34,1,6};
  RC4_prepare_key(key, keylen);
  RC4_encipher(key5,5,0); 
  RC4_prepare_key(key5,5);  
  {
    //char line[80];
    //sprintf(line,"%d %d %d %d %d",key5[0],key5[1],key5[2],key5[3],key5[4]);
    //UI_MessageBox(line,"a",0);
  }
  GETFILETABLE(file)->Enciphered(true);
}

i32  LSEEK(i32 offset,i32 file,i32 origin)
{
  i32 result = fseek(GETFILE((ui16)file), offset, origin);
  if (result!=0) return -1;
  return ftell(GETFILE((ui16)file));
}

char *GETS(char *buf, i32 max, i16 file)
{
  if (GETFILE((ui16)file) == NULL) return NULL;
  return fgets(buf, max, GETFILE((ui16)file));
}

i32 READ(i32 file, i32 num, ui8 *buf)
{
  i32 result;
  i32 position;
  if (GETFILE((ui16)file) == NULL) return -1;
  position = LSEEK(0, file, SEEK_CUR);
  result = fread(buf,1,num,GETFILE((ui16)file));
  if (GETFILETABLE((ui16)file)->Enciphered())
  {
    RC4_encipher((unsigned char *)buf,result,position);
  };
  return result;
}

i32 WRITE(i16 file, i32 num, ui8 *buf)
{
  i32 position, result;
  position = LSEEK(0, file, SEEK_CUR);
  if (GETFILETABLE(file)->Enciphered())
  {
    RC4_encipher((unsigned char *)buf, num, position);
  };
  result = fwrite(buf,1,num,GETFILE((ui16)file));
  if (GETFILETABLE(file)->Enciphered())
  {
    RC4_encipher((unsigned char *)buf, num, position);
  };
  return result;
}

i16 CLOSE(i32 file) {
  if (GETFILE((ui16)file)==NULL) return -1;
  fclose(GETFILE((ui16)file));
  GETFILETABLE((ui16)file)->SetFile(NULL, NULL);
  return 0;
}

char staticDTA[44];

pnt currentDTA = (pnt)staticDTA;

pnt GETDTA(void)
{
  return currentDTA;
}

void SETDTA(pnt P1)
{
  currentDTA = P1;
}

void xbtimer(i16 /*timer*/,i16 /*control*/ ,i16 /*data*/,void (* /*f*/)(void))
{ // TRAP 14 #31
  HopefullyNotNeeded(0x0612);
}

i16 Timer(pnt /*p*/, i16 /*d*/)
{
  HopefullyNotNeeded(0x71ca);
  return 0;
}

void setpalette(PALETTE *newPalette)
{
  i16 *pPalette;
  pPalette = (i16 *)globalPalette;
  for (i32 i=0; i<16; i++)
  {
    pPalette[i] = newPalette->color[i];
    ASSERT(pPalette[i] <= 0x777,"palette");
  };
}

i16 giaccess(i32 /*data*/,i32/* Register*/)
{ // TRAP 14 #28
  HopefullyNotNeeded(0x8c7b);
  return 0;
}

i16 drvmap(void) //TRAP #13
{
  return 3;
}

ui8 *dosound(ui8 * /*soungPgm*/)
{ // TRAP 14 #32
  HopefullyNotNeeded(0x22d9);
  //PlaySound("ugh.wav",NULL,SND_ASYNC);
  return NULL;
}

void jenabint(i16 /*P1*/)
{
  HopefullyNotNeeded(0x2345);
}


//i32 saveIndex = 0;
//i32 rgsv[200];

void Invalidate(void)
{
  Instrumentation(icntInvalidate);
  UI_Invalidate();
};


// TAG002ef0 TAG002ef4
void MouseInterrupt(aReg A0)
{
  dReg D0;
  //SaveRegs(0x8008);
  // A4 = (pnt)&data000b2c;//
  D0B = (UI8)(*A0 & 3);
  if (D0W != d.MouseSwitches)
  {
    OnMouseSwitchAction(0/*&data000b30*/); //
  };
  if (!IsPlayFileOpen() && (d.PressingEyeOrMouth == 0))
  {
    D0W = *(A0+1);
    if (D0W != 0)
    {
      D0W = sw(D0W + d.NewMouseX);
      if (D0W < 0) D0W = 0;
      if (D0W > 319) D0W = 319;
      d.NewMouseX = D0W;
    };
      D0W = *(A0+2);
    if (D0W != 0)
    {
      D0W = sw(D0W + d.NewMouseY);
      if (D0W < 0) D0W = 0;
      if (D0W > 199) D0W = 199;
      d.NewMouseY = D0W;
    };
    if(d.Word23134 != 0) d.Word23130 = 1;
  };
  //RestoreRegs(0x1001);
}

//   TAG002f70
void OnMouseSwitchAction(i32 buttons, i32 x, i32 y)
{
  dReg D1, D2;
  // Expects something in D0; Bottom two bits of mouse data.
  // we replaced it with a parameter named buttons
  ASSERT(d.OnMouseSwitchActionLock == 0,"mouse");
  // We cannot have two interrupts at once because we
  // never have even one interrupt.  CSBwin is single
  // thread!  But we copy the Atari code rather blindly.
  if (d.OnMouseSwitchActionLock != 0) return;
  // Prevent processing 2 interrupts at once.
  d.OnMouseSwitchActionLock = 1;
  if ((d.MouseSwitches==0)||(buttons==0))
  {
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile), " Set MouseSwitches to 0x%02x in OnMouseSwitchAction\n", buttons);
    };
    d.MouseSwitches = sw(buttons);
    if(buttons != 0)
    {
      if (x==-1)
      {
        D1W = d.NewMouseX;
        D2W = d.NewMouseY; // the other way around
      }
      else
      {
        d.NewMouseX = D1W = sw(x);
        d.NewMouseY = D2W = sw(y);
      };
      OnMouseClick(D1W,D2W,buttons);
    }
    else
    {
      //The following actions were originally performed at the instant
      // the mouse button was released.  But the button pressing was 
      // queued (see above).  So the release could be processed before the
      // press!!!  And it did not work well in Record/Playback.
      // So I have modified things so that the releases are put into the
      // queue right along with the presses.  The code (deleted below) 
      // will be executed when the release is removed from the queue.
      //
      //One thing I did was make sure that whenever we put a press into
      // the queue, we make sure that there is room for a release.  We don't
      // want an unmatched press.  So 'OnMouseClick' can put at most
      // three entries in the queue and 'OnMouseUnClick' can put four.
      //
//      if (d.PressingEye != 0)
//      {
//        d.PressingEyeOrMouth = 0;
//        d.QuitPressingEye = 1;
//      }
//      else
//      {
//        if (d.PressingMouth != 0)
//        {
//          d.PressingEyeOrMouth = 0;
//          d.QuitPressingMouth = 1;
//        };
//      };
    };
  };
  d.OnMouseSwitchActionLock = 0;
}


i16 OPEN(const char *name, const char *ref)
{
  i32 i;
  FILENAME fileName;
  FILE *f;
  for (i=1; i<maxFilesOpen; i++)
  {
    if (GETFILE((ui16)i)==NULL) break;
  };
  if (i==maxFilesOpen) die(141); // i=fileTable index
  f = fileName.Open(name, ref);
  GETFILETABLE((ui16)i)->SetFile(f, fileName.m_fName);
  if (GETFILE((ui16)i)==NULL)
  {
    char msg[100];
    sprintf(msg,"Cannot open %s",name);
//    die(0,msg);
    return -1;
  };
  GETFILETABLE((ui16)i)->Enciphered(false);
  return sw(i);
}


ui32 missedVBL = 0;

i32 dist[100];

ui32 CheckVBLCount = 0;

extern void LIN_Invalidate();
void checkVBL(void)
{
  bool forceVBL = false;
  static ui64 prevTime(0), curTime(0);
  static ui64 goal; // Number of vbls we should have processed.
  static ui64 actual;
  ui32 increment;
  ui64 delta;
  i32 pointX, pointY;
  CheckVBLCount++;
  if ((NoSpeedLimit!=0) || NoClock)
  {
    forceVBL = true;
  }
  else
  {
    curTime = UI_GetSystemTime(); //Units of milliseconds
    delta = curTime - prevTime;
    if (delta < 1000)
    {
      increment = (i32)((delta)/millisecondsPerVBL);
      if (increment<100)
      {
        dist[increment]++;
      };
      if (goal>1000)
      {
        ASSERT(1,"vbl");
      };
      prevTime += millisecondsPerVBL*increment;
      goal += increment;
      if (goal > actual)
      {
        if (goal-actual < 10)
        {
          actual++;
          forceVBL = true;
        }
        else
        {
          actual = goal-4;
          forceVBL = true;
          missedVBL++;
        };
      };
    }
    else
    {
      prevTime = curTime;
      actual = goal;
      forceVBL=true;
    };
  };
  if ((NoSpeedLimit==0)||((VBLCount%200)==0))
  {
    UI_GetCursorPos(&pointX,&pointY);
    Instrumentation(icntGetCursorPos);
    if (   (pointX >= 0)
        && (pointX < 320)
        && (pointY >= 0)
        && (pointY < 200))
    {
      if (!IsPlayFileOpen())
      {
        d.NewMouseX = sw(pointX);
        d.NewMouseY = sw(pointY);
      };
      if (!IsPlayFileOpen())
      {
        d.CurMouseX = d.NewMouseX;
        d.CurMouseY = d.NewMouseY;
      };
      if (d.Word23134 != 0)
      {
        d.Word23130 = 1;
      };
    };
  };
  if (forceVBL)
  {
    if ((NoSpeedLimit==0) || ((VBLCount%20)==0))
    {
      if ((VBLCount % 2) == 0)UI_Invalidate();
    };
    VBLCount++;
    UI_PushMessage(UIM_VBL);
    if (verticalIntEnabled)
    {
      Instrumentation(icntVBL);
      vblInterrupt();
      UI_Invalidate();
#ifdef _LINUX
	LIN_Invalidate();
#endif
    };
  };
}

RESTARTABLE _wvbl(const CSB_UI_MESSAGE *pMsg)
{ // Wait for vertical blank.
  // This is not reliable.  VBL's come in bursts of
  // three on the PC because the minimum SetTimer delay
  // is about 50 milliseconds.
  static ui32 oldCount;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
  oldCount = VBLCount;
#ifdef _DEBUG
  checkMemory();
#endif
  while (oldCount == VBLCount)
  {
    WAITFORMESSAGE(_1_); //a UIM_VBL will do.
    switch (pMsg->type)
    {
    case UIM_VBL: break;
    };
  };
  RETURN;
}

void ClearViewport(void)
{
  memset(d.pViewportBMP,0,15232);
}

void RevealViewport(void)
{
  static i16 changer;
  aReg A0, A1;
  changer++;
  A0 = (aReg)d.pViewportBMP;
  A1 = (aReg)d.LogicalScreenBase+5280;
  for (i32 i=0; i <136; i++)
  {
    memmove(A1,A0,112);
    A0 += 112;
    A1 += 160;
  };
  Invalidate();
  *(i16 *)d.LogicalScreenBase=changer;
  NotImplemented(0x99965234);//pumper();
}

struct STATEENTRY
{
  CODESTATE state;
  i32 p1;
  i32 p2;
  i32 p3;
  i32 p4;
  i32 p5;
  i32 p6;
  i32 p7;
  i32 p8;
  i32 p9;
};

#define MaxState 20
STATEENTRY stateStack[MaxState];
i32 numState = 0;

/*
void DispatchCSB(GAMESTATE)
{NotImplemented();}

void _CALL0(GAMESTATE )
{NotImplemented();}

void _CALL1(GAMESTATE ,int)
{NotImplemented();}

void _CALL2(GAMESTATE ,int,int)
{NotImplemented();}

void _CALL3(GAMESTATE ,int,int,int)
{NotImplemented();}

void _CALL4(GAMESTATE ,int,int,int,int)
{NotImplemented();}

void _CALL5(GAMESTATE ,int,int,int,int,int)
{NotImplemented();}

void _CALL6(GAMESTATE ,int,int,int,int,int,int)
{NotImplemented();}

void _CALL7(GAMESTATE ,int,int,int,int,int,int,int)
{NotImplemented();}

void _CALL8(GAMESTATE ,int,int,int,int,int,int,int,int)
{NotImplemented();}

void _CALL9(GAMESTATE ,int,int,int,int,int,int,int,int,int)
{NotImplemented();}
*/
void _CALL0(CODESTATE newState)
{
  ASSERT(numState < MaxState,"maxState");
  stateStack[numState].state = newState;
  numState++;
  UI_PushMessage(UIM_INITIALFUNCTIONENTRY);
  return ;
}

void _CALL1(CODESTATE newState,
            i32 p1)

{
  ASSERT(numState < MaxState,"maxState");
  stateStack[numState].state = newState;
  stateStack[numState].p1 = p1;
  numState++;
  UI_PushMessage(UIM_INITIALFUNCTIONENTRY);
  return ;
}

void _CALL2(CODESTATE newState,
            i32 p1,
            i32 p2)

{
  ASSERT(numState < MaxState,"maxState");
  stateStack[numState].state = newState;
  stateStack[numState].p1 = p1;
  stateStack[numState].p2 = p2;
  numState++;
  UI_PushMessage(UIM_INITIALFUNCTIONENTRY);
  return ;
}

void _CALL3(CODESTATE newState,
            i32 p1,
            i32 p2,
            i32 p3)

{
  ASSERT(numState < MaxState,"maxState");
  stateStack[numState].state = newState;
  stateStack[numState].p1 = p1;
  stateStack[numState].p2 = p2;
  stateStack[numState].p3 = p3;
  numState++;
  UI_PushMessage(UIM_INITIALFUNCTIONENTRY);
  return ;
}

void _CALL4(CODESTATE newState,
            i32 p1,
            i32 p2,
            i32 p3,
            i32 p4)

{
  ASSERT(numState < MaxState,"maxState");
  stateStack[numState].state = newState;
  stateStack[numState].p1 = p1;
  stateStack[numState].p2 = p2;
  stateStack[numState].p3 = p3;
  stateStack[numState].p4 = p4;
  numState++;
  UI_PushMessage(UIM_INITIALFUNCTIONENTRY);
  return ;
}

void _CALL5(CODESTATE newState,
            i32 p1,
            i32 p2,
            i32 p3,
            i32 p4,
            i32 p5)

{
  ASSERT(numState < MaxState,"maxState");
  stateStack[numState].state = newState;
  stateStack[numState].p1 = p1;
  stateStack[numState].p2 = p2;
  stateStack[numState].p3 = p3;
  stateStack[numState].p4 = p4;
  stateStack[numState].p5 = p5;
  numState++;
  UI_PushMessage(UIM_INITIALFUNCTIONENTRY);
  return ;
}

void _CALL6(CODESTATE newState,
            i32 p1,
            i32 p2,
            i32 p3,
            i32 p4,
            i32 p5,
            i32 p6)

{
  ASSERT(numState < MaxState,"maxState");
  stateStack[numState].state = newState;
  stateStack[numState].p1 = p1;
  stateStack[numState].p2 = p2;
  stateStack[numState].p3 = p3;
  stateStack[numState].p4 = p4;
  stateStack[numState].p5 = p5;
  stateStack[numState].p6 = p6;
  numState++;
  UI_PushMessage(UIM_INITIALFUNCTIONENTRY);
  return ;
}

void _CALL8(CODESTATE newState,
            i32 p1,
            i32 p2,
            i32 p3,
            i32 p4,
            i32 p5,
            i32 p6,
            i32 p7,
            i32 p8)

{
  ASSERT(numState < MaxState,"maxState");
  stateStack[numState].state = newState;
  stateStack[numState].p1 = p1;
  stateStack[numState].p2 = p2;
  stateStack[numState].p3 = p3;
  stateStack[numState].p4 = p4;
  stateStack[numState].p5 = p5;
  stateStack[numState].p6 = p6;
  stateStack[numState].p7 = p7;
  stateStack[numState].p8 = p8;
  numState++;
  UI_PushMessage(UIM_INITIALFUNCTIONENTRY);
  return ;
}

void _CALL9(CODESTATE newState,
            i32 p1,
            i32 p2,
            i32 p3,
            i32 p4,
            i32 p5,
            i32 p6,
            i32 p7,
            i32 p8,
            i32 p9)

{
  ASSERT(numState < MaxState,"maxState");
  stateStack[numState].state = newState;
  stateStack[numState].p1 = p1;
  stateStack[numState].p2 = p2;
  stateStack[numState].p3 = p3;
  stateStack[numState].p4 = p4;
  stateStack[numState].p5 = p5;
  stateStack[numState].p6 = p6;
  stateStack[numState].p7 = p7;
  stateStack[numState].p8 = p8;
  stateStack[numState].p9 = p9;
  numState++;
  UI_PushMessage(UIM_INITIALFUNCTIONENTRY);
  return ;
}

i32 intResult;
i16 i16Result;
RN  RNResult;
bool boolResult;
pnt pntResult;


class AT
{
public:
  i32 activeTable[st_Last-st_First+1];
  AT(void);
};

AT::AT(void)
{
  memset(activeTable,0,sizeof (AT));
}

AT at;

void Deactivate(i32 state)
{
  if (at.activeTable[state] == 0)
  {
    ASSERT(0,"state");
  };
  at.activeTable[state] = 0;
}

void _RETURN(void)
{
  ASSERT(numState != 0,"numState");
  numState--;
#ifdef _DEBUG
  Deactivate(stateStack[numState].state);
#endif
  UI_PushMessage(UIM_RETURN);
}

void _RETURN_int(i32 v)
{
  intResult = v;
  ASSERT(numState != 0,"numSTate");
  numState--;
#ifdef _DEBUG
  Deactivate(stateStack[numState].state);
#endif
  UI_PushMessage(UIM_RETURN);
}

void _RETURN_pnt(pnt v)
{
  pntResult = v;
  ASSERT(numState != 0,"numSTate");
  numState--;
#ifdef _DEBUG
  Deactivate(stateStack[numState].state);
#endif
  UI_PushMessage(UIM_RETURN);
}

void _RETURN_i16(i16 v)
{
  i16Result = v;
  ASSERT(numState != 0,"numSTate");
  numState--;
#ifdef _DEBUG
  Deactivate(stateStack[numState].state);
#endif
  UI_PushMessage(UIM_RETURN);
}

void _RETURN_RN(RN v)
{
  RNResult = v;
  ASSERT(numState != 0,"numSTate");
  numState--;
#ifdef _DEBUG
  Deactivate(stateStack[numState].state);
#endif
  UI_PushMessage(UIM_RETURN);
}

void _RETURN_BOOL(bool v)
{
  boolResult = v;
  ASSERT(numState != 0,"numState");
  numState--;
#ifdef _DEBUG
  Deactivate(stateStack[numState].state);
#endif
  UI_PushMessage(UIM_RETURN);
}

void DispatchCSB(CODESTATE g)
{
  ASSERT(numState < MaxState,"maxState");
  stateStack[numState].state = g;
  numState++;
  UI_PushMessage(UIM_INITIALFUNCTIONENTRY);
  return ;
}

extern int eventNum;

void DispatchCSB(CSB_UI_MESSAGE *pMsg)
{
  DOESNOTEXIST *result;
#ifdef _DEBUG //Check for recursion
  if (pMsg->type == UIM_INITIALFUNCTIONENTRY)
  {
    if (at.activeTable[stateStack[numState-1].state] != 0)
    {
      ASSERT(0,"dispatch");
    };
    at.activeTable[stateStack[numState-1].state] = 1;
  };
#endif
  ASSERT(numState!=0,"numState");
  EC
  switch (stateStack[numState-1].state)
  {
  case st_StartCSB:
    result = &_StartCSB(pMsg);
    break;
  case st_MainLoop:
    result = &_MainLoop(pMsg);
    break;
  case st_ReadEntireGame:
    result = &_ReadEntireGame();
    break;
  case st_GameSetup:
    result = &_GameSetup(stateStack[numState-1].p1);
    break;
  case st_ShowPrisonDoor:
    result = &_ShowPrisonDoor();
    break;
  case st_DisplayChaosStrikesBack:
    result = &_DisplayChaosStrikesBack();
    break;
  case st_FadeToPalette:
    result = &_FadeToPalette((PALETTE *)stateStack[numState-1].p1);
    break;
  case st_wvbl:
    result = &_wvbl(pMsg);
    break;
  case st_VBLDelay:
    result = &_VBLDelay(stateStack[numState-1].p1);
    break;
  case st_SelectSaveGame:
    result = &_SelectSaveGame(stateStack[numState-1].p1, stateStack[numState-1].p2, stateStack[numState-1].p3);
    break;
  case st_WaitForMenuSelect:
    result = &_WaitForMenuSelect(
      stateStack[numState-1].p1,
      stateStack[numState-1].p2,
      stateStack[numState-1].p3,
      stateStack[numState-1].p4 );
    break;
  case st_TAG01f746:
    result = &_TAG01f746();
    break;
  case st_TAG01f5ea:
    result = &_TAG01f5ea();
    break;
  case st_HandleMouseEvents:
    result = &_HandleMouseEvents(stateStack[numState-1].p1);
    break;
  case st_OpenPrisonDoors:
    result = &_OpenPrisonDoors();
    break;
//  case st_InsertDisk:
//    result = &_InsertDisk(
//      stateStack[numState-1].p1,
//      stateStack[numState-1].p2);
//    break;
  case st_TAG021028:
    result = &_TAG021028();
    break;
//  case st_RemoveTimedOutText:
//    result = &_RemoveTimedOutText();
//    break;
//  case st_LoadPartyLevel:
//    result = &_LoadPartyLevel(
//      stateStack[numState-1].p1);
//    break;
//  case st_ReadGraphicsForLevel:
//    result = &_ReadGraphicsForLevel();
//    break;
//  case st_LoadNeededGraphics:
//    result = &_LoadNeededGraphics(
//      (i16 *)stateStack[numState-1].p1,
//      stateStack[numState-1].p2);
//    break;
//  case st_ProcessTimers:
//    result = &_ProcessTimers();
//    break;
  case st_TAG01b29a:
    result = &_TAG01b29a();
    break;
  case st_FlashButton:
    result = &_FlashButton(
      stateStack[numState-1].p1,
      stateStack[numState-1].p2,
      stateStack[numState-1].p3,
      stateStack[numState-1].p4);
    break;
  case st_TurnParty:
    result = &_TurnParty(
      stateStack[numState-1].p1);
    break;
  case st_TAG01a6ea:
    result = &_TAG01a6ea(
      stateStack[numState-1].p1,
      stateStack[numState-1].p2);
    break;
  case st_ResurrectReincarnateCancel:
    result = &_ResurrectReincarnateCancel(
      stateStack[numState-1].p1);
    break;
  case st_HandleMagicClick:
    result = &_HandleMagicClick(
      stateStack[numState-1].p1,
      stateStack[numState-1].p2);
    break;
  case st_MagicSelection:
    result = &_MagicSelection(
      stateStack[numState-1].p1);
    break;
  case st_ShowCredits:
    result = &_ShowCredits(
      stateStack[numState-1].p1);
    break;
  case st_DisplayDiskMenu:
    result = &_DisplayDiskMenu();
    break;
  case st_DoMenu:
    result = &_DoMenu(
      (char *)stateStack[numState-1].p1,
      (char *)stateStack[numState-1].p2,
      (char *)stateStack[numState-1].p3,
      (char *)stateStack[numState-1].p4,
      (char *)stateStack[numState-1].p5,
      (char *)stateStack[numState-1].p6,
      stateStack[numState-1].p7,
      stateStack[numState-1].p8,
      stateStack[numState-1].p9);
    break;
  case st_ClickOnEye:
    result = &_ClickOnEye();
    break;
  case st_FeedCharacter:
    result = &_FeedCharacter();
    break;
  case st_ReIncarnate:
    result = &_ReIncarnate(
      (CHARDESC *)stateStack[numState-1].p1);
    break;
//  case st_PrintLines:
//    result = &_PrintLines(
//      stateStack[numState-1].p1,
//      (char *)stateStack[numState-1].p2);
//    break;
//  case st_CreateNewTextRow:
//    result = &_CreateNewTextRow();
//    break;
  case st_TAG019fac:
    result = &_TAG019fac(
      stateStack[numState-1].p1,
      stateStack[numState-1].p2);
    break;
  case st_ExecuteAttack:
    result = &_ExecuteAttack(
      stateStack[numState-1].p1);
    break;
  case st_Attack:
    result = &_Attack(
      stateStack[numState-1].p1,
      (ATTACKTYPE)stateStack[numState-1].p2);
    break;
  case st_AskWhatToDo:
    result = &_AskWhatToDo();
    break;
  case st_TAG01f928:
    result = &_TAG01f928();
    break;
  case st_StartChaos:
    result = &_StartChaos(pMsg);
    break;
  case st_Fusion:
    result = &_Fusion(
      stateStack[numState-1].p1,
      stateStack[numState-1].p2);
    break;
  case st_ReadSavedGame:
    result = &_ReadSavedGame(
      stateStack[numState-1].p1);
    break;
  case st_TAG00179c:
    result = &_TAG00179c(
      stateStack[numState-1].p1);
    break;
  case st_UtilityDialogBox:
    result = &_UtilityDialogBox(
      (char *)stateStack[numState-1].p1,
      stateStack[numState-1].p2,
      (char *)stateStack[numState-1].p3,
      (struct S12406 *)stateStack[numState-1].p4);
    break;
  case st_TAG00301c:
    result = &_TAG00301c(
      (pnt)stateStack[numState-1].p1);
    break;
  case st_InvertButton:
    result = &_InvertButton(
      (struct wordRectPos *)stateStack[numState-1].p1);
    break;
  case st_TAG004518:
    result = &_TAG004518();
    break;
  case st_FusionSequence:
    result = &_FusionSequence();
    break;
  case st_ProcessTimersViewportAndSound:
    result = &_ProcessTimersViewportAndSound();
    break;
  case st_TAG004430:
    result = &_TAG004430(
      stateStack[numState-1].p1);
    break;
  case st_DrawCharacterEditorScreen:
    result = &_DrawCharacterEditorScreen();
    break;
  case st_DrawCharacterDetails:
    result =  &_DrawCharacterDetails(
      stateStack[numState-1].p1,
      stateStack[numState-1].p2);
    break;
  case st_TAG00260e:
    result = &_TAG00260e();
    break;
  case st_CheckChampionNames:
    result = &_CheckChampionNames();
    break;
  case st_TAG0026c4:
    result = &_TAG0026c4(
      (pnt)stateStack[numState-1].p1);
    break;
  case st_TAG000fc4:
    result = &_TAG000fc4(
      stateStack[numState-1].p1);
    break;
  case st_TAG000ede:
    result = &_TAG000ede(
      stateStack[numState-1].p1,
      (char *)stateStack[numState-1].p2);
    break;
  case st_FlashAttackDamage:
    result = &_FlashAttackDamage(
      stateStack[numState-1].p1);
    break;
//  case st_ProcessTT_STONEROOM:
//    result = &_ProcessTT_STONEROOM(
//      (TIMER *)stateStack[numState-1].p1,
//      stateStack[numState-1].p2);
//    break;
  case st_StartHint:
    result = &_StartHint(pMsg);
    break;
  case st_DisplayText:
    result = &_DisplayText(
      sw(stateStack[numState-1].p1),
      (struct TEXT *)stateStack[numState-1].p2);
    break;
  case st_TAG005a1e_xxx:
    result = &_TAG005a1e_xxx(
      sw(stateStack[numState-1].p1),
      sw(stateStack[numState-1].p2),
      stateStack[numState-1].p3);
    break;
  case st_FadePalette:
    result = &_FadePalette(
      (struct PALETTE *)stateStack[numState-1].p1,
      (i16 *)stateStack[numState-1].p2,
      sw(stateStack[numState-1].p3),
      sw(stateStack[numState-1].p4),
      sw(stateStack[numState-1].p5));
    break;
  case st_TAG0051c2_1:
    result = &_TAG0051c2_1(
      sw(stateStack[numState-1].p1));
    break;
  case st_TAG006c7e_xxx:
    result = &_TAG006c7e_xxx(
      sw(stateStack[numState-1].p1),
      sw(stateStack[numState-1].p2),
      stateStack[numState-1].p3,
      stateStack[numState-1].p4,
      (pnt)stateStack[numState-1].p5);
    break;
  case st_TAG006c7e_2:
    result = &_TAG006c7e_2(
      sw(stateStack[numState-1].p1));
    break;
  case st_TAG006c7e_32:
    result = &_TAG006c7e_32(
      sw(stateStack[numState-1].p1),
      (pnt)stateStack[numState-1].p2);
    break;
  case st_TAG0076a0_12:
    result = &_TAG0076a0_12(
      sw(stateStack[numState-1].p1),
      (pnt)stateStack[numState-1].p2,
      (struct TEXT *)stateStack[numState-1].p3);
    break;
  case st_TAG006c7e_9:
    result = &_TAG006c7e_9(
      sw(stateStack[numState-1].p1),
      stateStack[numState-1].p2,
      stateStack[numState-1].p3,
      (struct HCTI *)stateStack[numState-1].p4);
    break;
  case st_TAG00799a_6:
    result = &_TAG00799a_6(
      sw(stateStack[numState-1].p1));
    break;
  case st_TAG00799a_9:
    result = &_TAG00799a_9(
      sw(stateStack[numState-1].p1));
    break;
  case st_TAG00799a_12:
    result = &_TAG00799a_12(
      sw(stateStack[numState-1].p1),
      stateStack[numState-1].p2,
      stateStack[numState-1].p3);
    break;
  case st_TAG007fdc_7:
    result = &_TAG007fdc_7(
      sw(stateStack[numState-1].p1),
      stateStack[numState-1].p2,
      stateStack[numState-1].p3,
      stateStack[numState-1].p4);
    break;
  case st_TAG007fdc_36:
    result = &_TAG007fdc_36(
      sw(stateStack[numState-1].p1),
      stateStack[numState-1].p2,
      stateStack[numState-1].p3);
    break;
  case st_TAG008c40_5:
    result = &_TAG008c40_5(
      sw(stateStack[numState-1].p1),
      stateStack[numState-1].p2);
    break;
  case st_TAG008c40_6:
    result = &_TAG008c40_6(
      sw(stateStack[numState-1].p1),
      stateStack[numState-1].p2);
    break;
  case st_TAG008c40_8:
    result = &_TAG008c40_8(
      sw(stateStack[numState-1].p1),
      sw(stateStack[numState-1].p2));
    break;
  case st_TAG0051c2_31:
    result = &_TAG0051c2_31(
      sw(stateStack[numState-1].p1));
    break;
  case st_TAG004e4c_xxx:
    result = &_TAG004e4c_xxx(
      sw(stateStack[numState-1].p1),
      sw(stateStack[numState-1].p2),
      stateStack[numState-1].p3);
    break;
  case st_TAG004e4c_6:
    result = &_TAG004e4c_6(
      sw(stateStack[numState-1].p1),
      (struct TEXT *)stateStack[numState-1].p2);
    break;
  case st_TAG004e4c_8:
    result = &_TAG004e4c_8(
      sw(stateStack[numState-1].p1),
      (struct TEXT *)stateStack[numState-1].p2);
    break;
  case st_PaletteFade:
    result = &_PaletteFade(
      sw(stateStack[numState-1].p1),
      (struct PALETTEPKT *)stateStack[numState-1].p2);
    break;
  case st_TAG0051c2_2:
    result = &_TAG0051c2_2(
      stateStack[numState-1].p1);
    break;
  case st_TAG005d2a:
    result = &_TAG005d2a(
      stateStack[numState-1].p1,
      stateStack[numState-1].p2);
    break;
  case st_TAG0089b0:
    result = &_TAG0089b0(
      sw(stateStack[numState-1].p1),
      (struct S20 *)stateStack[numState-1].p2,
      (struct T12 *)stateStack[numState-1].p3);
    break;
  case st_TAG005a1e_5:
    result = &_TAG005a1e_5(
      stateStack[numState-1].p1,
      stateStack[numState-1].p2);
    break;
  case st_TAG009410:
    result = &_TAG009410(
      (pnt)stateStack[numState-1].p1,
      stateStack[numState-1].p2);
    break;
  case st_TAG001dde:
    result = &_TAG001dde(
      (struct S6 *)stateStack[numState-1].p1);
    break;
  default: result = NULL; ASSERT(0,"dispatch");
  };
#ifdef _DEBUG
  if (result != &DoesNotExist)
  {
    ASSERT(0,"dispatch");
  };
#endif
}

#define maxloaditems 100
void DebugLoad(void *p, i32 a, i32 b, RN obj)
{
  static FILE *f=NULL;
  static i32 additions[maxloaditems];
  static i32 timeadd[maxloaditems];
  static RN objadd[maxloaditems];
  i32 newLoad, oldLoad, index;
  if (f == NULL)
  {
    f = fopen("debugload.txt","w");
    for (index=0; index<maxloaditems; index++) additions[index]=0;
  };
  index = ((CHARDESC *)p) - d.CH16482;
  if (index != 0) return;
  if ((obj != RNeof) && (obj!=RNnul))
  {
    RN cont;
    i32 w;
    DB9 *pChest;
    if (obj.dbType() == dbCHEST)
    {
      DEBUGLOAD(p,50,b,RNeof);
      pChest = GetRecordAddressDB9(obj);
      for (cont = pChest->contents();
           cont != RNeof;
           cont = GetDBRecordLink(cont))
      {
        w = GetObjectWeight(cont);
        DEBUGLOAD(p, w, b, cont);
      };
      return;
    };
  };
  oldLoad = d.CH16482[index].load;
  if (b == -1) newLoad = oldLoad - a;
  else if (b == 0) newLoad = a;
  else newLoad = oldLoad + a;
  fprintf(f,"%8d %2d %2d %5d  %4d --> %4d\n",
              d.Time,
              index,
              b,
              a,
              oldLoad,
              newLoad);
  if (b == 0) return;
  if (a == 0) return;
  if (b == 1)
  {
    for (index=0; index<maxloaditems; index++)
    {
      if (additions[index] == 0)
      {
        additions[index] = a;
        objadd[index] = obj;
        timeadd[index] = d.Time;
        return;
      };
    };
    die(0x33d1);
  };
  for (index=0; index<maxloaditems; index++)
  {
    if ( (additions[index] == a) && (objadd[index] == obj))
    {
      additions[index] = 0;
      return;
    };
  };
  die(0x4d4b);
}

