#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

void DisableTraceIfEnciphered();
void PlayFile_Close();
void RecordFile_Close();
bool IsPlayFileOpen();
bool IsRecordFileRecording();
void RecordFile_Open();
void RecordFile_Record(i32 x, i32 y, i32 z);

extern bool RepeatGame;
extern bool GameIsComplete;

// *********************************************************
//
// *********************************************************
//  TAG01dc4e
i16 Darken(i16 color)
{
  if (color & 0x007) color -= 0x001;
  if (color & 0x070) color -= 0x010;
  if (color & 0x700) color -= 0x100;
  return color;
}

// *********************************************************
//
// *********************************************************
//   TAG01f990
void DisplayHeroName(i16 P1,i16 P2,i16 P3,char *heroName)
{
  aReg A2, A3;
  char b_50[50];
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ASSERT(1,"impossible");
  A3 = (aReg)heroName;
  A2 = (aReg)b_50;
  while ( ((*A2)=*(A3++)) != 0)
  {
//
    if ( (*A2 >= 'A') && (*A2 <= 'Z'))
    {
      *A2 -= 0x40;
    };
    A2++;
//
  };
  TextOutToScreen(P1, P2, P3, 12, b_50);
}

extern ui32 VBLCount;

// *********************************************************
//
// *********************************************************
//           TAG01f9e6
RESTARTABLE _ShowCredits(const i32 P1) //()
{ // The game is lost!  Or won!!!
  static dReg D0, D4, D5, D6, D7;
  static aReg A0, A2, A3;
  static CHARDESC *pcA0;
  static i16 LOCAL_142;
  static i16 LOCAL_132;
  static i8  LOCAL_130[130-110];
  static PALETTE LOCAL_110;
  static PALETTE LOCAL_78; //32bytes
  static PALETTE LOCAL_46;
  static pnt LOCAL_14;
  static pnt LOCAL_10;
  static CHARDESC *LOCAL_6;
  static i8  LOCAL_1;
  static i32 before, after;
  static i32 i;
  static bool recording;

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
    RESTART(21)
    RESTART(22)
    RESTART(23)
    RESTART(24)
    RESTART(25)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  LOCAL_132 = 1;
  LOCAL_14 = NULL;
  A2 = NULL;
  SetCursorShape(0);
  STHideCursor(HC50);
  d.PrimaryButtonList = NULL;
  d.SecondaryButtonList = NULL;
  d.pKeyXlate1 = NULL;
  d.pKeyXlate2 = NULL;
  if (P1)
  {
    if (d.Word11694 == 0)
    {
      QueueSound(6, d.partyX, d.partyY, 0);//Dying sound
      VBLDelay(_1_,240); // About 4 seconds
    };
  };
  InsertDisk(0, 0);
  openGraphicsFile();
  A3 = (aReg)d.pWallBitmaps[2];
  if (P1)
  {
    A2 = A3;
    A3 += 560;
    ReadAndExpandGraphic(6, (ui8 *)A2, 0, 0);
    if (d.Word11694)
    {
      LOCAL_14 = A3;
      A3 += 1376;
      ReadAndExpandGraphic(208, (ui8 *)LOCAL_14, 0, 0);
    };
  };
  ReadAndExpandGraphic(0x8005, (ui8 *)A3, 0, 0);
  LOCAL_10 = A3;
  closeGraphicsFile();
  if (d.DynamicPaletteSwitching)
  {
    MemMove((ui8 *)&d.Palette11978, (ui8 *)&LOCAL_78, 32);
    for (D6W=0; D6W<=7; D6W++)
    {
      wvbl(_2_);
     for (D7W=0; D7W<16; D7W++)
     {
        D0W = Darken(d.Palette11946.color[D7W]);
        d.Palette11946.color[D7W] = D0W;
        D0W = Darken(d.Palette11978.color[D7W]);
        d.Palette11978.color[D7W] = D0W;
//
     };
//
    };
    wvbl(_3_);
    MemMove((ui8 *)&LOCAL_78, (ui8 *)&d.Palette11978, 32);
  }
  else
  {
    FadeToPalette(_4_,&d.Palette11914);
  };
  if (P1)
  {
    if (d.Word11694)
    {
      Flood(d.LogicalScreenBase, 12, 0xfa0);
      for (D7W=0; D7W<d.NumCharacter; D7W++)
      {
        D6L = 48 * D7W;
        pcA0 = &d.CH16482[D7W];
        LOCAL_6 = pcA0;
        BLT2Screen((ui8 *)LOCAL_14, (RectPos *)d.Word132, 32, 10);
        BLT2Screen((ui8 *)LOCAL_6->portrait, (RectPos *)d.Word140, 16, 1);
        DisplayHeroName(87, D6W=sw(D6W+14), 9, LOCAL_6->name);
        LOCAL_142 = StrLen(LOCAL_6->name);
        D5W = sw(6 * LOCAL_142 + 87);
        A0 = (aReg)LOCAL_6->name;
        LOCAL_1 = *(A0+9);
        if (LOCAL_1 != 44)
        {
          if (LOCAL_1 != 59)
          {
            if (LOCAL_1 != 45)
            {
              D5W += 5;
            };
          };
        };
        DisplayHeroName(D5W, D6W++, 9, LOCAL_6->title);
        for (D5W=0; D5W<=3; D5W++)
        {
          D0W = sw(DetermineMastery(D7W, D5W | 0xc000));
          D4W = sw(Smaller(16, D0W));
          if (D4W == 1) continue;
          if (D4W == 0) die (0x8d49,"Mastery = 0");
          StrCpy((char *)LOCAL_130, d.Pointer16770[D4W]);
          StrCat((char *)LOCAL_130, " ");
          StrCat((char *)LOCAL_130, d.Pointer16596[D5W]);
          DisplayHeroName(105, D6W=sw(D6W+8), 13, (char *)LOCAL_130);
//
        };
        d.Word132[3] += 48;
        d.Word132[2] += 48;
        d.Word140[3] += 48;
        d.Word140[2] += 48;
//
      };
      d.DynamicPaletteSwitching = 0;
      videoMode = VM_CAST;
      FadeToPalette(_5_,&LOCAL_78); //Show Champion's Levels
      for (i=0;(i<2000)&&(RepeatGame) || (!RepeatGame); i++)
      {
        wvbl(_24_)
      };
      GameIsComplete = true;
      RETURN;
    };
tag01fd10:
    ClearMemory(d.LogicalScreenBase, 32000);
    BLT2Screen((ui8 *)A2, (RectPos *)d.Word108, 40, -1);
    fillMemory((i16 *)&LOCAL_110, 16, 2, 2);
    MemMove((ui8 *)&LOCAL_110, (ui8 *)&LOCAL_46, 32);
    ForceScreenDraw();
    LOCAL_46.color[15] = 0x777;
    videoMode = VM_THEEND;
    FadeToPalette(_6_,&LOCAL_46);
    if (LOCAL_132)
    {
      VBLDelay(_7_,300); // About 5 seconds
    };
    if (d.CanRestartFromSavegame)
    {
      d.UseByteCoordinates = 0;
      FillRectangle(d.LogicalScreenBase, (RectPos *)d.Word116, 1, 160);
      ForceScreenDraw();
      wvbl(_21_);
      FillRectangle(d.LogicalScreenBase, (RectPos *)d.Word124, 0, 160);
      TextOutToScreen(110, 0x9a, 4, 0, "RESTART THIS GAME");
      LOCAL_46.color[1] = 0x437;
      LOCAL_46.color[4] = 0x777;
      d.PrimaryButtonList = d.Buttons16956;
      d.PrimaryButtonList->word0 = 0xd7;
      d.PrimaryButtonList->xMin = 0x67;
      d.PrimaryButtonList->xMax = 0xd9;
      d.PrimaryButtonList->yMin = 0x91;
      d.PrimaryButtonList->yMax = 0x9f;
      DiscardAllInput();
      STShowCursor(HC50);
      if (IsPlayFileOpen()) PlayFile_Close();
      FadeToPalette(_8_,&LOCAL_46);
      for (D7W=900; (--D7W!=0)&&(d.PartyHasDied==0);)
      {
        do
        {
          HandleMouseEvents(_22_,-999887); //initialize
                                           //  Sets DiskMenuNeeded = false
                                           //  Sets lastTime = 0;
        } while (intResult & 1);

        do 
        {
          HandleMouseEvents(_15_,0);       //'Restart' sets d.PartyHasDied=1
        } while (intResult & 1);
        if (DiskMenuNeeded) 
        {
          DisplayDiskMenu(_23_);
        };
        wvbl(_9_);
//
      };
      HandleMouseEvents(_16_,-999887); //initialize
      STHideCursor(HC33);
      ForceScreenDraw();
      if (d.PartyHasDied) // Did we time out?
      { //No. The player pressed 'Restart This Game'
        recording = IsRecordFileRecording();
        FadeToPalette(_10_,&LOCAL_110);
        ClearMemory(d.LogicalScreenBase, 32000);
        FadeToPalette(_11_,&d.Palette552[0]);
        MemMove((ui8 *)&d.Palette552[0], (ui8 *)&d.Palette11946, 32);
        d.gameState = GAMESTATE_ResumeSavedGame;
        if (recording)
        {
          RecordFile_Close();
          scrollingText.ResetTime(0);
          d.Time = 0;
          parameterMessageSequence = 0;
          do
          {
            HandleMouseEvents(_17_,-999887);
          } while (intResult & 1);
        };
        Cleanup(false);
        if (recording)
        {
          UI_BeginRecordOK(true);
          RecordFile_Open();
          RecordFile_Record(0x100,0x54,0xc9);
        };
        d.Initialize();
        GameSetup(_25_, 0);
        //STShowCursor(HC50);
        //STHideCursor(HC33);
        //SmartDiscard(true);
        //D0W = i16Result;
        //DisableTraceIfEnciphered();
        //if (D0W != -1)
        {
          TAG021028(_18_);
          d.PartyHasDied = 0;
          STShowCursor(HC33);
          DiscardAllInput();
          RETURN;
        };
      };
    };
    FadeToPalette(_12_,&LOCAL_110);
  };
  ExpandGraphic(LOCAL_10, d.LogicalScreenBase, 0, 0);
  FadeToPalette(_13_,&d.Palette328);
  i=0;
  do
  {
    i++;
    if (d.MouseSwitches != 0) break;
    wvbl(_19_);//NotImplemented(0x1fea0);//pumper();
    D0W = (I16)UI_CONSTAT();
  } while ((D0W == 0)&&(!RepeatGame) || (i<2000)&&(RepeatGame)); 
  // A mouse click gets us out of the credits screen to here.
  if (d.CanRestartFromSavegame)
  {
    if (P1)
    {
      LOCAL_132 = 0;
      FadeToPalette(_14_,&LOCAL_110);
      STShowCursor(HC33);
      STHideCursor(HC50);
      goto tag01fd10;
    };
  };  if (!RepeatGame) UI_PushMessage(UIM_TERMINATE);
  for(i=0;(i<2000)&&(RepeatGame) || (!RepeatGame); i++)
  {
    wvbl(_20_);
  };
  RETURN;
  //return DoesNotExist;
}

