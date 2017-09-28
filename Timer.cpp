#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

void ProcessPortraitTimer7(RN targetObj, const TIMER *pTimer);
void info(char *, unsigned int);
int MissileEncounterFilter(int type, LOCATIONREL locr, RN missile, RN projectile, i32 index);


extern i32 deleteDuplicateTimers;
extern i32 timerTypeModifier[3];

#ifdef _MSVC_INTEL
#pragma warning(disable:4710)
#endif

void TraceCellflags(i32 x, i32 y, const char *msg)
{
  if (TimerTraceActive)
  fprintf(GETFILE(TraceFile),"%s in %02x,%02x - new value = %02x\n",
             msg, x, y, d.LevelCellFlags[x][y]);
}

//#ifdef _DEBUG

void printHexLocation(FILE *f, i32 lev, i32 x, i32 y)
{
  fprintf(f,"%02x(%02x,%02x)",lev,x,y);
}

const char *action(i32 act)
{
  if (act == 0)return "Set   ";
  if (act == 1)return "Clear ";
  if (act == 2)return "Toggle";
  return "??????";
}

void TraceTimer(TIMER *pTimer, i32 index, const char *msg)
{
  char Msg[15];
  i32 len;
  if (!TimerTraceActive) return;
  memset(Msg,' ',14);
  Msg[14] = 0;
  len=strlen(msg);
  if (len>14) len=14;
  memcpy(Msg,msg,len);
  fprintf(GETFILE(TraceFile),"%08x timer %s %03x %02x "
                    "DLY=%04x "
                    "%02x %02x %02x %02x %02x %02x %04x %08x",
            d.Time,
            //Msg, index, pTimer->timerTime>>24,
            Msg, index, pTimer->Level(),
            //(pTimer->timerTime & 0xffffff) - d.Time,
            pTimer->Time() - d.Time,
            pTimer->Function(),pTimer->timerUByte5(),pTimer->timerUByte6(),
            pTimer->timerUByte7(),pTimer->timerUByte8(),pTimer->timerUByte9(),
            pTimer->TimerSequence(),
            d.RandomNumber);
  switch (pTimer->Function())
  {
  case 6:
      fprintf(GETFILE(TraceFile)," %s  0x%02x in DB at ",
                  action(timerTypeModifier[pTimer->timerUByte9()]),1<<pTimer->timerUByte8());
      printHexLocation(GETFILE(TraceFile),
                       //pTimer->timerTime>>24,
                       pTimer->Level(),
                       pTimer->timerUByte6(),
                       pTimer->timerUByte7());
      fprintf(GETFILE(TraceFile),"%02x",pTimer->timerUByte8());
    break;
  case TT_FALSEWALL:
    fprintf(GETFILE(TraceFile)," %s CELLFLAG 0x04 at %02x,%02x",
                action(timerTypeModifier[pTimer->timerUByte9()]),pTimer->timerUByte6(),pTimer->timerUByte7());
    break;
  case 8:
    fprintf(GETFILE(TraceFile)," %s CELLFLAG 0x08 at %02x,%02x",
                action(timerTypeModifier[pTimer->timerUByte9()]),pTimer->timerUByte6(),pTimer->timerUByte7());
    break;
  };
  fprintf(GETFILE(TraceFile),"\n");
}
//#else //_DEBUG
//void TraceTimer(TIMER *, i32 , char *)
//{
//}
//#endif //_DEBUG

void TIMER::swapTimerWord6()
{
  *(i16 *)(&m_timerUByte6) = LE16(*(i16 *)(&m_timerUByte6));
}


void TIMER::swapTimerObj8()
{
  *(i16 *)(&m_timerUByte8) = LE16(*(i16 *)(&m_timerUByte8));
}

void TIMER::swapTimerObj6()
{
  *(i16 *)(&m_timerUByte6) = LE16(*(i16 *)(&m_timerUByte6));
}


void TIMER::swapTimerWord8()
{
  *(i16 *)(&m_timerUByte8) = LE16(*(i16 *)(&m_timerUByte8));
}

void TIMER::swapTimerSequence()
{
  m_timerSequence = LE16(m_timerSequence);
}


//*********************************************************
//
//*********************************************************
//   TAG00e17a
void ProcessObjectFromMissile(RN missile, RN *pDest, i32 mapX, i32 mapY)
{ //()
  RN objD6;
  RN objD7;
  DB14     *DB14A3;
  DBCOMMON *pdb_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DB14A3 = GetRecordAddressDB14(missile);
  objD6 = DB14A3->flyingObject();
  if (!objD6.IsMagicSpell()&&(objD6.dbType() != dbCLOUD))
  {
    if (pDest != NULL)
    {
      objD7 = *pDest;
      if (objD7 == RNeof)
      {
        pdb_4 = GetCommonAddress(objD6);
        pdb_4->link(RNeof);
        *pDest = objD6;
      }
      else
      {
        AddObjectToRoom(objD6, objD7, -1, 0, NULL);
      };
    }
    else
    {
      MoveObject(
                 RN(objD6).pos(missile.pos()),
                 -2,
                 0,
                 mapX,
                 mapY,
                 NULL,
                 NULL);
    };
  };
  //DB14A3->link(RNnul);
  DeleteDBEntry(DB14A3);
}


// While we are processing a Missile Timer, the timer
// entry no longer exists in the time queue.  We have
// a copy of it right here.  If the MissileFilter want
// manipulate the timer, it must manipulate this copy
// of it.
RN    missileFilterObject = RNnul;
TIMER missileFilterTimer;


//*********************************************************
//
//*********************************************************
//   TAG00e962
void MissileTimer(TIMER *pTimer)
{//()
  // A thrown Screamer Slice passes this way, too.
  // A fireball is represented by a timer entry with:
  //    function = 48 then 49.
  //    obj6     = Record Name  (and position in cell)
  //    word8    = mapX (bits 0-4)
  //               mapY (bits 5-9)
  //               direction of travel (bits 10-11)
  //               delta energy (bits 12-15)
  //        The object is in database 0x0e (8 bytes)
  //
  dReg D0;
  RN objD0;
  DIRECTION missileDirection;
  DIRECTION newMissileDirection;

  //TIMER timer;
  DB14 *pDBMissile;
  i32 origMapY=-999;
  i32 origMapX=-999;
  i32 curMapY; // Set to current position then moved.
  i32 curMapX; // Set to current position then moved.
  i32 curPos;
  bool leavingThisCell;
  //RN  objD7;
  //RN  objMissile;
  //i32 nextCellFlags;
  i32 deltaEnergy; // From timer entry
  i32 newX, newY, newDir;
  i32 saveTimerIndex;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (missileFilterObject != RNnul)
  {
    die(0x567c, "MissileFilter");
  };
  memmove((pnt)&missileFilterTimer, (pnt)pTimer, 16); // 5 words = 10 bytes
  missileFilterObject = missileFilterTimer.timerObj6();
  //objD7 = objMissile;
  ASSERT(missileFilterObject.dbType() == dbMISSILE,"missile");
  pDBMissile = GetRecordAddressDB14(missileFilterObject);
  saveTimerIndex = pDBMissile->timerIndex();
  pDBMissile->timerIndex(0xffff);
  origMapX = BITS0_4(missileFilterTimer.timerWord8()); //mapX
  origMapY = BITS5_9(missileFilterTimer.timerWord8()); //mapY
  curMapX = origMapX;
  curMapY = origMapY;
  if (missileFilterTimer.Function() == TT_Missile0)
  {
    missileFilterTimer.Function(TT_Missile);
  }
  else
  {
    curPos = missileFilterObject.pos();
    if (   (d.LoadedLevel == d.partyLevel)
        && (curMapX == d.partyX)
        && (curMapY == d.partyY) )
    {
      // Did missile strike someone?
      D0W = ProcessMissileEncounter(
                   -2,  // Check party members
                   curMapX,  // party location
                   curMapY,
                   curMapX,  // missile location
                   curMapY,
                   curPos,
                   missileFilterObject,
                   NULL);
      if (D0W != 0) 
      {
        missileFilterObject = RNnul;
        return; // Missile is discarded.  Gone!
      };
    };
    objD0 = FindFirstMonster(curMapX, curMapY);
    if (objD0 != RNeof)
    {
      D0W = ProcessMissileEncounter(
             -1,  // check monsters???
             curMapX,   // Monster location
             curMapY,
             curMapX,   // Missile location
             curMapY,
             curPos,
             missileFilterObject,
             NULL);
      if (D0W != 0) 
      {
        missileFilterObject = RNnul;
        return;
      };
    };
    deltaEnergy = BITS12_15(missileFilterTimer.timerWord8());
    if (pDBMissile->rangeRemaining() <= deltaEnergy)
    {
      LOCATIONREL missileLocr;
      missileLocr.l = d.LoadedLevel;
      missileLocr.x = curMapX;
      missileLocr.y = curMapY;
      missileLocr.p = curPos;
      MissileEncounterFilter(5, missileLocr, missileFilterObject, pDBMissile->flyingObject(), -1);
      RemoveObjectFromRoom(missileFilterObject, curMapX, curMapY, NULL);
      ProcessObjectFromMissile(missileFilterObject, NULL, curMapX, curMapY);
      missileFilterObject = RNnul;
      return;
    };
    pDBMissile->rangeRemaining(
                  pDBMissile->rangeRemaining() - deltaEnergy);
    if (pDBMissile->damageRemaining() < deltaEnergy)
    {
      pDBMissile->damageRemaining(0);
    }
    else
    {
      pDBMissile->damageRemaining(
                    pDBMissile->damageRemaining() - deltaEnergy);
    };
  };
  do
  {
    missileDirection = BITS10_11(missileFilterTimer.timerWord8());
    //objD7 = timer.timerObj6();
    curPos = missileFilterObject.pos();
    leavingThisCell =   (missileDirection==curPos)
                     || (((missileDirection+1)&3) == curPos);
    curMapX = origMapX;
    curMapY = origMapY;
    if (leavingThisCell)
    {
      bool nextIsStairs, origIsStairs;
      bool nextIsStoneWall;
      bool nextIsFalseWall;
      bool falseWallIsSolid;
      i32  origCellFlags, nextCellFlags;
      ROOMTYPE nextRoomType;
      curMapX += d.DeltaX[missileDirection];
      curMapY += d.DeltaY[missileDirection];
      nextCellFlags = GetCellFlags(curMapX, curMapY);
      nextRoomType = (ROOMTYPE)(nextCellFlags >> 5);
      if (    (curMapX < 0) 
           || (curMapX >= d.width)
           || (curMapY < 0)
           || (curMapY >= d.height)
         )
      {
        nextRoomType = roomEXTERIOR;
      };
        //Cell directly ahead.
//    if (   (nextCellFlags>>5)
//        && (((nextCellFlags>>5)!=roomFALSEWALL) || (nextCellFlags&5)) )
//    {
//      if ((nextCellFlags>>5) != roomSTAIRS) goto tag00eb90;
//      if ((d.LevelCellFlags[origMapX][origMapY]>>5) != roomSTAIRS )
//                              goto tag00eb90;
//    };
      origCellFlags    = d.LevelCellFlags[origMapX][origMapY];
      nextIsStoneWall  = (nextRoomType == roomSTONE) || (nextRoomType == roomEXTERIOR);
      nextIsStairs     = nextRoomType == roomSTAIRS;
      origIsStairs     = (origCellFlags>>5) == roomSTAIRS;
      nextIsFalseWall  = nextRoomType == roomFALSEWALL;
      falseWallIsSolid = (nextCellFlags&5)  == 0;
      if (    nextIsStoneWall
           || (nextIsFalseWall && falseWallIsSolid)
           || (nextIsStairs && origIsStairs)
        )
      {
        // Will missile strike wall?
        D0W = ProcessMissileEncounter(
                nextRoomType,  //Cell type
                origMapX,    
                origMapY,
                origMapX,
                origMapY,   // missile location 
                curPos,
                missileFilterObject,
                NULL);
        if (D0W != 0) 
        {
          missileFilterObject = RNnul;
          return; // Missile is gone.
        };
      };
    };
    newMissileDirection = BITS10_11(missileFilterTimer.timerWord8());
  } while (newMissileDirection != missileDirection);
//tag00eb90:
  if ((missileDirection&1) == (curPos&1))
  {
    curPos -= 1;
  }
  else
  {
    curPos += 1;
  };
  //D4W &= 3;
  // (Later PRS 12/02) objD7.pos(curPos);
  if(leavingThisCell)
  {
    missileFilterObject.pos(curPos);
    MoveObject(missileFilterObject, origMapX, origMapY, curMapX, curMapY, NULL, NULL);
    newX = d.NewX & 31; //Set to newX by MoveObject
    missileFilterTimer.timerWord8() &= 0xffe0;
    missileFilterTimer.timerWord8() |= newX;   // New mapX to timer entry
    newY = d.NewY & 31; //Set to newY by MoveObject
    missileFilterTimer.timerWord8() &= 0xfc1f;
    missileFilterTimer.timerWord8() |= newY << 5; // New mapY to timer entry
    newDir = d.NewDir & 3; //Set to new Direction by MoveObject
    missileFilterTimer.timerWord8() &= 0xf3ff;
    missileFilterTimer.timerWord8() |= newDir<<10; //Set new direction in timer entry
    missileFilterObject.pos(d.NewPos); //Set new position in object. 
    //missileFilterTimer.timerTime &= 0xffffff;
    //D0L |= uw(d.NewLevel) << 24;
    //missileFilterTimer.timerTime |= (d.NewLevel<<24);
    missileFilterTimer.Level((ui8)d.NewLevel);
  }
  else
  {
    D0W = GetCellFlags(curMapX, curMapY);
    if ((D0W >> 5) == roomDOOR) // cell type 4
    {
      // Did missile strike door
      D0W = ProcessMissileEncounter(
             4,
             curMapX,
             curMapY,
             curMapX,
             curMapY,
             curPos,
             missileFilterObject,
             NULL);
      if (D0W != 0) 
      {
        missileFilterObject = RNnul;
        return;//Missile is gone.
      };
    };
    missileFilterObject.pos(curPos);
    RemoveObjectFromRoom(missileFilterObject, curMapX, curMapY, NULL);
    AddObjectToRoom(missileFilterObject, RN(RNempty), curMapX , curMapY, NULL);
  };
  //missileFilterTimer.timerTime++;
  missileFilterTimer.Time(missileFilterTimer.Time() + 1);
  missileFilterTimer.timerObj6() = missileFilterObject;
  missileFilterObject = RNnul;
  pDBMissile->timerIndex(gameTimers.SetTimer(&missileFilterTimer));
}


//*********************************************************
// Timer function 25 (0x19)
//*********************************************************
//   TAG00ecca
void ProcessTT_25(TIMER *pTimer)
{ //()
  //An open door spell???  Other missiles??
  dReg         D0, D1, D4, D5, D6, D7;
  aReg         A0, A1;
  DB15        *DB15A2;
  TIMER        timer; //26
  i16          w_16;
  MONSTERTYPE  mt_14=mon_undefined;
  i16          w_14;
  ui16         uw_12;
  RN           obj_10;
  MONSTERDESC *pmtDesc=NULL;
  ITEM16      *pi16A0;
  DB4         *pDB4_4=NULL;
  i32          damage;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = pTimer->timerUByte6(); //x
  D6W = pTimer->timerUByte7(); //y
  DB15A2 = GetRecordAddressDB15(pTimer->timerObj8());
  D4W = D6W;
  D4W = (UI8)(d.LevelCellFlags[D7W][D6W]); // Cell flag
  D4W = sw(BITS5_15(D4W));
  //SET(D0W, d.LoadedLevel==d.partyLevel);
  //if (D0B != 0)
  //{
    //SET(D0W, D7W == d.partyX);
  //};
  //if (D0B != 0)
  //{
    //SET(D0W, D6W == d.partyY);
  //};
  if (   (d.LoadedLevel==d.partyLevel)
      && (D7W==d.partyX)
      && (D6W==d.partyY) ) w_16=1; else w_16=0;
  //w_16 = D0W & 1;
  obj_10 = FindFirstMonster(D7W, D6W);
  if (obj_10 != RNeof)
  {
    pDB4_4 = GetRecordAddressDB4(obj_10);
    mt_14 = pDB4_4->monsterType();
    pmtDesc = &d.MonsterDescriptor[mt_14];
  };
  uw_12 = uw(0xff80 + (DB15A2->cloudType()));
  if (uw_12 == 0xff87) //Poison cloud
  {
    D0L = Smaller(DB15A2->value()/32, 4);
    D1L = STRandomBool();
    D5W = sw(Larger(1, D0W + D1W));
  }
  else
  {
    D5W = sw(DB15A2->value()/2 + 1);
    //D0L = STRandom() & 0xffff;
    D5W = sw(D5W + STRandom(D5W) + 1);
  };
  switch (uw_12)
  {
  case 0xff82:
      D5W = sw(D5W / 2);
      if (D5W == 0) break;
      // Note fall-through.
  case 0xff80: //Fireball
      if (D4W == 4)
      {
        HitDoor(D7W, D6W, D5W, 1, 0);
      };
      break;

  case RNDispellMissile:
      if (obj_10 != RNeof)
      {
        if (pmtDesc->nonMaterial())
        {
          if ( (mt_14==mon_Zytaz) && (d.LoadedLevel==d.partyLevel) )
          {
            w_14 = sw(D5W/8);
            D5W = sw(D5W - w_14);
            w_14 = sw(2*w_14 + 1);
            D4W = pDB4_4->numMonM1(); //#monsters-1
            do
            {
              D0W = sw(pDB4_4->groupIndex());
              ASSERT(D0W < d.MaxITEM16,"maxitem16");
              pi16A0 = &d.Item16[D0W];
              if (pi16A0->singleMonsterStatus[D4W].TestAttacking())
              {
                damage = STRandom(w_14);
                DamageMonster(
                     pDB4_4,
                     D4W,
                     D7W,
                     D6W,
                     sw(damage + STRandom0_3() + D5W),
                     1,
                     false,
                     NULL);
                // The following did not work because the debug and release
                // versions called the two Random functions in a different order!
                //DamageMonster(
                //     pDB4_4,
                //     D4W,
                //     D7W,
                //     D6W,
                //     sw(STRandom0_3() + STRandom(w_14) + D5W),
                //     1);
              };
              D4W--;
            } while (D4W >= 0);
          }
          else
          {
            DamageMonsterGroup(pDB4_4, D7W, D6W, D5W, 1, false, NULL);
          };
        };
      };
      break;
  case 0xffe4:
      //D3W = (DB15A2->type() + 1) & 0x7f;
      DB15A2->cloudType((CLOUDTYPE)(DB15A2->cloudType()+1));
      QueueSound(5, D7W, D6W,1);
      goto tag00f020;
  case 0xffa8:
      if (DB15A2->value() > 55)
      {
        //.W = DB15A2->value();
        //.W = D0W - 40;
        //D3W <<= 8;
        DB15A2->value(DB15A2->value()-40);
        goto tag00f020;
      };
      break;
  case 0xff87: //Poison Cloud
      if (w_16 != 0)
      {
        DamageAllCharacters(D5W, 0, 0);
      }
      else
      {
        if (obj_10 != RNeof)
        {
          D5W = DeterminePoisonDamage(mt_14, D5W);
          if (D5W != 0)
          {
            MONSTER_DAMAGE_RESULT monsterDamageResult;
            monsterDamageResult = DamageMonsterGroup(
                      pDB4_4,
                      D7W,
                      D6W,
                      D5W,
                      1,
                      false,
                      NULL);
            if (monsterDamageResult != ALL_MONSTERS_DIED)
            {
              if (D5W > 2)
              {
                ProcessTimers29to41(D7W, D6W, TT_M3, 0);
              };
            };
          };
        };
      };
      if (DB15A2->value() < 6) break;
      //D3W = (BITS8_15(dbA2->word(2)) - 3) & 0xff;
      //D3W <<= 8;
      DB15A2->value(DB15A2->value()-3);
tag00f020:
      A1 = (pnt)pTimer;
      A0 = (pnt)&timer;
      //MoveWords(A0, A1, 4); // 10 bytes
      memcpy(A0, A1, sizeof(TIMER));
      //timer.timerTime++;
      timer.Time(timer.Time() + 1);
      gameTimers.SetTimer(&timer);
      return;
  }; // end of switch statement
  RemoveObjectFromRoom(pTimer->timerObj8(),
                       D7W,
                       D6W,
                       NULL);
  //DB15A2->link(RNnul);
  DeleteDBEntry(DB15A2);
}

//*********************************************************
//
//*********************************************************
//  TAG00f132
bool OpenTeleporterPitOrDoor(i32 mapX, i32 mapY)
{
  i32 cf;
  cf = (GetCellFlags(mapX, mapY) & 255) >> 5;
  return   (cf == roomOPEN)
         ||(cf == roomTELEPORTER)
         ||(cf == roomPIT)
         ||(cf == roomDOOR);
}


GameTimers::GameTimers()
{
  m_timerQueue = NULL;
  m_searchList = NULL;
  m_timers = NULL;
  m_maxTimers = 0;
  m_searchActive = false;
  m_timerSequence = 0;
}

GameTimers::~GameTimers()
{
  if (m_timerQueue != NULL) UI_free(m_timerQueue);
  if (m_timers != NULL) UI_free(m_timers);
  if (m_searchList != NULL) UI_free(m_searchList);
  m_timerQueue = NULL;
  m_searchList = NULL;
  m_timers = NULL;
  m_maxTimers = 0;
}

void GameTimers::Allocate(i32 numEnt)
{
  i32 i;
  if (m_maxTimers >= numEnt) return;
  m_timerQueue = (ui16 *)UI_realloc(m_timerQueue, numEnt*sizeof(m_timerQueue[0]), MALLOC110);
  if (m_timerQueue == NULL)
  {
    die(0x41fb,"Cannot allocate timer index");
  };
  m_searchList = (ui16 *)UI_realloc(m_searchList, numEnt*sizeof(m_timerQueue[0]), MALLOC110);
  if (m_searchList == NULL)
  {
    die(0x41fb,"Cannot allocate timer search list");
  };
  m_timers = (TIMER *)UI_realloc(m_timers,numEnt*sizeof(TIMER), MALLOC110);
  if (m_timers == NULL)
  {
    die(0x41fb,"Cannot allocate timers");
  };
//  d.timerQue = (i16 *)m_index;
  //d.Timers = (TIMER *)m_timers;
  for (i=m_maxTimers; i<numEnt; i++)
  {
    memset(m_timers+i, 0, sizeof(m_timers[i]));
    m_timerQueue[i] = 0;
    m_timers[i].Function(TT_EMPTY);
  };
  m_maxTimers = (ui16)numEnt;
  //d.MaxTimers = (i16)m_size;
}

void GameTimers::Cleanup()
{
  if (m_timerQueue != NULL) UI_free(m_timerQueue);
  if (m_timers!= NULL) UI_free(m_timers);
  if (m_searchList != NULL) UI_free(m_searchList);
  m_searchList = NULL;
  m_timerQueue = NULL;
  m_timers = NULL;
  m_maxTimers = 0;
  m_timerSequence = 0;
}

TIMER *GameTimers::pTimer(HTIMER hTimer)
{
  if (hTimer == 0xffff)
  {
    if (missileFilterObject == RNnul)
    {
      die(0x5c5c, "Timer");
    }
    return &missileFilterTimer;
  }
  else
  {
    return m_timers+hTimer;
  };
}


GameTimers gameTimers;
//*********************************************************
//
//*********************************************************
//   TAG00fd1c
void GameTimers::InitializeTimers()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D7;
  TIMER *pTimer;
  //d.Timers = (TIMER *)malloc((ui16)d.MaxTimers*(ui32)10, 1);
  //d.timerQue = (i16 *)allocateMemory(d.MaxTimers*2,1);
  Allocate(m_maxTimers);
  if (d.gameState != GAMESTATE_ResumeSavedGame)
  {
    D7L = 0;
    for (pTimer=m_timers; D7W<m_maxTimers; D7W++, pTimer++)
    {
      pTimer->Function(TT_EMPTY);
    };

    //d.numTimer = 0;
    //d.FirstAvailTimer = 0;
    Clear();
    SetWatchdogTimer();
  };
}

//void GameTimers::TimerQueue(i32 /*v*/, ui16 /*index*/)
//{
//  NotImplemented();
//}

//void GameTimers::DecrementNumTimer()
//{
//  NotImplemented();
//}



void EnlargeTimerQueue(i32 numEnt)
{
  gameTimers.Allocate(numEnt);
}

//*********************************************************
//
//*********************************************************
bool TIMER::operator < (const TIMER& timer2) const // TAG00fd9e
{
  // return 1 if *p1 < *p2
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D1, D2;
  //A0 = (pnt)p1;
  //A1 = (pnt)p2;
  //D0L = 1;
  //D2L =        timerTime & 0xffffff; //D2=*p1
  D2L =        m_time; //D2=*p1
  D1L = timer2.m_time; //D1=*p2
  if (D2L < D1L) return true; // *p1 < *p2
  if (D2L > D1L) return false; // *p1 > *p2
  // Times are the same. Parameter messages come before everything else.
  if (timerFunction == TT_ParameterMessage)
  {
    if (timer2.timerFunction != TT_ParameterMessage) return true;
    // Both are Parameter messages; Compare sequence numbers.
    if (m_timerUByte5 < timer2.m_timerUByte5) return true;
    if (m_timerUByte5 > timer2.m_timerUByte5) return false;
    // Identical entries.  The smaller pointer comes first
#ifdef SEQUENCED_TIMERS
    if (m_timerSequence < timer2.m_timerSequence) return true;
    if (m_timerSequence > timer2.m_timerSequence) return false;
#endif
    if (&timer2 >= this) return true;
    return false;
  }
  else
  {
    if (timer2.timerFunction == TT_ParameterMessage) return false;
  };
  // Compare the first word.
  if (timerFunction > timer2.timerFunction) return true;
  if (timerFunction < timer2.timerFunction) return false;
  if (m_timerUByte5 > timer2.m_timerUByte5) return true;
  if (m_timerUByte5 < timer2.m_timerUByte5) return false;
  // Identical entries.  The smaller pointer comes first
#ifdef SEQUENCED_TIMERS
    if (m_timerSequence < timer2.m_timerSequence) return true;
    if (m_timerSequence > timer2.m_timerSequence) return false;
#endif
  if (&timer2 >= this) return true;
  return 0;
}

//  TAG00fdce
i16 GameTimers::FindTimerPosition(HTIMER P1)
{ // Find position of timer entry in timer queue.
  dReg D0, D1;
  ui16 *pwA0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //pwA0 = gameTimers.TimerQueue();
  pwA0 = m_timerQueue;
  D1L = m_maxTimers;
  D0L = 0;
  do
  {
    if (P1 == *(pwA0++)) return D0W;
    D0W++;
  } while (D0W < D1W);
//
  return 0;
}

//   TAG00fdf0
void GameTimers::AdjustTimerQueue(i32 timerQueueIndex)
{ // Current position of timer in queue.
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D0, D4, D5, D6, D7;
//  pnt A3;
  TIMER *ptA3, *ptA0;
  i16 LOCAL_2;
  D7W = sw(timerQueueIndex);
  D4L = m_numTimer;
  //D4W = D0W;
  if (D4W == 1) return;
  //D5W = gameTimers.TimerQueue()[D7W];
  D5W = m_timerQueue[D7W];
  ptA3 = &m_timers[D5W]; // New entry to insert
  LOCAL_2 = 0;
  while (D7W != 0) //Attempt to find entry
  { // Starting at middle word keep dividing the
    // que in half and moving the new entry down
    // as long as it is smaller than the one already
    // there.  Set local_2 if any such swap occurs.
    D6W = sw((D7W-1)>>1); // Locate middle item???
    //D0W = gameTimers.TimerQueue()[D6W];
    D0W = m_timerQueue[D6W];
    ptA0 = &m_timers[D0W];
    D0W = *ptA3 < *ptA0; // Is new smaller
    if (D0W == 0) break;
    //D0W = d.timerQue[D6W];
    //TimerQueue(D7W, gameTimers.TimerQueue()[D6W]);
    m_timerQueue[D7W] = m_timerQueue[D6W];
    D7W = D6W; //The new empty spot
    LOCAL_2 = 1; //we made a swap


  };
  if (LOCAL_2 == 0)
  {
    D4W = sw((D4W-2)/2);
    while (D7W <= D4W)
    {
      D6W = sw(2*D7W + 1);
      D0W = sw(D6W+1);
      if (D0W < m_numTimer)
      {
        //D0W = d.Pointer12998[D6W];
        //parm2 = &d.Pointer12994[D0W];
        //D0W = d.Pointer12998[D6W+1];
        //parm1 = &d.Pointer12994[D0W];
        //D0W = TestTimerLess(gameTimers.pTimer(gameTimers.TimerQueue()[D6W+1]),
        //                    gameTimers.pTimer(gameTimers.TimerQueue()[D6W]));
        D0W = m_timers[m_timerQueue[D6W+1]] < m_timers[m_timerQueue[D6W]];
        if (D0W != 0) D6W++;
      };
      D0W = m_timers[m_timerQueue[D6W]] < *ptA3;
      if (D0W == 0) break;
      //D0W = gameTimers.TimerQueue()[D6W];
      D0W = m_timerQueue[D6W];
      //gameTimers.TimerQueue(D7W, D0W);
      m_timerQueue[D7W] = D0W;
      D7W = D6W;


    }; //while
  };
  //gameTimers.TimerQueue(D7W, D5W);
  m_timerQueue[D7W] = D5W;
}


i16 TIMER_SEARCH::FindQPos()
{
  if (gameTimers.m_searchList[m_index] == gameTimers.m_timerQueue[m_index])
  {
    return (ui16)m_index;
  }
  else
  {
    return gameTimers.FindTimerPosition(TimerHandle());
  };
}



void TIMER_SEARCH::AdjustTimerPriority()
{
  gameTimers.AdjustTimerQueue(FindQPos());
}

void GameTimers::AdjustTimerPriority(HTIMER hTimer)
{
  AdjustTimerQueue(FindTimerPosition(hTimer));
}


//*********************************************************
//
//*********************************************************
#ifdef _DEBUG
#define ASSERTTimer(x) (ASSERT(x,"TimerCheck"))
#else
#define ASSERTTimer(x)
#endif
bool GameTimers::CheckTimers()
{ // Return true if all is well.
  // We check that the queue is properly ordered.
  i32 n, N;
  N = m_numTimer;
  for (i32 i=0; i<N; i++)
  {
    if (m_timers[m_timerQueue[i]].Function() == 0)
    {
      return false;
    };
    for (n=2*i+1; n<=2*i+2; n++)
    {
      if (n >= N) break;
      if (m_timers[m_timerQueue[n]] < m_timers[m_timerQueue[i]])
      {
        return false;
      };
    };
  };
  return true;
}

//*********************************************************
//
//*********************************************************
//   TAG00ff4c
void GameTimers::DeleteTimer(HTIMER hTimer, const char *tag)
{
  i32 QPos;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ASSERTTimer(CheckTimers());
  TraceTimer(m_timers + hTimer, hTimer, tag==NULL?"Delete":tag);
  m_timers[hTimer].Function(TT_EMPTY);//  .WB8.bytes[0] = 0;
  if (hTimer < m_firstAvailTimer)
  {
    m_firstAvailTimer=hTimer;
  };
  m_numTimer--;
  //D5W = d.numTimer;
  if (m_numTimer != 0)
  {
    QPos = FindTimerPosition(hTimer);
    if (QPos != m_numTimer) // If it was not the last entry
    {
      //TimerQueue((ui16)QPos, m_timerQueue[m_numTimer]);
      m_timerQueue[QPos] = m_timerQueue[m_numTimer];
        // Move last entry into vacated position.
      AdjustTimerQueue(QPos);
    };
  };
  ASSERTTimer(CheckTimers());
}


//*********************************************************
//
//*********************************************************
//  TAG00ffbe
HTIMER GameTimers::SetTimer(TIMER *pNewTimer) //
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //i32 D5,D7;
  dReg D0, D1, D5, D7;
  aReg A0, A1;
  //i16 *pwA0;
  TIMER *ptA0, *ptA2;
  ASSERTTimer(CheckTimers());
  //ptA3 = P1;
  if (m_numTimer >= m_maxTimers - 5)
  {
    if (m_maxTimers >= 5000)
    {
      die(45, "No empty Timer entries");
    };
    //gameTimers.FirstAvailTimer(gameTimers.MaxTimer());
    m_firstAvailTimer = m_maxTimers;
    EnlargeTimerQueue((m_maxTimers+10)*5/4);
  };
  if (   (pNewTimer->Function() >= TT_MAP_FIRST) 
      && (pNewTimer->Function() <= TT_MAP_LAST))
  {
    if (deleteDuplicateTimers != 0)
    {
      int D7W;
      TIMER_SEARCH timerSearch;
      //for (D7W=0, ptA2=gameTimers.Timers(); D7W<d.MaxTimer(); D7W++, ptA2++) xxTIMERxx Change to FindNextTimer
      while (timerSearch.FindNextTimer())
      {
        ptA2 = timerSearch.TimerAddr();
        if (   (ptA2->Function() >= TT_MAP_FIRST)
            && (ptA2->Function() <= TT_MAP_LAST)  )
        {
          //if (pNewTimer->timerTime != ptA2->timerTime) continue;
          if (pNewTimer->Time()  != ptA2->Time()) continue;
          if (pNewTimer->Level() != ptA2->Level()) continue;
          if (pNewTimer->timerWord6() != ptA2->timerWord6()) continue;
          if (ptA2->Function() == TT_STONEROOM)
          {
            if (ptA2->timerUByte8() != pNewTimer->timerUByte8()) continue;
          };
          ptA2->timerUByte9(pNewTimer->timerUByte9());
          TraceTimer(ptA2, timerSearch.TimerHandle(), "Byte9");
          ASSERTTimer(CheckTimers());
          return (i16)timerSearch.TimerHandle();//goto tag010224;
        };
        if (ptA2->Function() != TT_1) continue;
        //if (pNewTimer->timerTime != ptA2->timerTime) continue;
        if (pNewTimer->Time()  != ptA2->Time()) continue;
        if (pNewTimer->Level() != ptA2->Level()) continue;
        if (pNewTimer->timerWord6() != ptA2->timerWord6()) continue;
        if (pNewTimer->timerUByte9() == 2)
        {
          D0W = sw(1 - ptA2->timerUByte9());
          pNewTimer->timerUByte9(D0B);
          TraceTimer(pNewTimer,0,"Byte9");
        };
        timerSearch.DeleteTimer();
        TraceTimer(ptA2,timerSearch.TimerHandle(),"Delete");
        break;

      }; //for (D7W, A2)
    };
  }
  else
  {
    if (pNewTimer->Function() == TT_1) // goto tag010154;
    
    //  =====================================================
    //The old way... look through all timer entries
    {
      i32 D7W;
      TIMER_SEARCH timerSearch;
      //for (D7W=0, ptA2=gameTimers.Timers(); D7W<d.MaxTimer(); D7W++, ptA2++) xxTIMERxx Change to FindNextTimer
      while (timerSearch.FindNextTimer())
      {
        ptA2 = timerSearch.TimerAddr();
        //if (pNewTimer->timerTime != ptA2->timerTime) continue;
        if (pNewTimer->Time()  != ptA2->Time()) continue;
        if (pNewTimer->Level() != ptA2->Level()) continue;
        if (pNewTimer->timerWord6() != ptA2->timerWord6()) continue;
        if (ptA2->Function() == TT_DOOR)
        {
          if (ptA2->timerUByte9() == 2)
          {
            ptA2->timerUByte9(ub(1 - pNewTimer->timerUByte9()));
            TraceTimer(ptA2,timerSearch.TimerHandle(),"byte9");
          }
          else
          {
            TraceTimer(ptA2,timerSearch.TimerHandle(),"Unchanged");
          };
          ASSERTTimer(CheckTimers());
          return timerSearch.TimerHandle();
        }
        else if (ptA2->Function() == TT_1)
        {
          ptA2->timerUByte9(pNewTimer->timerUByte9());
          TraceTimer(ptA2, timerSearch.TimerHandle(), "Byte");
          ASSERTTimer(CheckTimers());
          return timerSearch.TimerHandle();
        };
      }; // for (D7W, uA2)
//goto tag0101c2;
    }
    // End of the old way.....look through all timer entries
    // ================================================================
    else
    {
//tag010154:
      if (pNewTimer->Function() == TT_BASH_DOOR)
      {// goto tag0101c2;
        TIMER_SEARCH timerSearch;
        //for (D7W=0, ptA2=gameTimers.Timers(); D7W<d.MaxTimer(); D7W++, ptA2++) xxTIMERxx Change to FindNextTimer
        while (timerSearch.FindNextTimer())
        {
          ptA2 = timerSearch.TimerAddr();
          if (pNewTimer->timerWord6() != ptA2->timerWord6()) continue;
          //D0L = (pNewTimer->timerTime>>24)&0xff;
          D0L = pNewTimer->Level();
          //D1L = (ptA2->timerTime>>24)&0xff;
          D1L = ptA2->Level();
//018e 6620                     BNE      $+34 (=0x0101b0)
          if (D1L != D0L) continue;
          if (ptA2->Function() != TT_1)
          {
            if (ptA2->Function() != TT_DOOR) continue;
          };
          timerSearch.DeleteTimer();
          TraceTimer(ptA2,timerSearch.TimerHandle(),"Delete");
//
        }; // for (D7W, A2)
      };
    };
  };
//tag0101c2:
  A1 = (pnt)pNewTimer;
  D5L = m_firstAvailTimer;
  ASSERT(m_firstAvailTimer < m_maxTimers,"maxTimer");
  A0 = (pnt)&m_timers[D5W];//A0 is next available timer entry
  //MoveWords(A0, A1, 4); // Copy Timer to d.timers array.
  memcpy(A0, A1, sizeof(TIMER));
#ifdef SEQUENCED_TIMERS
#define TimerSequencePageSize 32
  if (m_timerSequence == 0xffff)
  {
    // We must repack the sequence numbers every 65000 entries or thereabouts.
    // This should not be too tramatic compared to all that business of
    // deleting monster timers and such.
    ui8 *usedPages;
    ui16 *pageTranslate;
    ui32 maxPage;
    usedPages = (ui8 *)UI_malloc(0x10000/TimerSequencePageSize,MALLOC115);
    memset(usedPages,0,0x10000/TimerSequencePageSize);
    pageTranslate = (ui16 *)UI_malloc(2*0x10000/TimerSequencePageSize,MALLOC115);
    maxPage = 0;
    //
    // Fisrt find all the pages that are in use.
    {
      TIMER_SEARCH timerSearch;
      while (timerSearch.FindNextTimer())
      {
        ui32 page;
        page = timerSearch.TimerAddr()->m_timerSequence / TimerSequencePageSize;
        if (page > maxPage) maxPage = page;
        usedPages[page] = 1;
      };
    };
    //
    // Then pack the pages down and build a translation table.
    {
      ui32 page, nextAvailPage;
      nextAvailPage = 0;
      for (page=0; page<=maxPage; page++)
      {
        if (usedPages[page] != 0)
        {
          pageTranslate[page] = (ui16)nextAvailPage++;
        };
      };
    };
    // Then translate all the timer sequence nubers to their new pages.
    {
      TIMER_SEARCH timerSearch;
      while (timerSearch.FindNextTimer())
      {
        i32 page, oldSeq, newSeq;
        page = timerSearch.TimerAddr()->m_timerSequence / TimerSequencePageSize;
        page = pageTranslate[page];
        oldSeq = timerSearch.TimerAddr()->m_timerSequence;
        newSeq = (oldSeq  % TimerSequencePageSize) + (page * TimerSequencePageSize);
        timerSearch.TimerAddr()->m_timerSequence = (ui16)newSeq;
      };
    };
    UI_free(usedPages);
    UI_free(pageTranslate);
  };
  m_timers[D5W].m_timerSequence = m_timerSequence++;
#else
  m_timers[D5W].m_timerSequence = 0;
#endif
  do
  {
    m_firstAvailTimer++; //Try to find an empty entry for next time.
    if (m_firstAvailTimer == m_maxTimers) break;
    ptA0 = &m_timers[m_firstAvailTimer];
  } while (ptA0->Function() != TT_EMPTY);
  //gameTimers.TimerQueue((ui16)gameTimers.NumTimer(), D5W);
  m_timerQueue[m_numTimer] = D5W;
  m_numTimer++;
  AdjustTimerQueue(m_numTimer-1);
  TraceTimer(&m_timers[D5W], D5W, "create");
  ASSERTTimer(CheckTimers());
  return D5W;
}


i32 GameTimers::CreateSearchList()
{
  if (m_searchActive) die(0xccb7, "Recursive timer search");
  m_searchActive = true;
  memcpy(m_searchList, m_timerQueue, m_numTimer*sizeof(m_timerQueue[0]));
  return m_numTimer;
}


//*********************************************************
//
//*********************************************************
//   TAG01022c
void GameTimers::GetNextTimerEntry(TIMER *P1, ui32 *index)
{
  TIMER *ptA1;
  pnt A0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  *index = m_timerQueue[0];
  ptA1 = m_timers + (*index);
  A0 = (pnt)P1;
  //MoveWords(A0, (pnt)ptA1, 4); // 5 words = 10 Bytes
  memcpy(A0, ptA1, sizeof(TIMER));
  DeleteTimer((HTIMER)*index,"Process");
}


//*********************************************************
//
//*********************************************************
//  TAG010260
bool GameTimers::CheckForTimerEvent()
{
  dReg D0, D1;
  bool zflag;
  D0L=m_numTimer;
  zflag=false;
  if (D0W == 0) {zflag=true; goto tag01028c;};
  //A0=d.Pointer12998;
  D0W = m_timerQueue[0];
  //D0 = D0W * 10;
  //A0 += D0;
  //D0 = pA0->long0 & 0xffffff;
  //D0L = m_timers[D0W].timerTime & 0xffffff;
  D0L = m_timers[D0W].Time();
  D1L = d.Time;
  if ( D0L <= D1L ) D0B=1; else D0B=0;
  zflag = (D0B==0);
tag01028c:
  if (zflag) D0B=0; else D0B=1;
  //if (D0B != 0)
  //{
  //  if (d.Word11754 < 2) D0B=1; else D0B=0;
  //};
  D0W &= 1;
  return D0W!=0;
}

//*********************************************************
//
//*********************************************************
//   TAG0102a4
void ProcessTT_1(TIMER *pTimer)
{//()
  static dReg D0, D1, D4, D5;
  static CELLFLAG *cfA2;
  static i16  w_10;
  //static i16  w_8;
  static RN   obj_6;
  static DB0      *pDB0_4;
  static int x, y;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  y = pTimer->timerUByte7();
  x = pTimer->timerUByte6();
  cfA2 = &d.LevelCellFlags[x][y];
  D4W = sw(*cfA2 & 7);
  if (D4W == 5) return;
  //pTimer->timerTime++;
  pTimer->Time(pTimer->Time() + 1);
  D5W = pTimer->timerUByte9();
  if (D5W == 1)
  {
    pDB0_4 = FirstObjectRecordAddressDB0(x, y);
    w_10 = pDB0_4->mode(); //up/down or sideways
    if (   (d.LoadedLevel == d.partyLevel)
        && (x == d.partyX)
        && (y == d.partyY)
        && (D4W != 0)  )
    {
      if (d.NumCharacter > 0)
      {
        *cfA2 &= 0xf8; // Clear bits 0-2
        w_10 |= 8;
        D0W = DamageAllCharacters(5, 4, 2);
        if (D0W != 0)
        {
          QueueSound(soundDOOROPENING, x, y, 1);
        };
      };
      //pTimer->timerTime++;
      pTimer->Time(pTimer->Time() + 1);
      gameTimers.SetTimer(pTimer);
      return;
    };
    obj_6 = FindFirstMonster(x, y);
    if (obj_6 != RNeof)
    {
      //w_8 = GetMonsterDescWord2(obj_6);
      if (!obj_6.NonMaterial())
      {
        if (w_10 != 0)
        {
          D1W = obj_6.VerticalSize();
        }
        else
        {
          D1W = 1;
        };
        if (D4W >= D1W)
        {
          MONSTER_DAMAGE_RESULT monsterDamageResult;
          monsterDamageResult = DamageMonsterGroup(
                    GetRecordAddressDB4(obj_6),
                    x,
                    y,
                    5,
                    1,
                    false,
                    NULL);
          if (monsterDamageResult != ALL_MONSTERS_DIED)
          {
            ProcessTimers29to41(x, y, TT_M3, 0);
          };
          if (D4W == 0)
          {
            D0W = 0;
          }
          else
          {
            D0W = sw(D4W - 1);
          };
          D4W = D0W;
          *cfA2 = ub((*cfA2 & 0xf8) | D4W);
          QueueSound(4, x, y, 1);
          //pTimer->timerTime++;
          pTimer->Time(pTimer->Time() + 1);
          gameTimers.SetTimer(pTimer);
          return;
        };
      };
    };
  };
//
  if (   ((D5W==0) && (D4W==0))
      || ((D5W==1) && (D4W==4)) ) return;

  if (D5W == 0)
  {
    D0W = -1;
  }
  else
  {
    D0W = 1;
  };
  D4W = sw(D4W + D0W);
  *cfA2 = ub((*cfA2 & 0xf8) | D4W);
  QueueSound(2, x, y, 1);
//
//
//
  if (   ((D5W != 0) && (D4W != 4))
      || ((D5W == 0) && (D4W != 0))  )
  {
    gameTimers.SetTimer(pTimer);
  };
}

//*********************************************************
//
//*********************************************************
//   TAG0104bc
void ProcessTT_FALSEWALL(TIMER *pTimer, ui32 index) //function = 7
{
  dReg D5;
  CELLFLAG *pcfA2;
  RN    objD4;
  i32 mapX, mapY;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  mapY = pTimer->timerUByte7();
  mapX = pTimer->timerUByte6();
  if (pTimer->Function() == TT_ParameterMessage)
  {
    pDSAparameters[0] = expool.Read((EDT_MessageParameters<<24) + index,pDSAparameters+1, 100);
    pTimer->Function(TT_STONEROOM);
  }
  else
  {
    pDSAparameters[0] = 0;
  };
  {
    RN obj;
    DB3 *pDB3;
    i32 actuatorType;
    for (obj=FindFirstObject(d.LoadedLevel,mapX, mapY);
         obj != RNeof;
         obj = GetDBRecordLink(obj))
    {
      if (timerTypeModifier[pTimer->timerUByte9()] == 3) return;
      if (obj.dbType() != dbACTUATOR) continue;
      pDB3 = GetRecordAddressDB3(obj);
      actuatorType = pDB3->actuatorType();
      if (   (actuatorType == 127)
          && (obj.pos() == pTimer->timerUByte8()))
      {
        ProcessPortraitTimer7(obj, pTimer);
        continue;
      };
      if (actuatorType == 47)
      {
        LOCATIONREL locr;
        locr.l = d.LoadedLevel;
        locr.x = mapX;
        locr.y = mapY;
        locr.p = obj.pos();
        if (ProcessDSATimer7(obj, pTimer, locr)) break;
        continue;
      };
    };
  };
  pcfA2 = &d.LevelCellFlags[mapX][mapY];
  D5L = timerTypeModifier[pTimer->timerUByte9()];//Action to perform
  if (D5W == 2) // if toggle
  {
    D5W = sw(*pcfA2 & 4);
    if (D5W != 0)
    {
      D5W = 1; // Clear if currently set
    }
    else
    {
      D5W = 0; //Set if currently clear.
    };
  };
  if (D5W == 1)
  {
//
//
    if (   (d.LoadedLevel == d.partyLevel)
        && (mapX == d.partyX)
        && (mapY == d.partyY) )
    {
      //pTimer->timerTime++;
      pTimer->Time(pTimer->Time() + 1);
      gameTimers.SetTimer(pTimer); //Wait for party to leave
    }
    else
    {
      objD4 = FindFirstMonster(mapX, mapY);
      if (   (objD4 != RNeof)
          && objD4.NonMaterial() )
      {
        //pTimer->timerTime++;
        pTimer->Time(pTimer->Time() + 1);;
        gameTimers.SetTimer(pTimer); // Wait for Monsters to leave
      }
      else
      {
        *pcfA2 &= 0xfb; // clear 0x04 = ??
        TraceCellflags(mapX, mapY, "Clear 0x04");
      };
    };
  }
  else
  {
    *pcfA2 |= 0x04;
    TraceCellflags(mapX, mapY, "Set 0x04");
  };
}

//*********************************************************
//
//*********************************************************
//   TAG010576
void ProcessTT_BASH_DOOR(TIMER *pTimer)
{ //When I bashed a door down.
  CELLFLAG *pCF;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pCF = &d.LevelCellFlags[pTimer->timerUByte6()][pTimer->timerUByte7()];
  SETBBITS0_2(*pCF, 5); //Set door as bashed.
}

void ActivateDSA(int level, int x, int y, int pos, int action)
{
  RN objActuator;
  if (level != d.LoadedLevel)
  {
    NotImplemented(0xcca12);
  };
  for (objActuator = FindFirstObject(x,y);
       objActuator != RNeof; 
       objActuator = GetDBRecordLink(objActuator) )
  {


    DB3 *pActuator;
    i32 actuatorType;
    if (objActuator.dbType() != dbACTUATOR) continue;



    pActuator = GetRecordAddressDB3(objActuator);
    actuatorType = pActuator->actuatorType();
    if (actuatorType == 47)
    {
      LOCATIONREL locr(level, x, y);
      TIMER timer;
      NEWDSAPARAMETERS dsaParameters; // Ensure DSAparameters[0] = 0;
      timer.timerUByte9((ui8)action);
      timer.timerUByte8((ui8)pos);
      timer.timerUByte7((ui8)y);
      timer.timerUByte6((ui8)x);
      //timer.timerTime   = level << 24;
      timer.Time(0);
      timer.Level((ui8)level);
    
      ProcessDSATimer5(objActuator, &timer, locr);
    }; 


  };
}


//*********************************************************
// Under certain circumstances this timer will queue
// a type 1 timer with all other parameters the same.
// It does nothing else.
// The following table has an 'X' for those cases in
// which a new timer is queued.
//
//                    Cell flag & 7
//                   0 1 2 3 4 5 6 7
// timer byte 9 = 0  . X X X X . X X
// timer byte 9 = 1  X X X X . . X X
// timer byte 9 = 2  . X X X . . X X
//*********************************************************
//   TAG0105ba
void ProcessTT_DOOR(TIMER *pTimer)
{
  dReg D7;
  CELLFLAG *cfA0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D0B = pTimer->uByte7;
  if (timerTypeModifier[pTimer->timerUByte9()] == 3) return;
  ActivateDSA(d.LoadedLevel,
              pTimer->timerUByte6(),   //x
              pTimer->timerUByte7(),   //y
              pTimer->timerUByte8(),   //pos
              timerTypeModifier[pTimer->timerUByte9()]);  //action
  cfA0 = &d.LevelCellFlags[pTimer->timerUByte6()][pTimer->timerUByte7()];
  D7W = sw(*cfA0 & 0x7); // Bottom 3 bits of cell flag
  if (D7W == 5) return;
  if (timerTypeModifier[pTimer->timerUByte9()] == 2)
  {
    if (D7W == 0) pTimer->timerUByte9(1);
    else pTimer->timerUByte9(0);
  }
  else
  {
    if (pTimer->timerUByte9() == 0)
    {
      if (D7W == 0)  return;
    }
    else
    {
      if (D7W == 4) return;
    };
  }
  pTimer->Function(TT_1);
  gameTimers.SetTimer(pTimer);
}


//*********************************************************
// Called when a timer function (type 6)
// is directed to an actuator of type 6.
// Or when a timer funtion type 5 or 6 is directed
// at an actuator of type 48 (My invention - PRS).
// SET increments the counter.  Anything else decrements
// it.  The actuator is considered 'pressed' if the count
// goes to zero.
// For some reason, you cannot increment a counter that
// is initially zero.
//*********************************************************
void OperateCounterActuator(const TIMER *pTimer, DB3 *pActuator)
{
  i32 counterContents;
  bool swpressed, swclosed;
  i32 targetX, targetY, targetPos;
  const char *asciiAction;
  if (timerTypeModifier[pTimer->timerUByte9()] == 3) return;
  targetX   = pActuator->targetX();
  targetY   = pActuator->targetY();
  targetPos = pActuator->targetPos();
  counterContents = pActuator->value();

  if (counterContents == 0)
  {
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"Value is zero so no action\n");
    };
  };
  if (counterContents > 0)
  {  // Cannot affect a counter that equals zero. (Why?)
    if (timerTypeModifier[pTimer->timerUByte9()] == 0) // Action to perform == SET
    {                        // implies increment.
      if (counterContents < 511)
      {
        counterContents++;
        asciiAction = "Increment";
      }
      else
      {
        asciiAction = "Max value so no increment";
      };
    }
    else
    {
      counterContents--;
      asciiAction = "Decrement";
    };

    pActuator->value(counterContents);
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"%s newVal= %03x\n",
                        asciiAction, counterContents);
    };
    if (pActuator->action() == actuatorAction_CONSTPRESSURE)
    {
      swpressed = counterContents==0;
      swclosed = swpressed ^ pActuator->normallyClosed();
      if (TimerTraceActive)
      {
        fprintf(GETFILE(TraceFile), " ConstPressure %s\n",
                    swclosed?"Closed":"Open");
      };
      QueueSwitchAction(pActuator,
                        swclosed ? 0 : 1,
                        targetX,
                        targetY,
                        targetPos);
    }
    else
    {
      if (counterContents == 0)
      {
        if (TimerTraceActive)
        {
          fprintf(GETFILE(TraceFile)," Pressed %s\n",
                  pActuator->normallyClosed()?"NC":"NO");
        }
        QueueSwitchAction(pActuator,
                          pActuator->action(),
                          targetX,
                          targetY,
                          targetPos);
      };
    };
  };
}



//*********************************************************
//
//*********************************************************
//   TAG010642
void ProcessTT_OPENROOM(TIMER *pTimer, ui32 index)
{//()
  dReg D5, D6, D7;
  i32 targetX, targetY;
  RN  objD4;
  DB2 *pDB2_4;
  DB3 *pDB3;
  i32 w_10;
  TIMER timer;
  targetY = pTimer->timerUByte7();
  targetX = pTimer->timerUByte6();
  if (pTimer->Function() == TT_ParameterMessage)
  {
    pDSAparameters[0] = expool.Read((EDT_MessageParameters<<24) + index,pDSAparameters+1, 100);
    //pTimer->Function(TT_OPENROOM);
  }
  else
  {
    pDSAparameters[0] = 0;
  };
  for (objD4 = FindFirstObject(targetX, targetY);
       objD4 != RNeof;
       objD4 = GetDBRecordLink(objD4) )
  {
    if (timerTypeModifier[pTimer->timerUByte9()] == 3) return;
    D7W = (I16)objD4.dbType();
    if (   (D7W == dbTEXT) 
        && (pTimer->Function() != TT_DESSAGE) 
        && (pTimer->Function() != TT_ParameterMessage))
    {
      pDB2_4 = GetRecordAddressDB2(objD4);
      D6W = pDB2_4->show();
      switch (timerTypeModifier[pTimer->timerUByte9()])
      {
      case 0:  pDB2_4->show(1); break;
      case 1:  pDB2_4->show(0); break;
      case 2:  pDB2_4->show(pDB2_4->show() ^ 1);  break;
      };





      if (   (D6W==0)
          && (pDB2_4->show() != 0)
          && (d.LoadedLevel == d.partyLevel)
          && (targetX == d.partyX)
          && (targetY == d.partyY)      )
      {
        char text[1000];
        DecodeText(text, 
                   GetRecordAddressDB2(objD4), 
                   1, 
                   990);
        QuePrintLines(15, text);
      };
    } // End DB2 code
    else
    {
      if (D7W == dbACTUATOR) //DB3
      {
        pDB3 = GetRecordAddressDB3(objD4);
        if (pDB3->actuatorType() == 47)
        {
          LOCATIONREL locr;
          locr.l = d.LoadedLevel;
          locr.x = targetX;
          locr.y = targetY;
          locr.p = objD4.pos();
          if (ProcessDSATimer5(objD4, pTimer, locr)) break;
          continue;
        };
        if (pTimer->Function() == TT_DESSAGE) continue;
        if (pTimer->Function() == TT_ParameterMessage) continue;
        if (pDB3->actuatorType() == 48) //Counter
        {
          if (TimerTraceActive)
          fprintf(GETFILE(TraceFile), "Actuator6(counter) %03x %02x(%02x,%02x) ",
                     objD4.idx(),
                     d.LoadedLevel, targetX, targetY);

          OperateCounterActuator(pTimer, pDB3);
        };
        if (pDB3->actuatorType() == 6) // monster generator
        {
          D5W = pDB3->delay(); // # monsters or current value
          if ((D5W & 0x8) != 0)
          {
            D5W = sw(STRandom(D5W&7));
          }
          else
          {
            D5W -= 1; // modified delay
          };
          w_10 = pDB3->hitPointMultiplier();
          if (w_10 == 0)
          {
            w_10 = d.pCurLevelDesc->experienceMultiplier();
          };
          CreateMonster(
                        pDB3->value(),
                        w_10,
                        D5W,          //#monsters-1
                        STRandom0_3(), //facing??
                        targetX,
                        targetY,
                        pDB3->normallyClosed(),
                        pDB3->drawAsSize4()); //invisible
          if (pDB3->audible())
          {
            QueueSound(soundTELEPORT, targetX, targetY, 1);
          };
          if (pDB3->onceOnly())
          {
            pDB3->actuatorType(0);
          }
          else
          {
            w_10 = pDB3->disableTime();
            if (w_10 != 0)
            {
              pDB3->actuatorType(0);
              if (w_10 >= 128)
              {
                w_10 = 64 * (w_10 - 126);
              };
              timer.Function(TT_ReactivateGenerator);
              //timer.timerTime = d.Time + w_10 + (d.LoadedLevel<<24);
              timer.Time(d.Time + w_10);
              timer.Level((ui8)d.LoadedLevel);
              timer.timerUByte5(0);
              timer.timerUByte6(ub(targetX));
              timer.timerUByte7(ub(targetY));
              timer.timerObj8() = objD4;
              gameTimers.SetTimer(&timer);
            };
          };
        };
      };
    };
  }; // for ()
}

//*********************************************************
//
//*********************************************************
//   TAG0108a4
void ProcessTT_ReactivateGenerator(TIMER *pTimer)
{
  RN objD7;
  DB3 *DB3A2;
  // The original FTL code looked for the first actuator
  // with type == 0 and turned it into a monster generator.
  // We had a problem with this however.  If a 'once-only'
  // pressure pad was on the same square then, after the 
  // pad had been used once, its type is set to zero.  Then
  // when the timer regeneration timer came along it 
  // turned the pressure pad into a generator and that did 
  // not work well.  Now we store the generator's ID (RN id)
  // in the timer.  If we can find an actuator with that ID 
  // then we reactivate it.  Otherwise, we reactivate the 
  // first actuator with type==0.  That keeps us backward
  // compatible with old savegames.
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  RN generatorID;
  RN firstTypeZero;
  generatorID = pTimer->timerObj8();
  firstTypeZero = RNeof;
  for (objD7 = FindFirstObject(pTimer->timerUByte6(), pTimer->timerUByte7());
       objD7 != RNeof;
       objD7 = GetDBRecordLink(objD7) )
  {

    if (objD7.dbType() == dbACTUATOR)
    {
      DB3A2= GetRecordAddressDB3(objD7);
      if (DB3A2->actuatorType() != 0) continue;
      if (firstTypeZero == RNeof) firstTypeZero = objD7;
      if (objD7 == generatorID) break;
    };
//
//
  };
  if (objD7 == RNeof) objD7 = firstTypeZero;
  if (objD7 == RNeof) return;
  DB3A2 = GetRecordAddressDB3(objD7);
  DB3A2->actuatorTypeOR(6); // set two bits??? Generator = 6!
}

//*********************************************************
// Called from only one place.
// A Timer6 event was applied to an actuator
// of type 7, 8, 9, 10, 14, or 15.
//*********************************************************
//   TAG010910
void TriggerMissileLauncher(DB3 *pDB3, const TIMER *pTimer)
{//()
  //dReg D5;
  i32 actuatorTypeD4;
  RN  secondObject, firstObject;
  //i16 w_26;
  i16 objectPosition;
  bool T7or8or14; //w_14//actuator type 7, 8, or 14
  //i16 w_12;
  i16 energyRemaining;
  i16 missileType;
  i16 initialPosition;
  i16 timerMapY;
  i16 timerMapX;
  i32 launchX, launchY;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  timerMapX = pTimer->timerUByte6();
  timerMapY = pTimer->timerUByte7();
  //D5W = pTimer->timerUByte8; //position
  initialPosition = sw((pTimer->timerUByte8() + 2) & 3); //position
  actuatorTypeD4 = pDB3->actuatorType();
  missileType = pDB3->value();
  energyRemaining = sw(pDB3->energyRemaining());
  //w_12 = BITS12_15(pDB3->word6);
  T7or8or14 = (   (actuatorTypeD4==7)
               || (actuatorTypeD4==8)
               || (actuatorTypeD4==14));
  if ( (actuatorTypeD4 == 8) || (actuatorTypeD4==10) )
  {
    firstObject.CreateSpell(missileType);
    secondObject = firstObject;
  }
  else
  {
    if ( (actuatorTypeD4==14) || (actuatorTypeD4==15) )
    {
      for (firstObject =  FindFirstObject(timerMapX, timerMapY);
           firstObject != RNeof;//Original had RNnul;  // Why not RNeof??
           firstObject =  GetDBRecordLink(firstObject) )
      {
        ASSERT(firstObject != RNnul,"firstObj");
        objectPosition = firstObject.pos();
        if (firstObject.dbType() > dbACTUATOR)
        {
          if (objectPosition == pTimer->timerUByte8()) break; //correct position
          if (objectPosition == ((pTimer->timerUByte8() + 1) & 3)) break; //correct position
        };
//
      };
      ASSERT(firstObject != RNnul,"firstObj");
      if (firstObject == RNeof) return;// original had RNnul) return;
      RemoveObjectFromRoom(firstObject,
                           timerMapX, timerMapY,
                           NULL);
      if (!T7or8or14)
      {
        for (secondObject = FindFirstObject(timerMapX, timerMapY);
             secondObject != RNeof;
             secondObject = GetDBRecordLink(secondObject) )
        {
          objectPosition = secondObject.pos();
          if (secondObject.dbType() > dbACTUATOR)
          {
            if (objectPosition == pTimer->timerUByte8()) break; //correct position
            if (objectPosition == ((pTimer->timerUByte8()+1) & 3)) break; //correct position
          };
//
//
        };
        ASSERT(secondObject != RNnul,"secondObj"); // Should this be RNeof?
        if (secondObject == RNeof)//changed from RNnul
        {
          T7or8or14 = 1;
        }
        else
        {
          RemoveObjectFromRoom(secondObject,
                               timerMapX,
                               timerMapY,
                               NULL);
        };
      };
    }
    else
    {
      firstObject = ConstructPredefinedMissile(missileType);
      if (firstObject == RNnul) return;
      if (!T7or8or14)
      {
        secondObject = ConstructPredefinedMissile(missileType);
        if (secondObject == RNnul) T7or8or14 = true;
      };
    };
  };
  if (T7or8or14)
  {
    //w_26 = STRandomBool();
    initialPosition = sw((initialPosition + STRandomBool()) & 3);
  };
  launchX = timerMapX + d.DeltaX[pTimer->timerUByte8()]; //position
  launchY = timerMapY + d.DeltaY[pTimer->timerUByte8()]; //position
  //d.MissileDelay = 1;
  LaunchMissile(
                firstObject,
                launchX, //x
                launchY, //y
                initialPosition,         //pos
                pTimer->timerUByte8(),     //direction
                energyRemaining,    //range
                100,                //damage
                0,                  //decayRate
                true);
  if (!T7or8or14)
  {
    LaunchMissile(
                  secondObject,
                  launchX,
                  launchY,
                  (initialPosition + 1) & 3, //position
                  pTimer->timerUByte8(),       //direction
                  energyRemaining, //range
                  100,             //damage
                  0,               //decayRate
                  true);
  };
  //d.MissileDelay = 0;
}

//********************************************************
//
// Mostly copied from TriggerMissileLauncher()
//
//********************************************************
void ThrowMissile(i32 missileType,
                  const LOCATIONREL& launchLocation,
                  i32 direction,
                  const LOCATIONREL& objectLocation,
                  i32 range,
                  i32 damage,
                  i32 decayRate)
{
  RN  objectToThrow;
  DBTYPE objType;
  objectToThrow = RNeof;
  i32 oldLevel;
  if (!launchLocation.IsValid()) return;
  switch (missileType & 0xffff)
  {
  case RNFireball:
  case RNPoison:
  case RNLightning:
  case RNDispellMissile:
  case RNZoSpell:
  case RNPoisonBolt:
  case RNPoisonCloud:
  case RNMonsterDeath:
    objectToThrow.CreateSpell(missileType & 0x7f);
    break;
  case 0:
    {
      if (!objectLocation.IsValid()) return;
      for (objectToThrow =  FindFirstObject(objectLocation);
           objectToThrow != RNeof;
           objectToThrow =  GetDBRecordLink(objectToThrow) )
      {
        objType = objectToThrow.dbType();
        if (   (objType > dbMONSTER)
            && (objType <= dbMISC) )
        {
          if (objectLocation.p == objectToThrow.pos()) break;
        };
     
      };
      if (objectToThrow == RNeof) return;// original had RNnul) return;
      oldLevel = d.LoadedLevel;
      RemoveObjectFromRoom(objectToThrow,
                           objectLocation);
    };
    break;
  };
  if (objectToThrow == RNeof) return;
  //d.MissileDelay = 1;
  oldLevel = d.LoadedLevel;
  LoadLevel(launchLocation.l);
  LaunchMissile(
                objectToThrow,
                launchLocation.x, //x
                launchLocation.y, //y
                launchLocation.p, //pos
                direction,      //direction
                range,
                damage,
                decayRate,
                false);
  LoadLevel(oldLevel);
  //d.MissileDelay = 0;
}

static i32 NewState(i32 oldState,
                    i32 bitmask,
                    i32 responses,
                    i32 fieldStart)
{
  switch ((responses >> fieldStart) & 3)
  {
  case PORTRAIT_ResponseNone:
    return oldState;
  case PORTRAIT_ResponseClear:
    return oldState | bitmask;
    break;
  case PORTRAIT_ResponseSet:
    return oldState & ~bitmask;
    break;
  case PORTRAIT_ResponseToggle:
    return oldState ^= bitmask;
    break;
  default: return oldState; //impossible
  };
}

static void ProcessPortraitTimer6(
                     RN objPortrait, 
                     const TIMER *pTimer)
{
  i32 level;
  i32 responses;
  i32 timerFunction, timerX, timerY, timerPosition;
  i32 state;
  i32 inputMsgType;
  DB3 *pPortrait;
  pPortrait = GetRecordAddressDB3(objPortrait);
  level = d.LoadedLevel;
  timerFunction = pTimer->timerUByte9();
  timerPosition = pTimer->timerUByte8();
  timerY        = pTimer->timerUByte7();
  timerX        = pTimer->timerUByte6();
  state = pPortrait->State();
  inputMsgType = timerFunction;
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"Portrait State %d MSG %d\n",
                      state, 
                      inputMsgType);
  };
  switch (inputMsgType)
  {
  case 0: //Set
    responses = pPortrait->SetResponses();
    break;
  case 1: //Clear
    responses = pPortrait->ClearResponses();
    break;
  case 2: //Toggle
    responses = pPortrait->ToggleResponses();
    break;
  default:
    return; //Impossible
  };
  state = NewState (state,
                   PORTRAIT_InActive,
                   responses,
                   PORTRAIT_ActiveResponse);
  state = NewState (state,
                   PORTRAIT_HidePortrait,
                   responses,
                   PORTRAIT_PortraitResponse);
  state = NewState (state,
                   PORTRAIT_HideGraphic,
                   responses,
                   PORTRAIT_GraphicResponse);
  pPortrait->State((ui16)state);
}

void ProcessPortraitTimer7(RN targetObj, const TIMER *pTimer)
{
  ProcessPortraitTimer6(targetObj, pTimer);
}

//*********************************************************
//
//*********************************************************
//   TAG010b9a
bool ProcessTT_STONEROOM(TIMER *pTimer, ui32 index)
{//()
  dReg D4, D5;
  RN   objAtTarget;
  DB3 *pDB3A2;
  i32  targetPos;
  i32  targetY;
  i32  targetX;
  i32  objectDB;
  i32  actuatorType;
  DB2  *pDB2_4 = NULL;
  const char *sPressed, *sClosed;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (pTimer->Function() == TT_ParameterMessage)
  {
    pDSAparameters[0] = expool.Read((EDT_MessageParameters<<24) + index,pDSAparameters+1, 100);
    pTimer->Function(TT_STONEROOM);
  }
  else
  {
    pDSAparameters[0] = 0;
  };
  targetY = pTimer->timerUByte7();
  targetX = pTimer->timerUByte6();
  
  targetPos = pTimer->timerUByte8();
  for (objAtTarget = FindFirstObject(targetX, targetY);
       objAtTarget != RNeof;
       objAtTarget = GetDBRecordLink(objAtTarget))
  {
    if (timerTypeModifier[pTimer->timerUByte9()] == 3) return false;
    objectDB = objAtTarget.dbType();

    if (   (objectDB == dbTEXT) // Text of some sort?
        && (objAtTarget.pos() == pTimer->timerUByte8())   )
    {
      pDB2_4 = GetRecordAddressDB2(objAtTarget);
      if (timerTypeModifier[pTimer->timerUByte9()] == 2)
      { // Toggle bit 0 of word 2
        //D0W = pDB2_4->word2 & 1;
        //D3W = D0W==0 ? 1 : 0;
        //pDB2_4->word2 = (pDB2_4->word2 & 0xfffe) | D3W;
        pDB2_4->show(pDB2_4->show() ^ 1);
      }
      else
      { //Set bit 0 if function==0  else clear bit 0
        //D3W = pTimer->uByte9 == 0 ? 1 : 0;
        //pDB2_4->word2 = (pDB2_4->word2 & 0xfffe) | D3W;
        pDB2_4->show((timerTypeModifier[pTimer->timerUByte9()] & 1) == 0);
      };
    }
    else
    {
      if (objectDB == dbACTUATOR)
      {
        pDB3A2 = GetRecordAddressDB3(objAtTarget);
        actuatorType = pDB3A2->actuatorType();
        if (actuatorType == 47)
        {
          DSAVARS dsaVars;
          LOCATIONREL locr;
          locr.l = d.LoadedLevel;
          locr.x = targetX;
          locr.y = targetY;
          locr.p = objAtTarget.pos();
          if (ProcessDSATimer6(objAtTarget, pTimer, locr, false, &dsaVars)) break;
          continue;
        };
        if (   (actuatorType == 127)
            && (objAtTarget.pos() == pTimer->timerUByte8()))
        {
          ProcessPortraitTimer6(objAtTarget, pTimer);
          continue;
        };
        D4W = pDB3A2->value();
        if ( (actuatorType == 6)||(actuatorType == 48) ) 
        {    // Counter
          if (TimerTraceActive)
          fprintf(GETFILE(TraceFile), "Actuator6(counter) %03x %02x(%02x,%02x) ",
                     objAtTarget.idx(),
                     d.LoadedLevel, targetX, targetY);

          OperateCounterActuator(pTimer, pDB3A2);
        }
        else
        {
          if (actuatorType == 5)
          {
            i32 oldState, newState, targetState;
            bool oldMatch, newMatch;
            i32 bitmask, messageType;
            oldState = D4W & 0xf;
            bitmask = D5W = sw(1 << pTimer->timerUByte8());
            messageType = timerTypeModifier[pTimer->timerUByte9()];
            if (messageType == 2)
            {

//
//
//
              D4W ^= D5W;
            }
            else
            {
              if (messageType != 0)
              {
                D4W &= ~D5W;
              }
              else
              {
                D4W |= D5W;
              };
            };
            pDB3A2->value(D4W);
            D5W = (UI16)(D4W & 15);
            if (D5W == BITS4_7(D4W)) D5W=1; else D5W=0;
            //SET(D5W, D5W == BITS4_7(D4W));
            //D5W &= 1; // Set if actuator is 'pressed'
            sPressed = D5W?"Pressed":"Released";
            if (pDB3A2->normallyClosed()) D5W ^= 1;
            D5W &= 1;
            sClosed = D5W?"Closed":"Open";
            if (TimerTraceActive)
            fprintf(GETFILE(TraceFile), "Actuator5 %03x %02x(%02x,%02x) "
                               "new value = %03x, %s, %s\n",
                       objAtTarget.idx(),
                       d.LoadedLevel, targetX, targetY,
                       pDB3A2->value(), sPressed, sClosed);
            newState = pDB3A2->value() & 0xf;
            targetState = (pDB3A2->value() >> 4) & 0xf;
            oldMatch = oldState == targetState;
            newMatch = newState == targetState;
            if (ExtendedFeaturesVersion != '@')
            {
              switch (pDB3A2->andOrEdge())
              {
              case 0: //Always
                break; 
              case 1: //only if input changes
                if (oldState == newState) continue;
                break;
              case 2: //Only if output changes
                if (oldMatch == newMatch) continue;
                break;
              case 3: //Only if input change and not output change
                if (oldState == newState) continue;
                if (oldMatch != newMatch) continue;
                break;
              case 4: //Only if SET message
                if (messageType != 0) continue;
                break;
              case 5: //Only if CLEAR message
                if (messageType != 1) continue;
                break;
              case 6: //Only if bit is set by message
                if (oldState == newState) continue;
                if (oldState & bitmask) continue;
                break;
              case 7: //Only if bit is cleared by message
                if (oldState == newState) continue;
                if (newState & bitmask) continue;
                break;
              };
            };
            if (pDB3A2->action() == actuatorAction_CONSTPRESSURE)
            {
              QueueSwitchAction(pDB3A2,
                                D5W ^ 1,
                                targetX,
                                targetY,
                                objAtTarget.pos());
                                //targetPos);
            }
            else
            {
              if (D5W != 0)
              {
                QueueSwitchAction(pDB3A2,
                                  pDB3A2->action(),
                                  targetX,
                                  targetY,
                                  objAtTarget.pos());
                                  //targetPos);
              };
            };
          }
          else
          {

            if (   (   ((actuatorType>=7) && (actuatorType<=10))
                    || (actuatorType==14)
                    || (actuatorType==15) )
                && (objAtTarget.pos() == pTimer->timerUByte8()) )
            {
              if (TimerTraceActive)
              fprintf(GETFILE(TraceFile), "Actuator5 %03x (%02x,%02x) ",
                         objAtTarget.idx(),
                         targetX, targetY);
              TriggerMissileLauncher(pDB3A2, pTimer);
              if (pDB3A2->onceOnly()) pDB3A2->actuatorType(0);
            }
            else
            {
              if (actuatorType == 18)
              {
                d.CanRestartFromSavegame = 0;
                d.Word11694 = 1;
                return true;  // Tell folks this is the end of the game
              };  //if (actuatorType)
            };  // if (actuatorType)
          };  // if (actuatorType)
        };  // if (actuatorType)
      };  // if (actuator)
    };  // if (text)
    

  };  // for (each object in room)
  RotateActuatorList();
  return false;
}

//*********************************************************
//
//*********************************************************
//   TAG0110a6
void ProcessTT_TELEPORTER(TIMER *pTimer)
{//()
  CELLFLAG *pcf;
  i32 mapX, mapY;
  if (timerTypeModifier[pTimer->timerUByte9()] == 3) return;
  mapY = pTimer->timerUByte7();
  mapX = pTimer->timerUByte6();
  ActivateDSA(d.LoadedLevel,
              mapX,
              mapY,
              pTimer->timerUByte8(),   //pos
              timerTypeModifier[pTimer->timerUByte9()]);  //action

  pcf = &d.LevelCellFlags[mapX][mapY];
  if (timerTypeModifier[pTimer->timerUByte9()] == tmrAct_TOGGLE)
  {
    if (*pcf & 0x08) pTimer->timerUByte9(tmrAct_CLEAR);
    else pTimer->timerUByte9(tmrAct_SET);
  };
  if (timerTypeModifier[pTimer->timerUByte9()] == tmrAct_SET)
  {
    *pcf |= 8;
    WiggleEverything(mapX, mapY);//So they will fall through
                                 //new pit (or whatever)
  }
  else
  {
    *pcf &= 0xfff7;
  };
}

//*********************************************************
// This is done after activating a pit, for example.
// Everthing needs to be wiggled (moved 0 spaces) so that
// they will fall through the newly activated pit,
// teleporter, etc.
//*********************************************************
//   TAG010e98
void WiggleEverything(i32 mapX, i32 mapY)
{//()
  dReg D5;
  DB14  *pDB14A3;
  TIMER *ptA2;
  RN  objD4;
  i16 w_4;
  RN  obj_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//
//
  if (   (d.LoadedLevel == d.partyLevel)
      && (mapX == d.partyX)
      && (mapY == d.partyY) )
  {
    MoveObject(RN(RNnul), mapX, mapY, mapX, mapY, NULL, NULL); //move party
    DrawEightHands();
  };
  objD4 = FindFirstMonster(mapX, mapY);
  if (objD4 != RNeof)
  {
    MoveObject(objD4, mapX, mapY, mapX, mapY, NULL, NULL);
  };
  objD4 = FindFirstDrawableObject(mapX, mapY);
  obj_2 = objD4;
  w_4 = 0;
  while ( objD4 != RNeof)
  {
    if (objD4.dbType() > dbMONSTER)
    {
      w_4++; // Count objects > type 4
    };
    objD4 = GetDBRecordLink(objD4);
  };
  for (objD4 = obj_2;
       ( (objD4!=RNeof) && (w_4!=0) );
       objD4 = obj_2)
  {
    w_4--;
    obj_2 = GetDBRecordLink(objD4);
    D5W = sw(objD4.dbType());
    if (D5W > dbMONSTER)
    {
      MoveObject(objD4, mapX, mapY, mapX, mapY, NULL, NULL);
    };
    if (D5W == dbMISSILE)
    {
      pDB14A3 = GetRecordAddressDB14(objD4);
      ptA2 = gameTimers.pTimer(pDB14A3->timerIndex());
      SETWBITS0_4(ptA2->timerWord8(), d.NewX);
      SETWBITS5_9(ptA2->timerWord8(), d.NewY);
      SETWBITS10_11(ptA2->timerWord8(), d.NewDir);
      ptA2->timerObj6() = objD4;
      ptA2->timerObj6().pos(d.NewPos);
      //ptA2->timerTime = (ptA2->timerTime & 0xffffff) | (d.NewLevel << 24);
      ptA2->Level((ui8)d.NewLevel);
    }
    else
    {
      TIMER_SEARCH timerSearch;
      if (D5W != dbCLOUD) continue;
      D5W = 0;
      //ptA2 = gameTimers.pTimer(0);
      //for (D5W = 0;
      //     D5W < d.MaxTimer();
      //     D5W++, ptA2++)
      while (timerSearch.FindNextTimer())
      {  // Search entire array???? A bit ugly!
        ptA2 = timerSearch.TimerAddr();
        if (    (ptA2->Function() != TT_25)
             && (ptA2->Function() != TT_24) )continue;
        if (objD4 != ptA2->timerObj8()) continue;
        ptA2->timerUByte6((ui8)d.NewX);
        ptA2->timerUByte7((ui8)d.NewY);
        ptA2->timerObj8() = objD4;
        ptA2->timerObj8().pos(d.NewPos);
        //SETIBITS24_35(ptA2->timerTime, d.NewLevel);
        ptA2->Level((ui8)d.NewLevel);
//
//
      }; // for (D5W = each timer)
    };
    // continue
//
  }; // for (objD4)
}


//*********************************************************
//
//*********************************************************
//   TAG01111c
void ProcessTT_PITROOM(TIMER *pTimer)
{//()
  dReg D0;
  int x, y;
  CELLFLAG *pcfA2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (timerTypeModifier[pTimer->timerUByte9()] == 3) return;
  y = pTimer->timerUByte7();
  x = pTimer->timerUByte6();
  ActivateDSA(d.LoadedLevel,
              x,
              y,
              pTimer->timerUByte8(),   //pos
              timerTypeModifier[pTimer->timerUByte9()]);  //action

  pcfA2 = &d.LevelCellFlags[x][y];
  if (timerTypeModifier[pTimer->timerUByte9()] == 2)
  {
    if (*pcfA2 & 8)
    {
      D0W = 1;
    }
    else
    {
      D0W = 0;
    };
    pTimer->timerUByte9(D0B);
  };
  D0W = pTimer->timerUByte9();
  if (D0W == 0)
  {
    bool wasClosed = ((*pcfA2) & 0x8) == 0;
    *pcfA2 |= 0x08;
    if (wasClosed) WiggleEverything(x, y);//So they will fall through
                                 //new pit (or whatever)
  }
  else
  {
    *pcfA2 &= 0xfff7; // clear 0x08
  };
}

//*********************************************************
//
//*********************************************************
//   TAG011192
void ProcessTimer60and61(TIMER *pTimer)
{//()
  //Appears to be a pending monster generator.  A monster
  //was generated where a monster already existed.  Try
  //again later.  Or a monster teleported to such a place.
  dReg D6, D7;
  DB4 *pDB4A2;
  i16 w_2;
  RN object;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  w_2 = 0;
  D7W = pTimer->timerUByte6(); // X
  D6W = pTimer->timerUByte7(); // Y
  do
  {
//
//

    if (   (   (d.partyLevel != d.LoadedLevel)
              || (d.partyX     != D7W)
              || (d.partyY     != D6W) )
          && (FindFirstMonster(D7W, D6W) == RNeof) )
    { // Different position from party and no monsters here.
      if (pTimer->Function() == TT_61)
      {
        QueueSound(17, D7W, D6W, 1);
      };
      MoveObject(pTimer->timerObj8(), -1, 0, D7W, D6W, NULL, NULL);
      return;
    };
  // Same level, x, and y as party   -OR-  another monster is already there
    if (w_2 != 0) break;

    w_2 = 1;
    pDB4A2 = GetRecordAddressDB4(pTimer->timerObj8());
    if (pDB4A2->monsterType() != mon_LordChaos) break;

    if (STRandom0_3() != 0) break; // 25 percent chance of movement.

    switch (STRandom0_3()) // Which direction to move?
    {
    case 0:
        D7W--;
        break;

    case 1:
        D7W++;
        break;

    case 2:
        D6W--;
        break;

    case 3:
        D6W++;
        break;
    }; //End switch
  } while (OpenTeleporterPitOrDoor(D7W, D6W));





  //pTimer->timerTime += 5;
  pTimer->Time(pTimer->Time() + 5);
  gameTimers.SetTimer(pTimer);
}

//*********************************************************
//
//*********************************************************
//   TAG011292
void EnableCharacterAction(i32 chIdx)
{
  dReg D0, D5, D6;
  CHARDESC *pChar;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (chIdx > d.NumCharacter) die(0x65cc);
  pChar = &d.CH16482[chIdx];
  pChar->busyTimer = -1;
  pChar->charFlags &= ~CHARFLAG_attackDisable;
  if (pChar->attackType != -1) //Attack type in progress
  {
    //pChar->word64
    //     = sw(pChar->word64 - d.Byte20090[(UI8)(pChar->attackType)]);
    // See definition of word64 for explanation.
    pChar->word64 = 0;
  };
  if (pChar->HP() != 0)
  {
    if (   (pChar->attackType==atk_SHOOT)
        && (pChar->Possession(0)==RNnul))
    {
      D6W = 12;
      D0W = TAG0153c2(chIdx, 1, D6W);
      if (D0W != 0)
      {
tag01130c:
        AddCharacterPossession(chIdx, RemoveCharacterPossession(chIdx, D6W), 0);
      }
      else
      {
        for (D5W=0; D5W<3; D5W++)
        {
          D6W = sw(D5W+7);
          D0W = TAG0153c2(chIdx, 1, D6W);
          if (D0W != 0) goto tag01130c;
//
        };
      };
    };
    pChar->charFlags |= CHARFLAG_weaponAttack;
    DrawCharacterState(chIdx);
  };
  pChar->attackType = -1;
}

//*********************************************************
//
//*********************************************************
void TAG011366(i16 chIdx)
{ // Called when timer function 12 event occurs.
  CHARDESC *pcA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pcA3 = &d.CH16482[chIdx];
  pcA3->timerIndex = -1;
  if (pcA3->HP() == 0) return;
  if (chIdx+1 == d.SelectedCharacterOrdinal)
  {
    STHideCursor(HC46);
    CharacterPortraitToStatusBox(chIdx);
    STShowCursor(HC46);
  }
  else
  {
    pcA3->charFlags |= CHARFLAG_cursor;
    DrawCharacterState(chIdx);
  };
}

//*********************************************************
//
//*********************************************************
//   TAG0113c4
void ProcessTT_ViAltar(TIMER *pTimer)
{ //()
  //I got here when I put bones in altar of VI.
  dReg D5;
  i32 bonesX, bonesY;
  RN objD4;
  RN objBones;
  DB10 *DB10A2;
//  i16 w_6;
  i32 ViState; //w_4;
  OBJ_NAME_INDEX objNI_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  bonesX  = pTimer->timerUByte6();  // x-coordinate of bones
  bonesY  = pTimer->timerUByte7();  // y-coordinate of bones
  D5W     = pTimer->packedPos();  // position of bones
  ViState = pTimer->packedState();
  objBones = pTimer->timerObj8();
  switch (ViState)
  {
  case 2:
      CreateCloud(RN(RNffe4), 0, bonesX, bonesY, bonesX, bonesY, D5W, NULL);
      //pTimer->timerTime += 5;
      pTimer->Time(pTimer->Time() + 5);
      ViState--;
      pTimer->packedState(ViState);
      gameTimers.SetTimer(pTimer);
      break;

  case 1:
      for (objD4 = FindFirstObject(bonesX, bonesY);
           objD4 != RNeof;
           objD4 = GetDBRecordLink(objD4) )
      {
        int chIndex;
        ui32 bonesRecord[1];
        if (objD4.pos() != D5UW) continue;
        if (objD4.dbType() != dbMISC) continue;
        objNI_2 = objD4.NameIndex();
        if (objNI_2 != objNI_Bones_a) continue;
        if (objD4 != objBones) continue;
        if (expool.GetChampionBonesRecord(objD4, bonesRecord) == 1)
        {
          // If this character is in the party we proceed as before
          for (chIndex=0; chIndex<d.NumCharacter; chIndex++)
          {
            if (d.CH16482[chIndex].fingerPrint == (bonesRecord[0] & 0xffff)) break;
          };
          if (chIndex < d.NumCharacter)
          {
            if (d.CH16482[chIndex].HP() != 0) break; // ignore if character not dead!
            DB10A2 = GetRecordAddressDB10(objD4);
            RemoveObjectFromRoom(objD4, bonesX, bonesY, NULL);
            DeleteDBEntry(DB10A2);
            //pTimer->timerTime++;
            pTimer->Time(pTimer->Time() + 1);
            ViState--;
            pTimer->packedState(ViState);
            pTimer->packedPos(chIndex);
            gameTimers.SetTimer(pTimer);
            break;
          }
          else
          {
            //Perhaps character has been removed from party.
            CHARDESC character;
            if (!character.GetFromWings((ui16)(bonesRecord[0]&0xffff), false)) break;
            if (character.HP() > 0) break; // character not dead!
            character.HP(character.MaxHP()/2);
            character.SaveToWings();
            DB10A2 = GetRecordAddressDB10(objD4);
            RemoveObjectFromRoom(objD4, bonesX, bonesY, NULL);
            DeleteDBEntry(DB10A2);
            break;
          }
        }
        else
        {
          DB10A2 = GetRecordAddressDB10(objD4);
          chIndex = DB10A2->value();
          if (chIndex < d.NumCharacter)
          {
            if (d.CH16482[chIndex].HP() != 0) break; // ignore if character not dead!
            RemoveObjectFromRoom(objD4, bonesX, bonesY, NULL);
            DeleteDBEntry(DB10A2);
            //pTimer->timerTime++;
            pTimer->Time(pTimer->Time() + 1);
            ViState--;
            pTimer->packedState(ViState);
            pTimer->packedPos(chIndex);
            gameTimers.SetTimer(pTimer);
            break;
          }

        };
      };
      break;
  case 0:

      BringCharacterToLife(pTimer->packedPos());
      break;
  }; //case
}

//*********************************************************
//
//*********************************************************
//   TAG0114d0
void SetWatchdogTimer()
{
  TIMER LOCALa;
  //i32 LOCAL_10;
  //i8  LOCAL_6[6];
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  LOCALa.Function(TT_53);
  //LOCALa.timerTime = d.WatchdogTime = d.Time + 300; // about 50 seconds
  d.WatchdogTime = d.Time + 300; // about 50 seconds
  LOCALa.Time(d.WatchdogTime);
  LOCALa.Level(0);
  gameTimers.SetTimer(&LOCALa); //TAG00ffbe
}

//*********************************************************
//
//*********************************************************
//   TAG0114fa
void ProcessLightLevelTimer(TIMER *pTimer)
{
  TIMER timer;
  i32 oldNumberOfSteps;
  i32 newNumberOfSteps;
  i32 changeInBrightness;
  bool negativeChange;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  oldNumberOfSteps = pTimer->timerWord6();
  if (oldNumberOfSteps == 0) return;
  negativeChange = oldNumberOfSteps < 0;
  if (negativeChange)
  {
    oldNumberOfSteps = -oldNumberOfSteps;
  };
  newNumberOfSteps = oldNumberOfSteps - 1;
  ASSERT(oldNumberOfSteps < 16,"steps");
  changeInBrightness = d.Word1074[oldNumberOfSteps] 
                           - d.Word1074[newNumberOfSteps];
  if (negativeChange)
  {
    changeInBrightness = sw(-changeInBrightness);
    newNumberOfSteps = sw(-newNumberOfSteps);
  };
  d.Brightness = sw(d.Brightness + changeInBrightness);
  if (newNumberOfSteps != 0)
  {
    timer.Function(TT_LIGHT);
    timer.timerWord6() = (i16)newNumberOfSteps;
    //timer.timerTime = (d.Time+4) | (d.partyLevel << 24);;
    timer.Time(d.Time+4);
    timer.Level((ui8)d.partyLevel);
    timer.timerUByte5(0);
    gameTimers.SetTimer(&timer);
  };
}

//void GameTimers::DeleteTimer(TIMER_SEARCH& timerSearch)
//{
//  DeleteTimer(timerSearch.TimerHandle());
//}


void GameTimers::ConvertToSequencedTimers()
{
  // Note that the timers are still in big-endian order!
  // This will not bother us as we simpy copy them as-is and
  // append a couple of zero bytes which will bet swapped
  // but still remain zero!
  i32 i;
  //i32 oldSize = sizeof(TIMER) - sizeof(m_timers[0].m_timerSequence); 
  i32 oldSize = 10; 
  i32 newSize = 12;
  ui8 *pOldTimer;
  ui8 *pNewTimer;
  for (i=m_maxTimers-1; i>=0; i--)
  {
    pOldTimer =   (ui8 *)m_timers + i*oldSize;
    pNewTimer =   (ui8 *)m_timers + i*newSize;
    memmove(pNewTimer, pOldTimer, oldSize);
    ((TIMER *)pNewTimer)->m_timerSequence = 0;
  };
}

void GameTimers::ConvertToExtendedTimers()
{
  // Note that the timers are still in big-endian order!
  // This will not bother us as we simpy copy them as-is and
  // append a couple of zero bytes which will bet swapped
  // but still remain zero!
  i32 i;
  ui32 time;
  TIMER *pOldTimer, *pNewTimer;
  for (i=m_maxTimers-1; i>=0; i--)
  {
    pNewTimer =   m_timers + i;
    pOldTimer =   (TIMER *)(((char *)m_timers) + 12*i);
    time = pOldTimer->m_time;
    memmove(pNewTimer, pOldTimer, sizeof(TIMER));
    pNewTimer->Time(time & 0xffffff00);
    pNewTimer->Level((ui8)(time & 0xff));
    pNewTimer->unused13[0] = 0;
    pNewTimer->unused13[1] = 0;
    pNewTimer->unused13[2] = 0;
  };
}



bool TIMER_SEARCH::FindNextTimer()
{
  if (m_index < 0)
  {
    i32 gnt;
    gnt = gameTimers.NumTimer();
    m_numEntry = (i16)gnt;
    m_index = m_numEntry;
  };
  m_index--;
  return m_index >= 0;
}


void TIMER_SEARCH::DeleteTimer()
{
  gameTimers.DeleteTimer(TimerHandle());
}