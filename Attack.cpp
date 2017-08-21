#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

extern ui32 *pDSAparameters;
extern bool overlayActive;

//#define pauseAwhile(milliseconds) {};
//#define pauseAwhile(milliseconds) UI_Sleep(milliseconds)

#define MS2VBL(milliseconds) ((milliseconds)/16)

static i32 testdelay = 500;

//  TAG00ac9e
i16 DetermineAttackOrdinal(
              i32 attackX,
              i32 attackY,
              i32 partyX,
              i32 partyY,
              i32 charPos)
{
  dReg D6;
  RN  objD5;
  DB4 *DB4A3;
  ui8  attackedAbsPos[4];
  i32 i, pos;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  objD5 = FindFirstMonster(attackX, attackY);
  if (objD5 == RNeof)
  {
    return 0;
  };
  DB4A3 = GetRecordAddressDB4(objD5);
  DetermineAttackOrder(attackedAbsPos,
                       attackX, attackY,
                       partyX, partyY, charPos);
  for (i=0;;i++)
  {
    pos = attackedAbsPos[i];

    D6W = sw(OrdinalOfMonsterAtPosition(DB4A3, pos));
    if (D6W != 0) return D6W;
  };
}

//*********************************************************
//
//*********************************************************
//  TAG00f058
bool IsCellFluxcage(i32 mapX, i32 mapY)
{
  dReg  D0;
  ROOMTYPE rtD6;
  RN  objD7;
  DB15  *DB15A0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  rtD6 = (ROOMTYPE)(GetCellFlags(mapX, mapY) >> 5);//room type
  if ((rtD6==roomSTONE) || (rtD6==roomSTAIRS))
  {
    return false;
  };
  for (objD7 = FindFirstObject(mapX, mapY);
       objD7 != RNeof;
       objD7 = GetDBRecordLink(objD7) )
  {
    if (objD7.dbType() != dbCLOUD) continue;
    DB15A0 = GetRecordAddressDB15(objD7);
    D0W = sw(DB15A0->cloudType());
    if (D0W == CT_FLUXCAGE) return true;
//
//
  };
  return false;
}

//*********************************************************
//
//*********************************************************
// TAG00f0e8
RN IsLordChaosHere(i32 mapX, i32 mapY)
{
  RN objD7;
  DB4 *DB4A3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  objD7 = FindFirstMonster(mapX, mapY);
  if (objD7 == RNeof) return RNempty;
  DB4A3 = GetRecordAddressDB4(objD7);
  if (DB4A3->monsterType() != mon_LordChaos) return RNempty;
  return objD7;
}

bool legalXY(i32 x, i32 y)
{
  if ( (y < 0) || (y >= d.height) ) return false;
  if ( (x < 0) || (x >= d.width)  ) return false;
  return true;
};

bool IsCellWall(i32 x, i32 y)
{
  i32 cf;
  cf = (GetCellFlags(x, y) & 255) >> 5;
  return (cf==roomSTONE) || (cf==roomFALSEWALL);
}

bool IsCellOpenOrPit(i32 x, i32 y)
{
  i32 cf;
  cf = (GetCellFlags(x, y) & 255) >> 5;
  return   (cf == roomOPEN) ||(cf == roomPIT);
}

bool IsCellOccupied(i32 x, i32 y)
{
  if (FindFirstMonster(x, y) != RNeof) return true;
  if (   (d.LoadedLevel == d.partyLevel)
      && (d.partyX == x)
      && (d.partyY == y) ) return true;
  return false;
}

//*********************************************************
//
//*********************************************************
//   TAG00f340
RESTARTABLE _Fusion(const i32 attackX, const i32 attackY)
{//(void)
  static dReg D0, D4, D5, D6, D7;
  static RN  obj_16;
  static i32 i_14[4];
  static i16 w_6;
  static i16 w_4;
  static i16 w_2;
  i32 i, j, temp;
  i32 direction[4], x1[4], y1[4], x2[4], y2[4];
  bool fluxCageOrWall[4], empty1[4], empty2[4];
  RESTARTMAP
    RESTART(3)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(attackX);
  D6W = sw(attackY);
  if (D7W < 0) RETURN;
                 //I think this is what the programmer
                 //wanted.  But it appears that the variable
                 //was unsigned and therefore impossible to
                 //be less than zero.
  if (D7W >= d.width) RETURN;
  if (D6W < 0) RETURN;
  if (D6W >= d.height) RETURN;
  CreateCloud(RN(RNDispellMissile), 0xff, D7W, D6W, D7W, D6W, 0xff, NULL);
  obj_16 = IsLordChaosHere(D7W, D6W);
  if (obj_16 == RNempty) RETURN;
#define NewFusion
#ifdef NewFusion
  for (i=0; i<4; i++) direction[i] = i; // Initially north, east, south, west
  // Now scramble the four directions
  for (i=3; i>0; i--)
  {
    j = STRandom(i+1);
    temp = direction[i];
    direction[i] = direction[j];
    direction[j] = temp;
  };
  for (i=0; i<4; i++) //for each direction in scrambled order
  {
    x1[i] = D7W + d.DeltaX[direction[i]];
    y1[i] = D6W + d.DeltaY[direction[i]];
    x2[i] = D7W + 2*d.DeltaX[direction[i]];
    y2[i] = D6W + 2*d.DeltaY[direction[i]];
    fluxCageOrWall[i]
      = IsCellFluxcage(x1[i], y1[i]) || IsCellWall(x1[i], y1[i]);
    empty1[i] =   IsCellOpenOrPit(x1[i], y1[i])
               && !IsCellOccupied(x1[i], y1[i]);
    empty2[i] =   legalXY(x2[i], y2[i])
               && IsCellOpenOrPit(x2[i], y2[i])
               && !IsCellOccupied(x2[i], y2[i])
               && !IsCellFluxcage(x2[i], y2[i]);
  };
  // Now try to move Lord Chaos to an immediately adjacent cell.
  for (i=0; i<4; i++)
  {
    if (!fluxCageOrWall[i] && empty1[i])
    {
      D0W = MoveObject(obj_16, D7W, D6W, x1[i], y1[i], NULL, NULL);
      if (D0W == 0)
      {
        StartMonsterMovementTimers(x1[i], y1[i]);
        RETURN;
      };
    };
  };
  // If no fluxcage or wall is in the way try moving him two squares.
  for (i=0; i<4; i++)
  {
    if (!fluxCageOrWall[i] && empty2[i])
    {
      D0W = MoveObject(obj_16, D7W, D6W, x2[i], y2[i], NULL, NULL);
      if (D0W == 0)
      {
        StartMonsterMovementTimers(x2[i], y2[i]);
        RETURN;
      };
    };
  };
  // Sorry, Mr. Chaos.  You are toast.
#else  // OldFusion
  i_14[0] = IsCellFluxcage(D7W-1, D6W) ? 1 : 0;
  i_14[1] = IsCellFluxcage(D7W+1, D6W) ? 1 : 0;
  i_14[2] = IsCellFluxcage(D7W, D6W-1) ? 1 : 0;
  i_14[3] = IsCellFluxcage(D7W, D6W+1) ? 1 : 0;
  for (w_2 = sw(i_14[0] + i_14[1] + i_14[2] + i_14[3]);
       w_2 < 4;
       w_2++) // Examine each non-fluxcage cells in random order.
  {
    D5W = D7W;
    D4W = D6W;
    w_4 = (i16)STRandom0_3();
    for (w_6 = 4; w_6!=0; w_6--)
    { // Look in four directions from Lord Chaos starting with random direction in w_4.
      D0L = i_14[w_4];
      if (D0W == 0)
      {
        i_14[w_4] = 1;
        switch (w_4)
        {
        case 0: D5W--; break; //west
        case 1: D5W++; break; //east
        case 2: D4W--; break; //north
        case 3: D4W++; break; //south
        }; //switch
        break;
      };
      w_4 = sw((w_4 + 1) & 3);
//
    };
    if (OpenTeleporterPitOrDoor(D5W, D4W))
    if ( (D5W != d.partyX) || (D4W != d.partyY) )
    if (FindFirstMonster(D5W, D4W) == RNeof) // Don't put Lord Chaos on top
                                             // of another monster!
    {
      D0W = MoveObject(obj_16, D7W, D6W, D5W, D4W);
      if (D0W == 0)
      {
        StartMonsterMovementTimers(D5W, D4W);
        RETURN;
      };
    };
//
  };
#endif // NewFusion
  FusionSequence(_3_); //TAG01fefc
  RETURN;
}


enum ATTACKDATATYPE
{
  ADT_WarCry     =  1,
  ADT_Physical   =  2,
  ADT_Spell      =  3,
  ADT_HitDoor    =  4,
  ADT_Shoot      =  5,
  ADT_Flip       =  6,
  ADT_Shield     =  7,
  ADT_FluxCage   =  8,
  ADT_Fusion     =  9,
  ADT_Heal       = 10,
  ADT_Window     = 11,
  ADT_ClimbDown  = 12,
  ADT_FreezeLife = 13,
  ADT_Light      = 14,
  ADT_Throw      = 15,
  ADT_Default    = 16  //Block, Hit
};

struct SHIELD // dataType = ADT_Shield
{
// atk_SPELLSHIELD
// atk_FIRESHIELD
  i32 mustHaveMana;
  i32 strength;
};

struct FLIP // dataType = ADT_Flip
{
// atk_FLIP
  i32 heads;
};

struct SHOOT //dataType = ADT_Shoot
{
// atk_SHOOT
  i32 success;
  i32 range;
  i32 damage;
  i32 decayRate;
};


struct HITDOOR //dataType = ADT_HitDoor
{
//  atk_BASH:
//  atk_HACK: 
//  atk_BERZERK: 
//  atk_KICK: 
//  atk_SWING: 
//  atk_CHOP: 
  i32 strength;
};


struct WARCRYETC //dataType = ADT_WarCry
// atk_CONFUSE:
// atk_WARCRY:
// atk_CALM:
// atk_BRANDISH:
// atk_BLOWHORN:
{
  i32 mastery;
  i32 skillIncrement;
  i32 effectiveMastery;
  i32 requiredMastery;
};

struct PHYSICALATTACK //dataType = ADT_Physical
{
//  atk_BASH:
//  atk_HACK:
//  atk_BERZERK:
//  atk_KICK:
//  atk_SWING:
//  atk_CHOP:
//  atk_DISRUPT:
//  atk_JAB:
//  atk_PARRY:
//  atk_STAB2:
//  atk_STAB1:
//  atk_STUN:
//  atk_THRUST:
//  atk_MELEE:
//  atk_SLASH:
//  atk_CLEAVE:
//  atk_PUNCH:
  i32 monsterDamage;
  i32 staminaAdjust;
  i32 skillAdjust;
  i32 attackedMonsterOrdinal;
};


struct SPELLATTACK //dataType = ADT_Spell
{
//  atk_LIGHTNING:
//  atk_DISPELL:
//  atk_FIREBALL:
//  atk_SPIT:
//  atk_INVOKE
  i32 spellRange;
  i32 spellType;
  i32 decrementCharges; // if non-zero (default = 1)
};

struct HEAL
{
//  atk_HEAL
  i32 HPIncrement;
};

struct FREEZELIFE
{
  i32 oldTime;
  i32 deltaTime;
};

struct LIGHT
{
  i32 deltaLight;
  i32 decayRate;
  i32 time;
};

struct THROW
{
  i32 side;
  i32 abort;
  i32 facing;
  i32 range;
  i32 damage;
  i32 decayRate;
};

union ATTDEP
{
  WARCRYETC      warcryetc;
  PHYSICALATTACK physicalAttack;
  SPELLATTACK    spellAttack;
  HITDOOR        hitDoor;
  SHOOT          shoot;
  FLIP           flip;
  SHIELD         shield;
  HEAL           heal;
  FREEZELIFE     freezeLife;
  LIGHT          light;
  THROW          thro;  // Cannot say 'throw'; it is reserved!
};

struct ATTACKPARAMETERS
{
  i32    charIdx;                           //pre-attack
  i32    attackType;                        //pre-attack
  i32    attackX;                           //pre-attack
  i32    attackY;                           //pre-attack
  i32    monsterUnderAttack; //0 if none    //pre-attack
  i32    monsterType;                       //pre-attack
  i32    skillNumber;                       //pre-attack
  i32    staminaCost;                       //pre-attack
  i32    experienceGained;                  //pre-attack
  i32    disableTime;                       //pre-attack
  i32    neededMana;                        //pre-attack
  i32    unused; //damageToMonster;
  i32    decrementCharges;
  i32    activateMonster;
  i32    userInfo[10];
  i32    dataType; // = ADT_*****
  ATTDEP attdep;
  //ATTACKPARAMETERS(void){memset(this, 0, sizeof(*this));};
};


struct FILTER
{
  LOCATIONREL locrFilter;
  TIMER timer;
  RN  DSAobj;
};


void PartyAttackFilterError(ui8& flag, const char* msg)
{
  char message[500];
  if (flag != 0) return;
  strcpy(message, "Party attack Filter error\n");
  strcat(message, msg);
  strcat(message, "We will try to proceed quietly.");
  UI_MessageBox(message, "Party Attack Error",MESSAGE_OK);
  flag = 1;
}

void CallAttackFilter(FILTER *pFilter, ATTACKPARAMETERS *pParam, i32 msg)
{
  i32 currentLevel;
  DSAVARS dsaVars;
  static ui8 characterMsg = 0;
  if (pFilter->locrFilter.l < 0) return;
  currentLevel = d.LoadedLevel;
  if (pFilter->locrFilter.l != d.LoadedLevel)
  {
    LoadLevel(pFilter->locrFilter.l);
  };
  pFilter->timer.timerUByte9((ui8)msg);
  ProcessDSAFilter(pFilter->DSAobj, &pFilter->timer, pFilter->locrFilter, NULL, &dsaVars);
  if (pParam->charIdx >= d.NumCharacter)
  {
    PartyAttackFilterError(characterMsg,"Set illegal character index");
    pParam->charIdx = 0;
  };
  if (currentLevel != d.LoadedLevel)
  {
    LoadLevel(currentLevel);
  };
}



void PhysicalAttackFilter(ATTACKPARAMETERS *pParam, FILTER *pFilter, const char *traceID)
{
  MONSTER_DAMAGE_RESULT monsterDamageResult;
  RN objMon;
  pParam->dataType = ADT_Physical;
  CallAttackFilter(pFilter, pParam, 1);
  if (pParam->monsterUnderAttack == 0) return;
  objMon.ConstructFromInteger(pParam->monsterUnderAttack);
  if (pParam->attdep.physicalAttack.staminaAdjust != 0)
  {
    AdjustStamina(pParam->charIdx,
                  pParam->attdep.physicalAttack.staminaAdjust);
  };
  if (pParam->attdep.physicalAttack.skillAdjust != 0)
  {
    AdjustSkills(pParam->charIdx, 
                 pParam->skillNumber, 
                 pParam->attdep.physicalAttack.skillAdjust,
                 ASW_PhysicalAttack);
  };
  monsterDamageResult = DamageMonster(GetRecordAddressDB4(objMon),//DB4A2, 
                      pParam->attdep.physicalAttack.attackedMonsterOrdinal-1, //monsterPosIndex, 
                      pParam->attackX, 
                      pParam->attackY, 
                      pParam->attdep.physicalAttack.monsterDamage, //D7W, 
                      1, 
                      false,
                      NULL);
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile),"%sw_2 = DamageMonster() --> %d\n",
                       traceID,
                       pParam->attdep.physicalAttack.monsterDamage);
  };
  if (monsterDamageResult != NO_MONSTER_DIED) // If one or more of the monster group died.
  {
    pParam->activateMonster = 1;
    //ProcessTimers29to41(pParam->attackX, 
    //                    pParam->attackY, 
    //                    TT_M1, 
    //                    0);
  };
  
}

//*********************************************************
// Called when we hit monster with sword
//*********************************************************
//  TAG00f930
i32 DeterminePhysicalAttackDamage(
              ATTACKPARAMETERS *pParam,
              FILTER *pFilter,
              //CHARDESC *pChar,       
              //i32      chIdx,
              //DB4      *pMonster,
              i16      monsterPosIndex,
              //i32      attackX,
              //i32      attackY,
              i16      P7,
              i16      P8,
              //i32      skillNumber,
              bool     vorpalOrDisrupt,
              const char     *traceID)
{
  dReg          D0, D1, D3, D4, D6, D7;
  CHARDESC     *pchA3;
  RN            objMon;
  //DB4          *DB4A2;
  MONSTERDESC  *pmtDesc;
  OBJ_NAME_INDEX    objNI_attackWeapon;
  //i16           w_2;
  bool          luckyHit = false;
  i32           throwingDistance;
  //i32           StaminaDec;
  i32           ranResult;
  i32           levelDifficulty;
//  D6L = 0x1ccccccc; // Avoid compiler warning.
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile),
          "%sEntering DeterminePhysicalAttackDamage(char=%d,monster,P4=%d,P7=%d,P8=%d,skillNumber=%d)\n",
          traceID, pParam->charIdx, monsterPosIndex, P7, P8, pParam->skillNumber);
  };
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pchA3 = &d.CH16482[pParam->charIdx];
  objMon.ConstructFromInteger(pParam->monsterUnderAttack);
  //DB4A2 = pMonster;
  if (pParam->charIdx >= d.NumCharacter)
  {
    PhysicalAttackFilter(pParam, pFilter, traceID);
    return 0;
  };
  if (pchA3->HP() == 0)
  {
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%sCharacter is dead.  Return 0\n",traceID);
    };
    PhysicalAttackFilter(pParam, pFilter, traceID);
    return 0;
  };
  levelDifficulty = d.pCurLevelDesc->experienceMultiplier();
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile),"%slevelDifficulty = %d\n",
                      traceID, levelDifficulty);
  };
  pmtDesc = &d.MonsterDescriptor[GetRecordAddressDB4(objMon)->monsterType()];
  objNI_attackWeapon = pchA3->Possession(1).NameIndex(); //weapon hand
  // We modified the calling sequence to separate the
  // vorpalOrDisrupt parameter from parameter P7
  if (   (!pmtDesc->nonMaterial())
      || vorpalOrDisrupt  )
  {
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%spi26 word2 &0x40 == 0  or  vorpalOrDisrupt\n",traceID);
    };
    D0L = Quickness(pchA3,nextTracePrefix(traceID));
    D1L = STRandom(32);
    D3W = pmtDesc->dexterity12; // uByte8[4];
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%scharacter's quickenss = %d\n",traceID,D0L);
      fprintf(GETFILE(TraceFile),"%srequired quickness = (pi26.uByte8[4]=%d) + (random(32)=%d) + (levelDifficulty=%d) - 16 = %d\n",
                               traceID, D3W, D1W, levelDifficulty, D1W+D3W+levelDifficulty-16);
    };
    D1W = sw(D1W + D3W + levelDifficulty - 16);

    if (   (D0W <= D1W)       // if quickness too small
        && ((luckyHit = (STRandom0_3() == 0)) == false)   // and no luck
        && !IsCharacterLucky(pchA3, 75 - P7, nextTracePrefix(traceID))  )
    {
      if (traceID!=NULL)
      {
        fprintf(GETFILE(TraceFile),"%sAttack failed.  Quickness too small. Luck(1/4) failed.  TAG16476(75-(P7=%d) returned false.  Return 0\n",traceID, P7);
      };
      //AdjustStamina(pParam->charIdx, StaminaDec=STRandomBool() + 2);
      pParam->attdep.physicalAttack.staminaAdjust = 2 + STRandomBool();
      PhysicalAttackFilter(pParam, pFilter, traceID);
      if (traceID!=NULL)
      {
        fprintf(
          GETFILE(TraceFile),
          "%sDecrementStamina(2+random(1)=%d)\n",
          traceID,
          pParam->attdep.physicalAttack.staminaAdjust);
        fprintf(GETFILE(TraceFile),"%sPhysicalAttack Returning 0\n",traceID);
      };
      DrawCharacterState(pParam->charIdx);
      return 0;
    };
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%sAttack was successful because ",traceID);
      if (D0W>D1W)
      {
        fprintf(GETFILE(TraceFile), "Character's quickness is great enough\n");
      }
      else if (luckyHit)
      {
        fprintf(GETFILE(TraceFile),"Lucky Hit (1 chance in 4)\n");
      }
      else
      {
        fprintf(GETFILE(TraceFile),"IsCharacterLucky returned true\n");
      };
    };
      // It is good to get here, I think
    throwingDistance = DetermineThrowingDistance(pParam->charIdx, 1);
    D7W = sw(throwingDistance);
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile), "%sD7W = Throwing distance=%d\n",traceID,D7W);
    };
    if (throwingDistance != 0)  /////goto tag00faa2;
    {
      D7W = sw(D7W + (STRandom() & 0xffff) % (D7W/2 + 1));
      if (traceID!=NULL)
      {
        fprintf(GETFILE(TraceFile),"%sD7W = D7W+random(D7W/2+1) = %d\n",traceID,D7W);
      };
      D7W = sw(D7W * P8 / 32);
      if (traceID!=NULL)
      {
        fprintf(GETFILE(TraceFile),"%sD7W = D7W*(P8=%d)/32 = %d\n",traceID,P8,D7W);
      };
      D4W = sw((ranResult=STRandom(32)) + pmtDesc->defense08 + levelDifficulty);
      if (traceID!=NULL)
      {
        fprintf(GETFILE(TraceFile),"%sD4W = (i26->uByte8[0]=%d) + (levelDifficulty=%d) + (random(32)=%d) = %d\n",
                     traceID,pmtDesc->defense08,levelDifficulty,ranResult,D4W);
      };
      if (objNI_attackWeapon == objNI_DiamondEdge)
      {
        D4W = sw(D4W - D4W/4);
        if (traceID!=NULL)
        {
          fprintf(GETFILE(TraceFile),"%sDiamond Edge so subtract 25%% from D4W --> %d\n",traceID,D4W);
        };
      }
      else
      {
        if (objNI_attackWeapon == objNI_Executioner)
        {
          D4W = sw(D4W - D4W/8);
          if (traceID!=NULL)
          {
            fprintf(GETFILE(TraceFile),"%sExecutioner so subtract 12.5%% from D4W --> %d\n",traceID,D4W);
          };
        };
      };
    // D4 is perhaps like the monster's Armor effectiveness???
      D7W = sw(D7W + (ranResult=STRandom(32)) - D4W);
      D6W = D7W;  // chance of hitting?????
      if (traceID!=NULL)
      {
        fprintf(GETFILE(TraceFile),"%sD7W = D7W + (random(32)=%d) - (D4W=%d) = %d\n",
                           traceID,ranResult, D4W, D7W);
        fprintf(GETFILE(TraceFile),"%sD6W = D7W = %d\n", traceID, D6W);
      };
    }
    else
    {///  }}}}}}
      D6W = 0; //Added 23Jun2004.  Tiggy 'Bashing' with a stone 
               //club resulted in throwingDistance==0 and D6W 
               //never got set.
    };
    if ((throwingDistance==0)||(D6W <= 1))
    {
      if (traceID!=NULL)
      {
        fprintf(GETFILE(TraceFile),"%sAttack may fail because ",traceID);
        if (throwingDistance==0)
        {
          fprintf(GETFILE(TraceFile), "throwingDistance = 0.\n");
        }
        else
        {
          fprintf(GETFILE(TraceFile),"D6W < 2.\n");
        };
      };
      D7W = (i16)STRandom0_3();
      if (traceID!=NULL)
      {
        fprintf(GETFILE(TraceFile),"%sD7W = random(4) = %d\n",traceID,D7W);
      };
      if (D7W == 0)
      {
        if (traceID!=NULL)
        {
          fprintf(GETFILE(TraceFile),"%sAttack failed because ",traceID);
          fprintf(GETFILE(TraceFile)," we were not Lucky (D7W==0)\n");
        };
        //AdjustStamina(pParam->charIdx, StaminaDec=STRandomBool() + 2);
        pParam->attdep.physicalAttack.staminaAdjust = 2 + STRandomBool();
        PhysicalAttackFilter(pParam, pFilter, traceID);
        if (traceID!=NULL)
        {
          fprintf(GETFILE(TraceFile),
                  "%sDecrementStamina(2+random(1)=%d)\n",
                  traceID,
                  pParam->attdep.physicalAttack.staminaAdjust);
          fprintf(GETFILE(TraceFile),"%sPhysicalAttack Returning 0\n",traceID);
        };
        DrawCharacterState(pParam->charIdx);
        return 0;
      };
      if (traceID!=NULL)
      {
        fprintf(GETFILE(TraceFile),"%sWe were lucky.  D7W is non-zero\n",traceID);
      };
      D1W = sw(STRandom(16));
      //ASSERT(D6L != 0x1ccccccc);
      D6W = sw(D6W + D1W);
      if (traceID!=NULL)
      {
        fprintf(GETFILE(TraceFile),"%sD6W += (random(16)=%d) --> %d\n",
                           traceID, D1W, D6W);
      };
      if (   (D6W > 0)
          || (STRandomBool() != 0)  )
      {
        if (traceID!=NULL)
        {
          fprintf(GETFILE(TraceFile),"%sWe are going to recompute D7W because ",traceID);
          if (D6W>0)
          {
            fprintf(GETFILE(TraceFile), "D6W > 0\n");
          }
          else
          {
            fprintf(GETFILE(TraceFile),"we got lucky (1/2)\n");
          };
        };
        D7W = sw(D7W + (ranResult=STRandom0_3()));
        if (traceID!=NULL)
        {
          fprintf(GETFILE(TraceFile),"%sD7W += (random(3)=%d) --> %d\n",
                            traceID, ranResult, D7W);
        };
        if (STRandom0_3() == 0)
        {
          if (traceID!=NULL)
          {
            fprintf(GETFILE(TraceFile),"%sWe are going to recompute D7W because we got lucky(1/4)\n",traceID);
          };
          D7W = sw(D7W + Larger(0, (ranResult=STRandom(16)) + D6W));
          if (traceID!=NULL)
          {
            fprintf(GETFILE(TraceFile),"%sD7W += max((D6W=%d),(random(16)=%d))-->%d\n",
                                traceID,D6W, ranResult,D7W);
          };
        };
      };
    };
    D7W /= 2;
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%sDivide D7W by 2 --> %d\n",traceID,D7W);
    };
    if (D7W == 0) D0W = 0;
    else D0W = sw((STRandom() & 0xffff) % D7W);
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%sD0W=random(D7W) --> %d\n", traceID, D0W);
    };
    D1W = (i16)(ranResult=STRandom0_3());
    D7W = sw(D7W + D1W + D0W);
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile), "%sD7W += (random(4)=%d) + (D0W=%d) --> %d\n",
                          traceID, ranResult, D0W, D7W);
    };
    if (D7W != 0) D7W = sw(D7W + (ranResult=(STRandom() & 0xffff) % D7W));
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile), "%sD7W += (random(D7W)=%d)--> %d\n",
                          traceID, ranResult, D7W);
    };
    D7W /= 4;
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%sDivide D7W by 4 --> %d\n", traceID, D7W);
    };
    D7W = sw(D7W + (ranResult=STRandom0_3()) + 1);
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%sD7W += (random(4)=%d) + 1 --> %d\n",
                                traceID, ranResult, D7W);
    };
//
//
    if (traceID!=NULL)
    {
      if (    (objNI_attackWeapon == objNI_VorpalBlade)
           && !pmtDesc->nonMaterial() )
      {
        fprintf(GETFILE(TraceFile),"%sDivide D7W by 2 because VorpalBlade and i26 Word2 bit 6 = 0 --> %d\n",
                           traceID, D7W/2);
      };
    };
    if (   (objNI_attackWeapon == objNI_VorpalBlade  )
        && (!pmtDesc->nonMaterial()  )
        && ((D7W = sw(D7W/2)) == 0)  )
    {
      if (traceID!=NULL)
      {
        fprintf(GETFILE(TraceFile),"%sAttack failed because Vorpal, i26 word 2 bit 6=0, and D7W=0\n",traceID);
      };
      //AdjustStamina(pParam->charIdx, StaminaDec=STRandomBool() + 2);
      pParam->attdep.physicalAttack.staminaAdjust = 2 + STRandomBool();
      PhysicalAttackFilter(pParam, pFilter, traceID);
      if (traceID!=NULL)
      {
        fprintf(GETFILE(TraceFile),
                "%sDecrementStamina(2+random(1)=%d)\n",
                traceID,
                pParam->attdep.physicalAttack.staminaAdjust);
        fprintf(GETFILE(TraceFile),"%sPhysicalAttack Returning 0\n",traceID);
      };
      DrawCharacterState(pParam->charIdx);
      return 0;
    };
    D0W = sw((ranResult=STRandom(64)));
    D1W = sw(DetermineMastery(pParam->charIdx, pParam->skillNumber, traceID==NULL?NULL:traceID-2));
    if (D0W < D1W)
    {
      D7W += 10;
      if (traceID!=NULL)
      {
        fprintf(GETFILE(TraceFile),"%sAdd 10 to D7W because (Mastery(skill=%d)=%d) > (random(64)=%d) --> %d\n",
                           traceID,pParam->skillNumber,D1W, ranResult, D7W);
      };
    };
    if (D7W < 0) D7W = 0;
    pParam->attdep.physicalAttack.monsterDamage = D7W;
    //w_2 = DamageMonster(GetRecordAddressDB4(objMon), 
    //                    monsterPosIndex, 
    //                    pParam->attackX, 
    //                    pParam->attackY, 
    //                    D7W, 
    //                    1, 
    //                    false);
    D0W = sw((D7W * pmtDesc->word16_8_11() / 16) + 3);
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%sD0W = ((D7W=%d) * (Bits8_11(i26.word16)=%d)/16) + 3 --> %d\n",
                         traceID,D7W, pmtDesc->word16_8_11(),D0W);
    };
    //AdjustSkills(pParam->charIdx, pParam->skillNumber, D0W);
    pParam->attdep.physicalAttack.skillAdjust = D0W;
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%sAdjustSkills((skill=%d), (D0W=%d))\n",
                         traceID,pParam->skillNumber, D0W);
    };
    //AdjustStamina(pParam->charIdx, (ranResult=STRandom0_3()) + 4);
    pParam->attdep.physicalAttack.staminaAdjust = 4 + STRandom0_3();
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%sDecrement Stamina by random(4)+4 = %d\n",
                         traceID, pParam->attdep.physicalAttack.staminaAdjust);
    };
    PhysicalAttackFilter(pParam, pFilter, traceID);
  }
  else
  { // Non-material and Not (Vorpal or Disrupt)
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%spi26 word2 &0x40 != 0  and  w_4 == 0\n",traceID);
    };
    D7W = 0;
    pParam->attdep.physicalAttack.monsterDamage = D7W; //Should already be zero but
                                                       //let us be explicit.
    //w_2 = 0;
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%sD7W = 0\n+++w_2 = 0\n",traceID);
    };
    //AdjustStamina(pParam->charIdx, (ranResult=STRandomBool()) + 2);
    pParam->attdep.physicalAttack.staminaAdjust = 2 + STRandomBool();
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%sDecrement Stamina by random(2)+2 = %d\n",
                         traceID, pParam->attdep.physicalAttack.staminaAdjust);
    };
    PhysicalAttackFilter(pParam, pFilter, traceID);
  };
  DrawCharacterState(pParam->charIdx);
  //if (w_2 != 0)
  //{
  //  ProcessTimers29to41(pParam->attackX, pParam->attackY, TT_M1, 0);
  //};
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile),"%sPhysicalAttack returning D7W = %d\n",traceID,D7W);
  };
  return pParam->attdep.physicalAttack.monsterDamage;
}


//*********************************************************
//
//*********************************************************
//   TAG01adaa
RESTARTABLE _FlashAttackDamage(const i32 value)
{//(void)
  static dReg D0, D5, D6, D7;
  static const char* A2;
  static aReg A3;
  static RectPos *pR_10;
  static i8 b_6[6];
  static i32 damage;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
  damage = value;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  STHideCursor(HC1);
  d.UseByteCoordinates = 0;
  FillRectangle(d.LogicalScreenBase,
                (RectPos *)d.Word20,
                0,
                160);
  if (damage < 0)
  {
    if (damage == -1)
    {
      D5W = 242;
      A2 = "CAN'T REACH";
    }
    else
    {
      D5W = 248;
      A2 = "NEED AMMO";
    };
    TextOutToScreen(D5W, 100, 4, 0, A2, true);
  }
  else
  {
    if (damage > 40)
    {
      pR_10 = &d.wRectPos20202;
      A3 = (i8 *)GetBasicGraphicAddress(14);
      D6W = 48;
    }
    else
    {
      if (damage > 15)
      {
        D7W = 2;
        D5W = 64;
        D6W = 32;
        pR_10 = &d.wRectPos20226;
      }
      else
      {
        D7W = 3;
        D5W = 42;
        D6W = 24;
        pR_10 = &d.wRectPos20234;
      };
      D0W = AllocateDerivedGraphic(D7W);
      if (D0W == 0)
      {
        A2 = (char *)GetBasicGraphicAddress(14);
        A3 = (aReg)GetDerivedGraphicAddress(D7W);
        ShrinkBLT((ui8 *)A2,
                  (ui8 *)A3,
                  96,
                  45,
                  D5W,
                  37,
                  d.IdentityColorMap);
        //TAG022d5e(D7W);
      }
      else
      {
        A3 = (aReg)GetDerivedGraphicAddress(D7W);
      };
    };
    BLT2Screen((ui8 *)A3, pR_10, D6W, -1);
    D7W = 5;
    D5W = 274;
    b_6[5] = 0;
    do
    {
    D0W = sw((damage % 10) + '0');
    D7W--;
    b_6[D7W] = D0B;
    D5W -= 3;
    damage = sw(damage/10);
    } while (damage != 0);
    TextOutToScreen(D5W, 100, 4, 0, (char *)&b_6[D7W]);
  };
  VBLDelay(_1_,15);
  STShowCursor(HC1);
  RETURN;
}

//*********************************************************
//
//*********************************************************
//  TAG01bc2a
i16 AttackWithPhysicalForce(
              ATTACKPARAMETERS *pParam,
              FILTER *pFilter,
              //i32 chIdx,
              //CHARDESC *pChar,
              //ATTACKTYPE attackType,
              //i32 attackX,
              //i32 attackY,
              //i32 skillNumber,
              const char *traceID)
{//(i16)
  dReg D0, D6, D7;
  //i32 attackedMonsterOrdinal;
  OBJ_NAME_INDEX objNID0;
  bool vorpalOrDisrupt;
  D6L = 0xccccc;
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile),
            "%sEnter AttackWithPhysicalForce (attackType=%d, skillNumber=%d)\n",
            traceID, pParam->attackType, pParam->skillNumber);
  };
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  QueueSound(16, d.partyX, d.partyY, 1);
  if (d.MonsterUnderAttack == RNeof)
  {
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%sNo monster in attacked square.  Return 0\n",traceID);
    };
    PhysicalAttackFilter(pParam, pFilter, traceID);
    return 0;
  };
  pParam->attdep.physicalAttack.monsterDamage = 0;
  pParam->attdep.physicalAttack.staminaAdjust = 0;
  pParam->attdep.physicalAttack.skillAdjust   = 0;
  pParam->attdep.physicalAttack.attackedMonsterOrdinal = DetermineAttackOrdinal(
                              pParam->attackX, pParam->attackY,
                              d.partyX, d.partyY,
                              d.CH16482[pParam->charIdx].charPosition);
  if (pParam->attdep.physicalAttack.attackedMonsterOrdinal==0)
  {
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%sNo monster can be attacked. Return 0.\n",traceID);
    };
    PhysicalAttackFilter(pParam, pFilter, traceID);
    return 0;
  };
  D0W = sw((d.CH16482[pParam->charIdx].charPosition + 4 - d.CH16482[pParam->charIdx].facing) & 3);
  D6W = 0;
  switch (D0W)
  {
  case 2:
    D6W = 2;
    // Note fallthrough
  case 3:
    D6W++;
    D0W = sw(CharacterAtPosition((d.CH16482[pParam->charIdx].charPosition + D6W) & 3));
    if (D0W != -1)
    {
      d.attackDamageToDisplay = -1; //Can't Reach
      if (traceID!=NULL)
      {
        fprintf(GETFILE(TraceFile),"%sAttempt to attack through party member.  Return 0\n",traceID);
      };
      PhysicalAttackFilter(pParam, pFilter, traceID);
      return 0;
    };
  }; // switch (D0W)
  if (pParam->attackType == atk_DISRUPT)
  {
    if (!d.MonsterUnderAttack.NonMaterial())
    {
      if (traceID!=NULL)
      {
        fprintf(GETFILE(TraceFile),
              "%sDISRUPT but monsterDesc bit 6 word2 = 0. Return 0\n",traceID);
      };
      PhysicalAttackFilter(pParam, pFilter, traceID);
      return 0;
    };
  };
  D6W = (UI8)(d.Byte20002[pParam->attackType]);
       //Lots of zeroes with scattered 20s, 30s, 40s
       //block,chop,punch,kick,stab1,hit,swing,stab2,thrust,
       //jab,parry,hack,berzerk,disrupt,melee,slash,
       //cleave,bash,stun.  Appears to be things that
       //require strength or physical action.
  D7W = (UI8)(d.Byte19958[pParam->attackType]);
       //Scattered non-zero values.  Block, chop, punch,
       //kick, stab, hit, swng, stab, thrust, jab, parry,
       //hack, berzerk, disrupt, melee, slach, cleave,
       //bash, stun,
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile),"%sD6W=Byte20002[attackType=%d]=%d\n",
                      traceID, pParam->attackType, D6W);
    fprintf(GETFILE(TraceFile),"%sD7W=Byte19958[attackType=%d]=%d\n",
                      traceID, pParam->attackType, D7W);
  };
  objNID0 = d.CH16482[pParam->charIdx].Possession(1).NameIndex();
  vorpalOrDisrupt = false;
  if ( (objNID0==objNI_VorpalBlade) || (pParam->attackType==atk_DISRUPT) )
  {
    vorpalOrDisrupt = true;
    if (traceID!=NULL)
    {
      fprintf(GETFILE(TraceFile),"%sVorpalBlade or DISRUPT.\n",traceID);
    };
  };
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile), "%sCalling DeterminePhysicalAttackDamage(character, monster, D5W-1, D6W, D7W, skillNumber)\n",traceID);
  };
  D0L = DeterminePhysicalAttackDamage(
            pParam,
            pFilter,
            //&d.CH16482[pParam->charIdx],
            //pParam->charIdx,
            //GetRecordAddressDB4(d.MonsterUnderAttack),
            sw(pParam->attdep.physicalAttack.attackedMonsterOrdinal-1),
            //pParam->attackX,
            //pParam->attackY,
            D6W,
            D7W,
            //pParam->skillNumber,
            vorpalOrDisrupt,
            nextTracePrefix(traceID));
  d.attackDamageToDisplay = D0W;
  if (traceID!=NULL)
  {
    fprintf(GETFILE(TraceFile),"%sWord20264 = Result of calling DeterminePhysicalAttackDamage = %d\n", traceID, D0W);
    fprintf(GETFILE(TraceFile),"%sReturn 1\n",traceID);
  };
  return (1);
}

//*********************************************************
//
//*********************************************************
//  TAG01bd5e
i16 MagicShield(CHARDESC *pChar, i32 spellShield, i32 strength, i32 mustHaveMana)
{
  dReg D6;
  TIMER timer;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7W = sw(P3);
  D6W = 1;
  if (mustHaveMana != 0)
  {
    if (pChar->Mana() == 0) return 0;
    if (pChar->Mana() < 4)
    {
      strength /= 2;
      pChar->Mana(0);
      D6W = 0;
    }
    else
    {
      pChar->IncrMana(-4);
    };
  };
  timer.timerWord6() = sw(strength / 32);
  if (spellShield != 0)
  {
    timer.Function(TT_77);
    if (d.SpellShield > 50)
    {
      timer.timerWord6() /= 4;
    };
    d.SpellShield = sw(d.SpellShield + timer.timerWord6());
  }
  else
  {
    timer.Function(TT_78);
    if (d.FireShield > 50)
    {
      timer.timerWord6() /= 4;
    };
    d.FireShield = sw(d.FireShield + timer.timerWord6());
  };
  timer.timerUByte5(0);
  //timer.timerTime = (d.Time+(UI16)(strength)) | (d.partyLevel << 24);
  timer.Time(d.Time+(UI16)(strength));
  timer.Level((ui8)d.partyLevel);
  //timer.uByte6 = (ui8)timer.word6(); // Value expected in first byte
  // Not So!!!  We use the whole word.
  gameTimers.SetTimer(&timer);
  MarkAllPortraitsChanged();
  return D6W;
}

//*********************************************************
//
//*********************************************************
//   TAG01be82
void DecrementChargesRemaining(CHARDESC *pChar)
{//(void)
  RN   objD7;
  DB5  *DB5A3;
  DB6  *DB6A3;
  DB10 *DB10A3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  objD7 = pChar->Possession(1);
  //dbA3 = GetRecordAddress(objD7);
  switch (objD7.dbType())
  {
  case dbWEAPON:
      DB5A3 = GetRecordAddressDB5(objD7);
      if (DB5A3->charges() != 0)
      {
        //D3W = BITS10_13(DB4A3->word(2)) - 1;
        DB5A3->charges(DB5A3->charges() - 1);
      };
      break;
  case dbCLOTHING:
      DB6A3 = GetRecordAddressDB6(objD7);
      if (DB6A3->charges() != 0)
      {
        //D3W = BITS9_12(DB6A3->word(2)) - 1;
        DB6A3->charges(DB6A3->charges() - 1);
      };
      break;
  case dbMISC:
      DB10A3 = GetRecordAddressDB10(objD7);
      if (DB10A3->value() != 0)
      {
        //D3W = BITS14_15(DB10A3->word(2)) - 1;
        DB10A3->value(DB10A3->value() - 1);
      };
      break;
  }; //switch(db)
  DrawEightHands();
}

//*********************************************************
//
//*********************************************************
//   TAG01bf70
void SetCharToPartyFacing(CHARDESC *pChar)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (pChar->facing != d.partyFacing)
  {
    pChar->facing = ub(d.partyFacing);
    pChar->charFlags |= CHARFLAG_positionChanged;
  };
}


//*********************************************************
//
//*********************************************************
i32 AttackWithSpell(//i32   chIdx,
                    ATTACKPARAMETERS *pParam,
                    FILTER *pFilter,
                    i32   range,
                    RN    rnD5, 
                    i32   neededMana, 
                    i32&  experienceGained)
{
  i32 success;
  RN objSpell;
  SetCharToPartyFacing(&d.CH16482[pParam->charIdx]);
  //ASSERT(D4L != 0xccccc); // Uninitialized value
  if (d.CH16482[pParam->charIdx].Mana() < neededMana)
  {
    //range = Larger(2, (d.CH16482[chIdx].Mana() * range) / neededMana);
    range = range * d.CH16482[pParam->charIdx].Mana() / neededMana;
    if (range < 2) range = 2;
    neededMana = d.CH16482[pParam->charIdx].Mana();
  };
  ASSERT(range != 0xccc,"range not defined"); //uninitialized value
  pParam->dataType = ADT_Spell;
  pParam->attdep.spellAttack.spellRange = range;
  pParam->attdep.spellAttack.spellType = rnD5.ConvertToInteger();
  pParam->neededMana = neededMana;
  pParam->attdep.spellAttack.decrementCharges = 1;
  CallAttackFilter(pFilter, pParam, 1);
  objSpell = (RNVAL)pParam->attdep.spellAttack.spellType;
  success = CharacterThrowsSpell(
                pParam->charIdx, 
                objSpell,//rnD5, 
                pParam->attdep.spellAttack.spellRange,
                pParam->neededMana);
  if (success == 0)
  {
      experienceGained /= 2;
  };
  if (pParam->attdep.spellAttack.decrementCharges != 0)
  {
    DecrementChargesRemaining(&d.CH16482[pParam->charIdx]);
  };
  return success;
}


ATTACKPARAMETERS *SetupDSAParam(FILTER *pFilter)
{
  RN obj;
  i32 key;
  i32 len;
  ui32 *pRecord;
  i32 objectDB, actuatorType;
  DB3 * pActuator;
  LOCATIONREL locr;
  ATTACKPARAMETERS *pParam;
  //
  //
  pParam = (ATTACKPARAMETERS *)(pDSAparameters+1);
  pDSAparameters[0] = sizeof(ATTACKPARAMETERS);
  memset(pDSAparameters+1, 0, sizeof (*pParam));
  // See if there is a Party Attack Filter
  key = (EDT_SpecialLocations<<24)|ESL_PARTYATTACKFILTER;
  len = expool.Locate(key,&pRecord);
  pFilter->locrFilter.l = -1;
  if (len > 0)
  {
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
          pFilter->timer.timerUByte9(0);   //timerFunction
          pFilter->timer.timerUByte8(0);   //timerPosition
          pFilter->timer.timerUByte7((ui8)locr.y);
          pFilter->timer.timerUByte6((ui8)locr.x);
          //pFilter->timer.timerTime   = locr.l << 24;
          pFilter->timer.Time(0);
          pFilter->timer.Level((ui8)locr.l);
          pFilter->DSAobj = obj;
          pFilter->locrFilter = locr;
          break;
        };
      };
    };
  };
  return pParam;
}



//*********************************************************
//  case  atk_WARCRY:
//  case  atk_CALM:
//  case  atk_BRANDISH:
//  case  atk_BLOWHORN:
//  case  atk_CONFUSE
//*********************************************************
//  TAG01bac2
i32 WarCryEtc(ATTACKPARAMETERS *pParam,
              FILTER *pFilter,
              i32 attackX,
              i32 attackY,
              const char *prefix)
{//(i16)
  dReg        D0, D1;
  i32         success;
  //i32         skillIncrement;
  //i32         requiredMastery;
  DB4         *DB4A3;
  MONSTERDESC *pmtDesc;
  //i32         effectiveMastery;
  ITEM16      *pi16_4;
  i32         Mastery;
  i32         temp;
  pParam->dataType = ADT_WarCry;
  pParam->attdep.warcryetc.skillIncrement = 0xccccc;
  pParam->attdep.warcryetc.requiredMastery = 0xccccc;
  Mastery = 0xccccc;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  success = 0;
  if (AttackTraceActive)
  {
    fprintf(GETFILE(TraceFile),
            "%sEntering 'WarCryEtc' for skill 14(WARCRY, CALM, BRANDISH, BLOWHORN).  Setting D4W = %d\n",
            prefix,
            success);
  };
  if (d.MonsterUnderAttack == RNeof) 
  {
    CallAttackFilter(pFilter, pParam, 1); // We promised to always call
    return 0;
  };
  switch (pParam->attackType)
  {
  case atk_WARCRY:
    pParam->attdep.warcryetc.mastery = 3;
    pParam->attdep.warcryetc.skillIncrement = 12;
    
    if (AttackTraceActive)
    {
      fprintf(GETFILE(TraceFile),
             "%sAttack = Warcry....Set Mastery = %d, Set skillIncrement = %d\n",
             prefix,
             pParam->attdep.warcryetc.mastery,
             pParam->attdep.warcryetc.skillIncrement);
    };
    break;
  case atk_CALM:
    pParam->attdep.warcryetc.mastery = 7;
    pParam->attdep.warcryetc.skillIncrement = 35;
    break;
  case atk_BRANDISH:
    pParam->attdep.warcryetc.mastery = 6;
    pParam->attdep.warcryetc.skillIncrement = 30;
    break;
  case atk_BLOWHORN:
    pParam->attdep.warcryetc.mastery = 6;
    pParam->attdep.warcryetc.skillIncrement = 20;
    if (AttackTraceActive)
    {
      fprintf(GETFILE(TraceFile),
              "%sAttack = BlowHorn....Set Mastery = %d, Set skillIncrement = %d\n",
              prefix,
              pParam->attdep.warcryetc.mastery,
              pParam->attdep.warcryetc.skillIncrement);
    };
    break;
  case atk_CONFUSE:
    pParam->attdep.warcryetc.mastery = 12;
    pParam->attdep.warcryetc.skillIncrement = 45;
    break;
  }; // switch ()
  ASSERT(pParam->attdep.warcryetc.mastery != 0xccccc,"pParam->attdep");
  pParam->attdep.warcryetc.mastery = sw(pParam->attdep.warcryetc.mastery + sw(temp = DetermineMastery(pParam->charIdx, 14)));
  if (AttackTraceActive)
  {
    fprintf(GETFILE(TraceFile),
           "%sIncrement Mastery by (DetermineMastery(14)=%d) --> %d\n",
           prefix,
           temp,
           pParam->attdep.warcryetc.mastery);
  };
  DB4A3 = GetRecordAddressDB4(d.MonsterUnderAttack);
  pmtDesc = &d.MonsterDescriptor[DB4A3->monsterType()];
  pParam->attdep.warcryetc.effectiveMastery = sw((STRandom()&0xffff) % pParam->attdep.warcryetc.mastery);
  pParam->attdep.warcryetc.requiredMastery = pmtDesc->bravery();
  if (AttackTraceActive)
  {
    fprintf(GETFILE(TraceFile),
            "%seffectiveMastery = Random(Mastery) --> %d\n",
            prefix,
            pParam->attdep.warcryetc.effectiveMastery);
    fprintf(GETFILE(TraceFile),
            "%srequiredMastery = i26.bravery() --> %d\n",
            prefix,
            pParam->attdep.warcryetc.requiredMastery);
  };
  CallAttackFilter(pFilter, pParam, 1);
  if ( (pParam->attdep.warcryetc.requiredMastery > pParam->attdep.warcryetc.effectiveMastery) || (pParam->attdep.warcryetc.requiredMastery == 15) )
  {
    ASSERT(pParam->attdep.warcryetc.skillIncrement != 0xccccc,"skillIncrement");
    pParam->attdep.warcryetc.skillIncrement /= 2;
    if (AttackTraceActive)
    {
      fprintf(GETFILE(TraceFile),
              "%sAttack Failed and Divide skillIncrement by 2 because ",
              prefix);
      if (pParam->attdep.warcryetc.requiredMastery > pParam->attdep.warcryetc.effectiveMastery)
            fprintf(GETFILE(TraceFile),"requiredMastery > effectiveMastery\n");
      else fprintf(GETFILE(TraceFile), "requiredMastery == 15\n");
    };
  }
  else
  {
    ASSERT (DB4A3->groupIndex() < d.MaxITEM16,"GroupIndex too big");
    pi16_4 = &d.Item16[DB4A3->groupIndex()];
    if (DB4A3->fear() == StateOfFear6)
    {
      ClearAttacking_DeleteMovementTimers(pi16_4, attackX, attackY);
      StartMonsterMovementTimers(attackX, attackY);
    };
    DB4A3->fear(StateOfFear5);
    D0L = 0;
    D0W = sw(4 * (16-pParam->attdep.warcryetc.requiredMastery));
    D1L = 0; D1B = pmtDesc->timePerMove();//uByte6;
    D0W = sw(D0L / D1W);
    pi16_4->uByte5 = D0B;
    success = 1;
  };
  AdjustSkills(pParam->charIdx, 14, pParam->attdep.warcryetc.skillIncrement, ASW_WarCryEtc);
  return success;
}


//*********************************************************
//
//*********************************************************
//   TAG00f182
void FluxCage(i32 mapX,i32 mapY, ATTACKPARAMETERS *pParam)
{
  i32 LCX, LCY;
  i32 cageCount;
  ROOMTYPE rt;
  //dReg D0, D1, D4;
  //DB15 *DB15A0;
  RN rnD0, rnD5;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  rt = (ROOMTYPE)((GetCellFlags(mapX, mapY) >> 5) & 7);
  if ( (rt != roomSTONE) && (rt != roomSTAIRS) )
  {
    rnD5 = CreateFluxcage(mapX, mapY);
    if (rnD5 != RNnul)
    {
//      rnD0 = IsLordChaosHere(mapX, mapY-1);
//      if (rnD0 != RNempty)
//      {
//        D4L = IsCellFluxcage(mapX+1, --mapY) ? 1 : 0;
//      }
//      else
//      {
//        rnD0 = IsLordChaosHere(mapX-1, mapY);
//        if (rnD0 == RNempty) goto tag00f2ae;
//        D4L = IsCellFluxcage(--mapX, mapY+1) ? 1 : 0;
//      };
//      D0L = IsCellFluxcage(mapX, mapY-1) ? 1 : 0;
//      D1L = IsCellFluxcage(mapX-1, mapY) ? 1 : 0;
//      D4W = sw(D4W + D0W + D1W);
//      goto tag00f322;
//tag00f2ae:
//      rnD0 = IsLordChaosHere(mapX+1, mapY);
//      if (rnD0 != RNempty)
//      {
//        D4L = IsCellFluxcage(++mapX, mapY-1) ? 1 : 0;
//      }
//      else
//      {
//        rnD0 = IsLordChaosHere(mapX, mapY+1);
//        if (rnD0 == RNempty) goto tag00f320;
//        D4L = IsCellFluxcage(mapX-1, ++mapY) ? 1 : 0;
//      };
//      D0L = IsCellFluxcage(mapX, mapY+1) ? 1 : 0;
//      D1L = IsCellFluxcage(mapX+1, mapY) ? 1 : 0;
//      D4W = sw(D4W + D0W + D1W);
//      goto tag00f322;
//tag00f320:
//      D4W = 0;
//tag00f322:
//      if (D4W == 2)
//      {
//        ProcessTimers29to41(mapX, mapY, TT_M3, 0);
//      };
      if (    (IsLordChaosHere(LCX=mapX, LCY=mapY-1) != RNempty)
           || (IsLordChaosHere(LCX=mapX+1, LCY=mapY) != RNempty)
           || (IsLordChaosHere(LCX=mapX, LCY=mapY+1) != RNempty)
           || (IsLordChaosHere(LCX=mapX-1, LCY=mapY) != RNempty) )
      {
        cageCount =   IsCellFluxcage(LCX, LCY-1)
                    + IsCellFluxcage(LCX+1, LCY)
                    + IsCellFluxcage(LCX, LCY-1)
                    + IsCellFluxcage(LCX-1, LCY);
        if (cageCount == 3) 
        {
          pParam->activateMonster = 1;
          pParam->attackX = LCX;
          pParam->attackY = LCY;
        };
      };
    };
  };
}



//*********************************************************
//
//*********************************************************
//  TAG01bf9a
RESTARTABLE _Attack(const i32 initialChIdx, const ATTACKTYPE initialAttackType)
{//(i16)
  static dReg D0, D4, D6;
  static ROOMTYPE rtD0;
  static RN   rnD5;
  static CHARDESC *pChar;
  static DBCOMMON *dbA2;
  static DB5      *DB5A2;
  static DB10     *DB10A2;
  //static i32 attackX, attackY;
  static i16   w_42;
  static TIMER timer_32;
  static WEAPONDESC  *pWeapon_22;
  static WEAPONDESC  *pWeapon_18;
  //static i32 skillNumber; //w_12
  // w_8 = attackY
  // w_6 = attackX
  //static i32 disableTime, staminaCost, experienceGained;
  static i32 range;
  static i32 successfulAttack;
  static RN  obj_4;
  static i32 wpnByte1_4;
  static i32 neededHP_4;
  //static i32 neededMana;
  static ATTACKPARAMETERS *pParam;
  static FILTER filter;
  RESTARTMAP
    RESTART(8)
  END_RESTARTMAP
  range = 0xccc;//Compiler says not initialized.
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7W = sw(chIdx);




  if (initialChIdx >= d.NumCharacter)
  {
    RETURN_i16(0);
  };
  pChar = &d.CH16482[initialChIdx];
  // Weapon hand = 1;
  dbA2 = GetCommonAddress(pChar->Possession(1));
  if (pChar->HP() == 0) RETURN_i16(0);

  pParam = SetupDSAParam(&filter);
  pParam->charIdx = initialChIdx;
  pParam->attackType = initialAttackType;
  pParam->neededMana = 0xccccc;

  pParam->attackX = d.partyX;
  pParam->attackY = d.partyY;
  pParam->attackX += d.DeltaX[pChar->facing];
  pParam->attackY += d.DeltaY[pChar->facing];
  if (TimerTraceActive || AttackTraceActive)
  {
    fprintf(GETFILE(TraceFile),"%s attacks from %02x(%02x,%02x)"
                      " to %02x(%02x,%02x)\n",
            d.CH16482[pParam->charIdx].name,
            d.partyLevel,d.partyX,d.partyY,
            d.partyLevel,pParam->attackX, pParam->attackY);
  };
  if (AttackTraceActive)
  {
    i32 i;
    i = pParam->attackType;
    char *pAName;
    for (pAName = (char *)d.AttackNames; i != 0; i--)
    {
      while ( *(pAName++) != 0) {}; // Skip one string
    };

    fprintf(GETFILE(TraceFile),"Attack type = %d = %s\n",
                  pParam->attackType, pAName);
  };
  d.MonsterUnderAttack = FindFirstMonster(pParam->attackX, pParam->attackY);
  pParam->monsterUnderAttack = d.MonsterUnderAttack.ConvertToInteger();
  if (pParam->monsterUnderAttack != RNeof)
  {
    pParam->monsterType = GetRecordAddressDB4(d.MonsterUnderAttack)->monsterType();
  }
  else
  {
    pParam->monsterUnderAttack = 0;
    pParam->monsterType = mon_undefined;
  };
  pParam->disableTime = (UI8)(d.Byte19914[pParam->attackType]); // small integer (2,6,12)
  pParam->skillNumber = (UI8)(d.SkillNumber[pParam->attackType]);
        //small intgers (largest 18)
  if (TimerTraceActive || AttackTraceActive)
  {
    fprintf(GETFILE(TraceFile), "Skill number required = %02d\n", pParam->skillNumber);
  };
  pParam->staminaCost = (UI8)(d.Byte20046[pParam->attackType]) + STRandomBool();
        //small integers mostly 2,4,3 largest 40
  if (AttackTraceActive)
  {
    fprintf(GETFILE(TraceFile),"Byte20046[attackType=%d] = %d\n",
                      pParam->attackType, d.Byte20046[pParam->attackType]);
    fprintf(GETFILE(TraceFile),"staminaCost = Byte20046[attackType=%d] + Random(1) = %d\n",
                      pParam->attackType, pParam->staminaCost);
  };
  pParam->experienceGained = (UI8)(d.experienceForAttacking[pParam->attackType]);
        //small integers.  largest 35.
  if (AttackTraceActive)
  {
    fprintf(GETFILE(TraceFile), "experiencedGained = Byte20178[attackType=%d] = %d\n",
                        pParam->attackType, pParam->experienceGained);
  };
  pParam->activateMonster = 0;
  D6W = GetCellFlags(pParam->attackX, pParam->attackY);
  successfulAttack = 1;
//
  if (   (pParam->skillNumber == 16)  // All the Wizard skills
      || (pParam->skillNumber == 17)
      || (pParam->skillNumber == 18)
      || (pParam->skillNumber == 19)
      || (pParam->skillNumber ==  3) )
  {
    pParam->neededMana = sw(7-Smaller(6, DetermineMastery(pParam->charIdx,         //chIdx
      pParam->skillNumber,initialTracePrefix(AttackTraceActive))));
    if (AttackTraceActive)
    {
      fprintf(GETFILE(TraceFile),"Skill %d is Wizard skill.  Mastery=%d\n",
            pParam->skillNumber, DetermineMastery(pParam->charIdx,pParam->skillNumber));
      fprintf(GETFILE(TraceFile),"neededMana = 7-min(6,Mastery) = %d\n",pParam->neededMana);
    };
  }
  else
  {
    pParam->neededMana = 0;
  };
  CallAttackFilter(&filter, pParam, 0);
  if (pParam->attackType < 0) RETURN_i16(0);
  switch (pParam->attackType)
  {
  case atk_LIGHTNING:  //PAF
      range = 180;
      rnD5 = RNLightning;
      successfulAttack = AttackWithSpell(pParam, &filter, range, rnD5, pParam->neededMana, pParam->experienceGained);
      break;
  case atk_DISPELL: //PAF
      range = 150;
      rnD5 = RNDispellMissile;
      successfulAttack = AttackWithSpell(pParam, &filter, range, rnD5, pParam->neededMana, pParam->experienceGained);
      break;
  case atk_FIREBALL: //PAF
      range = 150;
      rnD5 = RNFireball;
      successfulAttack = AttackWithSpell(pParam, &filter, range, rnD5, pParam->neededMana, pParam->experienceGained);
      break;
  case atk_SPIT: //PAF
      range = 250;
      rnD5 = RNFireball;
      successfulAttack = AttackWithSpell(pParam, &filter, range, rnD5, pParam->neededMana, pParam->experienceGained);
      break;
  case atk_BASH: //PAF
  case atk_HACK: //PAF
  case atk_BERZERK: //PAF
  case atk_KICK: //PAF
  case atk_SWING: //PAF
  case atk_CHOP: //PAF
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile), "Attack BASH/HACK/BERZERK/KICK/SWING/CHOP\n");
    }
    if (RoomType(D6W) == roomDOOR) //
    {
      if ((D6W & 7) == 4) // Is it a breakable door?
      {
        QueueSound(16, d.partyX, d.partyY, 1);
        pParam->dataType = ADT_HitDoor;
        pParam->disableTime = 6;
        pParam->attdep.hitDoor.strength = DetermineThrowingDistance(pParam->charIdx, 1);
        CallAttackFilter(&filter, pParam, 1);
        HitDoor(pParam->attackX,
                pParam->attackY,
                pParam->attdep.hitDoor.strength,
                0,  //Not a ZO spell
                2); //delay
        QueueSound(4, d.partyX, d.partyY, 2);
        break;
      };
    };
    // NOTE!  Fallthrough.
  case atk_DISRUPT://PAF
  case atk_JAB://PAF
  case atk_PARRY://PAF
  case atk_STAB2://PAF
  case atk_STAB1://PAF
  case atk_STUN://PAF
  case atk_THRUST://PAF
  case atk_MELEE://PAF
  case atk_SLASH://PAF
  case atk_CLEAVE://PAF
  case atk_PUNCH://PAF
      successfulAttack = AttackWithPhysicalForce(
                            pParam,
                            &filter,
                            //pParam->charIdx,
                            //pChar,
                            //(ATTACKTYPE)pParam->attackType,
                            //pParam->attackX,
                            //pParam->attackY,
                            //pParam->skillNumber,
                            initialTracePrefix(AttackTraceActive));
      if (successfulAttack == 0)
      {
        pParam->experienceGained /= 2;
        pParam->disableTime /= 2;
      };
      break;
  case atk_CONFUSE://PAF
      DecrementChargesRemaining(pChar);
      // *** NOTE   Fallthrough  *****
  case  atk_WARCRY://PAF
  case  atk_CALM://PAF
  case  atk_BRANDISH://PAF
  case  atk_BLOWHORN://PAF
    successfulAttack = WarCryEtc(pParam, &filter,pParam->attackX, pParam->attackY,initialTracePrefix(AttackTraceActive));
    break;

  case atk_SHOOT: //PAF
      pParam->dataType = ADT_Shoot;
      pParam->attdep.shoot.success = 0;
      if (    (pChar->Possession(0) == RNnul)
           || (pChar->Possession(0).dbType() != dbWEAPON) )
      {
        d.attackDamageToDisplay = -2;// Need Ammo(unsigned)0xfffe;
        pParam->experienceGained = 0;
        successfulAttack = 0;
        CallAttackFilter(&filter, pParam, 1);
        break;
      };

      //I guess the following is safe.  We could not have
      //selected 'SHOOT' if we did not have the proper
      //weapon in the weapon hand.
      DB5A2 = dbA2->CastToDB5(); //We will crash if not weapon
      pWeapon_18 = &d.weapons[DB5A2->weaponType()];
      pWeapon_22 = TAG0099d2(pChar->Possession(0));
      D4W = pWeapon_18->uByte1;
      wpnByte1_4 = pWeapon_22->uByte1;
      if ( (D4W >= 16) && (D4W <= 31) )
      {
        if (wpnByte1_4 != 10)
        {
          d.attackDamageToDisplay = -2;//Need Ammo (unsigned)0xfffe;
          pParam->experienceGained = 0;
          successfulAttack = 0;
          CallAttackFilter(&filter, pParam, 1);
          break;
        };
        D4W -= 16;
      }
      else
      {
        if ( (D4W >= 32) && (D4W <= 47) )
        {
          if (wpnByte1_4 != 11)
          {
            d.attackDamageToDisplay = -2; //Need Ammo  (unsigned)0xfffe;
            pParam->experienceGained = 0;
            successfulAttack = 0;
            CallAttackFilter(&filter, pParam, 1);
            break;
          };
          D4W -= 32;
        };
      };
      SetCharToPartyFacing(pChar);
      obj_4 = RemoveCharacterPossession(pParam->charIdx, 0);
      QueueSound(16, d.partyX, d.partyY, 1);
      w_42 = sw(DetermineMastery(pParam->charIdx, 11));
      pParam->attdep.shoot.range = pWeapon_18->uByte3 + pWeapon_22->uByte3;
      pParam->attdep.shoot.damage = 2 * (pWeapon_18->word4 + w_42);
      pParam->attdep.shoot.damage = D4W;
      pParam->attdep.shoot.success = 1;
      CallAttackFilter(&filter, pParam, 1);
      LaunchObject(
                   pChar,
                   obj_4,
                   pParam->attdep.shoot.range,
                   pParam->attdep.shoot.damage,
                   pParam->attdep.shoot.damage );
      break;
  case atk_FLIP: //PAF
      pParam->attdep.flip.heads = STRandomBool();
      CallAttackFilter(&filter, pParam, 1);
      if (pParam->attdep.flip.heads != 0)
      {
        PrintWithSubstitution("IT COMES UP HEADS", 4, true);
      }
      else
      {
        PrintWithSubstitution("IT COMES UP TAILS", 4, true);
      };
      break;
  case atk_SPELLSHIELD://PAF
  case atk_FIRESHIELD://PAF
      pParam->attdep.shield.mustHaveMana = 1;
      pParam->attdep.shield.strength = 0x118;
      pParam->dataType = ADT_Shield;
      CallAttackFilter(&filter, pParam, 1);
      D0W = MagicShield(pChar,
                       pParam->attackType==atk_SPELLSHIELD ? 1 : 0,
                       pParam->attdep.shield.strength,
                       pParam->attdep.shield.mustHaveMana);
      if (D0W == 0)
      {
        pParam->experienceGained /= 4;
        pParam->disableTime /= 2;
      }
      else
      {
        DecrementChargesRemaining(pChar);
      };
      break;
  case atk_INVOKE://PAF
      range = STRandom(128) + 100;
      switch (STRandom(6))
      {
      case 0: rnD5 = RNPoisonBolt; break;
      case 1: rnD5 = RNPoisonCloud; break;
      case 2: rnD5 = RNDispellMissile; break;
      default:rnD5 = RNFireball; break;
      };
      successfulAttack = AttackWithSpell(pParam, &filter, range, rnD5, pParam->neededMana, pParam->experienceGained);
      break;
  case atk_FLUXCAGE: //PAF
    SetCharToPartyFacing(pChar);
    FluxCage(pParam->attackX, pParam->attackY, pParam);
    pParam->dataType = ADT_FluxCage;
    CallAttackFilter(&filter, pParam, 1);
    break;
  case atk_FUSE://PAF
    SetCharToPartyFacing(pChar);
    //w_6 = d.partyX;
    //w_8 = d.partyY;
    pParam->attackX = d.partyX + d.DeltaX[d.partyFacing];
    pParam->attackY = d.partyY + d.DeltaY[d.partyFacing];
    pParam->dataType = ADT_Fusion;
    CallAttackFilter(&filter, pParam, 1);
    Fusion(_8_,pParam->attackX, pParam->attackY);
    break;
  case atk_HEAL: //PAF
    neededHP_4 = pChar->MaxHP() - pChar->HP();
    pParam->dataType = ADT_Heal;
    if (neededHP_4 > 0)
    {
      if (pChar->Mana() != 0)
      {
        successfulAttack = 1;
        //D4W = sw(Smaller(10, DetermineMastery(pParam->charIdx, 13)));
        pParam->attdep.heal.HPIncrement 
                = sw(Smaller(10, DetermineMastery(pParam->charIdx, 13)));
        pParam->experienceGained = 2;
        CallAttackFilter(&filter, pParam, 1);
        do
        {
          //D6W = sw(Smaller(neededHP_4, D4W));
          D6W = sw(Smaller(neededHP_4, pParam->attdep.heal.HPIncrement));
          pChar->IncrHP(D6W);
          pParam->experienceGained += 2;
          pChar->IncrMana(-2);
          if (pChar->Mana() <0) break;
            neededHP_4 -= D6W;
        } while (neededHP_4 != 0);
        if (pChar->Mana() < 0)  pChar->Mana(0);
        pChar->charFlags |= CHARFLAG_statsChanged; //0x100;
      };
    }
    else
    {
      successfulAttack = 0;
      //D4W = sw(Smaller(10, DetermineMastery(pParam->charIdx, 13)));
      pParam->attdep.heal.HPIncrement = 0;
      CallAttackFilter(&filter, pParam, 1);
    };
    break;
  case atk_WINDOW: //PAF
    D4W = sw(STRandom(DetermineMastery(pParam->charIdx, pParam->skillNumber) + 8) + 5);
    timer_32.timerUByte5(0);
    timer_32.Function(TT_73);
    //timer_32.timerTime = ((d.partyLevel << 24) | d.Time) + D4W;
    timer_32.Time(d.Time + D4W);
    timer_32.Level((ui8)d.partyLevel);
    gameTimers.SetTimer(&timer_32);
    d.SeeThruWalls++;
    pParam->dataType = ADT_Window;
    CallAttackFilter(&filter, pParam, 1);
    DecrementChargesRemaining(pChar);
    break;
  case atk_CLIMBDOWN://PAF
    pParam->attackX = d.partyX;
    pParam->attackY = d.partyY;
    pParam->attackX += d.DeltaX[d.partyFacing];
    pParam->attackY += d.DeltaY[d.partyFacing];
    rtD0 = RoomType(GetCellFlags(pParam->attackX, pParam->attackY));

    if (   (rtD0 == roomPIT)
        && (FindFirstMonster(pParam->attackX, pParam->attackY) == RNeof) )
    {
      d.SupressPitDamage = 1;
      MoveObject(RN(RNnul), d.partyX, d.partyY, pParam->attackX, pParam->attackY, NULL, NULL);
      d.SupressPitDamage = 0;
    }
    else
    {
      pParam->disableTime = 0;
    };
    pParam->dataType = ADT_ClimbDown;
    CallAttackFilter(&filter, pParam, 1);
    break;
  case atk_FREEZELIFE://PAF
    D4W = 0;
    if (dbA2->IsDBType(dbMISC))
    {
      DB10A2 = dbA2->CastToDB10();
      if (DB10A2->miscType() == misc_MagicalBoxA) D4W = 30;
      if (DB10A2->miscType() == misc_MagicalBoxB) D4W = 125;
    };
    if (D4W != 0)
    {
      RemoveCharacterPossession(pParam->charIdx, 1);
      //DB10A2->link(RNnul);//The box is gone.
      DeleteDBEntry(DB10A2);
    }
    else
    {
      D4W = 70;
      DecrementChargesRemaining(pChar);
    };
    pParam->dataType = ADT_FreezeLife;
    pParam->attdep.freezeLife.oldTime = d.freezeLifeTimer;
    if (d.freezeLifeTimer + D4W > 200) D4L = 200 - d.freezeLifeTimer;
    if (D4W < 0) D4W = 0;
    pParam->attdep.freezeLife.deltaTime = D4W;
    CallAttackFilter(&filter, pParam, 1);
    d.freezeLifeTimer = ub(Smaller(255, d.freezeLifeTimer + pParam->attdep.freezeLife.deltaTime));
    break;
  case atk_LIGHT://PAF
    pParam->dataType = ADT_Light;
    pParam->attdep.light.deltaLight = d.Word1074[2];
    pParam->attdep.light.time       = 2500;
    pParam->attdep.light.decayRate  = -2;
    //d.Brightness = sw(d.Brightness + d.Word1074[2]);
    //SetBrightnessTimer(-2, 2500);
    CallAttackFilter(&filter, pParam, 1);
    d.Brightness = sw(d.Brightness + pParam->attdep.light.deltaLight);
    SetBrightnessTimer(
            pParam->attdep.light.decayRate, 
            pParam->attdep.light.time);
    DecrementChargesRemaining(pChar);
    break;
  case atk_THROW: //PAF
      SetCharToPartyFacing(pChar);
      if (   (pChar->charPosition == ((d.partyFacing + 1)&3))
          || (pChar->charPosition == ((d.partyFacing + 2)&3)) )
      {
        D0W = 1; //right side
      }
      else
      {
        D0W = 0; //left side
      };
      pParam->dataType = ADT_Throw;
      pParam->disableTime = -1;
      pParam->attdep.thro.side = D0W;
      pParam->attdep.thro.abort = 0;
      pParam->attdep.thro.facing = d.partyFacing;
      pParam->attdep.thro.range = -1;
      pParam->attdep.thro.damage = -1;
      pParam->attdep.thro.decayRate = -1;
      CallAttackFilter(&filter, pParam, 1);
      if (pParam->attdep.thro.abort == 0)
      {
        successfulAttack = ThrowByCharacter(pParam->charIdx, 1, D0W, pParam->disableTime,
                                            pParam->attdep.thro.range,
                                            pParam->attdep.thro.damage,
                                            pParam->attdep.thro.decayRate
                                            );
        // If successful the character's disable timer is set.
      }
      else
      {
        successfulAttack = 0;
      };



      //D5W = ThrowByCharacter(chIdx, 1, D0W);
      //if (D5W != 0)
      //{
        //ASSERT(pChar->busyTimer != -1);
        //d.pTimer(pChar->busyTimer)->timerUByte6 = 2;
      //};
 


      if (successfulAttack != 0)
      {
        ASSERT(pChar->busyTimer != -1,"busyTimer == -1");
        //
        // Set disable time to zero.  We have already sent a timer
        // a few lines up in 'ThrowByCharacter'.  If disable time
        // is non-zero we will do another 'DisableCharacterAction' just
        // after this 'switch' construct and that will cause the 'timerUByte6'
        // to be cleared.  That in turn will cause the champion's hand
        // not to be refilled when the timer expires.
        if (pParam->disableTime > 255) pParam->disableTime = 255;
        gameTimers.pTimer(pChar->busyTimer)->timerUByte6((ui8)((pParam->disableTime<0)?2:pParam->disableTime));
        pParam->disableTime = 0;
      };
      break;
  default:
    pParam->dataType = ADT_Default;
    CallAttackFilter(&filter, pParam, 1);
    break;
  }; //switch ()
  CallAttackFilter(&filter, pParam, 2);
  if (pParam->disableTime != 0)
  {
    DisableCharacterAction(pParam->charIdx, pParam->disableTime);
  };
  if (pParam->staminaCost != 0)
  {
    AdjustStamina(pParam->charIdx, pParam->staminaCost);
  };
  if (pParam->experienceGained != 0)
  {
    AdjustSkills(pParam->charIdx, pParam->skillNumber, pParam->experienceGained, ASW_Attack);
  };
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile), " Increase skill %02x by %02x to %02x\n", 
            pParam->skillNumber,
            pParam->experienceGained,
            pChar->skills92[pParam->skillNumber].experience);
  };
  DrawCharacterState(pParam->charIdx);
  if (pParam->activateMonster != 0)
  {
    //This used to be done in the 'DeterminePhysicalAttackDamage' code.  But
    //activating the monster caused the MonsterMoveFilter to be
    //called and that destroyed the DSA parameter area.  We could have
    //saved the parameters and restored them but even then it 
    //would cause the filters to be kind of 'nested'.  The 
    //Party Attack Filter would still be active when the Monster Movement
    //Filter was called.  I doubt it would be a problem but I'd rather
    //not find out that I was wrong.  So we delay the monster activation
    //until we are all done with the Party Attack Filter.
    ProcessTimers29to41(pParam->attackX, 
                        pParam->attackY, 
                        TT_M1, 
                        0);
  };
  RETURN_int(successfulAttack);
}

//*********************************************************
//
//*********************************************************
//           TAG01fed6
RESTARTABLE _ProcessTimersViewportAndSound(void)
{//(void)
  RESTARTMAP
    RESTART(1)
    RESTART(2)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (ProcessTimers())
  {
    VBLDelay(_1_,300);  //about 5 seconds
    ShowCredits(_2_,1); // We won the game with 4 corbums in FulYa pit.
                        // Show Credits will never return
  };
  DrawViewport(d.partyFacing, d.partyX, d.partyY);
  display();
  StartQueuedSound();
  d.Time++;
  if ((currentOverlay.m_p3 != 0) && (currentOverlay.m_p3 < d.Time)) overlayActive = false;
  parameterMessageSequence = 0;
  scrollingText.ClockTick();
  GameTime = d.Time;
  RETURN;
}

//*********************************************************
//
//*********************************************************
//           TAG01fefc
RESTARTABLE _FusionSequence(void)
{ //(void)    Called by Fusion attack only.
  static dReg D0, D1, D5, D6;
  static i16  D7w;
  static RN    objD4;
  static DB15 *DB15A2;
  static DB4  *DB4A3;
  static i8    b_226[200];
  static RN    obj_26[8];
  static i8    b_9;
  static RN    obj_8;
  static i16   w_6;
  static i16   w_4;
  static i16   i;
  static i16   x, y;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
    RESTART(4)
    RESTART(5)
    RESTART(6)
    RESTART(7)
    RESTART(8)
    RESTART(9)
    RESTART(10)
    RESTART(11)
    RESTART(12)
    RESTART(13)
    RESTART(14)
    RESTART(15)
    RESTART(16)
    RESTART(17)
    RESTART(18)
    RESTART(19)
    RESTART(20)
    //RESTART(21)
    //RESTART(22)
    //RESTART(23)
    RESTART(24)
    RESTART(25)
    RESTART(26)
    RESTART(27)
    RESTART(28)
    //RESTART(29)
    RESTART(30)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  d.Word11694 = 1;
  if (d.SelectedCharacterOrdinal != 0)
  {
    ShowHideInventory(4); //Hide
  };
  WaitForButtonFlash();
  d.Brightness = 200;
  SelectPaletteForLightLevel();
  d.PartyShield = 100;
  d.SpellShield = 100;
  d.FireShield = 100;
  MarkAllPortraitsChanged();
  ProcessTimersViewportAndSound(_1_);
  w_4 = sw(d.partyX + d.DeltaX[d.partyFacing]);
  w_6 = sw(d.partyY + d.DeltaY[d.partyFacing]);
  obj_8 = FindFirstMonster(w_4, w_6);
  DB4A3 = GetRecordAddressDB4(obj_8);
  DB4A3->hitPoints6[0] = 10000;
  SetMonsterPositionByte(DB4A3, 255, d.partyLevel, false);
  SetMonsterFacingByte(DB4A3, (d.partyFacing+2)&3, d.partyLevel, false);
  for (i=0, x=d.partyX, y=d.partyY; i<2; x=w_4, y=w_6, i++)
  {
    do
    {
      for (objD4 = FindFirstDrawableObject(x, y);
          objD4 != RNeof;
          objD4 = GetDBRecordLink(objD4) )
      {
        if (objD4.dbType() != dbCLOUD) continue;
        DB15A2 = GetRecordAddressDB15(objD4);
        if (DB15A2->cloudType() != CT_FLUXCAGE) continue;
        RemoveObjectFromRoom(objD4, x, y, NULL);
        break;
      };
    } while (objD4 != RNeof);
  };
  ProcessTimersViewportAndSound(_3_);
  for (D7w=55; D7w<=255; D7w+=40)
  {
    CreateCloud(RN(RNFireball), D7w, w_4, w_6, w_4, w_6, 255, NULL);
    ProcessTimersViewportAndSound(_4_);//TAG01fed6
    //if (testdelay) pauseAwhile(testdelay);
    if (testdelay) VBLDelay(_2_,MS2VBL(testdelay));
//
//
  };
  QueueSound(17, w_4, w_6, 1);
  DB4A3->monsterType(mon_25);
  ProcessTimersViewportAndSound(_5_);//TAG01fed6(_5_);
  //if (testdelay) pauseAwhile(testdelay);
  if (testdelay) VBLDelay(_6_, MS2VBL(testdelay));
  for (D7w=55; D7w<=255; D7w+=40)
  {
    CreateCloud(RN(RNDispellMissile), D7w, w_4, w_6, w_4, w_6, 255, NULL);
    ProcessTimersViewportAndSound(_7_);//TAG01fed6(_7_);
    if (testdelay) VBLDelay(_9_, MS2VBL(testdelay));
//
//
  };
  for (D6W=3; D6W!=0; D6W--)
  {
    for (D7w = 4; D7w!=0; D7w--)
    {
      QueueSound(17, w_4, w_6, 1);
      if (D7w & 1)
      {
        D0W = 25;
      }
      else
      {
        D0W = 23;
      };
      DB4A3->monsterType(D0W);
      for (D5W=sw(D6W-1); D5W!=0; D5W--)
      {
        ProcessTimersViewportAndSound(_8_);//TAG01fed6(_8_);
        if (testdelay) VBLDelay(_10_, MS2VBL(testdelay));
//
      };
//
//
    }; //D7W
//
//
  }; //D6W
  ProcessTimersViewportAndSound(_30_);//TAG01fed6(_7_);
  //pauseAwhile(400);
  VBLDelay(_11_, MS2VBL(400));
  CreateCloud(RN(RNFireball), 0xff, w_4, w_6, w_4, w_6, 255, NULL);
  ProcessTimersViewportAndSound(_12_);//TAG01fed6(_12_)
  //pauseAwhile(800);
  VBLDelay(_24_, MS2VBL(800));
  CreateCloud(RN(RNDispellMissile), 0xff, w_4, w_6, w_4, w_6, 255, NULL);
  ProcessTimersViewportAndSound(_15_);//TAG01fed6(_12_);
  //pauseAwhile(800);
  VBLDelay(_25_, MS2VBL(800));
  DB4A3->monsterType(mon_GreyLord);
  ProcessTimersViewportAndSound(_13_);//TAG01fed6(_13_);
  if (testdelay) VBLDelay(_26_, MS2VBL(testdelay));
  d.Word1868 = 1;
  ProcessTimersViewportAndSound(_14_);//TAG01fed6(_14_);
  if (testdelay) VBLDelay(_27_, MS2VBL(testdelay));
  for (D7w=0; D7w<d.width; D7w++)
  {
    for (D6W=0; D6W<d.height; D6W++)
    {
      objD4 = FindFirstMonster(D7w, D6W);
      if (objD4 ==RNeof) continue;
      if ((D7w != w_4) || (D6W != w_6))
      {
        DELETEMONSTERPARAMETERS dmp(DMW_Fusion, -1);
        ProcessMonsterDeleteFilter(D7w, D6W, &dmp, -1);
        DeleteMonster(D7w, D6W, NULL);
      };
//
//
    };
//
//
  };
  ProcessTimersViewportAndSound(_16_);//TAG01fed6(_16_);
  if (testdelay) VBLDelay(_28_, MS2VBL(testdelay));
  objD4 = FindFirstObject(0,0);
  for (D7w=0; objD4!=RNeof; objD4 = GetDBRecordLink(objD4))
  {
    if (objD4.dbType() != dbTEXT) continue;
    obj_26[D7w++] = objD4;
//
//
  };
  b_9 = 'A';
  for (D5W=D7w; (D7w--)!=0;)
  {
    for (D6W=0; D6W<D5W; D6W++)
    {
      DecodeText((char *)b_226,
                 GetRecordAddressDB2(obj_26[D6W]),
                 (unsigned)0x8001,
                 200);
      D0B = b_226[1];
      D1B = b_9;
      if (D0B == D1B)
      {
        TAG00187e();
        b_226[1] = 10;
        PrintLines(15, (char *)b_226+1);
        ProcessTimersViewportAndSound(_17_);//TAG01fed6(_17_);
        VBLDelay(_18_,780);
        b_9++;
        break;
      };
//
//
    };
//
  };
  PrintLines(0, (char *)d.Byte1830); // Linefeed
  PrintLines(15, "THE END.");
  PrintLines(0, (char *)d.Byte1830); // Linefeed
  VBLDelay(_19_,600);
  d.CanRestartFromSavegame = 0;
  ShowCredits(_20_,1);
  RETURN;
}
