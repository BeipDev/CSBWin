#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

extern ui8         monsterMoveInhibit[4];
extern bool neophyteSkills;

const char *MonsterName(MONSTERTYPE mt);
bool IsPlayFileOpen(void);

const char *TimerName(TIMERTYPE tt)
{
  const char *timeFuncName;
  char name[20];
  switch (tt)
  {
  case TT_M1:             timeFuncName = "TT_M1";             break;
  case TT_M2:             timeFuncName = "TT_M2";             break;
  case TT_31:             timeFuncName = "TT_31";             break;
  case TT_MONSTER_Agroup: timeFuncName = "TT_MONSTER_Agrpup"; break;
  case TT_MONSTER_A0:     timeFuncName = "TT_MONSTER_A0";     break;
  case TT_MONSTER_A1:     timeFuncName = "TT_MONSTER_A1";     break;
  case TT_MONSTER_A2:     timeFuncName = "TT_MONSTER_A2";     break;
  case TT_MONSTER_A3:     timeFuncName = "TT_MONSTER_A3";     break;
  case TT_MONSTER_Bgroup: timeFuncName = "TT_MONSTER_Bgroup"; break;
  case TT_MONSTER_B0:     timeFuncName = "TT_MONSTER_B0";     break;
  case TT_MONSTER_B1:     timeFuncName = "TT_MONSTER_B1";     break;
  case TT_MONSTER_B2:     timeFuncName = "TT_MONSTER_B2";     break;
  case TT_MONSTER_B3:     timeFuncName = "TT_MONSTER_B3";     break;
  default:
    sprintf(name, "%d", tt);
    timeFuncName = name;
  };
  return timeFuncName;
}



#ifdef _DEBUG
i32 timertrace[1000];
i32 timertraceindex=0;
void settrace(i32 n)
{
  if (n==-1)
  {
    while (timertraceindex>0) timertrace[--timertraceindex]=0;
  }
  else if (n==-2)
  {
    if (TimerTraceActive)
    {
      i32 i;
      for (i=0; i<timertraceindex; i++)
      {
        fprintf (GETFILE(TraceFile), "%4d %08x\n",i, timertrace[i]);
      };
    };
  }
  else if (timertraceindex>999)
  {
    return;
  }
  else
  {
    timertrace[timertraceindex++] = n;
  };
}
#endif

#ifdef _DEBUG
#define TIMERTRACE(n) settrace(n);
#else
#define TIMERTRACE(n)
#endif

void EnsureItem16Available(void)
{
  if (   (d.ITEM16QueLen >= d.MaxITEM16-5)
      && (d.LoadedLevel == d.partyLevel)  )
  {
    if (d.MaxITEM16 < 240)
    {
      i32 i, newMax;
      newMax = d.MaxITEM16 + 50;
      d.Item16 = (ITEM16 *)UI_realloc(d.Item16, 16 * newMax,
                                      MALLOC087);
      for (i=d.MaxITEM16; i< newMax; i++)
      {
        memset(&d.Item16[i], 0, sizeof(ITEM16));
        d.Item16[i].word0 = -1;
      };
      d.MaxITEM16 = sw(newMax);
    }
  };
}




//*********************************************************
//
//*********************************************************
// TAG00b1f0
RN CreateMonster(i32  value,   //8
                 i32  hitPointMultiplier, //10
                 i32  numMonM1,//12 Number Monsters-1??
                 i32  facing,  //14 P4
                 i32  mapX,    //16
                 i32  mapY,
                 bool invisible,
                 bool drawAsSize4)    //18
{// (RN)
  dReg         D0, D3, D4, D5, D6;
  RN           objD7;
  MONSTERDESC *pmtDesc;
  DB4         *DB4A3;
  i16          w_12;
  i16          w_2;
  i32          monsterSize;
  i32          alternateGraphic;
  MONSTERTYPE  mt;
  static bool  warningIssued = false;
  mt = (MONSTERTYPE)(value & 0x1f);
  alternateGraphic = (value >> 5) & 3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//

  if (!MonsterAllowedOnLevel(RNnul, d.LoadedLevel, mt)) return(RNnul);



  EnsureItem16Available();





  if (   (d.ITEM16QueLen >= d.MaxITEM16-5)
      && (d.LoadedLevel == d.partyLevel)  ) return (RNnul);
  objD7=FindEmptyDB4Entry(false);
  if (objD7 == RNnul) return (RNnul);
  DB4A3 = GetRecordAddressDB4(objD7);
  DB4A3->monsterType(mt); // Item26 index
  DB4A3->alternateGraphics(alternateGraphic);
  pmtDesc = &d.MonsterDescriptor[DB4A3->monsterType()];
  monsterSize = pmtDesc->horizontalSize();
  switch (monsterSize)
  {
  case 0: //Any number of monsters is OK.
    break;
  case 1: //size two.....worms perhaps?
    if (numMonM1 > 1)
    {
      if (!warningIssued && !IsPlayFileOpen())
        UI_MessageBox("Too many monsters of size 2","Fixing...",MESSAGE_OK);
      warningIssued = true;
      numMonM1 = 1;
    };
    break;
  case 2:
    if (numMonM1 > 0)
    {
      if (!warningIssued && !IsPlayFileOpen())
        UI_MessageBox("Too many monsters of size 4","Fixing...",MESSAGE_OK);
      warningIssued = true;
      numMonM1 = 0;
    };
    break;
  default:
    break;
  };
  DB4A3->possession(RNeof);
  D3W = 0;
  //30OCT02SETWBITS10_10(DB4A3->word14, uw(0));
  DB4A3->important(0);
  //30OCT02SETWBITS8_9(DB4A3->word14, P4);
  DB4A3->facing(facing);
  //30OCT02SETWBITS5_6(DB4A3->word14, numMonM1);
  DB4A3->numMonM1(numMonM1);
  DB4A3->invisible(invisible);
  DB4A3->drawAsSize4(drawAsSize4);
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
  ASSERT(DB4A3->monsterType() < 27,"monType");
  D6W = pmtDesc->baseHealth09; // uByte8[1];
  do
  {
    w_12 = sw(STRandom() % (D6W/4 + 1));
    D0W = uw(D6W * hitPointMultiplier + w_12);
    DB4A3->hitPoints6[numMonM1] = D0W;
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
  DB4A3->groupIndex(D4W);
  // The following function call fixes DB4A3->uByte5.
  D0W = MoveObject(objD7, -1, 0, mapX, mapY, NULL, NULL);
  ASSERT(D0W != 0xccc,"D0W");
  if (D0W != 0)
  {
    return (RNnul);
  }
  else
  {
    QueueSound(soundTELEPORT, mapX, mapY, 1);
    return (objD7);
  };
}

//*********************************************************
//
//*********************************************************
//   TAG00bc12
void StealFromCharacter(DB4 *pDB4, i32 chIdx)
{//(void)
  dReg D0, D1, D5, D6, D7;
  RN objD4;
  CHARDESC *pcA2;
  i16      w_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//
  w_2 = 0;
  pcA2 = &d.CH16482[chIdx];
  D1W = sw(Quickness(pcA2));
  D7W = sw(100 - D1W);
  D5W = sw(STRandom(8));
  while ( (D7W>0) && !IsCharacterLucky(pcA2,D7W) )
  {
    D6W = d.uByte13126[D5W];
    if (D6W == 13)
    {
      D6W = sw(D6W + STRandom(17));
    };
    objD4 = pcA2->Possession(D6W);
    if (objD4 != RNnul)
    {
      w_2 = 1;
      objD4 = RemoveCharacterPossession(chIdx, D6W);
      if (pDB4->possession() == RNeof)
      {
        pDB4->possession(objD4);
      }
      else
      {
        AddObjectToRoom(objD4, pDB4->possession(), -1, 0, NULL);
      };
      DrawCharacterState(chIdx);
    };
    D5W = sw((D5W + 1) & 7);
    D7W -= 20;

  };
  D0W = sw(STRandom(8));
//
  if ( (STRandom(8)==0) || ((w_2!=0) && (STRandomBool()!=0)) )
  {
    D0W = sw(STRandom(64) + 20);
    ASSERT(pDB4->groupIndex() < d.MaxITEM16,"maxitem16");
    d.Item16[pDB4->groupIndex()].uByte5 = D0B;
    //30OCT02SETWBITS0_3(pDB4->word14, 5);
    pDB4->fear(StateOfFear5);
  };
}

//*********************************************************
//
//*********************************************************
//   TAG00be5a
bool BlockedTypeA(i32 mapX, i32 mapY)
{ // return true if blocked to passage.
  // Certain monsters can go through certain doors.
  // Door types 3 & 4 are considered blocked as are
  // certain special doors on this level.
//  dReg D0;
  ui32 doorPosition;
  CELLFLAG cfD7;
  ROOMTYPE rtD6;
  DB0 *pDoor;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  cfD7 = d.LevelCellFlags[mapX][mapY];
  rtD6 = (ROOMTYPE)(cfD7>>5);
  if (rtD6 == roomDOOR)
  {
    pDoor = FirstObjectRecordAddressDB0(mapX, mapY); //Door
    doorPosition = (I16)(cfD7 & 7);
    if ( (doorPosition == 3) || (doorPosition == 4) ) //closed or almost closed
    {
    //SET(D0W, D5W==3); //Almost closed
    //if (D0W == 0)
    //{
    //  SET(D0W, D5W==4); //Closed
    //};
    //if (D0W != 0)
      //D0W = d.DoorTOC[pDoor->doorType()];//one of two types on level
      //SET(D0W, (D0W & 0x100) == 0);// The high order byte
      if (neophyteSkills)
      {
        return (d.DoorTOC[pDoor->doorType()] & 0x100) == 0; //Blocked if not special
      }
      else
      {
        return (d.DoorTOC[pDoor->doorType()] & 0x100) == 0; //Blocked if not special
      };
    };
    //D0W = (I16)(D0W & 1);
    return false;
  };
//    SET(D0W, rtD6==roomSTONE);
//    if (D0W == 0)
//    {
//      SET(D0W, rtD6 == roomFALSEWALL);
//      if (D0W != 0)
//      {
//        SET(D0W, (cfD7 & 4) == 0);
//      };
//    };
//  };
//  return D0W != 0;
  if (rtD6 == roomSTONE)     return true;
  if (rtD6 != roomFALSEWALL) return false;
  if (cfD7 & 4) return false;
  return true;
}

//*********************************************************
//
//*********************************************************
//  TAG00bf00
bool StoneOrClosedFalseWall(i32 mapX, i32 mapY)
{
  CELLFLAG cf;
  ROOMTYPE rt;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  cf = d.LevelCellFlags[mapX][mapY];
  rt = (ROOMTYPE)(cf>>5);
  if (rt == roomSTONE)     return true;
  if (rt != roomFALSEWALL) return false;
  if (cf & 4) return false;
  return true;
}

//*********************************************************
//Generally, we try to follow a straight line between
// the party and the monster by keeping the slope
// of the line as constant as possible.  If we run into
// a wall (as determined by the provided function) we
// say it is impossible and return a zero.
// Else we return the distance to target. Orthogonal crow.
//*********************************************************
i16 TestDirectPathOpen(i32 mapX,    //8
                       i32 mapY,    //10
                       i32 partyX,  //12
                       i32 partyY,  //14
                       bool (*function)(i32,i32))
{
  dReg D0, D1, D4, D5, D6, D7;
  i16  w_12;
  bool DIAG_10;
  i16  w_8;
  i16  w_6;
  bool NS_4;
  i16  w_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D0W = sw(abs(mapX-partyX));
  D1W = sw(abs(mapY-partyY));
  if (D0W+D1W  <= 1) return 1;
  D5W = sw(abs(partyX - mapX));
  D4W = sw(abs(partyY - mapY));
  NS_4 = D5W < D4W; // Set if north/south
  DIAG_10 = D5W == D4W; // // set if perfect diagonal
  D5W = sw(partyX);
  D7W =(I16)((partyX <= mapX)  ? 1 : -1); // Y increment party to monster
  D4W = sw(partyY);
  D6W = (I16)(partyY <= mapY ? 1 : -1); // Increment party to monster
  if (NS_4) // If North/south
  {
    w_2 = sw(D4W - mapY); // partyY-mapY
    if (w_2 != 0)
    {
      D0W = sw(64 * (D5W - mapX) / w_2);
    }
    else
    {
      D0W = 128;
    };
  }
  else
  {
    w_2 = sw(D5W - mapX);
    if (w_2 != 0)
    {
      D0W = sw(64 * (D4W - mapY)/w_2); //partyY-mapY
    }
    else
    {
      D0W = 128;
    };
  };
  w_12 = D0W;
  do
  {
    if (DIAG_10) //perfect diagonal
    {


      if (    (    ((*function)(sw(D5W+D7W), D4W) != 0)
                && ((*function)(D5W, sw(D4W+D6W)) != 0))
           || ((*function)(D5W=sw(D5W+D7W), D4W=sw(D4W+D6W))!=0) )
      {
        return 0;
      };
    }
    else
    {
      if (NS_4) //North/south
      {
        w_2 = sw(D4W-mapY); //
        if (w_2 != 0)
        {
          D0W = sw(64 * (D5W + D7W - mapX)/w_2);
        }
        else
        {
          D0W = 128;
        };
      }
      else
      {
        w_2 = sw(D5W + D7W - mapX); // one step in x-direction vs mapX
        if (w_2 != 0) // if not in adjacent column
        {
          D0W = sw(64 * (D4W - mapY)/w_2); // new slope to monster
        }
        else
        {
          D0W = 128;
        };
      };
      w_2 = sw(D0W - w_12); // new slope minus original slope
      w_6 = sw(abs(w_2));
      if (NS_4)
      {
        w_2 = sw(D4W + D6W - mapY);
        if (w_2 != 0)
        {
          D1W = sw(64 * (D5W - mapX)/w_2);
        }
        else
        {
          D1W = 128;
        };
      }
      else
      {
        w_2 = sw(D5W - mapX); //#columns to monster
        if (w_2 != 0)
        {
          D1W = sw(64 * (D4W + D6W - mapY)/w_2); // new slope if we take y-step
        }
        else
        {
          D1W = 128;
        };
      };
      w_2 = sw(D1W - w_12);//y-step slope vs original slope
      w_8 = sw(abs(w_2));//difference in slope if we take y-step
      if (w_6 < w_8)
      {
        D5W = sw(D5W + D7W); // Assume party moves in x-direction
      }
      else
      {
        D4W = sw(D4W + D6W);//Assume party takes the y-step
      };
      D0W = (*function)(D5W, D4W);
      if (D0W != 0) // non-zero if blocked
      {
        if (w_6 != w_8) return 0;
        D0W = (*function)(D5W=sw(D5W+D7W), D4W=sw(D4W-D6W));
        if (D0W != 0) return 0;
      };
    };
    D0W = sw(abs(D5W - mapX));
    D1W = sw(abs(D4W - mapY));
  } while (D1W + D0W > 1);
  return DistanceBetweenPoints(mapX, mapY, partyX, partyY);
}

//*********************************************************
//
//*********************************************************
//  TAG00c202
i16 Blocked(DB4 *pDB4, i32 individualMonsterIndex, i32 mapX, i32 mapY)
{
  dReg         D0, D4, D5, D6, D7;
  MONSTERDESC *pmtDesc;
  i16          w_18;
  i16          w_8[4];
  i32          i;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pmtDesc = &d.MonsterDescriptor[pDB4->monsterType()];
  if (   (d.Invisible != 0)
      && !pmtDesc->seesInvisible() ) return 0;
  if (pmtDesc->See360()) goto tag00c2fe;
  //ASSERT(pDB4->uByte5 < d.MaxITEM16);
  if (pDB4->groupIndex() < d.MaxITEM16)
  {
    D4W = d.Item16[pDB4->groupIndex()].facings();
  }
  else
  {
    D4W = 0x7ccc; //designed to cause error!
  };

  if (individualMonsterIndex < 0)
  {
    D6W = 0;
    for (i=pDB4->numMonM1(); i>=0; i--)
    {
//      D7W = sw((D4W >> (2*P2)) & 3); //Direction this monster facing?
      D7W = sw(TwoBitNibble(D4W,i)); //Direction this monster facing?
      D5W = D6W;
      while (D5W-- != 0)
      {
        D0W = w_8[D5W];
        if (D7W == D0W) break;
      };
      if (D5W == -1) // if we fell out bottom of loop.
      {
        w_8[D6W++] = D7W;
      };
      // for (P2) continue
//
    }; // while (P2 >= 0)
  }
  else
  {
//    w_8[0] = sw((D4W >> 2*P2) & 3);
    w_8[0] = sw(TwoBitNibble(D4W,individualMonsterIndex));
    D6W = 1;
  };
  while (D6W-- != 0)
  {
    D0W = IsItInThisDirection(
                   w_8[D6W],
                   mapX,
                   mapY,
                   d.partyX,
                   d.partyY);
    if (D0W == 0) continue;
tag00c2fe: //***
    D5W = pmtDesc->sightDistance();
    if (!pmtDesc->canSeeInDark())
    {
      D5W = sw(D5W - d.CurrentPalette/2);
    };
    w_18 = sw(Larger(1, D5W));
    if (d.OrthogonalDistance2Party > w_18) return 0; //Cannot see the party then zero.
    D0W = TestDirectPathOpen(
              mapX, mapY, d.partyX, d.partyY, BlockedTypeA);
    return D0W;
    //continue here
  }; // while (D6-- != 0)
  return 0;
}

//*********************************************************
//
//*********************************************************
//  TAG00c36c
i16 GetBestMonsterFacingP1(MONSTERDESC *pmtDesc, i32 mapX, i32 mapY)
{
  dReg D0, D6, D7;
  i16  w_10;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = pmtDesc->smellingDistance();
  if (D7W == 0) return 0;

  if (   ((D7W+1)/2 >= d.OrthogonalDistance2Party)
      && (TestDirectPathOpen(mapX,
                             mapY,
                             d.partyX,
                             d.partyY,
                             StoneOrClosedFalseWall) != 0) )
  {
    d.SecondaryDirection = d.SecondaryDirectionToParty;
    return sw(d.PrimaryDirectionToParty + 1);
  };
  D6W = SearchFootprints(mapX, mapY);
  if (D6W != 0)
  {
    w_10 = STRandom0_3();
    D0W = sw(w_10 + (UI8)(d.Byte13220[D6W-1]));
    if (D0W > 30 - 2*D7W)
    {
      return sw(GetPrimaryAndSecondaryDirection(
                     mapX,
                     mapY,
                     BITS0_4(d.PartyFootprints[D6W]),
                     BITS5_9(d.PartyFootprints[D6W]) ) + 1);
    };
  };
  return 0;
}

//*********************************************************
//
//*********************************************************
//   TAG00c442
bool PossibleMove(const MONSTERDESC *pmtDesc,   //8
                  i32 mapX,                     //12
                  i32 mapY,                     //14
                  i32 direction,                //16
                  i16 RFPAW) //RecognizesFalsePitsAndWalls
{ // This function expects d.Obj13020 to be the RN of
  // a monster.  After loading a new game, d.Obj13020 may
  // be zero.
  // And even during an extended game play, Obj13020 will
  // sometimes point to the wrong monster.
  // So what do we do about that?  To be compatible
  // with the initial Atari version, we should deal with the
  // zero.  To be correct, we should fix d.Obj13020 so that
  // it reflects the current monster.  So that is what I
  // have done.
  dReg           D0, D1, D4, D5, D6, D7;
  CELLFLAG      *pcfA0;
  DB0           *DB0A2;
  DB1           *DB1A2;
  DB15          *pDB15A2;
  RN             obj_2;
  D4L = 0xccccc;
  D5L = 0xccccc;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TIMERTRACE(0xc442);
  d.DirectionHasBeenTested[direction] = 1;
  d.Obj13040 = RNeof;
  d.Word13042 = 0;
  d.moveOntoPartyPosition = 0;
  if (pmtDesc->timePerMove() == 255) return 0;

  RelativeXY(direction, 1, 0, &mapX, &mapY);//One step forward
  D7W = sw(mapX);
  D6W = sw(mapY);
  D0W = 0;
  d.Word13038 = 1; // Assume move is impossible.
  if (D7W < 0)        return false;
  if (D7W >= d.width) return false;
  if (D6W < 0)        return false;
  if (D6W >= d.height)return false;
  if (monsterMoveInhibit[direction] != 0) return false;
  pcfA0 = d.LevelCellFlags[D7W]; // column pointer
  D5W = pcfA0[D6W]; // Get this room's CELLFLAG
  //D5W = D0W
  D4W = sw(D5W >> 5); // The type of room.
  if (D4W == roomSTONE) return false;
  if (D4W == roomSTAIRS)return false;
  if (D4W == roomPIT)
  {
    if ((D5W & 8) != 0) // if (pit is open)
    {
      if ( !pmtDesc->levitating()) //Levitate over open pit is ok.
      {
        if (!((D5W&1)?RFPAW:(D5W&1))) //Can monster walk on fake pit?
        {
          return false;
        };
      };
    };
  };
  if (D4W == roomFALSEWALL) 
  {
    if ((D5W&4) == 0)  // if (closed)
    {
      if (!((D5W&1)?RFPAW:(D5W&1))) // Can monster walk through false wall?
      {
        return false;
      };
    };
  };
  d.Word13038 = 0;

  if (pmtDesc->invincible())
  {
    for (obj_2 =  FindFirstObject(D7W, D6W);
         obj_2 != RNeof;
         obj_2 =  GetDBRecordLink(obj_2))
    {

      if (obj_2.dbType() != dbCLOUD) continue;
      pDB15A2 = GetRecordAddressDB15(obj_2);
      if (pDB15A2->cloudType() != CT_FLUXCAGE) continue;
      d.FluxCages[direction] = 1;
      d.FluxCageCount++;
      return 0;
//
//
    };
  };
  for (;;)
  {
    ASSERT(D4L != 0xccccc,"D4L");
    ASSERT(D5L != 0xccccc,"D5L");
    if (D4W != roomTELEPORTER) break;
    if ((D5W & 8)==0) break;//break if teleporter inactive
    if (pmtDesc->word16_12_15() < 10) break;
    DB1A2 = FirstObjectRecordAddressDB1(D7W, D6W);
    if (DB1A2->what() == 0) break;//if not monsters
	// See comment at top of this function about d.Obj13020.  It may
	// be zero and it may be wrong.  I have patched the Atari code
  // ProcessTimer29To41 to always put the current monster
  // in Obj13020.  This seemed the 'right' thing to do.
    if (MonsterAllowedOnLevel(d.LastMonsterMoved, DB1A2->destLevel()))
            break;
    d.Word13038 = 1;
    return 0;
  };
  d.moveOntoPartyPosition =    (d.LoadedLevel == d.partyLevel)
                            && (D7W == d.partyX)
                            && (D6W == d.partyY);
  if (d.moveOntoPartyPosition) return 0;
  for (;;)
  {
    if (D4W != roomDOOR) break;//
    DB0A2 = FirstObjectRecordAddressDB0(D7W, D6W);
    D0W = (I16)(D5W & 7);
    D1W = BITS5_5(DB0A2->mode()); //Up-down or sideways
    if (D1W != 0)
    {
      D1W = pmtDesc->verticalSize();
    }
    else
    {
      D1W = 1;
    };
    if (D0W <= D1W) break;
    if ((D5W & 7) == 5) break;
    if (pmtDesc->nonMaterial()) break;
    d.Word13042 = 1;
    return 0;
  };
  d.Obj13040 = FindFirstMonster(D7W, D6W);
  return d.Obj13040 == RNeof;
  ;
}

//*********************************************************
//
//*********************************************************
//  TAG00c70c
i16 FindPossibleMove(const MONSTERDESC *pmtDesc,i32 x,i32 y,i16 P4)
{
  dReg D0;
  i32 direction;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  for (direction = 0; direction < 4; direction++)
  {
    if (d.DirectionHasBeenTested[direction] != 0) continue;
    D0W = PossibleMove(pmtDesc, x, y, direction, P4);
    if (D0W != 0) return sw(direction + 1);
//
//
  };
  return 0;
}

void CallMonsterMovedFilter(void)
{
}

//*********************************************************
//
//*********************************************************
//  TAG00c756
i16 PossibleDoubleMove(const MONSTERDESC *pmtDesc,  //8
              i32 mapX,              //12
              i32 mapY,              //14
              i32 direction)         //16
{
  dReg D7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(direction);
  ASSERT(D7W < 4,"D7W");
  if (d.FluxCages[D7W] != 0)
  {
    return 0;
  };
  mapX = sw(mapX + d.DeltaX[D7W]);
  mapY = sw(mapY + d.DeltaY[D7W]);
  return PossibleMove(pmtDesc, mapX, mapY, D7W, 0);
}

//*********************************************************
//
//*********************************************************
//   TAG00c7bc
void TurnMonster(ITEM16 *pI16,
                 i32 preferredDirection,
                 i32 monsterIndex,  // within group
                 bool P4)
{ //P4 means to treat the monsters a pairs.  For example,
  //worms, which occupy two corners, must turn together.
  dReg D0, D5;
  i32 newDir;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7W = preferredDirection;
  //D6W = monsterIndex;
  D5W = P4;
  if (P4)
  {
    if (   (d.Time == d.Time13130)
        && (pI16 == d.pI1613134) ) return;
  };
  D5W = pI16->facings(); // Old facing byte
//  D0W = sw(D5W >> (2*monsterIndex)); // Current facing
  D0W = sw(TwoBitNibble(D5W,monsterIndex)); // Current facing
  D0W = (I16)((D0W - preferredDirection) & 3); // relative to preference
  if (D0W == 2) // if facing away from party
  {
    newDir = ((STRandom() & 2) + preferredDirection + 1) & 3;
    CallMonsterMovedFilter();
    D5W = SetMonsterPositionBits(
             D5W,  //old facing byte
             monsterIndex,
             newDir); //
  }
  else
  {
    newDir = preferredDirection;
    CallMonsterMovedFilter();
    D5W = SetMonsterPositionBits(
               D5W,  //old directions byte
               monsterIndex,
               newDir); //new direction
  };
  if (P4)
  {
    D5W = SetMonsterPositionBits(
                     D5W,             //old directions byte
                     monsterIndex ^ 1, //monster number
                     newDir);          //facing direction
    d.Time13130 = d.Time;
    d.pI1613134 = pI16;
  };
  pI16->facings() = D5B;
}

//*********************************************************
//
//*********************************************************
//   TAG00c86c
void TurnMonsterGroup(ITEM16 *pI16,
                      i32 PreferredDirection,
                      i32 numMonster,
                      i32 P4)
{ // P4 means monsters should be treated as pairs.
  // This particularly applies to worms that occupy
  // two corners of the room.  They must turn together.
  i32 monsterIndex;
  bool bD6;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  monsterIndex = numMonster;
  bD6 = ( (monsterIndex!=0) && (P4!=0) );
  if (bD6) monsterIndex--;
  do
  {
//
    if ( (monsterIndex == 0) || (STRandomBool() != 0) )
    {

      TurnMonster(pI16,
                  PreferredDirection,
                  monsterIndex,
                  bD6);
    };
    monsterIndex--;
  } while (monsterIndex >= 0);
}


i32 RanLiveChar(void)
{ //Return charIndex of a random live character.
  i32 ch[4];  // A list of live characters
  i32 i, n;
  for (i=n=0; i<d.NumCharacter; i++)
  {
    if (d.CH16482[i].HP() > 0) ch[n++] = i;
  };
  if (n == 0) return -1;
  return ch[STRandom(n)];
}

struct ATTACK_PARAMETERES {
  ui32         monsterID;
  ui32         monsterType;
  ui32         monsterIndex;
  i32          monsterLevel;
  i32          monsterX;
  i32          monsterY;
  i32          monsterPos;
  i32          missileOriginPosition;
  i32          missileRange;
  i32          missileDamage;
  i32          missileDecayRate;
  i32          directionToParty;
  i32          distanceToParty;
  ui32         missileType;
  i32          monsterShouldLaunchMissile;
  i32          monsterShouldSteal;
  i32          heroToDamage;
  i32          attackSoundOrdinal;
  i32          disableTime;
  i32          supressPoison;
} attackParameters;

//*********************************************************
//
//*********************************************************
//   TAG00c8c0
bool MonsterAttacks(RN  monster,
                    DB4 *pDB4, //8
                    i32 mapX,  //12
                    i32 mapY,  //14
                    i16 monsterIndex)    //16 //MonsterIndex within group
{ //(i16)
  //Called to compute last parameter of call
  // to NextMonsterUpdateTime. (is attacking?)
  //Return false if (no live character or ???).
  //      else true;
  dReg         D4;
  i32          len;
  MONSTERTYPE  mtD7;
  ITEM16       item16_20;
  DB4         *DB4A3;
  MONSTERDESC *pmtDesc;
  CHARDESC    *pch_4;
  i32          positionsByte;
  i32          rightSideMonster;
  ui32         key, *pRecord;
  RN           missile;
  /*
  RN           missileType;
  bool         monsterShouldThrowMissile;
  bool         monsterShouldSteal;
  i32          directionToParty;
  i32          heroToDamage;
  i32          attackSoundOrdinal;
  i32          missileOriginX;
  i32          missileOriginY;
  i32          missileOriginPosition;
  i32          missileDirection;
  i32          missileRange;
  i32          missileDamage;
  */
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.NumCharacter == 0) return false;
  DB4A3 = pDB4;
  d.LastMonsterAttackTime = d.Time;
  ASSERT(DB4A3->groupIndex() < d.MaxITEM16,"maxitem16");
  memmove(&item16_20, &d.Item16[DB4A3->groupIndex()], 16);
  mtD7 = DB4A3->monsterType();
  attackParameters.supressPoison = -1;
  attackParameters.monsterType = mtD7;
  attackParameters.monsterIndex = monsterIndex;
  attackParameters.monsterLevel = d.LoadedLevel;
  if (AITraceActive)
  {
    fprintf(GETFILE(TraceFile),
      "Monster AI Entering TAG00c9c0 %d(%02d,%02d) %s \n",
           d.partyLevel,mapX,mapY,
           MonsterName(mtD7));
  };
  pmtDesc = &d.MonsterDescriptor[mtD7];

  attackParameters.monsterID = monster.ConvertToInteger();
//==================================== directionToParty
  attackParameters.directionToParty 
                = d.PrimaryDirectionToParty;

//==================================== distance to Party
  attackParameters.distanceToParty = d.OrthogonalDistance2Party;


//==================================== missileOriginLocation
  positionsByte = item16_20.positions();
  attackParameters.monsterPos = TwoBitNibble(positionsByte,monsterIndex);
  if (positionsByte == 255)
  {
    rightSideMonster = STRandomBool();
  }
  else
  {
//    D5W = sw(((((D5W >> (2*P4)) & 3) + 5 - D6W) & 2) / 2);
    rightSideMonster = 
        ((  attackParameters.monsterPos
          + 5 
          - attackParameters.directionToParty) & 2) / 2;
    // 1 if monster is on right side of group when facing party.
  };
  attackParameters.missileOriginPosition
       =   (I16)((rightSideMonster 
         + attackParameters.directionToParty) & 3);
  attackParameters.monsterX = mapX;
  attackParameters.monsterY = mapY;

//================================= missileDirection
  attackParameters.directionToParty =  d.PrimaryDirectionToParty;


//================================== monsterShouldLaunchMissile
  attackParameters.monsterShouldLaunchMissile =     
          (pmtDesc->word14_12_15() > 1) // ie: 3, 4, or 6
       && ((d.OrthogonalDistance2Party > 1) || (STRandomBool() != 0));


//================================== missileType  
  missile = RNnul;
  switch (mtD7)
  {
  case mon_Vexirk:
  case mon_LordChaos:
      if (STRandomBool() != 0)
      {
        missile = RNFireball;
      }
      else
      {
        switch (STRandom0_3())
        {
        case 0: missile = RNDispellMissile; break;
        case 1: missile = RNLightning; break;
        case 2: missile = RNPoisonCloud; break;
        case 3: missile = RNZoSpell; break;
        }; //switch()
      };
      break;
  case mon_SlimeDevil:
      missile = RNPoison; break;
  case mon_FlyingEye:
      if (STRandom(8) != 0) missile = RNLightning;
      else missile = RNZoSpell;
      break;
  case mon_Zytaz:
      if (STRandomBool() != 0) missile = RNPoisonCloud;
      else missile = RNFireball;
      break;
  case mon_Demon:
  case mon_Dragon:
      missile = RNFireball;
      break;
  }; // switch
  attackParameters.missileType = missile.ConvertToInteger();

//================================== monsterShouldSteal
  attackParameters.monsterShouldSteal 
      = (mtD7 == mon_Giggler);

//================================== heroToDamage
  if (pmtDesc->word2Bit4())
  {
    attackParameters.heroToDamage = STRandom0_3();
    for (D4W = 0;
         (D4W<4) && (d.CH16482[attackParameters.heroToDamage].HP()==0);
         attackParameters.heroToDamage = ((attackParameters.heroToDamage +1) & 3), D4W++)
         // D4W++) //For four years we didn't increment heroToDamage!
    { //look for random live character.
    };
    if (D4W == 4) return false;
  }
  else
  {
    attackParameters.heroToDamage
        = GetCharacterToDamage(
              attackParameters.monsterX, 
              attackParameters.monsterY, 
              attackParameters.missileOriginPosition);
    //if (attackParameters.heroToDamage < 0) return false;
  };

//========================================= attackSoundOrdinal
  attackParameters.attackSoundOrdinal
           = pmtDesc->attackSound;


//========================================= missileRange
    attackParameters.missileRange
      = sw(pmtDesc->attack10 / 4 + 1); // uByte8[4] / 4 + 1);
    attackParameters.missileRange
            += STRandom(attackParameters.missileRange);
    attackParameters.missileRange
            += STRandom(attackParameters.missileRange);


//========================================= missileDamage
    attackParameters.missileDamage = pmtDesc->dexterity12; // uByte8[4];

//========================================= missileFriction
    attackParameters.missileDecayRate = 8;


  key = (EDT_SpecialLocations<<24)|ESL_MONSTERATTACKFILTER;
  len = expool.Locate(key,&pRecord);
  if (len > 0)
  //*****************************************
  //Let the DSA (Monster Attack Filter) process the parameters
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
          NEWDSAPARAMETERS ndp;
          currentLevel = d.LoadedLevel;
          LoadLevel(locr.l);
          timer.timerUByte9(0);//timerFunction
          timer.timerUByte8(0);//timerPosition
          timer.timerUByte7((ui8)locr.y);
          timer.timerUByte6((ui8)locr.x);
          //timer.timerTime   = locr.l << 24;
          timer.Time(0);
          timer.Level((ui8)locr.l);

          memcpy(pDSAparameters+1, &attackParameters, sizeof(attackParameters));
          pDSAparameters[0] = sizeof(attackParameters)/sizeof(pDSAparameters[0]);
          ProcessDSAFilter(obj, &timer, locr, NULL, &dsaVars);
          memcpy(&attackParameters, pDSAparameters+1, sizeof(attackParameters));
          LoadLevel(currentLevel);
          continue;
        };
      };
    };
  };    

//===================================================

  if (attackParameters.monsterShouldLaunchMissile)
  {
    switch (attackParameters.missileType)
    {
    case RNFireball:
    case RNPoison:
    case RNLightning:
    case RNDispellMissile:
    case RNZoSpell:
    case RNPoisonCloud:
    case RNPoisonBolt:
      missile = (RNVAL)attackParameters.missileType;
      break;
    default: missile = RNnul; break;
    };
    if (missile != RNnul)
    {
      LaunchMissile(
                    missile,
                    attackParameters.monsterX,
                    attackParameters.monsterY,
                    attackParameters.missileOriginPosition,
                    attackParameters.directionToParty,
                    ApplyLimits(20, attackParameters.missileRange, 255),
                    attackParameters.missileDamage,
                    attackParameters.missileDecayRate,
                    false);
    };
  }
  else
  {
    switch (attackParameters.heroToDamage)
    {
    case 0:
    case 1:
    case 2:
    case 3:
      break;
    case 4:  // Party Leader
      attackParameters.heroToDamage = d.HandChar;
      break;
    case 5:  // Random party member
      attackParameters.heroToDamage = RanLiveChar();
      break;
    case 6:  // Closest party member
      attackParameters.heroToDamage
          = GetCharacterToDamage(
                attackParameters.monsterX, 
                attackParameters.monsterY, 
                attackParameters.missileOriginPosition);
      if (attackParameters.heroToDamage < 0) return false;
      break;
    default: return false;
    };
    if (attackParameters.monsterShouldSteal)
    {
      StealFromCharacter(DB4A3, attackParameters.heroToDamage);
    }
    else
    {
      D4W = sw(1 + MonsterDamagesCharacter(DB4A3, attackParameters.heroToDamage, attackParameters.supressPoison==1));
      pch_4 = &d.CH16482[attackParameters.heroToDamage];
      if (D4W > pch_4->maxRecentDamage)
      {
        pch_4->maxRecentDamage = (UI8)(D4W);
        pch_4->facing3 = (UI8)((attackParameters.directionToParty+2) & 3);
      };
    };
  };
  if (attackParameters.attackSoundOrdinal != 0)
  {
    QueueSound(d.uByte9672[attackParameters.attackSoundOrdinal-1],
               attackParameters.monsterX,
               attackParameters.monsterY,
               1);
  };
  return true;
}

//*********************************************************
//
//*********************************************************
//   TAG00cb68
void SetMonsterTimerB(TIMER *pTimer,const ui32 AltTime)
{
  TIMERTYPE tt;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //if (AltTime < (pTimer->timerTime & 0xffffff))
  if (AltTime < pTimer->Time())
  {
    switch (pTimer->Function())
    {
    case TT_MONSTER_Bgroup:tt = TT_MONSTER_Agroup; break;
    case TT_MONSTER_B0:    tt = TT_MONSTER_A0;     break;
    case TT_MONSTER_B1:    tt = TT_MONSTER_A1;     break;
    case TT_MONSTER_B2:    tt = TT_MONSTER_A2;     break;
    case TT_MONSTER_B3:    tt = TT_MONSTER_A3;     break;
    default: die(0xf5af, "Illegal Monster AI Timer");
             tt = TT_MONSTER_B0;
             break;
    };
    pTimer->Function(tt);
    //pTimer->timerUByte8((UI8)((pTimer->timerTime & 0xffffff) - AltTime));
    pTimer->timerUByte8((UI8)(pTimer->Time() - AltTime));
    //pTimer->timerTime = AltTime | (pTimer->timerTime & 0xff000000);
    pTimer->Time(AltTime);
  }
  else
  {
    //pTimer->timerUByte8((UI8)(AltTime - (pTimer->timerTime & 0xffffff)));
    pTimer->timerUByte8((UI8)(AltTime - pTimer->Time()));
  };
  gameTimers.SetTimer(pTimer);
}


//*********************************************************
//
//*********************************************************
//   TAG00afbc
void DeleteMonsterMovementTimers(i32 mapX,i32 mapY)
{ //Timer types 29 to 41
  dReg D0;
  TIMER *pTimer;
  TIMERTYPE tt;
  TIMER_SEARCH timerSearch;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7W = P1;
  //D6W = P2;
  //pTimer = gameTimers.Timers();  xxTIMERxx Change to FindNextTimer
  if (AITraceActive)                                          
  {                                                           
    fprintf(GETFILE(TraceFile),                               
            "      MAI Delete all Monster Timers at (%d,%d)\n",
            mapX, mapY);
  };                                                          

  //for (D5W=0; D5W<d.MaxTimer(); psA3++,D5W++)
  while (timerSearch.FindNextTimer())
  {
    pTimer = timerSearch.TimerAddr();
    //D0L = (pTimer->timerTime >> 24) & 0xff;
    D0L = pTimer->Level();
    if (D0L != d.LoadedLevel) continue;
    tt = pTimer->Function();
    if (tt < TT_FIRST_MONSTER_AI) continue;
    if (tt > TT_LAST_MONSTER_AI) continue;
    if (pTimer->timerUByte6() != mapX) continue;
    if (pTimer->timerUByte7() != mapY) continue;
    //TraceTimer(psA3, D5W, "Delete");
    timerSearch.DeleteTimer();

  };
}


//********************************************************                //SMT
//  They are here to make duplicating pieces of code                      //SMT
//  easier.  We are duplicating to get rid of gotos.                      //SMT
//********************************************************                //SMT
void Set_Monster_Timer(                                                   //SMT
                   TIMER&    timer_70,                                    //SMT
                   i32&      i_60,                                        //SMT
             const i16       timeUntilAlternateUpdate,                    //SMT
             const bool      processingATimer)                            //SMT
  //If (processingATimer)                                                 //SMT
  //{                                                                     //SMT
  //  i_60=time of next A event.                                          //SMT
  //  timeUntilAlternateUpdate = delay until next B event.                //SMT 
  //}                                                                     //SMT
  //else                                                                  //SMT
  //{                                                                     //SMT
  //  i60=delay of next B event following next A event                    //SMT
  //  timeUntilAlternatUpdate = delay until next A event.                 //SMT
  //};                                                                    //SMT
{                                                                         //SMT
  /*tag00dcb8:*/                                                          //SMT
  if (AITraceActive)                                                      //SMT
  {                                                                       //SMT
    fprintf(GETFILE(TraceFile),                                           //SMT
       "      MAI Set_Monster_Timer(i_60=%d,"                             //SMT
       "timeUntilAlternateUpdate=%d,processingATimer=%d, function=%s)\n", //SMT
        i_60, timeUntilAlternateUpdate, processingATimer,                 //SMT
        TimerName(timer_70.Function()));                                  //SMT
  };                                                                      //SMT
  TIMERTRACE(0xdcb8);                                                     //SMT
  if (processingATimer)                                                   //SMT
  { //Set B event time                                                    //SMT
    TIMERTRACE(0xdcbe);                                                   //SMT
    //timer_70.timerTime += timeUntilAlternateUpdate & 0xffff;            //SMT
    timer_70.Time(timer_70.Time() + (timeUntilAlternateUpdate & 0xffff));//SMT
  }                                                                       //SMT
  else                                                                    //SMT
  {                                                                       //SMT
    i_60 += timeUntilAlternateUpdate & 0xffff;                            //SMT
  };                                                                      //SMT
  SetMonsterTimerB(&timer_70, i_60);                                      //SMT
}                                                                         //SMT


//***************************************************
//  They are here to make duplicating pieces of code 
//  easier.  We are duplicating to get rid of gotos. 
//***************************************************
void EnsureI60NonZero(i32& i_60, ITEM16 *pI16A2)     //I60NZ
{                                                    //I60NZ
  if (i_60 == 0)                                     //I60NZ
  {                                                  //I60NZ
    TIMERTRACE(0xdca6);                              //I60NZ
    i_60 = NextMonsterUpdateTime(pI16A2, -1, false); //I60NZ
  };                                                 //I60NZ
}                                                    //I60NZ


//***************************************************         //I52R
//  They are here to make duplicating pieces of code          //I52R
//  easier.  We are duplicating to get rid of gotos.          //I52R
//***************************************************         //I52R
void Increment_Time_By_W52_Plus_Random(                       //I52R
                    TIMER&  timer_70,                         //I52R
              const i16     timePerMove                       //I52R
              )                                               //I52R
{                                                             //I52R
  /*tag00dc7e:*/                                              //I52R
  TIMERTRACE(0xdc7e);                                         //I52R
  //timer_70.timerTime +=                                     //I52R
  //    Larger(1, timePerMove + STRandom0_3() - 1);           //I52R
  timer_70.Time( timer_70.Time() +                            //I52R
      Larger(1, timePerMove + STRandom0_3() - 1));            //I52R
}                                                             //I52R
                
//***************************************************         //DTMSF
//  They are here to make duplicating pieces of code          //DTMSF
//  easier.  We are duplicating to get rid of gotos.          //DTMSF
//***************************************************         //DTMSF
void Delete_Timers_Maybe_Set_Fear(                            //DTMSF
                      DB4    *DB4A3,                          //DTMSF
                      ITEM16 *pI16A2,                         //DTMSF
                const i32     mapX,                           //DTMSF
                const i32     mapY,                           //DTMSF
                const bool    fearFactorIs6                   //DTMSF
                                  )                           //DTMSF
{                                                             //DTMSF
  //ASSERT(fearFactorIs6 != 0x7ddd);                          //DTMSF
  if (fearFactorIs6)                                          //DTMSF
  {                                                           //DTMSF
    TIMERTRACE(0xd46c);                                       //DTMSF
    /*D3W = 7;*/                                              //DTMSF
    DB4A3->fear(StateOfFear7);                                //DTMSF
  };                                                          //DTMSF 
  ClearAttacking_DeleteMovementTimers(pI16A2, mapX, mapY);    //DTMSF
}                                                             //DTMSF


//****************************************************************
// This was extracted from ProcessTimers29to41 so that the code   
// could be 'unwound' a bit.  This code is used in several places 
// and used to be reached via 'goto' instructions.                
//****************************************************************
void MaybeDelTimers_Fear6_TurnIndividuals(                    //MDF6TI
                      ITEM16        *pI16A2,                  //MDF6TI
                      DB4           *DB4A3,                   //MDF6TI
                      TIMER&         timer_70,                //MDF6TI
                const i16            timeUntilAlternateUpdate,//MDF6TI
                const TIMERTYPE      timeFunc,                //MDF6TI
                const i32            mapX,                    //MDF6TI
                const i32            mapY,                    //MDF6TI
                const MONSTERDESC&   mtDescLocal,             //MDF6TI
                const i16            indexOfLastMonster,      //MDF6TI
                const i16            horizSize,               //MDF6TI
                const bool           processingATimer,        //MDF6TI
                      MMRECORD      *mmr                      //MDF6TI
                )                                             //MDF6TI
{                                                             //MDF6TI
  dReg D1, D3, D5;                                            //MDF6TI
  i32 monIdx;                                                 //MDF6TI
  i32 monsterDirection;                                       //MDF6TI
  i32 w_80;                                                   //MDF6TI
  TIMERTYPE tt;                                               //MDF6TI
  TIMERTRACE(0xd0f0);                                         //MDF6TI
  if (timeFunc == TT_M2)                                      //MDF6TI
  {                                                           //MDF6TI
    TIMERTRACE(0xd0f8);                                       //MDF6TI
    DeleteMonsterMovementTimers(mapX, mapY);                  //MDF6TI
  };                                                          //MDF6TI
  pI16A2->uByte6 = ub(d.partyX);                              //MDF6TI
  pI16A2->uByte7 = ub(d.partyY);                              //MDF6TI
  D3W = 6;                                                    //MDF6TI
  DB4A3->fear(StateOfFear6);                                  //MDF6TI
  if (AITraceActive)                                          //MDF6TI
  {                                                           //MDF6TI
    fprintf(GETFILE(TraceFile),                               //MDF6TI
      "      MAI Set Fear Factor to 6.  Turn Individuals\n"); //MDF6TI
    fprintf(GETFILE(TraceFile),                               //MDF6TI
      "      MAI Set TT_B<n> timer for each Group Member\n"); //MDF6TI
  };                                                          //MDF6TI
  D5W = d.PrimaryDirectionToParty;                            //MDF6TI
                                                              //MDF6TI
  for (monIdx = indexOfLastMonster; monIdx >= 0; monIdx--)    //MDF6TI
  {                                                           //MDF6TI
    i32 coinflip=2;                                           //MDF6TI
                                                              //MDF6TI
    /* Always turn monster 0 to the preferred direction.*/    //MDF6TI
    /* Turn other monsters with probability of 50       */    //MDF6TI
    /* percent.                                         */    //MDF6TI
    /* You may well ask what is happening here.         */    //MDF6TI
    /* The compiler optimization had                    */    //MDF6TI
    /* an error and treated (2*monIdx) as a loop        */    //MDF6TI
    /* constant.  So I could                            */    //MDF6TI
    /* not shift by (2*monIdx).  So I shifted twice     */    //MDF6TI
    /* by monIdx instead.                               */    //MDF6TI
    /* What grief!                                      */    //MDF6TI
   /*    monsterDirection =                             */    //MDF6TI
   /*        (pI16A2->facings() >> (2*monIdx)) & 3;     */    //MDF6TI
    monsterDirection =                                        //MDF6TI
         TwoBitNibble(pI16A2->facings(),monIdx);              //MDF6TI
    if (   (D5W != monsterDirection)                          //MDF6TI
                                                              //MDF6TI
      && ((monIdx==0) || ((coinflip=STRandomBool())==0)) )    //MDF6TI
    {                                                         //MDF6TI
      TIMERTRACE(0xd154);                                     //MDF6TI
                                                              //MDF6TI
      mmr->Setflg(MDF6TI_turnMonsterTowardParty);             //MDF6TI
      TurnMonster(pI16A2,                                     //MDF6TI
                  D5W,  /*preferred direction*/               //MDF6TI
                  monIdx,  /*monster index*/                  //MDF6TI
                  (indexOfLastMonster != 0)                   //MDF6TI
                        && (horizSize == 1));                 //MDF6TI
      w_80 = STRandom0_3();                                   //MDF6TI
      D1L = d.Time;                                           //MDF6TI
      D3L = w_80 & 0xffff;                                    //MDF6TI
      D1L += D3L + 2;                                         //MDF6TI
      //timer_70.timerTime =                                  //MDF6TI
      //       (timer_70.timerTime & 0xff000000) | D1L;       //MDF6TI
      timer_70.Time(D1L);                                     //MDF6TI
    }                                                         //MDF6TI
    else                                                      //MDF6TI
    {                                                         //MDF6TI
      TIMERTRACE(0xd1a8);                                     //MDF6TI
      //timer_70.timerTime =                                  //MDF6TI
      //  (timer_70.timerTime & 0xff000000) | (d.Time + 1);   //MDF6TI
      timer_70.Time(d.Time+1);                                //MDF6TI
    };                                                        //MDF6TI
    if (processingATimer)                                     //MDF6TI
    {                                                         //MDF6TI
      TIMERTRACE(0xd1c4);                                     //MDF6TI
      //timer_70.timerTime +=                                 //MDF6TI
      //  Smaller(STRandom0_3()+(UI8)(mtDescLocal.uByte7)/2,  //MDF6TI
      //          timeUntilAlternateUpdate);                  //MDF6TI
      timer_70.Time(timer_70.Time() +                         //MDF6TI
        Smaller(STRandom0_3()                                 //MDF6TI
                   +(UI8)(mtDescLocal.attackTicks07)/2,       //MDF6TI
                timeUntilAlternateUpdate));                   //MDF6TI
    };                                                        //MDF6TI
    switch (monIdx)                                           //MDF6TI
    {                                                         //MDF6TI
    case 0: tt = TT_MONSTER_B0; break;                        //MDF6TI
    case 1: tt = TT_MONSTER_B1; break;                        //MDF6TI
    case 2: tt = TT_MONSTER_B2; break;                        //MDF6TI
    case 3: tt = TT_MONSTER_B3; break;                        //MDF6TI
    default: die(0x3ffa, "Illegal Monster Index");            //MDF6TI
             tt = TT_MONSTER_B0;                              //MDF6TI
             break;                                           //MDF6TI
    };                                                        //MDF6TI
    timer_70.Function(tt);            //monIdx=0->3????       //MDF6TI
    SetMonsterTimerB(&timer_70,                               //MDF6TI
              NextMonsterUpdateTime(pI16A2,                   //MDF6TI
                                    (i16)monIdx,              //MDF6TI     
                                     false));                 //MDF6TI
    /*continue here*/                                         //MDF6TI
                                                              //MDF6TI
  }; /* for (monIdx) */                                       //MDF6TI
}                                                             //MDF6TI


//******************************************************************
// This was extracted from ProcessTimers29to41 so that the code     
// could be 'unwound' a bit.  This code is used in several places   
// and used to be reached via 'goto' instructions.                  
//******************************************************************
void Turn_Monsters_As_Group(                                       //TMAG
                    dReg&     D5,                                  //TMAG
                    ITEM16   *pI16A2,                              //TMAG
              const i16       w_54,                                //TMAG
              const i16       indexOfLastMonster,                  //TMAG
              const i16       horizSize,                           //TMAG
              const bool      bool_36,                             //TMAG
                    MMRECORD *mmr                                  //TMAG
                            )                                      //TMAG
{                                                                  //TMAG
  TIMERTRACE(0xd42e);                                              //TMAG
  if ( (bool_36 == false) && (w_54 >= 0) )                         //TMAG
  {                                                                //TMAG
    TIMERTRACE(0xd43a);                                            //TMAG
    D5W = STRandom0_3();                                           //TMAG
  };                                                               //TMAG
  mmr->Setflg(TMAG_turnMonsterGroup);                              //TMAG
  TurnMonsterGroup(pI16A2,                                         //TMAG
                   D5W,                /*preferred direction    */ //TMAG
                   indexOfLastMonster,                             //TMAG
                   horizSize);                                     //TMAG
  TIMERTRACE(0xd454);                                              //TMAG
}                                                                  //TMAG


//******************************************************************      //SBGT
// This was extracted from ProcessTimers29to41 so that the code           //SBGT
// could be 'unwound' a bit.  This code is used in several places         //SBGT
// and used to be reached via 'goto' instructions.                        //SBGT
//******************************************************************      //SBGT
void StartBGroupTimer(                                                    //SBGT
            TIMER&             timer_70,                                  //SBGT
            ITEM16             *pI16A2,                                   //SBGT
            i32&               i_60,                                      //SBGT
            const i16          timeUntilAlternateUpdate,                  //SBGT
            const bool         processingATimer                           //SBGT
                  )                                                       //SBGT
{                                                                         //SBGT
  TIMERTRACE(0xdc9a);                                                     //SBGT
  timer_70.Function(TT_MONSTER_Bgroup);                                   //SBGT
  EnsureI60NonZero(i_60, pI16A2);                                         //SBGT
  Set_Monster_Timer(                                                      //SBGT
                   timer_70,                                              //SBGT
                   i_60,                                                  //SBGT
                   timeUntilAlternateUpdate, //const                      //SBGT
                   processingATimer          //const                      //SBGT
                   );                                                     //SBGT

}

//******************************************************************      //PIn
// This was extracted from ProcessTimers29to41 so that the code           //PIn
// could be 'unwound' a bit.  This code is used in several places         //PIn
// and used to be reached via 'goto' instructions.                        //PIn
//******************************************************************      //PIn
bool Process_Invincible(                                                  //PIn
            TIMER&             timer_70,                                  //PIn
            ITEM16             *pI16A2,                                   //PIn
            i32&               i_60,                                      //PIn
            i16&               w_54,                                      //PIn
            i16&               timePerMove,                               //PIn
            bool&              bool_36, //Value ignored; Sometimes set.   //PIn
            dReg&              D5,                                        //PIn
            const MONSTERDESC& mtDescLocal,                               //PIn
            const RN&          monster,                                   //PIn
            const i16          timeUntilAlternateUpdate,                  //PIn
            const i16          indexOfLastMonster,                        //PIn
            const i16          horizSize,                                 //PIn
            const bool         processingATimer,                          //PIn
            const i32          mapX,                                      //PIn
            const i32          mapY,                                      //PIn
            const i32          PreferredDirection,                        //PIn
                  MMRECORD    *pmmr)                                      //PIn
{                                                                         //PIn
  dReg D0, D4;                                                            //PIn
  i16 w_30, w_32;                                                         //PIn
  TIMERTRACE(0xd63c);                                                     //PIn
  if (AITraceActive)                                                      //PIn
  {                                                                       //PIn
    fprintf(GETFILE(TraceFile),                                           //PIn
        "      MAI Entering Process_Invincible\n");                       //PIn
  };                                                                      //PIn
  FindPossibleMove(&mtDescLocal, mapX, mapY, 0);                          //PIn
  D5W = sw(PreferredDirection);                                           //PIn
  D0W = PossibleDoubleMove(&mtDescLocal, mapX, mapY, D5W);                //PIn
  if (D0W == 0)                                                           //PIn
  {                                                                       //PIn
    TIMERTRACE(0xd66a);                                                   //PIn
    D5W = sw((i32)d.SecondaryDirection);                                  //PIn
    D0W = PossibleDoubleMove(&mtDescLocal, mapX, mapY, D5W);              //PIn
    if (D0W == 0)                                                         //PIn
    {                                                                     //PIn
      TIMERTRACE(0xd686);                                                 //PIn
      if (d.FluxCageCount != 0)                                           //PIn
      {                                                                   //PIn
        TIMERTRACE(0xd686);                                               //PIn
        D5W = (I16)((D5W + 2) & 3);                                       //PIn
        D0W = PossibleDoubleMove(&mtDescLocal, mapX, mapY, D5W);          //PIn
      };                                                                  //PIn
      if (D0W == 0)                                                       //PIn
      {                                                                   //PIn
        TIMERTRACE(0xd6ac);                                               //PIn
        if (d.FluxCageCount < 2)                                          //PIn
        {                                                                 //PIn
          TIMERTRACE(0xd72c);                                             //PIn
          pmmr->Setflg(PIn_turnMonsterGroup);                             //PIn
          TurnMonsterGroup(pI16A2, PreferredDirection,                    //PIn
                           indexOfLastMonster, horizSize);                //PIn 
          Increment_Time_By_W52_Plus_Random(timer_70, timePerMove);       //PIn
          StartBGroupTimer(                                               //PIn
                 timer_70,                                                //PIn
                 pI16A2,                                                  //PIn
                 i_60,                                                    //PIn
                 timeUntilAlternateUpdate,  //const                       //PIn
                 processingATimer           //const                       //PIn
                 );                                                       //PIn
          return true;                                                    //PIn
        };                                                                //PIn
        TIMERTRACE(0xd6b4);                                               //PIn
        D5W = sw((PreferredDirection + 2) & 3);                           //PIn
        D0W = PossibleDoubleMove(&mtDescLocal, mapX, mapY, D5W);          //PIn
        if (D0W == 0)                                                     //PIn
        {                                                                 //PIn
          TIMERTRACE(0xd72c);                                             //PIn
          pmmr->Setflg(PIn_turnMonsterGroup);                             //PIn
          TurnMonsterGroup(pI16A2, PreferredDirection,                    //PIn
                                indexOfLastMonster, horizSize);           //PIn
          Increment_Time_By_W52_Plus_Random(timer_70, timePerMove);       //PIn
          StartBGroupTimer(                                               //PIn
                 timer_70,                                                //PIn
                 pI16A2,                                                  //PIn
                 i_60,                                                    //PIn
                 timeUntilAlternateUpdate,  //const                       //PIn
                 processingATimer           //const                       //PIn
                 );                                                       //PIn
          return true;                                                    //PIn
        };                                                                //PIn
      };                                                                  //PIn
    };                                                                    //PIn
  };                                                                      //PIn
//tag00d6d6:                                                              //PIn
  TIMERTRACE(0xd6d6);                                                     //PIn
  // Try two steps in direction D5W???                                    //PIn
  w_30 = sw(mapX);                                                        //PIn
  w_32 = sw(mapY);                                                        //PIn
  w_30 = sw(w_30 + 2*d.DeltaX[D5W]);                                      //PIn
  w_32 = sw(w_32 + 2*d.DeltaY[D5W]);                                      //PIn
  QueueSound(17, w_30, w_32, 1);                                          //PIn
  TIMERTRACE(0xd302);                                                     //PIn
  D4W = sw(timePerMove/2 - w_54);                                         //PIn
  bool_36 = D4W <= 0;                                                     //PIn
  if (bool_36)                                                            //PIn
  {                                                                       //PIn
    TIMERTRACE(0xd31c);                                                   //PIn
    D0W = MoveObject(monster, mapX, mapY, w_30, w_32, NULL, pmmr);        //PIn
    ASSERT(D0W != 0xccc,"D0W");                                           //PIn
    if (D0W != 0)                                                         //PIn
    {                                                                     //PIn
      pmmr->Setflg(PIn_moveTwoSquaresSucceeded);                          //PIn
      return true;                                                        //PIn
    };                                                                    //PIn
    pmmr->Setflg(PIn_moveTwoSquaresFailed);                               //PIn
    timer_70.timerUByte6(ub(d.NewX));                                     //PIn
    timer_70.timerUByte7(ub(d.NewY));                                     //PIn
    pI16A2->ubPrevX = ub(mapX);                                           //PIn
    pI16A2->ubPrevY = ub(mapY);                                           //PIn
    pI16A2->uByte4 = (ui8)d.Time;                                         //PIn
  }                                                                       //PIn
  else                                                                    //PIn
  {                                                                       //PIn
    TIMERTRACE(0xd362);                                                   //PIn
    timePerMove = D4W;                                                    //PIn
    w_54 = -1;                                                            //PIn
  };                                                                      //PIn
  return false;                                                           //PIn
}                                                                         //PIn
  

//******************************************************************//SF
// This was extracted from ProcessTimers29to41 so that the code     //SF
// could be 'unwound' a bit.  This code is used in several places   //SF
// and used to be reached via 'goto' instructions.                  //SF
//******************************************************************//SF
void Standard_Finish(                                               //SF
                           dReg&           D5,                      //SF
                           ITEM16         *pI16A2,                  //SF
                           TIMER&          timer_70,                //SF
                           i32&            i_60,                    //SF
                           DB4            *DB4A3,                   //SF
                     const i16             w_54,                    //SF
                     const i16             timePerMove,             //SF
                     const i16             indexOfLastMonster,      //SF
                     const i16             horizSize,               //SF
                     const bool            fearFactorIs6,           //SF
                     const bool            processingATimer,        //SF
                     const bool            bool_36,                 //SF
                     const i16             attackDistance,          //SF
                     const TIMERTYPE       timeFunc,                //SF
                     const MONSTERDESC&    mtDescLocal,             //SF
                     const i16             timeUntilAlternateUpdate,//SF
                     const i32             mapX,                    //SF
                     const i32             mapY,                    //SF
                           MMRECORD       *mmr                      //SF
                     )                                              //SF
{                                                                   //SF
  if (AITraceActive)                                                //SF
  {                                                                 //SF
    fprintf(GETFILE(TraceFile),                                     //SF
        "      MAI Entering Standard_Finish\n");                    //SF
  };                                                                //SF
  if (bool_36 == false)                                             //SF
  {                                                                 //SF
    TIMERTRACE(0xd40c);                                             //SF
    if (STRandom0_3() != 0)                                         //SF
    {                                                               //SF
      TIMERTRACE(0xd414);                                           //SF
                                                                    //SF
      ASSERT(    (timeFunc==-3)                                     //SF 
             ||  (attackDistance != 0x7ddd) ,"timeFunc");           //SF
      //                                                            //SF
      // This happened once when                                    //SF
      // I was bashing an oitu with a door.                         //SF
      // Happened when I sent fireball at oitu. (timeFunc=-2)       //SF
      // Happened again when I bashed a demon with a door.          //SF
      // Notice that if (timeFunc==-3) then we unconditionally      //SF
      // make the same jump as we make conditionally here.          //SF
      //                                                            //SF
      // Therefore, the fact that attackDistance is undefined will  //SF
      // never do any harm (if timeFunc==-3).                       //SF
      //                                                            //SF
      // If too far away then don't turn towards party.             //SF
      if (attackDistance > mtDescLocal.smellingDistance())          //SF
      {                                                             //SF
        mmr->Setflg(SF_greaterThanSmellingDistance);                //SF
        TIMERTRACE(0xd454);                                         //SF
        if (timeFunc < -1)                                          //SF
        {                                                           //SF
          TIMERTRACE(0xd45c);                                       //SF
          if (bool_36 == false)                                     //SF
          {                                                         //SF
            mmr->Setflg(SF_return);                                 //SF
            return;                                                 //SF
          };                                                        //SF
          mmr->Setflg(SF_DeleteTimersMaybeSetFear);                 //SF
          Delete_Timers_Maybe_Set_Fear(                             //SF
                                       DB4A3,                       //SF
                                       pI16A2,                      //SF
                                       mapX,                        //SF
                                       mapY,                        //SF
                                       fearFactorIs6                //SF
                                       );                           //SF
        };                                                          //SF
        Increment_Time_By_W52_Plus_Random(timer_70, timePerMove);   //SF
        StartBGroupTimer(                                           //SF
               timer_70,                                            //SF
               pI16A2,                                              //SF
               i_60,                                                //SF
               timeUntilAlternateUpdate, //const                    //SF
               processingATimer          //const                    //SF
               );                                                   //SF
        return;                                                     //SF
      };                                                            //SF
    };                                                              //SF
    if (timeFunc == -3)                                             //SF
    {                                                               //SF
      mmr->Setflg(SF_timeFuncMinusThree);                           //SF
      TIMERTRACE(0xd454);                                           //SF
      if (timeFunc < -1)                                            //SF
      {                                                             //SF
        TIMERTRACE(0xd45c);                                         //SF
        if (bool_36 == false)                                       //SF
        {                                                           //SF
          return;                                                   //SF
        };                                                          //SF
        mmr->Setflg(SF_DeleteTimersMaybeSetFear);                   //SF
        Delete_Timers_Maybe_Set_Fear(                               //SF
                                     DB4A3,                         //SF
                                     pI16A2,                        //SF
                                     mapX,                          //SF
                                     mapY,                          //SF
                                     fearFactorIs6                  //SF
                                     );                             //SF
      };                                                            //SF
      Increment_Time_By_W52_Plus_Random(timer_70, timePerMove);     //SF
      StartBGroupTimer(                                             //SF
             timer_70,                                              //SF
             pI16A2,                                                //SF
             i_60,                                                  //SF
             timeUntilAlternateUpdate,  //const                     //SF
             processingATimer           //const                     //SF
             );                                                     //SF
      return;                                                       //SF
    };                                                              //SF
  };                                                                //SF
  mmr->Setflg(SF_TurnMonstersAsGroup);                              //SF
  Turn_Monsters_As_Group(                                           //SF
                  D5,                                               //SF
                  pI16A2,                                           //SF
                  w_54,               //const                       //SF
                  indexOfLastMonster, //const                       //SF
                  horizSize,          //const                       //SF
                  bool_36,            //const                       //SF
                  mmr                                               //SF
                  );                                                //SF
  if (timeFunc < -1)                                                //SF
  {                                                                 //SF
    TIMERTRACE(0xd45c);                                             //SF
    if (bool_36 == false)                                           //SF
    {                                                               //SF
      return;                                                       //SF
    };                                                              //SF
    mmr->Setflg(SF_DeleteTimersMaybeSetFear);                       //SF
    Delete_Timers_Maybe_Set_Fear(                                   //SF
                                 DB4A3,                             //SF
                                 pI16A2,                            //SF
                                 mapX,                              //SF
                                 mapY,                              //SF
                                 fearFactorIs6                      //SF
                                 );                                 //SF
  };                                                                //SF
  mmr->Setflg(SF_IncrementTime);                                    //SF
  Increment_Time_By_W52_Plus_Random(timer_70, timePerMove);         //SF
  StartBGroupTimer(                                                 //SF
         timer_70,                                                  //SF
         pI16A2,                                                    //SF
         i_60,                                                      //SF
         timeUntilAlternateUpdate,  //const                         //SF
         processingATimer           //const                         //SF
         );                                                         //SF
}                                                                   //SF


//******************************************************************//PIAF
// This was extracted from ProcessTimers29to41 so that the code     //PIAF
// could be 'unwound' a bit.  This code is used in several places   //PIAF
// and used to be reached via 'goto' instructions.                  //PIAF
//******************************************************************//PIAF
void Process_Invincible_And_Finish(                                 //PIAF
           dReg&          D5,                                       //PIAF
           DB4            *DB4A3,                                   //PIAF
           i32&           PreferredDirection,                       //PIAF
           ITEM16         *pI16A2,                                  //PIAF
           TIMER&         timer_70,                                 //PIAF
           i32            i_60,                                     //PIAF
           i16            w_54,                                     //PIAF
           i16            timePerMove,                              //PIAF
           bool           bool_36,                                  //PIAF
     const i32            mapX,                                     //PIAF
     const i32            mapY,                                     //PIAF
     const TIMERTYPE      timeFunc,                                 //PIAF
     const RN             monster,                                  //PIAF
     const i16            timeUntilAlternateUpdate,                 //PIAF
     const MONSTERDESC&   mtDescLocal,                              //PIAF
     const bool           invincible,                               //PIAF
     const i16            indexOfLastMonster,                       //PIAF
     const i16            horizSize,                                //PIAF
     const bool           fearFactorIs6,                            //PIAF
     const bool           processingATimer,                         //PIAF
     const i16            attackDistance,                           //PIAF
           MMRECORD      *mmr                                       //PIAF
                                   )                                //PIAF
{                                                                   //PIAF
  TIMERTRACE(0xd3cc);                                               //PIAF
  if (AITraceActive)                                                //PIAF
  {                                                                 //PIAF
    fprintf(GETFILE(TraceFile),                                     //PIAF
        "      MAI Entering Process_Invincible_And_Finish\n");      //PIAF
    if (bool_36)                                                    //PIAF
    {                                                               //PIAF
      fprintf(GETFILE(TraceFile),                                   //PIAF
        "      MAI bool_36 true..skiping 'while'\n");               //PIAF
    }                                                               //PIAF
    else if (w_54 == -1)                                            //PIAF
    {                                                               //PIAF
      fprintf(GETFILE(TraceFile),                                   //PIAF
        "      MAI w_54 = -1...skipping 'while'\n");                //PIAF
    }                                                               //PIAF
    else if (!invincible)                                           //PIAF
    {                                                               //PIAF
      fprintf(GETFILE(TraceFile),                                   //PIAF
        "      MAI Not invincible...skipping 'while'\n");           //PIAF
    };                                                              //PIAF
  };                                                                //PIAF
  while (   (bool_36 == false)                                      //PIAF
         && (w_54 != -1)                                            //PIAF
         && (invincible)                                            //PIAF
         && ((timeFunc == -3) || (STRandom0_3() == 0)))             //PIAF
  {                                                                 //PIAF
    TIMERTRACE(0xd3f0);                                             //PIAF
    PreferredDirection = STRandom0_3();                             //PIAF
    d.SecondaryDirection =                                          //PIAF
                 (DIRECTION)((PreferredDirection + 1) & 3);         //PIAF
//tag00d63c:                                                        //PIAF
    mmr->Setflg(PIAF_processInvincible);                            //PIAF
    if (Process_Invincible(                                         //PIAF
           timer_70,                //Modified                      //PIAF
           pI16A2,                  //Modified                      //PIAF
           i_60,                    //Modified                      //PIAF
           w_54,                    //Modified                      //PIAF
           timePerMove,             //Modified                      //PIAF
           bool_36,                 //Value ignored; Sometimes set. //PIAF
           D5,                      //Modified dir of movement      //PIAF
           mtDescLocal,             //const                         //PIAF
           monster,                 //const                         //PIAF
           timeUntilAlternateUpdate,//const                         //PIAF
           indexOfLastMonster,      //const                         //PIAF
           horizSize,               //const                         //PIAF
           processingATimer,        //const                         //PIAF
           mapX,                    //const                         //PIAF
           mapY,                    //const                         //PIAF
           PreferredDirection,      //const                         //PIAF
           mmr                                                      //PIAF
         )) return;                                                 //PIAF
    // goto tag00d3cc;  Not needed because of 'while' //            //PIAF
  };                                                                //PIAF
  mmr->Setflg(PIAF_standardFinish);                                 //PIAF
  Standard_Finish(                                                  //PIAF
                    D5,                      //modified             //PIAF
                    pI16A2,                  //modified             //PIAF
                    timer_70,                //modified             //PIAF
                    i_60,                    //modified             //PIAF
                    DB4A3,                   //modified             //PIAF
                    w_54,                    //const                //PIAF
                    timePerMove,             //const                //PIAF
                    indexOfLastMonster,      //const                //PIAF
                    horizSize,               //const                //PIAF
                    fearFactorIs6,           //const                //PIAF
                    processingATimer,        //const                //PIAF
                    bool_36,                 //const                //PIAF
                    attackDistance,          //const                //PIAF
                    timeFunc,                //const                //PIAF
                    mtDescLocal,             //const                //PIAF
                    timeUntilAlternateUpdate,//const                //PIAF
                    mapX,                    //const                //PIAF
                    mapY,                    //const                //PIAF
                    mmr                                             //PIAF
                  );                                                //PIAF
}                                                                   //PIAF

//******************************************************************//TPTO
// This was extracted from ProcessTimers29to41 so that the code     //TPTO
// could be 'unwound' a bit.  This code is used in several places   //TPTO
// and used to be reached via 'goto' instructions.                  //TPTO
//******************************************************************//TPTO
void Try_Preferred_Direction_Then_Others(                           //TPTO
                       i32             PreferredDirection,          //TPTO
                       ITEM16         *pI16A2,                      //TPTO
                       DB4            *DB4A3,                       //TPTO
                       TIMER&          timer_70,                    //TPTO
                       i32             i_60,                        //TPTO
                       i16             w_54,                        //TPTO
                       i16             timePerMove,                 //TPTO
                       i16             w_32,                        //TPTO
                       i16             w_30,                        //TPTO
                 const MONSTERDESC&    mtDescLocal,                 //TPTO
                 const i32             mapX,                        //TPTO
                 const i32             mapY,                        //TPTO
                 const bool            invincible,                  //TPTO
                 const i16             timeUntilAlternateUpdate,    //TPTO
                 const RN              monster,                     //TPTO
                 const TIMERTYPE       timeFunc,                    //TPTO
                 const i16             indexOfLastMonster,          //TPTO
                 const i16             horizSize,                   //TPTO
                 const bool            fearFactorIs6,               //TPTO
                 const bool            RFWAP,                       //TPTO
                 const bool            processingATimer,            //TPTO
                 const i16             attackDistance,              //TPTO
                       MMRECORD       *pmmr                         //TPTO
                 )                                                  //TPTO
{                                                                   //TPTO
  dReg D0, D4, D5;                                                  //TPTO
  bool bool_36;                                                     //TPTO
  TIMERTRACE(0xd55c + (PreferredDirection<<16));                    //TPTO
                                                                    //TPTO
  if (    (!PossibleMove(&mtDescLocal,                              //TPTO
                         mapX,                                      //TPTO
                         mapY,                                      //TPTO
                         D5W = sw(PreferredDirection),              //TPTO
                         RFWAP))                                    //TPTO
                                                                    //TPTO
       && (!PossibleMove(&mtDescLocal,                              //TPTO
                         mapX,                                      //TPTO
                         mapY,                                      //TPTO
                         D5W=sw((i32)d.SecondaryDirection),         //TPTO
                         (RFWAP != 0) && (STRandomBool() !=0)))     //TPTO
                                                                    //TPTO
       && (!PossibleMove(&mtDescLocal,                              //TPTO
                         mapX,                                      //TPTO
                         mapY,                                      //TPTO
                         D5W=(I16)((D5W + 2) & 3),                  //TPTO
                         0))                                        //TPTO
       && (   (STRandom0_3() != 0)                                  //TPTO
           || (!PossibleMove(&mtDescLocal,                          //TPTO
                             mapX,                                  //TPTO
                             mapY,                                  //TPTO
                             D5W=sw((PreferredDirection + 2) & 3),  //TPTO
                             0))) )                                 //TPTO
  {                                                                 //TPTO
    //tag00d634:                                                    //TPTO
    TIMERTRACE(0xd634);                                             //TPTO
    if (!invincible)                                                //TPTO
    {                                                               //TPTO
      TIMERTRACE(0xd72c);                                           //TPTO
      TurnMonsterGroup(pI16A2, PreferredDirection,                  //TPTO
                           indexOfLastMonster, horizSize);          //TPTO
      Increment_Time_By_W52_Plus_Random(timer_70, timePerMove);     //TPTO
      StartBGroupTimer(                                             //TPTO
             timer_70,                                              //TPTO
             pI16A2,                                                //TPTO
             i_60,                                                  //TPTO
             timeUntilAlternateUpdate, //const                      //TPTO
             processingATimer          //const                      //TPTO
             );                                                     //TPTO
      return;                                                       //TPTO
    };                                                              //TPTO
    if (Process_Invincible(                                         //TPTO
          timer_70,                //Modified                       //TPTO
          pI16A2,                  //Modified                       //TPTO
          i_60,                    //Modified                       //TPTO
          w_54,                    //Modified                       //TPTO
          timePerMove,             //Modified                       //TPTO
          bool_36,                 //Value ignored; Sometimes set.  //TPTO
          D5,                      //Modified dir of movement       //TPTO
          mtDescLocal,             //const                          //TPTO
          monster,                 //const                          //TPTO
          timeUntilAlternateUpdate,//const                          //TPTO
          indexOfLastMonster,      //const                          //TPTO
          horizSize,               //const                          //TPTO
          processingATimer,        //const                          //TPTO
          mapX,                    //const                          //TPTO
          mapY,                    //const                          //TPTO
          PreferredDirection,      //const                          //TPTO
          pmmr                                                      //TPTO
         )) return;                                                 //TPTO
  }                                                                 //TPTO
  else                                                              //TPTO
  {                                                                 //TPTO
    TIMERTRACE(0xd5fc);                                             //TPTO
    w_30 = sw(mapX);                                                //TPTO
    w_32 = sw(mapY);                                                //TPTO
    w_30 = sw(w_30 + d.DeltaX[D5W]);                                //TPTO
    w_32 = sw(w_32 + d.DeltaY[D5W]);                                //TPTO
    TIMERTRACE(0xd302);                                             //TPTO
    D4W = sw(timePerMove/2 - w_54);                                 //TPTO
    bool_36 = D4W <= 0;                                             //TPTO
    if (bool_36)                                                    //TPTO
    {                                                               //TPTO
      TIMERTRACE(0xd31c);                                           //TPTO
      D0W = MoveObject(monster, mapX, mapY, w_30, w_32, NULL, pmmr);//TPTO
      ASSERT(D0W != 0xccc,"moveObject");                            //TPTO
      if (D0W != 0)                                                 //TPTO
      {                                                             //TPTO
        return;                                                     //TPTO
      };                                                            //TPTO
      timer_70.timerUByte6(ub(d.NewX));                             //TPTO
      timer_70.timerUByte7(ub(d.NewY));                             //TPTO
      pI16A2->ubPrevX = ub(mapX);                                   //TPTO
      pI16A2->ubPrevY = ub(mapY);                                   //TPTO
      pI16A2->uByte4 = (ui8)d.Time;                                 //TPTO
    }                                                               //TPTO
    else                                                            //TPTO
    {                                                               //TPTO
      TIMERTRACE(0xd362);                                           //TPTO
      timePerMove = D4W;                                            //TPTO
      w_54 = -1;                                                    //TPTO
    };                                                              //TPTO
  };                                                                //TPTO
  Process_Invincible_And_Finish(                                    //TPTO
                           D5,                       //modified     //TPTO
                           DB4A3,                    //modified     //TPTO
                           PreferredDirection,       //modified     //TPTO
                           pI16A2,                   //modified     //TPTO
                           timer_70,                 //modified     //TPTO
                           i_60,                     //Not Modified //TPTO
                           w_54,                     //Not Modified //TPTO
                           timePerMove,              //Not Modified //TPTO
                           bool_36,                  //Not Modified //TPTO
                           mapX,                     //const        //TPTO
                           mapY,                     //const        //TPTO
                           timeFunc,                 //const        //TPTO
                           monster,                  //const        //TPTO
                           timeUntilAlternateUpdate, //const        //TPTO
                           mtDescLocal,              //const        //TPTO
                           invincible,               //const        //TPTO
                           indexOfLastMonster,       //const        //TPTO
                           horizSize,                //const        //TPTO
                           fearFactorIs6,            //const        //TPTO
                           processingATimer,         //const        //TPTO
                           attackDistance,           //const        //TPTO
                           pmmr                                     //TPTO
                       );                                           //TPTO
}                                                                   //TPTO


//******************************************************************//TTM
// This was extracted from ProcessTimers29to41 so that the code     //TTM
// could be 'unwound' a bit.  This code is used in several places   //TTM
// and used to be reached via 'goto' instructions.                  //TTM
//******************************************************************//TTM
void Try_To_Move(                                                   //TTM
          dReg                 D5,                                  //TTM
          DB4                 *DB4A3,                               //TTM
          ITEM16              *pI16A2,                              //TTM
          TIMER&               timer_70,                            //TTM
          i32                  i_60,                                //TTM
          i16                  w_54,                                //TTM
          i16                  timePerMove,                         //TTM
          i16                  attackDistance,                      //TTM
          i16                  w_32,                                //TTM
          i16                  w_30,                                //TTM
          const TIMERTYPE      timeFunc,                            //TTM
          const RN             monster,                             //TTM
          const i16            timeUntilAlternateUpdate,            //TTM
          const MONSTERDESC&   mtDescLocal,                         //TTM
          const bool           invincible,                          //TTM
          const i32            mapX,                                //TTM
          const i32            mapY,                                //TTM
          const i16            indexOfLastMonster,                  //TTM
          const i16            horizSize,                           //TTM
          const bool           fearFactorIs6,                       //TTM
          const bool           processingATimer,                    //TTM
                MMRECORD      *mmr                                  //TTM
                 )                                                  //TTM
{                                                                   //TTM
  dReg D0, D1, D3;                                                  //TTM
  i32 PreferredDirection;                                           //TTM
  TIMERTRACE(0xd74e);                                               //TTM
  attackDistance = Blocked(DB4A3, -1, mapX, mapY);                  //TTM
                  //  Checks each member for 'looking our way'      //TTM
  if (attackDistance != 0)                                          //TTM
  {                                                                 //TTM
    TIMERTRACE(0xd76c);                                             //TTM
    pI16A2->uByte6 = ub(d.partyX);                                  //TTM
    w_30 = d.partyX;                                                //TTM
    pI16A2->uByte7 = ub(d.partyY);                                  //TTM
    w_32 = d.partyY;                                                //TTM
    TIMERTRACE(0xd834);                                             //TTM
    D0W = (I16)(i32)GetPrimaryAndSecondaryDirection(                //TTM
                       mapX, mapY, w_30, w_32);                     //TTM
    PreferredDirection = (D0W + 2) & 3;                             //TTM
    d.SecondaryDirection =                                          //TTM
       (DIRECTION)((d.SecondaryDirection+2)&3);//180 degrees        //TTM
    timePerMove = sw(timePerMove - timePerMove/4);                  //TTM
    Try_Preferred_Direction_Then_Others(                            //TTM
                         PreferredDirection,       //Not Modified   //TTM
                         pI16A2,                   //modified       //TTM
                         DB4A3,                    //modified       //TTM
                         timer_70,                 //modified       //TTM
                         i_60,                     //Not Modified   //TTM
                         w_54,                     //Not Modified   //TTM
                         timePerMove,              //Not Modified   //TTM
                         w_32,                     //Not Modified   //TTM
                         w_30,                     //Not Modified   //TTM
                         mtDescLocal,              //const          //TTM
                         mapX,                     //const          //TTM
                         mapY,                     //const          //TTM
                         invincible,               //const          //TTM
                         timeUntilAlternateUpdate, //const          //TTM
                         monster,                  //const          //TTM
                         timeFunc,                 //const          //TTM
                         indexOfLastMonster,       //const          //TTM
                         horizSize,                //const          //TTM
                         fearFactorIs6,            //const          //TTM
                         true, //RFWAP             //const          //TTM
                         processingATimer,         //const          //TTM
                         attackDistance,           //const          //TTM
                         mmr                                        //TTM
                  );                                                //TTM
    return;                                                         //TTM
  };                                                                //TTM
  pI16A2->uByte5--;                                                 //TTM
  if (pI16A2->uByte5 != 0)                                          //TTM
  {                                                                 //TTM
//tag00d7ba:                                                        //TTM
    TIMERTRACE(0xd7ba);                                             //TTM
    if (STRandomBool() != 0)                                        //TTM
    {                                                               //TTM
      TIMERTRACE(0xd7c2);                                           //TTM
      D0W = FindPossibleMove(&mtDescLocal, mapX, mapY, 0);          //TTM
      if (D0W == 0)                                                 //TTM
      {                                                             //TTM
        TIMERTRACE(0xd7d8);                                         //TTM
        D0W = sw(abs(mapX - d.partyX));                             //TTM
        D1W = sw(abs(mapY - d.partyY));                             //TTM
        if (D1W + D0W <= 1)                                         //TTM
        {                                                           //TTM
          TIMERTRACE(0xd79e);                                       //TTM
          //bool_36 = false;                                        //TTM
          D3W = 0;                                                  //TTM
          //30OCT02SETWBITS0_3(DB4A3->word14, D3W);                 //TTM
          DB4A3->fear(StateOfFear0);                                //TTM
          Turn_Monsters_As_Group(                                   //TTM
                          D5,                                       //TTM
                          pI16A2,                                   //TTM
                          w_54,               //const               //TTM
                          indexOfLastMonster, //const               //TTM
                          horizSize,          //const               //TTM
                          false,              //const               //TTM
                          mmr                                       //TTM
                          );                                        //TTM
          if (timeFunc < -1)                                        //TTM
          {                                                         //TTM
            return;                                                 //TTM
          };                                                        //TTM
          Increment_Time_By_W52_Plus_Random(timer_70, timePerMove); //TTM
          StartBGroupTimer(                                         //TTM
                 timer_70,                                          //TTM
                 pI16A2,                                            //TTM
                 i_60,                                              //TTM
                 timeUntilAlternateUpdate, //const                  //TTM
                 processingATimer          //const                  //TTM
                 );                                                 //TTM
          return;                                                   //TTM
        };                                                          //TTM
      };                                                            //TTM
      TIMERTRACE(0xd0808);                                          //TTM
      w_30 = pI16A2->uByte10;                                       //TTM
      w_32 = pI16A2->uByte11;                                       //TTM
      TIMERTRACE(0xd546);                                           //TTM
      PreferredDirection = GetPrimaryAndSecondaryDirection(         //TTM
                                  mapX,mapY,w_30,w_32);             //TTM
  // Direction to approach party.                                   //TTM
  //tag00d55c:                                                      //TTM
      Try_Preferred_Direction_Then_Others(                          //TTM
                        PreferredDirection,       //Not Modified    //TTM
                        pI16A2,                   //modified        //TTM
                        DB4A3,                    //modified        //TTM
                        timer_70,                 //modified        //TTM
                        i_60,                     //Not Modified    //TTM
                        w_54,                     //Not Modified    //TTM
                        timePerMove,              //Not Modified    //TTM
                        w_32,                     //Not Modified    //TTM
                        w_30,                     //Not Modified    //TTM
                        mtDescLocal,              //const           //TTM
                        mapX,                     //const           //TTM
                        mapY,                     //const           //TTM
                        invincible,               //const           //TTM
                        timeUntilAlternateUpdate, //const           //TTM
                        monster,                  //const           //TTM
                        timeFunc,                 //const           //TTM
                        indexOfLastMonster,       //const           //TTM
                        horizSize,                //const           //TTM
                        fearFactorIs6,            //const           //TTM
                        true, //RFWAP             //const           //TTM
                        processingATimer,         //const           //TTM
                        attackDistance,           //const           //TTM
                        mmr                                         //TTM
                  );                                                //TTM
      return;                                                       //TTM
    };                                                              //TTM
    TIMERTRACE(0xd820);                                             //TTM
    w_30 = pI16A2->uByte6;                                          //TTM
    w_32 = pI16A2->uByte7;                                          //TTM
    TIMERTRACE(0xd834);                                             //TTM
    D0W = (I16)(i32)GetPrimaryAndSecondaryDirection(                //TTM
                                      mapX, mapY, w_30, w_32);      //TTM
    PreferredDirection = (D0W + 2) & 3;                             //TTM
    d.SecondaryDirection =                                          //TTM
       (DIRECTION)((d.SecondaryDirection+2)&3);//180 degrees        //TTM
    timePerMove = sw(timePerMove - timePerMove/4);                  //TTM
    Try_Preferred_Direction_Then_Others(                            //TTM
                       PreferredDirection,       //Not Modified     //TTM
                       pI16A2,                   //modified         //TTM
                       DB4A3,                    //modified         //TTM
                       timer_70,                 //modified         //TTM
                       i_60,                     //Not Modified     //TTM
                       w_54,                     //Not Modified     //TTM
                       timePerMove,              //Not Modified     //TTM
                       w_32,                     //Not Modified     //TTM
                       w_30,                     //Not Modified     //TTM
                       mtDescLocal,              //const            //TTM
                       mapX,                     //const            //TTM
                       mapY,                     //const            //TTM
                       invincible,               //const            //TTM
                       timeUntilAlternateUpdate, //const            //TTM
                       monster,                  //const            //TTM
                       timeFunc,                 //const            //TTM
                       indexOfLastMonster,       //const            //TTM
                       horizSize,                //const            //TTM
                       fearFactorIs6,            //const            //TTM
                       true, //RFWAP             //const            //TTM
                       processingATimer,         //const            //TTM
                       attackDistance,           //const            //TTM
                       mmr                                          //TTM
                );                                                  //TTM
    return;                                                         //TTM
  };                                                                //TTM
                                                                    //TTM
//tag00d79e:                                                        //TTM
  TIMERTRACE(0xd79e);                                               //TTM
  //bool_36 = false;                                                //TTM
  D3W = 0;                                                          //TTM
  //30OCT02SETWBITS0_3(DB4A3->word14, D3W);                         //TTM
  DB4A3->fear(StateOfFear0);                                        //TTM
  Turn_Monsters_As_Group(                                           //TTM
                  D5,                                               //TTM
                  pI16A2,                                           //TTM
                  w_54,               //const                       //TTM
                  indexOfLastMonster, //const                       //TTM
                  horizSize,          //const                       //TTM
                  false,              //const                       //TTM
                  mmr                                               //TTM
                  );                                                //TTM
  if (timeFunc < -1)                                                //TTM
  {                                                                 //TTM
    return;                                                         //TTM
  };                                                                //TTM
  Increment_Time_By_W52_Plus_Random(timer_70, timePerMove);         //TTM
  StartBGroupTimer(                                                 //TTM
         timer_70,                                                  //TTM
         pI16A2,                                                    //TTM
         i_60,                                                      //TTM
         timeUntilAlternateUpdate,  //const                         //TTM
         processingATimer           //const                         //TTM
         );                                                         //TTM
}                                                                   //TTM


//******************************************************************//T524
// This was extracted from ProcessTimers29to41 so that the code     //T524
// could be 'unwound' a bit.  This code is used in several places   //T524
// and used to be reached via 'goto' instructions.                  //T524
//******************************************************************//T524
void Try_Directions_D5_To_D4(                                       //T524
                 ITEM16        *pI16A2,                             //T524
                 dReg           D4,                                 //T524
                 dReg           D5,                                 //T524
                 DB4           *DB4A3,                              //T524
                 TIMER&         timer_70,                           //T524
                 i32            i_60,                               //T524
                 i16            w_54,                               //T524
                 i16            timePerMove,                        //T524
                 i16            w_44,                               //T524
                 bool           bool_36,                            //T524
           const TIMERTYPE&     timeFunc,                           //T524
           const bool           invincible,                         //T524
           const i16            timeUntilAlternateUpdate,           //T524
           const RN             monster,                            //T524
           const MONSTERDESC&   mtDescLocal,                        //T524
           const i32            mapX,                               //T524
           const i32            mapY,                               //T524
           const i16            indexOfLastMonster,                 //T524
           const i16            horizSize,                          //T524
           const bool           fearFactorIs6,                      //T524
           const bool           processingATimer,                   //T524
           const i16            attackDistance,                     //T524
                 MMRECORD      *pmmr                                //T524
           )                                                        //T524
{                                                                   //T524
  dReg D0;                                                          //T524
  i16 w_30, w_32;                                                   //T524
  i32 PreferredDirection;                                           //T524
  do                                                                //T524
  {                                                                 //T524
    TIMERTRACE(0xd28a);                                             //T524
//tag00d28a:                                                        //T524
    w_30 = sw(mapX);                                                //T524
    w_32 = sw(mapY);                                                //T524
    w_30 = sw(w_30 + d.DeltaX[D5W]);                                //T524
    w_32 = sw(w_32 + d.DeltaY[D5W]);                                //T524
    if (pI16A2->ubPrevX == w_30)                                    //T524
    {                                                               //T524
      TIMERTRACE(0xd2ca);                                           //T524
      if (pI16A2->ubPrevY == w_32)                                  //T524
      {                                                             //T524
        w_44 = STRandom0_3() == 0;                                  //T524
        TIMERTRACE(0xd2e2);                                         //T524
        if (w_44 == 0) goto tag00d36e;                              //T524
      };                                                            //T524
    };                                                              //T524
    D0W = PossibleMove(&mtDescLocal, mapX, mapY, D5W, 0);           //T524
    TIMERTRACE(0xd2fe);                                             //T524
    if (D0W != 0)                                                   //T524
    {                                                               //T524
      //while (from d728)                                           //T524
      //while (from d630)                                           //T524
      pmmr->Setflg(T524_possibleMove);                              //T524 
      TIMERTRACE(0xd302);                                           //T524
      D4W = sw(timePerMove/2 - w_54);                               //T524
      bool_36 = D4W <= 0;                                           //T524
      if (bool_36)                                                  //T524
      {                                                             //T524
        TIMERTRACE(0xd31c);                                         //T524
        D0W = MoveObject(monster, mapX, mapY, w_30, w_32,NULL,pmmr);//T524
        ASSERT(D0W != 0xccc,"moveObj");                             //T524
        if (D0W != 0)                                               //T524
        {                                                           //T524
          pmmr->Setflg(T524_moveSucceeded);                         //T524
          return;                                                   //T524
        };                                                          //T524
        pmmr->Setflg(T524_moveFailed);                              //T524
        timer_70.timerUByte6(ub(d.NewX));                           //T524
        timer_70.timerUByte7(ub(d.NewY));                           //T524
        pI16A2->ubPrevX = ub(mapX);                                 //T524
        pI16A2->ubPrevY = ub(mapY);                                 //T524
        pI16A2->uByte4 = (ui8)d.Time;                               //T524
      }                                                             //T524
      else                                                          //T524
      {                                                             //T524
        TIMERTRACE(0xd362);                                         //T524
        timePerMove = D4W;                                          //T524
        w_54 = -1;                                                  //T524
      };                                                            //T524
        pmmr->Setflg(T524_processInvincibleAndFinish);              //T524
        Process_Invincible_And_Finish(                              //T524
                          D5,                       //modified      //T524
                          DB4A3,                    //modified      //T524
                          PreferredDirection,       //modified      //T524
                          pI16A2,                   //modified      //T524
                          timer_70,                 //modified      //T524
                          i_60,                     //Not Modified  //T524
                          w_54,                     //Not Modified  //T524
                          timePerMove,              //Not Modified  //T524
                          bool_36,                  //Not Modified  //T524
                          mapX,                     //const         //T524
                          mapY,                     //const         //T524
                          timeFunc,                 //const         //T524
                          monster,                  //const         //T524
                          timeUntilAlternateUpdate, //const         //T524
                          mtDescLocal,              //const         //T524
                          invincible,               //const         //T524
                          indexOfLastMonster,       //const         //T524
                          horizSize,                //const         //T524
                          fearFactorIs6,            //const         //T524
                          processingATimer,         //const         //T524
                          attackDistance,           //const         //T524
                          pmmr                                      //T524
            );                                                      //T524
      return;                                                       //T524
    };                                                              //T524
    tag00d36e:                                                      //T524
    TIMERTRACE(0xd36e);                                             //T524
    if (d.moveOntoPartyPosition != 0)                               //T524
    {                                                               //T524
      TIMERTRACE(0xd374);                                           //T524
      if (timeFunc != -3)                                           //T524
      {                                                             //T524
        TIMERTRACE(0xd37c);                                         //T524
        if (   ((timeUntilAlternateUpdate & 15)  != 5)              //T524
            || (FindPossibleMove(&mtDescLocal, mapX, mapY, 0)==0)   //T524
            || (STRandomBool() != 0) )                              //T524
        {                                                           //T524
          pmmr->Setflg(T524_mabeDeleteTimersFear6TurnIndividuals);  //T524
          MaybeDelTimers_Fear6_TurnIndividuals(                     //T524
                      pI16A2,                                       //T524
                      DB4A3,                                        //T524
                      timer_70,                                     //T524
                      timeUntilAlternateUpdate,//const              //T524
                      timeFunc,                //const              //T524
                      mapX,                    //const              //T524
                      mapY,                    //const              //T524
                      mtDescLocal,             //const              //T524
                      indexOfLastMonster,      //const              //T524
                      horizSize,               //const              //T524
                      processingATimer,        //const              //T524
                      pmmr                                          //T524
                      );                                            //T524
          return;                                                   //T524
        };                                                          //T524
        TIMERTRACE(0xd38a);                                         //T524
      };                                                            //T524
      pI16A2->uByte6 = ub(d.partyX);                                //T524
      pI16A2->uByte7 = ub(d.partyY);                                //T524
    };                                                              //T524
    D5W = (I16)((D5W + 1) & 3); // Next direction to try.           //T524
  } while (D4W != D5W);                                             //T524
                                                                    //T524
                                                                    //T524
  pmmr->Setflg(T524_ProcessInvincibleAndFinish);                    //T524
  Process_Invincible_And_Finish(                                    //T524
                           D5,                       //modified     //T524
                           DB4A3,                    //modified     //T524
                           PreferredDirection,       //modified     //T524
                           pI16A2,                   //modified     //T524
                           timer_70,                 //modified     //T524
                           i_60,                     //Not Modified //T524
                           w_54,                     //Not Modified //T524
                           timePerMove,              //Not Modified //T524
                           bool_36,                  //Not Modified //T524
                           mapX,                     //const        //T524
                           mapY,                     //const        //T524
                           timeFunc,                 //const        //T524
                           monster,                  //const        //T524
                           timeUntilAlternateUpdate, //const        //T524
                           mtDescLocal,              //const        //T524
                           invincible,               //const        //T524
                           indexOfLastMonster,       //const        //T524
                           horizSize,                //const        //T524
                           fearFactorIs6,            //const        //T524
                           processingATimer,         //const        //T524
                           attackDistance,           //const        //T524
                           pmmr                                     //T524
    );                                                              //T524
  return;                                                           //T524
}                                                                   //T524


//*********************************************************         //D4D0
//                                                                  //D4D0
//*********************************************************         //D4D0
void D4D0Finish(                                                    //D4D0
                   DB4           *DB4A3,                            //D4D0
                   TIMER&         timer_70,                         //D4D0
                   ITEM16        *pI16A2,                           //D4D0
                   i32            i_60,                             //D4D0
                   i16            w_54,               //modified    //D4D0
                   i16            timePerMove,                      //D4D0
                   i16            w_32,                             //D4D0
                   i16            w_30,                             //D4D0
             const i32            mapX,                             //D4D0
             const i32            mapY,                             //D4D0
             const TIMERTYPE&     timeFunc,                         //D4D0
             const RN             monster,                          //D4D0
             const bool           invincible,                       //D4D0
             const i16            timeUntilAlternateUpdate,         //D4D0
             const MONSTERDESC&   mtDescLocal,                      //D4D0
             const i16            indexOfLastMonster,               //D4D0
             const i16            horizSize,                        //D4D0
             const bool           fearFactorIs6,                    //D4D0
             const bool           processingATimer,                 //D4D0
             const i16            attackDistance,                   //D4D0
                   MMRECORD      *mmr                               //D4D0
                )                                                   //D4D0
{                                                                   //D4D0
  i32 PreferredDirection;                                           //D4D0
  TIMERTRACE(0xd4d0);                                               //D4D0
  timePerMove = sw((timePerMove + 1)/2);                            //D4D0
  pI16A2->uByte6 = ub(d.partyX);                                    //D4D0
  w_30 = d.partyX;                                                  //D4D0
  pI16A2->uByte7 = ub(d.partyY);                                    //D4D0
  w_32 = d.partyY;                                                  //D4D0
                                                                    //D4D0
//tag00d546:                                                        //D4D0
  TIMERTRACE(0xd546);                                               //D4D0
  PreferredDirection = GetPrimaryAndSecondaryDirection(             //D4D0
                                          mapX, mapY, w_30, w_32);  //D4D0
  // Direction to approach party.                                   //D4D0
//tag00d55c:                                                        //D4D0
  Try_Preferred_Direction_Then_Others(                              //D4D0
                      PreferredDirection,       //Not Modified      //D4D0
                      pI16A2,                   //modified          //D4D0
                      DB4A3,                    //modified          //D4D0
                      timer_70,                 //modified          //D4D0
                      i_60,                     //Not Modified      //D4D0
                      w_54,                     //Not Modified      //D4D0
                      timePerMove,              //Not Modified      //D4D0
                      w_32,                     //Not Modified      //D4D0
                      w_30,                     //Not Modified      //D4D0
                      mtDescLocal,              //const             //D4D0
                      mapX,                     //const             //D4D0
                      mapY,                     //const             //D4D0
                      invincible,               //const             //D4D0
                      timeUntilAlternateUpdate, //const             //D4D0
                      monster,                  //const             //D4D0
                      timeFunc,                 //const             //D4D0
                      indexOfLastMonster,       //const             //D4D0
                      horizSize,                //const             //D4D0
                      fearFactorIs6,            //const             //D4D0
                      true, //RFWAP             //const             //D4D0
                      processingATimer,         //const             //D4D0
                      attackDistance,           //const             //D4D0
                      mmr                                           //D4D0
                );                                                  //D4D0
}                                                                   //D4D0

//*********************************************************         //TU67
//                                                                  //TU67
//*********************************************************         //TU67
void TryUByte67(                                                    //TU67
                dReg&          D5,                                  //TU67
                i32&           PreferredDirection,                  //TU67
                ITEM16        *pI16A2,                              //TU67
                DB4           *DB4A3,                               //TU67
                TIMER&         timer_70,                            //TU67
                i32            i_60,                                //TU67
                i16            w_54,                                //TU67
                i16            timePerMove,                         //TU67
                i16            w_32,                                //TU67
                i16            w_30,                                //TU67
          const i32            mapX,                                //TU67
          const i32            mapY,                                //TU67
          const i16            timeUntilAlternateUpdate,            //TU67
          const TIMERTYPE&     timeFunc,                            //TU67
          const RN             monster,                             //TU67
          const bool           invincible,                          //TU67
          const MONSTERDESC&   mtDescLocal,                         //TU67
          const i16            indexOfLastMonster,                  //TU67
          const i16            horizSize,                           //TU67
          const bool           fearFactorIs6,                       //TU67
          const bool           processingATimer,                    //TU67
          const i16            attackDistance,                      //TU67
                MMRECORD      *mmr                                  //TU67
                )                                                   //TU67
{                                                                   //TU67
  dReg D3;                                                          //TU67
  TIMERTRACE(0xd500);                                               //TU67
  w_30 = pI16A2->uByte6;                                            //TU67
  w_32 = pI16A2->uByte7;                                            //TU67
  if (mapX == w_30)                                                 //TU67
  {                                                                 //TU67
    if (mapY == w_32)                                               //TU67
    {                                                               //TU67
      TIMERTRACE(0xd528);                                           //TU67
      //bool_36 = false;                                            //TU67
      D3W = 0;                                                      //TU67
      //30OCT02SETWBITS0_3(DB4A3->word14,D3W);                      //TU67
      DB4A3->fear(StateOfFear0);                                    //TU67
      Turn_Monsters_As_Group(                                       //TU67
                      D5,                                           //TU67
                      pI16A2,                                       //TU67
                      w_54,               //const                   //TU67
                      indexOfLastMonster, //const                   //TU67
                      horizSize,          //const                   //TU67
                      false,              //const                   //TU67
                      mmr                                           //TU67
                      );                                            //TU67
      if (timeFunc < -1)                                            //TU67
      {                                                             //TU67
        TIMERTRACE(0xd45c);                                         //TU67
        return;                                                     //TU67
      };                                                            //TU67
      Increment_Time_By_W52_Plus_Random(timer_70, timePerMove);     //TU67
      StartBGroupTimer(                                             //TU67
            timer_70,                                               //TU67
            pI16A2,                                                 //TU67
            i_60,                                                   //TU67
            timeUntilAlternateUpdate, //const                       //TU67
            processingATimer          //const                       //TU67
            );                                                      //TU67
      return;                                                       //TU67
    };                                                              //TU67
  };                                                                //TU67
  //RFWAP = true;                                                   //TU67
                                                                    //TU67
//tag00d546:                                                        //TU67
  TIMERTRACE(0xd546);                                               //TU67
  PreferredDirection = GetPrimaryAndSecondaryDirection(             //TU67
                                        mapX, mapY, w_30, w_32);    //TU67
  // Direction to approach party.                                   //TU67
//tag00d55c:                                                        //TU67
  Try_Preferred_Direction_Then_Others(                              //TU67
                      PreferredDirection,       //Not Modified      //TU67
                      pI16A2,                   //modified          //TU67
                      DB4A3,                    //modified          //TU67
                      timer_70,                 //modified          //TU67
                      i_60,                     //Not Modified      //TU67
                      w_54,                     //Not Modified      //TU67
                      timePerMove,              //Not Modified      //TU67
                      //bool_36,                //Not Modified      //TU67
                      w_32,                     //Not Modified      //TU67
                      w_30,                     //Not Modified      //TU67
                      mtDescLocal,              //const             //TU67
                      mapX,                     //const             //TU67
                      mapY,                     //const             //TU67
                      invincible,               //const             //TU67
                      timeUntilAlternateUpdate, //const             //TU67
                      monster,                  //const             //TU67
                      timeFunc,                 //const             //TU67
                      indexOfLastMonster,       //const             //TU67
                      horizSize,                //const             //TU67
                      fearFactorIs6,            //const             //TU67
                      true, //RFWAP             //const             //TU67
                      processingATimer,         //const             //TU67
                      attackDistance,           //const             //TU67
                      mmr                                           //TU67
                      );                                            //TU67
}                                                                   //TU67



//*********************************************************         //IC
//                                                                  //IC
//*********************************************************         //IC
bool InitialChecks(                                                 //IC
                TIMER&         timer_70,                            //IC
                i16&           w_54,                                //IC
                i16&           timePerMove,                         //IC
                i16&           attackDistance,                      //IC
          const RN             monster,                             //IC
          const i16            timeUntilAlternateUpdate,            //IC
          const TIMERTYPE&     timeFunc,                            //IC
          const i32            mapX,                                //IC
          const i32            mapY,                                //IC
                MMRECORD      *pmmr                                 //IC
                   )                                                //IC
{                                                                   //IC
  dReg D0, D3, D5;                                                  //IC
  DB4 *DB4A3;                                                       //IC
  i16 newX, newY;//w_30, w_32;                                      //IC
  bool invincible;                                                  //IC
  ITEM16      staticItem16;                                         //IC
  ITEM16      *pI16A2;                                              //IC
  MONSTERTYPE mtD0;                                                 //IC
  MONSTERDESC *pmtDesc;                                             //IC
  MONSTERDESC mtDescLocal;                                          //IC
  if (d.LoadedLevel != d.partyLevel)                                //IC
  {                                                                 //IC
    TIMERTRACE(0xcbe0);                                             //IC
    D5L = timeFunc;                                                 //IC
    if (timeFunc != TT_MONSTER_Bgroup)                              //IC
    {                                                               //IC
      if (timeFunc != TT_MONSTER_Agroup)                            //IC
      {                                                             //IC
        if (timeFunc != TT_MONSTER_B0)                              //IC
        {                                                           //IC
          if (timeFunc != TT_MONSTER_A0)                            //IC
          {                                                         //IC
            pmmr->Setflg(IC29to41_Not32_33_37_38);                  //IC
            if (AITraceActive)                                      //IC
            {                                                       //IC
              fprintf(GETFILE(TraceFile),                           //IC
                  "Skip monster AI because not party level "        //IC
                  " and timer is not for group leader\n");          //IC
            };                                                      //IC
            return true;                                            //IC
          };                                                        //IC
        };                                                          //IC
      };                                                            //IC
    };                                                              //IC
  };                                                                //IC
  TIMERTRACE(0xcc02);                                               //IC
  //monster = FindFirstMonster(mapX, mapY);                         //IC
        //                                                          //IC
		// The following three words can be patched to put              //IC
		// monster at d.Obj13020 for TAG00c442.  See the                //IC
		// comments at TAG00c442.                                       //IC
		// As follows:                                                  //IC
		// 	00cc14  67e8	     beq     <cbfe>  // return via            //IC
    //                                     //nearby long-jump       //IC
		// 	00cc16  3940 cd24  move.w  D0,-13020(A4)                    //IC
		//                                                              //IC
  if (monster == RNeof)                                             //IC
  {                                                                 //IC
    return true;                                                    //IC
  };                                                                //IC
  d.LastMonsterMoved = monster;                                     //IC
  DB4A3 = GetRecordAddressDB4(monster);                             //IC
  ////////////////////////////////////////////////////              //IC
  // If any of the monsters in the group has zero HP then           //IC
  // we put the timer back into the queue and call DamageMonster(). //IC
  // That will remove the monster from the game (and the whole      //IC
  // group if the number goes to zero).                             //IC
  // Then, if any are left, the timer will expire again and         //IC
  // we will come back here to process it.                          //IC
  ////////////////////////////////////////////////////              //IC
  if (   (timeFunc != TT_M1)                                        //IC
       &&(timeFunc != TT_M2)                                        //IC
       &&(timeFunc != TT_M3) )                                      //IC
  {                                                                 //IC
    i32 numMon, i;                                                  //IC
    numMon = DB4A3->numMonM1() + 1;                                 //IC
    for (i=0; i<numMon; i++)                                        //IC
    {                                                               //IC
      if (DB4A3->hitPoints6[i] == 0)                                //IC
      {                                                             //IC
        pmmr->Setflg(IC29to41_atLeastOneMemberAlreadyDead);         //IC
        if (DamageMonster(DB4A3,                                    //IC
                      i,                                            //IC
                      mapX, mapY,                                   //IC
                      0,                                            //IC
                      1,                                            //IC
                      d.LoadedLevel != d.partyLevel,                //IC
                      pmmr) != NO_MONSTER_DIED)                     //IC
        { //If any in group died.                                   //IC
          pmmr->Setflg(IC29to41_damageMonsterSucceeded);            //IC
          //timer_70.timerTime = d.Time | (d.LoadedLevel<<24);      //IC
          timer_70.Time(d.Time);                                    //IC
          timer_70.Level((ui8)d.LoadedLevel);                       //IC
          timer_70.Function(timeFunc);                              //IC
          timer_70.timerUByte5(0);                                  //IC
          timer_70.timerUByte6(ub(mapX));                           //IC
          timer_70.timerUByte7(ub(mapY));                           //IC
          timer_70.timerUByte8(ub(timeUntilAlternateUpdate));       //IC
          gameTimers.SetTimer(&timer_70);                           //IC
          return true;                                              //IC
        };                                                          //IC
      };                                                            //IC
    };                                                              //IC
    if (pmmr->Getflg(IC29to41_atLeastOneMemberAlreadDead))          //IC
            pmmr->Setflg(IC29to41_damageMonsterFailed);             //IC
  };                                                                //IC
  ////////////////////////////////////////////////////              //IC
  // All the monsters in this group have non-zero HP.               //IC
  ////////////////////////////////////////////////////              //IC
  if (AITraceActive)                                                //IC
  {                                                                 //IC
    fprintf(GETFILE(TraceFile),                                     //IC
      "      MAI All memebers of the group are alive\n");           //IC
  };                                                                //IC
  //ASSERT(DB4A3->uByte5 != 0xff);                                  //IC
  TIMERTRACE(0xcc26);                                               //IC
  TIMERTRACE(DB4A3->monsterType());                                 //IC
  mtD0 = DB4A3->monsterType();                                      //IC
                                                                    //IC
  pmtDesc = &d.MonsterDescriptor[mtD0];                             //IC
  //A0 = (pnt)&Item26_26;                                           //IC
  memmove((pnt)&mtDescLocal, (pnt)pmtDesc, 26);                     //IC
  //                                                                //IC
  // The following three words can be patched to ensure             //IC
  // that attackDistance is initialized.  Under rare circumstances, //IC
  // attackDistance does not get initialized.  This only causes     //IC
  // slightly erratic behaviour of the monster, but it makes        //IC
  // it nearly impossible to exactly match the PC code to           //IC
  // the Atari code.  So I have patched the Atari code              //IC
  // to set attackDistance to a (relatively) large number.          //IC
  // In general, the uninitialized value is part of an              //IC
  // address and is a large, positive number.  As follows:          //IC
  //                                                                //IC
  //    00cc4c  7618         moveq.l #24,D3     patch was 263c      //IC
  //    00cc4e  3d43 ffde    move.w  D3,-34(A6) patch was 0000 0018 //IC
  //                                                                //IC
  attackDistance = 24;                                              //IC
  //timer_70.timerTime = d.Time | (d.LoadedLevel<<24);              //IC
  timer_70.Time(d.Time);                                            //IC
  timer_70.Level((ui8)d.LoadedLevel);                               //IC
  D3W = mtDescLocal.timePerMove();                                  //IC
  timer_70.timerUByte5(ub(255 - D3W));                              //IC
  timer_70.timerUByte6(ub(mapX));                                   //IC
  timer_70.timerUByte7(ub(mapY));                                   //IC
  timer_70.timerUByte8(0);// This was also patched into the         //IC
                      // Atari binary code.  Sometimes a            //IC
                      // timer entry was created with garbage       //IC
                      // in uByte8.  It only caused an undefined    //IC
                      // delay but made it impossible to exactly    //IC
                      // match my code against the Atari code.      //IC
                      // Patch:  Paul Stevens                       //IC
                      //  cc6a  1d47 ffc0  move.b D7,-64(A6)        //IC
                      //                            patch was 3007  //IC
                      //  cc6e  1d46 ffc1  move.b D6,-63(A6)        //IC
                      //                            patch was ffc0  //IC
                      //  cc72  422e ffc2  clr.b  -62(A6)           //IC
                      //                            patch was ffc1  //IC
  if (d.LoadedLevel != d.partyLevel)                                //IC
  {                                                                 //IC
    TIMERTRACE(0xcc88);                                             //IC
    D5W = STRandom0_3();                                            //IC
    TIMERTRACE(0xcc94);                                             //IC
    D0W = PossibleMove(&mtDescLocal, mapX, mapY, D5W, 0);           //IC
    if (AITraceActive)                                              //IC
    {                                                               //IC
      fprintf(GETFILE(TraceFile),                                   //IC
        "      MAI Not on party level. PossibleMove(Random()=%s) "  //IC
        "returned %s.\n",d.DirectionNames[D5W], D0W?"true":"false");//IC
    };                                                              //IC
    if (D0W != 0)                                                   //IC
    {                                                               //IC
      TIMERTRACE(0xcca0);                                           //IC
      newX = sw(mapX + d.DeltaX[D5W]);                              //IC
      newY = sw(mapY + d.DeltaY[D5W]);                              //IC
      TIMERTRACE(0xccce);                                           //IC
      TIMERTRACE(monster.ConvertToInteger());                       //IC
      D0W = MoveObject(monster, mapX, mapY, newX, newY, NULL, pmmr);//IC
      if (AITraceActive)                                            //IC
      {                                                             //IC
        fprintf(GETFILE(TraceFile),                                 //IC
          "      MAI MoveObject to (%02d,%02d) returned %d\n",      //IC
          newX,newY,D0W);                                           //IC
      };                                                            //IC
      ASSERT(D0W != 0xccc,"IC");                                    //IC
      if (D0W != 0)                                                 //IC
      {                                                             //IC
        pmmr->Setflg(IC29to41_randomMoveSucceeded);                 //IC
        if (AITraceActive)                                          //IC
        {                                                           //IC
          fprintf(GETFILE(TraceFile),                               //IC
            "      MAI Skip monster AI because "                    //IC
            "MoveObject non-zero\n");                               //IC
        };                                                          //IC
        return true;                                                //IC
      };                                                            //IC
      timer_70.timerUByte6(ub(d.NewX));                             //IC
      timer_70.timerUByte7(ub(d.NewY));                             //IC
      pmmr->Setflg(IC29to41_randomMoveFailed);                      //IC
    }                                                               //IC
    else                                                            //IC
    {//IC                                                           //IC
      if (AITraceActive)                                            //IC
      {                                                             //IC
        fprintf(GETFILE(TraceFile),                                 //IC
          "      MAI  No attempt to move monster. "                 //IC                  
          "Que 37 timer\n");                                        //IC
      };                                                            //IC
    };                                                              //IC
//  tag00cd04:                                                      //IC
    TIMERTRACE(0xcd04);                                             //IC
    timer_70.Function(TT_MONSTER_Bgroup);                           //IC
    D0W = sw(abs(d.LoadedLevel-d.partyLevel));                      //IC
    D5W = sw(Larger(D0W << 4, 2*(UI8)(mtDescLocal.timePerMove()))); //IC
    if (AITraceActive)                                              //IC
    {                                                               //IC
      fprintf(GETFILE(TraceFile),                                   //IC
        "      MAI Setting timer 37.  Time equals:\n");             //IC
      fprintf(GETFILE(TraceFile),                                   //IC
        "      MAI   16 * difference in levels = %d\n", 16*D0W);    //IC
      fprintf(GETFILE(TraceFile),                                   //IC
        "      MAI   2*MonsterType->timePerMove = %d\n",            //IC
               2*mtDescLocal.timePerMove());                        //IC
      fprintf(GETFILE(TraceFile),                                   //IC
        "      MAI   The larger number.....equals %d\n", D5W);      //IC
    };                                                              //IC
  //tag00cd30:                                                      //IC
    TIMERTRACE(0xcd30);                                             //IC
    //timer_70.timerTime += D5W;                                    //IC
    timer_70.Time(timer_70.Time() + D5W);                           //IC
    gameTimers.SetTimer(&timer_70);                                 //IC
    pmmr->Setflg(IC29to41_doNothing);                               //IC
    return true;                                                    //IC
  };                                                                //IC
//tag00cd46:                                                        //IC
  TIMERTRACE(0xcd46);                                               //IC
  invincible = mtDescLocal.invincible(); //Impervious?              //IC
  if (invincible)                                                   //IC
  {                                                                 //IC
    if (d.Word11694 != 0) //Has the game been won?                  //IC
    {                                                               //IC
      pmmr->Setflg(IC29to41_doNothing);                             //IC
      return true;                                                  //IC
    };                                                              //IC
    d.FluxCageCount = 0;                                            //IC
    d.FluxCages[0] = 0;                                             //IC
    d.FluxCages[1] = 0;                                             //IC
    d.FluxCages[2] = 0;                                             //IC
    d.FluxCages[3] = 0;                                             //IC
  };                                                                //IC
  ASSERT(DB4A3->groupIndex() < d.MaxITEM16,"maxitem16");            //IC
  pI16A2 = &d.Item16[DB4A3->groupIndex()];                          //IC
  if (DB4A3->groupIndex() >= d.MaxITEM16)                           //IC
    pI16A2 = &staticItem16; // Just so we won't clobber something.  //IC
  D3W = pI16A2->uByte4;                                             //IC
  w_54 = sw((d.Time&255) - D3W );                                   //IC
  if (w_54 < 0)                                                     //IC
  {                                                                 //IC
    w_54 += 256;                                                    //IC
  };                                                                //IC
  timePerMove = (UI8)(mtDescLocal.timePerMove());                   //IC
  if (timePerMove == 255)                                           //IC
  {                                                                 //IC
    timePerMove = 100;                                              //IC
  };                                                                //IC
  if ( (d.freezeLifeTimer != 0) && (!invincible) )                  //IC
  {                                                                 //IC
    pmmr->Setflg(IC29to41_doNothing);                               //IC
    TIMERTRACE(0xcdc0);                                             //IC
    if (timeFunc < TT_EMPTY)                                        //IC
    {                                                               //IC
      return true;                                                  //IC
    };                                                              //IC
    timer_70.Function(timeFunc);                                    //IC
    timer_70.timerUByte8(ub(timeUntilAlternateUpdate));             //IC
    D5W = 4;                                                        //IC
    TIMERTRACE(0xcd30);                                             //IC
    //timer_70.timerTime += D5W;                                    //IC
    timer_70.Time(timer_70.Time() + D5W);                           //IC
    gameTimers.SetTimer(&timer_70);                                 //IC
    return true;                                                    //IC
  };                                                                //IC
  TIMERTRACE(0xcde2);                                               //IC
  if (timeFunc >= TT_EMPTY)                                         //IC
  {                                                                 //IC
    pmmr->Setflg(IC29to41_exitFalse);                               //IC
    return false;                                                   //IC
  };                                                                //IC
  TIMERTRACE(0xcde8);                                               //IC
  switch (timeFunc)                                                 //IC
  {                                                                 //IC
  case TT_M3:           timer_70.Function(TT_29);                   //IC
                        break;                                      //IC
  case TT_M2:           timer_70.Function(TT_30);                   //IC
                        break;                                      //IC
  case TT_M1:           timer_70.Function(TT_31);                   //IC
                        break;                                      //IC
  default: SystemError((UI16)0xcdf0);                               //IC
  };                                                                //IC
  //timer_70.function = ub(timeFunc + 32);                          //IC
//                                                                  //IC
  if (timeFunc == TT_M1)                                            //IC
  {                                                                 //IC
    D5W = 1;                                                        //IC
  }                                                                 //IC
  else                                                              //IC
  {                                                                 //IC
    D5W = sw((timePerMove+2)/4 - w_54);                             //IC
    if (D5W < 1)                                                    //IC
    {                                                               //IC
      D5W = 1;                                                      //IC
    };                                                              //IC
  };                                                                //IC
  TIMERTRACE(0xcd30);                                               //IC
  //timer_70.timerTime += D5W;                                      //IC
  timer_70.Time(timer_70.Time() + D5W);                             //IC
  if (AITraceActive)                                                //IC
  {                                                                 //IC
    fprintf(GETFILE(TraceFile),                                     //IC
      "      MAI Change negative timer function to type %d\n",      //IC
                       timer_70.Function());                        //IC
  };                                                                //IC
  gameTimers.SetTimer(&timer_70);                                   //IC
  pmmr->Setflg(IC29to41_doNothing);                                 //IC
  return true;                                                      //IC
}                                                                   //IC


MONSTERMOVEFILTERCACHE monsterMoveFilterCache;

MONSTERMOVEFILTERCACHE::MONSTERMOVEFILTERCACHE(void)
{
  ui32 i;
  global.partyLevelOnly = 0xfffe;
  global.filterObj = RNeof;
  for (i=0; i<64; i++)
  {
    mmfloc[i].partyLevelOnly = 0xfffe; //Not checked
    mmfloc[i].filterObj = RNeof;
  };
}

MONSTERMOVEFILTERCACHE::~MONSTERMOVEFILTERCACHE(void)
{
}

MONSTERMOVEFILTERLOCATION *MONSTERMOVEFILTERCACHE::GetLocation(i32 level)
{
  if (mmfloc[level].partyLevelOnly == 0xfffe)  
                            // We have not yet checked
                            // to see if a filter location
                            // is defined.
  {
    ui32 key, *pRecord;
    i32 len;
    mmfloc[level].partyLevelOnly = 0xffff; //No file location exists
    // First check for location defined for this level.
    key = (EDT_SpecialLocations<<24)|ESL_MONSTERMOVEFILTER|((level+1)<<8);
    len = expool.Locate(key,&pRecord);
    if (len > 0)
    //*****************************************
    {
      mmfloc[level].locr.Integer((*pRecord) & 0x03ffff);
      mmfloc[level].partyLevelOnly = (ui16)(((*pRecord) & 0x040000) >> 18);
      mmfloc[level].maxDistance = (ui16)(((*pRecord) & 0xf80000) >> 19);
      if (mmfloc[level].partyLevelOnly < 2)
      {
        RN obj;
        i32 objectDB, actuatorType;
        DB3 * pActuator;
        for (obj = FindFirstObject(
                        mmfloc[level].locr.l,
                        mmfloc[level].locr.x,
                        mmfloc[level].locr.y);
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
              mmfloc[level].filterObj = obj;
              break;
            };
          };
        };
      };
    };
    if (mmfloc[level].partyLevelOnly == 0xffff) // Still nothing. check global setting
    {
      if (global.partyLevelOnly == 0xfffe)
      {
        global.partyLevelOnly = 0xffff;
        key = (EDT_SpecialLocations<<24)|ESL_MONSTERMOVEFILTER;
        len = expool.Locate(key,&pRecord);
        if (len > 0)
        //*****************************************
        {
          RN obj;
          i32 objectDB, actuatorType;
          DB3 * pActuator;
          global.locr.Integer((*pRecord) & 0x03ffff);
          global.partyLevelOnly = (ui16)(((*pRecord) & 0x040000) >> 18);
          global.maxDistance = (ui16)(((*pRecord) & 0xf80000) >> 19);
          for (obj = FindFirstObject(
                          global.locr.l,
                          global.locr.x,
                          global.locr.y);
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
                global.filterObj = obj;
                break;
              };
            };
          };
        };
      };
      mmfloc[level].locr           = global.locr;
      mmfloc[level].partyLevelOnly = global.partyLevelOnly;
      mmfloc[level].maxDistance    = global.maxDistance;
      mmfloc[level].filterObj      = global.filterObj;
    };
  };
  return &mmfloc[level];
}


void ProcessPostMonsterMoveFilter(MONSTERMOVEFILTERLOCATION *pmmfloc, 
                                  MMRECORD *pmmr)
{
  if (!pmmr->CalledPreMoveFilter) return;
  {
    NEWDSAPARAMETERS ndp;

    if (   (pmmr->locr.l != pmmr->finalLocr.l)
         ||(pmmr->locr.x != pmmr->finalLocr.x)
         ||(pmmr->locr.y != pmmr->finalLocr.y))
    {
      pmmr->Setflg(MonMove_differentCell);
    };
    //int currentLevel;
    //currentLevel = d.LoadedLevel;
    //LoadLevel(locr.l);
    pmmr->timer.timerUByte9(1);//timerFunction
    pmmr->timer.timerUByte8(0);//timerPosition
  
    //**********************************
    // Some Debugging
    //**********************************
#ifdef _DEBUG
    {
      RN obj;
      if (pmmr->finalLocr.l == -1)
      {
        obj = FindFirstMonster(pmmr->locr.l,pmmr->locr.x,pmmr->locr.y);
        ASSERT( obj == RNeof,
               "Monster failed to disappear");
      }
      else
      {
        obj = FindFirstMonster(pmmr->finalLocr.l,pmmr->finalLocr.x,pmmr->finalLocr.y);
        ASSERT( obj == pmmr->objMonster,
               "Monster not at expected location");
      };
    };
#endif
    //**********************************
    //  Set information for DSA
    //**********************************
    pDSAparameters[1+0] = pmmr->finalLocr.l;
    pDSAparameters[1+1] = pmmr->finalLocr.x;
    pDSAparameters[1+2] = pmmr->finalLocr.y;
    pDSAparameters[1+3] = pmmr->objMonster.ConvertToInteger();
    pDSAparameters[1+4] = d.partyLevel;
    pDSAparameters[1+5] = d.partyX;
    pDSAparameters[1+6] = d.partyY;
    pDSAparameters[1+7] = pmmr->flgs[0];
    pDSAparameters[1+8] = pmmr->flgs[1];
    pDSAparameters[0] = 9;
    //**********************************
    //  Call the DSA
    //**********************************
  
    //memcpy(DSAparameters, &fp, sizeof(fp));
    ProcessDSAFilter(pmmfloc->filterObj, 
                    &pmmr->timer, 
                    pmmfloc->locr,
                    &monsterMoveFilterActive,
                    &pmmr->dsaVars);  // Filter location
  };
}


//*********************************************************
//
//*********************************************************
//   TAG00cbc6
void ProcessTimers29to41(
               const i32 mapX,
               const i32 mapY,
               TIMERTYPE timeFunc,
               i16 timeUntilAlternateUpdate) //From timer.uByte8
{//(void)
  ITEM16      staticItem16;
  dReg        D0, D1, D3, D4, D5;
  MONSTERTYPE mtD0;
  bool        boolD0;
  DB4         *DB4A3;
  ITEM16      *pI16A2;
  MONSTERDESC *pmtDesc;
  i16         w_110;
  i16         w_100;
  i16         w_90;
  //i16         w_80;
  TIMER       timer_70;
  i32         i_60;
  bool        invincible; //w_56
  i16         w_54;
  i16         timePerMove;
  i16         indexOfLastMonster;  //w_50;
  i16         horizSize;  //w_48;
  bool        fearFactorIs6 = false;
  i16         w_44;
  //bool        RFWAP;
  bool        processingATimer; //i16         w_40;
  i32         PreferredDirection;
  bool        bool_36;
  i16         attackDistance; // Blocked() = 0 or distance.
  i16         yDistance;//w_32;
  i16         xDistance;//w_30;
  const RN    monster = FindFirstMonster(mapX, mapY); //28
  MONSTERDESC mtDescLocal;
  STATE_OF_FEAR fearFactor;
  TIMERTYPE   tt;
  MMRECORD    mmr;
  MONSTERMOVEFILTERLOCATION *pmmfloc;
  //i32         monIdx;
  //i32 monsterDirection;
  // A timer event with function code 37 is put on
  // the queue when a character dies from lack of
  // food in the prison.  We had to fake byte 5 of
  // the entry.  We put a 255 into it.
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D3L = 0xccccc; // Uninitialized value
  //D7W = mapX;
  //D6W = mapY;
  TIMERTRACE(-1);
  TIMERTRACE(d.RandomNumber);
  if (AITraceActive)
  {
    fprintf(GETFILE(TraceFile),"MonsterAI [0x%04x] %02d(%02d,%02d)@%d Perform Initial Checks-timeFunc=%s, timeUntilAlternateUpdate=%d\n",
          monster.ConvertToInteger(),d.LoadedLevel,mapX,mapY,d.Time,TimerName(timeFunc),timeUntilAlternateUpdate);
  };



/////////////////////////////////////////////////////
//Now let the Monster Movement Filter DSA do its thing
/////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
// Clear all modifications to monster behaviour.
// The Monster Movement filter may set some of these things.
// Also called 'Monster Move Filter'  (for searches!!!!)
////////////////////////////////////////////////////////////
  monsterMoveInhibit[0] = 0; //north
  monsterMoveInhibit[1] = 0; //east
  monsterMoveInhibit[2] = 0; //south
  monsterMoveInhibit[3] = 0; //west
///////////////////////////////////////////////////////////
  pmmfloc = monsterMoveFilterCache.GetLocation(d.LoadedLevel);
  if (   (timeFunc > 0)
      && (pmmfloc->filterObj != RNeof)
      && (   (pmmfloc->partyLevelOnly == 0)
          || (d.LoadedLevel == d.partyLevel))
      && (    (mapX-d.partyX)*(mapX-d.partyX)
           +  (mapY-d.partyY)*(mapY-d.partyY)
           <= pmmfloc->maxDistance*pmmfloc->maxDistance)
     )
  {
    NEWDSAPARAMETERS ndp;
    mmr.timer.timerUByte9(0);//timerFunction
    mmr.timer.timerUByte8(0);//timerPosition
    mmr.timer.timerUByte7((ui8)mapX);
    mmr.timer.timerUByte6((ui8)mapY);
    //mmr.timer.timerTime   = d.LoadedLevel << 24;
    mmr.timer.Time(0);
    mmr.timer.Level((ui8)d.LoadedLevel);
    mmr.objMonster = monster;
    mmr.locr.l     = d.LoadedLevel;
    mmr.locr.x     = mapX;
    mmr.locr.y     = mapY;
    mmr.locr.p     = 0;
    mmr.finalLocr  = mmr.locr;

    //**********************************
    //  Set information for DSA
    //**********************************
    pDSAparameters[1+0] = d.LoadedLevel;
    pDSAparameters[1+1] = mapX;
    pDSAparameters[1+2] = mapY;
    pDSAparameters[1+3] = monster.ConvertToInteger();
    pDSAparameters[1+4] = d.partyLevel;
    pDSAparameters[1+5] = d.partyX;
    pDSAparameters[1+6] = d.partyY;
    pDSAparameters[0] = 7;
    //**********************************
    //  Call the DSA
    //**********************************

    mmr.CalledPreMoveFilter = true;
    ProcessDSAFilter(pmmfloc->filterObj, 
                     &mmr.timer, 
                     pmmfloc->locr,
                     &monsterMoveFilterActive,
                     &mmr.dsaVars);  // Filter location
  };
  


  if (InitialChecks(
                timer_70,
                w_54,                    //Modified
                timePerMove,             //Modified
                attackDistance,          //Modified
                monster,                 //const
                timeUntilAlternateUpdate,//const
                timeFunc,                //const
                mapX,                    //const
                mapY,                    //const
                &mmr
                ))
  {
    ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
    return;
  };
  DB4A3 = GetRecordAddressDB4(monster);
  mtD0 = DB4A3->monsterType();
  pmtDesc = &d.MonsterDescriptor[mtD0];
  memmove((pnt)&mtDescLocal, (pnt)pmtDesc, 26);
  pI16A2 = &d.Item16[DB4A3->groupIndex()];
  if (DB4A3->groupIndex() >= d.MaxITEM16)
    pI16A2 = &staticItem16; // Just so we won't clobber something.
  invincible = mtDescLocal.invincible(); //Impervious?
  TIMERTRACE(0xce18);
  fearFactor = DB4A3->fear();
  indexOfLastMonster = (I16)(DB4A3->numMonM1());//Number of monsters-1
  horizSize = mtDescLocal.horizontalSize(); //Size of monster 1=two squares
  D5W = sw(mapX - d.partyX);
  xDistance = sw(abs(D5W));
  D5W = sw(mapY - d.partyY);
  yDistance = sw(abs(D5W));
  d.monsterX = sw(mapX);
  d.monsterY = sw(mapY);
  //d.Obj13020 = monster;
  d.DirectionHasBeenTested[0] = 0;
  d.DirectionHasBeenTested[1] = 0;
  d.DirectionHasBeenTested[2] = 0;
  d.DirectionHasBeenTested[3] = 0;
  d.OrthogonalDistance2Party = DistanceBetweenPoints(mapX, mapY, d.partyX, d.partyY);
  d.PrimaryDirectionToParty = (I16)GetPrimaryAndSecondaryDirection(mapX, mapY, d.partyX, d.partyY);
  d.SecondaryDirectionToParty = (I16)d.SecondaryDirection;
  i_60 = 0;
  processingATimer = true;
  if (AITraceActive)
  {
    fprintf(GETFILE(TraceFile),"      MAI Passed. Invincible=%d fearFactor=%d w_54=%d indexOfLastMonster=%d horizSize=%d\n",invincible,fearFactor,w_54,indexOfLastMonster,horizSize);
  };
  ASSERT(timeFunc >= TT_29,"timeFunc");
  switch (timeFunc)
  {
    //timeFunc = sw(timeFunc-32);
  case TT_31:
      mmr.Setflg(TT29to41_TT31);
      timeFunc = TT_M1;
      TIMERTRACE(0xcedc);
      if ( (fearFactor != StateOfFear6) && (fearFactor != StateOfFear5) )
      {
        mmr.Setflg(TT29to41_fear5or6);
        if (AITraceActive)
        {
          fprintf(GETFILE(TraceFile),
              "      MAI timeFunc 31.  FearFactor not 5 or 6. So Delete all timers and set fear to 6.\n");
        };
        DeleteMonsterMovementTimers(mapX, mapY);
        MaybeDelTimers_Fear6_TurnIndividuals(
                    pI16A2,
                    DB4A3,
                    timer_70,
                    timeUntilAlternateUpdate,//const
                    timeFunc,                //const
                    mapX,                    //const
                    mapY,                    //const
                    mtDescLocal,             //const
                    indexOfLastMonster,      //const
                    horizSize,               //const
                    true, //processingATimer,        //const
                    &mmr
                    );
        ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
        return;
      };
      if (AITraceActive)
      {
        fprintf(GETFILE(TraceFile),"      MAI timeFunc 31.  FearFactor is 5 or 6.\n");
      };
      TIMERTRACE(0xcef6);
      pI16A2->uByte6 = ub(d.partyX);
      pI16A2->uByte7 = ub(d.partyY);
      ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
      return;

  case TT_30:
      mmr.Setflg(TT29to41_TT30);
      TIMERTRACE(0xcf14);
      timeFunc = TT_M2;
      if ( (fearFactor == StateOfFear6) || (fearFactor == StateOfFear5) )
      {
        if (AITraceActive)
        {
          fprintf(GETFILE(TraceFile),"      MAI timeFunc 30.  FearFactor is 5 or 6.\n");
        };
        mmr.Setflg(TT29to41_fear5or6doNothing);
        ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
        return;
      };
      D5L = ((fearFactor==StateOfFear2)||(fearFactor==StateOfFear3))?1:0;
      if ( (D5W != 0) || (STRandom0_3() != 0) )
      {
        TIMERTRACE(0xcf50);
        D0W = Blocked(DB4A3, -1, mapX, mapY);
                  //  Checks each member for 'looking our way'      
        if (AITraceActive)
        {
          fprintf(GETFILE(TraceFile),"      MAI timeFunc 31. FearFactor 2 or 3 or random. Block=%d\n", D0W);
        };
        if (D0W == 0)
        {
          TIMERTRACE(0xcd5c);
          bool_36 = false;
          fearFactorIs6 = false;
          Turn_Monsters_As_Group(
                          D5,
                          pI16A2,
                          w_54,               //const
                          indexOfLastMonster, //const//number of monsters
                          horizSize,          //const//Must turn as group flag
                          bool_36,            //const
                          &mmr
                          );
          mmr.Setflg(TT29to41_TT30turnAsGroup);
          if (timeFunc < -1)
          {
            TIMERTRACE(0xd45c);
            if (bool_36 == false)
            {
              ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
              return;
            };
            mmr.Setflg(TT29to41_TT30deleteTimersMaybeSetFear);
            Delete_Timers_Maybe_Set_Fear(
                                         DB4A3,
                                         pI16A2,
                                         mapX,
                                         mapY,
                                         fearFactorIs6
                                         );
          };
          mmr.Setflg(TT29to41_TT30IncrementTimeByW52PlusRandom);
          Increment_Time_By_W52_Plus_Random(timer_70, timePerMove);
          StartBGroupTimer(
                 timer_70,
                 pI16A2,
                 i_60,
                 timeUntilAlternateUpdate, //const
                 true//processingATimer    //const
                 );
          ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
          return;
        };
        if (D5W != 0)
        {
          ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
          return;
        };
        if (STRandom0_3() != 0)
        {
          ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
          return;
        };
      };
      fearFactorIs6 = fearFactor == StateOfFear6;
      bool_36 = false;
      TIMERTRACE(0xd27e);
      D5W = STRandom0_3();
      D4W = D5W;
      w_44 = 0;
      mmr.Setflg(TT29to41_TT30TryDirectionsD5toD4);
      Try_Directions_D5_To_D4(
                   pI16A2,
                   D4,                      //Not Modified
                   D5,                      //Not Modified
                   DB4A3,
                   timer_70,
                   i_60,
                   w_54,                    //Not Modified
                   timePerMove,             //Not Modified
                   w_44,                    //Not Modified
                   bool_36,                 //Not Modified
                   timeFunc,                //const
                   invincible,              //const
                   timeUntilAlternateUpdate,//const
                   monster,                 //const
                   mtDescLocal,             //const
                   mapX,                    //const
                   mapY,                    //const
                   indexOfLastMonster,      //const
                   horizSize,               //const
                   fearFactorIs6,           //const
                   true,//processingATimer, //const
                   attackDistance,          //const
                   &mmr
                   );
      ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
      return;
  case TT_29:
      mmr.Setflg(TT29to41_TT29);
      TIMERTRACE(0xcf86);
      if (AITraceActive)
      {
        fprintf(GETFILE(TraceFile),"      MAI timeFunc 29.\n");
      };
      timeFunc = TT_M3;
      fearFactorIs6 = fearFactor == StateOfFear6;
      bool_36 = false;
      TIMERTRACE(0xd27e);
      D5W = STRandom0_3();
      D4W = D5W;
      w_44 = 0;
      Try_Directions_D5_To_D4(
                  pI16A2,
                  D4,                      //Not Modified
                  D5,                      //Not Modified
                  DB4A3,
                  timer_70,
                  i_60,
                  w_54,                    //Not Modified
                  timePerMove,             //Not Modified
                  w_44,                    //Not Modified
                  bool_36,                 //Not Modified
                  timeFunc,                //const
                  invincible,              //const
                  timeUntilAlternateUpdate,//const
                  monster,                 //const
                  mtDescLocal,             //const
                  mapX,                    //const
                  mapY,                    //const
                  indexOfLastMonster,      //const
                  horizSize,               //const
                  fearFactorIs6,           //const
                  true,//processingATimer, //const
                  attackDistance,          //const
                  &mmr
                  );
      ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
      return;
  }; //switch
  if (timeFunc <= TT_MONSTER_A3)
  { // Process Timers of type A
    mmr.Setflg(TT29to41_TTmonsterA3);
    TIMERTRACE(0xcfa6);
    switch(timeFunc)
    {
    case TT_MONSTER_Agroup: tt = TT_MONSTER_Bgroup; break;
    case TT_MONSTER_A0:     tt = TT_MONSTER_B0;     break;
    case TT_MONSTER_A1:     tt = TT_MONSTER_B1;     break;
    case TT_MONSTER_A2:     tt = TT_MONSTER_B2;     break;
    case TT_MONSTER_A3:     tt = TT_MONSTER_B3;     break;
    default: die(0xd7d6,"Illegal Monster AI Timer");
             tt = TT_MONSTER_B0;
             break;
    };
    timer_70.Function(tt);
    if (AITraceActive)
    {
      fprintf(GETFILE(TraceFile),"      MAI timeFunc<37. Set New timerFunction to %s\n",TimerName(timer_70.Function()));
    };
    D0W = Blocked(DB4A3, -1, mapX, mapY);
                  //  Checks each member for 'looking our way' 
    if (AITraceActive)
    {
      fprintf(GETFILE(TraceFile),"      MAI timeFunc<37.  Blocked=%d\n",D0W);
    };
    if (D0W != 0)
    {
      mmr.Setflg(TT29to41_blocked);
      TIMERTRACE(0xcfc6);
      if ( (fearFactor != 6) && (fearFactor != 5) )
      {
        mmr.Setflg(TT29to41_fearNot5or6);
        TIMERTRACE(0xcfd2);
        D0W = sw(abs(d.partyX - mapX));
        D1W = sw(abs(d.partyY - mapY));
        if (D0W + D1W <= 1) 
        {
          mmr.Setflg(TT29to41_MaybeDeleteTimersFear6TurnIndividuals);
          MaybeDelTimers_Fear6_TurnIndividuals(
                      pI16A2,
                      DB4A3,
                      timer_70,
                      timeUntilAlternateUpdate, //const
                      timeFunc,                 //const
                      mapX,                     //const
                      mapY,                     //const
                      mtDescLocal,              //const
                      indexOfLastMonster,       //const
                      horizSize,                //const
                      true,//processingATimer,  //const
                      &mmr
                      );
          ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
          return;
        };
        // The following line makes no sense.  But I have studied the
        // original machine language over and over again.  This is what it did.
        // Perhaps it was written backward:
        //
        // if ( ((fearFactor!=0)&&(fearFactor!=3)) || (fearFactor==7) )
        //
        // This is less obviously redundant.
        if ( ((fearFactor==0)||(fearFactor==3)) && (fearFactor!=7) ) 
        {
          mmr.Setflg(TT29to41_fear0or3);
          TIMERTRACE(0xd21a);
          //D3W = 7;
          //30OCT02SETWBITS0_3(DB4A3->word14, D3W);
          DB4A3->fear(StateOfFear7);
          pI16A2->uByte6 = ub(d.partyX);
          pI16A2->uByte7 = ub(d.partyY);
          //timer_70.timerTime++;
          timer_70.Time(timer_70.Time()+1);
          StartBGroupTimer(
                 timer_70,
                 pI16A2,
                 i_60,
                 timeUntilAlternateUpdate, //const
                 true//processingATimer                      //const
                 );
          ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
          return;
        };
      };
      TIMERTRACE(0xd018);
      pI16A2->uByte6 = ub(d.partyX);
      pI16A2->uByte7 = ub(d.partyY);
    };
    if (fearFactor == 6)
    {
      mmr.Setflg(TT29to41_fear6);
      TIMERTRACE(0xd036);
      if (AITraceActive)
      {
        fprintf(GETFILE(TraceFile),"      MAI fearFactor==6\n");
      };
      D5W = sw(timeFunc-33);
      //D0W = (I16)(pI16A2->ITEM16_uByte12[D5W] & 0x80);
      i_60 = NextMonsterUpdateTime(
              pI16A2, 
              D5W, 
              pI16A2->singleMonsterStatus[D5W].TestAttacking());
      Set_Monster_Timer(
                       timer_70,
                       i_60,
                       timeUntilAlternateUpdate, //const
                       true//processingATimer    //const
                       );
      ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
      return;
    };
    TIMERTRACE(0xd0580);
    if ( (xDistance > 3) || (yDistance > 3) )
    {
      mmr.Setflg(TT29to41_w30w32GretaterThan3);
      TIMERTRACE(0xd068);
//      i_60 = d.Time + BITS4_7(mtDescLocal.word20);
      i_60 = d.Time + mtDescLocal.AdditionalDelayFromMoveToAttack();
      if (AITraceActive)
      {
        fprintf(GETFILE(TraceFile),
                  "      MAI fearFactor != 6.  Distance to party > 3. AdditionalDelayAfter(move/attack) = %d, %d\n",
                  mtDescLocal.AdditionalDelayFromMoveToAttack(),
				  mtDescLocal.AdditionalDelayFromAttackToMove());
      };
      Set_Monster_Timer(
                       timer_70,
                       i_60,
                       timeUntilAlternateUpdate, //const
                       true//processingATimer    //const
                       );
      ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
      return;
    };
    EnsureI60NonZero(i_60, pI16A2);
    Set_Monster_Timer(
                     timer_70,
                     i_60,
                     timeUntilAlternateUpdate, //const
                     true//processingATimer    //const
                     );
    ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
    return;
  };
  // ********************************************************
  // Process Timers of Type B
  // ********************************************************
  TIMERTRACE(0xd08a);
  processingATimer = false;//w_40 = 0;
  if (AITraceActive)
  {
    fprintf(GETFILE(TraceFile),"      MAI timeFunc>=37.\n");
  };
  if (timeUntilAlternateUpdate != 0)
  {
    TIMERTRACE(0xd094);
    i_60 = d.Time;
  };
  if (timeFunc != TT_MONSTER_Bgroup)
  {
    mmr.Setflg(TT29to41_TTnot37);
  //tag00d870:
    TIMERTRACE(0xd870);
    if (fearFactor == StateOfFear5)
    {
      mmr.Setflg(TT29to41_fear5);
      TIMERTRACE(0xd876);
      if (indexOfLastMonster != 0)
      {
        TIMERTRACE(0xd87c);
        ClearAttacking_DeleteMovementTimers(pI16A2, mapX, mapY);
      }
      Try_To_Move(
              D5,
              DB4A3,
              pI16A2,
              timer_70,            
              i_60,                     //Not Modified
              w_54,                     //Not Modified
              timePerMove,              //Not Modified
              attackDistance,           //Not Modified
              yDistance,                //Not Modified
              xDistance,                //Not Modified
              timeFunc,                 //const
              monster,                  //const
              timeUntilAlternateUpdate, //const
              mtDescLocal,              //const
              invincible,               //const
              mapX,                     //const
              mapY,                     //const
              indexOfLastMonster,       //const
              horizSize,                //const
              fearFactorIs6  ,          //const
              false, //processingATimer,//const
              &mmr
            );
      ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
      return;
    };
    D4W = sw(timeFunc - TT_MONSTER_B0);
    //D0W = pI16A2->ITEM16_uByte12[D4W];
    if (pI16A2->singleMonsterStatus[D4W].TestAttacking())
    {
      mmr.Setflg(TT29to41_attacking);
      TIMERTRACE(0xd8a6);
      i_60 = NextMonsterUpdateTime(pI16A2, D4W, false);
      w_90 = STRandom0_3();
      D4W = (UI8)(mtDescLocal.attackTicks07); // uByte7);
      //timer_70.timerTime += uw(D4W + w_90 - 1);
      timer_70.Time(timer_70.Time() + uw(D4W + w_90 - 1));
      if (D4W > 15)
      {
        TIMERTRACE(0xd8dc);
        D0W = sw((STRandom() & 7) - 2);
        //timer_70.timerTime += D0W;//D0 can be negative.
        timer_70.Time(timer_70.Time() + D0W);//D0 can be negative.
      };
    }
    else //not atacking
    {
      mmr.Setflg(TT29to41_notAttacking);
      if (D4W > indexOfLastMonster)
      {
        ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
        return;
      };
      PreferredDirection = d.PrimaryDirectionToParty;
      attackDistance = Blocked(DB4A3, D4W, mapX, mapY);
                  //  Checks member D4W for 'looking our way'   
      if (attackDistance != 0)
      {
        TIMERTRACE(0xd91c);
        pI16A2->uByte6 = ub(d.partyX);
        pI16A2->uByte7 = ub(d.partyY);
      };
  //
  //
      if (   (indexOfLastMonster == 0)
          && (horizSize != 2)
          && (((D5W=(I16)((i16)STRandom())) & 0xc0) == 0) )
      {
        TIMERTRACE(0xd94c);
        if (pI16A2->positions() != 255)
        {
          if (D5W & 0x38)
          {
            TIMERTRACE(0xd960);
            pI16A2->positions() = 0xff;
          }
          else
          {
            D0W = pI16A2->positions();
            if (D5W & 1)
            {
              TIMERTRACE(0xd97e);
              D1W = 1; // Turn right
            }
            else
            {
              TIMERTRACE(0xd980);
              D1W = -1; // Turn left
            };
            D5W = (I16)((D0W + D1W) & 3);
            // I think there is something wrong with D5 here.
          };
        };
  //tag00d98c:
        TIMERTRACE(0xd98c | (D5W<<16));
        if (   ((D5W & 0x38) == 0)
            && (attackDistance != 1)
            && (horizSize == 0) )
        {
          TIMERTRACE(0xd9a2);
          D0W = CheckMonsterVersusMissiles(mapX, mapY, pI16A2->positions(), &mmr);
          if (D0W != 0)
          {
            if (d.MonsterDamageResult == 2)
            {
              ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
              return;
            };
          };
          pI16A2->positions() = (UI8)(D5W & 3);
        };
      };
  //
      if (   (attackDistance != 0)
          && (  (mtDescLocal.See360())
  //            ||(((pI16A2->facings() >> (2*D4W)) & 3) == PreferredDirection ) ))
              ||((TwoBitNibble(pI16A2->facings(),D4W)) == PreferredDirection ) ))
      {
        TIMERTRACE(0xd9fe);
        D5W = mtDescLocal.word14_12_15();
        if (   (attackDistance <= D5W)
            && ((xDistance == 0)||(yDistance == 0))
            && (D5W <= (w_100=(I16)(STRandom(16) + 1))) )
        {
          TIMERTRACE(0xda38);
          if (D5W == 1)
          {
            TIMERTRACE(0xda40);
            //D5W = Item26_26.word2;
  //
  //
  //
  //
            if (  (   !mtDescLocal.word2Bit3()
                  || (STRandom0_3() == 0)
                  || !mtDescLocal.word2Bit4()
                  )
                && (horizSize == 0)
                && (pI16A2->positions() != 255))
            {
              TIMERTRACE(0xda74);
  //            D5W = (I16)((pI16A2->positions() >> (2*D4W)) & 3);
              D5W = (I16)(TwoBitNibble(pI16A2->positions(),D4W));
              if (PreferredDirection != D5W)
              {
                TIMERTRACE(0xda90);
                if (D5W != ((PreferredDirection + 1) & 3))
                {
                  TIMERTRACE(0xdaa2);
  //
                  if ( (indexOfLastMonster == 0) && (STRandomBool() != 0) )
                  {
                    TIMERTRACE(0xdab0);
  //                  pI16A2->uByte3 = 0xff;
                  }
                  else
                  {
                    if ((PreferredDirection & 1) == (D5W & 1))
                    {
                      TIMERTRACE(0xdacc);
                      D5W--; // Turn left
                    }
                    else
                    {
                      TIMERTRACE(0xdaad0);
                      D5W++; //Turn right
                    };
                    D5W &= 3;
                    D0W = OrdinalOfMonsterAtPosition(DB4A3, D5W);
  
  //
  //
  //
                    if (   (D0W==0)
                        || (  (STRandomBool()!=0)
                            &&((OrdinalOfMonsterAtPosition(DB4A3,D5W=(I16)((D5W+2)&3))))==0) )
                    {
                      TIMERTRACE(0xdb08);
                      D0W = CheckMonsterVersusMissiles(mapX, mapY, pI16A2->positions(), &mmr);
                      if ((D0W!=0) && (d.MonsterDamageResult==2))
                      {
                        ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
                        return;
                      };
                      if (d.MonsterDamageResult != 1)
                      {
                        TIMERTRACE(0xdb36);
                        D0W = SetMonsterPositionBits(
                                pI16A2->positions(), //old positions byte
                                D4W,           //monster number
                                D5W);          //position
                        pI16A2->positions() = D0UB;
                      };
                    };
                  };
                  D0W = sw(mtDescLocal.timePerMove()/2 + STRandomBool());
                  D0L = Larger(1, D0W);
                  //timer_70.timerTime += D0L;
                  timer_70.Time(timer_70.Time() + D0L);
                  timer_70.Function(timeFunc);
                  EnsureI60NonZero(i_60, pI16A2);
                  Set_Monster_Timer(
                                    timer_70,
                                    i_60,
                                    timeUntilAlternateUpdate, //const
                                    false//processingATimer                      //const
                                    );
                  ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
                  return;
                };//end if (preferedDirection +1 . . .)
              }; //end if (preferredDirection. . .)
            }; //end if (word2bit3 . . .)
          }; //end if(D5W==1)
          TIMERTRACE(0xdb7a);
          i_60 = NextMonsterUpdateTime(
                            pI16A2,
                            D4W,
                            MonsterAttacks(monster,DB4A3,mapX,mapY,D4W));
          w_110 = STRandomBool();
          //D0L = ((mtDescLocal.word20 & 15) + w_110) & 0xffff;
          D0L = ((mtDescLocal.Word20_0_3()) + w_110) & 0xffff;
          //timer_70.timerTime += D0L;
          timer_70.Time(timer_70.Time() + D0L);
        }
        else //if (attackDistance xDistance . . . )
        {
          D3W = 7;
          //ASSERT(D3L != 0xccccc);
          //30OCT02SETWBITS0_3(DB4A3->word14, D3W) ;
          DB4A3->fear(StateOfFear7);
          if (indexOfLastMonster != 0)
          {
            TIMERTRACE(0xdbd4);
            ClearAttacking_DeleteMovementTimers(pI16A2, mapX, mapY);
          };
          D4D0Finish(
                 DB4A3,
                 timer_70,
                 pI16A2,
                 i_60,                      //Not Modified
                 w_54,                      //Not Modified
                 timePerMove,               //Not Modified
                 yDistance,                 //Not Modified
                 xDistance,                 //Not Modified
                 mapX,                      //const
                 mapY,                      //const
                 timeFunc,                  //const
                 monster,                   //const
                 invincible,                //const
                 timeUntilAlternateUpdate,  //const
                 mtDescLocal,               //const
                 indexOfLastMonster,        //const
                 horizSize,                 //const
                 fearFactorIs6,             //const
                 false,//processingATimer,  //const
                 attackDistance,
                 &mmr
                 );
          ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
          return;
        };
      } //if(attackDistance . . .)
      else
      {
        D0W = Blocked(DB4A3, -1, mapX, mapY);
                  //  Checks each member for 'looking our way'
        if (D0W != 0)
        {
          TIMERTRACE(0xdbfe);
          pI16A2->uByte6 = ub(d.partyX);
          pI16A2->uByte7 = ub(d.partyY);
          boolD0 = (indexOfLastMonster != 0) & (horizSize == 1);
          TurnMonster(pI16A2,
                      PreferredDirection,
                      D4W,    //monster index
                      boolD0);
          //timer_70.timerTime += 2;
          timer_70.Time(timer_70.Time() + 2);
          //i_60 = timer_70.timerTime & 0xffffff;
          i_60 = timer_70.Time();
        }
        else
        {
          TIMERTRACE(0xdc4a);
          D3W = 7;
          //30OCT02SETWBITS0_3(DB4A3->word14, D3W);
          DB4A3->fear(StateOfFear7);
          if (indexOfLastMonster != 0)
          {
            ClearAttacking_DeleteMovementTimers(pI16A2, mapX, mapY);
          };
          TryUByte67(
                   D5,
                   PreferredDirection,
                   pI16A2,
                   DB4A3,
                   timer_70,
                   i_60,                     //Not Modified
                   w_54,                     //Not Modified
                   timePerMove,              //Not Modified
                   yDistance,                //Not Modified
                   xDistance,                //Not Modified
                   mapX,                     //const
                   mapY,                     //const  
                   timeUntilAlternateUpdate, //const
                   timeFunc,                 //const
                   monster,                  //const
                   invincible,               //const
                   mtDescLocal,              //const
                   indexOfLastMonster,       //const
                   horizSize,                //const
                   fearFactorIs6,            //const
                   false,//processingATimer, //const
                   attackDistance,           //const
                   &mmr
                   );
          ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
          return;
        };
      };
    }; //if(attacking) else ;
    timer_70.Function(timeFunc);
    Set_Monster_Timer(
                     timer_70,
                     i_60,
                     timeUntilAlternateUpdate, //const
                     false//processingATimer   //const
                     );
    ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
    return;
  };
  TIMERTRACE(0xd0b6);
  if (AITraceActive)
  {
    fprintf(GETFILE(TraceFile),"      MAI timeFunc=37.\n");
  };
  if (   (fearFactor != StateOfFear0)
      && (fearFactor != StateOfFear2)
      && (fearFactor != StateOfFear3) )
  {
//tag00d490:
    TIMERTRACE(0xd490);
    if (AITraceActive)
    {
      fprintf(GETFILE(TraceFile),"      MAI fearFactor not 0, 2, or 3\n");
    };
    if (fearFactor != StateOfFear7) 
    {
      TIMERTRACE(0xd746);
      if (fearFactor != StateOfFear5)  
      {
        Increment_Time_By_W52_Plus_Random(timer_70, timePerMove);
        StartBGroupTimer(
              timer_70,
              pI16A2,
              i_60,
              timeUntilAlternateUpdate, //const
              false//processingATimer   //const
              );
        ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
        return;
      };
      Try_To_Move(
              D5,
              DB4A3,
              pI16A2,
              timer_70,            
              i_60,                    //Not Modified
              w_54,                    //Not Modified 
              timePerMove,             //Not Modified
              attackDistance,          //Not Modified
              yDistance,               //Not Modified
              xDistance,               //Not Modified
              timeFunc,                //const
              monster,                 //const
              timeUntilAlternateUpdate,//const
              mtDescLocal,             //const
              invincible,              //const
              mapX,                    //const
              mapY,                    //const
              indexOfLastMonster,      //const
              horizSize,               //const
              fearFactorIs6,           //const
              false,//processingATimer,//const
              &mmr
            );
      ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
      return;
  
    };
    attackDistance = Blocked(DB4A3, -1, mapX, mapY);
                  //  Checks each member for 'looking our way'
    if (attackDistance != 0)
    {
      TIMERTRACE(0xd4b0);
      if (attackDistance <= mtDescLocal.word14_12_15())
      {
        TIMERTRACE(0xd4c0);
        if (    (xDistance == 0)
            || (yDistance == 0) )
        {
          MaybeDelTimers_Fear6_TurnIndividuals(
                      pI16A2,
                      DB4A3,
                      timer_70,
                      timeUntilAlternateUpdate, //const
                      timeFunc,                 //const
                      mapX,                     //const
                      mapY,                     //const
                      mtDescLocal,              //const
                      indexOfLastMonster,       //const
                      horizSize,                //const
                      false,//processingATimer, //const
                      &mmr
                      );
          ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
          return;
        };
      };
  //tag00d4d0:
      D4D0Finish(
                 DB4A3,
                 timer_70,
                 pI16A2,
                 i_60,                      //Not Modified
                 w_54,                      //Not Modified
                 timePerMove,               //Not Modified
                 yDistance,                 //Not Modified
                 xDistance,                 //Not Modified
                 mapX,                      //const
                 mapY,                      //const
                 timeFunc,                  //const
                 monster,                   //const
                 invincible,                //const
                 timeUntilAlternateUpdate,  //const
                 mtDescLocal,               //const
                 indexOfLastMonster,        //const
                 horizSize,                 //const
                 fearFactorIs6,             //const
                 false,//processingATimer,  //const
                 attackDistance,
                 &mmr
                 );
      ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
      return;
    };
//  tag00d500:
    TryUByte67(
               D5,
               PreferredDirection,
               pI16A2,
               DB4A3,
               timer_70,
               i_60,                     //Not Modified
               w_54,                     //Not Modified
               timePerMove,              //Not Modified
               yDistance,                //Not Modified
               xDistance,                //Not Modified
               mapX,                     //const
               mapY,                     //const  
               timeUntilAlternateUpdate, //const
               timeFunc,                 //const
               monster,                  //const
               invincible,               //const
               mtDescLocal,              //const
               indexOfLastMonster,       //const
               horizSize,                //const
               fearFactorIs6,            //const
               false,//processingATimer, //const
               attackDistance,           //const
               &mmr
               );
    ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
    return;

  };
  attackDistance = Blocked(DB4A3, -1, mapX, mapY);
                  //  Checks each member for 'looking our way'     
  if (AITraceActive)
  {
    fprintf(GETFILE(TraceFile),"      MAI fearFactor is 0, 2, or 3.  attackDistance=%d\n",attackDistance);
  };
  if (attackDistance != 0)
  {
    TIMERTRACE(0xd0d0);
    if (attackDistance <= mtDescLocal.word14_12_15())
    {
      TIMERTRACE(0xd0e2);
      if ( (xDistance == 0) || (yDistance == 0) )
      {  
        MaybeDelTimers_Fear6_TurnIndividuals(
                    pI16A2,
                    DB4A3,
                    timer_70,
                    timeUntilAlternateUpdate, //const
                    timeFunc,                 //const
                    mapX,                     //const
                    mapY,                     //const
                    mtDescLocal,              //const
                    indexOfLastMonster,       //const
                    horizSize,                //const
                    false,//processingATimer, //const
                    &mmr
                    );
        ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
        return;
      };
    };

    TIMERTRACE(0xd214);
    if (fearFactor != 2)
    {
      TIMERTRACE(0xd21a);
      D3W = 7;
      //30OCT02SETWBITS0_3(DB4A3->word14, D3W);
      DB4A3->fear(StateOfFear7);
      pI16A2->uByte6 = ub(d.partyX);
      pI16A2->uByte7 = ub(d.partyY);
      //timer_70.timerTime++;
      timer_70.Time(timer_70.Time()+1);
      StartBGroupTimer(
             timer_70,
             pI16A2,
             i_60,
             timeUntilAlternateUpdate, //const
             false//processingATimer   //const
             );
      ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
      return;
    };  
    Increment_Time_By_W52_Plus_Random(timer_70, timePerMove);
    StartBGroupTimer(
           timer_70,
           pI16A2,
           i_60,
           timeUntilAlternateUpdate,  //const
           false//processingATimer    //const
           );
    ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
    return;
  };
  TIMERTRACE(0xd24a);
  if (fearFactor != StateOfFear0)
  {
    Increment_Time_By_W52_Plus_Random(timer_70, timePerMove);
    StartBGroupTimer(
           timer_70,
           pI16A2,
           i_60,
           timeUntilAlternateUpdate,  //const
           false//processingATimer    //const
           );
    ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
    return;
  };

  TIMERTRACE(0xd250);
  PreferredDirection =
         GetBestMonsterFacingP1(&mtDescLocal, mapX, mapY);
  if (AITraceActive)
  {
    fprintf(GETFILE(TraceFile),"      MAI fearFactor is 0. Preferred Direction=%d\n",PreferredDirection);
  };
  if (PreferredDirection != 0)
  {
    TIMERTRACE(0xd264);
    PreferredDirection -= 1;
    //RFWAP = false;
    Try_Preferred_Direction_Then_Others(
                        PreferredDirection,       //Not Modified
                        pI16A2,                   //modified
                        DB4A3,                    //modified
                        timer_70,                 //modified
                        i_60,                     //Not Modified
                        w_54,                     //Not Modified
                        timePerMove,              //Not Modified
                        yDistance,                //Not Modified
                        xDistance,                //Not Modified
                        mtDescLocal,              //const
                        mapX,                     //const
                        mapY,                     //const
                        invincible,               //const
                        timeUntilAlternateUpdate, //const
                        monster,                  //const
                        timeFunc,                 //const
                        indexOfLastMonster,       //const
                        horizSize,                //const
                        fearFactorIs6,            //const
                        false, //RFWAP            //const
                        false,//processingATimer, //const
                        attackDistance,           //const
                        &mmr
                  );
    ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
    return;
  };
  bool_36 = false;
  if (STRandomBool() == 0) 
  {
    if (AITraceActive)
    {
      fprintf(GETFILE(TraceFile),"      MAI No Preferrred Direction.  RandomBool()=0.\n");
    };
    Process_Invincible_And_Finish(
                           D5,                       //modified
                           DB4A3,                    //modified
                           PreferredDirection,       //modified
                           pI16A2,                   //modified
                           timer_70,                 //modified
                           i_60,                     //Not Modified
                           w_54,                     //Not Modified
                           timePerMove,              //Not Modified
                           bool_36,                  //Not Modified
                           mapX,                     //const
                           mapY,                     //const
                           timeFunc,                 //const
                           monster,                  //const
                           timeUntilAlternateUpdate, //const
                           mtDescLocal,              //const
                           invincible,               //const
                           indexOfLastMonster,       //const
                           horizSize,                //const
                           fearFactorIs6,            //const
                           false,//processingATimer, //const
                           attackDistance,           //const
                           &mmr
          );
     ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
     return;
  };

//tag00d27e:
  TIMERTRACE(0xd27e);
  if (AITraceActive)
  {
    fprintf(GETFILE(TraceFile),"      MAI No Preferrred Direction.  RandomBool()=1.\n");
  };
  D5W = STRandom0_3();
  D4W = D5W;
  w_44 = 0;

  Try_Directions_D5_To_D4(
               pI16A2,
               D4,                      //Not Modified
               D5,                      //Not Modified
               DB4A3,
               timer_70,
               i_60,
               w_54,                    //Not Modified
               timePerMove,             //Not Modified
               w_44,                    //Not Modified
               bool_36,                 //Not Modified
               timeFunc,                //const
               invincible,              //const
               timeUntilAlternateUpdate,//const
               monster,                 //const
               mtDescLocal,             //const
               mapX,                    //const
               mapY,                    //const
               indexOfLastMonster,      //const
               horizSize,               //const
               fearFactorIs6,           //const
               false,//processingATimer,//const
               attackDistance,          //const
               &mmr
    );
  ProcessPostMonsterMoveFilter(pmmfloc, &mmr);
  return;
//tag00d48c:
//  TIMERTRACE(0xd48c);
//  goto tag00d86c;



}

//*********************************************************
//
//*********************************************************
//  TAG00e8ce
i16 CheckMonsterVersusMissiles(i32 mapX, i32 mapY, ui16 position, MMRECORD *pmmr)
{//(i16)
  dReg D0, D5;
  RN  objD6;
  HTIMER saveTimerHandle;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7W = position;
  D5W = 0;
  d.MonsterDamageResult = 0;
  do
  {
    for (objD6 = FindFirstObject(mapX, mapY);
         objD6 != RNeof;
         objD6 = GetDBRecordLink(objD6)  )
    {
      if (objD6.dbType() != dbMISSILE) continue;
      if (objD6.pos() != position) continue;
      saveTimerHandle = GetRecordAddressDB14(objD6)->timerIndex();
      D0W = ProcessMissileEncounter(
                -1,    //-1=monster
                mapX,  // monster location
                mapY,
                mapX,  // missile location
                mapY,
                position,
                objD6,
                pmmr);//missile
      if (D0W == 0) continue;
      gameTimers.DeleteTimer(saveTimerHandle); //TAG00e156(objD6);
      D5W++;
      break;

//
    };
  } while ((objD6 != RNeof) && (d.MonsterDamageResult!=2));

//
  return (D5W);
}

//*********************************************************
// Distance as the (orthoganal) crow flies.
//*********************************************************
//  TAG00f4c8
i16 DistanceBetweenPoints(
              i32 X1,
              i32 Y1,
              i32 X2,
              i32 Y2)
{
  return sw(abs(X1-X2) + abs(Y1-Y2));
}
//*********************************************************
//
//*********************************************************
//  TAG00f6ee
i16 MonsterDamagesCharacter(DB4 *pDB4,i32 chIdx, bool supressPoison)
{//(i16)
  dReg        D0, D1, D3, D4, D5, D6, D7;
  DB4        *DB4A3;
  CHARDESC   *chA2;
  MONSTERDESC mtDesc;
  i16         w_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  DB4A3 = pDB4;
  chA2 = &d.CH16482[chIdx];
  if (chIdx >= d.NumCharacter) return 0;
  if (chA2->HP() == 0) return 0;

  if (d.PartySleeping != 0) WakeUp();
  w_2 = sw(2 * d.pCurLevelDesc->experienceMultiplier());
  memmove(&mtDesc, &d.MonsterDescriptor[DB4A3->monsterType()],26);
  // local copy of monster descriptor
  AdjustSkills(chIdx, 7, mtDesc.word16_8_11(), ASW_MonsterDamagesCharacter);
  if (d.PartySleeping == 0)
  {
    D0W = sw(Quickness(chA2));
    D1W = sw(STRandom(32));
    D3W = mtDesc.dexterity12; // uByte8[4];
    D1W = sw(D1W + D3W + w_2 - 16);

    if ( (D0W >= D1W) && (STRandom0_3() != 0) ) return 0;
    // character is slower (or monster is lucky!)
    if (IsCharacterLucky(chA2, 60)) return 0;
  };
  D6W = (i16)STRandom();
  if (D6W & 0x70)
  {
    D6W &= 15;
    D5W = mtDesc.word22();
    for (D4W=0; D6W > (D5W&15); D4W++) D5W >>= 4;
//

    D4W = d.uByte590[D4W];
  }
  else
  {
    D4W = (I16)(D6W & 1);
  };
  D3W = mtDesc.attack10; // uByte8[2];
  D0W = sw(w_2 + STRandom(16) + D3W);
  D1W = sw(DetermineMastery(chIdx, 7));
  D7W = sw(D0W - 2*D1W);
  if (D7W <= 1)
  {
    if (STRandomBool() != 0) return 0;
    D7W = sw(STRandom0_3() + 2);
  };
  D7W /= 2;
  D0W = sw(STRandom(D7W));
  D1W = STRandom0_3();
  D7W = sw(D7W + D0W + D1W);
  D7W = sw(D7W + STRandom(D7W));
  D7W /= 4;
  D7W = sw(D7W + STRandom0_3() + 1);
  if (STRandomBool() != 0)
  {
    D7W = sw(D7W - (STRandom(D7W/2 + 1) - 1));
  };
  D7W = sw(DamageCharacter(chIdx, D7W, D4W, mtDesc.uByte22[2]));
  if (D7W != 0)
  {
    QueueSound(chIdx + 9, d.partyX, d.partyY, 2);
    D6W = mtDesc.poisonAttack11; // uByte8[3];
    if (D6W != 0)
    {
      if (STRandomBool() != 0)
      {
        D6W = TAG016426(chA2, 4, D6W);
        // There was an impossible jump around the following function call.
        if (!supressPoison)PoisonCharacter(chIdx, D6W);
      };
    };
    return (D7W);
  };
  return (0);
}

void DeleteMonsterInGroup(const LOCATIONREL& locr, ui32 indexInGroup)
{
  i32 i, j;
  RN objMon;
  const bool noItem16 = false;
  ITEM16 *pItem16 = NULL;
  TIMER *pTimer;
  bool onPartyLevel;
  MONSTERTYPE mt;
  DB4 *pMonster;
  //i32 timerIndex;
  ui32 mIndex;
  MONSTERDESC *pmtDesc;
  TIMERTYPE tt;
  i32 positionByte, w_20, numMonM1, monsterFacingByte;
  objMon = FindFirstMonster(locr.l, locr.x, locr.y);
  if (objMon ==RNeof) return;
  pMonster = GetRecordAddressDB4(objMon);
  mt = pMonster->monsterType();
  pmtDesc = &d.MonsterDescriptor[mt];
  if (pMonster->numMonM1() == 0) return; // Only one monster there!
  if (pMonster->numMonM1() < indexInGroup) return;
  if (AITraceActive)                                          
  {                                                           
    fprintf(GETFILE(TraceFile),                               
            "      Delete Individual monster in group. Index=%d lodation=%d(%d,%d)\n",
                   indexInGroup, locr.l,locr.x,locr.y);
  };                                                          
  positionByte = MonsterPositionByte(pMonster, (i16)locr.l, noItem16);
  if (positionByte == 255)
  {
    w_20 = 255;
  }
  else
  {
//      D0W = (I16)((positionByte >> 2*indexInGroup) & 3);
    w_20 = (I16)(TwoBitNibble(positionByte,indexInGroup));
  };
  //w_20 = D0W;
  numMonM1 = pMonster->numMonM1(); // #monsters-1
  monsterFacingByte = MonsterFacingByte(pMonster, (i16)locr.l, noItem16);
  onPartyLevel = locr.l==d.partyLevel; //)w_18=1; else w_18=0;
  if (onPartyLevel)
  {
    ASSERT(pMonster->groupIndex() < d.MaxITEM16,"maxitem16");
    pItem16 = &d.Item16[pMonster->groupIndex()];
  };
  if (pMonster->fear() == StateOfFear6)
  {
    TIMER_SEARCH timerSearch;
    //pTimer = gameTimers.Timers(); xxTIMERxx Change to FindNextTimer
    //for (timerIndex=0; timerIndex <d.MaxTimer(); timerIndex++, pTimer++)
    while (timerSearch.FindNextTimer())
    {
      pTimer = timerSearch.TimerAddr();
      //if ((pTimer->timerTime >> 24) != (ui16)locr.l) continue;
      if (pTimer->Level() != (ui16)locr.l) continue;
      if (pTimer->timerUByte6() != locr.x) continue;
      if (pTimer->timerUByte7() != locr.y) continue;
      tt = pTimer->Function();
      if (tt < TT_MONSTER_A0) continue;
      if (tt > TT_MONSTER_B3) continue;
      if (tt <= TT_MONSTER_A3)
      {
        mIndex = tt - TT_MONSTER_A0;//  -=33;
      }
      else
      {
        mIndex = tt - TT_MONSTER_B0;// -= 38;
      };
      if (mIndex == indexInGroup)
      {
        timerSearch.DeleteTimer();
      }
      else
      {
        if (mIndex > indexInGroup)
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
          pTimer->Function(tt);
          // D0W = FindTimerPosition(timerIndex);
          //AdjustTimerPosition(FindTimerPosition(timerSearch.TimerIndex()));
          timerSearch.AdjustTimerPriority();
        };
      };
//
    };
    if (onPartyLevel)
    {
      i32 w_12;
      dReg D0, D1;
      w_12 = pmtDesc->bravery();
      if (w_12 != 15) // 15 is fearless!
      {
        w_12 += numMonM1 - 1;
        D1W = (i16)STRandom(16);
        if (w_12 < D1W)
        {
          D1W = sw(100 - 4*w_12);
          D0W = sw(STRandom(D1W) + 20);
          pItem16->uByte5 = D0UB;
          //30OCT02SETWBITS0_3(pMonster->word14, 5);
          pMonster->fear(StateOfFear5);
        };
      };
    };
  };
  i = sw(indexInGroup);
  for (j = i; j < numMonM1; j++)
  {
    i++;
    pMonster->hitPoints6[j] = pMonster->hitPoints6[i];
//      monsterFacingByte = SetMonsterPositionBits(monsterFacingByte, D5W, (monsterFacingByte >> 2*D4W) & 3);
    monsterFacingByte = SetMonsterPositionBits((i16)monsterFacingByte, j, TwoBitNibble(monsterFacingByte,i));
//      positionByte = SetMonsterPositionBits(positionByte, D5W, (positionByte >> 2*D4W) & 3);
    positionByte = (i16) SetMonsterPositionBits((i16)positionByte, j, TwoBitNibble(positionByte,i));
    if (pItem16 != NULL) // if on party level
    {
      pItem16->singleMonsterStatus[j] = pItem16->singleMonsterStatus[i];
    };

//
  };
  positionByte &= 0x3f;
  SetMonsterPositionByte(pMonster, (i16)positionByte, (i16)locr.l, noItem16);
  SetMonsterFacingByte(pMonster, monsterFacingByte, (i16)locr.l, noItem16);
  // Decrement monsters remaining.
  //30OCT02SETWBITS5_6(pMonster->word14, (BITS5_6(pMonster->word14)-1)&3);
  pMonster->numMonM1(pMonster->numMonM1()-1);
}


void InsertMonsterInGroup(const LOCATIONREL& locr, ui32 positionMask)
{
  i32 i;
  RN objMon;
  const bool noItem16 = false;
  ITEM16 *pItem16 = NULL;
  TIMER *pTimer;
  MONSTERTYPE mt;
  DB4 *pMonster;
  i32 monsterSize, maxNumMon;
  MONSTERDESC *pmtDesc;
  TIMERTYPE tt;
  i32 positionByte, numMonM1, facingByte;
  i32 newpos;
  objMon = FindFirstMonster(locr.l, locr.x, locr.y);
  if (objMon ==RNeof) return;
  pMonster = GetRecordAddressDB4(objMon);
  mt = pMonster->monsterType();
  pmtDesc = &d.MonsterDescriptor[mt];
  monsterSize = pmtDesc->horizontalSize();
  if (monsterSize == 0) {maxNumMon = 4; positionMask &= 0x0f;}      // 0, 1, 2, and 3 allowed
  else if (monsterSize == 1) {maxNumMon = 2; positionMask &= 0x05;} // Only 0 and 2 allowed
  else return; // No room
  numMonM1 = pMonster->numMonM1(); // #monsters-1
  if (numMonM1 + 1 >= maxNumMon) return; // No Room
  positionByte = MonsterPositionByte(pMonster, (i16)locr.l, noItem16);
  // Now determine if any allowed position is empty.
  // First we clear any bits where monsters already exist.
  for (i=0; i<=numMonM1; i++)
  {
    positionMask &= ~(1<<TwoBitNibble(positionByte,i));
  };
  for (newpos=0; newpos<4; newpos++, positionMask>>=1)
  {
    if ((positionMask & 1) != 0) break;
  };
  if (newpos == 4) return; //No place to put new monster.
  //
  // Our strategy is to copy the monster at index [0] to index [numMonM1+1]
  //
  // First we put the new one at position newpos.
  positionByte = SetMonsterPositionBits((i16)positionByte, numMonM1+1, newpos);
  SetMonsterPositionByte(pMonster, (i16)positionByte, (i16)locr.l, noItem16);
  // Second, we set his facing the same as monster[0];
  facingByte = pMonster->facing();
  facingByte = SetMonsterFacingBits((i16)facingByte, numMonM1+1, facingByte & 3);
  SetMonsterFacingByte(pMonster, facingByte, locr.l, noItem16);
  // Third, we see if monster[0] has a timer entry.
  //pTimer = gameTimers.Timers();
  {
    TIMER_SEARCH timerSearch;
    //for (timerIndex=0; timerIndex <d.MaxTimer(); timerIndex++, pTimer++) xxTIMERxx Change to FindNextTimer
    while (timerSearch.FindNextTimer())
    {
      pTimer = timerSearch.TimerAddr();
      TIMER timer;
      //if ((pTimer->timerTime >> 24) != (ui16)locr.l) continue;
      if (pTimer->Level() != (ui16)locr.l) continue;
      if (pTimer->timerUByte6() != locr.x) continue;
      if (pTimer->timerUByte7() != locr.y) continue;
      tt = pTimer->Function();
      if ( (tt != TT_MONSTER_A0) && (tt != TT_MONSTER_B0) ) continue;
      memcpy (&timer, pTimer, sizeof(TIMER));
      timer.Function((TIMERTYPE)(tt + numMonM1 + 1));
      //AdjustTimerPosition(FindTimerPosition(timerSearch.TimerIndex()));
      timerSearch.AdjustTimerPriority();  // Why do this? We changed nothing.
      gameTimers.SetTimer(&timer);
      break;
    };
  };
  // Fourth we set the new monster's hit points.
  pMonster->hitPoints6[numMonM1+1] = pMonster->hitPoints6[0];
  // Fifth, we set the new monster's status.
  if (pItem16 != NULL)
  {
    pItem16->singleMonsterStatus[numMonM1+1] = pItem16->singleMonsterStatus[0];
  };
  pMonster->numMonM1(numMonM1+1);
}