#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

void info(char *, unsigned int);
void ClearOverlayPalette(void);
bool IsPlayFileOpen(void);

extern bool overlayActive;
extern ui8 overlayPaletteRed[512];
extern ui8 overlayPaletteGreen[512];
extern ui8 overlayPaletteBlue[512];

class BITMAPENTRY
{
public:
  pnt bitmap;
  i32 width;
  i32 height;
  BITMAPENTRY(void){bitmap = NULL;};
  ~BITMAPENTRY(void){if(bitmap!=NULL)UI_free(bitmap);bitmap=NULL;};
};

#if defined _bigEndian
void SwapFileHeader( CSB_BITMAPFILEHEADER* a)
{
    a->bfType = BE16(a->bfType);
    a->bfSize = BE32(a->bfSize);
    a->bfReserved1 = BE16(a->bfReserved1);
    a->bfReserved2 = BE16(a->bfReserved2);
    a->bfOffBits = BE32(a->bfOffBits);
}


void SwapInfoHeader( CSB_BITMAPINFOHEADER* a)
{
    a->biSize = BE32(a->biSize);
    a->biWidth = BE32(a->biWidth);
    a->biHeight = BE32(a->biHeight);
    a->biPlanes = BE16(a->biPlanes);
    a->biBitCount = BE16(a->biBitCount);
    a->biCompression = BE32(a->biCompression);
    a->biSizeImage = BE32(a->biSizeImage);
    a->biXPelsPerMeter = BE32(a->biXPelsPerMeter);
    a->biYPelsPerMeter = BE32(a->biYPelsPerMeter);
    a->biClrUsed = BE32(a->biClrUsed);
    a->biClrImportant = BE32(a->biClrImportant);
}
#endif //_bigendian

void bmpError(char *fname, const char *msg, bool Die)
{
  //if (!PlayFile.IsOpen())
  if (!IsPlayFileOpen())
  {
    char *m;
    m = (char *)UI_malloc(strlen(fname) + strlen(msg) + 100,
                          MALLOC003);
    sprintf(m,"Bitmap file error - filename = %s\n%s",
              fname, msg);
    UI_MessageBox(m, "Error", MESSAGE_OK);
    UI_free (m);
  };
  if (Die) die(0x33dc);
}

class SAFEFILE
{
public:
  i16 f;
  pnt buf;
  SAFEFILE(void){f=-1; buf=NULL;};
  ~SAFEFILE(void)
  {
    if(f>=0)CLOSE(f);f=-1;
    if (buf != NULL) UI_free(buf); buf = NULL;
  };
};

/*
bool ImportBitmap(char *filename1,
                  char *filename2, 
                  BITMAPENTRY *pbme, 
                  bool printIfError,
                  bool dieIfError) 
{
  ui32 width, height;
  i32 size;	
  char *filename;
  i32 destWordWidth, destHeight;
  CSB_BITMAPFILEHEADER bmFileHeader;
  CSB_BITMAPINFOHEADER bmInfoHeader;
  ui16 word0=0, word1=0, word2=0, word3=0;
  ui32 nibble;
  ui32 bmiColors[16];
  ui16 *dstLine, *dstWord;
  ui8  *srcLine, *srcByte;
  i32 srcByteWidth;
  ui32 srcX;
	// TODO: Add your command handler code here
	SAFEFILE f;
  i32 i, j, k;
  //char bitmap[(portraitPixelWidth+8)*portraitPixelHeight];
  filename = filename1;
  f.f = OPEN(filename,"rb");
  if (f.f < 0)
  {
    filename = filename2;
    f.f = OPEN(filename,"rb");
    if (f.f < 0)
    {
      if (!printIfError) return false;
      bmpError(filename," Cannot open file", dieIfError);
      return false;
    };
  };
  if (READ(f.f,sizeof(bmFileHeader),(char *)&bmFileHeader) != sizeof(bmFileHeader))
  {
    if (!printIfError) return false;
    bmpError(filename, "Cannot read bitmap file header",dieIfError);
    return false;
  };
#if defined _bigEndian
  SwapFileHeader(&bmFileHeader);
#endif
  if (bmFileHeader.bfType != ('M'<<8) + 'B')
  {
    if (!printIfError) return false;
    bmpError(filename, "That does not look like a bitmap file",dieIfError);
    return false;
  };
  if (READ(f.f,sizeof(bmInfoHeader),(char *)&bmInfoHeader) != sizeof(bmInfoHeader))
  {
    if (!printIfError) return false;
    bmpError(filename, "Cannot read the InfoHeader",dieIfError);
    return false;
  };
#if defined _bigEndian
  SwapInfoHeader(&bmInfoHeader);
#endif
//  if (   (bmInfoHeader.biWidth != portraitPixelWidth+8)
//       ||(bmInfoHeader.biHeight != 4 * portraitPixelHeight) )
//  {
//    CString msg;
//    msg.Format("The bitmap must be exactly\n%d by %d",
//                portraitPixelWidth+8,
//                4 * portraitPixelHeight);
//    Strange(msg);
//    return;
//  };
  if (bmInfoHeader.biBitCount != 4)
  {
    if (!printIfError) return false;
    bmpError(filename, "That is not a 16-color bitmap",dieIfError);
    return false;
  };
  if (READ(f.f,sizeof(bmiColors),(char *)bmiColors) != sizeof(bmiColors))
  {
    if (!printIfError) return false;
    bmpError(filename, "Cannot read palette",dieIfError);
    return false;
  };
  width = bmInfoHeader.biWidth;
  height = bmInfoHeader.biHeight;
  size = (width+1)/2 * height;
  f.buf = (pnt)UI_malloc(width*height/2, MALLOC004);
  if (f.buf == NULL)
  {
    if (!printIfError) return false;
    bmpError(filename,"Cannot allocate memory for bitmap",dieIfError);
    return false;
  };
  if (READ(f.f, size, f.buf) != size)
  {
    if (!printIfError) return false;
    bmpError(filename,"Cannot read bitmap pixel data",dieIfError);
    return false;
  };
  destWordWidth = (width+15)/16 * 4; //4 words per 16 pixels
  srcByteWidth = (width+1)/2;//4 bits per pixel
  destHeight = height;
  pbme->bitmap = (pnt)UI_malloc(destWordWidth*2*destHeight,
                                MALLOC005);
  pbme->width = width;
  pbme->height = height;
  if (pbme->bitmap == NULL)
  {
    if (!printIfError) return false;
    bmpError(filename,"Cannot allocate memory for bitmap",dieIfError);
    return false;
  };
  srcLine = (ui8 *)f.buf + srcByteWidth * (destHeight-1);
  dstLine = (ui16 *)pbme->bitmap;
  for (i=0; 
       i<destHeight; 
       i++, srcLine-=srcByteWidth, dstLine+=destWordWidth)
  {
    srcByte = srcLine;
    dstWord = dstLine;
    word0 = word1 = word2 = word3 = 0;
    srcX = 0;
    for (j=0; j<destWordWidth/4; j++)
    {
      for (k=0; k<16; k++, srcX++)
      {
        if (srcX >= width) nibble = 0;
        else
        {
          if ((k & 1) == 0)
          {
            nibble = ((*srcByte) >> 4) & 15;
          }
          else
          {
            nibble = (*srcByte) & 15;
            srcByte++;
          };
        };
        word0 <<= 1;
        word1 <<= 1;
        word2 <<= 1;
        word3 <<= 1;
        word0 |= (nibble >> 3) & 1;
        word1 |= (nibble >> 2) & 1;
        word2 |= (nibble >> 1) & 1;
        word3 |= (nibble >> 0) & 1;
      };
      dstWord[0] = LE16(word3);
      dstWord[1] = LE16(word2);
      dstWord[2] = LE16(word1);
      dstWord[3] = LE16(word0);
      dstWord += 4;
    };
  };
  return true;
}
*/

bool ImportBitmap(ui32 index, 
                  BITMAPENTRY *pbme, 
                  bool printIfError,
                  bool dieIfError) 
{
  ui32 width, height;
  ui32 fileSize,minSize,size;	
  char portraitName[30];
  TEMPORARY_MEMORY tm;
  i32 destWordWidth, destHeight;
  CSB_BITMAPFILEHEADER *pBmFileHeader;
  CSB_BITMAPINFOHEADER *pBmInfoHeader;
  ui16 word0=0, word1=0, word2=0, word3=0;
  ui32 nibble;
  ui32 *pBmiColors;
  i32 numColor;
  ui8 *pPixels;
  ui16 *dstLine, *dstWord;
  ui8  *srcLine, *srcByte;
  i32 srcByteWidth;
  ui32 srcX;
	// TODO: Add your command handler code here
//	SAFEFILE f;
  i32 i, j, k;
  //char bitmap[(portraitPixelWidth+8)*portraitPixelHeight];
//  filename = filename1;
//  f.f = OPEN(filename,"rb");
//  if (f.f < 0)
//  {
//    filename = filename2;
//    f.f = OPEN(filename,"rb");
//    if (f.f < 0)
//    {
//      if (!printIfError) return false;
//      bmpError(filename," Cannot open file", dieIfError);
//      return false;
//    };
//  };
  minSize =   sizeof(CSB_BITMAPFILEHEADER) 
            + sizeof(CSB_BITMAPINFOHEADER)
            + sizeof(*pBmiColors),
  tm.m = ReadCSBgraphic(
           CGT_Portrait,
           index, 
           minSize,
           &fileSize,
           true,
           MALLOC116);
//  if (READ(f.f,sizeof(bmFileHeader),(char *)&bmFileHeader) != sizeof(bmFileHeader))
//  {
//    if (!printIfError) return false;
//    bmpError(filename, "Cannot read bitmap file header",dieIfError);
//    return false;
//  };
  if (tm.m == NULL)
  {
    if (!printIfError) return false;
    {
      char portraitname[30];
      sprintf(portraitName,"Portrait[%d]",index);
      bmpError(portraitname,"Cannot find portrait",dieIfError);
    };
    return false;
  };
  pBmFileHeader = (CSB_BITMAPFILEHEADER *)tm.m;
  pBmInfoHeader = (CSB_BITMAPINFOHEADER *)(tm.m + sizeof (*pBmFileHeader));
  pBmiColors = (ui32 *)((ui8 *)pBmInfoHeader + sizeof(*pBmInfoHeader));
  numColor = 16;
  if (pBmInfoHeader->biClrUsed != 0)numColor = pBmInfoHeader->biClrUsed;
  pPixels = (ui8 *)(pBmiColors + numColor);
#if defined _bigEndian
  SwapFileHeader(pBmFileHeader);
#endif
  if (pBmFileHeader->bfType != ('M'<<8) + 'B')
  {
    if (!printIfError) return false;
    bmpError(portraitName, "That does not look like a bitmap file",dieIfError);
    return false;
  };
//  if (READ(f.f,sizeof(bmInfoHeader),(char *)&bmInfoHeader) != sizeof(bmInfoHeader))
//  {
//    if (!printIfError) return false;
//    bmpError(filename, "Cannot read the InfoHeader",dieIfError);
//    return false;
//  };
#if defined _bigEndian
  SwapInfoHeader(pBmInfoHeader);
#endif
  //if (   (pBmInfoHeader->biWidth != portraitPixelWidth+8)
  //     ||(pBmInfoHeader->biHeight != 4 * portraitPixelHeight) )
  //{
  //  CString msg;
  //  msg.Format("The bitmap must be exactly\n%d by %d",
  //              portraitPixelWidth+8,
  //              4 * portraitPixelHeight);
  //  Strange(msg);
  //  return;
  //};
  if (pBmInfoHeader->biBitCount != 4)
  {
    if (!printIfError) return false;
    bmpError(portraitName, "That is not a 16-color bitmap",dieIfError);
    return false;
  };
//  if (READ(f.f,sizeof(*pBmiColors),(char *)pBmiColors) != sizeof(*pBmiColors))
//  {
//    if (!printIfError) return false;
//    bmpError(filename, "Cannot read palette",dieIfError);
//    return false;
//  };
  width  = pBmInfoHeader->biWidth;
  height = pBmInfoHeader->biHeight;
  size = (width+1)/2 * height;
//  f.buf = (pnt)UI_malloc(width*height/2, MALLOC004);
//  if (f.buf == NULL)
//  {
//    if (!printIfError) return false;
//    bmpError(filename,"Cannot allocate memory for bitmap",dieIfError);
//    return false;
//  };
//  if (READ(f.f, size, f.buf) != size)
//  {
//    if (!printIfError) return false;
//    bmpError(filename,"Cannot read bitmap pixel data",dieIfError);
//    return false;
//  };
  if (fileSize < minSize + size)
  {
    if (!printIfError) return false;
    bmpError(portraitName,"Not enough pixel data",dieIfError);
    return false;
  };
  destWordWidth = (width+15)/16 * 4; //4 words per 16 pixels
  srcByteWidth = (width+1)/2;//4 bits per pixel
  destHeight = height;
  pbme->bitmap = (pnt)UI_malloc(destWordWidth*2*destHeight,
                                MALLOC005);
  pbme->width = width;
  pbme->height = height;
  if (pbme->bitmap == NULL)
  {
    if (!printIfError) return false;
    bmpError(portraitName,"Cannot allocate memory for bitmap",dieIfError);
    return false;
  };
  srcLine = (ui8 *)pPixels + srcByteWidth * (destHeight-1);
  dstLine = (ui16 *)pbme->bitmap;
  for (i=0; 
       i<destHeight; 
       i++, srcLine-=srcByteWidth, dstLine+=destWordWidth)
  {
    srcByte = srcLine;
    dstWord = dstLine;
    word0 = word1 = word2 = word3 = 0;
    srcX = 0;
    for (j=0; j<destWordWidth/4; j++)
    {
      for (k=0; k<16; k++, srcX++)
      {
        if (srcX >= width) nibble = 0;
        else
        {
          if ((k & 1) == 0)
          {
            nibble = ((*srcByte) >> 4) & 15;
          }
          else
          {
            nibble = (*srcByte) & 15;
            srcByte++;
          };
        };
        word0 <<= 1;
        word1 <<= 1;
        word2 <<= 1;
        word3 <<= 1;
        word0 |= (nibble >> 3) & 1;
        word1 |= (nibble >> 2) & 1;
        word2 |= (nibble >> 1) & 1;
        word3 |= (nibble >> 0) & 1;
      };
      dstWord[0] = LE16(word3);
      dstWord[1] = LE16(word2);
      dstWord[2] = LE16(word1);
      dstWord[3] = LE16(word0);
      dstWord += 4;
    };
  };
  return true;
}


struct PORTRAIT
{
  i32 index;
  PORTRAIT *next;
  char portrait[29*16];
};

class PORTRAITS
{
public:
  PORTRAIT *first;
  PORTRAITS(void){first=NULL;};
  ~PORTRAITS(void);
};

PORTRAITS::~PORTRAITS(void)
{
  PORTRAIT *temp;
  while (first != NULL)
  {
    temp = first;
    first = temp->next;
    delete temp;
  };
};

ui8 errorPortraitBitmap[] = 
{
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0x83,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xBF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xBF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0x86,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xBE,0x8F,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xBE,0x7F,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xBE,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xBE,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0x82,0xEF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFE,0xE8,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFE,0xE7,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFE,0xEF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFE,0xEF,0x00,0x00,0x00,0x00,
 0x00,0x00,0x0F,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xEE,0x00,0x00,0x00,0x00,
 0x00,0x00,0xF7,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xEE,0x00,0x00,0x00,0x00,
 0x00,0x00,0xF7,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xEE,0x00,0x00,0x00,0x00,
 0x00,0x00,0xF7,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xEE,0x00,0x00,0x00,0x00,
 0x00,0x00,0xF6,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFE,0x00,0x00,0x00,0x00,
 0x00,0x00,0xF6,0x8F,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFE,0x00,0x00,0x00,0x00,
 0x00,0x00,0xF6,0x7F,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFE,0x00,0x00,0x00,0x00,
 0x00,0x00,0xF6,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0x0E,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFE,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFE,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFE,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFE,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
 0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
};

void GetErrorPortrait(BITMAPENTRY *bitmapentry)
{
  bitmapentry->width = 32;
  bitmapentry->height = 29;
  bitmapentry->bitmap = (pnt)UI_malloc(32*29/2, MALLOC006);
  if (bitmapentry->bitmap == NULL)
  {
    die(0xcb4a,"Cannot allocate memory");
  };
  memcpy(bitmapentry->bitmap,errorPortraitBitmap,32*29/2);
}

PORTRAITS portraitCache;

pnt GetExternalPortraitAddress(i32 index)
{
  BITMAPENTRY bitmapentry;
  PORTRAIT *pp;
//  char filename1[30];
  char portraitname[30];
  for (pp=portraitCache.first;
       pp != NULL;
       pp = pp->next)
  {
    if (pp->index == index) return (pnt)pp->portrait;
  };
//  sprintf(filename1,"Portraits/Portrait%03d.bmp",index);
  if (!ImportBitmap(index,&bitmapentry, true, false))
  {
    GetErrorPortrait(&bitmapentry);
  };
  if (  (bitmapentry.width != 32)
      ||(bitmapentry.height != 29))
  {
    sprintf(portraitname,"Portrait[%03d]",index);
    bmpError(portraitname,"Portrait bitmap not 32 by 29",false);
    GetErrorPortrait(&bitmapentry);
  };
  pp = new PORTRAIT;
  pp->index = index;
  memcpy(pp->portrait, bitmapentry.bitmap, 32*29/2);
  pp->next = portraitCache.first;
  portraitCache.first = pp;
  return (pnt)pp->portrait;
}

bool OVERLAYDATA::ReadOverlay(i32 onum)
{
  static bool firstTime = true;
  TEMPORARY_MEMORY fileData;
  i32 numColor, bmpsize;
  ui32 fileSize;
  CSB_BITMAPINFOHEADER *pBmiHeader;
  ui32             *pColors;
  ui8              *pPixels;
  fileData.m = ReadCSBgraphic(
       CGT_ViewportOverlay,
       onum, 
       sizeof(CSB_BITMAPFILEHEADER) + sizeof(CSB_BITMAPINFOHEADER),
       &fileSize,
       true,
       MALLOC117);
  if (fileData.m == NULL)
  {
    if (!firstTime) return false;
    firstTime = false;
    UI_MessageBox("Cannot find overlay","error",MESSAGE_OK);
    return false;
  };
  pBmiHeader = (CSB_BITMAPINFOHEADER *)(fileData.m + sizeof(CSB_BITMAPFILEHEADER));
  pColors = (ui32 *)((ui8 *)pBmiHeader + sizeof(CSB_BITMAPINFOHEADER));
  //currentOverlay.Allocate();
  numColor = 256;
  if (pBmiHeader->biClrUsed != 0) numColor = pBmiHeader->biClrUsed;
  pPixels = (ui8 *)pColors + 4*numColor;
  bmpsize = (pBmiHeader->biWidth + 1) / 2 * 2;
  bmpsize *= pBmiHeader->biHeight;
  if (fileSize <  sizeof(CSB_BITMAPFILEHEADER)
                + sizeof(CSB_BITMAPINFOHEADER)
                + 4 * numColor
                + bmpsize)
  {
    if (!firstTime) return false;
    UI_MessageBox("Incomplete Overlay Bitmap","Error",MESSAGE_OK);
    return false;
  };
  if (bmpsize > 136 * 224)
  {
    if (firstTime)
    {
      UI_MessageBox("Overlay too big","error",MESSAGE_OK);
      firstTime = false;
    };
    return false;
  };
  Allocate();
  //ovlDecode.GetBytes(m_overlay, bmpsize);
  memcpy(m_overlay, pPixels, bmpsize);
  memcpy(m_overlayPalette, pColors, 4*numColor);
  m_p1 = 0;
  m_p2 = 0;
  m_p3 = 0;
  m_p4 = 0;
  return true;
}

void OVERLAYDATA::Mirror(void)
{
  ui8 *f, *l;
  i32 line;
  i32 i;
  for (line=0; line<136; line++)
  {
    char temp;
    f = m_overlay + line*224;
    l = f+223;
    for (i=0; i<112; i++)
    {
      temp=*f; *f=*l; *l=temp;
      f++;
      l--;
    };
  };
}

void SetOverlayPalette(i32 onum, i32 /*p1*/, i32 /*p2*/, i32 density)
{
  ui32 actualSize;
  ui16 *result;
  i32 i, intensity;
  i32 transparency = 100 - density;
  if (onum < 0)
  {
    ClearOverlayPalette();
    return;
  };
  result = (ui16 *)ReadCSBgraphic(
                          CGT_OverlayPalette,
                          onum,
                          1024,
                          &actualSize,
                          true,
                          MALLOC118);
  if (result == NULL) return;
  for (i=0; i<512; i++)
  {
    intensity = ((result[i]>>7)&0xf8);
    intensity |= intensity>>5;
    overlayPaletteRed[i] = (ui8)((overlayPaletteRed[i] * transparency + intensity * density + 50) / 100);
    intensity = ((result[i]>>2)&0xf8);
    intensity |= intensity>>5;
    overlayPaletteGreen[i] = (ui8)((overlayPaletteGreen[i] * transparency + intensity * density + 50) / 100);
    intensity = ((result[i]<<3)&0xf8);
    intensity |= intensity>>5;
    overlayPaletteBlue[i] = (ui8)((overlayPaletteBlue[i] * transparency + intensity * density + 50) / 100);
  };
  UI_free(result);
  ForceScreenDraw();
}


void SelectOverlay(i32 onum, i32 p1, i32 p2, i32 p3, i32 p4)
{
  if(currentOverlay.m_overlayNumber != onum)
  {
    if (currentOverlay.ReadOverlay(onum))
    {
      currentOverlay.m_overlayNumber = onum;
      overlayActive = true;
    }
    else
    {
      overlayActive = false;
    };
    currentOverlay.m_change = true;
  }
  else
  {
    overlayActive = true;
  };
  if (    (currentOverlay.m_p1 != p1)
       || (currentOverlay.m_p2 != p2)
       || (currentOverlay.m_p4 != p4)
     ) currentOverlay.m_change = true;

  if (((currentOverlay.m_p1 ^ p1) & 2) != 0)
  {
    currentOverlay.Mirror();
  };

  currentOverlay.m_p1 = p1;
  currentOverlay.m_p2 = p2;
  if (p3 != 0) currentOverlay.m_p3 = d.Time + p3;
  currentOverlay.m_p4 = p4;    
}




