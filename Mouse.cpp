#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"


extern i32 VBLperTimer;

void info(char *, unsigned int);
void SwapTextZOrder();
void RecordFile_Record(i32 x, i32 y, i32 z);
bool IsPlayFileOpen();
bool PlayFile_IsEOF();
void PlayFile_ReadEOF();
bool PlayFile_Play(MouseQueueEnt *MQ);
void PlayFile_Backspace(MouseQueueEnt *MQ);
void RecordFile_Record(MouseQueueEnt *MQ);

// *********************************************************
//
// *********************************************************
//  TAG0091e6
i32 GetObjectWeight(RN item)
{ // Note...recursive for chests.
  RN objD7;
  i32 totalWeight = 0;
  i32 index;
  if (item == RNnul) return totalWeight;
  switch (item.dbType())
  {
  case dbWEAPON:
    {
      DB5 *pDB5 = GetRecordAddressDB5(item);
      index = pDB5->weaponType();
      totalWeight = d.weapons[index].weight;
      break;
    };
  case dbCLOTHING:
    {
      DB6 *pDB6 = GetRecordAddressDB6(item);
      index = pDB6->clothingType();
      totalWeight = d.ClothingDesc[index].Weight();
      break;
    };
  case dbMISC:
    {
      DB10 *pDB10 = GetRecordAddressDB10(item);
      index = pDB10->miscType();//word2 & 0x7f;
      totalWeight = ub(d.Byte8946[index]);
      if (pDB10->miscType() == 1) //word2&0x7f) == 1)
      {
        totalWeight += 2 * pDB10->value(); //BITS14_15(pDB10->word2);
      };
      break;
    };
  case dbCHEST: 
    {
      int size;
      ui32 *pData;
      int chestWeight = -1;
      size = expool.Locate((EDT_Database<<24) + (EDBT_ObjectWeights<<16)+0,&pData);
      if (size < 1) chestWeight = 50;
      else chestWeight = pData[0];
      totalWeight = chestWeight; // weight of chest itself
      DB9 *pDB9 = GetRecordAddressDB9(item);
      RN objContents = pDB9->contents();
      while (objContents != RNeof)
      {
        totalWeight += GetObjectWeight(objContents);
        objContents = GetDBRecordLink(objContents);

      };
      break;
    };
  case dbPOTION:
    {
      DB8 *pDB8 = GetRecordAddressDB8(item);
      if (pDB8->potionType() == potion_EmptyFlask)
      {
        totalWeight = 1;
      }
      else
      {
        totalWeight = 3;
      };
      break;
    };
  case dbSCROLL:
      totalWeight = 1;
      break;
  };
  return totalWeight;
}

// *********************************************************
//
// *********************************************************
// TAG00a16e
RN ConstructPredefinedMissile(ui16 missileType)
{
  WEAPONTYPE wpnD6 = weapon_Rock; //To keep the compiler quiet.
  MISCTYPE miscD6 = misc_GorCoin; //To keep the compiler quiet
  RN RND4;
  DBTYPE dbTypeD5;
  DB5 *wpnA3;
  DB10 *miscA3;
  //D7W = P1;
  //dbTypeD5 = dbWEAPON;
  if ( (missileType >= 4) && (missileType <= 7)) missileType = 4; 
  switch (missileType)
  {
  case 54:
    wpnD6 = weapon_Rock;
    dbTypeD5 = dbWEAPON;
    break;
  case 128:
    miscD6 = misc_Boulder;
    dbTypeD5 = dbMISC;
    break;
  case 51:
    wpnD6 = weapon_Arrow;
    dbTypeD5 = dbWEAPON;
    break;
  case 52:
    wpnD6 = weapon_Slayer;
    dbTypeD5 = dbWEAPON;
    break;
  case 55:
    wpnD6 = weapon_PoisonDart;
    dbTypeD5 = dbWEAPON;
    break;
  case 56:
    wpnD6 = weapon_ThrowingStar;
    dbTypeD5 = dbWEAPON;
    break;
  case 32:
    wpnD6 = weapon_Dagger;
    dbTypeD5 = dbWEAPON;
    break;
  case 4:
    wpnD6 = weapon_Torch;
    dbTypeD5 = dbWEAPON;
    break;
  default:
    return RNnul;
  }; //switch (P1);
  RND4 = db.FindEmptyDBEntry(dbTypeD5, false);
  if (RND4 == RNnul) return RNnul;
  switch (dbTypeD5)
  {
    //The Atari code took advantage of the fact that the WEAPIN and MISC
    //records happen to look similar.  I will split things up.
  case dbWEAPON:
    wpnA3 = GetRecordAddressDB5(RND4);
    wpnA3->weaponType(wpnD6);
    if (missileType == 4)  // A torch
    {
      if (wpnA3->litTorch() != 0)
      {
        wpnA3->charges(15);
      };
    };
    break;
  case dbMISC:
    miscA3 = GetRecordAddressDB10(RND4);
    miscA3->miscType(miscD6);
    break;
  default:
    SystemError((UI16)0xa222);
  }; //switch (dbTypeD5)
  return RND4;
}

// *********************************************************
//
// *********************************************************
//   TAG012a74
void CreateTimer(TIMERTYPE function,
                 i32 x,
                 i32 y,
                 i32 pos,
                 i32 action,
                 i32 time)
{
  TIMER timer;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //timer.timerTime = (d.LoadedLevel << 24) | time;
  timer.Time(time);
  timer.Level((ui8)d.LoadedLevel);
  timer.Function(function);
  timer.timerUByte5(0);
  timer.timerUByte6((ui8)x);
  timer.timerUByte7((ui8)y);
  timer.timerUByte8((ui8)pos);
  timer.timerUByte9((ui8)action);
  gameTimers.SetTimer(&timer);
}

// *********************************************************
//
// *********************************************************
//   TAG012ac0
void SkillIncreaser(i32 skillNumber, i32 adjustment, i32 P3)
{//()
  dReg D7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (P3 != 0)
  { // apply adjustment to party member touching actuator
    if (d.HandChar != -1)
    {
      AdjustSkills(d.HandChar, skillNumber, adjustment, ASW_SkillIncreaser1);
    };
    return;
  };
  // (P3 == 0) //divide adjustment among entire party
  adjustment = sw(adjustment / d.NumCharacter) ;
  for (D7W=0; D7W<d.NumCharacter; D7W++)
  {
    if (d.CH16482[D7W].HP() != 0)
    {
      AdjustSkills(D7W, skillNumber, adjustment, ASW_SkillIncreaser2);
    };
//
//
  };
}

// *********************************************************
//
// *********************************************************
//   TAG012b3c
void PerformLocalAction(i16 localActionType, 
                        i32 mapX, 
                        i32 mapY, 
                        i32 mapPos)
{ //()
  //Actuator with bit 11 set in word4
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (localActionType == 10)
  {
    SkillIncreaser(8, 300, mapPos == -1 ? 0 : 1);
  }
  else
  {
    d.DelayedActuatorAction = localActionType;
    d.DelayedActuatorActionX = sw(mapX);
    d.DelayedActuatorActionY = sw(mapY);
    d.DelayedActuatorActionPos = sw(mapPos);
  };
}

// *********************************************************
//
// *********************************************************
// TAG012d80
RN FindObjectOfTypeAtPosition(i32 mapX,
                              i32 mapY,
                              i16 pos,
                              OBJ_NAME_INDEX objectNameIndex)
{
  OBJ_NAME_INDEX objNID0, objNID6;
  RN objD5;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  
  objNID6 = objectNameIndex;
  objD5 = FindFirstDrawableObject(mapX, mapY);
  while (objD5 != RNeof)
  {
    objNID0 = GetBasicObjectType(objD5);
    if (objNID0 == objNID6)
    {
      // ROQUEN: unless I'm missing something: pos cannot be -1
      ASSERT(pos < 4,"Pos");
      if ((pos==-1) || (objD5.pos() == pos)) return objD5;
    };
    objD5 = GetDBRecordLink(objD5);
  };
  return RN(RNnul);
}

// *********************************************************
//
// *********************************************************
//  TAG012eac
i16 TouchWall(i32 mapX,i32 mapY,i32 touchedPosition)
{//(i16)
  dReg D0, D6;
  RN objD0, objD5;
  OBJ_NAME_INDEX objNID0;
  DBCOMMON *dbA3;
  DB3 *DB3A2;
  RN currentListObject;
  OBJ_NAME_INDEX objNI_38;
  DBCOMMON *pdb_28;
  RN  firstListObject;
  RN  previousListObject;
  RN  obj_20;
  RN  objectInHand;
  i16 remainingActuators[4];
  i16 result;
  i16 actuatorValue;
  i16 actuatorType;
  i16 actuatorPos;
  ACTUATORACTION actuatorAction;
  bool characterAdded;
  bool activePortrait;
  bool switchOpen;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D4L = 0xbaddadee;
  result = 0;
  ASSERT(RememberToPutObjectInHand == -1,"objInHand");
  objectInHand = d.objectInHand;
  for (actuatorPos=0; actuatorPos<4; actuatorPos++)
  {
    remainingActuators[actuatorPos] = 0;

  };
  currentListObject = FindFirstObject(mapX, mapY);
  firstListObject = currentListObject;
  while (currentListObject != RNeof)
  {
    D6W = sw(currentListObject.dbType());
    if (D6W == dbACTUATOR)
    {
      D0W = currentListObject.pos();
      remainingActuators[D0W]++;
    }
    else
    {
      if (D6W >= dbMONSTER)
      {
        break;
      };
    };
    currentListObject = GetDBRecordLink(currentListObject);

  };
  characterAdded = false; //No Champion Portraits yet encounterd.
  for ( currentListObject=previousListObject=firstListObject;
        (currentListObject != RNeof);
        previousListObject=currentListObject, 
             currentListObject=GetDBRecordLink(currentListObject))
  {
    D6W = sw(currentListObject.dbType());
    if (D6W == dbACTUATOR) // Actuator
    {
      actuatorPos = currentListObject.pos();
      remainingActuators[actuatorPos]--;
      DB3A2 = GetRecordAddressDB3(currentListObject);
      actuatorType = DB3A2->actuatorType();
      if (actuatorType == 0) continue;
      if (d.HandChar == -1)//owner of cursor
      {
        if (actuatorType != 127) continue;
      };
      if (actuatorPos != touchedPosition) continue;
      actuatorValue = DB3A2->value();
      actuatorAction = DB3A2->action();
      switch (actuatorType)
      {
      case 1:
          //D4W = 0;
          switchOpen = false;
          if (DB3A2->action() == actuatorAction_CONSTPRESSURE)
                                     continue;
          break;
      case 2:
          //D4W = d.EmptyHanded;
          switchOpen = d.EmptyHanded != 0;
          //D1W = DB3A2->normallyClosed();
          switchOpen = (DB3A2->normallyClosed() != switchOpen);
          break;
      case 17:
      case 11:
          D0W = remainingActuators[actuatorPos];
          if (D0W != 0) continue;
          // Note fall-through
      case 3:
      case 4:
        //Fallthrough for types 11 and 17
          objNI_38 = GetBasicObjectType(objectInHand);
          //D0W = (actuatorValue==ot_38);
          //D1W = DB3A2->normallyClosed();
          switchOpen = (DB3A2->normallyClosed() == (actuatorValue==objNI_38));
            
          if ( (!switchOpen) && (actuatorType==17) )
          {
            if (previousListObject == currentListObject) break;
            pdb_28 = GetCommonAddress(previousListObject);
            pdb_28->link(DB3A2->link());
            //DB3A2->link(RNnul);
            DeleteDBEntry(DB3A2);
            currentListObject = previousListObject;
          };
          if ( (!switchOpen) && (actuatorType==11) )
          {
            PerformLocalAction(2, mapX, mapY, actuatorPos);
          };
          break;
      case 12:
          D0W = remainingActuators[actuatorPos];
          if (D0W != 0) continue;
          switchOpen = (d.EmptyHanded==0);
          if (!switchOpen)
          {
            PerformLocalAction(2, mapX, mapY, actuatorPos);
          };
          break;    
      case 13: // A torch holder was type 13.
          if (d.EmptyHanded)
          {
            objectInHand = FindObjectOfTypeAtPosition(
                               mapX, mapY, actuatorPos, 
                               OBJ_NAME_INDEX(actuatorValue));
            if (objectInHand == RN(RNnul)) continue;
            CURSORFILTER_PACKET cfp;
            cfp.type = CURSORFILTER_TakeFromTorchHolder;
            cfp.object = objectInHand.ConvertToInteger();
            cfp.p1 = d.LoadedLevel;
            cfp.p2 = mapX;
            cfp.p3 = mapY;
            cfp.p4 = actuatorPos;
            CursorFilter(&cfp);
            if (cfp.type == CURSORFILTER_Cancel) continue;
            ObjectToCursor(objectInHand, 1);
            RemoveObjectFromRoom(objectInHand, mapX, mapY, NULL);
          }
          else
          {
            objNID0 = GetBasicObjectType(objectInHand);
            if (objNID0 != actuatorValue) continue;
            objD0 = FindObjectOfTypeAtPosition(
                     mapX, mapY, actuatorPos, 
                     OBJ_NAME_INDEX(actuatorValue));
            if (objD0 != RNnul) continue;
            {
              CURSORFILTER_PACKET cfp;
              cfp.type = CURSORFILTER_PlaceInTorchHolder;
              cfp.object = objectInHand.ConvertToInteger();
              cfp.p1 = d.LoadedLevel;
              cfp.p2 = mapX;
              cfp.p3 = mapY;
              cfp.p4 = actuatorPos;
              CursorFilter(&cfp);
              if (cfp.type == CURSORFILTER_Cancel) continue;
              RemoveObjectFromHand();
            };
            objectInHand.pos(actuatorPos);
            AddObjectToRoom(
                 objectInHand,
                 RN(RNempty),
                 mapX,
                 mapY,
                 NULL);
            objectInHand = RN(RNnul);
          };
          PerformLocalAction(2, mapX, mapY, actuatorPos);
          //D4W = 1;
          switchOpen = true;
          if (  (actuatorAction!=actuatorAction_CONSTPRESSURE)
              ||(d.EmptyHanded!=0))
          {
            //D4W = 0;
            switchOpen = false;
          };
          break;   
      case 16:
          D0W = remainingActuators[actuatorPos];
          if (D0W != 0) continue;
          obj_20 = FindFirstDrawableObject(mapX, mapY);
          objNID0 = GetBasicObjectType(objectInHand);
          if ((objNID0!=actuatorValue) || (obj_20 == RNnul)) continue;
          {
            CURSORFILTER_PACKET cfp;
            cfp.type = CURSORFILTER_SwapRemove;
            cfp.object = objectInHand.ConvertToInteger();
            cfp.p1 = d.LoadedLevel;
            cfp.p2 = mapX;
            cfp.p3 = mapY;
            cfp.p4 = actuatorPos;
            CursorFilter(&cfp);
            if (cfp.type == CURSORFILTER_Cancel) continue;
            RemoveObjectFromHand();
          };
          RemoveObjectFromRoom(obj_20, mapX, mapY, NULL);
          objectInHand.pos(actuatorPos); //04Nov2002 PRS
          AddObjectToRoom(
                 objectInHand, 
                 RN(RNempty), 
                 mapX, 
                 mapY,
                 NULL);
          {
            CURSORFILTER_PACKET cfp;
            cfp.type = CURSORFILTER_SwapReplace;
            cfp.object = obj_20.ConvertToInteger();
            cfp.p1 = d.LoadedLevel;
            cfp.p2 = mapX;
            cfp.p3 = mapY;
            cfp.p4 = actuatorPos;
            CursorFilter(&cfp);  // We know obj_20 is not NULL.
            ObjectToCursor(obj_20, 1);
          };
          //D4W = 0;
          switchOpen = false;
          break;
      
      case 127:
          if (characterAdded) continue; //Only 1 please!
          activePortrait = 
                (DB3A2->State() & PORTRAIT_InActive) == 0;
          if (!activePortrait) continue;
          AddCharacter(actuatorValue);
          characterAdded = true;
          continue;

      default: continue;
      }; // switch (actuatorType)
      if (actuatorAction == actuatorAction_CONSTPRESSURE)
      {
        if (switchOpen)
        {
          actuatorAction = actuatorAction_CLEAR;
        }
        else
        {
          actuatorAction = actuatorAction_SET;
        };
        //D4W = 0;
        switchOpen = false;
      };
      if (!switchOpen)
      {
        result = 1;
        if (DB3A2->audible() != 0)
        {
          QueueSound(1, d.partyX, d.partyY, 1);
        };
        if (   (!d.EmptyHanded) 
            && (   (actuatorType==4)
                 ||(actuatorType==11)
                 ||(actuatorType==17)) )
        {
          dbA3 = GetCommonAddress(objectInHand);
          //dbA3->link(RNnul);
          {
            CURSORFILTER_PACKET cfp;
            cfp.type = CURSORFILTER_TakeKey;
            cfp.object = objectInHand.ConvertToInteger();
            cfp.p1 = d.LoadedLevel;
            cfp.p2 = mapX;
            cfp.p3 = mapY;
            cfp.p4 = actuatorPos;
            CursorFilter(&cfp);
            if (cfp.type != CURSORFILTER_Cancel)
            {
              RemoveObjectFromHand();
              //I reversed these two lines so that the object
              //will still exist when we delete it from the
              //player's hand.  So it has the proper weight!!!
              DeleteDBEntry(dbA3);
              objectInHand = RN(RNempty);
            };
          };
        }
        else
        {
          if ( (d.EmptyHanded) && (actuatorType==12) )
          {
            CURSORFILTER_PACKET cfp;
            cfp.type = CURSORFILTER_GiftFromGod;
            cfp.object = objectInHand.ConvertToInteger();
            cfp.p1 = d.LoadedLevel;
            cfp.p2 = mapX;
            cfp.p3 = mapY;
            cfp.p4 = actuatorPos;
            CursorFilter(&cfp);  // We know objectInHand != RNnul
            if (cfp.type != CURSORFILTER_Cancel)
            {
              objectInHand = ConstructPredefinedMissile(actuatorValue);
              if (objectInHand != RNnul)
              {
                ObjectToCursor(objectInHand, 1);
              };
            };
          };
        };
        QueueSwitchAction(DB3A2, 
                          actuatorAction, 
                          mapX, mapY, actuatorPos);
      };
      continue;
    };
    if (D6W >= dbMONSTER) break;

    // continue;
  }; // for (D7W != -2)
  RotateActuatorList();
  return (result);
}


//*********************************************************
//
//*********************************************************
// TAG01576a
RN RemoveObjectFromHand()
{//(RN)
  dReg D1;
  CHARDESC *pcA0;
  RN objD7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  d.EmptyHanded = 1;
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile), " Set EmptyHanded in RemoveObjectFromHand\n");
  };
  ASSERT(RememberToPutObjectInHand == -1,"objInHand");
  objD7 = d.objectInHand;
  if (objD7 == RNnul) return (objD7);

  {
    //cfp->object = objD7.ConvertToInteger();
    //CursorFilter(cfp);
    d.objectInHand = RN(RNnul);
  };

  d.ObjectType16574 = -1;
  STHideCursor(HC44);
  ClearHeldObjectName();
  TAG002572();
  STShowCursor(HC44);
  if (d.HandChar == -1) return (objD7); //owner of cursor
  D1L = GetObjectWeight(objD7);
  pcA0 = &d.CH16482[d.HandChar];//owner of cursor
  DEBUGLOAD(pcA0, D1W, -1, objD7);
  pcA0->load = sw(pcA0->load - D1W);
  ASSERT(pcA0->load < MAXLOAD,"maxload");
  pcA0->charFlags |= CHARFLAG_possession;
  DrawCharacterState(d.HandChar);//owner of cursor
  return (objD7);
}


//*********************************************************
//
//*********************************************************
i16 TAG016382(RN object)
{
  dReg D6, D7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(GetObjectWeight(object) / 2);
  D6W = ApplyLimits(1, D7W, 10);
  while ((D7W -= 10) > 0)
  {
    //D6W = sw(D7W / 2);
    // Fixed 7 Feb 2010 in version 11.024
    D6L = D6W + sw(D7W / 2);
//
  };
  return D6W;
}

//*********************************************************
//
//*********************************************************
//  TAG017570
i16 ThrowByCharacter(i32 chIdx,i16 hand,i16 LeftOrRight, int disableTime,
                     i32 range, i32 damage, i32 decayRate,
                     bool record, i32 x, i32 y, i32 f)
{//(i16)
  dReg D5, D6, D7;
  WEAPONDESC *weaponDescA2;
  CHARDESC   *pcA3=NULL;
  RN objD4;
  //i16 w_4;
  RN  origObjInWeaponHand = RNnul;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //w_4 = 0;
  if (hand < 0)
  {
    if (d.EmptyHanded) return 0;
    {
      CURSORFILTER_PACKET cfp;
      cfp.type = CURSORFILTER_Throw;
      cfp.object = d.objectInHand.ConvertToInteger();
      cfp.p1 = LeftOrRight;
      CursorFilter(&cfp);
      if (cfp.type == CURSORFILTER_Cancel) return 0;
      objD4 = RemoveObjectFromHand();
    };
    pcA3 = &d.CH16482[chIdx];
    origObjInWeaponHand = pcA3->Possession(1); // Save contents of fighting hand
    pcA3->SetPossession(1, objD4,false); //put obect into fighting hand.
    hand = 1;
    if (record)
    {
      RecordFile_Record(x, y, f);
    };
    D7W = (i16)DetermineThrowingDistance(chIdx, hand);
    pcA3->SetPossession(hand, origObjInWeaponHand, false);  // Replace object in weapon hand.
  }
  else
  {
    if (d.CH16482[chIdx].Possession(hand) != RNnul)
    {
      if (record)
      {
        RecordFile_Record(x, y, f);
      };
    };
    D7W = (i16)DetermineThrowingDistance(chIdx, hand);
    objD4 = RemoveCharacterPossession(chIdx, hand);
    if (objD4 == RNnul) return 0;
  };
  QueueSound(16, d.partyX, d.partyY, 1);
  AdjustStamina(chIdx, TAG016382(objD4));
  DisableCharacterAction(chIdx, (disableTime<0)?4:disableTime);
  D6W = 8;
  D5W = 1;
  //D0W = ;
  if (objD4.dbType() == dbWEAPON)
  {
    D6W += 4;
    weaponDescA2 = TAG0099d2(objD4);
    if (weaponDescA2->uByte1 <= 12)
    {
      D5W = weaponDescA2->uByte3;
      D6W = sw(D6W + D5W/4);
    };
  };
  AdjustSkills(chIdx, 10, D6W, ASW_ThrowByCharacter);
  D7W = sw(D7W + D5W);
  D5W = (i16)DetermineMastery(chIdx, 10); // Skill #10
  D7W = sw(D7W + STRandom(16) + D7W/2 + D5W);
  D6W = ApplyLimits(40, 8*D5W + (STRandom()&31), 200);
  D5W = sw(Larger(5, 11 - D5W));
  LaunchMissile(
                objD4,
                d.partyX,
                d.partyY,
                (d.partyFacing+LeftOrRight)&3,
                d.partyFacing,
                (range<0)?D7W:range,  //range
                (damage<0)?D6W:damage,  //damage
                (decayRate<0)?D5W:decayRate,  //DecayRate
                false);
  d.Word11712 = 4;
  d.Word11714 = d.partyFacing;
  DrawCharacterState(chIdx);
  return (1);
}

//*********************************************************
//
//*********************************************************
//  TAG01772e
i16 ThrowObjectInHand(i16 LeftOrRight,bool record, i32 x, i32 y, i32 f)
{//(i16)
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.HandChar == -1) return 0;//owner of cursor
  return ThrowByCharacter(d.HandChar, -1, LeftOrRight,-1,
         -1,-1,-1,
         record,x,y,f); //owner of cursor
}

//*********************************************************
//
//*********************************************************
//   TAG01a0c2
void TouchWallF1()
{//()
  i32 touchedX;
  i32 touchedY;
  touchedX = d.partyX;
  touchedY = d.partyY;
  touchedX += d.DeltaX[d.partyFacing];
  touchedY += d.DeltaY[d.partyFacing];
  if (touchedX < 0) return;
  if (touchedX >= d.width) return;
  if (touchedY < 0) return; 
  if (touchedY >= d.height) return;
  d.clockTick = TouchWall(touchedX, touchedY, (d.partyFacing+2)&3);
}

//*********************************************************
//
//*********************************************************
void TAG01a148(i16 P1)
{//()
  dReg D0, D1, D5, D6, D7;
  OBJ_NAME_INDEX objNID0;
  RN objD4;
  bool levitated;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = P1;
  if (d.HandChar == -1) return;//owner of cursor
  D6W = d.partyX;
  D5W = d.partyY;
  if (D7W >= 2) // If next square ahead of party
  {
    D1W = d.DeltaX[d.partyFacing];
    D6W = sw(D6W + D1W);
    D1W = d.DeltaY[d.partyFacing];  
    D5W = sw(D5W + D1W);
    objD4 = FindFirstMonster(D6W, D5W);
    if (objD4 != RNeof)
    {
      levitated = objD4.Levitating(); // /*, 4*/);
      if (!levitated)
      {
        D0W = OrdinalOfMonsterAtPosition(GetRecordAddressDB4(objD4),
                         (d.partyFacing+D7W)&3);
        if (D0W != 0) return;
      };
    };
  };
  objD4 = d.rn10574[D7W];
  objNID0 = objD4.NameIndex();
  if (objNID0 != -1)
  {
    CURSORFILTER_PACKET cfp;
    cfp.type = CURSORFILTER_PickFromFloor;
    cfp.object = objD4.ConvertToInteger();
    cfp.p1 = d.LoadedLevel;
    cfp.p2 = D6W;
    cfp.p3 = D5W;
    cfp.p4 = D7W;
    CursorFilter(&cfp);
    if (cfp.type == CURSORFILTER_Cancel) return;
    MoveObject(objD4, D6W, D5W, -1, 0, NULL, NULL);
    d.Word16580 = 1;
    ObjectToCursor(objD4, 1);
  };
  d.clockTick = 1;
}


//*********************************************************
//
//*********************************************************
//   TAG01a228
void DropObject(i16 pos)
{//()
  // 0 = this room left, 1= this room right
  // 2 =                 3 = 
  // 4 = niche
  dReg D0, D1, D5, D6, D7;
  RN objD4;
  DB10     *DB10A3;
  TIMER timer_16;
  bool dropInAlcove;//w_6
  i16 w_4;
  OBJ_NAME_INDEX objNI_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = pos;
  if (d.HandChar == -1) return; //owner of cursor
  D6W = d.partyX;
  D5W = d.partyY;
  dropInAlcove = D7W==4;
  if (dropInAlcove) D7W = 2;
  if (D7W > 1)
  { // In next room.
    D6W = sw(D6W + d.DeltaX[d.partyFacing]);
    D5W = sw(D5W + d.DeltaY[d.partyFacing]);
  };
  w_4 = (I16)((d.partyFacing + D7W) & 3);
  {
    CURSORFILTER_PACKET cfp;
    cfp.type   = CURSORFILTER_DropObject;
    cfp.object = d.objectInHand.ConvertToInteger();
    cfp.p1     = d.LoadedLevel;
    cfp.p2     = D6W;
    cfp.p3     = D5W;
    cfp.p4     = w_4;
    CursorFilter(&cfp);
    if (cfp.type == CURSORFILTER_Cancel) return;
    objD4 = RemoveObjectFromHand();
  };
  objD4.pos(w_4);//04Nov2002 prs
  MoveObject(
             objD4,
             -1,
             0,
             D6W,
             D5W,
             NULL,
             NULL);
  if (dropInAlcove != 0)
  {
    if (d.FacingViAltar != 0)
    {
      objNI_2 = objD4.NameIndex();
      if (objNI_2 == objNI_Bones_a) // bones??
      {
        DB10A3 = GetRecordAddressDB10(objD4);
        ui32 bonesRecord[1];
        if (expool.CopyChampionBonesRecord(objD4, bonesRecord))
        {
          CHARDESC character;
          if (character.CopyCharacter(bonesRecord[0]&0xffff))
          {
            if (character.HP() == 0)
            {
              if (DB10A3->value() == 2)
              {
                D0L = d.Time;
                //D1L = d.partyLevel;
                //D1L <<= 24;
                //timer_16.timerTime = D0L | D1L;
                timer_16.Time(D0L);
                timer_16.Level((ui8)d.partyLevel);
                timer_16.Function(TT_ViAltar);
                //not needed.  timer process will determine character index. timer_16.timerUByte5 = DB10A3->value();
                //The bones we put here should be on top of the pile of
                //objects in the alcove.
                timer_16.timerUByte6(D6B);     //location of bones
                timer_16.timerUByte7(D5B);     //location of bones
                timer_16.timerUByte5(0);
                *timer_16.pTimerObj8() = objD4; //The bones object
                timer_16.packedPos(w_4);        //position of bones
                timer_16.packedState(2);
                gameTimers.SetTimer(&timer_16);
              };
            };
          };
        }
        else
        {
          i32 chIdx;
          chIdx = DB10A3->value();
          if (chIdx < d.NumCharacter)
          {
            if (d.CH16482[chIdx].HP() == 0)
            {
              D0L = d.Time;
              D1L = d.partyLevel;
              D1L <<= 24;
              //timer_16.timerTime = D0L | D1L;
              timer_16.Time(D0L);
              timer_16.Level((ui8)d.partyLevel);
              timer_16.Function(TT_ViAltar);
              //not needed.  timer process will determine character index. timer_16.timerUByte5 = DB10A3->value();
              //The bones we put here should be on top of the pile of
              //objects in the alcove.
              timer_16.timerUByte6(D6B);     //location of bones
              timer_16.timerUByte7(D5B);     //location of bones
              timer_16.timerUByte5(0);
              *timer_16.pTimerObj8() = objD4; //The bones object
              timer_16.packedPos(w_4);        //position of bones
              timer_16.packedState(2);
              gameTimers.SetTimer(&timer_16);
            };
          };
        };
      };
    };
  };
  d.clockTick = 1;
}


//*********************************************************
//
//*********************************************************
//  TAG01a34c
i32 TestThrow(i16 clickX, i16 clickY)
{//(i16)
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if ( (clickY<47) || (clickY>102) )
  {
    return 0;
  };
  if (clickX <= 111)
  {
    if (clickX < 32) return 0;
    if (clickX < 64)
    {
      clickX = 63;
      if (d.CellTypeJustAhead == roomDOORFACING)
      {
        return 0;
      };
    }
    else
    {
      clickX = 111;
    };
    return ThrowObjectInHand(0,true,clickX,47,80);
  };
  if (clickX > 191) return 0;
  if (clickX > 163)
  {
    clickX = 191;
    if (d.CellTypeJustAhead == roomDOORFACING)
    {
      return 0;
    };
  }
  else
  {
    clickX = 163;
  };
  return ThrowObjectInHand(1,true,clickX,47,80);
}


//*********************************************************
//
//*********************************************************
//  TAG01a3d6
i16 TestInRectangle(ui8 *rectangle,i32 clickX, i32 clickY)
{
  ui8 *r = (ui8 *) rectangle;
  if (clickX < r[0]) return 0;
  if (clickX > r[1]) return 0;
  if (clickY < r[2]) return 0;
  if (clickY > r[3]) return 0;
  return 1;
}

//*********************************************************
//
//*********************************************************
//   TAG01a436
void HandleClickInViewport(i32 clickX, i32 clickY)
{//()
  dReg D0, D1, D5;
  DBCOMMON *dbA3;
  DB0      *DB0A3;
  CHARDESC *pcA0;
  i16 touchedY;
  i16 touchedX;
  i16 LOCAL_6;
  RN  obj_LOCAL_6;
  OBJ_NAME_INDEX objNI_4;
  RN  LOCAL_2;
  touchedX = d.partyX;
  touchedY = d.partyY;
  D1W = d.partyFacing;
  D1W = d.DeltaX[D1W];
  touchedX = sw(touchedX + D1W); // Cell touched X
  D1W = d.DeltaY[d.partyFacing];
  touchedY = sw(touchedY + D1W); // Cell touched Y 
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7W = sw(clickX);  //use clickX instead.
  //D6W = sw(clickY);  //use clickY instead.
  if (d.CellTypeJustAhead == roomDOORFACING)
  {
    if (d.HandChar == -1) 
    {
      RecordFile_Record(clickX, clickY, 80);
      return;//owner of cursor
    };
    D0W = d.EmptyHanded;
    if (D0W != 0)
    {
      DB0A3 = GetRecordAddressDB0(FindFirstDoor(touchedX, touchedY));
      D0W = DB0A3->doorSwitch();
      if (D0W != 0)
      {
        D0W = TestInRectangle((ui8 *)&d.ViewportObjectButtons[5], 
                         clickX, 
                         clickY-33);
        if (D0W != 0)
        {
          //RecordFile.Record((d.ViewportObjectButtons[5][0]+d.ViewportObjectButtons[5][1])/2,
          //                  (d.ViewportObjectButtons[5][2]+d.ViewportObjectButtons[5][3])/2+33,
          //                  80);
          RecordFile_Record((d.ViewportObjectButtons[5].x1+d.ViewportObjectButtons[5].x2)/2,
                            (d.ViewportObjectButtons[5].y1+d.ViewportObjectButtons[5].y2)/2+33,
                            80);
          d.clockTick = 1;
          QueueSound(1, d.partyX, d.partyY, 1);
          CreateTimer(TT_DOOR,
                      touchedX,
                      touchedY,
                      0,
                      2,
                      d.Time+1);
          return;
        };
      };
    }
    else
    {
      D0L = TestThrow(sw(clickX), sw(clickY));
      if (D0W != 0) return;
    };
  };
  D0W = d.EmptyHanded;
  if (D0W != 0)
  {
    for (D5W=0; D5W<6; D5W++)
    {
      D0W = TestInRectangle((ui8 *)&d.ViewportObjectButtons[D5W], 
                             clickX, 
                             clickY-33);
      if (D0W != 0)
      {
        i32 middleX, middleY, btn;
        middleX = (d.ViewportObjectButtons[D5W].x1+d.ViewportObjectButtons[D5W].x2)/2;
        middleY = (d.ViewportObjectButtons[D5W].y1+d.ViewportObjectButtons[D5W].y2)/2+33;
        for (btn=0; btn<D5W; btn++)
        {
          if (TestInRectangle((ui8 *)&d.ViewportObjectButtons[btn],
                              middleX,
                              middleY-33)) break;
        };
        if (btn == D5W)
        {
          RecordFile_Record(middleX, middleY, 80);
        }
        else
        {
          RecordFile_Record(clickX, clickY, 80);
        };
        if (D5W == 5)
        {
          if (d.FacingAlcove == 0)
          {
            TouchWallF1();
          };
        }
        else
        {
          TAG01a148(D5W);
        };
        return;
      };

    };
    {
      if (   (d.FacingAlcove == 0)
          && (d.CellTypeJustAhead == 0)) //Stone or visible FalseWall
      {
        CELLFLAG cf;
        ROOMTYPE rt;
        cf = GetCellFlags(touchedX, touchedY);
        rt = (ROOMTYPE)(cf>>5);
        if (   (rt==roomSTONE) 
            || ((rt==roomFALSEWALL) && ((cf & 5) == 0)))
        {
          if (   (clickX >= 33)
              && (clickX <= 189)
              && (clickY >= 44)
              && (clickY <= 150))
          {
            QueueSound(4,d.partyX,d.partyY,0);
          };
        };
      };
    };
    RecordFile_Record(clickX, clickY, 80);
    return;
  };
  ASSERT(RememberToPutObjectInHand == -1,"objInHand");
  LOCAL_2 = d.objectInHand;
  dbA3 = GetCommonAddress(LOCAL_2); // (UNKNOWN *)
  if (d.CellTypeJustAhead == 0) // Stone or visible false wall
  {
    for (D5W=0; D5W<2; D5W++)
    {
      D0W = TestInRectangle(d.DropAreas+4*D5W, clickX, clickY);
      if (D0W != 0)
      {
        RecordFile_Record((d.DropAreas[4*D5W+0]+d.DropAreas[4*D5W+1])/2,
                          (d.DropAreas[4*D5W+2]+d.DropAreas[4*D5W+3])/2,
                          80);
        DropObject(D5W); // D5=position
        return;
      };
//
    };
    D0W = TestInRectangle((ui8 *)&d.ViewportObjectButtons[5], 
                    clickX, 
                    clickY-33);
    if (D0W != 0)
    {
      RecordFile_Record((d.ViewportObjectButtons[5].x1+d.ViewportObjectButtons[5].x2)/2,
                        (d.ViewportObjectButtons[5].y1+d.ViewportObjectButtons[5].y2)/2+33,
                        80);
      if (d.FacingAlcove != 0)
      {
        DropObject(4); // 4 = position
      }
      else
      {
        if (d.FacingWaterFountain != 0)
        {
          objNI_4 = LOCAL_2.NameIndex();
          D0W = sw(GetObjectWeight(LOCAL_2));
          LOCAL_6 = D0W;
          obj_LOCAL_6 = LOCAL_2;
          if ( (objNI_4 >= objNI_Waterskin) && (objNI_4 <= objNI_Water) )
          {
            dbA3->CastToDB10()->value(3);
          }
          else
          {
            if (objNI_4 == objNI_EmptyFlask)
            {
              //D3W = 15 << 8;
              //dbA3->word(2) &= 0x80ff; // Bits 8-14
              //dbA3->word(2) |= D3W;
              dbA3->CastToDB8()->potionType(potion_WaterFlask);
            }
            else
            {
              goto tag01a69a;
            };
          };
          DrawEightHands();
          D1W = sw(GetObjectWeight(LOCAL_2));
          D1W = sw(D1W - LOCAL_6);
          pcA0 = &d.CH16482[d.HandChar];//owner of cursor
          DEBUGLOAD(pcA0, LOCAL_6, -1, obj_LOCAL_6);
          DEBUGLOAD(pcA0, GetObjectWeight(LOCAL_2), +1, LOCAL_2);
          pcA0->load = sw(pcA0->load + D1W);
          ASSERT(pcA0->load < MAXLOAD,"maxload");
        };
tag01a69a:
        TouchWallF1();
      };
      return;
    };
    RecordFile_Record(clickX, clickY, 80);
    return;
  };
  D0L = TestThrow(sw(clickX), sw(clickY));
  if (D0W != 0) return;
  for (D5W=0; D5W<4; D5W++)
  {
    D0W = TestInRectangle(d.DropAreas+4*D5W, clickX, clickY);
    if (D0W != 0)
    {
      RecordFile_Record((d.DropAreas[4*D5W+0]+d.DropAreas[4*D5W+1])/2,
                        (d.DropAreas[4*D5W+2]+d.DropAreas[4*D5W+3])/2,
                        80);
      DropObject(D5W); // D5=position
      return;
    };
//
  };
  RecordFile_Record(clickX, clickY, 80);
}


bool mouseQueueIsEmpty()
{
  return ((d.MouseQEnd + 1)%MOUSEQLEN) == d.MouseQStart;
}

bool GravityMove(MouseQueueEnt *ent)
{
  i32 axis, absdir, reldir;
  i32 i, delta, level, x, y;
  CELLFLAG cfh, cfd;
  ROOMTYPE rth, rtd;
  if (d.Time < nextGravityMoveTime) return false;
  ent->x = 0;
  ent->y = 0;
  for (i=2; i>=0; i--)
  {
    axis = moveHistory[i];
    delta = (moveHistory[3] & (1<<axis))?1:-1;
    level = d.partyLevel;
    x = d.partyX;
    y = d.partyY;
    switch (axis)
    {
    case 0: x += delta; break;
    case 1: y += delta; break;
    case 2: level += delta; break;
    };
    if (x < 0) continue;
    if (y < 0) continue;
    if (level < 0) continue;
    if (level >= d.dungeonDatIndex->NumLevel()) continue;
    if (x > d.pLevelDescriptors[level].LastColumn()) continue;
    if (y > d.pLevelDescriptors[level].LastRow()) continue;
    cfh = *GetCellFlagsAddress(d.partyLevel,d.partyX,d.partyY);
    rth = RoomType(cfh);
    cfd = *GetCellFlagsAddress(level,x,y);
    rtd = RoomType(cfd);
    switch (axis)
    {
    case 0:
    case 1:
      switch (rtd)
      {
      case roomPIT:
      case roomOPEN: break;
      default: continue;
      };
      break;
    case 2:
      switch (delta)
      {
      case +1: if (rth != roomPIT) continue; break;
      case -1: if (rtd != roomPIT) continue; break;
      };
      break;
    };
    // It appears that gravity will cause another move.
    if (FindFirstMonster(level, x, y) != RNeof) continue;
    switch (axis)
    {
    case 0: absdir = (delta==1)?1:3; break;
    case 1: absdir = (delta==1)?2:0; break;
    default: absdir = (delta==1)?5:4; break;
    };
    switch (absdir)
    {
    case 0:
    case 1:
    case 2:
    case 3:
      reldir = (absdir - d.partyFacing) & 3;
      switch (reldir)
      {
      case 0: ent->num = 3; break;
      case 1: ent->num = 4; break;
      case 2: ent->num = 5; break;
      case 3: ent->num = 6; break;
      };
      break;
    case 4:
      ent->num = 0x1125; break;
    case 5:
      ent->num = 0x1124; break;
    };
    return true;
  };
  gravityMove = false;
  return false;
}


//*********************************************************
//
//*********************************************************
//   TAG01a7b2
RESTARTABLE _HandleMouseEvents(const i32 delta)
{//(int)
  static i32 lastTime=0;
  static bool thawCommand;
  static i32 result;
  RESTARTMAP
    RESTART(1)
    RESTART(3)
    RESTART(4)
    RESTART(5)
    RESTART(8)
    RESTART(10)
    RESTART(11)
    RESTART(14)
  END_RESTARTMAP
  static dReg D0, D6, D7;
  static i32 clickX, clickY;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //i32 D6;
  //pnt A3;
  result = 0;
  DiskMenuNeeded = false;

  if (delta == -999887)
  {
    //Initialize
    lastTime = 0;
    RETURN_int(result);
  };
  
  d.MouseInterlock = 1;

  if (gravityMove)
  {
    d.MouseQStart = d.MouseQEnd = 0;
    if (!GravityMove(&pMouseQueue[d.MouseQStart]))
    {
      d.MouseInterlock = 0;
      d.MouseQStart = 1;
      RETURN_int(result);
    };
  };

  if (!gravityMove && IsPlayFileOpen())
  {
    //STHideCursor();
    d.MouseQStart = d.MouseQEnd = 0;
    if (!PlayFile_Play(&pMouseQueue[d.MouseQStart])) 
    {
      if (gameFrozen && PlayFile_IsEOF())
      {
        PlayFile_ReadEOF(); //Force close with next Play();
      };
      d.MouseInterlock = 0;
      d.MouseQStart = 1;
      RETURN_int(result);
    };
  };

//  if (d.Time == 0x2b828)
//  {
//    i32 jjj321 = 1; //Reminds us to remove this nonsense
//    RN containedObject;
//    DB9 *pDB9 = GetRecordAddressDB9(RNVAL(0x2402));
//    containedObject = pDB9->contents(); //Gem of Ages
//    DBCOMMON *pDB = GetRecordAddress(containedObject);
//    pDB->link(RNeof);
//  };  
  D0W = d.MouseQEnd;
  D0W++;
  D7W = D0W; // D7 = End+1
  if (D0W > MOUSEQLEN-1) D7W = 0;
  if (D7W == d.MouseQStart)
  { //The queue is empty
    d.MouseInterlock = 0;
    CauseFakeMouseClick();//If one is waiting
    RETURN_int(result);
  };

  // There is something in the mouse queue.
  //
  // If the game is frozen then we discard
  // any mouse events that do not thaw it.
  //
  if (!IsPlayFileOpen())
  {
    if (gameFrozen)
    {
      if (pMouseQueue[d.MouseQStart].num != 148) //Restart clock
      {
        d.MouseQStart = sw((d.MouseQStart+1)%MOUSEQLEN);
        d.MouseInterlock = 0;
        CauseFakeMouseClick(); //if one is waiting
        RETURN_int(result);
      };
    };
  }
  else
  {
    if (gameFrozen && PlayFile_IsEOF())
    {
      PlayFile_ReadEOF(); //Force close with next Play();
    };
  };
  // We limit mouse events to no more than one per 
  // clock tick.  This pretty much ensures that we
  // can record a session and play it back with identical
  // results.  The reasons that this works are a bit
  // complex and not too clear to me.  But it has
  // been very effective as a tool for reproducing
  // errors so that they can be more easily found and fixed.
  // It also makes things just slightly more sluggish
  // because you can only have 6 events per second.
  //
  thawCommand = 
           gameFrozen 
        && (pMouseQueue[d.MouseQStart].num == 148);

  //dragFromMarchingOrder = pMouseQueue[d.MouseQStart].num == 0xffff;

  if (    !IsPlayFileOpen()
       && (lastTime != 0) 
       && (d.Time < lastTime + delta)
       && (!thawCommand)
       /*&& (RecordFile.IsOpen())*/  ) 
  {
    d.MouseInterlock = 0;
    RETURN_int(result);
  };

  
  

//D6 = d.Word16854 * 6;
//A0 = &d.Word16848 + D6W;
  D6W = pMouseQueue[d.MouseQStart].num;
  if (  (D6W >= 3)
      &&(D6W <= 6)
      &&(  (d.partyMoveDisableTimer!=0)
         ||(  (d.Word11712!=0)
             &&(((d.partyFacing+D6W-3)&3) == d.Word11714)
           )
        )
      )
  {
    if (IsPlayFileOpen())
        PlayFile_Backspace(&pMouseQueue[d.MouseQStart]);
    d.MouseInterlock = 0;
    CauseFakeMouseClick();    
    RETURN_int(result);
  };
  
  if (pMouseQueue[d.MouseQStart].num != 80)
  { // HandleClickInViewport will do its own recording.
    // This is because the active areas in the viewport depend on
    // the game's state and we want to use as few xy coordinates as
    // possible so that the logfile can be compressed nicely.
    RecordFile_Record(&pMouseQueue[d.MouseQStart]);
  };
  clickX = pMouseQueue[d.MouseQStart].x;
  clickY = pMouseQueue[d.MouseQStart].y;
  if (IsPlayFileOpen())
  {
//    d.CurCursorX = sw(clickX);
//    d.CurCursorY = sw(clickY);
//    d.NewMouseX  = sw(clickX);
//    d.NewMouseY  = sw(clickY);
  };
  d.MouseQStart = sw((d.MouseQStart + 1) % MOUSEQLEN);
  //D0W = d.MouseQStart;
  //if (D0W > 4) d.MouseQStart = 0;
  d.MouseInterlock = 0;
  CauseFakeMouseClick();

  if (D6W == 0x0add)
  {
    //This is the mouse 'button-up' event (unClick).
    //It was moved here so that it would be processed in
    //the correct order along with the 'Click' events.  It used
    //to be processed immediately without being sent through 
    //the queue.  This event can be followed immediately (in
    //the same 'Clock Tick') by the following event.
    d.MouseSwitches = 0;
    if (d.PressingEye != 0)
    {
      d.PressingEyeOrMouth = 0;
      d.QuitPressingEye = 1;
    }
    else
    {
      if (d.PressingMouth != 0)
      {
        d.PressingEyeOrMouth = 0;
        d.QuitPressingMouth = 1;
      };
      //lastTime = d.Time;
    };
    result |= 1;  //Allow the next mouse event to follow in the same 
                  //'Clock-Tick'.
    RETURN_int(result);
  };
  if ((D6W == 2) || (D6W == 1)) // Turns
  {
    TurnParty(_1_,D6W);
    lastTime=d.Time;
    RETURN_int(result);
  };
  if (  ((D6W >= 3) && (D6W <= 6)) // Moves
      ||(D6W == 0x1124)    //Down
      ||(D6W == 0x1125))   //Up
  {
    MOVEBUTN *pButton;
    pButton = MoveParty(D6W); // TAG01992e
    if (pButton != NULL)
    {
      FlashButton(_5_,
                  pButton->x1,   // TAG1979c
                  pButton->x2,
                  pButton->y1,
                  pButton->y2);
    };
    lastTime=d.Time;
    RETURN_int(result);
  };

  if ((D6W >= 12) && (D6W <= 15))
  {
    D7W = D0W = sw(D6W - 12);
    if (   ((ui16)D0W<(ui16)d.NumCharacter) 
        && (d.PotentialCharacterOrdinal==0)) 
    {
      TAG019c34(D7W, clickX, clickY);
    };
    lastTime=d.Time;
    RETURN_int(result);
  };

  if ((D6W >=125)&&(D6W<=128))
  {
    if (IsPlayFileOpen())
    {
//      d.CurCursorX = sw(clickX);
//      d.CurCursorY = sw(clickY);
      d.NewMouseX  = sw(clickX);
      d.NewMouseY  = sw(clickY);
    };
    TAG0025a6(D6W-125);
     lastTime=d.Time;
    RETURN_int(result);
  };

  if ((D6W>=28)&&(D6W<66))
  {
    if (d.HandChar != -1) //owner of cursor
    {
      HandleClothingClick(D6W-20);
    };
    lastTime=d.Time;
    RETURN_int(result);
  };

  if ((D6W>=7) && (D6W<=11))
  {
    D7W = D0W = sw(D6W-7);

    if ((D7W==4) || (D7W<d.NumCharacter))
    {
      if (d.PotentialCharacterOrdinal==0)
      {
        ShowHideInventory(D7W);
      };
    };
    lastTime=d.Time;
    RETURN_int(result);
  };
  if (D6W == 83)
  {
    if (d.HandChar != -1) //owner of cursor
    {
      ShowHideInventory(d.HandChar);//owner of cursro
    };
    lastTime=d.Time;
    RETURN_int(result);
  };
  if (D6W == 100)
  {
    if ((d.PotentialCharacterOrdinal==0)&&(d.MagicCaster!=-1))
    {
      HandleMagicClick(_14_, clickX ,clickY);
    };
    lastTime=d.Time;
    RETURN_int(result);
  };

  if (D6W == 111)
  {
    if (d.PotentialCharacterOrdinal==0)
    {
      TAG019fac(_3_,clickX, clickY);
    };
    lastTime=d.Time;
    RETURN_int(result);
  };

  if (D6W == 70)
  {
    FeedCharacter(_10_);
    lastTime=d.Time;
    RETURN_int(result);
  };
  if (D6W == 71)
  {
    ClickOnEye(_4_);
    lastTime=d.Time;
    RETURN_int(result);
  };
  if (D6W == 80) // The Viewport while adventuring
  {
    if (d.ClockRunning != 0) HandleClickInViewport(clickX, clickY);
    lastTime=d.Time;
    RETURN_int(result);
  };
  if (D6W == 81)
  {
    TAG01a6ea(_11_, clickX, clickY);
    lastTime=d.Time;
    RETURN_int(result);
  };


  if (d.PressingEye!=0) {lastTime=d.Time; RETURN_int(result);};
  if (d.PressingMouth!=0) {lastTime=d.Time; RETURN_int(result);};


  if (D6W == 145)
  {
    if (d.PotentialCharacterOrdinal == 0)
    {
      if (d.SelectedCharacterOrdinal != 0)
      {
        ShowHideInventory(4); //Hide
      };
      TAG02076e();
      d.PartySleeping = 1;
      if (NoSpeedLimit > 0) VBLperTimer = 50;
      DisplaySleepScreen();
      MarkViewportUpdated(2);
      scrollingText.SetNewSpeed(d.Speed, 0);
      d.Speed = 0;
      d.PrimaryButtonList = d.Buttons17796;
      d.SecondaryButtonList = NULL;
      d.pKeyXlate2 = (KeyXlate *)d.Byte18440;
      d.pKeyXlate1 = NULL;
      DiscardAllInput();
      videoMode = VM_SLEEPING;
    };

    lastTime=d.Time;
    RETURN_int(result);
  };
  if (D6W == 146)
  {
    WakeUp();
    lastTime=d.Time;
    RETURN_int(result);
  };
  if (D6W == 140)
  {
    if (   ((ui16)d.NumCharacter > 0) 
        && (d.PotentialCharacterOrdinal == 0))
    {
      //DisplayDiskMenu(_9_);
      //We cannot call 'DisplayDiskMenu' here because
      //it would result in recursive call to ourselves.
      //So we set the flag and return.  Everyone who
      //call us checks the flag and does the call to
      //'DisplayDiskMenu' for us.
      DiskMenuNeeded = true;
    };
    lastTime=d.Time;
    RETURN_int(result);
  };
  if (D6W == 147)
  {
    d.ClockRunning = 0;
    gameFrozen = true;
    videoMode = VM_FROZEN;
    TAG02076e();
    Flood(d.pViewportBMP, 0, 1904); // Flood with color 0
    TextOut_OneLine(d.pViewportBMP, 
                    112, 
                    81, 
                    69, 
                    4, 
                    0, 
                    "GAME FROZEN",
                    999,
                    false);
    MarkViewportUpdated(2);
    d.Pointer18982 = d.PrimaryButtonList;
    d.Pointer18986 = d.SecondaryButtonList;
    d.pKeyXlate18974 = d.pKeyXlate2;
    d.pKeyXlate18978 = d.pKeyXlate1;
    d.PrimaryButtonList = d.Buttons17832;
    d.SecondaryButtonList = NULL;
    d.pKeyXlate2 = (KeyXlate *)d.Byte18448;
    d.pKeyXlate1 = NULL;
    DiscardAllInput();
    lastTime=d.Time;
    RETURN_int(result);
  };
  if (D6W == 148)
  {
    d.ClockRunning = 1;
    gameFrozen = false;
    videoMode = VM_ADVENTURE;
    TAG0207cc();
    d.PrimaryButtonList = d.Pointer18982;
    d.SecondaryButtonList = d.Pointer18986;
    d.pKeyXlate2 = d.pKeyXlate18974;
    d.pKeyXlate1 = d.pKeyXlate18978;
    DiscardAllInput();
    lastTime=d.Time;
    RETURN_int(result);
  };
  if (D6W == 200)
  {
    d.gameState = GAMESTATE_EnterPrison;
    lastTime=d.Time;
    RETURN_int(result);
  };
  if (D6W == 201)
  {
    d.gameState = GAMESTATE_ResumeSavedGame;
    lastTime=d.Time;
    RETURN_int(result);
  };
  if (D6W == 202)
  {
    TAG01f928(_8_);
    lastTime=d.Time;
    RETURN_int(result);
  };
  if ((D6W >= 210) && (D6W <= 213))
  {
    d.Word11764 = sw(D6W - 209);
      lastTime=d.Time;
      RETURN_int(result);
  };
  if (D6W == 215) d.PartyHasDied = 1;
  if (D6W == 216) SwapTextZOrder();
  if (D6W == -1) // Changing marching order.
                 // Cursor moved outside of rectangle.
  {
    d.NewMouseX = d.NewMouseY = 0;
    result |= 1; // Allow another event in this same tick of the clock.
  };
  lastTime=d.Time;
  RETURN_int(result);
}


ui8 cursorCallers[32];

//*********************************************************
//
//*********************************************************
//   TAG003026
#ifdef _DEBUG
void STShowCursor(HC hc)
#else
void STShowCursor(HC)
#endif
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ASSERT(d.Word23118 > 0, "Negative HideCursor Count");
  d.Word23118--; // Could this be a "hide cursor" count???
    ASSERT(cursorCallers[d.Word23118 & 31] == hc, "CursorCaller malfunction");
  if (d.Word23118==0) 
  {
    d.Word23134 = 1;
    d.Word23130 = 1;
  };
}


//*********************************************************
//
//*********************************************************
//   TAG002fd2
void STHideCursor(HC hc)
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D0W = d.Word23118;
  cursorCallers[d.Word23118 & 31] = (ui8)(hc & 0xff);
  d.Word23118++; // Hide cursor counter???
  if (D0W == 0)
  {
    d.Word23138 = 1; // some sort of cursor flag
    d.Word23134 = 0; // Mouse something
    d.Word23130 = 0;
    d.Word23132 = 0;
    if (d.Word23136 != 0) // If cursor on screen
    {
      d.Word23136 = 0;
      RemoveCursor(); //TAG00289a();
    };
    d.Word23138 = 0;
  };
  //if (d.Word19218 != 12777)
  //{
  //  D0W = TAG0194c0(&d.Byte11678);
  //};
}
