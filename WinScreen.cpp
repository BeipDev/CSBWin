
#include "stdafx.h"

//#ifdef _MSVC_INTEL // Windows screen handling


#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"
#ifdef _MSCV_INTEL
#include <io.h>
#include <FCNTL.h>
#include <SYS\STAT.h>
#endif

extern CntPtrTo<ID2D1Factory1> g_pID2D1Factory1;
extern CntPtrTo<ID2D1HwndRenderTarget> g_pID2DRenderTarget;
extern CntPtrTo<ID2D1Bitmap> g_pID2DBitmap;

//#define USE_DIBS

HBITMAP g_bmpOffscreen;

#ifdef _LINUX
#pragma pack(1)

#define DWORD unsigned int
#define LONG  int
#define WORD  unsigned short int
#define BYTE  unsigned char

#define BI_RGB 0
#define DIB_RGB_COLORS 0

struct BITMAPINFOHEADER
{
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD  biClrImportant;
};

struct RGBQUAD
{
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
};

struct BITMAPINFO
{
  BITMAPINFOHEADER bmiHeader;
  RGBQUAD          bmiColors[1];
};

#pragma pack()
#endif

extern i32 WindowX;
extern i32 WindowY;
extern i32 VBLMultiplier;
extern i16 globalPalette[16];
extern bool fullscreenRequested;
bool virtualFullscreen = false;
extern bool overlayActive;
extern i32 xGraphicJitter;
extern i32 xOverlayJitter;
extern i32 yGraphicJitter;
extern bool jitterChanged;
extern ui32 dumpWindow;
extern HWND hWnd;
//extern CDC *OnDrawDC;
ui32 STBLTCount=0;

i32 screenSize=2;
i16 palette16[16];
i16 g_bitmap[320*200];
i16 counter;
i16 bitSkip;
i32 dstLineLen;
BITMAPINFO bitmapInfo;
i16 palette1[16];
i16 palette2[16];
i16 oldPalette1[16];
i16 oldPalette2[16];
ui8 black[320]; //All zeros for one line of overlay
//ui8 nibbles[64000];
ui8 fourBitPixels[64000];
bool screenAlreadyUnpacked;


pnt logbase();
bool screenInconsistent = true;

RECT g_rcClient{0, 0, 1, 1}; // The rect that the DM screen is mapped to in the window


bool IsTextScrollArea(i32, i32)
{
  return false;
}

void SwapTextZOrder()
{
  return;
}


bool HasPaletteChanged(i16 *palette, i16 *oldpalette)
{
  bool change=false;
  for (i32 i=0; i<16; i++)
  {
    if (oldpalette[i] != palette[i])
    {
      oldpalette[i]=palette[i];
      change=true;
    };
  };
  return change;
}

ui32 prePixelMasks[16] = {
  0xffffffff,
  0x00010001,
  0x00030003,
  0x00070007,
  0x000f000f,
  0x001f001f,
  0x003f003f,
  0x007f007f,
  0x00ff00ff,
  0x01ff01ff,
  0x03ff03ff,
  0x07ff07ff,
  0x0fff0fff,
  0x1fff1fff,
  0x3fff3fff,
  0x7fff7fff,
};

ui32 postPixelMasks[16] = {
  0xffffffff,
  0x80008000,
  0xc000c000,
  0xe000e000,
  0xf000f000,
  0xf800f800,
  0xfc00fc00,
  0xfe00fe00,
  0xff00ff00,
  0xff60ff80,
  0xffc0ffc0,
  0xffe0ffe0,
  0xfff0fff0,
  0xfff8fff8,
  0xfffcfffc,
  0xfffefffe
};

bool HasAreaChanged(ui8 *STScreen,
                    int x0, int y0, int width, int height,
                    ui8 *prevSTScreen)
{
  // Each line will consist of
  //    ***  0-15 pixels (prePixelCount)
  //    ***  n quadwords of 16 pixels each (quadCount)
  //    ***  0-15 pixels (postPicelCount)
  // We will do the comparison by processing the pre-pixels,
  // then the quadwords, then the post-pixels.
  int prePixelCount, postPixelCount, lastPixel;
  int fullQuadCount, line;
  ui8 *pFirstQuadA, *pFirstQuadB;
  ui32 *pQuadA, *pQuadB;
  ui32 prePixelMask, postPixelMask;
  pFirstQuadA = STScreen + 160*y0 + 8*(x0/16);
  pFirstQuadB = prevSTScreen + 160*y0 + 8*(x0/16);
  lastPixel = x0 + width;
  postPixelCount = lastPixel % 16;
  postPixelMask = postPixelMasks[postPixelCount];
  prePixelCount = (16 - x0) & 15;
  fullQuadCount = (width - prePixelCount - postPixelCount) / 16;
  if (prePixelCount != 0)
  {
    prePixelMask = prePixelMasks[prePixelCount];
    if (prePixelCount >= width)
    {
      prePixelCount = width;
      prePixelMask &= postPixelMask;
      postPixelMask = 0;
    };
    pQuadA = (ui32 *)pFirstQuadA;
    pQuadB = (ui32 *)pFirstQuadB;
    for (line=0; line < height; line++)
    {
      if (((pQuadA[0] ^ pQuadB[0]) & prePixelMask) != 0)  return true;
      if (((pQuadA[1] ^ pQuadB[1]) & prePixelMask) != 0)  return true;
      pQuadA += 40;
      pQuadB += 40;
    };
  };
  pQuadA = (ui32 *)pFirstQuadA;
  pQuadB = (ui32 *)pFirstQuadB;
  if (prePixelCount != 0)
  {
    pQuadA += 2;
    pQuadB += 2;
  };
  for (line=0; line<height; line++)
  {
    ui32 *pQA, *pQB;
    int q;
    pQA = pQuadA;
    pQB = pQuadB;
    for (q=0; q<fullQuadCount; q++)
    {
      if ((pQA[0] != pQB[0]) || (pQA[1] != pQB[1])) 
      {
        return true;
      };
      pQA += 2;
      pQB += 2;
    };
    pQuadA += 40;
    pQuadB += 40;
  };
  if (postPixelCount != 0)
  {
    pQuadA = (ui32 *)pFirstQuadA + 2*fullQuadCount;
    pQuadB = (ui32 *)pFirstQuadB + 2*fullQuadCount;
    if (prePixelCount != 0)
    {
      pQuadA += 2;
      pQuadB += 2;
    };
    for (line=0; line<height; line++)
    {
      if (((pQuadA[0] ^ pQuadB[0]) & postPixelMask) != 0) 
      {
        return true;
      };
      if (((pQuadA[1] ^ pQuadB[1]) & postPixelMask) != 0)  
      {
        return true;
      };
      pQuadA += 40;
      pQuadB += 40;
    };
  };
  return false;
}
/*
void createPalette32(i16 *palette)
{ // Create 32-bit color palette from the ST 9-bit palette
  for (i32 i=0; i<16; i++)
  {
    ASSERT(palette[i]<=0x777);
    palette32[i]  = (((palette[i]>>8) & 7)*0xff/0x7)<<16;
    palette32[i] |= (((palette[i]>>4) & 7)*0xff/0x7)<< 8;
    palette32[i] |= (((palette[i]>>0) & 7)*0xff/0x7)<< 0;
  };
}
*/

void createPalette16(i16 *palette)
{ // Create 16-bit color palette from the ST 9-bit palette
  for (i32 i=0; i<16; i++)
  {
    ASSERT(palette[i]<=0x777,"palette");
    palette16[i]=0;
    palette16[i] |= (((palette[i]>>8) & 7)*0x1f/0x7)<<10;//red
    palette16[i] |= (((palette[i]>>4) & 7)*0x1f/0x7)<< 5;//green
    palette16[i] |= (((palette[i]>>0) & 7)*0x1f/0x7)<< 0;//blue
  };
}


bool ForcedScreenDraw = false;

void ForceScreenDraw()
{
  ForcedScreenDraw = true;
}

ui32 unpack[256];

void BuildUnpackTable()
{
  static bool finished = false;
  ui32 m, i;
  if (finished) return;
  finished = true;
  for (i=0; i<256; i++)
  {
    m = 0;
    if (i & 0x80) m |= 0x00000001;
    if (i & 0x40) m |= 0x00000100;
    if (i & 0x20) m |= 0x00010000;
    if (i & 0x10) m |= 0x01000000;
    if (i & 0x08) m |= 0x00000010;
    if (i & 0x04) m |= 0x00001000;
    if (i & 0x02) m |= 0x00100000;
    if (i & 0x01) m |= 0x10000000;
    unpack[i] = m;
  };
}


// Unpack 64000 (320x200) pixels of ST graphic.
// Convert from bit-plane to single byte-per-pixel format
// Takes about a millisecond on Raspberry Pi.
void UnpackScreen(ui8 *src, ui8 *dst)
{
  ui32 *piDest = (ui32 *)dst;
  ui32 r;
  int i;
  if (screenAlreadyUnpacked) return;
  screenAlreadyUnpacked = true;
  BuildUnpackTable();
  for (i=0; i<64000/16; i++,src+=8,piDest+=4)
  {
    r  =  (unpack[src[0]] << 0)
        | (unpack[src[2]] << 1)
        | (unpack[src[4]] << 2)
        | (unpack[src[6]] << 3);
    piDest[0] = r & 0x0f0f0f0f;
    piDest[1] = (r >> 4) & 0x0f0f0f0f;


    r =  (unpack[src[1]] << 0)
       | (unpack[src[3]] << 1)
       | (unpack[src[5]] << 2)
       | (unpack[src[7]] << 3);
    piDest[2] = r & 0x0f0f0f0f;
    piDest[3] = (r >> 4) & 0x0f0f0f0f;

  };
};



// Don't warning about modifing EBP
#if defined(_MSC_VER)
#pragma warning( disable : 4731)
#endif
void BLT1(ui8  *src,     // Raw 8-bit pixels
          ui16 *dst,     // Destination if 16-bit result pixels
          i32  num,      // Width
          ui16 *palette, // Final resulting colors of graphic plus overlay
          ui8  *overlay) // Overlay pixels
{
  ui16 color;
  for (;num>0; num--)
  {
    color = palette[(*(overlay++) << 4) + *(src++)];
    *(dst++) = color;
  };
}

int updateScreenAreaEnterCount = 0;
int updateScreenAreaLeaveCount = 0;

// Returns 1 if screen area changed.
int UpdateScreenArea(HDC hdc,
                      ui8  *STScreen,
                      i32  x0,
                      i32  y0,
                      i32  width,
                      i32  height,
                      i32  dstX,
                      i32  dstY,
                      i16 *palette,
                      bool paletteChanged,
                      ui8 *pOldScreen,
                      i32  size,
                      bool useOverlay)
{
#ifndef USE_DIBS
  i16 *pBitmap=g_bitmap+x0+y0*320;
#else
   i16 *pBitmap=g_bitmap;
#endif

  bool overlayChanged = false;
  i32 firstNibble[7];
  i32 firstOverlay[7];
  i32 segWidth[7];
  //i32 LineEnd   = (((x0&0xff0)+width+15)/16) * 8;
  //i32 LineStart = (x0/16) * 8;
  //ui8 *pNibbles = nibbles;
  ui8 *pPixels;
  i32 line, segment, xgj, xoj, ygj, currentGraphicLine, lastGraphicLine;
  //i32 numPixel, skipPixel, n;
  //i16 *pPalette;
  //char *pFirstGroup;
  updateScreenAreaEnterCount++;
  overlayChanged = useOverlay && currentOverlay.m_change;
  currentOverlay.m_change = false;
  if (!paletteChanged && !overlayChanged && !jitterChanged) 
  {
//    if (!HasScreenChanged(STScreen+160*y0+LineStart,
//                          (LineEnd-LineStart) / 4,
//                          height,
//                          160-(LineEnd-LineStart),
//                          pOldChecksum)) 
    if (!HasAreaChanged(STScreen,
                        x0, y0, width, height,
                        pOldScreen)) 
    {
      updateScreenAreaLeaveCount++;
      return 0;
    };
  };
  UnpackScreen(physbase(), fourBitPixels);
  //createPalette16(palette);
  //if (pOverlayData != NULL)
  //{
  //  pOverlayData->CreateOverlayTable(palette16);
  //  pPalette = pOverlayData->m_table;
  //}
  //else
  //{
  //  pPalette = palette16;
  //};
  jitterChanged = false;
  currentOverlay.Allocate();
  currentOverlay.CreateOverlayTable(palette, useOverlay);
  //pPalette = pOverlayData->m_table;

  STBLTCount++;
  if (useOverlay)
  {
    xgj = xGraphicJitter;
    xoj = xOverlayJitter;
    ygj = yGraphicJitter;
  }
  else
  {
    xgj = 0;
    xoj = 0;
    ygj = 0;
  };
  if (ygj >= 0)
  {
    currentGraphicLine = -ygj;
    lastGraphicLine = height - ygj - 1;
  }
  else
  {
    currentGraphicLine = -ygj;
    lastGraphicLine = height;
  };

/* There are 13 cases
 *    xGraphicJitter  xOverlayJitter
 *         -10            -15    GO-G-B
 *         -10            -10    GO-B
 *         -10             -5    GO-O-B
 *         -10              0    GO-O
 *         -10             10    G-GO-O
 *           0            -10    GO-G
 *           0              0    GO
 *           0             10    G-GO
 *          10            -10    O-GO-G
 *          10              5    B-O-GO
 *          10              0    O-GO
 *          10             10    B-GO
 *          10             15    B-G-GO
 *
 *    Altogether we have seven segments in each line
 *        n0 B
 *        n1 G
 *        n2 O
 *        n3 GO
 *        n4 G
 *        n5 O
 *        n6 B
 *
 *
 * Now we will determine all the parameters for displaying a
 * line for each of 13 possible cases.
 */
  if (xgj < 0)
  {
    if (xoj < 0)
    {
      NotImplemented(0x777107);
    }
    else if (xoj == 0)
    {
      segWidth[0] = 0; 
      segWidth[1] = 0;
      segWidth[2] = 0;
      firstNibble[3] = -xgj;
      firstOverlay[3] = 0;
      segWidth[3] = width + xgj;
      segWidth[4] = 0;
      segWidth[5] = 0;
      segWidth[6] = -xgj;
      firstNibble[6] = -1;
      firstOverlay[6] = width + xgj;
    }
    else
    {
      NotImplemented(0x777109);
    };
  }
  else if (xgj == 0)
  {
    if (xoj < 0)
    {
      NotImplemented(0x777101);
    }
    else if (xoj == 0)
    {
      segWidth[0] = 0;
      segWidth[1] = 0;
      segWidth[2] = xgj;
      firstNibble[2] = -1;
      firstOverlay[2] = 0;
      firstNibble[3] = 0;
      firstOverlay[3] = firstOverlay[2]+xgj;
      segWidth[3] = width - xgj;
      segWidth[4] = 0;
      segWidth[5] = 0;
      segWidth[6] = 0;
    }
    else
    {
      NotImplemented(0x777104);
    };
  }
  else
  {
    if (xoj == 0)
    {
      segWidth[0] = 0;
      segWidth[1] = 0;
      segWidth[2] = xgj;
      firstOverlay[2] = 0;
      firstNibble[2] = -1;
      firstNibble[3] = 0;
      firstOverlay[3] = xgj;
      segWidth[3] = width - xgj;
      segWidth[4] = 0;
      segWidth[5] = 0;
      segWidth[6] = 0;
    }
    else if (xoj < 0)
    {
      NotImplemented(0x777111);
    }
    else
    {
      NotImplemented(0x777112);
    };
  };



  for (line=0; line<height; line++, currentGraphicLine++)
  {
    i32 currentPixel;
    if ((currentGraphicLine < 0) || (currentGraphicLine > lastGraphicLine))
    {
      {
        pPixels = black;
      }
//      memset (nibbles,0,width);
    }
    else
    {
      // Setup parameters for testing newest 
      {
        pPixels = fourBitPixels+320*(y0+currentGraphicLine)+x0;
      }
      //pFirstGroup = (char *)STScreen + 160*(y0+currentGraphicLine) + LineStart;
      //numPixel = width;
      //skipPixel = x0 & 15;
      //pNibbles = nibbles;
      //if (skipPixel != 0)
      //{
      //  n = 16 - skipPixel;
      //  if (n > numPixel) n = numPixel;
      //  Unpack(pFirstGroup, pNibbles, skipPixel, n);
      //  pNibbles += n;
      //  numPixel -= n;
      //  pFirstGroup += 8;
      //};
      //while (numPixel > 0)
      //{
      //  n = 16;
      //  if (n > numPixel) n = numPixel;
      //  Unpack(pFirstGroup, pNibbles, 0, n);
      //  pNibbles += n;
      //  numPixel -= n;
      //  pFirstGroup += 8;
      //};
    };

    currentPixel = 0;

    for (segment=0; segment<7; segment++)
    {
      unsigned char *pNibbles;
      ui8 *pOverlay;
      if (segWidth[segment] == 0) continue;
      //pNibbles = (firstNibble[segment] < 0) ? black : nibbles + firstNibble[segment]; 
      pNibbles = (firstNibble[segment] < 0) ? black : pPixels+firstNibble[segment];
      pOverlay =   (useOverlay && overlayActive)
                 ? currentOverlay.m_overlay+224*(135-line) + firstOverlay[segment]
                 : black;

        BLT1(pNibbles,                               // Raw 8-bit pixel data
            (ui16 *)pBitmap + 1*(320*line + currentPixel), 
            segWidth[segment],          
            (ui16 *)currentOverlay.m_table, 
            pOverlay);

      currentPixel += segWidth[segment];
    };
  };
#ifdef BLUR
  if (size == 4)
  {
    int line, x;
    for (line=0; line<height-1; line++)
    {
      for (x=0; x<width-1; x++)
      {
        BLS4(bitmap + 4*320*4*line + 4*x);
      };
    };
  };
#endif

#ifdef USE_DIBS
  bitmapInfo.bmiHeader.biSize=0x28;
  bitmapInfo.bmiHeader.biWidth=320;
  bitmapInfo.bmiHeader.biHeight=-height;
  bitmapInfo.bmiHeader.biPlanes=1;
  bitmapInfo.bmiHeader.biBitCount=16;
  bitmapInfo.bmiHeader.biCompression=BI_RGB;
  bitmapInfo.bmiHeader.biSizeImage=0;
  bitmapInfo.bmiHeader.biXPelsPerMeter=0;
  bitmapInfo.bmiHeader.biYPelsPerMeter=0;
  bitmapInfo.bmiHeader.biClrUsed=0;
  bitmapInfo.bmiHeader.biClrImportant=0;
  SetDIBitsToDevice(hdc, dstX, dstY, width, height, 0, 0, 0, height, (char *)g_bitmap, &bitmapInfo, DIB_RGB_COLORS);
#endif

  updateScreenAreaLeaveCount++;
  return 1;
};


void MakeBMPPalette(RGBQUAD *colors, PALETTE *stpalette)
{
  // Create RGB color palette from the ST 9-bit palette
  ui32 i;
  for (i=0; i<16; i++)
  {
    ASSERT(stpalette->color[i]<=0x777,"palette");
    colors[i].rgbRed   = (ui8)((((stpalette->color[i]>>8) & 7)*0xff)/0x7);//red
    colors[i].rgbGreen = (ui8)((((stpalette->color[i]>>4) & 7)*0xff)/0x7);//green
    colors[i].rgbBlue  = (ui8)((((stpalette->color[i]>>0) & 7)*0xff)/0x7);//blue
  };
}


void MakeBMPBitmap(ui16 *src, ui8 *dst)
{
  ui32 k[16];
  i32 i, j, line;
  ui16 s[4];
  src += 200 * 80;
  for (line=0; line<200; line++)
  {
    src -= 80;
    for (i=0; i<20; i++)
    {
      //Convert 16 pixels at a time 
      s[0] = LE16(src[0]);
      s[1] = LE16(src[1]);
      s[2] = LE16(src[2]);
      s[3] = LE16(src[3]);
      for (j=0; j<16; j++)
      {
        k[j] = (s[0] >> 15) & 1;
        k[j] |= (s[1] >> 14) & 2;
        k[j] |= (s[2] >> 13) & 4;
        k[j] |= (s[3] >> 12) & 8;
        s[0] <<= 1;
        s[1] <<= 1;
        s[2] <<= 1;
        s[3] <<= 1;
      };
      for (j=0; j<8; j++)
      {
        dst[j] = (ui8)((k[j*2]<<4) + k[j*2+1]);
      };
      src += 4;
      dst += 8;
    };
    src -= 80;
  };
}

#ifdef _MSVC_INTEL
void DumpWindow(FILE *f)
{
  BITMAPFILEHEADER bmfh;
  BITMAPINFOHEADER bmih;
  RGBQUAD colors[16];
  ui8    bitmapA[320*200/2];
  memset(&bmfh,0,sizeof(bmfh));
  memset(&bmih,0,sizeof(bmih));
  bmih.biSize = sizeof (bmih);
  bmih.biWidth = 320;
  bmih.biHeight = 200;
  bmih.biPlanes = 1;
  bmih.biBitCount = 4;
  bmih.biCompression = BI_RGB;
  bmih.biSizeImage = 0;
  bmih.biClrUsed = 16;
  bmih.biClrImportant = 16;
  *((ui8 *)&bmfh+0) = 'B';
  *((ui8 *)&bmfh+1) = 'M';
  bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(bmih) + 320*200/2;
  bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + bmih.biSize + sizeof(colors);
  fwrite(&bmfh,1,sizeof(BITMAPFILEHEADER),f);
  fwrite(&bmih,1,sizeof(BITMAPINFOHEADER),f);
  MakeBMPPalette(colors, d.DynamicPaletteSwitching?&d.Palette11946:(PALETTE *)&globalPalette);
  fwrite(colors,1, sizeof(colors),f);
  MakeBMPBitmap((ui16 *)physbase(), bitmapA);
  fwrite(bitmapA,1,sizeof(bitmapA),f);
}
#endif

/*
void DumpImages()
{
  static ui64 prevTime = 0;
  static int count = 0;
  ui64 curTime;
  curTime = UI_GetSystemTime();
  if (curTime > prevTime)
  {
    //for (;;)
    {
      int FH;
      char name[100];
      sprintf(name,"ScreenDumps.bin");
      if (prevTime == 0) 
      {
        prevTime = curTime;
        FH = _open(name, _O_WRONLY|_O_CREAT|_O_TRUNC|O_SEQUENTIAL|_O_BINARY,_S_IWRITE);
      }
      else
      {
        FH = _open(name, _O_WRONLY|_O_APPEND|O_SEQUENTIAL|_O_BINARY);
        //append stops working at 2**32 bytes!!!!
        _lseeki64(FH,32118I64 * count,SEEK_SET);
      };
      prevTime += 167;
      DumpWindow(FH);
      count++;
      _close(FH);
    };
  }
}

*/

ui8 prevScreen[32000];

bool pc1, pc2;
void display (){
  static i32 numDisplay = 0;
  static bool initialized = false;
  int areaChangedCount;
  screenAlreadyUnpacked = false;
  if (!initialized)
  {
    memset(black,0,320);
    initialized = true;
  };
#ifdef _MSCV_INTEL
  if (dumpWindow == 1)
  {
    dumpWindow = 0;
    int FH;
    FH = _open("window.bmp", _O_WRONLY|_O_CREAT|O_SEQUENTIAL|_O_BINARY,_S_IWRITE);
    if (FH != -1)
    {
      DumpWindow(FH);
      _close(FH);
    };
  };
#endif
  numDisplay++;
  if ((VBLMultiplier!=1) && ((d.Time&0xf)!=0) && (VBLMultiplier!=99)) return;
  if (d.DynamicPaletteSwitching)
  {
    memcpy(palette1,&d.Palette11978, 32);
    memcpy(palette2,&d.Palette11946, 32);
    memcpy(globalPalette,&d.Palette11978,32);
  }
  else
  {
    memcpy(palette1,globalPalette, 32);
    memcpy(palette2,globalPalette, 32);
  };
  pc1 = HasPaletteChanged(palette1, oldPalette1);
  pc2 = HasPaletteChanged(palette2, oldPalette2);
  pc1 = pc1 || ForcedScreenDraw;
  pc2 = pc2 || ForcedScreenDraw;
  ForcedScreenDraw = false;
  {
    areaChangedCount = 0;
    if (!virtualFullscreen)
    {
#ifdef USE_DIBS
      if(!g_bmpOffscreen)
         g_bmpOffscreen=CreateCompatibleBitmap(GetDC(hWnd), 320,200);

      WindowDC dc(&Window(hWnd));
      BitmapDC dcBitmap(g_bmpOffscreen, dc);
      HDC hdc=dcBitmap.hDC();
#else
      HDC hdc{};
#endif

      i32 size = 1;
      areaChangedCount +=
      UpdateScreenArea(hdc,         //Viewport
                       physbase(),//STScreen,
                       0,
                       0x21,
                       0xe0,
                       0xa9-0x21,
                       0*size,
                       0x21*size,
                       palette2,
                       pc2,
                       prevScreen,
                       size,
                       videoMode==VM_ADVENTURE);
      areaChangedCount +=
      UpdateScreenArea(hdc,   //Text scrolling area
                       physbase(),//STScreen,
                       0,
                       0xa9,
                       320,
                       0xc8-0xa9,
                       0*size,
                       0xa9*size,
                       palette1,
                       pc1,
                       prevScreen,
                       size,
                       false);
      areaChangedCount +=
      UpdateScreenArea(hdc,             //portrait area
                       physbase(),//STScreen,
                       0,
                       0,
                       320,
                       0x21,
                       0*size,
                       0*size,
                       palette1,
                       pc1,
                       prevScreen,
                       size,
                       false);
      areaChangedCount +=
      UpdateScreenArea(hdc,              //spells,weapons,moves
                       physbase(),//STScreen,
                       0xe0,
                       0x21,
                       0x140-0xe0,
                       0xa9-0x21,
                       0xe0*size,
                       0x21*size,
                       palette2,
                       pc2,
                       prevScreen,
                       size,
                       false);

#ifdef USE_DIBS
      dc.StretchBlt(g_rcClient, dcBitmap, g_rcAtari, SRCCOPY);
#else
      struct RGB16
      {
         WORD b : 5;
         WORD g : 5;
         WORD r : 5;
         WORD unused : 1;
      };

      DWORD pixels[320*200];
      const RGB16 *pSrc=reinterpret_cast<const RGB16*>(g_bitmap);
      for(unsigned i=0;i<320*200;i++)
      {
         auto pixel=pSrc[i];
         pixels[i]=(pixel.r<<(3+16)) | (pixel.g<<(3+8)) | (pixel.b<<(3));
      }

      g_pID2DRenderTarget->BeginDraw();
      g_pID2DBitmap->CopyFromMemory(&D2D1_RECT_U{0, 0, uint32_t(g_rcAtari.right), uint32_t(g_rcAtari.bottom)}, pixels, sizeof(DWORD)*g_rcAtari.right);

      g_pID2DRenderTarget->DrawBitmap(g_pID2DBitmap, D2D1_RECT_F{float(g_rcClient.left), float(g_rcClient.top), float(g_rcClient.right), float(g_rcClient.bottom)}, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
      g_pID2DRenderTarget->EndDraw();
#endif

    }
    else
    {
#if 0
      if (videoSegSize[0] != 0)
      {
        areaChangedCount +=
        UpdateScreenArea(         //Viewport
                         physbase(),//STScreen,
                         videoSegSrcX[0],
                         videoSegSrcY[0],
                         videoSegWidth[0],
                         videoSegHeight[0],
                         videoSegX[0],
                         videoSegY[0],
                         palette2,
                         pc2,
                         prevScreen,
                         videoSegSize[0],
                         true);
      };
      if (videoSegSize[1] != 0)
      {
        areaChangedCount +=
        UpdateScreenArea(             //portrait area
                         physbase(),//STScreen,
                         videoSegSrcX[1],
                         videoSegSrcY[1],
                         videoSegWidth[1],
                         videoSegHeight[1],
                         videoSegX[1],
                         videoSegY[1],
                         palette1,
                         pc1,
                         prevScreen,
                         videoSegSize[1],
                         false);
      };
      if (videoSegSize[2] != 0)
      {
        areaChangedCount +=
        UpdateScreenArea(              //spells,weapons,moves
                         physbase(),//STScreen,
                         videoSegSrcX[2],
                         videoSegSrcY[2],
                         videoSegWidth[2],
                         videoSegHeight[2],
                         videoSegX[2],
                         videoSegY[2],
                         palette2,
                         pc2,
                         prevScreen,
                         videoSegSize[2],
                         false);
      };
      if (videoSegSize[3] != 0)
      {
        areaChangedCount +=
        UpdateScreenArea(   //Text scrolling area
                         physbase(),//STScreen,
                         videoSegSrcX[3],
                         videoSegSrcY[3],
                         videoSegWidth[3],
                         videoSegHeight[3],
                         videoSegX[3],
                         videoSegY[3],
                         palette1,
                         pc1,
                         prevScreen,
                         videoSegSize[3],
                         false);
      };
#endif
    };
    if (areaChangedCount)
    {
      memcpy(prevScreen, physbase(), 32000);
    };
  };
#ifdef _MOVIE
  UI_SetDIBitsToDevice(-1,0,0,0,0,0,0,0,0,0,0);
#endif
}


//#endif
