
#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"


//extern CDC *OnDrawDC;

void info(char *, unsigned int);
void CleanupAltMonCache();
void CleanupWallDecorations();
//void CleanupFloorDecorations();
extern FILETABLE fileTable[maxFilesOpen];
extern char *helpMessage;
extern bool overlayActive;
extern TEMPORARY_FILE CSBgraphicsFile;
extern ui8 overlayPaletteRed[512];
extern ui8 overlayPaletteGreen[512];
extern ui8 overlayPaletteBlue[512];


const char *(CSBGraphicsSectionNames[]) =  {
      "Viewport Overlay",
      "Portrait",
      "Sound",
      "Background Graphic",
      "Viewport Palette",
      "Alternate Monster Graphic",
      "Wall Decorations",
      "Floor Decorations",
};



#ifdef GraphicsDebug
extern FILE *GrphDbg;
#endif

class SCREEN
{
private:
ui8 *m_physScreenBase;
ui8 *m_logScreenBase;
ui8 *m_physAllocated; // The buffer we allocated
ui8 *m_logAllocated;  // The buffer we allocated
public:
  SCREEN();
  ~SCREEN();
  ui8 *physbase();
  ui8 *logbase();
  void physbase(ui8 *newphys);
  void logbase(ui8 *newlog);
};

SCREEN::SCREEN()
{
  m_physAllocated = NULL;
  m_logAllocated = NULL;
  m_physScreenBase = NULL;
  m_logScreenBase = NULL;
}

SCREEN::~SCREEN()
{
  if (m_physAllocated != NULL) UI_free (m_physAllocated);
  m_physScreenBase = NULL;
  m_physAllocated = NULL;
  if (m_logAllocated) UI_free(m_logAllocated);
  m_logScreenBase = NULL;
  m_logAllocated = NULL;
}

ui8 *SCREEN::physbase()
{
  if (m_physScreenBase == NULL)
  {
    if (m_physAllocated == NULL)
      m_physAllocated = (ui8 *)UI_malloc(32000, MALLOC026);
      if (m_physAllocated == NULL)
      {
        UI_MessageBox("Cannot allocate memory",NULL,MESSAGE_OK);
        die(0x5fd1);
      };
    m_physScreenBase = m_physAllocated;
  };
  return m_physScreenBase;
}

void SCREEN::physbase(ui8* newphys)
{
  m_physScreenBase = newphys;
}

ui8 *SCREEN::logbase()
{
  if (m_logScreenBase == NULL)
  {
    if (m_logAllocated == NULL)
      m_logAllocated = (ui8 *)UI_malloc(32000, MALLOC027);
      if (m_logAllocated == NULL)
      {
        UI_MessageBox("Cannot allocate memory",NULL,MESSAGE_OK);
        die(0x6fd3);
      };
    m_logScreenBase = m_logAllocated;
  };
  return m_logScreenBase;
}

void SCREEN::logbase(ui8* newlog)
{
  m_logScreenBase = newlog;
}



SCREEN screen;
i16 globalPalette[16] = { // RGB top-to-bottom
  0x777, 0x700, 0x070, 0x007, 0x111, 0x222, 0x333, 0x444,
  0x555, 0x000, 0x001, 0x010, 0x100, 0x200, 0x020, 0x002
};


void setscreen(ui8 *log,ui8 *phys,i16 /*res*/)
{
  if ((i32)phys != -1) screen.physbase(phys);
  if ((i32)log != -1) screen.logbase(log);
}

ui8 *physbase()
{
  return screen.physbase();
}

ui8 *logbase()
{
  return screen.logbase();
}

void SetDLogicalBase(ui8 *b)
{
  d.LogicalScreenBase = b;
  if (d.Pointer12926 == NULL) d.Pointer12926 =
             (pnt)UI_malloc(768, MALLOC028);
}


/*
void VerifyGraphicFreeList()
{
  ITEMQ *pCur, *pNext;
  pCur = d.pgUnused;
  if (pCur != NULL)
  {
    ASSERT(pCur->pgPrev() == NULL,"pCur");
  };
  while (pCur != NULL)
  {
    pNext=pCur->pgNext();
    if (pNext != NULL)
    {
      ASSERT(pNext->pgPrev() == pCur,"pNext");
      ASSERT(pNext->size <= pCur->size,"pNext");
      ASSERT(pNext != pCur,"pNext");
    };
    pCur = pNext;
  };
}
 */

// *********************************************************
//
// *********************************************************
void TAG0013e4(OBJ_NAME_INDEX P1, i32 xPixel, i32 yPixel)
{
  RectPos rectpos;
  ui8 *pnt_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pnt_4 = (ui8 *)allocateMemory(128, 0);
  rectpos.w.x1 = sw(xPixel);
  rectpos.w.x2 = (i16)(rectpos.w.x1 + 15);
  rectpos.w.y1 = sw(yPixel);
  rectpos.w.y2 = (i16)(rectpos.w.y1 + 15);
  GetIconGraphic(P1, pnt_4);
  BLT2Screen(pnt_4, &rectpos, 8, -1);
  FreeTempMemory(128);
}

// Internal function to make TextOut easier to read
//   TAG0016d8
void BltOneChar(dReg& D2,
                dReg& D3,
                //dReg& D4,
                dReg& D5,
                aReg& A2,
                aReg  A3,
                i32 A3inc,
                i32 jA0,
                i32 jA1)
{
  dReg D0, D1, D6, D7;
  i32 lineCounter;
  for (lineCounter=0; lineCounter<6; lineCounter++)
  {
    longGear((ui8 *)A3+0) &= LE32(D2L);
    longGear((ui8 *)A3+4) &= LE32(D2L);
    D0L = (UI8)(*A2);
    switch (D5W)
    {
    case 0:
      D0W <<= 11-D3W;
      break;
    case 8:
      D0UW >>= D3W-11;
      break;
    case 18:
      D0W <<= 27-D3W;
      break;
    default: NotImplemented(0x16fc);
    };
    //D1W = D0W;
    //SWAP(D1);
    //D1W = D0W;
    D1H2 = D0H2;
    D1H1 = D0H2;
    SWAP(D0);
    D6L = 0;
    D7L = 0;
    switch (jA0&0xffff)
    {
    case 1: D6L = D0L; break;
    case 2: D6W = D1W; break;
    case 3: D6L = D1L; break;
    case 4: D7L = D0L; break;
    case 5: D6L = D0L; D7L = D0L; break;
    case 6: D6W = D1W; D7L = D0L; break;
    case 7: D6L = D1L; D7L = D0L; break;
    case 8: D7W = D1W; break;
    case 9: D6L = D0L; D7W = D1W; break;
    case 10: D6W = D1W; D7W = D1W; break;
    case 11: D6L = D1L; D7W = D1W; break;
    case 12: D7L = D1L; break;
    case 13: D6L = D0L; D7L = D1L; break;
    case 14: D6W = D1W; D7L = D1L; break;
    case 15: D6L = D1L; D7L = D1L; break;
    case 0: break;
    default: NotImplemented(0x1756);
    };
    D0L |= D2L;
    D0L ^= 0xffffffff;
    D0W = 0;
    D1L |= D2L;
    D1L ^= 0xffffffff;
    switch (jA1&0xffff)
    {
    case 1: D6L |= D0L; break;
    case 2: D6W |= D1W; break;
    case 3: D6L |= D1L; break;
    case 4: D7L |= D0L; break;
    case 5: D6L |= D0L; D7L |= D0L; break;
    case 6: D6W |= D1W; D7L |= D0L; break;
    case 7: D6L |= D1L; D7L |= D0L; break;
    case 8: D7W |= D1W; break;
    case 9: D6L |= D0L; D7W |= D1W; break;
    case 10: D6W |= D1W; D7W |= D1W; break;
    case 11: D6L |= D1L; D7W |= D1W; break;
    case 12: D7L |= D1L; break;
    case 13: D6L |= D0L; D7L |= D1L; break;
    case 14: D6W |= D1W; D7L |= D1L; break;
    case 15: D6L |= D1L; D7L |= D1L; break;
    case 0: break;
    default: NotImplemented(0x17ae);
    }
    longGear((ui8 *)A3+0) |= LE32(D6L);
    longGear((ui8 *)A3+4) |= LE32(D7L);
    A2 += 128;
    A3 += A3inc;
  }
  A2 -= 128;
  A3 -= A3inc;
}

//   TAG00154c
void TextOut_OneLine(ui8 *dest,
                     i32 destWidth,
                     i32 x,
                     i32 y,
                     i32 color,
                     i32 P6,
                     const char *text,
                     i32 maxLineLength,
                     bool translate)
{
  dReg D2, D3, D4, D5, D6, D7;
  i32 saveD2;
  const char *A0;
  aReg A2, A3, saveA2, saveA3;
  i32 jA0, jA1;
  i32 numCharactersDisplayed;
  if (translate)
  {
    text = TranslateLanguage(text);
  };
  A2 = NULL;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A3  = (aReg)dest;
  D7W = (i16)x;
  D6W = (i16)y;
  //D5W = P5;
  D4W = (i16)P6;
  A0  = text;
  if (*A0 == 0) return;

  A3 += (D6W - 4) * destWidth;
  A3 += (D7UW & 0xfff0)>>1;//UWed
  D3W = (i16)(D7W & 15);
  jA0 = color; // 16 colors
  jA1 = D4W;
  numCharactersDisplayed = 0;
tag001660:
  for (;;)
  {
    do
    {
      if (numCharactersDisplayed == maxLineLength) return;
      numCharactersDisplayed++;
      D2L = 0;
      D2B = *text;
      if (D2W ==0) return;
      text++;
      A2 = d.Pointer12926 + D2W;
      if (D3W > 11) goto tag0016a0;
      //D4L = 6;
      D2L = 0x07ff07ff;
      D2UL >>= D3W;//ULed
      D2H1 = D2W;
      D5L = 0;
      BltOneChar(D2, D3, D5, A2, A3, destWidth, jA0, jA1);
      D3W += 6;
    } while (D3W < 16);
    D3W &= 1;
    A3 += 8;
  };
tag0016a0:
  //D4L = 6;
  saveA3 = A3 + 8;
  saveA2 = A2;
  D2L = LE32(d.Long1812[D3W-12]);
  //D2L = longGear((pnt)&d.Pointer1860+D3W);
  //saveD2L = longGear((pnt)&d.Word1876+D3W);
  saveD2 = LE32(d.Long1828[D3W-12]);
  A2 = saveA2;
  D5L = 8;
  BltOneChar(D2, D3, D5, A2, A3, destWidth, jA0, jA1);
  D2L = saveD2;
  A2 = saveA2;
  A3 = saveA3;;
  //D4L = 6;
  D5L = 18;
  BltOneChar(D2, D3, D5, A2, A3, destWidth, jA0, jA1);

  A3 = saveA3;
  D3W -= 10;
  goto tag001660;




}


//TAG008498
void ShrinkBLT(ui8 *src,
               ui8 *dst,
               i32 srcWidth,  // in pixels
               i32 srcHeight,
               i32 dstWidth,
               i32 dstHeight,
               ui8 *pColorMap)
{         //        8     12     16     18     20     22     24
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D0, D1, D2, D3, D4, D5, D7, D6;
  aReg A2, A3, A4;
  //i32 saveD4=D4,saveD5=D5,saveD6=D6,saveD7=D7;
  //pnt saveA2=A2,saveA3=A3,saveA4=A4,saveA5=A5;
  i16 LOCAL_10;
  i32 LOCAL_8, LOCAL_4;
  i16 pushedD3;
  //A5 = P7;
  //A0 = src;
  // A1 = dst;
  D1L = 0;
  D1W = sw(srcWidth);
  D0L = 15;
  D0W = (i16)((D0W + D1W) & 0xfff0); //# of 16 pixel hunks in source
  D0UW >>= 1;//UWed
  LOCAL_10 = D0W;
  D3W = sw(dstWidth);
  D2L = 10;
  D1L <<= D2B;
  D1L = (D1UL/D3UW) & 0xffff;
  D1L <<= 6;
  A2 = (pnt)D1L;
  D1UL >>= 1;//ULed
  D1L += 0x7fff;
  LOCAL_8 = D1L;
  D1L = 0;
  D1W = sw(srcHeight);
  D1L <<= D2B;
  D1L = (D1UL/(ui16)dstHeight)&0xffff;
  D1L <<= 6;
  LOCAL_4 = D1L;
  D1UL >>= 1;//ULed
  D1L += 0x7fff;
  A4 = (pnt)D1L;
  D1UL >>= 16;//ULed
  D0L = D1UW*D0UW;
  src += D0W;
  pushedD3 = D3W;
tag00850a:
  A3 = (pnt)-1;
  D4L = LOCAL_8;
  D7L = 0x80008000;
tag008518:
  D2L = 0;
  D3L = 0;
tag00851c:
  SWAP(D4);
  D6L = -16;
  D6W &= D4W;

  if ((ui16)D6W != LOW_I16(A3))
  {
    A3 = (pnt)((int)D6W);
    D6UW >>= 1;//UWed
    D0L = LE32(longGear(src+D6W));
    D1L = LE32(longGear(src+D6W+4));
  };
  D6L = 15;
  D6W &= D4W;
  D6W = (i16)(-D6W);
  D6W +=15;
  D5L = 0;
  if (D0L & (1<<D6B)) D5L = 2;

  if (D1L & (1<<D6B)) D5W += 8;
  D6W += 16;
  if (D0L & (1<<D6W)) D5W +=1;
  if (D1L & (1<<D6W)) D5W +=4;
  D5B = *(pColorMap + D5W);
//  if (D5B != 0)
//  {
//    i32 iii=1;
//  };
  switch (D5W/10)
  {
  case 0: break;
  case 1: D2H1 |= D7W; break;
  case 2: D2W |= D7W; break;
  case 3: D2L |= D7L; break;
  case 4: D3H1 |= D7W; break;
  case 5: D2H1 |= D7W; D3H1 |= D7W; break;
  case 6: D2W |= D7W; D3H1 |= D7W; break;
  case 7: D2L |= D7L; D3H1 |= D7W; break;
  case 8: D3W |= D7W; break;
  case 9: D3W |= D7W; D2H1 |= D7W; break;
  case 10: D2W |= D7W; D3W |= D7W; break;
  case 11: D2L |= D7L; D3W |= D7W; break;
  case 12: D3L |= D7L; break;
  case 13: D2H1 |= D7W; D3L |= D7L; break;
  case 14: D2W |= D7W; D3L |= D7L; break;
  case 15: D2L |= D7L; D3L |= D7L; break;
  default:
    NotImplemented(0x85f8);
  };
  pushedD3--;
  if (pushedD3 != 0)
  {
    SWAP(D4);
    D4L += (i32)A2;
    D7L = ((D7L>>1)&0x7fffffff)|((D7L&1)<<31);
    if(D7L >= 0) goto tag00851c;
    longGear(dst) = LE32(D2L); dst+=4;
    longGear(dst) = LE32(D3L); dst+=4;
    goto tag008518;
  };
  longGear(dst) = LE32(D2L); dst+=4;
  longGear(dst) = LE32(D3L); dst+=4;
  dstHeight--;
  if (dstHeight != 0)
  {
    D2L = (i32)A4;
    D3L = D2L;
    D3L += LOCAL_4;
    A4 = (pnt)D3L;
    SWAP(D2);// = ((D2>>16)&0xffff)|((D2&0xffff)<<16);
    SWAP(D3);// = ((D3>>16)&0xffff)|((D3&0xffff)<<16);
    D3W = (i16)(D3W-D2W);
    D3L = (ui16)LOCAL_10 * D3UW;
    src += D3W;
    pushedD3 = sw(dstWidth);
    goto tag00850a;
  };

  //D4=saveD4;D5=saveD5;D6=saveD6;D7=saveD7;
  //A2=saveA2;A3=saveA3;A4=saveA4;A5=saveA5;
}

//#define FusionDebug

#ifdef FusionDebug
extern ui8 *allocatedMemoryList;
extern i8 LScreenBase[320][210];
ui8 *FindMemoryBuffer(ui8 *addr, ui8* &end)
{
  ui8 *buffer;
  ui32 buffersize;
  i32 chIdx;
  for (buffer = allocatedMemoryList; buffer!=NULL; buffer = *(ui8 **)buffer)
  {
    if (addr < buffer + 12) continue;
    buffersize = *((ui32 *)buffer - 4);
    if (addr >= buffer + buffersize) continue;
    end = buffer + buffersize;
    return buffer;
  };
  if ((addr >= (ui8 *)LScreenBase[0]) && ( addr < (ui8 *)LScreenBase[0] +320*210)) 
  {
    end = (ui8 *)LScreenBase + 320*210;
    return (ui8 *)&LScreenBase[0][0];
  };
  if (addr == (ui8 *)d.Byte1222) 
  {
    end = (ui8 *)d.Byte1222 + 128;
    return (ui8 *)d.Byte1222;
  };
  if (addr == (ui8 *)d.Byte1350) 
  {
    end = (ui8 *)d.Byte1350 + 128;
    return (ui8 *)d.Byte1350;
  };
  for (chIdx =0; chIdx<4; chIdx++)
  {
    end = (ui8 *)d.CH16482[chIdx].portrait + 464;
    if (addr == (ui8 *)d.CH16482[chIdx].portrait) return (ui8 *)d.CH16482[chIdx].portrait;
  };
  return NULL;
}
#endif


void TAG0088b2(ui8 *src,
               ui8 *dst,
               RectPos *dstPos,
               i32 SrcOffsetX,  // pixel starting x offset
               i32 SrcOffsetY,  // pixel starting y offset
               i32 SrcByteWidth, // #bytes per line in source
               i32 DestByteWidth,// #bytes per line in destination
               i32 transparentColor)
//                  8      12      16     20     22     24     26     28
{
  dReg D0, D1, D2, D3, D4, D5, D6, D7;
  aReg A0, A1, A3, A4, A5;
  bool H1Valid;
  static ui16 masks[] = {
      0x0000, 0x8000, 0xc000, 0xe000, 0xf000, 0xf800, 0xfc00, 0xfe00,
      0xff00, 0xff80, 0xffc0, 0xffe0, 0xfff0, 0xfff8, 0xfffc, 0xfffe
//      0x0000, 0x0080, 0x00c0, 0x00e0, 0x00f0, 0x00f8, 0x00fc, 0x00fe,
//      0x00ff, 0x80ff, 0xc0ff, 0xe0ff, 0xf0ff, 0xf8ff, 0xfcff, 0xfeff
  };
#ifdef FusionDebug
  ui8 *end;
  ui8 *pm;
  {
    pm = FindMemoryBuffer((ui8 *)src, end);
  };
#endif
  i16 LOCAL_2;
  i16 LOCAL_4;
  i16 LOCAL_6 = 0;

  enum JUMP {
    none = 0,
    Tag008c5e,
    Tag008b30,
    Tag008b42,
    Tag008b56,
    Tag008b6a,
    Tag008b7e,
    Tag008b92,
    Tag008ba6,
    Tag008bba,
    Tag008bce,
    Tag008be2,
    Tag008c06,
    Tag008c18,
    Tag008c2a,
    Tag008c3c,
    Tag008c4e,
    Tag008bf4
  };
  D0L = 0;
  D1L = 0;
  D2L = 0;
  D7L = 0;
  ASSERT(verifyRectPos(dstPos),"rectpos");
  JUMP ja2 = none;

  //i32 saveD4=D4,saveD5=D5,saveD6=D6,saveD7=D7;
  //pnt saveA2=A2,saveA3=A3,saveA4=A4,saveA5=A5;
  A0 = (pnt)src;
  A1 = (pnt)dst;
  //D0W = (i16)transparentColor;
  D1W = (i16)DestByteWidth; // Destination line width
  D2W = (i16)SrcByteWidth;
  D3W = (i16)SrcOffsetY;
  D4W = (i16)SrcOffsetX; // SourceOffsetX
  if (A1 == NULL)
  {
    A1 = (pnt)physbase();
  }

  //A2 = (pnt)P3;
  A5 = (pnt)masks;
  D5L = D4UW & 15; // starting source bit offset within WORD
  D3L = D3UW*D2UW; // #bytes above starting source line
  A0 += D3L; // address of starting line in source.
  D4W = (i16)(D4W-D5W); // Starting source x offset rounded down to nearest 16.
  D4UW >>= 1;//UWed  // Starting source byte offset.
  A0 += D4W;  // Starting source byte address.  D5W = bit number
  D7L = 0;
  D4L = 0;
  if (d.UseByteCoordinates != 0)
  {
    D4B = dstPos->b.x1;  // *(A2++);
    D7B = dstPos->b.x2; // *(A2++);
  }
  else
  {
    D4W = dstPos->w.x1;  //wordGear(A2); A2+=2;
    D7W = dstPos->w.x2; //wordGear(A2); A2+=2;
  };
  // *****441020

  D7W = (i16)(D7W - D4W + 1); // width of result
  SWAP(D7);// = ((D7&0xffff)<<16) | ((D7>>16)&0xffff);
  D3L = 0;
  if (d.UseByteCoordinates != 0)
  {
    D3B = dstPos->b.y1;  // *(A2++);
    D7B = dstPos->b.y2;  // *(A2);
  }
  else
  {
    D3W = dstPos->w.y1;  //wordGear(A2); A2+=2;
    D7W = dstPos->w.y2;  //wordGear(A2);
  };
  // *****441078


  D7W = (i16)(D7W-D3W); // height of result - 1
  SWAP(D7);// = ((D7&0xffff)<<16) | ((D7>>16)&0xffff);
  D6L = 15 & D4UW; // Dest start bit within word.
  D3L = D1UW * D3UW; // Dest bytes above start line
  A1 += D3L; // First byte in destination line
  D4W = (i16)((D4UW - D6UW) >> 1);//UWed
  A1 += D4W; // Starting byte in destination.
  D3L = (15 + D7UW + D5UW) & 0xfff0; // src x of first full word beyond image
  D3UW >>= 1;//UWed // # bytes in src line containing information
  D4W = (i16)(D2W - D3W); // #source bytes to skip at end of line
  A3 = (pnt)D4L; // # bytes to skip at end of source line
  D7W = (i16)(D7W+D6W); // #bits in dest words including leading empty bits
  LOCAL_2 = D7W; // # bits in dest incl leading empty bits
  D3L = (15 + D7UW) & 0xfff0;
  D3UW >>= 1;//UWed // #bytes in dest containing some part of image
  D1W = (i16)(D1W-D3W); // #bytes in dest not containing any image
  A4 = (pnt)D1L; // #bytes in dest not conataining any imaage
  D1W = D5W; // source start bit offset
  D5W = (i16)(D6W - D5W); // D5 = right shift count src->dest
  if (D5W <0)   D5W += 16; // if source is left of dest in current word

  // *****441155

  D3W = D6W; // dest bit offset
  D3W = (i16)(D3W + D3W);
  LOCAL_4 = wordGear(A5 + D3W);  //Dest mask for starting word
  D3W = (i16)(D7W + D7W);
  if (D3W < 32)
  {
    D3W = wordGear(A5 + D3W);
    D3W ^= 0xffff;
    LOCAL_4 |= D3W;
    if (D6W < D5W)
    {
      D1W = (i16)(D1W + D7W - D6W);
      if (D1W < 17)
      {
        A3 -= 8;
      };
    };
  }
  else
  {
    D3W &= 31;
    D3W = wordGear(A5 + D3W);
    D3W ^= 0xffff;
    LOCAL_6 = D3W; // mask for trailing word
  };

  // *****4411c6

  D3W = (i16)(D5W + D5W);
  A5 = (pnt)((int)wordGear(A5 + D3W));
  SWAP(D5);// = ((D5&0xffff)<<16) | ((D5>>16)&0xffff);
  if (transparentColor < 0)
  {
    ja2 = Tag008c5e;
  }
  else
  {
//    D0W += D0W;
//    D3W = D0W;
//    D0W += D0W;
//    D0W += D3W;  // 6 * D0W
    switch(transparentColor)
    {
    case 0: ja2 = Tag008b30; break;
    case 1: ja2 = Tag008b42; break;
    case 2: ja2 = Tag008b56; break;
    case 3: ja2 = Tag008b6a; break;
    case 4: ja2 = Tag008b7e; break;
    case 5: ja2 = Tag008b92; break;
    case 6: ja2 = Tag008ba6; break;
    case 7: ja2 = Tag008bba; break;
    case 8: ja2 = Tag008bce; break;
    case 9: ja2 = Tag008be2; break;
    case 10: ja2 = Tag008bf4; break;
    case 11: ja2 = Tag008c06; break;
    case 12: ja2 = Tag008c18; break;
    case 13: ja2 = Tag008c2a; break;
    case 14: ja2 = Tag008c3c; break;
    case 15: ja2 = Tag008c4e; break;
    };
  };

    // *****4412bf
tag008a32:
  D5H2 = D5H1;
  D5H1 = LOCAL_4;
  if (D5W != 0) // Shift source to destination
  {

    D4W = LOW_I16(A5);
    if (D6W < D5W)
    {
      D0UH2 = D0UH1;
      D1UH2 = D1UH1;
      D2UH2 = D2UH1;
      D3UH2 = D3UH1;
      ui16 *puwA0 = (ui16 *)A0;
      A0 += 8;
      D1UH1 = (i16)(((LE16(*(puwA0++))*0x10001)>>D5W) & D4W);
      D0UH1 = (i16)(((LE16(*(puwA0++))*0x10001)>>D5W) & D4W);
      D3UH1 = (i16)(((LE16(*(puwA0++))*0x10001)>>D5W) & D4W);
#ifdef FusionDebug
      if ((ui8 *)puwA0 >= end)
      {
        char msg[100];
        sprintf(msg,"0x%08x 0x%08x", puwA0, end);
        UI_MessageBox(msg,"88b0",MESSAGE_OK);
        die(2);
      };
#endif
      D2UH1 = (i16)(((LE16(*(puwA0++))*0x10001)>>D5W) & D4W);
/*
      _asm
      {
        LODWAP(A0,D1)
        RORW(D5,D1)
        ANDW(D4,D1)
        SWAP(D1);
        LODWAP(A0,D0)
        RORW(D5,D0)
        ANDW(D4,D0)
        SWAP(D0)
        LODWAP(A0,D3)
        RORW(D5,D3)
        ANDW(D4,D3)
        SWAP(D3)
        LODWAP(A0,D2)
        RORW(D5,D2)
        ANDW(D4,D2)
        SWAP(D2)
      };
     */
    };
tag008a62:

    ui16 *puwA0 = (ui16 *)A0;
    A0 += 8;
    H1Valid = false;
    if (D5W < D7W) 
    {
      D0UH2 =  (i16) ((LE16(*(puwA0++))*0x10001)>>D5W);
      H1Valid = true;
    };
    D6UH1 =  (i16)  D0UH2;
    D1UH1 &= (i16)  D4UH2;
    D0UH2 &= (i16) ~D4UH2;
    D0UH2 |= (i16)  D1UH1;
    D1UH1 =  (i16)  D6UH1;

    /*
    _asm
    {
      SWAP(D6)
      LODWAP(A0,D0)
      RORW(D5,D0)
      MOVEW(D0,D6)
      SWAP(D1)
      ANDW(D4,D1)
      NOTW(D4)
      ANDW(D4,D0)
      ORW(D1,D0)
      MOVEW(D6,D1)
      SWAP(D1)
      //
      //
      //
      //
    */
    if (H1Valid) D1UH2 =  (ui16) ((LE16(*(puwA0++))*0x10001)>>D5W);
    D6UH1 =  (ui16)  D1UH2;
    D1UH2 &= (ui16) ~D4UH2;
    D0UH1 &= (ui16)  D4UH2;
    D1UH2 |= (ui16)  D0UH1;
    D0UH1 =  (ui16)  D6UH1;
    /*

      LODWAP(A0,D1)
      RORW(D5,D1)
      MOVEW(D1,D6)
      SWAP(D0)
      ANDW(D4,D1)
      NOTW(D4)
      ANDW(D4,D0)
      ORW(D0,D1)
      MOVEW(D6,D0)
      SWAP(D0)

      //
      //
      //
      //
      */
    if (H1Valid) D2UH2 =  (ui16) ((LE16(*(puwA0++))*0x10001)>>D5W);
    D6UH1 =   D2UH2;
    D3UH1 &=  D4UH2;
    D2UH2 &= ~D4UH2;
    D2UH2 |=  D3UH1;
    D3UH1 =   D6UH1;
    /*

      LODWAP(A0,D2)
      RORW(D5,D2)
      MOVEW(D2,D6)
      SWAP(D3)
      ANDW(D4,D3)
      NOT(D4)
      ANDW(D4,D2)
      ORW(D3,D2)
      MOVEW(D6,D3)
      SWAP(D3)

      //
      //
      //
      //
     */
    if (H1Valid) D3UH2 =  (ui16) ((LE16(*(puwA0++))*0x10001)>>D5W);
    D6UH1 =   D3UH2;
    D3UH2 &= ~D4UH2;
    D2UH1 &=  D4UH2;
    D3UH2 |=  D2UH1;
    D2UH1 =   D6UH1;
    /*

      LODWAP(A0,D3)
      RORW(D5,D3)
      MOVEW(D3,D6)
      SWAP(D2)
      ANDW(D4,D3)
      NOTW(D4)
      ANDW(D4,D2)
      ORW(D2,D3)
      MOVEW(D6,D2)
      SWAP(D2)
      //
      //
      //
      SWAP(D6)
      SWAP(D5)
    };
    */
#ifdef FusionDebug
    if (H1Valid && ((ui8 *)puwA0 > end))
    {
      char msg[100];
      sprintf(msg,"0x%08x 0x%08x", puwA0, end);
      UI_MessageBox(msg,"88b0",MESSAGE_OK);
      die(2);
    };
#endif
    D5L = ((D5L&0xffff)<<16) | ((D5L>>16)&0xffff);
    goto jumpA2;
  };

  // *****44139d **

tag008aba:
  D0W = LE16(wordGear(A0)); A0+=2;
  //D0W = ((*((ui16 *)A0)) * 0x10001) >> 8; A0+=2;
  D1W = LE16(wordGear(A0)); A0+=2;
  //D1W = ((*((ui16 *)A0)) * 0x10001) >> 8; A0+=2;
  D2W = LE16(wordGear(A0)); A0+=2;
  //D2W = ((*((ui16 *)A0)) * 0x10001) >> 8; A0+=2;
  D3W = LE16(wordGear(A0)); A0+=2;
  //D3W = ((*((ui16 *)A0)) * 0x10001) >> 8; A0+=2;
#ifdef FusionDebug
  if ((ui8 *)A0 >= end)
  {
    char msg[100];
    sprintf(msg,"0x%08x 0x%08x", A0, end);
    UI_MessageBox(msg,"88b0",MESSAGE_OK);
    die(2);
  };
#endif
  SWAP(D5);//D5 = ((D5&0xffff)<<16) | ((D5>>16)&0xffff);
  goto jumpA2;
  // *****441441



    // *****44162b

tag008ac2:
  D4W ^= 0xffff;
  D0W &= D4W;
  D1W &= D4W;
  D2W &= D4W;
  D3W &= D4W;
  D4W ^= 0xffff;
  wordGear(A1) &= LE16(D4W);
  wordGear(A1) |= LE16(D0W); A1+=2;
  wordGear(A1) &= LE16(D4W);
  wordGear(A1) |= LE16(D1W); A1+=2;
  wordGear(A1) &= LE16(D4W);
  wordGear(A1) |= LE16(D2W); A1+=2;
  wordGear(A1) &= LE16(D4W);
  wordGear(A1) |= LE16(D3W); A1+=2;
    // *****4414d1

tag008ade:

  D7W -= 16;
  if (D7W > 0)
  {
    //4414eb
    if (D7W >= 16)
    {
      //4414f5
      D5W = 0;
      SWAP(D5);// = ((D5&0xffff)<<16) | ((D5>>16)&0xffff);
tag008aee:
      if (D5W == 0) 
      {
        goto tag008aba;
      };
      D4W = LOW_I16(A5);
      goto tag008a62;
    };
    //4416f9
    D5W = LOCAL_6;
    SWAP(D5);// = ((D5&0xffff)<<16) | ((D5>>16)&0xffff);
    if (D5W < D7W) goto tag008aee;
    SWAP(D0);//D0 = ((D0&0xffff)<<16) | ((D0>>16)&0xffff);
    SWAP(D1);// = ((D1&0xffff)<<16) | ((D1>>16)&0xffff);
    SWAP(D2);// = ((D2&0xffff)<<16) | ((D2>>16)&0xffff);
    SWAP(D3);// = ((D3&0xffff)<<16) | ((D3>>16)&0xffff);
    D4W = D0W;
    D0W = D1W;
    D1W = D4W;
    D4W = D2W;
    D2W = D3W;
    D3W = D4W;
    SWAP(D5);// = ((D5&0xffff)<<16) | ((D5>>16)&0xffff);
    goto jumpA2;
  };

  // *****441942

  SWAP(D7);// = ((D7&0xffff)<<16) | ((D7>>16)&0xffff);
  D7W--;
  if (D7W < 0) goto tag008c90;
  SWAP(D7);// = ((D7&0xffff)<<16) | ((D7>>16)&0xffff);
  D7W = LOCAL_2;
  A0 += LOW_I16(A3);
  A1 += LOW_I16(A4);
  goto tag008a32;

    // *****441455

tag008b30:
  D4W = (i16)(D0W | D1W | D2W | D3W);
  D4W ^= 0xffff;
  D4W |= D5W;
  if (D4W != 0) goto tag008ac2; //44162b
  goto tag008c64;  //441473
tag008b42:
  D4W = (i16)(D1W | D2W | D3W);
  D4W ^= 0xffff;
  D4W = (i16)((D4W & D0W) | D5W);
  if (D4W != 0) goto tag008ac2;
  goto tag008c64;
tag008b56:
  D4W = (i16)(D0W | D2W | D3W);
  D4W ^= 0xffff;
  D4W = (i16)((D4W & D1W) |D5W);
  if (D4W != 0) goto tag008ac2;
  goto tag008c64;
tag008b6a:
  D4W = (i16)(D2W | D3W);
  D4W ^= 0xffff;
  D4W = (i16)((D4W & D0W & D1W) | D5W);
  if (D4W != 0) goto tag008ac2;
  goto tag008c64;
tag008b7e:
  D4W = (i16)(D0W | D1W | D3W);
  D4W ^= 0xffff;
  D4W = (i16)((D4W & D2W) | D5W);
  if (D4W != 0) goto tag008ac2;
  goto tag008c64;
tag008b92:
  D4W = (i16)(D1W | D3W);
  D4W ^= 0xffff;
  D4W = (i16)((D4W & D0W & D2W) | D5W);
  if (D4W != 0) goto tag008ac2;
  goto tag008c64;
tag008ba6:
  D4W = (i16)(D0W | D3W);
  D4W ^= 0xffff;
  D4W = (i16)((D4W & D1W & D2W) | D5W);
  if (D4W != 0) goto tag008ac2;
  goto tag008c64;
tag008bba:
  D4W = D3W;
  D4W ^= 0xffff;
  D4W = (i16)((D4W & D0W & D1W & D2W) | D5W);
  if (D4W != 0) goto tag008ac2;
  goto tag008c64;
tag008bce:
  D4W = (i16)(D0W | D1W | D2W);
  D4W ^= 0xffff;
  D4W = (i16)((D4W & D3W) | D5W);
  if (D4W != 0) goto tag008ac2;
  goto tag008c64;
tag008be2:
  D4W = (i16)(D1W | D2W);
  D4W ^= 0xffff;
  D4W = (i16)((D4W & D0W & D3W) | D5W);
  if (D4W != 0) goto tag008ac2;
  goto tag008c64;
tag008bf4:
  D4W = (i16)(D0W | D2W);
  D4W ^= 0xffff;
  D4W = (i16)((D4W & D1W & D3W) | D5W);
  if (D4W != 0) goto tag008ac2;
  goto tag008c64;
tag008c06:
  D4W = D2W;
  D4W ^= 0xffff;
  D4W = (i16)((D4W & D0W & D1W & D3W) | D5W);
  if (D4W != 0) goto tag008ac2;
  goto tag008c64;
tag008c18:
  D4W = (i16)(D0W | D1W);
  D4W ^= 0xffff;
  D4W = (i16)((D4W & D2W & D3W) | D5W);
  if (D4W != 0) goto tag008ac2;
  goto tag008c64;
tag008c2a:
  D4W = D1W;
  D4W ^= 0xffff;
  D4W = (i16)((D4W & D0W & D2W & D3W) | D5W);
  if (D4W != 0) goto tag008ac2;
  goto tag008c64;
tag008c3c:
  D4W = D0W;
  D4W ^= 0xffff;
  D4W = (i16)((D4W & D1W & D2W & D3W) | D5W);
  if (D4W != 0) goto tag008ac2;
  goto tag008c64;
tag008c4e:
  D4W = (i16)((D0W & D1W & D2W & D3W) | D5W);
  if (D4W != 0) goto tag008ac2;
  goto tag008c64;
tag008c5e:
  D4W = D5W;
  if (D4W != 0) goto tag008ac2;

  //441473
tag008c64:
  wordGear(A1) = LE16(D0W); A1+=2;
  // *((ui16 *)A1) = ((ui16)D0W * 0x10001)>>8; A1+=2;
  wordGear(A1) = LE16(D1W); A1+=2;
  // *((ui16 *)A1) = ((ui16)D1W * 0x10001)>>8; A1+=2;
  wordGear(A1) = LE16(D2W); A1+=2;
  // *((ui16 *)A1) = ((ui16)D2W * 0x10001)>>8; A1+=2;
  wordGear(A1) = LE16(D3W); A1+=2;
  // *((ui16 *)A1) = ((ui16)D3W * 0x10001)>>8; A1+=2;
  goto tag008ade;


  // *****44141e = jumA2

jumpA2:
  switch(ja2)
  {
  case Tag008c5e: goto tag008c5e;
  case Tag008b30: goto tag008b30;
  case Tag008b42: goto tag008b42;
  case Tag008b56: goto tag008b56;
  case Tag008b6a: goto tag008b6a;
  case Tag008b7e: goto tag008b7e;
  case Tag008b92: goto tag008b92;
  case Tag008ba6: goto tag008ba6;
  case Tag008bba: goto tag008bba;
  case Tag008bce: goto tag008bce;
  case Tag008be2: goto tag008be2;
  case Tag008c06: goto tag008c06;
  case Tag008c18: goto tag008c18;
  case Tag008c2a: goto tag008c2a;
  case Tag008c3c: goto tag008c3c;
  case Tag008c4e: goto tag008c4e;
  case Tag008bf4: goto tag008bf4;
  default:
    NotImplemented(0x8c90);
  };
    // *****4419ac

    // *****4419b9

tag008c90:
  //D4=saveD4;D5=saveD5;D6=saveD6;D7=saveD7;
  //A2=saveA2;A3=saveA3;A4=saveA4;A5=saveA5;
  return;
}

// *********************************************************
//
// *********************************************************
void TAG008c98(ui8 *src,  //8
               ui8 *dst, //12
               ui8 *P3,  //16
               ui8 *P4,  //20
               RectPos *P5, //24
               i16 P6,  //28
               i16 P7,  //30
               i16 P8,  //32
               i16 P9,  //34
               i16 P10, //36 // srcOffsetX
               i16 P11) //38 // srcOffsetY
{
  dReg D0, D1, D2, D3, D5, D6, D7;
  aReg A0, A1, A2;
  RectPos *rectA2;
  i32 ja1=-1, ja3, ja4;
  i16 w_2;
  D3L = 0;
  D7L = 0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D0W = P9;
  if (D0B == -1) goto tag008cb4;
  if (D0B >= 0) goto tag008cba;
  P9 &= 0x7f;
tag008cb4:
  ja3 = 0x8dc8;
  goto tag008d28;
tag008cba:
  ja3 = 0x8d8c;
  switch(D0W)
  {
  case 0: ja1 = 0x8dc8; break;
  case 1: ja1 = 0x8dc6; break;
  case 2: ja1 = 0x8da6; break;
  case 3: ja1 = 0x8dc4; break;
  case 4: ja1 = 0x8dac; break;
  case 5: ja1 = 0x8db2; break;
  case 6: ja1 = 0x8daa; break;
  case 7: ja1 = 0x8dc2; break;
  case 8: ja1 = 0x8da0; break;
  case 9: ja1 = 0x8dba; break;
  case 10: ja1 = 0x8da4; break;
  case 11: ja1 = 0x8db8; break;
  case 12: ja1 = 0x8d9e; break;
  case 13: ja1 = 0x8db0; break;
  case 14: ja1 = 0x8d9c; break;
  case 15: ja1 = 0x8dc0; break;
  };
tag008d28:
  A0 = (aReg)src;
  D6W = P6;
  D7W = P7;
  A0 += 8*D7W;
  SWAP(D7);
  rectA2 = P5;
  D3L = 0;
  D0L = 0;
  if (d.UseByteCoordinates)
  {
    D3B = rectA2->b.x1;
    D0B = rectA2->b.x2;
  }
  else
  {
    D3W = rectA2->w.x1;
    D0W = rectA2->w.x2;
  };
  D0W = (i16)((D0W - D3W)/16 + 1);
  w_2 = D0W;
  D3L = 0;
  D5L = 0;
  if (d.UseByteCoordinates)
  {
    D3B = rectA2->b.y1;
    D5B = rectA2->b.y2;
  }
  else
  {
    D3W = rectA2->w.y1;
    D5W = rectA2->w.y2;
  };
  D5W = (i16)((D5W - D3W + 1) * D0W - 1);
  D7W = D5W;
  ja4 = ja1;
  A1 = (aReg)P3;
  A2 = (aReg)P4;
tag008d86:
  D0W = LE16(wordGear(A0+0));
  D1W = LE16(wordGear(A0+2));
  D2W = LE16(wordGear(A0+4));
  D3W = LE16(wordGear(A0+6));
  A0 += 8;
  switch (ja3)
  {
  case 0x8d8c: goto tag008d8c;
  case 0x8dc8: goto tag008dc8;
  default: NotImplemented(0x8d8a);
  };
tag008d8c:
  ASSERT (A1!=NULL,"A1");
//  if (A1==NULL)D5W = 0x1234;
//  else
//  {
    D5W = LE16(wordGear(A1));
    A1 += 8;
//  };
  D0W &= D5W;
  D1W &= D5W;
  D2W &= D5W;
  D3W &= D5W;
  D5W ^= 0xffff;
  switch (ja4)
  {
  case 0x8dc8: goto tag008dc8;
  case 0x8da4: goto tag008da4;
  default: NotImplemented(0x8d9a);
  };
/*
*/
tag008da4:
  D3W |= D5W;
  D1W |= D5W;
  goto tag008dc8;
/*
*/
tag008dc8:
  wordGear(A2+0) = LE16(D0W);
  wordGear(A2+2) = LE16(D1W);
  wordGear(A2+4) = LE16(D2W);
  wordGear(A2+6) = LE16(D3W);
  A2 += 8;
  SWAP(D7);
  D7W++;
  if (D6W != D7W)
  {
tag008dd6:
    SWAP(D7);
    if ((D7W--) != 0) goto tag008d86;
    goto tag008de8;
  };
  D7W = 0;
  A0 = (aReg)src;
  goto tag008dd6;
tag008de8:
  TAG0088b2((ui8 *)P4,      // Src
            (ui8 *)dst,     // dst
            P5,      // rect
            P10,     // SrcOffsetX
            P11,     // SrcOffsetY
            8 * w_2, // SrcByteWidth
            P8,      // DstByteWidth
            P9);     // ???
}

ui16 data009032[] =
{
  0x0000, 0x8000, 0xc000, 0xe000, 0xf000, 0xf800, 0xfc00, 0xfe00,
  0xff00, 0xff80, 0xffc0, 0xffe0, 0xfff0, 0xfff8, 0xfffc, 0xfffe,
  0xffff
};

void tagja0(i32 ja0, pnt A1, dReg D0)
{
  switch (ja0)
  {
  case 0: break;
  case 1: wordGear(A1+0) |= LE16(D0W);
          break;
  case 2: wordGear(A1+2) |= LE16(D0W);
          break;
  case 3: wordGear(A1+2) |= LE16(D0W);
          wordGear(A1+0) |= LE16(D0W);
          break;
  case 4: wordGear(A1+4) |= LE16(D0W);
          break;
  case 5: wordGear(A1+4) |= LE16(D0W);
          wordGear(A1+0) |= LE16(D0W);
          break;
  case 6: wordGear(A1+2) |= LE16(D0W);
          wordGear(A1+4) |= LE16(D0W);
          break;
  case 7: wordGear(A1+2) |= LE16(D0W);
          wordGear(A1+0) |= LE16(D0W);
          wordGear(A1+4) |= LE16(D0W);
          break;
  case 8: wordGear(A1+6) |= LE16(D0W);
          break;
  case 9: wordGear(A1+6) |= LE16(D0W);
          wordGear(A1+0) |= LE16(D0W);
          break;
  case 10: wordGear(A1+6) |= LE16(D0W);
           wordGear(A1+2) |= LE16(D0W);
           break;
  case 11: wordGear(A1+6) |= LE16(D0W);
           wordGear(A1+2) |= LE16(D0W);
           wordGear(A1+0) |= LE16(D0W);
           break;
  case 12: wordGear(A1+6) |= LE16(D0W);
           wordGear(A1+4) |= LE16(D0W);
           break;
  case 13: wordGear(A1+6) |= LE16(D0W);
           wordGear(A1+4) |= LE16(D0W);
           wordGear(A1+0) |= LE16(D0W);
           break;
  case 14: wordGear(A1+6) |= LE16(D0W);
           wordGear(A1+4) |= LE16(D0W);
           wordGear(A1+2) |= LE16(D0W);
           break;
  case 15: wordGear(A1+6) |= LE16(D0W);
           wordGear(A1+4) |= LE16(D0W);
           wordGear(A1+0) |= LE16(D0W);
           wordGear(A1+2) |= LE16(D0W);
           break;
  };
}


//   TAG008e60
void FillRectangle(ui8 *dest,RectPos *dstRect,i32 color,i32 destWidth)
{
  dReg D0, D1, D2, D3, D4, D5, D6, D7;
  aReg A1;
  ui16 *puwA0;
  i32 ja0;
  D3L = 0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //A0 = P2;
  ASSERT(verifyRectPos(dstRect),"rectpos");
  if (d.UseByteCoordinates != 0)
  {
    D0W = dstRect->b.x1;
    D1W = dstRect->b.x2;
    D2W = dstRect->b.y1;
    D3W = dstRect->b.y2;
  }
  else
  {
    D0W = dstRect->w.x1;
    D1W = dstRect->w.x2;
    D2W = dstRect->w.y1;
    D3W = dstRect->w.y2;
  };
  D0H1 = D2W;// D0 = y,x
  A1 = (aReg)dest;
  D3W = (i16)(D3W-D2W);//D3W = height
  SWAP(D3); //D3 = (D3U&0xffff)|((D3W&0xffff)<<16);
  D2L = D2W * destWidth; // Offset to row start
  A1 += D2W;//First byte of first row
  puwA0 = data009032;
  D6W = (i16)(15&D0W);//Bit offset of first pixel
  D6W = puwA0[D6W];//Leading mask of bits to keep
  D0UW >>= 4;//UWed//#groups of 16 pixels before start
  D4W = (i16)((15&D1W)+1);//offset of last pixel +1 (1 to 16)
  D4W = puwA0[D4W]; // mask of trailing bits to replace
  D4W ^= 0xffff; // mask of trailing bits to keep
  D1UW >>= 4;//UWed//#16-bit pixels groups before end
  D2W = (i16)(D0W<<3);//#bytes in full groups before start
  A1 += D2W;//address of first group to change
  D1W = (i16)(D1W-D0W);//difference between end and start 16-bit groups
  D3W = D1W;//difference between end and start group number
  if (D3W == 0) // start and end in same group?
  {
    D6W |= D4W;
  };
  D3W--;//#full groups between start and end
  SWAP(D3);//D3 = (D3U&0xffff)|((D3W&0xffff)<<16);
        //#full groups, height-1
  D1W = (i16)((D1W+1)<<3);//#bytes affected
  D7W = sw(destWidth);
  D7W = (i16)(D7W-D1W);//Dest increment to next line
  D5W = (i16)(D4W ^ 0xffff);//Trailing replace mask
  D1W = (i16)(D6W ^ 0xffff);//Leading replace mask
  D7H1 = D7W;
  D7W = 0;//Row increment. 0
  if (color&0x8000)
  {
    SWAP(D0);//D0 = (D0UW&0xffff)|((D0W&0xffff)<<16);
    D7W = 0x5555;
    if ((D0W&1)==0)
    {
      D7W<<=1;
    };
  };
  D0W = (i16)(15&color);
  ja0 = D0W;
















tag008f6e:
  D2W = D3H1;//#full groups
  D0W = (i16)(D6W | D7W); //Lead keep
  wordGear(A1+0) &= LE16(D0W);
  wordGear(A1+2) &= LE16(D0W);
  wordGear(A1+4) &= LE16(D0W);
  wordGear(A1+6) &= LE16(D0W);
  D7W ^= 0xffff;//Trail replace
  D0W = (i16)(D1W & D7W);//Lead replace
  D7W ^= 0xffff;//Trail keep
  tagja0(ja0,A1,D0);
  A1 += 8;
  if (D2W >= 0) //#full groups
  {
    if ((--D2W) >= 0)
    {
      D0W = (i16)(D7W ^ 0xffff);
      do
      {
        wordGear(A1+0) &= LE16(D7W);
        wordGear(A1+2) &= LE16(D7W);
        wordGear(A1+4) &= LE16(D7W);
        wordGear(A1+6) &= LE16(D7W);
        tagja0(ja0,A1,D0);
        A1 += 8;
      } while ((--D2W) >= 0);
    };
    D0W = (i16)(D4W | D7W);//trailing keep
    wordGear(A1+0) &= LE16(D0W);
    wordGear(A1+2) &= LE16(D0W);
    wordGear(A1+4) &= LE16(D0W);
    wordGear(A1+6) &= LE16(D0W);
    D7W ^= 0xffff;//trailing replace
    D0W = (i16)(D5W & D7W);
    D7W ^= 0xffff;
    tagja0(ja0, A1, D0);
    A1 += 8;
  };
  A1 += D7H1;
  if (D7W != 0) D7W ^= 0xffff;
  D3W--;
  if (D3W >= 0) goto tag008f6e;
}


class OUTBUF
{
  char *outbuf;
  i32 outcol;
public:
  OUTBUF(){outbuf=NULL;outcol=0;};
  ~OUTBUF(){if(outbuf!=NULL)UI_free(outbuf);outbuf=NULL;};
  void addch(char c);
  char  prevch(){return outbuf[outcol-1];};
  char *buf(){return outbuf;};
};


void OUTBUF::addch(char c)
{
  i32 outsize;
  outsize = (outcol/20+1)*20;
  outbuf = (char *)UI_realloc(outbuf, outsize, MALLOC094);
  outbuf[outcol++] = c;
}

// *********************************************************
//
// *********************************************************
//   TAG01ab88
void PrintWithSubstitution(const char *txt, ui32 color, bool translate)
{
  const char *A3;
  //i8  b_132[128];
  OUTBUF outbuf;
  i32 srclen;
  if (translate) txt = TranslateLanguage(txt);
  const char *p_4 = "HUH???";
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A3 = txt;
  srclen = strlen(txt);
  // *(A2++) = 10;
  outbuf.addch(10);
  do
  {
    if (*A3 == '@')
    {
      A3++;
      //D0B = *(A2-1);
      if (outbuf.prevch() != 10)
      {
        // *(A2++) = ' ';
        outbuf.addch(' ');
      };
      switch (*A3)
      {
      case 'p':
        p_4 = d.CH16482[d.AttackingCharacterOrdinal-1].name;
        break;
      case 'N':
        outbuf.addch(10);
        break;
      default:
        p_4 = "HUH???";
        NotImplemented(0x1abde);
      };
      while (*p_4 != 0) outbuf.addch(*p_4++);
      // *A2 = 0;
      //StrCat(b_132, p_4);
      //A2 += StrLen(p_4);
      // *(A2++) = ' ';
      outbuf.addch(' ');
    }
    else
    {
      // *(A2++) = *A3;
      outbuf.addch(*A3);
    }
//
  } while (*(A3++) != 0);
  // *(A2) = 0;
  outbuf.addch(0);
  if (outbuf.buf()[1] != 0)
  {
    QuePrintLines((i16)color, outbuf.buf());
  };
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _FadeToPalette(const PALETTE *P1) //TAG01f04e
{//void
  static dReg D0, D1, D4, D5, D6, D7;
  static i16 *pPalette;
  static i32 i;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
  pPalette = (i16 *)globalPalette;
  // Set supervisor mode
  for (D4W=0; D4W<8; D4W++)
  {
    //for (i=0; i<40; i++) // slowly!
    wvbl(_1_);
    display();
    for (D5W=0; D5W<16; D5W++)
    {
      D7W = pPalette[D5W];
      D6W = P1->color[D5W];
      D0W = (i16)(D7UW & 7);
      D1W = (i16)(D6UW & 7);
      if (D0W > D1W) D7W--;
      if (D1W > D0W) D7W++;


      D0W = (i16)(D7W & 0x70);
      D1W = (i16)(D6W & 0x70);
      if (D0W > D1W) D7W -= 16;
      if (D1W > D0W) D7W += 16;

      D0W = (i16)(D7W & 0x0700);
      D1W = (i16)(D6W & 0x0700);
      if (D0W > D1W) D7W -= 256;
      if (D1W > D0W) D7W += 256;
      pPalette[D5W] = D7W;
    };
  };
  // Exit supervisor mode
  RETURN;
}

// *********************************************************
//
// *********************************************************
i32 LocateNthGraphic(i32 n) // TAG021714
{
  dReg D0;
  D0L = d.NumGraphic*4+2; // Skip index and count.
  for (i32 i=0; i < n; i++) D0L += d.GraphicCompressedSizes[i];
  return D0L;
}


/*
// *********************************************************
// Clear Graphic and add to 'free' list
// *********************************************************
//   TAG021888
void FreeGraphicMemory(ITEMQ *pgGraphic)
{ //Each entry in the d.pgUnused list has the form
  //  long 0  = size
  //  long 4  = back link
  //  long 8  = forward link
  dReg D6, D7;
  ITEMQ *pgA2, *pgA3;
  //VerifyGraphicFreeList();
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pgA2 = pgGraphic;
  if (d.pgUnused == NULL)
  {
    d.pgUnused = pgA2;
    pgA2->pgNext(NULL);
    pgA2->pgPrev(NULL);
    return;
  };
  pgA3 = d.pgUnused;
  ASSERT(pgA2 != pgA3,"pga2");
  D7L = pgA2->size; // Size of new entry
  if (D7L >= pgA3->size)
  { // The newly released space is larger than the first
    // entry in the unused list.  Add it to the front
    // of the unused list.
    d.pgUnused = pgA2;
    pgA2->pgPrev(NULL); //link to previous
    pgA2->pgNext(pgA3); //link to next
    pgA3->pgPrev(pgA2); //back link to new entry.
    return;
  };
  // The new entry is smaller than the first entry
  // in the unused list.
  for (D6W=0; (pgA2=pgA3->pgNext())!=NULL; pgA3=pgA2)
  {
    ASSERT(pgGraphic != pgA2,"ppgraphic");
    if (D7L >= pgA2->size) // Find an entry smaller than the new one.
    {
      pgA2->pgPrev(pgGraphic); // The next points back to us
      pgA3->pgNext(pgGraphic); // The previous points fwd to us
      pgGraphic->pgPrev(pgA3); // We point back
      pgGraphic->pgNext(pgA2); // We point forward
      D6W = 1; //
      break;
    };
//continue

  }; // while
  if (D6W != 0) return;
  // It appears the new entry is the smallest.
  pgA3->pgNext(pgGraphic); // The last point fwd to us.
  pgGraphic->pgPrev(pgA3); // We point back to old last entry.
  pgGraphic->pgNext(NULL); // We have to fwd link.
//
  //VerifyGraphicFreeList();
}
*/

// *********************************************************
//
// *********************************************************
void ReadGraphic(i16 graphicNumber, ui8 *buffer, i32 maxSize) // TAG021af2
{
  dReg D7;
  i32 curCluster;
  i32 bytesNeeded;
  i32 bytesToRead;
  i32 graphicStart;
  i32 clusterOffset;
  i32 bytesAvailable;
  i32 bytesToMove;
  //i32 saveD4=D4, saveD5=D5, saveD6=D6, saveD7=D7;
//  while (d.Word23228!=0) {};
  graphicStart = LocateNthGraphic(graphicNumber);
  D7L = graphicStart;
  bytesNeeded = d.GraphicCompressedSizes[graphicNumber];
  if (bytesNeeded > maxSize) bytesNeeded = maxSize;
  curCluster = graphicStart >> 10;
  while (bytesNeeded > 0)
  {
    if ((curCluster != d.ClusterInCache) || (d.CacheInvalid != 0))
    {
      d.ClusterInCache = (i16)curCluster;
      bytesToRead = d.GraphicFileEOF-(curCluster<<10);
      if (bytesToRead > 1024) bytesToRead = 1024;


      LSEEK(curCluster<<10,d.GraphicHandle,SEEK_SET);
      READ(d.GraphicHandle,bytesToRead,d.ClusterCache );
      d.CacheInvalid = 0;
    };
    clusterOffset = D7L - (curCluster<<10);
    bytesAvailable = 1024-clusterOffset;
    bytesToMove = Smaller(bytesNeeded, bytesAvailable);
    MemMove(d.ClusterCache+clusterOffset,buffer,(i16)bytesToMove);
    bytesNeeded -= bytesToMove;
    D7L          += bytesToMove;
    buffer      += bytesToMove;
    curCluster++;
  };
  //D4=saveD4;D5=saveD5;D6=saveD6;D7=saveD7;
}

// *********************************************************
//
// *********************************************************
void openGraphicsFile() // TAG021d36
{
  i32 m, n;
  ui32 o;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//  while (d.Word23228 != 0) {};
  if ((d.Word23244++) ==0) // Reference count??
  {
    d.GraphicHandle = OPEN("graphics.dat","rb");
    if (d.GraphicHandle < 0)
    {
      UI_MessageBox("Cannot find 'graphics.dat'",NULL,MESSAGE_OK);
      UI_MessageBox(helpMessage,"Help",MESSAGE_OK);
      die(0xbad);
    };
    d.Word23244++; //Increment one extra time so that the 
                   //file will never be closed.  We do this
                   //so that the player cannot substitute
                   //another graphics.dat during play.
  };
  n = graphicSignature1 | graphicSignature2;
  o = (n | (n>>16)) & 0xffff;
  m = expectedGraphicSignature1 | expectedGraphicSignature2;
  if (n==0)
  {
    Signature(d.GraphicHandle, &graphicSignature1, &graphicSignature2);
    n = graphicSignature1 | graphicSignature2;
    o = (n | (n>>16)) & 0xffff;
    if (m!=0)
    {
      if (   (graphicSignature1 != expectedGraphicSignature1)
          || (graphicSignature2 != expectedGraphicSignature2))
      {
        char msg[80];
        sprintf(msg,"Incorrect Graphics File signature\n"
                    "%08x %08x",
                    graphicSignature2,
                    graphicSignature1);
        die(0x3c6c,msg);
      };
    };
  };
  if (   (EDBT_GraphicsSignature_data != 0xffffffff)
       &&(EDBT_GraphicsSignature_data != 0)
     )
  {
    if (EDBT_GraphicsSignature_data != o)
    {
      char msg[80];
      sprintf(msg,"Incorrect Graphics File signature\n"
                  "Actuatl Signature = %d", o);
      die(0x3c6c,msg);
    };
  };

}


// *********************************************************
//
// *********************************************************
void closeGraphicsFile() //TAG021d76
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ASSERT(d.Word23244 > 1,"word23244");
  d.Word23244--;
  if (d.Word23244==1)
  {
//    while (d.Word23228 != 0){};
    LSEEK(0, d.GraphicHandle, SEEK_SET);
  };
}

// *********************************************************
//
// *********************************************************
void OpenCSBgraphicsFile()
{
  ui32 m, n, o;
  static bool errorMsg = false;
  if (CSBgraphicsFile.f >= 0)
  {
    LSEEK(0, CSBgraphicsFile.f, SEEK_SET);
    return;
  };
  n = CSBgraphicSignature1 | CSBgraphicSignature2;
  o = (n | (n>>16)) & 0xffff;
  if (o == 0) o = 1;
  m = expectedCSBgraphicSignature1 | expectedCSBgraphicSignature2;
  CSBgraphicsFile.f = OPEN("CSBgraphics.dat","rb");
  if (CSBgraphicsFile.f < 0)
  {
    if (!errorMsg)
    {
      UI_MessageBox("Cannot find 'CSBgraphics.dat'",NULL,MESSAGE_OK);
      UI_MessageBox(helpMessage,"Help",MESSAGE_OK);
      if (m != 0) die(0xbad);
      errorMsg = true;
    };
    return;
  };
  if ((n==0) && ((m!=0)||((EDBT_CSBGraphicsSignature_data != 0) && (EDBT_Debuging_data == 0))))
  {
    Signature(CSBgraphicsFile.f, &CSBgraphicSignature1, &CSBgraphicSignature2);
    n = CSBgraphicSignature1 | CSBgraphicSignature2;
    o = (n | (n>>16)) & 0xffff;
    if (o == 0) o = 1;
  };
  if ((n==0) && (m!=0))
  {
    if (   (CSBgraphicSignature1 != expectedCSBgraphicSignature1)
        || (CSBgraphicSignature2 != expectedCSBgraphicSignature2))
    {
      char msg[80];
      sprintf(msg,"Incorrect CSBGraphics File signature\n"
                  "%08x %08x",
                  CSBgraphicSignature2,
                  CSBgraphicSignature1);
      die(0x3c6c,msg);
    };
  };
  if ( (EDBT_CSBGraphicsSignature_data != 0) && (EDBT_Debuging_data == 0))
  {
    if ( o != EDBT_CSBGraphicsSignature_data)
    {
      char msg[80];
      sprintf(msg,"Incorrect CSBGraphics File signature\n"
                  "Actual Signature = %d", o);
      die(0x3c6c,msg);
    };
  };
  LSEEK(0, CSBgraphicsFile.f, SEEK_SET);
}

void info(char *msg, unsigned int n)
{
  static int inf[100];
  static int i=0;
  inf[i++] = *((unsigned int *)msg);
  inf[i++] = n;
  if (i>=100) i=0;
//  if (n & 0x80000000)
  {
    char t[80];
    sprintf(t,"%s %d",msg,n&0x7ffffff);
    UI_MessageBox(t,"Debug Graphics",MESSAGE_OK);
//    FILE *f = UI_fopen("CSBwin\\Info.txt","wb");
//    if (f==NULL) return;
//    fwrite(inf,1,400,f);
//    fclose(f);
  };
}

// *********************************************************
//
// *********************************************************
void ReadGraphicsIndex() // TAG021d9a
{
  dReg D0, D3, D6;
  bool success = false;
  bool bigEndianGraphics = true;
  i32 i;
  //i32 saveD6=D6, saveD7=D7;
  D6L = 0;
  openGraphicsFile();  // Open the file
  D0L = READ(d.GraphicHandle,2,(ui8 *)&d.NumGraphic);
  if (D0L == 2)
  {
    if (d.NumGraphic == 0x8001)
    {
      bigEndianGraphics = false;
      D0L = READ(d.GraphicHandle,2,(ui8 *)&d.NumGraphic);
    };
    if (bigEndianGraphics) 
    {
      d.NumGraphic = LE16(d.NumGraphic);
    };
    D6L = d.NumGraphic * 2; // Number of bytes
    success = D6L!= 0;
  };
  if (success)
  {
    if (d.GraphicCompressedSizes != NULL)
    {
      UI_free(d.GraphicCompressedSizes);
    };
    d.GraphicCompressedSizes = (ui16 *)UI_malloc(D6L&0xffff,MALLOC070);
    success = d.GraphicCompressedSizes!=NULL;
  };
  if (success)
  {
    if (d.GraphicDecompressedSizes != NULL)
    {
      UI_free(d.GraphicDecompressedSizes);
    };
    d.GraphicDecompressedSizes = (ui16 *)UI_malloc(D6L & 0xffff, MALLOC069);
    success = d.GraphicDecompressedSizes!=NULL;
  };
  if (success)
  {
    D0L=READ(d.GraphicHandle,D6L&0xffff,(ui8 *)d.GraphicCompressedSizes);
    success = D0L == (D6L&0xffff);
  };
  if (success)
  {
    D0L = READ(d.GraphicHandle,D6L&0xffff,(ui8 *)d.GraphicDecompressedSizes);
    success = D0L == (D6L&0xffff);
  };
  if (success)
  {
    if (bigEndianGraphics)
    {
      for (i=0; i<d.NumGraphic; i++)
      {
        d.GraphicCompressedSizes[i]=LE16(d.GraphicCompressedSizes[i]);
        d.GraphicDecompressedSizes[i]=LE16(d.GraphicDecompressedSizes[i]);
      };
    };
  };
  if (success)
  {
    d.ppUnExpandedGraphics = (pnt *)UI_malloc((D6L*2)&0xffff,MALLOC071);
    success = d.ppUnExpandedGraphics!=NULL;
  };
  if (success)
  {

    d.GraphicIndex0 = (i16 *)UI_malloc(D6L&0xffff, MALLOC072);
    success = d.GraphicIndex0!=NULL;
  };
  if (success)
  {
    ClearMemory((ui8 *)d.ppUnExpandedGraphics,(2*D6L)&0xffff);
    fillMemory((i16 *)d.GraphicIndex0,D6L >>= 1, -1, 2);
    D0L = LocateNthGraphic(--D6L); // Locate last graphic in file;
    D3L = d.GraphicCompressedSizes[D6W] & 0xffff; // Size of last graphic
    d.GraphicFileEOF = D0L + D3L; // Total size of GRAPHICS.DAT
    //d.Pointer11782 = (pnt)UI_malloc((ui16)d.GraphicDecompressedSizes[0], MALLOC074);
    d.compressedGraphic0 = (ui8 *)UI_malloc((ui16)d.GraphicDecompressedSizes[0], MALLOC074);
    if (d.compressedGraphic0 != NULL)
    {
      ReadGraphic(0, d.compressedGraphic0);
    }
    else 
    {
      success=false;
    };
  };
  if (!success) die(42);
  closeGraphicsFile();
  //D6=saveD6;D7=saveD7;
}

// *********************************************************
// Delete Graphic from Graphic List and release memory
// *********************************************************
//   TAG021f5e
void DeleteGraphic(i32 graphicNum)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//  dReg D0, D6, D7;
//  aReg A2 /*,A3*/;
    ITEMQ *pgThis;
//    ITEMQ *pgPrev, *pgNext;
//  i32 giPrev;
  i32 giThis;
//  i32 giNext;
//#ifdef _VERIFYPPQ
//  verifyppq();
//#endif
  if ((graphicNum &0x8000) == 0)
  {
    ASSERT(graphicNum < d.NumGraphic,"graphicNum");
    giThis = d.GraphicIndex0[graphicNum];
    if (giThis == -1)
    {
//#ifdef _VERIFYPPQ
//      verifyppq();
//#endif
      return;
    };
    d.GraphicIndex0[graphicNum] = -1;
  }
  else
  {
    graphicNum &= 0x7fff;
    giThis = d.GetDerivedGraphicCacheIndex(graphicNum);
    if (giThis == -1)
    {
//#ifdef _VERIFYPPQ
//      verifyppq();
//#endif
      d.SetDerivedGraphicSize(graphicNum, 0);
      return;
    };
    d.SetDerivedGraphicCacheIndex(graphicNum, -1);
  };
  pgThis = d.pGraphicCachePointers[giThis];
  d.pGraphicCachePointers[giThis] = NULL;
  if (pgThis != NULL) UI_free(pgThis);
  /*
  D7L = pgThis->size;
  D7L = -D7L;
  d.iAvailableGraphicMemory += D7L;
  giPrev = pgThis->prev;
  giNext = pgThis->next;
  //if (giNext < 0)
  if (giPrev == -1)
  {
    if (giNext == -1)
    {
      d.pqFirstGraphic = NULL;
      d.pqLastGraphic = NULL;
      d.pqFirstGraphic2 = NULL;
    }
    else
    {
      d.pqFirstGraphic = d.ppExpandedGraphics[giNext];
      d.pqFirstGraphic->prev = -1;

      if (pgThis == d.pqFirstGraphic2)
                d.pqFirstGraphic2 = d.pqFirstGraphic;
    };
  }
  else
  {
    pgPrev = d.ppExpandedGraphics[giPrev];//Pointer to previous
    pgPrev->next = (i16)giNext;//Set previous->next
    if (giNext == -1)
    {
      d.pqLastGraphic = pgPrev;
      if (pgThis == d.pqFirstGraphic2) d.pqFirstGraphic2 = NULL;
//
    }
    else
    {
      pgNext = d.ppExpandedGraphics[giNext];//Pointer to next
      pgNext->prev = (i16)giPrev;//Set next->previous
      //
      if (pgThis == d.pqFirstGraphic2) d.pqFirstGraphic2 = pgNext;
    };
  };
  if ((pnt)pgThis != d.pStartMemory)
  {
    D6L = D0L = longGear((pnt)pgThis - 4);
    if (D6L > 0)
    {
      pgThis = (ITEMQ *)((pnt)pgThis-D6L);
      RemoveFromFreeList(pgThis);
      D7L += D6L;
    };
  };
  if ((pnt)pgThis + D7L == d.Pointer23318)
  { // This is the last allocated graphic
    d.Pointer23318 = (pnt)pgThis;
  }
  else
  {
    A2 = (pnt)pgThis + D7L;
    D6L = longGear(A2);
    if (D6L > 0)
    {
      RemoveFromFreeList((ITEMQ *)A2);
      D7L += D6L;
    };
    pgThis->size = D7L;
    longGear((pnt)pgThis + D7L -4) = D7L;
    FreeGraphicMemory(pgThis);
  };
//#ifdef _VERIFYPPQ
//  verifyppq();
//#endif
*/
}

void CleanupGraphics()
{
  int i;
  if (d.GraphicHandle >= 0)
  {
    CLOSE(d.GraphicHandle);
    d.GraphicHandle = -1;
    d.Word23244 = 0;
  };
  if (d.pGraphicCachePointers != NULL)
  {
    for (i=0; i<NumExpandedGraphics; i++)
    {
      if (d.pGraphicCachePointers[i] != NULL)
      {
        UI_free(d.pGraphicCachePointers[i]);
        d.pGraphicCachePointers[i] = NULL;
      };
    };
    UI_free(d.pGraphicCachePointers);
    d.pGraphicCachePointers = NULL;
  };
  if (d.ppUnExpandedGraphics != NULL)
  {
    /* These are allocated on the permanent stack/heap
    for (i=0; i<d.NumGraphic; i++)
    {
      if (d.ppUnExpandedGraphics[i] != NULL)
      {
        UI_free(d.ppUnExpandedGraphics[i]);
        d.ppUnExpandedGraphics[i] = NULL;
      };
    };
    */
    UI_free(d.ppUnExpandedGraphics);
    d.ppUnExpandedGraphics = NULL;
  };
  if (d.compressedGraphic0 != NULL)
  {
    UI_free(d.compressedGraphic0);
    d.compressedGraphic0 = NULL;
  };
  if (d.GraphicIndex0 != NULL)
  {
    UI_free(d.GraphicIndex0);
    d.GraphicIndex0 = NULL;
  };
  if (d.GraphicDecompressedSizes != NULL)
  {
    UI_free(d.GraphicDecompressedSizes);
    d.GraphicDecompressedSizes = NULL;
  };
  if (d.GraphicCompressedSizes != NULL)
  {
    UI_free(d.GraphicCompressedSizes);
    d.GraphicCompressedSizes = NULL;
  };
  if (tempBitmap != NULL)
  {
    UI_free(tempBitmap);
    tempBitmap = NULL;
    tempBitmapSize = 0;
  };
  currentOverlay.Cleanup();
  currentSound.Cleanup();
  backgroundLib.Cleanup();
  skinCache.Cleanup();
  CleanupAltMonCache();
  scrollingText.Cleanup();
  CleanupWallDecorations();
//  CleanupFloorDecorations();
  CSBgraphicsFile.Cleanup();
}

// *********************************************************
//
// *********************************************************
//     TAG0221f6
ITEMQ *GetExpandedGraphicBuffer(i32 size)
{ // size must include 4 bytes for length of allocated space.
  // Return value points at longword at front containing size.
  pnt result;
  result = (pnt)UI_malloc(size, MALLOC029);
  longGear((ui8 *)result) = size; // Put -size in front of newly allocated space
  longGear((ui8 *)result+size-4) = size; // Put -size at end of block.. WHY???
  return (ITEMQ *)result;
/*
  dReg D0, D5, D6, D7;
  aReg A0, A2;
  ITEMQ *pqA2, *pqA3;
  VerifyGraphicFreeList();
  pqA3 = NULL;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7L = size;
  TAG0220fa(D7L);
  if (d.pStartAvailMemory - d.Pointer23318 >= D7L)
  {
    pqA3 = (ITEMQ *)d.Pointer23318;
    d.Pointer23318 += D7L;
  }
  else
  {
    D0L = d.pgUnused->size;
    if (D0L < D7L)
    {
      CompactMemory();
      pqA3 = (ITEMQ *)d.Pointer23318;
      d.Pointer23318 += D7L;
    }
    else
    {
      pqA3 = d.pgUnused; // first unused graphics buffer
      for (D6W = 0; D6W == 0; )
      {
        D0L = pqA3->size;
        if (D0L == D7L)
        {
          D6W = 1;
        }
        else
        {
          D0L = pqA3->size;
//
          if (   (D0L < D7L)
              || ((pqA2=pqA3->pgNext()) == NULL)  )
          {

            pqA3 = d.pgUnused;
            D6W = 1;
          }
          else
          {
            pqA3 = pqA2;
          };
        };
      };
      RemoveFromFreeList(pqA3);
      D5L = pqA3->size - D7L;
      if (D5L >= 24)
      {
        A2 = (pnt)pqA3 + D7L;
        A0 = A2 + D5L - 4;
        longGear(A0) = D5L;
        longGear(A2) = D5L;
        FreeGraphicMemory((ITEMQ *)A2);
      }
      else
      {
        D7L = pqA3->size;
      };
    };
  };
  d.iAvailableGraphicMemory -= D7L;
  D0L = -D7L;
  longGear((pnt)pqA3) = D0L; // Put -size in front of newly allocated space
  D0L = -D7L;
  longGear((pnt)pqA3+D7L-4) = D0L; // Put -size at end of block.. WHY???
  VerifyGraphicFreeList();
  return pqA3;
*/
}

/*
// *********************************************************
//
// *********************************************************
#ifdef _DEBUG
void verifyppq()
{
  ITEMQ *current, *next;
  bool foundSecondPart=false;
  if (d.pqFirstGraphic == NULL)
  {
    ASSERT(d.pqLastGraphic == NULL);
    ASSERT(d.pqFirstGraphic2==NULL);
  }
  else
  {
    current = d.pqFirstGraphic;
    ASSERT(current->prev == -1);
    ASSERT(longGear((pnt)current) == longGear((pnt)current-longGear((pnt)current)-4));
    if (current == d.pqFirstGraphic2) foundSecondPart=true;
    while (current->next != -1)
    {
      ASSERT(longGear((pnt)current) == longGear((pnt)current-longGear((pnt)current)-4));
      next = d.ppExpandedGraphics[current->next];
      ASSERT(d.ppExpandedGraphics[next->prev] == current);
      current = next;
      if (current == d.pqFirstGraphic2) foundSecondPart=true;
    };
    ASSERT(d.pqLastGraphic == current);
    ASSERT((d.pqFirstGraphic2==NULL)||foundSecondPart);
  };
};
#endif
*/

/*
// *********************************************************
//
// *********************************************************
//   TAG022720
void ClearGraphicList2()
{
  dReg D7;
  ITEMQ *pqA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  d.Long23358 = d.Time;
  d.pqFirstGraphic2 = NULL;
  pqA3 = d.pqLastGraphic;
  if (pqA3 == NULL) return;
  while (pqA3->word4 != 0)
  {
    pqA3->word4 = 0;
    D7W = pqA3->prev;
    if (D7W == -1) return;
    pqA3 = d.ppExpandedGraphics[D7W];

  };
}
*/

/*
// *********************************************************
//
// *********************************************************
void TAG022778(i16 P1)
{
  dReg D0, D4, D5, D6, D7;
  ITEMQ *pqA2, *pqA3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//#ifdef _VERIFYPPQ
//      verifyppq();
//#endif
  D7W = P1;
  pqA3 = d.ppExpandedGraphics[D7W];
  pqA3->word4 = 1;
  if (d.pqFirstGraphic2 == NULL)
  {
    pqA3->next = -1;
    if (d.pqLastGraphic == NULL)
    {
      pqA3->prev = -1;
      d.pqFirstGraphic = pqA3;
    }
    else
    {
      d.pqLastGraphic->next = D7W;
      D4W = d.pqLastGraphic->graphicNum;
      if (D4W >= 0)
      {
        D6W = d.GraphicIndex0[D4W];
      }
      else
      {
        D6W = d.GraphicIndex8000[D4W & 0x7fff];
      };
      pqA3->prev = D6W;
    };
    d.pqLastGraphic = pqA3;
  }
  else
  {
    D6W = d.pqFirstGraphic2->prev;
    d.pqFirstGraphic2->prev = D7W;
    pqA3->prev = D6W;
    if (D6W != -1)
    {
      pqA2 = d.ppExpandedGraphics[D6W];
      D0W = pqA2->next;
      pqA3->next = D0W;
      pqA2->next = D7W;
    }
    else
    {
      D4W = d.pqFirstGraphic2->graphicNum;
      if (D4W >= 0)
      {
        D5W = d.GraphicIndex0[D4W];
      }
      else
      {
        D5W = d.GraphicIndex8000[D4W & 0x7fff];
      };
    pqA3->next = D5W;
    d.pqFirstGraphic = pqA3;
    };
  };
  d.pqFirstGraphic2 = pqA3;
}
*/

/*
// *********************************************************
//
// *********************************************************
ITEMQ *TAG02289a(i16 P1)
{
  dReg D4, D5, D6, D7;
  ITEMQ *pqA2,*pqA3;
  ITEMQ *LOCAL_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//#ifdef _VERIFYPPQ
//      verifyppq();
//#endif
  D7W = P1;
  pqA3 = d.ppExpandedGraphics[D7W];
  LOCAL_4 = pqA3;
  D4W = pqA3->word4;
  if (D4W == 0)
  {
    D5W = pqA3->next;
    if (D5W == -1)
    {
      d.pqFirstGraphic2 = pqA3;
      pqA3->word4 = 1;
      return pqA3;
    };
    pqA2 = d.ppExpandedGraphics[D5W];
    D6W = pqA3->prev;
    if (D6W == -1)
    {
      d.pqFirstGraphic = pqA2;
      pqA2->prev = -1;
    }
    else
    {
      pqA3 = d.ppExpandedGraphics[D6W];
      pqA3->next = D5W;
      pqA2->prev = D6W;
    };
    //TAG022778(D7W);
    return LOCAL_4;
  };
  D4W++;
  pqA3->word4 = D4W;
  if (pqA3 == d.pqLastGraphic) return pqA3;
  D5W = pqA3->next;
  pqA2 = d.ppExpandedGraphics[D5W];
  if (pqA2->word4 < D4W) return pqA3;
  D6W = pqA3->prev;
  if (D6W == -1)
  {
    d.pqFirstGraphic2 = pqA2;
    d.pqFirstGraphic = pqA2;
    pqA2->prev = -1;
  }
  else
  {
    if (d.pqFirstGraphic2 == pqA3)
    {
      d.pqFirstGraphic2 = pqA2;
    };
    pqA3 = d.ppExpandedGraphics[D6W];
    pqA3->next = D5W;
    pqA2->prev = D6W;
  };
  for (;;)
  {
    pqA3 = pqA2;
    D6W = D5W;
    D5W = pqA3->next;
    if (D5W == -1)
    {
      pqA3->next = D7W;
      LOCAL_4->prev = D6W;
      LOCAL_4->next = -1;
      d.pqLastGraphic = LOCAL_4;
      return LOCAL_4;
    };
    pqA2 = d.ppExpandedGraphics[D5W];
    if (pqA2->word4 < D4W)
    {
      pqA3->next = D7W;
      LOCAL_4->prev = D6W;
      LOCAL_4->next = D5W;
      pqA2->prev = D7W;
      return LOCAL_4;
    };
  };
}
*/

// *********************************************************
//
// *********************************************************
//  TAG022a92
ui8 *GetBasicGraphicAddress(i32 graphicNum, i32 minimumWidth, i32 minimumHeight)
{
  //Flag 0x8000 = get UnExpanded Graphic address
  dReg D0, D5, D6, D7;
  aReg A2;
  ITEMQ *pqA3;
  i32 height;
  ui8 *result;
  i16 LOCAL_10;
//#ifdef _VERIFYPPQ
//      verifyppq();
//#endif
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = (i16)graphicNum;
  if (D7W & 0x8000)
  {
    D0W = (i16)(D7W & 0x7fff);
    //D0 <<= 2;
    result = (ui8 *)d.ppUnExpandedGraphics[D0W];
    //result=A2;
  }
  else
  {
//    if (d.Time != d.Long23358)
//    {
//      ClearGraphicList2();
//    };
    D6W = d.GraphicIndex0[D7W]; // Get index of ppq pointer
    if (D6W != -1)
    {
      result = (ui8 *)d.pGraphicCachePointers[D6W] + 12;//result = (pnt)TAG02289a(D6W) + 12;
    }
    else
    {
      A2 = d.ppUnExpandedGraphics[D7W];
      D5W = (i16)(LE16(wordGear(A2))); //width in pixels
      height = (ui16)LE16(wordGear(A2+2));
      if ( (D5W < minimumWidth) || (height < minimumHeight) )
      {
        // This error occurs in the standard Graphic.dat!!!!
        // If the buffer is allocated at the very end of a memory block then
        // we get a memory reference error.  So, we increase the size of the buffer
        // we get so that there is garbage at the end of the buffer but
        // at least it won't cause a crash.
        height = minimumHeight;
        //char msg[80];
        //sprintf(msg, "graphic number %d is too small", graphicNum);
        //UI_MessageBox(msg, "Fatal Error", MESSAGE_OK);
        //die(7610);
      };
      D5L = (D5W + 15) & 0xfff0; // Round up width in pixels.
      D5UW >>= 1; // Two pixels per byte
      // D5W = width in bytes
      LOCAL_10 = (i16)(D5W * height);
      // LOCAL_10 = width * height
      D5L = LOCAL_10 + 16; 
      //Add ITEMQ header length + room for trailing size
      pqA3 = GetExpandedGraphicBuffer(D5L);
      D6W = 0;
      while (d.pGraphicCachePointers[D6W++] != NULL) {};
      D6W--;
      if (D6W >= NumExpandedGraphics)
      {
        die(0xc6d7,"No space for basic graphic");
      };
      d.GraphicIndex0[D7W] = D6W;
      ASSERT(d.pGraphicCachePointers[D6W] == NULL,"ppexpandedGraphic");
      d.pGraphicCachePointers[D6W] = pqA3;
      pqA3->graphicNum = D7W;
      //TAG022778(D6W);
      result = (ui8 *)pqA3 + 12;
      ExpandGraphic(A2, result, 0, 0);
    };
  };
//#ifdef _VERIFYPPQ
//      verifyppq();
//#endif
  return result;
}


// *********************************************************
//
// *********************************************************
void ReadAndExpandGraphic(i32 graphicNum, ui8 *dest, i16 P3, i16 P4, i32 maxSize) // TAG022b86
{
  dReg D0, D6, D7;
  aReg A0, A2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //i32 saveD6=D6, saveD7=D7;
  //pnt saveA2=A2;
  D7L = graphicNum;
  openGraphicsFile(); //TAG021d36
  D6W = (i16)(D7W & 0x8000);
  D7W &= 0x7fff;
  //D0W = wordGear(d.Pointer23254 + (D7W<<1));
  D0L = d.GraphicCompressedSizes[D7W];
  //  if (D0W == wordGear(d.Pointer23258 + (D7W<<1)))
  if (D0L != d.GraphicDecompressedSizes[D7W])
  {
    A2 = (aReg)allocateMemory(5004, 0);
    LSEEK(LocateNthGraphic(D7W),d.GraphicHandle,SEEK_SET);

    D0L = LZWExpand(d.GraphicHandle,
                    d.GraphicCompressedSizes[D7W]&0xffff,
                    dest,
                    (ui8 *)d.pViewportBMP,
                    (ui8 *)A2); // TAG022f64
    ASSERT (D0L == d.GraphicDecompressedSizes[D7W],"decompSize");
    d.Word11684 = 1;
    FreeTempMemory(5004);
    if (D6W == 0)
    {
      MemMove(dest, (ui8 *)d.pViewportBMP, d.GraphicDecompressedSizes[D7W]);
    };
  }
  else
  {
    if (D6W != 0)
    {
      A0 = (pnt)dest;
    }
    else
    {
      A0 = (pnt)d.pViewportBMP;
    };
//    if (d.GraphicDecompressedSizes[D7W] > maxSize) return;
    ReadGraphic(D7W, (ui8 *)A0, maxSize);
  };
  if (D6W == 0)
  {
#ifdef GraphicsDebug
    if (GrphDbg != NULL)
    {
      i32 i;
      for (i=0; i<0x105a; i++)
      {
        if ((i&7) == 0)
          fprintf(GrphDbg,"\n%04x",i);
        fprintf(GrphDbg," %02x",(unsigned char)d.pViewportBMP[i]);
      };
    };
#endif
    ExpandGraphic((i8 *)d.pViewportBMP, dest, P3, P4, maxSize);
#ifdef GraphicsDebug
    if (GrphDbg != NULL)
    {
      i32 i;
      for (i=0; i<0x7d00; i++)
      {
        if ((i&7) == 0)
          fprintf(GrphDbg,"\n%04x",i);
        fprintf(GrphDbg," %02x",(unsigned char)dest[i]);
      };
    };
#endif
    d.Word11684 = 1;
  };
  closeGraphicsFile();
  //D6=saveD6;D7=saveD7;A2=saveA2;
}


// *********************************************************
//
// *********************************************************
i32 LZWExpand(i16 fileHandle,
              i32 graphicSize,
              ui8 *dest,
              ui8 *scratch,
              ui8 *stack) // TAG022f64
{
  // Returns size of result.
//               8     10             14     18    22
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D0, D4, D6, D7;
  pnt stackPointer;
  pnt LOCAL_12;
  //i32 saveD4=D4,saveD6=D6,saveD7=D7;
  i16 *WordArray;
  i8  *ByteArray;
  i16 prevCodeWord;
  i32 i;
  WordArray = (i16 *)scratch;
  ByteArray = (i8 *)scratch+10006;
  stackPointer = (pnt)stack; // empty right now
  LOCAL_12 = (pnt)dest;
  d.LZWrepeatFlag = 0;
  d.LZWCodeSize = 9;
  d.Word23382 = 0;
  d.LZWMaxCode = (i16)((1<<d.LZWCodeSize)-1);
  memset(WordArray, 0, 512);
  for (i=0; i<256; i++)
  {
    ByteArray[i] = (i8)i;
  };

  d.LZWNextCode = 257;
  D0W = LZWGetNextCodeword(fileHandle, &graphicSize);
  prevCodeWord = D0W;
  D7W = D0W;
  if (prevCodeWord == -1)
  {
    D0L = -1;
  }
  else
  {
    TAG022eec(D7B, (i8 **)&dest); // Updates dest
    while ((D6W=LZWGetNextCodeword(fileHandle, &graphicSize)) > -1)
    {
      if(D6W==256)
      {
        ClearMemory((ui8 *)WordArray, 512);
        d.Word23382 = 1;
        d.LZWNextCode = 256;
        D0W = LZWGetNextCodeword(fileHandle, &graphicSize);
        D6W = D0W;
        if(D0W == -1) break;
      };
      D4W = D6W;
      if (D6W >= d.LZWNextCode)
      {
        *(stackPointer++) = D7B;
        D6W = prevCodeWord;
      };
      while (D6W >= 256)
      {
        *(stackPointer++) = ByteArray[D6W];
        D6W = WordArray[D6W];

      };
      D7W = 0;
      D7B = ByteArray[D6W];
      *(stackPointer++) = D7B;
      do
      {
        TAG022eec(*(--stackPointer)&0xff,(i8 **)&dest);
      } while(stackPointer > (pnt)stack);

        D6W = d.LZWNextCode;
      if (D6W < d.Word23366)
      {
        WordArray[D6W] = prevCodeWord;
        ByteArray[D6W] = D7B;
        d.LZWNextCode = (i16)(D6W + 1);
      };
      prevCodeWord = D4W;
// while expression

    }; //while

    D0L = (aReg)dest - LOCAL_12; // Total bytes in result
  };
  //D4=saveD4;D6=saveD6;D7=saveD7;
  return D0L;
}

// *********************************************************
//
// *********************************************************
i16 LZWGetNextCodeword(i16 fileHandle,i32 *pGraphicSize) // TAG022d90
{
  dReg D0, D1, D3, D5, D6, D7;
  aReg A3=NULL;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //i32 saveD5=D5, saveD6=D6, saveD7=D7;
  //pnt saveA3=A3;
  A3 = (pnt)d.IOBuffer;

  if (   (d.Word23382 > 0)
       | (d.LZWBitNumber >= d.Word23406)
       | (d.LZWNextCode > d.LZWMaxCode) )

  {
    if (d.LZWNextCode > d.LZWMaxCode)
    {
      d.LZWCodeSize++;
      if (d.LZWCodeSize == 12)
      {
        d.LZWMaxCode = d.Word23366;
      }
      else
      {
        d.LZWMaxCode = (i16)((1 << d.LZWCodeSize) - 1);
      };
    };
    if (d.Word23382 > 0)
    {
      d.LZWCodeSize = 9;
      d.LZWMaxCode = (i16)((1 << d.LZWCodeSize) - 1);
      d.Word23382 = 0;
    };
    D0L = *pGraphicSize;
    D1L = d.LZWCodeSize;
    if (D0L > D1L)
    {
      d.Word23406 = d.LZWCodeSize;
    }
    else
    {
      d.Word23406 = (i16)(*pGraphicSize);
    };
    if (d.Word23406 > 0)
    {
      D0L = READ(fileHandle,d.Word23406,d.IOBuffer);
      *pGraphicSize -= d.Word23406;
      D0L = *pGraphicSize;
    }
    else
    {
      D0L = -1;
      return D0W;
    };
    d.LZWBitNumber = 0;
    D0W = (i16)((d.Word23406<<3) - d.LZWCodeSize + 1);
    d.Word23406 = D0W;
  };
  D6W = d.LZWBitNumber;
  D5W = d.LZWCodeSize;
  A3 += (D6UW>>3);//UWed
  D6W &= 7;
  D7W = 0;
  D7B = *(A3++);
  D7UW >>= D6W;//UWed
//  D0W = 8 - D6W;
  D6W = (i16)(8 - D6W);
  D5W = (i16)(D5W - D6W);
  if (D5W >= 8)
  {
    D0W = 0;
    D0B = *(A3++);
    D0W <<= D6W;
    D7W |= D0W;
    D6W += 8;
    D5W -=8;
  };
    D0B = *A3;
    D0W &= 0x00ff;
//    A0 = &d.RightOneMask + D5W;
    D3W = 0;
    D3B = d.RightOneMask[D5W];
    D0W &= D3W;
    D0W <<= D6W;
    D7W |= D0W;
    d.LZWBitNumber = (i16)(d.LZWBitNumber + d.LZWCodeSize);
    D0W = D7W;
  //D5=saveD5;D6=saveD6;D7=saveD7;A3=saveA3;
  return D0W;
}


// *********************************************************
//
// *********************************************************
void TAG022eec(i32 code,pnt *pDest)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dReg D0, D7;
  aReg A3;
  //i32 saveD7=D7;
  //pnt saveA3=A3;
  D7W = (i16)code;
  A3 = *pDest;
  if(d.LZWrepeatFlag == 0)
  {
    D0W=0;
    D0B=D7B;
    if (D0W == 0x90)
    {
      d.LZWrepeatFlag = 1;
    }
    else
    {
      D0W = 0;
      D0B = D7B;
      d.LZWrepeatChar = D0W; // In case repeat sequence follows
      *(A3++) = D0B;
    };
    *pDest = A3;
  }
  else
  {
    if (d.LZWrepeatFlag ==1) // Then this character is repeat count
    {
      if (D7UB != 0)
      {
        while ((--D7UB)!=0)
        {
          D0W = d.LZWrepeatChar;
          *(A3++) = D0B;
        };
      }
      else
      {
        *(A3++) = (ui8)0x90;
      };
      d.LZWrepeatFlag = 0;
      *pDest = A3; // Update destination address
      return;
    };
    NotImplemented(0x22f5a);
  };
  //D7=saveD7;A3=saveA3;
  return;
}



OVERLAYDATA::OVERLAYDATA()
{
  m_overlay = NULL;
  m_overlayPalette = NULL;
  m_table = NULL;
  m_overlayNumber = -1;
};

OVERLAYDATA::~OVERLAYDATA()
{
  Cleanup();
}

void OVERLAYDATA::Allocate()
{
  if (m_overlay == NULL) 
        m_overlay = (ui8 *)UI_malloc(136*224,MALLOC077);
  if (m_table == NULL) 
        m_table = (i16 *)UI_malloc(8192,MALLOC078);
  if (m_overlayPalette == NULL) 
        m_overlayPalette = (ui32 *)UI_malloc(256*4,MALLOC079);
}


void OVERLAYDATA::Cleanup()
{
  if (m_overlay != NULL)
  {
    UI_free(m_overlay);
    m_overlay = NULL;
  };
  if (m_overlayPalette != NULL)
  {
    UI_free(m_overlayPalette);
    m_overlayPalette = NULL;
  };
  if (m_table != NULL)
  {
    UI_free(m_table);
    m_table = NULL;
  };
  m_overlayNumber = -1;
}

void OVERLAYDATA::CreateOverlayTable(i16 *atariPalette, bool useOverlay)
{
  ui32 red[16],green[16],blue[16];
  ui32 red100[16],green100[16],blue100[16];
  i32 i, I, overlayPaletteEntry, RED, GREEN, BLUE;
  i32 transparency, opaqueness;
  i16 *pTable;
  transparency = useOverlay?m_p2:100;
  opaqueness = 100 - transparency;
  if (useOverlay)
  {
    if (overlayActive)
    {
      for (i=0; i<16; i++)
      { //Unpack the rgb values and multiply by 100-transparency.
        overlayPaletteEntry = 
            ((atariPalette[i] & 0x700) >> 2)
          | ((atariPalette[i] & 0x070) >> 1)
          | ((atariPalette[i] & 0x007) >> 0);  // One of 512 entries
        red[i]      = overlayPaletteRed[overlayPaletteEntry]   * transparency;
        green[i]    = overlayPaletteGreen[overlayPaletteEntry] * transparency;
        blue[i]     = overlayPaletteBlue[overlayPaletteEntry]  * transparency;
        red100[i]   = overlayPaletteRed[overlayPaletteEntry]   * 100;
        green100[i] = overlayPaletteGreen[overlayPaletteEntry] * 100;
        blue100[i]  = overlayPaletteBlue[overlayPaletteEntry]  * 100;
      }
      pTable = m_table;
      for (I=0; I<256; I++)
      {
        if (((m_p1 & 0x0001) != 0) && (I==m_p4))
        {
          // This is a transparent color.
          // Ignore the overlay color because transparency = 100.
          //RED   = ((m_overlayPalette[I] >> 16) & 0xff) * opaqueness;
          //GREEN = ((m_overlayPalette[I] >>  8) & 0xff) * opaqueness;
          //BLUE  = ((m_overlayPalette[I] >>  0) & 0xff) * opaqueness;
          for (i=0; i<16; i++, pTable++)
          {
#ifdef RGB555
            *pTable = (i16)(  ((red100[i])/800) << 10
                            | ((green100[i])/800) << 5
                            | ((blue100[i])/800) << 0);
#endif
#ifdef RGB655
            *pTable = (i16)(  ((red100[i])/800) << 11 //10
                            | ((green100[i])/800) <<  6//5
                            | ((blue100[i])/800) <<  0);
#endif
          };
        }
        else
        {
          RED   = ((m_overlayPalette[I] >> 16) & 0xff) * opaqueness;
          GREEN = ((m_overlayPalette[I] >>  8) & 0xff) * opaqueness;
          BLUE  = ((m_overlayPalette[I] >>  0) & 0xff) * opaqueness;
          for (i=0; i<16; i++, pTable++)
          {
#ifdef RGB555
            *pTable = (i16)(  ((red[i] + RED)/800) << 10
                            | ((green[i] + GREEN)/800) <<  5
                            | ((blue[i] + BLUE)/800) << 0);
#endif
#ifdef RGB655
            *pTable = (i16)(  ((red[i] + RED)/800) << 11 //10
                            | ((green[i] + GREEN)/800) <<  6//5
                            | ((blue[i] + BLUE)/800) <<  0);
#endif
          };
        };
      };
    }
    else
    {  // The overlay byte will always be zero  So we need 
       // only 16 entries (one for each of the possible 16 Atari colors).
      for (i=0; i<16; i++)
      { 
        overlayPaletteEntry = 
            ((atariPalette[i] & 0x700) >> 2)
          | ((atariPalette[i] & 0x070) >> 1)
          | ((atariPalette[i] & 0x007) >> 0);  // One of 512 entries
        RED = overlayPaletteRed[overlayPaletteEntry];
        GREEN = overlayPaletteGreen[overlayPaletteEntry];
        BLUE = overlayPaletteBlue[overlayPaletteEntry];
#ifdef RGB555
        m_table[i] = (i16)(((RED & 0xf8)<<7) | ((GREEN&0xf8)<<2) | ((BLUE&0xf8)>>3));
#endif
#ifdef RGB655
        m_table[i] = (i16)(((RED & 0xf8)<<8) | ((GREEN&0xf8)<<3) | ((BLUE&0xf8)>>3));
#endif
      };
    };
  }
  else
  {
    // use straight atari palette
    for (i=0; i<16; i++)
    {
      RED   = (atariPalette[i] >> 8) & 0x7;
      GREEN = (atariPalette[i] >> 4) & 0x7;
      BLUE  = (atariPalette[i] >> 0) & 0x7;
#ifdef RGB555
      m_table[i] = (i16)((RED<<12) | (GREEN<<7) | (BLUE<<2));
      m_table[i] |= (m_table[i] & 0x6318) >> 3;
#else
#ifdef RGB655
      m_table[i] = (i16)((RED<<13) | (GREEN<<8) | (BLUE<<2));
      m_table[i] |= (m_table[i] & 0xc318) >> 3;
#else
	  You must define some sort of RGB format!!
#endif
#endif
    };
  };
}

OVLDECODE::OVLDECODE(ui32 GetCodeword())
{
  m_getCodeword = GetCodeword;
  m_codes = (ui32 *)UI_malloc(4*512,MALLOC075);
  m_chars = (ui8 *)UI_malloc(512,MALLOC076);
  m_maxTableLen = 512;
  m_tableLen = 255;
  m_codeSize = 9;
  m_bitsRemaining = 0;
  m_codeword = 0;
  m_stklen = 0;
  m_stack = (ui8 *)UI_malloc(1024, MALLOC078);
  m_maxstack = 1024;
  m_a = 0;
}

OVLDECODE::~OVLDECODE()
{
  if (m_codes != NULL) UI_free(m_codes);
  if (m_chars != NULL) UI_free(m_chars);
  if (m_stack != NULL) UI_free(m_stack);
}

void OVLDECODE::EnlargeStack()
{
  NotImplemented(0xf864);
}

void OVLDECODE::EnlargeTable()
{
  m_maxTableLen *= 2;
  m_codes = (ui32 *)UI_realloc(m_codes, 4*m_maxTableLen, MALLOC080);
  m_chars = (ui8 *) UI_realloc(m_chars, m_maxTableLen, MALLOC081);
  m_codeSize++;
}


bool OVLDECODE::GetBytes(void *buf, ui32 num)
{
  ui8 *c;
  ui32 numbits;
  c = (ui8 *)buf;
  while (num > 0)
  {
    if (m_stklen > 0)
    {
      *(c++) = m_stack[--m_stklen];
      num--;
      continue;
    };
    numbits = m_codeSize;
    if (numbits > m_bitsRemaining) numbits = m_bitsRemaining;
    m_b = m_codeword & ((1<<numbits)-1);
    m_bitsRemaining -= numbits;
    m_codeword >>= numbits;
    numbits = m_codeSize - numbits;
    if (numbits > 0)
    {
      m_codeword = m_getCodeword();
      m_bitsRemaining = 32;
      m_b <<= numbits;
      m_b |= m_codeword  & ((1<<numbits)-1);
      m_bitsRemaining -= numbits;
      m_codeword >>= numbits;
    };
    if ((m_b >= m_tableLen) && (m_b > 0xff))
    {
      if (m_b == m_maxTableLen-1)
      {
        EnlargeTable();
        continue;
      };
      m_stack[m_stklen++] = (ui8)m_c;
      if (m_stklen >= m_maxstack) EnlargeStack();
      for (m_ch=m_a; m_ch>255; m_ch=m_codes[m_ch])
      {
        m_stack[m_stklen++] = m_chars[m_ch];
        if (m_stklen >= m_maxstack) EnlargeStack();
      };
      m_stack[m_stklen++] = (ui8)m_ch;
      if (m_stklen >= m_maxstack) EnlargeStack();
    }
    else
    {
      for (m_ch=m_b; m_ch>255; m_ch=m_codes[m_ch])
      {
        m_stack[m_stklen++] = m_chars[m_ch];
        if (m_stklen >= m_maxstack) EnlargeStack();
      };
      m_stack[m_stklen++] = (ui8)m_ch;
      if (m_stklen >= m_maxstack) EnlargeStack();
    };
    m_c = m_ch;
    m_codes[m_tableLen] = (ui32)m_a;
    m_chars[m_tableLen] = (ui8)m_c;
    m_tableLen++;
    m_a = m_b;
  };
  return false;
}


ui32 *nextCode;
ui32 GetCodeword()
{
  return *(nextCode++);
}

ui8 *ReadCSBgraphic(CSB_GRAPHICTYPE type,
                    ui32 id,
                    ui32 minimumSize,
                    ui32 *pActualSize,
                    bool mustExist,
                    i32 mallocID)
{
  TEMPORARY_MEMORY sectionHeader;
  TEMPORARY_MEMORY compressedData;
  SECTIONHEADER *pSectionHeader;
  OVLDECODE ovlDecode(GetCodeword);
  ui8 *result;
  ui32 i;
  static bool firstTime = true;
  ui8 sentinel[4];
  ui32 version;
  ui32 numSection;
  if (pActualSize != NULL) *pActualSize = 0;
  OpenCSBgraphicsFile();
  if (CSBgraphicsFile.f < 0)
  {
    if (firstTime)
    {
      UI_MessageBox("Can't open CSBgraphics.dat","error",MESSAGE_OK);
      overlayActive = false;
      firstTime = false;
    };
    return NULL;
  };
  READ(CSBgraphicsFile.f, 4, sentinel);
  if (   (sentinel[0] != 'C')
       ||(sentinel[1] != 'S')
       ||(sentinel[2] != 'B')
       ||(sentinel[3] != 'g'))
  {
    if (firstTime)
    {
      UI_MessageBox("CSBgraphics.dat file starts badly","error",MESSAGE_OK);
      firstTime = NULL;
    };
    return NULL;
  };
  READ(CSBgraphicsFile.f, 4, (ui8 *)&version);
  READ(CSBgraphicsFile.f, 4, (ui8 *)&numSection);
  //LSEEK(12 + onum*sizeof(SECTIONHEADER), ovlfile, SEEK_SET);
  sectionHeader.m = (ui8 *)UI_malloc(numSection * sizeof(SECTIONHEADER),MALLOC083);
  pSectionHeader = (SECTIONHEADER *)sectionHeader.m;
  READ(CSBgraphicsFile.f, numSection*sizeof(SECTIONHEADER), (ui8 *)sectionHeader.m);
  for (i=0; i<numSection; i++)
  {
    if (pSectionHeader[i].id != id) continue;
    if (pSectionHeader[i].type == (ui32)type) break;
  };
  if (i == numSection)
  {
    if (!mustExist) return NULL;
    if (firstTime)
    {
      char message[100];
      sprintf(message,"Missing section in CSBgraphics.dat.\nSection id=%d, Type=%s", id, 
                                CSBGraphicsSectionNames[type]);
      UI_MessageBox(message,"error",MESSAGE_OK);
      firstTime = false;
    };
    return NULL;
  };
  if (pSectionHeader[i].uncompressedSize < minimumSize)
  {
    return NULL;
  };
  LSEEK(pSectionHeader[i].fileOffset, CSBgraphicsFile.f, SEEK_SET);
  compressedData.m = (ui8 *)UI_malloc(pSectionHeader[i].compressedSize,MALLOC082);
  READ(CSBgraphicsFile.f, pSectionHeader[i].compressedSize, compressedData.m); 
  nextCode = (ui32 *)compressedData.m;
  result = (ui8 *)UI_malloc(pSectionHeader[i].uncompressedSize,mallocID);
  ovlDecode.GetBytes(result, pSectionHeader[i].uncompressedSize);
  if (pActualSize != NULL) *pActualSize = pSectionHeader[i].uncompressedSize;
  return result;
}


