#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"


void TAG021234()
{
  HopefullyNotNeeded(0x21234);
}

//   TAG021960
void SortWords(i16 *entries,i16 numEnt)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //pnt A3;
  ui16 uwD0, uwD1, uwD4, uwD5, uwD6, uwD7;
  ui16 uwLOCAL_8;
  ui16 uwLOCAL_6;
  ui16 logNumEnt;
  i32 i;
  logNumEnt = 0;
  i = numEnt;
  while ((i >>= 1) != 0)
  {
    logNumEnt++;
  };
  for (i=logNumEnt-1; i>=0; i--)
  {
    uwLOCAL_6 = uw((1 << i) - 1);
    uwLOCAL_8 = uw(uwLOCAL_6 << 1);
    for (uwD6=uwLOCAL_6; uwD6<=uwLOCAL_8; uwD6++) // 2nd half of current part??
    {
      uwD7 = uwD6;
      uwD4 = entries[uwD6]; // Middle entry
      for (;;)
      {
        uwD5 = uw((uwD7<<1)+1);
        if ((ui16)uwD5 >= numEnt) break;
        if ((ui16)uwD5+1 < numEnt)
        {
          uwD0 = entries[uwD5+1];
          if ((ui16)uwD0 >= entries[uwD5]) uwD5++;
        };
        if ((ui16)uwD4 >= (ui16)entries[uwD5]) break;
        uwD0 = entries[uwD5];
        entries[uwD7] = uwD0;
        uwD7 = uwD5;
        //continue;
      };
      entries[uwD7] = uwD4;
//
    };
//
  };
  for (uwLOCAL_6=uw(numEnt-1); uwLOCAL_6!=0; uwLOCAL_6--)
  {
    uwD4 = entries[uwLOCAL_6];
    uwD7=0;
    uwD0 = entries[uwD7];
    uwD1 = uwLOCAL_6;
    entries[uwD1] = uwD0;
    for (;;)
    {
      uwD5 = uw((uwD7<<1)+1);
      if (uwD5 > uwLOCAL_6-1) break;
      if (uwD5 < uwLOCAL_6 - 1)
      {
        uwD0 = entries[uwD5+1];
        if (uwD0 >= entries[uwD5]) uwD5++;
      };
      if (uwD4 >= entries[uwD5]) break;
      uwD0 = entries[uwD5];
      entries[uwD7] = uwD0;
      uwD7 = uwD5;
    }; // while (1)
    entries[uwD7] = uwD4;
//
  };
}

void TAG0220fa(i32 P1)
{
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  while (d.iAvailableGraphicMemory < P1)
  {
    if (d.pqFirstGraphic == NULL)
    {
      SystemError(43);
    };
    DeleteGraphic(d.pqFirstGraphic->graphicNum);

  };
}
/*
//*********************************************************
//
//*********************************************************
//   TAG022132
void CompactMemory()
{
  dReg D5, D6, D7;
  pnt A2, A3;
  VerifyGraphicFreeList();
//#ifdef _VERIFYPPQ
//  verifyppq();
//#endif
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.pgUnused == NULL) return;
  A2 = A3 = d.pStartMemory;
  do
  {
    D7L = longGear(A2);
    if (D7L > 0) // Not being used
    {
      A2 += D7L; // Look for next buffer in use
    }
    else
    {
      D7L = -D7L;
      if (A2 != A3)
      {
        D6W = wordGear(A2+10);
        if (D6W >= 0)
        {
          D5W = d.GraphicIndex0[D6W];
        }
        else
        {
          D5W = d.GraphicIndex8000[D6W & 0x7fff];
        };
        d.ppExpandedGraphics[D5W] = (ITEMQ *)A3;
        MemMove(A2, A3, D7W);
        if (d.pqFirstGraphic == (ITEMQ *)A2)
        {
          d.pqFirstGraphic = (ITEMQ *)A3;
        };
        if (d.pqLastGraphic == (ITEMQ *)A2)
        {
          d.pqLastGraphic = (ITEMQ *)A3;
        };
        if (d.pqFirstGraphic2 == (ITEMQ *)A2)
        {
          d.pqFirstGraphic2 = (ITEMQ *)A3;
        };
      };
      ASSERT(longGear(A3) == longGear(A3+D7L-4));
      A2 += D7L;
      A3 += D7L;
    };
  } while (A2 != d.Pointer23318);
//#ifdef _VERIFYPPQ
//  verifyppq();
//#endif
  d.Pointer23318 = A3;
  d.pgUnused = NULL;
}
*/

//*********************************************************
//
//*********************************************************
//          _TAG0222ea
void LoadNeededGraphics(i16 *P1, const i32 Count)
{//()
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D0, D4, D5, D6, D7;
  aReg A0, A2, A3;
  i32 iLOCAL_24;
//  i16 wLOCAL_14;
  pnt pLOCAL_12;
  i32 iLOCAL_8;
  i32 iLOCAL_4;
  i16 count;
  //RESTARTMAP
  //  RESTART(1)
  //END_RESTARTMAP
  count=sw(Count);
  bool fileOpen = false;
  SortWords(P1, count);
  iLOCAL_4 = 0;
  for (D7W=0; D7W<count; D7W++)
  {
    D5W = P1[D7W];
    while (  ((D6W=sw(D7W+1)) < count)
           &&(D5W == P1[D6W]) )
    {
    MemMove((ui8 *)&P1[D6W], (ui8 *)&P1[D7W], 2*((count--)-D6W));


    };
    D4L = d.GraphicDecompressedSizes[D5W] + 2;
    if (D4L & 1) D4L++;
    iLOCAL_4 += D4L;

  };
  if (d.Word23328 != 0)
  {
    iLOCAL_8 = (ui8 *)d.Pointer23310 - iLOCAL_4 - d.pStartMemory;
  }
  else
  {
    iLOCAL_8 = d.pStartAvailMemory - d.pStartMemory;
  };
  A0 = (aReg)d.LogicalScreenBase;
  longGear((ui8 *)A0+32000) = iLOCAL_8;
  if (iLOCAL_8 < 19576)
  {
    SystemError(44); // Error number
  };
  for (D7W=0; D7W<730; D7W++)
  {
    if (d.GetDerivedGraphicCacheIndex(D7W) != -1)
    {
      DeleteGraphic(D7W|0x8000);
    };
//
  };
  D7W = 0;
  do
  {
    D5W = P1[D7W];
    for (D6W=0; D6W<D5W; D6W++)
    {
      A3 = (aReg)d.ppUnExpandedGraphics[D6W];
      if (A3 == NULL) continue;
      DeleteGraphic(D6W);
      d.ppUnExpandedGraphics[D6W] = NULL;
      D4L = d.GraphicDecompressedSizes[D6W];
      if (D4L & 1) D4L++;
      D0L = -D4L;
      longGear((ui8 *)A3 + D4L) = D0W;

    };
    if (d.ppUnExpandedGraphics[D6W++] != NULL)
    {
      P1[D7W] ^= 0xffff;
    };
    D7W++;
  } while (D7W < count);
  for (; D6W<d.NumGraphic; D6W++)
  {
    A3 = d.ppUnExpandedGraphics[D6W];
    if (A3 != NULL)
    {
      DeleteGraphic(D6W);
      d.ppUnExpandedGraphics[D6W] = NULL;
      D4L = d.GraphicDecompressedSizes[D6W];
      D4L += D4L & 1;
      wordGear(A3 + D4L) = sw(-D4L);
    };
  };
  if (d.Word23328 != 0)
  {
    D4L = d.Pointer23314 - (d.Pointer23310 - iLOCAL_4);
    if (D4L > 0)
    {
      TAG0220fa(D4L);
    };
    //CompactMemory();
    d.iAvailableGraphicMemory -= D4L;
  };
  A2 = (aReg)d.Pointer23310;
  ASSERT(((int)A2 & 3) == 0,"A2");
  A3 = A2-2;
  while (A3>d.Pointer23314)
  {
    D5W = wordGear(A3);
    if (D5W < 0)
    {
      A3 += D5W-2;
    }
    else
    {
      iLOCAL_24 = A3 - d.ppUnExpandedGraphics[D5W];
      D4L = iLOCAL_24+2;
      A2 -= D4L;
      A3 -= D4L;
      if (A2 != A3+2)
      {
        MemMove((ui8 *)A3+2, (ui8 *)A2, D4W);
        ASSERT(((int)A2 & 3) == 0,"A2");
        d.ppUnExpandedGraphics[D5W] = A2;
      };
    };

  };
  pLOCAL_12 = (pnt)allocateMemory(5004,0);
//  while (d.Word23228 != 0) {};
  for (D7W=0; D7W<count; D7W++)
  {
    D5W = P1[D7W];
    if (D5W < 0)
    {
      P1[D7W] ^= 0xffff;
    }
    else
    {
      wordGear(A2-2) = D5W;
      D4L = (d.GraphicDecompressedSizes[D5W] + 3) & 0xfffffffc;
      A2 -= D4L;
      if (!fileOpen)
      {
        InsertDisk(0,0);
        openGraphicsFile();
        fileOpen = true;
      };
      D0L = d.GraphicCompressedSizes[D5W];
      if (d.GraphicDecompressedSizes[D5W] != D0L)
      {
        LSEEK(LocateNthGraphic(D5W), // file address
              d.GraphicHandle,       // handle
              SEEK_SET);             // relative to start of file
        LZWExpand(d.GraphicHandle,
                  d.GraphicCompressedSizes[D5W],
                  (ui8 *)A2,   // dest
                  d.pViewportBMP,   // scratch area
                  (ui8 *)pLOCAL_12);   // stack area

        d.Word11684 = 1;
      }
      else
      {
        ReadGraphic(D5W, (ui8 *)A2);
      };
      ASSERT(((int)A2 & 3) == 0,"A2");
      d.ppUnExpandedGraphics[D5W] = A2;
    };
//
  };
  if (fileOpen) closeGraphicsFile();
  d.Pointer23314 = A2;
  if (d.Word23328 != 0) d.pStartAvailMemory = (ui8 *)d.Pointer23314;
  TAG021234();
  FreeTempMemory(5004);
  return;
}
