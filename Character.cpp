#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

extern ui32 *pDSAparameters;
extern bool neophyteSkills;
ui8 reincarnateAttributePenalty, reincarnateStatPenalty, randomPoints;
pnt GetExternalPortraitAddress(i32 index);
void GetText(char *result, i32 index);

// *********************************************************
//
// *********************************************************
//  TAG001098
i16 ScaledMultiply(i16 Op1,i16 scale,i16 Op2)
{
  return sw((UI16)(Op1) * ((UI16)(Op2)) >> scale);
}

// *********************************************************
//
// *********************************************************
//   TAG00144e
void DrawItem(i32 squareNumber, OBJ_NAME_INDEX objectNameIndex)
{
  dReg D0, D4, D5, D6;
  aReg A2;
  ICONDISPLAY *idA3;
  ui8 *LOCAL_12;
  RectPos LOCAL_8;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7W = P1;
  //D6W = P2;
  idA3 = &d.IconDisplay[squareNumber]; // + 25 + 3*squareNumber;
  idA3->objectType(objectNameIndex);
  if (objectNameIndex == objNI_NotAnObject) return;
  LOCAL_8.w.x1 = idA3->pixelX;
  LOCAL_8.w.x2 = (i16)(LOCAL_8.w.x1 + 15);
  LOCAL_8.w.y1 = idA3->pixelY;
  LOCAL_8.w.y2 = (i16)(LOCAL_8.w.y1 + 15);
  for (D5W=0; D5W<7; D5W++)
  {
    D0W = d.Word612[D5W];
    if (objectNameIndex < D0W) break;

  };
  D5W--;
  A2 = (i8 *)GetBasicGraphicAddress(D5W + 42);
  D6W = sw(objectNameIndex - d.Word612[D5W]);
  if (squareNumber >= 8)
  {
    LOCAL_12 = d.pViewportBMP;
    D4W = 112;
  }
  else
  {
    LOCAL_12 = d.LogicalScreenBase;
    D4W = 160;
  };
  d.UseByteCoordinates = 0;
  TAG0088b2((ui8 *)A2,
            (ui8 *)LOCAL_12,
            &LOCAL_8,
            (D6W & 15) << 4,
            D6W & 0xff0,
            128,
            D4W,
            -1);
}

// *********************************************************
//
// *********************************************************
//   TAG0017d0
void DrawText(ui8 *dst,             //8
              i16 dstWidth,        //12
              i16 x,               //14
              i32 y ,              //16
              i16 textColor,       //18
              i16 backgroundColor, //20
              const char* text,            //22
               i16 len)             //26
{
   // Copies the item`s name into local buffer and adjusts it
   // to the maximalTextLength length by placing spaces at
   // the end of item`s name.

  dReg D7;
  const char* A3;
  char LOCAL_80[80];
//;;;;;;;;;;;
  A3 = text;
  D7W = 0;
  while ((LOCAL_80[D7W]=*(A3++)) != 0) D7W++;

  while (D7W < len) LOCAL_80[D7W++] = ' ';




  LOCAL_80[D7W] = 0;
  TextOut_OneLine(dst,
                  dstWidth,
                  x,
                  y,
                  textColor,
                  backgroundColor,
                  LOCAL_80,
                  999,
                  false);
}

// *********************************************************
//
// *********************************************************
//   TAG001c16
void TextToViewport(i32 x, i32 y, i32 color, const char* txt, bool translate)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TextOut_OneLine(d.pViewportBMP,
                  112,
                  x,
                  y,
                  color,
                  12,
                  txt,
                  999,
                  translate);
}

// *********************************************************
//
// *********************************************************
//   TAG003264
void StrCpy(char *dst,const char *src)
{
  do
  {
    *(dst++) = *src;
  } while (*(src++) != 0);
}

// *********************************************************
//
// *********************************************************
//   TAG009058
void ShadeRectangleInScreen(RectPos *P1,i16 P2)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ASSERT(verifyRectPos(P1),"Rectpos(P1)");
  FillRectangle(d.LogicalScreenBase,
                P1,
                P2 | 0x8000,
                160);
}

// *********************************************************
//
// *********************************************************
i32 TAG009470(CLOTHINGDESC *P1, bool scale)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (scale)
  {
    return ScaledMultiply(P1->uByte1(), 3, sw((P1->uByte2()&7) + 4));
  };
  return P1->uByte1();
}


// *********************************************************
//
// *********************************************************
// TAG009d9e
RN MakeMoreRoomInDB(DBTYPE dbType)
{ //(RN)
  //Oh, dear.  We have run out of space in one of
  //the databases.  What do we do now?  The standard Atari
  //version proceeds to discard objects, first from levels
  //the party does not currently occupy and, as a last
  //resort, from the party's level.
  //
  //The databases have rather arbitary size limits.  Perhaps
  //we can simply make the thing larger.  If it is possible
  //to enlarge the database then we will check to see if that
  //is what the player wants to do.
  dReg D0, D1, D4, D6, D7;
  RN  rnD5;
  LEVELDESC *pldA0;
  CELLFLAG  **pcfA0;
  CELLFLAG  *cfA3;
  DB3  *pDB3_4;
  DB4  *pDB4_4;
  DB5  *pDB5_4;
  DB6  *pDB6_4;
  DB8  *pDB8_4;
  DB10 *pDB10_4;
  DB14 *pDB14_4;
  RN  *rnA2;
  i16 w_14;
  i16 w_12;
  i16 w_10;
  i16 w_8;
  i16 w_6;
  D6L = 0x1baddade;
  i32 index;
  index = db.Enlarge(dbType); //If possible and player agrees.
  if (index != -1)
  {
    return RN(dbType,index);
  };
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (dbType == dbCLOUD)
  {
    return (RNnul); // Curious....I suppose this should never be.
  };
  w_8 = d.LoadedLevel;
  D4W = d.LevelCleared[dbType];
  if (d.partyLevel == D4W)
  {
    D4W++;
    D1W = d.dungeonDatIndex->NumLevel();
    if (D4W >= D1W) D4W = 0;
  };
  w_6 = D4W;
  for (;;)
  {
    pldA0 = &d.pLevelDescriptors[D4W];
//
    w_10 = sw(pldA0->LastColumn()); //level width-1
    w_12 = sw(pldA0->LastRow()); //level height-1
    pcfA0 = (CELLFLAG **)d.pppdPointer10450[D4W];
    cfA3 = *pcfA0;
    D0W = d.objectLevelIndex[D4W];
    D0W = d.objectListIndex[D0W];
    rnA2 = &d.objectList[D0W];
    for (D7W=0; D7W<=w_10; D7W++)
    {
      for (D6W=0; D6W<=w_12; D6W++)
      {
        if ((*(cfA3++) & 0x10) == 0) continue;
        rnD5 = *(rnA2++);
        if (D4W == d.partyLevel)
        {
          if (uw(D7W - d.partyX + 5) <= 10)
          {
            if (uw(D6W - d.partyY + 5) <= 10) continue;
          };
        };
        do
        {
          w_14 = sw(rnD5.dbType());
          if (w_14 == dbACTUATOR)
          {
            pDB3_4 = GetRecordAddressDB3(rnD5);
            if (pDB3_4->actuatorType() == 0) continue;
            break;
          }
          else
          {
            if (w_14 == dbType)
            {
              //punk_4 = GetDBRecordAddress(rnD5);
              switch (dbType)
              {
              case dbMONSTER:
                pDB4_4 = GetRecordAddressDB4(rnD5);
                D0W = pDB4_4->important();
                // Don't delete designer-planted monsters
                if (D0W != 0) continue;
                LoadLevel(D4W);
                {
                  DELETEMONSTERPARAMETERS dmp(DMW_MakeRoom, -1);
                  ProcessMonsterDeleteFilter(D7W, D6W, &dmp, -1);
                  DropMonsterRemains(D7W, D6W, rnD5, -1, d.LoadedLevel, dmp.dropResidue<2);
                  DeleteMonster(D7W, D6W, NULL);
                };
                goto tag00a000;
                // Note fall-through from DB4 code
                // The original code processed monsters and
                // missiles together with "if () else;"
                // around the things that needed to be different.
                // I separated the two entirely.
              case dbMISSILE:
                  pDB14_4 = GetRecordAddressDB14(rnD5);
                // We will use the DB4 pointer for both
                // types of object even though they are
                // very different.  Hmmm...
                LoadLevel(D4W);
                TAG00e156(rnD5);
                RemoveObjectFromRoom(rnD5, D7W, D6W, NULL);
                ProcessObjectFromMissile(rnD5, NULL, D7W, D6W);
                goto tag00a000;
              case dbCLOTHING:
                pDB6_4 = GetRecordAddressDB6(rnD5);
                if (pDB6_4->important()) continue;
                goto tag009fe4;
              case dbWEAPON:
                pDB5_4 = GetRecordAddressDB5(rnD5);
                if (pDB5_4->important()) continue;
                goto tag009fe4;

              case dbMISC:
                pDB10_4 = GetRecordAddressDB10(rnD5);
                if (pDB10_4->important()) continue;
                goto tag009fe4;
              case dbPOTION:
                pDB8_4 = GetRecordAddressDB8(rnD5);
                if (pDB8_4->important()) continue;
tag009fe4:
                LoadLevel(D4W);
                ASSERT(D6L != 0x1baddade,"D6L not initialized");
                MoveObject(rnD5, D7W, D6W, -1, 0, NULL, NULL);
              default:
tag00a000:
              LoadLevel(w_8);
              d.LevelCleared[dbType] = D4UB;
                rnD5.pos(0);
                return (rnD5);
              }; //switch
            };
          };
        // continue for each object in room
        } while ((rnD5 = GetDBRecordLink(rnD5)) != RNeof);

      //continue (y-coordinate)
//
      }; //D6W = y-coordinate
    //continue (x-coordinate)
//
    };// D7W = x-coordinate
//
    if (   (D4W == d.partyLevel)
        || (d.dungeonDatIndex->NumLevel() <= 1) ) //# levels
    {
      d.LevelCleared[dbType] = D4UB;
      return (RNnul);
    };
    do
    {
      D4W++;
      D1W = d.dungeonDatIndex->NumLevel();
      if (D4W >= D1W) D4W = 0;
    } while (D4W == d.partyLevel);
    if (D4W == w_6)
    {
      D4W = d.partyLevel;
    };
  };
//00a0a8 ia0ac 4e5e                     UNLK     A6
}

// *********************************************************
//
// *********************************************************

// *********************************************************
//Attack from      AttackedAbsPos[]
//  North pos 0 -> 0 1 3 2
//  North pos 1 -> 1 0 2 3
//  North pos 2 -> 1 0 2 3
//  North pos 3 -> 0 1 3 2
//  East  pos 0 -> 1 2 0 3
//  East  pos 1 -> 1 2 0 3
//  East  pos 2 -> 2 1 3 0
//  East  pos 3 -> 2 1 3 0
//  South pos 0 -> 3 2 0 1
//  South pos 1 -> 2 3 1 0
//  South pos 2 -> 2 3 1 0
//  South pos 3 -> 3 2 0 1
//  West  pos 0 -> 0 3 1 2
//  West  pos 1 -> 0 3 1 2
//  West  pos 2 -> 3 0 2 1
//  West  pos 3 -> 3 0 2 1
// *********************************************************
//   TAG00f694
void DetermineAttackOrder(
               ui8 attackedAbsPos[4],
               i32 victimX,
               i32 victimY,
               i32 attackerX,
               i32 attackerY,
               i32 attackerPos)
{ // attackerPos is the position within the room
  // of the attacker.
  dReg D7;
  i32 attackFromDirection;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  attackFromDirection =
          GetPrimaryAndSecondaryDirection(
                            victimX, victimY,
                            attackerX, attackerY);
  // If zero then the attacker is to the north, etc
  D7W = sw(2*attackFromDirection);
  if ((attackFromDirection & 1) == 0)
  { // if attack is from north or south
    attackerPos++;
  };
  D7W = sw(D7W + ((attackerPos/2)&1));
  memmove(attackedAbsPos, d.Byte586[D7W], 4);
}

// *********************************************************
//
// *********************************************************
//  TAG0139a2
i32 DecodeCharacterValue(pnt buf,i16 num, bool allowTruncation=false)
{
  i32 result = 0;
  do
  {
    result <<= 4;
    if ( (*buf < 'A') || (*buf > 'P') ) 
    {
      if (allowTruncation && (*buf == 0)) return result;
      else
        return -1;
    };
    result += *(buf++) - 'A';
    num--;
  } while (num!=0);
  return sw(result);

}

ui8 GetOneValue(aReg *A)
{
  if ( (**A < 'A') || (**A > 'Z') ) return 0xff;
  return (ui8)(*((*A)++) - 'A');
}

bool FingerprintExists(ui32 fingerprint)
{
  i32 i;
  CHARDESC character;
  for (i=0; i<d.NumCharacter; i++)
  {
    if (d.CH16482[i].fingerPrint == fingerprint) return true;
  };
  if (character.GetFromWings((ui16)fingerprint,true)) return true;
  return false;
}


// *********************************************************
//
// *********************************************************
//   TAG0139be
void AddCharacter(i16 charID)
{//(void)
  dReg D0, D4, D5, D6;
  aReg A2;
  CHARDESC *pcA3;
  RN firstObj;
  ui32 fingerprint;
  i8  LOCAL_96[1000];
  i32 LOCAL_18;
  i16 LOCAL_14;
  ui16 LOCAL_12; // contains position
  i16 LOCAL_10;
  i16 LOCAL_8;
  i16 LOCAL_6 = 0x2bad;
  i16 LOCAL_4;
  i16 LOCAL_2;
  bool illegalHero;
  i16 newHP, newStamina, newMana;
  static char legalHero[] =
    "BOGUS\012ILLEGAL HERO\012\012M\012AABBAABBAABB\012ABABABABABABAB\012CCCCCCCCCCCCCCCC";
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.EmptyHanded == 0) return; //cursor hand
  if (d.NumCharacter == 4) return;
  LOCAL_2 = d.NumCharacter;
  pcA3 = &d.CH16482[d.NumCharacter];
  ClearMemory((ui8 *)pcA3, 800);
  d.UseByteCoordinates = 1;

  
  
  
  if (charID <= 23)
  {
    TAG0088b2(GetBasicGraphicAddress(26),
              (ui8 *)pcA3->portrait,
              (RectPos *)d.Byte1390,
              (charID&7)<<5,
              (charID>>3)*29,
              128,
              16,
              -1);
  }
  else
  {
    TAG0088b2(
        (ui8 *)GetExternalPortraitAddress(charID),
        (ui8 *)pcA3->portrait,
        (RectPos *)d.Byte1390,
        0, //Src offset X
        0, //Src offset Y
        16, //Src Byte Width
        16,//Dst Byte Width
        -1);
  };

  
  pcA3->talents = 0;
  for (fingerprint = (ui16)((d.partyLevel<<10) | (d.partyX<<5) | (d.partyY));
       FingerprintExists(fingerprint);
       fingerprint=(ui16)(fingerprint+1)){};
  pcA3->fingerPrint = (ui16)fingerprint;
  pcA3->byte30 = 1;
  pcA3->byte31 = 2;
  pcA3->attackType = -1;
  pcA3->busyTimer = -1;
  pcA3->timerIndex = -1;
  pcA3->facing = (ui8)d.partyFacing;
  for (D6W=0;
       CharacterAtPosition((D6W+d.partyFacing)&3) != -1;
       D6W++) {};

//

  pcA3->charPosition = ub((D6W + d.partyFacing)&3);
  pcA3->charFlags = CHARFLAG_positionChanged; //0x400; // 1024
  pcA3->facing3 = (ui8)d.partyFacing;
  pcA3->food = sw((STRandom() & 0xff) + 1500);
  pcA3->water = sw((STRandom() & 0xff) + 1500);
  pcA3->InitializePossessions();
  firstObj = FindFirstObject(d.partyX, d.partyY);
  while (firstObj != RNeof)
  {
    if (firstObj.dbType() == dbTEXT) break;
    firstObj = GetDBRecordLink(firstObj); // Find character's name and title
  };
  if (firstObj != RNeof)
  {
    A2 = (aReg)LOCAL_96;
    DecodeText((char *)A2,
               GetRecordAddressDB2(firstObj),
               (ui16)0x8002,
               990);
  }
  else
  {
    A2 = (aReg)legalHero;
  };
  for (illegalHero=true; illegalHero; A2=(aReg)legalHero)
  {
    illegalHero = false;
    for (D6W=0; (D4W=(UI8)(*(A2++)))!=10; D6W++)
    {
      if (D6W >= 7)
      {
        illegalHero = true;
        break;
      };
      pcA3->name[D6W] = D4B; // At Byte0

    };
    if (illegalHero) continue;
    pcA3->name[D6W] = 0;
    D6W = 0;
    LOCAL_4 = 0;
    for (;;)
    {
      D4W = (UI8)(*(A2++));
      if (D4W == 10)
      {
        if (LOCAL_4 !=0) break;
        LOCAL_4 = 1;
      }
      else
      {
        if (D6W >= 19)
        {
          illegalHero = true;
          break;
        };
        pcA3->title[D6W] = D4B;
        D6W++;
      };
    };
    if (illegalHero) continue;
    pcA3->title[D6W] = 0;
    D0W = *(A2++);
    if (D0W == 'M')
    {
      pcA3->charFlags |= CHARFLAG_male; //16;
    }
    else if (D0W != 'F')
    {
      illegalHero = true;
      continue;
    };
    A2++;
    newHP = (i16)DecodeCharacterValue(A2, 4);
    if ((unsigned)newHP > MAXHP) 
    {
      pcA3->MaxHP(MAXHP);
      illegalHero = true;
    }
    else pcA3->MaxHP(newHP);
    pcA3->HP(pcA3->MaxHP());
    if (newHP == -1)
    {
      illegalHero = true;
      continue;
    };
    A2 += 4;
    newStamina = (i16)DecodeCharacterValue(A2, 4);
    if ((unsigned)newStamina <= MAXSTAMINA) pcA3->MaxStamina(newStamina);
    else 
    {
      pcA3->MaxStamina(MAXSTAMINA);
      illegalHero = true;
    };
    pcA3->Stamina(pcA3->MaxStamina());
    if (newStamina == -1)
    {
      illegalHero = true;
      continue;
    };
    A2 += 4;
    newMana = (i16)DecodeCharacterValue(A2, 4);
    if ((unsigned)newMana > MAXMANA) 
    {
      pcA3->MaxMana(MAXMANA);
      illegalHero = true;
    }
    else pcA3->MaxMana(newMana);
    pcA3->Mana(pcA3->MaxMana());
    if (newMana == -1)
    {
      illegalHero = true;
      continue;
    };
    A2 += 5;
    for (D6W=0; D6W<=6; D6W++) // strength, wisdom, etc
    {
      pcA3->Attributes[D6W].Minimum(30);
      D0W = (i16)DecodeCharacterValue(A2, 2);
      if (D0W == -1)
      {
        illegalHero = true;
        break;
      };
      if (D0W > 224) D0W = 224;
      pcA3->Attributes[D6W].Maximum(D0B);
      pcA3->Attributes[D6W].Current(D0B);
      A2 += 2;
//
    };
    if (illegalHero) continue;
    pcA3->Attributes[Luck].Minimum(10);
    A2++;
    for (D6W=4; D6W<=19; D6W++)
    {
      D4W = sw(*(A2++) - 'A');
      if (((D4W > 0) || neophyteSkills) && (D4W <= 17))
      {
        D0L = 125 << D4W;
		D0L = LimitSkillExperience(D0L);
        pcA3->skills92[D6W].experience = D0L;
      }
      else if (D4W == 0)
      {
        continue;
      }
      else
      {
        illegalHero = true;
        break;
      };
    };
    if (illegalHero) continue;
    reincarnateAttributePenalty = GetOneValue(&A2);
    if (reincarnateAttributePenalty > 16) reincarnateAttributePenalty = 2;
    reincarnateStatPenalty = GetOneValue(&A2);
    if (reincarnateStatPenalty > 16) reincarnateStatPenalty = 8;
    randomPoints = GetOneValue(&A2);
    if (randomPoints > 25) randomPoints = 3;
    pcA3->talents = DecodeCharacterValue(A2,8,true); // Allow truncation
    for (D6W=0; D6W<=3; D6W++)
    {
      LOCAL_18 = 0;
      D5W = sw(4 * (D6W + 1));
      for (LOCAL_4=0; LOCAL_4<4; LOCAL_4++)
      {
        LOCAL_18 += pcA3->skills92[D5W+LOCAL_4].experience;
//
      };
	  LOCAL_18 = LimitSkillExperience(LOCAL_18);
      pcA3->skills92[D6W].experience = LOCAL_18;
//
    };
  };
  if (pcA3->HP() == 0) return;
  d.PotentialCharacterOrdinal = sw(LOCAL_2 + 1);
  d.NumCharacter++;
  if (d.NumCharacter == 1)
  {
    SetHandOwner(0);
    d.Word20250 = 1;
  }
  else
  {
    TAG01b1c0();
    DrawAttackButton(d.NumCharacter-1); // Show additional weapon selector
  };
  LOCAL_8 = d.partyX;
  LOCAL_10 = d.partyY;
  LOCAL_12 = sw((d.partyFacing+2)&3);
  LOCAL_8 = sw(LOCAL_8 + d.DeltaX[d.partyFacing]);
  LOCAL_10 = sw(LOCAL_10 + d.DeltaY[d.partyFacing]);
  firstObj = FindFirstObject(LOCAL_8, LOCAL_10);
  D6W = 13;
  while (firstObj != RNeof)
  {
    D4W = sw(firstObj.dbType());
    if (D4W > dbACTUATOR)
    {
      if (firstObj.pos() == LOCAL_12)
      {
        //D0W = GetObjectDescIndex(firstObj);
        LOCAL_14 = d.ObjDesc[firstObj.DescIndex()].word4; //Carry locations mask
        switch (D4W)
        {
        case dbCLOTHING:
            for (LOCAL_6=2; LOCAL_6<=5; LOCAL_6++)
            {
              if (LOCAL_14 & d.CarryLocation[LOCAL_6])
                      goto switchbreak;

            };
            // Check Neck
            if (   ((LOCAL_14&d.CarryLocation[10])!=0)
                && (pcA3->Possession(10) == RNnul) )
            {
              LOCAL_6 = 10;
            }
            else
            {
              goto case9;
            };
            break;
        case dbWEAPON:
            if (pcA3->Possession(1) != RNnul) goto case9;
            LOCAL_6 = 1;
            break;

        case dbSCROLL:
        case dbPOTION:
            if (pcA3->Possession(11) == RNnul)
            {
              LOCAL_6 = 11;
            }
            else
            {
              if (pcA3->Possession(6) == RNnul)
              {
                LOCAL_6 = 6;
              };
              goto case9;
            };
            break;
        case dbCHEST:
        case dbMISC:
case9:
            // Check neck
            if (   ((LOCAL_14&d.CarryLocation[10])!=0)
                && (pcA3->Possession(10) == RNnul) )
            {
              LOCAL_6 = 10;
              break;
            };
            LOCAL_6 = D6W;
            D6W++;
        };
switchbreak:
        if (pcA3->Possession(LOCAL_6) != RNnul) goto case9;
        if (LOCAL_6 < 30)
        {
          AddCharacterPossession(LOCAL_2, firstObj, LOCAL_6);
        };
      };
    };
    firstObj = GetDBRecordLink(firstObj);

  };
  ShowHideInventory(LOCAL_2); // Show Display resurrect/reincarnate/cancel screen
  TAG02076e();
}

// *********************************************************
//
// *********************************************************
//   TAG014676
void BringCharacterToLife(ui16 chIdx)
{//(void)
  dReg D0, D7;
  CHARDESC *pcA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pcA3 = &d.CH16482[chIdx];
  D0L = CharacterAtPosition(pcA3->charPosition);
  if (D0L != -1)
  { //Find a position to put the character.
    for (D7W=0; CharacterAtPosition(D7W)!=-1; D7W++) {};
//
//
    pcA3->charPosition = D7UB;
  };
  D7W = pcA3->MaxHP();
  D0W = sw(D7W - D7W/64 - 1);
  pcA3->MaxHP(Larger(25, D0W));
  pcA3->HP(pcA3->MaxHP() / 2);
  DrawSpellControls(d.MagicCaster);
  pcA3->facing = (ui8)d.partyFacing;
  pcA3->charFlags |= CHARFLAG_weaponAttack | CHARFLAG_portraitChanged | CHARFLAG_positionChanged;//0x9400;
  DrawCharacterState(chIdx);
}

// *********************************************************
//
// *********************************************************
// Internal subroutine called by TAG014832
void TAG014a60(dReg D0,
               dReg D2,
               dReg D3,
               dReg D4,
               dReg D5,
               dReg D6,
               dReg D7,
               aReg& A2)
{
  dReg D1;
  D2W = LE16(D2W);
  D3W = LE16(D3W);
  D4W = LE16(D4W);
  D5W = LE16(D5W);
  for (D1W=24; D1W>=0; D1W--)
  {
    if (D0W <= D1W)
    {
      wordGear(A2+ 0) &= D4W;
      wordGear(A2+ 8) &= D5W;
      wordGear(A2+ 2) &= D4W;
      wordGear(A2+10) &= D5W;
      wordGear(A2+ 4) |= D2W;
      wordGear(A2+12) |= D3W;
      wordGear(A2+ 6) |= D2W;
      wordGear(A2+14) |= D3W;
    }
    else
    {
      if (D6W)
      {
        wordGear(A2+0) |= D2W;
        wordGear(A2+8) |= D3W;
      }
      else
      {
        wordGear(A2+0) &= D4W;
        wordGear(A2+8) &= D5W;
      };
      if (D6L < 0)
      {
        wordGear(A2+ 2) |= D2W;
        wordGear(A2+10) |= D3W;
      }
      else
      {
        wordGear(A2+ 2) &= D4W;
        wordGear(A2+10) &= D5W;
      };
      if (D7W)
      {
        wordGear(A2+ 4) |= D2W;
        wordGear(A2+12) |= D3W;
      }
      else
      {
        wordGear(A2+ 4) &= D4W;
        wordGear(A2+12) &= D5W;
      };
      if (D7L < 0)
      {
        wordGear(A2+ 6) |= D2W;
        wordGear(A2+14) |= D3W;
      }
      else
      {
        wordGear(A2+ 6) &= D4W;
        wordGear(A2+14) &= D5W;
      };
    };
    A2 += 160;
  }; // for (D1W...)
}

// *********************************************************
//
// *********************************************************
//   TAG014832
void DrawLifeBars(i32 chIdx)
{
  dReg D0, D1, D2, D3, D4, D5, D6, D7;
  aReg A0, A2;
  i16 LOCAL_6[3]; // The three bar heights
  i16 *pwA3;
  CHARDESC *pcA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pcA3 = &d.CH16482[chIdx];
  D6W = 0;
  if (pcA3->HP() > 0)
  {
    D5L = 25 * (pcA3->HP() << 10) / pcA3->MaxHP();
    if (D5L&0x3ff)
    {
      D0L = (D5L >> 10) + 1;
      //D1L = (D6W+1) << 1;
      LOCAL_6[D6W++] = D0W;
    }
    else
    {
      D0L = D5L >> 10;
      //D1L = (D6W+1)<<1;
      LOCAL_6[D6W++] = D0W;
    };
  }
  else
  {
    //D0L = (D6W++) << 1;
    LOCAL_6[D6W++] = 0;
  };
  if (pcA3->Stamina() > 0)
  {
    D5L = 25*((pcA3->Stamina())<<10) / pcA3->MaxStamina();
    //TAG0030e6(pcA3->word58, D5L, &D5L, NULL)
    if (D5L & 0x3ff)
    {
      D0L = (D5L >> 10) + 1;
      LOCAL_6[D6W++] = D0W;
    }
    else
    {
      D0L = D5L >> 10;
      LOCAL_6[D6W++] = D0W;
    };
  }
  else
  {
    LOCAL_6[D6W++] = 0;
  };
  if (pcA3->Mana() > 0)
  {
    if (pcA3->Mana() > pcA3->MaxMana())
    {
      LOCAL_6[D6W] = 25;
    }
    else
    {
      D5L = ((25 * pcA3->Mana()) << 10) / pcA3->MaxMana();
      if (D5L & 0x3ff)
      {
        D0L = (D5L >> 10) + 1;
        LOCAL_6[D6W] = D0W;

      }
      else
      {
        D0L = D5L >> 10;
        LOCAL_6[D6W] = D0W;
      };
    };
  }
  else
  {
    LOCAL_6[D6W] = 0;
  };
  STHideCursor(HC2);
  A0 = (aReg)d.LogicalScreenBase+320;
  D1L = chIdx;
  if (D1W >= 3)
  {
    D6L = 0x80000000;
    D7L = D6L + 1;
  }
  else
  {
    if (D1W >= 2)
    {
      D6L = 0;
      D7L = 0x80000000;
    }
    else
    {
      if (D1W >= 1)
      {
        D6L = 0x80000001;
        D7L = D6L - 1;
      }
      else
      {
        D6L = 0x80000001;
        D7L = 1;
      };
    };
  };
  //LOCAL_8 = D1W + 4;
  pwA3 = LOCAL_6;
  for (i32 i=0; i<3; i++) // Three bars
  {
    A2 = A0;
    //pwA1 = d.Word1526[0];
    A2 += d.Word1526[D1W][i];
    //pwA1 = d.Word1502[0];
    D2W = d.Word1502[D1W][2*i];
    D3W = d.Word1502[D1W][2*i+1];
    D4W = (I16)(D2W ^ 0xffff);
    D5W = (I16)(D3W ^ 0xffff);
    D0W = *(pwA3++); // bar height
    TAG014a60(D0,D2,D3,D4,D5,D6,D7,A2);
    //if (LOCAL_8 == D1W) break;
    //D1W++;
  };
  STShowCursor(HC2);
}

// *********************************************************
//
// *********************************************************
const char* TAG014af6(i32 num, i32 flag,i32 width)
{ // if flag==0 then no leading blanks.
  dReg D0;
  char *result;
  aReg A1;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (num > 9999) num = 9999;
  D0W = sw(4-width);
  A1 = (aReg)d.Byte16606;
  result = (char *)A1 + D0W;
  wordGear(A1) = 0x2020;;
  wordGear(A1+2) = 0x2020;
  A1 += 4;
  *A1 = 0;
  //D1L = 0;
  do
  {
    *(--A1) = sb((num%10) + '0');
    num /= 10;
  } while (num != 0);
  if (flag == 0) result = (char *)A1;
  return result;
}

// *********************************************************
//
// *********************************************************
//   TAG014b32
void PrintLifeForce(i32 line, i32 current, i32 max)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//
  TextToViewport(55,
            line,
            COLOR_13,  //color
            TAG014af6(current, 1, 3),
            false);
  TextToViewport(73, line, COLOR_13, "/", false);
  TextToViewport(79,
            line,
            COLOR_13,
            TAG014af6(max,1,3),
            false);
}

// *********************************************************
//
// *********************************************************
//   TAG014b9e
void PrintLifeForces(CHARDESC *pChar)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  PrintLifeForce(116, pChar->HP(), pChar->MaxHP());
  PrintLifeForce(124,pChar->Stamina()/10, pChar->MaxStamina()/10);
  PrintLifeForce(132, pChar->Mana(), pChar->MaxMana());
}

// *********************************************************
//
// *********************************************************
//   TAG014bf4
void DisplayBackpackItem(i32 chIdx, i32 itemNum)
{
  dReg D0, D5, D7;
  RN objD6;
//  aReg A2;
  ICONDISPLAY *idA2;
  CHARDESC *pcA3;
  OBJ_NAME_INDEX objNID4;
  i16 LOCAL_26;
  ui8 *LOCAL_24;
  i16 LOCAL_20;
  RectPos LOCAL_18;
  i32 squareNumber;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(itemNum);
  LOCAL_20 = -1;
  pcA3 = &d.CH16482[chIdx];
  // Is it to be drawn in viewport?
  D5W = sw((d.SelectedCharacterOrdinal == (chIdx+1)) ? 1 : 0);
  if (D5W == 0)
  {
    if (D7W > 1) return;
    if (d.PotentialCharacterOrdinal == (chIdx+1)) return;
    squareNumber = 2*chIdx + D7W;
  }
  else
  {
    squareNumber = D7W + 8; // Skip over hands in portrait areas
  };
  if (D7W >= 30)
  {
    objD6 = d.rnChestContents[D7W-30];
  }
  else
  {
    objD6 = pcA3->Possession(D7W);
  };
  idA2 = &d.IconDisplay[squareNumber];
  LOCAL_18.w.x1 = sw(idA2->pixelX - 1);
  LOCAL_18.w.x2 = sw(LOCAL_18.w.x1 + 17);
  LOCAL_18.w.y1 = sw(idA2->pixelY - 1);
  LOCAL_18.w.y2 = sw(LOCAL_18.w.y1 + 17);
  if (D5W) // if drawing backback screen
  {
    LOCAL_24 = d.pViewportBMP;
    LOCAL_26 = 112;
  }
  else
  {
    LOCAL_24 = d.LogicalScreenBase;
    LOCAL_26 = 160;
    STHideCursor(HC3);
  };
  if (objD6 == RNnul)
  {
    if (D7W <= 5)
    {
      objNID4 = objNI_Special_n;
      objNID4 = (OBJ_NAME_INDEX)(objNID4 + 2*D7W);
      D0W = sw(pcA3->ouches & (1 << D7W));
      if (D0W)
      {
        objNID4 = (OBJ_NAME_INDEX)(objNID4+1);
        LOCAL_20 = 34;
      }
      else
      {
        LOCAL_20 = 33;
      };
    }
    else
    {
      if ( (D7W>=10) && (D7W <= 13) )
      {
        objNID4 = (OBJ_NAME_INDEX)(objNI_Special_j + (D7W - 10));
      }
      else
      {
        objNID4 = objNI_Special_f;
      };
    };
  }
  else
  {
    objNID4 = objD6.NameIndex();
    if (   (D5W)
        && (D7W==1)
        && ((objNID4==objNI_Chest) || (objNID4==objNI_OpenScroll)) )
    {
      objNID4 = (OBJ_NAME_INDEX)(objNID4 + 1);;
    };
    if (D7W <= 5)
    {
      if (pcA3->ouches & (1 << D7W))
      {
        LOCAL_20 = 34;
      }
      else
      {
        LOCAL_20 = 33;
      };
    };
  };
  if ( (D7W==1) && ((chIdx+1) == d.AttackingCharacterOrdinal) )
  {
    LOCAL_20 = 35;
  };
  if (LOCAL_20 != -1)
  {
    d.UseByteCoordinates = 0;
    TAG0088b2(GetBasicGraphicAddress(LOCAL_20),
              (ui8 *)LOCAL_24,
              &LOCAL_18,
              0,
              0,
              16,
              LOCAL_26,
              12);
  };
  DrawItem(squareNumber, objNID4);
  if (D5W == 0)
  {
    STShowCursor(HC3);
  };
}

// *********************************************************
//
// *********************************************************
//   TAG014de0
void DrawCharacterState(i32 chIdx) // Character box at top of screen                                //
{//(void)
  static dReg D0, D1, D4;
  static bool inventoryOpen;
  static ui16 charFlags;
  static const char* pcA0;
  static i32 itemNum;
  static CHARDESC *pcA3, *pcA2;
  static i32 maxLoad;
  static i16 LOCAL_32;
  i16    shieldType[3]; //LOCAL_22[3];
  RectPos portraitRect;
  RectPos nameRect;
  i32     numShield; //D4W
  static i16 LOCAL_8;
  static i16 LOCAL_6;
  static i16 LOCAL_4;
  static i8  LOCAL_1;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  LOCAL_4 = sw(69 * chIdx);
  pcA3 = d.CH16482 + chIdx;
  charFlags = pcA3->charFlags;
  if ((charFlags & CHARFLAG_graphicFlags) == 0) return;
  inventoryOpen = ((chIdx+1) == d.SelectedCharacterOrdinal);
  d.UseByteCoordinates = 0;
  STHideCursor(HC4);
  if((charFlags & CHARFLAG_portraitChanged)!=0)
  {
    portraitRect.w.y1 = 0;
    portraitRect.w.y2 = 28;
    portraitRect.w.x1 = LOCAL_4;
    portraitRect.w.x2 = sw(portraitRect.w.x1 + 66);
    if (pcA3->HP() != 0)
    {
      FillRectangle(d.LogicalScreenBase, &portraitRect, 12, 160);//Clear rectangle at top
      ClearMemory((ui8 *)shieldType, sizeof(shieldType));
      numShield = 0;
      if (d.FireShield > 0)
      {
        shieldType[numShield++] = 38;
      };
      if (d.SpellShield > 0)
      {
        shieldType[numShield++] = 39;
      };
      if ((d.PartyShield>0) || (pcA3->shieldStrength!=0))
      {
        shieldType[numShield++] = 37;
      };
      while ((numShield--) != 0)
      {
        BLT2Screen(GetBasicGraphicAddress(shieldType[numShield]), // Puts dots around character status box
                  &portraitRect,
                  40,
                  10);
      } ;
      if (inventoryOpen)
      {
        CharacterPortraitToStatusBox(chIdx);
        charFlags |= CHARFLAG_statsChanged;
      }
      else
      {
        charFlags |=   CHARFLAG_weaponAttack
                     | CHARFLAG_ouches
                     | CHARFLAG_statsChanged
                     | CHARFLAG_cursor
                     ;
      };
    }
    else
    {
      BLT2Screen(GetBasicGraphicAddress(8), &portraitRect, 40, -1);
      TextOutToScreen(LOCAL_4+1, 5, 13, 1, pcA3->name);
      DrawAttackButton(chIdx);
      STShowCursor(HC4);
      return;
    };
  };
  if (pcA3->HP() != 0)
  {
    if ((charFlags & CHARFLAG_cursor) != 0)
    {
      D4W = sw(chIdx);
      if (D4W == d.HandChar) // cursor's owner
      {
        D4W = 9;
      }
      else
      {
        D4W = 13;
      };
      if (inventoryOpen)
      {
        pcA2 = pcA3;
        TextToViewport(3, 7, D4W, pcA2->name, false); //D4W is text color
        LOCAL_32 = StrLen(pcA2->name);
        LOCAL_6 = sw(6 * LOCAL_32 + 3);
        LOCAL_1 = pcA3->title[0];
        if (LOCAL_1 != 44)
        {
          if (LOCAL_1 != 59)
          {
            if (LOCAL_1 != 45)
            {
              LOCAL_6 += 6;
            };
          };
        };
        TextToViewport(LOCAL_6, 7, D4W, pcA3->title, false);//D4W is text color
        charFlags |= CHARFLAG_viewportChanged;
      }
      else
      {
        nameRect.w.y1 = 0;
        nameRect.w.y2 = 6;
        nameRect.w.x1 = LOCAL_4;
        nameRect.w.x2 = sw(nameRect.w.x1 + 42);
        FillRectangle(d.LogicalScreenBase,
                      &nameRect,
                      1,
                      160);
        TextOutToScreen(LOCAL_4+1,
                        5,
                        D4W,
                        1,
                        pcA3->name);
      };
    };
    if ((charFlags & CHARFLAG_statsChanged) != 0)
    {
      DrawLifeBars(chIdx);
      if (inventoryOpen)
      {
        PrintLifeForces(pcA3);//Health,stamina,mana bars
        if (   (pcA3->food < 0)
            || (pcA3->water < 0)
            || (pcA3->poisonCount != 0) )
        {
          LOCAL_8 = 34;
        }
        else
        {
          LOCAL_8 = 33;
        };
        BLT2Viewport(GetBasicGraphicAddress(LOCAL_8),    // Box around mouth
                  (RectPos *)d.Word1398,
                  16,
                  12);
        LOCAL_8 = 33;
        for (D4W=1; D4W<=6; D4W++)
        {
          D0W = pcA3->Attributes[D4W].Current();
          D1W = pcA3->Attributes[D4W].Maximum();
          if (D0W < D1W)
          {
            LOCAL_8 = 34;
            break;
          };
        };
        BLT2Viewport(GetBasicGraphicAddress(LOCAL_8),  //Box around eye
                    (RectPos *)d.Word1406,
                    16,
                    12);
        charFlags |= CHARFLAG_viewportChanged;
      };
    };
    if ((charFlags & CHARFLAG_ouches) != 0)
    {
 //     itemNum = D5W;
 //     if (itemNum != 0)
 //     {
 //       itemNum = 5;
 //     }
 //     else
 //     {
 //       itemNum = 1;
 //     };
      itemNum = inventoryOpen ? 5 : 1;
      while (itemNum >= 0)
      {
        DisplayBackpackItem(chIdx, itemNum);
        itemNum--;
      };
      if (inventoryOpen)
      {
        charFlags |= CHARFLAG_viewportChanged;
      };
    };
    if ( (charFlags & CHARFLAG_possession) && inventoryOpen )
    {
      D4W = sw(maxLoad = MaxLoad(pcA3));
      //LOCAL_42 = D4W;
      if (pcA3->load > maxLoad)
      {
        LOCAL_8 = COLOR_8;
      }
      else
      {
        D0L = 8*pcA3->load;
        D1L = D4W;
        D1L = 5 * D1L;
        if (D0L > D1L)
        {
          LOCAL_8 = COLOR_11;
        }
        else
        {
          LOCAL_8 = COLOR_13;
        };
      };
      TextToViewport(104, 132, LOCAL_8, "LOAD", true);//LOCAL_8 is text color
      D4W = sw(pcA3->load / 10);
      pcA0 = (char *)TAG014af6(D4W, 1, 3);
      StrCpy(d.Byte12914, pcA0);
      StrCat(d.Byte12914, ".");
      D4W = sw(pcA3->load - 10*D4W);
      pcA0 = (char *)TAG014af6(D4W, 0, 1);
      StrCat(d.Byte12914, pcA0);
      StrCat(d.Byte12914, "/");
      D0L = MaxLoad(pcA3);
      D4W = sw((D0L+5)/10);
      pcA0 = (char *)TAG014af6(D4W, 1, 3);
      StrCat(d.Byte12914, pcA0);
      StrCat(d.Byte12914, " KG");
      TextToViewport(148, 132, LOCAL_8, d.Byte12914, false);// "LOAD  actual/max" 
                                                     // LOCAL_8 is text color
      charFlags |= CHARFLAG_viewportChanged;
    };
    D4W = sw((pcA3->charPosition+4-d.partyFacing) & 3);
    if (charFlags & CHARFLAG_positionChanged)
    {
      if ((D4W+1) != d.Word23144)
      {
//
         FillRectangle(d.LogicalScreenBase,   // Fill pos/dir rectangle
                      &d.wRectPos1454[D4W], // with character's color
                      d.Byte1386[chIdx],
                      160);
         TAG0088b2(GetBasicGraphicAddress(28),  // Overlay character's pos/dir icon
                   (ui8 *)d.LogicalScreenBase,
                   &d.wRectPos1454[D4W],
                   ((pcA3->facing+4-d.partyFacing) & 3) * 19,
                   0,
                   40,
                   160,
                   12);
      };
    };
    if ( (charFlags & CHARFLAG_chestOrScroll) && inventoryOpen )
    {
      if (d.PressingMouth)
      {
        DisplayFoodWater();
      }
      else
      {
        if (d.PressingEye)
        {
          if (d.EmptyHanded) //cursor hand
          {
            TAG019036();
          };
        }
        else
        {
          TAG0189d4();
        };
      };
      charFlags |= CHARFLAG_viewportChanged;
    };
    if (charFlags & CHARFLAG_weaponAttack)
    {
      DisplayBackpackItem(chIdx, 1);
      DrawAttackButton(chIdx);
      if (inventoryOpen)
      {
        charFlags |= CHARFLAG_viewportChanged;
      };
    };
    if (charFlags & CHARFLAG_viewportChanged)
    {
      MarkViewportUpdated(0);
    };
  };
  pcA3->charFlags &= CHARFLAG_nonGraphicFlags;//0x7f;
  STShowCursor(HC4);
}

// *********************************************************
//
// *********************************************************
i16 TAG0153c2(i32 chIdx,
              i32 possessionIndex1,
              i32 possessionIndex2)
{
  dReg D0, D6;
  CHARDESC   *pcA3;
  WEAPONDESC *weaponDescA2;
  RN  objD7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pcA3 = &d.CH16482[chIdx];
  objD7 = pcA3->Possession(possessionIndex1);
  if (objD7 == RNnul) return 0;
  if (objD7.dbType() != dbWEAPON) return 0;
  weaponDescA2 = TAG0099d2(objD7);
  if ( (weaponDescA2->uByte1 >= 16) && (weaponDescA2->uByte1 <= 31) )
  {
    D6W = 10;
  }
  else
  {
    if ( (weaponDescA2->uByte1 >= 32) && (weaponDescA2->uByte1 <= 47) )
    {
      D6W = 11;
    }
    else
    {
      return 0;
    };
  };
  objD7 = pcA3->Possession(possessionIndex2);
  //
  // I added the following line.  So TAG0099d2
  // won't complain.
  // We return anyway if db != 5 and TAG0099d2 has no
  // side-effects.
  if (objD7 == RNnul) return 0;
  if (objD7.dbType() != dbWEAPON) return 0;
  weaponDescA2 = TAG0099d2(objD7);
  D0W = sw(objD7.dbType());
  //Must be 5! ... if (D0W != 5) return 0;
  if (weaponDescA2->uByte1 != D6W) return 0;
  return 1;
}

// *********************************************************
//
// *********************************************************
//   TAG0157e8
void AdjustStatsForItemCarried (
               CHARDESC *pChar,
               i32 possessionIndex,
               i32 addOrRemove,
               RN  item)
{
  dReg D4;
  OBJ_NAME_INDEX objNI;
  DB5      *pDB5_4;
  DB6      *pDB6_4;
  DBTYPE DBType;
  enum
  {
    adjustLuck      = 0,
    adjustStrength  = 1,
    adjustDexterity = 2,
    adjustWisdom    = 3,
    adjustVitality  = 4,
    adjustAntiMagic = 5,
    adjustAntiFire  = 6,
    adjustMaxMana   = 8,
    adjustIllegal   = 99
  } whatToAdjust = adjustIllegal;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7W = sw(P2);
  //D6W = P3;
  objNI = item.NameIndex();
  D4W = 0; // Assume no adjustment to Luck
  DBType = item.dbType();
  for (;;) // Fake to avoid 'goto's.  I like 'break's better.
  {
    // The D7 tests were strange in the original.  It appeared
    // to be an unsigned test for >= 0.  Unnecessary.
    if (    (DBType == dbWEAPON)
         && (possessionIndex>=0) && (possessionIndex<=12) )
    { // Weapon or clothing.  Person, purse, or quiver.
      // Handled together in the original.  I wroke them apart.
      pDB5_4 = GetRecordAddressDB5(item);
//
//
//
      if ( pDB5_4->cursed() )
      {
        whatToAdjust = adjustLuck;
        D4W = -3;
        break;
      };
    };
    if (    (DBType == dbCLOTHING)
         && (possessionIndex>=0)
         && (possessionIndex<=12) )
    { // Weapon or clothing.  Person, purse, or quiver.
      // Handled together in the original.  I wroke them apart.
      pDB6_4 = GetRecordAddressDB6(item);
      if ( pDB6_4->cursed() )
      {
        whatToAdjust = adjustLuck;
        D4W = -3;
        break;
      };
    };
    if ( (objNI==objNI_RabbitsFoot) && (possessionIndex<30) )
    {
      whatToAdjust = adjustLuck;
      D4W = 10;
      break;
    };
    if (possessionIndex == 1) //Weapon hand
    {
      if (objNI == objNI_MaceOfOrder)
      {
        whatToAdjust = adjustStrength;
        D4W = 5;
        break;
      }
      else
      {
        whatToAdjust = adjustMaxMana;
        if (   (objNI >= objNI_StaffOfClaws_a)
            && (objNI <= objNI_StaffOfClaws_c) )
        {
          D4W = 4;
        }
        else
        {
          if ( (objNI >= objNI_Staff) && (objNI <= objNI_SceptreOfLyf) )
          {
            switch (objNI)
            {
            case objNI_Staff: D4W = 2; break;

            case objNI_Wand: D4W = 1; break;

            case objNI_TeoWand: D4W = 6; break;

            case objNI_YewStaff: D4W = 4; break;

            case objNI_StaffOfIrra: D4W = 10; break;

            case objNI_CrossOfNeta: D4W = 8; break;

            case objNI_SerpentStaff: D4W = 16; break;

            case objNI_DragonSpit: D4W = 7; break;

            case objNI_SceptreOfLyf: D4W = 5; break;
            };
          }
          else
          {
            if (objNI == objNI_SideSplitter)
            {
              D4W = 1;
            }
            else
            {
              if (objNI == objNI_DragonFang)
              {
                D4W = 2;
              }
              else
              {
                if (objNI == objNI_VorpalBlade)
                {
                  D4W = 4;
                };
              };
            };
          };
        };
      };
      break;
    };
    if (possessionIndex == 4) // Legs
    {
      if (objNI == objNI_Powertowers)
      {
        whatToAdjust = adjustStrength;
        D4W = 10;
      };
      break;
    };
    if (possessionIndex == 2) // Head
    {
      if (objNI == objNI_CrownOfNerra)
      {
        whatToAdjust = adjustWisdom;
        D4W = 10;
      }
      else
      {
        if (objNI == objNI_Dexhelm)
        {
          whatToAdjust = adjustDexterity;
          D4W = 10;
        };
      };
      break;
    };
    if (possessionIndex == 3) // on Body
    {
      if (objNI == objNI_Flamebain)
      {
        whatToAdjust = adjustAntiFire;
        D4W = 12;
      }
      else
      {
        if (objNI == objNI_CloakOfNight)
        {
          whatToAdjust = adjustDexterity;
          D4W = 8;
        };
      };
      break;
    };
    if (possessionIndex != 10) break; //Necklace
    if ( (objNI >= objNI_JewelSymal_a) && (objNI <= objNI_JewelSymal_b) )
    {
      whatToAdjust = adjustAntiMagic;
      D4W = 15;
      break;
    };
    if (objNI == objNI_CloakOfNight)
    {
      whatToAdjust = adjustDexterity;
      D4W = 8;
      break;
    };
    if (objNI != objNI_Moonstone) break;
    whatToAdjust = adjustMaxMana;
    D4W = 3;
    break;
  }; // Fake 'while' loop to avoid 'goto's
  if (D4W == 0) return;
  ASSERT(whatToAdjust != adjustIllegal,"Illegal adjust");
  D4W = sw(D4W * addOrRemove);// Increment or decrement;
  if (whatToAdjust == adjustMaxMana)
  {
    pChar->MaxMana(pChar->MaxMana() + D4W);
  }
  else
  {
    if (whatToAdjust <= adjustAntiFire)
    {
        pChar->Attributes[whatToAdjust].IncrMaximum(D4W);
        pChar->Attributes[whatToAdjust].IncrCurrent(D4W);
        pChar->Attributes[whatToAdjust].IncrMinimum(D4W);
    };
  };
  pChar->charFlags |= CHARFLAG_chestOrScroll | CHARFLAG_statsChanged;//0x900;
}

void SetCursed(RN obj, bool curse)
{
  i32 chIdx;
  CHARDESC *pCH;
  pCH = NULL;
  chIdx = -1;
  i32 ch;
  i32 loc=-1;
  for (ch=0; ch<d.NumCharacter; ch++)
  {
    for (loc=0; loc<30; loc++)
    {
      if (obj == d.CH16482[ch].Possession(loc))
      {
        pCH = &d.CH16482[ch];
        break;
      };
    };
  };
  if (pCH != NULL) AdjustStatsForItemCarried(pCH, loc, -1, obj);
  switch(obj.dbType())
  {
  case dbWEAPON: 
      GetRecordAddressDB5(obj)->cursed(curse);
      break;
  case dbCLOTHING: 
      GetRecordAddressDB6(obj)->cursed(curse);
      break;
  case dbMISC: 
      GetRecordAddressDB10(obj)->cursed(curse);
      break;
  default: break;
  };
  if (pCH != NULL) AdjustStatsForItemCarried(pCH, loc, +1, obj);
}

// *********************************************************
//
// *********************************************************
// TAG015a66
RN RemoveCharacterPossession(i32 chIdx,i32 possessionIndex)
{//(RN)
  dReg D0;
  RN objD5, objRemoved;
  OBJ_NAME_INDEX objNID5;
  DBCOMMON *dbA2;
  CHARDESC *pcA3;
  i16 LOCAL_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pcA3 = &d.CH16482[chIdx];
  if (possessionIndex >= 30)
  {
    objRemoved = d.rnChestContents[possessionIndex-30];
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"Remove object %04x from chest index %d belonging to %s\n",
                   objRemoved.ConvertToInteger(), possessionIndex, pcA3->name);
    };
    d.rnChestContents[possessionIndex-30] = RN(RNnul);
  }
  else
  {
    objRemoved = pcA3->Possession(possessionIndex);
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"Remove object %04x from backpack index %d belonging to %s\n",
                   objRemoved.ConvertToInteger(), possessionIndex, pcA3->name);
    };
    pcA3->SetPossession(possessionIndex, RN(RNnul), true);
  };
  if (objRemoved == RNnul)
  {
    return (RNnul);
  };
  LOCAL_2 = sw(chIdx+1 == d.SelectedCharacterOrdinal ? 1 : 0);
  objNID5 = objRemoved.NameIndex();
  AdjustStatsForItemCarried(pcA3, possessionIndex, -1, objRemoved);
  dbA2 = GetCommonAddress(objRemoved);
  if (possessionIndex == 10)
  {
    if (   (objNID5 >= objNI_Illumulet_a)
        && (objNID5 <= objNI_Illumulet_b) )
    {
      dbA2->CastToDB10()->value(0); // Of no use anymore.
      d.Brightness = sw(d.Brightness - d.Word1074[2]);
      SelectPaletteForLightLevel();
    }
    else
    {
      if (   (objNID5 >= objNI_JewelSymal_a)
          && (objNID5 <= objNI_JewelSymal_b) )
      {
        dbA2->CastToDB10()->value(0);//Of no use anymore
      };
    };
  };
  DisplayBackpackItem(chIdx, possessionIndex);
  if (LOCAL_2)
  {
    pcA3->charFlags |= CHARFLAG_viewportChanged;
  };
  if (possessionIndex <2)
  {
    if (possessionIndex == 1)
    {
      pcA3->charFlags |= CHARFLAG_weaponAttack;
      if (d.AttackingCharacterOrdinal == chIdx+1)
      {
        TAG01b1c0();
      };
      if (   (objNID5 >= objNI_OpenScroll)
          && (objNID5 <= objNI_Scroll) )
      {
        dbA2->CastToDB7()->open(false);
      };
    };
    if (   (objNID5 >= objNI_Torch_a)
        && (objNID5 <= objNI_Torch_d) )
    {
      dbA2->CastToDB5()->litTorch(0);
      //dbA2->word(2) = dbA2->word(2) & 0x7fff;
      SelectPaletteForLightLevel();
      DrawEightHands();
    };
    if ( (LOCAL_2) && (possessionIndex==1) )
    {
      if (objNID5 == objNI_Chest)
      {
        RepackChest();
        if (TimerTraceActive)
        {
          fprintf(GETFILE(TraceFile),"%06x RemoveCharacterPossession calling Repack Chest\n",d.Time);
        };
        pcA3->charFlags |= CHARFLAG_chestOrScroll;
      }
      else
      {
        if (   (objNID5 >= objNI_OpenScroll)
            && (objNID5 <= objNI_Scroll) ) pcA3->charFlags |= CHARFLAG_chestOrScroll;
      };
    };
  };
  D0W = sw(GetObjectWeight(objRemoved));
  DEBUGLOAD(pcA3, D0W, -1, objRemoved);
  pcA3->load = sw(pcA3->load - D0W);
  ASSERT(pcA3->load < MAXLOAD,"load > maxload");
  pcA3->charFlags |= CHARFLAG_possession;
  return (objRemoved);
}

// *********************************************************
//
// *********************************************************
//   TAG015c4c
void AddCharacterPossession(i32 chIdx,RN object,i32 place)
{//(void)
  //dReg D4;
  OBJ_NAME_INDEX objNID4;
  CHARDESC *pcA3;
  DBCOMMON *dbA2;
  bool bool_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (object == RNnul) return;
  pcA3 = &d.CH16482[chIdx];
  if (place >= 30) // In a chest?
  {
    d.rnChestContents[place-30] = object;
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"Place object %04x in Chest location %d\n",
              object.ConvertToInteger(),place-30);
    };
  }
  else
  {
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"Place object %04x on %s at location %d\n",
                       object.ConvertToInteger(), pcA3->name, place);
    };
    pcA3->SetPossession(place, object, true);
  };
  DEBUGLOAD(pcA3, GetObjectWeight(object), +1, object);
  pcA3->load = sw(pcA3->load + GetObjectWeight(object));
  ASSERT(pcA3->load < MAXLOAD,"load > maxload");
  pcA3->charFlags |= CHARFLAG_possession;
  objNID4 = object.NameIndex();
  bool_2 = chIdx+1 == d.SelectedCharacterOrdinal;
  AdjustStatsForItemCarried(pcA3, place, 1, object);
  dbA2 = GetCommonAddress(object);
  if (place < 2)
  { // In a hand
    if (place == 1)
    { // In fighting hand
      pcA3->charFlags |= CHARFLAG_weaponAttack;
      if (d.AttackingCharacterOrdinal == chIdx+1)
      {
        TAG01b1c0();
      };
      if (   (objNID4 >= objNI_OpenScroll)
          && (objNID4 <= objNI_Scroll) )
      {
        dbA2->CastToDB7()->open(true);
        DrawEightHands();
      };
    };
    if (objNID4 == objNI_Torch_a)
    {
      dbA2->CastToDB5()->litTorch(1);
      //SETBITS15_15(dbA2->word(2), 1);
      SelectPaletteForLightLevel();
      DrawEightHands();
    }
    else
    {
      if ( bool_2 && (place==1) )
      {
        if (    (objNID4 == objNI_Chest)
             || ( (objNID4 >= objNI_OpenScroll) && (objNID4 <= objNI_Scroll) ) )
        {
          pcA3->charFlags |= CHARFLAG_chestOrScroll;
        };
      };
    };
  }
  else
  { //Not in a hand.
    if (place ==10)
    { // Necklace
      if (   (objNID4 >= objNI_Illumulet_a)
          && (objNID4 <= objNI_Illumulet_b) )
      {
        dbA2->CastToDB10()->value(1);
        //SETBITS14_15(dbA2->word(2), 1);
        d.Brightness = sw(d.Brightness + d.Word1074[2]);
        SelectPaletteForLightLevel();
        objNID4 = (OBJ_NAME_INDEX)(objNID4 + 1);
      }
      else
      {
        if (   (objNID4 >= objNI_JewelSymal_a)
            && (objNID4 <= objNI_JewelSymal_b) )
        {
          dbA2->CastToDB10()->value(1);
          //SETBITS14_15(dbA2->word(2), 1);
          objNID4 = (OBJ_NAME_INDEX)(objNID4 + 1);
        };
      };
    };
  };
  DisplayBackpackItem(chIdx, place);
  if (bool_2)
  {
    pcA3->charFlags |= CHARFLAG_viewportChanged;
  };
}

// *********************************************************
//
// *********************************************************
i16 TAG016426(CHARDESC *pChar,i16 attrNum,i16 P3)
{
  dReg D0, D7;
  i16 w_10;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  w_10 = pChar->Attributes[attrNum].Current();
  //
  // *************** Spohia thinks the '/256' is a bug.**********
  // I will remove it as a test to let people try the game without it.
  //
  //D7W = sw(170 - w_10/256); //They stored a word and fetch a byte!
                            //Appears it was purposeful!
  //D7W = sw(170 - w_10); //They stored a word and fetch a byte!
  // *************************************************************

  D7W = sw(170 - w_10); //They stored a word and fetch a byte!  See discussion below.
/*
 *              Forum discussion.  Christophe Fontanel wrote the following:
 *But I did some additional research, and now I still think you should fix CSBwin as proposed, 
 *but this time for a different reason 
 *
 *Two facts:
 *1) What I said about the C source code I provided in my first post stands true: 
 *this code is the code that FTL used, because when compiled with the compiler that
 *they used back at the time, I get the exact same binary machine code.
 *Additional information: I have disassembled all Atari ST versions of both Dungeon 
 *Master and Chaos Strikes Back, and this function is exactly the same
 *in all of these versions.
 *2) What you said about the 68000 processor is correct, and clearly shows that the 
 *actual byte that is used in the computation is not the expected one. I have run the 
 *game in Steem debugger (ST emulator) and confirm the behavior that you described:
 *At the beginning of the machine code, d0 is initialized (as a word) with the statistic 
 *index that was passed as a parameter. This value can be 0 to 6 (there are 7 
 *statistics per champion). Then it is multiplied by 3 resulting in value 0 to 18. 
 *Consequently, the high byte of d0 is always 0 in all calls to this function. This 
 *means that the code always computes 170 - 0 = 170, thus completely ignoring the 
 *antifire, antimagic or vitality values of the champion. Clearly, this cannot be 
 *a behavior intended by the designers, that would make no sense!
 *
 *Because of these two facts, the only possible conclusion is that this is a bug 
 *in the Megamax C compiler itself. The 10 bytes temporary local variable is fully 
 *managed by the compiler, so this byte/word mismatch is now clearly a bug on the 
 *compiler part. The C code, as written by FTL, is correct, but the resulting 
 *machine code is just wrong.
 *
 *By using "/256" in the CSBwin source code, you reproduce the same buggy 
 *behavior: the statistic value is stored in one byte so with a value 
 *between 0 and 255. Divided by 256, you always get 0. You could as 
 *well replace D7W = sw(170 - w_10/256); by D7W = sw(170);
 *
 *
 *I had a look at the Amiga version of Dungeon Master 2.0 (that I have also 
 *disassembled). It was compiled with a completely different compiler (Aztec C, from Manx company).
 *Here is the assembly code for the same function:
 *Code: Select all
 *            LINK.W    A5,#0
 *            MOVE.L    D4,-(A7)    Backup D4 to the stack (D4 is used for 
 *                                  a local variable in this function)
 *            MOVE.W    12(A5),D0   12(A5) contains the statistic index, 
 *                                  as a function parameter
 *            MULU      #$0003,D0   3 bytes for each statistic
 *            MOVEA.L   D0,A0
 *            ADDA.L    8(A5),A0    8(A5) contains the address of the 
 *                                  champion data
 *            MOVEQ     #0,D0
 *            MOVE.B    71(A0),D0   This gets the current statistic byte 
 *                                  value and "converts" it to a word (high byte 
 *                                  set to 0 on previous line)
 *            MOVE.W    #$00aa,D4
 *            SUB.W     D0,D4       D4 = 170 - CurrentStatistic
 *            CMP.W     #$0010,D4
 *            BGE.S     LAB_0D77    Branch if D4 >= 16
 *            MOVE.W    14(A5),D0   Attack value
 *            LSR.W     #3,D0
 *LAB_0D76:   MOVE.L    (A7)+,D4    Restore D4 from the stack
 *            UNLK      A5
 *            RTS
 *LAB_0D77:   MOVE.W    D4,-(A7)
 *            MOVE.W    #$0007,-(A7)
 *            MOVE.W    14(A5),-(A7)
 *            JSR       -31104(A4)  Call ScaledMultiply function
 *            ADDQ.W    #6,A7
 *            BRA.S     LAB_0D76
 *
 *
 *As you can see, there is no mumbo jumbo with hidden local variables as 
 *in the Atari ST version. There is no ambiguity, and this time the correct 
 *statistic value is used to compute the difference with 170. Antifire and Antimagic do work fine in Amiga versions of the game.
 *
 *As a conclusion, I think there is indeed a bug in the original Atari ST 
 *version, but it is not a bug by FTL, it is a compiler bug. This bug is not 
 *present in other versions of the game because they use other compilers.
 *
 *You may choose to leave CSBwin as it is in order to be as faithful as 
 *possible to the original Atari ST game, or to fix this bug.
 *As you have already fixed in CSBwin many bugs of the original game, I 
 *think this one should be fixed too.
 *
 *
 * And I agree that the intent of the designers was to use the full word.
*/
  //
  if (D7W < 16)
  {
    D0W = sw(P3 >> 3);
  }
  else
  {
    D0W = ScaledMultiply(P3, 7, D7W);//P3*D7/128
  };
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),
            "Tag016426(chIdx=%d,attrNum=%d,P3=%d) = %d\n",
            pChar-d.CH16482,attrNum,P3,D0W);
  };
  return D0W;
}

// *********************************************************
//
// *********************************************************
//   TAG016476
bool IsCharacterLucky(CHARDESC *pChar, i32 luckNeeded, const char *traceID)
{
  dReg D0;
  bool result;
  i32 currentLuck;
  ATTRIBUTE *attrA3;
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile),"%sEnter IsCharacterLucky (%s , (luckNeeded=%d))\n",
                      traceID, pChar->name, luckNeeded);
  };
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7W = sw(P2);
  if (   STRandomBool()
      && (STRandom(100) > luckNeeded)  )
  {
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%sReturning 1\n",traceID);
    };
    return true;
  };
  attrA3 = pChar->Attributes;
  currentLuck = attrA3[Luck].Current();
  if (currentLuck == 0) D0W = 0;
  else  D0W = (i16)STRandom(attrA3[Luck].Current());
  //SET(D0W, D0W > D7W);
  result = D0W > luckNeeded;
  //D7W = sw(D0W & 1);
  D0W = ApplyLimits(attrA3[Luck].Minimum(),
                     attrA3[Luck].Current() + (result ? -2 : +2),
                     attrA3[Luck].Maximum());
  attrA3[Luck].Current(D0W);
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile),"%sReturning result=%s\n",traceID,result?"true":"false");
  };
  return result;
}

// *********************************************************
//
// *********************************************************
//  TAG016610
i32 Quickness(CHARDESC *pChar, const char *traceID)
{ // Essentially:  0.5 * Dexterity*(1-load/Maxload/2)
  // With limits of about 5 and 95
  dReg D0, D1;
  i32 quickness;
  i32 loLimit, hiLimit, ranResult;
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile),"%sEntering function to determine character's Quickness\n",traceID);
  };
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  quickness = pChar->Attributes[Dexterity].Current()
              +(ranResult=STRandom(8));
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile),"%squickness = (Dexterity=%d) + (random(8)=%d) --> %d\n",
             traceID,pChar->Attributes[Dexterity].Current(),ranResult,quickness);
  };
  D0L = (quickness/2) * pChar->load;
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile),"%sAbout to compute Maximum load\n",traceID);
  };
  D1L = MaxLoad(pChar);
  quickness = quickness - (D0L / D1L);
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile),"%sLoading effect=(quickness/2)*(load=%d)/(maxload=%d)-->%d\n",
             traceID, pChar->load, D1L, D0L/D1L);
    fprintf(GETFILE(TraceFile),"%sSubtract loading effect from quickness -->%d\n",
                       traceID, quickness);
  };
  if (d.PartySleeping != 0)
  {
    quickness /= 2;
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%sSleeping.  Divide quickness by 2 --> %d\n",
                         traceID, quickness);
    };
  };
  quickness /= 2;
  loLimit = STRandom(8)+1;
  hiLimit = 100-STRandom(8);
  quickness = ApplyLimits(loLimit, quickness, hiLimit);
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile),"%sLow Limit = random(8) + 1 --> %d\n",traceID,loLimit);
    fprintf(GETFILE(TraceFile),"%sHigh Limit = 100-random(8) --> %d\n",traceID,hiLimit);
    fprintf(GETFILE(TraceFile),"%sFinal quickness = %d\n",traceID,quickness);
  };
  return quickness;
}

// *********************************************************
//
// *********************************************************
//  TAG0166b2
i32 DetermineThrowingDistance(i32 chIdx,i32 hand)
{
  dReg D0, D1, D3, D5, D6, D7;
  RN objD4;
  CHARDESC   *pcA3;
  WEAPONDESC *weaponDescA2;
  i32 result;
  i16 w_12;
  i16 w_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pcA3 = &d.CH16482[chIdx];
  D3W = pcA3->Attributes[Strength].Current();
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"%d ",D3W);
  };
  D7W = sw((STRandom()&15) + D3W);
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"%d ",D7W);
  };
  objD4 = pcA3->Possession(hand);
  D6L = GetObjectWeight(objD4);
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"%d ",D6W);
  };
  D5W = sw(MaxLoad(pcA3) / 16);
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"%d ",D5W);
  };
  if (D6W <= D5W)
  {
    D7W = sw(D7W + D6W - 12);
  }
  else
  {
    w_2 = sw((D5W - 12)/2 + D5W);
    w_12 = w_2;
    if (D6W <= w_12)
    {
      D7W = sw(D7W + (D6W - D5W)/2);
    }
    else
    {
      D7W = sw(D7W - 2 * (D6W-w_2));
    };
  };
  if (   (objD4 != RNnul)
       &&(objD4.dbType() == dbWEAPON))
  {
    weaponDescA2 = TAG0099d2(objD4);
    D7W = sw(D7W + weaponDescA2->uByte2);
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"%d ",D7W);
    };
    D6W = 0;
    D5W = weaponDescA2->uByte1;
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"%d ",D5W);
    };
    if ( (D5W == 0) || (D5W == 2) )
    {
      D6W = (i16)DetermineMastery(chIdx, 4);
      if (TimerTraceActive)
      {
        fprintf(GETFILE(TraceFile),"%d ",D6W);
      };
    };
    if ( (D5W != 0) && (D5W < 16) )
    {
      D6W = sw(D6W + DetermineMastery(chIdx, 10));
      if (TimerTraceActive)
      {
        fprintf(GETFILE(TraceFile),"%d ",D6W);
      };
    };
    if ( (D5W >= 16) && (D5W < 112) )
    {
      D6W = sw(D6W + DetermineMastery(chIdx, 11));
      if (TimerTraceActive)
      {
        fprintf(GETFILE(TraceFile),"%d ",D6W);
      };
    };
    D7W = sw(D7W + 2 * D6W);
  };
  D7W = StaminaAdjustedAttribute(pcA3, D7W);
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"%d ",D7W);
  };
  D0W = pcA3->ouches;
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"%d ",D0W);
  };
  if (hand == 0) D1W = 1;
  else D1W = 2;
//
  if (D1W & D0W)
  {
    D7W /= 2;
  };
  result = ApplyLimits(0, D7W/2, 100);
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"%d\n",result);
  };
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),
            "ThrowingDistance char=%d hand=%d value=%d\n",
            chIdx, hand, result);
  };
  return result;
}

// *********************************************************
//
// *********************************************************
i16 TAG01680a(i32 chIdx,i32 possessionIndex)
{
  dReg D0, D1, D5, D6, D7;
  CLOTHINGDESC *clA2;
  RN objD4;
  CHARDESC *pcA3;
  DB6      *DB6A2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pcA3 = &d.CH16482[chIdx];
  D5W = (UI16)(possessionIndex & 0x8000);
  possessionIndex &= 0x7fff;
  D6W = 0;
  for (D7W=0; D7W<2; D7W++)
  {
    objD4 = pcA3->Possession(D7W);
    if (  (objD4 == RNnul)
        ||(objD4.dbType() != dbCLOTHING)) continue;
    DB6A2 = GetRecordAddressDB6(objD4);
    //D0W = DB6A2->word2;
    //D0W = D0W & 0x7f;
    D0W = sw(DB6A2->clothingType());
    ASSERT(D0W <58,"D0 > 58");
    clA2 = &d.ClothingDesc[D0W];
    D0W = clA2->uByte2();
    if ((D0W & 0x80) == 0) continue;
    D0L = DetermineThrowingDistance(chIdx, D7W);
    D1L = TAG009470(clA2, D5W!=0);
    D0L += D1L;
    D1W = (UI8)(d.Byte1412[possessionIndex]);
    D0L = D0W * D1W;
    if (D7W == possessionIndex)
    {
      D1W = 4;
    }
    else
    {
      D1W = 5;
    };
    D6W = sw(D6W + (D0W >> D1W));
//
  };
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),
            "  Tag01680a D6W = %d\n",D6W);
  };
  D0L = STRandom();
  D1W = (UI8)(pcA3->Attributes[Vitality].Current());
  D1W = sw(D1W/8 + 1 );
  D0L &= 0xffff;
  D7W = sw(D0L % D1W); // zero to 8*vitality
  //D7W = D0W;
  if (D5W != 0)
  {
    D7W /= 2;
  };
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),
            "  Tag01680a D7W=%d,word64=%d,shieldStrength=%d,PartyShield=%d,D6W=%d\n",
            D7W, pcA3->word64,pcA3->shieldStrength,d.PartyShield,D6W);
  };
  D7W = sw(  D7W
           + pcA3->word64
           + pcA3->shieldStrength
           + d.PartyShield
           + D6W);
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),
            "  Tag01680a D7W = %d\n", D7W);
  };
  if (possessionIndex > 1)
  {
    objD4 = pcA3->Possession(possessionIndex);
    if (    (objD4 != RNnul)
         && (objD4.dbType() == dbCLOTHING))
    {
      DB6A2 = GetRecordAddressDB6(objD4);
      D7W = sw(D7W + TAG009470(&d.ClothingDesc[DB6A2->clothingType()], D5W!=0));
      if (TimerTraceActive)
      {
        fprintf(GETFILE(TraceFile),
                "  Tag01680a D7W + Tag009470() = %d\n", D7W);
      };
    };
  };
  if (pcA3->ouches & (1<<possessionIndex))
  {
    D7W = sw(D7W - (STRandom0_3() + 8));
  };
  if (d.PartySleeping)
  {
    D7W >>= 1;
  };
  D0W = ApplyLimits(0, D7W >>1, 100);
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),
            "Tag01680a(chIdx=%d,possession=%d) returns %d\n",
            chIdx, possessionIndex, D0W);
  };
  return D0W;
}

// *********************************************************
//
// *********************************************************
//   TAG016be8
void DropAllPossessions(i32 chIdx)
{//(void)
  dReg D4, D6;
  RN objD5;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D6W = d.CH16482[chIdx].charPosition;
  for (D4W=0; D4W<30; D4W++)
  {
    objD5 = RemoveCharacterPossession(chIdx, d.DropOrder[D4W]);
    if (objD5 == RNnul) continue;
    objD5.pos(D6W);//4Nov2002 PRS
    MoveObject(
               objD5,
               -1,  // oldX
               0,   // oldY
               d.partyX,
               d.partyY,
               NULL,
               NULL);
//
  };
}
// *********************************************************
//
// *********************************************************
//   TAG016c5a
void KillCharacter(i32 chIdx)
{//(void)
  dReg D5, D6;
  RN objD4, objD7;
  //DB10    *DB10A2;
  CHARDESC *pcA3;
  RN charDeathDSA = RNeof;
  LOCATIONREL charDeathFilterLocr;
  
  
// See if there is a character death filter
  {
    ui32 key;
    i32 len;
    ui32 *pRecord;
    key = (EDT_SpecialLocations<<24)|ESL_CHARDEATHFILTER;
    len = expool.Locate(key,&pRecord);
    if (len > 0)
    //Let the DSA process the parameters
    {
      RN obj;
      TIMER timer;
      i32 objectDB, actuatorType;
      DB3 * pActuator;
      NEWDSAPARAMETERS dsaParameters;
      charDeathFilterLocr.Integer(*pRecord);
      for (obj = FindFirstObject(
                      charDeathFilterLocr.l,
                      charDeathFilterLocr.x,
                      charDeathFilterLocr.y);
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
            charDeathDSA = obj;
            {
              TIMER timerA;
              int currentLevel;
              DSAVARS dsaVars;
              currentLevel = d.LoadedLevel;
              LoadLevel(charDeathFilterLocr.l);
              timerA.timerUByte9(0);//timerFunction
              timerA.timerUByte8(0);//timerPosition
              timerA.timerUByte7((ui8)charDeathFilterLocr.y);
              timerA.timerUByte6((ui8)charDeathFilterLocr.x);
              //timer.timerTime   = charDeathFilterLocr.l << 24;
              timerA.Time(0);
              timerA.Level((ui8)charDeathFilterLocr.l);
  
              pDSAparameters[0] = 1;
              pDSAparameters[1] = chIdx;
              ProcessDSAFilter(charDeathDSA, &timerA, charDeathFilterLocr, NULL, &dsaVars);
              LoadLevel(currentLevel);
            };
            break;
          };
        };
      };
    };
  };
 


  pcA3 = &d.CH16482[chIdx];
  pcA3->HP(0);
  pcA3->charFlags |= CHARFLAG_portraitChanged;
  if (d.SelectedCharacterOrdinal == chIdx+1)
  {
    if (d.PressingEye)
    {
      d.PressingEye = 0;
      d.PressingEyeOrMouth = 0;
      if (d.EmptyHanded == 0)// cursor hand
      {
        ASSERT(RememberToPutObjectInHand == -1,"Rememberobjecthand");
        DrawNameOfHeldObject(d.objectInHand);
      };
      d.Word23118 = 1;
      STShowCursor(HC47);
    }
    else
    {
      if (d.PressingMouth)
      {
        d.PressingMouth = 0;
        d.PressingEyeOrMouth = 0;
        d.Word23118 = 1;
        STShowCursor(HC5);
      };
    };
    ShowHideInventory(4); //Hide
  };
  DropAllPossessions(chIdx);
  objD4 = FindEmptyDB10Entry(true);
  if (objD4 != RNnul)
  {
    DB10 *pBones;
    pBones = GetRecordAddressDB10(objD4);
    pBones->miscType(misc_BonesOfHero);
    //DB10A2->word(2) = (DB10A2->word(2) & 0xff80) | 5;
    pBones->important(true);
    //DB10A2->word(2) = (DB10A2->word(2) & 0xff7f) | (1 << 7);
    pBones->value(2); // Indicate that these are the bones of a dead character
    //DB10A2->word(2) = (DB10A2->word(2) & 0x3fff) | ((chIdx & 3) << 14);
    D6W = (UI8)(pcA3->charPosition);
    objD4.pos(D6W);//4Nov2002 PRS
    MoveObject(
               objD4,
               -1, //oldX
               0,  //oldY
               d.partyX,
               d.partyY,
               NULL,
               NULL);
    expool.AddChampionBonesRecord(objD4, pcA3->fingerPrint);
  };
  pcA3->byte33 = 0;
  pcA3->incantation[0] = 0;
  pcA3->facing = (ui8)d.partyFacing;
  pcA3->maxRecentDamage = 0;
  D6W = sw(((D6W + 4) - d.partyFacing) & 3);
  if (D6W + 1 == d.Word23144)
  {
    d.NewCursorShape = 1;
    d.Word23244 = 0;
    d.Word23130 = 1;
  };
  if (pcA3->poisonCount)
  {
    CurePoison(chIdx);
  };
  d.UseByteCoordinates = 0;
  FillRectangle(d.LogicalScreenBase,      // The top view of positions
                &d.wRectPos1454[D6W],
                0,
                160);
  DrawCharacterState(chIdx);
  //D5W = 0;
  //pcA3 = &d.CH16482[0];
  for (D5W = 0, pcA3 = &d.CH16482[0];
      D5W<d.NumCharacter;
      D5W++, pcA3++)
  { // Find a living party member.
    if (   (d.PotentialCharacterOrdinal != D5W+1) 
        && (pcA3->HP() != 0) )break;
  };
  if (D5W == d.NumCharacter)
  {
    d.GameIsLost = 1;
  }
  else
  {
    if (chIdx == d.HandChar) // owner of cursor
    {
      SetHandOwner(D5W);
    };
    if (chIdx == d.MagicCaster)
    {
      SelectMagicCaster(D5W);
    }
    else
    {
      DrawSpellControls(d.MagicCaster);
    };
  };
}

// *********************************************************
//
// *********************************************************
//  TAG017068
i32 DamageCharacter(i32 chIdx,i32 damage,i16 mask,i16 P4)
{
  dReg D0, D1, D3, D4, D5, D6, D7;
  CHARDESC *pcA3;
  i16 LOCAL_30;
  i16 LOCAL_20;
  i16 LOCAL_10;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   D7W = sw(damage);
//
  if (chIdx==-1) return 0;
  if (chIdx+1 == d.PotentialCharacterOrdinal) return chIdx+1;
  if (d.Word11694 != 0) return d.Word11694;
  if (D7W <= 0) return 0;

  pcA3 = &d.CH16482[chIdx];
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"Damage character %d.  HP=%d, Basic damage = %d\n",
                                 chIdx, pcA3->HP(),damage);
  };
  if (pcA3->HP() == 0) return 0;
  if (P4 != 0)
  {
    for (D4W=D5W=D6W=0; D6W<=5; D6W++)
    {
      if ((mask & (1<<D6W)) == 0) continue;
      D4W++;
      D0W = D6W;
      if (P4 == 4)
      {
        D1UW = 0x8000;
      }
      else
      {
        D1W = 0;
      };
      D5W = sw(D5W + TAG01680a(chIdx, D0W | D1W));

//
    };
    if (D4W)
    {
      D5W = sw((D5W&0xffff)/(D4W&0xffff)); // Average
    };
    if (P4 == 6)
    {
      D0W = 115;
      D3W = (UI8)(pcA3->Attributes[Wisdom].Current());
      D6W = sw(D0W - D3W);
      if (D6W <= 0)
      {
        D7W = 0;
      }
      else
      {
        D7W = ScaledMultiply(D7W, 6, D6W);//D6*D7/64
      };
    }
    else
    {
      if (P4 == 5)
      {
        D7W = TAG016426(pcA3, 5, D7W);
        D7W = sw(D7W - d.SpellShield);
      }
      else
      {
        if (P4 == 1)
        {
          D7W = TAG016426(pcA3, 6, D7W);
          D7W = sw(D7W - d.FireShield);
        }
        else
        {
          if (P4 == 2)
          {
            D5W >>= 1;
          }
          else
          {
          };
        };
        if (D7W <= 0) return 0;

        D0W = sw(130 - D5W);
        D7W = ScaledMultiply(D7W, 6,D0W); //D6*D7/64
      };
    };
    if (D7W <= 0) return 0;
    D0W = sw(STRandom(128) + 10);
    D6W = TAG016426(pcA3, 4, D0W);
    LOCAL_10 = D6W;
    if (D7W > LOCAL_10)
    {
      do
      {
        LOCAL_20 = sw(STRandom(8));
        D1W = sw((1 << LOCAL_20) & mask);
        d.PendingOuches[chIdx] |= D1W;
        D6W <<= 1;
        LOCAL_30 = D6W;
        if (D7W <= LOCAL_30) break;
      } while (D6W != 0);
    };
    if (d.PartySleeping)
    {
      WakeUp();
    };
  };
  if (D7W <= 0)
  {
    D7W = 0;
  };
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),
            "Damage character %d incremented by %d = %d\n",
            chIdx, D7W, d.PendingDamage[chIdx]+D7W);
  };



  
  
// See if there is a character death filter
  {
    LOCATIONREL damageCharFilterLocr;
    ui32 key;
    i32 len;
    ui32 *pRecord;
    key = (EDT_SpecialLocations<<24)|ESL_DAMAGECHARFILTER;
    len = expool.Locate(key,&pRecord);
    if (len > 0)
    //Let the DSA process the parameters
    {
      RN obj;
      TIMER timer;
      i32 objectDB, actuatorType;
      DB3 * pActuator;
      NEWDSAPARAMETERS dsaParameters;
      RN damageCharDSA = RNeof;
      damageCharFilterLocr.Integer(*pRecord);
      for (obj = FindFirstObject(
                      damageCharFilterLocr.l,
                      damageCharFilterLocr.x,
                      damageCharFilterLocr.y);
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
            damageCharDSA = obj;
            {
              TIMER timerA;
              int currentLevel;
              DSAVARS dsaVars;
              currentLevel = d.LoadedLevel;
              LoadLevel(damageCharFilterLocr.l);
              timerA.timerUByte9(0);//timerFunction
              timerA.timerUByte8(0);//timerPosition
              timerA.timerUByte7((ui8)damageCharFilterLocr.y);
              timerA.timerUByte6((ui8)damageCharFilterLocr.x);
              //timer.timerTime   = damageCharFilterLocr.l << 24;
              timerA.Time(0);
              timerA.Level((ui8)damageCharFilterLocr.l);
  
              pDSAparameters[0] = 7;
              pDSAparameters[1] = chIdx;
              pDSAparameters[2] = d.CH16482[chIdx].fingerPrint;
              pDSAparameters[3] = damage;
              pDSAparameters[4] = D7W;
              pDSAparameters[5] = mask;
              pDSAparameters[6] = P4;
              pDSAparameters[7] = 0;
              ProcessDSAFilter(damageCharDSA, &timerA, damageCharFilterLocr, NULL, &dsaVars);
              LoadLevel(currentLevel);
              D7L = pDSAparameters[4];
            };
            break;
          };
        };
      };
    };
  };






  d.PendingDamage[chIdx] = sw(d.PendingDamage[chIdx] + D7W);
  return D7W;
}



void  CHARDESC::InitializePossessions(void)
{
  int i;
  for (i=0; i<30; i++)
  {
    ASSERT(possessions[i] == RNempty,"");
    possessions[i] = RNnul;
  };
}


void CHARDESC::SetPossession(int index, RN newObject, bool processFilter)
{
  LOCATIONREL equipFilterLocr;
  RN equipDSA;
// See if there is an equip filter
  if (processFilter)
  {
    {
      ui32 key;
      i32 len;
      ui32 *pRecord;
      key = (EDT_SpecialLocations<<24)|ESL_EQUIPFILTER;
      len = expool.Locate(key,&pRecord);
      if (len > 0)
      //Let the DSA process the parameters
      {
        RN obj;
        TIMER timer;
        i32 objectDB, actuatorType;
        DB3 * pActuator;
        NEWDSAPARAMETERS dsaParameters;
        equipFilterLocr.Integer(*pRecord);
        for (obj = FindFirstObject(
                        equipFilterLocr.l,
                        equipFilterLocr.x,
                        equipFilterLocr.y);
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
              equipDSA = obj;
              {
                TIMER timerA;
                int currentLevel;
                DSAVARS dsaVars;
                currentLevel = d.LoadedLevel;
                LoadLevel(equipFilterLocr.l);
                timerA.timerUByte9(0);//timerFunction
                timerA.timerUByte8(0);//timerPosition
                timerA.timerUByte7((ui8)equipFilterLocr.y);
                timerA.timerUByte6((ui8)equipFilterLocr.x);
                //timer.timerTime   = equipFilterLocr.l << 24;
                timerA.Time(0);
                timerA.Level((ui8)equipFilterLocr.l);
  
                pDSAparameters[0] = 4;
                pDSAparameters[1] = this-d.CH16482;
                pDSAparameters[2] = index;
                pDSAparameters[4] = 0;
  
                // Now we may have to call the DSA twice; once for the object 
                // being removed and once for the objct being added.
                if (possessions[index]!=RNnul)
                {
                  timerA.timerUByte9(1);//timerFunction
                  pDSAparameters[3] = possessions[index].ConvertToInteger();
                  ProcessDSAFilter(equipDSA, &timerA, equipFilterLocr, NULL, &dsaVars);
                };
                if (newObject != RNnul)
                {
                  timerA.timerUByte9(0);//timerFunction
                  pDSAparameters[3] = newObject.ConvertToInteger();
                  ProcessDSAFilter(equipDSA, &timerA, equipFilterLocr, NULL, &dsaVars);
                };
                LoadLevel(currentLevel);
              };
              break;
            };
          };
        };
      };
    };
  };
  // Then do the work of setting the possession.
  possessions[index] = newObject;
}




// *********************************************************
//
// *********************************************************
//   TAG017344
void CurePoison(i32 chIdx)
{
//  dReg D7;
  TIMER *pTimer;
  TIMER_SEARCH timerSearch;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (chIdx == -1) return;
  //pTimer = gameTimers.Timers(); xxTIMERxx change to FindNextTimer
  //for (D7W=0; D7W<d.MaxTimer(); D7W++, tA3++)
  while (timerSearch.FindNextTimer())
  {
    pTimer = timerSearch.TimerAddr();
    if (pTimer->Function() == TT_75)
    {
      if (pTimer->timerUByte5() == chIdx)
      {
        //DeleteTimer(D7W);
        timerSearch.DeleteTimer();
      };
    };
//
//
  };
  d.CH16482[chIdx].poisonCount = 0;
}

// *********************************************************
//
// *********************************************************
//   TAG017ce4
void ShowChestContents(RN objChest, DB9 *pChest, i16 P3)
{
  dReg D7;
  RN chestContents;
  DBCOMMON *pChestContentsRecord;
  i32 i;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.objOpenChest == objChest) return;
  ASSERT(d.objOpenChest == RNnul,"openChest");
  d.objOpenChest = objChest;
  if (P3 == 0)
  {
    DrawItem(9, objNI_OpenChest);
  };
  BLT2Viewport(GetBasicGraphicAddress(25),
               &d.wRectPos926,
               72,
               8);
  D7W = 0;
  for (chestContents=pChest->contents();
       chestContents!=RNeof;
       chestContents=GetDBRecordLink(chestContents)  )
  {
    DrawItem(D7W+38,
             chestContents.NameIndex());
    d.rnChestContents[D7W++] = chestContents;
//
//
  };
  while (D7W < 8)
  {
    DrawItem(D7W+38,objNI_NotAnObject);
    d.rnChestContents[D7W++] = RNnul;
//
  };
  // The links in the data records are still intact while the items are
  // on display in d.rnChestContents.  If we then remove one of the items
  // from the chest and put it in our non-weapon hand, the link still exists.
  // If a Giggler with no other possessions then steals this item the link
  // still exists and it appears that the Giggler has stolen the entire
  // list of items.  Many things can go wrong.  There will exist two copies
  // of the other items on the list.  The immediate problem that I fixed
  // here occurred when I put two copies of the same item on the same place
  // on the floor.  They then formed a loop and the program hung when trying
  // to display the items.  But other terrible things could obviously
  // go wrong.
  //
  // The same thing happened in the emulated Atari code.
  //
  // So I am going to clear the links here and now.
  //
  for (i=0; i<8; i++)
  {
    if (d.rnChestContents[i] != RNnul)
    {
      pChestContentsRecord = GetCommonAddress(d.rnChestContents[i]);
      pChestContentsRecord->link(RNeof);
    };
  };
  // Another thing that can go wrong here.  While the contents are
  // in the viewing area, the first item in the chest is still
  // linked to the chest's contents.  Then if the item gets deleted,
  // it will still appear to be in the chest until the chest gets
  // repacked.  So I am going to clear the chest's contents link.
  pChest->contents(RNeof);
}

// *********************************************************
//
// *********************************************************
//   TAG017da4
void RepackChest(void) //Put displayed items back into chest.
{ //We got here when we removed a chest from hand in stats screen.
  dReg D4, D5;
  RN   objD6, objD7;
  DBCOMMON *dbA2;
  DB9     *DB9A3;
  D4W = 1;
  if (d.objOpenChest == RNnul)
  {
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"%06x Skipping Repack Chest because d.objOpenChest==RNnul\n",
                     d.Time);
    };
    return;
  };
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"%06x Repacking Chest Contents\n",d.Time);
  };
  DB9A3 = GetRecordAddressDB9(d.objOpenChest);
  d.objOpenChest = RNnul;
  DB9A3->contents(RNeof);
  for (D5W=0; D5W<8; D5W++)
  {
    objD7 = d.rnChestContents[D5W];
    if (objD7 == RNnul) continue;
    if (D4W != 0)
    {
      D4W = 0;
      dbA2 = GetCommonAddress(objD7);
      dbA2->link(RNeof);
      objD6 = objD7;
      DB9A3->contents(objD7);
      if (TimerTraceActive)
      {
        fprintf(GETFILE(TraceFile),"Added %04x to %02x(%02x,%02x)%01x\n",
                  objD7.ConvertToInteger(), d.LoadedLevel, -1, 0, 0);
      };
      continue;
    };
    AddObjectToRoom(objD7, objD6, -1, 0, NULL);
    objD6 = objD7;
//
//
  };
}

// *********************************************************
//
// *********************************************************
//   TAG017e2a
void PrintItemDesc(const char* text, i32 color)
{ //Initialize if first character == 12
  dReg D0, D7;
  aReg A2, A3;
  i8  b_128[128];
  A2=NULL;
  //D4W = D5W = D6W = 0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D0W = text[0];
  if (*text == 12)
  {
    text++;
    d.TextOutX = 108;
    d.TextOutY = 59;
  };
  if (*text == 0) return;
  StrCpy((char *)b_128, text);
  A3 = b_128;
  for (D7W = 0; *A3 != 0; )
  {
    D0W = StrLen((char *)A3);
    if (D0W > 18)
    {
      A2 = A3 + 17;
      while ((A2 > A3+2) && (*A2 != ' ')) A2--;
      if (A2 == A3+2) A2 = A3+17;

      *A2 = 0;
      D7W = 1;
    };
    TextToViewport(d.TextOutX,
                   d.TextOutY,
                   color,
                   (char *)A3,
                   false);
    d.TextOutY += 7;
    if (D7W != 0)
    {
      D7W = 0;
      A3 = A2 + 1;
    }
    else
    {
      *A3 = 0;
    };
  };
}

// *********************************************************
//
// *********************************************************
//   TAG017ece
i32 EncodeDescriptivePhrases(
               i32 descriptionMask,  //Which phrases are possible
               i32 bitmask,
               const char **descriptiveTexts,
               i32 *colors,
               char *result,
               const char* leadingText,
               const char* trailingText)
{ // Returns color of last phrase printed.
  dReg D0, D4, D5;
  aReg A2;
  i16 w_2;
  i32 textColor = -1; // result
  //pnt *ppA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7W = P1;
  //D6W = bitmask;
  //ppA3 = descriptiveTexts;
  A2   = (aReg)result;
  w_2 = 0;
  for (D5W=0, D4W=1; D5W < 16; D5W++, D4W<<=1)
  {
    D0W = sw(D4W & descriptionMask & bitmask);
    if (D0W != 0) w_2++;

  };
  if (w_2 == 0)
  {
    *A2 = 0;
    return textColor;
  };
  StrCpy((char *)A2, leadingText);
  for (D5W=0, D4W=1; D5W<16; D5W++, D4W<<=1)
  {
    D0W = sw(D4W & descriptionMask & bitmask);
    if (D0W != 0)
    {
      StrCat((char *)A2, TranslateLanguage(descriptiveTexts[D5W]));
      textColor = colors[D5W];
      if (w_2-- > 2)
      {
        StrCat((char *)A2, ", ");
      }
      else
      {
        if (w_2 == 1)
        StrCat((char *)A2, " AND ");
      };
    };
//
  };
  StrCat((char *)A2, trailingText);
  return textColor;
}

// *********************************************************
//
// *********************************************************
void TAG018238(i16 P1)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  BLT2Viewport(GetBasicGraphicAddress(P1==0 ? 18 : 19),
                                 &d.wRectPos934,
                                 8,
                                 8);
}

// *********************************************************
//
// *********************************************************
//   TAG01826c
void DisplayScrollText_OneLine(i16 pixelY, char *text)
{
  aReg A2, A3;
  i32 length;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A3 = (aReg)text;
  for (A2=A3; *A2!=0; A2++)
  {
    if ( (*A2 >= 'A') && (*A2 <= 'Z') ) *A2 -= 64;

    else
      if (*A2 >= 0x7b) *A2 -= 0x60;
//
//
  };
  length = StrLen((char *)A3);
  if (length > MAX_SCROLL_LINE_LENGTH) 
                  length = MAX_SCROLL_LINE_LENGTH;
  TextOut_OneLine(d.pViewportBMP,
                  112,
                  162 - 6*length/2,
                  pixelY,
                  0,
                  15,
                  (char *)A3,
                  length,
                  false);
}

// *********************************************************
//
// *********************************************************
//   TAG0182f0
void DisplayScroll(DB7 *pDB7)
{
  dReg D1, D7;
  aReg A0, A2, A3;
  char b_260[1000];
  i32 i;
  i32 numLinesInScroll = 0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  // Word2 is textID
  DecodeText(b_260,
             GetRecordAddressDB2(pDB7->text()),
             (ui16)0x8002,
             990);
  //Make sure there are TWO zeroes at the end!
  //This fixes the problem when there is only
  //one line of text.
  for (i=0; b_260[i]!=0; i++);
  b_260[i+1] = 0;
  for (A2 = (aReg)b_260; (*A2 != 0) && (*A2 != 10); A2++) {};
//

  *A2 = 0;
  BLT2Viewport(GetBasicGraphicAddress(23),
               &d.wRectPos926,
               72,
               8);
  numLinesInScroll = 1;
  A2++;
  for (A3=A2; *A3!=0; A3++) if (*A3 == 10) numLinesInScroll++;
//
//
//

  A0 = A3 - 1;
  if (*A0 != 10)
  {
    numLinesInScroll++;
  }
  else
  {
    A0 = A3 - 2;
    if (*A0 == 10) numLinesInScroll--;
  };
  if (numLinesInScroll > MAX_LINES_IN_SCROLL)
        numLinesInScroll = MAX_LINES_IN_SCROLL;
  D1L = 7 * numLinesInScroll / 2;
  D7W = sw(92 - D1W);
  DisplayScrollText_OneLine(D7W, b_260);
  numLinesInScroll--;
  if (numLinesInScroll == 0) return;
  for (A3=A2; *A3!=0; A3=A2)
  {
    for (D7W+=7; (*A2!=0)&&(*A2!=10); A2++)
    {
//
    };
    if (*A2 == 0)
    {
      *(A2+1) = 0;
    };
    *(A2++) = 0;
    DisplayScrollText_OneLine(D7W, (char *)A3);
    numLinesInScroll--;
    if (numLinesInScroll == 0) return;
//
//
  };
}

class CUSTOMPHRASE
{
  char *m_address;
  public:
    CUSTOMPHRASE(void){m_address = NULL;};
    ~CUSTOMPHRASE(void){Clear();};
    void Clear(void){if(m_address!=NULL)UI_free(m_address);m_address=NULL;};
    void Set(char *a){Clear();m_address=(char *)UI_malloc(strlen(a)+1,MALLOC091);strcpy(m_address,a);};
    char *Get(void){return m_address;};
};

const char*  descriptivePhrases[10]; //pnt_72[6];
CUSTOMPHRASE customPhrases[10];
ui32         phraseMask;
i32          phraseColors[10];

void CleanupCustomPhrases(void)
{
  int i;
  for (i=0; i<10; i++) customPhrases[i].Clear();
}

// *********************************************************
//
// *********************************************************
//   TAG0183ea
void DescribeObject(RN object,i16 P2)
{ // Describe object???
  // We get here when we place an object on the 'eye'.
  dReg D0, D1, D4, D7;
  const char* A0;
  const char* A3;
  OBJ_NAME_INDEX objNID6;
  CHARDESC *pcA0;
  DB5 *DB5A0;
  DB6 *DB6A0;
  DB8 *DB8A0;
  DB10 *DB10A0;
  char descriptiveText[42];
  // moved to global so DSA filter can modify pnt pnt_72[6];
  //pnt pnt_68;
  //pnt pnt_64;
  //pnt pnt_60;
  //pnt pnt_56;
  //pnt pnt_52;
  DBCOMMON *dbA2;
  DBTYPE dbType;
  i32 descriptionMask;
  i32 len, i, textColor;
  ui32 key, *pRecord;
  NEWDSAPARAMETERS ndp;
  descriptionMask = 0x0ff0;
  D4W = 0x0ff0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  descriptivePhrases[1] = ")";
  descriptivePhrases[0] = "(";
  for (i=0; i<10; i++) phraseColors[i] = COLOR_13;
//
  if ( (d.PressingEye != 0) || (d.PressingMouth != 0) ) //If food/water or skill levels showing
  {
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"%06x DescribeObject calling RepackChest. d.PressingEye=%x. d.PressingMouth=%x\n",
                   d.Time, d.PressingEye, d.PressingMouth);
    };
    RepackChest();
  };
  dbA2 = GetCommonAddress(object);
  PrintItemDesc("\014"); //Initialize
  dbType = object.dbType();
  if (dbType == dbSCROLL)
  {


    
   {
      ui32 recKey;
      ui32 *pRec;
      i32 recLen;
      //  See if there is a Viewing Filter to notify of this action
      recKey = (EDT_SpecialLocations<<24)|ESL_VIEWINGFILTER;
      recLen = expool.Locate(recKey,&pRec);
      if (recLen > 0)
      //Let the DSA process the parameters
      {
        RN obj;
        TIMER timer;
        i32 objectDB, actuatorType;
        DB3 * pActuator;
        LOCATIONREL locr;
        locr.Integer(*pRec);
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
    
              pDSAparameters[1] = 0; //Phrase Mask
              pDSAparameters[2] = object.ConvertToInteger(); // Object ID
              pDSAparameters[3] = d.SelectedCharacterOrdinal-1;
              pDSAparameters[0] = 3;
              ProcessDSAFilter(obj, &timer, locr, NULL, &dsaVars);
              // *****************************
              // The DSA can modify the phraseMask and the contents of descriptivePhrases[]
              LoadLevel(currentLevel);
              if (obj.checkIndirectIndex(pDSAparameters[2]))
              {
                obj.ConstructFromInteger(pDSAparameters[2]);
                if (obj.dbType() == dbSCROLL)
                {
                  dbA2 = GetCommonAddress(obj);
                };
              };
              continue;
            };
          };
        };
      };    
    };    
    
    
    
    
    
    
    DisplayScroll(dbA2->CastToDB7());
  }
  else
  {
    if (dbType == dbCHEST)
    {
      ShowChestContents(object, dbA2->CastToDB9(), P2);
    }
    else
    {
      objNID6 = object.NameIndex();
      BLT2Viewport(GetBasicGraphicAddress(20),
                   &d.wRectPos926,
                   72,
                   8);
      BLT2Viewport(GetBasicGraphicAddress(29),
                   &d.wRectPos942,
                   16,
                   12);
      if (objNID6 == objNI_Bones_a)
      {

        
        
        
        i16 value;
        DB10 *pBones;
        pBones = GetCommonAddress(object)->CastToDB10();
        value = pBones->value();
        A3 = NULL;
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
              StrCpy(descriptiveText, pcA0->name);
              StrCat(descriptiveText, d.ObjectNames[objNID6]);
              A3 = descriptiveText;
            };
          };
        };
        if (A3 == NULL)
        {
          A3 = d.ObjectNames[objNID6];
        };

        
        
        
        
        
        
        
        
        
        
        //pcA0 = &d.CH16482[dbA2->CastToDB10()->value()];
        //StrCpy(descriptiveText, pcA0->name);
        //StrCat(descriptiveText, " ");
        //StrCat(descriptiveText, d.ObjectNames[objNID6]);
        //A3 = descriptiveText;
      }
      else
      {
//
//
        if (   (dbType == dbPOTION)
            && (objNID6 != objNI_WaterFlask)
            && (objNID6 != objNI_EmptyFlask)
            && (DetermineMastery(d.SelectedCharacterOrdinal-1, 2) > 1) )
        {
          descriptiveText[0] = sb(95 + dbA2->CastToDB8()->strength()/40);
          descriptiveText[1] = ' ';
          descriptiveText[2] = 0;
          StrCat(descriptiveText, d.ObjectNames[objNID6]);
          A3 = descriptiveText;
        }
        else
        {
          A3 = d.ObjectNames[objNID6];
        };
      };
      TextToViewport(134, 68, COLOR_13, A3, false);
      DrawSmallIcon(objNID6, 111, 59);
      descriptivePhrases[2] = "CONSUMABLE";
      descriptivePhrases[3] = "POISONED";
      descriptivePhrases[4] = "BROKEN";
      descriptivePhrases[5] = "CURSED";
      descriptivePhrases[6] = "";
      descriptivePhrases[7] = "";
      descriptivePhrases[8] = "";
      descriptivePhrases[9] = "";
      d.TextOutY = 87;
      switch (dbType)
      {
//
      case dbWEAPON:
          descriptionMask = 0x0e;//Cursed,broken,poisoned 
          DB5A0 = dbA2->CastToDB5();
          D4W = sw(DB5A0->cursed());
          D4W <<= 3; //Cursed
          D1W = sw(DB5A0->poisoned());
          D4W |= D1W << 1;//Poisoned
          D1W = sw(DB5A0->broken());
          D4W |= D1W << 2;//Broken
          if (   (objNID6 >= objNI_Torch_a)
              && (objNID6 <= objNI_Torch_d) )
          {
            D0W = sw(DB5A0->charges());
            if (D0W == 0)
            {
              PrintItemDesc("(BURNT OUT)");
            };
          };
          break;

      case dbCLOTHING:
          descriptionMask = 0x0c;//cursed, broken
          DB6A0 = dbA2->CastToDB6();
          D4W = sw(DB6A0->cursed());
          D4W <<= 3; //Cursed
          D1W = sw(DB6A0->broken());
          D4W |= D1W << 2;//broken
          break;
      case dbPOTION:
          descriptionMask = 0x01;//consumable
          DB8A0 = dbA2->CastToDB8();
          D1W = sw(DB8A0->potionType());
          D4W = sw(D1W + 2);//Compute object index
          D4W = d.ObjDesc[D4W].word4;
          break;
      case dbMISC: // Bones, for example.
          if (   (objNID6 >= objNI_Waterskin)
              && (objNID6 <= objNI_Water) )
          {
            descriptionMask = 0;
            DB10A0 = dbA2->CastToDB10();
            switch (DB10A0->value()) //BITS14_15(DB10A0->word2))
            {
            case 0:
              A3 = "(EMPTY)";
              break;
            case 1:
              A3 = "(ALMOST EMPTY)";
              break;
            case 2:
              A3 = "(ALMOST FULL)";
              break;
            case 3:
              A3 = "(FULL)";
              break;
            }; //switch ()
            PrintItemDesc(A3);
          }
          else
          {
            if (   (objNID6 >= objNI_Compass_N)
                && (objNID6 <= objNI_Compass_W) )
            {
              descriptionMask = 0;
              StrCpy(descriptiveText, TranslateLanguage("PARTY FACING"));
              StrCat(descriptiveText, TranslateLanguage(d.DirectionNames[objNID6-objNI_Compass_N]));
              PrintItemDesc(descriptiveText);
            }
            else
            {
              descriptionMask = 0x09; //Cursed, Consumable
              // compute object index
              D4W = sw(dbA2->CastToDB10()->miscType() + 127);
              D4W = d.ObjDesc[D4W].word4;
              if (dbA2->CastToDB10()->cursed()) D4W |= 8;
            };
          };
      }; //switch (DB type)
      
      phraseMask = descriptionMask & D4W & 0x0f;
      phraseMask |= 0x40; //Include weight by default
      StrCpy(descriptiveText, TranslateLanguage("WEIGHS "));
      D7W = sw(GetObjectWeight(object));
      A0 = TAG014af6((UI16)(D7W)/10, 0, 3);
      StrCat(descriptiveText, A0);
      StrCat(descriptiveText, ".");
      D7W = sw((D7W&0xffff)%10);
      A0 = TAG014af6(D7W, 0, 1);
      StrCat(descriptiveText, A0);
      StrCat(descriptiveText, " KG.");
      customPhrases[8].Set(descriptiveText);
      descriptivePhrases[8] = customPhrases[8].Get();
// ****************************************************************************
//    See if there is a Viewing Filter to modify the description.
      key = (EDT_SpecialLocations<<24)|ESL_VIEWINGFILTER;
      len = expool.Locate(key,&pRecord);
      if (len > 0)
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
    
              pDSAparameters[1] = phraseMask; //Phrases that are permitted by this object type
              pDSAparameters[2] = object.ConvertToInteger();
              pDSAparameters[3] = d.SelectedCharacterOrdinal-1;
              pDSAparameters[0] = 3;
              ProcessDSAFilter(obj, &timer, locr, NULL, &dsaVars);
              // *****************************
              // The DSA can modify the phraseMask and the contents of descriptivePhrases[]
              LoadLevel(currentLevel);
              continue;
            };
          };
        };
      };    
    
    
      // ****************************************************************************
      if (phraseMask != 0) //Any phrases allowed?
      {
        textColor = EncodeDescriptivePhrases(
                  phraseMask & 0x3f,     //Which phrases apply to this type
                  0xffff,                // 
                  &descriptivePhrases[2],//list of possible phrases
                  &phraseColors[2],      //Text color specified for each phrase
                  descriptiveText,       //Where result goes;
                  descriptivePhrases[0], //"("
                  descriptivePhrases[1]);//")"
        PrintItemDesc(descriptiveText, textColor);
      };
      if (phraseMask & 0x40) PrintItemDesc(descriptivePhrases[8], phraseColors[8]);
      if (phraseMask & 0x80) PrintItemDesc(descriptivePhrases[9], phraseColors[9]);
   };
  };
  TAG018238(P2);
}

void ModifyDescription(i32 locrInt, i32 index, i32 color)
{
  LOCATIONREL locr;
  RN obj;
  if ((index < 0) || (index >7)) return; //illegal
  phraseColors[index+2] = color;
  if (locrInt == -1)
  {
    // Enable Phrase using current text.
    phraseMask |= 1 << index;
    return;
  };
  if (locrInt == -2)
  {
    // Disable the phrase.
    phraseMask &= ~(1 << index);
    return;
  };
  // Enable with custom text.
  locr.Integer(locrInt);
  for (obj = FindFirstObject(locr); obj!=RNeof; obj= GetDBRecordLink(obj))
  {
    DB2 *pText;
    i32 textIndex;
    if (obj.dbType() != dbTEXT) continue;
    if (obj.pos()  != locr.p) continue;
    pText = GetRecordAddressDB2(obj);
    textIndex = pText->index();
    char result[1000];
    GetText(result, textIndex);
    customPhrases[index].Set(result);
    descriptivePhrases[index+2] = customPhrases[index].Get();
    phraseMask |= 1<<index;
  };
};

// *********************************************************
//
// *********************************************************
void TAG0189a8(void)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  d.DisplayResurrectChestOrScroll = 5;
  BLT2Viewport(GetBasicGraphicAddress(40), //Resurrect/Reincarnate/cancel
            &d.wRectPos926,
            72,
            6);
}

// *********************************************************
//
// *********************************************************
void TAG0189d4(void)
{
  dReg D0;
  RN objD7;
  DBCOMMON *dbA3;
  CHARDESC *pChar;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.PotentialCharacterOrdinal)
  {
    TAG0189a8();//Portrait/bars/Resurrect menu
  }
  else
  {
    //D7L = d.Word16614 * 800;
    pChar = &d.CH16482[d.SelectedCharacterOrdinal-1];
    objD7 = pChar->Possession(1); // item in Weapon hand
    dbA3 = GetCommonAddress(objD7);
    d.DisplayResurrectChestOrScroll = 0;
    D0W = -1;
    if (objD7 != RNnul) D0W = sw(objD7.dbType());
    if (D0W == dbCHEST)
    { // Chest.
      d.DisplayResurrectChestOrScroll = 4;
    }
    else
    {
      if (D0W == dbSCROLL)  //scroll
      {
        d.DisplayResurrectChestOrScroll = 2; // Displaying scroll in weapon hand
      }
      else objD7 = RNnul;
      //{
      //}
      //else
      //{
      //};


    };
    if (objD7 == RNnul)
    {
      DisplayFoodWater();
    }
    else
    {
      DescribeObject(objD7 ,0);
    };
  };
}

// *********************************************************
//
// *********************************************************
//   TAG018a58
void AdjustCurrentAttribute(CHARDESC *pChar, i32 attr, i32 adj)
{
  dReg D0, D3, D5, D6, D7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(attr);
  D6W = sw(adj);
  if (D6W == 0) return;
  if (D6W > 0)
  {
    D5W = pChar->Attributes[D7W].Current();//Wisdom, vitality, etc.
    if (D5W >120)
    {
      D6W /= 2;
      if (D5W > 150)
      {
        D6W /= 2;
      };
      D6W++;
    };
    D5W = sw(Smaller(170-D5W, D6W));
    // Don't let current go above 170.
  }
  else
  {
    D0W = pChar->Attributes[D7W].Minimum();
    D3W = pChar->Attributes[D7W].Current();
    D5W = sw(Larger(D0W-D3W, D6W));//Both are negative or zero
    // Don't let current fall below Minimum
    // D5W is maximum we can subtract.
  };
  pChar->Attributes[D7W].IncrCurrent(D5W);
}

struct FEEDPARAMETERS
{
  i32    /**/ performFeeding;
  ui32   /**/ characterIndex;  //readonly
  ui32   /**/ objectDB;
  ui32   /**/ objectType;
  ui32   /**/ objectID;
  i32    /**/ foodValue;
  i32    /**/ waterValue;  
  ui32   /**/ potionType;      //readonly
  ui32   /**/ potionStrength;  //readonly
  i32    /**/ strengthAdjust;
  i32    /**/ dexterityAdjust;
  i32    /**/ wisdomAdjust;
  i32    /**/ vitalityAdjust;
  bool32 /**/ antiVenin;
  i32    /**/ staminaAdjust;
  ui32   /**/ shieldStrengthAdjust;
  ui32   /**/ shieldDuration;
  i32    /**/ manaAdjust;
  i32    /**/ hitpointAdjust;
  ui32   /**/ healCount;
  bool32  emptyFlask;
  bool32  emptyHand;
  bool32  chew;
  i32     gulp;
  ui32    location;
  i32     antiMagicAdjust;
  i32     antiFireAdjust;
  void Clear(void);
};


void FEEDPARAMETERS::Clear(void)
{
  performFeeding       = false;
  characterIndex       = 0xffffffff;
  objectDB             = 0xffffffff;
  objectType           = 0xffffffff;
  objectID             = 0xffffffff;
  foodValue            = 0;
  waterValue           = 0;
  potionType           = 0xffffffff;
  potionStrength       = 0xffffffff;
  strengthAdjust       = 0;
  dexterityAdjust      = 0;
  wisdomAdjust         = 0;
  vitalityAdjust       = 0;
  antiVenin            = false;
  staminaAdjust        = 0;
  shieldStrengthAdjust = 0;
  shieldDuration       = 0;
  manaAdjust           = 0;
  hitpointAdjust       = 0;
  healCount            = 0;
  emptyFlask           = false;
  emptyHand            = false;
  chew                 = false;
  gulp                 = -1;
  location             = 0xffffffff;
  antiMagicAdjust      = 0;
  antiFireAdjust       = 0;
}

// *********************************************************
//
// *********************************************************
//   TAG018afc
RESTARTABLE _FeedCharacter(void)
{ //(void)
  //We got here when I fed a character some bones. (But
  // they turned out to be not consumable.)
  static dReg D0, D1, D4;
  static i32 chIdx;
  static OBJ_NAME_INDEX objNID6;
  static RN  objD7;
  static CHARDESC *pcA0, *pcA2;
  static DBCOMMON *dbA3;
  static i16  w_40;
  static i16  w_30;
  static i16  w_20;
  static RN   obj_w_20;
  static DBTYPE  dbType_18;
  static i32  i_18;
  static i16  w_16;
  static i16  w_14;
  static i16  w_12;
  static TIMER timer;
  static i32 i;
  static DBCOMMON *itemToDeleteFromDatabase;
  static FEEDPARAMETERS fp;
  static LOCATIONREL playerLocr;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
  END_RESTARTMAP
  fp.Clear();
  itemToDeleteFromDatabase = NULL;
  playerLocr.l = d.partyLevel;
  playerLocr.x = d.partyX;
  playerLocr.y = d.partyY;
  playerLocr.p = d.partyFacing;
  fp.location = playerLocr.Integer();
  objNID6 = objNI_NotAnObject;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.EmptyHanded)
  {
    if (d.DisplayResurrectChestOrScroll == 0) RETURN;
    d.PressingEyeOrMouth = 1;
    d.PressingMouth = 1;
    if ((d.MouseSwitches & 2) == 0)
    {
      d.PressingEyeOrMouth = 0;
      d.PressingMouth = 0;
      d.QuitPressingMouth = 0;
    }
    else
    {
      STHideCursor(HC5);
      d.Word23118 = 1;
      DisplayFoodWater();
      MarkViewportUpdated(0);
    };
    RETURN;
  };
  if (d.PotentialCharacterOrdinal != 0) RETURN;
  //Here is our plan: wish us luck.
  // Instead of making adjustments to the character's stats we
  // record the modifications in a FEEDPARAMETERS structure.
  // Then we pass that structure to the FeedFilter for possible
  // modification.  Finally we apply the modified values to
  // the character.
  ASSERT(RememberToPutObjectInHand == -1,"remember object hand");
  objD7 = d.objectInHand;
  {
    OBJ_DESC_INDEX objDI;
    objDI = objD7.DescIndex();
    ASSERT( (objDI >= 0) && (objDI < 180),"objDI>180");
    D0W = d.ObjDesc[objDI].word4;
  };
  if ((D0W & 1) == 0)
  {
    if (objD7.dbType() == dbCHEST) RETURN; // Not consumable
    if (objD7.dbType() == dbSCROLL) RETURN;
  }
  else // if consumable
  {
    fp.performFeeding = true;
  };
  dbType_18 = objD7.dbType();
  fp.objectID = objD7.ConvertToInteger();
  if (dbType_18 != dbSCROLL)
  {
    objNID6 = objD7.NameIndex();
    fp.objectType = objNID6;
  };
  fp.objectDB = dbType_18;
  w_20 = sw(GetObjectWeight(objD7));
  obj_w_20 = objD7;
  chIdx = sw(d.SelectedCharacterOrdinal-1);
  fp.characterIndex = chIdx;
  pcA2 = &d.CH16482[chIdx];
  dbA3 = GetCommonAddress(objD7);
  if ((objNID6 >= objNI_Waterskin) && (objNID6 <= objNI_Water))
  {
    // Is it empty
    if (dbA3->CastToDB10()->value() == 0)
    {
      fp.waterValue = 0;
      fp.emptyFlask = false;
    }
    else
    {
    // do this later!! pcA2->water = sw(Smaller(pcA2->water + 800, 2048));
      fp.waterValue = 800;
      fp.gulp = 8;
      fp.emptyFlask = true;
    //One gulp is now gone.
    //dbA3->CastToDB10()->value(dbA3->CastToDB10()->value()-1);
    };
    D4W = 0;
    fp.emptyHand = false;
    fp.performFeeding = true;
  }
  else
  {
    if (dbType_18 == dbPOTION) // DB type
    {
      D4W = 0;
      fp.emptyHand = false;
      fp.gulp = 8;
    }
    else
    {
      //dbA3->CastToDB10()->link(RNnul); // Remove the object from the database.
      //itemToDeleteFromDatabase = dbA3;
      //DeleteDBEntry(dbA3); //Remove object from database
      //D4W = 1;
      //fp.emptyHand = true;
    };
  };
  //STHideCursor();
  //if (D4W != 0)
  //{
  //  RemoveObjectFromHand();
  //};
  if (dbType_18 == dbPOTION)
  {
    w_12 = sw(dbA3->CastToDB8()->strength());
    fp.potionStrength = w_12;
    w_30 = sw(32 + (w_12 + 1) / 8);
    w_14 = sw(((511 - w_12) / w_30) / 2);
    w_12 = sw(w_12 / 25 + 8);
    fp.potionType = dbA3->CastToDB8()->potionType();
    switch (fp.potionType)
    {
    case potion_RosPotion:
        //AdjustCurrentAttribute(pcA2, 2, w_12);
        fp.dexterityAdjust = w_12;
        fp.performFeeding = true;
        break;

    case potion_KuPotion:
        //AdjustCurrentAttribute(pcA2,
        //                       1,
        //                       dbA3->CastToDB8()->strength()/35 + 5);
        fp.strengthAdjust = dbA3->CastToDB8()->strength()/35 + 5;
        fp.performFeeding = true;
        break;
    case potion_DanePotion:
        //AdjustCurrentAttribute(pcA2, 3, w_12);
        fp.wisdomAdjust = w_12;
        fp.performFeeding = true;
        break;
    case potion_NetaPotion:
        //AdjustCurrentAttribute(pcA2, 4, w_12);
        fp.vitalityAdjust = w_12;
        fp.performFeeding = true;
        break;
    case potion_AntiVenin:
        //CurePoison(chIdx);
        fp.antiVenin = true;
        fp.performFeeding = true;
        break;
    case potion_MonPotionB:
        //pcA2->Stamina(
        //       pcA2->Stamina() +
        //         Smaller(pcA2->MaxStamina() - pcA2->Stamina(),
        //           pcA2->MaxStamina()/w_14));
        fp.staminaAdjust = 
               Smaller(pcA2->MaxStamina() - pcA2->Stamina(),
                       pcA2->MaxStamina()/w_14);
        fp.performFeeding = true;
        break;
    case potion_YaPotion:
        w_12 = sw(w_12 + w_12/2);
        if (pcA2->shieldStrength > 50)
        {
          w_12 = sw(w_12/4);
        };
        fp.shieldStrengthAdjust = w_12;
        //{
        //  i32 oldShieldLevel, newShieldLevel;
        //  char ascii[10];
        //  oldShieldLevel = pcA2->shieldStrength/50;
        //  pcA2->shieldStrength = sw(pcA2->shieldStrength + w_12);
        //  newShieldLevel = pcA2->shieldStrength/50;
        //  if (oldShieldLevel != newShieldLevel)
        //  {
        //    PrintLinefeed();
        //    QuePrintLines(d.Byte1386[chIdx],pcA2->name);
        //    QuePrintLines(d.Byte1386[chIdx]," SHIELD INCREASED TO ");
        //    sprintf(ascii, "%d", pcA2->shieldStrength);
        //    QuePrintLines(d.Byte1386[chIdx], ascii);
        //  };
        //};
        //timer.Function(TT_72);
        //D0L = d.Time + w_12 * w_12;
        fp.shieldDuration = w_12 * w_12;
        //D0L |= d.partyLevel << 24;
        //timer.timerTime = D0L;
        //timer.timerUByte5 = ub(chIdx);
        //timer.timerUWord6() = w_12;
        //SetTimer(&timer);
        //pcA2->word48 |= 0x1000;
        fp.performFeeding = true;
        break;
    case potion_EEPotion:
        D0W = sw(pcA2->Mana() + w_12 + w_12 - 8);
        i_18 = Smaller(900, D0W);
        if (i_18 > pcA2->MaxMana())
        {
          w_40 = sw(Larger(pcA2->Mana(), pcA2->MaxMana()));
          i_18 -= (i_18 - w_40)/2;
        };
        //pcA2->Mana(i_18);
        fp.manaAdjust = i_18 - pcA2->Mana();
        fp.performFeeding = true;
        break;
    case potion_ViPotion: // Healing potion?
        i_18 = Larger(dbA3->CastToDB8()->strength()/42, 1);
        {
          i32 tempHP;
          tempHP = pcA2->HP() + pcA2->MaxHP()/w_14;
          if (tempHP > pcA2->MaxHP()) tempHP = pcA2->MaxHP();
          //pcA2->HP(tempHP);
          fp.hitpointAdjust = tempHP - pcA2->HP();
        };
        //w_16 = pcA2->ouches;
        //if (pcA2->ouches != 0)
        //{
        //  for (w_14=10; w_14!=0; w_14--)
        //  {
        //    for (w_12=0; w_12<i_18; w_12++)
        //    {
        //
        //      pcA2->ouches &= STRandom();
        //    };
        //    i_18 = 1;
        //    if (w_16 != pcA2->ouches) break; // Anything change?
        //  };
        //};
        fp.healCount = i_18;
        //pcA2->word48 |= 0x2200;
        fp.performFeeding = true;
        break;
    case potion_WaterFlask:
        //pcA2->water = sw(Smaller(2048, pcA2->water+1600));
        fp.waterValue = 1600;
        fp.performFeeding = true;
        break;
    }; // switch()
    //dbA3->CastToDB8()->potionType(potion_EmptyFlask);
    fp.emptyFlask = true;
  }
  else
  {
    if ( (objNID6 >= objNI_Apple) && (objNID6 <= objNI_DragonSteak) )
    { // Any kind of food.
      //pcA2->food =
      //    sw(Smaller(2048,
      //           pcA2->food+d.FoodValue[otD6-objNI_Apple]));
      fp.foodValue = d.FoodValue[objNID6-objNI_Apple];
      fp.emptyHand = true;
      fp.performFeeding = true;
      fp.chew = true;
      fp.gulp = 8;
    };
  };




/////////////////////////////////////////////////////
//Now let the Feeding Filter DSA do its thing
/////////////////////////////////////////////////////
  {
    ui32 key, *pRecord;
    i32 len;
    key = (EDT_SpecialLocations<<24)|ESL_FEEDINGFILTER;
    len = expool.Locate(key,&pRecord);
    if (len > 0)
    // *****************************************
    //Let the DSA process the parameters
    {
      RN obj;
      TIMER timerA;
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
            timerA.timerUByte9(0);//timerFunction
            timerA.timerUByte8(0);//timerPosition
            timerA.timerUByte7((ui8)locr.y);
            timerA.timerUByte6((ui8)locr.x);
            //timerA.timerTime   = locr.l << 24;
            timerA.Time(0);
            timerA.Level((ui8)locr.l);
  
            memcpy(pDSAparameters+1, &fp, sizeof(fp));
            pDSAparameters[0] = sizeof(fp)/sizeof(pDSAparameters[0]);
            ProcessDSAFilter(obj, &timerA, locr, NULL, &dsaVars);
            memcpy(&fp, pDSAparameters+1, sizeof(fp));
            LoadLevel(currentLevel);
            continue;
          };
        };
      };
    };    
  };



/////////////////////////////////////////////////////////////////
//Now we do the actual changes to the character's stats.
/////////////////////////////////////////////////////////////////
  if (fp.performFeeding)
  {
    STHideCursor(HC6);
    AdjustCurrentAttribute(pcA2, 1, fp.strengthAdjust);
    AdjustCurrentAttribute(pcA2, 2, fp.dexterityAdjust);
    AdjustCurrentAttribute(pcA2, 3, fp.wisdomAdjust);
    AdjustCurrentAttribute(pcA2, 4, fp.vitalityAdjust);
    AdjustCurrentAttribute(pcA2, 5, fp.antiMagicAdjust);
    AdjustCurrentAttribute(pcA2, 6, fp.antiFireAdjust);
    if (fp.antiVenin) CurePoison(chIdx);
    AdjustStamina(chIdx, -fp.staminaAdjust);
    if (fp.shieldStrengthAdjust != 0)
    {
      i32 oldShieldLevel, newShieldLevel;
      //char ascii[10];
      oldShieldLevel = pcA2->shieldStrength/50;
      if (pcA2->shieldStrength + fp.shieldStrengthAdjust < 0)
      {
        pcA2->shieldStrength = 0;
      }
      else
      {
        pcA2->shieldStrength 
           = sw(pcA2->shieldStrength + fp.shieldStrengthAdjust);
      };
      newShieldLevel = pcA2->shieldStrength/50;
      //if (oldShieldLevel != newShieldLevel)
      //{
      //  PrintLinefeed();
      //  QuePrintLines(d.Byte1386[chIdx],pcA2->name);
      //  QuePrintLines(d.Byte1386[chIdx]," SHIELD INCREASED TO ");
      //  sprintf(ascii, "%d", pcA2->shieldStrength);
      //  QuePrintLines(d.Byte1386[chIdx], ascii);
      //};
      timer.Function(TT_72);
      //D0L = d.Time + fp.shieldDuration;
      //D0L |= d.partyLevel << 24;
      //timer.timerTime = D0L;
      timer.Time(d.Time+fp.shieldDuration);
      timer.Level((ui8)d.partyLevel);
      timer.timerUByte5(ub(chIdx));
      timer.timerUWord6() = sw(fp.shieldStrengthAdjust);
      gameTimers.SetTimer(&timer);
      pcA2->charFlags |= CHARFLAG_portraitChanged;
    };
    if (d.PotentialCharacterOrdinal != chIdx+1)
    {
      pcA2->Mana(LIMIT(0, pcA2->Mana()+fp.manaAdjust, 900));
      if (pcA2->HP() + fp.hitpointAdjust <= 0)
      {
        KillCharacter(chIdx);
      }
      else
      {
        pcA2->HP(pcA2->HP() + fp.hitpointAdjust);
      
        i_18 = fp.healCount;
        if (i_18 > 0)
        {
          w_16 = pcA2->ouches;
          if (pcA2->ouches != 0)
          {
            for (w_14=10; w_14!=0; w_14--)
            {
              for (w_12=0; w_12<i_18; w_12++)
              {      
                pcA2->ouches &= STRandom();
              };
              i_18 = 1;
              if (w_16 != pcA2->ouches) break; // Anything change?
            };
          };
        };
      };
    };
    pcA2->charFlags |= CHARFLAG_ouches | CHARFLAG_possession;
    pcA2->water = sw(LIMIT(-1023,pcA2->water+fp.waterValue,2048));
    pcA2->food = sw(LIMIT(-1023,pcA2->food+fp.foodValue,2048));
/////////////////////////////////////////////////////////////////
//We are done adjusting.  Back to normal processing.
/////////////////////////////////////////////////////////////////

    if (pcA2->Stamina() > pcA2->MaxStamina())
    {
      pcA2->Stamina(pcA2->MaxStamina());
    };
    if (pcA2->HP() > pcA2->MaxHP()) pcA2->HP(pcA2->MaxHP());
    if (fp.chew)
    //if (D4W != 0)
    {
      for (i=0; i<2; i++)
      {
        TAG0013e4(objNI_Special_h, 56, 46);
        VBLDelay(_2_,8);  //Animation delay.  Chewing
        TAG0013e4(objNI_Special_g, 56, 46);
        VBLDelay(_1_,8);  //Animation delay.  Chewing
//
      };
    }
    else
    {
      if (fp.emptyFlask)
      {
        if (dbType_18 == dbPOTION)
        {
          dbA3->CastToDB8()->potionType(potion_EmptyFlask);
        };
        if ((objNID6 >= objNI_Waterskin) && (objNID6 <= objNI_Water))
        {
          dbA3->CastToDB10()->value(dbA3->CastToDB10()->value()-1);
        };
      };
      DrawEightHands();
      DEBUGLOAD(pcA0, w_20, -1, obj_w_20);
      D1L = GetObjectWeight(objD7) - w_20; // New weight - old
      pcA0 = &d.CH16482[d.HandChar];
      DEBUGLOAD(pcA0, GetObjectWeight(objD7), +1, objD7);
      pcA0->load = sw(pcA0->load + D1W);
      ASSERT(pcA0->load < MAXLOAD,"load > maxload");
      pcA0->charFlags |= CHARFLAG_possession;
    };
    if (fp.emptyHand)
    {
      CURSORFILTER_PACKET cfp;
      if (d.objectInHand != RNnul)
      {
        cfp.type = CURSORFILTER_Eat;
        cfp.object = d.objectInHand.ConvertToInteger();
        CursorFilter(&cfp);
      };
      RemoveObjectFromHand();
      itemToDeleteFromDatabase = dbA3;
    };
    if (fp.gulp >= 0)
    {
      QueueSound(fp.gulp, d.partyX, d.partyY, 0);
    };
    pcA2->charFlags |= CHARFLAG_statsChanged;
    if (d.DisplayResurrectChestOrScroll == 0)
    {
      pcA2->charFlags |= CHARFLAG_chestOrScroll;
    };
    DrawCharacterState(chIdx);
    STShowCursor(HC6);
    if (itemToDeleteFromDatabase != NULL) DeleteDBEntry(itemToDeleteFromDatabase);
  };
  RETURN;
}

// *********************************************************
//
// *********************************************************
void TAG019036(void)   // Click on eye with empty hand
{
  dReg D0, D4, D5, D6, D7;
  aReg A0;
  CHARDESC *pcA3;
  char LOCAL_24[20];
  i16  LOCAL_4;
  i16  LOCAL_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"%06x TAG019036 calling Repack Chest\n",d.Time);
  };
  RepackChest();









  {
    ui32 key;
    ui32 *pRecord;
    i32 len;
//  See if there is a Viewing Filter to notify of this action
    key = (EDT_SpecialLocations<<24)|ESL_VIEWINGFILTER;
    len = expool.Locate(key,&pRecord);
    if (len > 0)
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
  
            pDSAparameters[1] = 0; //Phrase Mask
            pDSAparameters[2] = (ui32)(-1); // Object ID
            pDSAparameters[3] = d.SelectedCharacterOrdinal-1;
            pDSAparameters[0] = 3;
            ProcessDSAFilter(obj, &timer, locr, NULL, &dsaVars);
            // *****************************
            // The DSA can modify the phraseMask and the contents of descriptivePhrases[]
            LoadLevel(currentLevel);
            continue;
          };
        };
      };
    };    
  };    








  D4W = sw(d.SelectedCharacterOrdinal-1);
  pcA3 = &d.CH16482[D4W];
  BLT2Viewport(GetBasicGraphicAddress(20), &d.wRectPos926, 72, 8);
  D6W = 58;
  for (D7W=0; D7W<=3; D7W++)
  { // fighter, ninja, priest, wizard
    D5W = (i16)DetermineMastery(D4W, D7W|0x8000);//Novice, master, etc.
    D5W = sw(Smaller(16, D5W));  //There are limits, you know.
    if (D5W == 1) continue;
    if (D5W == 0) die (0x8d49,"Mastery = 0");
    A0 = (aReg)d.Pointer16770[D5W];
    StrCpy(LOCAL_24, TranslateLanguage((char *)A0));// "Novice", "Master", etc
    StrCat(LOCAL_24, " ");
    StrCat(LOCAL_24, TranslateLanguage(d.Pointer16596[D7W]));//"Fighter", etc
    TextToViewport(108, D6W, COLOR_13, LOCAL_24, false);
    D6W += 7;
//
//
  };
  D6W = 86;
  for (D7W=1; D7W<=6; D7W++)
  {
    A0 = (aReg)d.Pointer16822[D7W];//"STRENGTH", "DEXTERITY", etc.
    TextToViewport(108, D6W, COLOR_13, (char *)A0, true);
    D5W = pcA3->Attributes[D7W].Current();
    D0W = pcA3->Attributes[D7W].Maximum();
    LOCAL_4 = D0W;
    if (D5W < LOCAL_4)
    {
      LOCAL_2 = COLOR_8;
    }
    else
    {
      if (D5W > LOCAL_4)
      {
        LOCAL_2 = COLOR_7;
      }
      else
      {
        LOCAL_2 = COLOR_13;
      };
    };
    TextToViewport(174, D6W, LOCAL_2, TAG014af6(D5W, 1, 3), false);//LOCAL_2 is text color.
    StrCpy(LOCAL_24,"/");
    StrCat(LOCAL_24, TAG014af6(LOCAL_4, 1, 3));
    TextToViewport(192,D6W, COLOR_13, LOCAL_24, false);
    D6W += 7;
//
  };
}

// *********************************************************
//
// *********************************************************
//   TAG0192a4
void CharacterPortraitToStatusBox(i32 chIdx)
{
  RectPos LOCAL_8;
  d.UseByteCoordinates = 0;
  LOCAL_8.w.y1 = 0;
  LOCAL_8.w.y2 = 28;
  LOCAL_8.w.x1 = sw(chIdx*69+7);
  LOCAL_8.w.x2 = sw(LOCAL_8.w.x1+31);
  BLT2Screen((ui8 *)d.CH16482[chIdx].portrait,
            &LOCAL_8,
            16,
            -1);
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG019fac(const i32 clickX, const i32 clickY)
{//(void)
  static dReg D7;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
    RESTART(4)
    RESTART(5)
    RESTART(6)
  END_RESTARTMAP

//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.AttackingCharacterOrdinal != 0)
  {
    D7W = SearchButtonList(d.Buttons17892, clickX, clickY, 2);
    if (D7W != 0)
    {
      if (D7W == 112)
      {
        FlashButton(_1_,0x11d, 0x13f, 0x4d, 0x0053);
        ExecuteAttack(_2_,-1);
      }
      else
      {
        if (D7W - 112 <= d.Word20248)
        {
          if (D7W == 113)
          {
            FlashButton(_3_,0xea, 0x13e, 0x56, 0x60);
          }
          else
          {
            if (D7W == 114)
            {
              FlashButton(_4_,0xea, 0x13e, 0x62, 0x6c);
              /*
              Code was jumped around.
              if (d.Word918 ?? 136)
              {
                D0W = TAG01385a(&d.Byte11678);
                if (D0W ?? 0)
                {
                  d.Word4012 = 3217;
                };
              }
              else
              {
              */
            }
            else
            {
              FlashButton(_5_,0xea, 0x13e, 0x6e, 0x78);
            };
          };
          ExecuteAttack(_6_,D7W-113);
          d.clockTick = (i16)intResult;
        };
      };
    };
  }
  else
  {
    if (d.Word20252)
    {
      D7W = SearchButtonList(d.Buttons17952,clickX,clickY,2);
      if (D7W != 0)
      {
        D7W -= 116;
        if (D7W < d.NumCharacter)
        {
          PrepareAttack(D7W); // Blue square appears around weapon hand
                              // and attack options appear.
        };
      };
    };
  };
  RETURN;
}

// *********************************************************
//
// *********************************************************
//  TAG01ac26
bool ItemHasChargesLeft(void)
{
	// Determines whether the item has enough charges (true if ready,
	// false if drained)
  CHARDESC *pcA0;
  RN      objD7;
  DBCOMMON *dbA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pcA0 = &d.CH16482[d.AttackingCharacterOrdinal-1];
  objD7 = pcA0->Possession(1);
  dbA3 = GetCommonAddress(objD7);
  switch (objD7.dbType())
  {
  case dbWEAPON:
      return dbA3->CastToDB5()->charges() != 0;
  case dbCLOTHING:
      return dbA3->CastToDB6()->charges() != 0;
  case dbMISC:
      return dbA3->CastToDB10()->value() != 0;

  default: return 1;
  };
}

// *********************************************************
//
// *********************************************************
//   TAG01acb0
void GetLegalAttackTypes(ui8 *P1)
{
	// This function always enable the first option and then
  // checks the rest of item`s options if party member is
  // good enough to use the option
  dReg D0, D4, D5, D6, D7;
  ui8 *uA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  uA3 = P1;
  d.PossibleAttackTypes[0] = *uA3;
  d.Byte20256[0] = 1;
  D6W = 1;
  for (D7W=1; D7W<3; D7W++)
  {
    D5W = *(uA3+D7W);
    if (D5W == 255) continue;
    D4W = *(uA3+D7W+3);
    if (D4W & 128)
    {
      if (!ItemHasChargesLeft()) continue;
    };
    D4W &= 0xff7f;
    D0W = (i16)DetermineMastery(d.AttackingCharacterOrdinal-1,
                                 (UI8)(d.SkillNumber[D5W]));

    if (D0W < D4W) continue;
    d.PossibleAttackTypes[D6W] = D5B;
    d.Byte20256[D6W] = D4B;
    D6W++;

//
  };
  d.Word20248 = D6W;
  for (D7W = D6W; D7W<3; D7W++)
  {
    d.PossibleAttackTypes[D7W] = -1;
//
  };
}

// *********************************************************
//
// *********************************************************
//   TAG01af32
void DrawAttackButton(i32 chIdx)
{
  dReg D0;
  RN objWeapon;
  OBJ_NAME_INDEX objNID7;
  aReg A3;
  CHARDESC *pcA2;
  RectPos LOCAL_16;
  RectPos LOCAL_8;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.Word20252 == 0) return;
  LOCAL_8.w.x1 = sw(22*chIdx + 233);
  LOCAL_8.w.x2 = sw(LOCAL_8.w.x1 + 19);
  LOCAL_8.w.y1 = 86;
  LOCAL_8.w.y2 = 120;
  d.UseByteCoordinates = 0;
  pcA2 = &d.CH16482[chIdx];
  if (pcA2->HP() == 0)
  {
    FillRectangle(d.LogicalScreenBase,
                  &LOCAL_8,
                  0,
                  160);
    return;
  };
  A3 = (aReg)allocateMemory(128, 0);
  objWeapon = pcA2->Possession(1); //Weapon hand
  if (objWeapon == RNnul)
  {
    objNID7 = objNI_Special_c;
  }
  else
  {
    D0L = objWeapon.DescIndex();
    D0L = d.ObjDesc[D0W].AttackClass();
    if (D0B)
    {
      objNID7 = objWeapon.NameIndex();
    }
    else
    {
#if TARGET_OS_MAC
      Flood(A3, 8, 16);
#else
      Flood((ui8 *)A3, 4, 16);
#endif
      goto tag01b018;
    };
  }
  GetIconGraphic(objNID7, (ui8 *)A3);
  ShrinkBLT((ui8 *)A3, (ui8 *)A3, 16, 16, 16, 16, d.Byte20194);
tag01b018:
  FillRectangle(d.LogicalScreenBase, &LOCAL_8, 4, 160);
  LOCAL_16.w.x1 = sw(LOCAL_8.w.x1 + 2);
  LOCAL_16.w.x2 = sw(LOCAL_8.w.x2 - 2);
  LOCAL_16.w.y1 = 95;
  LOCAL_16.w.y2 = 110;
  BLT2Screen((ui8 *)A3, &LOCAL_16, 8, -1);
  if (   (pcA2->charFlags & CHARFLAG_attackDisable)
      || (d.PotentialCharacterOrdinal)
      || (d.PartySleeping) )
  {
    ShadeRectangleInScreen(&LOCAL_8, 0); //Shade (disable) attack
  };
  FreeTempMemory(128);
}

// *********************************************************
//
// *********************************************************
void TAG01b1c0(void)
{//(void)
  CHARDESC *pChar;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  // Check somebody selecting attack option
  if (d.AttackingCharacterOrdinal == 0) return;
  pChar = &d.CH16482[d.AttackingCharacterOrdinal-1];
  pChar->charFlags |= CHARFLAG_weaponAttack;
  DrawCharacterState(d.AttackingCharacterOrdinal-1);
  d.AttackingCharacterOrdinal = 0;
  d.Word20250 = 1;
}

// *********************************************************
//
// *********************************************************
//   TAG01b1f8
void PrepareAttack(i16 chIdx)
{//(void)
	// This function look for the object in hand and pops up
  // object`s options (CHOP, SWING, ...).
  // It also draws the blue square around the right hand.
  //dReg D0;
  i32 attackClass;
  RN objWeapon;
  ui8 *pLegalAttacks;
  CHARDESC *pcA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pcA3 = &d.CH16482[chIdx];
  if (((pcA3->charFlags & CHARFLAG_attackDisable) != 0) || (pcA3->HP()==0)) return;
  objWeapon = pcA3->Possession(1);
  if (objWeapon == RNnul) // If no weapon wielded
  {
    attackClass = 2;
  }
  else
  {
    //D0W = GetObjectDescIndex(objWeapon);
    attackClass = (UI8)(d.ObjDesc[objWeapon.DescIndex()].AttackClass());
    if (attackClass == 0) return; // if no attack action
  };
//  A2 = &d.Byte19570[8*attackClass];
  pLegalAttacks = &d.legalAttacks[attackClass].byte0[0];

  d.AttackingCharacterOrdinal = sw(chIdx+1);
  //Not used anywhere!!  // d.Word20262 = *(pLegalAttacks+6);//This seems wrong but who cares?
  GetLegalAttackTypes(pLegalAttacks);
  d.Word20252 = 0;
  pcA3->charFlags |= CHARFLAG_weaponAttack;
  DrawCharacterState(chIdx); //box at top,pos,dir,weapon
  DrawLegalAttackTypes();
}

// *********************************************************
//
// *********************************************************
//  TAG01b37c
RESTARTABLE _ExecuteAttack(const i32 P1)
{ //(i16)
  //P1=attack number (or -1 for pass)
  static i32 successfulAttack;
  static CHARDESC *pChar;
  static ATTACKTYPE attackType;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  if (   (d.AttackingCharacterOrdinal==0)
      || ((UI8)(d.PossibleAttackTypes[P1])==255))
  {
    RETURN_i16(0);
  };
  pChar = &d.CH16482[d.AttackingCharacterOrdinal-1];
  if (P1 == -1)
  {
    successfulAttack = -1;
  }
  else
  {
    attackType = (ATTACKTYPE)(UI8)(d.PossibleAttackTypes[P1]);
    //pChar->word64 = sw(pChar->word64 + sb(d.Byte20090[attackType]));
    // See the definition of word64 for explanation
    pChar->word64 = d.Byte20090[attackType];
    pChar->charFlags |= CHARFLAG_statsChanged;
    Attack(_1_,d.AttackingCharacterOrdinal-1,attackType);
    successfulAttack = intResult;
    pChar->attackType = sb(attackType);
  };
  TAG01b1c0();
  RETURN_int(successfulAttack);
}

// *********************************************************
//
// *********************************************************
//   TAG01b50e
void DrawSpellControls(i16 chIdx)
{
  CHARDESC *pcA3;
  i16 LOCAL_8;
  i16 LOCAL_6;
  i16 LOCAL_4;
  i16 LOCAL_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pcA3 = &d.CH16482[chIdx];
  d.UseByteCoordinates = 0;
  LOCAL_2 = d.CH16482[0].HP();
  LOCAL_4 = d.CH16482[1].HP();
  LOCAL_6 = d.CH16482[2].HP();
  LOCAL_8 = d.CH16482[3].HP();
  STHideCursor(HC7);
  FillRectangle(d.LogicalScreenBase,
                (RectPos *)d.Word20242,
                0,
                160);
  if (chIdx == 0)
  {
    TraceFlash("vblFlashButn from 01b574");
    vblFlashButn(0xe9, 0x115, 0x2a, 0x31);
    TextOutToScreen(0xeb, 0x30, 0, 4, pcA3->name);
    if (d.NumCharacter > 1)
    {
      if (LOCAL_4 != 0)
      {
        TraceFlash("vblFlashButn from 01b5aa");
        vblFlashButn(0x118, 0x123, 0x2a, 0x30);
      };
tag01b5b0:
      if (d.NumCharacter > 2)
      {
        if (LOCAL_6 != 0)
        {
          TraceFlash("vblFlashButn from 01b5ca");
          vblFlashButn(0x126, 0x131, 0x2a, 0x30);
        };
tag01b5d0:
        if (d.NumCharacter > 3)
        {
          if (LOCAL_8 != 0)
          {
            TraceFlash("vblFlashButn from 01b5ea");
            vblFlashButn(0x134, 0x13f, 0x2a, 0x30);
          };
        };
      };
    };
    STShowCursor(HC7);
    return;
  };
  if (chIdx == 1)
  {
    if (LOCAL_2)
    {
      TraceFlash("vblFlashButn from 01b60e");
      vblFlashButn(0xe9, 0xf4, 0x2a, 0x30);
    };
    TraceFlash("vblFlashButn from 01b620");
    vblFlashButn(0xf7, 0x123, 0x2a, 0x31);
    TextOutToScreen(0xf9, 0x30, 0, 4, pcA3->name);
    goto tag01b5b0;
  };
  if (chIdx == 2)
  {
    if (LOCAL_2)
    {
      TraceFlash("vblFlashButn from 01b65a");
      vblFlashButn(0xe9, 0xf4, 0x2a, 0x30);
    };
    if (LOCAL_4)
    {
      TraceFlash("vblFlashButn from 01b672");
      vblFlashButn(0xf7, 0x102, 0x2a, 0x30);
    };
    TraceFlash("vblFlashButn from 01b684");
    vblFlashButn(0x105, 0x131, 0x2a, 0x31);
    TextOutToScreen(0x107, 0x30, 0, 4, pcA3->name);
    goto tag01b5d0;
  };
  if (chIdx != 3)
  {
    STShowCursor(HC7);
    return;
  };
  if (LOCAL_2)
  {
    TraceFlash("vblFlashButn from 01b6be");
    vblFlashButn(0xe9, 0xf4, 0x2a, 0x30);
  };
  if (LOCAL_4)
  {
    TraceFlash("vblFlashButn from 01b6d6");
    vblFlashButn(0xf7, 0x102, 0x2a, 0x30);
  };
  if (LOCAL_6)
  {
    TraceFlash("vblFlashButn from 01b6ee");
    vblFlashButn(0x105, 0x110, 0x2a, 0x30);
  };
  TraceFlash("vblFlashButn from 01b700");
  vblFlashButn(0x113, 0x13f, 0x2a, 0x31);
  TextOutToScreen(0x115, 0x30, 0, 4, pcA3->name);
  STShowCursor(HC7);
}

// *********************************************************
//
// *********************************************************

//   TAG01b880
void DrawMovementButtons(void)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  STHideCursor(HC8);//TAG002fd2
  BLT2Screen(GetBasicGraphicAddress(13), (RectPos *)d.Word28, 48, -1);//Movement buttons
  STShowCursor(HC8); //TAG003026
}

// *********************************************************
//
// *********************************************************
void TAG02076e(void)
{ //(void)
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.PartySleeping) return;
  STHideCursor(HC9);
  WaitForButtonFlash(); //If any are active//counted

  d.UseByteCoordinates = 0;
  if (d.SelectedCharacterOrdinal)
  {
    if (d.DisplayResurrectChestOrScroll == 4) // if displaying chest contents
    {
      if (TimerTraceActive)
      {
        fprintf(GETFILE(TraceFile),"%06x TAG02076e calling Repack Chest\n",d.Time);
      };
      RepackChest();
    };
  }
  else
  {
    ShadeRectangleInScreen((RectPos *)d.Word28, 0);
  };
  ShadeRectangleInScreen((RectPos *)d.Word12, 0);//Clear Magic area
  ShadeRectangleInScreen((RectPos *)d.Word20, 0);//Clear weapon ready area
  SetCursorShape(0);
  STShowCursor(HC9);
}

// *********************************************************
//
// *********************************************************
void TAG022a60(i16 P1,ui8 *P2)
{
  pnt A2, A3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A3 = (aReg)P2;
  A2 = d.ppUnExpandedGraphics[P1];
  ExpandGraphic(A2, (ui8 *)A3, 0, 0);
}

void CHARDESC::SaveToWings(void)
{
  // Our plan is to write the 800-byte character struct to
  // expool in the form of eight 25-word records each
  // with key = (EDT_Character<<24) + (recnum<<16) + fingerprint.
  //
  // Our strategy is to first read each of the records to ensure
  // that any old records are removed and then to write the
  // new records.
  //
  for (int i=0; i<8; i++)
  {
    ui32 *pData;
    ui32 key;
    ui32 junk[25];
    pData = (ui32 *)this + 25*i;
    key = (EDT_Character<<24) | (i<<16) | fingerPrint;
    expool.Read(key, junk, 25);
    expool.Write(key, pData, 25);
  };
}

bool CHARDESC::GetFromWings(ui16 fingerprint, bool testOnly)
{
  //Return true if successful.
  bool result;
  CHARDESC *pChar;
  ui8 character[sizeof(CHARDESC)]; // Avoid CHARDESC construction.
                                   // it will be overwritten completely.
  result = true;
  pChar = (CHARDESC *)character;
  pChar->fingerPrint = 0xffff;
  for (int i=0; i<8; i++)
  {
    ui32 *pData;
    ui32 *pRecord;
    ui32 key;
    int size;
    pData = (ui32 *)(&character) + 25*i;
    key = (EDT_Character<<24) | (i<<16) | fingerprint;
    if (testOnly)
    {
      size = expool.Locate(key, &pRecord);
      if (size == 25)
      {
        memcpy(pData, pRecord, 100);
      }
      else
      {
        result = false;
        break;
      };
    }
    else
    {
      size = expool.Read(key, pData, 25);
    };
    if (size != 25)
    {
      result = false;
      break;
    };
  };
  if (pChar->fingerPrint != fingerprint) result = false;
  if (result)
  {
    memcpy(this, &character, 800);
  };
  return result;
}

CHARDESC *CHARDESC::LocateInWings(ui32 fingerprint)
{
  //Return NULL if not found.
  ui32 *pRecord;
  ui32 key;
  int size;
  key = (EDT_Character<<24) | (fingerprint &0xffff);
  size = expool.Locate(key, &pRecord);
  if (size == 25)
  {
    return (CHARDESC *)pRecord;
  }
  else
  {
    return NULL;
  };
}


bool CHARDESC::CopyCharacter(ui32 fingerprint)
{
  i32 i;
  for (i=0; i<d.NumCharacter; i++)
  {
    if (d.CH16482[i].fingerPrint == (ui16)fingerprint)
    {
      memcpy(this, &d.CH16482[i], sizeof(CHARDESC));
      return true;
    };
  };
  return GetFromWings((ui16)fingerprint, true);
}


void RemoveCharacter(int chIdx)
{
  // Most of this copied from KillCharacter.
  // We have to:
  //   --Close any inventory screen
  //   --Remove the cursor from the character's hand
  //   --Cancel any poison timers.
  //   --Cancel any partial spells.
  //   --Make another character the magic caster.

  CHARDESC *pChar;
  int pos;
  pChar = &d.CH16482[chIdx];
  if (d.SelectedCharacterOrdinal == chIdx+1)
  {
    if (d.PressingEye)
    {
      d.PressingEye = 0;
      d.PressingEyeOrMouth = 0;
      if (d.EmptyHanded == 0)// cursor hand
      {
        ASSERT(RememberToPutObjectInHand == -1,"Rememberobjecthand");
        DrawNameOfHeldObject(d.objectInHand);
      };
      d.Word23118 = 1;
      STShowCursor(HC47);
    }
    else
    {
      if (d.PressingMouth)
      {
        d.PressingMouth = 0;
        d.PressingEyeOrMouth = 0;
        d.Word23118 = 1;
        STShowCursor(HC5);
      };
    };
    ShowHideInventory(4); //Hide the inventory screen
  };
  pChar->byte33 = 0;
  pChar->incantation[0] = 0;
  pChar->facing = (ui8)d.partyFacing;
  pChar->maxRecentDamage = 0;
  pos = (UI8)(pChar->charPosition);
  pos = sw(((pos + 4) - d.partyFacing) & 3);
  if (pos + 1 == d.Word23144)
  {
    d.NewCursorShape = 1;
    d.Word23244 = 0;
    d.Word23130 = 1;
  };
  if (pChar->poisonCount)
  {
    CurePoison(chIdx);
  };
  d.UseByteCoordinates = 0;
  FillRectangle(d.LogicalScreenBase,
                &d.wRectPos1454[pos],
                0,
                160);
  //D5W = 0;
  //pcA3 = &d.CH16482[0];
  {
    int livingChar;
    for (livingChar = 0; livingChar<d.NumCharacter; livingChar++)
    { // Find a living party member.
      if (   (chIdx != livingChar) 
          && (d.CH16482[livingChar].HP() != 0) )break;
    };

    if (chIdx == d.HandChar) // owner of cursor
    {
      SetHandOwner(livingChar);
    };
    if (chIdx == d.MagicCaster)
    {
      SelectMagicCaster((i16)(livingChar));
    }
    else
    {
      DrawSpellControls(d.MagicCaster);
    };
  };
}


void RedrawEverything(void)
{
  i32 i;
  for (i=0; i<d.NumCharacter; i++)
  {
    d.CH16482[i].charFlags |= CHARFLAG_positionChanged;
    d.CH16482[i].charFlags |= CHARFLAG_weaponAttack;
  };
  MarkAllPortraitsChanged();
}

ui32 CharacterDelete(int chIdx)
{
  i32 i;
  RectPos rectPos;

// No character will replace us so we must
// shift things left.
  if (d.CH16482[chIdx].timerIndex != -1)
  {
    // We will simply process the timer as if it had expired.
    gameTimers.DeleteTimer((HTIMER)d.CH16482[chIdx].timerIndex, "Delete");
    TAG011366((i16)chIdx);
  };
  RemoveCharacter(chIdx);
  d.CH16482[chIdx].SaveToWings();
  for (i=chIdx; i<d.NumCharacter-1; i++)
  {
    memcpy(&d.CH16482[i], &d.CH16482[i+1], sizeof(CHARDESC));
    // Adjust any timer entries pointing to this character.
    if (d.CH16482[i].timerIndex != -1)
    {
      die(0x452a,"Need Playfile to fix");
    };
  };
  if (d.HandChar > chIdx)
  {
    d.HandChar--;
  };
  if (d.SelectedCharacterOrdinal > chIdx)
  {
    d.SelectedCharacterOrdinal--;
  };
  if (d.MagicCaster > chIdx)
  {
    SelectMagicCaster((i16)(d.MagicCaster-1));
  };
  d.NumCharacter--;
  d.CH16482[d.NumCharacter].HP(0);
  rectPos.w.y1 = 0;
  rectPos.w.y2 = 28;
  rectPos.w.x1 = sw(69 * d.NumCharacter);
  rectPos.w.x2 = sw(rectPos.w.x1 + 66);
  d.UseByteCoordinates = 0;
  STHideCursor(HC26);
  FillRectangle(d.LogicalScreenBase,
                &rectPos,
                0,
                160);


  rectPos.w.x1 = sw(22*d.NumCharacter + 233);
  rectPos.w.x2 = sw(rectPos.w.x1 + 19);
  rectPos.w.y1 = 86;
  rectPos.w.y2 = 120;
  d.UseByteCoordinates = 0;
  FillRectangle(d.LogicalScreenBase,
                &rectPos,
                0,
                160);
  STShowCursor(HC26);
  RedrawEverything();
  DrawSpellControls(d.MagicCaster);
  return 0;
}


ui32 CharacterAdd(int fingerPrint)
{
  int i, pos;
  d.CH16482[d.NumCharacter].GetFromWings((ui16)(fingerPrint&0xffff), false);
  for (pos=0; pos<4; pos++)
  {
    for (i=0; i<d.NumCharacter; i++)
    {
      // Find a nice position for this new party member
      if (d.CH16482[i].charPosition == pos) break;
    };
    if (d.CH16482[i].charPosition == pos) continue;
    d.CH16482[d.NumCharacter].charPosition = (ui8)pos;
    break;
  };
  d.CH16482[d.NumCharacter].facing = 0;
  d.NumCharacter++;
  RedrawEverything();
  return 0;
}

ui32 CharacterSwap(int index, int fingerPrint)
{
  int i,pos;
  ui8 facing;
  if (d.CH16482[index].timerIndex != -1)
  {
    // We will simply process the timer as if it had expired.
    gameTimers.DeleteTimer((HTIMER)d.CH16482[index].timerIndex, "Delete");
    TAG011366((i16)index);
  };
  facing = d.CH16482[index].facing;
  RemoveCharacter(index);
  d.CH16482[index].SaveToWings();

  d.CH16482[index].GetFromWings((ui16)(fingerPrint&0xffff), false);
  d.CH16482[index].charPosition = 5; //An impossible
  for (pos=0; pos<4; pos++)
  { // See if we can find an empty position
    for (i=0; i<d.NumCharacter; i++)
    {
      // Find a nice position for this new party member
      if (d.CH16482[i].charPosition == pos) break;
    };
    if (d.CH16482[i].charPosition == pos) continue;
    d.CH16482[index].charPosition = (ui8)pos;
    break;
  };
  d.CH16482[index].facing = facing;
  RedrawEverything();

  return 0;
}


ui32 SwapCharacter(i32 index, i32 fingerPrint)
{
  ui8 character[sizeof(CHARDESC)];  // Avoid the construction of a CHARDESC
  CHARDESC *pChar;
  pChar = (CHARDESC *)character;
  // Is a character to be deleted?
  if ( (index >= 0) && (index < d.NumCharacter) )
  {
    // Make sure it is not the last character.
    if ((d.NumCharacter < 2) && (fingerPrint<0)) return 2;
  }
  else
  {
    index = -1;
  };
  // Is a character to be added?
  if (fingerPrint >= 0)
  {
    if ( (index == -1) && (d.NumCharacter == 4)) return 3;
    if (!pChar->GetFromWings((ui16)(fingerPrint&0xffff), true)) return 4;
  };
  // It appears that we can proceed.
  // First we delete the character at index
  // This seems complicated enough that we will process the three 
  // cases individually.
  if ((index >= 0) && (fingerPrint < 0)) 
  {
    return CharacterDelete(index);
  };
  if ((index < 0) && (fingerPrint >= 0))
  {
    return CharacterAdd(fingerPrint);
  };
  if ((index >= 0) && (fingerPrint >= 0))
  {
    return CharacterSwap(index, fingerPrint);
  };
  return 0;
}
/*
  newIndex = d.NumCharacter; // assume no delete
  if (index > 0)
  {
    d.CH16482[index].SaveToWings();
    newIndex = index;
    if (fingerPrint < 0)
    {
    };
  };
  if (fingerPrint>0)
  {
    d.CH16482[newIndex].SaveToWings();
    if (index >= 0) d.NumCharacter++;
  };
  return 1;
}
*/
