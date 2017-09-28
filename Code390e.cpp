#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

//*********************************************************
//
//*********************************************************
//          _TAG00390e
void ReadGraphicsForLevel()
{//()
  static dReg D0, D1, D3, D5, D6, D7;
  static aReg A0, A3;
  static i16 LOCAL_22;
  static i16 LOCAL_20;
  static i16 LOCAL_18;
  static i16 LOCAL_16;
  static i16 LOCAL_14;
  static i16 LOCAL_10;
  static i16 LOCAL_8;
  static ui8 ub_6[2];
  static i16 *pwLOCAL_4;
  static LEVELDESC *pA0;
  static CELLFLAG *pdA0;
  static ITEM12 *pI12A3;
  static i16 *pGraphicsList;
  i32 numGraphic;
  //RESTARTMAP
    //RESTART(1)
    //RESTART(2)
  //END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A0=NULL;
   //SaveRegs(0x0f30);
  //A3=NULL; //avoid warning
  LOCAL_10 = 0;
  if (d.Word7254 != 0) LOCAL_10 = 1; // If we've never been here before!
  pA0 = d.pCurLevelDesc;
  LOCAL_22 = 0;
  if (LOCAL_10 != 0)      LOCAL_22 = 1;
  if (d.CurrentFloorAndCeilingGraphic 
             != d.pCurLevelDesc->backgroundGraphicSet()) LOCAL_22 = 1;
  if (d.CurrentWallGraphic 
             != d.pCurLevelDesc->wallGraphicSet()) LOCAL_22 = 1;
//
  if (LOCAL_22 != 0)
  {
    InsertDisk(0,0);
    openGraphicsFile();
  };
  //else
  //{ // 23228 is initialized to zero and never changes.
  //  while (d.Word23228 != 0) {};
  //};
  pGraphicsList = (i16 *)allocateMemory(1126,0);
  numGraphic = 0;
  MemMove((ui8 *)d.DefaultGraphicList, (ui8 *)pGraphicsList, 140);
  numGraphic += 70;
  ReadFloorAndCeilingBitmaps(d.pCurLevelDesc->backgroundGraphicSet());
  ReadWallBitmaps(d.pCurLevelDesc->wallGraphicSet());
  //d.Pointer1848 = allocateMemory(5112, 0);
  AllocateTempBitmap(5112);
  d.UseByteCoordinates = 1;
  d.Pointer2234 = (pnt)d.pWallBitmaps[4];
  MakeMirror((ui8 *)d.Pointer2234,
             (ui8 *)tempBitmap,
             d.wallRectangles[0].b.uByte4,  //(UI8)(d.Byte3074[4]),
             d.wallRectangles[0].b.uByte5); //(UI8)(d.Byte3074[5]));
  Flood((ui8 *)d.Pointer2214, 10, 0x198);
  TAG0088b2((ui8 *)tempBitmap,    //src
            (ui8 *)d.Pointer2214,    //dest
            (RectPos *)d.Byte2958,       //rectPos
            11,               //src x offset
            0,                //src y offset
            64,               //src byte width
            64,               //dst byte width
            -1);
  d.Pointer2238 = (pnt)d.pWallBitmaps[3];
  MakeMirror((ui8 *)d.Pointer2238,
             (ui8 *)tempBitmap,
             d.wallRectangles[3].b.uByte4,  //(UI8)(d.Byte3050[4]),
             d.wallRectangles[3].b.uByte5); //(UI8)(d.Byte3050[5]));
  Flood((ui8 *)d.Pointer2218,10,0x27f);
  TAG0088b2((ui8 *)tempBitmap,     // src
            (ui8 *)d.Pointer2218,     // dst
            (RectPos *)d.Byte2962,        // rectPos
            8,                 // src x offset
            0,                 // src y offset
            72,                // src byte width
            72,                // dst byte width
            -1);
  //FreeTempMemory(5112);
  d.Pointer2242 = (pnt)d.pWallBitmaps[2];
  MakeMirror((ui8 *)d.Pointer2242,
             (ui8 *)d.Pointer2222,
             d.wallRectangles[6].b.uByte4,  //d.uByte3026[4],
             d.wallRectangles[6].b.uByte5); //d.uByte3026[5]);
  d.Pointer2246 = (pnt)d.pWallBitmaps[1];
  MakeMirror((ui8 *)d.Pointer2246,
             (ui8 *)d.Pointer2230,
             d.wallRectangles[10].b.uByte4,  //d.uByte2994[4],
             d.wallRectangles[10].b.uByte5); //d.uByte2994[5]);
  d.Pointer2250 = (pnt)d.pWallBitmaps[0];
  MakeMirror((ui8 *)d.Pointer2250,
             (ui8 *)d.Pointer2226,
             d.wallRectangles[10].b.uByte4,  //d.uByte2994[4],
             d.wallRectangles[10].b.uByte5); //d.uByte2994[5]);
  pGraphicsList[numGraphic++] = 40;
  pGraphicsList[numGraphic++] = 26;
  pGraphicsList[numGraphic++] = 27;
  pwLOCAL_4 = &d.StairFacingUpGraphic[6]; //&d.Word1872;
  D6W = d.pCurLevelDesc->wallGraphicSet();
  D6L = D6W * 18 + 90;
  D7W = sw(D6W + 18);
  while (D6W < D7W)
  {

    D0W = D6W;
    D6W++;
    *(pwLOCAL_4--) = D0W;
  };
  D7W = 556;
  D6W=533;
  while (D6W < D7W)
  {
    pGraphicsList[numGraphic++] = D6W++;

  };
  for (D6W=0; D6W<85; D6W++)
  {
    //D5W = 360;
    D3W = (UI8)(d.s5604[D6W].objectGraphicIndex);
    D5L = D3W + 360;
    pGraphicsList[numGraphic++] = D5W;
    D0W = (UI8)(d.s5604[D6W].byte4);
    if (D0W & 0x10)
    {
      pGraphicsList[numGraphic++] = sw(D5W+1);
    };
  };

  for (D6W=0; D6W<14; D6W++)
  {
    //D5W = 316;
    //D3W = *(ui8 *)(d.Byte5688 + 6*D6W);
    D3W = d.struct5688[D6W].uByte0;
    D5L = D3W + 316;
    pGraphicsList[numGraphic++] = D5W;
    //D0W = LE16(wordGear(d.Byte5688 + 6*D6W + 4));
    D0W = LE16(d.struct5688[D6W].word4);
    D7W = (I16)(D0W & 3);
    if (D0W != 3)
    {
      pGraphicsList[numGraphic++] = sw(D5W+1);
      if (D7W != 2)
      {
        pGraphicsList[numGraphic++] = sw(D5W+2);
      };
    };

  };

  D6W = 348;
  while (D6W < 360)
  {
    pGraphicsList[numGraphic++] = D6W++;

  };
  ub_6[0] = d.pCurLevelDesc->Door1Type();
  ub_6[1] = d.pCurLevelDesc->Door2Type();
  for (D5W=0; D5W<=1; D5W++)
  {
    //pwLOCAL_4 = d.Word1910 + D5W;
    pwLOCAL_4 = &d.DoorGraphic[2][D5W];
    D6W = 108;
    D1W = ub_6[D5W];
    D6W = sw(D6W + 3*D1W);
    D7W = sw(D6W + 3);
    while (D6W < D7W)
    {
      D0W = D6W;
      D6W++;
      *pwLOCAL_4 = D0W;
      pwLOCAL_4 -= 2;

    };

  };
  fillMemory(d.AlcoveGraphicIndices, 3, -1, 2);
  fillMemory(d.FountainGraphicIndex, 1, -1, 2);
  LOCAL_18 = 0;
  LOCAL_20 = 0;
  d.OrnateAlcoveGraphicIndex = -1;
  for (D5W=0; D5W<=d.pCurLevelDesc->NumWallDecoration(); D5W++)
  {
    DECORATION_DESC *pWDP; //Wall decoration position
    i32 decorationIndex;
    //D6W = 121;
    decorationIndex = d.WallDecorationTOC[D5W]; //From list of decorations for this level
    //D7W = decorationIndex;
    //D6W = sw(D6W + 2*decorationIndex);
    d.WallDecorationsForLevel[D5W][0] = (ui16)(121 + 2*decorationIndex);
    // Times 2 because each wall decoration has 2 graphics.
    for (LOCAL_16=0; LOCAL_16<3; LOCAL_16++)
    {
      D1W = (UI8)(d.Byte4038[LOCAL_16]); // Is it an alcove?
      if (decorationIndex == D1W)
      {
        D0W = LOCAL_18;
        LOCAL_18++;
        d.AlcoveGraphicIndices[D0W] = D5W;
        if (decorationIndex == 2) d.OrnateAlcoveGraphicIndex = D5W;
      };

    };
    for (LOCAL_16=0; LOCAL_16<1; LOCAL_16++)
    {
      if (decorationIndex == d.Word4040[LOCAL_16])
      {
        //d.Word10412[LOCAL_20++] = D5W;
        //There is only room for ONE fountain at d.FountainGraphicIndex
        d.FountainGraphicIndex[LOCAL_20] = D5W;
      };

    };
    D0B = d.Byte4100[decorationIndex];
    D0W &= 0xff;
    d.WallDecorationsForLevel[D5W][1] = D0W;
    //D0L = D0W * 78;
    //A3 = d.Byte4836[0][0] + D0W;
    pWDP = &d.wallDecorationPosition[D0W][0];
    D7W = 4;
    D1L = D5W * 4;
    D7W = sw(D7W + D1W);
    D6W = D7W;
    D7W += 4;
    while (D6W < D7W)
    {
      DeleteGraphic(D6W | 0x8000);
      //D0W = (UI8)A3[4]);
      //D1W = (UI8)(A3[5]);
      D0W = pWDP->width;
      D1W = pWDP->height;
      D0L = D0W * D1W;
      d.SetDerivedGraphicSize(D6W++, D0W);//d.pwDerivedGraphicSizes[D6W++] = D0W;
      D0W = sw(D7W - D6W);
      //if (D0W == 2) D0W = 18;
      //else D0W =12;
      //A3 += D0W;
      if (D0W == 2) D0W = 3;
      else D0W =2;
      pWDP += D0W;

    };

  };
  for (D5W=0; D5W < d.pCurLevelDesc->NumFloorDecorations(); D5W++)
  {
    D6W = 247;
    D1W = D7W = (UI8)(d.FloorDecorationTOC[D5W]);
    D6W = sw(D6W + 6*D1W);
    //wordGear(&d.Byte2446 + 4*D5W) = D6W;
    d.LoadedFloorDecorations[D5W][0] = D6W;
    D0B = (UI8)(d.Byte4110[D7W]);
    //wordGear(&d.Byte2446 + 4*D5W + 2) = D0W;
    d.LoadedFloorDecorations[D5W][1] = D0W;

  };
  for (D5W=0; D5W < d.pCurLevelDesc->NumDoorDecorations(); D5W++)
  {
    D6W = 303;
    D7W = (UI8)(d.DoorDecorationTOC[D5W]);
    D6W = sw(D6W + D7W);
    //wordGear(d.Byte2514 + D5W*4) = D6W;
    d.Word2514[D5W][0] = D6W;
    D0W = (UI8)(d.Byte4122[D7W]);
    //wordGear(d.Byte2512 + D5W*4) = D0W;
    d.Word2514[D5W][1] = D0W;
    A3 = d.Byte5070 + 18*D0W;
    D7W = 68;
    D7W = sw(D7W + 2*D5W);
    D6W = D7W;
    D7W += 2;
    while (D6W < D7W)
    {
      DeleteGraphic(D6W | 0x8000);
      D0W = (UI8)(A3[4]);
      D1W = (UI8)(A3[5]);
      D0L = D0W*D1W;
      d.SetDerivedGraphicSize(D6W++, D0W);//d.pwDerivedGraphicSizes[D6W++] = D0W;
      A3 += 6;

    };
//
  };
  D7W = 102;
  for (D6W=0; D6W<1; D6W++)
  {
    D0W = sw(D6W + 315);
    pGraphicsList[numGraphic++] = D0W;
    D0W = (UI8)(d.Byte4124[D6W]);
    D0L = D0W*24;
    A3 = d.Byte5088 + D0W;
    D0W = (UI8)(A3[4]);
    D1W = (UI8)(A3[5]);
    D0L = D0W*D1W;
    DeleteGraphic(D7W | 0x8000);
    d.SetDerivedGraphicSize(D7W++, D0W);//d.pwDerivedGraphicSizes[D7W++] = D0W;
    A3 += 6;
    D0W = (UI8)(A3[4]);
    D1W = (UI8)(A3[5]);
    D0L = D0W*D1W;
    DeleteGraphic(D7W | 0x8000);
    d.SetDerivedGraphicSize(D7W++, D0W);//d.pwDerivedGraphicSizes[D7W++] = D0W;

  };
  ModifyPalettes(9, 8);// color #, color scheme
  ModifyPalettes(10,12); //color#, color scheme
  for (LOCAL_8=0;
       LOCAL_8 < d.pCurLevelDesc->numMonsterType();
       LOCAL_8++)
  {
    pdA0 = LOCAL_8 + d.EndOfCELLFLAGS;
    D7W = D0W = (UI8)(*pdA0);
    //D0L = 12*D0W;
    pI12A3 = &d.Item6414[D7W];
    D5W = (I16)(pI12A3->uByte11 & 15);
    if (D5W != 0)
    {
      ModifyPalettes(9, D5W-1);//color#, color scheme
    };
    D5W = pI12A3->uByte11;
    D5W = (I16)((D5W >> 4) & 0x0f);
    if (D5W != 0)
    {
      ModifyPalettes(10,D5W-1);//color#, color scheme
    };
  };
  if (LOCAL_10 != 0)
  {
    for (D5W=90; D5W<108; D5W++)
    {
      pGraphicsList[numGraphic++] = D5W;
    };
    for (D5W = 108; D5W<120; D5W++)
    {
      pGraphicsList[numGraphic++] = D5W;
    };
    for (D5W=121; D5W<247; D5W++)
    {
      pGraphicsList[numGraphic++] = D5W;
    };
    for (D5W=247; D5W<301; D5W++)
    {
      pGraphicsList[numGraphic++] = D5W;
    };
    for (D5W = 301; D5W<316; D5W++)
    {
      pGraphicsList[numGraphic++] = D5W;
    };
    for (D5W=446; D5W<533; D5W++)
    {
      pGraphicsList[numGraphic++] = D5W;
    };
    LoadNeededGraphics(pGraphicsList, numGraphic);
  };
  if (LOCAL_22 != 0)
  {
    closeGraphicsFile();
  };
  FreeTempMemory(1126);
  d.Word11684 = 1;
  d.Word11778 = 1;
  d.Word7254 = 0;
  //RestoreRegs(0xcf0);
  return;
}
