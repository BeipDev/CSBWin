#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"


bool IsPlayFileOpen(void);
bool PlayFile_Play(MouseQueueEnt *MQ);
void RecordFile_Record(MouseQueueEnt *MQ);



//   TAG013ea4
RESTARTABLE _ReIncarnate(CHARDESC *pChar)
{ //void
  static dReg D0, D4, D5, D6, D7;
  static aReg A2;
  static i16 w_40;
  static i8  b_30[8];
  static i16 w_22;
  //static i16 w_20;
  //static i16 w_18 = 0x7ddd;
  static i16 w_16;
  static i16 w_14;
  static RectPos rect_12;
  static i16 w_4;
  static i16 w_2;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
  END_RESTARTMAP
  D5L = 0xccccc;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  keyboardMode = 2;
  rect_12.w.y1 = 3;
  rect_12.w.y2 = 8;
  rect_12.w.x1 = 3;
  rect_12.w.x2 = sw(rect_12.w.x1 + 167);
  FillRectangle(d.pViewportBMP, &rect_12, 12, 112);
  BLT2Viewport(GetBasicGraphicAddress(27),
               &d.wRectPos926,
               72,
               4);
  TextToViewport(177, 58, COLOR_13, "_______", false);//Seven of them
  TextToViewport(105, 76, COLOR_13, "___________________", false);// Nineteen
  STHideCursor(HC12);
  MarkViewportUpdated(0);
  SetCursorShape(0); // Arrow
  STShowCursor(HC12);
  D7W = 0;
  pChar->name[0] = 0;
  pChar->title[0] = 0;
  D6W = 1;
  A2 = (aReg)pChar->name;
  w_2 = 177;
  w_4 = 91;
  //w_20 = (I16)(d.MouseSwitches & 2);
  for (;;) 
  {
    SET(D0B, D6W==2);
    if (D0B != 0)
    {
      SET(D0B, D7W==19);
    };
    D4W = (I16)(D0W&1);
    if (D4W == 0)
    {
      STHideCursor(HC13);
      TextOut_OneLine(d.LogicalScreenBase,
                      160,
                      w_2,
                      w_4,
                      9,
                      12,
                      (char *)d.Byte1414,
                      999,
                      false); //Highlight the active character position
      STShowCursor(HC13);
    };
    while (IsPlayFileOpen() || UI_CONSTAT() == 0)
    {
      if (IsPlayFileOpen())
      {
        MouseQueueEnt MQ;
        PlayFile_Play(&MQ);
        if (MQ.num != 0x4444)
        {
          RETURN;
        };
        if (MQ.x != 0)
        {
          D5W = MQ.x;
          break;
        };
        RecordFile_Record(&MQ);
        keyboardMode = 1;
        RETURN;
      };
      WAITFORMESSAGE(_2_);
      if (!mouseQueueIsEmpty()) 
      //w_18 = (I16)(d.MouseSwitches & 2);
      //if ((w_18!=0) && (w_20==0)) 
      {
        bool unClick;
        {
          //MouseQueueEnt MQ;
          //MQ.num = 0x4445;
          //MQ.x = d.NewMouseX;
          //MQ.y = d.NewMouseY;
          //RecordFile.Record(&MQ);
        };
        w_14 = pMouseQueue[d.MouseQStart].x; //w_14 = d.NewMouseX;
        w_16 = pMouseQueue[d.MouseQStart].y; //w_16 = d.NewMouseY;
        unClick = pMouseQueue[d.MouseQStart].num == 0x0add;
        d.MouseQStart = (ui16)((d.MouseQStart + 1) % MOUSEQLEN);
        if (unClick)
        {
          continue;
        };
//    
        if (    ((D6W==2)||(D7W>0))
             && (w_14>=197)
             && (w_14<=215)
             && (w_16>=147)
             && (w_16<=155) )  //Name non-blank and pressed 'OK'
        {
          w_22 = D7W;
          StrCpy((char *)b_30, pChar->name);
          D7W = StrLen(pChar->name);
          while (pChar->name[--D7W]==' ')
          {
            pChar->name[D7W] = 0;
          };
          for (D7W = 0;
               D7W < d.NumCharacter-1;
               D7W++)
          {
            D0W = StrCmp(d.CH16482[D7W].name, pChar->name);
            if (D0W==0) goto tag014088;

          }; // for D7W
          {
            MouseQueueEnt MQ;
            MQ.num = 0x4444;
            MQ.x = 0;
            MQ.y = 0xffff;
            RecordFile_Record(&MQ);
          }
          keyboardMode=1;
          RETURN;
tag014088:
          if (D6W == 2)
          {
            A2 = (aReg)pChar->title;
          };
          StrCpy(pChar->name, (char *)b_30);
          D7W = w_22;
        }
        else
        {
          if (   (w_14>=107) 
              && (w_14<=175) 
              && (w_16>=147) 
              && (w_16<=155) )   // Backspace button
          {
            D5W = 8;
            break;
          };
          if (   (w_14>=107) 
              && (w_14<=215) 
              && (w_16>=116) 
              && (w_16<=144 ) ) // Letters, punctuation, carriage-return
          {
            D0W = (I16)((w_14+4) % 10); //pixel within column
//          
//
//         
            if (   (D0W!=0) //not on vertical line and not on horizontal line
                && (   (((w_16+5)%10)!=0) 
                    || ((w_14>=207)&&(w_16==135)) ) )
            {
              w_40 = sw(11 * ((w_16-116)/10));  // starting index in row
              D5W = sw(w_40 + 'A'); //starting letter in row
              D5W = sw(D5W + (w_14-107)/10); //letter
//

              if ( (D5W==86) || (D5W==97) ) //if carriage-return
              {
                D5W = 13;
                break;
              };
              if (D5W >= 87)
              {
                D5W--;  //adjust for carriage-return
              };
              if (D5W > 90)
              {
                D0W = sw(D5W - 90); 
                D5W = d.SpecialChars[D0W-1]; // comma, period, semi-colon, colon, space
              };
              break;
            };
          };
        };
      };
      //w_20 = w_18;
      wvbl(_1_);

    }; // while waiting for input.
    if (!IsPlayFileOpen())
    {
      //ASSERT(w_18 != 0x7ddd,"w_18");
      //w_20 = w_18;
      D0W = sw(UI_CONSTAT());
      if (D0W != 0)
      {
        D5W = sw(UI_DIRECT_CONIN());
      };
    };
    ASSERT(D5L != 0xccccc,"D5L");
    {
      MouseQueueEnt MQ;
      MQ.num = 0x4444;
      MQ.x = D5W;
      MQ.y = 0xffff;
      RecordFile_Record(&MQ);
    };
    if ( (D5W>='a') && (D5W <='z') )
    {
      D5W -= 32; // Convert to uppercase
    };
//
    if (   ((D5W>='A')&&(D5W<='Z')) 
        || (D5W=='.') 
        || (D5W==44) 
        || (D5W==59) 
        || (D5W==58)
        || (D5W==' ') )
    {
      if (D5W==' ')
      {
        if (D7W == 0) continue;
      };
      if (D4W != 0) continue;
      d.Byte1416[0] = D5B;
      STHideCursor(HC14);
      TextOut_OneLine(d.LogicalScreenBase,
                      160,
                      w_2,
                      w_4,
                      13,
                      12,
                      (char *)d.Byte1416,
                      999,
                      false);
      STShowCursor(HC14);
      A2[D7W++] = D5B;
      A2[D7W] = 0;
      w_2 += 6;
      if (D6W != 1) continue;
      if (D7W != 7) continue;
      goto tag0142c2;
    }
    else
    {
      if (D5W != 13) goto tag0142de;
      if (D6W != 1) continue;
      if (D7W <= 0) continue;
      STHideCursor(HC15);
      TextOut_OneLine(d.LogicalScreenBase,
                      160,
                      w_2,
                      w_4,
                      13,
                      12,
                      (char *)d.Byte1414,
                      999,
                      false);
      STShowCursor(HC15);
tag0142c2:
      D6W = 2;
      A2 = (aReg)pChar->title;
      w_2 = 105;
      w_4 = 109;
      D7W = 0;
      continue;
    };
tag0142de:
    if (D5W != 8) continue;
    if ( (D6W==1) && (D7W==0) ) continue;
    if (D4W == 0)
    {
      STHideCursor(HC16);
      TextOut_OneLine(d.LogicalScreenBase,
                      160,
                      w_2,
                      w_4,
                      13,
                      12,
                      (char *)d.Byte1414,
                      999,
                      false);
      STShowCursor(HC16);
    };
    if (D7W == 0)
    {
      A2 = (aReg)pChar->name;
      D0W = StrLen((char *)A2);
      D7W = sw(D0W - 1);
      D6W = 1;
      w_2 = sw(177 + 6*D7W);
      w_4 = 91;
    }
    else
    {
      D7W--;
      w_2 -= 6;
    };
    A2[D7W] = 0;
  };
  //RETURN;
}
