#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

extern bool drawAsSize4Monsters;

enum SIDE_OF_DOOR
{
  SOD_NotADoor = 0,
  SOD_FarSideOfDoor = 1,
  SOD_NearSideOfDoor = 2
};

pnt TAG005088(i16 P1, i16 P2, i16 *P3, i16 *P4)
{
  dReg D0, D4, D5, D6, D7;
  aReg A2, A3;
  ui8 *pnt_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = P1;
  A2 = &d.Byte5696[2*D7W];
  if (P2 > 32) P2 = 32;
  D6W = sw((P2 * (UI8)(A2[0]) + P2/2) / 32);
  //D6W += P2/2;
  //D6W /= 32;
  D5W = sw((P2 * (UI8)(A2[1]) + P2/2) / 32);
  //D5W += P1/2;
  //D5 /= 32;
  if ( (P2==32) && (D7W!=3) )
  {
    A3 = (pnt)GetBasicGraphicAddress(D7W+348);
  }
  else
  {
    D4W = sw(14*D7W + P2/2 +436);
    D0W = AllocateDerivedGraphic(D4W);
    if (D0W != 0)
    {
      A3 = (aReg)GetDerivedGraphicAddress(D4W);
    }
    else
    {
      ui8 *pColorMap;
      pnt_4 = GetBasicGraphicAddress(Smaller(D7W, 2)+348);
      if (D7W == 3)
      {
        pColorMap = d.Byte5712;
      }
      else
      {
        pColorMap = d.IdentityColorMap;
      };
      A3 = (aReg)GetDerivedGraphicAddress(D4W);
      ShrinkBLT(pnt_4,                //src
                (ui8 *)A3,//dst
                2 * (UI8)(A2[0]),        //src width
                (UI8)(A2[1]),            //src height
                2 * D6W,             //dst width
                D5W,                 //dst height
                pColorMap);                  // ??
      //TAG022d5e(D4W);
    };
  };
  *P3 = D6W;
  *P4 = D5W;
  return A3;
}

DBCOMMON *DrawClouds(
                i32 &RelativeCellNumber,
                i16 &RelativeCell_46,
          const RN   FirstObject,
          const i32  CurAbsPos,
          const i32  facing)
{
  //i32 RelativeCellNumber;
  RN objD7;
  dReg D0, D1, D4, D5;
  aReg A3;
  i32            w_162;
  bool           w_160;
  i16            w_156;
  i16           *pw_154;
  DBCOMMON      *pdb_150;
  DB15          *pDB15_146;
  bool           w_142;
  RectPos        rect_42;
  bool           LOCAL_36;
  i16            LOCAL_30;
  i16            LOCAL_24;
  i16            LOCAL_22;
  pdb_150 = NULL;
  RelativeCellNumber = RelativeCell_46 + 3;
  RelativeCell_46 = sw(RelativeCellNumber / 3);
  objD7 = FirstObject;
tag0065e0:
  if (objD7.dbType() != dbCLOUD) goto tag006ab6;
  D5W = objD7.pos();
  pDB15_146 = GetRecordAddressDB15(objD7);
  D4W = (i16)(pDB15_146->cloudType());
  SET(D0B, D4W >= 100);
  w_160 = (D0W & 1) != 0;
//
  if (w_160
      && (  (RelativeCellNumber<3)
          ||(RelativeCellNumber>9)
          ||(D5W!=CurAbsPos) ) ) goto tag006ab6;
  w_162 = 0;
  if ( (D4W==0) || (D4W==2) || (D4W==101) )
  {
    D4W = 0;
  }
  else
  {
    if ((D4W==6) || (D4W==7))
    {
      D4W = 2;
      goto tag006774;
    }
    if (D4W ==40)
    {
      w_162 = 1;
      D4W = 3;
    }
    else
    {
      if (D4W == 100)
      {
        i32 idx;
        D0L = GetGraphicClass(RN(RNLightning));
        idx = sw(-D0W) - 1;
        //A2 = d.Byte5688+6*(D0W-1);
        //A3 = GetBasicGraphicAddress((UI8)(A2[0]) + 317);
        A3 = (pnt)GetBasicGraphicAddress(d.struct5688[idx].uByte0 + 317);
        ASSERT(RelativeCellNumber >= 3,"RelativeCell");
        pw_154 = &d.Word7246[3*(RelativeCellNumber-3)];
        //D0L = (UI8)(A2[2]) * pw_154[2];
        D0L = d.struct5688[idx].width * pw_154[2];
        LOCAL_22 = sw((D0W + pw_154[2]/2)>>5);
        //D0W = (UI8)(A2[3]);
        D0W = d.struct5688[idx].height;
        D0L = D0W * pw_154[2];
        LOCAL_24 = sw((D0W + pw_154[2]/2)>>5);
        if (RelativeCellNumber != 9)
        {
          ShrinkBLT((ui8 *)A3,
                    (ui8 *)tempBitmap,
                    //2*(UI8)(A2[2]),
                    2*d.struct5688[idx].width,
                    //(UI8)(A2[3]),
                    d.struct5688[idx].height,
                    2*LOCAL_22,
                    LOCAL_24,
                    d.IdentityColorMap);
          A3 = (i8 *)tempBitmap;
        };
        goto tag006932;
      };
      if (D4W == 50)
      {
        if (RelativeCellNumber >= 4)
        {
          pdb_150 = pDB15_146;
        };
        goto tag006ab6;
      };
      D4W = 1;
    };
  };
tag006774:
  if (RelativeCellNumber == 12)
  {
    if (w_162 != 0)
    {
      D4W--;
    };
    D4W *= 3;
    D5W = sw(pDB15_146->value() >> 5);
    if (D5W != 0)
    {
      D4W++;
      if (D5W >3)
      {
        D4W++;
      };
    };
    AllocateDerivedGraphic(0);
    A3 = (pnt)GetBasicGraphicAddress(D4W + 351,48,32);
    if (w_162 != 0)
    {
      ShrinkBLT((ui8 *)A3,             //src
                (ui8 *)tempBitmap,  //dst
                48,             //src width
                32,             //src height
                48,             //dst width
                32,             //dst height
                d.Byte5712);    //???
      A3 = (i8 *)tempBitmap;
    };
    TAG008c98((ui8 *)A3,
              d.pViewportBMP,
              0,
              GetDerivedGraphicAddress(0),
              (RectPos *)d.Word2530,
              sw(STRandom0_3() + 87),
              sw(STRandom() & 63),
              112,
              138,
              0,
              0);
    //TAG022d5e(0);
    DeleteGraphic((ui16)0x8000);
    //D0W = TAG01385a(&d.Byte11678);
    //goto tag006852;
    //d.Word4012 = 3217;
    goto tag006ab6;
  }
  else
  {
    if (w_160)
    {
      pw_154 = d.Word7222+3*RelativeCellNumber;
      w_156 = pw_154[2];
    }
    else
    {
      if (pDB15_146->b7())
      {
        pw_154 = d.Word7042 + 2*RelativeCellNumber;
      }
      else
      {
//
        if ( (D5W==facing) || (D5W==((facing-1)&3)) )
        {
          D5W = 0;
        }
        else
        {
          D5W = 1;
        };
        pw_154 = d.Word7162 + 4*RelativeCellNumber + 2*D5W;
      };
      D0W = sw(Larger(48, pDB15_146->value()+1));
      D1W = (UI8)(d.Byte5758[RelativeCell_46]);
      D0W = sw(Larger(4, D1W*D0W >>8));
      w_156 = (UI16)(D0W & 0xfffe);
    };
    A3 = TAG005088(D4W, w_156, &LOCAL_22, &LOCAL_24);
tag006932:
    w_142 = STRandomBool() != 0;
    LOCAL_30 = 0;
    LOCAL_36 = STRandomBool() != 0;
    if (LOCAL_36)
    {
      LOCAL_30 = sw(2 * (7 - ((LOCAL_22-1)&7)));
    };
    rect_42.b.y2 = ub(Smaller(135,
                            pw_154[1] + LOCAL_24/2));
    D0W = pw_154[1];
    D0W = sw(D0W - LOCAL_24/2);
    D1W = sw((LOCAL_24 & 1) == 0 ? 1 : 0);
    D4W = sw(Larger(0, D0W + D1W));
    if (D4W < 136)
    {
      rect_42.b.y1 = D4UB;
      D4W = sw(Smaller(223, pw_154[0]+LOCAL_22));
      if (D4W >= 0)
      {
        rect_42.b.x2 = D4UB;
        D4W = pw_154[0];
        rect_42.b.x1 = (ui8)ApplyLimits(0,D4W-LOCAL_22 +1, 223);
        if (rect_42.b.x1 != 0)
        {
          D4W = LOCAL_30;
        }
        else
        {
          D4W = sw(Larger(LOCAL_30, LOCAL_22-D4W-1));
        };
        if (rect_42.b.x2 > rect_42.b.x1)
        {
          LOCAL_22 = (UI16)((LOCAL_22+7)&0xfff8);
          if ( (LOCAL_36!=0) || (w_142) )
          {
            MemMove((ui8 *)A3, (ui8 *)tempBitmap, LOCAL_22*LOCAL_24);
            A3 = (i8 *)tempBitmap;
          };
          if (LOCAL_36 != 0)
          {
            Mirror((ui8 *)A3, LOCAL_22, LOCAL_24);
          };
          if (w_142)
          {
            TAG008840((ui8 *)A3, LOCAL_22, LOCAL_24);
          };
          TAG0088b2((ui8 *)A3,
                    (ui8 *)d.pViewportBMP,
                    &rect_42,
                    D4W,
                    0,
                    LOCAL_22,
                    112,
                    10);
        };
      };
    };
  };
tag006ab6:
  objD7 = GetDBRecordLink(objD7);
  if (objD7 != RNeof) goto tag0065e0;
  return pdb_150;
}


i32 DrawSize4Index(DB4 *pDB4, i32 partyFacing)
{
  ITEM16 *pItem16;
  i32 i, monsterFacing, numMonM1, facingByte, mask;
  numMonM1 = pDB4->numMonM1(); // Number of monsters in group - 1
  if (numMonM1 == 0) return 1;
  ASSERT (pDB4->groupIndex() < d.MaxITEM16,"maxitem16");
  pItem16 = &d.Item16[pDB4->groupIndex()];
  for (i=0; i<=numMonM1; i++)
  {
    if (pItem16->singleMonsterStatus[i].TestAttacking())
    {
      return i+1;
    };
  };
  monsterFacing = (partyFacing + 2) & 3;
  facingByte = pDB4->facing();
  for (i=0, mask=3; i<=numMonM1; i++, mask<<=2, monsterFacing<<=2)
  {//A little convoluted because of the compiler error.
   //Cannot use loop variable as shift count.
    if (((facingByte ^ monsterFacing) & mask) == 0)
    {
      return i+1;
    };
  };
  return 1;
}

//   NewFunc5a60
void DrawMonster(
           const i32          facing,
                 i32         &RelativeCellNumber,
           const ui16         CurAbsPos,
                 ui16         positionWithinCell,
           const SIDE_OF_DOOR sideOfDoor,
                 bool        &FinishedDrawingMonsters_108,
                 SINGLE_MONSTER_STATUS &sms_78,
           const RN           objMonster_54,
                 i16         &distanceFromUs_52,
                 //i16         &graphicNumFBSAE,
                 RectPos     &rect_42,
           const i32          MiddleLeftRight,
                 i16         &w_94,
           const i32          PositionList,
           const i16          PositionListIndex)
{
  dReg D0, D1, D4;
  aReg A3;
  ITEM110       *pI110_A0;
  ITEM110s      *pI110s_A0;
  i16            MonsterAbsFacing_182;//0=north, 1=east, etc
  //i16            w_116;
  bool           derivedGraphicAlreadyExists; //w_114;
  i16            scaleFactor; //w_112;
  bool           frontViewMirror_106;
  bool           showAttackView_104;
  bool           b_102;
  bool           showBackView_100;
  bool           showSideView_98;
  i16            LOCAL_96;
  i16            w_90;
  i16            srcHeight;    //w_88;
  i16            srcByteWidth; //w_86;
  i32            transparentColor_84;
  i16            IndexWithinGroup_82;
  MONSTERDESC_WORD4 mtDescWord4_76;
  i16            MonsterRelFacing_74;//0=away,1=right,2=toward,3=left
  i16            MonsterSize_72=-1;
  ITEM12        *pI12_70 = NULL;
  ITEM16        *pi16_62 = NULL;
  DB4           *pDB4_58;
  i16            dstHeight; //LOCAL_24;
  i16            dstByteWidth; //LOCAL_22;
  ui8           *pMonsterXYonScreen_16;
  pnt            LOCAL_12;
  ui8           *distancePalette_8;
  static ITEM16  staticITEM16;
  MONSTERDESC   *pmtDesc;
  ITEM12        *pI12_A2;
  bool           drawAsSize4;
  i32            srcOffsetX;
  i32            rootIndexOfMonsterInGroup;
  i32            graphicNumFBSAE; //graphicNum_18;
  //drawAsSize4 = true; //Pretend any monster we find here is
                       //a big dragon-like monster and that the
                       //group consists of only one monster.
  //D1W = D6W;
  LOCAL_96 =   ((positionWithinCell <= 1) || (PositionListIndex==1))
            && ((PositionList==0) || ((PositionList&15) >= 3));
  // LOCAL_96 means working on last of far positions.
  if ( (objMonster_54 == RNnul) || FinishedDrawingMonsters_108 ) return;

  //if (pDB4_58 == NULL)
  //{
  pDB4_58 = GetRecordAddressDB4(objMonster_54);
  //D0L = 16 * pDB4_58->uByte(5);
  //ASSERT(pDB4_58->uByte(5) < d.MaxITEM16);
  drawAsSize4 = drawAsSize4Monsters && pDB4_58->drawAsSize4();
  if (pDB4_58->groupIndex() >= d.MaxITEM16)
    pi16_62 = &staticITEM16;
  else
    pi16_62 = &d.Item16[pDB4_58->groupIndex()];
  pmtDesc = &d.MonsterDescriptor[pDB4_58->monsterType()];
  pI12_70 = &d.Item6414[pmtDesc->uByte0];
  if (drawAsSize4) MonsterSize_72 = 2;
  else MonsterSize_72 = pmtDesc->horizontalSize();
  mtDescWord4_76 = pmtDesc->word4;
  //};
  pI12_A2 = pI12_70; // Monster's graphic information
  // If we are drawing as if the monster were size 4 then
  // we have to decide which monster in the group we will
  // select as the monster to draw.
  // If one is in attack position..then draw it.
  // Else if one is facing us..then draw it.
  // Else draw the first one.
  if (drawAsSize4)
  {
    rootIndexOfMonsterInGroup = DrawSize4Index(pDB4_58, facing);
  }
  else
  { //The usual way of doing things
    rootIndexOfMonsterInGroup = OrdinalOfMonsterAtPosition(pDB4_58, CurAbsPos);//TAG00abae
  };
  if (rootIndexOfMonsterInGroup != 0)
  { // At least part of a monster is here!
    rootIndexOfMonsterInGroup--; // index of monster seen at this position
    IndexWithinGroup_82 = (i16)rootIndexOfMonsterInGroup;
  }
  else
  {
    if (MonsterSize_72 == 1)
    {
      // Already zero!  D7W = 0;
      IndexWithinGroup_82 = -1;
    }
    else
    {
      return;
    };
  };
//  MonsterAbsFacing_182 = (I16)((pi16_62->facings() >> 2*D7W) & 3);//monster facing
  MonsterAbsFacing_182 
     = (I16)(TwoBitNibble(pi16_62->facings(),
                          rootIndexOfMonsterInGroup));//monster facing
  MonsterRelFacing_74 = (I16)((facing - MonsterAbsFacing_182) & 3);//facing relative to party facing
  w_94 = 0;
  if (drawAsSize4) D4W = 255;
  else D4W = pi16_62->positions();
  if ( (MonsterSize_72 == 2) && (D4W != 255) )
  {
    //
    // Actually, this will happen anyway.  But I am putting
    // this test here to make it more obvious and to catch
    // a special (illegal) condition in case the code
    // that follows gets changed and does not respond
    // in the same way.
    // The effect of this is that multiple monsters of
    // size 4 (like a dragon) will be invisible.
    // People have taken advantage of this ... uh .. feature.
    //
    return;
  };
  if (D4W == 255)
  { //If we have an opportunity to draw it later then exit now.
    //Ie: wait until the last minute to draw a centered monster.
    if ( (PositionList) || (sideOfDoor==SOD_FarSideOfDoor) ) return;
    FinishedDrawingMonsters_108 = true;
    if ( (MonsterSize_72==1) && (MonsterRelFacing_74 & 1) )
    {
      positionWithinCell = 3; //Centered Worm facing sidways
    }
    else
    {
      positionWithinCell = 4; //Centered (other than sideways worm).
    };
  }
  else
  {
    if (  (MonsterSize_72==1) //2-square monster
        &&((LOCAL_96) || (PositionList==0) || (IndexWithinGroup_82<0)) )
    {
      if ( (LOCAL_96) && (sideOfDoor!=SOD_NearSideOfDoor) )
      {
        if ( (IndexWithinGroup_82>=0) && (MonsterRelFacing_74&1) )
        {
          positionWithinCell = 2;
        }
        else
        {
          return;
        };
      }
      else
      {
        if ( (sideOfDoor!=SOD_FarSideOfDoor) && (PositionList==0) )
        {
          if (MonsterRelFacing_74 & 1)
          {
            if (IndexWithinGroup_82 >= 0)
            {
              positionWithinCell = 4;
            }
            else
            {
              return;
            };
          }
          else
          {
            FinishedDrawingMonsters_108 = true;
            if (IndexWithinGroup_82 < 0)
            {
              IndexWithinGroup_82 = 0;
            };
            w_94 = pDB4_58->numMonM1(); //#monsters-1
//            D4W = sw((D4W >> (2*rootIndexOfMonsterInGroup)) & 3);//position of this monster
            D4W = sw(TwoBitNibble(D4W,rootIndexOfMonsterInGroup));//position of this monster
            if ( (D4W==facing) || (D4W==((facing-1)&3)) )
            {
              positionWithinCell = 0;
            }
            else
            {
              positionWithinCell = 1;
            };
          };
        }
        else
        {
          return;
        };
      };
    }
    else
    {
      if (MonsterSize_72) return;
    };
  };
  sms_78 = pi16_62->singleMonsterStatus[IndexWithinGroup_82];
  if (RelativeCellNumber > 9) //To our immediate left or right
  {
    RelativeCellNumber--; //our right becomes our left
                          //our left becomes our current cell
  };
//tag005a60:
  A3 = NULL;
  for (;;)
  {
    D0W = pI12_A2->uByte10;
    //D0L = 110 * BITS4_7(D0W);
    pI110_A0 = &d.s6982[BITS4_7(D0W)];
    //D1L = 10 * D6W;
    pI110s_A0 = &pI110_A0->s[RelativeCellNumber];
    pMonsterXYonScreen_16 = &pI110s_A0->MonsterXYonScreen[2*positionWithinCell];
    //pub_16 = (ui8 *)A0;
    if (pMonsterXYonScreen_16[1] != 0)
    {
      i32 graphicNum0;
      //D7W = mtDescWord4_76;
      graphicNum0 = sw(446 + pI12_A2->ITEM12_word0);
      graphicNumFBSAE = pI12_A2->ITEM12_word2;
    
      //D0W = (I16)(mtDescWord4_76 & 8);
      //if (D0W != 0)
      //{
      //  D0W = (I16)(MonsterRelFacing_74 & 1);
      //};
      //SET(D0B, D0W!=0);
      //w_98 = (I16)(D0W&1);
      //showSideView_98 =   ((mtDescWord4_76 & 8) != 0)
      //                 && ((MonsterRelFacing_74 & 1) != 0);
      showSideView_98 =   mtDescWord4_76.HasSideGraphic()
                       && ((MonsterRelFacing_74 & 1) != 0);
      if (showSideView_98)
      {
        showBackView_100 = false;
        showAttackView_104 = false;
        frontViewMirror_106 = false;
        graphicNum0++;
        graphicNumFBSAE += 2;
        dstByteWidth = pI12_A2->sideByteWidth;
        srcByteWidth = dstByteWidth;
        dstHeight = pI12_A2->sideHeight;
        srcHeight = dstHeight;
      }
      else
      {
        //D0W = (I16)(mtDescWord4_76 & 16);
        //if (D0W != 0)
        //{
        //  SET(D0B, MonsterRelFacing_74==0);
        //};
        //SET(D0B, D0B!=0);
        //w_100 = (I16)(D0W & 1);
        //showBackView_100 =    ((mtDescWord4_76 & 16) != 0)
        //                   && (MonsterRelFacing_74 == 0);
        showBackView_100 =    mtDescWord4_76.HasBackGraphic()
                           && (MonsterRelFacing_74 == 0);
        //SET(D0W, !showBackView_100);
        //if (D0B != 0)
        //{
        //  D0W = (I16)(w_78 & 128);
        //}
        //SET(D0W, D0W!=0);
        //if (D0W != 0)
        //{
        //  D0W = (I16)(mtDescWord4_76 & 32);
        //};
        //SET(D0W, D0W!=0);
        //w_104 = (I16)(D0W & 1);
        //showAttackView_104 =   !showBackView_100 
        //                    && sms_78.TestAttacking()
        //                    && ((mtDescWord4_76 & 32) != 0);
        showAttackView_104 =   !showBackView_100 
                            && sms_78.TestAttacking()
                            && mtDescWord4_76.HasAttackGraphic();
        if (showAttackView_104)
        {
          frontViewMirror_106 = false;
          dstByteWidth = pI12_A2->attackByteWidth;
          srcByteWidth = dstByteWidth;
          dstHeight = pI12_A2->attackHeight;
          srcHeight = dstHeight;
          graphicNum0++;
          graphicNumFBSAE += 2;
          //if (mtDescWord4_76 & 8)
          if (mtDescWord4_76.HasSideGraphic())
          {
            graphicNum0++;
            graphicNumFBSAE += 2;
          };
          //if (mtDescWord4_76 & 16)
          if (mtDescWord4_76.HasBackGraphic())
          {
            graphicNum0++;
            graphicNumFBSAE += 2;
          };
        }
        else
        { //Front view (non-attacking) or back view
          dstByteWidth = pI12_A2->frontRearByteWidth;
          srcByteWidth = dstByteWidth;
          dstHeight = pI12_A2->frontRearHeight;
          srcHeight = dstHeight;
          if (showBackView_100)
          {
            frontViewMirror_106 = false;
            //if (mtDescWord4_76 & 8)
            if (mtDescWord4_76.HasSideGraphic())
            {
              graphicNum0 += 2;
              graphicNumFBSAE += 4;
            }
            else
            {
              graphicNum0++;
              graphicNumFBSAE += 2;
            };
          }
          else // non-attacking front view
          {
            //D0W = (I16)(mtDescWord4_76 & 4);
            //if (D0W != 0)
            //{
            //  D0W = (I16)(sms_78 & 64);
            //};
            //SET(D0W, D0W!=0);
            //w_106 = (I16)(D0W & 1);
            //b_106 =    ((mtDescWord4_76 & 4) != 0)
            //        && sms_78.TestBit6();
            frontViewMirror_106 =    mtDescWord4_76.NonAttackingMirrorOK()
                    && sms_78.TestMirror();
            if (frontViewMirror_106)
            {
              graphicNumFBSAE += 2;
              //if (mtDescWord4_76 & 8)
              if (mtDescWord4_76.HasSideGraphic())
              {
                graphicNumFBSAE += 2;
              };
              //if (mtDescWord4_76 & 16)
              if (mtDescWord4_76.HasBackGraphic())
              {
                graphicNumFBSAE += 2;
              };
              //if (mtDescWord4_76 & 32)
              if (mtDescWord4_76.HasAttackGraphic())
              {
                graphicNumFBSAE += 2;
              };
            };
          };
        };
      };
      if (RelativeCellNumber >= 6) // a//Within one cell of us
      {
        w_90 = 0;
        distanceFromUs_52 = 0;
        transparentColor_84 = (I16)(pI12_A2->uByte10 & 15);
        if (showSideView_98) //b
        {
          A3 = (pnt)GetBasicGraphicAddress(graphicNum0);
          if (MonsterRelFacing_74 == 1)
          {
            MakeMirror((ui8 *)A3, (ui8 *)tempBitmap, dstByteWidth, dstHeight);
            A3 = (i8 *)tempBitmap;
          };
        }
        else //b not side view
        {
          if ( showBackView_100 || !frontViewMirror_106 )
          {
            A3 = (pnt)GetBasicGraphicAddress(graphicNum0);
            if (showAttackView_104)
            {
              if (sms_78.TestMirror())
              {
                MakeMirror((ui8 *)A3, (ui8 *)tempBitmap, dstByteWidth, dstHeight);
                A3 = (i8 *)tempBitmap;
              };
            };
          }
          else
          {
            D0W = AllocateDerivedGraphic(graphicNumFBSAE);
            if (D0W)
            {
              A3 = (aReg)GetDerivedGraphicAddress(graphicNumFBSAE);
            }
            else
            {
              LOCAL_12 = (pnt)GetBasicGraphicAddress(graphicNum0);
              //if (mtDescWord4_76 & 4)
              if (mtDescWord4_76.NonAttackingMirrorOK())
              {
    //
                A3 = (aReg)GetDerivedGraphicAddress(graphicNumFBSAE);
                MakeMirror((ui8 *)LOCAL_12,
                           (ui8 *)A3,
                           dstByteWidth,
                           dstHeight);
              };
              //TAG022d5e(w_18);
            };
          };
        };
      }
      else //a
      { // RelativeCellNumber <= 5 (two or more rows ahead)
        if (frontViewMirror_106)
        {
          graphicNumFBSAE++;
        };
        if (RelativeCellNumber >= 3) //within two cells of us
        { //Exactly two rows ahead.
          graphicNumFBSAE++;
          distanceFromUs_52 = 1;
    
          //D0W = (I16)(mtDescWord4_76 & 128);
          //if (D0W != 0)
          //if (mtDescWord4_76.HasBit7())
          //{
          //  D0W = !showSideView_98;
          //};
          //SET(D0W, D0W!=0);
          //if (D0B !=0)
          //{
          //  SET(D0W, !showBackView_100);
          //};
          //if (D0W != 0)
          //{
          //  SET(D0W, !showAttackView_104);
          //};
          //w_102 = (I16)(D0W & 1);
          b_102 =    mtDescWord4_76.HasBit7()
                  && !showSideView_98
                  && !showBackView_100
                  && !showAttackView_104;
          distancePalette_8 = d.Byte6628;
          scaleFactor = 20;  // 20/32 = 0.625
        }
        else
        {//Three or more rows ahead
          b_102 = false; //Added this because otherwise
                         //this variable is undefined later.
                         //I am guessing a value.
          distanceFromUs_52 = 2;
          frontViewMirror_106 = false;
          distancePalette_8 = d.Byte6612;
          scaleFactor = 16; // 16/32 = 0.5
        };
        D0L = srcByteWidth * scaleFactor;
        D0W = sw(D0W + (scaleFactor >> 1));
        dstByteWidth = sw(D0W >> 5);  // dst width in bytes
        D0L = srcHeight * scaleFactor;
        D0W = sw(D0W + (scaleFactor >> 1));
        dstHeight = sw(D0W >> 5);
        D0W = (I16)(pI12_A2->uByte10 & 15);
        D0W = distancePalette_8[D0W];
        transparentColor_84 = sw(D0W / 10);
        derivedGraphicAlreadyExists = AllocateDerivedGraphic(graphicNumFBSAE);
        if (derivedGraphicAlreadyExists)
        {
          A3 = (aReg)GetDerivedGraphicAddress(graphicNumFBSAE);
        }
        else
        {
          LOCAL_12 = (pnt)GetBasicGraphicAddress(graphicNum0);
          A3 = (aReg)GetDerivedGraphicAddress(graphicNumFBSAE);
          ShrinkBLT((ui8 *)LOCAL_12,                                        // src
                    (ui8 *)A3,    // dst
                    2 * srcByteWidth,                                // src width in pixels
                    srcHeight,  // w_88                              // src height
                    2 * dstByteWidth,                                // dst width in pixels
                    dstHeight,                                       // dst height
                    distancePalette_8);
          //TAG022d5e(w_18);
        };

        /*
        if (w_98 == 0) goto tag005e60;
        if (MonsterRelFacing_74 == 1) goto tag005e8c;
tag005e60:
        if (w_104 == 0) goto tag005e70;
        if (w_78 & 64) goto tag005e8c;
tag005e70:
        if (w_102 == 0) goto tag005e7e;
        if (mtDescWord4_76 & 256) goto tag005e8c;
tag005e7e:
        if (w_106 == 0) goto tag005ee8;
        if ((mtDescWord4_76 & 4)==0) goto tag005ee8;
tag005e8c:
        if (w_106==0) goto tag005e98;
        if (w_114 != 0) goto tag005ed0;
tag005e98:
*/
        if (    (showSideView_98)  && (MonsterRelFacing_74 == 1)
             || (showAttackView_104) && (sms_78.TestMirror())
             //|| (w_102 != 0) && (mtDescWord4_76 & 256)
             || b_102 && mtDescWord4_76.HasBit8()
             //|| b_106 && ((mtDescWord4_76 & 4) == 0) )
             || frontViewMirror_106 && !mtDescWord4_76.NonAttackingMirrorOK() )
        {
          if ( !frontViewMirror_106 || (!derivedGraphicAlreadyExists) )
          {
            D4W = (UI16)((dstByteWidth +7) & 0xfff8);
            if (!frontViewMirror_106)
            {
              MemMove((ui8 *)A3, (ui8 *)tempBitmap, (ui16)D4W*(ui16)dstHeight);
              A3 = (i8 *)tempBitmap;
            };
            Mirror((ui8 *)A3, D4W, dstHeight);
          };
//tag005ed0:
          D1W = (I16)((dstByteWidth-1)&7);
          D0W = sw(7-D1W);
          w_90 = sw(D0W << 1);
          //goto tag005eec;
        }
        else
        {
//tag005ee8:
          w_90 = 0;
        };
//tag005eec:
        //A0 = (pnt)pub_16;
      }; //a
      D4W = pMonsterXYonScreen_16[1];
      D0W = sms_78.Nibble3();
      D1L = (UI16)(distanceFromUs_52) >> 3;
      D4W = sw(D4W + sb(d.smallOffsets[D1L+D0W]));
      rect_42.b.y2 = ub(Smaller(D4W, 135));
      rect_42.b.y1 = ub(Larger(0, D4W - dstHeight + 1));
      D4W = pMonsterXYonScreen_16[0];
      D0W = sms_78.Nibble0();
      D1L = (UI16)(distanceFromUs_52) << 3;
      D4W = sw(D4W + d.smallOffsets[D1L + D0W]);
      if (MiddleLeftRight == 1) // Left side
      {
        D4W -= 100;
      }
      else
      {
        if (MiddleLeftRight) // Right Side
        {
          D4W += 100;
        };
      };
      rect_42.b.x2 = (ui8)ApplyLimits(0, D4W+dstByteWidth, 223);
      if (rect_42.b.x2 != 0)
      {
        rect_42.b.x1 = (ui8)ApplyLimits(0, D4W-dstByteWidth+1, 223);
        if (rect_42.b.x1 != 223)
        {
          if (rect_42.b.x1 != 0)
          {
            srcOffsetX = w_90;
          }
          else
          {
            //D7W = w_90;
            srcOffsetX = sw(w_90 + dstByteWidth - D4W - 1);
          };
int size = 0;
if (size)
{
size = (rect_42.b.x2-rect_42.b.x1+1)*(rect_42.b.y2-rect_42.b.y1+1)/2;
FILE *f = fopen("graph.bin","wb");
fwrite(A3,1,size,f);
fclose(f);
};
          TAG0088b2((ui8 *)A3,
                    (ui8 *)d.pViewportBMP,
                    &rect_42,
                    srcOffsetX,
                    0,
                    (dstByteWidth + 7) & 0xfff8,
                    112,
                    transparentColor_84);
        };
      };
    };
//tag00600c:
    if (w_94 == 0) break;
    w_94 = 0;
    SET(D0W, IndexWithinGroup_82==0);
    D0W &= 1;
    sms_78 = pi16_62->singleMonsterStatus[D0W];
    if (positionWithinCell == 1)
    {
      positionWithinCell = 0;
    }
    else
    {
      positionWithinCell = 1;
    };
  }; //for (;;)
  return;
}


#define NUM_DERIVED_GRAPHIC 10


struct ALTMONGRAPH
{
  MONSTERDESC monsterDesc;
  ITEM12      item12;
  ITEM110     item110;
  i32         graphicOffsets[10]; // relative to graphic[0]
                                  // -1 means no graphic exists.
  //i32         derivedGraphicSizes[NUM_DERIVED_GRAPHIC];
  //ui8         exists;     // set to 0 if we have not yet looked for this segment
  //                        // set to 1 when we read the segment
  //                        // set to 2 if the segment does not exist.
  ui8         graphic[4];
};

struct ALTMONCACHE // 108 entries. One for each of 27 monstertypes and 4 alternate graphics
{
  ALTMONCACHE();
  ~ALTMONCACHE(){Clear();};
  void Clear();
  ALTMONGRAPH *pGraphic;
  i32  size;      // size initially set to -1;
                  // if (pGraphic == NULL and size == 0) then the entry does not exist.
  pnt  pDerivedGraphic[NUM_DERIVED_GRAPHIC];
  i32  derivedSize[NUM_DERIVED_GRAPHIC];
  i32  graphicID; // file number in CSBgraphics.dat
};


ALTMONCACHE::ALTMONCACHE()
{
  ui32 i;
  pGraphic = NULL;
  size = -1;
  for (i=0; i<NUM_DERIVED_GRAPHIC; i++)
  {
    pDerivedGraphic[i] = NULL;
    derivedSize[i] = -1;
  };
  graphicID = -1;
}

void ALTMONCACHE::Clear()
{
  ui32 i;
  if (pGraphic != NULL)
  {
    UI_free(pGraphic);
    pGraphic = NULL;
  };
  size = -1;
  for (i=0; i<NUM_DERIVED_GRAPHIC; i++)
  {
    if (pDerivedGraphic[i] != NULL)
    {
      UI_free(pDerivedGraphic[i]);
      pDerivedGraphic[i] = NULL;
    };
    derivedSize[i] = -1;
  };
  graphicID = -1;
}

// A cache of Derived graphics for Alternate Monster Graphics
ALTMONCACHE altMonCache[27][4]; //27 monster types - 4 alternate graphics.

void CleanupAltMonCache()
{
  i32 i, j;
  for (i=0; i<27; i++)
  {
    for (j=0; j<4; j++)
    {
      altMonCache[i][j].Clear();
    };
  };
}

bool AllocateAlternateDerivedGraphic(DB4 *pMonster, i32 graphicFBSAE, i32 width=0, i32 height=0)
{// Return true if graphic already exists.
  i32 type, alt, byteWidth;
  type = pMonster->monsterType();
  alt  = pMonster->alternateGraphics();
  if (altMonCache[type][alt].pDerivedGraphic[graphicFBSAE] != NULL)
  {
    return true;
  };
  if (width==0) die(0x3334,"Alternate Zero Width");
  byteWidth = (width + 15) / 16 * 8;
  altMonCache[type][alt].pDerivedGraphic[graphicFBSAE] = (pnt)UI_malloc(byteWidth * height, MALLOC102);
  return false;
}


i32 AltGraphicMapping(i32 mt, i32 altGraphic)
{
  ui32 key;
  i32 size;
  ui32 *pRecord;
  key = (EDT_Database<<24) | (EDBT_AltMonGraphics<<16) | (d.LoadedLevel<<8) | mt;
  size = expool.Locate(key, &pRecord);
  if (size != 4) return -1;
  return pRecord[altGraphic];
}

ALTMONGRAPH *GetAlternateGraphicsRecord(DB4 *pMonster)
{
  ui32 actualSize, mt, altGraphic;
  i32 id;
  ui8 *pRecord;
  ALTMONGRAPH *pGraphic;
  if (pMonster == NULL) return NULL;
  mt = pMonster->monsterType();
  altGraphic = pMonster->alternateGraphics();
  id = AltGraphicMapping(mt, altGraphic);
  if (id == -2) return NULL;
  if (id < 0) 
  {
    id = 3 * mt + altGraphic - 1;
    if (altGraphic == 0) return NULL;
  };
  if (   (altMonCache[mt][altGraphic].graphicID >= 0) 
      && (altMonCache[mt][altGraphic].graphicID != id))
  {
    altMonCache[mt][altGraphic].Clear();
  };
  pGraphic = altMonCache[mt][altGraphic].pGraphic;
  if (pGraphic == NULL)
  {
    // size = 0 means we have already searched for this graphic.
    if (altMonCache[mt][altGraphic].size == 0) return NULL;
    pRecord = ReadCSBgraphic(CGT_AlternateMonsterGraphic,
                      id,
                      sizeof(ALTMONGRAPH),
                      &actualSize,
                      true,
                      MALLOC119);
    altMonCache[mt][altGraphic].size = 0;
    if (pRecord == NULL) return NULL;
    altMonCache[mt][altGraphic].size = actualSize;
    pGraphic = (ALTMONGRAPH *)pRecord;
    altMonCache[mt][altGraphic].pGraphic = pGraphic;
    altMonCache[mt][altGraphic].graphicID = id;
  };
  return pGraphic;
}

pnt GetAlternateBasicGraphicAddress(const ALTMONGRAPH *pGraph, i32 graphicFBSAE)
{
  return (pnt)pGraph->graphic + pGraph->graphicOffsets[graphicFBSAE];
}


pnt GetAlternateDerivedGraphicAddress(DB4 *pMonster, i32 graphicFBSAE)
{
  i32 type, alt;
  type = pMonster->monsterType();
  alt  = pMonster->alternateGraphics();
  return altMonCache[type][alt].pDerivedGraphic[graphicFBSAE];
}




// ***************************************************************************************
// The following function was initially copied verbatim from the original DrawMonster.
// Then it was modified to use the graphic information from pAlternateGraphic
// instead of the information in the DATABANK.  In this way, I could mess around
// with the function without breaking the original function.  Also, I would never
// change the original function and could always use it as a model.
// In general, the ALTMONGRAPH structure contains all the information in the
// same format as the several original structures, as well as the graphics themselves.
// ***************************************************************************************
void DrawMonster(
           const i32          facing,
                 i32         &RelativeCellNumber,
           const ui16         CurAbsPos,
                 ui16         positionWithinCell,
           const SIDE_OF_DOOR sideOfDoor,
                 bool        &FinishedDrawingMonsters_108,
                 SINGLE_MONSTER_STATUS &sms_78,
           const RN           objMonster_54,
                 i16         &distanceFromUs_52,
                 //i16         &graphicNumFBSAE,
                 RectPos     &rect_42,
           const i32          MiddleLeftRight,
                 i16         &w_94,
           const i32          PositionList,
           const i16          PositionListIndex,
           const ALTMONGRAPH *pAlternateGraphic)
{
  dReg D0, D1, D4;
  aReg A3;
  //ITEM110           *pI110_A0;
  const ITEM110s    *pI110s_A0;
  i16                MonsterAbsFacing_182;//0=north, 1=east, etc
  //i16            w_116;
  bool               derivedGraphicAlreadyExists; //w_114;
  i16                scaleFactor; //w_112;
  bool               frontViewMirror_106;
  bool               showAttackView_104;
  bool               b_102;
  bool               showBackView_100;
  bool               showSideView_98;
  i16                LOCAL_96;
  i16                w_90;
  i16                srcHeight; //w_88;
  i16                srcByteWidth; //w_86;
  i32                transparentColor_84;
  i16                IndexWithinGroup_82;
  MONSTERDESC_WORD4  mtDescWord4_76;
  i16                MonsterRelFacing_74;//0=away,1=right,2=toward,3=left
  i16                MonsterSize_72=-1;
  const ITEM12      *pI12_70 = NULL;
  ITEM16            *pi16_62 = NULL;
  DB4               *pDB4_58;
  i16                dstHeight; //LOCAL_24;
  i16                dstByteWidth; //LOCAL_22;
  const ui8         *pMonsterXYonScreen_16;
  pnt                LOCAL_12;
  ui8               *distancePalette_8;
  static ITEM16      staticITEM16;
  const MONSTERDESC *pmtDesc;
  const ITEM12      *pI12_A2;
  bool               drawAsSize4;
  i32                srcOffsetX;
  i32                rootIndexOfMonsterInGroup;
  i32                graphicNumFBSAE; // Graphic Num. Front, Back, Side, Attack, Etc.graphicNum_18;
  //drawAsSize4 = true; //Pretend any monster we find here is
                       //a big dragon-like monster and that the
                       //group consists of only one monster.
  //D1W = D6W;
  LOCAL_96 =   ((positionWithinCell <= 1) || (PositionListIndex==1))
            && ((PositionList==0) || ((PositionList&15) >= 3));
  // LOCAL_96 means working on last of far positions.
  if ( (objMonster_54 == RNnul) || FinishedDrawingMonsters_108 ) return;

  //if (pDB4_58 == NULL)
  //{
  pDB4_58 = GetRecordAddressDB4(objMonster_54);
  //D0L = 16 * pDB4_58->uByte(5);
  //ASSERT(pDB4_58->uByte(5) < d.MaxITEM16);
  drawAsSize4 = drawAsSize4Monsters && pDB4_58->drawAsSize4();
  if (pDB4_58->groupIndex() >= d.MaxITEM16)
    pi16_62 = &staticITEM16;
  else
    pi16_62 = &d.Item16[pDB4_58->groupIndex()];
  //pmtDesc = &d.MonsterDescriptor[pDB4_58->monsterType()];
  pmtDesc = &pAlternateGraphic->monsterDesc;
  //pI12_70 = &d.Item6414[pmtDesc->uByte0];
  pI12_70 = &pAlternateGraphic->item12;
  if (drawAsSize4) MonsterSize_72 = 2;
  // else MonsterSize_72 = pmtDesc->horizontalSize();
  // This line was replaced to use the size of the monster as specified in the
  // original monster descriptor.  THis means that you must make your 
  // alternate graphics agree in size with the standard graphics.
  else MonsterSize_72 = d.MonsterDescriptor[pDB4_58->monsterType()].horizontalSize();
  mtDescWord4_76 = pmtDesc->word4;
  //};
  pI12_A2 = pI12_70; // Monster's graphic information
  // If we are drawing as if the monster were size 4 then
  // we have to decide which monster in the group we will
  // select as the monster to draw.
  // If one is in attack position..then draw it.
  // Else if one is facing us..then draw it.
  // Else draw the first one.
  if (drawAsSize4)
  {
    rootIndexOfMonsterInGroup = DrawSize4Index(pDB4_58, facing);
  }
  else
  { //The usual way of doing things
    rootIndexOfMonsterInGroup = OrdinalOfMonsterAtPosition(pDB4_58, CurAbsPos);//TAG00abae
  };
  if (rootIndexOfMonsterInGroup != 0)
  { // At least part of a monster is here!
    rootIndexOfMonsterInGroup--; // index of monster seen at this position
    IndexWithinGroup_82 = (i16)rootIndexOfMonsterInGroup;
  }
  else
  {
    if (MonsterSize_72 == 1)
    {
      // Already zero!  D7W = 0;
      IndexWithinGroup_82 = -1;
    }
    else
    {
      return;
    };
  };
//  MonsterAbsFacing_182 = (I16)((pi16_62->facings() >> 2*D7W) & 3);//monster facing
  MonsterAbsFacing_182 
     = (I16)(TwoBitNibble(pi16_62->facings(),
                          rootIndexOfMonsterInGroup));//monster facing
  MonsterRelFacing_74 = (I16)((facing - MonsterAbsFacing_182) & 3);//facing relative to party facing
  w_94 = 0;
  if (drawAsSize4) D4W = 255;
  else D4W = pi16_62->positions();
  if ( (MonsterSize_72 == 2) && (D4W != 255) )
  {
    //
    // Actually, this will happen anyway.  But I am putting
    // this test here to make it more obvious and to catch
    // a special (illegal) condition in case the code
    // that follows gets changed and does not respond
    // in the same way.
    // The effect of this is that multiple monsters of
    // size 4 (like a dragon) will be invisible.
    // People have taken advantage of this ... uh .. feature.
    //
    return;
  };
  if (D4W == 255)
  { //If we have an opportunity to draw it later then exit now.
    //Ie: wait until the last minute to draw a centered monster.
    if ( (PositionList) || (sideOfDoor==SOD_FarSideOfDoor) ) return;
    FinishedDrawingMonsters_108 = true;
    if ( (MonsterSize_72==1) && (MonsterRelFacing_74 & 1) )
    {
      positionWithinCell = 3; //Centered Worm facing sidways
    }
    else
    {
      positionWithinCell = 4; //Centered (other than sideways worm).
    };
  }
  else
  {
    if (  (MonsterSize_72==1) //2-square monster
        &&((LOCAL_96) || (PositionList==0) || (IndexWithinGroup_82<0)) )
    {
      if ( (LOCAL_96) && (sideOfDoor!=SOD_NearSideOfDoor) )
      {
        if ( (IndexWithinGroup_82>=0) && (MonsterRelFacing_74&1) )
        {
          positionWithinCell = 2;
        }
        else
        {
          return;
        };
      }
      else
      {
        if ( (sideOfDoor!=SOD_FarSideOfDoor) && (PositionList==0) )
        {
          if (MonsterRelFacing_74 & 1)
          {
            if (IndexWithinGroup_82 >= 0)
            {
              positionWithinCell = 4;
            }
            else
            {
              return;
            };
          }
          else
          {
            FinishedDrawingMonsters_108 = true;
            if (IndexWithinGroup_82 < 0)
            {
              IndexWithinGroup_82 = 0;
            };
            w_94 = pDB4_58->numMonM1(); //#monsters-1
//            D4W = sw((D4W >> (2*rootIndexOfMonsterInGroup)) & 3);//position of this monster
            D4W = sw(TwoBitNibble(D4W,rootIndexOfMonsterInGroup));//position of this monster
            if ( (D4W==facing) || (D4W==((facing-1)&3)) )
            {
              positionWithinCell = 0;
            }
            else
            {
              positionWithinCell = 1;
            };
          };
        }
        else
        {
          return;
        };
      };
    }
    else
    {
      if (MonsterSize_72) return;
    };
  };
  sms_78 = pi16_62->singleMonsterStatus[IndexWithinGroup_82];
  if (RelativeCellNumber > 9) //To our immediate left or right
  {
    RelativeCellNumber--; //our right becomes our left
                          //our left becomes our current cell
  };
//tag005a60:
  A3 = NULL;
  for (;;)
  {
    //D0W = pI12_A2->uByte10;
    //D0L = 110 * BITS4_7(D0W);
    //pI110_A0 = &d.s6982[BITS4_7(D0W)];
    //D1L = 10 * D6W;
    //pI110s_A0 = &pI110_A0->s[RelativeCellNumber];
    pI110s_A0 = &pAlternateGraphic->item110.s[RelativeCellNumber];
    pMonsterXYonScreen_16 = &pI110s_A0->MonsterXYonScreen[2*positionWithinCell];
    //pub_16 = (ui8 *)A0;
    if (pMonsterXYonScreen_16[1] != 0)
    {
      i32 graphicNum0;
      //D7W = mtDescWord4_76;
      //graphicNum0 = sw(446 + pI12_A2->ITEM12_word0);
      graphicNum0 = 0; // Assume non-mirrored front view.
      //graphicNumFBSAE = pI12_A2->ITEM12_word2;
      graphicNumFBSAE = 0; //Relative to this particular aternative monster's derived graphics
    
      //D0W = (I16)(mtDescWord4_76 & 8);
      //if (D0W != 0)
      //{
      //  D0W = (I16)(MonsterRelFacing_74 & 1);
      //};
      //SET(D0B, D0W!=0);
      //w_98 = (I16)(D0W&1);
      //showSideView_98 =   ((mtDescWord4_76 & 8) != 0)
      //                 && ((MonsterRelFacing_74 & 1) != 0);
      showSideView_98 =   mtDescWord4_76.HasSideGraphic()
                       && ((MonsterRelFacing_74 & 1) != 0);
      if (showSideView_98)
      {
        showBackView_100 = false;
        showAttackView_104 = false;
        frontViewMirror_106 = false;
        graphicNum0++;
        graphicNumFBSAE += 2;
        dstByteWidth = pI12_A2->sideByteWidth;
        srcByteWidth = dstByteWidth;
        dstHeight = pI12_A2->sideHeight;
        srcHeight = dstHeight;
      }
      else
      {
        //D0W = (I16)(mtDescWord4_76 & 16);
        //if (D0W != 0)
        //{
        //  SET(D0B, MonsterRelFacing_74==0);
        //};
        //SET(D0B, D0B!=0);
        //w_100 = (I16)(D0W & 1);
        //showBackView_100 =    ((mtDescWord4_76 & 16) != 0)
        //                   && (MonsterRelFacing_74 == 0);
        showBackView_100 =    mtDescWord4_76.HasBackGraphic()
                           && (MonsterRelFacing_74 == 0);
        //SET(D0W, !showBackView_100);
        //if (D0B != 0)
        //{
        //  D0W = (I16)(w_78 & 128);
        //}
        //SET(D0W, D0W!=0);
        //if (D0W != 0)
        //{
        //  D0W = (I16)(mtDescWord4_76 & 32);
        //};
        //SET(D0W, D0W!=0);
        //w_104 = (I16)(D0W & 1);
        //showAttackView_104 =   !showBackView_100 
        //                    && sms_78.TestAttacking()
        //                    && ((mtDescWord4_76 & 32) != 0);
        showAttackView_104 =   !showBackView_100 
                            && sms_78.TestAttacking()
                            && mtDescWord4_76.HasAttackGraphic();
        if (showAttackView_104)
        {
          frontViewMirror_106 = false;
          dstByteWidth = pI12_A2->attackByteWidth;
          srcByteWidth = dstByteWidth;
          dstHeight = pI12_A2->attackHeight;
          srcHeight = dstHeight;
          graphicNum0++;
          graphicNumFBSAE += 2;
          //if (mtDescWord4_76 & 8)
          if (mtDescWord4_76.HasSideGraphic())
          {
            graphicNum0++;
            graphicNumFBSAE += 2;
          };
          //if (mtDescWord4_76 & 16)
          if (mtDescWord4_76.HasBackGraphic())
          {
            graphicNum0++;
            graphicNumFBSAE += 2;
          };
        }
        else
        { //Front view (non-attacking) or back view
          dstByteWidth = pI12_A2->frontRearByteWidth;
          srcByteWidth = dstByteWidth;
          dstHeight = pI12_A2->frontRearHeight;
          srcHeight = dstHeight;
          if (showBackView_100)
          {
            frontViewMirror_106 = false;
            //if (mtDescWord4_76 & 8)
            if (mtDescWord4_76.HasSideGraphic())
            {
              graphicNum0 += 2;
              graphicNumFBSAE += 4;
            }
            else
            {
              graphicNum0++;
              graphicNumFBSAE += 2;
            };
          }
          else // non-attacking front view
          {
            //D0W = (I16)(mtDescWord4_76 & 4);
            //if (D0W != 0)
            //{
            //  D0W = (I16)(sms_78 & 64);
            //};
            //SET(D0W, D0W!=0);
            //w_106 = (I16)(D0W & 1);
            //b_106 =    ((mtDescWord4_76 & 4) != 0)
            //        && sms_78.TestBit6();
            frontViewMirror_106 =    mtDescWord4_76.NonAttackingMirrorOK()
                    && sms_78.TestMirror();
            if (frontViewMirror_106)
            {
              graphicNumFBSAE += 2;
              //if (mtDescWord4_76 & 8)
              if (mtDescWord4_76.HasSideGraphic())
              {
                graphicNumFBSAE += 2;
              };
              //if (mtDescWord4_76 & 16)
              if (mtDescWord4_76.HasBackGraphic())
              {
                graphicNumFBSAE += 2;
              };
              //if (mtDescWord4_76 & 32)
              if (mtDescWord4_76.HasAttackGraphic())
              {
                graphicNumFBSAE += 2;
              };
            };
          };
        };
      };
      if (RelativeCellNumber >= 6) // a//Within one cell of us
      {
        w_90 = 0;
        distanceFromUs_52 = 0;
        transparentColor_84 = (I16)(pI12_A2->uByte10 & 15);
        if (showSideView_98) //b
        {
          A3 = GetAlternateBasicGraphicAddress(pAlternateGraphic, graphicNum0);
          if (MonsterRelFacing_74 == 1)
          {
            MakeMirror((ui8 *)A3, (ui8 *)tempBitmap, dstByteWidth, dstHeight);
            A3 = (i8 *)tempBitmap;
          };
        }
        else //b not side view
        {
          if ( showBackView_100 || !frontViewMirror_106 )
          {
            A3 = GetAlternateBasicGraphicAddress(pAlternateGraphic, graphicNum0);
            if (showAttackView_104)
            {
              if (sms_78.TestMirror())
              {
                MakeMirror((ui8 *)A3, (ui8 *)tempBitmap, dstByteWidth, dstHeight);
                A3 = (i8 *)tempBitmap;
              };
            };
          }
          else
          {
            D0W = AllocateAlternateDerivedGraphic(pDB4_58, graphicNumFBSAE);
            if (D0W)
            {
              A3 = GetAlternateDerivedGraphicAddress(pDB4_58, graphicNumFBSAE);
            }
            else
            {
              LOCAL_12 = (pnt)GetBasicGraphicAddress(graphicNum0);
              //if (mtDescWord4_76 & 4)
              if (mtDescWord4_76.NonAttackingMirrorOK())
              {
    //
                A3 = (aReg)GetDerivedGraphicAddress(graphicNumFBSAE);
                MakeMirror((ui8 *)LOCAL_12,
                           (ui8 *)A3,
                           dstByteWidth,
                           dstHeight);
              };
              //TAG022d5e(w_18);
            };
          };
        };
      }
      else //a
      { // RelativeCellNumber <= 5 (two or more rows ahead)
        if (frontViewMirror_106)
        {
          graphicNumFBSAE++;
        };
        if (RelativeCellNumber >= 3) //within two cells of us
        { //Exactly two rows ahead.
          graphicNumFBSAE++;
          distanceFromUs_52 = 1;
    
          //D0W = (I16)(mtDescWord4_76 & 128);
          //if (D0W != 0)
          //if (mtDescWord4_76.HasBit7())
          //{
          //  D0W = !showSideView_98;
          //};
          //SET(D0W, D0W!=0);
          //if (D0B !=0)
          //{
          //  SET(D0W, !showBackView_100);
          //};
          //if (D0W != 0)
          //{
          //  SET(D0W, !showAttackView_104);
          //};
          //w_102 = (I16)(D0W & 1);
          b_102 =    mtDescWord4_76.HasBit7()
                  && !showSideView_98
                  && !showBackView_100
                  && !showAttackView_104;
          distancePalette_8 = d.Byte6628;
          scaleFactor = 20; // 20/32 = 0.625
        }
        else
        {//Three or more rows ahead
          b_102 = false; //Added this because otherwise
                         //this variable is undefined later.
                         //I am guessing a value.
          distanceFromUs_52 = 2;
          frontViewMirror_106 = false;
          distancePalette_8 = d.Byte6612;
          scaleFactor = 16; // 16/32 = 0.5
        };
        D0L = srcByteWidth * scaleFactor;
        D0W = sw(D0W + (scaleFactor >> 1));
        dstByteWidth = sw(D0W >> 5);
        D0L = srcHeight * scaleFactor;
        D0W = sw(D0W + (scaleFactor >> 1));
        dstHeight = sw(D0W >> 5);
        D0W = (I16)(pI12_A2->uByte10 & 15);
        D0W = distancePalette_8[D0W];
        transparentColor_84 = sw(D0W / 10);
        derivedGraphicAlreadyExists = 
              AllocateAlternateDerivedGraphic(pDB4_58, graphicNumFBSAE, 2*dstByteWidth, dstHeight);
        if (derivedGraphicAlreadyExists)
        {
          A3 = GetAlternateDerivedGraphicAddress(pDB4_58,graphicNumFBSAE);
        }
        else
        {
          LOCAL_12 = GetAlternateBasicGraphicAddress(pAlternateGraphic,graphicNum0);
          A3 = (aReg)GetAlternateDerivedGraphicAddress(pDB4_58, graphicNumFBSAE);
          ShrinkBLT((ui8 *)LOCAL_12,
                    (ui8 *)A3,
                    2 * srcByteWidth,
                    srcHeight,
                    2 * dstByteWidth,
                    dstHeight,
                    distancePalette_8);
          //TAG022d5e(w_18);
        };

        /*
        if (w_98 == 0) goto tag005e60;
        if (MonsterRelFacing_74 == 1) goto tag005e8c;
tag005e60:
        if (w_104 == 0) goto tag005e70;
        if (w_78 & 64) goto tag005e8c;
tag005e70:
        if (w_102 == 0) goto tag005e7e;
        if (mtDescWord4_76 & 256) goto tag005e8c;
tag005e7e:
        if (w_106 == 0) goto tag005ee8;
        if ((mtDescWord4_76 & 4)==0) goto tag005ee8;
tag005e8c:
        if (w_106==0) goto tag005e98;
        if (w_114 != 0) goto tag005ed0;
tag005e98:
*/
        if (    (showSideView_98)  && (MonsterRelFacing_74 == 1)
             || (showAttackView_104) && (sms_78.TestMirror())
             //|| (w_102 != 0) && (mtDescWord4_76 & 256)
             || b_102 && mtDescWord4_76.HasBit8()
             //|| b_106 && ((mtDescWord4_76 & 4) == 0) )
             || frontViewMirror_106 && !mtDescWord4_76.NonAttackingMirrorOK() )
        {
          if ( !frontViewMirror_106 || (!derivedGraphicAlreadyExists) )
          {
            D4W = (UI16)((dstByteWidth +7) & 0xfff8);
            if (!frontViewMirror_106)
            {
              MemMove((ui8 *)A3, (ui8 *)tempBitmap, (ui16)D4W*(ui16)dstHeight);
              A3 = (i8 *)tempBitmap;
            };
            Mirror((ui8 *)A3, D4W, dstHeight);
          };
//tag005ed0:
          D1W = (I16)((dstByteWidth-1)&7);
          D0W = sw(7-D1W);
          w_90 = sw(D0W << 1);
          //goto tag005eec;
        }
        else
        {
//tag005ee8:
          w_90 = 0;
        };
//tag005eec:
        //A0 = (pnt)pub_16;
      }; //a
      D4W = pMonsterXYonScreen_16[1];
      D0W = sms_78.Nibble3();
      D1L = (UI16)(distanceFromUs_52) >> 3;
      D4W = sw(D4W + sb(d.smallOffsets[D1L+D0W]));
      rect_42.b.y2 = ub(Smaller(D4W, 135));
      rect_42.b.y1 = ub(Larger(0, D4W - dstHeight + 1));
      D4W = pMonsterXYonScreen_16[0];
      D0W = sms_78.Nibble0();
      D1L = (UI16)(distanceFromUs_52) << 3;
      D4W = sw(D4W + d.smallOffsets[D1L + D0W]);
      if (MiddleLeftRight == 1) // Left side
      {
        D4W -= 100;
      }
      else
      {
        if (MiddleLeftRight) // Right Side
        {
          D4W += 100;
        };
      };
      rect_42.b.x2 = (ui8)ApplyLimits(0, D4W+dstByteWidth, 223);
      if (rect_42.b.x2 != 0)
      {
        rect_42.b.x1 = (ui8)ApplyLimits(0, D4W-dstByteWidth+1, 223);
        if (rect_42.b.x1 != 223)
        {
          if (rect_42.b.x1 != 0)
          {
            srcOffsetX = w_90;
          }
          else
          {
            //D7W = w_90;
            srcOffsetX = sw(w_90 + dstByteWidth - D4W - 1);
          };
          TAG0088b2((ui8 *)A3,
                    (ui8 *)d.pViewportBMP,
                    &rect_42,
                    srcOffsetX,
                    0,
                    (dstByteWidth + 7) & 0xfff8,
                    112,
                    transparentColor_84);
        };
      };
    };
//tag00600c:
    if (w_94 == 0) break;
    w_94 = 0;
    SET(D0W, IndexWithinGroup_82==0);
    D0W &= 1;
    sms_78 = pi16_62->singleMonsterStatus[D0W];
    if (positionWithinCell == 1)
    {
      positionWithinCell = 0;
    }
    else
    {
      positionWithinCell = 1;
    };
  }; //for (;;)
  return;
}


void DrawSingleObject(
                 GRAPHIC_CLASS  *pGraphicClass,
           const bool            LOCAL_34, // false when drawing missiles
           const i32             MiddleLeftRight, //26// 0, 1, or 2
                                            // 0 = Middle means a cell straight ahead.
                                            // 1 = A cell to the left
                                            // 2 = A cell to the right
           const i32             relativeLocationWithinCell, //0=far left; then clockwise
                 //bool            &LOCAL_36,
                 //i16             &LOCAL_30,
           const i32              RelativeCellNumber,
           const bool             drawingMissiles,
                 i16             &distanceFromUs_52,
                 i32             &currentSmallOffset,
                 RectPos         &rect_42,
                 i32              graphicNum_D4,
                 dReg            &D3,
                 i16             &height_LOCAL_24,
                 i16             &width_LOCAL_22,
           const bool             drawingAlcoveObjects,//Initially specified drawing order = 0
                 i16             &w_18,
                 ui8             *pub_16,
           const RN               objD7)
{
  dReg  D0, D1, D4;
  aReg  A3;
  bool  LOCAL_36;
  i16   LOCAL_30;
  pnt   pSrcGraphic_LOCAL_12;
  ui8  *LOCAL_8;
//  ui8  *uA3;
  bool  touchable;
  //i16   height_32;
  bool  rightSideOfCell, nearSideOfCell;
  i32   srcOffsetX;
  //D0W = (I16)(pI6_A2->byte4 & 1);
  //if (D0W != 0)
  //{
  //  D0W = LOCAL_34;
  //  SET(D0W, D0W==0);
  //};
  //SET(D0B, D0W!=0);
  //if (D0B != 0)
  //{
  //  SET(D0B, MiddleLeftRight==2);
  //  if (D0B == 0)
  //  {
  //    D0W = sw(MiddleLeftRight); // Middle
  //    SET(D0B, D0W==0);
  //    if (D0B != 0)
  //    {
  //      SET(D0B, D5W==1);
  //      if (D0B == 0)
  //      {
  //        SET(D0B, D5W==2);
  //      };
  //    };
  // };
  //};
  //LOCAL_36 = D0W & 1;
  rightSideOfCell = ((relativeLocationWithinCell + 1) & 2) != 0;
  nearSideOfCell  = (relativeLocationWithinCell >= 2);
  LOCAL_36 =   ((pGraphicClass->byte4 & 1) != 0) 
            && (!LOCAL_34) 
            && (
                    (MiddleLeftRight == 2)
                 || (MiddleLeftRight == 0) && rightSideOfCell
               );
  LOCAL_30 = 0;
  if (   (RelativeCellNumber ==9)      //F0
      || ( (RelativeCellNumber>=6) && nearSideOfCell ) ) // Near side of F1 row
  {
    //D0W = sw(MiddleLeftRight);
    //SET(D0B, D0W==0);
    //if (D0B != 0)
    //{
    //  SET(D0B, LOCAL_132 == 0);
    //};
    //LOCAL_38 = D0W&1;
    touchable = (MiddleLeftRight == 0) && (!drawingMissiles);
    distanceFromUs_52 = 0;
    A3 = (aReg)GetBasicGraphicAddress(graphicNum_D4);
    width_LOCAL_22 = (UI8)(pGraphicClass->width);
    height_LOCAL_24 = (UI8)(pGraphicClass->height);
    if (LOCAL_36)
    {
      MakeMirror((ui8 *)A3, (ui8 *)tempBitmap, width_LOCAL_22, height_LOCAL_24);
      A3 = (aReg)tempBitmap;
    };
  }
  else
  {
    touchable = false;
    D3W = pGraphicClass->uByte1;
    w_18 = sw(D3W + 104);

    if (   (RelativeCellNumber>=6)
        || ( (RelativeCellNumber>=3) && nearSideOfCell ) )
    {
      w_18 += 1;
      distanceFromUs_52 = 1;
      width_LOCAL_22  = sw(((UI8)(pGraphicClass->width) * 20 + 10)>>5); //times 20/32
      height_LOCAL_24 = sw((pGraphicClass->height * 20 + 10)>>5); //times 20/32
      LOCAL_8 = d.Byte5744;
    }
    else
    {
      distanceFromUs_52 = 2;
      width_LOCAL_22  = sw(((UI8)(pGraphicClass->width) * 16 + 8)>>5); // width in bytes
      height_LOCAL_24 = sw(((UI8)(pGraphicClass->height) * 16 + 8)>>5); //height
      LOCAL_8 = d.Byte5728;
    };
    if (LOCAL_36)
    {
      w_18 += 2;
      LOCAL_30 = sw((7 - ((width_LOCAL_22 - 1) & 7)) << 1);
    }
    else
    {
      if (LOCAL_34)
      {
        w_18 += 4;
      };
    };
    D0W = AllocateDerivedGraphic(w_18);
    if (D0W)
    {
      A3 = (aReg)GetDerivedGraphicAddress(w_18);
    }
    else
    {
      pSrcGraphic_LOCAL_12 = (pnt)GetBasicGraphicAddress(graphicNum_D4);
      A3 = (aReg)GetDerivedGraphicAddress(w_18);
   //   if (
      ShrinkBLT((ui8 *)pSrcGraphic_LOCAL_12,            //src
                (ui8 *)A3,                               //dest
                (UI8)(pGraphicClass->width) << 1, //src width
                (UI8)(pGraphicClass->height),     //src height
                width_LOCAL_22 << 1,              //dst width
                height_LOCAL_24,                  //dst height
                LOCAL_8);
      if (LOCAL_36)
      {
        Mirror((ui8 *)A3,
               (width_LOCAL_22+7)&0xfff8, // width
               height_LOCAL_24);      // height
      };
      //TAG022d5e(w_18);
    };
  };
  D4W = *pub_16; // Viewport X
  rect_42.b.y2 = *(pub_16+1); // Viewport Y

  if (!drawingMissiles)
  {
    D0W = d.smallOffsetIndicies[currentSmallOffset][0];
    D1L = (distanceFromUs_52 & 0xffff) << 3;
    //A0 = d.Byte6652;
    //A0 += D1L;
    //A0 += D0W;
    //D0W = *A0;
    D0W = d.smallOffsets[D1L + D0W];
    D4W = sw(D4W + D0W);
    D0W = d.smallOffsetIndicies[currentSmallOffset][1];
    D1L = (distanceFromUs_52&0xffff)<<3;
    //A0 = d.Byte6652;
    //A0 += D1L;
    //A0 += D0W;
    //D0B = *A0;
    D0W = d.smallOffsets[D1L + D0W];
    rect_42.b.y2 = ub(rect_42.b.y2 + D0B);
    currentSmallOffset++;
    if (drawingAlcoveObjects)
    {
      if (currentSmallOffset >= 14)
      {
        currentSmallOffset = 2;
      };
    }
    else
    {
      currentSmallOffset &= 15;
    };
  };
  rect_42.b.y1 = ub(rect_42.b.y2 - (height_LOCAL_24-1));
  if (rect_42.b.y2 > 135)
  {
    rect_42.b.y2 = 135;
  };
  D0W = sw(Smaller(223, D4W + width_LOCAL_22));
  rect_42.b.x2 = D0B;
  rect_42.b.x1 = (ui8)Larger(0, D4W-width_LOCAL_22+1);
  if (rect_42.b.x1 != 0)
  {
    if (LOCAL_36)
    {
      //D4W = LOCAL_30;
      srcOffsetX = LOCAL_30;
    }
    else
    {
      //D4W = 0;
      srcOffsetX = 0;
    }
  }
  else
  {
    //D4W = sw(width_LOCAL_22 - D4W - 1);
    srcOffsetX = sw(width_LOCAL_22 - D4W - 1);
  };
  if (touchable)
  {
    BUTTON_DEF *pButtonDef;
    //LOCAL_12 = A3; // Save A3 no longer necessary
    pButtonDef = &d.ViewportObjectButtons[relativeLocationWithinCell];
    if (pButtonDef->x1 == 255)
    {
      int height_32, top, bottom;
      MemMove((ui8 *)&rect_42, (ui8 *)pButtonDef, 4);
      bottom = pButtonDef->y2; //uA3[3];
      top = pButtonDef->y1;    //uA3[2];
      height_32 = sw(bottom - top);
      if (height_32 < 14)
      {
        height_32 >>= 1;
        //uA3[2] = ub(uA3[2] + height_LOCAL_24 - 7);
        pButtonDef->y1 = ub(pButtonDef->y1 + height_LOCAL_24 - 7);
        if (height_32 < 4)
        {
//          uA3[3] = ub(uA3[3] - (height_32 - 3));
          pButtonDef->y2 = ub(pButtonDef->y2 - (height_32 - 3));
        };
      };
    }
    else
    {
      //uA3[0] = ub(Smaller(uA3[0], rect_42.b.x1));
      //uA3[1] = ub(Larger(rect_42.b.x2, uA3[1]));
      //uA3[2] = ub(Smaller(uA3[2], rect_42.b.y1));
      //uA3[3] = ub(Larger(uA3[3], rect_42.b.y2));
      pButtonDef->x1 = ub(Smaller(pButtonDef->x1, rect_42.b.x1));
      pButtonDef->x2 = ub(Larger(rect_42.b.x2, pButtonDef->x2));
      pButtonDef->y1 = ub(Smaller(pButtonDef->y1, rect_42.b.y1));
      pButtonDef->y2 = ub(Larger(pButtonDef->y2, rect_42.b.y2));
    };
    //A3 = LOCAL_12; //Restore A3 no longer necessary
    d.rn10574[relativeLocationWithinCell] = objD7;
  };
  TAG0088b2((ui8 *)A3,
            (ui8 *)d.pViewportBMP,
            &rect_42,
            //D4W,   //srcOffsetX
            srcOffsetX,
            0,     //srcOffsetY
            (width_LOCAL_22 + 7) & 0xfff8,
            112,
            10);
  //if (LOCAL_132) goto tag006584;
  return;
}



void ProcessOneObject(
          const RN              objD7,
                RectPos        &rect_42,
          const dReg            D5,
          const i32             RelativeCellNumber,
          const i32             MiddleLeftRight,
                GRAPHIC_CLASS* &pGraphicClass,
          const bool            drawingAlcoveObjects,
          const bool            LOCAL_132,
                i16            &distanceFromUs_52,
                i32            &currentSmallOffset,
                //bool           &LOCAL_36,
                //i16            &LOCAL_34,
                //i16            &LOCAL_30,
                i16            &LOCAL_24,
                i16            &LOCAL_22,
                i16            &w_18,
                ui8*           &pub_16)
{
  dReg D0, D3, D4;
  bool LOCAL_34;
  // Loop here for each object in the room.
  ASSERT(objD7 != RNnul,"RNnul");
  D0W = D4W = sw(objD7.dbType());
//tag0052e6:
//tag0052f6:
  //D0W = GetObjectDescIndex(objD7);
  D0L = d.ObjDesc[objD7.DescIndex()].GraphicClass();
  if (D0W > 84)
  {
    UI_MessageBox("ObjectDesc byte 2 is too large.\n"
                  "Graphics file error??",
                  "Graphic Error",
                  MESSAGE_OK);
    die(0x5c3a);
  };
  pGraphicClass = &d.s5604[D0W];
  //D4W = 360;
  D3W = (UI8)(pGraphicClass->objectGraphicIndex);
  D4L = 360 + D3W; // Graphic number?
  //if (drawingAlcoveObjects)
  //{
  //  D0W = (I16)(pGraphicClass->byte4 & 16);
  //};
  //SET(D0B, (drawingAlcoveObjects)&&(D0W!=0));
  //if (D0B != 0)
  //{
  //  D0W = sw(MiddleLeftRight); // Middle
  //  SET(D0B, D0W==0);
  //};
  //LOCAL_34 = (I16)(D0W & 1);
  LOCAL_34 = drawingAlcoveObjects && ((pGraphicClass->byte4 & 16) != 0) && (MiddleLeftRight == 0);
  if (LOCAL_34)
  {
    D4W++;
  };
  //D0 = 100 * (UI8)(psA2->byte5);
  //D0 += d.Byte6090;
  pub_16 = (ui8 *)&d.viewportPosition[pGraphicClass->byte5].s[RelativeCellNumber].byte0[2*D5W];
  //A0 += 2*D5W;
  //pnt_16 = A0;
  if (pub_16[1] == 0) return;
////tag0053a0:
  DrawSingleObject(
              pGraphicClass,
              LOCAL_34,
              MiddleLeftRight, //26// 0, 1, or 2
              D5W,
              //LOCAL_36,
              //LOCAL_30,
              RelativeCellNumber,
              LOCAL_132,
              distanceFromUs_52,
              currentSmallOffset,
              rect_42,
              D4W,
              D3,
              LOCAL_24,
              LOCAL_22,
              drawingAlcoveObjects,//Initially specified drawing order = 0
              w_18,
              pub_16,
              objD7);
}


//ALTMONGRAPH altMonGraph;


void ProcessOnePosition(
              const i32           mapX,
              const i32           mapY,
              const bool          drawingAlcoveObjects,
                    ui16         &CurAbsPos,
              const i32           facing,
                    i32          &PositionList,
                    i16          &PositionListIndex,
              const RN            FirstObject,
                    i32          &RelativeCellNumber,
              const i16           RelativeCell_46,
                    i16          &MissilePresent,
                    i16          &LOCAL_158,
              const i32           MiddleLeftRight,
              const SIDE_OF_DOOR  sideOfDoor,
                    i16          &LOCAL_172,
                    bool         FinishedDrawingMonsters_108
              )
{
  dReg            D0, D1, D3, D4, D5;
  aReg            A0, A3;
  GRAPHIC_CLASS  *pGraphicClass;//pI6_A2;
  RN              objD7;
  //static ITEM16 staticITEM16;
  //ITEM12 *pI12_A2;
  //MONSTERDESC *pmtDesc;
  i16     w_192;
  //i16     MonsterAbsFacing_182;//0=north, 1=east, etc
  i16     w_142;
  i16     w_140;
  i16     w_138=-1;
  //i16     LOCAL_132;
  i16     w_130;
  i16     w_128;
  i16     w_126;
  i16     w_124;
  i16     w_122 = -1;
  i8      b_120[2];
  DB14   *pDB14_118;
  i16     w_112;
  //i16     LOCAL_96;
  i16     w_94;
  //i16     IndexWithinGroup_82;
  SINGLE_MONSTER_STATUS  sms_78;
  //i16     w_76=-1;
  //i16     MonsterRelFacing_74;//0=away,1=right,2=toward,3=left
  //i16     MonsterSize_72=-1;
  //ITEM12 *pI12_70 = NULL;
  //ITEM16 *pi16_62 = NULL;
  //DB4    *pDB4_58 = NULL;
  RN      objMonster_54(RNnul);
  i16     distanceFromUs_52 = -1;
  i32     currentSmallOffset;
  RectPos rect_42;
  bool    LOCAL_36;
  //i16     LOCAL_34;
  i16          LOCAL_30;
  i16          LOCAL_24;
  i16          LOCAL_22;
  i16          w_18 = -1;
  ui8         *pub_16;
  pnt          LOCAL_12;
  i16          CurrentRelativePosition;
  STRUCT5688  *pStruct5688;
  DB4         *pMonster = NULL;
  i32          alternateMonsterGraphics = 0;
  ALTMONGRAPH *pAltMonsterGraphic;
  //bool    drawAsSize4;
  //drawAsSize4 = false; //Pretend any monster we find here is
  //                     //a big dragon-like monster and that the
  //                     //group consists of only one monster.
  if (drawingAlcoveObjects)
  {
    D5W = 4;
    CurAbsPos = (I16)((facing+2) & 3);
    currentSmallOffset = 2;
    CurrentRelativePosition = -1;
  }
  else
  {
    D5W = (I16)((PositionList & 15) - 1);
    CurrentRelativePosition = D5W;
    PositionList >>= 4;//Put next nibble at bottom
    PositionListIndex++;//Advance to next nibble
    CurAbsPos = (I16)((D5W + facing) & 3);//position in cell
    RelativeCellNumber = RelativeCell_46;
    currentSmallOffset = 0;
  };
  currentSmallOffset += (CurAbsPos & 1) << 3;
  //LOCAL_132 = 0;
//NextObject:
  for (objD7=FirstObject; objD7!=RNeof; objD7=GetDBRecordLink(objD7))
  {
    DBTYPE dbType;
    dbType = objD7.dbType();
    switch (dbType)
    {
    case dbMONSTER:
      pMonster = GetRecordAddressDB4(objD7);
      if (!pMonster->invisible())
      {
        objMonster_54 = objD7;
        alternateMonsterGraphics = pMonster->alternateGraphics();
      };
      break;
    case dbTELEPORTER:
    case dbDOOR:
    case dbTEXT:
    case dbACTUATOR:
      break;
    case dbMISSILE:
      MissilePresent = 1; // Set if db==14 (fireball, eg)
      break;
    case dbCLOUD:
      LOCAL_158 = 1;
      break;
    default:
      if (RelativeCellNumber < 0) break;
      if (RelativeCellNumber > 9) break;
      if (CurAbsPos != objD7.pos()) break;
      ProcessOneObject(
                    objD7,
                    rect_42,
                    D5,
                    RelativeCellNumber,
                    MiddleLeftRight,
                    pGraphicClass,
                    drawingAlcoveObjects,
                    false, //LOCAL_132,
                    distanceFromUs_52,
                    currentSmallOffset,
                    //LOCAL_36,
                    //LOCAL_34,
                    //LOCAL_30,
                    LOCAL_24,
                    LOCAL_22,
                    w_18,
                    pub_16);
      break;
    }; //switch (dbType)
//tag0057d0:
  };
  // We have drawn all objects at this position.  If we saw
  // a monster then perhaps we should draw it now.
  // D5 should have the current relative position being
  // drawn.  0=far left, 1 = far right, etc.
//PositionCompleted:
//  if ( (D5W == 4) || (RelativeCellNumber < 0) )goto tag0065bc;
  if ( (D5W == 4) || (RelativeCellNumber < 0) )
  {
    PositionList = 0; //Force exit from DrawRoomObjects()
    return;
  };
  pAltMonsterGraphic = GetAlternateGraphicsRecord(pMonster);
  if (pAltMonsterGraphic != NULL) 
  {
    DrawMonster(facing,
                RelativeCellNumber,
                CurAbsPos,
                D5W, //Positon within Cell
                sideOfDoor,
                FinishedDrawingMonsters_108,
                sms_78,
                objMonster_54,
                distanceFromUs_52,
                //w_18,
                rect_42,
                MiddleLeftRight,
                w_94,
                PositionList,
                PositionListIndex,
                pAltMonsterGraphic);
  }
  else
  {
    DrawMonster(facing,
                RelativeCellNumber,
                CurAbsPos,
                D5W, //Positon within Cell
                sideOfDoor,
                FinishedDrawingMonsters_108,
                sms_78,
                objMonster_54,
                distanceFromUs_52,
                //w_18,
                rect_42,
                MiddleLeftRight,
                w_94,
                PositionList,
                PositionListIndex);
  };
////CheckNextPosition:
//
//if (   (MissilePresent==0)
//    || ((RelativeCellNumber = RelativeCell_46)>9) ) goto tag0065b4;
  if (   (MissilePresent==0)
      || ((RelativeCellNumber = RelativeCell_46)>9) ) return; //finished this position
  D5W = CurrentRelativePosition;
  A0 = &d.viewportPosition[0].s[RelativeCellNumber].byte0[2*D5W];//roomobject[relativeRoom][position]
  D0W = *((ui8 *)A0);
  w_122 = D0W; // x position within viewport
//if (w_122 == 0) goto tag0065b4;
  if (w_122 == 0) return;  //finished this position
  for (objD7 = FirstObject; objD7 != RNeof; objD7 = GetDBRecordLink(objD7))
  {
//tag006080:
    if (objD7.dbType() != dbMISSILE) continue; // fireball is 14
    if (objD7.pos() != CurAbsPos) continue;
    pDB14_118 = GetRecordAddressDB14(objD7);
    D4L = GetGraphicClass(pDB14_118->flyingObject());
    if (D4W >= 0) goto tag006548;
    //D0L = -6 * D4W;
    pStruct5688 = &d.struct5688[-D4W-1]; //&d.Byte5688[-6*D4W-6];
    //D4W = sw((UI8)(*A2) + 316);
    D4L = pStruct5688->uByte0 + 316;
    //w_126 = (I16)(LE16(wordGear(A2+4)) & 3);
    w_126 = (I16)(LE16(pStruct5688->word4) & 3);
    //SET(D0W, (LE16(wordGear(A2+4))&0x100)==0);
    SET(D0W, (LE16(pStruct5688->word4) & 0x100) == 0);
    w_130 = (I16)(D0W & 1);
//
//
    if (   ((w_130!=0) || (pDB14_118->rangeRemaining()==255))
        && (RelativeCellNumber==9) )
    {
      w_112 = 0;
      //LOCAL_22 = (UI8)(A2[2]);
      LOCAL_22 = pStruct5688->width;
      //LOCAL_24 = (UI8)(A2[3]);
      LOCAL_24 = pStruct5688->height;
    }
    else
    {
      distanceFromUs_52 = sw(2 * (RelativeCellNumber / 3) + D5W/2);
      w_112 = (UI8)(d.Byte5752[distanceFromUs_52]);
      if (w_130 == 0)
      {
        w_192 = sw(Larger(96, pDB14_118->rangeRemaining()+1));
        w_112 = sw((w_112 * w_192) >> 8);
      };
      //D0W = sw((UI8)(A2[2]) * w_112);
      D0W = sw(pStruct5688->width * w_112);
      D1W = sw(w_112 >> 1);
      LOCAL_22 = sw((D0W + D1W) >> 5);
      //D0W = sw((UI8)(A2[3]) * w_112);
      D0W = sw(pStruct5688->height * w_112);
      D1W = sw(w_112 >> 1);
      LOCAL_24 = sw((D0W + D1W) >> 5);
    };
    SET(D0W, w_126==0);
    w_140 = (I16)(D0W & 1);
    if (w_140 != 0)
    {
      w_138 = (I16)((mapX + mapY) & 1);
    };
    if (w_126 == 3) //a
    {
      w_128 = 0;
      LOCAL_36 = 0;
      w_142 = 0;
    }
    else //a
    {
      D0W = pDB14_118->timerIndex();
      D0W = gameTimers.pTimer(D0W)->timerWord8();
      w_124 = BITS10_11(D0W);
      D0W = (I16)(w_124 & 1);
      D1W = (I16)(facing & 1);
      if (D1W != D0W) //b
      {
        if (w_126 == 2)
        {
          w_128 = 1;
        }
        else
        {
          w_128 = 2;
        };
        D0W = w_140;
        if (D0W != 0) //c
        {
          SET(D0W, D5W==0);
          if (D0B == 0)
          {
            SET(D0W, D5W==3);
          };
          LOCAL_36 = D0W & 1;
          w_142 = w_138;
          if (w_142 == 0)
          {
            SET(D0W, LOCAL_36==0);
            LOCAL_36 = D0W & 1;
          };
        }
        else //c
        {
          w_142 = 0;
          SET(D0W, ((facing+1)&3)==w_124);
          LOCAL_36 = D0W & 1;
        };
      }
      else //b
      {
        if (    (w_126 >= 2)
            || (w_126 == 1) && (w_124 != facing)
            || (w_140 != 0) && (w_138 != 0))
        {
          w_128 = 0;
        }
        else
        {
          w_128 = 1;
        };
        /*
        if (w_126 >= 2) goto tag0062d6;
        if (w_126 != 1) goto tag0062ca;
        if (w_124 != facing) goto tag0062d6;
  tag0062ca:
        if (w_140 == 0) goto tag0062dc;
        if (w_138 == 0) goto tag0062dc;
  tag0062d6:
        w_128 = 0;
        goto tag0062e2;
  tag0062dc:
        w_128 = 1;
  tag0062e2:
  */
        D0W = w_140;
        if (D0W != 0)
        {
          SET(D0W, D5W < 2);
        };
        SET(D0W, D0W!=0);
        w_142 = (I16)(D0W & 1);
        //D0W = wordGear(A2+4);
        D0W = pStruct5688->word4;
        if (D0W != 0)
        {
          SET(D0W, MiddleLeftRight==2); // right side
          if (D0B == 0)
          {
            SET(D0W, MiddleLeftRight==0);
            if (D0B != 0)
            {
              SET(D0W, D5W==1);
              if (D0B == 0)
              {
                SET(D0W, D5W==2);
              };
            };
          };
          SET(D0W, D0B==0);
        };
        SET(D0W, D0W!=0);
        LOCAL_36 = D0W & 1;
      }; //b
    }; //a
    D4W = sw(D4W + w_128);
    LOCAL_30 = 0;
    if (w_112 == 0)
    {
      A3 = (pnt)GetBasicGraphicAddress(D4W);
    }
    else
    {
      if (LOCAL_36 != 0)
      {
        LOCAL_30 = sw((7 - ((LOCAL_22-1)&7))<<1);
      };
      if (w_130 != 0)
      {
        //D3W = (UI8)(A2[1]);
        D3W = pStruct5688->uByte1;
        D0W = sw(282 + D3W);
        D0W = sw(D0W + 6*w_128 + distanceFromUs_52);
        w_18 = D0W;
        D0W = AllocateDerivedGraphic(w_18);
        if (D0W==0) goto tag0063be;
        A3 = (aReg)GetDerivedGraphicAddress(w_18);
      }
      else
      {
  tag0063be:
        LOCAL_12 = (pnt)GetBasicGraphicAddress(D4W);
        if (w_130 != 0)
        {
          A3 = (aReg)GetDerivedGraphicAddress(w_18);
        }
        else
        {
          A3 = (i8 *)tempBitmap;
        };
        D0W = sw(distanceFromUs_52 >> 1);
        ShrinkBLT((ui8 *)LOCAL_12,
                  (ui8 *)A3,
                  //(UI8)(A2[2])<<1,
                  pStruct5688->width<<1,
                  //(UI8)(A2[3]),
                  pStruct5688->height,
                  LOCAL_22 <<1,
                  LOCAL_24,
                  d.Pointer1864[D0W]);
        if (w_130 != 0)
        {
          //TAG022d5e(w_18);
        };
      };
    };
  
    if ( (LOCAL_36!=0) || (w_142!=0) )
    { 
      D4W = (I16)((LOCAL_22 + 7) & 0xfff8);//width in bytes?
      if (A3 != (i8 *)tempBitmap)
      {
        MemMove((ui8 *)A3, (ui8 *)tempBitmap, D4W * LOCAL_24);
        A3 = (i8 *)tempBitmap;
      };
      if (w_142 != 0)
      {
        TAG008840((ui8 *)A3, D4W, LOCAL_24); // pnt, width, height??
      };
      if (LOCAL_36 != 0)
      {
        Mirror((ui8 *)A3, D4W, LOCAL_24);//pnt,width,height??
      };
    };
    rect_42.b.y2 = ub(47 + (LOCAL_24>>1));
    D0W = sw(47 - (LOCAL_24 >> 1));
    D1W = (I16)(LOCAL_24 & 1);
    SET(D1W, (LOCAL_24&1)==0);
    rect_42.b.y1 = ub(D0W + (D1W&1));
    rect_42.b.x2 = ub(Smaller(223, w_122 + LOCAL_22));
    rect_42.b.x1 = ub(Larger(0, w_122 - LOCAL_22 +1));
    if (rect_42.b.x1 != 0)
    {
      if (LOCAL_36 != 0)
      {
        D4W = LOCAL_30;
      }
      else
      {
        D4W = 0;
      };
    }
    else
    {
      D4W = sw(Larger(LOCAL_30, LOCAL_22 - w_122 -1));
    };
    TAG0088b2((ui8 *)A3,
              (ui8 *)d.pViewportBMP,
              &rect_42,
              D4W,
              0,
              (LOCAL_22+7)&0xfff8,
              112,
              10);
    continue;
  tag006548:
    //LOCAL_34 = 0;
    b_120[0] = (i8)w_122;
    b_120[1] = 47;
    pub_16 = (ui8 *)b_120;
    pGraphicClass = &d.s5604[D4W];
    D4W = sw((UI8)(pGraphicClass->objectGraphicIndex) + 360);
    //LOCAL_132 = 1;
    //goto tag0053a0;
    DrawSingleObject(
                pGraphicClass,
                0,//LOCAL_34,
                MiddleLeftRight, //26// 0, 1, or 2
                D5W,
                //LOCAL_36,
                //LOCAL_30,
                RelativeCellNumber,
                true, //LOCAL_132,
                distanceFromUs_52,
                currentSmallOffset,
                rect_42,
                D4W,
                D3,
                LOCAL_24,
                LOCAL_22,
                drawingAlcoveObjects,//Initially specified drawing order = 0
                w_18,
                pub_16,
                objD7);
  // tag006584:
    //objD7 = GetDBRecordLink(objD7);
    //if (objD7 != RNeof) goto tag006080;
  };
  if (LOCAL_172 != 0)
  {
    LOCAL_172 = 0;
    d.Word1836 += 6;
    if (d.Word1836 > 16)
    {
      d.Word1836 -= 3;
    };
  };
//tag0065b4:
}







// *********************************************************
//
// *********************************************************
//   TAG0051a4
void DrawRoomObjects(const RN firstObject,
                     const i32 facing,
                     const i32 mapX,
                     const i32 mapY,
                           i32 RelativeCellNumber,
                           i16 DrawingOrder)
{ // Drawing Order executed four bits at a time from
  // the bottom. Numbered from 1 clockwise starting in
  // far left corner.
  //
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	dReg           D0;
  aReg           A3;
  //ITEM6         *pI6_A2;
  //ITEM12        *pI12_A2;
  //ITEM110       *pI110_A0;
  //ITEM110s      *pI110s_A0;
  //RN             objD7;
  //i16            w_192;
  //i16            MonsterAbsFacing_182;//0=north, 1=east, etc
  i16            LOCAL_172;
  RectPos        b_170;
  //i16            w_162;
  //i16            w_160;
  i16            LOCAL_158=0;
  //i16            w_156;
  //i16           *pw_154;
  DBCOMMON      *pdb_150;
  //DB15          *pDB15_146;
  //i16            w_142;
  //i16            w_140;
  //i16            w_138=-1;
  //i16            CurrentRelativePosition=-1;
  //i16            LOCAL_132;
  i16            MissilePresent;//134;
  //i16            w_130;
  //i16            w_128;
  //i16            w_126;
  //i16            w_124;
  //i16            w_122=-1;
  //i8             b_120[2];
  //DB14          *pDB14_118;
  //i16            w_114;
  //i16            w_112;
  SIDE_OF_DOOR     sideOfDoor;
  bool             FinishedDrawingMonsters_108=false;
  //i16            w_106;
  //i16            w_104;
  //i16            w_102=-1;
  //i16            w_100;
  //i16            w_98;
  //i16            LOCAL_96; // boolean value?
  //i16            w_94;
  //i16            w_90;
  //i16            w_88;
  //i16            w_86;
  //i16            w_84;
  //i16            IndexWithinGroup_82;
  //i16            w_78;
  //i16            w_76=-1;
  //i16            MonsterRelFacing_74;//0=away,1=right,2=toward,3=left
  //i16            MonsterSize_72=-1;
  //ITEM12        *pI12_70=NULL;
  //MONSTERDESC   *pmtDesc;
  //ITEM16        *pi16_62 = NULL;
  //DB4           *pDB4_58=NULL;
  RN             objMonster_54(RNnul);
  //i16            w_52=-1;
  //i16            LOCAL_50;
  i16            PositionListIndex;//48//which nibble we are processing
  i16            RelativeCell_46;
  RN             FirstObject; //44// set to parameter 'firstObject' forever
  //RectPos        rect_42;
  //bool           LOCAL_38;
  //bool           LOCAL_36;
  //i16            LOCAL_34;
  //i16            LOCAL_32;
  //i16            LOCAL_30;
  ui16           CurAbsPos; //28//contains position
  i32            MiddleLeftRight;//26// 0, 1, or 2
  //i16            LOCAL_24; // Graphic height?
  //i16            LOCAL_22;
  bool           drawingAlcoveObjects;//Initially specified drawing order = 0
  //i16            w_18=-1;
  //ui8           *pub_16;
  //pnt            LOCAL_12;
  //pnt            LOCAL_8;
  i32            PositionList;
  A3 = NULL;
  //D7L = -1;
//#ifdef _VERIFYPPQ
//  verifyppq();
//#endif
	//objD7 = firstObject;
	//D6W = relativeCellNum;
	if (firstObject == RNeof) return;
  //pDB4_58 = NULL;
  //objMonster_54 = RN(RNnul);
  //FinishedDrawingMonsters_108 = 0;
  MissilePresent = 0;
  //LOCAL_158 = 0;
  PositionListIndex = 0; // start with rightmost nibble
  FirstObject = firstObject;
//  D0W = P6 & 8;
  if ((DrawingOrder & 8) != 0)
  {
    if (DrawingOrder & 1) sideOfDoor = SOD_NearSideOfDoor;
    else sideOfDoor = SOD_FarSideOfDoor;
    //sideOfDoor = (DrawingOrder & 1) + 1;
    DrawingOrder = (I16)((DrawingOrder>>4) & 0xfff);
  }
  else
  {
    sideOfDoor = SOD_NotADoor;
  };
  PositionList = (ui16)DrawingOrder;

  drawingAlcoveObjects = (PositionList == 0);
  RelativeCell_46 = sw(RelativeCellNumber);
  MiddleLeftRight = (RelativeCellNumber+3) % 3;
  SET(D0B, (d.Word1836 & 0xfe) == 10);
  if (D0B != 0)
  {
    D0W = 136;
  };
  SET(D0B, D0B!=0);
  LOCAL_172 = (I16)(D0W & 1);
//NextPosition:
  do
  { //Process one position within cell
  //  if (PositionList != 0) goto NextPosition;
    ProcessOnePosition(
                  mapX,
                  mapY,
                  drawingAlcoveObjects,
                  CurAbsPos,
                  facing,
                  PositionList,
                  PositionListIndex,
                  FirstObject,
                  RelativeCellNumber,
                  RelativeCell_46,
                  MissilePresent,
                  LOCAL_158,
                  MiddleLeftRight,
                  sideOfDoor,
                  LOCAL_172,
                  FinishedDrawingMonsters_108);
  } while (PositionList != 0);
//tag0065bc:
  if (LOCAL_158 == 0) return;
  pdb_150 = DrawClouds(RelativeCellNumber,
                       RelativeCell_46,
                       FirstObject,
                       CurAbsPos,
                       facing); //Moved to separate function to clean things up.
  if ( (pdb_150!=NULL) && (sideOfDoor!=SOD_FarSideOfDoor) && (d.Word1868==0) )
  {
    RelativeCellNumber -= 3;
    //MemMove(d.Byte4010 + 8*RelativeCellNumber, b_170, 8);
    MemMove((ui8 *)&d.teleporterRectangles[RelativeCellNumber], (ui8 *)&b_170, 8);
    b_170.b.x1++;
    //DrawTeleporter(&b_170, (RectPos *)(d.Byte3074 + 8*RelativeCellNumber));
    DrawTeleporter(&b_170, &d.wallRectangles[RelativeCellNumber]);
  };
  return;
  // */
}

void TAG008840(ui8 *P1,i16 width, i16 height)
{
  dReg D6, D7;
  aReg A0, A1;
  i32 D7U, dA2;
  i8 temp24[24];
  i32 temp4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = height;
  D6W = width;
  A0 = (aReg)P1;
  A1 = A0 + width * (height-1);
  dA2 = 2 * D6W;
  D6W /= 8;
  do
  {
    D7W-=2;
    if (D7W < 0) break;
    D7U = D6W;
    do
    {
      D7U -= 3;
      if (D7U < 0) break;
      memmove(temp24,(ui8 *)A0,24);
      memmove((ui8 *)A0, (ui8 *)A1, 24);
      memmove((ui8 *)A1, temp24 ,24);
      A1 += 24;
      A0 += 24;
    } while (D7U != 0);
    if (D7U < 0) // If we 'broke' out of loop.
    {
      D7U += 2;
      do
      {
        temp4 = longGear((ui8 *)A0);
        longGear((ui8 *)A0) = longGear((ui8 *)A1);
        longGear((ui8 *)A1) = temp4;
        A0 += 4;
        A1 += 4;
        temp4 = longGear((ui8 *)A0);
        longGear((ui8 *)A0) = longGear((ui8 *)A1);
        longGear((ui8 *)A1) = temp4;
        A0 += 4;
        A1 += 4;
      } while ((D7U--) != 0);
    };
    A1 -= dA2;
  } while (D7W != 0); // each line
}


i32 GetGraphicClass(RN P1)
{
  dReg  D5, D6;
  WEAPONDESC *weaponDescA3;
  RN objD7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  objD7 = P1;
  if (objD7.IsMagicSpell()) D6W = dbCLOUD;
  else  D6W = sw(objD7.dbType());
  if (D6W == dbCLOUD)
  {
    if (objD7 == RNFireball) return -11;
    if (objD7 == RNPoison) return -13;
    if (objD7 == RNLightning) return -4;
    if (objD7 == RNPoisonBolt) return -14;
    if (objD7 == RNPoisonCloud) return -14;
    return -12;
  }
  else
  {
    if (D6W == dbWEAPON)
    {
      weaponDescA3 = TAG0099d2(objD7);
      D5W = BITS8_12(weaponDescA3->word4);
      if (D5W != 0)
      {
        return sw(-D5W);
      };
    };
  };
  //D0W = GetObjectDescIndex(objD7);
  return d.ObjDesc[objD7.DescIndex()].GraphicClass();
}

//*********************************************************
//
//*********************************************************
WEAPONDESC *TAG0099d2(RN object)
{
  DB5 *pDB5_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ASSERT(object.dbType() == dbWEAPON,"Weapon");
  pDB5_4 = (DB5 *)GetCommonAddress(object);
  return &d.weapons[pDB5_4->weaponType()];
}
