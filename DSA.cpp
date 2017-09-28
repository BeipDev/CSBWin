#include "stdafx.h"

#include <stdio.h>

#include "CSBTypes.h"

#include "UI.h"
//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

bool IsPlayFileOpen();
void WarnIllegalTimer(i32 srcX, i32 srcY, i32 dstX, i32 dstY);
void SelectOverlay(i32, i32, i32, i32, i32);
void SetOverlayPalette(i32 onum, i32 p1, i32 p2, i32 density);
void PlayCustomSound(i32 soundNum, i32 volume, i32 flags);
void info(char *, unsigned int);
void EnsureItem16Available();
void ModifyDescription(i32 locrInt, i32 index, i32 color);
i32 MoveObject (i32 srcType, i32 srcObjMsk, i32 srcPosMsk, i32 srcLoc, i32 srcDepth,
                i32 dstType, i32 dstObjMsk, i32 dstPosMsk, i32 dstLoc, i32 dstDepth);
void SetCursed(RN obj, bool curse);

void DeleteMonsterInGroup(const LOCATIONREL& locr, ui32 indx);
void InsertMonsterInGroup(const LOCATIONREL& locr, ui32 posMask);
void AddToSkill(i32 charnum, i32 skillnum, i32 experience);
ui32 SwapCharacter(i32 index, i32 fingerPrint);
void SetGlobalText(const char *text, ui32 index);
extern ui8 monsterMoveInhibit[4];
extern i32 numGlobalVariables;
extern ui32 *globalVariables;
bool dsaActive = false;
extern i32 xGraphicJitter;
extern i32 yGraphicJitter;
extern i32 xOverlayJitter;
extern i32 yOverlayJitter;
extern bool jitterChanged;
extern i32 timerTypeModifier[3];
i32 operationCount = -1;
i32 prevTime = 0;
bool excessiveWarning = false;
ui32 initialDSAparameters[101] = {0};
ui32 *pDSAparameters = initialDSAparameters;
ui8  bitCounts[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};

#ifdef _DEBUG
class INSTRUMENTATION
{
  i32 m_DSACounts[256];
  i32 m_DSAOperations[256];
public:
  INSTRUMENTATION()
  {
    memset(m_DSACounts,0,256*4);
    memset(m_DSAOperations,0,256*4);
  };
  void AddToDSACount(i32 dsaIndex, i32 operations)
  {
    m_DSACounts[dsaIndex]++;
    m_DSAOperations[dsaIndex] += operations;
  };
  void Dump(FILE *f);
};

void INSTRUMENTATION::Dump(FILE *f)
{
  fprintf(f,"DSA Instrumentation\n");
  int absDSAIndex;
  DSA *pMaster;
  for (absDSAIndex=0; absDSAIndex<256; absDSAIndex++)
  {
    pMaster = DSAIndex.GetDSA(absDSAIndex);
    if (pMaster == NULL) continue;
    fprintf(f," %03d %8d %8d %-80.80s\n",
                absDSAIndex,
                m_DSACounts[absDSAIndex],
                m_DSAOperations[absDSAIndex],
                pMaster->Description());
    m_DSACounts[absDSAIndex] = 0,
    m_DSAOperations[absDSAIndex] = 0;
  };
}

INSTRUMENTATION instrumentation;
#endif

void DSAInstrumentation_Dump()
{
#ifdef _DEBUG
  if (DSAIndex.AnyTraceActive())
  {
    FILE *f;
    f = fopen("DSAInfo.txt","w");
    if (f != NULL)
    {
      instrumentation.Dump(f);
      fclose(f);
    };
  };
#endif
}


class STACK
{
  i32 stack[100];
  ui32 stacklen;
public:
  STACK();
  ~STACK();
  void Clear(){stacklen=0;};
  void push(LOCATIONREL locr);
  void push(i32 i);
  i32 pop();
  void pick(ui32 i);
  void poke(ui32 i);
  void roll(ui32 i);
  void minusRoll(ui32 i);
  char *underflowMsg();
  void dump();
};

struct DSADBANK
{
  i32 numText;
  char *textStrings[10];
  i32 forceState;
  i32 mostRecentInterestingObjectID;
  STACK stack;
  DSAVARS *pDsaVars;
  RN  m_objectDeleteList;
  //i32 tempVar[100];
  //i8  tempVarValue[100];
  bool filter;
  bool tracing;
  i32  IndirectActionDelay;
  void SetText(const char *text, i32 len, i32 index);
  const char *GetText(ui32 index);
  void NoValue(i32 i);
  i32  Var(i32 i){if(pDsaVars->definedFlags[i]!=DVT_WellDefined)NoValue(i);return pDsaVars->values[i];};
  void Var(i32 i, i32 value){pDsaVars->values[i]=value;pDsaVars->definedFlags[i]=(ui8)DVT_WellDefined;};
  void SetVarBit(i32 i, i32 bitmask){if(pDsaVars->definedFlags[i]!=DVT_WellDefined)NoValue(i);pDsaVars->values[i]|=bitmask;};
  void SetVarState(i32 i, DSAVAR_TYPE dvt){pDsaVars->definedFlags[i]=(ui8)dvt;};
  bool ConsumateDeletes(RN RNslave);
  void DeleteObject(RN obj);
  DSAVAR_TYPE GetVarState(i32 i){return (DSAVAR_TYPE)pDsaVars->definedFlags[i];};
  DSADBANK(){numText = 0;mostRecentInterestingObjectID=-1;m_objectDeleteList=RNeof;};
  ~DSADBANK();
};

enum ExecutionResultFlags
{
  ERF_DeletedSlave = 0x00000001,
};

void DSADBANK::DeleteObject(RN obj)
{
  DBCOMMON *pDB;
  pDB = GetCommonAddress(obj);
  pDB->link(m_objectDeleteList);
  m_objectDeleteList = obj;
};


DSADBANK::~DSADBANK()
{
  i32 i;
  if (numText != 0)
  {
    for (i=0; i<10; i++)
    {
      if (textStrings[i] != NULL) UI_free(textStrings[i]);
    };
  };
}

bool DSADBANK::ConsumateDeletes(RN RNslave)
{
  bool result = false;
  while (m_objectDeleteList != RNeof)
  {
    RN obj;
    DBCOMMON *pDB;
    obj = m_objectDeleteList;
    if (obj == RNslave) result = true;
    pDB = GetCommonAddress(obj);
    m_objectDeleteList = pDB->link();
    DeleteDBEntry(pDB);
  };
  return result;
}

void DSADBANK::SetText(const char *text, i32 len, i32 index)
{
  i32 i;
  if (index > 9) return;
  if (numText == 0)
  {
    for (i=0; i<10; i++) textStrings[i] = NULL;
    numText = 10;
  };
  textStrings[index] = (char *)UI_realloc(textStrings[index], len+1, MALLOC104);
  memcpy(textStrings[index], text, len+1);
}

const char *DSADBANK::GetText(ui32 index)
{
  if (numText == 0) return "";
  if (textStrings[index] == NULL) return "";
  return textStrings[index];
};

DSADBANK *pdsaDbank = NULL;


static void PrintTrace(const char *text)
{
  if (pdsaDbank->tracing)
  {
    fprintf(GETFILE(TraceFile),"%s",text);
  };
}



void DSADBANK::NoValue(i32 i)
{
  static bool errorPrinted = false;
  char msg[300];
  i32 result;
  pDsaVars->values[i] = 0;
  if (errorPrinted) return;
  sprintf(msg,"DSA variable[%d] used without\nhaving been assigned a value.\nShould Future warnings be supressed?", i);
  result = UI_MessageBox(msg, "DSA programming Error",MESSAGE_YESNO);
  PrintTrace(msg);
  if (result == MESSAGE_IDYES) errorPrinted = true;
}



static const char *colNames[12] =
{
  "S0","C0","T0","S1","C1","T1","S2","C2","T2","S3","C3","T3"
};

struct EXECUTIONPACKET 
{
  i32         m_nextState;      //The prefix '5' in "5MS"
  ui16       *m_PC;             //Updated
  i32         m_wordsRemaining; //Updated
  DSA        *m_pMaster;        //Unchanged
  i32         m_curState;       //Unchanged
  i32         m_column;         //Unchanged
  LOCATIONREL m_locrMaster;     //Unchanged
  LOCATIONREL m_locrSlave;      //Unchanged
  i32         m_transferState;  //-1 unless Jump
  i32         m_transferCol;    //-1 unless Jump
  i32         m_subroutineLevel;//Unchanged
  RN          m_RNmaster;       //Unchanged
  RN          m_RNslave;        //Unchanged
};


struct LOCATIONABS
{
  i32 l;
  i32 x;
  i32 y;
  i32 p;
public:
  LOCATIONABS& operator =(LOCATIONREL& locr);
};

// **********  RUNTIME DEBUGGING HELP **********
EXECUTIONPACKET *pExPkt; //only for debugging purposes
static char *constructDebugMessage(const char *msg)
{
  static char debugMessage[300];
  sprintf(debugMessage,"%s\nDSA at %d(%d,%d)\nRow=%d;Col=%d",
          msg,
          pExPkt->m_locrMaster.l,
          pExPkt->m_locrMaster.x,
          pExPkt->m_locrMaster.y,
          pExPkt->m_curState,
          pExPkt->m_column);
  return debugMessage;
}

// **********  RUNTIME DEBUGGING HELP **********

ui32 LOCATIONREL::Integer() const
{
  ASSERT(x < 32,"locr");
  ASSERT(y < 32,"locr");
  ASSERT(l < 64,"locr");
  ASSERT(p < 4,"locr");
  return (p<<16)|(l<<10)|(x<<5)|y;
}

LOCATIONREL *LOCATIONREL::Integer(i32 t)
{
  p = (t >> 16) & 3;
  l = (t >> 10) & 63;
  x = (t >> 5) & 31;
  y = t & 31;
  return this;
}

bool LOCATIONREL::IsValid() const
{
  if (l >= d.dungeonDatIndex->NumLevel()) return false;
  if (x > d.pLevelDescriptors[l].LastColumn()) return false;
  if (y > d.pLevelDescriptors[l].LastRow()) return false;
  if (x < 0) return false;
  if (y < 0) return false;
  return true;
}

LOCATIONABS& LOCATIONABS::operator =(LOCATIONREL& locr)
{
  l = locr.l;
  x = locr.x + d.pLevelDescriptors[l].offsetX;
  y = locr.y + d.pLevelDescriptors[l].offsetY;
  p = locr.p;
  return *this;
}



STACK::STACK()
{
  stacklen = 0;
}

STACK::~STACK()
{
}

char *STACK::underflowMsg()
{
  return constructDebugMessage("DSA stack underflow");
}

void STACK::dump()
{
  i32 i, n;
  if (pdsaDbank->tracing)
  {
    fprintf(GETFILE(TraceFile), " (");
    if (stacklen > 7) PrintTrace(" ... ");
    n = 7;
    if (stacklen < 7) n= stacklen;
    for (i=n; i>0; i--)
    {
      fprintf(GETFILE(TraceFile),
              i==n?"%d":" %d",
              stack[stacklen-i]);
    };
    fprintf(GETFILE(TraceFile), ")");
  };
}

void STACK::push(LOCATIONREL locr)
{
  push(locr.Integer());
}

void STACK::push(i32 i)
{
  if (stacklen > 99)
  {
    die(0xdcd6);
  };
  stack[stacklen++] = i;
}

i32 STACK::pop()
{
  if (stacklen < 1)
  {
    die(0xdcd7,underflowMsg());
  };
  return stack[--stacklen];
}

void STACK::pick(ui32 i)
{
  if (stacklen < i+1) die(0xdcdb,underflowMsg());
  push(stack[stacklen-1-i]);
}

void STACK::poke(ui32 i)
{
  ui32 v;
  v = pop();
  if (stacklen < i) die(0xdcdb,underflowMsg());
  stack[stacklen-i] = v;
}

void STACK::roll(ui32 i)
{
  ui32 temp;
  ui32 j;
  if (stacklen < i+1)
  {
    die(0xdcda,underflowMsg());
  };
  j = stacklen-i-1;
  temp = stack[j];
  for (; j<stacklen-1; j++) stack[j] = stack[j+1];
  stack[j] = temp;
}

void STACK::minusRoll(ui32 i)
{
  ui32 temp;
  ui32 j, k;
  if (stacklen < i+1)
  {
    die(0xdcda,underflowMsg());
  };
  j = stacklen-1;
  k = j-i;
  temp = stack[j];
  for (; j>k; j--) stack[j] = stack[j-1];
  stack[j] = temp;
}


static i32 Execute(RN  RNmaster,
                   RN  RNslave,
                   DSA *pMaster, 
                   DSA *pSlave, 
                   i32 curState, 
                   i32 msgType,
                   LOCATIONREL locrMaster,
                   LOCATIONREL locrSlave,
                   i32 subroutineLevel,
                   bool filter);

static i32 warningCount = 3;
static bool Override_P = false;
static i32 Override_Pos;

static void message(const char *msg, bool always=false)
{
  char *m;
  warningCount--;
  if (!always && (warningCount < 0)) return;
  m = constructDebugMessage(msg);
  UI_MessageBox(m, "Warning", MESSAGE_OK);
  PrintTrace(m);
}


void NoFilter()
{
  message("Filter Modifying Dungeon");
}

#define NOFILTER if (pdsaDbank->filter) NoFilter();


// *********************************************************
// All of this copied from QueueSwitchAction and CreateTimer
// Que timer as shown in table below
//       If target room is not stone position = 0
//       If target room is stone position = targetPos;
//  ---------------------------------------------------
//    target room    timerFunction
//     0=roomSTONE           6          set/clear bits in DB2 and DB3
//                                      If actuator type 14 is in the
//                                      room then an object is ejected
//                                      from the room as a missile.
//     1=roomOPEN            5          ????????
//     2=roomPIT             9          ????????
//     3=roomSTAIRS          0          ????????
//     4=roomDOOR           10          ????????
//     5=roomTELEPORTER      8          set/clear room-active  (0x08)
//     6=roomFALSEWALL       7          set/clear wall invisible (0x04)
//     7=unused              0          ????????
// *********************************************************
//   TAG012cb0
void QueueDSASwitchAction(
                   i32 delay,        //
                   i32 tmrAct,       //Action set/clear/toggle
                   i32 level,        //
                   i32 targetX,i32 targetY,
                   i32 targetPos,
                   char MorD)
{
  i32 actionTime;
  i32 numericFunction;
  TIMERTYPE tt;
  CELLFLAG cfD4;
  TIMER timer;
  actionTime = d.Time + delay;
  //timer.timerTime = (level << 24) | actionTime;
  timer.Time(actionTime);
  timer.Level((ui8)level);
  if (MorD == 'M')
  {
    ROOMTYPE targetRoomType;
    cfD4 = d.pppdPointer10450[level][targetX][targetY]; //Cellflag
    targetRoomType = (ROOMTYPE)(cfD4>>5); // roomType
    if (   (targetRoomType != roomSTONE)
        && (targetRoomType != roomFALSEWALL))targetPos = 0;
    numericFunction = d.Byte1596[targetRoomType];
    switch (numericFunction)
    {
    case 7: tt = TT_FALSEWALL; break;
    default: tt = (TIMERTYPE)numericFunction; break;
    };
    if (tt == 0) 
    {
      WarnIllegalTimer(-1, -1, targetX, targetY);
      return;
    }
  }
  else
  {
    tt = TT_DESSAGE;
  };
  timer.Function(tt);
  timer.timerUByte5(0);
  timer.timerUByte6((ui8)targetX);
  timer.timerUByte7((ui8)targetY);
  timer.timerUByte8((ui8)targetPos);
  timer.timerUByte9((ui8)tmrAct);
  gameTimers.SetTimer(&timer);
}

static RN FindMaster(DSA *pSlave, RN objSlave, i32 x, i32 y,
                             i32 *masterX, i32 *masterY,
                             i32 *masterPos) 
{
  if (pSlave->IsMaster()) 
  {
    *masterX = x;
    *masterY = y;
    *masterPos = objSlave.pos();
    return objSlave;
  };
  message("Slave DSA Not Implemented");
  return objSlave;
}

static i32 GetState(DB3 *pDBMaster, DSA *pMaster)
{
  switch (pMaster->LocalState())
  {
  case 0: return pDBMaster->DSAstate();
  case 1: return pMaster->State();
  case 2: return pDBMaster->ParameterB();
  default:
    message("Illegal DSA LocalState");
    return 0;
  };
}

static void PutState(DB3 *pDBMaster, DSA *pMaster, i32 state)
{
  switch (pMaster->LocalState())
  {
  case 0: pDBMaster->DSAstate((ui8)state); break;
  case 1: pMaster->State(state); break;
  case 2: pDBMaster->ParameterB(state); break;
  default:
    message("Illegal DSA LocalState");
  };
}

static void EX_NOOP(EXECUTIONPACKET& exPkt)
{
  i32 relState;
  DSAnoopCmd cmd;
  cmd = ((DSAcommand *)(exPkt.m_PC++))->noopCmd;
  exPkt.m_wordsRemaining--;
  if (cmd.IsMaxState())
  {
    relState = *(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  }
  else
  {
    relState = cmd.NextState();
  };
  exPkt.m_nextState = exPkt.m_curState + relState;
}

LOCATIONREL GetTarget(TARGETTYPE targetType,
                      EXECUTIONPACKET& exPkt,
                      bool thirtyTwo)
{
  LOCATIONREL result;
  ui32 target;
  switch (targetType)
  {
  case TARGET_A:
    target = GetRecordAddressDB3(exPkt.m_RNmaster)->ParameterA();
    break;
  case TARGET_B:
    target = GetRecordAddressDB3(exPkt.m_RNmaster)->ParameterB();
    break;
  case TARGET_ABS:
    target = *(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
    if (thirtyTwo)
    {
      target |= (*(exPkt.m_PC++)) << 16;
      exPkt.m_wordsRemaining--;
    };
    break;
  case TARGET_GEAR:
    target = pdsaDbank->stack.pop();
//    target =  (ui16)((locrMaster.l & 0xf) << 10);
//    target |= (locrMaster.p & 3) << 14;
//    target |= (locrMaster.x &0x1f) <<5;
//    target |= locrMaster.y & 0x1f;
    break;
  default:
    message("Illegal Target Type in DSA");
    target = 0;
  };
  result.l = (target>>10) & 0x3f;
  result.p = (target>>16) & 3;
  result.x = (target>>5)  & 0x1f;
  result.y = target & 0x1f;
  if (Override_P) result.p = Override_Pos;
  Override_P = false;
  return result;
}

static void EX_MESSAGE(EXECUTIONPACKET& exPkt, bool thirtyTwo, char MorD)
{
  i32 relState;
  i32 action;
  LOCATIONREL locr;
  DSAmessageCmd cmd;
  i32 delay=0;  //To keep compiler happy.
  MESSAGETYPE messageType;
  TIMER timer;
  cmd = ((DSAcommand *)(exPkt.m_PC++))->msgCmd;
  exPkt.m_wordsRemaining--;
  if (cmd.IsMaxState())
  {
    relState = *(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  }
  else
  {
    relState = cmd.NextState();
  };
  exPkt.m_nextState = exPkt.m_curState + relState;
  messageType = cmd.MsgType();
  switch (cmd.Delay())
  {
  case 0: delay = 0; break;
  case 1: delay = GetRecordAddressDB3(exPkt.m_RNmaster)->ParameterA(); break;
  case 2: delay = GetRecordAddressDB3(exPkt.m_RNmaster)->ParameterB(); break;
  case 3: delay = *(exPkt.m_PC++); exPkt.m_wordsRemaining--; break;
  };
//  locr = GetTarget(cmd.Target(),
//                   PC,
//                   wordsRemaining,
//                   RNmaster,
//                   pDBMaster,
//                   pMaster,
//                   locrMaster);
  locr = GetTarget(cmd.Target(), exPkt, thirtyTwo);
  switch (messageType)
  {
  case MSG_NULL: 
    if (pdsaDbank->tracing)
    {
      PrintTrace("No Message");
    };
    return;
  case MSG_SET: 
    if (pdsaDbank->tracing)
    {
      PrintTrace("SET");
    };
    action = tmrAct_SET; 
    break;
  case MSG_CLEAR: 
    if (pdsaDbank->tracing)
    {
      PrintTrace("CLEAR");
    };
    action = tmrAct_CLEAR; 
    break;
  case MSG_TOGGLE: 
    if (pdsaDbank->tracing)
    {
      PrintTrace("TOGGLE");
    };
    action = tmrAct_TOGGLE; 
    break;
  default: 
    if (pdsaDbank->tracing)
    {
      PrintTrace("UNKNOWN");
    };
    message("Unknown DSA messageType"); 
    return;
  };
  if (pdsaDbank->tracing)
  {
    LOCATIONABS loca;
    loca = locr;
    fprintf(GETFILE(TraceFile)," delay=%d, %d(%d,%d)%d\n",
            delay,loca.l,loca.x,loca.y,loca.p);
  };
  QueueDSASwitchAction(
                   delay,        //
                   action,       //Action set/clear/toggle
                   locr.l,       //level
                   locr.x,
                   locr.y,
                   locr.p,
                   MorD);

  return;
}

static void EX_COPYTELEPORTER(EXECUTIONPACKET& exPkt, bool thirtyTwo)
{
  RN RNsrc, RNdst;
  CELLFLAG *pCFsrc, *pCFdst;
  DB1 *pDB1Src, *pDB1Dst;
  i32 relState;
  LOCATIONREL locrSrc, locrDst;
  DSAcopyTeleporterCmd cmd;
  cmd = ((DSAcommand *)(exPkt.m_PC++))->copyTeleporterCmd;
  exPkt.m_wordsRemaining--;
  if (cmd.IsMaxState())
  {
    relState = *(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  }
  else
  {
    relState = cmd.NextState();
  };
  exPkt.m_nextState = exPkt.m_curState + relState;
  locrSrc = GetTarget(cmd.TargetFrom(), exPkt, thirtyTwo);
  locrDst = GetTarget(cmd.TargetTo(), exPkt, thirtyTwo);
  //swap locrsrc and locrdst
  RNsrc = FindFirstTeleporter(locrSrc.l,locrSrc.x,locrSrc.y);
  pCFsrc = GetCellFlagsAddress(locrSrc.l,locrSrc.x,locrSrc.y);
  RNdst = FindFirstTeleporter(locrDst.l,locrDst.x,locrDst.y);
  pCFdst = GetCellFlagsAddress(locrDst.l,locrDst.x,locrDst.y);
  if (RNsrc == RNeof) return;
  if (RNdst == RNeof) return;
  pDB1Src = GetRecordAddressDB1(RNsrc);
  pDB1Dst = GetRecordAddressDB1(RNdst);
  pDB1Dst->copyTeleporter(pDB1Src);
  *pCFdst = *pCFsrc;
  return;
}

static void EX_GOSUB(EXECUTIONPACKET& exPkt)
{
  i32 relState;
  i32 row, column;
  DSAgosubCmd cmd;
  TIMER timer;
  cmd = ((DSAcommand *)(exPkt.m_PC++))->gosubCmd;
  exPkt.m_wordsRemaining--;
  if (cmd.IsMaxState())
  {
    relState = *(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  }
  else
  {
    relState = cmd.NextState();
  };
  exPkt.m_nextState = exPkt.m_curState + relState;
  row = cmd.Row();
  if (row == 63)
  {
    row = *(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  };
  column = cmd.Column();
  if (column != 0)
  {
    column = *(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  };
  if (pdsaDbank->tracing)
  {
    fprintf(GETFILE(TraceFile),"%d%s\n",row,colNames[column]);
  };
  Execute(exPkt.m_RNmaster,
          exPkt.m_RNslave,
          exPkt.m_pMaster,
          exPkt.m_pMaster,
          row,
          column,
          exPkt.m_locrMaster,
          exPkt.m_locrSlave,
          exPkt.m_subroutineLevel+1,
          pdsaDbank->filter);
  return;
}

static void EX_JUMP(EXECUTIONPACKET& exPkt)
{
  i32 relState;
  i32 row, column;
  DSAjumpCmd cmd;
  //TIMER timer;
  cmd = ((DSAcommand *)(exPkt.m_PC++))->jumpCmd;
  exPkt.m_wordsRemaining--;
  if (cmd.IsMaxState())
  {
    relState = *(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  }
  else
  {
    relState = cmd.NextState();
  };
  exPkt.m_nextState = exPkt.m_curState + relState;
  row = cmd.Row();
  if (row == 63)
  {
    row = *(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  };
  exPkt.m_transferState = row;
  column = cmd.Column();
  if (column != 0)
  {
    column = *(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  };
  exPkt.m_transferCol = column;
  if (pdsaDbank->tracing)
  {
    fprintf(GETFILE(TraceFile)," Jump %d%s",
              exPkt.m_transferState, colNames[exPkt.m_transferCol]);
  };
}

static void EX_QUESTION(EXECUTIONPACKET& exPkt)
{
  i32 relState;
  DSAquestionCmd cmd;
  TIMER timer;
  i32 cCmd, cCol, addr;
  cmd = ((DSAcommand *)(exPkt.m_PC++))->questionCmd;
  exPkt.m_wordsRemaining--;
  if (cmd.IsMaxState())
  {
    relState = *(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  }
  else
  {
    relState = cmd.NextState();
  };
  exPkt.m_nextState = exPkt.m_curState + relState;
  if (pdsaDbank->stack.pop())
  {
    if (pdsaDbank->tracing)
    {
      PrintTrace("(true)");
    };
    cCmd = cmd.IfCmd();
    cCol = 0;
    addr = 0;
    if (cCmd != 0) 
    {
      addr = *(exPkt.m_PC++); 
      exPkt.m_wordsRemaining--;
    };
    if (cmd.IfColumn() != 0) 
    {
      cCol = *(exPkt.m_PC++);
      exPkt.m_wordsRemaining--;
    };
    if (cmd.ElseCmd() != 0) 
    {
      exPkt.m_PC++; 
      exPkt.m_wordsRemaining--;
    };
    if (cmd.ElseColumn() != 0)
    {
      exPkt.m_PC++; 
      exPkt.m_wordsRemaining--;
    };
  }
  else
  {
    if (pdsaDbank->tracing)
    {
      PrintTrace("(false)");
    };
    if (cmd.IfCmd() != 0) 
    {
      exPkt.m_PC++; 
      exPkt.m_wordsRemaining--;
    };
    if (cmd.IfColumn() != 0)
    {
      exPkt.m_PC++; 
      exPkt.m_wordsRemaining--;
    };
    cCmd = cmd.ElseCmd();
    cCol = 0;
    addr = 0;
    if (cCmd != 0) 
    {
      addr = *(exPkt.m_PC++); 
      exPkt.m_wordsRemaining--;
    };
    if (cmd.ElseColumn() != 0) 
    {
      cCol = *(exPkt.m_PC++);
      exPkt.m_wordsRemaining--;
    };
  };
  switch (cCmd)
  {
  case 1:
    exPkt.m_transferState = addr;
    exPkt.m_transferCol = cCol;
    if (pdsaDbank->tracing)
    {
      fprintf(GETFILE(TraceFile)," Jump %d%s",
                addr, colNames[cCol]);
    };
    return; 
  case 2:
    // Gosub
    if (pdsaDbank->tracing)
    {
      fprintf(GETFILE(TraceFile)," GoSub %d%s\n",
                addr, colNames[cCol]);
    };
    Execute(exPkt.m_RNmaster,
            exPkt.m_RNslave,
            exPkt.m_pMaster,
            exPkt.m_pMaster,
            addr,
            cCol,
            exPkt.m_locrMaster,
            exPkt.m_locrSlave,
            exPkt.m_subroutineLevel+1,
            pdsaDbank->filter);
    break;
  default:
    if (pdsaDbank->tracing)
    {
      PrintTrace(" Do Nothing");
    };
    break;
  };
  return;
}


struct ui16_16
{
  //A 32-bit unsigned integer constructed from two 16-bit numbers.
  ui16 low;
  i16  high;
  ui32  val(){return (high<<16) | low;};
  bool operator ==(ui32 v){return val()==v;};
  bool operator >(ui32 v){return val()>v;};
  i32  operator >>(ui32 n){return val() >> n;};
  i32 operator &(ui32 m){return val() & m;};
};

static void EX_CASE(EXECUTIONPACKET& exPkt)
{
  i32 relState;
  DSAcaseCmd cmd;
  TIMER timer;
  i32 col, addr;
  i32 value;
  ui32 numCase, n, m;
  ui16_16 *pCase;
  cmd = ((DSAcommand *)(exPkt.m_PC++))->caseCmd;
  exPkt.m_wordsRemaining--;
  if (cmd.IsMaxState())
  {
    relState = *(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  }
  else
  {
    relState = cmd.NextState();
  };
  exPkt.m_nextState = exPkt.m_curState + relState;
  value = pdsaDbank->stack.pop();
  numCase = *(exPkt.m_PC++);
  exPkt.m_wordsRemaining--;
  //Search the cases for value
  pCase = (ui16_16 *)exPkt.m_PC;
  for (n=numCase; n>0;)
  {
    m=n/2;
    if (pCase[2*m] == value)
    {
      addr = (pCase[2*m+1] >> 8) & 0xffff;
      col  = pCase[2*m+1] & 0xff;
      exPkt.m_transferState = addr;
      exPkt.m_transferCol = col;
      if (pdsaDbank->tracing)
      {
        fprintf(GETFILE(TraceFile)," Jump %d%s",
                  addr, colNames[col]);
      };
      break;
    };
    if (pCase[2*m] > value)
    {
      n = m;
    }
    else
    {
      pCase += 2*(m+1);
      n -= m+1;
    };
  };
  exPkt.m_PC += 4*numCase;
  exPkt.m_wordsRemaining -= 4*numCase;
  return;
}

static void EX_OVERRIDE(EXECUTIONPACKET& exPkt)
{
  i32 relState;
  i32 what;
  DSAoverrideCmd cmd;
  TIMER timer;
  cmd = ((DSAcommand *)(exPkt.m_PC++))->overrideCmd;
  exPkt.m_wordsRemaining--;
  what = cmd.What();
  switch(what)
  {
  case 1:
    Override_P = true;
    Override_Pos = cmd.Value();
    if (Override_Pos == 7)
    {
      Override_Pos = *(exPkt.m_PC++);
      exPkt.m_wordsRemaining--;
    };
    break;
  default:
    message("Illegal DSA override command");
    return;
  };
  if (cmd.IsMaxState())
  {
    relState = *(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  }
  else
  {
    relState = cmd.NextState();
  };
  exPkt.m_nextState = exPkt.m_curState + relState;
  return;
}  
  
static void EX_LOAD(EXECUTIONPACKET& exPkt)
{
  i32 relState;
  i32 where;
  RN RNparameter;
  DB3 *pParameter;
  DSAloadCmd cmd;
  //TIMER timer;
  LOCATIONREL locr;
  LOCATIONABS loca;
  cmd = ((DSAcommand *)(exPkt.m_PC++))->loadCmd;
  exPkt.m_wordsRemaining--;
  if (cmd.IsMaxState())
  {
    relState = (i16)*(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  }
  else
  {
    relState = cmd.NextState();
  };
  exPkt.m_nextState = exPkt.m_curState + relState;
  where = cmd.Where();
  switch(where)
  {
  case LOAD_INTEGER:
    if (pdsaDbank->tracing)
    {
      fprintf(GETFILE(TraceFile),"INTEGER %d ",*exPkt.m_PC);
    };
    pdsaDbank->stack.push(*(exPkt.m_PC++));
    exPkt.m_wordsRemaining--;
    break;
  case LOAD_ABS:
    if (pdsaDbank->tracing)
    {
      locr.Integer(*exPkt.m_PC);
      loca = locr;
      fprintf(GETFILE(TraceFile),"ABSLOCATION %d(%d,%d)%d",
            loca.l, loca.x, loca.y,loca.p);
    };
    pdsaDbank->stack.push(*(exPkt.m_PC++));
    exPkt.m_wordsRemaining--;
    break;
  case LOAD_DOLLAR: //L$
    if (pdsaDbank->tracing)
    {
      PrintTrace("DOLLAR ");
    };
    pdsaDbank->stack.push(exPkt.m_locrMaster);
    break;
  case LOAD_INTEGER32:
    {
      i32 bigInt;
      bigInt = *(exPkt.m_PC++);
      bigInt |= *(exPkt.m_PC++) << 16;
      if (pdsaDbank->tracing)
      {
        fprintf(GETFILE(TraceFile), "BigInteger %d ",bigInt);
      };
      pdsaDbank->stack.push(bigInt);
      exPkt.m_wordsRemaining -= 2;
    };
    break;
  default:
    if (where > 25)
    {
      message("Illegal DSA load command");
      return;
    };
    RNparameter = exPkt.m_RNmaster;
    if (pdsaDbank->tracing)
    {
      fprintf(GETFILE(TraceFile),"Parameter %c = ",where+'A');
    };
    while (where > 1)
    {
      RNparameter = GetDBRecordLink(RNparameter);
      if (RNparameter == RNeof) break;
      if (RNparameter.dbType() != dbACTUATOR) continue;
      pParameter = GetRecordAddressDB3(RNparameter);
      if (pParameter->actuatorType() != 47) continue;
      where -= 2;
    };
    if (RNparameter == RNeof) 
    {
      if (pdsaDbank->tracing)
      {
        PrintTrace("Missing = 0");
      };
      pdsaDbank->stack.push(0);
    }
    else
    {
      pParameter = GetRecordAddressDB3(RNparameter);
      if (where == 0) 
      {
        pdsaDbank->stack.push(pParameter->ParameterA());
        if (pdsaDbank->tracing)
        {
          fprintf(GETFILE(TraceFile),"%d",pParameter->ParameterA());
        };
      }
      else 
      {
        pdsaDbank->stack.push(pParameter->ParameterB());
        if (pdsaDbank->tracing)
        {
          fprintf(GETFILE(TraceFile),"%d",pParameter->ParameterB());
        };
      };
    };
  };
  return;
}  
  

static void EX_VARIABLESTORE(EXECUTIONPACKET& exPkt)
{
  i32 relState;
  i32 idx;
  DSAvariableStoreCmd cmd;
  cmd = ((DSAcommand *)(exPkt.m_PC++))->variableStoreCmd;
  exPkt.m_wordsRemaining--;
  if (cmd.IsMaxState())
  {
    relState = (i16)*(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  }
  else
  {
    relState = cmd.NextState();
  };
  exPkt.m_nextState = exPkt.m_curState + relState;
  idx = cmd.Index();
  if (pdsaDbank->tracing)
  {
    fprintf(GETFILE(TraceFile),"%d!", idx);
  };
  pdsaDbank->Var(idx, pdsaDbank->stack.pop());
  return;
}  
  
static void EX_VARIABLEFETCH(EXECUTIONPACKET& exPkt)
{
  i32 relState;
  i32 idx;
  DSAvariableFetchCmd cmd;
  cmd = ((DSAcommand *)(exPkt.m_PC++))->variableFetchCmd;
  exPkt.m_wordsRemaining--;
  if (cmd.IsMaxState())
  {
    relState = (i16)*(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  }
  else
  {
    relState = cmd.NextState();
  };
  exPkt.m_nextState = exPkt.m_curState + relState;
  idx = cmd.Index();
  if (pdsaDbank->tracing)
  {
    fprintf(GETFILE(TraceFile),"%d@", idx);
  };
  pdsaDbank->stack.push(pdsaDbank->Var(idx));
  return;
}  
  

static void EX_GLOBALSTORE(EXECUTIONPACKET& exPkt)
{
  i32 relState;
  i32 idx;
  ui32 value;
  DSAglobalStoreCmd cmd;
  cmd = ((DSAcommand *)(exPkt.m_PC++))->globalStoreCmd;
  exPkt.m_wordsRemaining--;
  if (cmd.IsMaxState())
  {
    relState = (i16)*(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  }
  else
  {
    relState = cmd.NextState();
  };
  exPkt.m_nextState = exPkt.m_curState + relState;
  idx = cmd.Index();
  if (pdsaDbank->tracing)
  {
    fprintf(GETFILE(TraceFile),"%d!", idx);
  };
  value = pdsaDbank->stack.pop();
  if (numGlobalVariables <= idx) 
  {
    message("Global Store to non-existent variable", true);
  }
  else
  {
    globalVariables[idx] = value;
  };
  return;
}  
  
static void EX_GLOBALFETCH(EXECUTIONPACKET& exPkt)
{
  i32 relState;
  i32 idx;
  ui32 value;
  DSAglobalFetchCmd cmd;
  cmd = ((DSAcommand *)(exPkt.m_PC++))->globalFetchCmd;
  exPkt.m_wordsRemaining--;
  if (cmd.IsMaxState())
  {
    relState = (i16)*(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  }
  else
  {
    relState = cmd.NextState();
  };
  exPkt.m_nextState = exPkt.m_curState + relState;
  idx = cmd.Index();
  if (pdsaDbank->tracing)
  {
    fprintf(GETFILE(TraceFile),"%d@", idx);
  };
  if (idx >= numGlobalVariables)
  {
    message("Gloal fetch from non-existent variable", true);
    value = 0xcdcdcdcd;
  }
  else
  {
    value = globalVariables[idx];
  };
  pdsaDbank->stack.push(value);
  return;
}  
  


static void EX_STORE(EXECUTIONPACKET& exPkt)
{
  i32 relState;
  i32 where;
  RN RNparameter;
  DB3 *pParameter;
  DSAstoreCmd cmd;
  //TIMER timer;
  cmd = ((DSAcommand *)(exPkt.m_PC++))->storeCmd;
  exPkt.m_wordsRemaining--;
  where = cmd.Where();
  if (where > 25)
  {
    message("Illegal DSA store command");
    return;
  };
  RNparameter = exPkt.m_RNmaster;
  if (pdsaDbank->tracing)
  {
    fprintf(GETFILE(TraceFile),"Parameter %c = ",where+'A');
  };
  while (where > 1)
  {
    RNparameter = GetDBRecordLink(RNparameter);
    if (RNparameter == RNeof) break;
    if (RNparameter.dbType() != dbACTUATOR) continue;
    pParameter = GetRecordAddressDB3(RNparameter);
    if (pParameter->actuatorType() != 47) continue;
    where -= 2;
  };
  if (RNparameter == RNeof) 
  {
    if (pdsaDbank->tracing)
    {
      PrintTrace("Missing = 0");
    };
    pdsaDbank->stack.pop();
  }
  else
  {
    pParameter = GetRecordAddressDB3(RNparameter);
    if (where == 0) 
    {
      pParameter->ParameterA(pdsaDbank->stack.pop());
      if (pdsaDbank->tracing)
      {
        fprintf(GETFILE(TraceFile),"%d",pParameter->ParameterA());
      };
    }
    else 
    {
      pParameter->ParameterB(pdsaDbank->stack.pop());
      if (pdsaDbank->tracing)
      {
        fprintf(GETFILE(TraceFile),"%d",pParameter->ParameterB());
      };
    };
  };
  if (cmd.IsMaxState())
  {
    relState = *(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  }
  else
  {
    relState = cmd.NextState();
  };
  exPkt.m_nextState = exPkt.m_curState + relState;
  return;
}  
  
static void EX_TYPE()
{
  i32 id, objID;
  RN thisObj, contents;
  DB0 *pDB0;
  DB1 *pDB1;
  DB2 *pDB2;
  DB3 *pDB3;
  DB4 *pDB4;
  DB5 *pDB5;
  DB6 *pDB6;
  DB7 *pDB7;
  DB8 *pDB8;
  DB9 *pDB9;
  DB10 *pDB10;
  DB15 *pDB15;
  i32 objIndex;
  objIndex = pdsaDbank->stack.pop();
  if ((objIndex < 0) || !thisObj.checkIndirectIndex(objIndex))
  {
    pdsaDbank->stack.push(-1);
    return;
  };
  thisObj.ConstructFromInteger(objIndex);
  switch (thisObj.dbType())
  {
  case dbDOOR:
    pDB0 = GetRecordAddressDB0(thisObj);
    id = pDB0->doorSwitch();                 //bit 0
    if (pDB0->canOpenWithAxe()) id |= 2;     //bit 1
    if (pDB0->canOpenWithFireball()) id |= 4;//bit 2
    id |= pDB0->mode() << 3;                 //bit 3
    id |= pDB0->ornateness() << 4;           //bit 4-7
    id |= pDB0->doorType() << 8;             //bit 8
    break;
  case dbTELEPORTER:
    pDB1 = GetRecordAddressDB1(thisObj);
    id = pDB1->rotation();        //bit0-1
    id |= pDB1->facingMode() << 2;//bit 2
    id |= pDB1->what() << 3;      //bit 3-4
    id |= pDB1->audible() << 5;   //bit 5
    break;
  case dbTEXT:
    pDB2 = GetRecordAddressDB2(thisObj);
    id = pDB2->show(); //bit 0
    break;
  case dbACTUATOR:
    pDB3 = GetRecordAddressDB3(thisObj);
    id = pDB3->audible();       //bit 0
    id |= pDB3->onceOnly()<<1;  //bit 1
    id |= pDB3->actuatorType()<<2;//bit 2-8
    break;
  case dbMONSTER:
    pDB4 = GetRecordAddressDB4(thisObj);
    id = pDB4->monsterType();
    break;
  case dbWEAPON:
    pDB5 = GetRecordAddressDB5(thisObj);
    id = pDB5->weaponType();
    break;
  case dbCLOTHING:
    pDB6 = GetRecordAddressDB6(thisObj);
    id = pDB6->clothingType();
    break;
  case dbSCROLL:
    pDB7 = GetRecordAddressDB7(thisObj);
    id = 0;
    break;
  case dbPOTION:
    pDB8 = GetRecordAddressDB8(thisObj);
    id = pDB8->potionType() << 8;
    id |= pDB8->strength();
    break;
  case dbCHEST:
    pDB9 = GetRecordAddressDB9(thisObj);
    for (id=0,contents = pDB9->contents();
         contents != RNeof;
         contents = GetDBRecordLink(contents))
    {
      id++;
    };
    break;
  case dbMISC:
    pDB10 = GetRecordAddressDB10(thisObj);
    id = pDB10->miscType();
    break;
  case dbCLOUD:
    pDB15 = GetRecordAddressDB15(thisObj);
    id = pDB15->cloudType();
    break;
  case dbEXPOOL:
  case db12:
  case db13:
  case dbMISSILE:
  default:
    id = 0;
    break;
  };
  objID = thisObj.dbType() *10000;
  objID += id;
  pdsaDbank->stack.push(objID);
}
  
static void EX_EQUAL(EXECUTIONPACKET& exPkt)
{
  i32 v1, v2;
  i32 relState;
  DSAequalCmd cmd;
  cmd = ((DSAcommand *)(exPkt.m_PC++))->equalCmd;
  exPkt.m_wordsRemaining--;
  v1 = pdsaDbank->stack.pop();
  v2 = pdsaDbank->stack.pop();
  if (pdsaDbank->tracing)
  {
    fprintf(GETFILE(TraceFile),"(%d,%d) --> %d",
      v1, v2, v1==v2 ? 1 : 0);
  };
  pdsaDbank->stack.push(((v1==v2) ? 1 : 0));
  if (cmd.IsMaxState())
  {
    relState = *(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  }
  else
  {
    relState = cmd.NextState();
  };
  exPkt.m_nextState = exPkt.m_curState + relState;
  return;
}  
  

bool IsCarriedCallback(RN thisObj, int srcObj)
{
  // Return true to terminate search.
  if (srcObj < 0)
  {
    srcObj = -srcObj;  // A generic object type.
    if (srcObj == 1) srcObj = 0;
    if (srcObj == GetBasicObjectType(thisObj))
    {
      pdsaDbank->stack.push(pdsaDbank->stack.pop() + 1);
    };
    return false;
  }
  else
  {
    // A particular object.
    if (srcObj == thisObj.ConvertToInteger())
    {
      pdsaDbank->stack.push(pdsaDbank->stack.pop() + 1);
      return true;
    };
    return false;
  };
};

bool ForEach(int thisObj, int srcObj, bool callback(RN thisObj, int srcObj))
{
  // return true to terminate search.
  RN obj;
  int i, chIdx;
  if (thisObj == RNnul) return false;
  if (thisObj < 0)  // A character index
  {
    chIdx = -thisObj-1;
    for (i=0; i<30; i++)
    {
      if (ForEach(d.CH16482[chIdx].Possession(i).ConvertToInteger(), srcObj, callback))
      {

        pdsaDbank->stack.pop();
        pdsaDbank->stack.push(i);
        return true;
      };
    };
    if (chIdx == d.HandChar)
    {
      if (ForEach(d.objectInHand.ConvertToInteger(), srcObj, callback))
      {
        pdsaDbank->stack.pop();
        pdsaDbank->stack.push(255);
        return true;
      };
    };
  }
  else
  {
    DBTYPE dbType;
    obj.ConstructFromInteger(thisObj);
    if (callback(obj, srcObj))
    {
      return true;
    };
    dbType  = obj.dbType();
    switch (dbType)
    {
    case dbSCROLL:
    case dbPOTION:
    case dbMISC: 
    case dbCLOTHING:
    case dbWEAPON:
      break;
    case dbCHEST:
      {
        DB9 *pChest;
        RN contents;
        pChest = GetRecordAddressDB9(obj);
        for (contents=pChest->contents(); contents!=RNeof; contents=GetDBRecordLink(contents))
        {
          if (ForEach(contents.ConvertToInteger(), srcObj, callback))
          {
            return true;
          };
        };
      };
      break;
    default:
      NotImplemented(0xdd6a);
      break;
    };
  };
  return false;
};

static void EX_IsCarried()
{
  int chidx;
  int objid, charnum;
  objid = pdsaDbank->stack.pop();
  charnum = pdsaDbank->stack.pop();
  pdsaDbank->stack.push(0);
  for (chidx=0; chidx<d.NumCharacter; chidx++)
  {
    if (    (charnum == chidx)
         || ((charnum == 4) && (chidx == d.HandChar))
         || (charnum == 5)
       )
    {
      if (ForEach (-chidx-1, objid, IsCarriedCallback))
      {
        // Terminate Search.  Return location of object.
        // Add character index to location.
        pdsaDbank->stack.push(pdsaDbank->stack.pop() + 256*chidx);
        return;
      };
    };
  };
  if (objid >= 0)
  {
    // Looking for a particular object and it was not found.
    pdsaDbank->stack.pop();
    pdsaDbank->stack.push(-1);
  };
}

static void EX_DEL()
{
  LOCATIONREL locr;
  i32 iLocation;
  i32 objID;
  i32 oldLevel;
  i32 chIdx=-1, posIdx=-1;
  DBTYPE dbType;
  RN thisObj;
  iLocation = pdsaDbank->stack.pop();
  objID = pdsaDbank->stack.pop();
  NOFILTER
  if (iLocation < 0)
  {
    if (iLocation == -1)
    {
      if (d.objectInHand == RNnul) return;
      CURSORFILTER_PACKET cfp;
      cfp.type = CURSORFILTER_DSA_DEL;
      cfp.object = d.objectInHand.ConvertToInteger();
      CursorFilter(&cfp);
      thisObj = RemoveObjectFromHand();
    }
    else
    {
      chIdx = (-iLocation)/100 - 1;
      posIdx = (-iLocation)%100;
      if (chIdx >= d.NumCharacter) return;
      if (posIdx >= 30) return;
      if (d.CH16482[chIdx].Possession(posIdx) == RNnul) return;
      thisObj = RemoveCharacterPossession(chIdx, posIdx);
      DrawCharacterState(chIdx);
    };
  }
  else
  {
    locr.Integer(iLocation);
    if (!locr.IsValid())
    {
      message("Illegal location in DSA &DEL");
      return;
    };
    for (thisObj = FindFirstObject(locr.l,locr.x,locr.y);
        thisObj != RNeof;
        thisObj = GetDBRecordLink(thisObj))
    {
      if (thisObj.ConvertToInteger() != objID) continue;
      dbType = thisObj.dbType();
      switch (dbType)
      {
      case dbACTUATOR:
      case dbMISC:
      case dbPOTION:
      case dbWEAPON:
      case dbCLOTHING:
        oldLevel = d.LoadedLevel;
        LoadLevel(locr.l);
        MoveObject(thisObj, locr.x, locr.y, -1, 0, NULL, NULL);
        //db.GetCommonAddress(thisObj)->link(RNnul);
        LoadLevel(oldLevel);
        break;
      default:
        return;
      };
      break;
    };
  };
  if (thisObj == RNnul) return;
  if (thisObj == RNeof) return;
  dbType = thisObj.dbType();
  switch (dbType)
  {
  case dbSCROLL:
    {
      DB7 *pScroll;
      RN RNText;
      pScroll = GetRecordAddressDB7(thisObj);
      pdsaDbank->DeleteObject(pScroll->text());
      //DeleteDBEntry(db.GetCommonAddress(pScroll->text()));
    };
    break;
  case dbMONSTER:
  case dbCHEST:
  case dbMISSILE:
  case dbDOOR:
  case dbTELEPORTER:
  case dbTEXT:
  //case dbACTUATOR:
  case dbEXPOOL:
    NotImplemented(0x89de); //Need to remove possessions, etc.
    break;
  };
  pdsaDbank->DeleteObject(thisObj);
  //DeleteDBEntry(db.GetCommonAddress(thisObj));
}


static void EX_DELMON()
{
  LOCATIONREL locr;
  i32 indx;
  indx = pdsaDbank->stack.pop();
  locr.Integer(pdsaDbank->stack.pop());
  DeleteMonsterInGroup(locr, indx);
}

static void EX_INSMON()
{
  LOCATIONREL locr;
  i32 posMask;
  posMask = pdsaDbank->stack.pop();
  locr.Integer(pdsaDbank->stack.pop());
  InsertMonsterInGroup(locr, posMask);
}
  
static RN CopyItemList (RN origRN);
static RN CopyItem (RN origRN)
{
  DBTYPE dbType;
  RN CopyRN = RNeof;
  RN copiedPossessions;
  if (origRN == RNeof) return CopyRN;
  dbType = origRN.dbType();
  switch (dbType)
  {
  case dbACTUATOR:
    {
      DB3 *pDB3, *newPDB3;
      i32 i;
      CopyRN = FindEmptyDB3Entry(true);
      pDB3 = GetRecordAddressDB3(origRN);
      if (CopyRN == RNnul)
      {
        CopyRN = RNeof;
        break;
      };
      newPDB3 = GetRecordAddressDB3(CopyRN);
      for (i=2; i<sizeof(DB3); i++)
      {
        *((char *)newPDB3+i) = *((char *)pDB3+i);
      };
    };
    break;
  case dbMISC:
    {
      DB10 *pDB10, *newPDB10;
      i32 i;
      CopyRN = FindEmptyDB10Entry(true);
      pDB10 = GetRecordAddressDB10(origRN);
      if (CopyRN == RNnul)
      {
        CopyRN = RNeof;
        break;
      };
      newPDB10 = GetRecordAddressDB10(CopyRN);
      for (i=2; i<sizeof(DB10); i++)
      {
        *((char *)newPDB10+i) = *((char *)pDB10+i);
      };
    };
    break;
  case dbWEAPON:
    {
      DB5 *pDB5, *newPDB5;
      i32 i;
      CopyRN = FindEmptyDB5Entry(true);
      pDB5 = GetRecordAddressDB5(origRN);
      if (CopyRN == RNnul)
      {
        CopyRN = RNeof;
        break;
      };
      newPDB5 = GetRecordAddressDB5(CopyRN);
      for (i=2; i<sizeof(DB5); i++)
      {
        *((char *)newPDB5+i) = *((char *)pDB5+i);
      };
    };
    break;
  case dbCLOTHING:
    {
      DB6 *pDB6, *newPDB6;
      i32 i;
      CopyRN = FindEmptyDB6Entry(true);
      pDB6 = GetRecordAddressDB6(origRN);
      if (CopyRN == RNnul)
      {
        CopyRN = RNeof;
        break;
      };
      newPDB6 = GetRecordAddressDB6(CopyRN);
      for (i=2; i<sizeof(DB6); i++)
      {
        *((char *)newPDB6+i) = *((char *)pDB6+i);
      };
    };
    break;
  case dbPOTION:
    {
      DB8 *pDB8, *newPDB8;
      i32 i;
      CopyRN = FindEmptyDB8Entry(true);
      pDB8 = GetRecordAddressDB8(origRN);
      if (CopyRN == RNnul)
      {
        CopyRN = RNeof;
        break;
      };
      newPDB8 = GetRecordAddressDB8(CopyRN);
      for (i=2; i<sizeof(DB8); i++)
      {
        *((char *)newPDB8+i) = *((char *)pDB8+i);
      };
    };
    break;
  case dbMONSTER:
    {
      DB4 *pDB4, *newPDB4;
      MONSTERDESC *pmtDesc;
      i32 i;
      i32 monsterSize;
      EnsureItem16Available();
      if (   (d.ITEM16QueLen >= d.MaxITEM16-5)
          && (d.LoadedLevel == d.partyLevel)  ) break;
      CopyRN = FindEmptyDB4Entry(true);
      pDB4 = GetRecordAddressDB4(origRN);
      if (CopyRN == RNnul)
      {
        CopyRN = RNeof;
        break;
      };
      newPDB4 = GetRecordAddressDB4(CopyRN);
      for (i=2; i<sizeof(DB4); i++)
      {
        *((char *)newPDB4+i) = *((char *)pDB4+i);
      };
      // already set; newPDB4->monsterType(value); // Item26 index
      pmtDesc = &d.MonsterDescriptor[newPDB4->monsterType()];
      monsterSize = pmtDesc->horizontalSize();
      copiedPossessions 
         = CopyItemList(pDB4->possession());
      newPDB4 = GetRecordAddressDB4(CopyRN);
      newPDB4->possession(copiedPossessions);
      newPDB4->groupIndex(0xd8);
      if (monsterSize == 2) newPDB4->groupIndex(0xff);

      // not needed. D3W = 0;
      //30OCT02SETWBITS10_10(DB4A3->word14, uw(0));
      //important copied; newPDB4->important(0);
      //30OCT02SETWBITS8_9(DB4A3->word14, P4);
      // already set. newPDB4->facing(facing);
      //30OCT02SETWBITS5_6(DB4A3->word14, numMonM1);
      // already set. newPDB4->numMonM1(numMonM1);
      // already set. newPDB4->invisible(invisible);
      // already set. newPDB4->drawAsSize4(drawAsSize4);
      /* already set.
      w_2 = sw(numMonM1);
      if (w_2 != 0)
      {
        D4W = 0;// Added to avoid compiler warning. Not needed.
        D5W = STRandom0_3(); // Monster position????
      }
      else
      {
        D4W = 255;
        D5W = 0; //Added to avoid compiler warning. Not needed.
      };
      ASSERT(DB4A3->monsterType() < 27);
      D6W = pmtDesc->uByte8[1];
      do
      {
        w_12 = sw(STRandom() % (D6W/4 + 1));
        D0W = uw(D6W * hitPointMultiplier + w_12);
        newPDB4->hitPoints6[numMonM1] = D0W;
        if (w_2 != 0)
        {
          D4W = SetMonsterPositionBits(
                    D4W,     //old positions byte
                    numMonM1, //monster number
                    D5W++);  //monster position
          if (pmtDesc->horizontalSize() == 1)
          {
            D5W++;
          };
          D5W &= 3;
        };
      } while ((numMonM1--) != 0);
      newPDB4->groupIndex(D4W);
      // The MoveObject call fixes DB4A3->uByte5.
      */


    };
    break;
  default:
    if (pdsaDbank->tracing)
    {
      PrintTrace(" &ADD of unimplemented type ");
    };
  };
  return CopyRN;
}

static RN CopyItemList (RN origRN)
{
  RN result;
  RN lastItem;
  RN newItem;
  for (result=RNeof; 
       origRN != RNeof;
       origRN = GetDBRecordLink(origRN))
  {
    newItem = CopyItem(origRN);
    if (result == RNeof) result = newItem;
    else db.GetCommonAddress(lastItem)->link(newItem);
    lastItem = newItem;
  };
  return result;
}



static void EX_ADD()
{
  LOCATIONREL locr;
  RN object, newObj, RNmon;
  ui32 objID;
  i32 iLocation;
  ui32 posMsk;
  i32 oldLevel;
  i32 chIdx=-1, posIdx=-1;
  newObj = RNeof;
  objID = pdsaDbank->stack.pop();
  iLocation = pdsaDbank->stack.pop();
  posMsk = pdsaDbank->stack.pop() & 15;
  NOFILTER
  locr.x = 0; //Compiler complaint about not being initialized.
  locr.y = 0; //Compiler complaint about not being initialized.
  // First we check that the destination is valid and empty
  if (iLocation < 0)
  {
    if (iLocation == -1)
    {
      if (d.objectInHand != RNnul) return;
    }
    else if (iLocation > -1000)
    {
      chIdx = (-iLocation)/100 - 1;
      posIdx = (-iLocation)%100;
      if (chIdx >= d.NumCharacter) return;
      if (posIdx > 29) return;
      if (d.CH16482[chIdx].Possession(posIdx) != RNnul) return;
    }
    else // Must be a monster
    {
      RNmon.checkIndirectIndex(-iLocation/1000);
      RNmon.ConstructFromInteger(-iLocation/1000);
      if (RNmon.dbType() != dbMONSTER)
      {
        if (pdsaDbank->tracing)
        {
          PrintTrace(" illegal monster in &ADD to monster");
        };
        return;
      };
    };
  }
  else
  {
    locr.Integer(iLocation);
    if (locr.l >= d.dungeonDatIndex->NumLevel()) return;
    if (locr.x > d.pLevelDescriptors[locr.l].LastColumn()) return;
    if (locr.y > d.pLevelDescriptors[locr.l].LastRow()) return;
  };
  
  if (!object.checkIndirectIndex(objID)) 
  {
    if (pdsaDbank->tracing)
    {
      PrintTrace(" illegal object in &ADD ");
    };
    return;
  };
  object.ConstructFromInteger(objID);
  newObj = CopyItem(object);
  pdsaDbank->mostRecentInterestingObjectID = newObj.ConvertToInteger();
  if (newObj != RNeof)
  {
    if (iLocation >= 0)
    {
      i32 m = posMsk, n = 0, p = 0;
      if (m != 0)
      {
        for (p=0; p<4; p++)
        {
          if (m&1) n++;
          m >>= 1;
        };
        n = STRandom(n);
        m = posMsk;
        for (p=0; p<4; p++)
        {
          if (m&1)
          {
            if (n==0) break;
            n--;
          };
          m >>= 1;
        };
      };
      oldLevel = d.LoadedLevel;
      LoadLevel(locr.l);
      MoveObject(newObj, -1, 0, locr.x, locr.y, NULL, NULL);
      LoadLevel(oldLevel);
      newObj.pos(p);
    }
    else //negative location - place on character
    {
      if (iLocation == -1)
      {
        CURSORFILTER_PACKET cfp;
        cfp.type = CURSORFILTER_DSA_ADD;
        cfp.object = newObj.ConvertToInteger();
        if (newObj != RNnul)
        {
          CursorFilter(&cfp);
        };
        ObjectToCursor(newObj, 1);
      }
      else if (iLocation > -1000)
      {
        AddCharacterPossession(chIdx,newObj,posIdx);
        DrawCharacterState(chIdx);
      }
      else //must be a monster
      {
        RN *pDest;
        DB4 *pMon;
        pMon = GetRecordAddressDB4(RNmon);
        pDest = pMon->pPossession();
        if (*pDest == RNeof) *pDest = newObj;
        else AddObjectToRoom(newObj, *pDest, -1, 0, NULL);
      };
    };
  };  
  return;
}


void Indirect(const char *m, i32 type, bool array, i32 numVar, i32 numParam)
{
  ui32 record[100], j;
  i32 i, r;
  static int indirectLocChecked = -1; // -1=not checked 0=nonexistent 1=OK
  static LOCATIONREL indirectLocr;
  
  
  
  ui32 key, *pRecord;
  i32 len;
  if (pdsaDbank->tracing)
  {
    PrintTrace(m);
  };

  if (indirectLocChecked < 0)
  {
    key = (EDT_SpecialLocations<<24)|ESL_INDIRECTACTIONS;
    len = expool.Locate(key,&pRecord);
    indirectLocChecked = 0;
    if (len > 0)
    {
      indirectLocr.Integer(*pRecord);
      indirectLocChecked = 1;
    };
  };
  if (indirectLocChecked < 1) return;

      
  
  
  
  
  
  
  
  r = 0;
  record[r++] = type;
  record[r++] = numVar;
  for (i=0; i<numVar; i++)
  {
    record[r++] = pdsaDbank->stack.pop();
  };
  record[r++] = array?1:0;
  if (array)
  {
    if (record[2] > 20) return;
    for (j=0; j<record[2]; j++)
    {
      record[r++] = pdsaDbank->Var(record[3]+j);
    };
  };
  record[r++] = numParam;
  for (i=0; i<numParam; i++)
  {
    record[r++] = pDSAparameters[i+1];
  };
  LOCATIONREL target;
  TIMER timer;
  ui32 timerID;
  i32 actionTime;

  
  
  
  
  if (r > 29) return; 
  
  if (pdsaDbank->tracing)
  {
    PrintTrace("MESSAGE ");
  };
  if (r > 29) 
  {
    if (pdsaDbank->tracing)
    {
        PrintTrace("Aborted - Exceeded 29 Parameters ");
    };
    return;
  };
  actionTime = d.Time;
  if (pdsaDbank->IndirectActionDelay > 0)
  {
    actionTime += pdsaDbank->IndirectActionDelay;
    pdsaDbank->IndirectActionDelay = -1;
  };
  //timer.timerTime = (indirectLocr.l << 24) | (actionTime & 0xffffff);
  timer.Time(actionTime);
  timer.Level((ui8)indirectLocr.l);
  timer.Function(TT_ParameterMessage);
  timer.timerUByte5((ui8)parameterMessageSequence++);
  timer.timerUByte6((ui8)indirectLocr.x);
  timer.timerUByte7((ui8)indirectLocr.y);
  timer.timerUByte8(0);
  timer.timerUByte9(0);
  timerID = gameTimers.SetTimer(&timer);
  key = (EDT_MessageParameters<<24) + timerID;
  expool.Write(key, record, r);
};



#define INDIRECT(m,num)     Indirect(m,subcode, false, num, 0);
#define INDIRECTA(m,num)    Indirect(m,subcode, true,  num, 0);
#define INDIRECTP(m,num, p) Indirect(m,subcode, false, num, p);


static ui32 Contents(LOCATIONREL& locr)
{
  if ((d.partyLevel == locr.l) && (d.partyX == locr.x) && (d.partyY == locr.y)) return 1<<29;
  if (FindFirstMonster(locr.l, locr.x, locr.y) != RNeof) return 1<<30;
  return 0x80000000;
}

ui8 teleporterBit[4] = {11,15,13,17};

static void ExamineCell(ui32 first, ui32 last, const char *trc)
{
  LOCATIONREL locr;
  LOCATIONREL cellLocr;
  DB1 *pTeleporter;
  ui32 what;
  ui32 criteriaMask;
  ui32 cellBits;
  ui32 resultbit = 1;
  ui32 result = 0;
  ui32 i;
  CELLFLAG cf;
  if (pdsaDbank->tracing)
  {
    fputs(trc, GETFILE(TraceFile));
  };
  criteriaMask = pdsaDbank->stack.pop();
  locr.Integer(pdsaDbank->stack.pop());
  for (i=first; i<=last; i++)
  {
    cellLocr = locr;
    cellBits = 0;
    switch(i)
    {
    case 0: cellLocr.y--; break;
    case 1: cellLocr.x++; break;
    case 2: cellLocr.y++; break;
    case 3: cellLocr.x--; break;
    };
    if (!cellLocr.IsValid())
    {
      cellBits |= (1 << 0) | (1 << 31);
    }
    else
    {
      cf = GetCellFlags(cellLocr);
      ROOMTYPE rt;
      rt = (ROOMTYPE)((cf >> 5) & 7);
      switch (rt)
      {
      case roomSTONE:
        cellBits |= (1 << 1) | (1 << 31);
        break;
      case roomOPEN:
        cellBits |= (1 << 19);
        cellBits |= Contents(cellLocr);
        break;
      case roomPIT:
        if (cf & 1) // if false pit
        {
          cellBits |= (1 << 4);
        }
        else
        {
          if (cf & 0x8) // if open
          {
            cellBits |= (1 << 2);
          }
          else
          {
            cellBits |= (1 << 3);
          };
        };
        cellBits |= Contents(cellLocr);
        break;
      case roomDOOR:
        cellBits = 1 << (((cf & 7) > 5) ? 9 : ((cf & 7) + 5));
        cellBits |= Contents(cellLocr);
        break;
      case roomTELEPORTER:
        {
          RN RNteleporter;
          RNteleporter = FindFirstTeleporter(cellLocr.l, cellLocr.x, cellLocr.y);
          if (RNteleporter != RNeof)
          {
            int bit;
            pTeleporter = GetRecordAddressDB1(RNteleporter);
            what = pTeleporter->what();
                // 0 = objects only
                // 1 = monsters only
                // 2 = party or objects only
                // 3 = anything
            bit = teleporterBit[what];
            bit += (((cf & 8) != 0) ? 0 : 1);
            cellBits |= (1 << bit);
          };
          cellBits |= Contents(cellLocr);
        };
        break;
      case roomFALSEWALL:
        if (cf & 0x04) cellBits |= 1 << 22;
        else if (cf & 0x01) cellBits |= 1 << 20;
        else cellBits |= 1 << 21;
        cellBits |= Contents(cellLocr);
        break;
      case roomSTAIRS:
        if (cf & 0x04) cellBits |= 1 << 23;
        else cellBits |= 1 << 24;
        cellBits |= Contents(cellLocr);
        break;
      };
    };
    if (   ((criteriaMask & cellBits & 0x01ffffff) != 0)
        && ((criteriaMask & cellBits & 0xe0000000) != 0)
      )
    {
      result |= resultbit;
    };
    resultbit <<= 1;
  };
  pdsaDbank->stack.push(result);
}


static void EX_AMPERSAND(EXECUTIONPACKET& exPkt, int cmdOffset)
{
  i32 relState;
  i32 v;
  STKOP subcode;
  RN obj;
  LOCATIONREL locr;
  DSAampersandCmd cmd;
  cmd = ((DSAcommand *)(exPkt.m_PC++))->ampersandCmd;
  exPkt.m_wordsRemaining--;
  if (cmd.IsMaxState())
  {
    relState = *(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  }
  else
  {
    relState = cmd.NextState();
  };
  exPkt.m_nextState = exPkt.m_curState + relState;
  subcode = (STKOP)(cmd.SubCode() + cmdOffset);
  if (subcode == STKOP_I_Indirect)
  {
    // Unpack the parameters into the stack, the array tempVar, and parameters)
    ui32 r, n, i, indx;
    r = 0;
    switch ((STKOP)pDSAparameters[1+r++])
    {
    case STKOP_I_Del:            subcode = STKOP_Del;          break;
    case STKOP_I_DelMon:         subcode = STKOP_DelMon;       break;
    case STKOP_I_InsMon:         subcode = STKOP_InsMon;       break;
    case STKOP_I_Add:            subcode = STKOP_Add;          break;
    case STKOP_I_CreateCloud:    subcode = STKOP_CreateCloud;  break;
    case STKOP_I_Cast:           subcode = STKOP_Cast;         break;
    case STKOP_I_FilteredCast:   subcode = STKOP_FilteredCast; break;
    case STKOP_I_TeleportParty:  subcode = STKOP_TeleportParty;break;
    case STKOP_I_MonsterStore:   subcode = STKOP_MonsterStore; break;
    case STKOP_I_CharStore:      subcode = STKOP_CharStore;    break;
    case STKOP_I_CausePoison:    subcode = STKOP_CausePoison;  break;
    case STKOP_I_Move:           subcode = STKOP_Move;         break;
    case STKOP_I_Copy:           subcode = STKOP_Copy;         break;
    case STKOP_I_CellStore:      subcode = STKOP_CellStore;    break;
    case STKOP_I_Throw:          subcode = STKOP_Throw;        break;
    case STKOP_I_SwapCharacter:  subcode = STKOP_SwapCharacter;break;
    default: return;
    };
    n = pDSAparameters[1+r++]; // Number stack variables.
    r += n;
    for (i=0; i<n; i++)
    {
      pdsaDbank->stack.push(pDSAparameters[--r+1]);
    };
    r += n;
    if (pDSAparameters[1+r++] != 0)
    {
      n = pDSAparameters[1+2];
      indx = pDSAparameters[1+3];
      for (i=0; i<n; i++)
      {
        pdsaDbank->Var(indx++,pDSAparameters[1+r++]);
      };
    };
    n = pDSAparameters[1+r++];
    for (i=0; i<n; i++)
    {
      pDSAparameters[1+i] = pDSAparameters[1+r++];
    };
  };
  switch(subcode)
  {
  case STKOP_2Drop:
    if (pdsaDbank->tracing)
    {
      PrintTrace("2DROP ");
    };
    pdsaDbank->stack.pop();
    pdsaDbank->stack.pop();
    break;
  case STKOP_1Plus:
    if (pdsaDbank->tracing)
    {
      PrintTrace("1+ ");
    };
    pdsaDbank->stack.push(pdsaDbank->stack.pop() + 1);
    break;
  case STKOP_Swap:
    if (pdsaDbank->tracing)
    {
      PrintTrace("SWAP ");
    };
    pdsaDbank->stack.roll(1);
    break;
  case STKOP_Neg:
    if (pdsaDbank->tracing)
    {
      PrintTrace("NEG ");
    };
    pdsaDbank->stack.push(-pdsaDbank->stack.pop());
    break;
  case STKOP_Comp:
    if (pdsaDbank->tracing)
    {
      PrintTrace("COMP ");
    };
    pdsaDbank->stack.push(~pdsaDbank->stack.pop());
    break;
  case STKOP_Dup:
    if (pdsaDbank->tracing)
    {
      PrintTrace("DUP ");
    };
    v = pdsaDbank->stack.pop();
    pdsaDbank->stack.push(v);
    pdsaDbank->stack.push(v);
    break;
  case STKOP_DSAInfoFetch:
    {
      RN dsaObj;
      int n;
      int dsatype=-1, dsastate=-1, dsaPA=-1,dsaPB=-1;
      if (pdsaDbank->tracing)
      {
        PrintTrace("DSAINFO@ ");
      };
      n = pdsaDbank->stack.pop();
      if (dsaObj.checkIndirectIndex(n))
      {
        dsaObj.ConstructFromInteger(n);
        if (dsaObj.dbType() == dbACTUATOR)
        {
          int acType;
          DB3 *pObj;
          pObj = GetRecordAddressDB3(dsaObj);
          acType = pObj->actuatorType();
          if (acType == 47)
          {
            dsatype = pObj->DSAselector();
            dsastate = pObj->DSAstate();
            dsaPA = pObj->ParameterA();
            dsaPB = pObj->ParameterB();
          };
        };
      };
      pdsaDbank->stack.push(dsatype);
      pdsaDbank->stack.push(dsastate);
      pdsaDbank->stack.push(dsaPA);
      pdsaDbank->stack.push(dsaPB);
    };
    break;
  case STKOP_Fetch:
    if (pdsaDbank->tracing)
    {
      PrintTrace("Fetch ");
    };
    v = pdsaDbank->stack.pop();
    pdsaDbank->stack.push(pdsaDbank->Var(v));
    break;
  case STKOP_Store:
    if (pdsaDbank->tracing)
    {
      PrintTrace("Store ");
    };
    v = pdsaDbank->stack.pop();
    pdsaDbank->Var(v, pdsaDbank->stack.pop());
    break;
  case STKOP_Del:
    if (pdsaDbank->tracing)
    {
      PrintTrace("DEL ");
    };
    EX_DEL();
    break;
  case STKOP_DelMon:
    if (pdsaDbank->tracing)
    {
      PrintTrace("DELMON ");
    };
    NOFILTER
    EX_DELMON();
    break;
  case STKOP_InsMon:
    if (pdsaDbank->tracing)
    {
      PrintTrace("INSMON ");
    };
    NOFILTER
    EX_INSMON();
    break;
  case STKOP_TimeFetch:
    if (pdsaDbank->tracing)
    {
      PrintTrace("TIME@ ");
    };
    pdsaDbank->stack.push(d.Time);
    break;
  case STKOP_Add:
    if (pdsaDbank->tracing)
    {
      PrintTrace("ADD ");
    };
    EX_ADD();
    break;
  case STKOP_Plus:
    if (pdsaDbank->tracing)
    {
      PrintTrace("+ ");
    };
    pdsaDbank->stack.push(pdsaDbank->stack.pop()+pdsaDbank->stack.pop());
    break;
  case STKOP_Gear:
    if (pdsaDbank->tracing)
    {
      PrintTrace("* ");
    };
    pdsaDbank->stack.push(pdsaDbank->stack.pop()*pdsaDbank->stack.pop());
    break;
  case STKOP_1Minus:
    if (pdsaDbank->tracing)
    {
      PrintTrace("1- ");
    };
    pdsaDbank->stack.push(pdsaDbank->stack.pop() - 1);
    break;
  case STKOP_Roll:
    if (pdsaDbank->tracing)
    {
      PrintTrace("ROLL ");
    };
    v = pdsaDbank->stack.pop();
    pdsaDbank->stack.roll(v);
    break;
  case STKOP_Rot:
    if (pdsaDbank->tracing)
    {
      PrintTrace("ROT ");
    };
    pdsaDbank->stack.roll(2);
    break;
  case STKOP_MinusRoll:
    if (pdsaDbank->tracing)
    {
      PrintTrace("-ROLL ");
    };
    v = pdsaDbank->stack.pop();
    pdsaDbank->stack.minusRoll(v);
    break;
  case STKOP_MinusRot:
    if (pdsaDbank->tracing)
    {
      PrintTrace("-ROT ");
    };
    pdsaDbank->stack.minusRoll(2);
    break;
  case STKOP_Pick:
    if (pdsaDbank->tracing)
    {
      PrintTrace("PICK ");
    };
    pdsaDbank->stack.pick(pdsaDbank->stack.pop());
    break;
  case STKOP_2Pick:
    if (pdsaDbank->tracing)
    {
      PrintTrace("2PICK ");
    };
    pdsaDbank->stack.pick(2);
    break;
  case STKOP_Over:
    if (pdsaDbank->tracing)
    {
      PrintTrace("OVER ");
    };
    pdsaDbank->stack.pick(1);
    break;
  case STKOP_Poke:
    if (pdsaDbank->tracing)
    {
      PrintTrace("POKE ");
    };
    pdsaDbank->stack.poke(pdsaDbank->stack.pop());
    break;
  case STKOP_Drop:
    if (pdsaDbank->tracing)
    {
      PrintTrace("DROP ");
    };
    pdsaDbank->stack.pop();
    break;
  case STKOP_Equal:
    if (pdsaDbank->tracing)
    {
      PrintTrace("= ");
    };
    pdsaDbank->stack.push((pdsaDbank->stack.pop()==pdsaDbank->stack.pop())?1:0);
    break;
  case STKOP_NotEqual:
    if (pdsaDbank->tracing)
    {
      PrintTrace("!=");
    };
    pdsaDbank->stack.push((pdsaDbank->stack.pop()==pdsaDbank->stack.pop())?0:1);
    break;
  case STKOP_Not:
    if (pdsaDbank->tracing)
    {
      PrintTrace("NOT ");
    };
    pdsaDbank->stack.push((pdsaDbank->stack.pop() == 0)?1:0);
    break;
  case STKOP_And:
    if (pdsaDbank->tracing)
    {
      PrintTrace("AND ");
    };
    pdsaDbank->stack.push(pdsaDbank->stack.pop() & pdsaDbank->stack.pop());
    break;
  case STKOP_Xor:
    if (pdsaDbank->tracing)
    {
      PrintTrace("XOR ");
    };
    pdsaDbank->stack.push(pdsaDbank->stack.pop() ^ pdsaDbank->stack.pop());
    break;
  case STKOP_Or:
    if (pdsaDbank->tracing)
    {
      PrintTrace("OR ");
    };
    pdsaDbank->stack.push(pdsaDbank->stack.pop() | pdsaDbank->stack.pop());
    break;
  case STKOP_Shift:
    if (pdsaDbank->tracing)
    {
      PrintTrace("SHIFT ");
    };
    {
      i32 V = pdsaDbank->stack.pop();
      i32 W = pdsaDbank->stack.pop();
      pdsaDbank->stack.push( (V>=0)?(W<<(V&31)):(W>>((-V)&31)));   
    };
    break;
  case STKOP_RShift:
    if (pdsaDbank->tracing)
    {
      PrintTrace("RSHIFT ");
    };
    {
      i32 V = pdsaDbank->stack.pop();
      i32 W = pdsaDbank->stack.pop();
      pdsaDbank->stack.push( (V>=0)?(W>>(V&31)):(W<<((-V)&31)));
    };
    break;
  case STKOP_Less:
    if (pdsaDbank->tracing)
    {
      PrintTrace("< ");
    };
    {
      i32 V = pdsaDbank->stack.pop();
      i32 W = pdsaDbank->stack.pop();
      pdsaDbank->stack.push( (W<V)?1:0 );
    };
    break;
  case STKOP_ULess:
    if (pdsaDbank->tracing)
    {
      PrintTrace("U< ");
    };
    {
      ui32 V = pdsaDbank->stack.pop();
      ui32 W = pdsaDbank->stack.pop();
      pdsaDbank->stack.push( (W<V)?1:0 );
    };
    break;
  case STKOP_Slash:
    if (pdsaDbank->tracing)
    {
      PrintTrace("/ ");
    };
    {
      i32 V = pdsaDbank->stack.pop();
      i32 W = pdsaDbank->stack.pop();
      pdsaDbank->stack.push(W/V);
    };
    break;
  case STKOP_Percent:
    if (pdsaDbank->tracing)
    {
      PrintTrace("% ");
    };
    {
      i32 V = pdsaDbank->stack.pop();
      i32 W = pdsaDbank->stack.pop();
      pdsaDbank->stack.push(W%V);
    };
    break;
  case STKOP_Random:
    {
      ui32 V;
      if (pdsaDbank->tracing)
      {
        PrintTrace("RANDOM ");
      };
      V = pdsaDbank->stack.pop();
      if (V == 0) V = 1;
      V = (ui32)STRandom() % V;
      pdsaDbank->stack.push(V);
    };
    break;
  case STKOP_ObjectID:
    if (pdsaDbank->tracing)
    {
      PrintTrace("OBJECTID ");
    };
    pdsaDbank->stack.push(pdsaDbank->mostRecentInterestingObjectID);
    break;
  case STKOP_CreateCloud:
    {
      i32 type;
      i32 size;
      i32 oldLevel;
      RN spellObj = RNeof;
      size = pdsaDbank->stack.pop();
      type = pdsaDbank->stack.pop();
      locr.Integer(pdsaDbank->stack.pop());
      NOFILTER
      if (pdsaDbank->tracing)
      {
        PrintTrace("CREATECLOUD");
      };
      switch (type)
      {
      case 0:  spellObj = RNFireball;        break;
      case 3:  spellObj = RNDispellMissile;  break;
      case 4:  spellObj = RNZoSpell;         break;
      case 7:  spellObj = RNPoisonCloud;     break;
      case 40: spellObj = RNMonsterDeath;    break;
      case 50: spellObj = RNFluxCage;        break;
      default:
        if (pdsaDbank->tracing)
        {
          PrintTrace("Illegal cloud type");
        };
        break;
      };
      if (spellObj != RNeof)
      {
        oldLevel = d.LoadedLevel;
        if (locr.l != oldLevel) LoadLevel(locr.l);
        if (spellObj == RNFluxCage)
        {
          CreateFluxcage(locr.x, locr.y);
        }
        else
        {
          CreateCloud(spellObj, (i16)size, 
                     (i16)locr.x,(i16)locr.y,
                     (i16)locr.x,(i16)locr.y,(i16)locr.p,
                     NULL);
        };
        if (oldLevel != d.LoadedLevel) LoadLevel(oldLevel);
      };
    };
    break;
  case STKOP_Type:
    if (pdsaDbank->tracing)
    {
      PrintTrace("TYPE ");
    };
    EX_TYPE();
    break;
  case STKOP_MissileInfoFetch:
    {
      i32 k, range=-1, damage=-1, contents=-1, direction=-1;
      if (pdsaDbank->tracing)
      {
        PrintTrace("MISSILEINFO@ ");
      };
      k = pdsaDbank->stack.pop();
      if (obj.checkIndirectIndex(k))
      {
        obj.ConstructFromInteger(k);
        if (obj.dbType() == dbMISSILE)
        {
          DB14 *pMissile;
          HTIMER hTimer;
          pMissile = GetRecordAddressDB14(obj);
          contents = pMissile->flyingObject().ConvertToInteger();
          range = pMissile->rangeRemaining();
          damage = pMissile->damageRemaining();
          hTimer = pMissile->timerIndex();
          direction = (gameTimers.pTimer(hTimer)->timerWord8()>>10) & 3;
        };
      };
      pdsaDbank->stack.push(contents);
      pdsaDbank->stack.push(range);
      pdsaDbank->stack.push(damage);
      pdsaDbank->stack.push(direction);
    };
    break;
  case STKOP_MissileInfoStore:
    {
      i32 k, range, damage, direction;
      if (pdsaDbank->tracing)
      {
        PrintTrace("MISSILEINFO! ");
      };
      k = pdsaDbank->stack.pop();
      if (obj.checkIndirectIndex(k))
      {
        obj.ConstructFromInteger(k);
        direction = pdsaDbank->stack.pop();
        damage = pdsaDbank->stack.pop();
        range = pdsaDbank->stack.pop();
        if (obj.dbType() == dbMISSILE)
        {
          DB14 *pMissile;
          HTIMER hTimer;
          pMissile = GetRecordAddressDB14(obj);
          pMissile->rangeRemaining(range);
          pMissile->damageRemaining(damage);
          hTimer = pMissile->timerIndex();
          gameTimers.pTimer(hTimer)->timerWord8()
              = (i16)((gameTimers.pTimer(hTimer)->timerWord8() & (~(0x3 << 10))) | ((direction & 3) << 10));
        };
      };
    };
    break;
  case STKOP_SetNewState:
    if (pdsaDbank->tracing)
    {
      PrintTrace("SETNEWSTATE ");
    };
    pdsaDbank->forceState = pdsaDbank->stack.pop();
    break;
  case STKOP_FalsePit: //(target bool ...)
    {
      i32 V = pdsaDbank->stack.pop();
      i32 W = pdsaDbank->stack.pop();
      CELLFLAG *pcf;
      if (pdsaDbank->tracing)
      {
        PrintTrace("FalsePit ");
      };
      locr.Integer(W);
      if (!locr.IsValid()) break; // if bad location
      pcf = GetCellFlagsAddress(locr.l, locr.x, locr.y);
      if (RoomType(*pcf) != roomPIT) break;
      if (V != 0) *pcf |= 1;
      else *pcf &= ~1;
    };
    break;
  case STKOP_GeneratorDelayStore:
    {
      i32 W = pdsaDbank->stack.pop(); //location
      i32 V = pdsaDbank->stack.pop(); //delay
      DBTYPE dbType;
      CELLFLAG *pcf;
      RN object;
      i32 at;
      DB3 *pDB3, *pDisabled;
      locr.Integer(W);
      if (pdsaDbank->tracing)
      {
        PrintTrace("GeneratorDelay! ");
      };
      if (!locr.IsValid()) break; // if bad location
      pcf = GetCellFlagsAddress(locr.l, locr.x, locr.y);
      if (RoomType(*pcf) == roomSTONE) break; //bad location
      pDisabled = pDB3 = NULL;
      for (object = FindFirstObject(locr.l,locr.x,locr.y);
           object != RNeof;
           object = GetDBRecordLink(object))
      {
        dbType = object.dbType();
        if (dbType != dbACTUATOR) continue;
        pDB3 = GetRecordAddressDB3(object);
        at = pDB3->actuatorType();
        if (at == 6)
        {
          pDB3->disableTime(V);
          break;
        };
        if ((at == 0) && (pDisabled == NULL)) pDisabled = pDB3;
        pDB3 = NULL;
      };
      if ((pDB3 == NULL) && (pDisabled != NULL))
      {
        pDisabled->disableTime(V);
      };
    };
    break;
  case STKOP_Overlay:
    {
      ui32 p1, p2, p3, p4, onum;
      if (pdsaDbank->tracing)
      {
        PrintTrace("Overlay ");
      };
      onum = pdsaDbank->stack.pop();
      p4 = pdsaDbank->stack.pop();
      p3 = pdsaDbank->stack.pop();
      p2 = pdsaDbank->stack.pop();
      p1 = pdsaDbank->stack.pop();
      SelectOverlay(onum, p1, p2, p3, p4);
    };
    break;
  case STKOP_Palette:
    {
      ui32 p1, p2, density, onum;
      if (pdsaDbank->tracing)
      {
        PrintTrace("Palette ");
      };
      density = pdsaDbank->stack.pop();
      onum = pdsaDbank->stack.pop();
      p2 = pdsaDbank->stack.pop();
      p1 = pdsaDbank->stack.pop();
      SetOverlayPalette(onum, p1, p2, density);
    };
    break;
  case STKOP_DisableSaves:
    {
      ui32 k;
      if (pdsaDbank->tracing)
      {
        PrintTrace("DisableSaves ");
      };
      k = pdsaDbank->stack.pop();
      disableSaves = k != 0;
    };
    break;
  case STKOP_ParamFetch:
    {
      ui32 i, j, n;
      if (pdsaDbank->tracing)
      {
        PrintTrace("PARAM@ {");
      };
      i = pdsaDbank->stack.pop();
      n = pdsaDbank->stack.pop();
      if (n > 100)
      {
        message("DSA &PARAM@ too many parameters");
      }
      else
      {
        //memcpy(pdsaDbank->stack.tempVar+i, pDSAparameters, 4*n);
        if (n > pDSAparameters[0]) 
        {
          message("DSA &PARAM@ non-existent parameter");
          for (j=pDSAparameters[0]; j<n; j++)
          {
            pdsaDbank->Var(0);
            pdsaDbank->SetVarState(i+j, DVT_NonParameter);
          };
          n = pDSAparameters[0];
        };
        for (j=0; j<n; j++)
        {
          pdsaDbank->Var(i+j, pDSAparameters[1+j]);
        };
      };
      if (pdsaDbank->tracing)
      {
        ui32 k;
        char l[30];
        for (k=0; k<n; k++)
        {
          sprintf(l," %d",pDSAparameters[1+k]);
          PrintTrace(l);
        };
        PrintTrace("}");
      };
    };
    break;
  case STKOP_ParamStore:
    {
      i32 n, i, j;
      if (pdsaDbank->tracing)
      {
        PrintTrace("PARAM! {");
      };
      i = pdsaDbank->stack.pop();
      n = pdsaDbank->stack.pop();
      if (n > 100)
      {
        message("DSA &PARAM! too many parameters");
      }
      else
      {
        //memcpy(pDSAparameters, pdsaDbank->stack.tempVar+i, 4*n);
        for (j=0; j<n; j++)
        {
          if (pdsaDbank->GetVarState(i+j) != DVT_NonParameter) pDSAparameters[1+j] = pdsaDbank->Var(i+j);
        };
      };
      if (pdsaDbank->tracing)
      {
        i32 k;
        char l[30];
        for (k=0; k<n; k++)
        {
          sprintf(l," %d",pDSAparameters[1+k]);
          PrintTrace(l);
        };
        PrintTrace("}");
      };
    };
    break;
  case STKOP_SetAdjustSkillsParameters:
    if (pdsaDbank->tracing)
    {
      PrintTrace("SETADJUSTSKILLSPARAMETERS! ");
    };
    adjustSkillsParameters[4] = pdsaDbank->stack.pop();
    adjustSkillsParameters[3] = pdsaDbank->stack.pop();
    adjustSkillsParameters[2] = pdsaDbank->stack.pop();
    adjustSkillsParameters[1] = pdsaDbank->stack.pop();
    adjustSkillsParameters[0] = pdsaDbank->stack.pop();
    break;
  case STKOP_Message:
    {
      LOCATIONREL target;
      TIMER timer;
      ui32 delay;
      ui32 msgType;
      ui32 numParam;
      ui32 timerID;
      i32 actionTime;
      ui32 key;
      delay = pdsaDbank->stack.pop();
      numParam = pdsaDbank->stack.pop();
      msgType = pdsaDbank->stack.pop();
      target.Integer(pdsaDbank->stack.pop());
      if (pdsaDbank->tracing)
      {
        PrintTrace("MESSAGE ");
      };
      if (numParam > 29) 
      {
        if (pdsaDbank->tracing)
        {
            PrintTrace("Aborted - Exceeded 29 Parameters ");
        };
        break;
      };
      if (Override_P)
      {
        target.p = Override_Pos;
        Override_P = false;
      };
      actionTime = d.Time + delay;
      //timer.timerTime = (target.l << 24) | actionTime;
      timer.Time(actionTime);
      timer.Level((ui8)target.l);
      timer.Function(TT_ParameterMessage);
      timer.timerUByte5((ui8)parameterMessageSequence++);
      timer.timerUByte6((ui8)target.x);
      timer.timerUByte7((ui8)target.y);
      timer.timerUByte8((ui8)target.p);
      timer.timerUByte9((ui8)msgType);
      timerID = gameTimers.SetTimer(&timer);
      key = (EDT_MessageParameters<<24) + timerID;
      expool.Write(key, pDSAparameters+1, numParam);
    };
    break;
  case STKOP_Cast:
    NOFILTER
    if (pdsaDbank->tracing)
    {
      PrintTrace("CAST ");
    };
    DSACastSpell(false);
    break;
  case STKOP_FilteredCast:
    NOFILTER
    if (pdsaDbank->tracing)
    {
      PrintTrace("FILTEREDCAST ");
    };
    DSACastSpell(true);
    break;
  case STKOP_GetSkin:
    {
      ui32 loc, level, x, y, skin;
      if (pdsaDbank->tracing)
      {
        PrintTrace("GETSKIN ");
      };
      loc = pdsaDbank->stack.pop();
      y = (loc >> 0) & 31;
      x = (loc >> 5) & 31;
      level = (loc >> 10) & 63;
      skin = skinCache.GetSkin(level, x, y);
      pdsaDbank->stack.push(skin);
    };
    break;
  case STKOP_SetSkin:
    {
      ui32 loc, level, x, y, skin;
      if (pdsaDbank->tracing)
      {
        PrintTrace("SETSKIN ");
      };
      loc = pdsaDbank->stack.pop();
      y = (loc >> 0) & 31;
      x = (loc >> 5) & 31;
      level = (loc >> 10) & 63;
      skin = pdsaDbank->stack.pop();
      skinCache.SetSkin(level, x, y,(ui8)skin);
    };
    break;
  case STKOP_Say:
    {
      ui32 iloc, color;
      char ExpandedText[1001];
      RN RNtext;
      if (pdsaDbank->tracing)
      {
        PrintTrace("SAY ");
      };
      color = pdsaDbank->stack.pop();
      iloc = pdsaDbank->stack.pop();
      locr.Integer(iloc);
      if (!locr.IsValid()) break;
      RNtext = FindFirstText(locr);
      if (RNtext == RNeof) break;
      DecodeText(ExpandedText,//+1,
                 //GetRecordAddressDB2(d.rn10540),
                 GetRecordAddressDB2(RNtext),
                 (ui16)0x8002,
                 999);
      //ExpandedText[0] = 10;
      PrintWithSubstitution(ExpandedText,color, false);
    };
    break;
  case STKOP_DiscardText:
    if (pdsaDbank->tracing)
    {
      PrintTrace("DISCARDTEXT");
    };
    scrollingText.DiscardText();
    break;
  case STKOP_TextFetch:
    {
      ui32 textIndex, objIndex;
      i32 len;
      char ExpandedText[1001];
      RN RNtext;
      if (pdsaDbank->tracing)
      {
        PrintTrace("TEXT@ ");
      };
      textIndex = pdsaDbank->stack.pop();
      objIndex = pdsaDbank->stack.pop();
      if (textIndex > 9) break;
      if (!RNtext.checkIndirectIndex(objIndex)) break;
      RNtext.ConstructFromInteger(objIndex);
      if (RNtext == RNeof) break;
      if (RNtext.dbNum() != dbTEXT) break;
      DecodeText(ExpandedText,
                 //GetRecordAddressDB2(d.rn10540),
                 GetRecordAddressDB2(RNtext),
                 (ui16)0x8002,
                 999);
      len = strlen(ExpandedText);
      pdsaDbank->SetText(ExpandedText, len, textIndex);
    };
    break;
  case STKOP_GlobalTextStore:
    {
      ui32 localIndex, globalIndex;
      const char *text;
      globalIndex = pdsaDbank->stack.pop();
      localIndex = pdsaDbank->stack.pop();
      text = pdsaDbank->GetText(localIndex);
      SetGlobalText(text, globalIndex);
    };
    break;
  case STKOP_CharNameFetch:
    {
      ui32 index, fingerprint;
      int i;
      char *pName = NULL;
      if (pdsaDbank->tracing)
      {
        PrintTrace("CharName@@ ");
      };
      index = pdsaDbank->stack.pop() & 0xffff;
      fingerprint = pdsaDbank->stack.pop() & 0xffff;
      for (i=0; i<d.NumCharacter; i++)
      {
        if (d.CH16482[i].fingerPrint == fingerprint)
        {
          pName = d.CH16482[i].name;
          break;
        };
      };
      if (i == d.NumCharacter)
      {
        CHARDESC *pChar;
        pChar = d.CH16482[0].LocateInWings(fingerprint);
        if (pChar != NULL) pName = pChar->name;
      };
      if (pName != NULL)
      {
        pdsaDbank->SetText(pName, 8, index);
      }
      else
      {
        pdsaDbank->SetText("",0,index);
      };
    };
    break;
  case STKOP_TextSay:
    {
      ui32 index, color;
      const char *text;
      if (pdsaDbank->tracing)
      {
        PrintTrace("TEXTSAY ");
      };
      color = pdsaDbank->stack.pop();
      index = pdsaDbank->stack.pop();
      text = pdsaDbank->GetText(index);
      PrintWithSubstitution(text, color, false);
    };
    break;
  case STKOP_Loc2AbsCoord:
    {
      ui32 iLoc;
      LOCATIONABS loca;
      if (pdsaDbank->tracing)
      {
        PrintTrace("LOC2ABSCOORD ");
      };
      iLoc = pdsaDbank->stack.pop();
      locr.Integer(iLoc);
      loca = locr;
      pdsaDbank->stack.push(loca.l);
      pdsaDbank->stack.push(loca.x);
      pdsaDbank->stack.push(loca.y);
      pdsaDbank->stack.push(loca.p);
    };
    break;
  case STKOP_FetchExCellFlg:
    {
      ui32 *pRecord;
      ui32 iLoc;
      ui32 flags, mask;
      i32 len, i;
      if (pdsaDbank->tracing)
      {
        PrintTrace("ECF@ ");
      };
      iLoc = pdsaDbank->stack.pop();
      locr.Integer(iLoc);
      len = expool.Locate(
             (EDT_ExtendedCellFlags<<24)|(locr.l<<5)|locr.x,
             &pRecord);
      flags = 0;
      if (len >= 8)
      {
        mask = 1<<locr.y;
        for (i=0; i<8; i++)
        {
          if (pRecord[i] & mask) flags |= 0x100;
          flags >>= 1;
        };
      };
      pdsaDbank->stack.push(flags);
    };
    break;
  case STKOP_StoreExCellFlg:
    {
      ui32 mask, i, loc, flags, record[8];
      i32 len;
      if (pdsaDbank->tracing)
      {
        PrintTrace("ECF! ");
      };
      loc = pdsaDbank->stack.pop();
      flags = pdsaDbank->stack.pop();
      locr.Integer(loc);
      len = expool.Read(
             (EDT_ExtendedCellFlags<<24)|(locr.l<<5)|locr.x,
             record,
             8);
      if (len < 8)
      {
        memset(record, 0, 32);
      };
      mask = 1<<locr.y;
      for (i=0; i<8; i++)
      {
        record[i] &= ~mask;
        if (flags & 1) record[i] |= mask;
        flags >>= 1;
      };
      expool.Write(
             (EDT_ExtendedCellFlags<<24)|(locr.l<<5)|locr.x,
             record,
             8);
    };
    break;
  case STKOP_ChPoss:
    {
      ui32 possindex;
      i32 chindex;
      i32 objIndex;
      RN object;
      if (pdsaDbank->tracing)
      {
        PrintTrace("CHPOSS ");
      };
      possindex = pdsaDbank->stack.pop();
      chindex = pdsaDbank->stack.pop();
      if (chindex > 4) chindex = 0;
      if (chindex == 4) chindex = d.HandChar;
      if (possindex > 29) possindex = 0;
      if (chindex >= 0)
      {
        object = d.CH16482[chindex].Possession(possindex);
      }
      else
      {
        object = d.objectInHand;
      };
      if (object == RNnul) objIndex = -1;
      else objIndex = object.ConvertToInteger();
      pdsaDbank->stack.push(objIndex);
    };
    break;
  case STKOP_MonPoss:
    {
      ui32 possindex;
      ui32 idx;
      i32 objIndex = -1;
      DB4 *pMon;
      RN object, monster;
      if (pdsaDbank->tracing)
      {
        PrintTrace("MONPOSS ");
      };
      possindex = pdsaDbank->stack.pop();
      idx = pdsaDbank->stack.pop();
      if (monster.checkIndirectIndex(idx))
      {
        monster.ConstructFromInteger(idx);
        if (monster.dbType() == dbMONSTER)
        {
          pMon = GetRecordAddressDB4(monster);
          for ( object=pMon->possession(); 
                (possindex>0)&&(object!=RNeof);
                object=GetDBRecordLink(object))
          {
            possindex--;
          };
        };
      };
      if (object!=RNeof) objIndex=object.ConvertToInteger();
      pdsaDbank->stack.push(objIndex);
    };
    break;
  case STKOP_Mastery:
    {
      ui32 chIndex;
      ui32 skillIndex, flags;
      flags = pdsaDbank->stack.pop();
      skillIndex = pdsaDbank->stack.pop();
      chIndex = pdsaDbank->stack.pop();
      if (chIndex == 4) chIndex = d.HandChar; 
      if (pdsaDbank->tracing)
      {
        PrintTrace("MASTERY ");
      };
      if ((chIndex >= (ui32)d.NumCharacter) || (skillIndex >= 20))
      {
        pdsaDbank->stack.push(0);
        break;
      };
      if (flags & 1) skillIndex |= 0x4000;
      if (flags & 2) skillIndex |= 0x8000;
      pdsaDbank->stack.push(DetermineMastery(chIndex, skillIndex, NULL));
    };
    break;
  case STKOP_GetCurse:
    {
      ui32 objIndex, result=0;
      RN object;
      if (pdsaDbank->tracing)
      {
        PrintTrace("GetCurse ");
      };
      objIndex = pdsaDbank->stack.pop();
      if (object.checkIndirectIndex(objIndex))
      {
        object.ConstructFromInteger(objIndex);
        switch(object.dbType())
        {
        case dbWEAPON: 
            result = GetRecordAddressDB5(object)->cursed();
            break;
        case dbCLOTHING: 
            result = GetRecordAddressDB6(object)->cursed();
            break;
        case dbMISC: 
            result = GetRecordAddressDB10(object)->cursed();
            break;
        default: break; //result is 0
        };
      };
      pdsaDbank->stack.push(result);
    };
    break;
  case STKOP_SetCurse:
    {
      ui32 objIndex;
      bool curse;
      RN object;
      if (pdsaDbank->tracing)
      {
        PrintTrace("SetCurse ");
      };
      curse = pdsaDbank->stack.pop() != 0;
      objIndex = pdsaDbank->stack.pop();
      if (object.checkIndirectIndex(objIndex))
      {
        object.ConstructFromInteger(objIndex);
        SetCursed(object, curse);
      };
    };
    break;
  case STKOP_GetBroken:
    {
      ui32 objIndex, result=0;
      RN object;
      if (pdsaDbank->tracing)
      {
        PrintTrace("GetBroken ");
      };
      objIndex = pdsaDbank->stack.pop();
      if (object.checkIndirectIndex(objIndex))
      {
        object.ConstructFromInteger(objIndex);
        switch(object.dbType())
        {
        case dbWEAPON: 
            result = GetRecordAddressDB5(object)->broken();
            break;
        case dbCLOTHING: 
            result = GetRecordAddressDB6(object)->broken();
            break;
        default: break; //result is 0
        };
      };
      pdsaDbank->stack.push(result);
    };
    break;
  case STKOP_SetBroken:
    {
      ui32 objIndex;
      bool broken;
      RN object;
      if (pdsaDbank->tracing)
      {
        PrintTrace("SetBroken ");
      };
      broken = pdsaDbank->stack.pop() != 0;
      objIndex = pdsaDbank->stack.pop();
      if (object.checkIndirectIndex(objIndex))
      {
        object.ConstructFromInteger(objIndex);
        switch(object.dbType())
        {
        case dbWEAPON: 
            GetRecordAddressDB5(object)->broken(broken);
            break;
        case dbCLOTHING: 
            GetRecordAddressDB6(object)->broken(broken);
            break;
        default: break;
        };
      };
    };
    break;
  case STKOP_GetPoisoned:
    {
      ui32 objIndex, result=0;
      RN object;
      if (pdsaDbank->tracing)
      {
        PrintTrace("GetPoisoned ");
      };
      objIndex = pdsaDbank->stack.pop();
      if (object.checkIndirectIndex(objIndex))
      {
        object.ConstructFromInteger(objIndex);
        switch(object.dbType())
        {
        case dbWEAPON: 
            result = GetRecordAddressDB5(object)->poisoned();
            break;
        //case dbCLOTHING: 
        //    result = GetRecordAddressDB6(object)->poisoned();
        //    break;
        default: break; //result is 0
        };
      };
      pdsaDbank->stack.push(result);
    };
    break;
  case STKOP_SetPoisoned:
    {
      ui32 objIndex;
      bool poisoned;
      RN object;
      if (pdsaDbank->tracing)
      {
        PrintTrace("SetPoisoned ");
      };
      poisoned = pdsaDbank->stack.pop() != 0;
      objIndex = pdsaDbank->stack.pop();
      if (object.checkIndirectIndex(objIndex))
      {
        object.ConstructFromInteger(objIndex);
        switch(object.dbType())
        {
        case dbWEAPON: 
            GetRecordAddressDB5(object)->poisoned(poisoned);
            break;
        //case dbCLOTHING: 
        //    GetRecordAddressDB6(object)->poisoned(poisoned);
        //    break;
        default: break;
        };
      };
    };
    break;
  case STKOP_GetCharges:
    {
      ui32 objIndex, result=0;
      RN object;
      if (pdsaDbank->tracing)
      {
        PrintTrace("GetCharges ");
      };
      objIndex = pdsaDbank->stack.pop();
      if (object.checkIndirectIndex(objIndex))
      {
        object.ConstructFromInteger(objIndex);
        switch(object.dbType())
        {
        case dbWEAPON: 
            result = GetRecordAddressDB5(object)->charges();
            break;
        case dbCLOTHING:
            result = GetRecordAddressDB6(object)->charges();
            break;
        case dbPOTION:
            result = GetRecordAddressDB8(object)->strength();
            break;
        case dbMISC:
            result = GetRecordAddressDB10(object)->value();
            break;
        default: break; //result is 0
        };
      };
      pdsaDbank->stack.push(result);
    };
    break;
  case STKOP_SetCharges:
    {
      ui32 objIndex;
      ui32 charges;
      RN object;
      if (pdsaDbank->tracing)
      {
        PrintTrace("SetCharges ");
      };
      charges = pdsaDbank->stack.pop();
      objIndex = pdsaDbank->stack.pop();
      if (object.checkIndirectIndex(objIndex))
      {
        object.ConstructFromInteger(objIndex);
        switch(object.dbType())
        {
        case dbWEAPON: 
            GetRecordAddressDB5(object)->charges(charges);
            break;
        case dbCLOTHING:
            GetRecordAddressDB6(object)->charges(charges);
            break;
        case dbPOTION:
            GetRecordAddressDB8(object)->strength(charges);
            break;
        case dbMISC:
            GetRecordAddressDB10(object)->value(charges);
            break;
        default: break;
        };
      };
    };
    break;
  case STKOP_GetSubType:
    {
      ui32 objIndex, result=0;
      RN object;
      if (pdsaDbank->tracing)
      {
        PrintTrace("GetSubType ");
      };
      objIndex = pdsaDbank->stack.pop();
      if (object.checkIndirectIndex(objIndex))
      {
        object.ConstructFromInteger(objIndex);
        switch(object.dbType())
        {
        case dbMISC:
            result = GetRecordAddressDB10(object)->subType();
            break;
        default: break; //result is 0
        };
      };
      pdsaDbank->stack.push(result);
    };
    break;
  case STKOP_SetSubType:
    {
      ui32 objIndex;
      ui32 subType;
      RN object;
      if (pdsaDbank->tracing)
      {
        PrintTrace("SetSubType ");
      };
      subType = pdsaDbank->stack.pop();
      objIndex = pdsaDbank->stack.pop();
      if (object.checkIndirectIndex(objIndex))
      {
        object.ConstructFromInteger(objIndex);
        switch(object.dbType())
        {
        case dbMISC:
            GetRecordAddressDB10(object)->subType(subType);
            break;
        default: break;
        };
      };
    };
    break;
  case STKOP_CellFetch:
    {
      CELLFLAG cf;
      ROOMTYPE rt;
      ui32 j;
      ui32 arrayIndex, arraySize;
      if (pdsaDbank->tracing)
      {
        PrintTrace("Cell@ ");
      };
      arraySize = pdsaDbank->stack.pop();
      arrayIndex = pdsaDbank->stack.pop();
      if (arrayIndex < 0) break;
      if (arrayIndex + arraySize > 100) break;
      //memset (pdsaDbank->stack.tempVar + arrayIndex, 0, 4*arraySize);
      for (j=0; j<arraySize; j++)
      {
        pdsaDbank->Var(arrayIndex+j, 0);
      };
      locr.Integer(pdsaDbank->stack.pop());
      if (!locr.IsValid()) break;
      cf = GetCellFlags(locr);
      rt = RoomType(cf);
      if (arraySize == 0) break;
      pdsaDbank->Var(arrayIndex + 0, rt);
      if (arraySize < 2) break;
      switch (rt)
      {
      case roomSTONE:
        {
          if (cf & 0x8)
            //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x01;
            pdsaDbank->Var(arrayIndex+1, pdsaDbank->Var(arrayIndex+1) | 0x01);
          if (cf & 0x4)
            //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x02;
            pdsaDbank->Var(arrayIndex+1, pdsaDbank->Var(arrayIndex+1) | 0x02);
          if (cf & 0x2)
            //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x04;
            pdsaDbank->Var(arrayIndex+1, pdsaDbank->Var(arrayIndex+1) | 0x04);
          if (cf & 0x1)
            //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x08;
            pdsaDbank->Var(arrayIndex+1, pdsaDbank->Var(arrayIndex+1) | 0x08);
        };
        break;
      case roomOPEN:
        {
          if (cf & 0x08)
            //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x01;
            pdsaDbank->Var(arrayIndex+1, pdsaDbank->Var(arrayIndex+1) | 0x01);
        };
        break;
      case roomTELEPORTER:
        {
          DB1 *pdbTELEPORTER;
          LOCATIONREL dest;
          pdbTELEPORTER = FirstObjectRecordAddressDB1(locr);
          if (pdbTELEPORTER == NULL) break;
          if (cf & 0x08)
            //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x08;
            pdsaDbank->Var(arrayIndex+1, pdsaDbank->Var(arrayIndex+1) | 0x08);
          if (cf & 0x04)
            //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x04;
            pdsaDbank->Var(arrayIndex+1, pdsaDbank->Var(arrayIndex+1) | 0x04);
          if (arraySize < 3) break;
          //facing/turning
          pdsaDbank->Var(arrayIndex+2,
            pdbTELEPORTER->rotation() | (pdbTELEPORTER->facingMode() << 2));
          if (arraySize < 4) break;
          //what to teleport.
          pdsaDbank->Var(arrayIndex+3, 
            pdbTELEPORTER->what());
          //destination
          if (arraySize < 5) break;
          dest.l = pdbTELEPORTER->destLevel();
          dest.x = pdbTELEPORTER->destX();
          dest.y = pdbTELEPORTER->destY();
          dest.p = 0;
          pdsaDbank->Var(arrayIndex+4,
            dest.Integer());
        };
        break;
      case roomSTAIRS:
        {
          if (cf & 0x04)
            //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x04;
            pdsaDbank->Var(arrayIndex+1, pdsaDbank->Var(arrayIndex+1) | 0x04);
          if (cf & 0x08)
            //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x08;
            pdsaDbank->Var(arrayIndex+1, pdsaDbank->Var(arrayIndex+1) | 0x08);
        };
        break;
      case roomPIT:
        {
          if (cf & 0x01)
            //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x01;
            pdsaDbank->SetVarBit(arrayIndex+1, 0x01);
          if (cf & 0x04)
            //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x04;
            pdsaDbank->SetVarBit(arrayIndex+1, 0x04);
          if (cf & 0x08)
            //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x08;
            pdsaDbank->SetVarBit(arrayIndex+1, 0x08);
        };
        break;
      case roomFALSEWALL:
        {
          if (cf & 0x01)
            //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x01;
            pdsaDbank->SetVarBit(arrayIndex+1, 0x01);
          if (cf & 0x04)
            //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x04;
            pdsaDbank->SetVarBit(arrayIndex+1, 0x04);
        };
        break;
      case roomDOOR:
        {
          DB0 *pdbDOOR;
          pdbDOOR = FirstObjectRecordAddressDB0(locr);
          if (pdbDOOR == NULL) break;
          pdsaDbank->Var(arrayIndex+1, 0);
          if (cf & 0x04)
                  //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x01;
                  pdsaDbank->SetVarBit(arrayIndex+1, 0x01);
          if (pdbDOOR->mode())
                  //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x02;
                  pdsaDbank->SetVarBit(arrayIndex+1, 0x02);
          if (pdbDOOR->doorSwitch())
                  //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x04;
                  pdsaDbank->SetVarBit(arrayIndex+1, 0x04);
          if (pdbDOOR->canOpenWithFireball())
                  //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x08;
                  pdsaDbank->SetVarBit(arrayIndex+1, 0x08);
          if (pdbDOOR->canOpenWithAxe())
                  //pdsaDbank->stack.tempVar[arrayIndex+1] |= 0x10;
                  pdsaDbank->SetVarBit(arrayIndex+1, 0x10);
          if (arraySize < 3) break;
          //Open/closed state goes to word 2
          pdsaDbank->Var(arrayIndex+2, cf & 7);
          if (arraySize < 4) break;
          //Door type is next (wood, iron, etc).
          pdsaDbank->Var(arrayIndex+3, pdbDOOR->doorType());
          if (arraySize < 5) break;
          //Decoration ordinal is next.
          pdsaDbank->Var(arrayIndex+4, pdbDOOR->ornateness());
        };
        break;
      }; //switch(roomtype)
      if (pdsaDbank->tracing)
      {
        ui32 i;
        char l[30];
        PrintTrace("{");
        for (i=0; i<arraySize; i++)
        {
          sprintf(l," %d",pdsaDbank->Var(arrayIndex+i));
          PrintTrace(l);
        };
        PrintTrace("}");
      };
    };
    break;
  case STKOP_CellStore:
    {
      CELLFLAG cf, *pcf;
      ROOMTYPE rt;
      ui32 arrayIndex, arraySize;
      if (pdsaDbank->tracing)
      {
        PrintTrace("Cell! ");
      };
      arraySize = pdsaDbank->stack.pop();
      arrayIndex = pdsaDbank->stack.pop();
      locr.Integer(pdsaDbank->stack.pop());
      NOFILTER
      if (!locr.IsValid()) break;
      if (arrayIndex < 0) break;
      if (arrayIndex + arraySize > 100) break;
      pcf = GetCellFlagsAddress(locr);
      cf = *pcf;
      rt = RoomType(cf);
      if (arraySize == 0) break;
      if (pdsaDbank->Var(arrayIndex + 0) != rt) break;
      if (arraySize < 2) break;
      switch (rt)
      {
      case roomSTONE:
        {
          cf &= 0xe0;
          if (pdsaDbank->Var(arrayIndex+1) & 1) cf |= 0x8;
          if (pdsaDbank->Var(arrayIndex+1) & 2) cf |= 0x4;
          if (pdsaDbank->Var(arrayIndex+1) & 4) cf |= 0x2;
          if (pdsaDbank->Var(arrayIndex+1) & 8) cf |= 0x1;
        };
        break;
      case roomOPEN:
        {
          cf &= 0xf7;
          if (pdsaDbank->Var(arrayIndex+1) & 1) cf |= 0x08;
        };
        break;
      case roomSTAIRS:
        break;
      case roomTELEPORTER:
        {
          DB1 *pdbTELEPORTER;
          pdbTELEPORTER = FirstObjectRecordAddressDB1(locr);
          if (pdbTELEPORTER == NULL) break;
          cf = (CELLFLAG)(((pdsaDbank->Var(arrayIndex+1)&8) != 0) ?
               (cf | 0x0008) : (cf & 0xfff7));
          cf = (CELLFLAG)(((pdsaDbank->Var(arrayIndex+1)&4) != 0) ?
               (cf | 0x0004) : (cf & 0xfffb));
          if (arraySize < 3) break;
          //facing/turning
          pdbTELEPORTER->rotation(
               pdsaDbank->Var(arrayIndex+2) & 0x3);
          pdbTELEPORTER->facingMode(
               (pdsaDbank->Var(arrayIndex+2) >> 2) & 1);
          if (arraySize < 4) break;
          //what to teleport.
          pdbTELEPORTER->what(
               pdsaDbank->Var(arrayIndex+3));
          //destination
          if (arraySize < 5) break;
        };
        break;
      case roomPIT:
        {
          cf &= 0xf2;
          if (pdsaDbank->Var(arrayIndex+1) & 1) cf |= 0x01;
          if (pdsaDbank->Var(arrayIndex+1) & 4) cf |= 0x04;
          if (pdsaDbank->Var(arrayIndex+1) & 8) cf |= 0x08;
        };
        break;
      case roomFALSEWALL:
        {
          cf &= 0xfa;
          if (pdsaDbank->Var(arrayIndex+1) & 1) cf |= 0x01;
          if (pdsaDbank->Var(arrayIndex+1) & 4) cf |= 0x04;
        };
        break;
      case roomDOOR:
        {
          DB0 *pdbDOOR;
          ui32 oldState, newState;
          pdbDOOR = FirstObjectRecordAddressDB0(locr);
          if (pdbDOOR == NULL) break;
          if (arraySize < 2) break;
          pdbDOOR->mode(
                (pdsaDbank->Var(arrayIndex+1) & 2)!=0);
          pdbDOOR->doorSwitch(
                (pdsaDbank->Var(arrayIndex+1) & 4)!=0);
          pdbDOOR->canOpenWithFireball(
                (pdsaDbank->Var(arrayIndex+1) & 8)!=0);
          pdbDOOR->canOpenWithAxe(
                (pdsaDbank->Var(arrayIndex+1) & 0x10) != 0);
          if (arraySize < 3) break; 
          oldState = cf & 7;
          newState = pdsaDbank->Var(arrayIndex+2);
          switch (oldState)
          {
          case 5: //if it is bashed
            if (newState == 0) oldState=newState; //set open
            break;
          case 4: //if it is closed
            if (newState == 5) oldState = newState; //set bashed
            break;
          default: newState = oldState;
          };
          cf &= ~7;
          cf |= oldState;
          if (arraySize < 4) break;
          pdbDOOR->doorType(
                pdsaDbank->Var(arrayIndex+3));
          if (arraySize < 5) break;
          pdbDOOR->ornateness(
                pdsaDbank->Var(arrayIndex+4));
        };
        break;
      };
      *pcf = cf;
    };
    break;
  case STKOP_GlobalFetch:
    {
      ui32 result = 0;
      if (pdsaDbank->tracing)
      {
        PrintTrace("Global@ ");
      };
      switch (pdsaDbank->stack.pop())
      {
      case 1:
        result = LOCATIONREL(d.partyLevel,d.partyX,d.partyY,d.partyFacing).Integer();
        break;
      };
      pdsaDbank->stack.push(result);
    };
    break;
  case STKOP_MultiplierFetch:
    {
      i32 level;
      if (pdsaDbank->tracing)
      {
        PrintTrace("Multiplier@ ");
      };
      level = pdsaDbank->stack.pop();
      if (level < d.dungeonDatIndex->NumLevel())
      {
        pdsaDbank->stack.push(d.pLevelDescriptors[level].experienceMultiplier());
      }
      else
      {
        pdsaDbank->stack.push(1);
      };
    };
    break;
  case STKOP_MonsterFetch:
    {
      ui32 index, num, id, i, n;
      //i32 result[7];
      i32 result[8];
      char line[10];
      RN rnMon;
      DB4 *pMon;
      if (pdsaDbank->tracing)
      {
        PrintTrace("Monster@ ");
      };
      num = pdsaDbank->stack.pop();
      index = pdsaDbank->stack.pop();
      id = pdsaDbank->stack.pop();
      result[0] = 0;
      result[6] = 0;
      if ( rnMon.checkIndirectIndex(id))
      {
        rnMon.ConstructFromInteger(id);
        if (rnMon.dbType() == dbMONSTER)
        {
          pMon = GetRecordAddressDB4(rnMon);
          result[0] = pMon->numMonM1() + 1;
          result[1] = pMon->monsterType();
          for (i=0; i<4; i++)
          {
            result[2 + i] = pMon->hitPoints6[i];
          };
          if (pMon->invisible()) result[6] |= 1;
          if (pMon->drawAsSize4()) result[6] |= 2;
          if (pMon->Unique()) result[6] |= 4;
          if (pMon->Poisoned()) result[6] |= 8;
          result[7] = pMon->alternateGraphics();
        } 
        else 
        {
          PrintTrace("Not a monster");
          memset(result,0,sizeof(result));
          result[1] = -1;
        };
      }
      else
      {
        memset(result,0,sizeof(result));
        result[1] = -1;
      };
      n = 8;
      if (num < n) n = num;
      for (i=0; i<n; i++)
      {
        pdsaDbank->Var(index+i, result[i]);
      };
      if (pdsaDbank->tracing)
      {
        PrintTrace("{");
        for (i=0; i<n; i++)
        {
          sprintf(line,"% d", result[i]); 
          PrintTrace(line);
        };
        PrintTrace("}");
      };
    };
    break;
  case STKOP_PartyDistance:
    {
      LOCATIONREL locationr;
      if (pdsaDbank->tracing)
      {
        PrintTrace("PartyDistance ");
      };
      locationr.Integer(pdsaDbank->stack.pop());
      if (d.partyLevel == locationr.l)
      {
        pdsaDbank->stack.push(abs(d.partyX- locationr.x) + abs(d.partyY- locationr.y));
      }
      else
      {
        pdsaDbank->stack.push(-abs(d.partyLevel- locationr.l));
      };
      break;
    };
  case STKOP_MonsterStore:
    {
      ui32 index, num, id, i;
      RN rnMon;
      DB4 *pMon;
      char line[30];
      num = pdsaDbank->stack.pop();
      index = pdsaDbank->stack.pop();
      id = pdsaDbank->stack.pop();
      NOFILTER
      if (pdsaDbank->tracing)
      {
        PrintTrace("Monster! {");
        for (i=0; i<num; i++)
        {
          sprintf(line,"% d", pdsaDbank->Var(index+i)); 
          PrintTrace(line);
        };
        PrintTrace("}");
      };
      if ( rnMon.checkIndirectIndex(id))
      {
        rnMon.ConstructFromInteger(id);
        if (rnMon.dbType() == dbMONSTER)
        {
          pMon = GetRecordAddressDB4(rnMon);
          for (i=0; (i<4)&&(i<num-2); i++)
          {
            //if (pdsaDbank->stack.tempVar[index+i+2] == 0)
            //{
            //  KillMonster(rnMon, i);
            //}
            //else
            //{
            pMon->hitPoints6[i] = (ui16)pdsaDbank->Var(index+i+2);
            //};
          };
          if (num > 6)
          {
            pMon->invisible((pdsaDbank->Var(index+6) & 1) !=0);
            pMon->drawAsSize4((pdsaDbank->Var(index+6) & 2) != 0);
            pMon->Unique((pdsaDbank->Var(index+6) & 4) != 0);
            pMon->Poisoned((pdsaDbank->Var(index+6) & 8) != 0);
          };
          if (num>7)
          {
            pMon->alternateGraphics(pdsaDbank->Var(index+7));
          };
        } else   PrintTrace("Not a Monster");
      };
    };
    break;
  case STKOP_PartyFetch:
    {
      ui32 index, num, i, n;
      ui32 result[12];
      char line[10];
      num = pdsaDbank->stack.pop();
      index = pdsaDbank->stack.pop();
      if (index+num > 100) break;
      result[0] = d.NumCharacter;
      result[1] = d.partyLevel;
      result[2] = d.partyX;
      result[3] = d.partyY;
      result[4] = d.partyFacing;
      result[5] = d.PartySleeping;
      result[6] = d.SeeThruWalls;
      result[7] = d.MagicFootprintsActive;
      result[8] = d.HandChar;
      result[9] = d.Invisible;
      result[10]= d.FireShield;
      result[11]= d.SpellShield;
      n = 12;
      if (num < n) n = num;
      for (i=0; i<n; i++)
      {
        pdsaDbank->Var(index+i, result[i]);
      };
      if (pdsaDbank->tracing)
      {
        PrintTrace("Party@ ");
        PrintTrace("{");
        for (i=0; i<n; i++)
        {
          sprintf(line,"% d", result[i]); 
          PrintTrace(line);
        };
        PrintTrace("}");
      };
    };
    break;
  case STKOP_CharFetch:
    {
      ui32 index, num, i, n;
      ui32 result[59];
      i32 charIdx;
      CHARDESC character;
      CHARDESC *pc;
      char line[10];
      num = pdsaDbank->stack.pop();
      index = pdsaDbank->stack.pop();
      charIdx = pdsaDbank->stack.pop();
      if (charIdx == 4) charIdx = d.HandChar;
      if (   (index+num > 100) 
          || ((charIdx < 0x10000) && (charIdx >= d.NumCharacter))
          || (charIdx < 0)  )
      {
        memset(result, 0, sizeof(result));
      }
      else
      {
        if (charIdx & 0x10000)
        {
          if (!character.GetFromWings((ui16)(charIdx & 0xffff), true))
          {
            memset(&character,0,sizeof(character));
          };
          pc = &character;
        }
        else
        {
          pc = d.CH16482 + charIdx;
        };
        result[0] = pc->facing;
        result[1] = pc->food;
        result[2] = _max(pc->HP() - d.PendingDamage[charIdx], 0);
        result[3] = pc->load;
        result[4] = pc->Mana();
        result[5] = pc->ouches;
        result[6] = pc->charPosition;
        result[7] = pc->shieldStrength;
        result[8] = pc->Stamina();
        result[9] = pc->water;
        for (i=0; i<7; i++)
        {
          result[10+i]=  (pc->Attributes[i].Minimum()<<16)
                       | (pc->Attributes[i].Maximum()<<8)
                       | (pc->Attributes[i].Current()<<0);
        };
        for (i=0; i<40; i+=2)
        {
          result[17+i]   = pc->skills92[i/2].tempAdjust;
          result[17+i+1] = pc->skills92[i/2].experience;
        };
        result[57] = pc->fingerPrint;
        result[58] = pc->talents;
      };
      n = 59;
      if (num < n) n = num;
      for (i=0; i<n; i++)
      {
        if (index + i > 99) break;
        pdsaDbank->Var(index+i, result[i]);
      };
      if (pdsaDbank->tracing)
      {
        PrintTrace("Char@ ");
        PrintTrace("{");
        for (i=0; i<n; i++)
        {
          sprintf(line,"% d", result[i]); 
          PrintTrace(line);
        };
        PrintTrace("}");
      };
    };
    break;
  case STKOP_TalentsFetch:
    {
      i32 charIdx;
      CHARDESC character;
      CHARDESC *pc;
      if (pdsaDbank->tracing)
      {
        PrintTrace("Talents@");
      };
      charIdx = pdsaDbank->stack.pop();
      if (charIdx == 4) charIdx = d.HandChar;
      if (
             ((charIdx < 0x10000) && (charIdx >= d.NumCharacter))
          || (charIdx < 0)  )
      {
        pc = NULL;
      }
      else
      {
        if (charIdx & 0x10000)
        {
          if (!character.GetFromWings((ui16)(charIdx & 0xffff), true))
          {
            pc=NULL;
          }
          else
          {
            pc = &character;
          };
        }
        else
        {
          pc = d.CH16482 + charIdx;
        };
      };
      if (pc==NULL)
      {
        if (pdsaDbank->tracing)
        {
          PrintTrace(" Error-no such character");
        };
        pdsaDbank->stack.push(0);
      }
      else
      {
        pdsaDbank->stack.push(pc->talents);
      };
    };
    break;
  case STKOP_TalentsStore:
    {
      i32 charIdx;
      ui32 newTalents;
      CHARDESC character;
      CHARDESC *pc;
      if (pdsaDbank->tracing)
      {
        PrintTrace("Talents!");
      };
      charIdx = pdsaDbank->stack.pop();
      newTalents = pdsaDbank->stack.pop();
      if (charIdx == 4) charIdx = d.HandChar;
      if (
             ((charIdx < 0x10000) && (charIdx >= d.NumCharacter))
          || (charIdx < 0)  )
      {
        pc = NULL;
      }
      else
      {
        if (charIdx & 0x10000)
        {
          if (!character.GetFromWings((ui16)(charIdx & 0xffff), true))
          {
            pc=NULL;
          }
          else
          {
            pc = &character;
          };
        }
        else
        {
          pc = d.CH16482 + charIdx;
        };
      };
      if (pc == NULL)
      {
        if (pdsaDbank->tracing)
        {
          PrintTrace(" Error-no such character");
        };
      }
      else
      {
        if (pc->talents != newTalents)
        {
          pc->talents = newTalents;
          if (charIdx & 0x10000)
          {
            character.SaveToWings();
          };
        };
      };
    };
    break;
  case STKOP_CausePoison:
    {
      i32 charIdx, val;
      charIdx = pdsaDbank->stack.pop(); //Character Index
      val     = pdsaDbank->stack.pop(); //Poison value
      NOFILTER
      if (charIdx >= d.NumCharacter) break;
      if (charIdx < 0) break;
      if (pdsaDbank->tracing)
      {
        PrintTrace("CausePoison ");
      };
      PoisonCharacter(charIdx, val);
    };
    break;
  case STKOP_WhoHasTalent:
    {
      i32 msk, charIdx, val;
      ui32 talents;
      val = 0;
      talents = pdsaDbank->stack.pop(); //Required talents
      for (charIdx=0, msk=1; charIdx<d.NumCharacter; charIdx++, msk<<=1)
      {
        if ((d.CH16482[charIdx].talents & talents) == talents)
        {
          val |= msk;
        };
      };
      if (pdsaDbank->tracing)
      {
        PrintTrace("WhoHasTalent ");
      };
      pdsaDbank->stack.push(val);
    };
    break;
  case STKOP_WhereIsChar:
    {
      i32 fingerPrint, charIdx, val;
      val = 4;
      fingerPrint = pdsaDbank->stack.pop(); //Character'figerprint
      for (charIdx=0; charIdx<d.NumCharacter; charIdx++)
      {
        if (d.CH16482[charIdx].fingerPrint == fingerPrint)
        {
          val = charIdx;
        };
      };
      if (val == 4)
      {
        //Perhaps the character is waiting in the wings.
        ui32 *pData;
        ui32 key;
        int size;
        key = (EDT_Character<<24)| (fingerPrint & 0xffff);
        size = expool.Locate(key, &pData);
        if (size == 25) val = 5;
      };

      if (pdsaDbank->tracing)
      {
        PrintTrace("WhereIsChar ");
      };
      pdsaDbank->stack.push(val);
    };
    break;
  case STKOP_SwapCharacter:
    {
      i32 index, fingerPrint;
      NOFILTER
      fingerPrint = pdsaDbank->stack.pop();
      index = pdsaDbank->stack.pop();
      pdsaDbank->stack.push(SwapCharacter(index, fingerPrint));
      if (pdsaDbank->tracing)
      {
        PrintTrace("SwapCharacter ");
      };
    };
    break;
  case STKOP_CharStore:
    {
      ui32 index, num, i, n;
      //ui32 result[57];
      i32 charIdx;
      CHARDESC *pc;
      char line[10];
      num = pdsaDbank->stack.pop();
      index = pdsaDbank->stack.pop();
      charIdx = pdsaDbank->stack.pop();
      if (charIdx == 4) charIdx = d.HandChar;
      NOFILTER
      if (index+num > 100) break;
      if (charIdx >= d.NumCharacter) break;
      if (charIdx < 0) break;
      if (d.PotentialCharacterOrdinal == charIdx+1) break;
      pc = d.CH16482 + charIdx;
      n = 59;
      if (num < n) n = num;
      if (n > 100 - index) n = 100-index;
      if (pdsaDbank->tracing)
      {
        PrintTrace("Char! ");
        PrintTrace("{");
        for (i=0; i<n; i++)
        {
          sprintf(line,"% d", pdsaDbank->Var(index+i)); 
          PrintTrace(line);
        };
        PrintTrace("}");
      };
      //pc->facing(result[0]);;
      if (n>1) 
      {
        //pc->food = result[1];
        pc->food = sw(LIMIT(-1023,pdsaDbank->Var(1+index),2048));
      };
      if (pc->HP() - d.PendingDamage[charIdx] > 0) // Nothing we can do about dead heroes!
      {
        if (n>2) 
        {
          // should we adjust d.pendingdamage?
          if (pdsaDbank->Var(2+index) < 0) pdsaDbank->Var(2+index, 0);
          if (pdsaDbank->Var(2+index) > pc->MaxHP()) 
                       pdsaDbank->Var(2+index, pc->MaxHP());
          if (pdsaDbank->Var(2+index) < pc->HP() - d.PendingDamage[charIdx])
          {
            if (TimerTraceActive)
            {
              i32 adj = (ui16)(pc->HP() - pdsaDbank->Var(2+index)) - d.PendingDamage[charIdx];
              fprintf(GETFILE(TraceFile),
                      "Damage character %d incremented by %d = %d\n",
                      charIdx, adj, d.PendingDamage[charIdx]+adj);
            };
            d.PendingDamage[charIdx] = 
                 (ui16)(pc->HP() - pdsaDbank->Var(2+index));
          }
          else
          {
            pc->HP(pdsaDbank->Var(2+index) + d.PendingDamage[charIdx]);
          };
          //if (pdsaDbank->stack.tempVar[2+index] == 0)
          //{
          //  i32 oldLevel;
          //  oldLevel = d.LoadedLevel;
          //  LoadLevel(d.partyLevel);
          //  KillCharacter(charIdx);
          //  LoadLevel(oldLevel);
          //}
          //else if (pdsaDbank->stack.tempVar[2+index] > pc->MaxHP()) pc->HP(pc->MaxHP());
          //else pc->HP(pdsaDbank->stack.tempVar[2+index]); 
          //pc->word48 |= 0x100;
          //DrawCharacterState(charIdx);
        };
        pc->charFlags |= CHARFLAG_statsChanged;
        DrawCharacterState(charIdx);
      };
      //result[3] = pc->load;
      if (n>4) 
      {
      //  pc->Mana(result[4]);
        pc->Mana(LIMIT(0, pdsaDbank->Var(4+index), pc->MaxMana()));
        pc->charFlags |= CHARFLAG_statsChanged;
        DrawCharacterState(charIdx);
      };
      //result[5] = pc->ouches;
      //result[6] = pc->position;
      //result[7] = pc->shieldStrength;
      if (n>8)
      {
        pc->Stamina(LIMIT(0,pdsaDbank->Var(8+index),pc->MaxStamina()));
        pc->charFlags |= CHARFLAG_statsChanged;
        DrawCharacterState(charIdx);
      };
      if (n>9) 
      {
        //pcA2->water = sw(LIMIT(-1023,pcA2->water+fp.waterValue,2048));
        pc->water = sw(LIMIT(-1023,pdsaDbank->Var(9+index),2048));
      };
      for (i=0; i<7; i++)
      {
        if (n <= i+10) break;
        pc->Attributes[i].Minimum(pdsaDbank->Var(10+index+i)>>16);
        pc->Attributes[i].Maximum((pdsaDbank->Var(10+index+i)>>8)&0xff);
        pc->Attributes[i].Current(pdsaDbank->Var(10+index+i) & 0xff);
      };
     // if (n>57)
     // {
     //   pc->fingerPrint = (ui16)pdsaDbank->Var(index+57);
     // };
      if (n>58)
      {
        pc->talents = pdsaDbank->Var(index+58);
      };
    };
    break;
  case STKOP_ExperiencePlus:
    {
      i32 charnum, skillnum, experience;
      if (pdsaDbank->tracing)
      {
        PrintTrace("EXPERIENCE+ ");
      };
      experience = pdsaDbank->stack.pop();
      skillnum   = pdsaDbank->stack.pop();
      charnum    = pdsaDbank->stack.pop();
      if ((charnum<d.NumCharacter) && (d.CH16482[charnum].HP()>0)) 
      {
        AddToSkill(charnum, skillnum, experience);
      };
    };
    break;
  case STKOP_Throw:
    {
      LOCATIONREL objectLocation, launchLocation;
      ui32 objectType, delta, damage, range, direction;
      if (pdsaDbank->tracing)
      {
        PrintTrace("THROW ");
      };
      delta = pdsaDbank->stack.pop();
      damage = pdsaDbank->stack.pop();
      range = pdsaDbank->stack.pop();
      direction = pdsaDbank->stack.pop();
      launchLocation.Integer(pdsaDbank->stack.pop());
      objectLocation.Integer(pdsaDbank->stack.pop());
      objectType = pdsaDbank->stack.pop();
      NOFILTER
      ThrowMissile(objectType,
                   launchLocation,
                   direction,
                   objectLocation,
                   range,
                   damage,
                   delta);
    };
    break;        
  case STKOP_TeleportParty:
    {
      DB1 pretendTeleporter;
      unsigned int uilocr;
      i32 oldLevel;
      if (pdsaDbank->tracing)
      {
        PrintTrace("TeleportParty ");
      };
      pretendTeleporter.what(2); //Party and objects
      uilocr = pdsaDbank->stack.pop();
      NOFILTER
      locr.Integer(uilocr);
      pretendTeleporter.destLevel(locr.l);
      pretendTeleporter.destX(locr.x);
      pretendTeleporter.destY(locr.y);
      pretendTeleporter.rotation(locr.p);
      pretendTeleporter.facingMode(0);
      pretendTeleporter.audible(0);
      oldLevel = d.LoadedLevel;
      if (oldLevel != d.partyLevel) LoadLevel(d.partyLevel);
      MoveObject(RNnul,              // We wiggle the party and pretend a
                 d.partyX, d.partyY, // teleporter is present!
                 d.partyX, d.partyY,
                 &pretendTeleporter,
                 NULL);
      if (oldLevel != d.LoadedLevel) LoadLevel(oldLevel);
    };
    break;
  case STKOP_Sound:
    {
      i32 soundNum, volume, flags;
      if (pdsaDbank->tracing)
      {
        PrintTrace("SOUND ");
      };
      flags = pdsaDbank->stack.pop();
      volume = pdsaDbank->stack.pop();
      soundNum = pdsaDbank->stack.pop();
      PlayCustomSound(soundNum, volume, flags);
    };
    break;
  case STKOP_MonBlk:
    {
      i32 flags;
      if (pdsaDbank->tracing)
      {
        PrintTrace("MONBLK ");
      };
      flags = pdsaDbank->stack.pop();
      monsterMoveInhibit[0] = (ui8)(flags & 1);
      monsterMoveInhibit[1] = (ui8)(flags & 2);
      monsterMoveInhibit[2] = (ui8)(flags & 4);
      monsterMoveInhibit[3] = (ui8)(flags & 8);
    };
    break;
  case STKOP_Describe:
    {
      i32 index, color;
      i32 locrInt;
      if (pdsaDbank->tracing)
      {
        PrintTrace("DESCRIBE ");
      };
      color = pdsaDbank->stack.pop();
      index = pdsaDbank->stack.pop();
      locrInt = pdsaDbank->stack.pop();
      if (Override_P) 
      {
        Override_P = false;
        if (locrInt >= 0)
        {
          locr.Integer(locrInt);
          locr.p = Override_Pos;
          locrInt = locr.Integer();
        };
      };
      ModifyDescription(locrInt, index, color);
    };
    break;
  case STKOP_Move:
    {
      i32 srcType,   dstType;
      i32 srcObjMsk, dstObjMsk;
      i32 srcPosMsk, dstPosMsk;
      i32 srcLoc,    dstLoc;
      i32 srcDepth,  dstDepth;
      i32 result;
      if (pdsaDbank->tracing)
      {
        PrintTrace("MOVE ");
      };
      dstDepth  = pdsaDbank->stack.pop();
      dstLoc    = pdsaDbank->stack.pop();
      dstPosMsk = pdsaDbank->stack.pop();
      dstObjMsk = pdsaDbank->stack.pop();
      dstType   = pdsaDbank->stack.pop();
      srcDepth  = pdsaDbank->stack.pop();
      srcLoc    = pdsaDbank->stack.pop();
      srcPosMsk = pdsaDbank->stack.pop();
      srcObjMsk = pdsaDbank->stack.pop();
      srcType   = pdsaDbank->stack.pop();
      NOFILTER
      result = MoveObject (srcType, srcObjMsk, srcPosMsk, srcLoc, srcDepth,
                           dstType, dstObjMsk, dstPosMsk, dstLoc, dstDepth);
      if ((result != 0) && pdsaDbank->tracing)
      {
        char msg[50];
        sprintf(msg,"  Move status=%d", result);
        PrintTrace(msg);
      };
    };
    break;
  case STKOP_Copy:
    {
      DBTYPE dbType;
      RN objSrc, objDst;
      if (pdsaDbank->tracing)
      {
        PrintTrace("COPY ");
      };
      objDst.ConstructFromInteger(pdsaDbank->stack.pop());
      objSrc.ConstructFromInteger(pdsaDbank->stack.pop());
      dbType = objSrc.dbType();
      if (objDst.dbType() != dbType) break;
      NOFILTER
      switch (dbType)
      {
      case dbACTUATOR:
        {
          DB3 *pSrc, *pDst;
          pSrc = GetRecordAddressDB3(objSrc);
          pDst = GetRecordAddressDB3(objDst);
          memcpy(((ui8 *)pDst)+2, ((ui8 *)pSrc)+2, sizeof(*pSrc)-2);
        };
        break;
      default: break;
      };
    };
    break;

  case STKOP_GeneratorDelayFetch:
    {
      RN rnGenerator;
      if (pdsaDbank->tracing)
      {
        PrintTrace("GeneratorDelay@ ");
      };
      locr.Integer(pdsaDbank->stack.pop());
      rnGenerator = FindFirstMonsterGenerator(locr);
      if (rnGenerator == RNeof) pdsaDbank->stack.push(-1);
      else pdsaDbank->stack.push(GetRecordAddressDB3(rnGenerator)->disableTime());
    };
    break;
  case STKOP_JumpGear:
    exPkt.m_transferState = pdsaDbank->stack.pop();
    exPkt.m_transferCol = pdsaDbank->stack.pop();
    if (pdsaDbank->tracing)
    {
      fprintf(GETFILE(TraceFile),"J* %d%s",
                exPkt.m_transferState, colNames[exPkt.m_transferCol]);
    };
    break; 
  case STKOP_GosubGear:
    {
      int state, msg;
      state = pdsaDbank->stack.pop();
      msg = pdsaDbank->stack.pop();
      if (pdsaDbank->tracing)
      {
        fprintf(GETFILE(TraceFile),"G* %d,%s\n",
                  state, colNames[msg]);
      };
      Execute(exPkt.m_RNmaster,
              exPkt.m_RNslave,
              exPkt.m_pMaster,
              exPkt.m_pMaster,
              state,
              msg,
              exPkt.m_locrMaster,
              exPkt.m_locrSlave,
              exPkt.m_subroutineLevel+1,
              pdsaDbank->filter);
    };
    break;
  case STKOP_MonLandD:
    {
      ui32 idx, location, distance;
      if (pdsaDbank->tracing)
      {
        fprintf(GETFILE(TraceFile),"MONL&D");
      };
      if (!monsterMoveFilterActive) break;
      idx = pdsaDbank->stack.pop();
      if (idx > 98) break;
      location = 1024*pDSAparameters[1+0] + 32*pDSAparameters[1+1]+pDSAparameters[1+2];
      pdsaDbank->Var(idx, location);
	  distance =   abs((int)pDSAparameters[1 + 1] - (int)pDSAparameters[1 + 5]) 
		         + abs((int)pDSAparameters[1 + 2] - (int)pDSAparameters[1 + 6]);
      pdsaDbank->Var(idx+1, distance-1);
      if (pdsaDbank->tracing)
      {
        char line[40];
        sprintf(line,"{%d %d}", location, distance-1); 
        PrintTrace(line);
      };
    };
    break;
  case STKOP_IsCarried:
    if (pdsaDbank->tracing)
    {
      fprintf(GETFILE(TraceFile), "ISCARRIED");
    };
    EX_IsCarried();
    break;
  case STKOP_CountInjury:
    {
      i32 injuryMask, chMask, injuries;
      i32 chidx;
      i32 count = 0;
      injuryMask = pdsaDbank->stack.pop();
      chMask = pdsaDbank->stack.pop();
      for (chidx=0; chidx<d.NumCharacter; chidx++, chMask>>=1)
      {
        if ((chMask & 1) == 0) continue;
        if (d.CH16482[chidx].HP() <= 0) continue;
        injuries = d.CH16482[chidx].ouches & injuryMask;
        count +=   bitCounts[injuries&15]
                 + bitCounts[(injuries>>4)&15]
                 + bitCounts[(injuries>>8)&15]
                 + bitCounts[(injuries>>12)&15];
      };
      pdsaDbank->stack.push(count);
    };
    break;

  case STKOP_ThisCell:
    ExamineCell(4,4,"THISCELL");
    break;
  case STKOP_ThisDSAId:
    if (pdsaDbank->tracing)
    {
      fprintf(GETFILE(TraceFile),"THIS_DSA_ID");
    };
    pdsaDbank->stack.push(exPkt.m_RNslave.ConvertToInteger());
    break;
  case STKOP_Neighbors:
    ExamineCell(0,3,"NEIGHBORS");
    break;
  case STKOP_BitCount:
    {
      ui32 V;
      if (pdsaDbank->tracing)
      {
        fprintf(GETFILE(TraceFile),"BITCOUNT");
      };
      V = pdsaDbank->stack.pop();
      pdsaDbank->stack.push(   bitCounts[(V>>0)  & 0x0f]
                             + bitCounts[(V>>4)  & 0x0f]
                             + bitCounts[(V>>8)  & 0x0f]
                             + bitCounts[(V>>12) & 0x0f]
                             + bitCounts[(V>>16) & 0x0f]
                             + bitCounts[(V>>20) & 0x0f]
                             + bitCounts[(V>>24) & 0x0f]
                             + bitCounts[(V>>28) & 0x0f]);
    };
    break;
  case STKOP_VSET:
    {
      ui32 n, dst, i;
      i32 src;
      if (pdsaDbank->tracing)
      {
        PrintTrace("VSET {");
      };
      n   = pdsaDbank->stack.pop();
      dst = pdsaDbank->stack.pop();
      src = pdsaDbank->stack.pop();
      if (dst > 99) break;
      if (dst+n > 100) n = 100-dst;
      if (src < 0)
      {
        for (i=0; i<n; i++)
        {
          pdsaDbank->pDsaVars->values[dst+i] = -src;
        };
        memset(pdsaDbank->pDsaVars->definedFlags+dst, DVT_WellDefined, n);
      }
      else if (src > 99)
      {
        for (i=0; i<n; i++)
        {
          pdsaDbank->pDsaVars->values[dst+i] = src-100;
        };
        memset(pdsaDbank->pDsaVars->definedFlags+dst, DVT_WellDefined, n);
      }
      else
      {
        if (src+n > 100) n = 100-src;
        memmove(pdsaDbank->pDsaVars->values+dst, pdsaDbank->pDsaVars->values+src, 4*n);
        memmove(pdsaDbank->pDsaVars->definedFlags+dst, pdsaDbank->pDsaVars->definedFlags+src, n);

      };
      if (pdsaDbank->tracing)
      {
        char l[30];
        for (i=0; i<n; i++)
        {
          sprintf(l," %d",pdsaDbank->pDsaVars->values[dst+i]);
          PrintTrace(l);
        };
        PrintTrace("}");
      };
    };
    break;
  case STKOP_Jitter:
    {
      i32 temp;
      if (pdsaDbank->tracing)
      {
        fprintf(GETFILE(TraceFile),"JITTER");
      };
      temp = pdsaDbank->stack.pop();
      if (temp != yOverlayJitter)
      {
        yOverlayJitter = temp;
        jitterChanged = true;
      };
      temp = pdsaDbank->stack.pop();
      if (temp != xOverlayJitter)
      {
        xOverlayJitter = temp;
        jitterChanged = true;
      };
      temp = pdsaDbank->stack.pop();
      if (temp != yGraphicJitter)
      {
        yGraphicJitter = temp;
        jitterChanged = true;
      };
      temp = pdsaDbank->stack.pop();
      if (xGraphicJitter != temp)
      {
        xGraphicJitter = temp;
        jitterChanged = true;
      };
    };
    break;
  case STKOP_ModifyMessage:
    {
      unsigned int s, c, t;
      if (pdsaDbank->tracing)
      {
        fprintf(GETFILE(TraceFile),"MODIFYMESSAGE");
      };
      s=pdsaDbank->stack.pop();
      c=pdsaDbank->stack.pop();
      t=pdsaDbank->stack.pop();
      if (s > 3) s = 3;
      if (c > 3) c = 3;
      if (t > 3) t = 3;
      timerTypeModifier[0] = s;
      timerTypeModifier[1] = c;
      timerTypeModifier[2] = t;
    };
    break;
  case STKOP_NumParam:
    if (pdsaDbank->tracing)
    {
      fprintf(GETFILE(TraceFile),"NUMPARAM");
    };
    pdsaDbank->stack.push(pDSAparameters[0]);
    break;

  case STKOP_I_Del:            INDIRECT ("%DEL "          ,2);   break;
  case STKOP_I_DelMon:         INDIRECT ("%DELMON "       ,2);   break;
  case STKOP_I_InsMon:         INDIRECT ("%INSMON "       ,2);   break;
  case STKOP_I_Add:            INDIRECT ("%ADD "          ,3);   break;
  case STKOP_I_CreateCloud:    INDIRECT ("%CREATECLOUD "  ,3);   break;
  case STKOP_I_Cast:           INDIRECTP("%CAST "         ,0,13);break;
  case STKOP_I_TeleportParty:  INDIRECT ("%TELEPORTPARTY ",1);   break;
  case STKOP_I_MonsterStore:   INDIRECTA("%MONSTER! "     ,3);   break;
  case STKOP_I_CharStore:      INDIRECTA("%CHAR! "        ,3);   break;
  case STKOP_I_CausePoison:    INDIRECT ("%CAUSEPOISON"   ,2);   break;
  case STKOP_I_Move:           INDIRECT ("%MOVE "        ,10);   break;
  case STKOP_I_Copy:           INDIRECT ("%COPY "         ,2);   break;
  case STKOP_I_CellStore:      INDIRECTA("%CELL! "        ,3);   break;
  case STKOP_I_Throw:          INDIRECT ("%THROW "        ,7);   break;
  case STKOP_I_SwapCharacter:  INDIRECT ("%SWAPCHARACTER" ,2);   break;
  case STKOP_I_Delay:
    pdsaDbank->IndirectActionDelay = pdsaDbank->stack.pop();
    if (pdsaDbank->tracing)
    {
      PrintTrace("%DELAY ");
    };
    break;
  default:
    message("Illegal subcode in EX_AMPERSAND");
    return;
  };
  return;
}  

static void EX_FETCH(EXECUTIONPACKET& exPkt)
{
  i32 objMask, positionMask;
  i32 location, depth;
  i32 objID;
  i32 relState;
  LOCATIONREL locr;
  //LOCATIONABS loca;
  DSAfetchCmd cmd;
  RN thisObj, contents;
  cmd = ((DSAcommand *)(exPkt.m_PC++))->fetchCmd;
  exPkt.m_wordsRemaining--;
  objMask = pdsaDbank->stack.pop();
  positionMask = pdsaDbank->stack.pop();
  location = pdsaDbank->stack.pop();
  depth = pdsaDbank->stack.pop();
  locr.Integer(location);
  if (!locr.IsValid())
  {
    message("Illegal location in DSA FETCH");
    return;
  };
  if (positionMask == -1) positionMask = 1<<locr.p;
  //loca = locr;
  objID = -1;
  for (thisObj = FindFirstObject(locr.l,locr.x,locr.y);
       thisObj != RNeof;
       thisObj = GetDBRecordLink(thisObj))
  {
    if (((1<<thisObj.pos()) & positionMask) == 0) continue;
    if (((1<<thisObj.dbType()) & objMask) == 0) continue;
    if (depth == 0)
    {
      objID = thisObj.ConvertToInteger();//indirectIndex;
      break;
    }
    else
    {
      depth--;
    };
  };
  if (pdsaDbank->tracing)
  {
    fprintf(GETFILE(TraceFile),"%d",objID);
  };
  pdsaDbank->stack.push(objID);
  if (cmd.IsMaxState())
  {
    relState = *(exPkt.m_PC++);
    exPkt.m_wordsRemaining--;
  }
  else
  {
    relState = cmd.NextState();
  };
  exPkt.m_nextState = exPkt.m_curState + relState;
  return;
}

class SAVEPEXPKT
{
  EXECUTIONPACKET *save;
public:
  SAVEPEXPKT(EXECUTIONPACKET *p){save=pExPkt;pExPkt=p;};
  ~SAVEPEXPKT(){pExPkt=save;};
};

static i32 Execute(RN  RNmaster,
                   RN  RNslave,
                   DSA *pMaster, 
                   DSA *pSlave, 
                   i32 curState, 
                   i32 msgType,
                   LOCATIONREL locrMaster,
                   LOCATIONREL locrSlave,
                   i32 subroutineLevel,
                   bool filter)
{
  DSAState *pState;
  DSAcommand *pCMD;
  LOCATIONABS locaSlave;
  i32 finalState = -1;
  DSACOMMAND opcode;
  EXECUTIONPACKET exPkt;
  SAVEPEXPKT save(&exPkt);
  exPkt.m_locrMaster = locrMaster;
  exPkt.m_subroutineLevel = subroutineLevel;
  exPkt.m_RNmaster       = RNmaster;
  exPkt.m_RNslave        = RNslave;
  exPkt.m_pMaster        = pMaster; 
  exPkt.m_curState       = curState;
  exPkt.m_column         = msgType;
  exPkt.m_locrMaster     = locrMaster;
  exPkt.m_locrSlave      = locrSlave;
  pdsaDbank->filter      = filter;
  if (pdsaDbank->tracing)
  {
    locaSlave  = locrSlave;
    fprintf(GETFILE(TraceFile),
      "(%d)Execute DSA at %d(%d,%d) master at %d(%d,%d) state=%d,msg=%d\n",
      exPkt.m_subroutineLevel,
      locaSlave.l,locaSlave.x,locaSlave.y,
      exPkt.m_locrMaster.l,exPkt.m_locrMaster.x,exPkt.m_locrMaster.y,
      exPkt.m_curState,msgType);
  };
  do
  { 
    pState = pSlave->StatePointer(exPkt.m_curState);
    if (pState == NULL) 
    {
      if (finalState == -1) finalState = exPkt.m_curState;
      if (pdsaDbank->tracing)
      {
        fprintf(GETFILE(TraceFile),"(%d)RETURN",exPkt.m_subroutineLevel);
      };
      return finalState;
    };
    exPkt.m_PC = pState->Program(exPkt.m_column);
    exPkt.m_wordsRemaining = pState->ProgramSize(exPkt.m_column);
    exPkt.m_transferState  = -1;
    exPkt.m_transferCol    = -1;
    exPkt.m_nextState      = exPkt.m_curState;
    while (exPkt.m_wordsRemaining > 0)
    {
      pCMD = (DSAcommand *)exPkt.m_PC;
      opcode = (DSACOMMAND)pCMD->generalCmd.CmdCode();
      operationCount++;
      switch (opcode)
      {
      case DSACMD_QUESTION:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  QUESTION ");
        };
        EX_QUESTION(exPkt);
        break;
      case DSACMD_CASE:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  CASE ");
        };
        EX_CASE(exPkt);
        break;
      case DSACMD_EQUAL:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  EQUAL ");
        };
        EX_EQUAL(exPkt);
        break;
      case DSACMD_AMPERSAND:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  &");
        };
        EX_AMPERSAND(exPkt,0);
        break;
      case DSACMD_AMPERSAND2:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  &");
        };
        EX_AMPERSAND(exPkt,128);
        break;
      case DSACMD_FETCH:
        // (depth, location, position mask, object mask)
        if (pdsaDbank->tracing)
        {
          PrintTrace("  FETCH ");
        };
        EX_FETCH(exPkt);
        break;
      case DSACMD_NOOP:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  NOOP ");
        };
        EX_NOOP(exPkt);
        break;
      case DSACMD_LOAD:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  LOAD ");
        };
        EX_LOAD(exPkt);
        break;
      case DSACMD_STORE:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  STORE ");
        };
        EX_STORE(exPkt);
        break;
      case DSACMD_VARIABLESTORE:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  V");
        };
        EX_VARIABLESTORE(exPkt);
        break;
      case DSACMD_VARIABLEFETCH:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  V");
        };
        EX_VARIABLEFETCH(exPkt);
        break;
      case DSACMD_GLOBALSTORE:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  GV");
        };
        EX_GLOBALSTORE(exPkt);
        break;
      case DSACMD_GLOBALFETCH:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  GV");
        };
        EX_GLOBALFETCH(exPkt);
        break;
      case DSACMD_MESSAGE:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  MESSAGE ");
        };
        EX_MESSAGE(exPkt, false, 'M');
        break;
      case DSACMD_MESSAGE32:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  MESSAGE ");
        };
        EX_MESSAGE(exPkt, true, 'M');
        break;
      case DSACMD_DESSAGE32:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  MESSAGE ");
        };
        EX_MESSAGE(exPkt, true, 'D');
        break;
      case DSACMD_COPYTELEPORTER:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  COPYTELEPORTER ");
        };
        EX_COPYTELEPORTER(exPkt, false);
        break;
      case DSACMD_COPYTELEPORTER32:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  COPYTELEPORTER ");
        };
        EX_COPYTELEPORTER(exPkt, true);
        break;
      case DSACMD_GOSUB:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  GOSUB ");
        };
        EX_GOSUB(exPkt);
        break;
      case DSACMD_JUMP:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  JUMP ");
        };
        EX_JUMP(exPkt);
        break;
      case DSACMD_OVERRIDE:
        if (pdsaDbank->tracing)
        {
          PrintTrace("  OVERRIDE ");
        };
        EX_OVERRIDE(exPkt);
        break;
      default: 
        message("Unknown DSA command code");
        return curState;
      };
      if (pdsaDbank->tracing)
      {
        pdsaDbank->stack.dump();
        PrintTrace("\n");
      };
      if (exPkt.m_transferState >= 0) break; //Jump command
      if (    (finalState == -1)
           && (exPkt.m_nextState >= 0))
      {
        finalState = exPkt.m_nextState;
      };
    };
    if (exPkt.m_nextState >= 0) 
    {
      if (finalState == -1) finalState = exPkt.m_nextState;
    };
    // Process result of jump command
    exPkt.m_curState = exPkt.m_transferState;
    exPkt.m_column   = exPkt.m_transferCol;
    exPkt.m_transferState = -1;
    exPkt.m_transferCol = -1;
  } while (exPkt.m_curState >= 0);
  if (pdsaDbank->tracing)
  {
    fprintf(GETFILE(TraceFile),"(%d)RETURN",exPkt.m_subroutineLevel);
  };
  return finalState;
}

bool ProcessDSATimer5(RN objSlave, 
                      const TIMER *pTimer,
                      LOCATIONREL locrSlave)
{
  DSAVARS dsaVars;
  return ProcessDSATimer6(objSlave, pTimer, locrSlave, false, &dsaVars);
}

bool ProcessDSATimer7(RN objSlave, 
                      const TIMER *pTimer,
                      LOCATIONREL locrSlave)
{
  DSAVARS dsaVars;
  return ProcessDSATimer6(objSlave, pTimer, locrSlave, false, &dsaVars);
}

int dsaFilterEnterCount = 0;
int dsaFilterLeaveCount = 0;

void ProcessDSAFilter(RN objSlave, 
                      const TIMER *pTimer,
                      LOCATIONREL locrSlave,
                      bool *pFilterActive,
                      DSAVARS *dsaVars)
{
  dsaFilterEnterCount++;
  if (pFilterActive != NULL) *pFilterActive = true;
  ProcessDSATimer6(objSlave, pTimer, locrSlave, true, dsaVars);
  if (pFilterActive != NULL) *pFilterActive = false;
  dsaFilterLeaveCount++;
}


bool ProcessDSATimer6(RN objSlave, 
                      const TIMER *pTimer,
                      LOCATIONREL locrSlave,
                      bool filter,
                      DSAVARS *dsaVars)
{
  i32 level;
  i32 levelDSAindex, absDSAindex;
  i32 timerFunction, timerX, timerY, timerPosition;
  i32 masterX, masterY, masterPos;
  i32 curState, startingState;
  i32 inputMsgType;
  i32 startingOperationCount;
  bool deletedSlave;
  LOCATIONREL locrMaster;
//  EXECUTIONPACKET exPkt;
//  SAVEPEXPKT save(&exPkt);
  RN objMaster;
  DB3 *pDBSlave ,*pDBMaster;
  DSA *pSlave   ,*pMaster;
  DSADBANK dsaDbank;
  DSADBANK *saveDsaDbank;
  if (timerTypeModifier[pTimer->timerUByte9()] == 3) return false;
  dsaActive = true;
  saveDsaDbank = pdsaDbank;
  pdsaDbank = &dsaDbank;
  pdsaDbank->IndirectActionDelay = -1;
  pdsaDbank->pDsaVars = dsaVars;
  if ( (operationCount == -1) || (d.Time < prevTime) )
  {
    operationCount = 0;
    prevTime = d.Time;
  };

  level = locrSlave.l;
  locrMaster.l = level;
  pDBSlave = GetRecordAddressDB3(objSlave);
  levelDSAindex = pDBSlave->DSAselector();
  absDSAindex = DSALevelIndex[level][levelDSAindex];
  if (absDSAindex > 255)
  {
    UI_MessageBox("Undefined Slave DSA Level Index","DSA",MESSAGE_OK);
    pdsaDbank = saveDsaDbank;
    dsaActive = false;
    return false;
  };
  pSlave = DSAIndex.GetDSA(absDSAindex);
  if (pSlave == NULL)
  {
    UI_MessageBox("Undefined Slave DSA","DSA",MESSAGE_OK);
    pdsaDbank = saveDsaDbank;
    dsaActive = false;
    return false;
  };
  timerFunction = timerTypeModifier[pTimer->timerUByte9()];
  timerPosition = pTimer->timerUByte8();
  timerY        = pTimer->timerUByte7();
  timerX        = pTimer->timerUByte6();
  objMaster = FindMaster(pSlave, objSlave, timerX, timerY,
                            &masterX, &masterY, &masterPos);
  locrMaster.x = masterX;
  locrMaster.y = masterY;
  locrMaster.p = objMaster.pos();
  pDBMaster = GetRecordAddressDB3(objMaster);
  levelDSAindex = pDBMaster->DSAselector();
  absDSAindex = DSALevelIndex[level][levelDSAindex];
  if (absDSAindex > 255)
  {
    UI_MessageBox("Undefined Master DSA Level Index","DSA",MESSAGE_OK);
    dsaActive = false;
    pdsaDbank = saveDsaDbank;
    return false;
  };
  pMaster = DSAIndex.GetDSA(absDSAindex);
  if (pMaster == NULL)
  {
    UI_MessageBox("Undefined Master DSA","DSA",MESSAGE_OK);
    dsaActive = false;
    pdsaDbank = saveDsaDbank;
    return false;
  };
  //
  //  Get the current state of the machine.
  //
  curState = GetState(pDBMaster, pMaster);
  startingState = curState;
  inputMsgType = 3 * timerPosition + timerFunction;
  Override_P = false;
  pdsaDbank->stack.Clear();
  pdsaDbank->forceState = -1;
#ifdef _MSVC_INTEL
  pdsaDbank->tracing = TimerTraceActive && DSAIndex.IsTracing(absDSAindex);
#else
  pdsaDbank->tracing = TimerTraceActive && DSATraceActive;
#endif
  if (pdsaDbank->tracing)
  {
    fprintf(GETFILE(TraceFile),"DSA %s State %d MSG %d\n",
                      pSlave->Description(), 
                      curState, 
                      inputMsgType);
  };
  startingOperationCount = operationCount;
  curState = Execute(objMaster,
                     objSlave,
                     pMaster, 
                     pSlave, 
                     curState, 
                     inputMsgType,
                     locrMaster,
                     locrSlave,
                     0,
                     filter);
  pDBSlave = GetRecordAddressDB3(objSlave);
#ifdef _DEBUG
  instrumentation.AddToDSACount(absDSAindex, operationCount-startingOperationCount);
#endif
  if (pdsaDbank->tracing)
  {
    pdsaDbank->stack.dump();
    fprintf(GETFILE(TraceFile), "[%d]\n",operationCount-startingOperationCount);
  }
  else
  {
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"DSA %s State %d MSG %d [%d]\n",
                      pSlave->Description(), 
                      curState, 
                      inputMsgType,
                      operationCount-startingOperationCount);
    };
  };
  if (pdsaDbank->forceState >= 0) curState = pdsaDbank->forceState;
  PutState(GetRecordAddressDB3(objMaster), pMaster, curState);
  if (!excessiveWarning)
  {
    if (operationCount > 50000)
    {
      if (    (d.Time < prevTime + 6) 
          || ((6*operationCount) > 50000*(d.Time-prevTime)))
      {
        if (!IsPlayFileOpen())
        {
          const char *m = "Excessive DSA operations.\nThis is the LAST warning.";
          UI_MessageBox(m,"Warning",MESSAGE_OK);
          PrintTrace(m);
        };
        excessiveWarning = true;
      };
      operationCount = 0;
      prevTime = d.Time;
    };
  };
  deletedSlave = pdsaDbank->ConsumateDeletes(objSlave);
  pdsaDbank = saveDsaDbank;
  dsaActive = false;
  return deletedSlave;
}


DSAINDEX::DSAINDEX()
{
  i32 i;
  for (i=0; i<256; i++)
  {
    m_pDSA[i] = NULL;
  };
}

DSAINDEX::~DSAINDEX()
{
  Cleanup();
}

void DSAINDEX::Cleanup()
{
  i32 i;
  for (i=0; i<256; i++)
  {
    if (m_pDSA[i] != NULL) delete m_pDSA[i];
    m_pDSA[i] = NULL;
  };
  for (i=0; i<8; i++)
  {
    m_tracing[i] = 0;
  };
}

void DSAINDEX::SetDSA(i32 index, DSA *pDSA)
{
  if (index > 255)
  {
    die(0,"DSA index > 255");
  };
  if (m_pDSA[index] != NULL) delete m_pDSA[index];
  m_pDSA[index] = pDSA;
}

DSA *DSAINDEX::GetDSA(i32 index)
{
  if (index > 255)
  {
    die(0,"DSA index > 255");
  };
  return m_pDSA[index];
}

bool DSAINDEX::AnyTraceActive()
{
  return (m_tracing[0]|m_tracing[1]|m_tracing[2]|m_tracing[3]|m_tracing[4]) != 0;
}

void DSAINDEX::SaveTracing()
{
  memcpy(m_savedTracing, m_tracing, sizeof(m_tracing));
}

void DSAINDEX::RestoreTracing(ui32 *pArray)
{
  if (pArray==NULL) pArray = m_savedTracing;
  memcpy(m_tracing, pArray, sizeof(m_tracing));
}

void DSAINDEX::ReadTracing()
{
  i32 size;
  ui32 *pRecord;
  if (m_traceOverride == 0)
  {
    size = expool.Locate((EDT_Database<<24) | (EDBT_DSAtraces<<16), &pRecord);
    if (size == 8)
    {
      RestoreTracing(pRecord);
    };
  }
  else 
  {
    if (m_traceOverride == 256) 
    {
      AllTracing(); 
    } 
    else 
    {
      NoTracing();
    };
  }
  m_traceOverride = 0;
}


void DSAINDEX::WriteTracing()
{
  expool.Write((EDT_Database<<24) | (EDBT_DSAtraces<<16), m_tracing, sizeof(m_tracing)/4);
}


bool DSAINDEX::IsTracing(i32 dsaIndex)
{
  return (m_tracing[dsaIndex/32] & (1<<(dsaIndex%32))) != 0;
}

void DSAINDEX::SetTracing(i32 dsaIndex)
{
  m_tracing[dsaIndex/32] |= (1<<(dsaIndex%32));
}


void DSAINDEX::AllTracing()
{
  memset(m_tracing, 0xff, sizeof(m_tracing));
}

void DSAINDEX::NoTracing()
{
  memset(m_tracing, 0x00, sizeof(m_tracing));
}

DSA::DSA()
{
  strcpy(m_description,"New DSA");
  m_localState = true;
  m_groupID = 0;
  m_numState = 0;
  m_pStates = NULL;
  m_firstDisplayedState = 0;
  m_state = 0;
}

DSA::~DSA()
{
  i32 i;
  for (i=0; i<m_numState; i++)
  {
    delete m_pStates[i];
    m_pStates[i] = NULL;
  };
  m_numState = 0;
  if (m_pStates != NULL) UI_free (m_pStates);
  m_pStates = NULL;
}

DSAState *DSA::StatePointer(i32 stateNum)
{
  if (stateNum >= m_numState) return NULL;
  return m_pStates[stateNum];
}

void DSA::Read()
{
  ui32 i;
  ui32 numNonEmptyStates;
  ui32 stateNum;
  RCS(m_description, 80);
  RCS(&m_state);
  RCS(&m_localState);
  RCS(&m_groupID);
  RCS(&m_numState);
  RCS(&m_firstDisplayedState);
  RCS(&numNonEmptyStates);
  if (m_pStates != NULL)
  {
    die(0,"DSA state table not NULL");
  };
  m_pStates = (DSAState **)UI_malloc(m_numState*sizeof(DSAState *),
                                     MALLOC023);
  for (i=0; i<(unsigned)m_numState; i++) m_pStates[i] = NULL;
  for (i=0; i<numNonEmptyStates; i++)
  {
    RCS(&stateNum);
    if (stateNum >= (unsigned)m_numState)
    {
      die(0,"DSA state number > #states");
    };
    if (m_pStates[stateNum] != NULL)
    {
      die(0,"Duplicate DSA state");
    };
    m_pStates[stateNum] = new DSAState;
    m_pStates[stateNum]->Read();
  };
};

void DSA::Write()
{
  ui32 i;
  ui32 numNonEmptyStates = 0;
  WCS(m_description, 80);
  WCS(&m_state);
  WCS(&m_localState);
  WCS(&m_groupID);
  WCS(&m_numState);
  WCS(&m_firstDisplayedState);
  for (i=0; i<(unsigned)m_numState; i++)
  {
    if (m_pStates[i] != NULL) numNonEmptyStates++;
  };
  WCS(&numNonEmptyStates);
  for (i=0; i<(unsigned)m_numState; i++)
  {
    if (m_pStates[i] == NULL) continue;
    WCS(&i);
    m_pStates[i]->Write();
  };
};



DSAState::DSAState()
{
  m_numAction = 0;
  m_pActions = NULL;
}

DSAState::~DSAState()
{
  i32 i;
  for (i=0; i<m_numAction; i++)
  {
    delete m_pActions[i];
    m_pActions[i] = NULL;
  };
  if (m_pActions != NULL) UI_free (m_pActions);
  m_pActions = NULL;
  m_numAction = 0;
}

ui16 *DSAState::Program(i32 column)
{
  i32 i;
  for (i=0; i<m_numAction; i++)
  {
    if (m_pActions[i]->Column() == column)
    {
      return m_pActions[i]->Program();
    };
  };
  return NULL;
}

i32 DSAState::ProgramSize(i32 column)
{
  i32 i;
  for (i=0; i<m_numAction; i++)
  {
    if (m_pActions[i]->Column() == column)
    {
      return m_pActions[i]->ProgramSize();
    };
  };
  return 0;
}

void DSAState::Read()
{
  i32 i;
  RCS(&m_numAction);
  if (m_numAction > 12)
  {
    die(0,"Number of DSA actions > 12");
  };
  if (m_pActions != NULL)
  {
    die(0,"Action table npt NULL");
  };
  m_pActions = (DSAAction **)UI_malloc(
                    m_numAction * sizeof (DSAAction*),
                    MALLOC024);
  for (i=0; i<m_numAction; i++)
  {
    m_pActions[i] = new DSAAction;
    m_pActions[i]->Read();
  };
}

void DSAState::Write()
{
  i32 i;
  WCS(&m_numAction);
  for (i=0; i<m_numAction; i++)
  {
    m_pActions[i]->Write();
  };
}



DSAAction::DSAAction()
{
  m_column      = -1;
  m_numPgmWords = 0;
  m_program     = NULL;
}

DSAAction::~DSAAction()
{
  if (m_program != NULL) UI_free (m_program);
  m_program = NULL;
  m_numPgmWords = 0;
}

void DSAAction::Read()
{
  RCS(&m_column);
  RCS(&m_numPgmWords);
  m_program = (ui16 *)UI_malloc(
           m_numPgmWords * sizeof (ui16),
           MALLOC025);
  RCS(m_program, m_numPgmWords);
}

void DSAAction::Write()
{
  WCS(&m_column);
  WCS(&m_numPgmWords);
  WCS(m_program, m_numPgmWords);
}

