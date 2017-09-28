#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

#ifdef _DEBUG
extern i32 timertrace[1000];
void settrace(i32 n);
#define TIMERTRACE(n) settrace(n);
#else
#define TIMERTRACE(n)
#endif


bool IsPlayFileOpen();

extern bool version90Compatible;
extern bool drawAsSize4Monsters;
extern TIMER missileFilterTimer;


// *********************************************************
//
// *********************************************************
//   TAG0012a0
void DrawNameOfHeldObject(RN object)
{
  char *pName = NULL;
  OBJ_NAME_INDEX objNID6;
  CHARDESC *pcA0;
  char LOCAL_16[16];
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  objNID6 = object.NameIndex();
  if (objNID6 == objNI_Bones_a) // bones
  {
    i16 value;
    DB10 *pBones;
    pBones = GetCommonAddress(object)->CastToDB10();
    value = pBones->value();
    if (value != 0)  // Are these bones associated with a champion?
    {
      ui32 bonesRecord[1];
      if (expool.CopyChampionBonesRecord(object, bonesRecord))
      {
        // Now see if we can find the champion.
        CHARDESC character;
        if (character.CopyCharacter(bonesRecord[0]))
        {
          pcA0 = &character;
          StrCpy(LOCAL_16, pcA0->name);
          StrCat(LOCAL_16, d.ObjectNames[objNID6]);
          pName = LOCAL_16;
        };
      };
    };
  };
  if (pName == NULL)
  {
    pName = d.ObjectNames[objNID6];
  };
  DrawText(
           d.LogicalScreenBase,
           160,
           0xe9,
           0x25,
           4,
           0,
           pName,
           14);
}

// *********************************************************
//
// *********************************************************
//   TAG00135c
void GetIconGraphic(OBJ_NAME_INDEX iconNum, ui8 *dest)
{
  dReg D0, D1, D6, D7;
  OBJ_NAME_INDEX objNID7;
  aReg A2, A3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  objNID7 = iconNum;
  A3 = (aReg)dest;
  for (D6W=0; D6W<7; D6W++)
  {
    D0W = d.Word612[D6W]; //(0,32,64,96,128,160,192)
    if (D0W > objNID7) break;
//
  };
  A2 = (pnt)GetBasicGraphicAddress((--D6W) + 42);
  D7W = sw(objNID7 - d.Word612[D6W]);
  D0W = (I16)(D7W & 0xff0);
  D0W = sw(D0W << 5); // Won't fit!
  D1W = (I16)((D7W & 15) << 1);
  D0W = sw(D0W + D1W);
  D0L = (D0L & 0xffff) << 2;
  A2 += D0L;

  for (D6W=0; D6W<16; D6W++)
  {
    longGear((ui8 *)A3) = longGear((ui8 *)A2);
    A2 += 4;
    A3 += 4;
    longGear((ui8 *)A3) = longGear((ui8 *)A2);
    A3 += 4;
    A2 += 124;
//
  };
}

// *********************************************************
//
// *********************************************************
bool MonsterAllowedOnLevel(RN object, i32 level, MONSTERTYPE mt)
{
  dReg D7;
  MONSTERTYPE mtD6;
  LEVELDESC *ldA2;
  CELLFLAG  *cfA3;
  DB4       *DB4A3;
  ASSERT(level < d.dungeonDatIndex->NumLevel(),">MAX level");
  if (object != RNnul)
  {
    DB4A3 = GetRecordAddressDB4(object);
    if (DB4A3==NULL)
    {
      mtD6 = mon_undefined;
      HopefullyNotNeeded(0x58fa);
    }
    else
    {
      mtD6 = DB4A3->monsterType();
    };
  }
  else
  {
    mtD6 = mt;
    ASSERT(mt!=mon_undefined,"mt");
  };
  ldA2 = &d.pLevelDescriptors[level];
  cfA3 = &d.pppdPointer10450[level][ldA2->LastColumn()][ldA2->LastRow()+1];
  // cfA3 points to cellflag just beyond level.
  for (D7W = ldA2->numMonsterType();
       D7W >0;
       D7W--)
  {
    if (mtD6 == *(cfA3++)) return true;
//
  };
  return false;
}

// *********************************************************
//
// *********************************************************
//  TAG0094ae
  //i16 GetMonsterDescWord2(RN monster)
  //{
  //  DB4 *pDB_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //  pDB_4 = GetRecordAddressDB4(monster);
  //  ASSERT(pDB_4->monsterType() < 27);
  //  return d.MonsterDescriptor[pDB_4->monsterType()].word2;
  //}

// *********************************************************
//
// *********************************************************
//  TAG0094da
i16 MonsterPositionByte(DB4 *pDB4,i16 level, bool noItem16)
{
  dReg D7;
  D7W = (i16)pDB4->groupIndex(); //Item16 index
  if (noItem16) return D7W;
  if (level == d.partyLevel)
  {
    if (D7W >= d.MaxITEM16)
    {
      ASSERT(false, "maxitem16");
      //switch(d.Time)
      //{
      //case 0x120ed:
      //case 0x13d92:
        return 0;
      //};
    };
    //ASSERT(D7W < d.MaxITEM16);
    D7W = d.Item16[D7W].positions(); // 4 2-bit fields
  };
  return D7W;
}

// *********************************************************
//
// *********************************************************
//   TAG009512
void SetMonsterPositionByte(DB4 *pDB4, i16 P2, i16 level, bool noItem16)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if ( (level == d.partyLevel) && !noItem16)
  {
    ASSERT(pDB4->groupIndex() < d.MaxITEM16,"maxitem16");
    d.Item16[pDB4->groupIndex()].positions() = (ui8)P2;
  }
  else
  {
    pDB4->groupIndex(P2);
  };
}

// *********************************************************
//
// *********************************************************
//  TAG009550
i16 MonsterFacingByte(DB4 *pDB4, i16 level, bool noItem16)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if ( (d.partyLevel == level) && !noItem16)
  {
    ASSERT(pDB4->groupIndex() < d.MaxITEM16,"maxitem16");
    if (pDB4->groupIndex() >= d.MaxITEM16) return 0;
    return d.Item16[pDB4->groupIndex()].facings();
  };
  return d.Byte10340[pDB4->facing()]; //0000,1111,2222,3333 encoded in 2-bit nibbles
}

// ********************************************************
//
// ********************************************************
//   TAG009598
void SetMonsterFacingByte(DB4 *pDB4, i32 P2, i32 level, bool noItem16)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if ( (level == d.partyLevel) && !noItem16)
  {
    ASSERT(pDB4->groupIndex() < d.MaxITEM16,"maxitem16");
    d.Item16[pDB4->groupIndex()].facings() = (ui8)P2;
  }
  else
  {
    //30OCT02SETWBITS8_9(pDB4->word14, P2);
    pDB4->facing(P2);
  };
}

//// *********************************************************
////
//// *********************************************************
////  TAG009904
//i16 ExitStairsDirection(i32 mapX, i32 mapY)
//{
//  dReg D0, D4, D5;
////;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//  D0W = GetCellFlags(mapX, mapY);
//  D4W = (i16)(((D0W & 8) == 0) ? 1 : 0); // 1 if east/west
//  if (D4W != 0) //east/west
//  {
//    mapX += d.DeltaX[1]; //Look to the east
//    mapY += d.DeltaY[1];
//  }
//  else
//  {
//    mapX += d.DeltaX[0]; //Look to the north
//    mapY += d.DeltaY[0];
//  };
//  D5W = (i16)(GetCellFlags(mapX, mapY) >> 5);
//  // When we exit the stairs we run into stone wall in
//  // one direction and empty space in the other.
//  D0W = 0;
//  if (D5W==roomSTONE) D0W = 1;
//  if (D5W==roomSTAIRS) D0W = 1;
//  return sw(D4W + (D0W << 1));//
//  // 0  north = open
//  // 1  east  = open
//  // 2  north = stairs or stone
//  // 3  east  = stairs or stone
//}

// *********************************************************
// Rewrite of ExitStairsDirection to allow east/west
// on one level and north/south on the other.
// *********************************************************
//  TAG009904
i16 ExitStairsDirection(i32 mapX, i32 mapY, i32 level)
{
  i32 direction;
  i32 newX, newY;
  CELLFLAG cf;
  ROOMTYPE rt;
  //static bool msg = false;
  for (direction=0; direction<4; direction++)
  {
    newX = mapX + d.DeltaX[direction];
    newY = mapY + d.DeltaY[direction];
    if (newX < 0) continue;
    if (newY < 0) continue;
    if (newX > d.pLevelDescriptors[level].LastColumn())
              continue;
    if (newY > d.pLevelDescriptors[level].LastRow())
              continue;
    cf = *GetCellFlagsAddress(level, newX, newY);
    rt = (ROOMTYPE)(cf >> 5);
    if (rt == roomSTONE) continue;
    if (rt == roomSTAIRS) continue;
    if (rt == roomFALSEWALL)
    {
      if ((cf & 5) == 0) continue;
    };
    return (ui16)direction;
  };
  //if (!msg)
  //{
  //  UI_MessageBox("Illegal Stairway exit\n"
  //                "I won't repeat this warning",
  //                "Warning",
  //                MESSAGE_OK);
  //  msg = true;
  //};
  return d.partyFacing;
}

// *********************************************************
//
// *********************************************************
//  TAG0099b4
DBCOMMON *FirstObjectRecordAddress(i32 mapX, i32 mapY)
{
  return GetCommonAddress(FindFirstObject(mapX, mapY));
}

// *********************************************************
//
// *********************************************************
DB0 *FirstObjectRecordAddressDB0(i32 mapX, i32 mapY)
{
  return GetRecordAddressDB0(FindFirstDoor(mapX, mapY));
}

DB0 *FirstObjectRecordAddressDB0(i32 level, i32 mapX, i32 mapY)
{
  return GetRecordAddressDB0(FindFirstDoor(level, mapX, mapY));
}

DB0 *FirstObjectRecordAddressDB0(const LOCATIONREL& locr)
{
  return GetRecordAddressDB0(FindFirstDoor(locr.l, locr.x, locr.y));
}



// *********************************************************
//
// *********************************************************
DB1 *FirstObjectRecordAddressDB1(i32 mapX, i32 mapY)
{
  RN obj;
  for (obj = FindFirstObject(mapX, mapY);
       obj != RNeof;
       obj = GetDBRecordLink(obj))
  {
    if (obj.dbType() == dbTELEPORTER)
    {
      return GetRecordAddressDB1(obj);
    };
  };
  return NULL;
}

DB1 *FirstObjectRecordAddressDB1(const LOCATIONREL& locr)
{
  return GetRecordAddressDB1(FindFirstTeleporter(locr.l, locr.x, locr.y));
}

// *********************************************************
//
// *********************************************************
DB3 *FirstObjectRecordAddressDB3(i32 mapX, i32 mapY)
{
  return GetRecordAddressDB3(FindFirstObject(mapX, mapY));
}

// *********************************************************
//
// *********************************************************
//   TAG009b08
void AddObjectToRoom(RN newObj,RN prevObj,i32 mapX,i32 mapY,MMRECORD *pmmr)
{
  dReg D0, D1, D4;
  RN objD5;
  CELLFLAG *cfA2;
  RN *prnA3;
  ui16 *puwA3;
  i32 j;
  DBCOMMON *newObjAddr;
  DBCOMMON *prevObjAddr;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"Added %04x to %02x(%02x,%02x)%01x\n",
              newObj.ConvertToInteger(), d.LoadedLevel, mapX, mapY,newObj.pos());
  };
  if (newObj == RNeof) return;
  prnA3 = d.objectList.data() + d.dungeonDatIndex->ObjectListLength();
  for (j=0; j<5; j++)
  {
    if ((*(--prnA3)).ConvertToInteger() != 0xffff) break;
  };
  if (j < 5)
  {
    // There are fewer than 5 empty entries.  We will try to
    // expand  d.objectList.
    i32 newLength;
    newLength = d.dungeonDatIndex->ObjectListLength() + 50;
    d.objectList.resize(newLength, RNnul);
    d.dungeonDatIndex->ObjectListLength((ui16)newLength);
  };
  newObjAddr = GetCommonAddress(newObj);
  newObjAddr->link(RNeof); // Clear forward link
  if (mapX >= 0)
  {
    if ((pmmr != NULL) && (pmmr->objMonster == newObj))
    {
      pmmr->finalLocr = LOCATIONREL(d.LoadedLevel, mapX, mapY);
    };
    //pdA0 = d.ppdPointer10424[mapX];
    cfA2 = &d.LevelCellFlags[mapX][mapY];
    if (*cfA2 & 0x10)
    { // Something in this room already
      prevObj = FindFirstObject(mapX, mapY);// Add after prevObj???
      if (prevObj == RNeof)
      {
        //An error in some converted dungeon caused the 
        //CF flag to be set when there were no objects
        //in the room.  We will tolerate this condition.
        //Find the g_root and stuff our new object into it.
        i32 objectListIndex;
        objectListIndex = GetObjectListIndex(mapX, mapY);
        ASSERT (objectListIndex >= 0,"objIndex");
        ASSERT (d.objectList[objectListIndex] == RNeof,"objectList");
        d.objectList[objectListIndex] = newObj;
        return;
      };
    }
    else
    {
      *cfA2 |= 0x10;
      puwA3 = &d.pCurrentLevelObjectIndex[mapX+1];
      D4W = d.numColumnPointers;
      D1W = d.objectLevelIndex[d.LoadedLevel];
      D1W = (i16)(D1W + mapX);
      D4W = (i16)(D4W - D1W);
      D4W--;
      while ((D4W--) != 0) (*(puwA3++))++;
//

      j = 0;
      cfA2 -= mapY;
      D4W = d.pCurrentLevelObjectIndex[mapX];
      while ((j++) != mapY)
      {
        if (*(cfA2++) & 0x10) D4W++;
      };
      prnA3 = &d.objectList[D4W];
      D0W = d.dungeonDatIndex->ObjectListLength();
      D0W = (i16)(D0W - (D4W+1));
      MemMove((ui8 *)prnA3, (ui8 *)prnA3+2, 2*D0W);
      *prnA3 = newObj;
      ASSERT(FindFirstObject(mapX,mapY)!=RNnul,"firstObject");
      ASSERT(GetCommonAddress(newObj)->link() != RNnul,"link");
      return;
    };
  }
  else
  {
    if ((pmmr != NULL) && (pmmr->objMonster == newObj))
    {
      pmmr->finalLocr = LOCATIONREL(-1,0,0);
    };
  };
  objD5 = GetDBRecordLink(prevObj);
  while (objD5 != RNeof)
  { // Find end of list
    prevObj = objD5;
    objD5 = GetDBRecordLink(objD5);
  };
  prevObjAddr = GetCommonAddress(prevObj);
  ASSERT(newObj != RNnul,"object=RNnul");
  prevObjAddr->link(newObj);
  ASSERT(FindFirstObject(mapX,mapY) != RNnul,"RNnul");
  ASSERT(GetCommonAddress(newObj)->link() != RNnul,"RNnul");
}

RN FindFirstText(const LOCATIONREL& locr)
{
  RN obj;
  obj = FindFirstObject(locr.l, locr.x, locr.y);
  while ((obj!=RNeof) && (obj.dbType()!=dbTEXT) ) // DBNum
  {
    obj = GetDBRecordLink(obj);
  };
  return obj;
}

RN FindFirstMonsterGenerator(const LOCATIONREL& locr)
{
  RN obj;
  if ((GetCellFlags(locr) >> 5) == roomSTONE) return RNeof;
  for (obj = FindFirstObject(locr.l, locr.x, locr.y);
       obj != RNeof;
       obj = GetDBRecordLink(obj))
  {
    if (obj.dbType() != dbACTUATOR) continue;
    if (GetRecordAddressDB3(obj)->actuatorType() == 6) break;
  };
  return obj;
}

// *********************************************************
//
// *********************************************************
// TAG00ab6e
RN FindFirstMonster(i32 mapX,i32 mapY)
{
  RN objD7;
  objD7 = FindFirstObject(mapX, mapY);
  while ((objD7!=RNeof) && (objD7.dbType()!=dbMONSTER) ) // DBNum
  {
    objD7 = GetDBRecordLink(objD7);
  };
  return objD7;
}

RN FindNextMonster(RN obj)
{
  for (obj = GetDBRecordLink(obj);
       (obj!=RNeof) && (obj.dbType()!=dbMONSTER);
       obj = GetDBRecordLink(obj));
  return obj;
}

// *********************************************************
//
// *********************************************************
// 
RN FindFirstMissile(i32 mapX,i32 mapY)
{
  RN objD7;
  objD7 = FindFirstObject(mapX, mapY);
  while ((objD7!=RNeof) && (objD7.dbType()!=dbMISSILE) ) // DBNum
  {
    objD7 = GetDBRecordLink(objD7);
  };
  return objD7;
}

RN FindNextMissile(RN obj)
{
  for (obj = GetDBRecordLink(obj);
       (obj!=RNeof) && (obj.dbType()!=dbMISSILE);
       obj = GetDBRecordLink(obj));
  return obj;
}

// *********************************************************
// *********************************************************
//
// *********************************************************
RN FindFirstDoor(i32 mapX,i32 mapY)
{
  RN objD7;
  objD7 = FindFirstObject(mapX, mapY);
  while ((objD7!=RNeof) && (objD7.dbType()!=dbDOOR) ) // DBNum
  {
    objD7 = GetDBRecordLink(objD7);
  };
  return objD7;
}

RN FindFirstDoor(i32 level, i32 mapX,i32 mapY)
{
  RN objD7;
  objD7 = FindFirstObject(level, mapX, mapY);
  while ((objD7!=RNeof) && (objD7.dbType()!=dbDOOR) ) // DBNum
  {
    objD7 = GetDBRecordLink(objD7);
  };
  return objD7;
}
// *********************************************************
//
// *********************************************************
// TAG00ab6e
RN FindFirstMonster(i32 level, i32 mapX,i32 mapY)
{
  RN objD7;
  objD7 = FindFirstObject(level, mapX, mapY);
  while ((objD7!=RNeof) && (objD7.dbType()!=dbMONSTER) ) // DBNum
  {
    objD7 = GetDBRecordLink(objD7);
  };
  return objD7;
}

// *********************************************************
//
// *********************************************************
//  TAG00abae
i16 OrdinalOfMonsterAtPosition(DB4 *pDB4,i32 pos)
{ //pos is absolute position within cell.
  dReg D0, D4, D5, D6, D7;
  ui8  horizontalSize;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(pos);
  D6W = MonsterPositionByte(pDB4, d.LoadedLevel, false);
  if ((D6W & 255) == 255) return 1;
  D5W = pDB4->numMonM1(); // Number of monsters in group - 1
  horizontalSize = d.MonsterDescriptor[pDB4->monsterType()].horizontalSize();
  if (horizontalSize == 1) // Occupy 2 of the cell's four corners????
  {
    D0W = MonsterFacingByte(pDB4, d.LoadedLevel, false);
    if ( (D0W&1) == (D7W&1) )
    {
      D7W = (I16)((D7W-1) & 3); //Look at CCW-most end of monster
    };
    do
    {
//      D4W = (I16)((D6W >> (2*D5W)) & 3); //position
      D4W = (I16)(TwoBitNibble(D6W,D5W)); //position
//
      if (   (D4W == D7W)
          || (D4W == ((D7W+1)&3)) )
      {
        return sw(D5W+1);
      }
      D5W--;
    } while (D5W >= 0);
    return 0;
  };
  do
  {
//    D0W = (I16)((D6W >> (2*D5W)) & 3);
    D0W = (I16)(TwoBitNibble(D6W,D5W));
    if (D7W == D0W)
    {
      return sw(D5W+1);
    };
    D5W--;
  } while (D5W >= 0);
  return 0;
}

// *********************************************************
//
// *********************************************************
//   TAG00b02c
void ClearAttacking_DeleteMovementTimers(ITEM16 *pi16, i32 mapX, i32 mapY)
{
  i32 i;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  for (i=0; i<4; i++)
  {
    pi16->singleMonsterStatus[i].SetNotAttacking();
//
  };
  DeleteMonsterMovementTimers(mapX, mapY);
}

// *********************************************************
// Even if you specify a position there is still a 25%
// chance that an object will be dropped at random.
// *********************************************************
//   TAG00b364
void CreateAndDropObject(i32 P1,
                         i32 mapX,
                         i32 mapY ,
                         i32 position,  // (or 255)
                         i32 delay)     // sound delay
{ //()
  dReg D1, D6, D7;
  RN   objD5;
  DBCOMMON *dbA2;
  i16  *pwA3;
  i16   w_4;
  i16   w_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  w_2 = 0;
  w_4 = 0;

  switch (P1)
  {
  case mon_Skeleton:
      pwA3 = &d.MonsterDroppings[37];
      break;
  case mon_StoneGolem:
      pwA3 = &d.MonsterDroppings[35];
      break;
  case mon_AntMan:
      pwA3 = &d.MonsterDroppings[33];
      break;
  case mon_DethKnight:
      w_2 = 1;
      pwA3 = &d.MonsterDroppings[26];
      break;
  case mon_RockPile:
      pwA3 = &d.MonsterDroppings[21];
      break;
  case mon_Hellhound:
      pwA3 = &d.MonsterDroppings[18];
      break;
  case mon_Screamer:
      pwA3 = &d.MonsterDroppings[15];
      break;
  case mon_Worm:
      pwA3 = &d.MonsterDroppings[11];
      break;
  case mon_Dragon:
      pwA3 = &d.MonsterDroppings[0];
      break;
  default: return;
  };
  while ( (D7W = *(pwA3++)) != 0)
  {
//
    if (   ((D7W & 0x8000) != 0)
        && (STRandomBool() != 0)   )  continue;
    D7W &= 0x7fff;
    if (D7W >= 127)
    {
      D6W = dbMISC;
      D7W -= 127;
    }
    else if (D7W >=69)
    {
      D6W = dbCLOTHING;
      D7W -= 69;
    }
      else if (D7W >= 23)
    {
      w_4 = 1;
      D6W = dbWEAPON;
      D7W -= 23;
    }
    else continue;
    switch (D6W)
    {
    case dbWEAPON:
      objD5 = FindEmptyDB5Entry(false); break;
    case dbCLOTHING:
      objD5 = FindEmptyDB6Entry(false); break;
    case dbMISC:
      objD5 = FindEmptyDB10Entry(false); break;
    default: ASSERT(0,"bad type");
    };
    //objD5 = FindEmptyDBEntry(D6W);
    if (objD5 == RNnul) continue;
    dbA2 = GetCommonAddress(objD5);
    switch(D6W)
    {
    case dbWEAPON:
      dbA2->CastToDB5()->weaponType((WEAPONTYPE)D7W);
      dbA2->CastToDB5()->cursed(w_2);
      break;
    case dbCLOTHING:
      dbA2->CastToDB6()->clothingType((CLOTHINGTYPE)D7W);
      dbA2->CastToDB6()->cursed(w_2);
      break;
    case dbMISC:
      dbA2->CastToDB10()->miscType((MISCTYPE)D7W);
      dbA2->CastToDB10()->cursed(w_2);
      break;
    default:
      ASSERT(0,"badtype");
    };
    //SETBITS0_6(dbA2->word(2), D7W);
    //SETBITS8_8(dbA2->word(2), w_2);
//
  if ((position == 255) || (STRandom0_3() == 0))
  {
    D1W = STRandom0_3();
  }
  else
  {
    D1W = sw(position);
  };
  objD5.pos(D1W);
  MoveObject(objD5, -1, 0, mapX, mapY, NULL, NULL);

  };
  QueueSound(w_4?0:4, mapX, mapY, delay);
}

// *********************************************************
//
// *********************************************************
void TAG00b522(RN monster, i32 newX, i32 newY)
{//()
  MONSTERTYPE monsterType;
  DB4 *pDB4_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.Word13046 == 0) return;
  pDB4_4 = GetRecordAddressDB4(monster);
  monsterType = pDB4_4->monsterType();
  while (d.Word13046 != 0)
  {
    d.Word13046--;
    CreateAndDropObject(
                        monsterType,
                        newX, newY,
                        (UI8)(d.Byte13050[d.Word13046]),2);
//
  };
}


// *********************************************************
// I got here when I defeated a monster.
// *********************************************************
//   TAG00b580
void DropMonsterRemains(i32 mapX, 
                        i32 mapY, 
                        RN monster, 
                        i16 bodyPartSoundDelay, 
                        i32 monsterAttachedToLevel,
                        bool dropResidue)
{ //()
  dReg D6, D7;
  DB4  *DB4A3;
  RN    objD4, objD5;
  bool droppedWeapon; // w_8;
  i16 w_6;
  i16 w_4;
  //i16 w_2;
  MONSTERTYPE mt_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(mapX);
  D6W = sw(mapY);
  DB4A3 = GetRecordAddressDB4(monster);
  if (bodyPartSoundDelay >= 0)
  {
    mt_2 = DB4A3->monsterType();
    if (dropResidue && d.MonsterDescriptor[mt_2].leavesDroppings())
    {
      w_4 = DB4A3->numMonM1();//#in group-1

      // We used to call MonsterPositionByte with the wrong level!
      // When a monster is teleported to a level that is
      // not legal for the monster type, the monster dies
      // and any leftovers (worm rounds, for example) have
      // to be dropped.  But 'LoadedLevel' is the destination
      // level rather than the source level.  And if the
      // destination level is the same as the party level
      // then we assume an ITEM16 entry exists.  Yukkkk.
      //
      // So anyone who knows better calls us with the actual
      // level to which the monster is attached.
      if (monsterAttachedToLevel != -1)
      {
        //ASSERT(   (monsterAttachedToLevel == d.LoadedLevel)
        //        ||(d.partyLevel != d.LoadedLevel));
        w_6 = MonsterPositionByte(DB4A3, sw(monsterAttachedToLevel), false);
        if (version90Compatible)
        {
          w_6 = MonsterPositionByte(DB4A3, d.LoadedLevel, false);
        };
      }
      else
      {
        w_6 = MonsterPositionByte(DB4A3, -1, false);
      };
      do
      {
        CreateAndDropObject(
                            mt_2,
                            D7W,
                            D6W,
//                            (w_6 == 255) ? 255 : (w_6 >> 2*w_4) & 3,
                            (w_6 == 255) ? 255 : TwoBitNibble(w_6,w_4),
                            bodyPartSoundDelay);

        w_4--;
      } while (w_4 >= 0);
    };
  };
  objD5 = DB4A3->possession();
  if (objD5 == RNeof) return;
  ASSERT(objD5 != RNnul,"RNnul");
  if (objD5 == RNnul) return;
  droppedWeapon = false;
  do
  {
    objD4 = GetDBRecordLink(objD5);
    objD5.pos(STRandom0_3());
    if (objD5.dbType() == dbWEAPON) //door???
    {
      droppedWeapon = true;
    };
    MoveObject(objD5, -1, 0, D7W, D6W, NULL, NULL);
    objD5 = objD4;//Next possession in list
  } while (objD5 != RNeof);
  if (bodyPartSoundDelay >= 0)
  {
    QueueSound(droppedWeapon?0:soundMONDEATH, D7W, D6W, bodyPartSoundDelay);
  };
}


DELETEMONSTERPARAMETERS::DELETEMONSTERPARAMETERS(DELETEMONSTERWHY w, i32 idx)
{
  why = w;
  dropResidue = 0;
  deathCloud = 0;
  index = idx;
}

void ProcessMonsterDeleteFilter(i32 mapX, i32 mapY, DELETEMONSTERPARAMETERS *pdmp, i32 attachedLevel)
{
  NEWDSAPARAMETERS ndp;
  RN obj;
  TIMER timer;
  i32 objectDB, actuatorType;
  DB3 * pActuator;
  LOCATIONREL locr;
  ui32 *pRecord;
  ui32 key;
  i32 len;
  RN  objMonster;
  if (attachedLevel == -1) attachedLevel = d.LoadedLevel;
  objMonster =  FindFirstMonster(attachedLevel, mapX, mapY);
  if (objMonster == RNeof) return;
  //
  // Increment the monsters killed counters.
  {
    DB4 *pMonster;
    MONSTERTYPE mt;
    int subtype;
    ui32 nil[4] = {0,0,0,0};
    pMonster = GetRecordAddressDB4(objMonster);
    mt = pMonster->monsterType();
    subtype = pMonster->alternateGraphics();
    key = (EDT_Statistics << 24) + (ESTAT_NumMonsterKilled << 16) + (pdmp->why << 8) + mt;
    len = expool.Locate(key, &pRecord);
    if (len < 4)
    {
      nil[subtype]++;
      expool.Write(key, nil, 4);
    }
    else
    {
      pRecord[subtype]++;
    };
  };
  //
  //
  key = (EDT_SpecialLocations<<24)|ESL_DELETEMONSTERFILTER;
  len = expool.Locate(key,&pRecord);
  if (len > 0)
  {
    // *****************************************
    // Let the DSA process the parameters
    pdmp->monsterID = objMonster.ConvertToInteger();
    pdmp->level = attachedLevel;//d.LoadedLevel;
    pdmp->x = mapX;
    pdmp->y = mapY;
    //ProcessMonsterDeleteFilter(pRecord, pdmp);
    //memcpy(pDSAparameters+4, adjustSkillsParameters, sizeof(adjustSkillsParameters));
    //ProcessDSAFilter(obj, &timer, locr);
    //LoadLevel(currentLevel);

  
  
  
  
  
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
          memcpy(pDSAparameters+1, pdmp, sizeof (*pdmp));
          pDSAparameters[0] = sizeof(*pdmp)/sizeof(pDSAparameters[0]);
          ProcessDSAFilter(obj, &timer, locr, NULL, &dsaVars);
          memcpy(pdmp, pDSAparameters+1, sizeof (*pdmp));
          LoadLevel(currentLevel);
        };
      };
    };
  };
}

// *********************************************************
//
// *********************************************************
//   TAG00b6ac
void DeleteMonster(i32 mapX, i32 mapY, MMRECORD *pmmr)
{//()
  RN   objD7;
  DB4 *DB4A3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  objD7 = FindFirstMonster(mapX, mapY);
  if (objD7 == RNeof) return;
  if (TimerTraceActive)
  fprintf(GETFILE(TraceFile),"Deleting monster %04x %02x(%02x,%02x)\n",
                  objD7.ConvertToInteger(),d.LoadedLevel,mapX,mapY);
  DB4A3 = GetRecordAddressDB4(objD7);
  ClearMemory((ui8 *)DB4A3->hitPoints6, 8);
  MoveObject(objD7, mapX, mapY, -1, 0, NULL, pmmr);
  //DB4A3->link(RNnul);
  DeleteDBEntry(DB4A3);
  if (d.partyLevel == d.LoadedLevel)
  {
    ASSERT(DB4A3->groupIndex() < d.MaxITEM16,"maxitem16");
    d.Item16[DB4A3->groupIndex()].word0 = -1;
    d.ITEM16QueLen--;
  };
  DeleteMonsterMovementTimers(mapX, mapY);
}

// *********************************************************
//
// *********************************************************
//  TAG00b73a
MONSTER_DAMAGE_RESULT DamageMonster(DB4 *pMonster,   //8
                                    i32 indexInGroup,//12
                                    i32 attackX,     //14
                                    i32 attackY,     //16
                                    i32 damage,          //18
                                    i16 P6,  //20
                                    bool noItem16,
                                    MMRECORD *pmmr)
{//(i16)
  //Return ALL_MONSTERS_DIED if last monster in group died; 
  //  else return SOME_NOT_ALL_MONSTERS_DIED if any monster in group died;
  //    else return NO_MONSTER_DIED
  dReg        D0, D1, D5, D6, D7;
  MONSTER_DAMAGE_RESULT monsterDamageResult;
  i32         d4Index;
  MONSTERDESC *pmtDesc;
  i16         w_20;
  bool        onPartyLevel; //w_18;
  i16         w_16;
  i16         w_14;
  MONSTERTYPE mt_12;
  i16         w_10;
  ITEM16      *pi16_8=NULL;
  TIMER       *pTimer_4;
  TIMERTYPE   tt;
  bool        showDeathCloud = true;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(attackX);
  D6W = sw(attackY);
  mt_12 = pMonster->monsterType();
  pmtDesc = &d.MonsterDescriptor[mt_12];
  if (pmtDesc->invincible()) return NO_MONSTER_DIED;
  if (pMonster->hitPoints6[indexInGroup] == 0xffff) return NO_MONSTER_DIED;
  D0W = pMonster->hitPoints6[indexInGroup];
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile), "Monster[%x] at %02x(%02x,%02x) {HP=%x} loses %02x hitpoints\n",
               indexInGroup, d.LoadedLevel, attackX, attackY, 
               pMonster->hitPoints6[indexInGroup],damage);
  };
  if (D0UW <= damage)
  {  //An individual monster has died.
    w_14 = MonsterPositionByte(pMonster, d.LoadedLevel, noItem16);
    if (w_14 == 255)
    {
      D0W = 255;
    }
    else
    {
//      D0W = (I16)((w_14 >> 2*indexInGroup) & 3);
      D0W = (I16)(TwoBitNibble(w_14,indexInGroup));
    };
    w_20 = D0W;
    w_10 = pMonster->numMonM1(); // #monsters-1
    if (w_10 == 0)
    { // It was the last monster in a group of monsters
      if (P6 != 0)
      {
        DELETEMONSTERPARAMETERS dmp(DMW_Damage, -1);
        ProcessMonsterDeleteFilter(D7W, D6W, &dmp, -1);
        if (dmp.deathCloud == 2) showDeathCloud = false;
        DropMonsterRemains(D7W,D6W,FindFirstMonster(D7W, D6W),2,d.LoadedLevel, dmp.dropResidue<2);
        DeleteMonster(D7W, D6W, pmmr);
      };
      monsterDamageResult = ALL_MONSTERS_DIED; //Group died.
    }
    else
    { //Another monster in the group still survives.
      DELETEMONSTERPARAMETERS dmp(DMW_Damage, indexInGroup);
      ProcessMonsterDeleteFilter(D7W, D6W, &dmp, -1);
      if (dmp.deathCloud == 2) showDeathCloud = false;
      w_16 = MonsterFacingByte(pMonster, d.LoadedLevel, noItem16);
      if (pmtDesc->leavesDroppings()
          && (!pMonster->drawAsSize4() || !drawAsSize4Monsters)) 
      {
        if (P6 != 0)
        {
          if (dmp.dropResidue != 2)
          {
            CreateAndDropObject(mt_12, D7W, D6W, w_20, 2);
          };
        }
        else
        {
          d.Byte13050[d.Word13046++] = (i8)w_20;
        };
      };
      onPartyLevel = d.LoadedLevel==d.partyLevel; //)w_18=1; else w_18=0;
      if (onPartyLevel)
      {
        ASSERT(pMonster->groupIndex() < d.MaxITEM16,"maxitem16");
        pi16_8 = &d.Item16[pMonster->groupIndex()];
      };
      if (pMonster->fear() == StateOfFear6)
      {
        TIMER_SEARCH timerSearch;
        //pTimer_4 = gameTimers.Timers();  xxTIMERxx change to FindNextTimer
        //for (D5W=0; D5W <d.MaxTimer(); D5W++, pTimer_4++)
        while (timerSearch.FindNextTimer())
        {
          pTimer_4 = timerSearch.TimerAddr();
          //if ((pTimer_4->timerTime >> 24) != (ui16)d.LoadedLevel) continue;
          if (pTimer_4->Level() != (ui16)d.LoadedLevel) continue;
          if (pTimer_4->timerUByte6() != D7W) continue;
          if (pTimer_4->timerUByte7() != D6W) continue;
          tt = pTimer_4->Function();
          if (tt < TT_MONSTER_A0) continue;
          if (tt > TT_MONSTER_B3) continue;
          if (tt <= TT_MONSTER_A3)
          {
            d4Index = tt - TT_MONSTER_A0;//  -=33;
          }
          else
          {
            d4Index = tt - TT_MONSTER_B0;// -= 38;
          };
          if (d4Index == indexInGroup)
          {
            //DeleteTimer(D5W);
            timerSearch.DeleteTimer();
          }
          else
          {
            if (d4Index > indexInGroup)
            {
              switch (tt)
              {
              case TT_MONSTER_A1: tt = TT_MONSTER_A0; break;
              case TT_MONSTER_A2: tt = TT_MONSTER_A1; break;
              case TT_MONSTER_A3: tt = TT_MONSTER_A2; break;
              case TT_MONSTER_B1: tt = TT_MONSTER_B0; break;
              case TT_MONSTER_B2: tt = TT_MONSTER_B1; break;
              case TT_MONSTER_B3: tt = TT_MONSTER_B2; break;
              default:
                die(0xff7a,"Illegal MonsterAI Timer");
              };
              pTimer_4->Function(tt);
              //D0W = FindTimerPosition(D5W);
              //D0W = FindTimerPosition(timerSearch.TimerIndex());
              //AdjustTimerPosition(D0W);
              timerSearch.AdjustTimerPriority();
            };
          };
//
        };
        if (onPartyLevel)
        {
          i32 w_12;
          w_12 = pmtDesc->bravery();
          if (w_12 != 15) // 15 is fearless!
          {
            w_12 += w_10 - 1;
            D1W = (i16)STRandom(16);
            if (w_12 < D1W)
            {
              D1W = sw(100 - 4*w_12);
              D0W = sw(STRandom(D1W) + 20);
              pi16_8->uByte5 = D0UB;
              //30OCT02SETWBITS0_3(pMonster->word14, 5);
              pMonster->fear(StateOfFear5);
            };
          };
        };
      };
      d4Index = sw(indexInGroup);
      for (D5L = d4Index; D5W < w_10; D5W++)
      {
        d4Index++;
        pMonster->hitPoints6[D5W] = pMonster->hitPoints6[d4Index];
//        w_16 = SetMonsterPositionBits(w_16, D5W, (w_16 >> 2*D4W) & 3);
        w_16 = SetMonsterPositionBits(w_16, D5W, TwoBitNibble(w_16,d4Index));
//        w_14 = SetMonsterPositionBits(w_14, D5W, (w_14 >> 2*D4W) & 3);
        w_14 = SetMonsterPositionBits(w_14, D5W, TwoBitNibble(w_14,d4Index));
        if (onPartyLevel)
        {
          pi16_8->singleMonsterStatus[D5W] = pi16_8->singleMonsterStatus[d4Index];
        };

//
      };
      w_14 &= 0x3f;
      SetMonsterPositionByte(pMonster, w_14, d.LoadedLevel, noItem16);
      SetMonsterFacingByte(pMonster, w_16, d.LoadedLevel, noItem16);
      // Decrement monsters remaining.
      //30OCT02SETWBITS5_6(pMonster->word14, (BITS5_6(pMonster->word14)-1)&3);
      pMonster->numMonM1(pMonster->numMonM1()-1);
      monsterDamageResult = SOME_NOT_ALL_MONSTERS_DIED;
    };
    D5W = pmtDesc->horizontalSize();
    if (D5W == 0)
    {
      D5W = 110;
    }
    else
    {
      if (D5W ==1)
      {
        D5W = 190;
      }
      else
      {
        D5W = 255;
      };
    };
    if (showDeathCloud)
    {
      if (pMonster->drawAsSize4() && drawAsSize4Monsters)
      {
        if (w_10 == 0)
        {
          CreateCloud(RN(RNMonsterDeath), 255, D7W, D6W, D7W, D6W, 255, pmmr);
        };
      }
      else
      {
        CreateCloud(RN(RNMonsterDeath), D5W, D7W, D6W, D7W, D6W, w_20, pmmr);
      };
    };
    return monsterDamageResult;
  }
  else
  {
    if (damage > 0)
    {
      pMonster->hitPoints6[indexInGroup]
            = uw(pMonster->hitPoints6[indexInGroup] - damage);

    };
    return NO_MONSTER_DIED;
  };
}

void KillMonster(RN rnMon, ui32 indexInGroup)
{
  DB4 *pMon;
  i32 origLoadedLevel;
  pMon = GetRecordAddressDB4(rnMon);
  if (pMon->numMonM1() >= indexInGroup)
  {
    SRCHPKT *pSrchPkt;
    pSrchPkt = SearchForObject(rnMon);
    if (   (pSrchPkt != NULL)
        && (pSrchPkt->place == PLACE_InDungeon))
    {
      origLoadedLevel = d.LoadedLevel;
      LoadLevel(pSrchPkt->level);
      DamageMonster(pMon,
                    indexInGroup, 
                    pSrchPkt->x, 
                    pSrchPkt->y, 
                    pMon->hitPoints6[indexInGroup], 
                    1, 
                    pSrchPkt->level != (unsigned)d.partyLevel,
                    NULL);
      LoadLevel(origLoadedLevel);
    };
  };
}

// *********************************************************
//
// *********************************************************
//  TAG00baec
MONSTER_DAMAGE_RESULT DamageMonsterGroup(DB4 *pDB4,i32 mapX,i32 mapY,i32 damage,i16 P5, bool noItem16, MMRECORD *pmmr)
{//(i16)
  // Return 0 if no damage done.
  // Return 2 if all monsters in group are killed
  // Return 1 if at least one monster in group is killed.
  dReg   D1, D6, D7;
  DB4   *DB4A3;
  i16    w_8;
  i16    w_6;
  //i16    w_4;
  i16    w_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DB4A3 = pDB4;
  D7W = sw(damage);
  w_6 = 0;
  w_8 = 1;
  d.Word13046 = 0;
  if (D7W <= 0) return NO_MONSTER_DIED;
  w_2 = DB4A3->numMonM1();
  D6W = sw(D7W/8 + 1);
  D7W = sw(D7W - D6W);
  D6W *= 2;
  do
  {
    MONSTER_DAMAGE_RESULT monsterDamageResult;
    D1W = (i16)STRandom(D6W);
    monsterDamageResult = 
      DamageMonster(DB4A3, w_2, mapX, mapY, sw(D7W + D1W), P5, noItem16, pmmr);
    w_8 = (I16)((w_8!=0)&&(monsterDamageResult != NO_MONSTER_DIED) ? 1 : 0);
    w_6 = (I16)((w_6!=0)||(monsterDamageResult != NO_MONSTER_DIED) ? 1 : 0);
    w_2--;
  } while (w_2 >= 0);
  if (w_8 != 0) return ALL_MONSTERS_DIED; //If the entire group died.
  if (w_6 != 0) return SOME_NOT_ALL_MONSTERS_DIED; //If one or more died.
  return NO_MONSTER_DIED;               //If none died.
}

// *********************************************************
//
// *********************************************************
//  TAG00bbbe
i16 DeterminePoisonDamage(MONSTERTYPE monsterType, i16 P2)
{
  dReg D0, D7;
  i16 w_10;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (P2 == 0) return 0;
  D7W = d.MonsterDescriptor[monsterType].PoisonResistance();
  if (D7W == 15) return 0;
//
  w_10 = STRandom0_3();
  D0W = sw((8 * (P2 + w_10)) / (D7W + 1));
  return D0W;
}

// *********************************************************
//
// *********************************************************
//   TAG00dea8
void CreateCloud(RN P1,
                 i16 size,
                 i32 objX,i32 objY,
                 i32 missileX, i32 missileY,
                 i16 position, 
                 MMRECORD *pmmr)
{//()
  dReg        D0, D1, D3, D4;
  MONSTERTYPE mtD0;
  DB15        *DB15A3;
  MONSTERDESC *pmtDesc;
  TIMER       timer; //20
  //i32         objY;
  //i32         objX;
  //i32         missileX;
  //i32         missileY;
  RN          objCloud;   //6
  RN          objMonster; //6
  DB4         *pDB4_4;
  i32         damage;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7W = P2;
  //missileX = (i16)XX;
  //missileY = (i16)YY;
  if (objX < 0) return;
  objCloud = FindEmptyDB15Entry(false);
  if (objCloud == RNnul) return;
  //unkA3 = d.misc1052eight[15] + objMissile.idx();
  DB15A3 = GetRecordAddressDB15(objCloud);
  //if (missileX <= 255) // mapY
  //{
  //  objX = missileX;
  //  objY = missileY;
  //}
  //else
  //{
  //  objX = (I16)(missileX & 255);
  //  objY = (I16)(missileY & 255);
  //  missileX = sw(BITS8_15(missileX) - 1);
  //  missileY = BITS8_15(missileY);
  //};
  if (position == 255)
  {
    // not needed! unkA3->word(2) &= 0xff7f;
    DB15A3->b7(1);
  }
  else
  {
    //D3W = 0;
    DB15A3->b7(0);
    objCloud.pos(position);
  };
  D3W = P1.GetSpellType();
  DB15A3->cloudType((CLOUDTYPE)D3W);
  //D3W = (size & 255) << 8;
  DB15A3->value(size);//word(2) = (dbA3->word(2) & 255) | D3W;
  if (   (P1 == RNFireball)
       ||(P1 == RNPoison)
       ||(P1 == RNLightning) )
  {
    QueueSound(size>80 ? 5 : 20,
              missileX,
              missileY,
              1);
  }
  else
  {
    if (P1 != RNMonsterDeath)
    {
      QueueSound(13, missileX, missileY, 1);
    };
  };
  AddObjectToRoom(objCloud, RN(RNempty), missileX, missileY, NULL);
  D0L = d.Time;
  if (P1 == RNffe4)
  {
    D1W = 5;
  }
  else
  {
    D1W = 1;
  };
  D0L += D1W;
  //D0L |= d.LoadedLevel << 24;
  //timer.timerTime = D0L;
  timer.Time(D0L);
  timer.Level((ui8)d.LoadedLevel);
  timer.Function(TT_25);
  timer.timerUByte5(0);
  ASSERT(GetRecordAddressDB15(objCloud)!=NULL,"cloud");
  timer.timerObj8() = objCloud;
  timer.timerUByte6((ui8)missileX);
  timer.timerUByte7((ui8)missileY);
  gameTimers.SetTimer(&timer);
  if ( (P1!=RNLightning) && (P1!=RNFireball) ) return;
  //missileX = w_8;
  //missileY = w_10;
  damage = sw(size/2 + 1);
  damage = sw(damage + STRandom(damage));

  if (P1 != RNFireball)
  {
    damage /= 2;
    if (damage == 0) return;
  };
  if (   (d.LoadedLevel == d.partyLevel)
      && (objX == d.partyX)
      && (objY == d.partyY) )
  {
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),
              "Lightning or fireball - damage parameters w8=%d, w10=%d\n",
              objX, objY);
    };
    DamageAllCharacters(damage, 63, 1);
    return;
  };
  objMonster = FindFirstMonster(objX, objY);
  if (objMonster == RNeof) return;
  pDB4_4 = GetRecordAddressDB4(objMonster);
  mtD0 = pDB4_4->monsterType();
  pmtDesc = &d.MonsterDescriptor[mtD0];
  D4W = pmtDesc->FireResistance();
  if (D4W == 15) return;
  if (pmtDesc->nonMaterial())
  {
    damage >>= 2;
  };
  D1W = sw(STRandom(2*D4W + 1));
  damage = sw(damage - D1W);
  if (damage <= 0) return;
  d.MonsterDamageResult 
     = (i16)DamageMonsterGroup(pDB4_4, objX, objY, damage, 1, false, pmmr);
}

// *********************************************************
//
// *********************************************************
RN CreateFluxcage(i32 mapX, i32 mapY)
{
  RN rnD5;
  dReg D0;
  DB15 *DB15A0;
  TIMER fluxTimer;
  rnD5 = FindEmptyDB15Entry(false);
  if (rnD5 != RNnul)
  {
    AddObjectToRoom(rnD5, RNempty, mapX, mapY, NULL);
    DB15A0 = GetRecordAddressDB15(rnD5);
    DB15A0->cloudType(CT_FLUXCAGE);
    D0L = d.Time + 100;
    //D0L |= d.LoadedLevel << 24;
    //fluxTimer.timerTime = D0L;
    fluxTimer.Time(D0L);
    fluxTimer.Level((ui8)d.LoadedLevel);
    fluxTimer.Function(TT_24);
    fluxTimer.timerUByte5(0);
    fluxTimer.timerObj8() = rnD5;
    fluxTimer.timerUByte6(ub(mapX));
    fluxTimer.timerUByte7(ub(mapY));
    gameTimers.SetTimer(&fluxTimer);
  };
  return rnD5;
}

// *********************************************************
//
// *********************************************************
void TAG00e156(RN missileObject)
{
  DB14 *pDB14;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pDB14 = GetRecordAddressDB14(missileObject);
  gameTimers.DeleteTimer(pDB14->timerIndex());
}

// *********************************************************
// I think I got here when I ran into a fireball.
// *********************************************************
//  TAG00e218
i16 DetermineMagicDamage(DB14 *rec, RN object)
{
  dReg D0, D1, D5, D6;
  DBTYPE dbType;
  //aReg A2;
  WEAPONDESC *weaponDescA2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  d.Word12962 = 0;
  d.Word12964 = 3;
  D5W = sw(rec->rangeRemaining());
  if (object.IsMagicSpell()) dbType = dbCLOUD;
  else dbType = object.dbType(); //missile type
  if (dbType != dbCLOUD)
  {
    if (dbType == dbWEAPON)
    {
      weaponDescA2 = TAG0099d2(object);
      D6W = weaponDescA2->uByte3;
      d.Word12964 = 3;
    }
    else
    {
      D6W = STRandom0_3();
    };
    D0W = sw(GetObjectWeight(object));
    D6W = sw(D6W + D0W/2);
  }
  else
  {
    if (object == RNPoison)
    {
      D6W = (I16)(STRandom() & 15);
      d.Word12962 = sw(D6W + 10);
      D6W = sw(D6W + STRandom(32));
    }
    else
    {
      if (object.IsNonPhysicalSpell()) // >= RNDispellMissile)
      {
        d.Word12964 = 5;
        if (object == RNPoisonBolt)
        {
          d.Word12962 = D5W;
          return 1;
        };
        return 0;
      };
      d.Word12964 = 1;
      D0L = STRandom(16);
      D1L = STRandom(16);
      D6W = sw(D0W + D1W + 10);
      if (object == RNLightning)
      {
        d.Word12964 = 7;
        D6W = sw(5 * D6W);
      };
    };
  };
  D6W = sw((D6W + D5W)/16 + 1);
  D0W = (i16)STRandom(D6W/2 + 1);
  D1W = STRandom0_3();
  D6W = sw(D6W + D1W + D0W);
  D1W = 32;
  D0W = rec->damageRemaining();
  D1W = sw(D1W - D0W/8);
  D6W = sw(Larger(D6W/2, D6W-D1W));
  return D6W;
}

int MissileEncounterFilter(int type, LOCATIONREL locr, RN missile, RN projectile, i32 index)
// See if there is a missile encounter filter.
// Return non-zero if encounter is to be modified.
{
  ui32 key;
  int result = 0;
  RN missileEncounterDSA;
  LOCATIONREL missileEncounterFilterLocr;
  i32 len;
  ui32 *pRecord;
  key = (EDT_SpecialLocations<<24)|ESL_MISSILEENCOUNTERFILTER;
  len = expool.Locate(key,&pRecord);
  if (len > 0)
  //Let the DSA process the parameters
  {
    RN obj;
    TIMER timer;
    i32 objectDB, actuatorType;
    DB3 * pActuator;
    NEWDSAPARAMETERS dsaParameters;
    missileEncounterFilterLocr.Integer(*pRecord);
    for (obj = FindFirstObject(
                    missileEncounterFilterLocr.l,
                    missileEncounterFilterLocr.x,
                    missileEncounterFilterLocr.y);
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
          missileEncounterDSA = obj;
          {
            TIMER timerA;
            int currentLevel;
            DSAVARS dsaVars;
            currentLevel = d.LoadedLevel;
            LoadLevel(missileEncounterFilterLocr.l);
            timerA.timerUByte9(0);//timerFunction
            timerA.timerUByte8(0);//timerPosition
            timerA.timerUByte7((ui8)missileEncounterFilterLocr.y);
            timerA.timerUByte6((ui8)missileEncounterFilterLocr.x);
            //timerA.timerTime   = missileEncounterFilterLocr.l << 24;
            timerA.Time(0);
            timerA.Level((ui8)missileEncounterFilterLocr.l);
            pDSAparameters[0] = 7;
            pDSAparameters[1] = 0; // Default action 
            pDSAparameters[2] = type;
            pDSAparameters[3] = locr.Integer();
            pDSAparameters[4] = missile.ConvertToInteger();
            pDSAparameters[5] = projectile.ConvertToInteger();
            pDSAparameters[6] = BITS10_11(missileFilterTimer.timerWord8());
            pDSAparameters[7] = index;
            ProcessDSAFilter(missileEncounterDSA, &timerA, missileEncounterFilterLocr, NULL, &dsaVars);
            result = pDSAparameters[1];
            LoadLevel(currentLevel);
          };
          break;
        };
      };
    };
  };
  return result;
}


// *********************************************************
//    Process Missile Encounter
//
//  A missile is about to move INTO a fixed object
//  such as a wall
//
//           OR
//
//  A missile is about to move AWAY from from a living
//  being such as a monster or party member.
//  
//
int encounterCount = 0;
// *********************************************************
//  TAG00e35a
i16 ProcessMissileEncounter(
              i32 cellType, //-2 = party; -1 = monster
                            // 8 = Exterior wall of dungeon
              i32 objX, // May contain two coordinates !!!
                        // The missile location is in the upper byte.
                        // Party/monster location in lower byte.
              i32 objY, // May contain two coordinates !!!
                        // The missile location is in the upper byte.
                        // Party/monster location in lower byte.
              i32 missileX,
              i32 missileY,
              i32 objectPos, // Perhaps not same as missilePos!!!!
              RN missile,
              MMRECORD *pmmr)
{//(i16)
  dReg        D0, D1, D4, D5;
  WEAPONTYPE  wtD6;
  RN          RNprojectile;
  OBJ_NAME_INDEX  objNID6;
  DB4         *DBmonA2;
  DB8         *DB8A2;
  DB14        *pDBmissile;
  i16         w_72;
  i16         monsterOrdinal;
  MONSTERTYPE mt_60=mon_undefined;
  DBTYPE      projectileDBType;
  i16         w_56;
  i16         w_54=-1;
  i16         w_52;
  bool        nonPoison_Spell;
  i8          b_35;
  //i16         w_34;
  //i16         w_32;
  //i16         w_30; // y_octant
  //i16         w_28; // x_octant
  //i32         objX; // location of party, monster, door, or wall
  //i32         objY; //   that the missile encluntered.
//  i32         missileX;
//  i32         missileY;
  RN          obj_26;
  RN          *pobj_24;
  ui16        *puw_20 = NULL;
  DB5         *pDB5_16;
  DBCOMMON    *pdb_12;
  MONSTERDESC *pmtDesc;
  DB8         *pdb_4=NULL;
  LOCATIONREL missileLocr;
  D5L = -1;
  objNID6 = (OBJ_NAME_INDEX)0x7ddd;
  encounterCount++;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pDBmissile = GetRecordAddressDB14(missile);
  //w_32 = (I16)XX; //May have two x's packed together
  //w_34 = (I16)YY;
  missileLocr.l=d.LoadedLevel;
  missileLocr.x=missileX;
  missileLocr.y=missileY;
  missileLocr.p=missile.pos();
  w_56 = 0;
  d.MonsterDamageResult = NO_MONSTER_DIED;
  RNprojectile = pDBmissile->flyingObject();
  if (RNprojectile.IsMagicSpell()) projectileDBType = dbCLOUD;
  else projectileDBType = RNprojectile.dbType();
  if (projectileDBType == dbPOTION) // Database type = potion
  {
    DB8A2 = GetRecordAddressDB8(RNprojectile);
//
    //ptD0 = DB8A2->potionType();

    switch (DB8A2->potionType())
    {
    case potion_VenPotion:
      obj_26 = RNPoisonCloud;
      w_56 = 1;
      w_54 = DB8A2->strength();
      pdb_4 = DB8A2;
      break;
    case potion_FulBomb:
      obj_26 = RNFireball;
      w_56 = 1;
      w_54 = DB8A2->strength();
      pdb_4 = DB8A2;
      break;
    default:
      break;
    };
    //if (ptD0 == potion_VenPotion)
    //{
    //  obj_26 = RNPoisonCloud;
    //}
    //else
    //{
    //  if (ptD0 != potion_FulBomb) goto tag00e3ec;
    //  obj_26 = RNFireball;
    //};
    //w_56 = 1;
    //w_54 = DB8A2->strength();
    //pdb_4 = DB8A2;
  };
  nonPoison_Spell =   (projectileDBType == dbCLOUD)
                   && (RNprojectile     != RNPoison) 
                   && (RNprojectile     != RNPoisonBolt);
  pobj_24 = NULL;
  D4W = 0;
  //if (XX < 255)
  //{
  //  missileX = (I16)XX;//because there may be two x's packed together
  //  missileY = (I16)YY;
  //}
  //else
  //{
  //  missileX = (I16)((XX >> 8)-1);
  //  missileY = (I16)(YY >> 8);
  //};
  //objX = XX & 255;
  //objY = YY & 255;
  switch (cellType)
  {
  case roomFALSEWALL:
    switch (MissileEncounterFilter(3, missileLocr, missile, RNprojectile, -1))
    {
    case 1: return 0;
    };
    break;
  case roomSTONE:
    switch (MissileEncounterFilter(4, missileLocr, missile, RNprojectile, -1))
    {
    case 1: return 0;
    };
    break;
  case roomEXTERIOR:
    MissileEncounterFilter(6, missileLocr, missile, RNprojectile, -1);
    break;
  case roomDOOR:
    { 
      i32 cellflags, doorstate;
      cellflags = d.LevelCellFlags[objX][objY];
      b_35 = (ui8)cellflags;
      doorstate = cellflags & 7;
      pdb_12 = GetRecordAddressDB0(FindFirstDoor(objX, objY));

      // door type 5(Not already open) and 'open door' spell
      if ( (doorstate != 5) && (RNprojectile==RNZoSpell) )
      {
        if (pdb_12->CastToDB0()->doorSwitch())
        {
          CreateTimer(TT_DOOR,
                      objX,
                      objY,
                      0,
                      2,
                      d.Time+1);
        };
        break;
      };
      if ( (doorstate==5) || (doorstate<=1) ) return 0;
      D0W = pdb_12->CastToDB0()->doorType();
      //Should it be high byte or low?
      //68000 code references high order byte.  So shall we.
      D0B = BITS8_15(d.DoorTOC[D0W]);
      if (D0W & 2)
      {
        if (projectileDBType == dbCLOUD)
        {
          if (RNprojectile.IsNonPhysicalSpell()) D0W = 1;
          else D0W = 0;
        }
        else
        {
          w_72 = (I16)STRandom(128);
          /*
          D0W = 0;
          //E
          SET(D0B,DBmissileA3->rangeRemaining() > w_72);
          if (D0B != 0)
          {
            D0W = GetObjectTyepIndex(RNprojectile);
            D0W = (I16)(d.ObjDesc[D0W].word4 & 0x100);
          };
          SET(D0B,D0W!=0);
          //D
          if (D0B != 0)
          {
            //C
            SET(D0B,projectileDBType!=dbMISC);
            if (D0B == 0)
            {
              otD6 = GetModifiedObjectType(RNprojectile);
              SET(D0B,otD6<0);
            };
            //A
            if (D0B == 0)
            {
              ASSERT(otD6!=0x7ddd);
              SET(D0B,otD6 >= objNI_FirstKey);
              if (D0B != 0)
              {
                SET(D0B,otD6 <= objNI_LastKey);
              };
              SET(D0B,D0B==0);
            };
            //B
          };
          D0W &= 1;
        */
          D0W =    (pDBmissile->rangeRemaining() > w_72)                         
                          //Sufficient range
                && ((d.ObjDesc[RNprojectile.DescIndex()].word4 & 0x100) != 0) 
                          //Fits in pouch
                && !RNprojectile.IsAKey();
                          //Is not a Key
        };
        if (D0W != 0)
        {
          return 0;
        }
      };
      D5W = sw(DetermineMagicDamage(
                     pDBmissile, RNprojectile) + 1);



      switch (MissileEncounterFilter(2, missileLocr, missile, RNprojectile, -1))
      {
      case 1: return 0;
      };
      


      HitDoor(objX,
              objY,
              D5W + (STRandom()&0xffff)%D5W, //i32 force
              0,                             //bool explosion
              0);                            //i32 delay
    };
    break;
  case -2: // Party
      objectPos = CharacterAtPosition(objectPos);
      if (objectPos < 0)
      {
        return 0;
      };
      switch (MissileEncounterFilter(7, missileLocr, missile, RNprojectile, objectPos))
      {
      case 1: return 0;
      };
      D5W = DetermineMagicDamage(pDBmissile, RNprojectile);
      D4W = D5W;
      break;
  case -1: //Monster
      DBmonA2 = GetRecordAddressDB4(FindFirstMonster(objX, objY));
      monsterOrdinal = sw(OrdinalOfMonsterAtPosition(DBmonA2, objectPos));
      if (monsterOrdinal == 0)
      {
        if (!DBmonA2->drawAsSize4()) return 0;
        monsterOrdinal = 1;
      };
      switch (MissileEncounterFilter(1, missileLocr, missile, RNprojectile, monsterOrdinal-1))
      {
      case 1: return 0;
      }
      //w_62--;
      mt_60 = DBmonA2->monsterType();
      pmtDesc = &d.MonsterDescriptor[mt_60];

      if ( (RNprojectile == RNFireball) && (mt_60 == mon_BlackFlame) )
      {
        puw_20 = &(DBmonA2->hitPoints6[monsterOrdinal-1]);
        D1L = DetermineMagicDamage(pDBmissile, RNprojectile);
        D0W = sw(Smaller(1000, *puw_20 + D1W));
        *puw_20 = D0W;
        //goto tag00e882;
        if (w_56 != 0)
        {
          //pdb_4->link(RNnul); // empty entry
          DeleteDBEntry(pdb_4);
          pDBmissile->flyingObject(obj_26);
        };
        ASSERT(missileX < 32,"W_28");
        ASSERT(missileY < 32,"w_30");
        RemoveObjectFromRoom(missile,
                             missileX,
                             missileY,
                             NULL);
        ProcessObjectFromMissile(missile, pobj_24, missileX, missileY);
        return (1);
      };
      if ( pmtDesc->nonMaterial() && (RNprojectile != RNDispellMissile) )
      {
        return 0;
      }
      
      
      
      ;
      D0L = 64 * (DetermineMagicDamage(
                      pDBmissile, RNprojectile) & 0xffff);

      D1W = pmtDesc->defense08; // uByte8[0];
      if (D1W == 0) D1W = 1;
      D5W = sw((D0L & 0xffff) / D1W);
      if (D5W == 0) break;
      D1L = DeterminePoisonDamage(mt_60, d.Word12962);
      {
        MONSTER_DAMAGE_RESULT monsterDamageResult;
        monsterDamageResult = DamageMonster(
                                            DBmonA2,
                                            monsterOrdinal-1,
                                            objX,
                                            objY,
                                            D5W + D1W,
                                            1,
                                            false,
                                            pmmr);
        if (monsterDamageResult != ALL_MONSTERS_DIED)
        { //At least one monster in group remains.
          ProcessTimers29to41(objX, objY, TT_M2, 0);
        };
        d.MonsterDamageResult = (i16)monsterDamageResult;
        if (nonPoison_Spell) break;
        if (monsterDamageResult != NO_MONSTER_DIED) break;
      };
      if (projectileDBType != dbWEAPON) break;
      if (!pmtDesc->absorbsDaggersEtc()) break;
      pDB5_16 = GetRecordAddressDB5(RNprojectile);
      wtD6 = pDB5_16->weaponType();
      if (   (wtD6 != weapon_Dagger)
          && (wtD6 != weapon_Arrow)
          && (wtD6 != weapon_Slayer)
          && (wtD6 != weapon_PoisonDart)
          && (wtD6 != weapon_ThrowingStar) ) break;
      pobj_24 = DBmonA2->pPossession();
  };  // switch (cellType)
  if (D4W != 0)
  {
    D0W = sw(DamageCharacter(objectPos, D5W, 12, d.Word12964));
    if ( (D0W != 0) && (d.Word12962 != 0) )
    {
      D0W = STRandomBool();
      if (D0W != 0)
      {
        // Note : position converted to chIdx above.
        PoisonCharacter(objectPos, d.Word12962);
      };
    };
  };
//
  if ( nonPoison_Spell || (w_56 != 0) )
  {
    if (w_56 != 0)
    {
      RNprojectile = obj_26;
      w_52 = w_54;
    }
    else
    {
      //unkA3->byte(4) = 0;
      w_52 = pDBmissile->rangeRemaining();
    };
    if (RNprojectile==RNLightning)
    {
      w_52 /= 2;
      if (w_52 == 0) 
      { //goto tag00e882;
        if (w_56 != 0)
        {
          //pdb_4->link(RNnul); // empty entry
          DeleteDBEntry(pdb_4);
          pDBmissile->flyingObject(obj_26);
        };
        ASSERT(missileX < 32,"w_28");
        ASSERT(missileY < 32,"w_30");
        RemoveObjectFromRoom(missile,
                             missileX,
                             missileY,
                             NULL);
        ProcessObjectFromMissile(missile, pobj_24, missileX, missileY);
        return (1);
      };
    };
    if (RNprojectile == RNPoisonCloud)
    {
      D0W = 255;
    }
    else
    {
      D0W = sw(objectPos);
    };
    CreateCloud(RNprojectile, w_52, objX, objY, missileX, missileY, D0W, pmmr); //cause damage
  }
  else
  {
    int soundNum;
    if (     !RNprojectile.IsMagicSpell()
          && (RNprojectile.dbType() == dbWEAPON) )
    {
      soundNum = 0;
    }
    else
    {
      if (RNprojectile == RNPoisonBolt)
      {
        soundNum = 13;
      }
      else
      {
        soundNum = 4;
      };
    };
    QueueSound(soundNum, missileX, missileY, 1);
  };
  if (w_56 != 0)
  {
    //pdb_4->link(RNnul); // empty entry
    DeleteDBEntry(pdb_4);
    pDBmissile->flyingObject(obj_26);
  };
  ASSERT(missileX < 32,"w_28");
  ASSERT(missileY < 32,"w_30");
  RemoveObjectFromRoom(missile,
                       missileX,
                       missileY,
                       NULL);
  ProcessObjectFromMissile(missile, pobj_24, missileX, missileY);
  return (1);
}


// *********************************************************
// Returns true if destination is within 45 degrees of
//  straight ahead.
// *********************************************************
//   TAG00f50a
bool IsItInThisDirection(
              DIRECTION dir,
              i32 myX,
              i32 myY,
              i32 destX,
              i32 destY)
{
  i32 ahead=0, sideways=0;
  switch (dir)
  {
  case 0:
    ahead = myY-destY;
    sideways = destX-myX;
    break;
  case 1:
    ahead = destX-myX;
    sideways = destY-myY;
    break;
  case 2:
    ahead = destY-myY;
    sideways = destX-myX;
    break;
  case 3:
    ahead = myX-destX;
    sideways = destY-myY;
    break;
  }; // switch
  if (ahead < 0) return false;
  // It is in front of us or even with us.
  if (abs(sideways)>ahead+1) return false;
  return true;
}

// *********************************************************
//
// *********************************************************
//  TAG00f582
DIRECTION GetPrimaryAndSecondaryDirection(
                            i32 oldX,
                            i32 oldY,
                            i32 newX,
                            i32 newY)
{
  // Returns the primary direction.  The primary direction is the
  // direction within 45 degrees of the target.  If the target is
  // EXACTLY 45 degrees then primary and secondary are chosen at
  // random from the two directions 45 degress from the target.
  // If the target is less than 45 degrees from the primary direction
  // then the secondary is chosen at random from the two directions
  // 90 degress from the primary direction.
  dReg D0, D5, D6, D7;
  DIRECTION dirD4, dirD7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(oldX);
  D6W = sw(oldY);
  D5W = sw(newX);
  if (oldX == newX)
  {
    D0L = STRandom();
    d.SecondaryDirection = (DIRECTION)((D0UW & 2) + 1);
    if (oldY > newY)
    {
      return dirNORTH; // move north
    };
    return dirSOUTH; // move south (or no move?)
  };
  if (oldY == newY)
  {
    d.SecondaryDirection = (DIRECTION)((STRandom() & 2) + 0);
    if (oldX > newX)
    {
      return dirWEST; // west
    };
    return dirEAST; // east
  };
  // Moved in both axes
  dirD4 = 0; // direction
  for(;;)
  {
    D0W = IsItInThisDirection(dirD4, oldX, oldY, newX, newY);
    if (D0W != 0)
    {
      D0W = IsItInThisDirection(
                       d.SecondaryDirection = (DIRECTION)((dirD4+1)&3),
                       oldX,
                       oldY,
                       newX,
                       newY);
      if (D0W == 0)
      {
        if (   (dirD4 != 0)
            || (IsItInThisDirection(
                         d.SecondaryDirection = (DIRECTION)((dirD4+3)&3),
                         oldX,
                         oldY,
                         newX,
                         newY)==0)  )
        {
          d.SecondaryDirection = (DIRECTION)((dirD4 + (STRandom() & 2) + 1) & 3);
          return dirD4;
        };
      };
      if (STRandomBool() != 0)
      {
        dirD7 = d.SecondaryDirection;
        d.SecondaryDirection = dirD4;
        return dirD7;
      }

      return dirD4;
    };
    dirD4 = (DIRECTION)((dirD4 + 1) & 3);
  };
}

// *********************************************************
//
// *********************************************************
//   TAG00fc20
i16 HitDoor(i32 mapX, i32 mapY,
            i32 force, bool explosion, i32 delay)
{
  //explosion=false for atk_CHOP, atk_BERZERK, etc.
  //explosion=false when fireball first hits door.
  //explosion=true when Timer type 25
  //          (fireball explosion) expires.
  DB0 *DB0A3;
  CELLFLAG *cfA2;
  TIMER timer;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DB0A3 = FirstObjectRecordAddressDB0(mapX, mapY);//door
//
  if (   (explosion  &&  !DB0A3->canOpenWithFireball())
      || (!explosion &&  !DB0A3->canOpenWithAxe())  )
  {
    return 0;
  }
  if (force < (d.DoorTOC[DB0A3->doorType()] & 0xff)) return 0;
  cfA2 = &d.LevelCellFlags[mapX][mapY];
  if ((*cfA2 & 7) == 4) // door type
  {
    if (delay != 0)
    {
      //timer.timerTime = (d.Time + delay) | (d.LoadedLevel << 24);
      timer.Time(d.Time + delay);
      timer.Level((ui8)d.LoadedLevel);
      timer.Function(TT_BASH_DOOR);
      timer.timerUByte6((ui8)mapX);
      timer.timerUByte7((ui8)mapY);
      gameTimers.SetTimer(&timer);
    }
    else
    {
      *cfA2 = (UI8)((*cfA2 & 0xf8) | 5);//Set bottom 3 bits to 5
    };
    return 1;
  };
  return 0;
}

// *********************************************************
//
// *********************************************************
//  TAG011a16
i16 TeleportMonster(const DB1 *pDB1, RN object, i16 attachedLevel, bool noItem16)
{
  dReg D0, D4, D5, D6;
  MONSTERTYPE mtD0;
  RN   objD7;
  const DB1  *DB1A3;
  DB4  *DB4A2;
  i16 w_12;
  i16 w_10;
  i16 w_8;
  i16 w_6;
  i16 w_4;
  i16 w_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DB1A3 = pDB1;
  objD7 = object;
  DB4A2 = GetRecordAddressDB4(objD7);
  D6W = DB1A3->rotation();
  if (!noItem16) D5W = MonsterFacingByte(DB4A2, attachedLevel, noItem16);
  else D5W = d.Byte10340[DB4A2->facing()]; //0000,1111,2222,3333 encoded in 2-bit nibbles
  w_6 = DB1A3->facingMode();
  if (w_6 != 0)
  {
    D4W = D6W;
  }
  else
  {
    D4W = (i16)((D5W + D6W) & 3);
  };
  if (!noItem16)  w_2 = MonsterPositionByte(DB4A2, attachedLevel, noItem16);
  else w_2 = DB4A2->groupIndex();
  if (w_2 != 255)
  {
    w_8 = w_2;
    mtD0 = DB4A2->monsterType();
    w_10 = d.MonsterDescriptor[mtD0].horizontalSize();
    w_12 = (I16)((D4W + 4 - D5W) & 3);
    for (w_4=0; w_4<=DB4A2->numMonM1(); w_4++)
    {
      if (w_6 != 0)
      {
        D0W = D6W;
      }
      else
      {
        D0W = (I16)((D5W + D6W) & 3);
      };
      D4W = SetMonsterPositionBits(D4W, w_4, D0W);
      if (w_10 == 0)
      {
        w_12 = (I16)(w_6==0 ? 1 : 0);
        if (w_12 != 0) w_12 = D6W;
      };
      if (w_12 != 0)
      {
        w_2 = SetMonsterPositionBits(w_2, w_4, (w_8+w_12)&3);
      };
      D5W >>= 2;
      w_8 >>= 2;
//
//
    };
  };
  if (!noItem16) SetMonsterFacingByte(DB4A2, D4W, attachedLevel, noItem16);
  //30OCT02else SETWBITS8_9(DB4A2->word14, D4W);
  else DB4A2->facing(D4W);
  if (!noItem16) SetMonsterPositionByte(DB4A2, w_2, attachedLevel, noItem16);
  else DB4A2->groupIndex(w_2);
  if (    (attachedLevel == d.partyLevel)
       && (DB4A2->fear() == StateOfFear6)  )
  {
    D0L = DB4A2->groupIndex() + 2;
  }
  else
  {
    D0W = 1;
  };
  return D0W;
}

// *********************************************************
//
// *********************************************************
//  TAG011b8e
RN  TeleportMissile(const DB1 *pDB1, RN object)
{
  dReg D5, D6;
  RN   objD7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D6W = d.NewDir;
  objD7 = object;
  D5W = pDB1->rotation(); //New direction
  if (pDB1->facingMode() != 0)
  { // absolute facing
    D6W = D5W;
  }
  else
  {
    D6W = (I16)((D6W + D5W) & 3);
    objD7.pos((objD7.pos() + D5W) & 3);
  };
  d.NewDir = D6W;
  return objD7;
}

// *********************************************************
//
// *********************************************************
//  TAG011bf6
bool Levitating(RN item)
{
  DBTYPE dbType;
  RN objD7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  objD7 = item;
  dbType = objD7.dbType(); // DBNum
  if (dbType == dbMONSTER)
  {
    return objD7.Levitating(); //) & 0x20);
  }
  else
  {
    if (dbType == dbMISSILE)
    {
      return true;
    };
  };
  //if (dbType == dbCLOUD)
  //{
  //  if (GetRecordAddressDB15(objD7)->type() == 50) return true;
  //};
  return false;
}

// *********************************************************
//
// *********************************************************
//   TAG011c3a
void CreateMovementTimer(RN object,
                         i32 mapX,
                         i32 mapY,
                         i32 level,
                         i16 P5)
{
  static bool error = false;
  TIMERTYPE tt;
  TIMER timer;
  TIMER *pTimer;
  TIMER_SEARCH timerSearch;
  i32 count = 0;
  tt = (P5==0)?TT_60:TT_61;
  //pTimer = d.pTimer(0);
  //for (i = 0;
  //     i < d.MaxTimer();
  //     i++, pTimer++)
  while (timerSearch.FindNextTimer())
  {
    pTimer = timerSearch.TimerAddr();
    if (pTimer->Function() != tt) continue;
    if (pTimer->timerUByte6() != mapX) continue;
    if (pTimer->timerUByte7() != mapY) continue;
    //if (pTimer->timerTime >> 24 != (ui32)level) continue;
    if (pTimer->Level() != level) continue;
    count++;
  };
  if (count >= 31)
  {
    if (!error)
    {
      //char message[200];
      //sprintf(message,"Excessive monster generator queue.\n"
      //                "Location %d(%02d,%02d)\n"
      //                "We will simply discard extras\n"
      //                "without further warning",
      //                level, 
      //                mapX + d.pLevelDescriptors[level].offsetX,
      //                mapY + d.pLevelDescriptors[level].offsetY);
      //UI_MessageBox(message, "Warning", MESSAGE_OK);
      error = true;
    };
    {
      DBTYPE type;
      bool delObj = true;
      type = object.dbType();
      if (type == dbMONSTER)
      {
        DB4 *pMon;
        pMon = GetRecordAddressDB4(object);
        if (pMon->Unique())
        {
          delObj = false;
        }
        else
        {
          if (pMon->possession() != RNeof)
          {
            delObj = false;
          };
        };
      };
      if (delObj)
      {
        DeleteDBEntry(GetCommonAddress(object));
        return;
      };
    };
  };
  //timer.timerTime = (d.Time + 5) | (level << 24);
  timer.Time(d.Time + 5);
  timer.Level((ui8)level);
  timer.Function(tt);
  timer.timerUByte5(0);
  timer.timerUByte6((ui8)mapX);
  timer.timerUByte7((ui8)mapY);
  timer.timerObj8() = object;
  gameTimers.SetTimer(&timer);
}

// *********************************************************
//
// *********************************************************
//  TAG011c9a
i16 CheckMoveFromMissile(i32 oldX,i32 oldY,i32 newX,i32 newY, RN movedObject, MMRECORD *pmmr)
{//(i16)
  // A missile encounter occurs as the monster or party member
  // moves AWAY from the missile's position.
  // movedObject is the monster being moved;  RNnul means the party.
  dReg D0, D4, D5;
  RN objD7;
  DB4 *DB4A3;
  ui8 whoLeavesCell[4];     // who first leaves each position in cell
                            //  starting with old cell and then replaced with new cell
  ui8 whoLeavesNewCell[4];  // who first leaves each position in new cell..
                            // (As result of second half-step)
  i32 missileX;
  i32 missileY;
  i32 numCellsToTry = 1;
  i16 movedObjectType;
  HTIMER saveTimerIndex;
  TIMERTYPE tt;

//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if ((oldX==3)&&(oldY==1)&&(newX==3)&&(newY==0))
  {
    RN obj;
    for (obj = FindFirstObject(3,0);
         obj != RNeof;
         obj = GetDBRecordLink(obj))
    {
      if (obj.dbType() == dbMISSILE)
      {
      };
    }
  };
  ClearMemory(whoLeavesCell,4);
  if (movedObject == RNnul)
  { //The party is being moved.
    movedObjectType = -2;
    for (D5W=0; D5W<4; D5W++)
    {
      D0L = CharacterAtPosition(D5W);
      if (D0L >= 0)
      {
        whoLeavesCell[D5UW] = ub(D5UW+1);
      };
    };

  }
  else
  { //A monster is being moved
    movedObjectType = -1;
    DB4A3 = GetRecordAddressDB4(movedObject);
    for (D4W=D5W=0; D5W<4; D5W++)
    {
      D4L |= DB4A3->hitPoints6[D5W];
      D0W = OrdinalOfMonsterAtPosition(DB4A3, D5W);
      TIMERTRACE(0x11d20);
      TIMERTRACE(D0W);
      if (D0W != 0)
      {
        whoLeavesCell[D5W] = ub(D5W+1);
        TIMERTRACE(0x11d30);
        TIMERTRACE(D5W);
      };
    };
    if (D4W == 0) //any hit poits left?
    {
      return 0;
    };
  };
  if (newX >= 0)
  {
    i32 distanceTraveled;
    //D4W = sw(oldX - newX);
    //if (D4W < 0)
    //{
    //  D0W = sw(-D4W);
    //}
    //else
    //{
    //  D0W = D4W;
    //};
    //D4W = sw(oldY - newY);
    //if (D4W < 0)
    //{
    //  D1W = sw(-D4W);
    //}
    //else
    //{
    //  D1W = D4W;
    //};
    //D0W = sw(D0W + D1W); //total distance moved by orthogonal crow
    distanceTraveled = abs(oldX-newX) + abs(oldY-newY);
    TIMERTRACE(0x11d7c);
    TIMERTRACE(distanceTraveled);
    /*
    if (D0W == 1)
    {
      DIRECTION leadPosLeft, leadPosRight;
      // In the direction the group moved, leadPosLeft and leadPosRight
      // are the positions that first entered the new cell.
      leadPosLeft = GetPrimaryAndSecondaryDirection(oldX, oldY, newX, newY);
      leadPosRight = (DIRECTION)((leadPosLeft + 1) & 3);
      ClearMemory((pnt)LOCAL_12,4);
      D0W = occupiedPosition[leadPosLeft];
      LOCAL_12[(leadPosLeft+3)&3] = D0UB;
      if (D0B != 0) LOCAL_4 = 1;
      D0B = occupiedPosition[leadPosRight];
      LOCAL_12[(leadPosRight + 1) & 3] = D0UB;
      if (D0B != 0) LOCAL_4 = 1;
      D0B = occupiedPosition[leadPosLeft];
      if (D0B == 0)
      {
        D0B = occupiedPosition[(leadPosLeft + 3)&3];
        occupiedPosition[leadPosLeft] = D0UB;
      };
      D0B = occupiedPosition[leadPosRight];
      if (D0B == 0)
      {
        D0B = occupiedPosition[(leadPosRight + 1)&3];
        occupiedPosition[leadPosRight] = D0UB;
      };
    };
    */
    if (distanceTraveled == 1)
    {
      DIRECTION headPosLeft, headPosRight, tailPosLeft, tailPosRight;
      // The party/monsters enter the new cell in two columns of two.
      // There is the left column and the right column.  The columns
      // each has a head and tail.
      // headPosLeft and headPosRight
      // are the final positions of the head of the two columns.
      // 'fullStepOccupant' contains the member that will occupy
      // each position in the cell after taking a full step.
      // 'halfStepOccupant' contains the member of the group that
      // will occupy each position in the cell after taking a half-step.
      headPosLeft = GetPrimaryAndSecondaryDirection(oldX, oldY, newX, newY);
      headPosRight = (DIRECTION)((headPosLeft + 1) & 3);
      tailPosLeft =  (DIRECTION)((headPosLeft + 3) & 3);
      tailPosRight = (DIRECTION)((headPosLeft + 2) & 3);
      ClearMemory(whoLeavesNewCell,4);
      //D0W = occupiedPosition[leadPosLeft];
      if (whoLeavesCell[headPosLeft])
      {
        whoLeavesNewCell[tailPosLeft] = whoLeavesCell[headPosLeft];
        numCellsToTry = 2;
      }
      else
      {
        //D0B = occupiedPosition[(leadPosLeft + 3)&3];
        whoLeavesCell[headPosLeft] = whoLeavesCell[tailPosLeft];
      };
      //D0B = occupiedPosition[leadPosRight];
      if (whoLeavesCell[headPosRight])
      {
        whoLeavesNewCell[tailPosRight] = whoLeavesCell[headPosRight];
        numCellsToTry = 2;
      }
      else
      {
        //D0B = fullStepOccupant[(leadPosRight + 1)&3];
        whoLeavesCell[headPosRight] = whoLeavesCell[tailPosRight];
      };
      //D0B = occupiedPosition[leadPosLeft];
      //if (D0B == 0)
      //D0B = occupiedPosition[leadPosRight];
      //if (D0B == 0)
    };
  };


  missileX = sw(oldX);
  missileY = sw(oldY); // This is where we start.
//Loop:
  for (;numCellsToTry>0; numCellsToTry--)
  {
    bool timerDeleted;
    do
    {
      timerDeleted = false;
      for (objD7 = FindFirstMissile(missileX, missileY);
           objD7 != RNeof;
           objD7 = FindNextMissile(objD7))
      {
        TIMERTRACE(0x11e66);
        TIMERTRACE((missileX<<16) + missileY);
        TIMERTRACE(objD7.ConvertToInteger());
        //D0W = ; // misc data type
        i32 missilePos;
        //D0W = objD7.idx() << 3;
        //D0W = wordGear((pnt)d.misc1052eight[14] + D0W + 6);
        D0W = GetRecordAddressDB14(objD7)->timerIndex();
        tt = gameTimers.pTimer(D0W)->Function();
        TIMERTRACE(0x11ea6);
        TIMERTRACE(tt);
        if (tt == TT_Missile0) continue; // Still in thrower's square
        missilePos = objD7.pos();
        D0W = objD7.pos();
        D5W = whoLeavesCell[D0W];
        if (D5W == 0) continue;
        // Did missile strike something?
        // oldX and oldY may contain two coordinates!
        saveTimerIndex = GetRecordAddressDB14(objD7)->timerIndex();
        D0W = ProcessMissileEncounter(
                          movedObjectType,
                          oldX,oldY,
                          missileX, missileY, 
                          D5W-1,
                          objD7, 
                          pmmr);
        //The objD7 object (A missile) may have been deleted
        //from the database!!!!
        if (D0W != 0)
        {
          gameTimers.DeleteTimer(saveTimerIndex); //TAG00e156(objD7);
          if (d.MonsterDamageResult == 2)
          {
            return (1);
          };
          timerDeleted = true;
          //goto Loop;
          break;
        };
      }; //for (objD7 ...)
    } while (timerDeleted);
    TIMERTRACE(0x11f06);
    TIMERTRACE(numCellsToTry);

    if (numCellsToTry > 1)
    {
      // At least one of the columns had a head.
      // The head of the column may encounter a missile
      // in the new cell and then vacate the position.
      // That would results in an encounter.
      //D0W = sw(newX);
      missileX = newX;
      //D0W++;
      //D0W <<= 8;
      //oldX |= ((newX+1) << 8);
      //D0W =  sw(newY);
      missileY = newY;
      //oldY |= (newY << 8);
      MemMove((ui8 *)&whoLeavesNewCell, (ui8 *)&whoLeavesCell, 4);
      //goto Loop;
    };
  };
  return 0;
}

bool PitGoesSomewhere(i32 level, i32 x, i32 y)
{
  return IncrementLevel(level, 1, &x, &y) >= 0;
}


enum MPO
{
  MPO_unknown = 0,
  MPO_monster = 1,
  MPO_party   = 2,
  MPO_other   = 3
};

// *********************************************************
// As a side-effect we set
//   d.NewX, d.NewY, d.NewPos, d.NewDir, d.NewLevel
// *********************************************************
//  TAG011f52
i16 MoveObject(const RN        object,
               const i32       oldX,
               const i32       oldY,
                     i32       newX,
                     i32       newY,
               const DB1      *pPretendTeleporter,
                     MMRECORD *pmmr)
{ //(i16)
  // New position is encoded in object.
  // If object = RNnul then the party is being moved.
  // Return 1 if the object is no longer in the dungeon.
  //  One strange example of this is if you try to put a monster
  //  on top of the party.  Then the monster is removed from the
  //  the dungeon and placed in 'abeyance' to be attempted later.
  static ui8 infiniteTeleportMessage = 0;
  dReg D0, D1, D5, D6;
  i32 objectDBTypeD4;
  RN objD0, objD7;
  aReg A2;
  DBCOMMON  *dbA2;
  const DB1 *DB1A2;
  CHARDESC  *pcA3;
  CELLFLAG  *pdA0;
  i32 maxLoad;
  i16 LOCAL_30;
  i16 LOCAL_28;
  i16 LOCAL_26=0;
  bool unchangedRoom=false; //Stays in same room flag
  i16 LOCAL_22=0;
  i16 LOCAL_20=0;
  i16 LOCAL_18=0;
  bool item16Attached=false;//LOCAL_16 = false;
  i16 LOCAL_14=0;
  MPO monsterPartyOther=MPO_unknown;//LOCAL_12=0;//1 if monster; 2 if party; else 3
  i16 objectPosition=0;
  i16 LOCAL_8=0;
  i32 curLevel = 9999;  // LOCAL_6
  i16 AttachedLevel_4=-1; // Level to which object is attached.
  bool levitated=false;//w_2
  D6L = 0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  objD7 = object;
  objectDBTypeD4 = -1;
  if (objD7 != RNnul)  //If not the party being moved
  {
    ASSERT(GetCommonAddress(objD7)!=NULL,"objNULL");
    objectDBTypeD4 = objD7.dbType(); // DBNum
    D0W = objD7.pos(); // location in room
    objectPosition = D0W;
    levitated = Levitating(objD7); // flying monster or missile
  };
  if (   (oldX>=0)
      && (   (objD7 == RNnul)
          || ((objectDBTypeD4==dbMONSTER)&&(d.LoadedLevel==d.partyLevel))))
  {
    D0W = CheckMoveFromMissile(oldX, oldY, newX, newY, objD7, pmmr);
    if (D0W != 0) return (1);
  };
  if (newX >= 0)
  {
    AttachedLevel_4 = d.LoadedLevel;
    curLevel = d.LoadedLevel;
    //SET(D0B, AttachedLevel_4 == d.partyLevel);
    //if (D0B != 0)
    //{
    //  SET(D0B, oldX >= 0);
    //};
    //D0W &= 1;
    item16Attached = (AttachedLevel_4 == d.partyLevel) && (oldX>=0);
            // proper level && oldX>=0 means ITEM16 exists
    if (objD7 == RNnul)
    {
      d.partyX = sw(newX);
      d.partyY = sw(newY);
      monsterPartyOther = MPO_party;
      SET(D0B, d.SelectedCharacterOrdinal == 0);
      if (D0B != 0)
      {
        SET(D0B, d.PartySleeping == 0);
      };
      D0W &= 1;
      LOCAL_20 = D0W;
      LOCAL_8 = d.partyFacing;
    }
    else
    {
      if (objectDBTypeD4 == dbMONSTER)
      {
        monsterPartyOther = MPO_monster;
      }
      else
      {
        monsterPartyOther = MPO_other;
      };
    };
    if (objectDBTypeD4 == dbMISSILE)
    {
      dbA2 = GetCommonAddress(objD7);
      D0W = dbA2->CastToDB14()->timerIndex();
      //D0 = D0W*10;
      D0W = gameTimers.pTimer(D0W)->timerWord8();
      d.NewDir = BITS10_11(D0W);
    };
    for (LOCAL_30=1000; LOCAL_30!=0; LOCAL_30--)
    { // Limited teleportation??? 1000 seems sufficient.
      if (pPretendTeleporter != NULL)
      {
        D6W = (roomTELEPORTER << 5) | 8; // active teleporter
      }
      else
      {
        pdA0 = d.LevelCellFlags[newX];
        D6W = pdA0[newY];
      };
      D5W = sw(D6W >> 5);
      if (    (D5W == roomTELEPORTER)
           && ((objectDBTypeD4 < 0) || (objectDBTypeD4 >= dbMONSTER))
         )
      {
        if ((D6W & 8)==0) break;
        if (pPretendTeleporter != NULL)
        {
          DB1A2 = pPretendTeleporter;
          pPretendTeleporter = NULL; //Just once, thank you.
        }
        else
        {
          DB1A2 = FirstObjectRecordAddressDB1(newX, newY);
        };
        if (DB1A2 == NULL) break;
        //D0W = DB1A2->word2;
        if ((DB1A2->what()==1) && (objectDBTypeD4!=dbMONSTER)) break; // monsters only
        //if ((BITS13_14(D0W)==1) && (D4W!=4)) break; // monsters only
        if (monsterPartyOther != MPO_other)
        {  // Monster or party entering teleporter
          // Check for party teleport allowed
          if (DB1A2->what() == 0) break; //Objects Only
          if ((DB1A2->what() == 1) & (monsterPartyOther != MPO_monster)) break;
          if ((DB1A2->what() == 2) & (monsterPartyOther != MPO_party)) break;
        };
        SET(D0B, newX == (DB1A2->destX()));
        if (D0B!=0)
        {
          SET(D0B, newY==(DB1A2->destY()));
        };
        if (D0B!=0)
        {
          SET(D0B, curLevel==(DB1A2->destLevel()));
        };
        LOCAL_22 = (I16)(D0W&1);
        if (TimerTraceActive)
        {
          fprintf(GETFILE(TraceFile),"Teleporting object %04x from %02x(%02x,%02x)",
                     object.ConvertToInteger(),curLevel, newX, newY);
        };
        newX = DB1A2->destX();
        newY = DB1A2->destY();
        LOCAL_26 = DB1A2->audible();
        curLevel = DB1A2->destLevel();
        LoadLevel(curLevel);  //teleporter destination level
        if (TimerTraceActive)
        fprintf(GETFILE(TraceFile)," to %02x(%02x,%02x)\n",curLevel,newX,newY);
        if (objD7 == RNnul)
        {
          d.partyX = sw(newX);
          d.partyY = sw(newY);
          if (DB1A2->audible() != 0)
          { //audible buzz
            QueueSound(soundTELEPORT, d.partyX, d.partyY, 0);
          };
          LOCAL_20 &= 1;//d.BigMemory;
          if (DB1A2->facingMode() != 0)
          { // absolute face
            SetPartyFacing(DB1A2->rotation());
          }
          else
          {
            D0W = d.partyFacing;
            D1W = DB1A2->rotation();
            D0W = (I16)((D0W + D1W) & 3);
            SetPartyFacing(D0W);
          };
        }
        else
        {
          if (objectDBTypeD4 == dbMONSTER)
          {
            if (DB1A2->audible() != 0)
            { // audible buzz
              QueueSound(17, newX, newY, 1);
            };
            ASSERT(AttachedLevel_4 >= 0,"attachedLevel");
            LOCAL_14 = TeleportMonster(DB1A2, objD7, AttachedLevel_4, !item16Attached);
          }
          else
          {
            if (objectDBTypeD4 == dbMISSILE)
            {
              objD7 = TeleportMissile(DB1A2, objD7);
            }
            else
            {
              if (DB1A2->facingMode() == 0)
              { // absolute facing
                if (oldX != -1)
                {
                  D1W = objD7.pos();
                  D0W = DB1A2->rotation();
                  //D1W = ((D0W + D1W) & 3) << 14;
                  objD7.pos(D0W+D1W);
                };
              };
            };
          };
        };
        if (LOCAL_22 != 0) break;
      }
      else //not teleporter
      {
        if (   (D5W == roomPIT)  // Room type = PIT
            && (!levitated)      // Not missile or flying monster
            && ((D6W&8) != 0)    // Pit is active
            && ((D6W&1) == 0)
            && PitGoesSomewhere(curLevel, newX, newY)
            && ((objectDBTypeD4 < 0) || (objectDBTypeD4 >= dbMONSTER))
           )
        {
          if (   (LOCAL_20 != 0)
              && (d.SupressPitDamage == 0))
          {
            LOCAL_20 = 1;//d.BigMemory;
            //conflux movie - ReadGraphicsForLevel();
            //  The game can crash if we don't read the graphics for
            //  this new level.  This was an Atari bug, IMHO.
            //  But I don't want to ReadGraphicsForLevel because it is
            //  RESTARTABLE and that is too much work.
            //
            //confluxmovie - DrawViewport(d.partyFacing, newX, newY);
            // Calling DrawViewport here causes problems because we have not LoadedPartyLevel.
            // Therefore, the monsters we draw have the wrong data in uByte5.  We could try 
            // LoadPartyLevel here or we could simply skip the DrawViewport call.
            //
            //  I removed this because of the bug reported a few lines up.
            //
            //Delay(_4_,15);//This was my invention so that
            //I could see the floors as we went by.
            //But making MoveObject RESTARTABLE turned out
            //to be just too much work. PRS
          };
          if (TimerTraceActive)
          fprintf(GETFILE(TraceFile),"Object %04x falls from %02x(%02x,%02x)",
                 object.ConvertToInteger(),curLevel,newX,newY);
          curLevel = IncrementLevel(curLevel, 1, &newX, &newY);
          if (TimerTraceActive)
          fprintf(GETFILE(TraceFile),"to %02x(%02x,%02x)\n",
                        curLevel,newX,newY);
          LoadLevel(curLevel);
          if (objD7 == RNnul)
          {
            d.partyX = sw(newX);
            d.partyY = sw(newY);
            if (d.NumCharacter > 0)
            {
              if (d.SupressPitDamage != 0)
              {
                D5W = 0;
                pcA3 = d.CH16482;
                while (D5UW < d.NumCharacter)
                {
                  if (pcA3->HP() != 0)
                  {
                    maxLoad=MaxLoad(pcA3);
                    D0W = pcA3->load;
                    D0L = (25 * D0UW) & 0xffff;
                    D0W = sw((D0L / maxLoad) + 1);
                    AdjustStamina(D5W, D0W);
                  };
                  D5W++;
                  pcA3 ++;
                }; // while
              }
              else
              {
                D0W = DamageAllCharacters(20, 48, 2);
                if (D0W != 0)
                {
                  QueueSound(6, d.partyX, d.partyY, 0);
                };
              };
            };
            d.SupressPitDamage = 0;
          }
          else
          {
            if (objectDBTypeD4 == dbMONSTER)
            {
              MONSTER_DAMAGE_RESULT monsterDamageResult;
              ASSERT(AttachedLevel_4 >= 0,"attached_level");
              LoadLevel(AttachedLevel_4);

              monsterDamageResult = 
                DamageMonsterGroup(GetRecordAddressDB4(objD7), oldX, oldY, 20, 0, !item16Attached, pmmr);
              LoadLevel(curLevel);
              LOCAL_18 = (I16)((monsterDamageResult == 2) ? 1 : 0);
              if (LOCAL_18 != 0) break;
              if (monsterDamageResult == SOME_NOT_ALL_MONSTERS_DIED)
              {
                TAG00b522(objD7, newX, newY);
              };
            };
          };
        }
        else
        {
          if (D5W !=3) break;
          if (objD7 == RNnul) break;
          if (objectDBTypeD4 == dbMISSILE) break;
          if ((objectDBTypeD4 >= 0) && (objectDBTypeD4 < dbMONSTER)) break;
          if ((D6W & 4) == 0)
          {
            curLevel = IncrementLevel(curLevel, 1, &newX, &newY);
            LoadLevel(curLevel);
          };
          LOCAL_8 = ExitStairsDirection(newX, newY, d.LoadedLevel);
          D1W = d.DeltaX[LOCAL_8];
          newX += D1W;
          D1W = d.DeltaY[LOCAL_8];
          newY += D1W;
          LOCAL_8 = (I16)((LOCAL_8 + 2) & 3);
          LOCAL_28 = objD7.pos();
          D0W = sw(LOCAL_28 - LOCAL_8 + 1);
          LOCAL_28 = (I16)((((D0W & 2)>>1)+LOCAL_8)&3);
          //D0W = objD7.rid();
          //D1W = LOCAL_28 << 14;
          objD7.pos(LOCAL_28);
        };
        //break;
      };

    }; // for (LOCAL_30)
    if (LOCAL_30 == 0)
    {
      if (infiniteTeleportMessage==0)
      {
        char msg[80];
        sprintf(msg,"Infinite teleportation %d(%d,%d)",curLevel,newX,newY);
        //UI_MessageBox(msg, "Design Error", MB_OK);
        infiniteTeleportMessage++;
      };
    };
    if (objectDBTypeD4 == dbMONSTER) // DBNum
    {
      if (   (LOCAL_18 != 0)
          || !MonsterAllowedOnLevel(objD7,curLevel)  )
      {
        DELETEMONSTERPARAMETERS dmp(DMW_Movement1, -1);
        if (pmmr != NULL)
        {
          pmmr->Setflg(MoveObject_NotAllowedOnLevel);
          pmmr->finalLocr.l = -1;
        };
        TAG00b522(objD7, newX, newY);
        ASSERT(AttachedLevel_4 >= 0,"attachedLevel");
        if (oldX >= 0) ProcessMonsterDeleteFilter(oldX, oldY, &dmp, AttachedLevel_4);
        DropMonsterRemains(newX, 
                           newY, 
                           objD7, 
                           2, 
                           oldX>=0?AttachedLevel_4:-1,
                           dmp.dropResidue<2);
        LoadLevel(AttachedLevel_4);
        if (oldX >= 0)
        {
          DeleteMonster(oldX, oldY, pmmr);
        };
        return (1);
      };
    };
    d.NewX = sw(newX);
    d.NewY = sw(newY);
    d.NewLevel = sw(curLevel); //level
    d.NewPos = 0;
    if (objD7 != RNnul) d.NewPos = objD7.pos();
    //if (d.Word554 != 555)
    //{
    //  d.Word11754++;
    //};
    SET(D0B, curLevel == AttachedLevel_4);
    if (D0B != 0)
    {
      SET(D0B, oldX == newX);
    };
    if (D0B != 0)
    {
      SET(D0B,newY == oldY);
    };
    unchangedRoom = (D0W & 1)!=0;
//    if (!unchangedRoom) goto tag012668;
    if (unchangedRoom)
    {
//      if (objD7 != RNnul) goto tag01264c;
//      if (d.partyFacing != LOCAL_8) goto tag01274e;
//      return 0;
      if (objD7 == RNnul)
      {
        if (d.partyFacing == LOCAL_8) return 0;
//tag01264a:
      //goto tag01274e;
      }
      else
      {
//tag01264c:
//      if (d.NewPos != objectPosition) goto tag01274e;
//      if (objectDBTypeD4 == dbMISSILE) goto tag01274e;
        if (   (d.NewPos == objectPosition)
            && (objectDBTypeD4 != dbMISSILE) )
        {
          return 0;
        };
//tag012664:
      //goto tag01274e;
      };
    }
    else
    {
//tag012668:
//    if (objD7 != RNnul) goto tag01273a;
//    if (d.NumCharacter == 0) goto tag01273a; // #characters?
      if (   (objD7 == RNnul)
          && (d.NumCharacter != 0) )
      {
        LOCAL_28 = D6W;
        for (D6W=d.NumFootprintEnt; D6W>=24; D6W--)
        {
          DeleteFootprintEntry(0);
        };
        if (D6W != 0)
        {
          AddFootprintEntry(oldX, oldY, (ui16)(d.Time-d.LastPartyMoveTime));
        };
        d.LastPartyMoveTime = d.Time;
        d.NumFootprintEnt++;
        if (d.MagicFootprintsActive != 0)
        {
          d.IndexOfLastMagicFootprint = d.NumFootprintEnt;
        };
        d.PartyFootprints[D6W] =sw(    ((curLevel & 0x3f) << 10)
                                    | ((newY & 0x1f) << 5)
                                    | (newX & 0x1f) );
        d.Byte13220[D6W] = 0;
        AddFootprintEntry(newX, newY, (ui16)0x8018);
        D6W = LOCAL_28;
      };
//tag01273a:
      if (curLevel != AttachedLevel_4)
      {
        ASSERT(AttachedLevel_4 >= 0,"attached level");
        LoadLevel(AttachedLevel_4);
      };
    };
  };
//tag01274e:
  if (oldX >= 0)
  {
    if (objD7 == RNnul)
    {
      PlaceOrRemoveObjectInRoom(
                     oldX,
                     oldY,
                     RN(RNnul),
                     unchangedRoom,  // if location unchanged
                     false,          // 'Remove' rather than 'place'
                     pmmr);
    }
    else
    {
      if (levitated)
      {
        RemoveObjectFromRoom(objD7,oldX,oldY,pmmr);
      }
      else
      {
        SET(D0B,d.LoadedLevel==d.partyLevel);
        if (D0B != 0)
        {
          SET(D0B, oldX==d.partyX);
        };
        if (D0B != 0)
        {
          SET(D0B, oldY==d.partyY);
        };
        PlaceOrRemoveObjectInRoom(
                          oldX,
                          oldY,
                          objD7,
                          (D0W&1)!=0,  // Room is unchanged
                          false,       // 'remove' rather than 'place'
                          pmmr);
      };
    };
  };
  if (newX < 0) return 0;
  if (objD7 == RNnul)
  {
    LoadLevel(curLevel);
    objD7 = FindFirstMonster(d.partyX, d.partyY);
    if (objD7 != RNeof)
    {
      DELETEMONSTERPARAMETERS dmp(DMW_Movement2, -1);
      ProcessMonsterDeleteFilter(d.partyX, d.partyY, &dmp, -1);
      DropMonsterRemains(d.partyX, d.partyY, objD7, 1, d.LoadedLevel, dmp.dropResidue<2);
      DeleteMonster(d.partyX, d.partyY, pmmr);
    };
    if (curLevel == AttachedLevel_4)
    {
      PlaceOrRemoveObjectInRoom(
                 d.partyX,
                 d.partyY,
                 RN(RNnul),
                 unchangedRoom,  // If location is unchanged
                 true,           // 'place' rather than 'remove'
                 pmmr);
    }
    else
    {
      ASSERT(AttachedLevel_4 >= 0,"attached level");
      LoadLevel(AttachedLevel_4);
      d.newPartyLevel = sw(curLevel);
    };
    return 0;
  };
  if (objectDBTypeD4 == dbMONSTER)
  {
    LoadLevel(curLevel);
    A2 = (pnt)GetCommonAddress(objD7);
    D6W = *((ui8 *)A2 + 5); //Positions
//    if (curLevel != d.partyLevel) goto tag0128aa;
//    if (newX != d.partyX) goto tag0128aa;
//    if (newY == d.partyY) goto tag0128be;
    if (    (   (curLevel == d.partyLevel)
             && (newX == d.partyX)
             && (newY == d.partyY)
            )
         || ((objD0 = FindFirstMonster(newX, newY)) != RNeof) )
//tag0128aa:
//      objD0 = FindFirstMonster(newX, newY);
//      if (objD0 == RNeof) goto tag01290c;
    {
//tag0128be:
      ASSERT(AttachedLevel_4 >= 0,"attached level");
      LoadLevel(AttachedLevel_4);
      if (oldX >= 0)
      {
        DeleteMonsterMovementTimers(oldX, oldY);
      };
      if (item16Attached)
      {
        DetachItem16(D6W); // (item16Index)
      };
      CreateMovementTimer(objD7, newX, newY, curLevel, LOCAL_26);
      return (1);
    };
//tag01290c:
    //if (!item16Attached) goto tag01292e;
    //if (curLevel == d.partyLevel) goto tag01292e;
    if (   item16Attached
        && (curLevel != d.partyLevel))
    {
      DetachItem16(D6W);
      LOCAL_14 = 1;
    //goto tag012956;
//tag01292e:
    }
    else
    {
      if (curLevel == d.partyLevel)
      {
        if (!item16Attached)
        {
          AttachItem16ToMonster(objD7, newX, newY);
          LOCAL_14++;
        };
      };
    };
//tag012956:
//    if (!levitated) goto tag012970;
//    AddObjectToRoom(objD7, RN(RNempty), newX, newY);
//    goto tag012988;
    if (levitated)
    {
      AddObjectToRoom(objD7, RN(RNempty), newX, newY, pmmr);
//    goto tag012988;
    }
    else
    {
//tag012970: // Jump into middle of 'while' ??
      PlaceOrRemoveObjectInRoom(
                        newX,
                        newY,
                        objD7,
                        false,   // Not current room.  Room changed.
                        true,    // 'place' rather than 'remove'
                        pmmr);
    };
//tag012988:

    if ((LOCAL_14 != 0) || (oldX < 0))
    {
      StartMonsterMovementTimers(newX, newY);
    };
    ASSERT(AttachedLevel_4 >= 0,"attached level");
    LoadLevel(AttachedLevel_4);
    if (oldX >= 0)
    {
      if (LOCAL_14 > 1)
      {
        ASSERT(LOCAL_14-2 < d.MaxITEM16,"maxitem16");
        ClearAttacking_DeleteMovementTimers(&d.Item16[LOCAL_14-2], oldX, oldY);
      }
      else
      {
        if (LOCAL_14 != 0)
        {
          DeleteMonsterMovementTimers(oldX, oldY);
        };
      };
    };
    return (LOCAL_14);
  };
  ASSERT(curLevel < 100,"curLevel");
  LoadLevel(curLevel);
  if (objectDBTypeD4 == dbMISSILE)
  {
    AddObjectToRoom(objD7, RN(RNempty), newX, newY, NULL);
  }
  else
  {
    SET(D0B, d.LoadedLevel==d.partyLevel);
    if (D0B != 0)
    {
      SET(D0B, newX == d.partyX);
    };
    if (D0B != 0)
    {
      SET(D0B, newY==d.partyY);
    };
    PlaceOrRemoveObjectInRoom(
                      newX,
                      newY,
                      objD7,
                      (D0W&1)!=0,   // If room is unchanged
                      true,         // 'place' (as opposed to 'remove')
                      pmmr);
  };
  ASSERT(AttachedLevel_4 >= 0,"attachedLevel");
  LoadLevel(AttachedLevel_4);
  return (0xccc);
                // A return is necessary to avoid a compiler
                // bug that runs into the jump table for the labels.
                // The Atari code put nothing in particular in D0W.
                // So I put a 0xccc and everyone that calls me
                // that expects a value in return ASSERTS that the
                // value 0xccc is not returned.
}

// *********************************************************
//
// *********************************************************
//   TAG012b80
void RotateActuatorList()
{
  DBCOMMON *dbA3;
  DB3     *DB3A2;
  RN objD6, objD7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.DelayedActuatorAction == -1) return;
  if (   (d.DelayedActuatorAction == 1)
      || (d.DelayedActuatorAction == 2) )
  {
    objD7 = FindFirstObject(d.DelayedActuatorActionX,
                            d.DelayedActuatorActionY);
    while (   (objD7.dbType() != dbACTUATOR)
           || (    (d.DelayedActuatorActionPos != -1)
                && (objD7.pos() != (unsigned)d.DelayedActuatorActionPos) ) )
    {
      objD7 = GetDBRecordLink(objD7);
//
//
    }; // while
    dbA3 = GetCommonAddress(objD7);
    objD6 = dbA3->link();
    while (   (objD6!=RNeof)
           && (   (objD6.dbType()!=dbACTUATOR)
               || (   (d.DelayedActuatorActionPos != -1)
                   && (objD6.pos() != (unsigned)d.DelayedActuatorActionPos))))
    {
      objD6 = GetDBRecordLink(objD6);
//
//
//
    };
    if (objD6 != RNeof)
    {
      // objD7 is the first occurance of an
      // actuator at the proper position and
      // objD6 is the second
      // occurance of such an actuator.
      RemoveObjectFromRoom(objD7,
                           d.DelayedActuatorActionX,
                           d.DelayedActuatorActionY,
                           NULL);
      DB3A2 = GetRecordAddressDB3(objD6);
      objD6 = GetDBRecordLink(objD6);
      while ( (objD6!=RNeof) && (objD6.dbType() == dbACTUATOR))
      {
//
        if (   (d.DelayedActuatorActionPos == -1)
            || (objD6.pos() == (unsigned)d.DelayedActuatorActionPos) )
        {
          DB3A2 = GetRecordAddressDB3(objD6);
        };
        objD6 = GetDBRecordLink(objD6);

//
      };
      dbA3->link(DB3A2->link());
      DB3A2->link(objD7);
    };
  };
  d.DelayedActuatorAction = -1;
}

void WarnIllegalTimer(i32 srcX, i32 srcY, i32 dstX, i32 dstY)
{
  
  static bool warned = false;
  if (warned) return;
  warned = true;
  if (IsPlayFileOpen()) return;
  char msg[200];
  sprintf(msg,"Illegal Actuator Target.\n"
              "Source = %d(%02d,%02d)\n"
              "Destination = (%02d,%02d)"
              "No great harm but the designer\n"
              "probably meant something else.",
              d.LoadedLevel,srcX,srcY,dstX,dstY);
  UI_MessageBox(msg,"Warning",MESSAGE_OK);
}

// *********************************************************
// If pDB3->word4 bit 11 == 0 que timer as shown in table below
//       If target room is not stone position = 0
//       If target room is stone position = pDB3->tagetPos();
// If pDB3->word4 bit 11 == 1
//       ???????????
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
void QueueSwitchAction(DB3 *pDB3,        //Source switch's DB entry
                       i32 tmrAct,       //Action set/clear/toggle
                       i32 mapX,i32 mapY,//Source switch's location
                       i32 P5)
{//()
  i32 targetX, targetY, targetPos;
  i32 actionTime;
  i32 numericFunction;
  TIMERTYPE tt;
  CELLFLAG cfD4;
  ROOMTYPE targetRoomType;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (pDB3->onceOnly()) pDB3->actuatorType(0);
  actionTime = d.Time + pDB3->delay();
  if (pDB3->localActionOnly())
  {
    PerformLocalAction(pDB3->localActionType(), mapX, mapY, P5);
  }
  else
  {
    targetX = pDB3->targetX();
    targetY = pDB3->targetY();
    cfD4 = d.LevelCellFlags[targetX][targetY]; //Cellflag
    targetRoomType = (ROOMTYPE)(cfD4>>5); // roomType
    if (   (targetRoomType == roomSTONE)
        || (targetRoomType == roomFALSEWALL))
    {
      targetPos = pDB3->targetPos();
    }
    else
    {
      targetPos = 0;
    };
    numericFunction = d.Byte1596[targetRoomType];
    switch (numericFunction)
    {
    case 7: tt = TT_FALSEWALL; break;
    default:tt = (TIMERTYPE)numericFunction; break;
    };
    if (tt == 0) 
    {
      WarnIllegalTimer(mapX, mapY, targetX, targetY);
      return;
    };
    CreateTimer(tt,
                targetX,
                targetY,
                targetPos,
                tmrAct,   //tmrAct_SET, _CLEAR, or _TOGGLE
                actionTime);
  };
}

// *********************************************************
//
// *********************************************************
//   TAG012de0
bool DoesPartyHave(OBJ_NAME_INDEX actuatorValue)
{ // Called only when actuator type 8 is pressed.
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D5, D6;
  OBJ_NAME_INDEX objNID7;
  RN  objD4;
  CHARDESC *chA3;
//  RN      *objA2=NULL;
  DB9     *pDB9_8;
  i16 w_4;
  OBJ_NAME_INDEX objNI_2;
  D5L=0;
  objNID7 = actuatorValue;
  w_4 = 0;

  for (D6W=0, chA3 = &d.CH16482[0];
       D6W < d.NumCharacter;
       D6W++, chA3++)
  {
    if (chA3->HP() != 0)
    {
      //objA2 = chA3->possessions;
      for (D5W = 0; (D5W < 30) && (w_4==0); D5W++)
      {
        //objD4 = *(objA2++);
        objD4 = chA3->Possession(D5W);
tag012e14:
        objNI_2 = GetBasicObjectType(objD4);
        if (objNI_2 == objNID7)
        {
          return 1;
        };
        if (objNI_2 != objNI_Chest) continue;
        pDB9_8 = GetRecordAddressDB9(objD4);
        for (objD4 = pDB9_8->contents();
             objD4 != RNeof;
             objD4 = GetDBRecordLink(objD4) )
        {
          //D0W = GetBasicObjectType(objD4);
          if (GetBasicObjectType(objD4) == objNID7) return 1;
//
//
        };
//
      };
    };
//
//
  };
  if (w_4 == 0)
  {
    w_4 = 1;
    ASSERT(RememberToPutObjectInHand == -1,"objInHand");
    objD4 = d.objectInHand;
    goto tag012e14;
  };
  return 0;
}

// *********************************************************
//
// *********************************************************
void PressurePadOrAlcove (
         const RN objD6,
         const i32 mapX,
         const i32 mapY,
         const bool place,                     // As opposed to 'remove'
         const i16 unchangedRoom,              // object is in same room as before
         const RN object,
         const i16 switchPosition,             // -1 means party
         const bool objectDB5To13Present,
         const bool monstersPresent,           // Other than the object just moved.
         const bool sameTypeObjectPresent,
         const bool differentTypeObjectPresent,
         const i32 movedObjectDBNum)
{ // I broke this out of 13380 to make it more manageable.
  //                   party move
  //                   on    off
  // objD6            0fc7  0fc7
  // mapX               13    13
  // mapY               1b    1b
  // steppingOn          1     0
  // currentRoom         0     0
  // cellFlags          30    30
  // object           ffff  ffff
  // switchPosition   ffff  ffff
  // DBNum               3     3
  // LOCAL_6             0     0
  // monsters            0     0
  // LOCAL_12            0     0
  // LOCAL_14            0     0
  // movedObjDB         -1    -1
  dReg D0, D1;
  DB3  *DB3A3;
  OBJ_NAME_INDEX objNI_50;
  OBJ_NAME_INDEX objNI_40;
  OBJ_NAME_INDEX movedObjectNameIndex;
  i32 movedDBType;
  bool switchPressed, switchClosed;
  i16 ActuatorValue;//20;
  ACTUATORACTION padAction; //padAction_SET, _CLEAR, _TOGGLE, _???
  i16 tmrAction; //tmr_SET, _CLEAR, _TOGGLE
  DB3A3 = GetRecordAddressDB3(objD6);
  if (object != RNnul) movedDBType = object.dbType();
  else movedDBType = -1;
  D0W = DB3A3->actuatorType();//operated by
  ActuatorValue = DB3A3->value();
  switchPressed = place;  // As opposed to 'remove'.
  //Assume that if we are stepping on the switch we are pressing it.
  if (switchPosition == -1)
  { // Actuator in open room - like pressure pad
    switch(DB3A3->actuatorType())
    {
    case 0: return;
    case 1: //Pad operated by anything.
        if (unchangedRoom) return; // No change in location
        //Check if pad was already pressed by objects
        if (objectDB5To13Present) return; //no action
        if (object != RNnul)
        {
          if (movedDBType < 4) return;
          if (movedDBType > 13) return;
        };
        //Check if pad already pressed by monsters.
        if (monstersPresent) return; //no action
        break;
    case 2: //operated by party or monsters
        if (movedObjectDBNum > 4) return; //no action
        //D0W = D7W;
        if (unchangedRoom) return;//no action
        //Check if pad already pressed by monsters.
        if (monstersPresent) return;//no action
        break;
    case 3: //operated by party 
        if (movedObjectDBNum != -1) return;//no action
        if (d.NumCharacter == 0) return;//no action
        if (ActuatorValue == 0) //("PartyMovesOnOrOff")
        {
          //D0W = D7W;
          if (unchangedRoom) return;//no action
        }
        else
        {
          if (!place) // if ('remove')
          {
            switchPressed = false;
            // No particular facing direction is specified.
            // Since we are stepping off the switch we
            // say that the switch is not pressed.
          }
          else
          {
            //D4W = ActuatorValue;
            D1W = sw(d.partyFacing + 1);
            switchPressed = (D1W == ActuatorValue);
            //A particular direction is specified.
            //Switch is pressed only if we are facing the proper
            //direction.
          };
        };
        break;

    case 4:
        movedObjectNameIndex = GetBasicObjectType(object);
        if (ActuatorValue != movedObjectNameIndex) return;//no action
        //D0W = LOCAL_12;
        //Check if pad was already pressed by such an object.
        if (sameTypeObjectPresent) return;//no action
        break;
    case 5:
        return;//no action
    case 6:
        return;//no action
    case 7: //operated by Monsters
        if (movedObjectDBNum > 4) return;//no action
        if (movedObjectDBNum == -1) return;//no action
        //Check if pad already pressed by monsters.
        if (monstersPresent) return;//no action
        break;
    case 8:
        if (movedObjectDBNum != -1) return;//no action
        switchPressed = DoesPartyHave((OBJ_NAME_INDEX)ActuatorValue);
        break;
    case 9:
        if (movedObjectDBNum != -1) return;//no action
        if (!place) return;// if object is being removed then no action
        if (unchangedRoom) return;//no action
        switchPressed = (ActuatorValue <= 20);
        break;
    default: return;//no action
    }; // switch
  }
  else
  { // Actuator in Stone wall.  Like lever.  Position of
    // the actuator is important.
    if ((unsigned)switchPosition != objD6.pos()) return;//no action
    switch (D0W = DB3A3->actuatorType())
    {
    case 1:
      //Check if pad already pressed by objects.
      if (objectDB5To13Present) return;//no action
      break;
    case 2:
      //Check if pad already pressed by such an object.
      if (sameTypeObjectPresent) return;//no action
      objNI_40 = GetBasicObjectType(object);
      D0W = DB3A3->value();

      if (D0W != objNI_40) return;//no action
      break;
    case 3: //next object in room;
      //Check for pad pressed by some other object
      if (differentTypeObjectPresent) return;//no action
      objNI_50 = GetBasicObjectType(object);
      D0W = DB3A3->value();
      if (D0W == objNI_50) return;  //no action
      break;
    default: return;
    }; //switch ()
  };
  switchClosed = switchPressed ^ DB3A3->normallyClosed();
  // Toggle if Normally closed.
  // Result=1 means perform action.
  padAction = DB3A3->action(); //pacAction_SET,_CLEAR,_TOGGLE
  if (padAction == actuatorAction_CONSTPRESSURE)
  {
    if (switchClosed)
    {
      padAction = actuatorAction_SET;
    }
    else
    {
      padAction = actuatorAction_CLEAR;
    };
  }
  else
  {
    if (!switchClosed) return;//no action
  };
  if (DB3A3->audible())
  {
    QueueSound(soundPRESSUREPAD, mapX, mapY, 1);
  };
  tmrAction = (I16)padAction; // easy translation
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile), "Object %04x stepped %s pressure pad %04x ",
                         object.ConvertToInteger(),
                         place?"On":"Off",
                         objD6.ConvertToInteger());
    fprintf(GETFILE(TraceFile),"%02x(%02x,%02x)\n", d.LoadedLevel,mapX,mapY);
  };
  QueueSwitchAction(DB3A3, tmrAction, mapX, mapY, -1);
}

// *********************************************************
//
// *********************************************************
//   TAG013380
void PlaceOrRemoveObjectInRoom(
                       i32 mapX,
                       i32 mapY,
                       RN object,          // RNnul refers to the party
                       bool unchangedRoom, // object is in same room as before
                       bool place,         // place object rather than remove
                       MMRECORD *pmmr)
{ // currentRoom is 1 if object being placed in same room as party
  dReg D0;
  OBJ_NAME_INDEX objNID0;
  RN objD6;
  i16 cellFlags;
  bool differentTypeObjectPresent;
  bool sameTypeObjectPresent;
  OBJ_NAME_INDEX movedObjectNameIndex;
  bool monstersPresent;
  bool objectDB5To13Present;
  i32 DBType;
  i32 movedObjectDBType;
  i16 positionOfObject; // contains position if in stone else -1
  //D7W = currentRoom;
  if (object != RNnul)
  {
    movedObjectDBType = object.dbType();
    movedObjectNameIndex = GetBasicObjectType(object);
          // Gets word0 of weapon table associated with object
  }
  else
  {
    movedObjectNameIndex = objNI_NotAnObject;
    movedObjectDBType = -1;
  };
  if (!place)  // if not 'place' then 'remove'
  {
    if (movedObjectDBType != -1)
    {
      RemoveObjectFromRoom(object, mapX, mapY, pmmr);
    };
  };
  cellFlags = d.LevelCellFlags[mapX][mapY];
  //cellFlags = pdA0[mapY];
  if (BITS5_15(cellFlags) == roomSTONE)
  {
    if (object == RNnul) positionOfObject = 0;
    else positionOfObject = object.pos();
  }
  else
  {
    positionOfObject = -1;
  };
  objectDB5To13Present = false;
  monstersPresent = false;
  sameTypeObjectPresent = false;
  differentTypeObjectPresent = false;
  ASSERT(FindFirstObject(mapX, mapY) != RNnul,"RNnul");
  objD6 = FindFirstObject(mapX, mapY);
  if (positionOfObject == -1)
  { // Not in stone

    while (objD6 != RNeof)
    {
      DBType = objD6.dbType();
      if (DBType == dbMONSTER)
      {
        monstersPresent = true;
      }
      else
      {
        if (   (DBType == dbTEXT)
            && (movedObjectDBType == -1)
            && (place)
            && (!unchangedRoom) )
        {
          char text[1000];
          // The original Byte12914 destroyed d.Pointer12786
          // in the case of the "THERE IS ONLY ONE LEVEL....."
          // DecodeText(d.Byte12914, objD6, 1);
          //text[0] = 0x0a;
          if (DecodeText(text, GetRecordAddressDB2(objD6), 1, 990)
                 != -1)
          {
            QuePrintLines(15, text);
          };
          //PrintLines(15, d.Byte12914);
        }
        else
        {
          if (   (DBType > dbMONSTER)
              && (DBType < dbMISSILE) )
          {
            objectDB5To13Present = true;
            objNID0 = GetBasicObjectType(objD6);
            SET(D0B, movedObjectNameIndex == objNID0);
            sameTypeObjectPresent |= (D0W & 1);
            objNID0 = GetBasicObjectType(objD6);
            SET(D0B, movedObjectNameIndex != objNID0);
            differentTypeObjectPresent |= (D0W & 1);
          };
        };
      };
      ASSERT(GetDBRecordLink(objD6) != RNnul,"RNnul");
      objD6 = GetDBRecordLink(objD6);

    }; // while (D6W);
  }
  else
  { // in stone cell
    while (objD6 != RNeof)
    {
      if (   ((i32)objD6.pos() == positionOfObject)
          && (objD6.dbType() > dbMONSTER) )
      {
        objectDB5To13Present = true;
        objNID0 = GetBasicObjectType(objD6);
        SET(D0B, objNID0 == movedObjectNameIndex);
        sameTypeObjectPresent |= (D0W&1);
        objNID0 = GetBasicObjectType(objD6);
        if (objNID0 != movedObjectNameIndex)
             differentTypeObjectPresent |= 1;
      };
      objD6 = GetDBRecordLink(objD6);

    }; // while (D6W)
  };
  if (   (place)  // as opposed to 'remove'
      && (movedObjectDBType != -1) ) //database number
  {
    AddObjectToRoom(object, RN(RNempty), mapX, mapY, pmmr);
  };

  for ( objD6 = FindFirstObject(mapX, mapY);
        objD6 != RNeof;
        objD6 = GetDBRecordLink(objD6))
  {
    DBType = objD6.dbType();
    if (DBType == dbACTUATOR) // pressure pad?
    {
      PressurePadOrAlcove
                 (objD6,
                  mapX,
                  mapY,
                  place,           // true='place' ;  false='remove'
                  unchangedRoom,
                  object,
                  positionOfObject,
                  objectDB5To13Present,
                  monstersPresent,
                  sameTypeObjectPresent,
                  differentTypeObjectPresent,
                  movedObjectDBType);
    }
    else
    { //not database 3
      //if (DBType >= dbMONSTER) break;
    };

  }; // while (D6W)
  RotateActuatorList();
}

// *********************************************************
//
// *********************************************************
//  TAG01474e
i32 CharacterAtPosition(i32 pos)
{
//  dReg D6, D7;
//  CHARDESC *pcA3;
//  D7W = sw(P1);
//  for (D6W=0, pcA3=d.CH16482; D6W<d.NumCharacter; D6W++, pcA3++)
//  {
//    if ((D7W==pcA3->position)&&(pcA3->HP()))
//    {
//      return D6W;
//    };
//  };
//  return -1;
  i32 i;
  for (i=0; i<d.NumCharacter; i++)
  {
    if (d.CH16482[i].charPosition == pos)
    {
      if (d.CH16482[i].HP() != 0) return i;
      //return -1;
    };
  };
  return -1;
}

// *********************************************************
//
// *********************************************************
//   TAG01470c
void SetPartyFacing(i32 newDirection)
{//()
  dReg D0;
  CHARDESC *pcA0;
  i32 deltaFacing;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D3W = newDirection;
  //D2W = d.partyFacing;
  deltaFacing = newDirection - d.partyFacing;
  d.partyFacing = sw(newDirection);
  if (deltaFacing != 0) // If direction changed.
  {
    deltaFacing += 4;
    pcA0 = &d.CH16482[0];
    D0W = d.NumCharacter;
    while (D0W > 0)
    {
      pcA0->charPosition = (UI8)((pcA0->charPosition + deltaFacing) & 3);
      pcA0->facing = (UI8)((pcA0->facing + deltaFacing) & 3);
      pcA0 ++;
      D0W--;
    };
    DrawEightHands(); // Maybe a compass?
  };
}

// *********************************************************
//
// *********************************************************
//  TAG015492
bool DrawModifiedObjectAtLocation(i32 ScreenLocation,RN object)
{ // ScreenLocation = 0 to 7 for the portraits
  // and 8 to 37 for clothes and backpack.
  // Return true if new object is drawn.
  dReg D7;
  bool result;
  OBJ_NAME_INDEX objNID6;
  OBJ_NAME_INDEX objNID5;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(ScreenLocation);
  objNID6 = ObjectAtLocation(ScreenLocation);

//
//
  if (   (   (objNID6 >= objNI_FirstModifiableObject)
           &&(objNID6 <= objNI_LastModifiableObject))
      || (   (objNID6 >= objNI_FirstFullFlask)
           &&(objNID6 <= objNI_LastFullFlask))
      || (    objNID6 == objNI_EmptyFlask) )
  {
  objNID5 = object.NameIndex();
    if (objNID5 == objNID6) return false; // not modified
    if (D7W < 8)
    {
      if (d.Word16608 == 0)
      {
        d.Word16608 = 1;
        STHideCursor(HC10);
      };
    };
    DrawItem(D7W, objNID5);
    result = true; // object was modified
  }
  else
  {
    result = false;
  };
  return result;
}

// *********************************************************
//
// *********************************************************
//   TAG015508
void DrawEightHands()
{//()
  dReg D0, D4, D6, D7;
  OBJ_NAME_INDEX objNID5;
  bool modifiedD5;
  CHARDESC *pcA0, *pcA2;
  RN *rnA3;
  i16  w_4;
  OBJ_NAME_INDEX objNI_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D6W = d.SelectedCharacterOrdinal;
//
  if ( (d.PotentialCharacterOrdinal != 0) && (D6W == 0) ) return;

  d.Word16608 = 0;
  objNID5 = (OBJ_NAME_INDEX)d.ObjectType16574;
  if (objNID5 > objNI_LastModifiableObject) goto tag01553c;
  //otD5 = (OBJECTTYPE)d.Word16574;
  if (objNID5 >= objNI_FirstModifiableObject) goto tag015560;
tag01553c:
  objNID5 = (OBJ_NAME_INDEX)d.ObjectType16574;
  if (objNID5 < objNI_FirstFullFlask) goto tag015554;
  //D5W = d.Word16574;
  if (objNID5 <= objNI_LastFullFlask) goto tag015560;
tag015554:
  objNID5 = (OBJ_NAME_INDEX)d.ObjectType16574;
  if (objNID5 != objNI_EmptyFlask) goto tag0155a8;
tag015560:
  ASSERT(RememberToPutObjectInHand == -1,"objInHand");
  objNI_2 = d.objectInHand.NameIndex();
  if (objNI_2 != objNID5)
  {
    d.Word16608 = 1;
    STHideCursor(HC10);
    GetIconGraphic(objNI_2, (ui8 *)d.Pointer16572);
    CreateObjectCursor(d.Pointer16572);
    d.ObjectType16574 = (I16)objNI_2;
    ASSERT(RememberToPutObjectInHand == -1,"objInHand");
    DrawNameOfHeldObject(d.objectInHand);
  };
tag0155a8:
  for (D7W=0; D7W < d.NumCharacter*2; D7W++)
  { // Draw the 8 hands in portraits at top of screen
    D4W = sw(D7W >> 1);
    if (D6W == D4W+1) continue;
    D0W = (I16)(D7W&1); // One hand, then the other.
    pcA0 = &d.CH16482[D4W];
    D0W = DrawModifiedObjectAtLocation(D7W, pcA0->Possession(D0W));
    if (D0W == 0) continue;
    if (D7W & 1) continue; // if (weapon hand)
    DrawAttackButton(D4W); // D4 is chIdx
//

  };
  if (D6W != 0) // Selected Character Ordinal
  {
    pcA2 = &d.CH16482[D6W-1];
//    for  (D7W=0, rnA3=&pcA2->possessions[0], modifiedD5=false;
    for  (D7W=0, modifiedD5=false;
          D7W < 30;
//          D7W++, rnA3++)
          D7W++)
    {
//      w_4 = DrawModifiedObjectAtLocation(D7W+8, *rnA3);
      w_4 = DrawModifiedObjectAtLocation(D7W+8, pcA2->Possession(D7W));
      modifiedD5 = modifiedD5 || w_4;
      if (w_4 != 0)
      {
        if (D7W == 1)
        {
          DrawAttackButton(D6W-1);
        };
      };

    };
    if (d.DisplayResurrectChestOrScroll == 4) // If displaying chest contents
    {
      for (D7W=0, rnA3 = d.rnChestContents;
           D7W<8;
           D7W++, rnA3++)
      {
        D0W = DrawModifiedObjectAtLocation(D7W + 38, *rnA3);
        modifiedD5 = modifiedD5 || D0W;

      };
    };
    if (modifiedD5) // if some possession was changed
    {
      pcA2->charFlags |= CHARFLAG_viewportChanged;
      DrawCharacterState(D6W-1);
    };
  };
  if (d.Word16608 != 0)
  {
    STShowCursor(HC10);
    d.Word16608 = 0;
  };

}

// *********************************************************
//
// *********************************************************
//  TAG0163c8
i16 StaminaAdjustedAttribute(CHARDESC *pChar, i16 baseValue)
{
  dReg D0, D1, D2, D6, D7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = pChar->Stamina();
  D6W = sw(pChar->MaxStamina()/2);
  if (D7W < D6W) // If below half stamina
  {
    D0W = sw(baseValue/2);
    baseValue = D0W;
    D1L = baseValue;
    D2L = D7W;
    D1L = D1L * D2L;
    D1L = D1L / D6W;
    D0W = sw(D0W + D1W);
  }
  else
  {
    D0W = baseValue;
  }
  return D0W;
}

// *********************************************************
//
// *********************************************************
//  TAG016508
i32 MaxLoad(CHARDESC *pChar)
{
  dReg D0, D1, D6;
  i32 maxLoad;
  OBJ_NAME_INDEX objNID0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  maxLoad = sw(8 * pChar->Attributes[Strength].Current() + 100);
  maxLoad = StaminaAdjustedAttribute(pChar, sw(maxLoad));
  D6W = pChar->ouches;
  if (D6W)
  {
    D0L = maxLoad;
    D1W = (I16)(D6W & 16);
    if (D1W)
    {
      D1W = 2;
    }
    else
    {
      D1W = 3;
    };
    maxLoad = sw(maxLoad - (D0L >> D1W));
  };
  objNID0 = pChar->Possession(5).NameIndex(); // feet??
  if (objNID0 == objNI_ElvenBoots)
  {
    maxLoad = sw(maxLoad + (maxLoad >> 4));
  };
  maxLoad += 9;
  D0W = sw((maxLoad & 0xffff)%10);
  maxLoad = sw(maxLoad - D0W); // Round down to multiple of 10;
  return maxLoad;
}

// *********************************************************
//
// *********************************************************
//   TAG016a76
void DeleteFootprintEntry(i16 P1)
{
  dReg D6, D7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = P1;
  d.NumFootprintEnt--;
  D6W = d.NumFootprintEnt;
  D6W = sw(D6W - D7W);
  if (D6W) // Number we need to move
  {
    MemMove((ui8 *)&d.PartyFootprints[D7W+1],(ui8 *)&d.PartyFootprints[D7W],2*D6W);
    MemMove((ui8 *)&d.Byte13220[D7W+1],(ui8 *)&d.Byte13220[D7W],D6W);
  };
  if (D7W < (UI8)(d.IndexOfFirstMagicFootprint))
  {
    d.IndexOfFirstMagicFootprint--;
  };
  if (D7W < (UI8)(d.IndexOfLastMagicFootprint))
  {
    d.IndexOfLastMagicFootprint--;
  };
}

// *********************************************************
//
// *********************************************************
//   TAG016b10
void AddFootprintEntry(i32 /*newX*/,i32 /*newY*/, ui16 deltaTime)
{ //I think this is much hogwash.  Does nothing useful.
  //Is full of errors.  Perhaps an old, abandoned
  //debug trace or something.
  dReg D4, D5;
  i32 index;
  index = d.NumFootprintEnt;
  if (index == 0) return;
  D5W = (UI16)(deltaTime & 0x8000);
  deltaTime &= 0x7fff;
  D4W = 0;
  while (index-- != 1)
  {
    if (D4W == 0)
    {
      D4W = 1;
      if (D5W) // If deltaTime was negative
      {
        deltaTime = uw(Larger((UI8)(d.Byte13220[index]), deltaTime));
      }
      else
      {
        deltaTime = sw(Smaller(80, deltaTime+(UI8)(d.Byte13220[index])));
      };
    };
    d.Byte13220[index] = (i8)deltaTime;
  };
}

// *********************************************************
//
// *********************************************************
//  TAG0173ae
i16 DamageAllCharacters(i32 damage, i16 mask, i16 P3)
{
  dReg D0, D1, D4, D5, D6, D7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),
           "DamageAllCharacters basic damage = %d\n",damage);
  };
  //D7W = damage;
  if (damage == 0) return 0;
  D5W = sw(damage/8 + 1);
  D7W = sw(damage - D5W);
  D5W = sw(2*D5W);
  for (D4W=D6W=0; D6W<d.NumCharacter; D6W++)
  {
    D1L = STRandom() & 0xffff;
    D1W = sw(D1L % D5W);
    D0W = sw(DamageCharacter(D6W,
                              Larger(1, D7W+D1W),
                              mask,
                              P3));
    if (D0W != 0)
    {
      D4W++;
    };
//
  };
  return D4W;
}

// *********************************************************
//
// *********************************************************
//   TAG01742a
void AdjustStamina(i32 chIdx, i32 decrement)
{
  i32 absStamina;
  i32 newStamina;
  CHARDESC *pcA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (chIdx == -1) return;
  pcA3 = &d.CH16482[chIdx];
  newStamina = pcA3->Stamina() - decrement;
  //pcA3->Stamina(pcA3->Stamina() - decrement);
  //D7W = pcA3->stamina;
  if (newStamina <= 0)
  {
    absStamina = -newStamina;
    pcA3->Stamina(0);
    DamageCharacter(chIdx, absStamina/2, 0, 0);
  }
  else
  {
    if (newStamina > pcA3->MaxStamina())
    {
      pcA3->Stamina(pcA3->MaxStamina());
    }
    else
    {
      pcA3->Stamina(newStamina);
    };
  };
  pcA3->charFlags |= CHARFLAG_possession | CHARFLAG_statsChanged;
}
