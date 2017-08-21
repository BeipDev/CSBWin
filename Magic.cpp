#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"


// ***********************************************************
//
// ***********************************************************
//  TAG00dd90
i16 LaunchMissile(
              RN   object,
              i32  mapX,
              i32  mapY,
              i32  pos,
              i32  dir,
              i32  range,
              i32  damage,
              i32  decayRate,
              bool missileDelay)
{ //(i16)
  dReg D0, D3;
  RN objD6;
  DB14 *DB14A3;
  TIMER timer;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  objD6 = FindEmptyDB14Entry(false);
  if (objD6 == RNnul) return (-1);
  objD6.pos(pos);
  DB14A3 = GetRecordAddressDB14(objD6);
  DB14A3->flyingObject(object);
  DB14A3->rangeRemaining(Smaller(ub(range), 255));
  DB14A3->damageRemaining(ub(damage));
  //
  // **************************************************
  //
  //  We really should check to see of the room contains
  //  a teleporter that affects this missile.
  //  'AddObjectToRoom' does not check.
  //  'MoveObject' does check be we cannot use it.
  AddObjectToRoom(objD6, RN(RNempty), mapX, mapY, NULL);
  D0L = d.Time+1;
  //D1L = d.LoadedLevel << 24;
  //timer.timerTime = D0L | D1L;
  timer.Time(D0L);
  timer.Level((ui8)d.LoadedLevel);
  //if (d.MissileDelay != 0)
  if (missileDelay)
  {
    timer.Function(TT_Missile);
  }
  else
  {
    timer.Function(TT_Missile0);
  };
  timer.timerUByte5(0);
  timer.timerWord6() = objD6.ConvertToInteger();
  D3W = (I16)(mapX & 31);
  timer.timerWord8() = (I16)((timer.timerWord8() & 0xffe0) | D3W);
  timer.timerWord8() = (I16)((timer.timerWord8() & 0xfc1f) | ((mapY & 31) << 5));
  D3W = (I16)(decayRate & 15);
  timer.timerWord8() = (I16)((timer.timerWord8() & 0x0fff) | (D3W << 12));
  timer.timerWord8() = (I16)((timer.timerWord8() & 0xf3ff) | ((dir & 3) << 10));
  DB14A3->timerIndex(gameTimers.SetTimer(&timer));
  return sw((DB14A3->timerIndex()));
}


// **********************************************************************
// LevelUp
// Extracted from AdjustSkills to make things easier to document
// **********************************************************************
void LevelUp(i32 chIdx, i32 basicSkill) // Fighter, ninja, etc.
{
  i32 newMastery;
  i32 maxManaIncrement, maxVitalityIncrement, maxStaminaIncrement=0, maxHPIncrement;
  i32 antiFireIncrement;
  i32 majorAttributeMaxIncrement, minorAttributeMaxIncrement;
  i32 color;
  //
  newMastery = DetermineMastery(chIdx, basicSkill | 0xc000);
  maxManaIncrement = newMastery;
  //D0L = STRandomBool();
  minorAttributeMaxIncrement = STRandomBool();
  //D1L = STRandomBool();
  majorAttributeMaxIncrement = STRandomBool() + 1;
  maxVitalityIncrement = STRandomBool();
  if (basicSkill != 2)
  {
    maxVitalityIncrement &= newMastery;
  };
  d.CH16482[chIdx].Attributes[Vitality].IncrMaximum(maxVitalityIncrement);
  antiFireIncrement = STRandomBool() & (~newMastery);
  d.CH16482[chIdx].Attributes[AntiFire].IncrMaximum(antiFireIncrement);
  switch (basicSkill)
  {
  case 0:  // fighter
    //D6W /= 16;
    maxStaminaIncrement = d.CH16482[chIdx].MaxStamina() / 16;
    newMastery *= 3;
    d.CH16482[chIdx].Attributes[Strength].IncrMaximum(majorAttributeMaxIncrement);
    d.CH16482[chIdx].Attributes[Dexterity].IncrMaximum(minorAttributeMaxIncrement);
    break;

  case 1: // ninja
    //D6W = sw(D6W / 21);
    maxStaminaIncrement = d.CH16482[chIdx].MaxStamina() / 21;
    newMastery = sw(2 * newMastery);
    d.CH16482[chIdx].Attributes[Strength].IncrMaximum(minorAttributeMaxIncrement) ;
    d.CH16482[chIdx].Attributes[Dexterity].IncrMaximum(majorAttributeMaxIncrement);
    break;
  case 2: // priest
    //D6W = sw(D6W / 25);
    maxStaminaIncrement = d.CH16482[chIdx].MaxStamina() / 25;
    d.CH16482[chIdx].IncrMaxMana(newMastery);
    newMastery = sw(newMastery + (newMastery+1)/2);
    d.CH16482[chIdx].Attributes[Wisdom].IncrMaximum(minorAttributeMaxIncrement);
//tag016272:
    maxManaIncrement = Smaller(STRandom0_3(), maxManaIncrement-1);
    if (d.CH16482[chIdx].MaxMana()+maxManaIncrement > MAXMANA) d.CH16482[chIdx].MaxMana(MAXMANA);
    else d.CH16482[chIdx].IncrMaxMana(maxManaIncrement);
    d.CH16482[chIdx].Attributes[AntiMagic].IncrMaximum(STRandom(3));
    break;
  case 3: // wizard
    //D6W = sw(D6W / 32);
    maxStaminaIncrement = d.CH16482[chIdx].MaxStamina() / 32;
    //D0W = D7W;
    //D1W = D7W / 2;
    if (d.CH16482[chIdx].MaxMana() + newMastery + newMastery/2 <= MAXMANA)
    {
      d.CH16482[chIdx].IncrMaxMana(newMastery + newMastery/2);
    }
    else
    {
      d.CH16482[chIdx].MaxMana(MAXMANA);
    };
    d.CH16482[chIdx].Attributes[Wisdom].IncrMaximum(majorAttributeMaxIncrement);
//tag016272:
    maxManaIncrement = Smaller(STRandom0_3(), maxManaIncrement-1);
    if (d.CH16482[chIdx].MaxMana()+maxManaIncrement > MAXMANA) d.CH16482[chIdx].MaxMana(MAXMANA);
    else d.CH16482[chIdx].IncrMaxMana(maxManaIncrement);
    d.CH16482[chIdx].Attributes[AntiMagic].IncrMaximum(STRandom(3));
    break;
  };
  maxHPIncrement = newMastery + uw(STRandom(((UI16)newMastery / 2) + 1));
  if (d.CH16482[chIdx].MaxHP() + maxHPIncrement > 999)
  {
    d.CH16482[chIdx].MaxHP(999);
  }
  else
  {
    d.CH16482[chIdx].IncrMaxHP(maxHPIncrement);
  };
  //w_26 = uw(STRandom(((UI16)D6W/2)+1));
  if (maxStaminaIncrement == 0) maxStaminaIncrement = 1;
  else maxStaminaIncrement += STRandom(maxStaminaIncrement/2) + 1;
  if (d.CH16482[chIdx].MaxStamina() + maxStaminaIncrement > 9999)
  {
    d.CH16482[chIdx].MaxStamina(9999);
  }
  else
  {
    d.CH16482[chIdx].IncrMaxStamina(maxStaminaIncrement);
  };
  d.CH16482[chIdx].charFlags |= CHARFLAG_statsChanged;
  DrawCharacterState(chIdx);
  PrintLinefeed();
  QuePrintLines(color = (UI8)(d.Byte1386[chIdx]), d.CH16482[chIdx].name);
  QuePrintLines(color, " JUST GAINED A ");
  QuePrintLines(color, d.Pointer16596[basicSkill]); //"FIGHTER", "NINJA", etc
  QuePrintLines(color, " LEVEL");
// ***********************************************************
}


//
// ***********************************************************
//   TAG01605a
void AdjustSkills(i32 chIdx,i32 skillNumber,i32 experience, i32 why)
{//(void)
  //dReg D6;
  SKILL *psA3;
  CHARDESC *pcA2;
  //i16 w_26;
  //i16 w_6;
  //i16 w_4;
  //i16 w_2;
  i32 basicSkill;
  i32 oldMastery, newMastery;
  NEWDSAPARAMETERS ndp;



  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile)," AdjustSkills chidx=%02x skill=%02x by %02x ",chIdx, skillNumber, experience);
  };





/////////////////////////////////////////////////////////////////
//Now we do the actual changes to the character's stats.
/////////////////////////////////////////////////////////////////





//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ASSERT( (skillNumber>=0) && (skillNumber<=19),"skillNum");
  //D7W = sw(Experience);
  if ( (skillNumber >= 4) && (skillNumber <= 11) )
  {
    if (d.LastMonsterAttackTime < d.Time-150)
    {
      experience /= 2;
    };
  };
  //if (experience == 0) return;
  //D6W = d.pCurLevelDesc->experienceMultiplier();
  if (d.pCurLevelDesc->experienceMultiplier() != 0)
  {
    experience *= d.pCurLevelDesc->experienceMultiplier();
  };
  pcA2 = &d.CH16482[chIdx];
  if (skillNumber >= 4)
  {
    basicSkill = sw((skillNumber-4)/4);
  }
  else
  {
    basicSkill = sw(skillNumber);
  };
  oldMastery = (i16)DetermineMastery(chIdx, basicSkill | 0xc000);
  if (skillNumber >= 4)
  {
    if (d.LastMonsterAttackTime > d.Time-25)
    {
      experience *= 2;
    };
  };
  psA3 = &pcA2->skills92[skillNumber];

  
/////////////////////////////////////////////////////
//Now let the Adjust Skills Filter DSA do its thing
/////////////////////////////////////////////////////
  {
    ui32 key, *pRecord;
    i32 len;
    key = (EDT_SpecialLocations<<24)|ESL_ADJUSTSKILLSFILTER;
    len = expool.Locate(key,&pRecord);
    if (len > 0)
    // *****************************************
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
  
            pDSAparameters[1+0] = chIdx;
            pDSAparameters[1+1] = skillNumber;
            pDSAparameters[1+2] = experience;
            pDSAparameters[1+3] = why;
            memcpy(pDSAparameters+1+4, adjustSkillsParameters, sizeof(adjustSkillsParameters));
            pDSAparameters[0] = 4 + sizeof(adjustSkillsParameters)/sizeof(pDSAparameters[0]);
            ProcessDSAFilter(obj, &timer, locr, NULL, &dsaVars);
            chIdx = pDSAparameters[1+0] & 3;
            skillNumber = pDSAparameters[1+1] % 20;
            experience = pDSAparameters[1+2] & 0xfffff;
            LoadLevel(currentLevel);
            continue;
          };
        };
      };
    };    
  };

  if (experience == 0) return;
 
  
  psA3->experience += (UI16)(experience);
  psA3->experience = LimitSkillExperience(psA3->experience);
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"skill[%02x]=%08x ",skillNumber, psA3->experience);
  };
  MostRecentlyAdjustedSkills[AdjustedSkillNumber] = skillNumber;
  LatestSkillValues[AdjustedSkillNumber] = psA3->experience;
  AdjustedSkillNumber = (AdjustedSkillNumber + 1)%2;
  if (psA3->tempAdjust < 32000)
  {
    psA3->tempAdjust = sw(psA3->tempAdjust + ApplyLimits(1, experience/8, 100));
  };
  psA3 = &pcA2->skills92[basicSkill];
  if (skillNumber >= 4)
  {
    psA3->experience += (UI16)(experience);
	psA3->experience = LimitSkillExperience(psA3->experience);
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"skill[%02x]=%08x",basicSkill, psA3->experience);
    };
  };
  newMastery = (i16)DetermineMastery(chIdx, basicSkill | 0xc000);
  if (newMastery <= oldMastery) 
  {
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"\n");
    };
    return;
  };
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile)," LevelUp\n");
  };
  LevelUp(chIdx, basicSkill);
}

//
// ***********************************************************
//   TAG01605a
void AddToSkill(i32 chIdx,i32 skillNumber,i32 experience)
{//(void)
  //dReg D6;
  SKILL *psA3;
  CHARDESC *pcA2;
  //i16 w_26;
  //i16 w_6;
  //i16 w_4;
  //i16 w_2;
  i32 basicSkill;
  i32 oldMastery, newMastery;



  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile)," AddToSkill chidx=%02x skill=%02x by %02x ",chIdx, skillNumber, experience);
  };





/////////////////////////////////////////////////////////////////
//Now we do the actual changes to the character's stats.
/////////////////////////////////////////////////////////////////





//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ASSERT( (skillNumber>=0) && (skillNumber<=19),"skillNum");
  pcA2 = &d.CH16482[chIdx];
  if (skillNumber >= 4)
  {
    basicSkill = sw((skillNumber-4)/4);
  }
  else
  {
    basicSkill = sw(skillNumber);
  };
  oldMastery = (i16)DetermineMastery(chIdx, basicSkill | 0xc000);
  psA3 = &pcA2->skills92[skillNumber];

  
  if (experience <= 0) return;
 
  
  psA3->experience += (UI16)(experience);
  psA3->experience = LimitSkillExperience(psA3->experience);
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"skill[%02x]=%08x ",skillNumber, psA3->experience);
  };
  MostRecentlyAdjustedSkills[AdjustedSkillNumber] = skillNumber;
  LatestSkillValues[AdjustedSkillNumber] = psA3->experience;
  AdjustedSkillNumber = (AdjustedSkillNumber + 1)%2;
  //if (psA3->tempAdjust < 32000)
  //{
  //  psA3->tempAdjust = sw(psA3->tempAdjust + ApplyLimits(1, experience/8, 100));
  //};
  psA3 = &pcA2->skills92[basicSkill];
  if (skillNumber >= 4)
  {
    psA3->experience += (UI16)(experience);
	psA3->experience = LimitSkillExperience(psA3->experience);
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"skill[%02x]=%08x",basicSkill, psA3->experience);
    };
  };
  newMastery = (i16)DetermineMastery(chIdx, basicSkill | 0xc000);
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"\n");
  };
  if (newMastery <= oldMastery) return;
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile)," LevelUp");
  };
  LevelUp(chIdx, basicSkill);
}

// ***********************************************************
//
// ***********************************************************
//  TAG017498
i16 LaunchObject(CHARDESC * pChar,RN object,i32 range,i32 damage,i32 decayRate)
{//(i16)
  dReg D7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = pChar->facing;
  return LaunchMissile(
                object,
                d.partyX,
                d.partyY,
                ((((pChar->charPosition-D7W+1)&2)>>1)+D7W)&3,
                D7W,
                range,      //range
                damage,         //damage
                decayRate,  //decayRate
                false);
}

// ***********************************************************
//
// ***********************************************************
//  TAG0174ec
i16 CharacterThrowsSpell(i32 chIdx, RN object, i32 range, i32 neededMana)
{//(i16)
  dReg D1;
  i32 decayRate;
  CHARDESC *pcA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7W = sw(P3);
  pcA3 = &d.CH16482[chIdx];
  if (pcA3->Mana() < neededMana)
  {
    return 0;
  }
  pcA3->IncrMana(-neededMana);
  pcA3->charFlags |= CHARFLAG_statsChanged;
  D1L = Smaller(8, pcA3->MaxMana()/8);
  decayRate = sw(10 - D1W);
  if (range < 4*decayRate)
  {
    range += 3;
    decayRate -= 1;
  };
  return LaunchObject(pcA3, 
                      object, 
                      range,        //range
                      90,           //damage
                      decayRate);   //decayRate
}

// ***********************************************************
//
// ***********************************************************
//   TAG017756
void DisableCharacterAction(i32 chIdx, i32 timeDelay)
{//(void)
  dReg D5, D6, D7;
  CHARDESC *pcA3;
  TIMER timer;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pcA3 = &d.CH16482[chIdx];
  D7L = d.Time + timeDelay;
  timer.Function(TT_11);
  timer.timerUByte5((ui8)chIdx);
  timer.timerUByte6(0);
  D5W = pcA3->busyTimer;
  if (D5W >= 0)
  {
    //D6L = gameTimers.pTimer(D5W)->timerTime & 0xffffff;
    D6L = gameTimers.pTimer(D5W)->Time();
    if (D7L >= D6L)
    {
      D7L += (D6L-d.Time) >> 1;
    }
    else
    {
      D7L = D6L + timeDelay/2;
    };
    gameTimers.DeleteTimer(D5W);
  }
  else
  {
    pcA3->charFlags |= CHARFLAG_weaponAttack | CHARFLAG_attackDisable;
    DrawCharacterState(chIdx);
  };
  //timer.timerTime = D7L | (d.partyLevel << 24);
  timer.Time(D7L);
  timer.Level((ui8)d.partyLevel);
  pcA3->busyTimer = gameTimers.SetTimer(&timer);

}


// *********************************************************
//
// *********************************************************
//   TAG019d90
RESTARTABLE _MagicSelection(const i32 button)
{//(void)
  static dReg D0, D7;
  static CHARDESC *pcA0;
  static i16 *pwA3;
  RESTARTMAP
    RESTART(1)
    RESTART(3)
    RESTART(5)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (button == 108)
  {
    pcA0 = &d.CH16482[d.MagicCaster];
    D0W = pcA0->incantation[0];
    if (D0W == 0) RETURN;
    FlashButton(_1_,0xea, 0x12f, 0x3f, 0x49);


    d.clockTick = CastMagic(0);
    //d.clockTick = i16Result;
    RETURN;
  };
  D7W = sw(button-101);
  pwA3 = &d.Word18552[4 * D7W];
  FlashButton(_5_,pwA3[0], pwA3[1], pwA3[2], pwA3[3]);
  wvbl(_3_);
  WaitForButtonFlash();// If any are active.
  if (D7W <6)
  {
    TAG01b990(D7W);
  }
  else
  {
    BackspaceMagicSelection();
  };
  RETURN;
}

// **********************************************************
//
// **********************************************************

//   TAG019e4a
RESTARTABLE _HandleMagicClick(const i32 clickX, const i32 clickY)
{//(void)
  static dReg D6, D7;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D6W = -1;
  if (clickY <= 48)
  {
    if (d.MagicCaster == 0)
    {
      if (   ((clickX >= 280) && (clickX <= 291))
          || (clickX == 513)  )
      {
        D6W = 1;
      }
      else
      {
        if (   ((clickX >= 294) && (clickX <= 305))
            || (clickX == 514)  )
        {
          D6W = 2;
        }
        else
        {
          if (   ((clickX >= 308) && (clickX <= 319))
              || (clickX == 515) ) D6W = 3;
        };
      };
    }
    else
    {
      if (d.MagicCaster == 1)
      {
        if (   ((clickX >= 233) && (clickX <= 244))
            || (clickX == 512)  )
        {
          D6W = 0;
        }
        else
        {
          if (   ((clickX >= 294) && (clickX <= 305))
              || (clickX == 514)  )
          {
            D6W = 2;
          }
          else
          {
            if (   ((clickX >= 308) && (clickX <= 319))
                || (clickX == 515)  ) D6W = 3;
          };
        };
      }
      else
      {
        if (d.MagicCaster == 2)
        {
          if (   ((clickX >= 233) && (clickX <= 244))
              || (clickX == 512) )
          {
            D6W = 0;
          }
          else
          {
            if (   ((clickX >= 247) && (clickX <= 258))
                || (clickX == 513)  )
            {
              D6W = 1;
            }
            else
            {
              if (   ((clickX >= 308) && (clickX <=319))
                  || (clickX == 515)  ) D6W = 3;
            };
          };
        }
        else
        {
          if (d.MagicCaster == 3)
          {
            if (   ((clickX >= 247) && (clickX <= 258))
                || (clickX == 513)   )
            {
              D6W = 1;
            }
            else
            {
              if (   ((clickX >= 261) && (clickX <= 272))
                  || (clickX == 514)  )
              {

                D6W = 2;
              }
              else
              {
                if ( (clickX <= 244) || (clickX == 512) ) D6W = 0;
              };
            };
          };
        };
      };
    };
    if (D6W != -1)
    {
      if (D6W < d.NumCharacter)
      {
        SelectMagicCaster(D6W);
      };
    };
    RETURN;
  };
  D7W = SearchButtonList(d.Buttons18060,
                          clickX,
                          clickY,
                          2);
  if (D7W)
  {
    MagicSelection(_1_, D7W);
  };
  RETURN;
}

// *******************************************************************
//
// *******************************************************************
//   TAG01b726
void SelectMagicCaster(i16 chIdx)
{
  dReg D5, D6;
  ui8 *pD4;
  CHARDESC *pcA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7W = P1;
  if (chIdx == d.MagicCaster) return;
  if ( (chIdx!=-1) && (d.CH16482[chIdx].HP()==0) ) return;
  if (d.MagicCaster == -1)
  {
    STHideCursor(HC36);
    BLT2Screen(GetBasicGraphicAddress(9), (RectPos *)d.Word12, 48, -1);
    STShowCursor(HC36);
  };
  if (chIdx == -1)
  {
    STHideCursor(HC37);
    d.UseByteCoordinates = 0;
    NotImplemented(0x1b7ac); // Check word12
    FillRectangle(d.LogicalScreenBase, (RectPos *)d.Word12, 0, 160);
    STShowCursor(HC37);
    return;
  };
  d.MagicCaster = chIdx;
  pcA3 = &d.CH16482[chIdx];
  TAG01b408(2);
  D5W = 0;
  pD4 = d.LogicalScreenBase+8112;
  STHideCursor(HC38);
  DrawSpellControls(chIdx);
//
  for (D6W=1; D6W<12; D6W++)
  {
    D5W += 24;
    pD4 += 160;
    MemMove((ui8 *)d.Byte20842+2*D5W, pD4, 48);

//
  };
  TAG01b408(3);
  D5W = 0;
  pD4 = d.LogicalScreenBase + 10032;

  for (D6W=1; D6W<12; D6W++)
  {
    pD4 += 160;
    D5W += 24;
    MemMove((ui8 *)d.Byte20842+2*D5W, pD4, 48);
//
  };
  STShowCursor(HC38);
}


// *******************************************************************
//
// *******************************************************************

//   TAG01b922
void DisplayMagicSelection(CHARDESC *pChar) // Magic button pressed
{
  dReg D5, D6, D7;
  char b_2[2];
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//
  D5W = StrLen((char *)pChar->incantation);
  D6W = 232;
  b_2[1] = 0;
  for (D7W=0; D7W<4; D7W++)
  {
    if (D7W >= D5W)
    {
      b_2[0]= ' ';
    }
    else
    {
      b_2[0] = pChar->incantation[D7W];
    };
    TextOutToScreen(D6W += 9, 70, 4, 0, b_2);


  };
}


// *********************************************************
//
// *********************************************************
//   TAG01ba5a
void BackspaceMagicSelection(void)
{
  dReg D0, D7;
  CHARDESC *pcA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pcA3 = &d.CH16482[d.MagicCaster];
  D0W = StrLen((char *)pcA3->incantation);
  if (D0W == 0) return;
  D0W = (UI8)(pcA3->byte33);
  D7W = (I16)((D0W + 3) & 3);
  pcA3->byte33 = D7B;
  pcA3->incantation[D7W] = 0;
  STHideCursor(HC39);
  DrawSpellMenuCharacters(D7W); // Zo, Rath, etc.
  DisplayMagicSelection(pcA3);
  STShowCursor(HC39);
}

// *********************************************************
//
// *********************************************************
//   TAG01be38
void SetBrightnessTimer(i32 deltaBrightness, i32 timeDelay)
{
  TIMER timer;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  timer.Function(TT_LIGHT);
  timer.timerWord6() = sw(deltaBrightness);
  //timer.timerTime = (d.Time + timeDelay) | (d.partyLevel << 24);
  timer.Time(d.Time + timeDelay);
  timer.Level((ui8)d.partyLevel);
  timer.timerUByte5(0);
  gameTimers.SetTimer(&timer);
  SelectPaletteForLightLevel();
}

void FillSpellParameters(
               int caster, 
               SPELL_PARAMETERS& spellParameters)
{
  i32 i, multiplier;
  spellParameters.action = 0;
  spellParameters.chIdx = caster;
  spellParameters.disableTime = -1;
  spellParameters.facing = d.partyFacing;
  spellParameters.locationAbs =
    LOCATIONREL(d.partyLevel,d.partyX,d.partyY).Integer();
  spellParameters.missileType = -1;
  spellParameters.skillRequired = -1;
  spellParameters.spellByte5 = -1;
  spellParameters.spellClass = -1;
  spellParameters.incantation = 0;
  for (multiplier=1000,i=0; 
       (i<4) && (d.CH16482[caster].incantation[i]!=0); 
       i++, multiplier/=10)
  {
    spellParameters.incantation +=
      (d.CH16482[caster].incantation[i]-0x60-6*i+1) * multiplier;
  };
  spellParameters.unused1 = -1;
  spellParameters.unused2 = -1;
  spellParameters.unused3 = -1;
  spellParameters.unused4 = -1;
}


void CallSpellFilter(SPELL_PARAMETERS *pSpellParameters)
{
  if (spellFilterLocation & 0x80000000)
  {
    NEWDSAPARAMETERS ndp;
    RN obj;
    TIMER timer;
    i32 objectDB, actuatorType;
    DB3 * pActuator;
    LOCATIONREL locr;
    locr.Integer(spellFilterLocation & 0x7fffffff);
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
// *****************************************
//Let the DSA process the parameters
          memcpy(pDSAparameters+1, pSpellParameters, sizeof(*pSpellParameters));
          pDSAparameters[0] = sizeof(*pSpellParameters)/sizeof(pDSAparameters[0]);
          ProcessDSAFilter(obj, &timer, locr, NULL, &dsaVars);
          memcpy(pSpellParameters, pDSAparameters+1, sizeof(*pSpellParameters));
// *****************************************
          LoadLevel(currentLevel);
          continue;
        };
      };
    };
  };
}


// *********************************************************************
//
// **********************************************************************
//  TAG01c826
i16 CastMagic(const i32)
{ //(i16)
  static dReg D7;
  static CHARDESC *pcA3;
  SPELL_PARAMETERS spellParameters;
  //RESTARTMAP
//    RESTART(1)
  //END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pcA3 = &d.CH16482[d.MagicCaster];
  STHideCursor(HC40);
  WaitForButtonFlash(); // If any are active.
  FillSpellParameters(d.MagicCaster, spellParameters);

  
  CallSpellFilter(&spellParameters);




  D7W = CastSpell(d.MagicCaster, spellParameters);
  if (D7W != 3)
  {
    pcA3->incantation[0] = 0;
    pcA3->byte33 = 0;
    DrawSpellMenuCharacters(0);//Zo, Rath, etc.
    DisplayMagicSelection(pcA3); // clear magic selection display
  }
  else
  {
    D7W = 0;
  };
  STShowCursor(HC40);
  return D7W;
}

// ************************************************************
//
// ************************************************************
//    TAG01c88e
SPELL *Incantation2Spell(pnt incantation)
{
  dReg D6, D7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (incantation[1] == 0) return NULL;
  D6W = 24;
  D7L = 0;
  do
  { // Gather incantation bytes into longword D7L
    D7L |= ((UI8)(*(incantation++)))<<D6W;
    if (*incantation == 0) break;
  } while  ((D6W -= 8) >= 0);
  for (D6W = 0; D6W < 25; D6W++)
  {
    if (d.Spells[D6W].spellID & 0xff000000)
    { // All the spells in d.Spells have byte0 = 0 !
      ASSERT(0,"spell"); // I don't think this is possible!
      if (D7L == d.Spells[D6W].spellID)
      {
        return &d.Spells[D6W];;
      }
    }
    else
    {
      if ((D7L & 0xffffff) == d.Spells[D6W].spellID)
                return &d.Spells[D6W];
    };
//
  };
  return NULL;
}

// ************************************************************
//
// ************************************************************
//   TAG01c90c
void SpellErrorMsg(CHARDESC *pChar,i16 P2,i16 P3)
{
  const char *msg="UNKNOWN SPELL ERROR";
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (P3 > 3)
  {
    P3 = sw(((P3-4)&0xffff) / 4);
  };
  PrintLinefeed();
  QuePrintLines(4, pChar->name);
  if (P2 == 0)
  {
    msg = TranslateLanguage(" NEEDS MORE PRACTICE WITH THIS ");
    QuePrintLines(4, msg);
    QuePrintLines(4, TranslateLanguage(d.Pointer16596[P3]));
    msg = TranslateLanguage(" SPELL.");
  }
  else
  {
    if (P2 == 1)
    {
      msg = TranslateLanguage(" MUMBLES A MEANINGLESS SPELL.");
    }
    else
    {
      if (P2 == 10)
    msg = TranslateLanguage(" NEEDS AN EMPTY FLASK IN HAND FOR POTION.");
    };
  };
  msg = TranslateLanguage(msg);
  QuePrintLines(4, msg);
}
//       TAG01c9b2
DB8 *GetFlaskInHand(CHARDESC *pChar,RN *pObject)
{
  dReg D6;
  RN objD7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  for (D6W=2; (--D6W)>=0; )
  {
    objD7 = pChar->Possession(D6W);
    if (objD7 == RNnul) continue;
    if (objD7.NameIndex() != objNI_EmptyFlask) continue;
    *pObject = objD7;
    return GetRecordAddressDB8(objD7);
//
  };
  return NULL;
}

void SpellError(const char * /*msg*/)
{
  NotImplemented(0xbc12);
}

void StandardSpellMessage(const char *text, int charIndex=0)
{ // Substitute character name for '%'.
  PrintLinefeed();
  const char *pPercent;
  pPercent = strchr(text,'%');
  if (pPercent != NULL)
  {
    i32 i, len = strlen(text);
    char *newText;
    newText = (char *)UI_malloc(len+20,MALLOC088);
    memcpy(newText, text, pPercent-text);
    for (i=0; i<8; i++)
    {
      if (d.CH16482[charIndex].name[i] == 0) break;
      newText[pPercent-text+i] = d.CH16482[charIndex].name[i];
    };
    strcpy(newText+(pPercent-text)+i, text+(pPercent-text)+1);
    QuePrintLines(4,newText);
    UI_free(newText);
  }
  else
  {
    QuePrintLines(4, text);
  };
}


// ************************************************************
//
// ************************************************************
//  TAG01ca0c
i16 CastSpell(i16 chIdx, SPELL_PARAMETERS& spellParameters)
{//(i16)
  dReg D0, D1, D3, D4, D6;
  i32 spellPower; //1 to 6
  i32 delay;
  SPELL *pSpell;
  CHARDESC *pcA3;
  DB10    *pDB10_24;
  TIMER timer_20;
  DB8      *pDB8_10;
  i16 w_6;
  i16 w_4;
  i8  *incantation;
  i8  overrideIncantation[4];
  i32 facing, skillRequired, missileType, spellByte5;
  i32 spellClass, disableTime;
  RN  obj_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (spellParameters.chIdx != -1) chIdx = (ui16)spellParameters.chIdx;
  const char *msg = NULL;
  switch (spellParameters.action)
  {
  case 0: //Normal processing
    break;
  case 1: //Abort no message
    msg = "";
    break;
  case 2:
    msg = "THAT IS A DIFFICULT SPELL.  STUDY IT.";
    break;
  case 3:
    msg = "THE SPELL FIZZLES.";
    break;
  case 4:
    msg = "THE SPELL FIZZLES AND DIES.";
    break;
  case 5:
    msg = "THIS SPELL IS TOO DIFFICULT.";
    break;
  case 6:
    msg = "% NEEDS MORE PRACTICE WITH THIS SPELL.", chIdx;
    break;
  case 7:
    msg = "THIS SPELL REQUIRES A COMPONENT.";
    break;
  };
  if (msg != NULL)
  {
    msg = TranslateLanguage(msg);
    if (msg[0] != 0)
    {
      StandardSpellMessage(msg, chIdx);
    };
    if (spellParameters.disableTime != -1)
    {
      disableTime = spellParameters.disableTime;
      DisableCharacterAction(chIdx, disableTime);
    };
    return 0;
  };
  if (chIdx >= d.NumCharacter) return 0;

  pcA3 = &d.CH16482[chIdx];
  if (pcA3->HP() == 0) return 0;
  incantation = pcA3->incantation;
  facing = d.partyFacing;
  if (spellParameters.facing != -1)
        facing = spellParameters.facing;
  if ((facing<0) || (facing>3))
  {
    SpellError("Facing");
    return 0;
  };
  if (spellParameters.incantation != -1)
  {
    ui32 n;
    incantation = overrideIncantation;
    n = spellParameters.incantation;
    overrideIncantation[3] = (i8)((n % 10)+0x72-1); n /= 10;
    if (overrideIncantation[3] == 0x71)
          overrideIncantation[3] = 0;
    overrideIncantation[2] = (i8)((n % 10)+0x6c-1); n /= 10;
    if (overrideIncantation[2] == 0x6b)
          overrideIncantation[2] = 0;
    overrideIncantation[1] = (i8)((n % 10)+0x66-1); n /= 10;
    if (overrideIncantation[1] == 0x65)
          overrideIncantation[1] = 0;
    overrideIncantation[0] = (i8)((n % 10)+0x60-1); n /= 10;
    if (overrideIncantation[0] == 0x5f)
          overrideIncantation[0] = 0;
  };
  pSpell = Incantation2Spell(incantation);
  if (pSpell == NULL)
  {
    if (spellParameters.disableTime != -1)
    {
      disableTime = spellParameters.disableTime;
      DisableCharacterAction(chIdx, disableTime);
    };
    SpellErrorMsg(pcA3, 1, 0);
    return 0;
  };
  spellPower = sw(incantation[0] - 95); // Spell power.1 to 6
  if ((spellPower < 1) || (spellPower > 6))
  {
    SpellError("Bad Power");
    return 0;
  };
  skillRequired = pSpell->skillRequired;
  if (spellParameters.skillRequired != -1)
        skillRequired = spellParameters.skillRequired;
  missileType = BITS4_9(pSpell->word6);
  if (spellParameters.missileType != -1)
  {
    missileType = spellParameters.missileType;
  };
  D0L = STRandom() & 7;
  D4W = sw(pSpell->skillRequired + spellPower);
  D0W = sw(D0W + 16 * D4W);
  D1W = sw(spellPower - 1);
  D1W = sw(8 * (D1W * skillRequired));
  D0W = sw(D0W + D1W);
  w_4 = sw(D0W + D4W * D4W);
  spellByte5 = pSpell->byte5;
  if (spellParameters.spellByte5 != -1)
       spellByte5 = spellParameters.spellByte5;
  D6W = (ui16)DetermineMastery(chIdx, spellByte5);
  if (D6W < D4W)
  {
//
    for (w_6=sw(D4W - D6W); w_6--!=0; )
    {
      D0L = STRandom() & 127;
      D1L = Smaller(pcA3->Attributes[Wisdom].Current()+15, 115);
      if (D0W > D1W)
      {
        AdjustSkills(chIdx,             //chIdx
                     spellByte5,        //Which skill
                     w_4 >> (D4W-D6W), //Experience gained
                     ASW_CastSpell1);
        SpellErrorMsg(pcA3, 0, (i16)spellByte5);
        return 0;
      };
    };
  };
  spellClass = pSpell->word6 & 15;
  if (spellParameters.spellClass != -1)
       spellClass = spellParameters.spellClass;
  switch (spellClass)
  {

  case 1: // make potion, for example.  Needs flask
      pDB8_10 = GetFlaskInHand(pcA3, &obj_2);//TAG01c9b2
      if (pDB8_10 == NULL)
      {
        SpellErrorMsg(pcA3, 10, 0);
        return (3);
      };
      D4W = sw(GetObjectWeight(obj_2));
      DEBUGLOAD(pcA3, D4W, -1, obj_2);
      pDB8_10->potionType(POTIONTYPE((pSpell->word6>>4)&0x3f));
      D3W = sw(40*spellPower + STRandom(16));
      D3W &= 255;
      pDB8_10->strength(D3W);
      DEBUGLOAD(pcA3, GetObjectWeight(obj_2), +1, obj_2);
      pcA3->load = sw(pcA3->load + GetObjectWeight(obj_2) - D4W);
      ASSERT(pcA3->load < MAXLOAD,"maxload");
      DrawEightHands();
      if (d.SelectedCharacterOrdinal == chIdx+1)
      {
        pcA3->charFlags |= CHARFLAG_possession;
        DrawCharacterState(chIdx);
      };
      break;

  case 2: // Fireball, for example
      if (d.partyFacing != (UI8)(pcA3->facing))
      {
        pcA3->facing = (ui8)d.partyFacing;
        pcA3->charFlags |= CHARFLAG_positionChanged;
        DrawCharacterState(chIdx);
      };
      if (missileType == 4)
      {
        D6W *= 2;
      };
      {
        RN temp;
        temp.CreateSpell(missileType);
        CharacterThrowsSpell(
                     chIdx,
                     //RN(3,15,BITS4_9(pSpell->word6)+0x380), //0xff80+type
                     temp,
                     ApplyLimits(21, (2*D6W+4)*(spellPower+2),255),
                     0); // required mana
      };
      break;
  case 3: // Light, for example
      timer_20.timerUByte5(0);
      D6W = sw(4*(spellPower + 1));
      switch (missileType)
      {
      case 0: //(3-4-5) OH IR RA Light
          delay = 10000;
          D1W = sw((D6W - 8) * 512);
          delay = sw(delay + D1W);
          D6W /= 2;
          D6W--;
          goto tag01ccca;
      case 5:
          delay = sw(4000 + 128 * (D6W-3));
          D6W = sw(D6W/4 + 1);
tag01ccca:
          ASSERT(D6W < 16,"D6W");
          d.Brightness = sw(d.Brightness + d.Word1074[D6W]);
          SetBrightnessTimer(-D6W, delay); // Set to decrement brightness later
         break;
      case 1: //(5-4-6 spell) DES IR SAR  Darkness
          D6W /= 4;
          d.Brightness = sw(d.Brightness - d.Word1074[D6W]);
          SetBrightnessTimer(D6W, 98);
          break;
      case 2: // 3-2-5  OH EW RA See thru walls
          timer_20.Function(TT_73);
          d.SeeThruWalls++;
          D6W /= 2;
          goto tag01cda4;
      case 3: //(3-2-6) OH EW SAR = invisibility
          timer_20.Function(TT_71);
          d.Invisible++;
          goto tag01cdaa;
      case 4: // 1-4 YA-IR
          timer_20.Function(TT_74);
          timer_20.timerWord6() = D6W;
          if (d.PartyShield > 50)
          {
            timer_20.timerWord6() >>= 2;
          };
          d.PartyShield = sw(d.PartyShield + timer_20.timerWord6());
          MarkAllPortraitsChanged();
          goto tag01cda4;
      case 6: //1-5-2 YA-BRO-ROS
          timer_20.Function(TT_79);
          d.MagicFootprintsActive++;
          d.IndexOfFirstMagicFootprint = d.NumFootprintEnt;
          if (spellPower < 3)
          {
            d.IndexOfLastMagicFootprint = d.IndexOfFirstMagicFootprint;
          }
          else
          {
            d.IndexOfLastMagicFootprint = 0;
          };
tag01cda4:
          D6W = sw(D6W * D6W);
tag01cdaa:
          D0L = d.Time;
          D6L &= 0xffff;
          D0L += D6L;
          //D1L = d.partyLevel << 24;
          //timer_20.timerTime = D1L | D0L;
          timer_20.Time(D0L);
          timer_20.Level((ui8)d.partyLevel);
          gameTimers.SetTimer(&timer_20);
          break;
      case 7:
          obj_2 = FindEmptyDB10Entry(false);
          if (obj_2 == RNnul) break;
          pDB10_24 = GetRecordAddressDB10(obj_2);
          pDB10_24->miscType(misc_ZokathraSpell);
          if (pcA3->Possession(0) == RNnul)
          {
            D6W = 0;
          }
          else
          {
            if (pcA3->Possession(1) == RNnul)
            {
              D6W = 1;
            }
            else
            {
              D6W = -1;
            };
          };
          if ((D6W == 0) || (D6W==1))
          {
            AddCharacterPossession(chIdx, obj_2, D6W);
            DrawCharacterState(chIdx);
          }
          else
          {
            i32 oldLevel;
            oldLevel = d.LoadedLevel;
            LoadLevel(d.partyLevel);
            MoveObject(obj_2, -1, 0, d.partyX, d.partyY, NULL, NULL);
            LoadLevel(oldLevel);
          };
          break;
      case 8: //fireshield
          MagicShield(pcA3, 0, D6W * D6W + 100, 0);
          break;
      default: NotImplemented(0x1ce98);
      }; //switch
  }; // switch
  AdjustSkills(chIdx,          //chIdx
               spellByte5,     //Which skill
               w_4,            //Experience gained
               ASW_CastSpell2);
  disableTime = BITS10_15(pSpell->word6);
  if (spellParameters.disableTime != -1)
         disableTime = spellParameters.disableTime;
  DisableCharacterAction(chIdx, disableTime);
  return (1);
}

void DSACastSpell(bool filter)
{
  int oldLevel;
  SPELL_PARAMETERS spellParameters;
  memcpy(&spellParameters, pDSAparameters+1, sizeof(spellParameters));
  oldLevel = d.LoadedLevel;
  LoadLevel(d.partyLevel);
  if (filter) CallSpellFilter(&spellParameters);
  CastSpell(0, spellParameters);
  LoadLevel(oldLevel);
}
