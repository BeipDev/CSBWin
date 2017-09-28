#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

extern bool playback_71;


bool OpenGraphicTraceFile();
void CloseGraphicTraceFile();
void info(char *msg, ui32 n);


#ifdef _DEBUG
void GTrace_s(char *s)
{
  if (GraphicTraceActive)
  {
    fprintf(GETFILE(GraphicTraceFile),"%s",s);
  };
}

void GTrace_1(char *s, i32 a)
{
  if (GraphicTraceActive)
  {
    fprintf(GETFILE(GraphicTraceFile), s, a);
  };
}

void GTrace_2(char *s, i32 a, i32 b)
{
  if (GraphicTraceActive)
  {
    fprintf(GETFILE(GraphicTraceFile), s, a, b);
  };
}

void GTrace_3(char *s, i32 a, i32 b, i32 c)
{
  if (GraphicTraceActive)
  {
    fprintf(GETFILE(GraphicTraceFile), s, a, b, c);
  };
}

void GTrace_4(char *s, i32 a, i32 b, i32 c, i32 dd)
{
  if (GraphicTraceActive)
  {
    fprintf(GETFILE(GraphicTraceFile), s, a, b, c, dd);
  };
}

void GTrace_6(char *s, i32 a, i32 b, i32 c, i32 dd, i32 e, i32 f)
{
  if (GraphicTraceActive)
  {
    fprintf(GETFILE(GraphicTraceFile), s, a, b, c, dd, e, f);
  };
}

void GTrace_8(char *s, i32 a, i32 b, i32 c, i32 dd, i32 e, i32 f, i32 g, i32 h)
{
  if (GraphicTraceActive)
  {
    fprintf(GETFILE(GraphicTraceFile), s, a, b, c, dd, e, f, g, h);
  };
}

void GTrace_12(char *s, i32 a, i32 b, i32 c, i32 dd, i32 e, i32 f, i32 g, i32 h, i32 i, i32 j, i32 k, i32 l)
{
  if (GraphicTraceActive)
  {
    fprintf(GETFILE(GraphicTraceFile), s, a, b, c, dd, e, f, g, h, i, j, k, l);
  };
}

#endif

#if _DEBUG
#define GTRACE_S(s) GTrace_s(s);
#define GTRACE_1(s,a) GTrace_1((s),(a));
#define GTRACE_2(s,a,b) GTrace_2((s),(a),(b));
#define GTRACE_3(s,a,b,c) GTrace_3((s),(a),(b),(c));
#define GTRACE_4(s,a,b,c,d) GTrace_4((s),(a),(b),(c), (d));
#define GTRACE_6(s,a,b,c,d,e,f) GTrace_6((s),(a),(b),(c), (d), (e), (f));
#define GTRACE_8(s,a,b,c,d,e,f,g,h) GTrace_8((s),(a),(b),(c), (d), (e), (f), (g), (h));
#define GTRACE_12(s,a,b,c,d,e,f,g,h,i,j,k,l) GTrace_12((s),(a),(b),(c), (d), (e), (f), (g), (h), (i), (j), (k), (l));
#else
#define GTRACE_S(s)
#define GTRACE_1(s,a)
#define GTRACE_2(s,a,b)
#define GTRACE_3(s,a,b,c)
#define GTRACE_4(s,a,b,c,d)
#define GTRACE_6(s,a,b,c,d,e,f)
#define GTRACE_8(s,a,b,c,d,e,f,g,h)
#define GTRACE_12(s,a,b,c,d,e,f,g,h,i,j,k,l)
#endif


class CUSTOMBITMAP
{
  ui16 *m_pGraphic;
  i32   m_numberOfBitPlanes;
  i32   m_width;
  i32   m_height;
  i32   m_wordsPerRow;
  i32   m_wordsPerGroup;
  i32   m_maxIndex;      //First non-accesible word
public:
  CUSTOMBITMAP();
  ~CUSTOMBITMAP();
  void  Initialize(ui16 *pGraphic, 
                   i32 numberOfBitPlanes, 
                   i32 grapahicSize);  //i32 units
  void  InitializeMask(BACKGROUND_MASK *pMask);
  i32   GetPixel1(i32 x, i32 y);
  void  PutPixel1(i32 x, i32 y, i32 pixel);
  i32   GetPixel4(i32 x, i32 y);
  void  PutPixel4(i32 x, i32 y, i32 pixel);
};


CUSTOMBITMAP::CUSTOMBITMAP()
{
  m_wordsPerRow = 0;
  m_wordsPerGroup = 0;
  m_maxIndex = -1;
}

CUSTOMBITMAP::~CUSTOMBITMAP()
{
}

void CUSTOMBITMAP::Initialize(
                      ui16 *pGraphic, 
                      i32   numberOfBitPlanes,
                      i32   graphicSize) // i32 units
{
  m_pGraphic = pGraphic+2;
  m_numberOfBitPlanes = numberOfBitPlanes;
  m_width = *((ui32 *)(pGraphic));
  ASSERT(m_width > 0,"Background Graphic Width = 0");
  m_height = (graphicSize * 8) / m_width;
  m_wordsPerGroup = m_numberOfBitPlanes;
  m_wordsPerRow = m_width/16 * m_wordsPerGroup;
  m_maxIndex = graphicSize*2;  //First non-accesible word
}

i32 CUSTOMBITMAP::GetPixel4(i32 x, i32 y)
{
  i32 i;
  ui32 mask, b;
  ui8 *p;
  i = (x/16)*m_wordsPerGroup + y*m_wordsPerRow;
  b = 15 - (x & 15);
  p = (ui8 *)(m_pGraphic + i);
  if (b < 8) p++; else b-=8;
  mask  = 1 << b;
  if (i > m_maxIndex) return -1;
  return   (((((((p[6]&mask) << 1)
           | (p[4]&mask)) << 1)
           | (p[2]&mask)) << 1)
           | (p[0]&mask)) >> b;
}

void CUSTOMBITMAP::PutPixel4(i32 x, i32 y, i32 pixel)
{
  i32 b, i, mask, nmask;
  ui8 *p;
  i = (x/16)*m_wordsPerGroup + y*m_wordsPerRow;
  b = 15 - (x & 15);
  p = (ui8 *)(m_pGraphic + i);
  if (b < 8) p++; else b-=8;
  mask  = 1 << b;
  nmask = ~mask;
  if (i > m_maxIndex) return;
  pixel <<= b;  // Position bottom bit
  p[0] = (ui8)((p[0]&nmask) | (pixel&mask));
  pixel>>=1;
  p[2] = (ui8)((p[2]&nmask) | (pixel&mask));
  pixel>>=1;
  p[4] = (ui8)((p[4]&nmask) | (pixel&mask));
  pixel>>=1;
  p[6] = (ui8)((p[6]&nmask) | (pixel&mask));
}


void AffineTransform4(CUSTOMBITMAP *pSrc,
                      CUSTOMBITMAP *pDst,
                      AFFINEMASK   *pAM)
{ // Four bitplanes
  i32 row, col;
  i32 srcX, srcY;
  i32 xModulus, yModulus;
  xModulus = 32 * pAM->srcWidth;
  yModulus = 32 * pAM->srcHeight;
  for (row=0; row<pAM->dstHeight; row++)
  {
    for (col=0; col<pAM->dstWidth; col++)
    {
      srcX = pAM->Cxx * col + pAM->Cxy * row + pAM->Cxc + 16;
      srcY = pAM->Cyx * col + pAM->Cyy * row + pAM->Cyc + 16;
      srcX = (((srcX % xModulus) + xModulus) % xModulus) / 32;
      srcY = (((srcY % yModulus) + yModulus) % yModulus) / 32;
      pDst->PutPixel4(col, row, pSrc->GetPixel4(pAM->srcX+srcX, pAM->srcY+srcY));
    };
  };
}

i32 CUSTOMBITMAP::GetPixel1(i32 x, i32 y)
{
  i32 i;
  ui32 mask, b;
  ui8 *p;
  i = (x/16)*m_wordsPerGroup + y*m_wordsPerRow;
  b = 15 - (x & 15);
  p = (ui8 *)(m_pGraphic + i);
  if (b < 8) p++; else b-=8;
  mask  = 1 << b;
  if (i > m_maxIndex) return -1;
  return (p[0]&mask) >> b;
}

void CUSTOMBITMAP::PutPixel1(i32 x, i32 y, i32 pixel)
{
  i32 b, i, mask, nmask;
  ui8 *p;
  i = (x/16)*m_wordsPerGroup + y*m_wordsPerRow;
  b = 15 - (x & 15);
  p = (ui8 *)(m_pGraphic + i);
  if (b < 8) p++; else b-=8;
  mask  = 1 << b;
  nmask = ~mask;
  if (i > m_maxIndex) return;
  pixel <<= b;  // Position bottom bit
  p[0] = (ui8)((p[0]&nmask) | (pixel&mask));
}


void AffineTransform1(CUSTOMBITMAP *pSrc,
                      CUSTOMBITMAP *pDst,
                      AFFINEMASK   *pAM)
{ // Four bitplanes
  i32 row, col;
  i32 srcX, srcY;
  i32 xModulus, yModulus;
  xModulus = 32 * pAM->srcWidth;
  yModulus = 32 * pAM->srcHeight;
  for (row=0; row<pAM->dstHeight; row++)
  {
    for (col=0; col<pAM->dstWidth; col++)
    {
      srcX = pAM->Cxx * col + pAM->Cxy * row + pAM->Cxc + 16;
      srcY = pAM->Cyx * col + pAM->Cyy * row + pAM->Cyc + 16;
      srcX = (((srcX % xModulus) + xModulus) % xModulus) / 32;
      srcY = (((srcY % yModulus) + yModulus) % yModulus) / 32;
      pDst->PutPixel1(col, row, pSrc->GetPixel1(pAM->srcX+srcX, pAM->srcY+srcY));
    };
  };
}

void CUSTOMBITMAP::InitializeMask(BACKGROUND_MASK *pMask)
{
  m_pGraphic = pMask->mask;
  m_numberOfBitPlanes = 1;
  m_width = pMask->width;
  m_height = pMask->height;
  m_wordsPerGroup = m_numberOfBitPlanes;
  m_wordsPerRow = m_width/16 * m_wordsPerGroup;
  m_maxIndex = m_width*m_height/16;  //First non-accesible word
}


#ifdef _DEBUG
void cgeDebug(char *s, i32 i)
{
  if (GraphicTraceActive)
  {
    fprintf(GETFILE(TraceFile),s,i);
    fflush(GETFILE(TraceFile));
  };
}
void cgeDebug(char *s, i32 i, i32 j, i32 k)
{
  if (GraphicTraceActive)
  {
    fprintf(GETFILE(TraceFile),s,i, j, k);
    fflush(GETFILE(TraceFile));
  };
}
#define CGEDEBUG_1(s, i) cgeDebug((s), (i));
#define CGEDEBUG_3(s, i,j,k) cgeDebug((s),(i),(j),(k));
#else
#define CGEDEBUG_1(s, i)
#define CGEDEBUG_3(s, i,j,k)
#endif


void ApplyBackground (BACKGROUND_MASK *mask, 
                      ui32            *bitmap,
                      ui32             bitmapsize);
void ApplyDecoration (ui16            *pDecoration,
                      ui8             *location,
                      ui32             bitmapsize);


#define newgraphics


extern ui32 numRandomCalls;
extern i32 VBLMultiplier;
extern i32 DrawViewportCount;


#define NUMCELL 16

#define RF4L2   0
#define RF4L1   4
#define RF4     6
#define RF4R1   5
#define RF4R2   1
#define RF3L2   2
#define RF3L1   7
#define RF3     9
#define RF3R1   8
#define RF2L2   3
#define RF3R2  10
#define RF2L1  12
#define RF2    14
#define RF2R1  13
#define RF2R2  11
#define RF1L1  15
#define RF1    17
#define RF1R1  16
#define RF0L1  18
#define RF0    20
#define RF0R1  19

i8 relativeX[21] =
{
  -2,+2,-2,+2,-1,+1,+0,-1,+1,+0,-2,+2,-1,+1,+0,-1,+1,+0,-1,+1,+0
};

i8 relativeY[21] =
{
  4,4,3,3,4,4,4,3,3,3,2,2,2,2,2,1,1,1,0,0,0
};

class VIEWPORTCANVAS
{
private:
  ui8 x[NUMCELL];
  ui8 y[NUMCELL];
  ui8 *computationResults[NUMCELL];
  ui8 skinNum[NUMCELL];
  ui8 cellType[NUMCELL];
  ui32 *pInstructions;
};



enum OPCODE
{
  Return                              = 60000,
  //
  StdBltShape                         = 60001,
  StdDrawWallText                     = 60002,
  StdDrawWallDecoration               = 60003,
  StdDrawFloorDecoration              = 60004,
  StdDrawCeilingPit                   = 60005,
  StdDrawRoomObjects                  = 60006,
  //
  wallDecorationLeft                  = 60007,
  wallDecorationFront                 = 60008,
  wallDecorationRight                 = 60009,
  //
  StdBltShapeToViewport               = 60010,
  //
  DoorSwitch                          = 60011,
  StdDrawDoorSwitch                   = 60012,
  SeeThroughWalls                     = 60013,
  StdDrawSeeThruDoorEdge              = 60014,
  GetDecoration0                      = 60015,
  //
  Literal                             = 60016,
  //
  StdMirrorShapeToViewport            = 60017,
  StdMirrorGraphicToViewport          = 60018,
  StdGraphicToViewport                = 60019,
  StdDrawTeleporter                   = 60020,
  //
  StdStairFacingUpRectangleF3L1       = 60021,
  StdStairFacingUpRectangleF3         = 60022,
  StdStairFacingUpRectangleF3R1       = 60023,
  StdStairFacingUpRectangleF2L1       = 60024,
  StdStairFacingUpRectangleF2         = 60025,
  StdStairFacingUpRectangleF2R1       = 60026,
  StdStairFacingUpRectangleF1L1       = 60027,
  StdStairFacingUpRectangleF1         = 60028,
  StdStairFacingUpRectangleF1R1       = 60029,
  StdStairFacingUpRectangleF0L        = 60030,
  StdStairFacingUpRectangleF0R        = 60031,
  StdStairFacingDownRectangleF3L1     = 60032,
  StdStairFacingDownRectangleF3       = 60033,
  StdStairFacingDownRectangleF3R1     = 60034,
  StdStairFacingDownRectangleF2L1     = 60035,
  StdStairFacingDownRectangleF2       = 60036,
  StdStairFacingDownRectangleF2R1     = 60037,
  StdStairFacingDownRectangleF1L1     = 60038,
  StdStairFacingDownRectangleF1       = 60039,
  StdStairFacingDownRectangleF1R1     = 60040,
  StdStairFacingDownRectangleF0L      = 60041,
  StdStairFacingDownRectangleF0R      = 60042,
  //
  FloorDecorationGraphicOrdinalF4L1   = 60043,
  FloorDecorationGraphicOrdinalF4     = 60044,
  FloorDecorationGraphicOrdinalF4R1   = 60045,
  FloorDecorationGraphicOrdinalF3L1   = 60046,
  FloorDecorationGraphicOrdinalF3     = 60047,
  FloorDecorationGraphicOrdinalF3R1   = 60048,
  FloorDecorationGraphicOrdinalF2L1   = 60049,
  FloorDecorationGraphicOrdinalF2     = 60050,
  FloorDecorationGraphicOrdinalF2R1   = 60051,
  FloorDecorationGraphicOrdinalF1L1   = 60052,
  FloorDecorationGraphicOrdinalF1     = 60053,
  FloorDecorationGraphicOrdinalF1R1   = 60054,
  FloorDecorationGraphicOrdinalF0L1   = 60055,
  FloorDecorationGraphicOrdinalF0     = 60056,
  FloorDecorationGraphicOrdinalF0R1   = 60057,
  //
  StdFloorPitRectangleF3L1            = 60058,
  StdFloorPitRectangleF3              = 60059,
  StdFloorPitRectangleF3R1            = 60060,
  StdFloorPitRectangleF2L1            = 60061,
  StdFloorPitRectangleF2              = 60062,
  StdFloorPitRectangleF2R1            = 60063,
  StdFloorPitRectangleF1L1            = 60064,
  StdFloorPitRectangleF1              = 60065,
  StdFloorPitRectangleF1R1            = 60066,
  StdFloorPitRectangleF0L1            = 60067,
  StdFloorPitRectangleF0              = 60068,
  StdFloorPitRectangleF0R1            = 60069,
  //
  StdTeleporterRectangleF3L1          = 60070,
  StdTeleporterRectangleF3            = 60071,
  StdTeleporterRectangleF3R1          = 60072,
  StdTeleporterRectangleF2L1          = 60073,
  StdTeleporterRectangleF2            = 60074,
  StdTeleporterRectangleF2R1          = 60075,
  StdTeleporterRectangleF1L1          = 60076,
  StdTeleporterRectangleF1            = 60077,
  StdTeleporterRectangleF1R1          = 60078,
  StdTeleporterRectangleF0L1          = 60079,
  StdTeleporterRectangleF0            = 60080,
  StdTeleporterRectangleF0R1          = 60081,
  //
  StdWallRectangleF3L2                = 60082,
  StdWallRectangleF3L1                = 60083,
  StdWallRectangleF3                  = 60084,
  StdWallRectangleF3R1                = 60085,
  StdWallRectangleF3R2                = 60086,
  StdWallRectangleF2L1                = 60087,
  StdWallRectangleF2                  = 60088,
  StdWallRectangleF2R1                = 60089,
  StdWallRectangleF1L1                = 60090,
  StdWallRectangleF1                  = 60091,
  StdWallRectangleF1R1                = 60092,
  StdWallRectangleF0L1                = 60093,
  StdWallRectangleF0                  = 60094,
  StdWallRectangleF0R1                = 60095,
  //
  StdDrawDoor                         = 60096,
  //
  F4L1xy                              = 60097,
  F4xy                                = 60098,
  F4R1xy                              = 60099,
  F3L2xy                              = 60100,
  F3L1xy                              = 60101,
  F3xy                                = 60102,
  F3R1xy                              = 60103,
  F3R2xy                              = 60104,
  F2L1xy                              = 60105,
  F2xy                                = 60106,
  F2R1xy                              = 60107,
  F1L1xy                              = 60108,
  F1xy                                = 60109,
  F1R1xy                              = 60110,
  F0L1xy                              = 60111,
  F0xy                                = 60112,
  F0R1xy                              = 60113,
  //
  F4L1Contents                        = 60114,
  F4Contents                          = 60115,
  F4R1Contents                        = 60116,
  F3L2Contents                        = 60117,
  F3L1Contents                        = 60118,
  F3Contents                          = 60119,
  F3R1Contents                        = 60120,
  F3R2Contents                        = 60121,
  F2L1Contents                        = 60122,
  F2Contents                          = 60123,
  F2R1Contents                        = 60124,
  F1L1Contents                        = 60125,
  F1Contents                          = 60126,
  F1R1Contents                        = 60127,
  F0L1Contents                        = 60128,
  F0Contents                          = 60129,
  F0R1Contents                        = 60130,
  //
  F4L1                                = 60131,
  F4                                  = 60132,
  F4R1                                = 60133,
  F3L2                                = 60134,
  F3L1                                = 60135,
  F3                                  = 60136,
  F3R1                                = 60137,
  F3R2                                = 60138,
  F2L1                                = 60139,
  F2                                  = 60140,
  F2R1                                = 60141,
  F1L1                                = 60142,
  F1                                  = 60143,
  F1R1                                = 60144,
  F0L1                                = 60145,
  F0                                  = 60146,
  F0R1                                = 60147,
  //
  StdStairFacingDownGraphicF3L1       = 60148,
  StdStairFacingDownGraphicF3         = 60149,
  StdStairFacingDownGraphicF3R1       = 60150,
  StdStairFacingDownGraphicF2L1       = 60151,
  StdStairFacingDownGraphicF2         = 60152,
  StdStairFacingDownGraphicF2R1       = 60153,
  StdStairFacingDownGraphicF1L1       = 60154,
  StdStairFacingDownGraphicF1         = 60155,
  StdStairFacingDownGraphicF1R1       = 60156,
  StdStairFacingDownGraphicF0         = 60157,
  //
  StdStairFacingUpGraphicF3L1         = 60158,
  StdStairFacingUpGraphicF3           = 60159,
  StdStairFacingUpGraphicF3R1         = 60160,
  StdStairFacingUpGraphicF2L1         = 60161,
  StdStairFacingUpGraphicF2           = 60162,
  StdStairFacingUpGraphicF2R1         = 60163,
  StdStairFacingUpGraphicF1L1         = 60164,
  StdStairFacingUpGraphicF1           = 60165,
  StdStairFacingUpGraphicF1R1         = 60166,
  StdStairFacingUpGraphicF0           = 60167,
  //
  WallF3L1Front                       = 60168,
  WallF3L1Right                       = 60169,
  WallF3Front                         = 60170,
  WallF3R1Left                        = 60171,
  WallF3R1Front                       = 60172,
  WallF2L1Front                       = 60173,
  WallF2L1Right                       = 60174,
  WallF2Front                         = 60175,
  WallF2R1Left                        = 60176,
  WallF2R1Front                       = 60177,
  WallF1L1Right                       = 60178,
  WallF1Front                         = 60179,
  WallF1R1Left                        = 60180,
  //
  StdStairEdgeGraphicF2               = 60181,
  StdStairEdgeGraphicUpF1             = 60182,
  StdStairEdgeGraphicDownF1           = 60183,
  StdStairEdgeGraphicF0L1             = 60184,
  StdStairEdgeGraphicF0R1             = 60185,
  StdStairEdgeRectF2L1                = 60186,
  StdStairEdgeRectF2R1                = 60187,
  StdStairEdgeUpRectF1L1              = 60188,
  StdStairEdgeUpRectF1R1              = 60189,
  StdStairEdgeDownRectF1L1            = 60190,
  StdStairEdgeDownRectF1R1            = 60191,
  StdStairEdgeRectF0L1                = 60192,
  StdStairEdgeRectF0R1                = 60193,
  //
  StdDoorEdgeFrameBitmapF0            = 60194,
  StdDoorFacingFrameLeftBitmapF2      = 60195,
  StdDoorFacingFrameLeftBitmapF1      = 60196,
  StdDoorFacingFrameRightBitmapF1     = 60197,
  StdDoorFacingFrameLeftBitmapF3      = 60198,
  StdDoorFacingFrameLeftBitmapF3R1    = 60199,
  StdDoorFacingFrameLeftBitmapF3L1    = 60200,
  StdDoorFacingTopTrackBitmapF1       = 60201,
  StdDoorFacingTopTrackBitmapF2       = 60202,
  //
  F3L1DoorRecordIndex                 = 60203,
  F3DoorRecordIndex                   = 60204,
  F3R1DoorRecordIndex                 = 60205,
  F2L1DoorRecordIndex                 = 60206,
  F2DoorRecordIndex                   = 60207,
  F2R1DoorRecordIndex                 = 60208,
  F1L1DoorRecordIndex                 = 60209,
  F1DoorRecordIndex                   = 60210,
  F1R1DoorRecordIndex                 = 60211,
  F0DoorRecordIndex                   = 60212,
  //
  F3L1DoorState                       = 60213,
  F3DoorState                         = 60214,
  F3R1DoorState                       = 60215,
  F2L1DoorState                       = 60216,
  F2DoorState                         = 60217,
  F2R1DoorState                       = 60218,
  F1L1DoorState                       = 60219,
  F1DoorState                         = 60220,
  F1R1DoorState                       = 60221,
  F0DoorState                         = 60222,
  //
  StdDoorGraphicsF3                   = 60223,
  StdDoorGraphicsF2                   = 60224,
  StdDoorGraphicsF1                   = 60225,
  //
  StdDoorRectsF3L1                    = 60226,
  StdDoorRectsF3                      = 60227,
  StdDoorRectsF3R1                    = 60228,
  StdDoorRectsF2L1                    = 60229,
  StdDoorRectsF2                      = 60230,
  StdDoorRectsF2R1                    = 60231,
  StdDoorRectsF1L1                    = 60232,
  StdDoorRectsF1                      = 60233,
  StdDoorRectsF1R1                    = 60234,
  StdDoorRectsF0                      = 60235,
  //
  Alcove                              = 60236,
  HasDecoration                       = 60237,
  LeaveMortarJoint                    = 60238,
  //
  StdDoorEdgeFrameRectF0              = 60239,
  StdDoorFacingTopTrackRectF2L1       = 60240,
  StdDoorFacingTopTrackRectF2         = 60241,
  StdDoorFacingTopTrackRectF2R1       = 60242,
  StdDoorFacingTopTrackRectF1L1       = 60243,
  StdDoorFacingTopTrackRectF1         = 60244,
  StdDoorFacingTopTrackRectF1R1       = 60245,
  StdDoorFacingTopTrackRectF0         = 60246,
  //
  JumpZ                               = 60247,
  JumpNZ                              = 60248,
  Jump                                = 60249,
  //
  StdDoorFacingFrameRectF3L1          = 60250,
  StdDoorFacingFrameRectF3R1          = 60251,
  StdDoorFacingFrameRightRectF3       = 60252,
  StdDoorFacingFrameLeftRectF3        = 60253,
  StdDoorFacingFrameRightRectF2       = 60254,
  StdDoorFacingFrameLeftRectF2        = 60255,
  StdDoorFacingFrameRightRectF1       = 60256,
  StdDoorFacingFrameLeftRectF1        = 60257,
  //
  StdWallBitmapF3L2                   = 60258,
  StdWallBitmapF3L1                   = 60259,
  StdWallBitmapF3                     = 60260,
  StdWallBitmapF3R1                   = 60261,
  StdWallBitmapF3R2                   = 60262,
  StdWallBitmapF2L1                   = 60263,
  StdWallBitmapF2                     = 60264,
  StdWallBitmapF2R1                   = 60265,
  StdWallBitmapF1L1                   = 60266,
  StdWallBitmapF1                     = 60267,
  StdWallBitmapF1R1                   = 60268,
  StdWallBitmapF0L1                   = 60269,
  StdWallBitmapF0R1                   = 60270,
  //
  DrawOrder00                         = 60271,
  DrawOrder32                         = 60272,
  DrawOrder28                         = 60273,
  DrawOrder39                         = 60274,
  DrawOrder41                         = 60275,
  DrawOrder18                         = 60276,
  DrawOrder49                         = 60277,
  DrawOrder3421                       = 60278,
  DrawOrder218                        = 60279,
  DrawOrder349                        = 60280,
  DrawOrder4312                       = 60281,
  DrawOrder431                        = 60282,
  DrawOrder128                        = 60283,
  DrawOrder439                        = 60284,
  DrawOrder321                        = 60285,
  DrawOrder412                        = 60286,
  DrawOrder01                         = 60287,
  DrawOrder02                         = 60288,
  DrawOrder21                         = 60289,
  //
  StdAllocateDerivedGraphic           = 60290,
  StdBitmapViewport                   = 60291,
  StdBltToDerivedGraphic              = 60292,
  StdSeeThruWallsRect                 = 60293,
  StdSeeThruWallsRectUByte4           = 60294,
  StdSeeThruWallsRectUByte6           = 60295,
  //
  MinusOne                            = 60296,
  StdHoleInWallBitmap                 = 60297,
  Zero                                = 60298,
  StdDeleteDerivedGraphic             = 60299,
  StdBltFromDerivedGraphic            = 60300,
  StdSeeThruWallsRect2                = 60301,

  CurrentSkin                         = 60302,
  WallGraphicID                       = 60303,
  WallMaskID                          = 60304,
  CurrentCell                         = 60305,
  DrawCustomGraphic                   = 60306,
  DrawCustomDecoration                = 60307,
  WallDecorationID                    = 60308,
  FloorDecorationID                   = 60309,
  FloorGraphicID                      = 60310,
  MiddleGraphicID                     = 60311,
  CeilingGraphicID                    = 60312,
  FloorMaskID                         = 60313,
  MiddleMaskID                        = 60314,
  CeilingMaskID                       = 60315,
  SkinNumber                          = 60316,
  CreateVirtualBitmap                 = 60317,

  SWAP                                = 60318,
  DROP                                = 60319,
  OVER                                = 60320,
  ROT                                 = 60321,
  DUP                                 = 60322,
  QuestionDUP                         = 60323,
  PICK                                = 60324,
  ROLL                                = 60325,
  PLUS                                = 60326,
  MINUS                               = 60327,
  GEAR                                = 60338,
  SLASH                               = 60339,
  EQUAL                               = 60340,
  LESS                                = 60341,
  GREATER                             = 60342,
  ULESS                               = 60343,
  ZEROEQUAL                           = 60344,
  ZEROLESS                            = 60345,
  ZEROGREATER                         = 60346,
  NOT                                 = 60347,
  AND                                 = 60348,
  OR                                  = 60349,
  IsOdd                               = 60350,
  CreateVirtualMask                   = 60351,
  NeedVirtualMaskQ                    = 60352,
  TwoDup                              = 60353,
  GetMaskHeader                       = 60356,
  ToR                                 = 60357,
  RTo                                 = 60358,
  TwoDrop                             = 60359,
  BitmapInfo                          = 60360,
  CreateVirtualAffineMask             = 60361,
  LSHIFT                              = 60362,

  DrawOrder342                        = 60363,

  // Some things we forgot
  F4L2xy                              = 60364,
  F4R2xy                              = 60365,
  F4L2                                = 60366,
  F4R2                                = 60367,

};
 


enum RELATIVE_WALL_NUMBERS
{
  RWF3L1Front    = 2,
  RWF3L1Right    = 0,
  RWF3Front      = 3,
  RWF3R1Left     = 1,
  RWF3R1Front    = 4,
  RWF2L1Front    = 7,
  RWF2L1Right    = 5,
  RWF2Front      = 8,
  RWF2R1Left     = 6,
  RWF2R1Front    = 9,
  RWF1L1Right    = 10,
  RWF1Front      = 12,
  RWF1R1Left     = 11
};


enum RECT_POINTERS
{
  RECT_StdWallRectangleF3L2        = 0,
  RECT_StdWallRectangleF3L1,
  RECT_StdWallRectangleF3,
  RECT_StdWallRectangleF3R1,
  RECT_StdWallRectangleF3R2,
  RECT_StdWallRectangleF2L1,
  RECT_StdWallRectangleF2,
  RECT_StdWallRectangleF2R1,
  RECT_StdWallRectangleF1L1,
  RECT_StdWallRectangleF1,
  RECT_StdWallRectangleF1R1,
  RECT_StdWallRectangleF0L1,
  RECT_StdWallRectangleF0,
  RECT_StdWallRectangleF0R1,
  RECT_StdFloorPitRectangleF3L1,
  RECT_StdFloorPitRectangleF3,
  RECT_StdFloorPitRectangleF3R1,
  RECT_StdFloorPitRectangleF2L1,
  RECT_StdFloorPitRectangleF2,
  RECT_StdFloorPitRectangleF2R1,
  RECT_StdFloorPitRectangleF1L1,
  RECT_StdFloorPitRectangleF1,
  RECT_StdFloorPitRectangleF1R1,
  RECT_StdFloorPitRectangleF0L1,
  RECT_StdFloorPitRectangleF0,
  RECT_StdFloorPitRectangleF0R1,
  RECT_StdStairFacingDownRectangleF3L1,
  RECT_StdStairFacingDownRectangleF3,
  RECT_StdStairFacingDownRectangleF3R1,
  RECT_StdStairFacingDownRectangleF2L1,
  RECT_StdStairFacingDownRectangleF2,
  RECT_StdStairFacingDownRectangleF2R1,
  RECT_StdStairFacingDownRectangleF1L1,
  RECT_StdStairFacingDownRectangleF1,
  RECT_StdStairFacingDownRectangleF1R1,
  RECT_StdStairFacingDownRectangleF0L,
  RECT_StdStairFacingDownRectangleF0R,
  RECT_StdStairFacingUpRectangleF3L1,
  RECT_StdStairFacingUpRectangleF3,
  RECT_StdStairFacingUpRectangleF3R1,
  RECT_StdStairFacingUpRectangleF2L1,
  RECT_StdStairFacingUpRectangleF2,
  RECT_StdStairFacingUpRectangleF2R1,
  RECT_StdStairFacingUpRectangleF1L1,
  RECT_StdStairFacingUpRectangleF1,
  RECT_StdStairFacingUpRectangleF1R1,
  RECT_StdStairFacingUpRectangleF0L,
  RECT_StdStairFacingUpRectangleF0R,
  RECT_StdTeleporterRectangleF3L1,
  RECT_StdTeleporterRectangleF3,
  RECT_StdTeleporterRectangleF3R1,
  RECT_StdTeleporterRectangleF2L1,
  RECT_StdTeleporterRectangleF2,
  RECT_StdTeleporterRectangleF2R1,
  RECT_StdTeleporterRectangleF1L1,
  RECT_StdTeleporterRectangleF1,
  RECT_StdTeleporterRectangleF1R1,
  RECT_StdTeleporterRectangleF0L1,
  RECT_StdTeleporterRectangleF0,
  RECT_StdTeleporterRectangleF0R1,
  //
  RECT_StdStairEdgeRectangleF2L1,
  RECT_StdStairEdgeRectangleF2R1,
  RECT_StdStairEdgeUpRectangleF1L1,
  RECT_StdStairEdgeUpRectangleF1R1,
  RECT_StdStairEdgeDownRectangleF1L1,
  RECT_StdStairEdgeDownRectangleF1R1,
  RECT_StdStairEdgeRectangleF0L,
  RECT_StdStairEdgeRectangleF0R,
  //
  RECT_StdDoorFacingTopTrackRectF2L1,
  RECT_StdDoorFacingTopTrackRectF2,
  RECT_StdDoorFacingTopTrackRectF2R1,
  RECT_StdDoorFacingTopTrackRectF1L1,
  RECT_StdDoorFacingTopTrackRectF1,
  RECT_StdDoorFacingTopTrackRectF1R1,
  RECT_StdDoorEdgeFrameRectF0,

  RECT_StdDoorRectsF3L1,
  RECT_StdDoorRectsF3,
  RECT_StdDoorRectsF3R1,
  RECT_StdDoorRectsF2L1,
  RECT_StdDoorRectsF2,
  RECT_StdDoorRectsF2R1,
  RECT_StdDoorRectsF1L1,
  RECT_StdDoorRectsF1,
  RECT_StdDoorRectsF1R1,
  RECT_StdDoorRectsF0,
  //
  RECT_StdDoorFacingFrameRectF3L1,
  RECT_StdDoorFacingFrameRectF3R1,
  RECT_StdDoorFacingFrameRightRectF1,
  RECT_StdDoorFacingFrameLeftRectF1,
  RECT_StdDoorFacingFrameRightRectF2,
  RECT_StdDoorFacingFrameLeftRectF2,
  RECT_StdDoorFacingFrameRightRectF3,
  RECT_StdDoorFacingFrameLeftRectF3,
  //
  RECT_StdSeeThruWallsRect,
  RECT_StdSeeThruWallsRect2,
};

enum BITMAP_POINTERS
{
  BMP_StdWallBitmapF3L2 = 1000,
  BMP_StdWallBitmapF3L1,
  BMP_StdWallBitmapF3,
  BMP_StdWallBitmapF3R1,
  BMP_StdWallBitmapF3R2,
  BMP_StdWallBitmapF2L1,
  BMP_StdWallBitmapF2,
  BMP_StdWallBitmapF2R1,
  BMP_StdWallBitmapF1L1,
  BMP_StdWallBitmapF1,
  BMP_StdWallBitmapF1R1,
  BMP_StdWallBitmapF0L1,
  BMP_StdWallBitmapF0R1,
  //
  BMP_StdDoorFacingFrameLeftBitmapF3L1,
  BMP_StdDoorFacingFrameLeftBitmapF3,
  BMP_StdDoorFacingFrameLeftBitmapF3R1,
  BMP_StdDoorFacingFrameLeftBitmapF2,
  BMP_StdDoorFacingFrameLeftBitmapF1,
  BMP_StdDoorFacingFrameRightBitmapF1,
  BMP_StdDoorEdgeFrameBitmapF0,
  BMP_StdDoorFacingTopTrackBitmapF2,
  BMP_StdDoorFacingTopTrackBitmapF1,
  //
  BMP_StdBitmapViewport,
  BMP_StdHoleInWallBitmap,
};

// ****************************************************************
//              Do nothing
// ****************************************************************
ui16 NOP[] =
{
  Return
};

// ****************************************************************
//              Any Room Type (don't care)
// ****************************************************************
ui16 StdDrawF4L1Any[] =
{
  F4L1Contents,  F4L1xy, F4L1 , DrawOrder01,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF4Any[] =
{
  F4Contents,  F4xy, F4 , DrawOrder01,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF4R1Any[] =
{
  F4R1Contents,  F4R1xy, F4R1 , DrawOrder01,  StdDrawRoomObjects,
  Return
};

// ****************************************************************
//              roomSTONE
// ****************************************************************
ui16 StdDrawF3L2Stone[] = 
{
  StdWallBitmapF3L2,   StdWallRectangleF3L2,   StdBltShape,       
  Return
};

ui16 StdDrawF3R2Stone[] = 
{
  StdWallBitmapF3R2,   StdWallRectangleF3R2,   StdBltShape,       
  Return
};

ui16 StdDrawF3L1Stone[] = 
{
  StdWallBitmapF3L1,   StdWallRectangleF3L1,   StdBltShape,       
  F3L1,  wallDecorationRight,  WallF3L1Right,  StdDrawWallText,
  F3L1,  wallDecorationRight,  WallF3L1Right,  StdDrawWallDecoration,
  F3L1,  wallDecorationFront,  WallF3L1Front,  StdDrawWallText,
  F3L1,  wallDecorationFront,  WallF3L1Front,  StdDrawWallDecoration,
  Alcove, 
      JumpZ, 5,
          F3L1Contents,  F3L1xy, F3L1 , DrawOrder00,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF3Stone[] = 
{
  StdWallBitmapF3,   StdWallRectangleF3,   StdBltShape,       
  F3,  wallDecorationFront,  WallF3Front,  StdDrawWallText,
  F3,  wallDecorationFront,  WallF3Front,  StdDrawWallDecoration,
  Alcove, 
      JumpZ, 5,
          F3Contents,  F3xy, F3 , DrawOrder00,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF3R1Stone[] = 
{
  StdWallBitmapF3R1,   StdWallRectangleF3R1,   StdBltShape,       
  F3R1,  wallDecorationLeft,  WallF3R1Left,  StdDrawWallText,
  F3R1,  wallDecorationLeft,  WallF3R1Left,  StdDrawWallDecoration,
  F3R1,  wallDecorationFront,  WallF3R1Front,  StdDrawWallText,
  F3R1,  wallDecorationFront,  WallF3R1Front,  StdDrawWallDecoration,
  Alcove, 
      JumpZ, 5,
          F3R1Contents,  F3R1xy, F3R1 , DrawOrder00,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF2L1Stone[] = 
{
  StdWallBitmapF2L1,   StdWallRectangleF2L1,   StdBltShape,       
  F2L1,  wallDecorationRight,  WallF2L1Right,  StdDrawWallText,
  F2L1,  wallDecorationRight,  WallF2L1Right,  StdDrawWallDecoration,
  F2L1,  wallDecorationFront,  WallF2L1Front,  StdDrawWallText,
  F2L1,  wallDecorationFront,  WallF2L1Front,  StdDrawWallDecoration,
  Alcove, 
      JumpZ, 5,
          F2L1Contents,  F2L1xy, F2L1 , DrawOrder00,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF2Stone[] = 
{
  StdWallBitmapF2,   StdWallRectangleF2,   StdBltShape,       
  F2,  wallDecorationFront,  WallF2Front,  StdDrawWallText,
  F2,  wallDecorationFront,  WallF2Front,  StdDrawWallDecoration,
  Alcove, 
      JumpZ, 5,
          F2Contents,  F2xy, F2 , DrawOrder00,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF2R1Stone[] = 
{
  StdWallBitmapF2R1,   StdWallRectangleF2R1,   StdBltShape,       
  F2R1,  wallDecorationLeft,  WallF2R1Left,  StdDrawWallText,
  F2R1,  wallDecorationLeft,  WallF2R1Left,  StdDrawWallDecoration,
  F2R1,  wallDecorationFront,  WallF2R1Front,  StdDrawWallText,
  F2R1,  wallDecorationFront,  WallF2R1Front,  StdDrawWallDecoration,
  Alcove, 
      JumpZ, 5,
          F2R1Contents,  F2R1xy, F2R1 , DrawOrder00,  StdDrawRoomObjects,
  Return
};


ui16 StdDrawF1L1Stone[] = 
{
  StdWallBitmapF1L1,   StdWallRectangleF1L1,   StdBltShape,       
  F1L1,  wallDecorationRight,  WallF1L1Right,  StdDrawWallText,
  F1L1,  wallDecorationRight,  WallF1L1Right,  StdDrawWallDecoration,
  Return
};

ui16 StdDrawF1Stone[] = 
{
  SeeThroughWalls, JumpZ, 28,
    Literal, 1, StdAllocateDerivedGraphic,
    StdBitmapViewport,                  // source
    Literal, 1,                         // destination = derived graphic 1
    StdSeeThruWallsRect,                // destination rectangle
    StdSeeThruWallsRectUByte4,          // Source X
    StdSeeThruWallsRectUByte6,          // Source Y
    Literal, 112,                       // Source Width
    Literal, 48,                        // destination width
    MinusOne,                           // Transparent color
        StdBltToDerivedGraphic,                         // 
    StdHoleInWallBitmap,               // source
    Literal, 1,                        // destination = derived graphic 1
    StdSeeThruWallsRect,               // destination rectangle
    Zero,                              // Source X
    Zero,                              // Source Y
    Literal, 48,                       // Source Width
    Literal, 48,                       // Destination width
    Literal, 10,                       // Transparent color
        StdBltToDerivedGraphic,
  StdWallBitmapF1,   StdWallRectangleF1,   StdBltShape,       
  F1,  wallDecorationFront,  WallF1Front,  StdDrawWallDecoration,
  F1,  wallDecorationFront, HasDecoration, JumpZ, 1, LeaveMortarJoint,
  F1,  wallDecorationFront,  WallF1Front,  StdDrawWallText,
  Alcove, 
      JumpZ, 5,
          F1Contents,  F1xy, F1 , DrawOrder00,  StdDrawRoomObjects,
  SeeThroughWalls, JumpZ, 16,
    Literal, 1,                           // Source
    StdBitmapViewport,                    // Destination
    StdSeeThruWallsRect2,                 // Destination Rectangle
    Zero,                                 // Source X
    Zero,                                 // Source Y
    Literal, 48,                          // Source Width
    Literal, 112,                         // Destination Width
    Literal, 9,                           // Transparent color
        StdBltFromDerivedGraphic,
    Literal, 1, StdDeleteDerivedGraphic,
  Return
};

ui16 StdDrawF1R1Stone[] = 
{
  StdWallBitmapF1R1,   StdWallRectangleF1R1,   StdBltShape,       
  F1R1,  wallDecorationLeft,  WallF1R1Left,  StdDrawWallText,
  F1R1,  wallDecorationLeft,  WallF1R1Left,  StdDrawWallDecoration,
  Return
};

ui16 StdDrawF0L1Stone[] = 
{
  StdWallBitmapF0L1,   StdWallRectangleF0L1,   StdBltShape,       
  Return
};

ui16 StdDrawF0Stone[] = 
{
  F0, F0xy, StdDrawCeilingPit,
  F0Contents,  F0xy, F0 , DrawOrder21,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF0R1Stone[] = 
{
  StdWallBitmapF0R1,   StdWallRectangleF0R1,   StdBltShape,       
  Return
};

// ****************************************************************
//              roomOPEN
// ****************************************************************
ui16 StdDrawF3L1Open[] =
{
  F3L1, FloorDecorationGraphicOrdinalF3L1, StdDrawFloorDecoration,
  F3L1Contents,  F3L1xy, F3L1 , DrawOrder3421,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF3Open[] =
{
  F3, FloorDecorationGraphicOrdinalF3, StdDrawFloorDecoration,
  F3Contents,  F3xy, F3 , DrawOrder3421,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF3R1Open[] =
{
  F3R1, FloorDecorationGraphicOrdinalF3R1, StdDrawFloorDecoration,
  F3R1Contents,  F3R1xy, F3R1 , DrawOrder4312,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF2L1Open[] =
{
  F2L1, FloorDecorationGraphicOrdinalF2L1, StdDrawFloorDecoration,
  F2L1, F2L1xy, StdDrawCeilingPit,
  F2L1Contents,  F2L1xy, F2L1 , DrawOrder3421,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF2Open[] =
{
  F2, FloorDecorationGraphicOrdinalF2, StdDrawFloorDecoration,
  F2, F2xy, StdDrawCeilingPit,
  F2Contents,  F2xy, F2 , DrawOrder3421,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF2R1Open[] =
{
  F2R1, FloorDecorationGraphicOrdinalF2R1, StdDrawFloorDecoration,
  F2R1, F2R1xy, StdDrawCeilingPit,
  F2R1Contents,  F2R1xy, F2R1 , DrawOrder4312,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF1L1Open[] =
{
  F1L1, FloorDecorationGraphicOrdinalF1L1, StdDrawFloorDecoration,
  F1L1, F1L1xy, StdDrawCeilingPit,
  F1L1Contents,  F1L1xy, F1L1 , DrawOrder32,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF1Open[] =
{
  F1, FloorDecorationGraphicOrdinalF1, StdDrawFloorDecoration,
  F1, F1xy, StdDrawCeilingPit,
  F1Contents,  F1xy, F1 , DrawOrder3421,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF1R1Open[] = 
{
    
  F1R1, FloorDecorationGraphicOrdinalF1R1, StdDrawFloorDecoration,
  F1R1, F1R1xy, StdDrawCeilingPit,
  F1R1Contents,  F1R1xy, F1R1 , DrawOrder41,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF0L1Open[] = 
{
  F0L1Contents,  F0L1xy, F0L1 , DrawOrder02,  StdDrawRoomObjects,
  F0L1, F0L1xy, StdDrawCeilingPit,
  Return
};

ui16 StdDrawF0Open[] = 
{
  F0, F0xy, StdDrawCeilingPit,
  F0Contents,  F0xy, F0 , DrawOrder21,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF0R1Open[] = 
{
  F0R1, F0R1xy, StdDrawCeilingPit,
  F0R1Contents,  F0R1xy, F0R1 , DrawOrder01,  StdDrawRoomObjects,
  Return
};

// ****************************************************************
//              roomPIT
// ****************************************************************
ui16 StdDrawF3L1Pit[] =
{
  F3L1, GetDecoration0, 
      JumpNZ, 4, 
        Literal, 49, StdFloorPitRectangleF3L1, StdGraphicToViewport, 
  F3L1, FloorDecorationGraphicOrdinalF3L1, StdDrawFloorDecoration,
  F3L1Contents,  F3L1xy, F3L1 , DrawOrder3421,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF3Pit[] =
{
  F3, GetDecoration0, 
      JumpNZ, 4, 
        Literal, 50, StdFloorPitRectangleF3, StdGraphicToViewport, 
  F3, FloorDecorationGraphicOrdinalF3, StdDrawFloorDecoration,
  F3Contents,  F3xy, F3 , DrawOrder3421,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF3R1Pit[] =
{
  F3R1, GetDecoration0, 
      JumpNZ, 4, 
        Literal, 49, StdFloorPitRectangleF3R1, StdMirrorGraphicToViewport, 
  F3R1, FloorDecorationGraphicOrdinalF3R1, StdDrawFloorDecoration,
  F3R1Contents,  F3R1xy, F3R1 , DrawOrder4312,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF2L1Pit[] =
{
  F2L1, GetDecoration0, 
      JumpZ, 4, 
        Literal, 57, 
      Jump, 2, 
        Literal, 51, 
      StdFloorPitRectangleF2L1, 
           StdGraphicToViewport, 
  F2L1, FloorDecorationGraphicOrdinalF2L1, StdDrawFloorDecoration,
  F2L1, F2L1xy, StdDrawCeilingPit,
  F2L1Contents,  F2L1xy, F2L1 , DrawOrder3421,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF2Pit[] =
{
  F2, GetDecoration0, 
      JumpZ, 4, 
        Literal, 58, 
      Jump, 2, 
        Literal, 52, 
      StdFloorPitRectangleF2, 
           StdGraphicToViewport, 
  F2, FloorDecorationGraphicOrdinalF2, StdDrawFloorDecoration,
  F2, F2xy, StdDrawCeilingPit,
  F2Contents,  F2xy, F2 , DrawOrder3421,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF2R1Pit[] =
{
  F2R1, GetDecoration0, 
      JumpZ, 4, 
        Literal, 57, 
      Jump, 2, 
        Literal, 51, 
      StdFloorPitRectangleF2R1, 
           StdMirrorGraphicToViewport, 
  F2R1, FloorDecorationGraphicOrdinalF2R1, StdDrawFloorDecoration,
  F2R1, F2R1xy, StdDrawCeilingPit,
  F2R1Contents,  F2R1xy, F2R1 , DrawOrder4312,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF1L1Pit[] =
{
  F1L1, GetDecoration0, 
      JumpZ, 4, 
        Literal, 59, 
      Jump, 2, 
        Literal, 53, 
      StdFloorPitRectangleF1L1, 
           StdGraphicToViewport, 
  F1L1, FloorDecorationGraphicOrdinalF1L1, StdDrawFloorDecoration,
  F1L1, F1L1xy, StdDrawCeilingPit,
  F1L1Contents,  F1L1xy, F1L1 , DrawOrder32,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF1Pit[] = 
{
  F1, GetDecoration0, 
      JumpZ, 4, 
        Literal, 60, 
      Jump, 2, 
        Literal, 54, 
      StdFloorPitRectangleF1, 
           StdGraphicToViewport, 
  F1, FloorDecorationGraphicOrdinalF1, StdDrawFloorDecoration,
  F1, F1xy, StdDrawCeilingPit,
  F1Contents,  F1xy, F1 , DrawOrder3421,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF1R1Pit[] = 
{
  F1R1, GetDecoration0, 
      JumpZ, 4, 
        Literal, 59, 
      Jump, 2, 
        Literal, 53, 
      StdFloorPitRectangleF1R1, 
           StdMirrorGraphicToViewport, 
  F1R1, FloorDecorationGraphicOrdinalF1R1, StdDrawFloorDecoration,
  F1R1, F1R1xy, StdDrawCeilingPit,
  F1R1Contents,  F1R1xy, F1R1 , DrawOrder41,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF0L1Pit[] = 
{
  F0L1, GetDecoration0, 
      JumpZ, 4, 
        Literal, 61, 
      Jump, 2, 
        Literal, 55, 
      StdFloorPitRectangleF0L1, 
           StdGraphicToViewport, 
  F0L1Contents,  F0L1xy, F0L1 , DrawOrder02,  StdDrawRoomObjects,
  F0L1, F0L1xy, StdDrawCeilingPit,
  Return
};

ui16 StdDrawF0Pit[] = 
{
  F0, GetDecoration0, 
      JumpZ, 4, 
        Literal, 62, 
      Jump, 2, 
        Literal, 56, 
      StdFloorPitRectangleF0, 
           StdGraphicToViewport, 
  F0, F0xy, StdDrawCeilingPit,
  F0Contents,  F0xy, F0 , DrawOrder21,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF0R1Pit[] = 
{
  F0R1, GetDecoration0, 
      JumpZ, 4, 
        Literal, 61, 
      Jump, 2, 
        Literal, 55, 
      StdFloorPitRectangleF0R1, 
           StdMirrorGraphicToViewport, 
  F0R1, F0R1xy, StdDrawCeilingPit,
  F0R1Contents,  F0R1xy, F0R1 , DrawOrder01,  StdDrawRoomObjects,
  Return
};

// ****************************************************************
//              roomTELEPORTER
// ****************************************************************
ui16 StdDrawF3L1Teleporter[] =
{
  F3L1, FloorDecorationGraphicOrdinalF3L1, StdDrawFloorDecoration,
  F3L1Contents,  F3L1xy, F3L1 , DrawOrder3421,  StdDrawRoomObjects,
  F3L1, GetDecoration0, JumpZ, 3, StdTeleporterRectangleF3L1, StdWallRectangleF3L1, StdDrawTeleporter,
  Return
};

ui16 StdDrawF3Teleporter[] =
{
  F3, FloorDecorationGraphicOrdinalF3, StdDrawFloorDecoration,
  F3Contents,  F3xy, F3 , DrawOrder3421,  StdDrawRoomObjects,
  F3, GetDecoration0, JumpZ, 3, StdTeleporterRectangleF3, StdWallRectangleF3, StdDrawTeleporter,
  Return
};

ui16 StdDrawF3R1Teleporter[] =
{
  F3R1, FloorDecorationGraphicOrdinalF3R1, StdDrawFloorDecoration,
  F3R1Contents,  F3R1xy, F3R1 , DrawOrder4312,  StdDrawRoomObjects,
  F3R1, GetDecoration0, JumpZ, 3, StdTeleporterRectangleF3R1, StdWallRectangleF3R1, StdDrawTeleporter,
  Return
};

ui16 StdDrawF2L1Teleporter[] =
{
  F2L1, FloorDecorationGraphicOrdinalF2L1, StdDrawFloorDecoration,
  F2L1, F2L1xy, StdDrawCeilingPit,
  F2L1Contents,  F2L1xy, F2L1 , DrawOrder3421,  StdDrawRoomObjects,
  F2L1, GetDecoration0, JumpZ, 3, StdTeleporterRectangleF2L1, StdWallRectangleF2L1, StdDrawTeleporter,
  Return
};

ui16 StdDrawF2Teleporter[] =
{
  F2, FloorDecorationGraphicOrdinalF2, StdDrawFloorDecoration,
  F2, F2xy, StdDrawCeilingPit,
  F2Contents,  F2xy, F2 , DrawOrder3421,  StdDrawRoomObjects,
  F2, GetDecoration0, JumpZ, 3, StdTeleporterRectangleF2, StdWallRectangleF2, StdDrawTeleporter,
  Return
};

ui16 StdDrawF2R1Teleporter[] =
{
  F2R1, FloorDecorationGraphicOrdinalF2R1, StdDrawFloorDecoration,
  F2R1, F2R1xy, StdDrawCeilingPit,
  F2R1Contents,  F2R1xy, F2R1 , DrawOrder4312,  StdDrawRoomObjects,
  F2R1, GetDecoration0, JumpZ, 3, StdTeleporterRectangleF2R1, StdWallRectangleF2R1, StdDrawTeleporter,
  Return
};

ui16 StdDrawF1L1Teleporter[] =
{
  F1L1, FloorDecorationGraphicOrdinalF1L1, StdDrawFloorDecoration,
  F1L1, F1L1xy, StdDrawCeilingPit,
  F1L1Contents,  F1L1xy, F1L1 , DrawOrder32,  StdDrawRoomObjects,
  F1L1, GetDecoration0, JumpZ, 3, StdTeleporterRectangleF1L1, StdWallRectangleF1L1, StdDrawTeleporter,
  Return
};

ui16 StdDrawF1R1Teleporter[] =
{
  F1R1, FloorDecorationGraphicOrdinalF1R1, StdDrawFloorDecoration,
  F1R1, F1R1xy, StdDrawCeilingPit,
  F1R1Contents,  F1R1xy, F1R1 , DrawOrder41,  StdDrawRoomObjects,
  F1R1, GetDecoration0, JumpZ, 3, StdTeleporterRectangleF1R1, StdWallRectangleF1R1, StdDrawTeleporter,
  Return
};

ui16 StdDrawF1Teleporter[] =
{
  F1, FloorDecorationGraphicOrdinalF1, StdDrawFloorDecoration,
  F1, F1xy, StdDrawCeilingPit,
  F1Contents,  F1xy, F1 , DrawOrder3421,  StdDrawRoomObjects,
  F1, GetDecoration0, JumpZ, 3, StdTeleporterRectangleF1, StdWallRectangleF1, StdDrawTeleporter,
  Return
};

ui16 StdDrawF0L1Teleporter[] =
{
  F0L1Contents,  F0L1xy, F0L1 , DrawOrder02,  StdDrawRoomObjects,
  F0L1, F0L1xy, StdDrawCeilingPit,
  F0L1, GetDecoration0, JumpZ, 3, StdTeleporterRectangleF0L1, StdWallRectangleF0L1, StdDrawTeleporter,
  Return
};

ui16 StdDrawF0Teleporter[] =
{
  F0, F0xy, StdDrawCeilingPit,
  F0Contents,  F0xy, F0 , DrawOrder21,  StdDrawRoomObjects,
  F0, GetDecoration0, JumpZ, 3, StdTeleporterRectangleF0, StdWallRectangleF0, StdDrawTeleporter,
  Return
};

ui16 StdDrawF0R1Teleporter[] =
{
  F0R1, F0R1xy, StdDrawCeilingPit,
  F0R1Contents,  F0R1xy, F0R1 , DrawOrder01,  StdDrawRoomObjects,
  F0R1, GetDecoration0, JumpZ, 3, StdTeleporterRectangleF0R1, StdWallRectangleF0R1, StdDrawTeleporter,
  Return
};

// ****************************************************************
//              roomSTAIRFACING
// ****************************************************************
ui16 StdDrawF3L1StairFacing[] =
{
  F3L1, GetDecoration0, 
    JumpZ, 5,
      StdStairFacingUpGraphicF3L1, StdStairFacingUpRectangleF3L1, StdGraphicToViewport,
    Jump, 3,
      StdStairFacingDownGraphicF3L1, StdStairFacingDownRectangleF3L1, StdGraphicToViewport,
  F3L1, FloorDecorationGraphicOrdinalF3L1, StdDrawFloorDecoration,
  F3L1Contents,  F3L1xy, F3L1, DrawOrder3421,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF3StairFacing[] =
{
  F3, GetDecoration0, 
    JumpZ, 5,
      StdStairFacingUpGraphicF3, StdStairFacingUpRectangleF3, StdGraphicToViewport,
    Jump, 3,
      StdStairFacingDownGraphicF3, StdStairFacingDownRectangleF3, StdGraphicToViewport,
  F3, FloorDecorationGraphicOrdinalF3, StdDrawFloorDecoration,
  F3Contents,  F3xy, F3, DrawOrder3421,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF3R1StairFacing[] =
{
  F3R1, GetDecoration0, 
    JumpZ, 5,
      StdStairFacingUpGraphicF3R1, StdStairFacingUpRectangleF3R1, StdMirrorGraphicToViewport,
    Jump, 3,
      StdStairFacingDownGraphicF3R1, StdStairFacingDownRectangleF3R1, StdMirrorGraphicToViewport,
  F3R1, FloorDecorationGraphicOrdinalF3R1, StdDrawFloorDecoration,
  F3R1Contents,  F3R1xy, F3R1, DrawOrder4312,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF2L1StairFacing[] =
{
  F2L1, GetDecoration0, 
    JumpZ, 5,
      StdStairFacingUpGraphicF2L1, StdStairFacingUpRectangleF2L1, StdGraphicToViewport,
    Jump, 3,
      StdStairFacingDownGraphicF2L1, StdStairFacingDownRectangleF2L1, StdGraphicToViewport,
  F2L1, FloorDecorationGraphicOrdinalF2L1, StdDrawFloorDecoration,
  F2L1, F2L1xy,  StdDrawCeilingPit,
  F2L1Contents,  F2L1xy, F2L1, DrawOrder3421,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF2StairFacing[] =
{
  F2, GetDecoration0, 
    JumpZ, 5,
      StdStairFacingUpGraphicF2, StdStairFacingUpRectangleF2, StdGraphicToViewport,
    Jump, 3,
      StdStairFacingDownGraphicF2, StdStairFacingDownRectangleF2, StdGraphicToViewport,
  F2, FloorDecorationGraphicOrdinalF2, StdDrawFloorDecoration,
  F2, F2xy,  StdDrawCeilingPit,
  F2Contents,  F2xy, F2, DrawOrder3421,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF2R1StairFacing[] =
{
  F2R1, GetDecoration0, 
    JumpZ, 5,
      StdStairFacingUpGraphicF2R1, StdStairFacingUpRectangleF2R1, StdMirrorGraphicToViewport,
    Jump, 3,
      StdStairFacingDownGraphicF2R1, StdStairFacingDownRectangleF2R1, StdMirrorGraphicToViewport,
  F2R1, FloorDecorationGraphicOrdinalF2R1, StdDrawFloorDecoration,
  F2R1, F2R1xy,  StdDrawCeilingPit,
  F2R1Contents,  F2R1xy, F2R1, DrawOrder4312,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF1L1StairFacing[] =
{
  F1L1, GetDecoration0, 
    JumpZ, 5,
      StdStairFacingUpGraphicF1L1, StdStairFacingUpRectangleF1L1, StdGraphicToViewport,
    Jump, 3,
      StdStairFacingDownGraphicF1L1, StdStairFacingDownRectangleF1L1, StdGraphicToViewport,
  F1L1, FloorDecorationGraphicOrdinalF1L1, StdDrawFloorDecoration,
  F1L1, F1L1xy,  StdDrawCeilingPit,
  F1L1Contents,  F1L1xy, F1L1, DrawOrder32,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF1StairFacing[] =
{
  F1, GetDecoration0, 
    JumpZ, 5,
      StdStairFacingUpGraphicF1, StdStairFacingUpRectangleF1, StdGraphicToViewport,
    Jump, 3,
      StdStairFacingDownGraphicF1, StdStairFacingDownRectangleF1, StdGraphicToViewport,
  F1, FloorDecorationGraphicOrdinalF1, StdDrawFloorDecoration,
  F1, F1xy,  StdDrawCeilingPit,
  F1Contents,  F1xy, F1, DrawOrder3421,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF1R1StairFacing[] =
{
  F1R1, GetDecoration0, 
    JumpZ, 5,
      StdStairFacingUpGraphicF1R1, StdStairFacingUpRectangleF1R1, StdMirrorGraphicToViewport,
    Jump, 3,
      StdStairFacingDownGraphicF1R1, StdStairFacingDownRectangleF1R1, StdMirrorGraphicToViewport,
  F1R1, FloorDecorationGraphicOrdinalF1R1, StdDrawFloorDecoration,
  F1R1, F1R1xy,  StdDrawCeilingPit,
  F1R1Contents,  F1R1xy, F1R1, DrawOrder41,  StdDrawRoomObjects,
 Return
};

ui16 StdDrawF0L1StairFacing[] =
{
  Return
};

ui16 StdDrawF0StairFacing[] =
{
  F0, GetDecoration0, 
    JumpZ, 8,
      StdStairFacingUpGraphicF0, StdStairFacingUpRectangleF0L, StdGraphicToViewport,
      StdStairFacingUpGraphicF0, StdStairFacingUpRectangleF0R, StdMirrorGraphicToViewport,
    Jump, 6,
      StdStairFacingDownGraphicF0, StdStairFacingDownRectangleF0L, StdGraphicToViewport,
      StdStairFacingDownGraphicF0, StdStairFacingDownRectangleF0R, StdMirrorGraphicToViewport,
  F0, F0xy,  StdDrawCeilingPit,
  F0Contents,  F0xy, F0, DrawOrder21,  StdDrawRoomObjects,
  Return
};

ui16 StdDrawF0R1StairFacing[] =
{
  Return
};

// ****************************************************************
//              roomSTAIREDGE
// ****************************************************************
ui16 StdDrawF3L1StairEdge[] =
{
  F3L1, FloorDecorationGraphicOrdinalF3L1, StdDrawFloorDecoration,
  F3L1Contents,  F3L1xy, F3L1, DrawOrder321,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF3StairEdge[] =
{
  Return
};

ui16 StdDrawF3R1StairEdge[] =
{
  F3R1, FloorDecorationGraphicOrdinalF3R1, StdDrawFloorDecoration,
  F3R1Contents,  F3R1xy, F3R1, DrawOrder412,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF2L1StairEdge[] =
{
  StdStairEdgeGraphicF2, StdStairEdgeRectF2L1, StdGraphicToViewport,
  F2L1, FloorDecorationGraphicOrdinalF2L1, StdDrawFloorDecoration,
  F2L1, F2L1xy,  StdDrawCeilingPit,
  F2L1Contents,  F2L1xy, F2L1, DrawOrder342,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF2StairEdge[] =
{
  Return
};

ui16 StdDrawF2R1StairEdge[] =
{
  StdStairEdgeGraphicF2, StdStairEdgeRectF2R1, StdMirrorGraphicToViewport,
  F2R1, FloorDecorationGraphicOrdinalF2R1, StdDrawFloorDecoration,
  F2R1, F2R1xy,  StdDrawCeilingPit,
  F2R1Contents,  F2R1xy, F2R1, DrawOrder431,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF1L1StairEdge[] =
{
  F1L1, GetDecoration0, 
    JumpZ, 5,
      StdStairEdgeGraphicUpF1, StdStairEdgeUpRectF1L1, StdGraphicToViewport,
    Jump, 3,
      StdStairEdgeGraphicDownF1, StdStairEdgeDownRectF1L1, StdGraphicToViewport,
  F1L1, FloorDecorationGraphicOrdinalF1L1, StdDrawFloorDecoration,
  F1L1, F1L1xy,  StdDrawCeilingPit,
  F1L1Contents,  F1L1xy, F1L1, DrawOrder32,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF1StairEdge[] =
{
  Return
};

ui16 StdDrawF1R1StairEdge[] =
{
  F1R1, GetDecoration0, 
    JumpZ, 5,
      StdStairEdgeGraphicUpF1, StdStairEdgeUpRectF1R1, StdMirrorGraphicToViewport,
    Jump, 3,
      StdStairEdgeGraphicDownF1, StdStairEdgeDownRectF1R1, StdMirrorGraphicToViewport,
  F1R1, FloorDecorationGraphicOrdinalF1R1, StdDrawFloorDecoration,
  F1R1, F1R1xy,  StdDrawCeilingPit,
  F1R1Contents,  F1R1xy, F1R1, DrawOrder41,  StdDrawRoomObjects,      
  Return    
};

ui16 StdDrawF0L1StairEdge[] =
{
  StdStairEdgeGraphicF0L1, StdStairEdgeRectF0L1, StdGraphicToViewport,
  Return
};


ui16 StdDrawF0StairEdge[] =
{
  F0, F0xy,  StdDrawCeilingPit,
  F0Contents,  F0xy, F0, DrawOrder21,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF0R1StairEdge[] =
{
  StdStairEdgeGraphicF0R1, StdStairEdgeRectF0R1, StdMirrorGraphicToViewport,
  Return
};

// ****************************************************************
//              roomDOOREDGE
// ****************************************************************
ui16 StdDrawF3L1DoorEdge[] =
{
  F3L1, FloorDecorationGraphicOrdinalF3L1, StdDrawFloorDecoration,
  F3L1Contents,  F3L1xy, F3L1, DrawOrder321,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF3DoorEdge[] =
{
  Return
};

ui16 StdDrawF3R1DoorEdge[] =
{
  F3R1, FloorDecorationGraphicOrdinalF3R1, StdDrawFloorDecoration,
  F3R1Contents,  F3R1xy, F3R1, DrawOrder412,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF2L1DoorEdge[] =
{
  F2L1, FloorDecorationGraphicOrdinalF2L1, StdDrawFloorDecoration,
  F2L1, F2L1xy,  StdDrawCeilingPit,
  F2L1Contents,  F2L1xy, F2L1, DrawOrder342,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF2DoorEdge[] =
{
  Return
};

ui16 StdDrawF2R1DoorEdge[] =
{
  F2R1, FloorDecorationGraphicOrdinalF2R1, StdDrawFloorDecoration,
  F2R1, F2R1xy,  StdDrawCeilingPit,
  F2R1Contents,  F2R1xy, F2R1, DrawOrder431,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF1L1DoorEdge[] =
{
  F1L1, FloorDecorationGraphicOrdinalF1L1, StdDrawFloorDecoration,
  F1L1, F1L1xy,  StdDrawCeilingPit,
  F1L1Contents,  F1L1xy, F1L1, DrawOrder32,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF1DoorEdge[] =
{
  Return
};

ui16 StdDrawF1R1DoorEdge[] =
{
  F1R1, FloorDecorationGraphicOrdinalF1R1, StdDrawFloorDecoration,
  F1R1, F1R1xy,  StdDrawCeilingPit,
  F1R1Contents,  F1R1xy, F1R1, DrawOrder41,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF0L1DoorEdge[] =
{
  F0L1, F0L1xy,  StdDrawCeilingPit,
  F0L1, F0L1xy,  StdDrawCeilingPit,
  Return
};

ui16 StdDrawF0DoorEdge[] =
{
  SeeThroughWalls, 
    JumpZ, 3,
      StdDrawSeeThruDoorEdge, 
    Jump, 3,
      StdDoorEdgeFrameBitmapF0, StdDoorEdgeFrameRectF0, StdBltShapeToViewport,    
  F0, F0xy,  StdDrawCeilingPit,
  F0Contents,  F0xy, F0, DrawOrder21,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF0R1DoorEdge[] =
{
  F0R1, F0R1xy,  StdDrawCeilingPit,
  F0R1, F0R1xy,  StdDrawCeilingPit,
  Return
};

// ****************************************************************
//              roomDOORFACING
// ****************************************************************
ui16 StdDrawF3L1DoorFacing[] =
{
  F3L1, FloorDecorationGraphicOrdinalF3L1, StdDrawFloorDecoration,
  F3L1Contents,  F3L1xy, F3L1, DrawOrder218,  StdDrawRoomObjects,      
  StdDoorFacingFrameLeftBitmapF3L1, StdDoorFacingFrameRectF3L1, StdBltShapeToViewport,
  F3L1DoorRecordIndex, F3L1DoorState, StdDoorGraphicsF3, StdDoorRectsF3L1,
          StdDrawDoor,
  F3L1Contents,  F3L1xy, F3L1, DrawOrder349,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF3DoorFacing[] =
{
  F3, FloorDecorationGraphicOrdinalF3, StdDrawFloorDecoration,
  F3Contents,  F3xy, F3, DrawOrder218,  StdDrawRoomObjects,      
  StdDoorFacingFrameLeftBitmapF3, StdDoorFacingFrameLeftRectF3, StdBltShapeToViewport,
  StdDoorFacingFrameLeftBitmapF3, StdDoorFacingFrameRightRectF3, StdMirrorShapeToViewport,
  F3, DoorSwitch,
      JumpZ, 5,
          Literal, 1, Literal, 1, StdDrawDoorSwitch,
  F3DoorRecordIndex, F3DoorState, StdDoorGraphicsF3, StdDoorRectsF3,
          StdDrawDoor,
  F3Contents,  F3xy, F3, DrawOrder349,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF3R1DoorFacing[] =
{
  F3R1, FloorDecorationGraphicOrdinalF3R1, StdDrawFloorDecoration,
  F3R1Contents,  F3R1xy, F3R1, DrawOrder128,  StdDrawRoomObjects,      
  StdDoorFacingFrameLeftBitmapF3R1, StdDoorFacingFrameRectF3R1, StdMirrorShapeToViewport,
  F3R1, DoorSwitch,
      JumpZ, 5,
          Literal, 1, Literal, 0, StdDrawDoorSwitch,
  F3R1DoorRecordIndex, F3R1DoorState, StdDoorGraphicsF3, StdDoorRectsF3R1,
          StdDrawDoor,
  F3R1Contents,  F3R1xy, F3R1, DrawOrder439,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF2L1DoorFacing[] =
{
  F2L1, FloorDecorationGraphicOrdinalF2L1, StdDrawFloorDecoration,
  F2L1Contents,  F2L1xy, F2L1, DrawOrder218,  StdDrawRoomObjects,      
  StdDoorFacingTopTrackBitmapF2, StdDoorFacingTopTrackRectF2L1, StdBltShapeToViewport,
  F2L1DoorRecordIndex, F2L1DoorState, StdDoorGraphicsF2, StdDoorRectsF2L1,
          StdDrawDoor,
  F2L1, F2L1xy,  StdDrawCeilingPit,
  F2L1Contents,  F2L1xy, F2L1, DrawOrder349,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF2DoorFacing[] =
{
  F2, FloorDecorationGraphicOrdinalF2, StdDrawFloorDecoration,
  F2Contents,  F2xy, F2, DrawOrder218,  StdDrawRoomObjects,      
  StdDoorFacingTopTrackBitmapF2, StdDoorFacingTopTrackRectF2, StdBltShapeToViewport,
  StdDoorFacingFrameLeftBitmapF2, StdDoorFacingFrameLeftRectF2, StdBltShapeToViewport,
  StdDoorFacingFrameLeftBitmapF2, StdDoorFacingFrameRightRectF2, StdMirrorShapeToViewport,
  F2, DoorSwitch,
      JumpZ, 5,
          Literal, 1, Literal, 2, StdDrawDoorSwitch,
  F2DoorRecordIndex, F2DoorState, StdDoorGraphicsF2, StdDoorRectsF2,
          StdDrawDoor,
  F2Contents,  F2xy, F2, DrawOrder349,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF2R1DoorFacing[] =
{
  F2R1, FloorDecorationGraphicOrdinalF2R1, StdDrawFloorDecoration,
  F2R1Contents,  F2R1xy, F2R1, DrawOrder128,  StdDrawRoomObjects,      
  StdDoorFacingTopTrackBitmapF2, StdDoorFacingTopTrackRectF2R1, StdBltShapeToViewport,
  F2R1DoorRecordIndex, F2R1DoorState, StdDoorGraphicsF2, StdDoorRectsF2R1,
          StdDrawDoor,
  F2R1Contents,  F2R1xy, F2R1, DrawOrder439,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF1L1DoorFacing[] =
{
  F1L1, FloorDecorationGraphicOrdinalF1L1, StdDrawFloorDecoration,
  F1L1Contents,  F1L1xy, F1L1, DrawOrder28,  StdDrawRoomObjects,      
  StdDoorFacingTopTrackBitmapF1, StdDoorFacingTopTrackRectF1L1, StdBltShapeToViewport,
  F1L1DoorRecordIndex, F1L1DoorState, StdDoorGraphicsF1, StdDoorRectsF1L1,
          StdDrawDoor,
  F1L1Contents,  F1L1xy, F1L1, DrawOrder39,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF1DoorFacing[] =
{
  F1, FloorDecorationGraphicOrdinalF1, StdDrawFloorDecoration,
  F1Contents,  F1xy, F1, DrawOrder218,  StdDrawRoomObjects,      
  StdDoorFacingTopTrackBitmapF1, StdDoorFacingTopTrackRectF1, StdBltShapeToViewport,
  StdDoorFacingFrameLeftBitmapF1, StdDoorFacingFrameLeftRectF1, StdBltShapeToViewport,
  StdDoorFacingFrameRightBitmapF1, StdDoorFacingFrameRightRectF1, StdBltShapeToViewport,
  F1, DoorSwitch,
      JumpZ, 5,
          Literal, 1, Literal, 3, StdDrawDoorSwitch,
  F1DoorRecordIndex, F1DoorState, StdDoorGraphicsF1, StdDoorRectsF1,
          StdDrawDoor,
  F1Contents,  F1xy, F1, DrawOrder349,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF1R1DoorFacing[] =
{
  F1R1, FloorDecorationGraphicOrdinalF1R1, StdDrawFloorDecoration,
  F1R1Contents,  F1R1xy, F1R1, DrawOrder18,  StdDrawRoomObjects,      
  StdDoorFacingTopTrackBitmapF1, StdDoorFacingTopTrackRectF1R1, StdBltShapeToViewport,
  F1R1DoorRecordIndex, F1R1DoorState, StdDoorGraphicsF1, StdDoorRectsF1R1,
          StdDrawDoor,
  F1R1Contents,  F1R1xy, F1R1, DrawOrder49,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF0L1DoorFacing[] =
{
  Return
};

ui16 StdDrawF0DoorFacing[] =
{
  F0, F0xy,  StdDrawCeilingPit,
  F0Contents,  F0xy, F0, DrawOrder21,  StdDrawRoomObjects,      
  Return
};

ui16 StdDrawF0R1DoorFacing[] =
{
  Return
};

// ****************************************************************
//              End of drawing code
// ****************************************************************


ui16 *pStdDrawCode[21][8] = 
{
  { // F4L2
    NOP,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP
  },

  { // F4R2
    NOP,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP
  },


  { //F3L2
    StdDrawF3L2Stone,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP
  },


  { //F3R2
    StdDrawF3R2Stone,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP
  },

  { //F4L1
    StdDrawF4L1Any,
    StdDrawF4L1Any,
    StdDrawF4L1Any,
    StdDrawF4L1Any,
    StdDrawF4L1Any,
    StdDrawF4L1Any,
    StdDrawF4L1Any,
    StdDrawF4L1Any
  },


  { //F4R1
    StdDrawF4R1Any,
    StdDrawF4R1Any,
    StdDrawF4R1Any,
    StdDrawF4R1Any,
    StdDrawF4R1Any,
    StdDrawF4R1Any,
    StdDrawF4R1Any,
    StdDrawF4R1Any
  },

  { //F4
    StdDrawF4Any,
    StdDrawF4Any,   
    StdDrawF4Any,
    StdDrawF4Any,
    StdDrawF4Any,
    StdDrawF4Any,
    StdDrawF4Any,
    StdDrawF4Any
  },

  { //F3L1
    StdDrawF3L1Stone,
    StdDrawF3L1Open,
    StdDrawF3L1Pit,
    StdDrawF3L1Teleporter,
    StdDrawF3L1DoorEdge,
    StdDrawF3L1DoorFacing,
    StdDrawF3L1StairEdge,
    StdDrawF3L1StairFacing
  }, 


  { //F3R1
    StdDrawF3R1Stone,
    StdDrawF3R1Open,
    StdDrawF3R1Pit,
    StdDrawF3R1Teleporter,
    StdDrawF3R1DoorEdge,
    StdDrawF3R1DoorFacing,
    StdDrawF3R1StairEdge,
    StdDrawF3R1StairFacing
  }, 

  { //F3
    StdDrawF3Stone,
    StdDrawF3Open,
    StdDrawF3Pit,
    StdDrawF3Teleporter,
    StdDrawF3DoorEdge,
    StdDrawF3DoorFacing,
    StdDrawF3StairEdge,
    StdDrawF3StairFacing
  },



  { //F2L2
    NOP,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP
  },


  { //F2R2
    NOP,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP,
    NOP
  },

  { //F2L1
    StdDrawF2L1Stone,
    StdDrawF2L1Open,
    StdDrawF2L1Pit,
    StdDrawF2L1Teleporter,
    StdDrawF2L1DoorEdge,
    StdDrawF2L1DoorFacing,
    StdDrawF2L1StairEdge,
    StdDrawF2L1StairFacing
  }, 

  { //F2R1
    StdDrawF2R1Stone,
    StdDrawF2R1Open,
    StdDrawF2R1Pit,
    StdDrawF2R1Teleporter,
    StdDrawF2R1DoorEdge,
    StdDrawF2R1DoorFacing,
    StdDrawF2R1StairEdge,
    StdDrawF2R1StairFacing
  },

  { //F2
    StdDrawF2Stone,
    StdDrawF2Open,
    StdDrawF2Pit,
    StdDrawF2Teleporter,
    StdDrawF2DoorEdge,
    StdDrawF2DoorFacing,
    StdDrawF2StairEdge,
    StdDrawF2StairFacing
  },



  { //F1L1
    StdDrawF1L1Stone,
    StdDrawF1L1Open,
    StdDrawF1L1Pit,
    StdDrawF1L1Teleporter,
    StdDrawF1L1DoorEdge,
    StdDrawF1L1DoorFacing,
    StdDrawF1L1StairEdge,
    StdDrawF1L1StairFacing
  },


  { //F1R1
    StdDrawF1R1Stone,
    StdDrawF1R1Open,
    StdDrawF1R1Pit,
    StdDrawF1R1Teleporter,
    StdDrawF1R1DoorEdge,
    StdDrawF1R1DoorFacing,
    StdDrawF1R1StairEdge,
    StdDrawF1R1StairFacing
  },

  { //F1
    StdDrawF1Stone,
    StdDrawF1Open,
    StdDrawF1Pit,
    StdDrawF1Teleporter,
    StdDrawF1DoorEdge,
    StdDrawF1DoorFacing,
    StdDrawF1StairEdge,
    StdDrawF1StairFacing
  },




  { //F0L1
    StdDrawF0L1Stone,
    StdDrawF0L1Open,
    StdDrawF0L1Pit,
    StdDrawF0L1Teleporter,
    StdDrawF0L1DoorEdge,
    StdDrawF0L1DoorFacing,
    StdDrawF0L1StairEdge,
    StdDrawF0L1StairFacing
  },


  { //F0R1
    StdDrawF0R1Stone,
    StdDrawF0R1Open,
    StdDrawF0R1Pit,
    StdDrawF0R1Teleporter,
    StdDrawF0R1DoorEdge,
    StdDrawF0R1DoorFacing,
    StdDrawF0R1StairEdge,
    StdDrawF0R1StairFacing
  },


  { //F0
    StdDrawF0Stone,
    StdDrawF0Open,
    StdDrawF0Pit,
    StdDrawF0Teleporter,
    StdDrawF0DoorEdge,
    StdDrawF0DoorFacing,
    StdDrawF0StairEdge,
    StdDrawF0StairFacing},



};


void InterpretError(const char *msg)
{
  UI_MessageBox(msg, "Viewport Display Error", MESSAGE_OK);
}

void InterpretError(const char *msg, int param)
{
  char  m[1000];
  sprintf(m, msg, param);
  InterpretError(m);
}


static ui16 stack[256];
static ui16 rstack[256];

static ui8 sp = 0;
static ui8 rsp = 0;

i16 pop()
{
  return stack[(sp++) & 0xff];
}

void push(i32 v)
{
  stack[(--sp) & 0xff] = (i16)v;
}

void push(RN obj)
{
  stack[(--sp) & 0xff] = obj.ConvertToInteger();
}

i16 rpop()
{
  return rstack[(rsp++) & 0xff];
}

void rpush(i32 v)
{
  rstack[(--rsp) & 0xff] = (i16)v;
}


RN popRN()
{
  unsigned int n;
  RN result;
  n = stack[(sp++) & 0xff];
  if (n == 0xfffe) result = RNeof;
  else if (n == 0xffff) result = RNnul;
  else if (result.checkIndirectIndex(n)) result.ConstructFromInteger(n);
  return result;
}


ui8 *StdBitmapPointers(i32 n)
{
  BITMAP_POINTERS m;
  m = (BITMAP_POINTERS)n;
  switch (m)
  {
  case BMP_StdWallBitmapF3L2:            return d.pWallBitmaps[5];
  case BMP_StdWallBitmapF3L1:            return d.pWallBitmaps[4];
  case BMP_StdWallBitmapF3:              return d.pWallBitmaps[4];
  case BMP_StdWallBitmapF3R1:            return d.pWallBitmaps[4];
  case BMP_StdWallBitmapF3R2:            return d.pWallBitmaps[6];
  case BMP_StdWallBitmapF2L1:            return d.pWallBitmaps[3];
  case BMP_StdWallBitmapF2:              return d.pWallBitmaps[3];
  case BMP_StdWallBitmapF2R1:            return d.pWallBitmaps[3];
  case BMP_StdWallBitmapF1L1:            return d.pWallBitmaps[2];
  case BMP_StdWallBitmapF1:              return d.pWallBitmaps[2];
  case BMP_StdWallBitmapF1R1:            return d.pWallBitmaps[2];
  case BMP_StdWallBitmapF0L1:            return d.pWallBitmaps[1];
  case BMP_StdWallBitmapF0R1:            return d.pWallBitmaps[0];
    //
  case BMP_StdDoorFacingFrameLeftBitmapF3L1:   return d.pDoorBitmaps[5];
  case BMP_StdDoorFacingFrameLeftBitmapF3R1:   return d.pDoorBitmaps[5];
  case BMP_StdDoorFacingTopTrackBitmapF1:      return d.pDoorBitmaps[6];
  case BMP_StdDoorFacingTopTrackBitmapF2:      return d.pDoorBitmaps[7];
  case BMP_StdDoorEdgeFrameBitmapF0:           return d.pDoorBitmaps[1];
  case BMP_StdDoorFacingFrameLeftBitmapF3:     return d.pDoorBitmaps[4];
  case BMP_StdDoorFacingFrameLeftBitmapF2:     return d.pDoorBitmaps[3];
  case BMP_StdDoorFacingFrameLeftBitmapF1:     return d.pDoorBitmaps[2];
  case BMP_StdDoorFacingFrameRightBitmapF1:    return d.pDoorBitmaps[0];
    //
  case BMP_StdBitmapViewport:                  return d.pViewportBMP;
  case BMP_StdHoleInWallBitmap:                return GetBasicGraphicAddress(41);
  };
  InterpretError("Unknown Standard Bitmap Pointer\nIndex = %d",n);
  return d.pDoorBitmaps[0];
}

RectPos *StdRectanglePointers(i32 n)
{
  RECT_POINTERS r;
  r = (RECT_POINTERS)n;
  switch (r)
  {
  case RECT_StdWallRectangleF3L2:            return &d.wallRectangles[13];
  case RECT_StdWallRectangleF3L1:            return &d.wallRectangles[1];
  case RECT_StdWallRectangleF3:              return &d.wallRectangles[0];
  case RECT_StdWallRectangleF3R1:            return &d.wallRectangles[2];
  case RECT_StdWallRectangleF3R2:            return &d.wallRectangles[12];
  case RECT_StdWallRectangleF2L1:            return &d.wallRectangles[4];
  case RECT_StdWallRectangleF2:              return &d.wallRectangles[3];
  case RECT_StdWallRectangleF2R1:            return &d.wallRectangles[5];
  case RECT_StdWallRectangleF1L1:            return &d.wallRectangles[7];
  case RECT_StdWallRectangleF1:              return &d.wallRectangles[6];
  case RECT_StdWallRectangleF1R1:            return &d.wallRectangles[8];
  case RECT_StdWallRectangleF0L1:            return &d.wallRectangles[10];
  case RECT_StdWallRectangleF0:              return &d.wallRectangles[9];
  case RECT_StdWallRectangleF0R1:            return &d.wallRectangles[11];
    //
  case RECT_StdFloorPitRectangleF3L1:        return &d.FloorPitRect[11];
  case RECT_StdFloorPitRectangleF3:          return &d.FloorPitRect[10];
  case RECT_StdFloorPitRectangleF3R1:        return &d.FloorPitRect[9];
  case RECT_StdFloorPitRectangleF2L1:        return &d.FloorPitRect[8];
  case RECT_StdFloorPitRectangleF2:          return &d.FloorPitRect[7];
  case RECT_StdFloorPitRectangleF2R1:        return &d.FloorPitRect[6];
  case RECT_StdFloorPitRectangleF1L1:        return &d.FloorPitRect[5];
  case RECT_StdFloorPitRectangleF1:          return &d.FloorPitRect[4];
  case RECT_StdFloorPitRectangleF1R1:        return &d.FloorPitRect[3];
  case RECT_StdFloorPitRectangleF0L1:        return &d.FloorPitRect[2];
  case RECT_StdFloorPitRectangleF0:          return &d.FloorPitRect[1];
  case RECT_StdFloorPitRectangleF0R1:        return &d.FloorPitRect[0];
    //
  case RECT_StdTeleporterRectangleF3L1:      return &d.teleporterRectangles[1];
  case RECT_StdTeleporterRectangleF3:        return &d.teleporterRectangles[0];
  case RECT_StdTeleporterRectangleF3R1:      return &d.teleporterRectangles[2];
  case RECT_StdTeleporterRectangleF2L1:      return &d.teleporterRectangles[4];
  case RECT_StdTeleporterRectangleF2:        return &d.teleporterRectangles[3];
  case RECT_StdTeleporterRectangleF2R1:      return &d.teleporterRectangles[5];
  case RECT_StdTeleporterRectangleF1L1:      return &d.teleporterRectangles[7];
  case RECT_StdTeleporterRectangleF1:        return &d.teleporterRectangles[6];
  case RECT_StdTeleporterRectangleF1R1:      return &d.teleporterRectangles[8];
  case RECT_StdTeleporterRectangleF0L1:      return &d.teleporterRectangles[10];
  case RECT_StdTeleporterRectangleF0:        return &d.teleporterRectangles[9];
  case RECT_StdTeleporterRectangleF0R1:      return &d.teleporterRectangles[11];
    //
  case RECT_StdStairFacingDownRectangleF3L1: return &d.StairFacingDownRect[10];
  case RECT_StdStairFacingDownRectangleF3:   return &d.StairFacingDownRect[9];
  case RECT_StdStairFacingDownRectangleF3R1: return &d.StairFacingDownRect[8];
  case RECT_StdStairFacingDownRectangleF2L1: return &d.StairFacingDownRect[7];
  case RECT_StdStairFacingDownRectangleF2:   return &d.StairFacingDownRect[6];
  case RECT_StdStairFacingDownRectangleF2R1: return &d.StairFacingDownRect[5];
  case RECT_StdStairFacingDownRectangleF1L1: return &d.StairFacingDownRect[4];
  case RECT_StdStairFacingDownRectangleF1:   return &d.StairFacingDownRect[3];
  case RECT_StdStairFacingDownRectangleF1R1: return &d.StairFacingDownRect[2];
  case RECT_StdStairFacingDownRectangleF0L:  return &d.StairFacingDownRect[1];
  case RECT_StdStairFacingDownRectangleF0R:  return &d.StairFacingDownRect[0];
    //
  case RECT_StdStairFacingUpRectangleF3L1:   return &d.StairFacingUpRect[10];
  case RECT_StdStairFacingUpRectangleF3:     return &d.StairFacingUpRect[9];
  case RECT_StdStairFacingUpRectangleF3R1:   return &d.StairFacingUpRect[8];
  case RECT_StdStairFacingUpRectangleF2L1:   return &d.StairFacingUpRect[7];
  case RECT_StdStairFacingUpRectangleF2:     return &d.StairFacingUpRect[6];
  case RECT_StdStairFacingUpRectangleF2R1:   return &d.StairFacingUpRect[5];
  case RECT_StdStairFacingUpRectangleF1L1:   return &d.StairFacingUpRect[4];
  case RECT_StdStairFacingUpRectangleF1:     return &d.StairFacingUpRect[3];
  case RECT_StdStairFacingUpRectangleF1R1:   return &d.StairFacingUpRect[2];
  case RECT_StdStairFacingUpRectangleF0L:    return &d.StairFacingUpRect[1];
  case RECT_StdStairFacingUpRectangleF0R:    return &d.StairFacingUpRect[0];
    //
  case RECT_StdStairEdgeRectangleF2L1:     return &d.StairEdgeRect[7];
  case RECT_StdStairEdgeRectangleF2R1:     return &d.StairEdgeRect[6];
  case RECT_StdStairEdgeDownRectangleF1L1: return &d.StairEdgeRect[3];
  case RECT_StdStairEdgeUpRectangleF1L1:   return &d.StairEdgeRect[5];
  case RECT_StdStairEdgeDownRectangleF1R1: return &d.StairEdgeRect[2];
  case RECT_StdStairEdgeUpRectangleF1R1:   return &d.StairEdgeRect[4];
  case RECT_StdStairEdgeRectangleF0L:      return &d.StairEdgeRect[1];
  case RECT_StdStairEdgeRectangleF0R:      return &d.StairEdgeRect[0];
    //
  case RECT_StdDoorFacingTopTrackRectF2L1:   return &d.DoorTrackTopRect[5];
  case RECT_StdDoorFacingTopTrackRectF2:     return &d.DoorTrackTopRect[4];
  case RECT_StdDoorFacingTopTrackRectF2R1:   return &d.DoorTrackTopRect[3];
  case RECT_StdDoorFacingTopTrackRectF1L1:   return &d.DoorTrackTopRect[2];
  case RECT_StdDoorFacingTopTrackRectF1:     return &d.DoorTrackTopRect[1];
  case RECT_StdDoorFacingTopTrackRectF1R1:   return &d.DoorTrackTopRect[0];
  case RECT_StdDoorEdgeFrameRectF0:          return &d.DoorTrackTopRect[6];
    //
  case RECT_StdDoorFacingFrameRectF3L1:      return &d.DoorFrameRect[7];
  case RECT_StdDoorFacingFrameRectF3R1:      return &d.DoorFrameRect[6];
  case RECT_StdDoorFacingFrameRightRectF1:   return &d.DoorFrameRect[0];
  case RECT_StdDoorFacingFrameLeftRectF1:    return &d.DoorFrameRect[1];
  case RECT_StdDoorFacingFrameRightRectF2:   return &d.DoorFrameRect[2];
  case RECT_StdDoorFacingFrameLeftRectF2:    return &d.DoorFrameRect[3];
  case RECT_StdDoorFacingFrameRightRectF3:   return &d.DoorFrameRect[4];
  case RECT_StdDoorFacingFrameLeftRectF3:    return &d.DoorFrameRect[5];
    //
  case RECT_StdDoorRectsF3L1:                return d.DoorRectsF3L1;
  case RECT_StdDoorRectsF3:                  return d.DoorRectsF3;
  case RECT_StdDoorRectsF3R1:                return d.DoorRectsF3R1;
  case RECT_StdDoorRectsF2L1:                return d.DoorRectsF2L1;
  case RECT_StdDoorRectsF2:                  return d.DoorRectsF2;
  case RECT_StdDoorRectsF2R1:                return d.DoorRectsF2R1;
  case RECT_StdDoorRectsF1L1:                return d.DoorRectsF1L1;
  case RECT_StdDoorRectsF1:                  return d.DoorRectsF1;
  case RECT_StdDoorRectsF1R1:                return d.DoorRectsF1R1;
  //case RECT_StdDoorRectsF0:                  return d.DoorRectsF0;
  case RECT_StdSeeThruWallsRect:             return &d.SeeThruWallsRect;
  case RECT_StdSeeThruWallsRect2:            return (RectPos *)&d.SeeThruWallsRect.b.uByte4;
  };
  InterpretError("Unknown Standard Rectangle Pointer\nValue = %d",n);
  return &d.wallRectangles[0];
}


i8 floorDrawingLocations [21] = //Translate from relative cell numbers.
{
  0,0,0,0,0,0,0,0,2,1,0,0,3,5,4,6,8,7,0,0,9
  //1,0,2,4,3,5,7,6,8,9,10,11,12,13,14,15,16
};

i8 objectDrawingLocations [21] =
{
  0,0,0,0,-2,-1,-3,1,2,0,0,0,4,5,3,7,8,6,10,11,9
};


void Interpret(ui16 *code, 
               ui16 startAddr, 
               SUMMARIZEROOMDATA *roomData, 
               i32 cellNum,
               i32 facing
#ifdef _DEBUG
               ,bool /*debugging*/
#endif              
               )
{
  pnt      pBitmap;
  i16     *pDoorGraphics;
  RectPos *pRectangle;
  RectPos *pWallRectangle;
  RectPos *pTeleporterRectangle;
  i32      wallDecorationDrawLocation;
  i32      wallDecorationPos;
  i32      locationNumber, decorationNumber, decorationID;
  i32      floorDecorationGraphicOrdinal;
  i32      relativeCell;
  i32      drawOrder;
  i32      doorGraphicIndex;
  i32      maskNumber;
  i32      maskID;
  i32      graphicID, virtualGraphicID;
  BACKGROUND_MASK *pMask;
  ui32    *pGraphic;
  ui16    *pDecorationBitmap;
  ui16     temp;
  ui8     *pDecorationLocation;
  RN       contents;
  ui16    *P;
  ui16     raddr;
  i32      hasAlcove = 0;
  bool     removeMortarJoint = true;
  i32      x;
  i32      y;
  i32      n;
  i32      skinNum;
  i32      wallDecorationNum;
  i32      nearness;
  ui32     graphicSize;
  i32      doorState;
  i32      db0Index;
  i32      graphicNum;
  DB0     *pDB0;
  i32      P1;
  i32      P2;
  P = code + startAddr;
  rpush(0xffff);
  for (;;)
  {
    GTRACE_8("    P=%6d instr=%6d  sp=%3d TOS = %d %d %d %d %d\n", P-code, *P, (ui8)sp, 
                  stack[(ui8)(sp+0)],stack[(ui8)(sp+1)],stack[(ui8)(sp+2)],stack[(ui8)(sp+3)],stack[(ui8)(sp+4)])
    switch (*(P++))  // opcode
    {
    case F4L1:  push(RF4L1);  break;
    case F4:    push(RF4);    break;
    case F4R1:  push(RF4R1);  break;
    case F4L2:  push(RF4L2);  break;
    case F4R2:  push(RF4R2);  break;
    //case F3L2:  push(RF3L2);  break;
    case F3L1:  push(RF3L1);  break;
    case F3:    push(RF3);    break;
    case F3R1:  push(RF3R1);  break;
    //case F3R2:  push(RF3R2);  break;
    case F2L1:  push(RF2L1);  break;
    case F2:    push(RF2);    break;
    case F2R1:  push(RF2R1);  break;
    case F1L1:  push(RF1L1);  break;
    case F1:    push(RF1);    break;
    case F1R1:  push(RF1R1);  break;
    case F0L1:  push(RF0L1);  break;
    case F0:    push(RF0);    break;
    case F0R1:  push(RF0R1);  break;
      //
    case F4L1xy: push(roomData[RF4L1].x); push(roomData[RF4L1].y); break;
    case F4xy:   push(roomData[RF4  ].x); push(roomData[RF4  ].y); break;
    case F4R1xy: push(roomData[RF4R1].x); push(roomData[RF4R1].y); break;
    case F4L2xy: push(roomData[RF4L2].x); push(roomData[RF4L2].y); break;
    case F4R2xy: push(roomData[RF4R2].x); push(roomData[RF4R2].y); break;
    //case F3L2xy: push(roomData[RF3L2].x); push(roomData[RF3L2].y); break;
    case F3L1xy: push(roomData[RF3L1].x); push(roomData[RF3L1].y); break;
    case F3xy:   push(roomData[RF3  ].x); push(roomData[RF3  ].y); break;
    case F3R1xy: push(roomData[RF3R1].x); push(roomData[RF3R1].y); break;
    //case F3R2xy: push(roomData[RF3R2].x); push(roomData[RF3R2].y); break;
    case F2L1xy: push(roomData[RF2L1].x); push(roomData[RF2L1].y); break;
    case F2xy:   push(roomData[RF2  ].x); push(roomData[RF2  ].y); break;
    case F2R1xy: push(roomData[RF2R1].x); push(roomData[RF2R1].y); break;
    case F1L1xy: push(roomData[RF1L1].x); push(roomData[RF1L1].y); break;
    case F1xy:   push(roomData[RF1  ].x); push(roomData[RF1  ].y); break;
    case F1R1xy: push(roomData[RF1R1].x); push(roomData[RF1R1].y); break;
    case F0L1xy: push(roomData[RF0L1].x); push(roomData[RF0L1].y); break;
    case F0xy:   push(roomData[RF0  ].x); push(roomData[RF0  ].y); break;
    case F0R1xy: push(roomData[RF0R1].x); push(roomData[RF0R1].y); break;
      //
    case F4L1Contents: push(roomData[RF4L1].rn2); break;
    case F4Contents:   push(roomData[RF4  ].rn2); break;
    case F4R1Contents: push(roomData[RF4R1].rn2); break;
    //case F3L2Contents: push(roomData[RF3L2].rn2); break;
    case F3L1Contents: push(roomData[RF3L1].rn2); break;
    case F3Contents:   push(roomData[RF3  ].rn2); break;
    case F3R1Contents: push(roomData[RF3R1].rn2); break;
    //case F3R2Contents: push(roomData[RF3R2].rn2); break;
    case F2L1Contents: push(roomData[RF2L1].rn2); break;
    case F2Contents:   push(roomData[RF2  ].rn2); break;
    case F2R1Contents: push(roomData[RF2R1].rn2); break;
    case F1L1Contents: push(roomData[RF1L1].rn2); break;
    case F1Contents:   push(roomData[RF1  ].rn2); break;
    case F1R1Contents: push(roomData[RF1R1].rn2); break;
    case F0L1Contents: push(roomData[RF0L1].rn2); break;
    case F0Contents:   push(roomData[RF0  ].rn2); break;
    case F0R1Contents: push(roomData[RF0R1].rn2); break;
      //
    case F3L1DoorRecordIndex: push(roomData[RF3L1].decorations[1]); break;
    case F3DoorRecordIndex:   push(roomData[RF3  ].decorations[1]); break;
    case F3R1DoorRecordIndex: push(roomData[RF3R1].decorations[1]); break;
    case F2L1DoorRecordIndex: push(roomData[RF2L1].decorations[1]); break;
    case F2DoorRecordIndex:   push(roomData[RF2  ].decorations[1]); break;
    case F2R1DoorRecordIndex: push(roomData[RF2R1].decorations[1]); break;
    case F1L1DoorRecordIndex: push(roomData[RF1L1].decorations[1]); break;
    case F1DoorRecordIndex:   push(roomData[RF1  ].decorations[1]); break;
    case F1R1DoorRecordIndex: push(roomData[RF1R1].decorations[1]); break;
    case F0DoorRecordIndex:   push(roomData[RF0  ].decorations[1]); break;
      //
    case F3L1DoorState: push(roomData[RF3L1].decorations[0]); break;
    case F3DoorState:   push(roomData[RF3  ].decorations[0]); break;
    case F3R1DoorState: push(roomData[RF3R1].decorations[0]); break;
    case F2L1DoorState: push(roomData[RF2L1].decorations[0]); break;
    case F2DoorState:   push(roomData[RF2  ].decorations[0]); break;
    case F2R1DoorState: push(roomData[RF2R1].decorations[0]); break;
    case F1L1DoorState: push(roomData[RF1L1].decorations[0]); break;
    case F1DoorState:   push(roomData[RF1  ].decorations[0]); break;
    case F1R1DoorState: push(roomData[RF1R1].decorations[0]); break;
    //case F0R1DoorState: push(roomData[8].decorations[0]); break;
      //
    case StdDoorGraphicsF3:   push(2); break;
    case StdDoorGraphicsF2:   push(1); break;
    case StdDoorGraphicsF1:   push(0); break;
      //
    case StdDoorRectsF3L1: push(RECT_StdDoorRectsF3L1); break;
    case StdDoorRectsF3:   push(RECT_StdDoorRectsF3);   break;
    case StdDoorRectsF3R1: push(RECT_StdDoorRectsF3R1); break;
    case StdDoorRectsF2L1: push(RECT_StdDoorRectsF2L1); break;
    case StdDoorRectsF2:   push(RECT_StdDoorRectsF2);   break;
    case StdDoorRectsF2R1: push(RECT_StdDoorRectsF2R1); break;
    case StdDoorRectsF1L1: push(RECT_StdDoorRectsF1L1); break;
    case StdDoorRectsF1:   push(RECT_StdDoorRectsF1);   break;
    case StdDoorRectsF1R1: push(RECT_StdDoorRectsF1R1); break;
    case StdDoorRectsF0:   push(RECT_StdDoorRectsF0);   break;
      //
    case StdDrawSeeThruDoorEdge:
               MemMove(d.pDoorBitmaps[1], (ui8 *)tempBitmap, 1968);
               TAG0088b2(GetBasicGraphicAddress(41),
                  (ui8 *)tempBitmap,
                  (RectPos *)d.uByte2542,
                  //d.DoorTrackTopRect[0].b.x1-d.uByte2534[0], // This isn't right (srcOffsetX)
                  d.DoorTrackTopRect[0].b.x1-d.SeeThruWallsRect.b.uByte4, // This isn't right (srcOffsetX)
                  0,
                  48,
                  16,
                  9);
               BltShapeToViewport(tempBitmap, &d.DoorTrackTopRect[6]);
                break;
    case StdDrawDoor:
                pRectangle = StdRectanglePointers(pop());
                doorGraphicIndex = pop();
                pDoorGraphics = d.DoorGraphic[doorGraphicIndex];
                switch (doorGraphicIndex)
                {
                case 0: nearness=2; graphicSize=4224; break;
                case 1: nearness=1; graphicSize=1952; break;
                case 2: nearness=0; graphicSize =984; break;
                default:
                  InterpretError("Illegal door graphic index");
                  return;
                };
                doorState = pop();
                db0Index = pop();
                DrawDoor(db0Index, 
                         doorState,
                         pDoorGraphics,
                         graphicSize,
                         nearness,
                         pRectangle);
      //
    case DrawOrder00:     push(0x00);    break;
    case DrawOrder01:     push(0x01);    break;
    case DrawOrder02:     push(0x02);    break;
    case DrawOrder18:     push(0x18);    break;
    case DrawOrder21:     push(0x21);    break;
    case DrawOrder28:     push(0x28);    break;
    case DrawOrder32:     push(0x32);    break;
    case DrawOrder39:     push(0x39);    break;
    case DrawOrder41:     push(0x41);    break;
    case DrawOrder49:     push(0x49);    break;
    case DrawOrder3421:   push(0x3421);  break;
    case DrawOrder218:    push(0x218);   break;
    case DrawOrder321:    push(0x321);   break;
    case DrawOrder349:    push(0x349);   break;
    case DrawOrder412:    push(0x412);   break;
    case DrawOrder431:    push(0x431);   break;
    case DrawOrder342:    push(0x342);   break;
    case DrawOrder4312:   push(0x4312);  break;
    case DrawOrder128:    push(0x128);   break;
    case DrawOrder439:    push(0x439);   break;
      //
    case Alcove:          push(hasAlcove);          break;
    case HasDecoration:
      wallDecorationPos = pop();
      relativeCell      = pop();
      push(roomData[relativeCell].decorations[wallDecorationPos] != 0);
      break;
    case LeaveMortarJoint: removeMortarJoint = false; break;
    case SeeThroughWalls: push(d.SeeThruWalls?1:0); break;
      //
    case wallDecorationRight: push(0); break;
    case wallDecorationFront: push(1); break;
    case wallDecorationLeft:  push(2); break;
      //
    case WallF3L1Front:  push(RWF3L1Front);  break;
    case WallF3L1Right:  push(RWF3L1Right);  break;
    case WallF3Front:    push(RWF3Front);    break;
    case WallF3R1Left:   push(RWF3R1Left);   break;
    case WallF3R1Front:  push(RWF3R1Front);  break;
    case WallF2L1Front:  push(RWF2L1Front);  break;
    case WallF2L1Right:  push(RWF2L1Right);  break;
    case WallF2Front:    push(RWF2Front);    break;
    case WallF2R1Left:   push(RWF2R1Left);   break;
    case WallF2R1Front:  push(RWF2R1Front);  break;
    case WallF1L1Right:  push(RWF1L1Right);  break;
    case WallF1Front:    push(RWF1Front);    break;
    case WallF1R1Left:   push(RWF1R1Left);   break;
      //
    case StdWallBitmapF3L2: push(BMP_StdWallBitmapF3L2); break;
    case StdWallBitmapF3L1: push(BMP_StdWallBitmapF3L1); break;
    case StdWallBitmapF3:   push(BMP_StdWallBitmapF3);   break;
    case StdWallBitmapF3R1: push(BMP_StdWallBitmapF3R1); break;
    case StdWallBitmapF3R2: push(BMP_StdWallBitmapF3R2); break;
    case StdWallBitmapF2L1: push(BMP_StdWallBitmapF2L1); break;
    case StdWallBitmapF2:   push(BMP_StdWallBitmapF2);   break;
    case StdWallBitmapF2R1: push(BMP_StdWallBitmapF2R1); break;
    case StdWallBitmapF1L1: push(BMP_StdWallBitmapF1L1); break;
    case StdWallBitmapF1:   push(BMP_StdWallBitmapF1);   break;
    case StdWallBitmapF1R1: push(BMP_StdWallBitmapF1R1); break;
    case StdWallBitmapF0L1: push(BMP_StdWallBitmapF0L1); break;
    case StdWallBitmapF0R1: push(BMP_StdWallBitmapF0R1); break;
      //
    case StdWallRectangleF3L2:push(RECT_StdWallRectangleF3L2); break;
    case StdWallRectangleF3L1:push(RECT_StdWallRectangleF3L1); break;
    case StdWallRectangleF3:  push(RECT_StdWallRectangleF3);   break;
    case StdWallRectangleF3R1:push(RECT_StdWallRectangleF3R1); break;
    case StdWallRectangleF3R2:push(RECT_StdWallRectangleF3R2); break;
    case StdWallRectangleF2L1:push(RECT_StdWallRectangleF2L1); break;
    case StdWallRectangleF2:  push(RECT_StdWallRectangleF2);   break;
    case StdWallRectangleF2R1:push(RECT_StdWallRectangleF2R1); break;
    case StdWallRectangleF1L1:push(RECT_StdWallRectangleF1L1); break;
    case StdWallRectangleF1:  push(RECT_StdWallRectangleF1);   break;
    case StdWallRectangleF1R1:push(RECT_StdWallRectangleF1R1); break;
    case StdWallRectangleF0L1:push(RECT_StdWallRectangleF0L1); break;
    case StdWallRectangleF0:  push(RECT_StdWallRectangleF0);   break;
    case StdWallRectangleF0R1:push(RECT_StdWallRectangleF0R1); break;
      //
    case StdFloorPitRectangleF3L1: push(RECT_StdFloorPitRectangleF3L1); break;
    case StdFloorPitRectangleF3:   push(RECT_StdFloorPitRectangleF3);   break;
    case StdFloorPitRectangleF3R1: push(RECT_StdFloorPitRectangleF3R1); break;
    case StdFloorPitRectangleF2L1: push(RECT_StdFloorPitRectangleF2L1); break;
    case StdFloorPitRectangleF2:   push(RECT_StdFloorPitRectangleF2);   break;
    case StdFloorPitRectangleF2R1: push(RECT_StdFloorPitRectangleF2R1); break;
    case StdFloorPitRectangleF1L1: push(RECT_StdFloorPitRectangleF1L1); break;
    case StdFloorPitRectangleF1:   push(RECT_StdFloorPitRectangleF1);   break;
    case StdFloorPitRectangleF1R1: push(RECT_StdFloorPitRectangleF1R1); break;
    case StdFloorPitRectangleF0L1: push(RECT_StdFloorPitRectangleF0L1); break;
    case StdFloorPitRectangleF0:   push(RECT_StdFloorPitRectangleF0);   break;
    case StdFloorPitRectangleF0R1: push(RECT_StdFloorPitRectangleF0R1); break;
      //
    case StdTeleporterRectangleF3L1: push(RECT_StdTeleporterRectangleF3L1);  break;
    case StdTeleporterRectangleF3:   push(RECT_StdTeleporterRectangleF3);    break;
    case StdTeleporterRectangleF3R1: push(RECT_StdTeleporterRectangleF3R1);  break;
    case StdTeleporterRectangleF2L1: push(RECT_StdTeleporterRectangleF2L1);  break;
    case StdTeleporterRectangleF2:   push(RECT_StdTeleporterRectangleF2);    break;
    case StdTeleporterRectangleF2R1: push(RECT_StdTeleporterRectangleF2R1);  break;
    case StdTeleporterRectangleF1L1: push(RECT_StdTeleporterRectangleF1L1);  break;
    case StdTeleporterRectangleF1:   push(RECT_StdTeleporterRectangleF1);    break;
    case StdTeleporterRectangleF1R1: push(RECT_StdTeleporterRectangleF1R1);  break;
    case StdTeleporterRectangleF0L1: push(RECT_StdTeleporterRectangleF0L1);  break;
    case StdTeleporterRectangleF0:   push(RECT_StdTeleporterRectangleF0);    break;
    case StdTeleporterRectangleF0R1: push(RECT_StdTeleporterRectangleF0R1);  break;
      //
    case FloorDecorationGraphicOrdinalF4L1: push(roomData[RF4L1].decorations[2]); break;
    case FloorDecorationGraphicOrdinalF4:   push(roomData[RF4  ].decorations[2]); break;
    case FloorDecorationGraphicOrdinalF4R1: push(roomData[RF4R1].decorations[2]); break;
    case FloorDecorationGraphicOrdinalF3L1: push(roomData[RF3L1].decorations[2]); break;
    case FloorDecorationGraphicOrdinalF3:   push(roomData[RF3  ].decorations[2]); break;
    case FloorDecorationGraphicOrdinalF3R1: push(roomData[RF3R1].decorations[2]); break;
    case FloorDecorationGraphicOrdinalF2L1: push(roomData[RF2L1].decorations[2]); break;
    case FloorDecorationGraphicOrdinalF2:   push(roomData[RF2  ].decorations[2]); break;
    case FloorDecorationGraphicOrdinalF2R1: push(roomData[RF2R1].decorations[2]); break;
    case FloorDecorationGraphicOrdinalF1L1: push(roomData[RF1L1].decorations[2]); break;
    case FloorDecorationGraphicOrdinalF1:   push(roomData[RF1  ].decorations[2]); break;
    case FloorDecorationGraphicOrdinalF1R1: push(roomData[RF1R1].decorations[2]); break;
    case FloorDecorationGraphicOrdinalF0L1: push(roomData[RF0L1].decorations[2]); break;
    case FloorDecorationGraphicOrdinalF0:   push(roomData[RF0  ].decorations[2]); break;
    case FloorDecorationGraphicOrdinalF0R1: push(roomData[RF0R1].decorations[2]); break;
      //
    case StdStairFacingDownGraphicF3L1: push(d.StairFacingDownGraphic[6]); break;
    case StdStairFacingDownGraphicF3:   push(d.StairFacingDownGraphic[5]); break;
    case StdStairFacingDownGraphicF3R1: push(d.StairFacingDownGraphic[6]); break;
    case StdStairFacingDownGraphicF2L1: push(d.StairFacingDownGraphic[4]); break;
    case StdStairFacingDownGraphicF2:   push(d.StairFacingDownGraphic[3]); break;
    case StdStairFacingDownGraphicF2R1: push(d.StairFacingDownGraphic[4]); break;
    case StdStairFacingDownGraphicF1L1: push(d.StairFacingDownGraphic[2]); break;
    case StdStairFacingDownGraphicF1:   push(d.StairFacingDownGraphic[1]); break;
    case StdStairFacingDownGraphicF1R1: push(d.StairFacingDownGraphic[2]); break;
    case StdStairFacingDownGraphicF0:   push(d.StairFacingDownGraphic[0]); break;
      //
    case StdStairFacingUpGraphicF3L1:   push(d.StairFacingUpGraphic[6]);   break;
    case StdStairFacingUpGraphicF3:     push(d.StairFacingUpGraphic[5]);   break;
    case StdStairFacingUpGraphicF3R1:   push(d.StairFacingUpGraphic[6]);   break;
    case StdStairFacingUpGraphicF2L1:   push(d.StairFacingUpGraphic[4]);   break;
    case StdStairFacingUpGraphicF2:     push(d.StairFacingUpGraphic[3]);   break;
    case StdStairFacingUpGraphicF2R1:   push(d.StairFacingUpGraphic[4]);   break;
    case StdStairFacingUpGraphicF1L1:   push(d.StairFacingUpGraphic[2]);   break;
    case StdStairFacingUpGraphicF1:     push(d.StairFacingUpGraphic[1]);   break;
    case StdStairFacingUpGraphicF1R1:   push(d.StairFacingUpGraphic[2]);   break;
    case StdStairFacingUpGraphicF0:     push(d.StairFacingUpGraphic[0]);   break;
      //
    case StdStairFacingDownRectangleF3L1:
                push(RECT_StdStairFacingDownRectangleF3L1);
                break;
    case StdStairFacingDownRectangleF3:
                push(RECT_StdStairFacingDownRectangleF3);
                break;
    case StdStairFacingDownRectangleF3R1:
                push(RECT_StdStairFacingDownRectangleF3R1);
                break;
    case StdStairFacingDownRectangleF2L1:
                push(RECT_StdStairFacingDownRectangleF2L1);
                break;
    case StdStairFacingDownRectangleF2:
                push(RECT_StdStairFacingDownRectangleF2);
                break;
    case StdStairFacingDownRectangleF2R1:
                push(RECT_StdStairFacingDownRectangleF2R1);
                break;
    case StdStairFacingDownRectangleF1L1:
                push(RECT_StdStairFacingDownRectangleF1L1);
                break;
    case StdStairFacingDownRectangleF1:
                push(RECT_StdStairFacingDownRectangleF1);
                break;
    case StdStairFacingDownRectangleF1R1:
                push(RECT_StdStairFacingDownRectangleF1R1);
                break;
    case StdStairFacingDownRectangleF0L:
                push(RECT_StdStairFacingDownRectangleF0L);
                break;
    case StdStairFacingDownRectangleF0R:
                push(RECT_StdStairFacingDownRectangleF0R);
                break;
      //
    case StdStairFacingUpRectangleF3L1: push(RECT_StdStairFacingUpRectangleF3L1); break;
    case StdStairFacingUpRectangleF3:   push(RECT_StdStairFacingUpRectangleF3);   break;
    case StdStairFacingUpRectangleF3R1: push(RECT_StdStairFacingUpRectangleF3R1); break;
    case StdStairFacingUpRectangleF2L1: push(RECT_StdStairFacingUpRectangleF2L1); break;
    case StdStairFacingUpRectangleF2:   push(RECT_StdStairFacingUpRectangleF2);   break;
    case StdStairFacingUpRectangleF2R1: push(RECT_StdStairFacingUpRectangleF2R1); break;
    case StdStairFacingUpRectangleF1L1: push(RECT_StdStairFacingUpRectangleF1L1); break;
    case StdStairFacingUpRectangleF1:   push(RECT_StdStairFacingUpRectangleF1);   break;
    case StdStairFacingUpRectangleF1R1: push(RECT_StdStairFacingUpRectangleF1R1); break;
    case StdStairFacingUpRectangleF0L:  push(RECT_StdStairFacingUpRectangleF0L);  break;
    case StdStairFacingUpRectangleF0R:  push(RECT_StdStairFacingUpRectangleF0R);  break;
      //


    case StdStairEdgeGraphicF2:       push(d.StairEdgeGraphic[3]);  break;
    //case StdStairEdgeGraphicF2R1:     push(d.StairEdgeGraphic[3]);  break;
    case StdStairEdgeGraphicUpF1:     push(d.StairEdgeGraphic[2]);  break;
    //case StdStairEdgeGraphicUpF1R1:   push(d.StairEdgeGraphic[2]);  break;
    case StdStairEdgeGraphicDownF1:   push(d.StairEdgeGraphic[1]);  break;
    //case StdStairEdgeGraphicDownF1R1: push(d.StairEdgeGraphic[1]);  break;
    case StdStairEdgeGraphicF0L1:     push(d.StairEdgeGraphic[0]);  break;
    case StdStairEdgeGraphicF0R1:     push(d.StairEdgeGraphic[0]);  break;
      //
    case StdStairEdgeRectF2L1:      push(RECT_StdStairEdgeRectangleF2L1);   break;
    case StdStairEdgeRectF2R1:      push(RECT_StdStairEdgeRectangleF2R1);   break;
    case StdStairEdgeUpRectF1L1:    push(RECT_StdStairEdgeUpRectangleF1L1);   break;
    case StdStairEdgeUpRectF1R1:    push(RECT_StdStairEdgeUpRectangleF1R1);   break;
    case StdStairEdgeDownRectF1L1:  push(RECT_StdStairEdgeDownRectangleF1L1);   break;
    case StdStairEdgeDownRectF1R1:  push(RECT_StdStairEdgeDownRectangleF1R1); break;
    case StdStairEdgeRectF0L1:      push(RECT_StdStairEdgeRectangleF0L);   break;
    case StdStairEdgeRectF0R1:      push(RECT_StdStairEdgeRectangleF0R);   break;
      //
    case StdDoorFacingTopTrackBitmapF2: push(BMP_StdDoorFacingTopTrackBitmapF2);  break;
    case StdDoorFacingTopTrackBitmapF1: push(BMP_StdDoorFacingTopTrackBitmapF1);  break;
      //
    case StdDoorFacingTopTrackRectF2L1: push(RECT_StdDoorFacingTopTrackRectF2L1); break;
    case StdDoorFacingTopTrackRectF2:   push(RECT_StdDoorFacingTopTrackRectF2);   break;
    case StdDoorFacingTopTrackRectF2R1: push(RECT_StdDoorFacingTopTrackRectF2R1); break;
    case StdDoorFacingTopTrackRectF1L1: push(RECT_StdDoorFacingTopTrackRectF1L1); break;
    case StdDoorFacingTopTrackRectF1:   push(RECT_StdDoorFacingTopTrackRectF1);   break;
    case StdDoorFacingTopTrackRectF1R1: push(RECT_StdDoorFacingTopTrackRectF1R1); break;
      //
    case StdDoorEdgeFrameRectF0:        push(RECT_StdDoorEdgeFrameRectF0);        break;

      //
    case StdDoorFacingFrameLeftBitmapF3L1:push(BMP_StdDoorFacingFrameLeftBitmapF3L1);break;
    case StdDoorFacingFrameLeftBitmapF3R1:push(BMP_StdDoorFacingFrameLeftBitmapF3R1);break;
    case StdDoorFacingFrameLeftBitmapF1:  push(BMP_StdDoorFacingFrameLeftBitmapF1);  break;
    case StdDoorFacingFrameRightBitmapF1: push(BMP_StdDoorFacingFrameRightBitmapF1); break;
    case StdDoorFacingFrameLeftBitmapF2:  push(BMP_StdDoorFacingFrameLeftBitmapF2);  break;
    case StdDoorFacingFrameLeftBitmapF3:  push(BMP_StdDoorFacingFrameLeftBitmapF3);  break;
      //
    case StdDoorEdgeFrameBitmapF0:        push(BMP_StdDoorEdgeFrameBitmapF0);        break;
      //
    case StdDoorFacingFrameRectF3L1:   push(RECT_StdDoorFacingFrameRectF3L1);   break;
    case StdDoorFacingFrameRectF3R1:   push(RECT_StdDoorFacingFrameRectF3R1);   break;
    case StdDoorFacingFrameRightRectF1:push(RECT_StdDoorFacingFrameRightRectF1);break;
    case StdDoorFacingFrameLeftRectF1: push(RECT_StdDoorFacingFrameLeftRectF1); break;
    case StdDoorFacingFrameRightRectF2:push(RECT_StdDoorFacingFrameRightRectF2);break;
    case StdDoorFacingFrameLeftRectF2: push(RECT_StdDoorFacingFrameLeftRectF2); break;
    case StdDoorFacingFrameRightRectF3:push(RECT_StdDoorFacingFrameRightRectF3);break;
    case StdDoorFacingFrameLeftRectF3: push(RECT_StdDoorFacingFrameLeftRectF3); break;
      //
    case StdBltShapeToViewport:
            pRectangle = StdRectanglePointers(pop());
            pBitmap = (pnt)StdBitmapPointers(pop());
            BltShapeToViewport( (ui8 *)pBitmap, pRectangle);
            break;
    case StdMirrorShapeToViewport: 
      pRectangle = StdRectanglePointers(pop());
      pBitmap = (pnt)StdBitmapPointers(pop());
      MemMove((ui8 *)pBitmap,(ui8 *)tempBitmap,pRectangle->b.uByte4*pRectangle->b.uByte5);
      MirrorShapeBltToViewport((ui8 *)tempBitmap, pRectangle);
      break;
    case Literal:
      push(*(P++));
      break;
    case Jump:
      n = *(P++);
      P += (i16)n;
      break;
    case JumpZ:
      n = *(P++);
      if (pop() == 0) P += (i16)n;
      break;
    case JumpNZ:
      n = *(P++);
      if (pop() != 0) P += (i16)n;
      break;
    case GetDecoration0:
      relativeCell = pop();
      push(roomData[relativeCell].decorations[0]);
      break;
    case DoorSwitch:
      relativeCell = pop();
      //pDB0 = GetRecordAddressDB0(pRoomData->decorations[1]);
      //if (pDB0->doorSwitch()) DrawDoorSwitch(1, 0);
      pDB0 = GetRecordAddressDB0(roomData[relativeCell].decorations[1]);
      push(pDB0->doorSwitch()?1:0);
      break;
    case StdDrawDoorSwitch:
      P2 = pop();
      P1 = pop();
      DrawDoorSwitch(P1, P2);
      break;
    case StdDrawTeleporter:
      pWallRectangle = StdRectanglePointers(pop());
      pTeleporterRectangle = StdRectanglePointers(pop());
      DrawTeleporter(pTeleporterRectangle, pWallRectangle);
      break;
      //      DrawTeleporter(
      //           &d.teleporterRectangles[pRoomData->relativeCellNumber],
      //           &d.wallRectangles[pRoomData->relativeCellNumber]);
    case StdMirrorGraphicToViewport:
      pRectangle = StdRectanglePointers(pop());
      graphicNum = pop();
      MirrorGraphicToViewport(graphicNum,pRectangle);
      break;
    case StdGraphicToViewport:
      pRectangle = StdRectanglePointers(pop());
      graphicNum = pop();
      GraphicToViewport(graphicNum,pRectangle);
      break;
    case StdDrawRoomObjects: //(contents x y relativeCell drawOrder ... )
      drawOrder = pop();
      relativeCell = objectDrawingLocations[pop()];
      y = pop();
      x = pop();
      contents = popRN();
      DrawRoomObjects(contents, facing, x, y, relativeCell, (i16)drawOrder);
      break;
    case StdDrawCeilingPit: //( relativeCellToDraw PitX PitY ... )
      y = pop();
      x = pop();
      relativeCell = objectDrawingLocations[pop()];
      CheckCeilingPit(relativeCell, x, y);
      break;
    case StdDrawFloorDecoration: // ( relativeCell floorGraphicOrdinal ... )
      floorDecorationGraphicOrdinal = pop();
      relativeCell                  = pop();
      DrawFloorDecorations(
              floorDecorationGraphicOrdinal,
              floorDrawingLocations[relativeCell]);
      break;
    case StdDrawWallText: //(relativeCell wallDecorationPos wallDecorationDrawlocation ... )
      wallDecorationDrawLocation = pop();
      wallDecorationPos          = pop();
      relativeCell               = pop();
      DrawWallDecoration(
             d.NumWallDecoration+1,
             wallDecorationDrawLocation,
             roomData[relativeCell].text[wallDecorationPos],
             0,
             removeMortarJoint);
      removeMortarJoint = true;
      break;
    case StdDrawWallDecoration: //(RelativeCell decPos, walllocation ... )
      wallDecorationDrawLocation = pop();
      wallDecorationPos          = pop();
      relativeCell               = pop();
      hasAlcove = DrawWallDecoration(
                    roomData[relativeCell].decorations[wallDecorationPos],
                    wallDecorationDrawLocation,
                    RNeof,
                    roomData[relativeCell].championPortraitOrdinal);
      break;
    case StdBltShape: //(bitmap rectangle ... )
      pRectangle = StdRectanglePointers(pop());
      pBitmap    = (pnt)StdBitmapPointers(pop());
      BltShapeToViewport((ui8 *)pBitmap, pRectangle);
      break;
    case StdAllocateDerivedGraphic: AllocateDerivedGraphic(pop());     break;
    case StdBitmapViewport:         push(BMP_StdBitmapViewport);       break;
    case StdSeeThruWallsRect:       push(RECT_StdSeeThruWallsRect);    break;
    case StdSeeThruWallsRect2:      push(RECT_StdSeeThruWallsRect2);   break;
    case StdSeeThruWallsRectUByte4: push(d.SeeThruWallsRect.b.uByte4); break;
    case StdSeeThruWallsRectUByte6: push(d.SeeThruWallsRect.b.uByte6); break;
    case MinusOne:                  push(-1);                          break;
    case Zero:                      push(0);                           break;
    case StdBltToDerivedGraphic:
      {
        pnt srcBitmap, dstBitmap;
        RectPos *pDstRect;
        i32 srcX, srcY, srcWidth, dstWidth, xparentColor;
        //
        xparentColor = pop();
        dstWidth = pop();
        srcWidth = pop();
        srcY = pop();
        srcX = pop();
        pDstRect = StdRectanglePointers(pop());
        dstBitmap = (pnt)GetDerivedGraphicAddress(pop());
        srcBitmap = (pnt)StdBitmapPointers(pop());
        TAG0088b2(
          (ui8 *)srcBitmap,
          (ui8 *)dstBitmap,
          pDstRect,
          srcX,
          srcY,
          srcWidth,
          dstWidth,
          xparentColor);
      };
      break;
    case StdBltFromDerivedGraphic:
      {
        pnt srcBitmap, dstBitmap;
        RectPos *pDstRect;
        i32 srcX, srcY, srcWidth, dstWidth, xparentColor;
        //
        xparentColor = pop();
        dstWidth = pop();
        srcWidth = pop();
        srcY = pop();
        srcX = pop();
        pDstRect = StdRectanglePointers(pop());
        dstBitmap = (pnt)StdBitmapPointers(pop());
        srcBitmap = (pnt)GetDerivedGraphicAddress(pop());
        TAG0088b2(
          (ui8 *)srcBitmap,
          (ui8 *)dstBitmap,
          pDstRect,
          srcX,
          srcY,
          srcWidth,
          dstWidth,
          xparentColor);
      };
      break;
    case StdDeleteDerivedGraphic: DeleteGraphic(pop() | 0x8000);
    case StdHoleInWallBitmap: push(BMP_StdHoleInWallBitmap); break;
    case Return:
      raddr = rpop();
      if (raddr == 0xffff) return;
      P = code + raddr;
      break;
    case ToR:
      rpush(pop());
      break;
    case RTo:
      push(rpop());
      break;
    case CurrentSkin: push(roomData[cellNum].skinNumber); break;
    case SkinNumber:
      relativeCell = pop();
      push(roomData[relativeCell].skinNumber);
      break;
    case CurrentCell: push(roomData[cellNum].relativeCellNumber); break;
    case WallGraphicID: 
      push(backgroundLib.GetWallGraphicID(pop()));
      break;
    case FloorGraphicID: 
      push(backgroundLib.GetFloorGraphicID(pop()));
      break;
    case MiddleGraphicID: 
      push(backgroundLib.GetMiddleGraphicID(pop()));
      break;
    case CeilingGraphicID: 
      push(backgroundLib.GetCeilingGraphicID(pop()));
      break;
    case WallMaskID: 
      push(backgroundLib.GetWallMaskID(pop()));
      break;
    case FloorMaskID: 
      push(backgroundLib.GetFloorMaskID(pop()));
      break;
    case MiddleMaskID: 
      push(backgroundLib.GetMiddleMaskID(pop()));
      break;
    case CeilingMaskID: 
      push(backgroundLib.GetCeilingMaskID(pop()));
      break;
    case WallDecorationID:
      wallDecorationNum = pop();
      skinNum = pop();
      push(backgroundLib.GetWallDecorationID(skinNum, wallDecorationNum));
      break;
    case DrawCustomGraphic:
      maskNumber = pop();
      maskID = pop();
      graphicID = pop();
      //if (maskNumber == 19) break;
      //if (maskNumber == 13) break;
      //if (maskNumber == 8) break;
      //if (maskNumber == 16) break;
      pMask = backgroundLib.GetMask(maskID, maskNumber, 12);
      pGraphic = backgroundLib.GetBitmap(graphicID, 2, &graphicSize);
      ApplyBackground (pMask, pGraphic, graphicSize);
#ifdef _DEBUG
      GTRACE_3("   DrawCustomGraphic(%d, %d, %d)\n",graphicID, maskID, maskNumber);
      if (pMask != NULL)
      {
        GTRACE_4("    dstX=%d, dstY=%d, width=%d, height=%d\n",
                      pMask->dstX, pMask->dstY, pMask->width, pMask->height);
      };
#endif
      break;
    case CreateVirtualBitmap:
      {
        AFFINEMASK *pAffineMask;
        maskNumber = pop();
        maskID = pop();
        virtualGraphicID = pop();
        graphicID = pop();
        GTRACE_4("  CreateVirtualBitmap(%d, %d, %d, %d\n",
                         graphicID, virtualGraphicID, maskID, maskNumber);
        pGraphic = backgroundLib.GetBitmap(graphicID, 2, &graphicSize);
        if (pGraphic == NULL) break; // do nothing
        pAffineMask = (AFFINEMASK *)backgroundLib.GetMask(maskID, maskNumber, 48);
        if (pAffineMask == NULL) break; // do nothing
        backgroundLib.CreateVirtualBitmap(pGraphic, 
                                          graphicSize,      //i32 units
                                          virtualGraphicID, 
                                          pAffineMask);
      };
      break;
    case DrawCustomDecoration:
      locationNumber = pop();
      decorationNumber = pop();
      decorationID = pop();
      if ( backgroundLib.GetDecorationBitmap(
                            decorationID, 
                            decorationNumber, 
                            locationNumber,  
                            &pDecorationBitmap,
                            &pDecorationLocation,
                            &graphicSize) < 0) break;
      ApplyDecoration(pDecorationBitmap, pDecorationLocation, graphicSize);
      break;

    case  SWAP:
      temp = stack[sp]; 
      stack[sp] = stack[(ui8)(sp+1)];
      stack[(ui8)(sp+1)] = temp;
      break;
    case  DROP:
      sp = (ui8)(sp+1); 
      break;
    case  TwoDrop:
      sp = (ui8)(sp+2); 
      break;
    case  OVER:
      push(stack[(ui8)(sp+1)]); 
      break;
    case  ROT:        
      temp = stack[(ui8)(sp+2)];
      stack[(ui8)(sp+2)] = stack[(ui8)(sp+1)];
      stack[(ui8)(sp+1)] = stack[sp];
      stack[sp] = temp;
      break;
    case  DUP:        
      push(stack[(ui8)(sp+0)]);  
      break;
    case TwoDup:
      push(stack[(ui8)(sp+1)]);  
      push(stack[(ui8)(sp+1)]);  
      break;
    case  QuestionDUP:
      if (stack[sp] != 0) push(stack[sp]); 
      break;
    case  PICK:
      temp = stack[sp];
      stack[sp] = stack[(ui8)(sp+temp)];
      break;
    case  ROLL:
      n = pop() - 1;
      temp = stack[(ui8)(sp + n)];
      for (x = n; x > 0; x--) stack[(ui8)(sp + x)] = stack[(ui8)(sp + x -1)];
      stack[sp] = temp;
      break;
    case   PLUS:
      push(pop() + pop()); break;
    case   MINUS:
      n = pop(); push(pop() - n); break;
    case   GEAR:
      push(pop() * pop()); break;
    case   SLASH:
      n = pop(); push(pop() / n); break;
    case   EQUAL:
      push((pop() == pop()) ? 1 : 0); break;
    case   LESS:
      n = pop(); push((n<pop()) ? 1 : 0); break;
    case   GREATER:
      n = pop(); push((n>pop()) ? 1 : 0); break;
    case   ULESS:
      n = pop(); push(((unsigned)n<(unsigned)pop()) ? 1 : 0); break;
    case   ZEROEQUAL:
    case   NOT:
      push((pop()==0) ? 1 : 0); break;
    case   ZEROLESS:
      push((pop()<0) ? 1 : 0); break;
    case   ZEROGREATER:
      push((pop()>0) ? 1 : 0); break;
    case   AND:
      push(pop() & pop()); break;
    case   OR:
      push(pop() | pop()); break;
    case LSHIFT:
      n = pop(); push(pop() << n); break;
    case IsOdd:
      push((d.partyLevel + d.partyX + d.partyY + d.partyFacing) & 1); break;
    case GetMaskHeader:
      maskNumber = pop();
      maskID     = pop();
      pMask = backgroundLib.GetMask(maskID, maskNumber, 12);
      if (pMask == NULL) break;
      push(pMask->srcX);
      push(pMask->srcY);
      push(pMask->dstX);
      push(pMask->dstY);
      push(pMask->width);
      push(pMask->height);
      break;
    case NeedVirtualMaskQ:
      maskNumber = pop();
      maskID     = pop();
      pMask = backgroundLib.MaskExists(maskID, maskNumber);
      push ((pMask==NULL) ? 1 : 0);
      break;
    case CreateVirtualMask:
      {
        AFFINEMASK aMask;
        BACKGROUND_MASK bMask, *pOldMask, *pNewMask;
        i32 newMaskNumber, newMaskID, oldMaskNumber, oldMaskID;
        newMaskNumber = pop();
        newMaskID     = pop();
        aMask.Cyc     = pop();
        aMask.Cyy     = pop();
        aMask.Cyx     = pop();
        aMask.Cxc     = pop();
        aMask.Cxy     = pop();
        aMask.Cxx     = pop();
        bMask.height  = pop();
        bMask.width   = pop();
        bMask.dstY    = pop();
        bMask.dstX    = pop();
        bMask.srcY    = pop();
        bMask.srcX    = pop();
        oldMaskNumber = pop();
        oldMaskID     = pop();
        GTRACE_4("CreateVirtualMask(%d, %d, bmask, amask, %d, %d\n",
                    oldMaskID, oldMaskNumber, newMaskID, newMaskNumber);
        GTRACE_6("   bMask = %d %d %d %d %d %d\n",
                     bMask.srcX, bMask.srcY, bMask.dstX, bMask.dstY, bMask.width, bMask.height);
        GTRACE_6("   aMask = %d %d %d %d %d %d\n",
                     aMask.Cxx, aMask.Cxy, aMask.Cxc, aMask.Cyx, aMask.Cyy, aMask.Cyc);
        ASSERT(bMask.width > 0,"CreateVirtualMask.width=0");
        if (bMask.width <= 0) break;
        if ((bMask.width & 15) != 0) break; 
        pNewMask = backgroundLib.CreateNewMask(newMaskID, newMaskNumber, &bMask);
        if (pNewMask == NULL) break;
        //memcpy(pNewMask, &bMask, 12);
        pOldMask = backgroundLib.GetMask(oldMaskID, oldMaskNumber, 6);
        if (pOldMask == NULL) break;
        backgroundLib.CreateVirtualMask(pOldMask, pNewMask, &aMask);
#ifdef _DEBUG
        backgroundLib.DumpGraphic(newMaskID, newMaskNumber, "msk");
#endif
      };
      break;
    case CreateVirtualAffineMask:
      {
        AFFINEMASK aMask;
        //BACKGROUND_MASK bMask, *pOldMask, *pNewMask;
        i32 newMaskNumber, newMaskID;
        newMaskNumber   = pop();
        newMaskID       = pop();
        aMask.dstHeight = pop();
        aMask.dstWidth  = pop();
        aMask.srcHeight = pop();
        aMask.srcWidth  = pop();
        aMask.srcY      = pop();
        aMask.srcX      = pop();
        aMask.Cyc       = pop();
        aMask.Cyy       = pop();
        aMask.Cyx       = pop();
        aMask.Cxc       = pop();
        aMask.Cxy       = pop();
        aMask.Cxx       = pop();
        GTRACE_2("CreateVirtualAffineMask (aMask, %d %d\n", newMaskID, newMaskNumber);
        GTRACE_12("    aMask = %d %d %d %d %d %d %d %d %d %d %d %d\n",
                        aMask.Cxx, aMask.Cxy, aMask.Cxc, aMask.Cyx, aMask.Cyy, aMask.Cyc,
                        aMask.srcX, aMask.srcY, aMask.srcWidth, aMask.srcHeight, aMask.dstWidth, aMask.dstHeight);
        ASSERT(aMask.dstWidth > 0,"CreateVirtualAffinMask.width=0");
        if (aMask.dstWidth <= 0) break;
        if ((aMask.dstWidth & 15) != 0) break; 
        backgroundLib.CreateNewAffineMask(newMaskID, newMaskNumber, &aMask);
#ifdef _DDEBUG
        backgroundLib.DumpGraphic(newMaskID, "msk");
#endif
      };
      break;
    case ::BitmapInfo:
      {
        int width = 0;
        int height = 0;
        graphicID = pop();
        pGraphic = backgroundLib.GetBitmap(graphicID, 4, &graphicSize);
        if (pGraphic!= NULL)
        {
          width = *pGraphic;
          height = (graphicSize-1)*8/width;
        };
        push(width);
        push(height);
      }
      break;
    default:
      if (*(P-1) < 60000)
      {
        rpush(P-code);
        P = code + *(P-1);
        break;
      };
      InterpretError("Unknown display opcode\nCode = %d.",*(P-1));
      return;
    };
  };
}



//   TAG00133e
void ClearHeldObjectName()
{
  i16 save;
  save = d.UseByteCoordinates;
  d.UseByteCoordinates = false;
  FillRectangle(d.LogicalScreenBase,
                &d.wRectPos624,
                0,
                160);
  d.UseByteCoordinates = save;
}

//   TAG004308
void FloorAndCeilingOnly()
{
  static i32 count=0;
  if ((VBLMultiplier!=1) && (((count++)%1) != 0) && (VBLMultiplier!=99)) return;
  Instrumentation(icntFloorAndCeiling);
  ClearMemory(d.pViewportBlack,4144);//space between floor and ceiling
  MemMove(d.pCeilingBitmap, d.pViewportBMP, 3248);//ceiling
  MemMove(d.pFloorBitmap, d.pViewportFloor, 7840);//floor
  d.Word11684=0;
}



void DrawCellF4L1(i32 /*facing*/,  SUMMARIZEROOMDATA * /*pRoomData*/)
{
  /*
  DrawRoomObjects(
        FindFirstDrawableObject(pRoomData->x,pRoomData->y),
        facing,
        pRoomData->x,
        pRoomData->y,
        -2,
        1);
  */
}

void DrawCellF4R1(i32 /*facing*/,  SUMMARIZEROOMDATA * /*pRoomData*/)
{
  /*
  DrawRoomObjects(
        FindFirstDrawableObject(pRoomData->x, pRoomData->y), 
        facing,
        pRoomData->x, 
        pRoomData->y,
        -1, 
        1);
 */
}

void DrawCellF4(i32 /*facing*/,  SUMMARIZEROOMDATA * /*pRoomData*/)
{
  /*
  DrawRoomObjects(
        FindFirstDrawableObject(pRoomData->x,pRoomData->y), 
        facing, 
        pRoomData->x, 
        pRoomData->y, 
        -3, 
        1);
  */
}

void DrawCellF3L2(i32 /*facing*/, SUMMARIZEROOMDATA *pRoomData)
{
  switch ((GetCellFlags(pRoomData->x, pRoomData->y) >> 5) & 0x7)
  {
  case roomSTONE:
    BltShapeToViewport(d.pWallBitmaps[5], &d.wallRectangles[13]);
    break;
  };
}

void DrawCellF3R2(i32 /*facing*/, SUMMARIZEROOMDATA *pRoomData)
{
  switch ((GetCellFlags(pRoomData->x, pRoomData->y) >> 5) & 0x7)
  {
  case roomSTONE:
  /*
    BltShapeToViewport(d.pWallBitmaps[6], &d.wallRectangles[12]);
  */
    break;
  };
}

/*
//   TAG00788c
void DrawCellF1R1(i32  facing, SUMMARIZEROOMDATA *pRoomData)
{
  //dReg D4;
  //SUMMARIZEROOMDATA roomData;
  //D7W=P1;D6W=P2,D5W=P3;
  //SummarizeRoomInfo(&roomData,facing,pRoomData->x,pRoomData->y);
  switch (pRoomData->roomType)
  {

  case roomSTAIRFACING:
        
      if (pRoomData->decorations[0] != 0) MirrorGraphicToViewport(d.Word1880,&d.StairFacingRect[13]);
      else MirrorGraphicToViewport(d.Word1894,&d.StairFacingRect[2]);
      DrawFloorDecorations(
              pRoomData->decorations[2],
              pRoomData->relativeCellNumber);
      CheckCeilingPit(
              pRoomData->relativeCellNumber,
              pRoomData->x, 
              pRoomData->y);
      DrawRoomObjects(
              pRoomData->rn2,
              facing,
              pRoomData->x,
              pRoomData->y,
              pRoomData->relativeCellNumber,
              0x41);
    
      break;
  case roomSTONE:
    
      BltShapeToViewport(d.pWallBitmaps[2], &d.wallRectangles[relativeCellNumber]);
      DrawWallDecoration(d.NumWallDecoration+1,11,pRoomData->text[2]);
      DrawWallDecoration(pRoomData->decorations[2],11,RNeof);
    
      break;
  case roomSTAIREDGE:
       
      if (pRoomData->decorations[0] != 0) MirrorGraphicToViewport(d.StairEdgeGraphic[2], &d.StairEdgeRect[4]);
      else MirrorGraphicToViewport(d.StairEdgeGraphic[1],&d.StairEdgeRect[2]);
      DrawFloorDecorations(
              pRoomData->decorations[2],
              pRoomData->relativeCellNumber);
      CheckCeilingPit(
              pRoomData->relativeCellNumber,
              pRoomData->x,
              pRoomData->y);
      DrawRoomObjects(
              pRoomData->rn2,
              facing,
              pRoomData->x,
              pRoomData->y,
              pRoomData->relativeCellNumber,
              0x41);
    
      break;
  case roomDOOREDGE:
    
      DrawFloorDecorations(
              pRoomData->decorations[2],
              pRoomData->relativeCellNumber);
      CheckCeilingPit(
              pRoomData->relativeCellNumber,
              pRoomData->x,
              pRoomData->y);
      DrawRoomObjects(
              pRoomData->rn2,
              facing,
              pRoomData->x,
              pRoomData->y,
              pRoomData->relativeCellNumber,
              0x41);
    
      break;
  case roomDOORFACING:
    
      DrawFloorDecorations(
              pRoomData->decorations[2],
              pRoomData->relativeCellNumber);
      DrawRoomObjects(
              pRoomData->rn2,
              facing,
              pRoomData->x,
              pRoomData->y,
              pRoomData->relativeCellNumber,
              0x18);
      BltShapeToViewport(
              d.pDoorBitmaps[6], 
              (RectPos *)d.Byte3194);
      DrawDoor(
              pRoomData->decorations[1],
              pRoomData->decorations[0],
              d.Word1918,
              0x1080,2,
              d.Byte3914);
      DrawRoomObjects(
              pRoomData->rn2,
              facing,
              pRoomData->x,
              pRoomData->y,
              pRoomData->relativeCellNumber,
              0x49);
    
      break;
  case roomPIT:
    
      MirrorGraphicToViewport(pRoomData->decorations[0]!=0?59:53,(RectPos *)d.Byte2858);
      DrawFloorDecorations(
              pRoomData->decorations[2],
              pRoomData->relativeCellNumber);
      CheckCeilingPit(
              pRoomData->relativeCellNumber,
              pRoomData->x,
              pRoomData->y);
      DrawRoomObjects(
              pRoomData->rn2,
              facing,
              pRoomData->x,
              pRoomData->y,
              pRoomData->relativeCellNumber,
              0x41);
    
      break;
  case roomTELEPORTER:
    
      DrawFloorDecorations(
              pRoomData->decorations[2],
              pRoomData->relativeCellNumber);
      CheckCeilingPit(
              pRoomData->relativeCellNumber,
              pRoomData->x,
              pRoomData->y);
      DrawRoomObjects(
              pRoomData->rn2,
              facing,
              pRoomData->x,
              pRoomData->y,
              pRoomData->relativeCellNumber,
              0x41);
      if (pRoomData->decorations[0] != 0)
            DrawTeleporter(
                 &d.teleporterRectangles[pRoomData->relativeCellNumber],
                 &d.wallRectangles[pRoomData->relativeCellNumber]);
    
      break;
  case roomOPEN:
    
      DrawFloorDecorations(
              pRoomData->decorations[2],
              pRoomData->relativeCellNumber);
      CheckCeilingPit(
              pRoomData->relativeCellNumber,
              pRoomData->x,
              pRoomData->y);
      DrawRoomObjects(
              pRoomData->rn2,
              facing,
              pRoomData->x,
              pRoomData->y,
              pRoomData->relativeCellNumber,
              0x41);
              
      break;
  };
}
*/

//*********************************************************
//
//*********************************************************
//   TAG007a4a
void DrawCellF1(i32 /*facing*/, SUMMARIZEROOMDATA *pRoomData, bool /*skipDrawing*/)
{
  //dReg D0;
  //DB0 *pDB0;
  //i16 *pwA0;
  //SUMMARIZEROOMDATA roomData;
  //pnt LOCAL_14;
  //D7W = P1; D6W = P2; D5W = P3;
  //SummarizeRoomInfo(&roomData,facing,pRoomData->x,pRoomData->y);//very quick
  d.CellTypeJustAhead = sw(pRoomData->roomType);
  switch (pRoomData->roomType)
  {
  case roomSTAIRFACING:
    /*
      if (!skipDrawing)
      {
        if (pRoomData->decorations[0] != 0) 
                GraphicToViewport(d.StairFacingUpGraphic[1], &d.StairFacingUpRect[3]);
        else GraphicToViewport(d.StairFacingDownGraphic[1], &d.StairFacingDownRect[3]);
      }
      DrawFloorDecorations(
              pRoomData->decorations[2],
              0x7);
      if (!skipDrawing)
      CheckCeilingPit(
              pRoomData->relativeCellNumber,
              pRoomData->x,
              pRoomData->y);
      DrawRoomObjects(
              pRoomData->rn2,
              facing,
              pRoomData->x,
              pRoomData->y,
              pRoomData->relativeCellNumber,
              0x3421);
    */
      break;
  case roomSTONE:
    /*
      d.FacingWaterFountain = d.FacingViAltar = d.FacingAlcove = 0;
      if (d.SeeThruWalls != 0)
      {
        AllocateDerivedGraphic(1);
        if (!skipDrawing)
        TAG0088b2(d.pViewportBMP,         //Copy what is behind wall
                  TAG022d38(1),
                  //(RectPos *)d.Word2538,
                  &d.SeeThruWallsRect,
                  //d.uByte2534[0],
                  d.SeeThruWallsRect.b.uByte4,
                  //d.uByte2534[2],
                  d.SeeThruWallsRect.b.uByte6,
                  112,
                  48,
                  -1);
        LOCAL_14 = GetBasicGraphicAddress(41);
        if (!skipDrawing)
        TAG0088b2(LOCAL_14,                // Hole?
                  TAG022d38(1),
                  //(RectPos *)d.Word2538,
                  &d.SeeThruWallsRect,
                  0,
                  0,
                  48,
                  48,
                  10);
      };
      //ClearViewport();
      if (!skipDrawing)
      BltRectangleToViewport(
              d.pWallBitmaps[2], 
              &d.wallRectangles[pRoomData->relativeCellNumber]);
      D0W = DrawWallDecoration(
              pRoomData->decorations[1],
              12,
              RNeof);//Draws wall decorations
      DrawWallDecoration(
              d.NumWallDecoration+1,
              12,
              pRoomData->text[1],
              pRoomData->decorations[1] == 0);
      if (D0W != 0) //if alcove
      {
        //if (!skipDrawing)
        DrawRoomObjects(
              pRoomData->rn2,
              facing,
              pRoomData->x,
              pRoomData->y,
              pRoomData->relativeCellNumber,
              0);
      };
      if (d.SeeThruWalls != 0)
      {
        TAG0088b2(TAG022d38(1),           // Hole and things behind wall.
                  d.pViewportBMP,
                  //(RectPos *)d.uByte2534, //0x40,0x9f,0x13,0x71
                  (RectPos *)&d.SeeThruWallsRect.b.uByte4, //0x40,0x9f,0x13,0x71
                  0,
                  0,
                  48,
                  112,
                  9);
        //TAG022d5e(1);
        DeleteGraphic((ui16)0x8001);
      };
    */
      break;
  case roomDOORFACING:
    /*
      DrawFloorDecorations(
              pRoomData->decorations[2], 
              0x7);
      DrawRoomObjects(
              pRoomData->rn2,
              facing,
              pRoomData->x,
              pRoomData->y,
              pRoomData->relativeCellNumber,
              0x218);
      BltShapeToViewport(d.pDoorBitmaps[6], &d.DoorTrackTopRect[1]);
      BltShapeToViewport(d.pDoorBitmaps[2], &d.DoorFrameRect[1]);//(RectPos *)d.Byte3130);
      BltShapeToViewport(d.pDoorBitmaps[0], &d.DoorFrameRect[0]);//(RectPos *)d.Byte3138);
      //A0 = (pnt)d.misc1052eight[0] + pRoomData->decorations[1]*4;
      //D0W = (wordGear(A0+2)>>6) & 1;
      //if (D0W != 0) DrawDoorSwitch(1,3);
      pDB0 = GetRecordAddressDB0(pRoomData->decorations[1]);
      if (pDB0->doorSwitch()) DrawDoorSwitch(1, 3);
      DrawDoor(
              pRoomData->decorations[1],
              pRoomData->decorations[0],
              d.DoorGraphic[0], //d.Word1918,
              0x1080,
              2,
              d.DoorRectsF1);//d.Byte3834);
      DrawRoomObjects(
              pRoomData->rn2,
              facing,
              pRoomData->x,
              pRoomData->y,
              pRoomData->relativeCellNumber,
              0x349);
    */
      break;
  case roomPIT:
    /*
      if (!skipDrawing)
      GraphicToViewport(pRoomData->decorations[0]!=0?60:54, &d.FloorPitRect[4]);

      DrawFloorDecorations(
              pRoomData->decorations[2],
              7);
      if (!skipDrawing)
      CheckCeilingPit(
              pRoomData->relativeCellNumber,
              pRoomData->x,
              pRoomData->y);
      DrawRoomObjects(
              pRoomData->rn2,
              facing,
              pRoomData->x,
              pRoomData->y,
              pRoomData->relativeCellNumber,
              0x3421);
    */
      break;
  case roomTELEPORTER:
    /*
      DrawFloorDecorations(
              pRoomData->decorations[2],
              7);
      if (!skipDrawing)
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(pRoomData->rn2,facing,pRoomData->x,pRoomData->y,pRoomData->relativeCellNumber,0x3421);
      if (pRoomData->decorations[0] == 0)   break;
      if (!skipDrawing)
      DrawTeleporter(
            &d.teleporterRectangles[pRoomData->relativeCellNumber],
            &d.wallRectangles[pRoomData->relativeCellNumber]);
    */
      break;
  case roomOPEN:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2],
            7);
      if (!skipDrawing)
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x3421);
    */
      break;
  };
}

//   TAG006b20
void DrawCellF3L1(i32 /*facing*/, SUMMARIZEROOMDATA *pRoomData)
{
  //dReg D0;
  ROOMTYPE rtD0;
  //SUMMARIZEROOMDATA roomData; // array???
//  D7W = P1;
//  D6W = P2;
//  D5W = P3;
  //SummarizeRoomInfo(&roomData, facing, pRoomData->x, pRoomData->y);
  rtD0 = pRoomData->roomType;
  switch (rtD0)
  {
  case roomSTAIRFACING:
      //D0W = pRoomData->decorations[0];
    /*
      if (pRoomData->decorations[0] != 0)
      {
        GraphicToViewport(d.StairFacingUpGraphic[6], &d.StairFacingUpRect[10]);
      }
      else
      {
        GraphicToViewport(d.StairFacingDownGraphic[6], &d.StairFacingDownRect[10]);
      };
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            0);
      DrawRoomObjects(
            pRoomData->rn2, 
            facing, 
            pRoomData->x, 
            pRoomData->y, 
            pRoomData->relativeCellNumber, 
            0x3421);
    */
      break;
  case roomSTONE:
    /*
      BltShapeToViewport(d.pWallBitmaps[4], &d.wallRectangles[pRoomData->relativeCellNumber]);
      DrawWallDecoration(
            d.NumWallDecoration+1,
            0,
            pRoomData->text[0]);
      DrawWallDecoration(
            pRoomData->decorations[0],
            0,
            RNeof);
      DrawWallDecoration(
            d.NumWallDecoration+1, 
            2,
            pRoomData->text[1]);
      D0W = DrawWallDecoration(
            pRoomData->decorations[1], 
            2,
            RNeof);
      if (D0W == 0) return;
      DrawRoomObjects(
            pRoomData->rn2, 
            facing, 
            pRoomData->x, 
            pRoomData->y, 
            pRoomData->relativeCellNumber, 
            0);
    */
      break;
  case roomDOOREDGE:
  case roomSTAIREDGE:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            0);
      DrawRoomObjects(
            pRoomData->rn2, 
            facing, 
            pRoomData->x, 
            pRoomData->y, 
            pRoomData->relativeCellNumber, 
            0x321);
    */
      break;
  case roomDOORFACING:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            0);
      DrawRoomObjects(
            pRoomData->rn2, 
            facing, 
            pRoomData->x, 
            pRoomData->y, 
            pRoomData->relativeCellNumber, 
            0x218);
      BltShapeToViewport(d.pDoorBitmaps[5], (RectPos *)d.Byte3082);
      DrawDoor(
            pRoomData->decorations[1], 
            pRoomData->decorations[0], 
            d.DoorGraphic[2],//d.Word1910, 
            984, 
            0,
            d.DoorRectsF3L1);//d.Byte3274);
      DrawRoomObjects(
            pRoomData->rn2, 
            facing, 
            pRoomData->x, 
            pRoomData->y, 
            pRoomData->relativeCellNumber, 
            0x349);
    */
      break;
  case roomPIT:
      //D0W = pRoomData->decorations[0];
    /*
      if (pRoomData->decorations[0] == 0)
      {
        GraphicToViewport(49, &d.FloorPitRect[11]);
      };
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            0);
      DrawRoomObjects(
            pRoomData->rn2, 
            facing, 
            pRoomData->x, 
            pRoomData->y, 
            pRoomData->relativeCellNumber, 
            0x3421);
    */
      break;
  case roomTELEPORTER:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            0);
      DrawRoomObjects(
            pRoomData->rn2, 
            facing, 
            pRoomData->x, 
            pRoomData->y, 
            pRoomData->relativeCellNumber, 
            0x3421);
      if (pRoomData->decorations[0] != 0)
      {
        DrawTeleporter(
            &d.teleporterRectangles[pRoomData->relativeCellNumber], 
            &d.wallRectangles[pRoomData->relativeCellNumber]);
      };
    */
      break;
  case roomOPEN:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            0);
      DrawRoomObjects(
            pRoomData->rn2, 
            facing, 
            pRoomData->x, 
            pRoomData->y, 
            pRoomData->relativeCellNumber, 
            0x3421);
    */
      break;
  };
}

//   TAG006ca2
void DrawCellF3R1(i32 /*facing*/, SUMMARIZEROOMDATA *pRoomData)
{
  //dReg D0;
  //DB0 *pDB0;
  //SUMMARIZEROOMDATA roomData;
  //D4L = 0;
  //SummarizeRoomInfo(&roomData, facing, pRoomData->x, pRoomData->y);
  switch(pRoomData->roomType)
  {
  case roomSTONE:
    /*
      BltShapeToViewport(d.pWallBitmaps[4], &d.wallRectangles[pRoomData->relativeCellNumber]);
      DrawWallDecoration(
            d.NumWallDecoration+1, 
            1,
            pRoomData->text[2]);
      DrawWallDecoration(
            pRoomData->decorations[2], 
            1,
            RNeof);
      DrawWallDecoration(
            d.NumWallDecoration+1, 
            4,
            pRoomData->text[1]);
      D0W = DrawWallDecoration(
            pRoomData->decorations[1], 
            4,
            RNeof);
      if (D0W != 0) DrawRoomObjects(
                        pRoomData->rn2,
                        facing, 
                        pRoomData->x, 
                        pRoomData->y,
                        pRoomData->relativeCellNumber,
                        0);
    */
      break;
  case roomTELEPORTER:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            pRoomData->relativeCellNumber);
      DrawRoomObjects(
            pRoomData->rn2,
            facing, 
            pRoomData->x, 
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x4312);
      if (pRoomData->decorations[0] != 0)
              //TAG004f82(d.Byte3994, (RectPos *)d.Byte3058);
              // I replaced the 3994 with 4002 because 3994[3]
              // was zero and cause TAG004f82 to use NULL as a
              // source pointer.  This seems to cause no harm
              // on the ATARI.  In fact, it seems to make no
              // difference what data is used.
              DrawTeleporter(
                         &d.teleporterRectangles[pRoomData->relativeCellNumber-1], 
                         &d.wallRectangles[pRoomData->relativeCellNumber]);
    */
      break;
  case roomOPEN:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            pRoomData->relativeCellNumber);
      DrawRoomObjects(
            pRoomData->rn2,
            facing, 
            pRoomData->x, 
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x4312);
    */
      break;
  case roomPIT:
    /*
      if (pRoomData->decorations[0] == 0) 
            MirrorGraphicToViewport(49, &d.FloorPitRect[9]);
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            pRoomData->relativeCellNumber);
      DrawRoomObjects(
            pRoomData->rn2,
            facing, 
            pRoomData->x, 
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x4312);
    */
      break;
  case roomDOORFACING:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            pRoomData->relativeCellNumber);
      DrawRoomObjects(
            pRoomData->rn2,
            facing, 
            pRoomData->x, 
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x128);
      MemMove(d.pDoorBitmaps[5], d.Pointer1848, 704);
      MirrorShapeBltToViewport(d.Pointer1848, &d.DoorFrameRect[6]);
      //A0 = (pnt)d.misc1052eight[0] + pRoomData->decorations[1]*4;
      //D0W = (wordGear(A0+2) >> 6) & 1;
      //if (D0W != 0) DrawDoorSwitch(1, 0);
      pDB0 = GetRecordAddressDB0(pRoomData->decorations[1]);
      if (pDB0->doorSwitch()) DrawDoorSwitch(1, 0);
      DrawDoor(
            pRoomData->decorations[1],
            pRoomData->decorations[0],
            d.DoorGraphic[2],//d.Word1910,
            984,
            0,
            d.DoorRectsF3R1);//d.Byte3434);
      DrawRoomObjects(
            pRoomData->rn2,
            facing, 
            pRoomData->x, 
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x439);
    */
      break;
  case roomDOOREDGE:
  case roomSTAIREDGE:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            pRoomData->relativeCellNumber);
      DrawRoomObjects(
            pRoomData->rn2,
            facing, 
            pRoomData->x, 
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x412);
    */
      break;
  case roomSTAIRFACING:
    /*
      if (pRoomData->decorations[0] != 0) 
             MirrorGraphicToViewport(d.StairFacingUpGraphic[6], &d.StairFacingUpRect[8]);
      else MirrorGraphicToViewport(d.StairFacingDownGraphic[6], &d.StairFacingDownRect[8]);
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            pRoomData->relativeCellNumber);
      DrawRoomObjects(
            pRoomData->rn2,
            facing, 
            pRoomData->x, 
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x4312);
    */
      break;
  };
}

//   TAG006e66
void DrawCellF3(i32 /*facing*/,SUMMARIZEROOMDATA *pRoomData)
{
  //dReg D0;
  //DB0 *pDB0;
  //SUMMARIZEROOMDATA roomData;
  //void (*fn)(i32, i32, i32) = NULL;
  //SummarizeRoomInfo(&roomData, facing, pRoomData->x, pRoomData->y);
  switch(pRoomData->roomType)
  {
  case roomOPEN:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            1);
      DrawRoomObjects(
            pRoomData->rn2, 
            facing, 
            pRoomData->x, 
            pRoomData->y, 
            pRoomData->relativeCellNumber, 
            0x3421);
    */
      break;
  case roomTELEPORTER:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            1);
      DrawRoomObjects(
            pRoomData->rn2, 
            facing, 
            pRoomData->x, 
            pRoomData->y, 
            pRoomData->relativeCellNumber, 
            0x3421);
      if (pRoomData->decorations[0] != 0)
          DrawTeleporter(
               &d.teleporterRectangles[pRoomData->relativeCellNumber],
               &d.wallRectangles[pRoomData->relativeCellNumber]);
    */
      break;
  case roomPIT:
    /*
      if (pRoomData->decorations[0] == 0) 
              GraphicToViewport(50, &d.FloorPitRect[10]);
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            1);
      DrawRoomObjects(
            pRoomData->rn2, 
            facing, 
            pRoomData->x, 
            pRoomData->y, 
            pRoomData->relativeCellNumber, 
            0x3421);
    */
      break;
  case roomDOORFACING:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            1);
      DrawRoomObjects(
            pRoomData->rn2, 
            facing, 
            pRoomData->x, 
            pRoomData->y, 
            pRoomData->relativeCellNumber, 
            0x218);
      BltShapeToViewport(d.pDoorBitmaps[4], &d.DoorFrameRect[5]);//(RectPos *)d.Byte3098);
      MemMove(d.pDoorBitmaps[4], d.Pointer1848, 704);
      MirrorShapeBltToViewport(d.Pointer1848, &d.DoorFrameRect[4]);//d.Byte3106);
      //A0 = (pnt)d.misc1052eight[0] + pRoomData->decorations[1]*4;
      //D0W = (wordGear(A0+2)>>6)&1;
      //if (D0W != 0) DrawDoorSwitch(1, 1);
      pDB0 = GetRecordAddressDB0(pRoomData->decorations[1]);
      if (pDB0->doorSwitch()) DrawDoorSwitch(1, 1);
      DrawDoor(
            pRoomData->decorations[1],     //DB0index
            pRoomData->decorations[0],     //Open, closed, bashed, etc.
            d.DoorGraphic[2],//d.Word1910, //Two graphic numbers
            984,                           //Graphic size
            0,                             //Nearness (0=F3)
            d.DoorRectsF3);                 //rectangle);
      DrawRoomObjects(
            pRoomData->rn2, 
            facing, 
            pRoomData->x, 
            pRoomData->y, 
            pRoomData->relativeCellNumber, 
            0x349);
    */
      break;
  case roomSTONE:
    /*
      BltRectangleToViewport(d.pWallBitmaps[4], &d.wallRectangles[pRoomData->relativeCellNumber]);
      DrawWallDecoration(
            d.NumWallDecoration+1, 
            3,
            pRoomData->text[1]);
      D0W = DrawWallDecoration(
            pRoomData->decorations[1], 
            3,
            RNeof);
      if (D0W != 0) DrawRoomObjects(
                          pRoomData->rn2, 
                          facing, 
                          pRoomData->x, 
                          pRoomData->y, 
                          pRoomData->relativeCellNumber, 0);
    */
      break;
  case roomSTAIRFACING:
    /*
      if (pRoomData->decorations[0] != 0) 
           GraphicToViewport(d.StairFacingUpGraphic[5], &d.StairFacingUpRect[9]);
      else GraphicToViewport(d.StairFacingDownGraphic[5], &d.StairFacingDownRect[9]);
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            1);
      DrawRoomObjects(
            pRoomData->rn2, 
            facing, 
            pRoomData->x, 
            pRoomData->y, 
            pRoomData->relativeCellNumber, 
            0x3421);
    */
      break;
  };
}

//   TAG0070d8
void DrawCellF2L1(i32 /*facing*/, SUMMARIZEROOMDATA *pRoomData)
{
  //  dReg D0;
  //SUMMARIZEROOMDATA roomData;
  //D7W = P1;
  //D6W = P2;
  //D5W = P3;
  //SummarizeRoomInfo(&roomData, facing, pRoomData->x, pRoomData->y);
  switch (pRoomData->roomType)
  {
  case roomTELEPORTER:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            3);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x3421);
      if (pRoomData->decorations[0] != 0) 
                 DrawTeleporter(
                      &d.teleporterRectangles[pRoomData->relativeCellNumber], 
                      &d.wallRectangles[pRoomData->relativeCellNumber]);
      break;
    */
  case roomOPEN:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            3);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x3421);
    */
      break;
  case roomPIT:
    /*
      GraphicToViewport(pRoomData->decorations[0]==0?51:57,&d.FloorPitRect[8]);
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            3);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x3421);
    */
      break;
  case roomDOORFACING:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            3);
      DrawRoomObjects(
            pRoomData->rn2,
            facing, 
            pRoomData->x, 
            pRoomData->y, 
            pRoomData->relativeCellNumber, 
            0x218);
      BltShapeToViewport(d.pDoorBitmaps[7], &d.DoorTrackTopRect[5]);
      DrawDoor(
            pRoomData->decorations[1], 
            pRoomData->decorations[0], 
            d.DoorGraphic[1],//d.Word1914, 
            1952, 
            1, 
            d.DoorRectsF2L1);//d.Byte3514);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x349);
            */
      break;
  case roomDOOREDGE:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            3);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0);
    */
      break;
  case roomSTAIREDGE:
    /*
      GraphicToViewport(d.StairEdgeGraphic[3], &d.StairEdgeRect[7]);
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            3);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0);
    */
      break;
  case roomSTONE:
    /*
      BltShapeToViewport(d.pWallBitmaps[3], &d.wallRectangles[pRoomData->relativeCellNumber]);
      DrawWallDecoration(
            d.NumWallDecoration+1, 
            5,
            pRoomData->text[0]); //wall decorations?
      DrawWallDecoration(
            pRoomData->decorations[0], 
            5,
            RNeof); //wall decorations?
      DrawWallDecoration(
            d.NumWallDecoration+1, 
            7,
            pRoomData->text[1]);//wall decorations
      D0W = DrawWallDecoration(
            pRoomData->decorations[1], 
            7,
            RNeof);//wall decorations
      if (D0W != 0)
      {
        //CheckCeilingPit(  Where did this come from???
        //    pRoomData->relativeCellNumber,
        //    pRoomData->x,
        //    pRoomData->y);
        DrawRoomObjects(
              pRoomData->rn2,
              facing,
              pRoomData->x,
              pRoomData->y,
              pRoomData->relativeCellNumber,
              0);
      };
    */
      break;
  case roomSTAIRFACING:
    /*
      if (pRoomData->decorations[0] != 0) 
           GraphicToViewport(d.StairFacingUpGraphic[4], &d.StairFacingUpRect[7]);
      else GraphicToViewport(d.StairFacingDownGraphic[4], &d.StairFacingDownRect[7]);
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            3);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(pRoomData->rn2,facing,pRoomData->x,pRoomData->y,pRoomData->relativeCellNumber,0x3421);
    */
      break;
  };
}

//   TAG007290
void DrawCellF2R1(i32 /*facing*/, SUMMARIZEROOMDATA *pRoomData)
{
  //dReg D0;
  //SUMMARIZEROOMDATA roomData;
  //SummarizeRoomInfo(&roomData, facing, pRoomData->x, pRoomData->y);
  switch(pRoomData->roomType)
  {
  case roomTELEPORTER:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2],
            pRoomData->relativeCellNumber);
      CheckCeilingPit(
            pRoomData->relativeCellNumber, 
            pRoomData->x, 
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x4312);
      if (pRoomData->decorations[0] != 0)
              DrawTeleporter(
                    &d.teleporterRectangles[pRoomData->relativeCellNumber], 
                    &d.wallRectangles[pRoomData->relativeCellNumber]);
    */
      break;
  case roomOPEN:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2],
            pRoomData->relativeCellNumber);
      CheckCeilingPit(
            pRoomData->relativeCellNumber, 
            pRoomData->x, 
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x4312);
    */
      break;
  case roomPIT:
    /*
      MirrorGraphicToViewport(pRoomData->decorations[0]==0?51:57,&d.FloorPitRect[6]);
      DrawFloorDecorations(
            pRoomData->decorations[2],
            pRoomData->relativeCellNumber);
      CheckCeilingPit(
            pRoomData->relativeCellNumber, 
            pRoomData->x, 
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x4312);
     */
      break;
  case roomDOORFACING:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            pRoomData->relativeCellNumber);
      DrawRoomObjects(
            pRoomData->rn2,
            facing, 
            pRoomData->x, 
            pRoomData->y, 
            pRoomData->relativeCellNumber, 
            0x128);
      BltShapeToViewport(d.pDoorBitmaps[7], &d.DoorTrackTopRect[3]);
      DrawDoor(
            pRoomData->decorations[1],
            pRoomData->decorations[0],
            d.DoorGraphic[1],//d.Word1914,
            1952,
            1,
            d.DoorRectsF2R1);//d.Byte3674);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x439);
    */
      break;
  case roomDOOREDGE:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2],
            pRoomData->relativeCellNumber);
      CheckCeilingPit(
            pRoomData->relativeCellNumber, 
            pRoomData->x, 
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x431);
    */
      break;
  case roomSTAIREDGE:
    /*
      MirrorGraphicToViewport(d.StairEdgeGraphic[3], &d.StairEdgeRect[6]);
      DrawFloorDecorations(
            pRoomData->decorations[2],
            pRoomData->relativeCellNumber);
      CheckCeilingPit(
            pRoomData->relativeCellNumber, 
            pRoomData->x, 
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x431);
    */
      break;
  case roomSTONE:
    /*
      BltShapeToViewport(d.pWallBitmaps[3], &d.wallRectangles[pRoomData->relativeCellNumber]);
      DrawWallDecoration(
            d.NumWallDecoration+1,
            6,
            pRoomData->text[2]);
      DrawWallDecoration(
            pRoomData->decorations[2],
            6,
            RNeof);
      DrawWallDecoration(
            d.NumWallDecoration+1, 
            9,
            pRoomData->text[1]);
      D0W = DrawWallDecoration(
            pRoomData->decorations[1], 
            9,
            RNeof);
      if (D0W != 0)
      {
        DrawRoomObjects(
              pRoomData->rn2,
              facing,
              pRoomData->x,
              pRoomData->y,
              pRoomData->relativeCellNumber,
              0);
        break;
      };
      if ((ui16)d.Word1836 > 6)  break;
      if (d.Word1840 ==0)  break;
      if (d.Word23228 != 0)  break;
      if ((d.Word11722 & 1) == 0 )
      {
        if ((d.CacheInvalid & 1) != 0)
        {
          d.Word1844 = 555;
          HopefullyNotNeeded();
          d.Word1836 += 5;
          break;
        };
      };
      if ((d.Word11724 & 1) == 0) break;
      if ((d.Word11722 & 1) != 0)
      {
        d.Word1842++;
        d.Word11722 &= 0xfffe;
      };
      d.Word11724 &= 0xfffe;
      d.Word1836 += 10;
    */
      break;
  case roomSTAIRFACING:
    /*
      if (pRoomData->decorations[0]!=0) 
           MirrorGraphicToViewport(d.StairFacingUpGraphic[4],&d.StairFacingUpRect[5]);
      else MirrorGraphicToViewport(d.StairFacingDownGraphic[4], &d.StairFacingDownRect[5]);
      DrawFloorDecorations(
            pRoomData->decorations[2],
            pRoomData->relativeCellNumber);
      CheckCeilingPit(
            pRoomData->relativeCellNumber, 
            pRoomData->x, 
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x4312);
    */
      break;
  };
}

//*********************************************************
//
//*********************************************************
//   TAG0074f0
void DrawCellF2(i32 /*facing*/, SUMMARIZEROOMDATA *pRoomData)
{
  //dReg D0;
  //DB0 *pDB0;
  //i16 *pwA0;
  //SUMMARIZEROOMDATA roomData;
  //D7W = P1; D6W = P2; D5W = P3;
  //SummarizeRoomInfo(&roomData,facing,pRoomData->x,pRoomData->y);
  switch (pRoomData->roomType)
  {
  case roomTELEPORTER:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            4);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x3421);
      if (pRoomData->decorations[0] != 0)
          DrawTeleporter(
               &d.teleporterRectangles[pRoomData->relativeCellNumber], 
               &d.wallRectangles[pRoomData->relativeCellNumber]);
    */
      break;
  case roomOPEN:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            4);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x3421);
    */
      break;
  case roomPIT:
    /*
      GraphicToViewport(pRoomData->decorations[0]==0?52:58,&d.FloorPitRect[7]);
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            4);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x3421);
    */
      break;
  case roomDOORFACING:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            4);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x218);
      BltShapeToViewport(d.pDoorBitmaps[7], &d.DoorTrackTopRect[4]);
      BltShapeToViewport(d.pDoorBitmaps[3], &d.DoorFrameRect[3]);//(RectPos *)d.Byte3114); // Door sides appear
      MemMove(d.pDoorBitmaps[3], d.Pointer1848, 1560);
      MirrorShapeBltToViewport(d.Pointer1848, &d.DoorFrameRect[2]);//d.Byte3122); // Other side of door.
      pDB0 = GetRecordAddressDB0(pRoomData->decorations[1]);
      if (pDB0->doorSwitch()) DrawDoorSwitch(1, 2);
      DrawDoor(
            pRoomData->decorations[1],
            pRoomData->decorations[0],
            d.DoorGraphic[1],//d.Word1914,
            1952,
            1,
            d.DoorRectsF2);//d.Byte3594);
      //  A door has appeared.
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x349);
    */
      break;
  case roomSTONE:
    /*
      BltRectangleToViewport(d.pWallBitmaps[3], &d.wallRectangles[pRoomData->relativeCellNumber]);
      DrawWallDecoration(
            d.NumWallDecoration+1, 
            8, 
            pRoomData->text[1]);
      D0W = DrawWallDecoration(
            pRoomData->decorations[1], 
            8,
            RNeof);
      if (D0W != 0)
      {
        DrawRoomObjects(
              pRoomData->rn2,
              facing,
              pRoomData->x,
              pRoomData->y,
              pRoomData->relativeCellNumber,
              0);
      };
    */
      break;
  case roomSTAIRFACING:
    /*
      if (pRoomData->decorations[0] != 0) 
           GraphicToViewport(d.StairFacingUpGraphic[3], &d.StairFacingUpRect[6]);
      else GraphicToViewport(d.StairFacingDownGraphic[3], &d.StairFacingDownRect[6]);
      DrawFloorDecorations(
            pRoomData->decorations[2], 
            4);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x3421);
    */
      break;
  };
}

//*********************************************************
//
//*********************************************************
//   TAG0076d0
void DrawCellF1L1(i32 /* facing */, SUMMARIZEROOMDATA *pRoomData)
{
  //dReg D4;
  //SUMMARIZEROOMDATA roomData;
  //D7W = P1; D6W = P2; D5W = P3;
  //SummarizeRoomInfo(&roomData,facing, pRoomData->x, pRoomData->y);
  switch (pRoomData->roomType)
  {
  case roomTELEPORTER:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2],
            6);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x32);
      if (pRoomData->decorations[0] != 0)
              DrawTeleporter(
                  &d.teleporterRectangles[pRoomData->relativeCellNumber],
                  &d.wallRectangles[pRoomData->relativeCellNumber]);
    */
      break;
  case roomOPEN:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2],
            6);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x32);
            */
      break;
  case roomPIT:
    /*
      GraphicToViewport(pRoomData->decorations[0]!=0?59:53,&d.FloorPit[5]);
      DrawFloorDecorations(
            pRoomData->decorations[2],
            6);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x32);
            */
      break;
  case roomDOORFACING:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2],
            6);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,40);
      BltShapeToViewport(d.pDoorBitmaps[6], &d.DoorTrackTopRect[2]);
      DrawDoor(
            pRoomData->decorations[1],
            pRoomData->decorations[0],
            d.DoorGraphic[0],//d.Word1918,
            4224,
            2,
            d.DoorRectsF1L1);//d.Byte3754);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x39);
    */
      break;
  case roomDOOREDGE:
    /*
      DrawFloorDecorations(
            pRoomData->decorations[2],
            6);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x32);
    */
      break;
  case roomSTAIREDGE:
    /*
      if (pRoomData->decorations[0] != 0) GraphicToViewport(d.StairEdgeGraphic[2], &d.StairEdgeRect[5]);
      else GraphicToViewport(d.StairEdgeGraphic[1],&d.StairEdgeRect[3]);
      DrawFloorDecorations(
            pRoomData->decorations[2],
            6);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x32);
    */
      break;
  case roomSTONE:
    /*
      BltShapeToViewport(d.pWallBitmaps[2], &d.wallRectangles[pRoomData->relativeCellNumber]);
      DrawWallDecoration(d.NumWallDecoration+1, 10,pRoomData->text[0]);
      DrawWallDecoration(pRoomData->decorations[0], 10,RNeof);
    */
      break;
  case roomSTAIRFACING:
    /*
      if (pRoomData->decorations[0] != 0) 
           GraphicToViewport(d.StairFacingUpGraphic[2], &d.StairFacingUpRect[4]);
      else GraphicToViewport(d.StairFacingDownGraphic[2], &d.StairFacingDownRect[4]);
      DrawFloorDecorations(
            pRoomData->decorations[2],
            6);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x32);
    */
      break;
  };
}

//   TAG007d00
void DrawCellF0L1(i32 /*facing*/,SUMMARIZEROOMDATA *pRoomData)
{
  //dReg D5,D6,D7;
  //SUMMARIZEROOMDATA roomData;
  //SummarizeRoomInfo(&roomData,facing,pRoomData->x,pRoomData->y);
  switch (pRoomData->roomType)
  {
  case roomSTONE:
    /*
      BltShapeToViewport(d.pWallBitmaps[1], &d.wallRectangles[pRoomData->relativeCellNumber]);
    */
      break;
  case roomOPEN:
    /*
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            2);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
     */
      break;
  case roomDOOREDGE:
    /*
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            2);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
    */
      break;
  case roomTELEPORTER:
    /*
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            2);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      if (pRoomData->decorations[0] != 0)
              DrawTeleporter(
                    &d.teleporterRectangles[pRoomData->relativeCellNumber], 
                    &d.wallRectangles[pRoomData->relativeCellNumber]);
    */
      break;
  case roomPIT:
    /*
      GraphicToViewport(pRoomData->decorations[0] != 0? 61:55,&d.FloorPitRect[2]);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            2);
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
    */
      break;
  case roomSTAIREDGE:
    /*
      GraphicToViewport(d.StairEdgeGraphic[0], &d.StairEdgeRect[1]);
    */
      break;
  };
}

//   TAG007de8
void DrawCellF0R1(i32 /* facing*/, SUMMARIZEROOMDATA *pRoomData)
{
  //SUMMARIZEROOMDATA roomData;
  //SummarizeRoomInfo(&roomData,facing,pRoomData->x, pRoomData->y);
  switch (pRoomData->roomType)
  {
  case roomSTONE:
    /*
      BltShapeToViewport(d.pWallBitmaps[0], &d.wallRectangles[pRoomData->relativeCellNumber]);
    */
      break;
  case roomDOOREDGE:
    /*
      CheckCeilingPit(
            pRoomData->relativeCellNumber, 
            pRoomData->x, 
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing, 
            pRoomData->x, 
            pRoomData->y,
            pRoomData->relativeCellNumber,1);
    */
      break;
  case roomTELEPORTER:
    /*
      CheckCeilingPit(
            pRoomData->relativeCellNumber, 
            pRoomData->x, 
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing, 
            pRoomData->x, 
            pRoomData->y,
            pRoomData->relativeCellNumber,1);
      if (pRoomData->decorations[0] != 0) 
              DrawTeleporter(
                        &d.teleporterRectangles[pRoomData->relativeCellNumber], 
                        &d.wallRectangles[pRoomData->relativeCellNumber]);
    */
      break;
  case roomOPEN:
    /*
      CheckCeilingPit(
            pRoomData->relativeCellNumber, 
            pRoomData->x, 
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing, 
            pRoomData->x, 
            pRoomData->y,
            pRoomData->relativeCellNumber,1);
    */
      break;
  case roomPIT:
    /*
      MirrorGraphicToViewport(pRoomData->decorations[0]!=0?61:55, &d.FloorPitRect[0]);  
      CheckCeilingPit(
            pRoomData->relativeCellNumber, 
            pRoomData->x, 
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing, 
            pRoomData->x, 
            pRoomData->y,
            pRoomData->relativeCellNumber,
            1);
    */
      break;
  case roomSTAIREDGE:
    /*
      MirrorGraphicToViewport(d.StairEdgeGraphic[0],&d.StairEdgeRect[0]);
    */
      break;
  };
}

//   TAG007ed2
void DrawCellF0(i32 /*facing*/, SUMMARIZEROOMDATA *pRoomData,bool /*skipDrawing*/)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //dReg D5, D6, D7;
  //SUMMARIZEROOMDATA roomData;
  //D7W = P1; D6W = P2; D5W = P3;
  //SummarizeRoomInfo(&roomData, facing, pRoomData->x, pRoomData->y);
  switch (pRoomData->roomType)
  {
  case roomPIT:
    /*
      GraphicToViewport(pRoomData->decorations[0]!=0?62:56,&d.FloorPitRect[1]);
      if (!skipDrawing)
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x21);
    */
      break;
  case roomSTAIRFACING:
    /*
      if (pRoomData->decorations[0] != 0)
      {
        GraphicToViewport(d.StairFacingUpGraphic[0], &d.StairFacingUpRect[1]);
        MirrorGraphicToViewport(d.StairFacingUpGraphic[0], &d.StairFacingUpRect[0]);
      }
      else
      {
        GraphicToViewport(d.StairFacingDownGraphic[0], &d.StairFacingDownRect[1]);
        MirrorGraphicToViewport(d.StairFacingDownGraphic[0], &d.StairFacingDownRect[0]);
      };
      if (!skipDrawing)
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x21);
    */
      break;
  case roomDOOREDGE:
    /*
      if (d.SeeThruWalls != 0)
      {
        MemMove(d.pDoorBitmaps[1], d.Pointer1848, 1968);
        if (!skipDrawing)
        TAG0088b2(GetBasicGraphicAddress(41),
                  d.Pointer1848,
                  (RectPos *)d.uByte2542,
                  d.DoorTrackTopRect[0].b.x1-d.uByte2534[0], // This isn't right (srcOffsetX)
                  0,
                  48,
                  16,
                  9);
        BltShapeToViewport(d.Pointer1848, &d.DoorTrackTopRect[6]);
      }
      else
      {
        BltShapeToViewport(d.pDoorBitmaps[1], &d.DoorTrackTopRect[6]);
      };
      if (!skipDrawing)
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x21);
    */
      break;
  case roomTELEPORTER:
    /*
      if (!skipDrawing)
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x21);
      if (pRoomData->decorations[0] != 0)
      if (!skipDrawing)
              DrawTeleporter(
                      &d.teleporterRectangles[pRoomData->relativeCellNumber], 
                      &d.wallRectangles[pRoomData->relativeCellNumber]);
    */
      break;
  case roomOPEN:
    /*
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x21);
    */
      break;
  case roomSTONE:
    /*
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x21);
     */
      break;
  case roomDOORFACING:
    /*
      if (!skipDrawing)
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x21);
    */
      break;
  case roomSTAIREDGE:
    /*
      if (!skipDrawing)
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x21);
    */
  default:
    /*
      if (!skipDrawing)
      CheckCeilingPit(
            pRoomData->relativeCellNumber,
            pRoomData->x,
            pRoomData->y);
      DrawRoomObjects(
            pRoomData->rn2,
            facing,
            pRoomData->x,
            pRoomData->y,
            pRoomData->relativeCellNumber,
            0x21);
    */
      break;
  };
}

i8 relposSid[16] = 
{
  -2,2,-1,1,0,-2,2,-1,1,0,-1,1,0,-1,1,0
};

i8 relposFwd[16] =
{
  3,3,3,3,3,2,2,2,2,2,1,1,1,0,0,0
};

i8 DXFWD[4] = {0,1,0,-1};
i8 DYFWD[4] = {-1,0,1,0};
i8 DXSID[4] = {1,0,-1,0};
i8 DYSID[4] = {0,1,0,-1};


BACKGROUND_LIB::BACKGROUND_LIB()
{
  backgroundGraphics = NULL;
  size = 0;
  num = 0;
  sequence = 0;
}

void BACKGROUND_LIB::Cleanup()
{
  int i;
  if (backgroundGraphics != NULL) 
  {
    for (i=0; i<num; i++)
    {
      if (backgroundGraphics[i].data != NULL)
      {
        UI_free(backgroundGraphics[i].data);
        backgroundGraphics[i].data = NULL;
      };
    };
    UI_free(backgroundGraphics);
  };
  backgroundGraphics = NULL;
  size = 0;
  num = 0;
}

BACKGROUND_LIB::~BACKGROUND_LIB()
{
  Cleanup();
}


BACKGROUND_MASK *BACKGROUND_LIB::GetMask(ui32 graphicID, ui32 masknum, ui32 /*minimumSize*/)
{
  ui32 *pGraphic;
  ui32 actualSize;
  ui32 offset;
  i32 idx;
  idx = BackgroundGraphicExists(graphicID, 4, &actualSize, false);
  if (idx < 0) return NULL;
  pGraphic = (ui32 *)backgroundGraphics[idx].data;
  if (*pGraphic <= masknum) return NULL;
  if (4*masknum + 8 > actualSize) return NULL; // No room for the offset!
  offset = pGraphic[masknum+1];
  if (offset == 0) return NULL;
  if (offset >= actualSize) return NULL;       // No room for the mask itself!
  return (BACKGROUND_MASK *)((i8*)pGraphic + offset);
}

BACKGROUND_MASK *BACKGROUND_LIB::MaskExists(ui32 graphicID, ui32 masknum)
{
  ui32 *pGraphic;
  ui32 actualSize;
  ui32 offset;
  i32 idx;
  idx = BackgroundGraphicExists(graphicID, 4, &actualSize, false);
  if (idx < 0) return NULL;
  pGraphic = (ui32 *)backgroundGraphics[idx].data;
  if (*pGraphic <= masknum) return NULL;
  if (4*masknum + 8 > actualSize) return NULL; // No room for the offset!
  offset = pGraphic[masknum+1];
  if (offset == 0) return NULL;
  if (offset >= actualSize) return NULL;
  return (BACKGROUND_MASK *)((i8*)pGraphic + offset);
}


ui32 *BACKGROUND_LIB::GetBitmap(ui32 graphicID, ui32 minimumSize, ui32 *bitmapSize)
{
  i8 *result;
  i32 idx;
  ui32 actualSize;
  idx = GetBackgroundGraphic(graphicID, minimumSize, &actualSize, true);
  if (idx < 0) return NULL;
  result = (i8 *)backgroundGraphics[idx].data;
  if (actualSize < minimumSize) return NULL;
  *bitmapSize = actualSize/4; // i32 units
  return (ui32 *)result;
}

#ifdef _DEBUG
void BACKGROUND_LIB::DumpGraphic(i32 graphicID, i32 maskID, char *extension)
{
  i32 idx;
  ui32 actualSize;
  ui8 *pGraphic;
  if (!GraphicTraceActive) return;
  idx = BackgroundGraphicExists(graphicID, 0, &actualSize, false);
  if (idx < 0) return;
  pGraphic = backgroundGraphics[idx].data;
  {
    char fname[80];
    FILE *f;
    sequence++;
    if (maskID >= 0)
    {
      sprintf(fname,".\\Virtual\\Graphic_%06d_%06d_%06d.%s", graphicID, maskID, sequence, extension);
    }
    else
    {
      sprintf(fname,".\\Virtual\\Graphic_%06d_%06d.%s", graphicID, sequence, extension);
    };
    f = fopen(fname,"wb");
    if (f == NULL) return;
    fwrite(pGraphic,1,actualSize,f);
    fclose(f);
  };
}
#endif

void BACKGROUND_LIB::CreateVirtualBitmap(
                            ui32         *pGraphic, 
                            i32           srcSize, 
                            i32           virtualGraphicID, 
                            AFFINEMASK   *pMask)
{
  AFFINEMASK *pxform;
  CUSTOMBITMAP cSrcBitmap, cDstBitmap;
  ui32 dstSize;
  i32 idx;
  ui32 * pV; //The virtual graphic.
  pxform = (AFFINEMASK *)pMask;
  if ((pxform->dstWidth & 15) != 0) return;
  idx = BackgroundGraphicExists(virtualGraphicID, 2, NULL, false);
  if (idx >= 0) return;
  dstSize = pxform->dstWidth * pxform->dstHeight / 2; // bytes
  CreateBackgroundGraphic(virtualGraphicID, 4 * (dstSize+1));
  idx = GetBackgroundGraphic(virtualGraphicID, 4 * (dstSize+1), NULL, true);
  if (idx < 0) return;
  pV = (ui32 *)backgroundGraphics[idx].data;
  //pV = (ui32 *)UI_malloc(pMask->dstWidth * pMask->dstHeight / 2 + 4, MALLOC099);
  *pV = pMask->dstWidth;
  ASSERT(pMask->dstWidth > 0,"AFFINEMASK dstWidth = 0");
  if (pMask->dstWidth <= 0) return;
  //InsertBitmap(virtualGraphicID, pMask->dstWidth * pMask->dstHeight / 2 + 4, pV);
  cSrcBitmap.Initialize(
                 (ui16 *)(pGraphic),
                 4,
                 srcSize - 1);          // i32 units
  cDstBitmap.Initialize((ui16*)(pV),
                        4,
                        dstSize);            //i32 units
  AffineTransform4(&cSrcBitmap, &cDstBitmap, pxform);
#ifdef _DEBUG
  DumpGraphic(virtualGraphicID, -1, "bin");
#endif
}


BACKGROUND_MASK *BACKGROUND_LIB::CreateNewMask(ui32 id, ui32 number, BACKGROUND_MASK *pMask)
{ // size in bytes including the header.
  static bool errorMsg = false;
  ui8 *pbg;
  ui32 actualSize, n, i, gsize;
  i32 idx;
  ui32 *pMaskIndex;
  gsize = (pMask->width + 15)/16 * pMask->height * 2 + 12;
  idx = BackgroundGraphicExists(id, 0, &actualSize, false);
  if (idx < 0)
  {
    actualSize = 4;
    idx = CreateBackgroundGraphic(id, actualSize);
    *(ui32 *)backgroundGraphics[idx].data = 0;  // Number of masks
  };
  pMaskIndex = (ui32 *)backgroundGraphics[idx].data;
  if (pMaskIndex[0] <= number)
  {
    // We need to make some room for the new index.
    n = number - pMaskIndex[0] + 1; // # index entries we need to add.
    backgroundGraphics[idx].size += 4*n;
    backgroundGraphics[idx].data = (ui8 *)UI_realloc(backgroundGraphics[idx].data, 
                                                    backgroundGraphics[idx].size,
                                                    MALLOC100);
    pbg = backgroundGraphics[idx].data;
    pMaskIndex = (ui32 *)pbg;
    memmove(pbg + 4 * number + 8,
            pbg + 4 * pMaskIndex[0] + 4,
            backgroundGraphics[idx].size - 4 * number - 8);
    memset(pbg + 4 * pMaskIndex[0] + 4, 0, 4 * n);
    for (i=0; i<pMaskIndex[0]; i++)
    {
      if (pMaskIndex[i+1] != 0) pMaskIndex[i+1] += 4*n;
    };
    pMaskIndex[0] += n;
  };
  if (pMaskIndex[number +1] != 0)
  {
    if (!errorMsg)
    {
      errorMsg = true;
      UI_MessageBox("Creating a Virtual Mask\nthat already exists.", "Design Error", MESSAGE_OK);
    };
    return NULL;
  };
  // Now the index entry exists but is zero.  All we have to do is
  // set the existing entry to the current size, increase the size,
  // and copy the old mask to the newly allocated area.
  pMaskIndex[number +1] = backgroundGraphics[idx].size;
  backgroundGraphics[idx].size += gsize;
  backgroundGraphics[idx].data 
        = (ui8 *)UI_realloc(backgroundGraphics[idx].data,
                            backgroundGraphics[idx].size,
                            MALLOC101);
  pMaskIndex = (ui32 *)backgroundGraphics[idx].data;
  memcpy (backgroundGraphics[idx].data + pMaskIndex[number +1], pMask, sizeof (*pMask));//gsize);
  return (BACKGROUND_MASK *)(backgroundGraphics[idx].data + pMaskIndex[number +1]);
}

AFFINEMASK *BACKGROUND_LIB::CreateNewAffineMask(ui32 id, ui32 number, AFFINEMASK *pMask)
{ // size in bytes including the header.
  static bool errorMsg = false;
  ui8 *pbg;
  ui32 actualSize, n, i, gsize;
  i32 idx;
  ui32 *pMaskIndex;
  //UI_MessageBox("CreateNewAffineMask","info",MESSAGE_OK);
  gsize = sizeof (AFFINEMASK);
  idx = BackgroundGraphicExists(id, 0, &actualSize, false);
  if (idx < 0)
  {
    actualSize = 4;
    idx = CreateBackgroundGraphic(id, actualSize);
    *(ui32 *)backgroundGraphics[idx].data = 0;  // Number of masks
  };
  pMaskIndex = (ui32 *)backgroundGraphics[idx].data;
  if (pMaskIndex[0] <= number)
  {
    // We need to make some room for the new index.
    n = number - pMaskIndex[0] + 1; // # index entries we need to add.
    backgroundGraphics[idx].size += 4*n;
    backgroundGraphics[idx].data = (ui8 *)UI_realloc(backgroundGraphics[idx].data, 
                                                    backgroundGraphics[idx].size,
                                                    MALLOC100);
    pbg = backgroundGraphics[idx].data;
    pMaskIndex = (ui32 *)pbg;
    memmove(pbg + 4 * number + 8,
            pbg + 4 * pMaskIndex[0] + 4,
            backgroundGraphics[idx].size - 4 * number - 8);
    memset(pbg + 4 * pMaskIndex[0] + 4, 0, 4 * n);
    for (i=0; i<pMaskIndex[0]; i++)
    {
      if (pMaskIndex[i+1] != 0) pMaskIndex[i+1] += 4;
    };
    pMaskIndex[0] += n;
  };
  if (pMaskIndex[number +1] != 0)
  {
    if (!errorMsg)
    {
      errorMsg = true;
      UI_MessageBox("Creating a Virtual Affine Mask\nthat already exists.", "Design Error", MESSAGE_OK);
    };
    return NULL;
  };
  // Now the index entry exists but is zero.  All we have to do is
  // set the existing entry to the current size, increase the size,
  // and copy the old mask to the newly allocated area.
  pMaskIndex[number +1] = backgroundGraphics[idx].size;
  backgroundGraphics[idx].size += gsize;
  backgroundGraphics[idx].data 
        = (ui8 *)UI_realloc(backgroundGraphics[idx].data,
                            backgroundGraphics[idx].size,
                            MALLOC101);
  pMaskIndex = (ui32 *)backgroundGraphics[idx].data;
  memcpy (backgroundGraphics[idx].data + pMaskIndex[number +1], pMask, gsize);
  return (AFFINEMASK *)(backgroundGraphics[idx].data + pMaskIndex[number +1]);
}

void BACKGROUND_LIB::CreateVirtualMask(BACKGROUND_MASK *pOldMask,
                                       BACKGROUND_MASK *pNewMask,
                                       AFFINEMASK      *pAffine)
{
  CUSTOMBITMAP srcBitmap, dstBitmap;
  AFFINEMASK Xform;
  srcBitmap.InitializeMask(pOldMask);
  dstBitmap.InitializeMask(pNewMask);
  Xform.srcX = 0;
  Xform.srcY = 0;
  Xform.Cxc = pAffine->Cxc;
  Xform.Cxx = pAffine->Cxx;
  Xform.Cxy = pAffine->Cxy;
  Xform.Cyc = pAffine->Cyc;
  Xform.Cyx = pAffine->Cyx;
  Xform.Cyy = pAffine->Cyy;
  Xform.dstHeight = pNewMask->height;
  Xform.dstWidth  = pNewMask->width;
  Xform.srcHeight = pOldMask->height;
  Xform.srcWidth  = pOldMask->width;
  AffineTransform1(&srcBitmap, &dstBitmap, &Xform);
}

i32 BACKGROUND_LIB::GetDecorationBitmap(
                         ui32 decorationID,
                         ui32 decorationNumber,
                         ui32 locationNumber,
                         ui16 **pBitmap,
                         ui8  **pLocation,
                         ui32 *pBitmapSize)
{
  i8 *result;
  ui16 *pGraphic;
  ui32 actualSize;
  ui32 numDecoration, numLocation;
  ui32 offset;
  i32 idx, bsize;
  idx = GetBackgroundGraphic(decorationID, 4, &actualSize, true);
  if (idx < 0) return -1;
  result = (i8 *)backgroundGraphics[idx].data;
  pGraphic = (ui16 *)result;
  numDecoration = pGraphic[0];
  if (decorationNumber >= numDecoration) return -1;
  pGraphic++;
  offset = pGraphic[decorationNumber];
  if (offset >= actualSize) return -1;
  *pBitmap = (ui16 *)(result + offset);
  pGraphic += numDecoration;
  numLocation = pGraphic[0];
  if (locationNumber >= numLocation) return -1;
  pGraphic++;
  *pLocation = (ui8 *)(pGraphic + locationNumber);
  bsize = (result + actualSize) - (i8 *)(*pBitmap);
  if (bsize < 0) return -1;
  *pBitmapSize = bsize;
  return 0;
}


ui16 *BACKGROUND_LIB::GetSkinDef(ui32 skinNum,
                                 ui32 minimumSize, 
                                 ui32 *skinDefSize)
{
  ui8 *result;
  ui16 *pSkinIndex;
  ui32 actualSize;
  ui32 skinDefOffset;
  ui32 numSkins;
  i32 idx;
  // works with this return stmt. return NULL; //debugging
  idx = GetBackgroundGraphic(1, 2, &actualSize, false);
  // works with this return stmt. return NULL;//debugging
  //this stmt make it work idx = -1; //debuggin
  if (idx < 0) 
  {
    return NULL;
  };
  //works with this return stmt return NULL; //debugging
  result = backgroundGraphics[idx].data;
  pSkinIndex = (ui16 *)result;
  numSkins = pSkinIndex[0];
  if (skinNum >= numSkins) return NULL;
  if (actualSize < 2 * (1 + skinNum+1)) return NULL;
  skinDefOffset = pSkinIndex[skinNum+1];
  if (skinDefOffset == 0) return NULL;
  if (skinDefOffset + minimumSize >= actualSize) return NULL;
  *skinDefSize = actualSize - skinDefOffset;
  return (ui16 *)(result + skinDefOffset);
}


ui16 *BACKGROUND_LIB::GetCode(ui32 skinNum)
{
  ui32 codeSize;
  ui32 codeID;
  i32 idx;
  // This makes it work.return NULL;
  codeID = GetSkinID(skinNum, 8);
  //Still fails with this return.  return NULL;
  if (codeID == 0) return NULL;
  idx = GetBackgroundGraphic(
                      codeID,
                      2 * (1 + 8*21),  // startAddress + 21 functionAddresses
                      &codeSize,
                      false);
  if (idx < 0) return NULL;
  return (ui16*)backgroundGraphics[idx].data;
}

ui16 BACKGROUND_LIB::GetSkinID(ui32 skinNum, ui32 IDnum)
{
  ui16 ID;
  ui16 *pSkinDef;
  ui32 skinDefSize;
  // works with this return stmt.  return 0; //debugging
  pSkinDef = GetSkinDef(skinNum, 18, &skinDefSize);
  // fails with this return stmt return 0; //debugging
  if (pSkinDef == NULL) return 0;
  ID = pSkinDef[IDnum];
  return ID;
}

ui16 BACKGROUND_LIB::GetFloorGraphicID(ui32 skinNum)
{
  return GetSkinID(skinNum,0);
}

ui16 BACKGROUND_LIB::GetMiddleGraphicID(ui32 skinNum)
{
  return GetSkinID(skinNum,1);
}

ui16 BACKGROUND_LIB::GetCeilingGraphicID(ui32 skinNum)
{
  return GetSkinID(skinNum,2);
}

ui16 BACKGROUND_LIB::GetWallGraphicID(ui32 skinNum)
{
  return GetSkinID(skinNum,3);
}

ui16 BACKGROUND_LIB::GetWallDecorationID(ui32 skinNum, ui32 decorationNum)
{
  ui16 ID;
  ui16 *pSkinDef;
  ui32 skinDefSize;
  ui32 index, numFloorDec, numWallDec;
  pSkinDef = GetSkinDef(skinNum, 18, &skinDefSize);
  if (pSkinDef == NULL) return 0;
  index = 9;
  numFloorDec = pSkinDef[index];
  index += numFloorDec + 1;
  if (index >= skinDefSize) return 0;
  numWallDec = pSkinDef[index];
  if (decorationNum >= numWallDec) return 0;
  index += decorationNum + 1;
  if (index >= skinDefSize) return 0;
  ID = pSkinDef[index];
  return ID;
}

ui16 BACKGROUND_LIB::GetFloorMaskID(ui32 skinNum)
{
  return GetSkinID(skinNum, 4);
}

ui16 BACKGROUND_LIB::GetMiddleMaskID(ui32 skinNum)
{
  return GetSkinID(skinNum, 5);
}

ui16 BACKGROUND_LIB::GetCeilingMaskID(ui32 skinNum)
{
  return GetSkinID(skinNum, 6);
}

ui16 BACKGROUND_LIB::GetWallMaskID(ui32 skinNum)
{
  return GetSkinID(skinNum, 7);
}

void BACKGROUND_LIB::InsertBitmap(ui32 ID, ui32 gsize, ui32 *address)
{
  i32 i;
  for (i=0; i<num; i++)
  {
    if (backgroundGraphics[i].id == ID)
    {
      if (backgroundGraphics[i].data != NULL) return;
      backgroundGraphics[i].data = (ui8 *)address;
      backgroundGraphics[i].size = gsize;
      return;
    };
  };
}


i32 BACKGROUND_LIB::FindGraphicInCache(ui32 graphicID, bool mustExist)
{
  i32 i;
  ui32 graphicSize;
  for (i=0; i<num; i++)
  {
    if (backgroundGraphics[i].id == graphicID)
    {
      return i;
    };
  };
  if (num == size)
  {
    size += 10;
    backgroundGraphics = (BACKGROUND_GRAPHIC *)UI_realloc(
                                    backgroundGraphics, 
                                    size * sizeof(BACKGROUND_GRAPHIC),
                                    MALLOC092);
    for (i=num; i<size; i++)
    {
      backgroundGraphics[i].id = 0;
      backgroundGraphics[i].data = NULL;
    };
  };
  backgroundGraphics[num].data = ReadCSBgraphic(
                                            CGT_Backgrounds,
                                            graphicID,
                                            0,
                                            &graphicSize,
                                            mustExist,
                                            MALLOC123);
  if (backgroundGraphics[num].data != NULL)
  {
    backgroundGraphics[num].size = graphicSize;
    backgroundGraphics[num].id = graphicID;
    return num++;
  };
  return -1;
}

i32 BACKGROUND_LIB::BackgroundGraphicExists(ui32 graphicID, 
                                            i32 minimumSize, 
                                            ui32 *actualSize,
                                            bool mustExist)
{
  i32 i;
  i = FindGraphicInCache(graphicID, mustExist);
  if (i < 0) return -1;
  if (actualSize != NULL) *actualSize = backgroundGraphics[i].size;
  if (backgroundGraphics[i].size < minimumSize) return -1;
  return i;
}


i32 BACKGROUND_LIB::GetBackgroundGraphic(ui32 graphicID, 
                                         i32 minimumSize, 
                                         ui32 *actualSize,
                                         bool mustExist)
{
  i32 i;
  i = FindGraphicInCache(graphicID, mustExist);
  if (i < 0) 
  {
    //backgroundGraphics[num].data = NULL;
    backgroundGraphics[num].size = -1;
    backgroundGraphics[num++].id = graphicID;
    return -1;
  };
  if (actualSize != NULL) *actualSize = backgroundGraphics[i].size;
  if (backgroundGraphics[i].size < minimumSize) return -1;
  return i;
}


i32 BACKGROUND_LIB::CreateBackgroundGraphic(ui32 graphicID, 
                                             ui32 graphicSize)
{
  i32 i, idx;
  ui32 gsize;
  idx = BackgroundGraphicExists(graphicID, 0, &gsize, false);
  if (idx >= 0) return -1;
  if (num == size)
  {
    size += 10;
    backgroundGraphics = (BACKGROUND_GRAPHIC *)UI_realloc(
                                    backgroundGraphics, 
                                    size * sizeof(BACKGROUND_GRAPHIC),
                                    MALLOC092);
    for (i=num; i<size; i++)
    {
      backgroundGraphics[i].id = 0;
      backgroundGraphics[i].data = NULL;
    };
  };
  backgroundGraphics[num].id = graphicID;
  backgroundGraphics[num].data = (ui8 *)UI_malloc(graphicSize, MALLOC097);
  backgroundGraphics[num].size = graphicSize;
  return num++;
}


static ui32 LeftMask[16] = {
  0x00000000,
  0x80008000,
  0xc000c000,
  0xe000e000,
  0xf000f000,
  0xf800f800,
  0xfc00fc00,
  0xfe00fe00,
  0xff00ff00,
  0xff80ff80,
  0xffc0ffc0,
  0xffe0ffe0,
  0xfff0fff0,
  0xfff8fff8,
  0xfffcfffc,
  0xfffefffe
};

static ui32 RightMask[16] = {
  0xffffffff,
  0x7fff7fff,
  0x3fff3fff,
  0x1fff1fff,
  0x0fff0fff,
  0x07ff07ff,
  0x03ff03ff,
  0x01ff01ff,
  0x00ff00ff,
  0x007f007f,
  0x003f003f,
  0x001f001f,
  0x000f000f,
  0x00070007,
  0x00030003,
  0x00010001,
};


/*
void ApplyBackground2 (BACKGROUND_MASK *msk, 
                       ui32            *src,
                       ui32            bitmapsize)
{
  ui32 SR; //Shift Right Count
  ui32 SL; //Shift Left count = 15-SR
  i32  i,j, numFullGroups, postPixelCount;
  ui32 jj;
//  ui32 Md, nMd;
  ui8 M0, N0, M1, N1;
  ui8 *D, *S, *M;
  ui32 pixelsPerLine, dstWidth, srcWidth;
  ui32 *pSrcLine, *pDstLine;
  ui32 numPixelsInSrc, srcPixelsNeeded;
  ui16 *pMskLine;
  int srcLineIncr, dstLineIncr, mskLineIncr;
  //ui32 *pSrc;
  //ui16 *pMsk;
  numPixelsInSrc = (bitmapsize-1) * 8;
  SR = msk->dstX & 15;
  SL = 16 - SR;
  pMskLine = (ui16 *)(msk) + 6;
  pSrcLine = src + 1;
  pDstLine = (ui32 *)d.pViewportBMP;
  srcWidth = *src;
  srcLineIncr = (srcWidth/16*2) & 0xffff; //Prevent overflows on really bad data.
  src++;
  srcPixelsNeeded = srcWidth * (msk->srcY + msk->height - 1) + msk->srcX + msk->width;
  if (srcPixelsNeeded > numPixelsInSrc) return;
  pSrcLine += srcLineIncr * msk->srcY;
  pSrcLine += 2 * (msk->srcX / 16);
  dstWidth = 224;
  dstLineIncr = dstWidth >> 3;
  pDstLine += dstLineIncr * msk->dstY;
  pDstLine += 2 * (msk->dstX / 16);
  mskLineIncr = msk->width >> 4;
  pixelsPerLine = msk->width;
  if (msk->dstX + pixelsPerLine > dstWidth)
  {
    pixelsPerLine = dstWidth - msk->dstX;
  };
  postPixelCount = (pixelsPerLine - SL) & 15;
  numFullGroups = (pixelsPerLine - SL) / 16;
  //One line.
  //pSrc = first 4-word group in source.
  //pDst = first 4-word group in destination
  //pMsk = first mask word.
  //The first group has zero bits on the front of the mask.
  if (SR < 8)
  { 
    SL -= 8;
    for (i=0; i<msk->height; i++)
    {
      S = (ui8 *)pSrcLine;
      D = (ui8 *)pDstLine;
      M = (ui8 *)pMskLine;
      M0 = (ui8)(M[0] >> SR);
      M1 = (ui8)(((M[0] << 8) | M[1]) >> SR);
      N0 = (ui8)(~M0);
      N1 = (ui8)(~M1);
      for (jj=0; jj<4; jj++, D+=2, S+=2)
      {
        D[0] = (ui8)(((S[0] >> SR) & M0) | (D[0] & N0));
        D[1] = (ui8)(((((S[0]<<8)|S[1]) >> SR) & M1) | (D[1] & N1));
      };
      S -= 8;
      D -= 8;
      for (j=0; j<numFullGroups; j++, M+=2)
      {
        M0 = (ui8)(((M[1] << 8) | M[2]) >> SR);
        M1 = (ui8)(((M[2] << 8) | M[3]) >> SR);
        N0 = (ui8)(~M0);
        N1 = (ui8)(~M1);
        for (jj=0; jj<4; jj++, D+=2, S+=2)
        {
          D[8] = (ui8)(((((S[ 1]<<8)|S[ 8])>>SR)&M0)|(D[8]&N0));
          D[9] = (ui8)(((((S[ 8]<<8)|S[ 9])>>SR)&M1)|(D[9]&N1));
        };
      };
      if (postPixelCount > 0)
      {
        M0 = (ui8)(M[1] << SL);
        N0 = (ui8)(~M0);
        for (jj=0; jj<4; jj++, D+=2, S+=2)
        {
          D[8] = (ui8)(((S[1] << SL) & M0) | (D[8] & N0));
        };
      };
      pSrcLine += srcLineIncr;
      pDstLine += dstLineIncr;
      pMskLine += mskLineIncr;
    };
  }
  else if (SR > 8)
  {
    SR -= 8;
    for (i=0; i<msk->height; i++)
    {
      S = (ui8 *)pSrcLine;
      D = (ui8 *)pDstLine;
      M = (ui8 *)pMskLine;
      M1 = (ui8)(M[0] >> SR);
      N1 = (ui8)(~M1);
      for (jj=0; jj<4; jj++, D+=2, S+=2)
      {
        D[1] = (ui8)(((S[0] >> SR) & M1) | (D[1] & N1));
      };
      S -= 8;
      D -= 8;
      for (j=0; j<numFullGroups; j++, M+=2)
      {
        M0 = (ui8)(((M[0] << 8) | M[1]) >> SR);
        M1 = (ui8)(((M[1] << 8) | M[2]) >> SR);
        N0 = (ui8)(~M0);
        N1 = (ui8)(~M1);
        for (jj=0; jj<4; jj++, D+=2, S+=2)
        {
          D[8] = (ui8)(((((S[ 0]<<8)|S[ 1])>>SR)&M0)|(D[8]&N0));
          D[9] = (ui8)(((((S[ 1]<<8)|S[ 8])>>SR)&M1)|(D[9]&N1));
        };
      };
      if (postPixelCount > 0)
      {
        M0 = (ui8)(((M[0] << 8) | M[1]) >> SR);
        M1 = (ui8)(M[1] << SL);
        N0 = (ui8)(~M0);
        N1 = (ui8)(~M1);
        for (jj=0; jj<4; jj++, D+=2, S+=2)
        {
          D[8] = (ui8)(((((S[ 0]<<8)|S[ 1])>>SR)&M0)|(D[8]&N0));
          D[9] = (ui8)(((((S[ 1]<<8)|S[ 8])>>SR)&M1)|(D[9]&N1));
        };
      };
      pSrcLine += srcLineIncr;
      pDstLine += dstLineIncr;
      pMskLine += mskLineIncr;
    };
  }
  else
  {
    for (i=0; i<msk->height; i++)
    {
      S = (ui8 *)pSrcLine;
      D = (ui8 *)pDstLine;
      M = (ui8 *)pMskLine;
      M1 = M[0];
      N1 = (ui8)(~M1);
      for (jj=0; jj<4; jj++, D+=2, S+=2)
      {
        D[1] = (ui8)((S[0] & M1) | (D[1] & N1));
      };
      S -= 8;
      D -= 8;
      for (j=0; j<numFullGroups; j++, M+=2)
      {
        M0 = (ui8)M[1];
        M1 = (ui8)M[2];
        N0 = (ui8)(~M0);
        N1 = (ui8)(~M1);
        for (jj=0; jj<4; jj++, D+=2, S+=2)
        {
          D[8] = (ui8)((S[ 1]&M0)|(D[8]&N0));
          D[9] = (ui8)((S[ 8]&M1)|(D[9]&N1));
        };
      };
      if (postPixelCount > 0)
      {
        M0 = (ui8)M[1];
        N0 = (ui8)(~M0);
        for (jj=0; jj<4; jj++, D+=2, S+=2)
        {
          D[8] = (ui8)((S[1] & M0) | (D[8] & N0));
        };
      };
      pSrcLine += srcLineIncr;
      pDstLine += dstLineIncr;
      pMskLine += mskLineIncr;
    };
  };
}
*/

void ApplyBackground3 (BACKGROUND_MASK *msk, 
                       ui32            *src,
                       ui32            bitmapsize)
{
  ui32 SSR; //Shift Source Right Count
  ui32 SSL; //Shift Source Left count = 15-SSR
  ui32 SMR; //Shift Mask Right Count
  ui32 SML; //Shift Mask Left Count = 15-SMR
  ui32 sBit, dBit;
  i32  i,j, numFullGroups, prePixelCount, postPixelCount;
  i32 negativeWords;
  ui32 jj;
//  ui32 Md, nMd;
  ui8 M0, N0, M1, N1;
  ui8 *D, *S, *M;
  ui32 pixelsPerLine, dstWidth, srcWidth;
  ui32 *pSrcLine, *pDstLine;
  ui32 numPixelsInSrc, srcPixelsNeeded;
  ui16 *pMskLine;
  int srcLineIncr, dstLineIncr, mskLineIncr;
  //ui32 *pSrc;
  //ui16 *pMsk;
  sBit = msk->srcX & 15;
  if (msk->dstX < 0) 
  {
    negativeWords = (-msk->dstX + 15) / 16; // # words negative
  }
  else
  {
    negativeWords = 0;
  };
  dBit = (msk->dstX + negativeWords * 16) & 15;
  numPixelsInSrc = (bitmapsize-1) * 8;
  SMR = dBit;
  SML = 16 - SMR;
  SSR = dBit - sBit;
  if (sBit > dBit) SSR += 16;
  SSL = 16 - SSR;
  prePixelCount = dBit;
  pMskLine = (ui16 *)(msk) + 6;
  pSrcLine = src + 1;
  pDstLine = (ui32 *)d.pViewportBMP;
  srcWidth = *src;
  srcLineIncr = (srcWidth/16*2) & 0xffff; //Prevent overflows on really bad data.
  //src++;
  srcPixelsNeeded = srcWidth * (msk->srcY + msk->height - 1) + msk->srcX + msk->width;
  if (srcPixelsNeeded > numPixelsInSrc) return;
  pSrcLine += srcLineIncr * msk->srcY;
  pSrcLine += 2 * (msk->srcX / 16);
  dstWidth = 224;
  dstLineIncr = dstWidth >> 3;
  pDstLine += dstLineIncr * msk->dstY;
  if (negativeWords != 0)
  {
    pDstLine -= 2 * negativeWords;
  }
  else
  {
    pDstLine += 2 * (msk->dstX / 16);
  };
  mskLineIncr = msk->width >> 4;
  pixelsPerLine = msk->width;
  if (msk->dstX + pixelsPerLine > dstWidth)
  {
    pixelsPerLine = dstWidth - msk->dstX;
  };
  postPixelCount = (pixelsPerLine - SML) & 15;
  numFullGroups = (pixelsPerLine - SML) / 16;
  //One line.
  //pSrc = first 4-word group in source.
  //pDst = first 4-word group in destination
  //pMsk = first mask word.
  //The first group has zero bits on the front of the mask.
  if (negativeWords) 
  {
    numFullGroups -= negativeWords - (prePixelCount?1:0);
    prePixelCount = 0;
    pMskLine += negativeWords;
    pDstLine += negativeWords*2;
    pSrcLine += negativeWords*2;
  };
  if (SMR == 0)
  { 
    SSL -= 8;
    for (i=0; i<msk->height; i++)
    {
      S = (ui8 *)pSrcLine;
      D = (ui8 *)pDstLine;
      M = (ui8 *)pMskLine;
      for (j=0; j<=numFullGroups; j++, M+=2)
      {
        M0 = M[0];
        M1 = M[1];
        N0 = (ui8)(~M0);
        N1 = (ui8)(~M1);
        for (jj=0; jj<4; jj++, D+=2, S+=2)
        {
          D[0] = (ui8)(((((S[ 1]<<8)|S[ 8])>>SSR)&M0)|(D[0]&N0));
          D[1] = (ui8)(((((S[ 8]<<8)|S[ 9])>>SSR)&M1)|(D[1]&N1));
        };
      };
      if (postPixelCount > 0)
      {
        M0 = M[1];
        N0 = (ui8)(~M0);
        for (jj=0; jj<4; jj++, D+=2, S+=2)
        {
          D[0] = (ui8)(((S[1] << SSL) & M0) | (D[0] & N0));
        };
      };
      pSrcLine += srcLineIncr;
      pDstLine += dstLineIncr;
      pMskLine += mskLineIncr;
    };
  }
  else if (SMR < 8)
  { 
    SML -= 8;
    if (SSR < 8)
    {  // SMR<8  SSR<8
      SSL -= 8;
      for (i=0; i<msk->height; i++)
      {
        S = (ui8 *)pSrcLine;
        D = (ui8 *)pDstLine;
        M = (ui8 *)pMskLine;
        if (prePixelCount != 0)
        {
          M0 = (ui8)(M[0] >> SMR);
          M1 = (ui8)(((M[0] << 8) | M[1]) >> SMR);
          N0 = (ui8)(~M0);
          N1 = (ui8)(~M1);
          M += 2;
          for (jj=0; jj<4; jj++, D+=2, S+=2)
          {
            D[0] = (ui8)(((S[0] >> SSR) & M0) | (D[0] & N0));
            D[1] = (ui8)(((((S[0]<<8)|S[1]) >> SSR) & M1) | (D[1] & N1));
          };
        };
        S -= 8;
        D -= 8;
        M -= 2;
        for (j=0; j<numFullGroups; j++, M+=2)
        {
          M0 = (ui8)(((M[1] << 8) | M[2]) >> SMR);
          M1 = (ui8)(((M[2] << 8) | M[3]) >> SMR);
          N0 = (ui8)(~M0);
          N1 = (ui8)(~M1);
          for (jj=0; jj<4; jj++, D+=2, S+=2)
          {
            D[8] = (ui8)(((((S[ 1]<<8)|S[ 8])>>SSR)&M0)|(D[8]&N0));
            D[9] = (ui8)(((((S[ 8]<<8)|S[ 9])>>SSR)&M1)|(D[9]&N1));
          };
        };
        if (postPixelCount > 0)
        {
          M0 = (ui8)(M[1] << SML);
          N0 = (ui8)(~M0);
          for (jj=0; jj<4; jj++, D+=2, S+=2)
          {
            D[8] = (ui8)(((S[1] << SSL) & M0) | (D[8] & N0));
          };
        };
        pSrcLine += srcLineIncr;
        pDstLine += dstLineIncr;
        pMskLine += mskLineIncr;
      };
    }
    else
    { // SMR<8  SSR>=8
/*      for (i=0; i<msk->height; i++)
      {
        S = (ui8 *)pSrcLine;
        D = (ui8 *)pDstLine;
        M = (ui8 *)pMskLine;
        M0 = (ui8)(M[0] >> SR);
        M1 = (ui8)(((M[0] << 8) | M[1]) >> SR);
        N0 = (ui8)(~M0);
        N1 = (ui8)(~M1);
        for (jj=0; jj<4; jj++, D+=2, S+=2)
        {
          D[0] = (ui8)(((S[0] >> SR) & M0) | (D[0] & N0));
          D[1] = (ui8)(((((S[0]<<8)|S[1]) >> SR) & M1) | (D[1] & N1));
        };
        S -= 8;
        D -= 8;
        for (j=0; j<numFullGroups; j++, M+=2)
        {
          M0 = (ui8)(((M[1] << 8) | M[2]) >> SR);
          M1 = (ui8)(((M[2] << 8) | M[3]) >> SR);
          N0 = (ui8)(~M0);
          N1 = (ui8)(~M1);
          for (jj=0; jj<4; jj++, D+=2, S+=2)
          {
            D[8] = (ui8)(((((S[ 1]<<8)|S[ 8])>>SR)&M0)|(D[8]&N0));
            D[9] = (ui8)(((((S[ 8]<<8)|S[ 9])>>SR)&M1)|(D[9]&N1));
          };
        };
        if (postPixelCount != 0)
        {
          M0 = (ui8)(M[1] << SL);
          N0 = (ui8)(~M0);
          for (jj=0; jj<4; jj++, D+=2, S+=2)
          {
            D[8] = (ui8)(((S[1] << SL) & M0) | (D[8] & N0));
          };
        };
        pSrcLine += srcLineIncr;
        pDstLine += dstLineIncr;
        pMskLine += mskLineIncr;
      };
      NotImplemented(0xcc12);
    */
    };
  }
  else
  { // SMR>=8
    SMR -= 8;
    if (SSR < 8)
    { // SMR>=8  SSR<8
      //NotImplemented(0xc561);
      ///*
      for (i=0; i<msk->height; i++)
      {
        S = (ui8 *)pSrcLine;
        D = (ui8 *)pDstLine;
        M = (ui8 *)pMskLine;
        M1 = (ui8)(M[0] >> SMR);
        N1 = (ui8)(~M1);
        for (jj=0; jj<4; jj++, D+=2, S+=2)
        {
          D[1] = (ui8)(((S[0] >> SSR) & M1) | (D[1] & N1));
        };
        S -= 8;
        D -= 8;
        for (j=0; j<numFullGroups; j++, M+=2)
        {
          M0 = (ui8)(((M[0] << 8) | M[1]) >> SMR);
          M1 = (ui8)(((M[1] << 8) | M[2]) >> SMR);
          N0 = (ui8)(~M0);
          N1 = (ui8)(~M1);
          for (jj=0; jj<4; jj++, D+=2, S+=2)
          {
            D[8] = (ui8)(((((S[ 0]<<8)|S[ 1])>>SSR)&M0)|(D[8]&N0));
            D[9] = (ui8)(((((S[ 1]<<8)|S[ 8])>>SSR)&M1)|(D[9]&N1));
          };
        };
        if (postPixelCount != 0)
        {
          M0 = (ui8)(((M[0] << 8) | M[1]) >> SMR);
          M1 = (ui8)(M[1] << SML);
          N0 = (ui8)(~M0);
          N1 = (ui8)(~M1);
          for (jj=0; jj<4; jj++, D+=2, S+=2)
          {
            D[8] = (ui8)(((((S[ 0]<<8)|S[ 1])>>SSR)&M0)|(D[8]&N0));
            D[9] = (ui8)(((((S[ 1]<<8)|S[ 8])>>SSR)&M1)|(D[9]&N1));
          };
        };
        pSrcLine += srcLineIncr;
        pDstLine += dstLineIncr;
        pMskLine += mskLineIncr;
      };
      //*/
    }
    else
    { //SMR>=8   SSR>=8
      SSR -= 8;
      for (i=0; i<msk->height; i++)
      {
        S = (ui8 *)pSrcLine;
        D = (ui8 *)pDstLine;
        M = (ui8 *)pMskLine;
        if (prePixelCount != 0)
        {
          M1 = (ui8)(M[0] >> SMR);
          N1 = (ui8)(~M1);
          M += 2;
          for (jj=0; jj<4; jj++, D+=2, S+=2)
          {
            D[1] = (ui8)(((S[0] >> SSR) & M1) | (D[1] & N1));
          };
        };
        S -= 8;
        D -= 8;
        M -= 2;
        for (j=0; j<numFullGroups; j++, M+=2)
        {
          M0 = (ui8)(((M[0] << 8) | M[1]) >> SMR);
          M1 = (ui8)(((M[1] << 8) | M[2]) >> SMR);
          N0 = (ui8)(~M0);
          N1 = (ui8)(~M1);
          for (jj=0; jj<4; jj++, D+=2, S+=2)
          {
            D[8] = (ui8)(((((S[ 0]<<8)|S[ 1])>>SSR)&M0)|(D[8]&N0));
            D[9] = (ui8)(((((S[ 1]<<8)|S[ 8])>>SSR)&M1)|(D[9]&N1));
          };
        };
        if (postPixelCount != 0)
        {
          M0 = (ui8)(((M[0] << 8) | M[1]) >> SMR);
          M1 = (ui8)(M[1] << SML);
          N0 = (ui8)(~M0);
          N1 = (ui8)(~M1);
          for (jj=0; jj<4; jj++, D+=2, S+=2)
          {
            D[8] = (ui8)(((((S[ 0]<<8)|S[ 1])>>SSR)&M0)|(D[8]&N0));
            D[9] = (ui8)(((((S[ 1]<<8)|S[ 8])>>SSR)&M1)|(D[9]&N1));
          };
        };
        pSrcLine += srcLineIncr;
        pDstLine += dstLineIncr;
        pMskLine += mskLineIncr;
      };
    };
  };
}



void ApplyBackground (BACKGROUND_MASK *mask, 
                      ui32            *bitmap,
                      ui32             bitmapsize)
{
  ui16 *pMask;
  ui32 m, n, width, height, col, row;
  ui32 *dst, *src, *pDstRow, *pSrcRow;
  ui32 srcX, srcY, dstX, dstY, bitmapWidth;
  if (mask ==NULL) return;
  if (bitmap == NULL) return;
  if (mask->dstX > 224) return;
  if (((mask->width)  & 15) != 0) return;
  //if ((mask->width | mask->srcX)  & 15 != 0) return;
  if (((mask->dstX & 15) | (mask->srcX & 15)) != 0) 
  {
    ApplyBackground3(mask, bitmap, bitmapsize);
    return;
  };
  srcX = mask->srcX/16*2;  // in i32 units
  srcY = mask->srcY;
  dstX = mask->dstX/16*2;  // in i32 units
  dstY = mask->dstY;
  width = mask->width/16*2;// in i32 Units
  height = mask->height;
  bitmapWidth = (*bitmap/16*2) & 0xffff; //Prevent overflows on really bad data.
  bitmap++;
  if (28 * (dstY+height-1) + dstX + width > 3808)
  {
    return;  //won't fit in viewport
  };
  if (bitmapWidth * (srcY+height-1) + srcX + width > bitmapsize)
  {
    return;  // not enough source data
  };
  pDstRow = (ui32 *)d.pViewportBMP + 28*dstY          + dstX;
  pSrcRow = bitmap                 + bitmapWidth*srcY + srcX;
  pMask = &mask->mask[0];
  for (row=0; row<height; row++)
  {
    dst = pDstRow;
    src = pSrcRow;
    for (col=0; col<width; col+=2)
    {
      m = *pMask | (*pMask << 16);
      n = ~m;
      dst[0] = (dst[0] & n) | (src[0] & m);
      dst[1] = (dst[1] & n) | (src[1] & m);
      dst += 2;
      src += 2;
      pMask++;
    };
    pDstRow += 28;           // destination width
    pSrcRow += bitmapWidth;  // source width
  };
}

void ApplyDecoration (ui16            *pDecoration,
                      ui8             *location,
                      ui32             bitmapsize)
{
  ui32 width, height, col, row;
  ui16 m, n;
  ui16 *dst, *pDstRow, *pSrcRow;
  ui32 dstX, dstY;
  if (pDecoration == NULL) return;
  width = *(ui8 *)pDecoration;
  height = *(((ui8 *)pDecoration)+1);
  if ((width & 15) != 0) return;
  dstX = location[0]/16*4;  // in i16 units
  dstY = location[1];
  if (28 * (dstY+height-1) + dstX + width > 3808)
  {
    return;  //won't fit in viewport
  };
  if (((width/2 + width/8) * height) > bitmapsize)
  {
    return;  // not enough source data
  };
  pDstRow = (ui16 *)d.pViewportBMP + 56*dstY          + dstX;
  pSrcRow = pDecoration + 1;
  width = width/16; //16-pixel units
  for (row=0; row<height; row++)
  {
    dst = pDstRow;
    for (col=0; col<width; col+=2)
    {
      m = pSrcRow[0];
      n = (ui16)(~m);
      dst[0] = (ui16)((dst[0] & n) | (pSrcRow[1] & m));
      dst[1] = (ui16)((dst[1] & n) | (pSrcRow[2] & m));
      dst[2] = (ui16)((dst[2] & n) | (pSrcRow[3] & m));
      dst[3] = (ui16)((dst[3] & n) | (pSrcRow[4] & m));
      dst += 4;
      pSrcRow += 5;
    };
    pDstRow += 56;           // destination width
  };
}


// ------------------------------------
// |      |      |      |      |      |
// |  0   |  4   |  6   |  5   |  1   |
// |      |      |      |      |      |
// ------------------------------------
// |      |      |      |      |      |
// |  2   |  7   |  9   |  8   |  3   |
// |      |      |      |      |      |
// ------------------------------------
// |      |      |      |      |      |
// |  10  |  12  |  14  |  13  |  11  |
// |      |      |      |      |      |
// ------------------------------------
//        |      |      |      |
//        |  15  |  17  |  16  |
//        |      |      |      |
//        ----------------------
//        |      |      |      |
//        |  18  |  20  |  19  |
//        |      |      |      |
//        ----------------------
//
#ifndef newgraphics
void CustomBackgrounds(i32 px, i32 py, i32 facing, i32 roomNum)
{
  i32 x, y;
  i32 skinNum; 
  ui16 *pSkinDef;
  ui32 skinDefSize;
  BACKGROUND_MASK *pMask;
  ui32 *pBitmap, bitmapSize;
  //pSkins = backgroundLib.GetSkins(d.LoadedLevel);  

  x = px+DXSID[facing]*relposSid[roomNum] + DXFWD[facing]*relposFwd[roomNum];
  y = py+DYSID[facing]*relposSid[roomNum] + DYFWD[facing]*relposFwd[roomNum];
  if (    (x >= d.width)
       || (y >= d.height)
       || (x < 0)
       || (y < 0)) return;
  //skinNum = pSkins[d.width*y + x];
  skinNum = skinCache.GetSkin(d.LoadedLevel, x, y);
  if (skinNum == 0)
  {
    skinNum = skinCache.GetDefaultSkin(d.LoadedLevel);
    if (skinNum == 0) return;
  };
  pSkinDef = backgroundLib.GetSkinDef(skinNum, 18, &skinDefSize);
  if (pSkinDef == NULL) return;
  if ((pSkinDef[0] != 0) && (pSkinDef[4] != 0))
  {
    pMask = backgroundLib.GetMask(pSkinDef[4], roomNum, 64);
    pBitmap = backgroundLib.GetBitmap(pSkinDef[0], 7840, &bitmapSize);
    ApplyBackground(pMask, pBitmap, bitmapSize);
  };

  if ((pSkinDef[2] != 0) && (pSkinDef[6] != 0))
  {
    pMask = backgroundLib.GetMask(pSkinDef[6], roomNum, 64);
    pBitmap = backgroundLib.GetBitmap(pSkinDef[2], 3248, &bitmapSize);
    ApplyBackground(pMask, pBitmap, bitmapSize);
  };

  if (roomNum<5)
  {
    if ((pSkinDef[1] != 0) && (pSkinDef[5] != 0))
    {
      pMask = backgroundLib.GetMask(pSkinDef[5], roomNum, 20);
      pBitmap = backgroundLib.GetBitmap(pSkinDef[1], 4144, &bitmapSize);
      ApplyBackground(pMask, pBitmap, bitmapSize);
    };
  };
}
#endif

void FillViewportCanvas(VIEWPORTCANVAS * /*pvc*/, i32 /*x*/, i32 /*y*/, i32 /*facing*/)
{
}
/*
  ui16 *pSkinDef;
  i32 roomNum;
  for (roomNum = 0; roomNum<NUMCELL; roomNum++)
  {
    x = px+DXSID[facing]*relposSid[roomNum] + DXFWD[facing]*relposFwd[roomNum];
    y = py+DYSID[facing]*relposSid[roomNum] + DYFWD[facing]*relposFwd[roomNum];
    if (    (x >= d.width)
         || (y >= d.height)
         || (x < 0)
         || (y < 0))
    {
      pvc->cellType[roomNum] = roomSTONE;
      pvc->x[roomNum] = 0;
      pvc->y[roomNum] = 0;
      pvc->skinNum[roomNum] = 0;
      pvc->computationResults = NULL;
      pvc->pInstructions = NULL;
    }
    else
    {
      pvc->x[roomNum] = x;
      pvc->y[roomNum] = y;
      pvc->cellType[roomNum] = GetCellFlags(x, y);
      pvc->skinNum[roomNum] = skinCache.GetSkin(d.LoadedLevel, x, y);
      pSkinDef = backgroundLib.GetSkinDef(skinNum);
      pvc->pInstructions = backgroundLib.

  //skinNum = pSkins[d.width*y + x];
  if (skinNum == 0) return;

*/


//    Order of drawing walls (and wall numbers)
//      (User Cell Numbers)
//      [Relative Cell Numbers]
//  +----------+----------+-----------+-----------+----------+
//  |          |          |           |           |          | 
//  |          |          |           |           |          |
//  |   (0)    |   (4)    |   (6)     |    (5)    |   (1)    |
//  |          |   [-2]   |   [-3]    |   [-1]    |          |
//  |          |          |           |           |          |
//  |          |          |           |           |          |
//  +----------+----------+-----------+-----------+----------+
//  |          |          |           |           |          |
//  |          |          |           |           |          |
//  |   (2)    |   (7)    |    (9)    |    (8)    |   (3)    |
//  |          |   [0]    |0   [1]   1|    [2]    |          |
//  |          |          |           |           |          |
//  |          |          |           |           |          |
//  +----------+----------+-----------+-----------+----------+
//  |          |    2     |     3     |     4     |          | 
//  |          |          |           |           |          |
//  |   (10)   |   (12)   |    (14)   |    (13)   |   (11)   |
//  |          |   [3]    |5   [4]   6|    [5]    |          |
//  |          |          |           |           |          |
//  |          |          |           |           |          |
//  +----------+----------+-----------+-----------+----------+
//             |    7     |     8     |     9     | 
//             |          |           |           |
//             |   (15)   |10  (17) 11|    (16)   |
//             |   [6]    |    [7]    |    [8]    |
//             |          |           |           |
//             +----------+-----------+-----------+
//             |          |    12     |           |
//             |   (18)   |    (20)   |    (19)   |
//             |   [10]   |    [9]    |    [11]   |
//             |          |           |           |
//             |          |           |           |
//             +----------+-----------+-----------+

//   TAG00803a
void DrawViewport(i32 facing,i32 x,i32 y)
{
  SUMMARIZEROOMDATA roomData[21];
  static ui32 viewportRandom = 0x34175432;
  i32  userCellNum;
  ui16 *pCode;
  ui32 saveRandom;
  ui32 saveNumRan;
  i32 mainOffset;
  ui16 startOffset;
  CGEDEBUG_3("Draw Viewport x=%d,y=%d,facing=%d\n",x,y,facing);
  saveRandom = d.RandomNumber; // Don't upset the game
  saveNumRan = numRandomCalls;
  d.RandomNumber = viewportRandom;
  bool skipDrawing;
  skipDrawing = (VBLMultiplier != 1) && ((d.Time & 0xf) != 0) && (VBLMultiplier!=99);
//#ifdef _VERIFYPPQ
//  verifyppq();
//#endif

#ifdef _DEBUG
  if ( (traceViewportDrawing != 0) && !GraphicTraceActive)
  {
    if (OpenGraphicTraceFile())
    {
      GraphicTraceActive = true;
    };
  };
#endif

  GTRACE_1("Start DrawViewport at time=%d\n",d.Time);
  DrawViewportCount++;
  //i32 cellY;
  //i32 cellX;
  dReg D0, D4;
  D4W=0;
  if (d.Word11684 != 0)
  {
    FloorAndCeilingOnly();
  };
  d.UseByteCoordinates = 1;
  // Clear all the object buttons.
  memset(&d.ViewportObjectButtons[0], 0, 24);
  fillWithByte((ui8 *)&d.ViewportObjectButtons[0],6,ui8(0xff), 4);
  AllocateTempBitmap(8880);
  //d.Pointer1848=allocateMemory(8880, 0);
  d.UseMirrorImages = (I16)((x + y + facing)&1);
  if (d.UseMirrorImages != 0)
  { //Mirror Floor
    if (!skipDrawing) Mirror(d.pViewportFloor, 0x0070, 0x0046); //height=0x46
    D4W = 1;//d.BigMemory;
    d.pWallBitmaps[4] = (ui8 *)d.Pointer2214;
    d.pWallBitmaps[3] = (ui8 *)d.Pointer2218;
    d.pWallBitmaps[2] = (ui8 *)d.Pointer2222;
    d.pWallBitmaps[1] = (ui8 *)d.Pointer2226;
    d.pWallBitmaps[0] = (ui8 *)d.Pointer2230;
  }
  else
  { //Mirror Ceiling
    if (!skipDrawing) Mirror(d.pViewportBMP, 0x0070, 0x001d); //ceiling
  };

//D0W = GetCelltypeRelative(facing, 3, -2, x, y);
//if (D0W==roomSTONE)
//{
//  BltShapeToViewport(d.pWallBitmaps[5], &d.wallRectangles[13]);
//};
  d.FacingWaterFountain = d.FacingViAltar = d.FacingAlcove = 0;
  for (userCellNum = 0; userCellNum < 21; userCellNum++)
  {
    roomData[userCellNum].x = x;
    roomData[userCellNum].y = y;
    RelativeXY(facing, 
               relativeY[userCellNum], 
               relativeX[userCellNum], 
               &roomData[userCellNum].x,
               &roomData[userCellNum].y);
    roomData[userCellNum].skinNumber = 
                        skinCache.GetSkin(d.LoadedLevel, 
                                          roomData[userCellNum].x,
                                          roomData[userCellNum].y);
    if (roomData[userCellNum].skinNumber == 0)
    {
      roomData[userCellNum].skinNumber = 
                        skinCache.GetDefaultSkin(d.LoadedLevel);
    };
    SummarizeRoomInfo(&roomData[userCellNum], facing);
    roomData[userCellNum].relativeCellNumber = userCellNum;
    if (userCellNum == 17)  
    {
      d.CellTypeJustAhead = sw(roomData[userCellNum].roomType);
    };
  };
  for (userCellNum = 0; userCellNum < 21; userCellNum++)
  {

    if (roomData[userCellNum].skinNumber != 0)
    { // Execute user-supplied code
      pCode = backgroundLib.GetCode(roomData[userCellNum].skinNumber);
//      if (userCellNum > 13) continue;
      if (pCode != NULL)
      {
        mainOffset = pCode[0];
        startOffset = pCode[mainOffset + 8*userCellNum + roomData[userCellNum].graphicRoomType];
        CGEDEBUG_1("drawing cell %d\n", userCellNum)
        GTRACE_2("  Draw Cell %d skin %d\n",userCellNum, roomData[userCellNum].skinNumber);
#ifdef _DEBUG
        Interpret(pCode, startOffset, roomData, userCellNum, facing, userCellNum == 7);
#else
        Interpret(pCode, startOffset, roomData, userCellNum, facing);
#endif
      };
    }
    else
    {
      Interpret(pStdDrawCode[userCellNum][roomData[userCellNum].graphicRoomType], 
                0,
                roomData, 
                userCellNum,
                facing
#ifdef _DEBUG
                ,false
#endif
                );
    };
  };
#ifndef newgraphics  
  roomData[RF3L2].x = x;
  roomData[RF3L2].y = y;
  RelativeXY(facing, 3, -2, &roomData[RF3L2].x, &roomData[RF3L2].y);
  roomData[RF3L2].relativeCellNumber = RF3L2;
  SummarizeRoomInfo(&roomData[RF3L2], facing);
  if (pStdDrawCode[RF3L2][roomData[RF3L2].graphicRoomType] != NULL)
  {
    Interpret(pStdDrawCode[RF3L2][roomData[RF3L2].graphicRoomType],roomData, facing);
  }
  else
  {
    DrawCellF3L2(facing, &roomData[RF3L2]);
  };
  

//D0W=GetCelltypeRelative(facing, 3, 2, x, y);
//if (D0W==roomSTONE)
//{
//  BltShapeToViewport(d.pWallBitmaps[6], &d.wallRectangles[12]);
//};

  roomData[RF3R2].x = x;
  roomData[RF3R2].y = y;
  RelativeXY(facing, 3, 2, &roomData[RF3R2].x, &roomData[RF3R2].y);
  roomData[RF3R2].relativeCellNumber = RF3R2;
  SummarizeRoomInfo(&roomData[RF3R2], facing);
  if (pStdDrawCode[RF3R2][roomData[RF3R2].graphicRoomType] != NULL)
  {
    Interpret(pStdDrawCode[RF3R2][roomData[RF3R2].graphicRoomType],roomData, facing);
  }
  else
  {
    DrawCellF3R2(facing, &roomData[RF3R2]);
  };


  if (!skipDrawing)
  {
    //cellX=x;
    //cellY=y;
    //RelativeXY(facing, 4, -1, &cellX, &cellY);
    //DrawRoomObjects(FindFirstDrawableObject(cellX,cellY),facing,cellX,cellY,-2,1);
    roomData[RF4L1].x = x;
    roomData[RF4L1].y = y;
    RelativeXY(facing, 4, -1, &roomData[RF4L1].x, &roomData[RF4L1].y);
    roomData[RF4L1].relativeCellNumber = RF4L1;
    SummarizeRoomInfo(&roomData[RF4L1], facing);
    if (pStdDrawCode[RF4L1][roomData[RF4L1].graphicRoomType] != NULL)
    {
      Interpret(pStdDrawCode[RF4L1][roomData[RF4L1].graphicRoomType],roomData, facing);
    }
    else
    {
      DrawCellF4L1(facing, &roomData[RF4L1]);
    };
  };
  if (!skipDrawing)
  {
    //cellX = x;
    //cellY = y;
    //RelativeXY(facing, 4, 1, &cellX, &cellY);
    //DrawRoomObjects(FindFirstDrawableObject(cellX, cellY), facing,cellX, cellY,-1, 1);
    roomData[RF4R1].x = x;
    roomData[RF4R1].y = y;
    RelativeXY(facing, 4, 1, &roomData[RF4R1].x, &roomData[RF4R1].y);
    roomData[RF4R1].relativeCellNumber = RF4R1;
    SummarizeRoomInfo(&roomData[RF4R1], facing);
    if (pStdDrawCode[RF4R1][roomData[RF4R1].graphicRoomType] != NULL)
    {
      Interpret(pStdDrawCode[RF4R1][roomData[RF4R1].graphicRoomType],roomData, facing);
    }
    else
    {
      DrawCellF4R1(facing, &roomData[RF4R1]);
    };
  };
  if (!skipDrawing)
  {
    //cellX = x;
    //cellY = y;
    //RelativeXY(facing,4,0,&cellX,&cellY);
    //DrawRoomObjects(FindFirstDrawableObject(cellX,cellY), facing, cellX, cellY, -3, 1);
    roomData[RF4].x = x;
    roomData[RF4].y = y;
    RelativeXY(facing, 4, 0, &roomData[RF4].x, &roomData[RF4].y);
    roomData[RF4].relativeCellNumber = RF4;
    SummarizeRoomInfo(&roomData[RF4], facing);
    if (pStdDrawCode[RF4][roomData[RF4].graphicRoomType] != NULL)
    {
      Interpret(pStdDrawCode[RF4][roomData[RF4].graphicRoomType],roomData, facing);
    }
    else
    {
      DrawCellF4(facing, &roomData[RF4]);
    };
  };
  if (!skipDrawing)
  {
    CustomBackgrounds(x, y, facing, 0);
    CustomBackgrounds(x, y, facing, 2);
    //cellX = x;
    //cellY = y;
    //RelativeXY(facing,3,-1,&cellX,&cellY);
    roomData[RF3L1].x = x;
    roomData[RF3L1].y = y;
    RelativeXY(facing, 3, -1, &roomData[RF3L1].x, &roomData[RF3L1].y);
    roomData[RF3L1].relativeCellNumber = RF3L1;
    SummarizeRoomInfo(&roomData[RF3L1], facing);
    if (pStdDrawCode[RF3L1][roomData[RF3L1].graphicRoomType] != NULL)
    {
      Interpret(pStdDrawCode[RF3L1][roomData[RF3L1].graphicRoomType],roomData, facing);
    }
    else
    {
      DrawCellF3L1(facing, &roomData[RF3L1]);
    };
  };
  if (!skipDrawing)
  {
    CustomBackgrounds(x, y, facing, 1);
    CustomBackgrounds(x, y, facing, 3);
    //cellY= y;
    //cellX=x;
    //RelativeXY(facing,3,1,&cellX,&cellY);
    roomData[RF3R1].x = x;
    roomData[RF3R1].y = y;
    RelativeXY(facing, 3, 1, &roomData[RF3R1].x, &roomData[RF3R1].y);
    roomData[RF3R1].relativeCellNumber = RF3R1;
    SummarizeRoomInfo(&roomData[RF3R1], facing);
    if (pStdDrawCode[RF3R1][roomData[RF3R1].graphicRoomType] != NULL)
    {
      Interpret(pStdDrawCode[RF3R1][roomData[RF3R1].graphicRoomType],roomData, facing);
    }
    else
    {
      DrawCellF3R1(facing,&roomData[RF3R1]);
    };
  };
  if (!skipDrawing)
  {
    CustomBackgrounds(x, y, facing, 4);
    //cellX=x;
    //cellY=y;
    //RelativeXY(facing,3,0,&cellX,&cellY);
    roomData[RF3].x = x;
    roomData[RF3].y = y;
    RelativeXY(facing, 3, 0, &roomData[RF3].x, &roomData[RF3].y);
    roomData[RF3].relativeCellNumber = RF3;
    SummarizeRoomInfo(&roomData[RF3], facing);
    if (pStdDrawCode[RF3][roomData[RF3].graphicRoomType] != NULL)
    {
      Interpret(pStdDrawCode[RF3][roomData[RF3].graphicRoomType],roomData, facing);
    }
    else
    {
      DrawCellF3(facing,&roomData[RF3]);
    };
  };
  if (!skipDrawing)
  {
    CustomBackgrounds(x, y, facing, 5);
    CustomBackgrounds(x, y, facing, 7);
    //cellX=x;
    //cellY=y;
    //RelativeXY(facing,2,-1,&cellX,&cellY);
    roomData[RF2L1].x = x;
    roomData[RF2L1].y = y;
    RelativeXY(facing, 2, -1, &roomData[RF2L1].x, &roomData[RF2L1].y);
    roomData[RF2L1].relativeCellNumber = RF2L1;
    SummarizeRoomInfo(&roomData[RF2L1], facing);
    if (pStdDrawCode[RF2L1][roomData[RF2L1].graphicRoomType] != NULL)
    {
      Interpret(pStdDrawCode[RF2L1][roomData[RF2L1].graphicRoomType],roomData, facing);
    }
    else
    {
      DrawCellF2L1(facing,&roomData[RF2L1]);
    };
  };
  if (!skipDrawing)
  {
    CustomBackgrounds(x, y, facing, 6);
    CustomBackgrounds(x, y, facing, 8);
    //cellX=x;
    //cellY=y;
    //RelativeXY(facing,2,1,&cellX,&cellY);
    roomData[RF2R1].x = x;
    roomData[RF2R1].y = y;
    RelativeXY(facing, 2, 1, &roomData[RF2R1].x, &roomData[RF2R1].y);
    roomData[RF2R1].relativeCellNumber = RF2R1;
    SummarizeRoomInfo(&roomData[RF2R1], facing);
    if (pStdDrawCode[RF2R1][roomData[RF2R1].graphicRoomType] != NULL)
    {
      Interpret(pStdDrawCode[RF2R1][roomData[RF2R1].graphicRoomType],roomData, facing);
    }
    else
    {
      DrawCellF2R1(facing,&roomData[RF2R1]);
    };
  };
  if (!skipDrawing)
  {
    CustomBackgrounds(x, y, facing, 9);
    //cellX=x;
    //cellY=y;
    //RelativeXY(facing,2,0,&cellX,&cellY);
    roomData[RF2].x = x;
    roomData[RF2].y = y;
    RelativeXY(facing, 2, 0, &roomData[RF2].x, &roomData[RF2].y);
    roomData[RF2].relativeCellNumber = RF2;
    SummarizeRoomInfo(&roomData[RF2], facing);
    if (pStdDrawCode[RF2][roomData[RF2].graphicRoomType] != NULL)
    {
      Interpret(pStdDrawCode[RF2][roomData[RF2].graphicRoomType],roomData, facing);
    }
    else
    {
      DrawCellF2(facing,&roomData[RF2]);
    };
  };
  if (!skipDrawing)
  {
    CustomBackgrounds(x, y, facing, 10);
    //cellX=x;
    //cellY=y;
    //RelativeXY(facing,1,-1,&cellX,&cellY);
    roomData[RF1L1].x = x;
    roomData[RF1L1].y = y;
    RelativeXY(facing, 1, -1, &roomData[RF1L1].x, &roomData[RF1L1].y);
    roomData[RF1L1].relativeCellNumber = RF1L1;
    SummarizeRoomInfo(&roomData[RF1L1], facing);
    if (pStdDrawCode[RF1L1][roomData[RF1L1].graphicRoomType] != NULL)
    {
      Interpret(pStdDrawCode[RF1L1][roomData[RF1L1].graphicRoomType],roomData, facing);
    }
    else
    {
      DrawCellF1L1(facing,&roomData[RF1L1]);
    };
  };
  if (!skipDrawing)
  {
    CustomBackgrounds(x, y, facing, 11);
    //cellX=x;
    //cellY=y;
    //RelativeXY(facing,1,1,&cellX,&cellY);
    roomData[RF1R1].x = x;
    roomData[RF1R1].y = y;
    RelativeXY(facing, 1, 1, &roomData[RF1R1].x, &roomData[RF1R1].y);
    roomData[RF1R1].relativeCellNumber = RF1R1;
    SummarizeRoomInfo(&roomData[RF1R1], facing);
    if (pStdDrawCode[RF1R1][roomData[RF1R1].graphicRoomType] != NULL)
    {
      Interpret(pStdDrawCode[RF1R1][roomData[RF1R1].graphicRoomType],roomData, facing);
    }
    else
    {
      NotImplemented(0x3ac2);//DrawCellF1R1(facing,&roomData[RF1R1]);
    };
  };
  CustomBackgrounds(x, y, facing, 12);
  //cellX = x;
  //cellY = y;
  //RelativeXY(facing,1,0,&cellX,&cellY);
  roomData[RF1].x = x;
  roomData[RF1].y = y;
  RelativeXY(facing, 1, 0, &roomData[RF1].x, &roomData[RF1].y);
  roomData[RF1].relativeCellNumber = RF1;
  SummarizeRoomInfo(&roomData[RF1], facing);
  d.FacingWaterFountain = d.FacingViAltar = d.FacingAlcove = 0;
  d.CellTypeJustAhead = (i16)roomData[RF1].roomType;
  if (pStdDrawCode[RF1][roomData[RF1].graphicRoomType] != NULL)
  {
    Interpret(pStdDrawCode[RF1][roomData[RF1].graphicRoomType],roomData, facing);
  }
  else
  {
    DrawCellF1(facing,&roomData[RF1],skipDrawing);
  };
  if (!skipDrawing)
  {
    CustomBackgrounds(x, y, facing, 13);
    //cellX = x;
    //cellY = y;
    //RelativeXY(facing,0,-1,&cellX,&cellY);
    roomData[RF0L1].x = x;
    roomData[RF0L1].y = y;
    RelativeXY(facing, 0, -1, &roomData[RF0L1].x, &roomData[RF0L1].y);
    roomData[RF0L1].relativeCellNumber = RF0L1;
    SummarizeRoomInfo(&roomData[RF0L1], facing);
    if (pStdDrawCode[RF0L1][roomData[RF0L1].graphicRoomType] != NULL)
    {
      Interpret(pStdDrawCode[RF0L1][roomData[RF0L1].graphicRoomType],roomData, facing);
    }
    else
    {
      DrawCellF0L1(facing,&roomData[RF0L1]);
    };
  };
  if (!skipDrawing)
  {
    CustomBackgrounds(x, y, facing, 14);
    //cellX = x;
    //cellY = y;
    //RelativeXY(facing,0,1,&cellX,&cellY);
    roomData[RF0R1].x = x;
    roomData[RF0R1].y = y;
    RelativeXY(facing, 0, 1, &roomData[RF0R1].x, &roomData[RF0R1].y);
    roomData[RF0R1].relativeCellNumber = RF0R1;
    SummarizeRoomInfo(&roomData[RF0R1], facing);
    if (pStdDrawCode[RF0R1][roomData[RF0R1].graphicRoomType] != NULL)
    {
      Interpret(pStdDrawCode[RF0R1][roomData[RF0R1].graphicRoomType],roomData, facing);
    }
    else
    {
      DrawCellF0R1(facing,&roomData[RF0R1]);
    };
  };
  CustomBackgrounds(x, y, facing, 15);
  roomData[RF0].x = x;
  roomData[RF0].y = y;
  roomData[RF0].relativeCellNumber = RF0;
  SummarizeRoomInfo(&roomData[RF0], facing);
  if (pStdDrawCode[RF0][roomData[RF0].graphicRoomType] != NULL)
  {
    Interpret(pStdDrawCode[RF0][roomData[RF0].graphicRoomType],roomData, facing);
  }
  else
  {
    DrawCellF0(facing,&roomData[RF0],skipDrawing);
  };

#endif  
  if ((D0W=D4W)!=0)
  {
    d.pWallBitmaps[4] = (ui8 *)d.Pointer2234;
    d.pWallBitmaps[3] = (ui8 *)d.Pointer2238;
    d.pWallBitmaps[2] = (ui8 *)d.Pointer2242;
    d.pWallBitmaps[1] = (ui8 *)d.Pointer2246;
    d.pWallBitmaps[0] = (ui8 *)d.Pointer2250;
  };
  // What was this for??? if (((D0W=d.Word1840)!=0) || true)
  {
  d.Word1840 = 1;
  //FreeTempMemory(8880);
  };
  D0W = (I16)((d.partyLevel!=255)?1:0);
  if (!skipDrawing) MarkViewportUpdated(D0W);
  if (d.partyLevel!=255)
  {
    FloorAndCeilingOnly();
  };
  viewportRandom = d.RandomNumber;
  d.RandomNumber = saveRandom;
  numRandomCalls = saveNumRan;
#ifdef _DEBUG
//  if (traceViewportDrawing != 0) traceViewportDrawing--;
  if ((traceViewportDrawing == 0) && GraphicTraceActive)
  {
    GraphicTraceActive = false;
    CloseGraphicTraceFile();
  };
#endif
}

//   TAG017c7a
void DrawSmallIcon(OBJ_NAME_INDEX objNI,i16 x,i16 y)
{ // For example, the eye on the status viewport
  ui8 *BMP;
  RectPos rect;
  BMP = allocateMemory(128,0);
  rect.w.x1 = x;
  rect.w.x2 = sw(rect.w.x1 + 15);
  rect.w.y1 = y;
  rect.w.y2 = sw(rect.w.y1 + 15);
  GetIconGraphic(objNI, BMP);
  BLT2Viewport(BMP, &rect, 8, -1);
  FreeTempMemory(128);
}

//   TAG018836
void DrawHorzBar(i32 x0, i32 y0, i32 width, i32 color)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  RectPos barPos;
  barPos.w.x1 = sw(x0);
  barPos.w.x2 = sw(barPos.w.x1 + width);
  barPos.w.y1 = sw(y0);
  barPos.w.y2 = sw(barPos.w.y1 + 6);
  d.UseByteCoordinates = 0;
  FillRectangle(d.pViewportBMP, &barPos, color, 112);
}

//   TAG018878
void DrawFoodWaterBar(i16 value,i16 y,i16 color)
{
  i32 barColor;
  barColor = color;
  if (value < 0) barColor = 11;   // Yellow
  if (value < -512) barColor = 8; //Red
  value += 1024;
  if (value >= 3072) value = 3071;
  value /= 32;
  DrawHorzBar(115, y+2, value, 0);//Black Background
  DrawHorzBar(113, y, value, barColor);//The bar itself
}

//*********************************************************
//
//*********************************************************
//   TAG0188e4
void DisplayFoodWater()
{
  CHARDESC *pcA3;
  pcA3 = &d.CH16482[d.SelectedCharacterOrdinal-1];
  if (TimerTraceActive)
  {
    fprintf(GETFILE(TraceFile),"%06x DisplayFoodWater calling Repack Chest\n",d.Time);
  };
  RepackChest();
  BLT2Viewport(GetBasicGraphicAddress(20), &d.wRectPos926, 72, 8);//Food/water box
  BLT2Viewport(GetBasicGraphicAddress(30), &d.wRectPos950, 24, 12);//"Food"
  BLT2Viewport(GetBasicGraphicAddress(31), &d.wRectPos958, 24, 12);//"Water"
  if (pcA3->poisonCount != 0)
  {
    BLT2Viewport(GetBasicGraphicAddress(32), &d.wRectPos966, 48, 12);
  };
  DrawFoodWaterBar(pcA3->food, 69, 5);
  DrawFoodWaterBar(pcA3->water, 92, 14);
}

//*********************************************************
//
//*********************************************************
//           TAG0191e8
RESTARTABLE _ClickOnEye()
{//()
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (d.PressingEye) RETURN;
  d.PressingEyeOrMouth = 1;
  d.PressingEye = 1;
  if (!playback_71 && ((d.MouseSwitches & 2) == 0))
  {
    d.PressingEyeOrMouth = 0;
    d.PressingEye = 0;
    d.QuitPressingEye = 0;
    RETURN;
  };
  playback_71 = false;
  DiscardAllInput();
  STHideCursor(HC47);
  VBLDelay(_1_,8); // 8 vertical blanks
  DrawSmallIcon(objNI_Special_e, 12, 13);//Eye looking towards skill scores
  if (d.EmptyHanded != 0)
  {
    TAG019036();
  }
  else
  {
    ClearHeldObjectName();
    ASSERT(RememberToPutObjectInHand == -1,"objInHand");
    DescribeObject(d.objectInHand, 1);
  };
  MarkViewportUpdated(0);
  RETURN;
}
