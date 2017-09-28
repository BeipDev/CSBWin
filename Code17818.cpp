#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

//  TAG015f1e
i32 DetermineMastery(i32 chIdx,i32 skill,const char *traceID)
{
  // if skill & 0x4000 then don't add adjustment for possessions.
  // if skill & 0x8000 then don't add temporary adjustment.
  i32 experience, mastery;
  OBJ_NAME_INDEX objNID4, objNID5;;
  i32 ignorePossessions;
  i32 ignoreTemporaryAdjustments;
  CHARDESC *pChar;
  SKILL *pSkill;
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile), "%sEnter DetermineMastery(hero=%s, skill=%d)\n",
                       traceID, d.CH16482[chIdx].name, skill);
  };
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.PartySleeping!=0)
  {
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%sParty sleeping. Report mastery=1.\n",traceID);
    };
    return 1; // Report no mastery of this skill
  };
  ignoreTemporaryAdjustments = (UI16)(skill&0x8000);
  ignorePossessions = (I16)(skill & 0x4000);
  skill &= 0x3fff;
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile),"%sExtract two flags from skill number.\n",traceID);
    fprintf(GETFILE(TraceFile),"%sflag8000 = skill&0x8000 --> %d\n",traceID,ignoreTemporaryAdjustments);
    fprintf(GETFILE(TraceFile),"%sflag4000 = skill&0x4000 --> %d\n",traceID,ignorePossessions);
    fprintf(GETFILE(TraceFile),"%sSkillNumber = %d\n",traceID,skill);
  };
  pChar = &d.CH16482[chIdx];
  pSkill = &pChar->skills92[skill];
  experience = pSkill->experience;
  if (ignoreTemporaryAdjustments == 0)
  {
    experience += pSkill->tempAdjust;
  };
  if (skill > 3) // If subskill
  {
    pSkill = &pChar->skills92[(skill-4)/4];
    experience += pSkill->experience;
    if (ignoreTemporaryAdjustments == 0)
    {
      experience += pSkill->tempAdjust;
    };
    experience >>= 1;
  };
  for (mastery=1; experience >= 500; mastery++)
  {
    experience >>= 1;
//
  };
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile),"%sMastery = log2(Experience=%d) --> %d\n",
                      traceID, experience, mastery);
  };
  if (ignorePossessions == 0)
  {
    objNID4 = pChar->Possession(1).NameIndex();
    if (objNID4 == objNI_TheFirestaff_a)
    {
      mastery++;
      if (traceID!=NULL)
      {
        fprintf(GETFILE(TraceFile),"%sflag4000==0 and Firestaff_a so increment mastery->%d\n",
                          traceID, mastery);
      };
    }
    else
    {
      if (objNID4 == objNI_TheFirestaff_b)
      {
        mastery+=2;
        if (traceID!=NULL)
        {
          fprintf(GETFILE(TraceFile),"%sflag4000==0 and Firestaff_b so add 2 to mastery -->%d\n",
                             traceID, mastery);
        };
      };
    };
    objNID5 = pChar->Possession(10).NameIndex();
    switch (skill)
    {
    case SKILL_Wizard:
        if (objNID5 == objNI_PendantFeral)
        {
          mastery++;
          if (traceID!=NULL)
          {
            fprintf(GETFILE(TraceFile),"%sflag4000==0 and skill==3 and wearing PendantFeral so Increment mastery -->%d\n",
                               traceID, mastery);
          };
        };
        break;
//
    case 15: //Priestly skill
        if (objNID5 == objNI_EkkhardCross)
        {
          mastery++;
          if (traceID!=NULL)
          {
            fprintf(GETFILE(TraceFile),"%sflag4000==0 and skill=15 and wearing EkkhardCross so increment mastery --> %d\n",
                               traceID, mastery);
          };
        };
        break;
//
    case 13: //Priestly skill
        if (  (objNID5==objNI_GemOfAges)
            ||(objNID4==objNI_SceptreOfLyf))
        {
          if (traceID!=NULL)
          {
            fprintf(GETFILE(TraceFile),"%sflag4000==0 and skill==13 and wearing GemOfAges or using SceptreOfLyf so increment mastery --> %d\n",
                               traceID, mastery);
          };
          mastery++;
        };
        break;
//
    case 14: //Priestly skill
        if (objNID5 == objNI_Moonstone)
        {
          mastery++;
          if (traceID!=NULL)
          {
            fprintf(GETFILE(TraceFile),"%sflag4000==0 and skill==14 and wearing Moonstone so increment mastery --> %d\n",
                               traceID, mastery);
          };
        };
        break;
    };
  };
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile),"%sMastery of skill %d is %d\n",
                      traceID, skill, mastery);
  };
  return mastery;
}

//   TAG017818 //2.5 seconds while sleeping
void TenSecondUpdate()
{
  dReg D0, D1, D3, D4, D5, D6;
  OBJ_NAME_INDEX objNID0;
  i16 *pwA0, *pwA1;
  ATTRIBUTE *ptA2;
  CHARDESC *pcA3;
  i16 LOCAL_26;
  //i16 LOCAL_16;
  i16 LOCAL_6 = 0x2bad; // Party location.
  i16 LOCAL_4;
  i32 random112; //sort of random number 0 to 112; i16 LOCAL_2;
  i32 chIdx;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.NumCharacter == 0) return;
  D3W = (I16)(d.partyX & 31);
  LOCAL_6 = sw((LOCAL_6 & 0xffe0) | D3W);
  D3W = (I16)(d.partyY & 31);
  LOCAL_6 = sw((LOCAL_6 & 0xfc1f) | (D3W << 5));
  LOCAL_6 = sw((LOCAL_6 & 0x03ff) | ((d.partyLevel&63)<<10));

  for (D6W=0; D6W<d.NumFootprintEnt-1; D6W++)
  {
    pwA1 = &d.PartyFootprints[D6W];
    pwA0 = &LOCAL_6;
    D0L = 0;
    //This does nothing that I can see.
    //SearchWordNE(&D0, &pwA0, pwA1);
    D0W = sw((UI8)(d.Byte13220[D6W]) - 1);
    D0W = sw(Larger(0, D0W));
    d.Byte13220[D6W] = D0B;
    if (D0B != 0) continue;
    if (D6W != 0) continue;
    DeleteFootprintEntry(0);
    break;

  };
// OKKKKKKKKKKKK
  //D7L = d.Time;
  //D0W = (I16)(d.Time & 0x80);
  //D1W = (I16)((d.Time & 0x100) >> 2);
  //D0W = sw(D0W + D1W);
  //D1W = (I16)((d.Time & 0x40) << 2);
  //D0W = sw(D0W + D1W);
  //LOCAL_2 = sw(D0W >> 2);
  random112 = (d.Time&0x40)|((d.Time&0x80)>>2)|((d.Time&0x100)>>4);
  //D7W = 0;
  //pcA3 = &d.CH16482;

  for (chIdx=0; chIdx < d.NumCharacter; chIdx++)
  {
    pcA3 = &d.CH16482[chIdx];
    if (pcA3->HP() == 0) continue;
    if (chIdx + 1 == d.PotentialCharacterOrdinal) continue;
    if (pcA3->Mana() < pcA3->MaxMana())
    {
      i32 magicMastery, magicMasteryPlusWisdom;
      magicMastery = DetermineMastery(chIdx, SKILL_Wizard) + DetermineMastery(chIdx, SKILL_Priest);
      magicMasteryPlusWisdom = magicMastery + pcA3->Attributes[Wisdom].Current();
      //D0L = random112; // Reversed 3 bits of time
      if (random112 >= magicMasteryPlusWisdom)// goto tag0179b0;
      {
        if (pcA3->Mana() > pcA3->MaxMana())
        {
          pcA3->IncrMana(-1);
        };
      }
      else
      {
        i32 manaAdjust, staminaAdjust;
        manaAdjust = (pcA3->MaxMana()) / 40;
        if (d.PartySleeping)
        {
          manaAdjust *= 2;
        };
        manaAdjust++;
        staminaAdjust = manaAdjust * Larger(7, 16-magicMastery);
        if (TimerTraceActive)
        {
          fprintf(GETFILE(TraceFile), "10sUpdate name=%s, mana=%d, maxMana=%d, manaAdjust=%d, stamina=%d, staminaAdjust=%d\n",
            pcA3->name, pcA3->Mana(), pcA3->MaxMana(), manaAdjust, pcA3->Stamina(), staminaAdjust);
        };

        AdjustStamina(chIdx, staminaAdjust);
        pcA3->Mana(pcA3->Mana() + Smaller(manaAdjust, pcA3->MaxMana()-pcA3->Mana()));
      };
    }
    else
    {
//tag0179b0:
      if (pcA3->Mana() > pcA3->MaxMana())
      {
        pcA3->IncrMana(-1);
      };
    };

    for (D4W=19; D4W>=0; D4W--)
    {
      if (pcA3->skills92[D4W].tempAdjust > 0)
      {
        pcA3->skills92[D4W].tempAdjust--;
      };
//
    };
    for (D6W=4, D4W=pcA3->MaxStamina();
         D4W>>=1, LOCAL_26=D4W, pcA3->Stamina()<LOCAL_26;
         D6W+=2)
    {

    };
    D4W = 0;
    // Limit to between 1 and 6
    LOCAL_4 = ApplyLimits(1, (pcA3->MaxStamina()>>8)-1, 6);
    if (d.PartySleeping)
    {
      LOCAL_4 <<= 1;
    };
    D5W = (i16)(d.Time-d.LastPartyMoveTime);
    if (D5W > 80)
    {
      LOCAL_4++;
      if (D5W > 250)
      {
        LOCAL_4++;
      };
    };
    do
    {
      D5W = (I16)((D6W <= 4) ? 1 : 0);
      if (pcA3->food < -512)
      {
        if (D5W)
        {
          D4W = sw(D4W + LOCAL_4);
          pcA3->food -= 2;
        };
      }
      else
      {
        if (pcA3->food >= 0)
        {
          D4W = sw(D4W - LOCAL_4);
        };
        if (D5W)
        {
          D0W = 2;
        }
        else
        {
          D0W = sw(D6W / 2);
        };
        pcA3->food = sw(pcA3->food - D0W);
      };
      if (pcA3->water < -512)
      {
        if (D5W)
        {
          D4W = sw(D4W + LOCAL_4);
          pcA3->water--;
        };
      }
      else
      {
        if (pcA3->water >= 0)
        {
          D4W = sw(D4W - LOCAL_4);
        };
        if (D5W)
        {
          D0W = 1;
        }
        else
        {
          D0W = sw(D6W / 2);
        };
        pcA3->water = sw(pcA3->water - D0W);
      };
      D6W--;
      if (D6W == 0) break;
    } while (pcA3->Stamina() < pcA3->MaxStamina() + D4W);

    AdjustStamina(chIdx, D4W);
    if (pcA3->food < -1024)
    {
      pcA3->food = -1024;
    };
    if (pcA3->water < -1024)
    {
      pcA3->water = -1024;
    };
    if (pcA3->HP() < pcA3->MaxHP())
    {
      D0W = pcA3->Stamina();
      D1W = sw(pcA3->MaxStamina() / 4);
      if (D0W >= D1W)
      {
        D0L = random112;
        D1W = sw(pcA3->Attributes[Vitality].Current() + 12);
        if (D0UW < D1UW)
        {
          LOCAL_4 = sw((pcA3->MaxHP() >> 7) + 1);
          if (d.PartySleeping)
          {
            LOCAL_4 = sw(2 * LOCAL_4);
          };
          objNID0 = pcA3->Possession(10).NameIndex(); //necklace
          if (objNID0 == objNI_EkkhardCross)
          {
            LOCAL_4 = sw(LOCAL_4 + (LOCAL_4 / 2) + 1);
          };
          D0W = sw(Smaller(LOCAL_4, pcA3->MaxHP()-pcA3->HP()));
          pcA3->HP(pcA3->HP() + D0W);
        };
      };
    };
    if (d.Word11750)
    {
      d.Word11750--;
      if (d.Word11750 == 0)
      {
        d.CanRestartFromSavegame = 0;
        DamageAllCharacters(0x1000, 0, 0);
      };
    };
    D0L = d.Time;
    if (d.PartySleeping)
    {
      D1W = 63;
    }
    else
    {
      D1W = 255;
    };
    if ((D0W & D1W) == 0)
    {
      for (D6W=0; D6W<=6; D6W++)
      {
        ptA2 = &pcA3->Attributes[D6W];
        D5W = ptA2->Maximum();
        D0W = ptA2->Current();
        if (D0W < D5W)
        {
          ptA2->IncrCurrent(1);
        }
        else
        {
          D0W = ptA2->Current();
          if (D0W > D5W)
          {
            D0L = ptA2->Current() / ((D5W==0)?1:D5W);
            ptA2->IncrCurrent(-D0W);
          };
        };
// continue

      };
    };
    if (d.PartySleeping == 0)
    {
      D0W = (UI8)(pcA3->facing);
      if (D0W != d.partyFacing)
      {
        D0L = d.LastMonsterAttackTime;
        D1L = d.Time - 60; // About 10 seconds
        if (D0L < D1L)
        {
          pcA3->facing = (ui8)d.partyFacing;
          pcA3->maxRecentDamage = 0;
          pcA3->charFlags |= CHARFLAG_positionChanged;
        };
      };
    };
    pcA3->charFlags |= CHARFLAG_statsChanged;
    D0W = sw(chIdx + 1);
    D1W = d.SelectedCharacterOrdinal;
    if (D0W == D1W)
    {
      if (   (d.PressingMouth)
          || (d.PressingEye)
          || (d.DisplayResurrectChestOrScroll==0) )
      {
        pcA3->charFlags |= CHARFLAG_chestOrScroll;
      };
    };
// continue here
// *or* continue here

  };
  DrawAllCharacterState();

}
