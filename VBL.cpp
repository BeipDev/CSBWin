#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

//#ifdef _DEBUG

void traceFlash (const char * /*msg*/,i32 /*x*/,i32 /*y*/)
{
  /*
  if (x==-1) 
  {
    if (TraceFileActive)
    {
      fprintf(TraceFile,"%08x %s\n",d.Time,msg);
    };
    return;
  };
  if (TraceFileActive)
  {
    char *name = "??????";
    fprintf(TraceFile,"%08x vblFlashButn x=%d,y=%d",d.Time, x,y);
    switch (x)
    {
    case 291:
      switch (y)
      {
      case 125: name = "Turn Right"; break;
      case 147: name = "Slide Right"; break;
      };
      break;
    case 263:
      switch (y)
      {
      case 125: name = "Forward"; break;
      case 147: name = "Backward"; break;
      };
      break;
    case 234:
      switch (y)
      {
      case 125: name = "Turn Left"; break;
      case 147: name = "Slide Left"; break;
      };
      break;
    };
    fprintf(TraceFile," %s\n",name);
  };
  */
}


//  TAG00091c
void vblFlashButn(i16 x1,i16 x2,i16 y1,i16 y2) // called by VBL handler
{ // x1, x2, y1, y2 come directly from d.MoveButn
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;       
  dReg D0, D1, D2, D3, D4, D5, D6, D7;
  aReg A0, A1;
  traceFlash("vblFlashButn", x1, y1);
  A0 = (aReg)d.LogicalScreenBase;
  D5W = y1;
  D0W = sw(y2-y1); // line counter
  A0 += 160*y1;
  D5W = x1;
  D6W = x1;
  D5W = (I16)((D5W >> 1) & 0x00f8);
  D7W = D5W;
  A0 += D5W+4;
  D6W &= 15;
  D3L = D4L = 0xffff;
  D3H1 = 0; D3L >>= D6W;
  D5W = x2;
  D6W = (I16)((D5W>>1) & 0x00f8);
  D5W &= 15;
  D5W = sw(15 - D5W);
  D4W <<= D5W;
  if (D7W != D6W)
  {
    D1W = (I16)((((D6W - D7W) - 8) >> 3) & 0x1fff);
tag00097a:
    A1 = A0 + 160;
    D6W = LE16(wordGear(A0));
    D5W = (UI16)(D6W ^ 0xffff);
    D5W &= D3W;
    D7W = (UI16)(D3W ^ 0xffff);
    D6W = (UI16)((D6W & D7W) | D5W);
    wordGear(A0) = LE16(D6W);
    A0 += 8;
    D2W = D1W;
    goto tag0099a;
tag000996:
    wordGear(A0) ^= 0xffff;
    A0 += 8;
tag0099a:
    if (--D2W >= 0) goto tag000996;
    D5W = LE16(wordGear(A0));
    D6W = D5W;
    D5W ^= 0xffff;
    D5W &= D4W;
    D7W = (UI16)(D4W ^ 0xffff);
    D6W = (UI16)((D6W & D7W) | D5W);
    wordGear(A0) = LE16(D6W);
    A0 = A1;
    if (--D0W >= 0) goto tag00097a;
  }
  else
  {
    D3W &= D4W;
tag0009ba:
    D5W = LE16(wordGear(A0));
    D6W = D5W;
    D5W ^= 0xffff;
    D5W &= D3W;
    D7W = (UI16)(D3W ^ 0xffff);
    D6W &= D7W;
    D6W |= D5W;
    wordGear(A0) = LE16(D6W);
    A0 += 160;
    if (--D0W >= 0) goto tag0009ba;
  };
}
