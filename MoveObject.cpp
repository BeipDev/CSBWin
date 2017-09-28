#include "stdafx.h"

#include <stdio.h>

#include "UI.h"
//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"


  

struct CHARACTER_SEARCH_PACKET
{
  RN   objectToLookFor;
  bool found;
  i32  characterIndex;
  i32  locationOnCharacter; //-1 means cursor
  RN   RNchest;  // if found as direct content of a chest
  RN   RNprevObject; // if not first in a list;
};

bool SearchList(CHARACTER_SEARCH_PACKET *pPkt, RN first)
{//return true if in list directly (rather than in an object in the list).
  RN prevObj, thisObj;
  DBTYPE dbtype;
  bool direct;
  prevObj = RNeof;
  for (thisObj = first;
       thisObj != RNeof;
       prevObj = thisObj, thisObj = GetDBRecordLink(thisObj))
  {
    if (thisObj == pPkt->objectToLookFor)
    {
      pPkt->RNprevObject = prevObj;
      pPkt->found = true;
      return true;
    };
    dbtype = thisObj.dbType();
    switch (dbtype)
    {
    case dbCHEST:
      {
        pPkt->RNchest = RNeof;
        DB9 *pChest;
        pChest = GetRecordAddressDB9(thisObj);
        if (pChest->contents() == RNeof) break;
        direct = SearchList(pPkt, pChest->contents());
        if (pPkt->found)
        {
          if (direct) pPkt->RNchest = thisObj;
          return false;
        };
      };
    case dbMONSTER:
      return false;
    default:
      break;
    }; //switch
  }; //for
  return false;
}

void SearchCharactersForObject(CHARACTER_SEARCH_PACKET *pPkt)
{
  pPkt->found = false;
  CHARDESC *pChar;
  RN possession;
  for (pPkt->characterIndex = 0;
       pPkt->characterIndex < d.NumCharacter; 
       pPkt->characterIndex++)
  {
    pChar = &d.CH16482[pPkt->characterIndex];
    for (pPkt->locationOnCharacter = 0;
         pPkt->locationOnCharacter < 30;
         pPkt->locationOnCharacter++)
    {
      possession = pChar->Possession(pPkt->locationOnCharacter);
      if (possession == RNnul) continue;
      SearchList(pPkt, pChar->Possession(pPkt->locationOnCharacter));
      if (pPkt->found) return;
    };
  };
  if (d.objectInHand !=RNnul)
  {
    SearchList(pPkt, d.objectInHand);
    if (pPkt->found) return;
  };
}

enum ERRORCODES
{
  SER_OK                            = 0,
  SER_UnknownLocType                = 1,
  SER_NoObjInCursor                 = 2,
  SER_IllegalCharacterIndex         = 3,
  SER_IllegalCarryLocation          = 4,
  SER_NoObjectAtCarryLocation       = 5,
  SER_InvalidCellLocation           = 6,
  SER_NoSuchObjectInCell            = 7,
  SER_IllegalMonsterID              = 8,
  SER_MonsterIsNotMonster           = 9,
  SER_NoSuchObjectOnMonster         = 10,
  SER_IllegalChestID                = 11,
  SER_ChestIsNotChest               = 12,
  SER_NoSuchObjectInChest           = 13,
  SER_ChestBelongsToCharacter       = 14,
  SER_NoCloudsPlease                = 15,
  SER_BadCloudTimer                 = 16,
  DER_OK                            = 0,
  DER_UnknownLocType                = -1,
  DER_IllegalCharacterIndex         = -2,
  DER_IllegalPossessionIndex        = -3,
  DER_PossessionSlotNotEmpty        = -4,
  DER_IllegalObjectForCarryLocation = -5,
  DER_CursorNotEmpty                = -6,
  DER_InvalidCellLocation           = -7,
  DER_IllegalDepth                  = -8,
  DER_IllegalChestID                = -9,
  DER_ChestIsNotAChest              = -10,
  DER_ChestBelongsToCharacter       = -11,
  DER_ChestIsFull                   = -12,
  DER_IllegalMonsterID              = -13,
  DER_MonsterIsNotAMonster          = -14,
  DER_NoCloudsPlease                = -15,
};


RN FindCursorSource(bool doit)
{
  RN result;
  if (d.objectInHand == RNnul) return RNeof;
  result = d.objectInHand;
  if (doit) 
  {
    CURSORFILTER_PACKET cfp;
    cfp.type = CURSORFILTER_DSA_MoveFrom;
    cfp.object = d.objectInHand.ConvertToInteger();
    CursorFilter(&cfp);
    RemoveObjectFromHand();
  };
  return result;
}

i32 FindCharacterSource(i32 chIdx, i32 carryLoc, RN *object, bool doit)
{
  DBTYPE dbType;
  if (chIdx >= d.NumCharacter) return SER_IllegalCharacterIndex;
  if (carryLoc > 29) return SER_IllegalCarryLocation;
  *object = d.CH16482[chIdx].Possession(carryLoc);
  if (*object == RNnul) return SER_NoObjectAtCarryLocation;
  dbType = object->dbType();
  switch (dbType)
  {
    case dbCLOUD: return SER_NoCloudsPlease;
  };
  if (doit)
  {
    RemoveCharacterPossession(chIdx, carryLoc);
  };
  return SER_OK;
}


i32 FindCellSource(i32 srcObjMsk, 
                   i32 srcPosMsk, 
                   i32 srcLoc, 
                   i32 srcDepth, 
                   RN  *object, 
                   i32 dstType,
                   i32 dstLoc,
                   HTIMER *hTimer,
                   i32 doit)
{
  LOCATIONREL locr, dstLocr;
  RN thisObj;
  DBTYPE dbType;
  TIMER *pTimer;
  locr.Integer(srcLoc);
  if (!locr.IsValid()) return SER_InvalidCellLocation;


  if (srcPosMsk == -1) srcPosMsk = 1<<locr.p;
  *object = RNeof;
  for (thisObj = FindFirstObject(locr.l,locr.x,locr.y);
       thisObj != RNeof;
       thisObj = GetDBRecordLink(thisObj))
  {
    if (((1<<thisObj.pos()) & srcPosMsk) == 0) continue;
    if (((1<<thisObj.dbType()) & srcObjMsk) == 0) continue;
    if (srcDepth == 0) 
    {
      *object = thisObj;
      break;
    }
    else
    {
      srcDepth--;
    };
  };
  if (*object == RNeof) return SER_NoSuchObjectInCell;
  if (doit)
  {
    i32 oldLevel;
    DBTYPE ot;
    ot = object->dbType();
    //GetRecordAddressDB4(*object);
    if (    (ot != dbMONSTER)
         || (dstType != 1) 
         || (dstLocr.Integer(dstLoc)->l != d.partyLevel)
         || (locr.l != d.partyLevel)
       )
    {
      oldLevel = d.LoadedLevel;
      LoadLevel(locr.l);
      MoveObject(thisObj, locr.x, locr.y, -1, 0, NULL, NULL);
      //db.GetCommonAddress(thisObj)->link(RNnul);
      LoadLevel(oldLevel);
    };
  }
  else
  {
    dbType = thisObj.dbType();
    switch (dbType)
    {
    case dbCLOUD:
      //pTimer = d.pTimer(0);
      //for (*timerIndex=0;
      //     *timerIndex < d.MaxTimer();
      //     (*timerIndex)++, pTimer++)
      {
        TIMER_SEARCH timerSearch;
        while (timerSearch.FindNextTimer())
        {  // Search entire array???? A bit ugly!
          pTimer = timerSearch.TimerAddr();
          if (    (pTimer->Function() != TT_25)
               && (pTimer->Function() != TT_24) )continue;
          if (thisObj != pTimer->timerObj8()) continue;
          *hTimer = timerSearch.TimerHandle();
          return SER_OK;
        };
        //*timerIndex = timerSearch.TimerIndex();
        //if (*timerIndex == gameTimers.MaxTimer()) return SER_BadCloudTimer;
        return SER_BadCloudTimer;
      };
      break;
    };
  };
  return SER_OK;
}


i32 FindMonsterSource(i32 srcObjMsk, 
                      i32 srcLoc, 
                      i32 srcDepth, 
                      RN  *object, 
                      i32 doit)
{
  RN thisObj, prevObj;
  RN RNmonster;
  DB4 *pMon;
  DBTYPE dbType;
  DBCOMMON *pThisObj, *pPrevObj;
  if (!RNmonster.checkIndirectIndex(srcLoc)) return SER_IllegalMonsterID;
  RNmonster.ConstructFromInteger(srcLoc);
  if (RNmonster.dbNum() !=dbMONSTER) return SER_MonsterIsNotMonster;
  pMon = GetRecordAddressDB4(RNmonster);
  *object = RNeof;
  prevObj = RNeof;
  for (thisObj = pMon->possession();
       thisObj != RNeof;
       prevObj = thisObj, thisObj = GetDBRecordLink(thisObj))
  {
    if (((1<<thisObj.dbType()) & srcObjMsk) == 0) continue;
    if (srcDepth == 0) 
    {
      *object = thisObj;
      break;
    }
    else
    {
      srcDepth--;
    };
  };
  if (*object == RNeof) return SER_NoSuchObjectOnMonster;
  dbType = object->dbType();
  switch (dbType)
  {
    case dbCLOUD: return SER_NoCloudsPlease;
  };
  if (doit)
  {
    pThisObj = GetCommonAddress(thisObj);
    if (prevObj == RNeof)
    {
      pMon->possession(pThisObj->link());
    }
    else
    {
      pPrevObj = GetCommonAddress(prevObj);
      pPrevObj->link(pThisObj->link());
    };
    pThisObj->link(RNeof);
  };
  return SER_OK;
}

i32 FindChestSource(i32 srcObjMsk, 
                    i32 srcLoc,
                    i32 srcDepth, 
                    RN  *object, 
                    i32 doit)
{
  RN thisObj, prevObj;
  RN RNchest;
  DBTYPE dbType;
  DB9 *pChest;
  CHARACTER_SEARCH_PACKET charSrchPkt;
  DBCOMMON *pThisObj, *pPrevObj;
  if (!RNchest.checkIndirectIndex(srcLoc)) return SER_IllegalChestID;
  RNchest.ConstructFromInteger(srcLoc);
  if (RNchest.dbNum() !=dbCHEST) return SER_ChestIsNotChest;
  charSrchPkt.objectToLookFor = RNchest;
  SearchCharactersForObject(&charSrchPkt);
  if (charSrchPkt.found) return SER_ChestBelongsToCharacter;
  pChest = GetRecordAddressDB9(RNchest);
  *object = RNeof;
  prevObj = RNeof;
  for (thisObj = pChest->contents();
       thisObj != RNeof;
       prevObj = thisObj, thisObj = GetDBRecordLink(thisObj))
  {
    if (((1<<thisObj.dbType()) & srcObjMsk) == 0) continue;
    if (srcDepth == 0) 
    {
      *object = thisObj;
      break;
    }
    else
    {
      srcDepth--;
    };
  };
  if (*object == RNeof) return SER_NoSuchObjectInChest;
  dbType = object->dbType();
  switch (dbType)
  {
    case dbCLOUD: return SER_NoCloudsPlease;
  };
  if (doit)
  {
    pThisObj = GetCommonAddress(thisObj);
    if (prevObj == RNeof)
    {
      pChest->contents(pThisObj->link());
    }
    else
    {
      pPrevObj = GetCommonAddress(prevObj);
      pPrevObj->link(pThisObj->link());
    };
    pThisObj->link(RNeof);
  };
  return SER_OK;
}


i32 CheckSource(i32 srcType, 
                i32 srcObjMsk, 
                i32 srcPosMsk, 
                i32 srcLoc, 
                i32 srcDepth,
                RN  *object,
                i32 dstType,
                i32 dstLoc,
                HTIMER *hTimer,
                bool doit)
{
  switch (srcType)
  {
  case 2:  //Object in cursor;
    {
      *object = FindCursorSource(doit);
      if (*object == RNeof) return SER_NoObjInCursor;
      return SER_OK;
    };
  case 4:  //Character Possession
    {
      return FindCharacterSource(srcPosMsk, srcLoc, object, doit);
    };
  case 1:  // Object is in a cell of the dungeon.
    {
      return FindCellSource(
              srcObjMsk, 
              srcPosMsk, 
              srcLoc, 
              srcDepth, 
              object, 
              dstType,
              dstLoc,
              hTimer, 
              doit);
    };
  case 3: //MonsterPossession
    {
      return FindMonsterSource(srcObjMsk, srcLoc, srcDepth, object, doit);
    };
  case 5: //Object in chest
    {
      return FindChestSource(srcObjMsk, srcLoc, srcDepth, object, doit);
    };
  };
  return SER_UnknownLocType;
}


i32 FindCharacterDestination(i32 chIdx, i32 carryLoc, RN object, bool doit)
{
  //i32 objIdx;
  i32 locMask;
  DBTYPE dbType;
  dbType = object.dbType();
  switch (dbType)
  {
    case dbCLOUD: return DER_NoCloudsPlease;
  };
  if (chIdx >= d.NumCharacter) return DER_IllegalCharacterIndex;
  if (carryLoc > 29) return DER_IllegalPossessionIndex;
  if (d.CH16482[chIdx].Possession(carryLoc) != RNnul) return DER_PossessionSlotNotEmpty;
  //objIdx = GetObjectDescIndex(object);
  locMask = d.ObjDesc[object.DescIndex()].word4;// Possible carry locations
  if ((locMask & d.CarryLocation[carryLoc]) == 0) return DER_IllegalObjectForCarryLocation;
  if (!doit) return DER_OK;
  AddCharacterPossession(chIdx, object, carryLoc);
  return DER_OK;
}

i32 CheckObjectToCursor(RN object, bool doit)
{
  DBTYPE dbType;
  if (d.objectInHand != RNnul) return DER_CursorNotEmpty;
  dbType = object.dbType();
  switch (dbType)
  {
    case dbCLOUD: return DER_NoCloudsPlease;
  };
  if (RememberToPutObjectInHand != -1) return DER_CursorNotEmpty;
  if (doit)
  {
    CURSORFILTER_PACKET cfp;
    cfp.type = CURSORFILTER_DSA_MoveTo;
    cfp.object = object.ConvertToInteger();
    if (object != RNnul) CursorFilter(&cfp);
    ObjectToCursor(object,1);
  };
  return 0;
}

i32 FindCellDestination(i32 /* dstObjMsk */,
                        i32 dstPosMsk, 
                        i32 dstLoc, 
                        i32 dstDepth, 
                        RN  object, 
                        i32 srcType,
                        i32 srcLoc,  // -1 or location to move object from
                        HTIMER hTimer,
                        bool doit)
{
  LOCATIONREL locr, srcLocr;
  i32 m, n = 0, p = 0;
  locr.Integer(dstLoc);
  if (!locr.IsValid()) return DER_InvalidCellLocation;
  if (dstDepth != 0) return DER_IllegalDepth;
  p = locr.p;
  m = dstPosMsk & 15;
  if (m != 0)
  {
    for (p=0; p<4; p++)
    {
      if (m&1) n++;
      m >>= 1;
    };
    n = STRandom(n);
    m = dstPosMsk & 15;
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
  if (doit)
  {
    i32 oldLevel;
    TIMER *pTimer;
    oldLevel = d.LoadedLevel;
    LoadLevel(locr.l);
    switch (object.dbType())
    {
    case dbMONSTER:
      {
        if (     (locr.l != d.partyLevel)
              || (srcType != 1)
              || (srcLocr.Integer(srcLoc)->l != locr.l)
           )
        {
          MONSTERDESC *pmtDesc;
          i32 monsterSize;
          DB4 *pDB4;
          pDB4 = GetRecordAddressDB4(object);
          pmtDesc = &d.MonsterDescriptor[pDB4->monsterType()];
          monsterSize = pmtDesc->horizontalSize();
          pDB4->groupIndex(0xd8);
          if (monsterSize == 2) pDB4->groupIndex(0xff);
        };
      };
      break;
    case dbCLOUD:
      pTimer = gameTimers.pTimer(hTimer);
      pTimer->timerUByte6((ui8)locr.x);
      pTimer->timerUByte7((ui8)locr.y);
      pTimer->timerObj8().pos(d.NewPos);
      //SETIBITS24_35(pTimer->timerTime, locr.l);
      pTimer->Level((ui8)locr.l);
      break;
    };
    if (    (object.dbType() != dbMONSTER)
         || (locr.l != d.partyLevel)
         || (srcType != 1)
         || (srcLocr.Integer(srcLoc)->l != locr.l)
       )
    {
      MoveObject(object, -1, 0, locr.x, locr.y, NULL, NULL);
      object.pos(p);
    }
    else
    {
      i32 removed;
      i32 tLevel;;
      TIMERTYPE tt;
      TIMER *pTimerA;
      removed = MoveObject(object, srcLocr.x, srcLocr.y, locr.x, locr.y, NULL, NULL);   
      ASSERT(removed != 0xccc,"moveObject");                            
      if (removed == 0)                                              
      {   
        TIMER_SEARCH timerSearch;
        //for ( pTimerA = gameTimers.Timers(), tIndex = 0;  xxTIMERxx Change to FindNextTimer
        //      tIndex < d.MaxTimer(); 
        //      pTimerA++, tIndex++)
        while (timerSearch.FindNextTimer())
        {
          pTimerA = timerSearch.TimerAddr();
          //tLevel = (pTimerA->timerTime >> 24) & 0xff;
          tLevel = pTimerA->Level();
          if (tLevel != d.LoadedLevel) continue;
          tt = pTimerA->Function();
          if (tt < TT_FIRST_MONSTER_AI) continue;
          if (tt > TT_LAST_MONSTER_AI) continue;
          if (pTimerA->timerUByte6() != srcLocr.x) continue;
          if (pTimerA->timerUByte7() != srcLocr.y) continue;
          pTimerA->timerUByte6(ub(locr.x));                            
          pTimerA->timerUByte7(ub(locr.y));                            
          //pI16A2->ubPrevX = ub(mapX);                                   
          //pI16A2->ubPrevY = ub(mapY);                                   
          //pI16A2->uByte4 = (ui8)d.Time;                                 
        };
      };                                                            
    };

    switch (object.dbType())
    {
    case dbMISSILE:
      {
        TIMER *pTimerA;
        int newX, newY;
        DB14 *pdbMissile;
        pdbMissile = GetRecordAddressDB14(object);

        newX = locr.x & 31; //Set to newX by MoveObject
        pTimerA = gameTimers.pTimer(pdbMissile->timerIndex());
        pTimerA->timerWord8() &= 0xffe0;
        pTimerA->timerWord8() |= newX;   // New mapX to timer entry
        newY = locr.y & 31; //Set to newY by MoveObject
        pTimerA->timerWord8() &= 0xfc1f;
        pTimerA->timerWord8() |= newY << 5; // New mapY to timer entry
        //newDir = d.NewDir & 3; //Set to new Direction by MoveObject
        //pTimerA->timerWord8() &= 0xf3ff;
        //pTimerA->timerWord8() |= newDir<<10; //Set new direction in timer entry
        //pTimerA->timerTime &= 0xffffff;
        //pTimerA->timerTime |= (locr.l<<24);
        pTimerA->Level((ui8)locr.l);
      };
    };

    LoadLevel(oldLevel);
  };
  return DER_OK;
}

i32 FindChestDestination(i32 dstLoc, 
                         i32 dstDepth, 
                         RN  object, 
                         bool doit)
{
  RN RNchest, RNcontents;
  DBTYPE dbType;
  DBCOMMON *pObject;
  i32 count;
  DB9 *pChest;
  CHARACTER_SEARCH_PACKET charSrchPacket;
  dbType = object.dbType();
  switch (dbType)
  {
    case dbCLOUD: return DER_NoCloudsPlease;
  };
  if (!RNchest.checkIndirectIndex(dstLoc)) return DER_IllegalChestID;
  RNchest.ConstructFromInteger(dstLoc);
  if (RNchest.dbType() != dbCHEST) return DER_ChestIsNotAChest;
  if (dstDepth != 0) return DER_IllegalDepth;
  charSrchPacket.objectToLookFor = RNchest;
  SearchCharactersForObject(&charSrchPacket);
  if (charSrchPacket.found) return DER_ChestBelongsToCharacter;
  count=0;
  pChest = GetRecordAddressDB9(RNchest);
  for (RNcontents = pChest->contents();
       RNcontents != RNeof;
       RNcontents = GetDBRecordLink(RNcontents))
  {
    count++;
  };
  if (count > 7) return DER_ChestIsFull;
  if (doit)
  {
    pObject = GetCommonAddress(object);
    pObject->link(pChest->contents());
    pChest->contents(object);
  };
  return DER_OK;
}

i32 FindMonsterDestination(i32 dstLoc, 
                           i32 dstDepth, 
                           RN  object, 
                           bool doit)
{
  RN RNmonster, RNpossessions;
  DBTYPE dbType;
  DBCOMMON *pObject;
  DB4 *pMonster;
  if (!RNmonster.checkIndirectIndex(dstLoc)) return DER_IllegalMonsterID;
  dbType = object.dbType();
  switch (dbType)
  {
    case dbCLOUD: return DER_NoCloudsPlease;
  };
  RNmonster.ConstructFromInteger(dstLoc);
  if (RNmonster.dbType() != dbMONSTER) return DER_MonsterIsNotAMonster;
  if (dstDepth != 0) return DER_IllegalDepth;
  pMonster = GetRecordAddressDB4(RNmonster);
  if (doit)
  {
    pObject = GetCommonAddress(object);
    pObject->link(pMonster->possession());
    pMonster->possession(object);
  };
  return DER_OK;
}


i32 CheckDestination(i32 dstType, 
                     i32 dstObjMsk, 
                     i32 dstPosMsk, 
                     i32 dstLoc, 
                     i32 dstDepth,
                     RN  object,
                     i32 srcType,
                     i32 srcLoc, // -1 or source to move object from.
                     HTIMER hTimer,
                     bool doit)
{
  switch (dstType)
  {
  case 4: //Character possession
    {
      return FindCharacterDestination(dstPosMsk, dstLoc, object, doit);
    };
  case 2: // Cursor
    {
      return CheckObjectToCursor(object, doit);
    };
  case 1: // Cell Location
    {
      return FindCellDestination(
                      dstObjMsk, 
                      dstPosMsk, 
                      dstLoc, 
                      dstDepth, 
                      object, 
                      srcType,
                      srcLoc, 
                      hTimer, 
                      doit);
    };
  case 5: //Chest contents
    {
      return FindChestDestination(dstLoc, dstDepth, object, doit);
    };
  case 3: //Monster possession
    {
      return FindMonsterDestination(dstLoc, dstDepth, object, doit);
    };
  };
  return DER_UnknownLocType;
}


i32 MoveObject (i32 srcType, i32 srcObjMsk, i32 srcPosMsk, i32 srcLoc, i32 srcDepth,
                i32 dstType, i32 dstObjMsk, i32 dstPosMsk, i32 dstLoc, i32 dstDepth)
{
  // return 0 if all is well.
  // positive numbers = source is bad.
  // negative numbers = destination is bad.
  //
  // First we check that the entire MOVE can be completed without error
  //
  i32 result;
  HTIMER hTimer;
  RN  object;
  result = CheckSource(srcType,
                       srcObjMsk, 
                       srcPosMsk, 
                       srcLoc, 
                       srcDepth,
                       &object,
                       dstType,
                       dstLoc,
                       &hTimer,
                       false);  // Just check....don't remove it.
  if (result != 0) return result;
  result = CheckDestination(dstType, 
                            dstObjMsk, 
                            dstPosMsk, 
                            dstLoc, 
                            dstDepth,
                            object,
                            srcType,
                            srcLoc,
                            hTimer,
                            false); //Just check ... don't put it there.
  if (result != 0) return result;
  //
  // Now we know that nothing can go wrong.
  //
  result = CheckSource(
         srcType, 
         srcObjMsk, 
         srcPosMsk, 
         srcLoc, 
         srcDepth,
         &object,
         dstType,
         dstLoc,
         &hTimer,
         true);   // Really fetch the object.  Remove it from current location.
                  // Except that monsters being moved from and to the party's
                  // level are not actually removed.
                  // We leave them there for CheckDestination to move.  In this way
                  // the monster retains all of its state.
  if (result != 0) SystemError(0x63e7);
  result = CheckDestination(
         dstType, 
         dstObjMsk, 
         dstPosMsk, 
         dstLoc, 
         dstDepth,
         object,
         srcType,
         srcLoc,
         hTimer,
         true); //Really put the object in its new location.       
  if (result != 0) SystemError(0x63e8);
  return 0;
}


void CursorFilter(CURSORFILTER_PACKET *cfp)
{

/////////////////////////////////////////////////////
//Now let the Cursor Filter DSA do its thing
/////////////////////////////////////////////////////
  {
    ui32 key, *pRecord;
    i32 len;
    key = (EDT_SpecialLocations<<24)|ESL_CURSORFILTER;
    len = expool.Locate(key,&pRecord);
    if (len > 0)
    //*****************************************
    //Let the DSA process the parameters
    {
      RN obj;
      TIMER timer;
      i32 objectDB, actuatorType;
      DB3 * pActuator;
      LOCATIONREL locr;
      locr.Integer(*pRecord);
      for (obj = FindFirstObject(
                      locr.l,
                      locr.x,
                      locr.y);
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
            int currentLevel;
            DSAVARS dsaVars;
            currentLevel = d.LoadedLevel;
            LoadLevel(locr.l);
            timer.timerUByte9(0);//timerFunction
            timer.timerUByte8(0);//timerPosition
            timer.timerUByte7((ui8)locr.y);
            timer.timerUByte6((ui8)locr.x);
            //timer.timerTime   = locr.l << 24;
            timer.Time(0);
            timer.Level((ui8)locr.l);
  
            memcpy(pDSAparameters+1, cfp, sizeof(*cfp));
            pDSAparameters[0] = sizeof(*cfp)/sizeof(pDSAparameters[0]);
            ProcessDSAFilter(obj, &timer, locr, NULL, &dsaVars);
            memcpy(cfp, pDSAparameters+1, sizeof(*cfp));
            LoadLevel(currentLevel);
            continue;
          };
        };
      };
    };    
  };



}
