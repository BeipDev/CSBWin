#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"


bool IsPlayFileOpen(void);

//*********************************************************
// I separated this from SummarizeRoomInfo to make it 
// a little easier to see what is going on.
//*********************************************************
void SummarizeStoneRoom(SUMMARIZEROOMDATA *rslt,
                        const i32 facing,
                        bool FalseStoneWall)
{
  dReg D0;
  i32 i;
  i32 rightCFmask;
  i32 rearCFmask;
  i32 leftCFmask;
  i32 cellFlags;
  i32 relativePosition;
  i32 DBType;
  RN  LOCAL_12;
  ROOMTYPE rtD0;
  DBCOMMON *dbA2;
  DB3 *pActuator;
  i32 actuatorType;

  cellFlags = GetCellFlags(rslt->x, rslt->y);
  LOCAL_12 = FindFirstObject(rslt->x, rslt->y);
  rtD0 = (ROOMTYPE)(cellFlags >> 5);
  rslt->roomType = roomSTONE; // even for false wall
  rightCFmask = cellFlags & 8;
  rearCFmask  = cellFlags & 8;
  leftCFmask  = cellFlags & 8;
  bool realDecoration[3];
  realDecoration[0]=realDecoration[1]=realDecoration[2]=false;
  if (!FalseStoneWall)
  {
    switch (facing)
    {
    case 0:
      rightCFmask = cellFlags & 4;
      rearCFmask  = cellFlags & 2; 
      leftCFmask  = cellFlags & 1; 
      break;
    case 1:
      rightCFmask = cellFlags & 2; 
      rearCFmask  = cellFlags & 1; 
      leftCFmask  = cellFlags & 8; 
      break;
    case 2:
      rightCFmask = cellFlags & 1; 
      rearCFmask  = cellFlags & 8; 
      leftCFmask  = cellFlags & 4; 
      break;
    case 3:
      rightCFmask = cellFlags & 8; 
      rearCFmask  = cellFlags & 4; 
      leftCFmask  = cellFlags & 2; 
      break;
    default:
      NotImplemented(0xa6a0);
    };
  };
  //d.ChampionPortraitOrdinal = 0;
  rslt->championPortraitOrdinal = 0;
  TAG00a4a2(rslt,
            (UI16)rightCFmask, // Which bit to look
            (UI16)rearCFmask,  //  at in cellflags
            (UI16)leftCFmask,  //   for the three directions
            facing,
            rslt->x,
            rslt->y,
            FalseStoneWall?1:0);

  while (   (LOCAL_12 != RNeof)
          && ((DBType=LOCAL_12.dbType()) <= dbACTUATOR) )
  { // Must be a dbTEXT or a dbACTUATOR.
    // You should not have doors or teleporters in stone!
    if ( (DBType == dbDOOR) || (DBType == dbTELEPORTER) )
    {
      // But the TeamDungeon of Zyx et al did have a door in a wall!!!
      LOCAL_12 = GetDBRecordLink(LOCAL_12);
      continue;
    };
    relativePosition = (LOCAL_12.pos()-facing)&3;
    if (relativePosition != 0) // Same side as facing?
    {
      dbA2 = GetCommonAddress(LOCAL_12);
      if (DBType == dbTEXT)
      {
        D0W = dbA2->CastToDB2()->show();      
        //D0W &= 1;
        if (D0W != 0)
        {
          D0W = sw(d.NumWallDecoration + 1);
          ASSERT((relativePosition>=1) && (relativePosition<=3),"relpos");
          //rslt->decorations[relativePosition-1] = D0W;
          //d.rn10540[relativePosition-1] = LOCAL_12;
          rslt->text[relativePosition-1] = LOCAL_12;
        };
      }
      else
      { //Must be an ACTUATOR
        if (dbA2->CastToDB3()->actuatorType() == 47)
        { //No graphics in DSA!
          // Wrong!!!  A DSA of type 255 has a 16-bit graphic ID (from CSBgraphics.dat)
          DB3 *pDSA;
          i32 levelIndex, absIndex;
          pDSA = dbA2->CastToDB3();
          levelIndex = pDSA->DSAselector();
          absIndex = DSALevelIndex[d.LoadedLevel][levelIndex];
          if (absIndex == 255)
          {
            i32 graphicID;
            graphicID = 0x10000 | pDSA->ParameterA();
            rslt->decorations[relativePosition-1] = graphicID;
          };
          LOCAL_12 = GetDBRecordLink(LOCAL_12);
          continue;
        };
        ASSERT((relativePosition>=1) && (relativePosition<=3),"relpos");
        pActuator = dbA2->CastToDB3();
        actuatorType = pActuator->actuatorType();
        if (   (actuatorType != 127)
            || ((pActuator->State() & PORTRAIT_HideGraphic)==0))
        {
          rslt->decorations[relativePosition-1] = 
              pActuator->wallGraphicOrdinal();
          realDecoration[relativePosition-1] = true;
          if (rslt->decorations[relativePosition-1] > d.NumWallDecoration)
          {
            if ((d.newFlags & IllegalWallDecorationWarning) == 0)
            {
              d.newFlags |= IllegalWallDecorationWarning;
              if (IsPlayFileOpen())
              {
	      	      UI_MessageBox("Illegal Wall Graphic\nI will not warn you again!",
                            "Warning",MB_OK);
              };
            };
            rslt->decorations[relativePosition-1] = 0;
          };
        };
        D0W = dbA2->CastToDB3()->actuatorType();
        if (D0W == 127)
        {
          if (relativePosition == 2)
          {
            DB3 *pPortrait;
            pPortrait = dbA2->CastToDB3();
            if ((pPortrait->State() & PORTRAIT_HidePortrait) == 0)
            {
              //d.ChampionPortraitOrdinal 
              rslt->championPortraitOrdinal
                   = ((dbA2->CastToDB3()->value() & 511)+1);
            };
          };
        };
      };
    };
    LOCAL_12 = GetDBRecordLink(LOCAL_12);
  };
  for (i=0; i<3; i++)
  {
    if (   (rslt->text[i] != RNeof)
        && !realDecoration[i])
    {
      rslt->decorations[i] = 0;
    };
  };
  if (FalseStoneWall)
  {
    if (d.partyX != rslt->x)
    {
      if (d.partyY != rslt->y)
      {
        rslt->rn2 = RN(RNeof);
        return;
      };
    };
  };
  rslt->rn2 = LOCAL_12;
  return;
}


ui8 graphicRoomTypes[20] =
{
  gRoomSTONE,      // STONE       -> STONE
  gRoomOPEN,       // OPEN        -> OPEN
  gRoomPIT,        // PIT         -> PIT
  255,             // STAIRS      -> illegal
  255,             // DOOR        -> ILLEGAL
  gRoomTELEPORTER, // TELEPORTER  -> TELEPORTER
  255,             // FALSEWALL   -> illegal
  255,             // 7           -> illegal
  255,             // 8           -> illegal
  255,             // 9           -> illegal
  255,             // 10          -> illegal
  255,             // 11          -> illegal
  255,             // 12          -> illegal
  255,             // 13          -> illegal
  255,             // 14          -> illegal
  255,             // 15          -> illegal
  gRoomDOOREDGE,   // DOOREDGE    -> DOOREDGE
  gRoomDOORFACING, // DOORFACING  -> DOORFACING
  gRoomSTAIREDGE,  // STAIREDGE   -> STAIREDGE
  gRoomSTAIRFACING // STAIRFACING -> STAIRFACING
};

//*********************************************************
//
//*********************************************************
//TAG00a59a
void SummarizeRoomInfo(SUMMARIZEROOMDATA *rslt,i32 facing)
{
  // rslt[0] = Cell's DUDAD word
  dReg D0, D1;
  CELLFLAG cfD4;
  ROOMTYPE rtD0;
  DB3      *DB3A2;
  RN LOCAL_12;
  RN firstObject;
  i32 dbType;
  //i16 LOCAL_10;
  //i16 LOCAL_8;
  //i32  relativePosition;
  //ui16 leftCFmask=0xbade;
  //ui16 rearCFmask=0xbade;
  //ui16 rightCFmask=0xbade;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //ClearMemory((pnt)rslt, sizeof (*rslt));
  rslt->text[0] = RNeof;
  rslt->text[1] = RNeof;
  rslt->text[2] = RNeof;
  rslt->decorations[0] = 0;
  rslt->decorations[1] = 0;
  rslt->decorations[2] = 0;
  rslt->rn2 = RNeof;
  cfD4 = GetCellFlags(rslt->x, rslt->y);
  rtD0 = (ROOMTYPE)(cfD4 >> 5);
  rslt->roomType = rtD0;
  LOCAL_12 = firstObject = FindFirstObject(rslt->x, rslt->y);
  switch (rtD0)
  {
  case roomSTONE:
    SummarizeStoneRoom(rslt,
                       facing,
                       false);     // Real Stone!
    rslt->graphicRoomType = (GRAPHICROOMTYPE)graphicRoomTypes[rslt->roomType];
    return;
  
  case  roomPIT:
    D0W = (I16)(cfD4 & 8);
    if (D0W != 0)
    {
      D0W = (I16)(cfD4 & 4);
      rslt->decorations[0] = D0W;
      cfD4 &= 1;
    }
    else
    {
      rslt->roomType = roomOPEN;
      cfD4 = 1;
    };
    goto tag00a860;

  case roomFALSEWALL:
    if ((cfD4 & 4) == 0)
    {
      //rslt->roomType = roomSTONE;
      //D0W = D4W & 8;
      //rightCFmask = D0W;
      //rearCFmask = D0W;
      //leftCFmask  = D0W;
      //LOCAL_10 = 1;
      SummarizeStoneRoom(rslt,
                         facing,
                         true);      // Fake stone!
      rslt->graphicRoomType = (GRAPHICROOMTYPE)graphicRoomTypes[rslt->roomType];
      return;
    };
    rslt->roomType = roomOPEN;
    D0W = (I16)(cfD4 & 8);
    if (D0W != 0) cfD4 = 8;
    else cfD4 = 0;
    // NOTE fall-through
  
  case roomOPEN:
    rslt->decorations[2] = RandomWallDecoration(
                        (I16)(cfD4 & 8),
                        d.pCurLevelDesc->NumRandomFloorDecorations(),
                        rslt->x,
                        rslt->y,
                        30);
tag00a85c:
    cfD4 = 1;
tag00a860:
    while (LOCAL_12 != RNeof)
    {
      dbType = LOCAL_12.dbType();
      if (dbType == dbACTUATOR)
      {
        DB3A2 = GetRecordAddressDB3(LOCAL_12);

        
     
        



        if (DB3A2->actuatorType() == 47)
        { //No graphics in DSA!
          // Wrong!!!  A DSA of type 255 has a 16-bit graphic ID (from CSBgraphics.dat)
          DB3 *pDSA;
          i32 levelIndex, absIndex;
          pDSA = DB3A2;
          levelIndex = pDSA->DSAselector();
          absIndex = DSALevelIndex[d.LoadedLevel][levelIndex];
          if (absIndex == 255)
          {
            i32 graphicID;
            graphicID = 0x4000 | pDSA->ParameterB();
            rslt->decorations[2] = graphicID;
          };
        }
        else
        {
          i32 decoration;
          decoration = DB3A2->wallGraphicOrdinal();
          if ( (decoration > 0) && (decoration <= d.pCurLevelDesc->NumFloorDecorations())  )     
          {
            rslt->decorations[2] = decoration;
          };
        };
      };
      LOCAL_12 = GetDBRecordLink(LOCAL_12);

    };
    LOCAL_12 = firstObject;
    goto tag00a984;


  case roomTELEPORTER:

    rslt->decorations[0] = sw((cfD4 & 0x0c) == 0x0c ? 1 : 0);
    goto tag00a85c;
  
  case roomSTAIRS: 
    D0W = (I16)((cfD4 & 8) >> 3); // Set if N/S travel 
    D1W = (I16)(facing & 1);
    if (D1W == D0W)
    {
      rtD0 = roomSTAIREDGE;
    }
    else
    {
      rtD0 = roomSTAIRFACING;
    };
    rslt->roomType = rtD0;
    D0W = (I16)(cfD4 & 4);
    rslt->decorations[0] = D0W;
    cfD4 = 0;
    break;

  case roomDOOR:
    D0W = (I16)((cfD4 & 8) >> 3); // N/S door
    D1W = (I16)(facing & 1);
    if (D0W == D1W) 
    {
      rslt->roomType = roomDOOREDGE; //facing at right angle to door direction
    }
    else
    {
      rslt->roomType = roomDOORFACING; //facing in door direction
      D0W = (I16)(cfD4 & 7);
      rslt->decorations[0] = D0W;
      rslt->decorations[1] = FindFirstDoor(rslt->x, rslt->y).idx();
    };
    goto tag00a85c;
  default:
    cfD4 = 1;
  }; // switch
  while (  (LOCAL_12 != RNeof)
         &&(LOCAL_12.dbType() <= dbACTUATOR) )
  {
    LOCAL_12 = GetDBRecordLink(LOCAL_12);

  };
tag00a984:
  if (cfD4 != 0) //Can footprints be shown here?
  {
    cfD4 = ub(SearchFootprints(rslt->x, rslt->y));
    if (cfD4 != 0)
    {
      cfD4--;
      D0W = (UI8)(cfD4);
      D1W = (UI8)(d.IndexOfFirstMagicFootprint);
      if (D0W >= D1W)
      {
        D0W = (UI8)(cfD4);
        D1W = (UI8)(d.IndexOfLastMagicFootprint);
        if (D0W < D1W)
        {
          rslt->decorations[2] |= 0x8000; // footprints?
        };
      };
    }
    if (d.Time > 0)
    {
      if (GetExtendedCellFlag(
               d.LoadedLevel, rslt->x, rslt->y, ECF_FOOTPRINTS))
      {
        rslt->decorations[2] |= 0x8000; // footprints?
      };
    };
  };
  rslt->rn2 = LOCAL_12;
  rslt->graphicRoomType = (GRAPHICROOMTYPE)graphicRoomTypes[rslt->roomType];
}