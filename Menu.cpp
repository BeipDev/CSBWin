#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"



//   TAG000f4a
void BLT2Viewport(ui8 *P1,RectPos *rectPos,i16 P3,i16 P4)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  d.UseByteCoordinates = 0;
  ASSERT(verifyRectPos(rectPos),"rectpos");
  TAG0088b2(P1,
            d.pViewportBMP,
            rectPos,
            0,
            0,
            P3,
            112,
            P4);
}

//*********************************************************
//
//*********************************************************
RESTARTABLE _WaitForMenuSelect(
                       const i32 numButton, 
                       const i32 P2, 
                       const i32 /*P3*/, 
                       const i32 /*P4*/) 
{ //returns i16
  static dReg D0;
  static RectPos rect34;
  static RectPos rect26;
  static btn *pButnA0;
  static KeyXlate *LOCAL_16;
  static KeyXlate *LOCAL_12;
  static btn *saveSecondaryButtonList;
  static btn *savePrimaryButtonList;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
    RESTART(4)
    RESTART(5)
    RESTART(6)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  // Moved back to caller.  //STShowCursor(HCbbb);
  savePrimaryButtonList = d.PrimaryButtonList;
  saveSecondaryButtonList = d.SecondaryButtonList;
  LOCAL_12 = d.pKeyXlate2;
  LOCAL_16 = d.pKeyXlate1;
  d.pKeyXlate1 = NULL;
  d.pKeyXlate2 = NULL;
  d.SecondaryButtonList = NULL;
  pButnA0 = d.pButn18970[P2][numButton-1];
  
  d.PrimaryButtonList = pButnA0; // Pointer to button list
  d.Word11764 = 99;
  do
  {
    do
    {
      HandleMouseEvents(_5_,0);
    } while (intResult & 1);

    if (DiskMenuNeeded) 
    {
      DisplayDiskMenu(_6_);
    };
    wvbl(_1_);
    if (d.Word11764 == 99)
    {
      if (numButton == 1)
      {
        D0W = sw(UI_CONSTAT());
        if (D0W !=0)
        {
          NotImplemented(0x1d25a);//D0L = DIRECT_CONIN();
          if (D0W == 13) // Carriage return
          {
            d.Word11764 = 1;
          };
        };
      };
    };
    //if (0 != 0)
    //{
    //};
  } while (d.Word11764 == 99);
 
  d.UseByteCoordinates = 0;  
  MemMove((ui8 *)&d.PrimaryButtonList[d.Word11764-1].xMin, 
          (ui8 *)&rect34, 8);
  rect34.w.x1 -= 3;
  rect34.w.x2 += 3;
  rect34.w.y1 -= 3;
  rect34.w.y2 += 4;
  STHideCursor(HC41);
  d.Word11684 = 1;
  rect26.w.x1 = 0;
  rect26.w.y1 = 0;
  rect26.w.y2 = sw(rect34.w.y2 - rect34.w.y1 + 3);
  rect26.w.x2 = sw(rect34.w.x2 - rect34.w.x1 + 3);
  TAG0088b2((ui8 *)d.LogicalScreenBase, 
            (ui8 *)d.pViewportBMP, 
            &rect26,
            rect34.w.x1,
            rect34.w.y1,
            160,
            160,
            -1);
  wvbl(_2_);
  
  rect26 = rect34;
  rect26.w.y2 = rect26.w.y1;
  FillRectangle(d.LogicalScreenBase, &rect26, 5, 160);
  rect26 = rect34;
  rect26.w.x2 = rect26.w.x1;
  rect26.w.y2--;
  FillRectangle(d.LogicalScreenBase, &rect26, 5, 160);
  rect26 = rect34;
  rect26.w.y2--;
  rect26.w.y1 = rect26.w.y2;
  rect26.w.x1 -= 2;
  FillRectangle(d.LogicalScreenBase, &rect26, 0, 160);
  rect26 = rect34;
  rect26.w.x1 = rect26.w.x2;
  FillRectangle(d.LogicalScreenBase, &rect26, 0, 160);
  VBLDelay(_3_,5); // 5 vertical blanks
  rect26 = rect34;
  rect26.w.y1++;
  rect26.w.y2 = rect26.w.y1;
  rect26.w.x2 -= 2;
  FillRectangle(d.LogicalScreenBase, &rect26, 5, 160);
  rect26 = rect34;
  rect26.w.x1++;
  rect26.w.x2 = rect26.w.x1;
  rect26.w.y2--;
  FillRectangle(d.LogicalScreenBase, &rect26, 5, 160);
  rect26 = rect34;
  rect26.w.x2--;
  rect26.w.x1 = rect26.w.x2;
  FillRectangle(d.LogicalScreenBase, &rect26, 0, 160);
  rect26 = rect34;
  rect26.w.y2 -= 2;
  rect26.w.y1 = rect26.w.y2;
  rect26.w.x1++;
  FillRectangle(d.LogicalScreenBase, &rect26, 0, 160);
  rect26 = rect34;
  rect26.w.y2 += 2;
  rect26.w.y1 = rect26.w.y2;
  rect26.w.x1--;
  rect26.w.x2 += 2;
  FillRectangle(d.LogicalScreenBase, &rect26, 13, 160);
  rect26 = rect34;
  rect26.w.x2 += 3;
  rect26.w.x1 = rect26.w.x2;
  rect26.w.y2 += 2;
  FillRectangle(d.LogicalScreenBase, &rect26, 13, 160);
  VBLDelay(_4_,5);// 5 vertical blanks
  rect34.w.x2 += 3;
  rect34.w.y2 += 3;
  TAG0088b2((ui8 *)d.pViewportBMP,
            (ui8 *)d.LogicalScreenBase,
            &rect34,
            0,
            0,
            160,
            160,
            -1);
  STShowCursor(HC41);
  d.PrimaryButtonList = savePrimaryButtonList;
  d.SecondaryButtonList = saveSecondaryButtonList;
  d.pKeyXlate2 = LOCAL_12;
  d.pKeyXlate1 = LOCAL_16;
  DiscardAllInput();
  // Moved to caller. //STHideCursor(HC42);

  RETURN_i16(d.Word11764);
}

//   TAG01d5aa
void CenteredText(ui8 *dest, const char *text, i16 x, i16 y)
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (text == NULL) return;
  text = TranslateLanguage(text);
  D0W = StrLen(text);
  x = sw(x - D0W*3);
  TextOut_OneLine(dest, 
                  112, 
                  x, 
                  y, 
                  9, 
                  5, 
                  text,
                  999,
                  false);
}

//  TAG01d5ee
i16 SplitLongLine(const char *text,char *firstLine,char *secondLine)
{
  dReg D0;
  i16 LOCAL_4;
  i16 LOCAL_2;
  D0L=0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;        
  LOCAL_2 = StrLen(text);
  if (LOCAL_2 < 30)
  {
    D0W=0;
  }
  else
  {
    StrCpy(firstLine, text);
    LOCAL_4 = sw(LOCAL_2>>1); 
    while ( (firstLine[LOCAL_4] != ' ') && (LOCAL_4 < LOCAL_2) )
    {
      LOCAL_4++;
    };

    firstLine[LOCAL_4] = 0;
    StrCpy(secondLine, firstLine+LOCAL_4+1);
    D0W = 1;
  };
  return D0W;
}


//*********************************************************
//
//*********************************************************
//   TAG01d678
RESTARTABLE _DoMenu(
            const char *title1,        //8
            const char *title2,        //12
            const char *opt1,          //16
            const char *opt2,          //20
            const char *opt3,          //24
            const char *opt4,          //28
            const i32 I1,            //32
            const i32 ClearOldScreen,//34
            const i32 FadeOldScreen) //36
{
  static dReg D0, D1, D4, D5, D6;
  static pnt  LOCAL_112;
  static RectPos rectpos;
  static char LOCAL_100[50];
  static char LOCAL_50[50];
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
  END_RESTARTMAP
  setscreen(d.LogicalScreenBase,d.LogicalScreenBase,0);
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ExpandGraphic((i8 *)d.compressedGraphic0,
                d.pViewportBMP,
                0,
                0);
  TextOut_OneLine(d.pViewportBMP,
                  112,
                  0xc0,
                  7,
                  2,
                  1,
                  "V2.0",
                  999,
                  false);
  D4W = 1;
  if (opt2 != NULL) D4W++;
  if (opt3 != NULL) D4W++;
  if (opt4 != NULL) D4W++;
  if (FadeOldScreen !=0)
  {
    FadeToPalette(_2_,&d.Palette11914);
  }
  if (ClearOldScreen != 0)
  {
    ClearMemory(d.LogicalScreenBase, 32000);
  };
  d.UseByteCoordinates = 0;
  if (D4W == 1)
  {
    LOCAL_112 = (pnt)allocateMemory(8400,0);
    TAG0088b2((ui8 *)d.pViewportBMP,
              (ui8 *)LOCAL_112,
              (RectPos *)d.Byte18576,
              0,
              14,
              112,
              112,
              -1);
    BLT2Viewport((ui8 *)LOCAL_112,(RectPos *)d.Byte18584,112,-1);
    FreeTempMemory(8400);
    CenteredText(d.pViewportBMP, opt1, 112, 114);
  }
  else
  {
    if (D4W == 2)
    {
      TAG0088b2((ui8 *)d.pViewportBMP,
                (ui8 *)d.pViewportBMP,
                (RectPos *)d.Byte18592,
                102,
                52,
                112,
                112,
                -1);
      CenteredText(d.pViewportBMP, opt1, 112, 77);
      CenteredText(d.pViewportBMP, opt2, 112, 114);
    }
    else
    {
      if (D4W == 3)
      {
        CenteredText(d.pViewportBMP, opt1, 112, 77);
        CenteredText(d.pViewportBMP, opt2, 59, 114);
        CenteredText(d.pViewportBMP, opt3, 166, 114);
      }
      else
      {
        if (D4W == 4)
        {
          TAG0088b2((ui8 *)d.pViewportBMP,
                    (ui8 *)d.pViewportBMP,
                    (RectPos *)d.Byte18600,
                    102,
                    99,
                    112,
                    112,
                    -1);
          CenteredText(d.pViewportBMP, opt1, 59, 77);
          CenteredText(d.pViewportBMP, opt2, 166, 77);
          CenteredText(d.pViewportBMP, opt3, 59, 114);
          CenteredText(d.pViewportBMP, opt4, 166, 114);
        };
      };
    };
  };
  D5W = 29;
  if (title1 != NULL)
  {
    D0W = SplitLongLine(title1, LOCAL_50, LOCAL_100);
    if (D0W != 0)
    {
      D5W = 21;
      D0W = StrLen(LOCAL_50);
      D1L = 3*D0W;
      D6W = sw(113 - D1W);
      TextOut_OneLine(d.pViewportBMP,
                      112,
                      D6W,
                      D5W,
                      11,
                      5,
                      LOCAL_50,
                      999,
                      false);
      D5W += 8;
      D0W = StrLen(LOCAL_100);
      D1W = sw(3*D0W);
      D6W = sw(113 - D1W);
      TextOut_OneLine(d.pViewportBMP,
                      112,
                      D6W,
                      D5W,
                      11,
                      5,
                      LOCAL_100,
                      999,
                      false);
      D5W += 8;
    }
    else
    {
      D0W = StrLen(title1);
      D1W = sw(D0W * 3);
      D6W = sw(113 - D1W);
      TextOut_OneLine(d.pViewportBMP,
                      112,
                      D6W,
                      D5W,
                      11,
                      5,
                      title1,
                      999,
                      false);
      D5W += 8;
    };
  };
  if (title2 != NULL)
  {
    D0W = SplitLongLine(title2, LOCAL_50, LOCAL_100);
    if (D0W != 0)
    {
      D0W = StrLen(LOCAL_50);
      D1W = sw(D0W*3);
      D6W = sw(113 - D1W);
      TextOut_OneLine(d.pViewportBMP,
                      112,
                      D6W,
                      D5W,
                      9,  
                      5,
                      LOCAL_50,
                      999,
                      false);
      D5W += 8;
      D0W = StrLen(LOCAL_100);
      D1W = sw(3 * D0W);
      D6W = sw(113 - D1W);
      TextOut_OneLine(d.pViewportBMP,
                      112,
                      D6W,
                      D5W,
                      9,
                      5,
                      LOCAL_100,
                      999,
                      false);
    }
    else
    {
      D0W = StrLen(title2);
      D1W = sw(D0W * 3);
      D6W = sw(113 - D1W);
      TextOut_OneLine(d.pViewportBMP,
                      112,
                      D6W,
                      D5W,
                      9,
                      5,
                      title2,
                      999,
                      false);
    };
  };
  if (I1 != 0)
  {
    rectpos.w.y1 = 33;
    rectpos.w.y2 = 168;
    rectpos.w.x1 = 47;
    rectpos.w.x2 = 270;
    STHideCursor(HC43);
    BLT2Screen(d.pViewportBMP, &rectpos, 112, -1);
    STShowCursor(HC43);
  }
  else
  {
    MarkViewportUpdated(0);
    wvbl(_1_);
  };
  if (ClearOldScreen)
  {
    videoMode = VM_CENTERMENU;
  }
  else
  {
    videoMode = VM_LEFTMENU;
  };
  if (FadeOldScreen != 0)
  {
    FadeToPalette(_3_,&d.Palette11978);
  };
  d.Word11684 = 1;
  RETURN;
}
