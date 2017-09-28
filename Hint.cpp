#include "stdafx.h"

#include "UI.h"

#include "Dispatch.h"

//#include "Objects.h"
#include "CSB.h"

#include <stdio.h>

#undef wordGear
i16& wordGear(pnt A){return *((i16 *)A);}
i16& wordGear(unsigned char * A){return *((i16 *)A);};

#define testfornotimplemented false

#define HINTFILL(a,b) i8 fill##a[a-b];

#define MAXHINTINDICES 9


i32 ReadExtendedFeatures(i32 handle);
void RC4_prepare_key(unsigned char *key_data_ptr, i32 key_data_len);

extern bool ForcedScreenDraw;
extern char hintKey[8];

#pragma pack(1)  



typedef pnt aReg;

//#define longGear(A) (*((i32 *)(A)))
//i32& longGear(pnt p);
//pnt& pntGear(pnt p);
//#define wordGear(A) (*((i16 *)(A)))

void StorePnt(pnt addr, pnt data);
ui8 *LoadPnt(aReg addr);
ui8 *LoadPnt(ui8 *addr);
i32 LoadLong(pnt addr);
i32 LoadLong(i32 *addr){return LoadLong((pnt)addr);};
void StoreLong(pnt addr, i32 data);
void StoreLong(i32 *addr, i32 data){StoreLong((pnt)addr, data);};
void info(char *, unsigned int);

#ifdef _MSVC_INTEL
i32 LE32(i32 lng);
#endif


struct S6
{
  ui8  b0;
  ui8  b1;
  i16  w2;
  i16  w4;
};

struct S12 
{
  i32   i0;
  i16   w4;
  i16   w6;
  void  (*pnt8)(pnt);
};

struct SS12
{
  i32   i0;
  i32   i4;
  i32   (*pnt8)(i32, pnt);
};

struct T12
{
  i8   b0;
  i8   b1;
  wordRectPos rectPos2;
  i16  w10;
};

struct S20
{
  i16  w0;
  i8   b2;
  i8   b3;
  wordRectPos rectPos4;
  i16  w12;
  const char*  pnt14;
  i16  w18;
};

struct S18
{
  pnt   pnt0;
  i32   i6;
  pnt   pnt10;
  i32   i14;
  i16   w4;
  i16   unused;
};

struct TEXT
{
  i16  w0;
  char *pnt2;
  wordRectPos rectPos6;
  i16  w14;
  i16  w16;
  i16  w18;
  i16  w20;
};

struct PAGE
{
  i16   w0;
  i16   w2;
  i32   i4;
  TEXT *pText8;
  i32   i12;
};

struct NODE
{
  PAGE  *pPage0;
  NODE  *pNode4;
};

struct BOOK
{
  i16  w0;  //#pages?
  NODE *pNode2; // node of first page
  NODE *pNode6; // node of last page
};

struct S4076
{
  i16  w0;
  i8   FirstBlock2[512];
  i8   SecondBlock514[128];
  pnt  pnt642;
  pnt  pnt646;
  pnt  pnt650;
  pnt  pnt654;
  pnt  pnt658;
  pnt  pnt662;
  pnt  pnt668;
  pnt  pnt672[16];
  i8   Characters[3328];
  pnt  pnt4064;
  pnt  pnt4068;
  pnt  pnt4072;
  HINTFILL(668,666)
  //HINTFILL(670,666)
};

struct PALETTEPKT
{
  i16   w0; //always 13?
  i16   *pw2;
  PALETTE  *pPalette6;
  i16   w10;
  i16   w12;
  i16   w14;
};

struct HCTI
{
  pnt  *pSeg1Addr;
  pnt  *pSeg2Addr;
  pnt  *pSeg3Addr;
  i16  *pSeg1Len;
  i16  *pSeg2Len;
  i32  *pSeg3Len;
};

struct DATINDEX
{
  i16   handle0;
  i16   numSeg2;
  i32  *fileOffsets4;
};

struct F {
  pnt   Pointer16908;
  pnt   Pointer16904;
  pnt   Pointer16900;
  pnt   Pointer16506[2];//set to logbase()
  pnt   Pointer15794;
  pnt   Pointer15080;
  pnt   Pointer15076;
  BOOK *pBook14608[23]; //uses memory to 14516
  //i16   rectPos14516[4];(use rectPos14516)
  i16   (*Pointer14508)(i32, pnt, i16);
  i32   Long14504;
  i32   Long16876;
  SS12  ss12_14498[1];
  i32   Long14110;//VBLCnt copied here
  //T12   t12_14106[12];(use t12_14106)
  pnt   Pointer13962[12];
  i32   Long12814;//disguised code pointer???
  i32   Long12810;//disguised code pointer???
  DATINDEX *pDatIndex12802; //HCSB.DAT handle, numseg, offsets
  pnt   Pointer12798;
  pnt   Seg1Addr10550;
  pnt   Seg2Addr10546;
  pnt   Seg3Addr10542;
  HCTI  HCTi10514; //Three pointers to pointers (segment addresses)
                   //followed by three pointers to words (lengths)
  i32   Seg3Len10534;
  TEXT *pText10480;
  TEXT *pText10476;
  S4076 s4076_8816[2];
  pnt   Pointer9710[100];
  pnt   Pointer9310[100];  
  TEXT *Pointer652;
  i32   Long288[3];
  i16  *Pointer268;
  pnt   Pointer264;
  i16  *Pointer260;
  i16  *Pointer256;
  i32  *Pointer252;
  pnt   Pointer248;
  i16   **Pointer244;
  i16  *Pointer218;
  pnt   Pointer214;
  i16  *Pointer210;
  i16  *Pointer206;
  i16   (*Pointer184)(i16, i16);// initially MinA
  i16   (*Pointer180)(i16, i16);// initially MinB
  pnt   Pointer136;
  pnt   Pointer132;
  S18   s18_124[1];

// *************************************************

  i16   Word16914;
  i16   Word16912;
  i16   Word16910;
  i8    Byte16896[1];
  HINTFILL(16895,16888)
  //i32   VBLCnt16888; // (use VBLCount)
  i16   Segment1_16872[50];//First segment of HCBS
  //i32   Long16864; // part of Segment1_16872
  //i16   Word16860; // part of Segment1_16872
  //i16   Word16858; // part of Segment1_16872
  //i16   Word16856; // part of Segment1_16872
  //i16   Word16848; // part of Segment1_16872
  //i16   Word16846; // part of Segment1_16872
  //i16   Word16808; // part of Segment1_16872
  //i16   Word16806; // part of Segment1_16872
  //i16   Word16800; // part of Segment1_16872
  PALETTE Palette16672;
  PALETTE Palette16640;
  PALETTE Palette16608;
  //i8    Byte16544[16];(use byte16544)
  //i8    Byte16528[16];(use byte16528)
  //HINTFILL(16607,16508)
  i16   Word16508;
  i16   Word16498;
  i16   Word16496;
  //i16   Word16494[3];(use word16494)
  //i16   Word16488[3];(use word16488)
  //pnt   Pointer16272[16];(use pText_16272)
  //S20     s20_16208[1];(use s20_16208)
  i8    Byte15808[1];
  HINTFILL(15807,15794)
  i8    Byte15788[1];
  HINTFILL(15787,15708)
  //i16   Word15708;//(use word15708)
  i16   Word15706;
  i16   Word15704;
  i16   Word15702;
  i16   Word15700;
  i16   Word15698;
  i16   Word15696;
  i8    Byte15694;
  i16   Word15072;
  i16   Word15070;
  i8    Byte15068[1];
  HINTFILL(15067,15046)
  i8    Byte15046[6];
  HINTFILL(15040,15024)
  //pnt   Pointer15024[5];//(use text15024)
  i16   Word14500;
  i8    Byte14210[2];
  i8    Byte14208[2];
  //i16   Word14204;(use word14204)
  //i16   Word14202;(use word14202)
  //i16   Word14200;(use word14200)
  i16   Word14198;
  i16   Word14196;
  i16   Word14194;
  i16   Word14192;
  S6    s6_14190[5]; //Keystroke queue???
  S6    s6_14160;
  HINTFILL(14156,14130)
  //pnt  *Pointer14130; (use pointer14130)
  //i16   Word14126[7];(use word14126)
  i16   Word14112;
  //i8    Byte13914[1];(use byte13914)
  HINTFILL(13913,13900)
  i16   Word13900;
  i16   Word13898;
  i16   Word13896;
  S6    s6_13884;
  i16   Word13878;
  i16   Word13876;
  i16   Word13874;
  i16   Word13870;
  i16   Word13864;
  i16   Word13862; //upper half of pointer into code
  i16   Word13860; //lower half of pointer into code
  i16   Word13858; //upper half of disassembled long at 12814
  i16   Word13856; //lower half of disassembled long at 12814
  i16   Word13852[1];
  HINTFILL(13850,13596)
  i16   Word13596;
  HINTFILL(13594,13340)
  i8    Byte13340[1];
  HINTFILL(13339,13084)
  i16   Word13084;
  i16   Word13082;
  HINTFILL(13080,12824)
  i16   Word12824;
  i16   Word12822;
  i16   Word12820;
  i16   Word12818;
  i16   Word12804;
  i8    Byte12792[2240];//Hints are expanded into this area.
  //HINTFILL(12791,10552)
  i16   NumHint10552; // #entries in HintIndices10530
  i16   Seg1Len10538;
  i16   Seg2Len10536;
  i16   HintIndices10530[MAXHINTINDICES];
  i16   Word10516;
  i16   Word10486;
  i16   CurPage10484;//page of current hint being displayed
  i16   Word10482;
  //i32   Long10472;(use long10472)
  //i32   Long10468;(use long10468)
  i16   Word10464;
  i16   Word10462;
  i16   Word8910;
  //i16   Word8908;//use word8908
  //i16   Word8906;//use word8906
  //i16   Word8904;//use word8904
  //i16   Word8902;//use word8902
  i16   Word8900;
  i8    Byte8898[12];
  i16   Word8886;
  i16   Word8884;
  //i8    Byte8873;(use byte8873)
  i16   Word8864;
  i16   Word8862;
  i16   Word8860;
  //pnt   Pointer8858[6];//use pointer8858[0]
  //pnt   Pointer8854;//use pointer8858[1]
  //pnt   Pointer8850;//use pointer8858[2]
  //pnt   Pointer8846;//use pointer8858[3]
  //pnt   Pointer8842;//use pointer8858[4]
  //pnt   Pointer8838;//use pointer8858[5]
  i16   Word8818;
  i16   Word664;
  i16   Word662;
  i8    Byte658;
  HINTFILL(657,652)
  i16   Word380[5];
  i8    Byte370[32];
  i16   Word338[17];
  i16   Word304[8];
  i8    Byte276[8];
  i16   Word240;
  i16   Word194;
  i16   Word128;
  i16   Word126;
};

F f;

#if defined _MSVC_INTEL || defined _MSVC_CE2002ARM
#pragma warning (disable:4305)
#pragma warning (disable:4309)
#endif

ui8 byte16544[16] ALIGN4 = 
{
    0x00,0x0a,0x14,0x1e,0x28,0x32,0x3c,0x46,
    0x50,0x5a,0x64,0x6e,0x78,0x82,0x8c,0x96
};

ui8 byte16528[16] ALIGN4 =
{
    0x00,0x0a,0x14,0x1e,0x28,0x32,0x3c,0x46,
    0x50,0x5a,0x64,0x6e,0x5a,0x82,0x8c,0x96
};

i16 word16494[3] ALIGN4 = {0x700, 0x070, 0x007};
i16 word16488[3] ALIGN4 = {0x100, 0x010, 0x001};


//struct S20
//{
//  i16  w0;
//  i8   b2;
//  i8   b3;
//  wordRectPos rectPos4;
//  i16  w12;
//  pnt  pnt14;
//  i16  w18;
//};


S20 s20_16208[] ALIGN4 = 
{
  {0xd,0x01,0,0x7f,0x00c2,0xb3,0xc0,0x0,"LOAD"  ,0x1},
  {0xd,0x03,0,0x21,0x0064,0xb3,0xc0,0x0,"LAST"  ,0x1},
  {0xd,0x08,0,0x21,0x0064,0xb3,0xc0,0x0,"Dump"  ,0x1},
  {0xd,0x04,0,0x7f,0x00c2,0xb3,0xc0,0x0,"NEXT"  ,0x1},
  {0xd,0x05,0,0xdd,0x0121,0xb3,0xc0,0x0,"DONE"  ,0x1},
  {0xd,0x0a,0,0x28,0x0118,0x1e,0x2a,0x0,NULL    ,0x2},
  {0xd,0x0b,0,0x28,0x0118,0x2e,0x3a,0x0,NULL    ,0x2},
  {0xd,0x0c,0,0x28,0x0118,0x3e,0x4a,0x0,NULL    ,0x2},
  {0xd,0x0d,0,0x28,0x0118,0x4e,0x5a,0x0,NULL    ,0x2},
  {0xd,0x0e,0,0x28,0x0118,0x5e,0x6a,0x0,NULL    ,0x2},
  {0xd,0x0f,0,0x28,0x0118,0x6e,0x7a,0x0,NULL    ,0x2},
  {0xd,0x10,0,0x28,0x0118,0x7e,0x8a,0x0,NULL    ,0x2},
  {0xd,0x11,0,0x28,0x0118,0x8e,0x9a,0x0,NULL    ,0x2},
  {0xd,0x06,0,0x7f,0x00c2,0xb3,0xc0,0x0,"OK"    ,0x1},
  {0xd,0x02,0,0xdd,0x0121,0xb3,0xc0,0x0,"EXIT"  ,0x1},
  {0xd,0x00,0,0x00,0x0000,0x00,0x00,0x0,NULL    ,0x0},
  {0xd,0x07,0,0xdd,0x0121,0xb3,0xc0,0x0,"CANCEL",0x1},
  {0xd,0x06,0,0x00,0x0021,0xb3,0xc0,0x0,NULL    ,0x1},
  {0x0,0x00,0,0x00,0x0000,0x00,0x00,0x0,NULL    ,0x0},
  {0x0,0x00,0,0x00,0x0000,0x00,0x00,0x0,NULL    ,0x0}
};

#if defined _MSVC_INTEL || defined _MSVC_CE2002ARM
#pragma warning (default:4305)
#pragma warning (default:4309)
#endif

char str15692_0[] =
    "Chaos Strikes Back/Hint Oracle//"
       "Please insert a Chaos Strikes Back"
       " saved game disk in drive A://Then press LOAD" ;
char str15692_1[] =
    "Chaos Strikes Back/Hint Oracle//"
        "Please insert a Chaos Strikes Back"
        " saved game disk in drive B://Then press LOAD" ;
char str15692_2[] =
    "Please insert a saved game disk in drive A:" ;
char str15692_3[] = 
    "Please insert a saved game disk in drive B:" ;
char str15692_4[] = 
    "There are no clues for this location." ;
char str15692_5[] = 
    "Chaos Strikes Back/Hint Oracle Version A//"
        "Safe to turn system off." ;
char str15692_6[] = 
    "Please Insert The Chaos Strikes Back Utility"
        " Disk in Drive A: " ;
char str15692_7[] =
    "Disk Error/Please Try Again" ;
char str15692_8[] =
    "Incorrect/or damaged saved game./"
        "Retry with a new saved game." ;



TEXT text15024[9] ALIGN4 =
{
  {3, str15692_0, 0x1f, 0x122, 0x32, 0x96, 1, 0, 0, 0},
  {3, str15692_1, 0x1f, 0x122, 0x32, 0x96, 1, 0, 0, 0},
  {3, str15692_2, 0x1f, 0x122, 0x46, 0x96, 1, 0, 0, 0},
  {3, str15692_3, 0x1f, 0x122, 0x46, 0x96, 1, 0, 0, 0}, 
  {3, str15692_4, 0x1f, 0x122, 0x46, 0x96, 1, 0, 0, 0},
  {3, str15692_5, 0x1f, 0x122, 0x46, 0x96, 1, 0, 0, 0},
  {3, str15692_6, 0x1f, 0x122, 0x46, 0x96, 1, 0, 0, 0},
  {3, str15692_7, 0x1f, 0x122, 0x46, 0x96, 1, 0, 0, 0},
  {3, str15692_8, 0x1f, 0x122, 0x46, 0x96, 1, 0, 0, 0}
};

TEXT *(pText16272[]) =
{
  &text15024[0],  //[0]16272
  &text15024[1],  //[1]16268
  &text15024[2],  //[2]16264
  &text15024[3],  //[3]16260
  &text15024[4],  //[4]16256
  &text15024[5],  //[5]16252
  &text15024[6],  //[6]16248
  &text15024[6],  //[7]16244
  &text15024[7],  //[8]16240
  &text15024[7],  //[9]16236
  &text15024[7],  //[10]16232
  &text15024[7],  //[11]16228
  &text15024[7],  //[12]16224
  &text15024[7],  //[13]16220
  &text15024[7],  //[14]16216
  &text15024[8]   //[15]16212
};

i16 word15708 = -1;

const char* pointer15024[5] =
{
  "A:\\", "B:\\", "C:\\", "D:\\", "E:\\"
};

wordRectPos rectPos14516 = {0x0000, 0x013f, 0x0000, 0x00c7};

i16 word14204 = 0;
i16 word14202 = 4;
i16 word14200 = 1;

pnt pointer14154[6] = {NULL,NULL,NULL,NULL,NULL,NULL};
pnt *pointer14130 = pointer14154;
i16 word14126[7] = {0x4737,0x4838,0x4b34,0x4d36,0x5032,0x5230,0x532e};

T12 t12_14106[12];

struct CLICKMAP
{
  i8   b0;
  i8   b1;
  T12  *pt12_2;
  pnt  *pPnt6;
  pnt  p10;
};

CLICKMAP clickMap13914 = {0x0a,0x00,t12_14106,f.Pointer13962,NULL};

i32 long10472 = 9999999;
i32 long10468 = 9999999;

i16 word8908 = 0x0014; //dd34 
i16 word8906 = 0x0000; //dd36 
i16 bitsPerCodeword = 0x0000; //word8904 = 0x0000; //dd38 
i16 codewordMask = 0x000; //word8902 = 0x0000; //dd3a 
i16 word8900 = 0x1000; //dd3c

ui8 byte8873[]= {0x00,0x01,0x03,0x07,0x0f,0x1f,0x3f,0x7f,0xff};

static const char *gameName;


const char* pointer8858[6] = {
  "",              //[0]8858
  "",              //[1]8854
  "0hcsb.hct",     //[2]8850
  "0hcsb.dat",     //[3]8846
  "1CSBGAME.DAT",  //[4]8842
  "1CSBGAME.BAK"   //[5]8838
};

extern ui32 VBLCount;
extern i32 NumberFloppyDrives;
extern i16 globalPalette[];

i16 dumpFile = -1;

void NotImplemented(i32);
void SetSupervisorMode();
void ClearSupervisorMode();
i32  READ(i32 file, i32 len, ui8 *buf);
i32  LSEEK(i32 offset, i32 file, i32 origin);
i16  CLOSE(i32 handle);
ui8 *MALLOC(i32 size);
void MFREE(ui8 *);
pnt  GETDTA();
void SETDTA(pnt);
i16  drvmap(); //Bios function TRAP #13
void StrCpy(char *dst, const char *src);//TAG003264
i32  atari_sprintf(char *,const char*, i32, i32=0, i32=0);//TAG0065e0
void MemoryMove(ui8 *src, ui8 *dst, i16, i16, i32);//like MemMove
ui8 *physbase();
ui8 *logbase();
void setscreen(ui8 *logaddr, ui8 *physaddr, i16 res);
#define wvbl(n) CALL0(n,wvbl)
void setpalette(PALETTE *palette);
void ForceScreenDraw();
void ShrinkBLT(ui8 *src, 
               ui8 *dst, 
               i32 srcWidth, 
               i32 srcHeight, 
               i32 dstWidth, 
               i32 dstHeight, 
               ui8 *colorMap);
void TAG0088b2(ui8 *src, 
               ui8 *dst, 
               RectPos *dstPos, // Position in dest image
               i32 SrcOffsetX,  // Source Starting x Pixel Offset 
               i32 SrcOffsetY,  // Source Starting y Pixel Offset
               i32 SrcByteWidth,   // #bytes per line in source bitmap 
               i32 DestByteWidth,  // #bytes per line in destination bitmap
               i32 P8);
i16  Unscramble(ui8 *buf, i32 initialChecksum, i32 numword);
void   ExpandRectangle(wordRectPos *, wordRectPos *, i16, i16);


class FILE_DESCRIPTOR
{
public:
  i16  word0;
  i16  fileHandle;  //word2
  i16  word4;
  i16  word6;
  i16  word8;
  char byte10[1]; //Don't know how big
  char xxbyte11[14-11];
  i32  long14;
  i32  long18;
  char byte22[1]; //Don't know how big. Error message?
  char xxbyte23[82-23];
  pnt  pointer82;
  i32  long86;
  pnt  pointer90;
  i32  long94;
  char byte98[1]; //Don't know how big.
  char xxbyte99[104-99];
  char byte104[1];//Don't know how big
  char xxbyte105[116-105];
  i16  word116;
  i16  word118;
  i32  long120;
};

FILE_DESCRIPTOR *FindFileDescriptor(i32 FDnum);//TAG000450(i32);
i32   TAG0004b6(i32, FILE_DESCRIPTOR *);//TAG0004b6(i32, pnt);
i32   TAG000606(i16, i16, i16);
void  TAG000678(pnt, pnt, i32);
i16   TAG0006bc(pnt, pnt, i16);
void  TAG0006f8(FILE_DESCRIPTOR *, i16, i16);//TAG0006f8(pnt, i16, i16);
i16   GetAvailFloppyMap();//TAG00073c
i32   TAG000784(i16, i32 *);
i32   TAG0008c4(FILE_DESCRIPTOR *pFD);//TAG0008c4(pnt);
i16   TAG000956(FILE_DESCRIPTOR *);//TAG000956(pnt);
i32   TAG000b46(i16, FILE_DESCRIPTOR *pFD);//TAG000b46(i16, pnt);
i32   OpenHelpFile(FILE_DESCRIPTOR *pFD);//TAG000bd2(pnt);
void  TAG000f50(i16);
void  TAG001006();
i16   TAG00101c();
void  TAG001064(i16);
i32   TAG001096_2(i16);
i32   ReadFile(i16);//TAG001096_7
i32   FileSeek(i16);//TAG001096_9
i32   TAG001096_10(i16);
i32   TAG001096_11(i16);
i32   TAG001096_22(i16);
void  SetFileName(i32, pnt);//TAG0015d6_2
void  SetBufferAddress(i16, pnt);//TAG0015d6_8
void  SetBufferLength(i16, i32);//TAG0015d6_16
void  TAG0015d6_128(i16, i32);
void  SetDriveName(i32, i32);//TAG0015d6_256
i32   TAG00181e(i16);
i16   TAG001840(i32, pnt, i16);
void  TAG00187e();
i32   TAG001896();
i32   TAG00189c(i16, i32, const char*);
i16   TAG001964(i16, i32, i32, i32 (*)(i32, pnt));
i16   TAG001a32(pnt, i16, i16);
void  ClearKeyboard();//TAG001a6a
i16   TAG001a9e(pnt, i16, i16, i16);
void  TAG001afe(i32, i32, i32);
void  TAG001bee();
void  TAG001c0c(i32);
i16   EnableButtons(CLICKMAP *);
void  TAG001ec0(pnt,pnt,wordRectPos *,i16,i16,i16,i16,i16);
void  ExpandGraphic(i8 *, ui8 *, i16, i16, i32 maxSize);//TAG00227a
void  TAG0025a0(ui8 *,ui8 *,i16,i16,i16,i16,ui8 *);
void  TAG002798(ui8 *, wordRectPos *, i16, i16);
void  TAG0029de();
void  TAG002a2a();
void  TAG002a5a(i16);
void  TAG002ae4();
void  TAG002af4();
void  TAG002b0c();
void  TAG002b3c(i16, i16);
void  TAG002b5c(i16, i16);
void  TAG002bf4();
void  TAG002c1a();
i32   TAG002c5e(pnt P1,pnt P2,pnt P3);
void  TAG002e64();
i32   ReadDatSeg(pnt, i16);//TAG002eb8
i32   TAG002f6c(ui8 *P1, i16 P2, i16 P3, i16 P4);
i32   TAG003006(ui8 *P1, i16 P2);
i32   LZWRawChar(i16 P1, i32 *P2);//TAG003022
void  TAG003194(i16, pnt *);
i32   TAG003210(i16 handle,   // @8
                i32 fileSize, // @10
                pnt rsltAddr, // @14
                pnt tmpBuf1,  // @18
                pnt tmpBuf2); // @22
void  TAG00339e(wordRectPos *, i16);
//void  TAG003444(i16 *, pnt); StrCpy
i16   StrLen(const char *);//TAG0034a2
//void  TAG0034c6(pnt, pnt, pnt);//(Use TAG0065e0)
pnt   TAG0035e6(pnt);
S18  *TAG00360a(pnt);
void  TAG00364c(pnt, S18 *);
void  TAG003682(pnt, S18 *);
void  TAG0036e4(pnt, i32, S18 *);
void  TAG003720(pnt, i32, S18 *);
pnt   TAG003788(i32, S18 *, i16 *);
i16   TAG003b3a(S18 *, i32);
pnt   TAG003c50(i16);
pnt   TAG003cf6(pnt *);
pnt   TAG003f82(S12 *);
void  TAG003d9e(pnt);
S18  *TAG003df0(S12 *);
pnt   TAG003efc(i32, i16, S18 *);
void  TAG00404c(pnt, S18 *);
i16   TAG004078(i16, i16);
void  TAG00418a();
pnt   TAG00419a(i32, i16);
i32   TAG0041c8(pnt);
pnt   TAG00421e(pnt);
void  TAG004254(pnt);
i32   TAG004316(pnt);
//void  TAG0047c2(pnt, pnt, i16, i16, i32);//MemoryMovevoid  y
void  ClearMemory(ui8 *dest, i32 numByte); // TAG0048de(i32 *, i32);
void  TAG004962(i16, i16, pnt);
i32   TAG0049fe(TEXT *,i32,i32);
pnt   TAG004a22_14(i16 P1);
pnt   TAG004a22_16(i16 P1, i32 nP2);
//   TAG004a22_20
void  Free_TEXT(i16 P1, TEXT *nP2);
void  TAG004a22_26(i16 /*P1*/, i32 nP2, i32 nP3);
pnt   TAG004f3a_1(i16 /*P1*/);
void  TAG004f3a_2(i16 /*P1*/);
pnt   AssignMemory(i16 /*P1*/, i32 nP2);//TAG004f3a_14
pnt   TAG004f3a_16(i16 P1, i32 nP2);
pnt   ReleaseMem(i32 /*P1*/, ui8 *nP2);//TAG004f3a_20
i16   TAG0051a2(i16 P1, i16 P2);
void  TAG0051c2_35(i16 P1);
void  TAG0051c2_36(i16 P1);
void  TAG0051c2_37(i16 /*P1*/);
void  TAG005ae8_1(i16 P1);
void  TAG005ae8_2(i16 P1);
void  TAG005ae8_9(i16 P1, PALETTEPKT *nP2);
void  TAG005ae8_38(i16 P1, PALETTEPKT *nP2);
void  TAG005d98();
//void  TAG005dcc(wordRectPos *, wordRectPos *, i16, i16);(use expandRectangle)
void  TAG005f9a_1(wordRectPos *, i16, i16);
void  TAG0059fe(i32 P1, i32 P2); //Fake function
static i16 MinA(i16, i16);// TAG0060a4
i32   TAG0060c4();
static i16 MinB(i16, i16);// TAG006282
void  TAG0062a2(wordRectPos *P1, ui8 *P2);
void  TAG00636a();
void  TAG006388();
void  TAG0063a6(i16);
i16   TAG0064fc(pnt, pnt);
pnt   StrCpyTo(pnt,pnt,i16);//TAG006538
i32   TAG006570(pnt, i32);
//void  TAG0065d0(pnt buf, i32 initialChecksum, i32 numword);(use Unscramble)
i32   TAG006636(pnt, i32, i16, i16);
//    TAG006672;
i32   MyReadFirstBlock(pnt P1);
void  TAG006718(i16 P1);
i32   TAG00686a(i16 P1);
pnt   TAG006bc0(i32 P1);
i32   TAG006c1c(i16, i32, pnt);
void  TAG006bfc(pnt P1);
void  TAG006c5e();
void  TAG007540();
void  TAG007560(i16);
void  TAG0075c8();
i32   TAG0076a0_8(i16 P1, pnt nP2, i32 nP3);
i32   TAG0076a0_13(i16 P1);
void  TAG0076a0_40(i32 /*P1*/, i32 nP2, const char* nP3, i32 nP4);
void  TAG0078fa(TEXT *);
i32   TAG007908(i32, pnt);
i16   TAG00794e(pnt, pnt);
void  TAG00797a();
void  TAG00799a_1(i16 /*P1*/);
void  TAG00799a_2(i16 /*P1*/);
void  SetSelectedLine(i16 /*P1*/, i32 nP2);//TAG00799a_7
i32   TAG00799a_13(i16 P1);
void  TAG007fbc();
void  TAG007fdc_2(i16 P1);
i32   TAG00835c_25(i16 P1);
void  TAG0083ac(i16, i16, i16, i16, i16);
i16   TAG0083cc(pnt, pnt); //Adds words between two addresses
void  TAG008898(i16, i16);
void  TAG0088b8(wordRectPos *, ui8 *, i16);
void  TAG008a42();
i32   TAG008a62();
void  TAG008b90(i16,S20 *,T12 *);
void  TAG008c20();
void  TAG008c40_1(i16 P1);
void  TAG008c40_2(i16 /*P1*/);
void  TAG008c40_3(i16 P1, i32 nP2, i32 nP3);
i32   TAG008c40_12(i16 /*P1*/, i32 nP2, i32 nP3, pnt nP4);
void  TAG008c40_30(i16 /*P1*/);
void  TAG0093a0_17(i16,i32);
void  TAG0093a0_18(i32 , i32);
void  Clear_14608();//TAG009462
i32   TAG00948c(i16 P1);
pnt   TAG0094de(i16 P1, PAGE *P2, NODE *P3);
i32   AddPage(i16 P1, PAGE *P2);//TAG009516
i16   PageCount(i16);//TAG00958a
pnt   TAG0095b0(i32 P1, i32 P2);
pnt   TAG00964e(i32 P1, i32 P2);
void  TAG00978a_1(i16 /*P1*/);
void  TAG00978a_3(i16 P1, PAGE *nP2);
pnt   TAG00978a_12(i16 P1, i32 nP2);
i32   TAG00978a_13(i16 P1);
pnt   TAG00978a_21(i16 P1, i32 nP2);
void  TAG00978a_27(i16 P1, i32 nP2, pnt nP3, i32 nP4);
pnt   TAG00978a_28(i16 P1,i16 nP2,i16 nP3);
void  TAG00978a_29(i16 P1, i16 nP2);
void  TAG009d7e(i16, i16 *, i16 *, i16 *);
//void  TAG009a02(i32, i32);
//void  TAG009a1c(i32, i32);
void  TAG009a40();
void  TAG009d36(i16, i16);
void  TAG009d5e(i16);
void  TAG009db6(i16, i32, i32 *);
void  TAG009dea(i16, i32, i32 *);
void  TAG009ea0(i16 *, i16 *, i16 *);
void  TAG009f1c(i16 *, i16 *, i16 *);
void  TAG009f80(i16);
void  TAG00a05c(i16 **);
i16   TAG00a07a(i16);
i16   TAG00a238();
i16   TAG00a28c();
i16   TAG00aa22(i16 *, i16 *, i16 *, i16 *);
void  TAG00aa58(i16, i32);
void  TAG00adf4(i32);
void  TAG00ae38(i32 *);
void  TAG00ae58();
//pnt   TAG00aeda(i16, i16); TRAP #1
void  TAG00aef0(i16);
void  TAG00af06();


void NotImpMsg(i32 addr)
{
  char msg[1000];
  sprintf(msg,
             "I did not translate all of the Atari\n"
             "code.  Some of it appeared to never be used.\n"
             "You have encountered code that I did not\n"
             "translate at address 0x%04x.\n"
             "Let me know and I will fix it.\n"
             "prsteven@facstaff.wisc.edu", addr);
  UI_MessageBox(msg,NULL,MESSAGE_OK);
  die(0);
}

void SFIRST(pnt /*mask*/, i32 /*flg*/)
{
  char *dta;
  dta = (char *)GETDTA();
  strcpy(dta+30,"abc");
}

// *********************************************************
//
// *********************************************************
//  TAG000450
FILE_DESCRIPTOR *FindFileDescriptor(i32 FDnum)
{
  FILE_DESCRIPTOR *A4;
//
  if ( (FDnum < 0) || (FDnum >= f.Word15070) )
  {
    TAG00189c(1, 0x0106000e, (char *)FDnum);
  };
  A4 = (FILE_DESCRIPTOR *)LoadPnt(f.Pointer15080);
  //A4 += 124 * FDnum;
  ASSERT(sizeof(*A4) == 124,"A4");
  A4 += FDnum;
  //if (wordGear(A4+4) == 0)
  if (A4->word4 == 0)
  {
    TAG00189c(1, 0x0106000e, (char *)FDnum);
  };
  return A4;
}

// *********************************************************
//
// *********************************************************
i32 TAG0004b6(i32 P1, FILE_DESCRIPTOR *P2)
{
  dReg D7;
  FILE_DESCRIPTOR *A4;
  D7L = P1;
  A4 = P2;
  switch (D7L)
  {
  case 0x01060003:
      NotImpMsg(0x506);
      /*
*/
      break;
  case 0x0106000c:
  case 0x01060004:
      NotImpMsg(0x51e);
      /*
*/
      break;
  case 0x01060006:
      NotImpMsg(0x530);
      /*
*/
      break;
  case 0x01060007:
      //if (wordGear(A4+2) == -1)
      if (A4->fileHandle == -1)
      {
        //StrCpy(A4+22, "A read error occurred on %s");
        StrCpy(A4->byte22, "A read error occurred on %s");
      }
      else
      {
        atari_sprintf((char *)A4->byte22, 
                      "A read error occurred on file %s in %%s",
                      (i32)LoadPnt((ui8 *)&A4->pointer82));
      };
      break;
  case 0x01060008:
      NotImpMsg(0x57a);
      /*
*/
      break;
  case 0x01060009:
      NotImpMsg(0x58a);
      /*
*/
      break;
  case 0x0106000d:
    NotImpMsg(0x5be);
    /*
*/
      break;
  case 0x010600ff:
      //atari_sprintf(
      //        A4+22, 
      //        "IO Error occured on %%s in %s", 
      //        (i32)A4+10,
      //        0);
      atari_sprintf(
              (char *)A4->byte22, 
              "IO Error occured on %%s in %s", 
              (i32)A4->byte10,
              0);
      break;
  }; //switch
  //TAG00189c(0, D7L, (pnt)(UI16)(wordGear(A4)));
  TAG00189c(0, D7L, (char *)((int)(A4->word0)));
  return D7L;
}

// *********************************************************
//
// *********************************************************
i32 TAG000606(i16 P1, i16 P2, i16 P3)
{
  dReg D5, D6, D7;
  D7W = P1;
  for (D6W=1, D5W=0, D7W>>=P2;
       (P2++) <= P3; 
       D6W <<= 1)
  {
    if (D7W & D6W)
    {
      D5W++;
    };
//
  };
  return D5W;
}

// *********************************************************
//
// *********************************************************
void TAG000678(pnt src, pnt dst, i32 P3)
{
  dReg D6, D7;
  aReg A3, A4;
  A4 = src;
  A3 = dst;
  D7W = sw(P3);
  for (D6L = 0; (*A4 != 0) && (D7W != 0); D7W--)
  {
    *(A3++) = (*A4++);
//
//
  };
  while ((D7W--) != 0)
  {
    *(A3++) = 0;
  };
}

// *********************************************************
//
// *********************************************************
i16 TAG0006bc(pnt,pnt,i16)
{
  NotImpMsg(0x6bc); return 0;
}

// *********************************************************
//
// *********************************************************
void TAG0006f8(FILE_DESCRIPTOR *P1, i16 P2, i16 P3)
{
  dReg D6, D7;
  FILE_DESCRIPTOR *A4;
  A4 = P1;
  D7W = P2;
  D6W = P3;
//
  //if (  ((wordGear(A4+6) & D7W)==0)
  //    ||((wordGear(A4+6) & D6W)!=D6W) )
  if (  ((A4->word6 & D7W)==0)
      ||((A4->word6 & D6W)!=D6W) )
  {
    TAG00189c(1, 0x01060005, (char *)A4);
  };
}

// *********************************************************
// Bit 0 = drive A, etc
// *********************************************************
//  TAG00073c
i16 GetAvailFloppyMap()
{
  dReg D0, D7;
  SetSupervisorMode();
  D7W = sw(NumberFloppyDrives);
  ClearSupervisorMode();
  if (D7W == 2)
  {
    D7L = 3; // map of drives
  };
  D0W = drvmap();
  D0W &= 0xfc;
  D7W |= D0W;
  return D7W;
}

// *********************************************************
//
// *********************************************************
i32 TAG000784(i16 P1, i32 *P2)
{
  dReg D0, D1, D7;
  aReg A0, A4;
  pnt pnt_8;
  D7L = 0;
  A0 = f.Pointer15076;
  A4 = (aReg)LoadPnt(A0);
  pnt_8 = GETDTA();
  SETDTA(A4);
  StrCpy((char *)f.Byte15068, pointer15024[P1-1]);
  StrCpy((char *)f.Byte15068, "*.*");
  SFIRST(f.Byte15068, 8);
  TAG000678(A4+30, f.Byte15068, 12);//Get volume label
  SETDTA(pnt_8);
  if (P1 < 3)
  {
    D0L = 0;
    if (D0L == 0)
    {
      TAG000678(A4+2, f.Byte15046, 6);
      D0L = (UI8)(A4[8]);
      D0L <<= 16;
      D1L = (UI8)(A4[9]);
      D0L = D0L | (D1L<<8);
      D1L = (UI8)(A4[10]);
      D0L = 0;
      *P2 = D0L;      
      return D7L;
    };
    //return 0x01060004;
  }
  else
  {
    NotImpMsg(0x882);
    /*
*/
  };
  return D7L;
}

// *********************************************************
//
// *********************************************************
i32 TAG0008c4(FILE_DESCRIPTOR *P1)
{
  dReg D6, D7;
  FILE_DESCRIPTOR *A4;
  D6L = 0x1baddade;
  A4 = P1;
  do
  {
    if (A4->word116 == 32)
    {
      D7L = 0;
      switch (D7W)
      {
      case 2:
          D6L = 0x106000d;
          break;
      case 1:
          NotImpMsg(0x902);
          /*
*/
          break;
      case 0:
          D6L = 0;
          break;
      }; //switch
    }
    else
    {
      D6L = 0;
    };
    ASSERT(D6L != 0x1baddade,"D6L");
    if (D6L != 0)
    {
      D6L = TAG0004b6(D6L, A4);
    };
  } while (D6L == 0x0102fffe);
  return D6L;
}

// *********************************************************
//
// *********************************************************
i16 TAG000956(FILE_DESCRIPTOR *P1)
{
  dReg D0, D7;
  FILE_DESCRIPTOR *A4;
  i32 i_4 = 0x1baddade;
  D7L = 0x1baddade;
  A4 = P1;
  //if (wordGear(A4+6) & 0x8000)
  if (A4->word6 & 0x8000)
  {
    do
    {
      if (A4->word6 & 0x100)
      {
        TAG000784(A4->word8, &i_4);
        D0L = 0;
        //if (D0L != 0) goto tag000ca
//
        //if (    ((wordGear(A4+6) & 0x200) == 0)
        //     || (LoadLong(A4+120) == i_4)  )
        if (    ((A4->word6 & 0x200) == 0)
             || (LoadLong(&A4->long120) == i_4)  )
        {

          //if (    ((wordGear(A4+6) & 0x20) == 0)
          //     || (TAG0006bc(f.Byte15046, A4+98, 6) != 0)  )
          if (    ((A4->word6 & 0x20) == 0)
               || (TAG0006bc(f.Byte15046, (pnt)A4->byte98, 6) != 0)  )
          {
            if (A4->word6 & 0x40)
            {
              //D0W = TAG0006bc(f.Byte15068, A4+104, 12);
              D0W = TAG0006bc(f.Byte15068, (pnt)A4->byte104, 12);
              if (D0W != 0) D7L = 0;
              else D7L = 0x0106000c;
            }
            else
            {
              D7L = 0;
            };
          }
          else
          {
            D7L = 0x0106000c;
          };
        }
        else
        {
          D7L = 0x010600c;
        };
      }
      else
      {
        NotImpMsg(0x9ee);
        /*

*/
      };
      if (    (D7L == 0x0106000c)
         || (D7L == 0x01060004)  )
      {
        D7L = TAG0004b6(D7L, A4);
      }
      else
      {
        if (D7L == 0)
        {
          TAG000678(f.Byte15068, (pnt)A4->byte104, 12);
          TAG000678(f.Byte15046, (pnt)A4->byte98, 6);
          StoreLong(&A4->long120, i_4);
          A4->word6 |= 0x03e0;
          A4->word6 &= 0x7fff;
        };
      };
    } while (D7L == 0x0102fffe);
  }
  else
  {
    D7L = TAG0008c4(A4);
  };
  return D7W;
}

// *********************************************************
//
// *********************************************************
//  TAG000bd2
i32 OpenHelpFile(FILE_DESCRIPTOR *P1) 
{
  dReg D0, D6, D7;
  TAG0006f8(P1, 992, 2);
  D7L = TAG000956(P1);
  if (D7W == 0)
  {
    //StrCpy(f.Byte15068, P1+10);
    StrCpy((char *)f.Byte15068, P1->byte10);
    //StrCpy(f.Byte15068, LoadPnt(P1+82));
    StrCpy((char *)f.Byte15068, (char *)LoadPnt((ui8 *)&P1->pointer82));
    if (P1->word4 == 2)
    {
      D6W = OPEN((char *)f.Byte15068, "rb");
    }
    else
    {
      D6W = CREATE((char *)f.Byte15068, "wb", true);
    };
    TAG000b46(D6W, P1);
    D0L = 0;
    //D6W = 6;//What was this for?  Maybe TOS assigns handle 6?
    P1->fileHandle = D6W;
  };
  P1->word4 = 2;  
  return 0;
}

// *********************************************************
//
// *********************************************************
i32 TAG000b46(i16 P1, FILE_DESCRIPTOR *P2)
{
  dReg D7;
  FILE_DESCRIPTOR *A4;
  A4 = P2;
  if (P1 >= 0)
  {
    return 0;
  };
  switch ((UI16)(P1))
  {
  case 0xfff6:
      D7L = 0x01060008;
      break;
  case 0xfff5:
      D7L = 0x01060007;
      break;
  case 0xfff3:
      D7L = 0x01060009;
      break;
  case 0xfff2:
      D7L = 0x0106000d;
      break;
  case 0xffdf:
      D7L = 0x01060003;
      break;
  case 0xffde:
      D7L = 0x0106000b;
      break;
  default:
      D7L = 0x10600ff;
      //StoreLong(A4+18, D7L);
      StoreLong(&A4->long18, D7L);
  };
  return TAG0004b6(D7L, A4);
}

// *********************************************************
//
// *********************************************************
void TAG000f50(i16 P1)
{
  dReg D0, D6, D7;
  aReg A3;
  FILE_DESCRIPTOR *A4;
  pnt  pnt_4;
  D7W = P1;
  pnt_4 = NULL;
  D0L = (UI16)(D7W);
  D0L = 124 * D0L;
  A3 = TAG00419a(D0L, 1025);
  if (A3 == NULL)
  {
    TAG00189c(1, 0x1060010, "No mem for IORequests");
  };
  f.Pointer15080 = A3;
  A3 = TAG00419a(1024, 129);
  if (A3 == NULL)
  {
    TAG00189c(1, 0x01060010, "No mem for IO buffer");
  };
  f.Pointer15076 = A3;
  f.Word15072 = 0;
  f.Word15070 = D7W;
  A4 = (FILE_DESCRIPTOR *)LoadPnt(f.Pointer15080);
  for (D6L=0; D6W < D7W; D6W++)
  {
    A4->word0 = D6W;
    A4->word4 = 0;
    //A4 += 124;
    A4 += 1;
//
  };
}

// *********************************************************
//
// *********************************************************
void TAG001006()
{
  TAG0041c8(f.Pointer15076);
  TAG0041c8(f.Pointer15080);
}

// *********************************************************
//
// *********************************************************
i16 TAG00101c()
{
  dReg D7;
  FILE_DESCRIPTOR *A4;
  for (A4 = (FILE_DESCRIPTOR *)LoadPnt(f.Pointer15080), D7L=0;
       D7W < f.Word15070;
       D7W++, A4++ )
  {
    if (A4->word4 != 0) continue;
    A4->byte10[0] = 0;
    A4->word4 = 1;
    A4->fileHandle = -1;
    A4->word6 = 0;
    break;
//
//
  };
  if (D7W >= f.Word15070)
  {
    return -1;
  };
  return D7W;
}

// *********************************************************
//
// *********************************************************
void TAG001064(i16 FDnum)
{
  FILE_DESCRIPTOR *A4;
  A4 = FindFileDescriptor(FDnum);
  if (A4->fileHandle != -1)
  {
  };
  //wordGear(A4+4) = 0;
  A4->word4 = 0;
}

// *********************************************************
//
// *********************************************************
i32 TAG001096_xxx(i16 P1, i16 P2)
{
  dReg D0, D4, D5, D6, D7;
  FILE_DESCRIPTOR *A4;
  D7W = P1;
  D6L = 0;
  A4 = FindFileDescriptor(D7W);
  if (P2 != 0)
  {
    //wordGear(A4+4) = P2;
    A4->word4 = P2;
  };
  switch (P2)
  {
  case 2:
  case 3:
      D6L = OpenHelpFile(A4);
      break;
  case 10:
      do
      {
        if (A4->fileHandle == -1)
        {
          D6L = TAG0004b6(0x0106000f, A4);
        }
        else
        {
          D6L = 0;
          if (D6L == 0)
          {
            D5W = CLOSE(A4->fileHandle);
            D6W = sw(TAG000b46(D5W, A4));
          };
        };
      } while (D6L == 0x0102fffe);
      if (D6L == 0)
      {
        A4->fileHandle = -1;
      };
      break;
  case 4:
    /*
*/
      break;
  case 5:
      NotImpMsg(0x1172);
      /*
*/
      break;
  case 6:
      NotImpMsg(0x1180);
      /*
*/
      break;
  case 7:
      TAG0006f8(A4, -1, 26);
      do
      {
        if (A4->fileHandle == -1)
        {
          D6L = TAG0004b6(0x0106000f, A4);
        }
        else
        {
          D6L = TAG0008c4(A4);
          if (D6L == 0)
          {
            D4L = READ(A4->fileHandle, 
                       LoadLong(&A4->long94), 
                       (ui8 *)LoadPnt((ui8 *)&A4->pointer90));
            if (D4L != LoadLong(&A4->long94))
            {
              D6L = TAG0004b6(0x01060007, A4);
            };
          };
        };
      } while (D6L == 0x0102fffe);
      break;
  case 8:
      NotImpMsg(0x1286);
      /*
*/
      break;
  case 9:
      TAG0006f8(A4, -1, 18);
      do
      {
        if (A4->fileHandle == -1)
        {
          D6L = TAG0004b6(0x0106000f, A4);
        }
        else
        {
          D6L = TAG0008c4(A4);
          if (D6L == 0)
          {
            D4L = LSEEK(LoadLong(&A4->long94), A4->fileHandle, SEEK_SET);
            StoreLong(&A4->long14, D4L);
            if (D4L < 0)
            {
              D6L = TAG0004b6(0x010600ff, A4);
            };
          };
        };
      } while (D6L == 0x0106fffe);
      break;
  case 11:
      do
      {
        if (A4->fileHandle == -1)
        {
          D6L = TAG0004b6(0x0106000f, A4);
        }
        else
        {
          D6L = TAG0008c4(A4);
          if (D6L == 0)
          {
            D4L = LSEEK(0, A4->fileHandle, SEEK_CUR);
            StoreLong(&A4->long14, D4L);
            if (D4L < 0)
            {
              D6L = TAG0004b6(0x010600ff, A4);
            };
          };
        };
      } while (D6L == 0x0102fffe);
      break;
  case 12:
      NotImpMsg(0x13d2);
      /*
*/
      break;
  case 20:
      NotImpMsg(0x13e0);
      /*
*/
      break;
  case 21:
      NotImpMsg(0x1404);
      /*
*/
      break;
  case 22:
      TAG0006f8(A4, -1, 128);
      do
      {
        D5W = GetAvailFloppyMap();
        if (A4->word116 == 32)
        {
          D0L = TAG000606(D5W, 0, 1) & 0xffff;
        }
        else
        {
          D0L = TAG000606(D5W, 2, 5) & 0xffff;
        };
        StoreLong(&A4->long14, D0L);
      } while (D6L == 0x0102fffe);
      break;
  case 23:
      NotImpMsg(0x1496);
      /*
*/
      break;
  case 24:
      NotImpMsg(0x14ae);
      /*
*/
      break;
  case 25:
  case 26:
      NotImpMsg(0x14c6);
      /*
*/
      break;
  case 13:
  case 14:
  case 15:
  case 16:
  case 17:
  case 18:
  case 19:
  default:
    NotImpMsg(0x14d2);
    /*
*/
      break;
  }; //switch
  if (D6L != 0)
  {
    StoreLong(&A4->long18, D6L);
  };
  return D6L;
}

// *********************************************************
//
// *********************************************************
i32 TAG001096_2(i16 P1)
{
  return TAG001096_xxx(P1, 2);
}

// *********************************************************
//
// *********************************************************
i32 ReadFile(i16 P1)
{
  return TAG001096_xxx(P1, 7);
}

// *********************************************************
//
// *********************************************************
i32 FileSeek(i16 P1)
{
  return TAG001096_xxx(P1, 9);
}

// *********************************************************
//
// *********************************************************
i32 TAG001096_10(i16 P1)
{
  return TAG001096_xxx(P1, 10);
}

// *********************************************************
//
// *********************************************************
i32 TAG001096_11(i16 P1)
{
  return TAG001096_xxx(P1, 11);
}

// *********************************************************
//
// *********************************************************
i32 TAG001096_22(i16 P1)
{
  return TAG001096_xxx(P1, 22);
}

// *********************************************************
//
// *********************************************************
i32 TAG0015d6_xxx(i32 P1, i32 P2, i32 P3, i32)
{
  dReg D0, D7;
  FILE_DESCRIPTOR *A4;
  D7L = 0x0106000e;
  A4 = FindFileDescriptor(P1);
  D7L = 0;
  switch (P2)
  {
  case 1:
      StoreLong(&A4->long18, P3);
      break;
  case 2:
      StoreLong((i32 *)(&A4->pointer82), P3);
      break;
  case 4:
      StoreLong(&A4->long86, P3);
      break;
  case 8:
      StoreLong((i32 *)(&A4->pointer90), P3);
      break;
  case 16:
      StoreLong(&A4->long94, P3);     
      break;
  case 32:
      NotImpMsg(0x165c);
      /*
*/
      break;
  case 64:
      NotImpMsg(0x168c);
      /*
*/
      break;
  case 128:
      if (P3 == 0)
      {
        P2 = 0;
        A4->word6 &= 0xff7f;//I guess!
      }
      else
      {
        if ( (P3 == 32) || (P3 == 64) )
        { 
          A4->word116 = sw(P3); //lower 16 bits!
        }
        else
        {
          D7L = 0x0106000b;
        };
      };
      P2 |= 0x8000; // I guess!
      break;
  case 256:
      //A4[10] = 0;
      A4->byte10[0] = 0;
      if (P3 == 0)
      {
        P2 = (unsigned)0x8000;
        A4->word6 &= 0xfeff;
      }
      else
      {
        if ( (P3 < 32) || (P3 >= 68) )
        {
          D7L = 0x0106000b;
        }
        else
        {
          A4->word118 = sw(P3);
          if (P3 >= 64)
          {
            //A4->word8 = sw(A4->word14 + 65475);
            A4->word8 = sw(wordGear((pnt)A4+14) + 65475);
            A4->word116 = 64;  
          }
          else
          {
            A4->word8 = sw(P3 - 31);
            if ((UI16)(A4->word8) > 2)
            {
              D7L = 0x106000b;
            }
            else
            {
              A4->word116 = 32;
            };
          };
          if (D7W == 0)
          {
            P2 |= 0x8080;
            D0L = A4->word8 - 1;
            D0L &= 0xffff;
            // "A:\", "B:\",  etc
            StrCpy(A4->byte10, pointer15024[D0L]);
          };
        };
      };
      break;
  case 512:
      NotImpMsg(0x17a2);
      /*
*/
      break;
  case 1024:
      NotImpMsg(0x17b0);
      /*
*/
      break;
  default:
      NotImpMsg(0x17c0);
      /*
*/
      break;
  }; //switch
  A4->word6 |= P2;
  if (D7L != 0)
  {
    StoreLong(&A4->long18, D7L);
  };
  return D7L;
}

// *********************************************************
//
// *********************************************************
void SetFileName(i32 P1, pnt P2)
{
  TAG0015d6_xxx(P1, 2, (i32)P2, 0x1baddade);
}

// *********************************************************
//
// *********************************************************
void SetBufferAddress(i16 P1, pnt P2)
{
  TAG0015d6_xxx(P1, 8, (i32)P2, 0x1baddade);
}

// *********************************************************
//
// *********************************************************
void SetBufferLength(i16 P1, i32 P2)
{
  TAG0015d6_xxx(P1, 16, P2, 0x1baddade);
}

// *********************************************************
//
// *********************************************************
void TAG0015d6_128(i16 P1, i32 P2)
{
  TAG0015d6_xxx(P1, 128, P2, 0x1baddade);
}

// *********************************************************
//
// *********************************************************
void SetDriveName(i32 P1, i32 P2)
{
  TAG0015d6_xxx(P1, 256, P2, 0x1baddade);
}

// *********************************************************
//
// *********************************************************
i32 TAG00181e(i16 P1)
{
  FILE_DESCRIPTOR *A4;
  A4 = FindFileDescriptor(P1);
  return LoadLong(&A4->long14); 
}

// *********************************************************
//
// *********************************************************
i16 TAG001840(i32, pnt, i16)
{
  NotImpMsg(0x1840); return 0;
}

// *********************************************************
//
// *********************************************************
void TAG00187e()
{
  f.Pointer14508 = TAG001840;
  f.Long14504 = 0;
  f.Word14500 = 0;
}

// *********************************************************
//
// *********************************************************
i32 TAG001896()
{
  return f.Long14504;
}

// *********************************************************
//
// *********************************************************
i32 TAG00189c(i16 P1, i32 , const char* P3)
{
  dReg D5, D6, D7;
  SS12 *ss12A4;
  D7L = P1;
  if (D7L == 0)
  {
    f.Long14504 = 0;
    D6L = 0;
  }
  else
  {
    D5W = f.Word14500;
    ss12A4 = &f.ss12_14498[D5W-1];
    do
    {
      for (D6L = D7L;
           D5W != 0;
           D5W--, ss12A4--)
      {
        if (D7L > ss12A4->i4) continue;
        if (D7L < ss12A4->i0) continue;
        D6L = (ss12A4->pnt8)(D7L, (pnt)P3);
        D5W--;
        ss12A4--;
        break;
//
//
      };
      switch (D6L)
      {
      case 0x0102ffff:
          (f.Pointer14508)(D7L, (pnt)P3, 1);
          break;
      default:
          f.Long14504 = D6L;
          if (P1 == 0) break;
          (f.Pointer14508)(D6L, (pnt)P3, 1);
      case 0x0102fffe:
      case 0x00000000:
      case 0x0102fffd:
          break;
      };
    } while (D6L == 0x0102fffd);
  };
  return D6L;
}

i16 TAG001964(i16 P1,i32 P2,i32 P3,i32 (*P4)(i32, pnt))
{
  dReg D4, D5, D6, D7;
  SS12 *ss12A3;
  i32 (*fA4)(i32, pnt);
  D4L = 0x1baddade;
  D7L = P2;
  D6L = P3;
  fA4 = P4;
  D5L = 0;
  ss12A3 = &f.ss12_14498[f.Word14500];
  if (P1 != 0)
  {
    if (f.Word14500 == 24)
    {
    }
    else
    {
      f.Word14500++;
      ss12A3->i0 = D7L;
      ss12A3->i4 = D6L;
      ss12A3->pnt8 = fA4;
      D4L = 0;
    };
  }
  else
  {
    NotImpMsg(0x19b6);
    /*
*/
  };
  return D4W;
}

// *********************************************************
//
// *********************************************************
i16 TAG001a32(pnt P1, i16 P2, i16 P3)
{
  dReg D6, D7;
  aReg A4;
  A4 = P1;
  D7W = P2;
  D6W = P3;
  if (D7W > wordGear(A4+2)) return 0;
  if (D7W < wordGear(A4+0)) return 0;
  if (D6W > wordGear(A4+6)) return 0;
  if (D6W < wordGear(A4+4)) return 0;
  return 1;
}

// *********************************************************
//
// *********************************************************
//   TAG001a6a
void ClearKeyboard()
{
  while (UI_CONSTAT())
  {
    UI_DIRECT_CONIN();
  };
  word14200 = 1;
  word14204 = 0;
  word14202 = 4;
  f.Word14198 = 0;
  word14200 = 0;
}

// *********************************************************
//
// *********************************************************
i16 TAG001a9e(pnt P1, i16 P2, i16 P3, i16 P4)
{
  dReg D0, D5, D6, D7;
  aReg A4;
  A4 = P1;
  D7W = P2;
  D6W = P3;
  if (A4 == NULL) return 0;
  for (; (D5W = (UI8)(A4[0])) != 0; A4 += 12)
  {
    D0W = TAG001a32(A4+2, D7W, D6W);
    if (D0W == 0) continue;
    if ((P4 & wordGear(A4+10)) == 0) continue;
    if ((A4[1] & 1) == 0) break;
//
  };
  return D5W;
}

// *********************************************************
//
// *********************************************************
void TAG001afe(i32 P1, i32 P2, i32 P3)
{//Enqueues a mouse click
  dReg D4, D5, D6, D7;
  pnt *pA4;
  S6  *s6A0;
  i32 i;
  D7W = sw(P1);
  D6W = sw(P2);
  if (word14200 != 0)
  {
    f.Word14198 = 1;
    f.Word14196 = D7W;
    f.Word14194 = D6W;
    f.Word14192 = sw(P3);
    return;
  };
  word14200 = 1;
  D5W = sw(word14202 + 2);
  if (D5W > 4) D5W -= 5;
  if (D5W == word14202) return;
  for (i = 0, pA4 = pointer14130;
       i < 6;
       i++, pA4 ++)
  {
    if (*pA4 == NULL) continue;
    if ((*pA4)[1] & 1) continue;
    D4W = TAG001a9e((pnt)LoadPnt((*pA4)+2),
                     D7W,
                     D6W,
                     (UI16)P3);
    if (D4W == 0) continue;
    if ((D5W--) == 0)
    {
      D5W = 4;
    }
    word14202 = D5W;
    s6A0 = &f.s6_14190[D5W];
    s6A0->b0 = D4B;
    s6A0->b1 = ub(i);
    s6A0->w2 = D7W;
    s6A0->w4 = D6W;
    break;
//
//
  };
  word14200 = 0;
}

// *********************************************************
//
// *********************************************************
void TAG001bee()
{
  if (f.Word14198 != 0)
  {
    f.Word14198 = 0;
    TAG001afe(f.Word14196, f.Word14194, f.Word14192);
  };
}

// *********************************************************
//
// *********************************************************
void TAG001c0c(i32 P1)
{
  dReg D0, D1, D4, D5, D6, D7;
  aReg A0, A3;
  S6  *s6A0;
  pnt *pA4;;
  i16   w_4 = 0x1bad;
  i16   w_2;
  D7L = P1;
  if (D7W == 0)
  {
    D7L >>= 8;
  }
  else
  {
    D0W = (I16)(D7W & 255);
    D1L = (D7L >>8) & 0xff00;
    D4W = sw(D0W | D1W);
    for (D5L=0; 
         (D5L < 7) && (D4W >= word14126[D5L]); 
         D5L++)
    {
      if (D4W == word14126[D5L])
      {
        D7L = (UI16)(D4W);
        break;
      };
//
//
//   
    };
  }
  word14200 = 1;
  D6W = sw(word14202+2);
  if (D6W > 4) D6W-=5;
  if (D6W != word14204)
  {
    w_2 = 0;
    for (D5L = 0, pA4 = pointer14130;
         D5L < 6;
         D5L++, pA4++)
    {
      if (*pA4 == NULL) continue;
      A0 = *pA4;
      if (wordGear(A0) & 1) continue;
      A3 = (aReg)LoadPnt(A0+6);
      if (A3 == NULL) continue;
      for (; (D4W = (UI8)(*A3)) != 0; A3+=4)
      {
        if (A3[1] & 1) continue;
        if (wordGear(A3+2) == D7W)
        {
tag001cd2:
          D0W = D6W;
          D6W--;
          if (D0W == 0)
          {
            D6W = 4;
          };
          word14202 = D6W;
          s6A0 = &f.s6_14190[D6W];
          s6A0->b0 = D4B;
          s6A0->b1 = D5B;
          s6A0->w2 = -1;
          s6A0->w4 = D7W;
          goto tag001d5c;
        };
        if (wordGear(A3+2) == -1)
        {
          w_2 = D4W;
          w_4 = D5W;
        };
//
//
      };
//
//
    };
    if (w_2 != 0)
    {
      D4W = w_2;
      D5W = w_4;
      goto tag001cd2;
    };
  };
tag001d5c:
  word14200 = 0;
  TAG001bee();
}

// *********************************************************
//
// *********************************************************
//  TAG001d6c
i16 EnableButtons(CLICKMAP *P1)
{
  dReg D7;
  pnt *pA4;
  for (D7L = 0, pA4 = pointer14130;
       D7W < 6;
       D7W++, pA4++)
  {
    if (*pA4 != NULL) continue;
    *pA4 = (pnt)P1;
    ClearKeyboard();
    return D7W;  
//
//
  };
  return -1;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG001dde(S6 *P1)
{//(i32)
  static dReg D0, D1, D7;
  static aReg A0, A3;
  static S6  *s6A0, *s6A1, *s6A4;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
  s6A4 = P1;
  for (;;)
  //while (pumper(), CONSTAT() != 0)
  {
    wvbl(_1_);
    if (UI_CONSTAT() == 0) break;
    D0L = UI_DIRECT_CONIN();
    TAG001c0c(D0L);
  };
  word14200 = 1;
  D7W = sw(word14202 + 1);
  if (D7W > 4)
  {
    D7W = 0;
  }
  if (D7W == word14204)
  {
    s6A1 = &f.s6_14160;
    *s6A4 = *s6A1;
  }
  else
  {
    s6A0 = &f.s6_14190[word14204];
    *s6A4 = *s6A0;
    word14204++;
    if (word14204>4) word14204 = 0;
  };
  word14200 = 0;
  TAG001bee();
  if (s6A4->b0 != 0)
  {
    D0W = s6A4->b1;
    D7W = D0W;
    A0 = pointer14130[D0W];
    s6A4->b1 = *A0;
    A3 = (aReg)LoadPnt(A0+10);
    if (A3 != NULL)
    {
      NotImpMsg(0x1ea0);//(*A3)(A4);
    };
  };
  if (s6A4->b0 == 0) s6A4->b1 = 0;
  D0W = sw(s6A4->b1 << 8);
  D1W = s6A4->b0;
  RETURN_int(sw(D0W | D1W));
}

void TAG001ec0(pnt a,pnt b,wordRectPos *c,
               i16 d,i16 e,i16 ff,i16 g,i16 h)
{
  TAG0088b2((ui8 *)a,(ui8 *)b,(RectPos *)c,d,e,ff,g,h);
}

// *********************************************************
//
// *********************************************************
void TAG0025a0(ui8 *a,ui8 *b,i16 c,i16 d,i16 e,i16 ff,ui8 *g)
{
  ShrinkBLT(a,b,c,d,e,ff,g);
}

// *********************************************************
//
// *********************************************************
void  TAG002798(ui8 *P1,wordRectPos *P2,i16 P3,i16 P4)
{
  FillRectangle(P1, (RectPos *)P2, P3, P4);
}

// *********************************************************
//
// *********************************************************
void TAG0029de()
{
  NotImpMsg(0x29de);
}

// *********************************************************
//
// *********************************************************
void TAG002a2a()
{
  NotImpMsg(0x2a2a);
}

// *********************************************************
//
// *********************************************************
void TAG002a5a(i16 P1)
{
  i16  w_138[57];
  i16  w_24[10];
  i16  w_4;
  i16  w_2;
  TAG00a238();
  f.Word13878 = TAG00aa22(&w_2, &w_2, &w_2, &w_2);
  for (w_2=0; w_2<10; w_2++)
  {
    w_24[w_2] = 1;
//
  };
  w_4 = 2;
  if (P1 == 0)
  {
    TAG009f1c(w_24, &f.Word13878, w_138);
  }
  else
  {
    TAG0029de();
    TAG009ea0(w_24, &f.Word13878, w_138);
    TAG002a2a();
  };
}

// *********************************************************
//
// *********************************************************
void TAG002ae4()
{
  TAG009f80(f.Word13878);
  TAG00a28c();
}

// *********************************************************
//
// *********************************************************
void TAG002af4()
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;
  D0W = f.Word12824;
  f.Word12824++;
  if (D0W == 0)
  {
    TAG009d5e(f.Word12804);
  };
}

// *********************************************************
//
// *********************************************************
void TAG002b0c()
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;
  D0W = f.Word12824;
  f.Word12824--;
  if (D0W == 1)
  {
    TAG009d36(f.Word12804, 1);
  };
  return;
}

// *********************************************************
//
// *********************************************************
void TAG002b3c(i16 P1, i16 P2)
{
  TAG00aa58(0, P2);
  TAG009d36(P1, 0);
}

// *********************************************************
//
// *********************************************************
void TAG002b5c(i16 P1, i16 P2)
{
  i32 i_10;
  i16 w_6;
  switch (P1)
  {
  case 1:
      f.Word12804 = P2;
      TAG009d7e(f.Word12804, &w_6, &f.Word12820, &f.Word12818);
      TAG009dea(f.Word12804, (i32)TAG002bf4, &f.Long12814);
      TAG009db6(f.Word12804, (i32)TAG002c1a, &f.Long12810);
      break;
  case 2:
      TAG009dea(f.Word12804, f.Long12814, &i_10);
      TAG009db6(f.Word12804, f.Long12810, &i_10);
      break;
  };//switch
}

// *********************************************************
//
// *********************************************************
void TAG002bf4()
{
  /*
*/
}

// *********************************************************
//
// *********************************************************
void TAG002c1a()
{
  NotImpMsg(0x2c16);
  /*
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
*/
}

// *********************************************************
//
// *********************************************************
i32 TAG002c5e(const char* P1,pnt P2,pnt P3)
{//(i32)
  dReg D7;
  aReg A0, A1, A3, A4;
  i16 w_20;
  i32 i_18;
  i32 i_10;
  i32 i_6;
  i16 w_2;
  A4 = P2;
  wordGear(A4) = TAG00101c(); //Get a FILE_DESCRIPTOR
  if (wordGear(A4) != 0)
  {
    return (0);
  };
  TAG0076a0_40(14, wordGear(A4), P1, -1);//Set filename=*P1
  i_18 = TAG001096_2(wordGear(A4)); //Open the file
  if (i_18 != 0)
  {
    w_20 = 0;
  }
  else
  {
    SetBufferLength(wordGear(A4), 2);
    SetBufferAddress(wordGear(A4), (pnt)&w_2);
    i_18 = ReadFile(wordGear(A4));//read w_2
    w_2 = LE16(w_2);
    if (i_18 != 0)
    {
      w_20 = 0;
    }
    else
    {
      StorePnt(A4+4, AssignMemory(17, 4*(w_2+1)));
      SetBufferLength(wordGear(A4), 2*w_2);
      SetBufferAddress(wordGear(A4), (pnt)LoadPnt(A4+4));
      i_18 = ReadFile(wordGear(A4));//read 4 words
      if (i_18 != 0)
      {
        w_20 = 0;
      }
      else
      {
        wordGear(A4+2) = sw(w_2 - 1);
        A3 = (aReg)LoadPnt(A4+4); //the 4 words we just read
        if (P3 != 0)
        {
 
          StorePnt(P3, AssignMemory(14, 2*(w_2 + 1)));
          MemoryMove(LoadPnt(A4+4),
                     LoadPnt(P3),
                     1024,
                     1024,
                     2*(w_2 + 1) );

        };
        for (D7W = sw(w_2-1); D7W >= 0; D7W--)
        {
          A0 = A3 + 2*D7W;
          A1 = (aReg)LoadPnt(A4+4) + 4*D7W;
          StoreLong(A1, LE16(wordGear(A0)));
//
//
        };
        i_6 = 4*w_2 + 2; // Offset of first segment
        for (D7L = 0; D7W <= w_2; D7W++)
        {
          A0 = (aReg)LoadPnt(A4+4) + 4*D7W;
          i_10 = LoadLong(A0);
          //A0 = LoadPnt(A4+4) + 4*D7W;
          StoreLong(A0, i_6);//file offset of each of 4 segments
          i_6 += i_10;
//
//
        };
        f.pDatIndex12802 = (DATINDEX *)A4;
        return (0);
      };
    };
  };
  TAG001096_10(wordGear(A4));
  TAG001064(wordGear(A4));
  return (1);
}

// *********************************************************
//
// *********************************************************
void TAG002e64()
{//()
  DATINDEX *DatIA4;
  DatIA4 = f.pDatIndex12802;
  TAG001096_10(DatIA4->handle0);
  TAG001064(DatIA4->handle0);
  ReleaseMem(17, (ui8 *)DatIA4->fileOffsets4);
  TAG0060c4();
  return;
}


// *********************************************************
//
// *********************************************************
//  TAG002eb8
i32 ReadDatSeg(pnt P1, i16 P2)
{ // Read sement of HCSB.DAT
  dReg D0;
  i32 *iA0, *iA1;
  DATINDEX *DatIA4;
  i32 i_8;
  i32 i_4;
  DatIA4 = f.pDatIndex12802;
  iA0 = &DatIA4->fileOffsets4[P2];;
  SetBufferLength(DatIA4->handle0, *iA0); //Actually the file offset
  i_4 = FileSeek(DatIA4->handle0);//LSEEK to start of segment
  if (i_4 != 0)
  {
    i_8 = 66;
  }
  else
  {
    iA0 = &DatIA4->fileOffsets4[P2];
    iA1 = &DatIA4->fileOffsets4[P2+1];
    D0L = *iA1;
    D0L -= *iA0;
    SetBufferLength(DatIA4->handle0, D0L);
    SetBufferAddress(DatIA4->handle0, P1);
    i_4 = ReadFile(DatIA4->handle0);
    if (i_4 != 0)
    {
      i_8 = 67;
    }
    else
    {
      i_8 = 0;
    };
  };
  return i_8;
}

// *********************************************************
//
// *********************************************************
i32 TAG002f6c(ui8 *P1, i16 P2, i16 P3, i16 P4)
{//(i32)
  dReg D0;
  i32  *iA0;
  i32 i_10;
  pnt pnt_4;
  i_10 = 0x1baddade;
  iA0 = &f.pDatIndex12802->fileOffsets4[P2];//offset of this segment
  D0L = iA0[1] - iA0[0];// segment length
  pnt_4 = AssignMemory(17, D0L);
  if (pnt_4 != NULL)
  {
    i_10 = ReadDatSeg(pnt_4, P2);
    if (i_10 == 0)
    {
      ExpandGraphic(pnt_4, //src
                    P1,    //dst
                    P3,    //X
                    P4);   //Y
    };
    ReleaseMem(17, (ui8 *)pnt_4);
  }
  else
  {
    TAG00aef0(2);
  };
  return (i_10);
}

// *********************************************************
//
// *********************************************************
i32 TAG003006(ui8 *P1, i16 P2)
{//(i32)
  return (TAG002f6c(P1, P2, 0, 0));
}

// *********************************************************
//
// *********************************************************
//  TAG003022
i32 LZWRawChar(i16 P1, i32 *P2)
{//(i32)
  dReg D0, D1, D5, D7;
  aReg A0, A4;
  i32 bitNumber;
  i32 i_4;
  A4 = f.Byte8898;
  if (   (f.Word8884 > 0) 
      || (f.Word8864 >= f.Word8862) 
      // || (f.Word8886 > word8902)   )
      || (f.Word8886 > codewordMask)   )
  {
    //if (f.Word8886 > word8902)
    if (f.Word8886 > codewordMask)
    {
      bitsPerCodeword++; //word8904++;
      // if (word8904 == 12)
      if (bitsPerCodeword == 12)
      {
        //word8902 = f.Word8900;
        codewordMask = f.Word8900;
      }
      else
      {
        //word8902 = sw((1 << word8904) - 1);
        //word8902 = sw((1 << bitsPerCodeword) - 1);
        codewordMask = sw((1 << bitsPerCodeword) - 1);
      };
    };
    if (f.Word8884 > 0)
    {
      //word8904 = 9;
      bitsPerCodeword = 9;
      //word8902 = sw((1 << word8904) -1);
      //word8902 = sw((1 << bitsPerCodeword) -1);
      codewordMask = sw((1 << bitsPerCodeword) -1);
      f.Word8884 = 0;
    };
    //if (word8904 < *P2)
    if (bitsPerCodeword < *P2)
    {
      f.Word8862 = bitsPerCodeword; //word8904;
    }
    else
    {
      f.Word8862 = sw(*P2);
    };
    if (f.Word8862 > 0)
    {
      SetBufferLength(P1, f.Word8862);
      SetBufferAddress(P1, f.Byte8898);
      i_4 = ReadFile(P1);
      if (i_4 != 0)
      {
        die (0,"Error decompressing file");
        //TAG0093a0_18(_1_, 3, 93);
      };
      *P2 -= f.Word8862;
    }
    else
    {
      return (-1);
    };
    f.Word8864 = 0;
    //f.Word8862 = sw((8 * f.Word8862) - word8904 + 1);
    f.Word8862 = sw((8 * f.Word8862) - bitsPerCodeword + 1);
  };
  bitNumber = f.Word8864;
  D5W = bitsPerCodeword; //word8904;
  A4 += bitNumber >> 3;
  bitNumber &= 7;
  A0 = A4;
  A4++;
  D0L = 0;
  D0B = *A0;
  D7W = sw(D0W >> bitNumber);
  D5W = sw(D5W -(8-bitNumber));
  bitNumber = sw(8-bitNumber);
  if (D5W >= 8)
  {
    A0 = A4++;
    D0L = 0;
    D0B = *A0;
    D0L <<= bitNumber;
    D7W |= D0W;
    bitNumber += 8;
    D5W -= 8;
  };
  D0L = 0;
  D0B = *A4;
  D1W = byte8873[D5W];
  D0W &= D1W;
  D7W |= D0W << bitNumber;
  f.Word8864 = sw(f.Word8864 + bitsPerCodeword); //word8904);
  return (D7W);
}

// *********************************************************
//
// *********************************************************
void TAG003194(i16 P1, pnt *P2)
{
  dReg D0, D7;
  aReg A4;
  D7B = (i8)P1;
  A4 = *P2;
  switch (word8906)
  {
  case 0:
    D0W = D7B;
    if (D0W == 144)
    {
      word8906 = 1;
    }
    else
    {
      f.Word8860 = D7B;
      *(A4++) = D7B;
    };
    *P2 = A4;
    break;
  case 1:
    if (D7B != 0)
    {
      while ((--D7B) != 0)
      {
        *(A4++) = (i8)f.Word8860;
//
      };
    }
    else
    {
      *(A4++) = (unsigned)0x90;
    };
    word8906 = 0;
    *P2 = A4;
  };//switch
}

// *********************************************************
//
// *********************************************************
i32 TAG003210(i16 handle,   // @8
              i32 fileSize, // @10
              pnt rsltAddr, // @14
              pnt tmpBuf1,  // @18
              pnt tmpBuf2)  // @22
{//(i32)
  // P1 = handle
  // P2 = hint size in file
  // P3 = result buffer address
  // P4 = temporary buffer
  // P5 = temporary buffer
  static int count=0;
  dReg D4, D5, D6, D7;
  aReg A3, A4;
  pnt  pnt_12;
  pnt  pnt_8;
  pnt  pnt_4;
  A4 = tmpBuf1;  
  A3 = tmpBuf1+10006;
  pnt_8 = tmpBuf2;
  pnt_4 = tmpBuf2;
  pnt_12 = rsltAddr;
  word8906 = 0;
  D6W = 12;
  bitsPerCodeword = 9; //word8904 = 9;
  f.Word8884 = 0;
  //word8904 = 9;
  //word8902 = sw((1 << word8904) - 1);
  //word8902 = sw((1 << bitsPerCodeword) - 1);
  codewordMask = sw((1 << bitsPerCodeword) - 1);
  for (D6W=255; D6W>=0; D6W--)
  {//looks like preparation for LZW decompression
    wordGear(A4 + 2*D6W) = 0;
    A3[D6W] = D6B;
//
//
  };
  f.Word8886 = 257;
  D5W = sw(LZWRawChar(handle, &fileSize));//Get LZW 9-bit character
  D7W = D5W;
  if (D5W == -1)
  {
    return (-1);
  };
  TAG003194((UI8)(D7B), &rsltAddr);//Store resulting character
  while ((D6W=sw(LZWRawChar(handle, &fileSize))) > -1)
  { // while 9-bit character available
    if (D6W > codewordMask)
    {
      die(0x78a4, "LZW Decode Error- Code too large");
    };
    if (D6W == 256)
    {
      ClearMemory((ui8 *)A4, 512);
      f.Word8884 = 1;
      f.Word8886 = 256;
      D6W = sw(LZWRawChar(handle, &fileSize));
      if (D6W == -1) break;
    };
    D4W = D6W;
    if (D6W >= f.Word8886)
    {
      if (D6W > f.Word8886+1)
      {
        die (0x4aab, "LZW Decode Error-Code too large");
      };
      *(pnt_4++) = D7B;    
      D6W = D5W;
    };
    while (D6W >= 256)
    {
      *(pnt_4++) = A3[D6W];
      D6W = wordGear(A4 + 2*D6W);
//
    };
    D7W = (UI8)(A3[D6W]);
    *(pnt_4++) = D7B;
    do 
    {
      pnt_4--;
      TAG003194((UI8)(*pnt_4), &rsltAddr);
    } while (pnt_4 > pnt_8);
    D6W = f.Word8886;
    if (f.Word8886 < word8900)
    {
      wordGear(A4 + 2*D6W) = D5W;
      A3[D6W] = D7B;
      f.Word8886 = sw(D6W + 1);
    };
    D5W = D4W;
  };
  return (rsltAddr - pnt_12);
}

// *********************************************************
//
// *********************************************************
void TAG00339e(wordRectPos *P1, i16 P2)
{
  TAG002798((ui8 *)f.Pointer12798, P1, P2, 160);
}

// *********************************************************
//
// *********************************************************
pnt TAG0035e6(pnt P1)
{
  dReg D0;
  aReg A4;
  A4 = (aReg)LoadPnt(P1);
  A4 -= 2;
  D0L = (UI16)(wordGear(A4));
  D0L += 12;
  return A4 - D0L;
}

// *********************************************************
//
// *********************************************************
void TAG00364c(pnt P1, S18 *P2)
{
  aReg A2, A3, A4;
  A4 = P1;
  A2 = (aReg)LoadPnt(A4+4);
  A3 = (aReg)LoadPnt(A4+8);
  if (A2 != NULL)
  {
    StorePnt(A2+8, A3);
  }
  else
  {
    P2->pnt10 = A3;
  };
  if (A3 != NULL) StorePnt(A3+4, A2);
}

// *********************************************************
//
// *********************************************************
S18 *TAG00360a(pnt P1)
{
  dReg  D7;
  aReg  A4;
  S18  *s18A3;
  A4 = P1;
  for (D7L = 0, s18A3 = f.s18_124; 
       D7W < f.Word126; 
       D7W++, s18A3++)
  {
    if (A4 >= s18A3->pnt0)
    {
      if (A4 <= s18A3->pnt0 + s18A3->i6) break;
    };

//
  };
  if (D7W == f.Word126)
  {
    s18A3 = NULL;
  };
  return s18A3;
}

// *********************************************************
//
// *********************************************************
void TAG003682(pnt P1, S18 *P2)
{
  dReg D7;
  aReg A2, A3, A4;
  A4 = P1;
  A3 = P2->pnt10;
  A2 = NULL;
  D7L = LoadLong(A4) & 0x7fffffff;
  while (A3 != NULL)
  {
    if (D7L > (LoadLong(A3) & 0x7fffffff)) break;
    A2 = A3;
    A3 = (aReg)LoadPnt(A3+8);
//
  };
  if (A2 != NULL)
  {
    StorePnt(A2+8, A4);
  }
  else
  {
    P2->pnt10 = A4;
  };
  if (A3 != NULL)
  {
    StorePnt(A3+4, A4);
  };
  StorePnt(A4+8, A3);
  StorePnt(A4+4, A2);
}

// *********************************************************
//
// *********************************************************
void TAG0036e4(pnt P1, i32 P2, S18 *P3)
{
  dReg  D6, D7;
  aReg  A0, A4;
  S18  *s18A3;
  A4 = P1;
  D7L = P2;
  s18A3 = P3;
  D6L = D7L | 0x80000000;
  StoreLong(A4, D6L);
  A0 = A4 + D7L;
  StoreLong(A0-4, D6L);
  TAG003682(A4, s18A3);
}

// *********************************************************
//
// *********************************************************
void TAG003720(pnt P1, i32 P2, S18 *P3)
{
  dReg D6, D7;
  aReg A0, A4;
  S18 *s18A3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = P1;
  D7L = P2;
  s18A3 = P3;
  D6L = LoadLong(A4-4);
  if (D6L & 0x80000000)
  {
    D6L &= 0x7fffffff;
    A4 -= D6L;
    TAG00364c(A4, s18A3);
    D7L += D6L;
  };
  A0 = A4 + D7L;
  D6L = LoadLong(A0);
  if (D6L & 0x80000000)
  {
    D6L &= 0x7fffffff;
    TAG00364c(A4+D7L, s18A3);
    D7L += D6L;
  };
  TAG0036e4(A4, D7L, s18A3);
}

// *********************************************************
//
// *********************************************************
pnt TAG003788(i32 P1, S18 *P2, i16 *P3)
{
  dReg D0, D6, D7;
  aReg  A2, A3;
  S18  *s18A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7L = P1;
  s18A4 = P2;
  D7L |= 0x80000000;
  A3 = s18A4->pnt10;
  A2 = NULL;
  while (A3 != NULL)
  {
    D6L = LoadLong(A3);
    if (D6L < D7L) break;
    D0L = D6L - D7L;
    if (D0L <= 34) A2 = A3;
//
    A3 = (aReg)LoadPnt(A3+8);
//
  };
  if (A2 == NULL)
  {
    A3 = s18A4->pnt10;
    if (D7L > LoadLong(A3)) goto tag0037e0;
    A2 = A3;
    *P3 = 0;
  }
  else
  {
    *P3 = 1;
  };
tag0037e0:
  return A2;
}

// *********************************************************
//
// *********************************************************
i16 TAG003b3a(S18 *,i32)
{
  NotImpMsg(0x3b3a); return 0;
}

// *********************************************************
//
// *********************************************************
pnt TAG003c50(i16 P1)
{
  dReg D6, D7;
  aReg A3, A4;
  S18  *s18A2;
  S12  s_12;
//;;;;;;;;;;;;;;;;;;;;;;;
  D7W = P1;
  A4 = NULL;
  D6L = 4 * (UI16)(D7W) + 20;
  A3 = f.Pointer132;
  s_12.w4 = 1024;
  s_12.i0 = D6L;
  s18A2 = TAG003df0(&s_12);
  if (s18A2 == NULL) return A4;
  A4 = TAG003efc(D6L, 1024, s18A2);
  if (A4 == NULL) return A4;
  A4[4] = -1;
  A4[5] = -128;
  StorePnt(A4+6, NULL);
  wordGear(A4+10) = D7W;
  wordGear(A4+12) = 0;
  wordGear(A4+14) = 0;
  ClearMemory((ui8 *)A4+16, D6L-20);
  if  (A3 != NULL)
  {
    while (LoadPnt(A3+6) != NULL) A3 = (aReg)LoadPnt(A3+6);
//

    StorePnt(A3+6, A4);
  }
  else
  {
    f.Pointer132 = A4;
  };
  return A4;
}

// *********************************************************
//
// *********************************************************
pnt TAG003cf6(pnt *P1)
{
  dReg D5, D6, D7;
  aReg A2, A3;
  pnt *pntA4;
//;;;;;;;;;;;
  A3 = NULL;
  pntA4 = P1;
  D5L = 0;
  for (A2=f.Pointer132; (A2 != NULL); A2=(aReg)LoadPnt(A2+6))
  {
    if (wordGear(A2+12) >= wordGear(A2+10)) continue;
    D6W = wordGear(A2+14);
    A3 = A2 + (UI16)(4*D6W) + 16;
    for (D7W = wordGear(A2+10); D7W-- != 0; D6W++, A3+=4)
    {
      if (wordGear(A2+10) == 0)
      {
        D6L = 0;
        A3 = A2 + 16;
      };
      if (A3 == NULL)
      {
        wordGear(A2+14) = D6W;
        wordGear(A2+12)++;
        D5L = 1;
        break;
      };
//
    };
    if (D5W != 0) break;
//
//
  };
  if (D5W == 0)
  {
    A2 = TAG003c50(f.Word128);
    if (A2 != NULL)
    {
      A3 = A2 + 16;
      wordGear(A2+12)++;
      D5L = 1;
    };
  };
  if (D5W == 0)
  {
    A3 = NULL;
  };
  *pntA4 = A2;
  return A3;
}

// *********************************************************
//
// *********************************************************
void  TAG003d9e(pnt P1)
{
  aReg A3, A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = P1;
  for (A3 = (aReg)f.Pointer132; A3 != NULL; A3 = (aReg)LoadPnt(A3+6))
  {
    if (A3 > A4) continue;
    if (A4 >= A3 + 20 + 4*(UI16)(wordGear(A3+10))) continue;
    StoreLong(A4, 0);
    wordGear(A3+12)--;
    wordGear(A3+14) = sw((A4 - A3 - 16)/4);
    return;
//
//
  };
}

// *********************************************************
//
// *********************************************************
S18 *TAG003df0(S12 * P1)
{
  dReg D0, D1, D3, D4, D5, D6, D7;
  aReg A0;
  S18  *s18A2, *s18A3;
  S12  *s12A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  s12A4 = P1;
  if (s12A4->w4 & 1)
  {
    D7L = 1;
    s18A3 = &f.s18_124[0]; // The first one
  }
  else
  {
    D7L = -1;
    s18A3 = &f.s18_124[f.Word126 - 1];//The last one.
  };
  D5L = s12A4->i0;
  s18A2 = s18A3;
tag003e26:
  D3L = 0;
  D4L = 0;
  for (D6L=0; D6W<f.Word126; D6W++, s18A3+=D7W)
  {
    D0W = sw(s18A3->w4 & s12A4->w4);
    D1W = (I16)(s12A4->w4 & 0x7f8);
    if (D0W != D1W) continue;
    D4L = 1;
    A0 = s18A3->pnt10;
    D0L = LoadLong(A0) & 0x7fffffff;
    if (D0L < D5L) continue;
    D3L = 1;
    break;
//
//
  };
  if (D4W == 0)
  {
    TAG00189c(1, 0x010d0005, (char *)((int)(UI16)(s12A4->w4)));
  };
  if (D3W == 0)
  {
    for (s18A3 = s18A2, D6L = 0;
         D6W < f.Word126;
         D6W++, s18A3 += D7W)
    {
      D0W = (I16)(s12A4->w4 & s18A3->w4);
      D1W = (I16)(s12A4->w4 & 0x7f8);
      if (D1W != D0W) continue;
      D0W = TAG003b3a(s18A3, D5L);
      if (D0W != 0) break;  
//
    };
    if (D6W == f.Word126)
    {
      D0L = TAG00189c(0, 0x010d0001, (char *)((int)(UI16)(s12A4->w4)));
      if (D0L == 0x0102fffe)
      {
        s18A3 = s18A2;
        goto tag003e26;
      };
      s18A3 = NULL;
    };
  };
  return s18A3;
}

// *********************************************************
//
// *********************************************************
pnt TAG003efc(i32 P1, i16 P2, S18 *P3)
{
  dReg D5, D6, D7;
  aReg A0, A2, A3;
  S18  *s18A4;
  i16  w_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7L = P1;
  D6W = P2;
  s18A4 = P3;
  A3 = TAG003788(D7L, s18A4, &w_2);
  if (A3 != NULL)
  {
    TAG00364c(A3, s18A4);
    D5L = LoadLong(A3) & 0x7fffffff;
    if (w_2 != 0)
    {
      D7L = D5L;
    }
    else
    {
      D5L -= D7L;
      if (D6L & 1)
      {
        A2 = A3 + D7L;
      }
      else
      {
        A2 = A3;
        A3 += D5L;
      };
      TAG0036e4(A2, D5L, s18A4);
    };
    s18A4->i14 -= D7L;
    StoreLong(A3, D7L);
    A0= A3 + D7L;
    StoreLong(A0-4, D7L);
  };
  return A3;
}

// *********************************************************
//
// *********************************************************
pnt TAG003f82(S12 *P1)
{
  dReg D7;
  aReg A0, A2, A3;
  S12 *s12A4;
  pnt   pnt_8;
  S18   *ps18_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  s12A4 = P1;
  D7L = (UI16)(s12A4->w6) + s12A4->i0 + 18;
  if (D7L & 1) D7L++;
  s12A4->i0 = D7L;
  A2 = TAG003cf6(&pnt_8);
  if (A2 == NULL) return NULL;
  ps18_4 = TAG003df0(s12A4);
  if (ps18_4 == NULL) goto tag004038;
  A3 = TAG003efc(D7L, s12A4->w4, ps18_4);
  if (A3 == NULL) goto tag004038;
  StorePnt(A2, A3 + (UI16)(s12A4->w6) + 14);  
  A3[4] = 0;
  A3[5] = (I8)(s12A4->w4 & 7);
  wordGear(A3+10) = s12A4->w4;
  StorePnt(A3+6, A2);
  A3 += 12;
  if ((s12A4->pnt8 != NULL) && (s12A4->w6 != 0) )
  {
    (s12A4->pnt8)(A3);
  };
  A0 = A3 + (UI16)(s12A4->w6);
  wordGear(A0) = s12A4->w6;
  return A2;
tag004038:
  A0 = pnt_8;
  wordGear(A0+12) -= 1;
  return NULL;

}

// *********************************************************
//
// *********************************************************
void TAG00404c(pnt P1, S18 *P2)
{
  dReg D7;
  aReg A4;
  S18 *s18A3;
//;;;;;;;;;;;;;;;;;;;;
  A4 = P1;
  s18A3 = P2;
  D7L = LoadLong(A4);
  s18A3->i14 += D7L;
  TAG003720(A4, D7L, s18A3);
}

// *********************************************************
//
// *********************************************************
i16 TAG004078(i16 P1,i16 P2)
{ // Initialize heap..
  dReg D5, D7;
  aReg A0, A4;
  S18 *s18A3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7L = 300000;
  A4 = (aReg)MALLOC(D7L);
  if (A4 == NULL)
  {
    TAG00189c(1, 0x010d000b, "ERR No system memory");
  };
  f.Pointer136 = A4;
  if ((i32)A4 & 1) {A4++; D7L--;};
  D7L &= -2;
  f.Word126 = 1;
  f.s18_124[0].pnt0 = A4;
  f.s18_124[0].i6 = D7L;
  f.s18_124[0].w4 = 2040;
//
  for (D5L = 0, s18A3 = &f.s18_124[0];
       D5W < f.Word126;
       D5W++, s18A3 ++)
  {
    A4 = s18A3->pnt0;
    StorePnt(A4, NULL);
    A0 = A4 + s18A3->i6;
    StorePnt(A0-4, NULL);
    A4 += 4;
    s18A3->pnt0 = A4;
    s18A3->i6 -= 8;
    s18A3->i14 = s18A3->i6;
    s18A3->pnt10 = NULL;
    TAG0036e4(A4, s18A3->i14, s18A3);
//
//
//
  };
  f.Word128 = P2;
  f.Pointer132 = NULL;
  A0 = TAG003c50(P1);
  if (A0 == NULL)
  {
    TAG00189c(1, 0x010d000b, "ERR Can't allocate first Handle block");
  };
  return 0;
}

// *********************************************************
//
// *********************************************************
void TAG00418a()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  MFREE((ui8 *)f.Pointer136);
}

// *********************************************************
//
// *********************************************************
pnt TAG00419a(i32 P1,i16 P2)
{
  S12 s12;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  s12.i0 = P1;
  s12.w4 = P2;
  s12.w6 = 0;
  s12.pnt8 = 0;
  return TAG003f82(&s12);
//  
}

// *********************************************************
//
// *********************************************************
i32 TAG0041c8(pnt P1)
{
  dReg D7;
  aReg A3, A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = P1;
  D7L = TAG004316(A4);
  if (D7L == 0)
  {
    A3 = TAG0035e6(A4);
    TAG00404c(A3, TAG00360a(A3));
  };
  if ( (D7L == 0) || (D7L==0x010d0003) )
  {
    TAG003d9e(A4);
    D7L = 0;
  };
  return D7L;
}

// *********************************************************
//
// *********************************************************
pnt TAG00421e(pnt P1)
{
  dReg D7;
  aReg A2, A3, A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = P1;
  A2 = NULL;
  D7L = TAG004316(A4);//Check legal address
  if (D7L == 0)
  {
    A3 = TAG0035e6(A4);
    A3[4]++;
    A2 = (aReg)LoadPnt(A4);
  };
  return A2;
}

// *********************************************************
//
// *********************************************************
void TAG004254(pnt P1)
{
  aReg A3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A3 = TAG0035e6(P1);
  A3[4]--;
}

// *********************************************************
//
// *********************************************************
i32 TAG004316(pnt P1)
{
  dReg D7;
  aReg A3, A4;
  D7L = 0x1baddade;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = P1;
  if ( (A4 == NULL) || (LoadPnt(A4) == NULL) )
  {
    TAG00189c(1, 0x010d0002, (char *)A4);
  }
  else
  {
    if (LoadLong(A4) == -1)
    {
      D7L = 0x010d0003;
    }
    else
    {
      A3 = TAG0035e6(A4);
      if (TAG00360a(A3) == NULL)//is address in one of memory areas
      {
        D7L = 0x010d0002;
      }
      else
      {
        D7L = 0;
      };
    };
  };
  return D7L;
}


// *********************************************************
//
// *********************************************************
void TAG004962(i16 P1, i16 P2, pnt P3)
{
  dReg D0;
  aReg A4;
  i16 w_14;
  i16 w_12;
  i8  b_9;
  wordRectPos rectPos_8;
//;;;;;;;;;;;;;;;;;;;;;;;
  A4 = P3;
  rectPos_8.y2 = P2;
  rectPos_8.y1 = sw(P2 - 8);
  for (rectPos_8.x1=P1; *A4!=0; rectPos_8.x1+=9)
  {
    D0L = (UI8)(*(A4++));
    b_9 = sb(D0W - ' ');
    if (b_9 == 0) continue;
    rectPos_8.x2 = sw(rectPos_8.x1 + 7);
    w_12 = sw(8 * (b_9 & 0x1f));
    w_14 = sw(9 * (b_9 / 32));
    TAG001ec0(f.Pointer16908,
              f.Pointer12798,
              &rectPos_8,
              w_12,
              w_14,
              128,
              160,
              12);

//
  };
}

// *********************************************************
//
// *********************************************************
i32 TAG0049fe(TEXT *, i32 P2, i32 P3)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (P3 >= P2)
  {
    return 9*(P3-P2+1);
  };
  return 9;
}


// *********************************************************
//
// *********************************************************
//   TAG004a22_6
RESTARTABLE _DisplayText(const i16 P1, const TEXT *nP2)
{//()
  static dReg D0;
  static aReg A0, A1;
  static TEXT *pTextP3;
  static i16 w_6;
  static union 
        {
          i32 i;
          TEXT *pText;
          pnt p;
        } r_4;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
  r_4.i = 0x1baddade;
  pTextP3 = (TEXT *)nP2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(9, P2);
  pTextP3 = (TEXT *)nP2;
  if (pTextP3->w0 != 3)
  {
    TAG0093a0_18(P1, 1);
  };
  f.Word15706=0; 
  f.Word15702 = StrLen(pTextP3->pnt2);
  f.Word15700 = sw(pTextP3->rectPos6.x2 - pTextP3->rectPos6.x1);
  f.Word15698 = sw(pTextP3->rectPos6.y1 + 11);
  while (f.Word15706<f.Word15702)
  {
    A1 = (aReg)pTextP3->pnt2;
    if (A1[f.Word15706] == ' ') {f.Word15706++; continue;};
    f.Word15704 = f.Word15706;
    for (;;)
    {
      if (f.Word15706 >= f.Word15702)
      {
        f.Byte15694 = ' ';
      }
      else
      {
        f.Byte15694 = pTextP3->pnt2[f.Word15706];
      };
      A0 = (aReg)f.Byte15788+f.Word15706-f.Word15704;
      *A0 = f.Byte15694;
      if (f.Byte15694 == ' ') 
      {
        D0W = sw(TAG0049fe(pTextP3,
                            f.Word15704,
                            f.Word15706-1));
        if (D0W > f.Word15700)
        {
          for (f.Word15706--; f.Word15706>f.Word15704; f.Word15706--)
          {
            A0 = (aReg)f.Byte15788+f.Word15706-f.Word15704;
            if (*A0 == ' ') break;
//
          };
          if (f.Word15706 != f.Word15704) break;//do while (1)
          do
          {
            f.Word15706++;
            D0W = sw(TAG0049fe(pTextP3,f.Word15704,f.Word15706));
          } while (D0W < f.Word15700);
          f.Word15706--;
          break;//do while(1)
        };
        if (f.Word15706 >= f.Word15702) break;//do while(1)
        f.Word15706++;
      }
      else
      {
        if (f.Byte15694 == 47)
        {
          A0 = (aReg)f.Byte15788+f.Word15706-f.Word15704;
          *A0 = 0;
          break;//do while (1)
        };   
        f.Word15706++;
      };
    } ;
    A0 = (aReg)f.Byte15788+f.Word15706-f.Word15704;
    *A0 = 0;
    f.Word15696 = pTextP3->rectPos6.x1;
    if (pTextP3->w14 & 1)
    {
      D0W = sw(TAG0049fe(pTextP3, f.Word15704, f.Word15706-1));
      f.Word15696 = sw(f.Word15696 + (f.Word15700-D0W)/2);
    };
    if (f.Word15698 <= pTextP3->rectPos6.y2)
    {
      for (w_6=0; f.Byte15788[w_6]!=0; w_6++)
      {
        if (f.Byte15788[w_6] >= ' ')
        {
          if ((UI8)(f.Byte15788[w_6]) <= 127) continue;
        };
        f.Byte15788[w_6] = 95;
//   
      };
      TAG004962(f.Word15696, f.Word15698, f.Byte15788);
    };
    f.Word15698 += 11;
    if (pTextP3->pnt2[f.Word15706] == 47)
    {
      f.Word15706++;
    };
  } ;//while (f.Word15706)
  //TAG009a1c(9, P2);
  wvbl(_1_);
  RETURN;
}

// *********************************************************
//
// *********************************************************
pnt TAG004a22_14(i16 P1)
{//(pnt)
  union 
        {
          i32 i;
          TEXT *pText;
          pnt p;
        } r_4;
  r_4.i = 0x1baddade;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(9, P2);
  r_4.p = AssignMemory(P1, 22);
  wordGear(r_4.p) = 3;
  //TAG009a1c(9, P2);
  return ((pnt)r_4.i);
}

// *********************************************************
//
// *********************************************************
pnt TAG004a22_16(i16 P1, i32 nP2)
{//(pnt)
  TEXT *pTextP3;
  union 
        {
          i32 i;
          TEXT *pText;
          pnt p;
        } r_4;
  r_4.i = 0x1baddade;
  pTextP3 = (TEXT *)nP2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(9, P2);
  r_4.pText = (TEXT *)TAG004a22_14(P1);
  if (nP2 != 0)
  {
    r_4.pText->pnt2 = (char *)AssignMemory(P1, nP2);
  };
  r_4.pText->rectPos6.x1 = rectPos14516.x1;
  r_4.pText->rectPos6.x2 = rectPos14516.x2;
  r_4.pText->rectPos6.y1 = rectPos14516.y1;
  r_4.pText->rectPos6.y2 = rectPos14516.y2;
  r_4.pText->w18 = 0;
  r_4.pText->w20 = 0;
  r_4.pText->w14 = 1;
  //TAG009a1c(9, P2);
  return ((pnt)r_4.i);
}

// *********************************************************
//
// *********************************************************
//   TAG004a22_20
void Free_TEXT(i16 P1, TEXT *nP2)
{//()
  TEXT *pTextP3;
  union 
        {
          i32 i;
          TEXT *pText;
          pnt p;
        } r_4;
  r_4.i = 0x1baddade;
  pTextP3 = (TEXT *)nP2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(9, P2);
  {
    if (pTextP3->w0 != 3)
    {
      TAG0093a0_18(P1, 2);
    };
    ReleaseMem(P1, (ui8 *)pTextP3->pnt2);
    ReleaseMem(P1, (ui8 *)pTextP3);
  };
  //TAG009a1c(9, P2);
  return;
}

// *********************************************************
//
// *********************************************************
void TAG004a22_26(i16 /*P1*/, i32 nP2, i32 nP3)
{//()
  dReg D0;
  ui8 *A0;
  TEXT *pTextP3;
  union 
        {
          i32 i;
          TEXT *pText;
          pnt p;
        } r_4;
  r_4.i = 0x1baddade;
  pTextP3 = (TEXT *)nP2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(9, P2);
  if (word15708 == -1)
  {
    A0 = byte16544 + nP2;
    D0W = (I16)(nP3 & 0xff);
    A0[0] = sb(10*D0W);
    ASSERT(nP3 != 0x1baddade,"nP3");
    word15708 = sw(nP3);
  }
  else
  {
    A0 = byte16544 + word15708;
    D0W = sw(10*(nP2&0xff));
    A0[0] = D0B;
    word15708 = sw(nP2);
  };
  TAG0025a0((ui8 *)f.Pointer16908,
            (ui8 *)f.Pointer16908,
            256,
            27,
            256,
            27,
            byte16544);
  //TAG009a1c(9, P2);
  //RETURN_int(r_4.i);
  return;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG004e4c_xxx(i16 /*P1*/, i16 P2, i32 P3)
{//()
  static TEXT *pTextP3;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
  pTextP3 = (TEXT *)P3;
//;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(21, P2);
  switch (P2)
  {
  case 6:
    {
      TAG00978a_27(9, 34, (pnt)pTextP3, 6);
      TAG0051c2_31(_1_, 9);
    };
      break;
  case 8:
      NotImpMsg(0x4ea0);
      /*
      do
      {
      } while (D0L != 6);
      */
      break;
  case 7:
  default:
      TAG0093a0_18(-1, 4);
      break;
  }; // switch
  //TAG009a1c(21, P2);
  RETURN;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG004e4c_6(i16 P1,TEXT *P2)
{//()
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
  TAG004e4c_xxx(_1_, P1, 6, (i32)P2);
  RETURN;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG004e4c_8(i16 P1,TEXT *P2)
{//()
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
  TAG004e4c_xxx(_1_, P1, 8, (i32)P2);
  RETURN;
}


// *********************************************************
//
// *********************************************************
pnt TAG004f3a_1(i16 /*P1*/)
{//(pnt)
  i16   w_10;
  pnt   pnt_4 = NULL;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(15, P2);
  f.Word8910 = 0;
  for (w_10=0; w_10<100; w_10++)
  {
    f.Pointer9310[w_10] = NULL;
    f.Pointer9710[w_10] = NULL;
//
//
  };
    TAG004078(100, 50);
  //TAG009a1c(15, P2);
  return (pnt_4);
}

// *********************************************************
//
// *********************************************************
void TAG004f3a_2(i16 /*P1*/)
{//()
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(15, P2);
  TAG00418a();
  //TAG009a1c(15, P2);
  return;
}

// *********************************************************
//
// *********************************************************
//  TAG004f3a_14
pnt AssignMemory(i16 /*P1*/, i32 nP2)
{//(pnt)
  pnt result;
  int len = (nP2 + 3) & 0xfffffffc;
  result = (pnt)malloc(len);
  memset(result,0xcd,len);
  return result;
  /*
  pnt   *pA0;
  pnt   pnt_16;
  pnt   pnt_4 = NULL;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(15, P2);
  if (nP2 > 0xffff)
  {
    NotImpMsg(0x4fda);
 

  };
  pnt_16 = TAG00419a(nP2, 1024);
  if (pnt_16 == NULL)
  {
    die(0,"Cannot allocalte Memory");
    //TAG0093a0_18(_1_, P1, 45);
  };
  pnt_4 = TAG00421e(pnt_16);
  pA0 = &f.Pointer9310[f.Word8910];
  *pA0 = pnt_4;
  pA0 = &f.Pointer9710[f.Word8910];
  *pA0 = pnt_16;
  f.Word8910 += 1;
  if (f.Word8910 > word8908)
  {
    word8908 += 4;
  };
  if (f.Word8910 > 100)
  {
    //TAG0093a0_18(_2_, P1, 98);  
    die (0,"Excessive Memory Allocations");
  };
  //TAG009a1c(15, P2);
  memset(pnt_4, 0xdc, nP2);
  return (pnt_4);
  */
}

// *********************************************************
//
// *********************************************************
pnt TAG004f3a_16(i16 P1, i32 nP2)
{//(pnt)
  pnt   *pA0;
  pnt   pnt_16;
  pnt   pnt_4 = NULL;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(15, P2);
  if (nP2 > 0xffff)
  {
    NotImpMsg(0x4fda);
    /*
*/
  };
  pnt_16 = TAG00419a(nP2, 1024);
  if (pnt_16 == NULL)
  {
    TAG0093a0_18(P1, 45);
  };
  pnt_4 = TAG00421e(pnt_16);
  pA0 = &f.Pointer9310[f.Word8910];
  *pA0 = pnt_4;
  pA0 = &f.Pointer9710[f.Word8910];
  *pA0 = pnt_16;
  f.Word8910 += 1;
  if (f.Word8910 > word8908)
  {
    word8908 += 4;
  };
  if (f.Word8910 > 100)
  {
    TAG0093a0_18(P1, 98);  
  };
  //TAG009a1c(15, P2);
  return (pnt_4);
}

// *********************************************************
//
// *********************************************************
//    TAG004f3a_20
pnt ReleaseMem(i32 /*P1*/, ui8 *nP2)
{//(pnt)
  free (nP2);
  return NULL;
  /*
  pnt   *pA0, *pA1;
  pnt   pnt_16;
  i16   w_10;
  pnt   pnt_4 = NULL;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(15, P2);
  pnt_16 = NULL;
  for (w_10 = 0; w_10 < f.Word8910; w_10++)
  {
    pA0 = &f.Pointer9310[w_10];
    if (*pA0 != (pnt)nP2) continue;
    pA0 = &f.Pointer9710[w_10];
    pnt_16 = *pA0;
    f.Word8910--;
    if (w_10 < f.Word8910)
    {
      pA0 = &f.Pointer9310[w_10];
      pA1 = &f.Pointer9310[f.Word8910];
      *pA0 = *pA1;
      pA0 = &f.Pointer9710[w_10];
      pA1 = &f.Pointer9710[f.Word8910];
      *pA0 = *pA1;
    };
    pA0 = &f.Pointer9310[f.Word8910];
    *pA0 = NULL;
    pA0 = &f.Pointer9710[f.Word8910];
    *pA0 = NULL;
//
//
  };
  if (pnt_16 == NULL)
  {
    die (0,"Error releasing memory");
    //TAG0093a0_18(_3_, P1, 97);
  };
  TAG004254(pnt_16);
  TAG0041c8(pnt_16);
  //TAG009a1c(15, P2);
  return (pnt_4);
  */
}



// *********************************************************
//
// *********************************************************
i16 TAG0051a2(i16 P1, i16 P2)
{
//;;;;;;;;;;;;;;;;;;
  if (P1 > P2) return P1;
  else return P2;
}



// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG0051c2_1(const i16 P1)
{//()
  static dReg D0, D1;
  static pnt *pntA0;
  static i32   i_52;
  static i16   w_48;
  static PALETTEPKT pkt_18;
  RESTARTMAP
    RESTART(5)
    RESTART(19)
    RESTART(20)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(14, P2);
  f.Pointer16900 = NULL;
  f.Pointer16904 = NULL;
  f.Pointer16908 = NULL;
  //f.VBLCnt16888 = 0;//Vertical blank interrupt count
  TAG0063a6(1); // Set some interrupt vectors.
  TAG005d98();
  TAG004f3a_1(14);
  TAG00187e();
  TAG000f50(10);
  D0L = TAG001896();// **019b52
  if (D0L != 0)
  {
    TAG0093a0_18(14, 96);
  };
  for (w_48 = 0; w_48<3; w_48++)
  {
    TAG0076a0_12(_5_, P1, pointer8858[3], 0);// **019b80 "hcsb.dat"
    D0L = intResult;
    if (D0L == 1) break;  //check
//
//
  };
  if (w_48 >= 3)
  {
    TAG0093a0_18(P1, 12);
  };
  i_52 = TAG002c5e(pointer8858[3], f.Byte16896, 0);//read index of HCSB
  if (i_52 != 0)
  {
    //TAG0093a0_18(_29_, P1, i_52);
    die(0,"Error reading hcsb.dat");
  };
  TAG00af06(); // generate secret keys???
  // Now read the first 100-byte segment (0x64)
  i_52 = ReadDatSeg((pnt)f.Segment1_16872, 0);
  if (i_52 != 0)
  {
    TAG0093a0_18(P1, i_52);
  };
  f.Pointer16900 = AssignMemory(P1, 0x08000);
  i_52 = TAG003006((ui8 *)f.Pointer16900, 1);
  if (i_52 != 0)
  {
    TAG0093a0_18(P1, i_52);
  };
  TAG005ae8_1(P1);// creates Palette16608
  f.Pointer16908 = TAG004f3a_16(14, 3456);
  pntResult;
  i_52 = TAG003006((ui8 *)f.Pointer16908, 2);
  if (i_52 != 0)
  {
    TAG0093a0_18(P1, i_52);
  };
  //D0L = (UI16)(LE16(wordGear(f.Segment1_16872+10)));
  //D1L = (UI16)(LE16(wordGear(f.Segment1_16872+8)));
  D0L = (UI16)(LE16(f.Segment1_16872[10]));
  D1L = (UI16)(LE16(f.Segment1_16872[8]));
  TAG004a22_26(P1, D0L, D1L);
  pkt_18.w0 = 13;
  pkt_18.pw2 = NULL;
  pkt_18.pPalette6 = &f.Palette16640;
  pkt_18.w10 = 273;
  pkt_18.w12 = 2;
  pkt_18.w14 = 7;
  PaletteFade(_19_, 14,&pkt_18);
  TAG002a5a(0);
  f.Pointer16506[0] = (pnt)logbase();
  f.Pointer16506[1] = AssignMemory(6, 33024);
  f.Pointer16904 = f.Pointer16506[1];
  f.Word16498 = 1;
  if (f.Pointer16506[f.Word16498] != NULL)
  {
    pntA0 = &f.Pointer16506[f.Word16498];
    //pA1 = &f.Pointer16506[f.Word16498];
    //pA2 = &f.Pointer16506[f.Word16498];
    D0L = 256;
    D0L -= (i32)(*pntA0);
    D0L &= 255;
    D0L += (i32)(*pntA0);
    *pntA0 = (pnt)D0L;//round up to multiple of 256.
  };
  pntA0 = &f.Pointer16506[f.Word16498];
  f.Pointer12798 = *pntA0;
  TAG0088b8(&rectPos14516, (ui8 *)f.Pointer16900, 1);
  TAG002b5c(1, f.Word13878);
  TAG002b3c(f.Word13878, 0);
  TAG0051c2_35(P1);
  f.Word16508++;
  pkt_18.pPalette6 = &f.Palette16608;
  PaletteFade(_20_, 14, &pkt_18); //Window appears
  TAG002e64();
  TAG0078fa(pText16272[12]);
  TAG00636a();
  ClearMemory((ui8 *)f.s4076_8816, 8152);
  RETURN;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG0051c2_2(i32 P1)
{//()
  static dReg D0;
  static pnt *pntA0, *pntA1;
  RESTARTMAP
    RESTART(4)
    RESTART(6)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(14, P2);
  TAG0078fa(pText16272[13]);
  TAG0076a0_12(_6_, P1, pointer8858[3], pText16272[7]);
  TAG002af4();
  if (f.Word16498 == 0)
  {
    D0L = 1;
  }
  else
  {
    D0L = 0;
  };
  if (D0W != 0)
  {
    if (f.Word16498 == 0)
    {
      D0L = 1;
    }
    else
    {
      D0L = 0;
    };
    pntA0 = &f.Pointer16506[D0L];
    pntA1 = &f.Pointer16506[f.Word16498];
    MemoryMove((ui8 *)*pntA0, (ui8 *)*pntA1, 8, 8, 32000);
    TAG0051c2_35(14);
  };
  TAG00799a_2(sw(P1));
  TAG008c40_30(sw(P1));
  TAG001006();
  TAG006388();
  TAG006c7e_2(_4_, P1);
  TAG007fdc_2(sw(P1));
  TAG005ae8_2(sw(P1));
  if (f.Pointer16904 != NULL)
  {
    ReleaseMem(P1, (ui8 *)f.Pointer16904);
    f.Pointer16904 = NULL;
  };
  if (f.Pointer16900 != NULL)
  {
    ReleaseMem(P1, (ui8 *)f.Pointer16900);
    f.Pointer16900 = NULL;
  };
  if (f.Pointer16908 != NULL)
  {
    //ReleaseMem(P1, f.Pointer16908);
    f.Pointer16908 = NULL;
  };
  TAG004f3a_2(14);
  TAG002b5c(2, 0);
  TAG0063a6(2);
  TAG002ae4();
  RETURN;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG0051c2_31(i16 P1)
{//()
  static dReg D0;
  static i16 *wA0;
  static i16   w_48;
  static i16   w_46;
  static PAGE *pPage_40;
  static PALETTEPKT pkt_18;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
    RESTART(10)
    RESTART(11)
    RESTART(21)
    RESTART(22)
    RESTART(23)
    RESTART(24)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(14, P2);
  TAG004a22_26(P1, (UI16)(LE16(f.Segment1_16872[2])),0x1baddade);
  TAG008c40_6(_10_, P1, 0);
  pkt_18.w0 = 13;
  pkt_18.pw2 = NULL;
  pkt_18.pPalette6 = &f.Palette16608;
  TAG005ae8_9(14, &pkt_18);
  wA0 = &f.Palette16608.color[LE16(f.Segment1_16872[0])];
  *wA0 = LE16(f.Segment1_16872[1]);
  pkt_18.w10 = 273;
  pkt_18.w12 = LE16(f.Segment1_16872[12]);//Word16848;
  pkt_18.w14 = LE16(f.Segment1_16872[13]);//Word16846;
  PaletteFade(_21_, P1, &pkt_18); //Fades palette
  pkt_18.w12 = LE16(f.Segment1_16872[10]);
  pkt_18.w14 = LE16(f.Segment1_16872[11]);
  PaletteFade(_22_, P1, &pkt_18);//Fades palette
  ClearKeyboard();
  TAG0051c2_35(P1);
  f.Long16876 = VBLCount;
  TAG004a22_26(P1, (UI16)(LE16(f.Segment1_16872[2])),0x1baddade);
  wvbl(_1_); 
  TAG008c40_6(_11_, P1, 0);//Text appears on buttons
  TAG004a22_26(P1, (UI16)(LE16(f.Segment1_16872[0])),0x1baddade);
  w_46 = sw(TAG00978a_13(9));
  for (w_48 = w_46; w_48>0; w_48--)
  {
    pPage_40 = (PAGE *)TAG00978a_12(9, w_48);
    if (pPage_40->w0 != 8) continue;
    if (pPage_40->pText8->w0 != 3) continue;
    DisplayText(_3_, 9, pPage_40->pText8);
    if (pPage_40->i12 == 8)
    {
      TAG005f9a_1(&pPage_40->pText8->rectPos6,
                  (LE16(f.Segment1_16872[6])),//Word16860,
                  -1);
    };
    if (pPage_40->i4 == 20)
    {
      Free_TEXT(9, pPage_40->pText8);
    };
    TAG00978a_29(9, w_48);
//
//
  };
  f.Palette16608.color[LE16(f.Segment1_16872[0])] 
       = LE16(f.Segment1_16872[3]);
  do
  {
    wvbl(_2_);
    D0L = f.Long16876 + (UI16)(LE16(f.Segment1_16872[9]));
  } while ((unsigned)D0L > VBLCount);
  TAG0051c2_35(P1);
  pkt_18.w12 = LE16(f.Segment1_16872[10]);
  pkt_18.w14 = LE16(f.Segment1_16872[11]);
  PaletteFade(_23_, P1, &pkt_18);//Yellow rectangles appear
  pkt_18.w12 = LE16(f.Segment1_16872[12]);
  pkt_18.w14 = LE16(f.Segment1_16872[13]);
  PaletteFade(_24_, P1, &pkt_18);
  TAG004a22_26(P1, LE16(f.Segment1_16872[4]),0x1baddade);
  RETURN;
}

// *********************************************************
//
// *********************************************************
void TAG0051c2_35(i16 /*P1*/)
{//()
  dReg D0;
  ui8 **pntA0, **pntA1;
  i16   w_48;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(14, P2);
  if (f.Word16498 == 0)
  {
    D0L = 1;
  }
  else
  {
    D0L = 0;
  };
  f.Word16498 = D0W;
  TAG002af4();
  if (f.Word16498 == 0)
  {
    D0L = 1;
  }
  else
  {
    D0L = 0;
  };
  pntA0 = (ui8 **)&f.Pointer16506[D0L];
  if (f.Word16498 != 0)
  {
    D0L = 0;
  }
  else
  {
    D0L = 1;
  };
  pntA1 = (ui8 **)&f.Pointer16506[D0L];
  setscreen(*pntA0, *pntA1, -1);
  TAG002b0c();
  if (f.Word16496 != 0)
  {
    f.Word16496 = 0;
    f.Pointer12798 = (pnt)physbase();
  };
  w_48 = TAG00794e((pnt)TAG006c5e, (pnt)TAG007540);








  pntA0 = (ui8 **)&f.Pointer16506[f.Word16498];
  f.Pointer12798 = (pnt)*pntA0;
  TAG0088b8(&rectPos14516, (ui8 *)f.Pointer16900, 1);
  return;
}

// *********************************************************
//
// *********************************************************
void TAG0051c2_36(i16 /*P1*/)
{
  dReg D0;
  pnt *pntA0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(14, P2);
  if (f.Word16498 == 0)
  {
    D0L = 1;
  }
  else
  {
    D0L = 0;
  };
  pntA0 = &f.Pointer16506[D0L];
  f.Pointer12798 = *pntA0;
  return;
}

// *********************************************************
//
// *********************************************************
void TAG0051c2_37(i16 /*P1*/)
{//()
  pnt *pntA0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(14, P2);
  pntA0 = &f.Pointer16506[f.Word16498];
  f.Pointer12798 = *pntA0;
  return;
}


// *********************************************************
//
// *********************************************************
void TAG0059fe(i32 P1, i32 P2)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  while (P1 < P2)
  {
    P1 = P1 + P2+2;
  };
}


// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG005a1e_xxx(const i16, const i16 P2, const i32 P3)
{//void
  static dReg D0;
  static i16 w_2;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(11, P2);
  switch (P2)
  {
  case 5:
      //w_2 = TAG0083cc((pnt)TAG0051a2, (pnt)TAG0059fe);

      //********** Deleted Nonsense ********//

      w_2 = sw(P3);
      do
      {
        wvbl(_1_);
        D0W = w_2;
        w_2--;
      } while (D0W != 0);
      break;
  default:
      NotImpMsg(0x5ac4);
      /*
*/
  };//switch
  //TAG009a1c(11, P2);
  RETURN;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG005a1e_5(i32 P1, i32 P2)
{ //()
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
  //sleep for P2 VBLs
  TAG005a1e_xxx(_1_, P1, 5, P2);
  RETURN;
}


// *********************************************************
//
// *********************************************************
void TAG005ae8_1(i16 P1)
{//()
  i32 i;
  i32  i_28;
  PALETTEPKT  pkt_24;
//;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(22, P2);
  i_28 = ReadDatSeg((pnt)&f.Palette16608, 3);//read palette
  if (i_28 != 0)
  {
    TAG0093a0_18(P1, 212);
  };
  for (i=0; i<16; i++)
    f.Palette16608.color[i] = LE16(f.Palette16608.color[i]);
  pkt_24.w0 = 13;
  pkt_24.pw2 = NULL;
  pkt_24.pPalette6 = &f.Palette16672;
  TAG005ae8_9(P1, &pkt_24);
  //TAG009a1c(22, P2);
  return;
}

// *********************************************************
//
// *********************************************************
void TAG005ae8_2(i16 /*P1*/)
{//()
//;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(22, P2);
  //TAG009a1c(22, P2);
  return;
}

// *********************************************************
//
// *********************************************************
void TAG005ae8_9(i16 P1, PALETTEPKT *nP2)
{//()
  i16  *wA1;
  PALETTEPKT *pktA0;
  i16 *pw_6;
  i16  w_2;
//;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(22, P2);
  pktA0 = (PALETTEPKT *)nP2;
  if (pktA0->w0 != 13)
  {
    TAG0093a0_18(P1, 80);
  };
  if (pktA0->pw2 == NULL)
  {
    pw_6 = globalPalette;
  }
  else
  {
    pw_6 = pktA0->pw2;
  };
  if (pktA0->pPalette6 == NULL)
  {
    pktA0->pPalette6 = (PALETTE *)globalPalette;
  };
  SetSupervisorMode();
  for (w_2=0; w_2<16; w_2++)
  {
    wA1 = &pktA0->pPalette6->color[w_2];
    *wA1 = pw_6[w_2];
//
  };
  ClearSupervisorMode();

  //TAG009a1c(22, P2);
  return;
}

// *********************************************************
//
// *********************************************************
//   TAG005ae8_34
RESTARTABLE _PaletteFade(i16 P1, PALETTEPKT *nP2)
{//()
  static PALETTEPKT *pktA0;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(22, P2);
  pktA0 = (PALETTEPKT *)nP2;
  if (pktA0->w0 != 13)
  {
    TAG0093a0_18(P1, 77);
  };
  FadePalette(_1_,
              pktA0->pPalette6,
              pktA0->pw2,
              pktA0->w10,
              pktA0->w12,
              pktA0->w14);
  //TAG009a1c(22, P2);
  RETURN;
}

// *********************************************************
//
// *********************************************************
void TAG005ae8_38(i16 P1, PALETTEPKT *nP2)
{//()
  PALETTEPKT *pktA0;
//;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(22, P2);
  pktA0 = (PALETTEPKT *)nP2;
  if (pktA0->w0 != 13)
  {
    TAG0093a0_18(P1, 78);
  };
  setpalette(pktA0->pPalette6);
  //TAG009a1c(22, P2);
  return;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG005d2a(i32 P1, i32 P2)
{//()
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(12, P2);
  switch (P2)
  {
  case 0x12:
  case 0x02:
  case 0x1f:
      TAG0051c2_2(_1_, P1);
      HopefullyNotNeeded(0x65eb);

      break;
  default:
      TAG0093a0_18(P1, 16);
  }; //switch
  //TAG009a1c(12, P2);
  RETURN;
}

// *********************************************************
//
// *********************************************************
void TAG005d98()
{ //Set critical error handler to set D0 to -1.
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  SetSupervisorMode();
  //Set critical error handler
  ClearSupervisorMode();
}


// *********************************************************
//
// *********************************************************
//   TAG005e1c
RESTARTABLE _FadePalette(PALETTE *P1,
                        const i16 *P2,
                        const i16 P3,
                        const i16 P4,
                        const i16 p5)
{//()
  static dReg D0, D4, D5, D6, D7;
  static i16  *wA0, *wA1;
  static i16 w_34[16];
  static i16 w_2;
  static i16 P5;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
  P5 = p5;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  SetSupervisorMode();
  if (P1 == NULL)
  {
    P2 = globalPalette;
  };
  if (P2 == NULL)
  {
    P2 = globalPalette;
  };
  for (D7L=0; D7W<16; D7W++)
  {
    w_34[D7W] = P2[D7W];;
//
//
  };
  ClearSupervisorMode();
  while ((P5--) != 0)
  {
    w_2 = P4;
    for (D7L = 0; D7L<16; D7L++)
    {
      for (D6L=0; D6L<3; D6L++)
      {
        wA0 = &word16494[D6L];
        D0W = sw(P3 & *wA0);
        if (D0W != 0)
        {
          wA1 = &w_34[D7W];
          wA0 = &word16494[D6W];
          D5W = sw(*wA1 & *wA0);
          wA1 = &P1->color[D7W];
          wA0 = &word16494[D6W];
          D4W = sw(*wA1 & *wA0);
          if (D4W == D5W) continue;
          if (D5W < D4W)
          {
            wA1 = &w_34[D7W];
            wA0 = &word16488[D6W];
            D0W = *wA0;
            *wA1 = sw(*wA1 + D0W);
          }
          else
          {
            wA1 = &w_34[D7W];
            wA0 = &word16488[D6W];
            D0W = *wA0;
            *wA1 = sw(*wA1 - D0W);
          };
        };
//
//
      };
//
    };
    if (w_2 != 0)
    {
      setpalette((PALETTE *)w_34);
    };
    while ((w_2--) != 0)
    {
      wvbl(_1_);
//
    };
  };//while (P5...)
  if (P4 == 0)
  {
    setpalette((PALETTE *)w_34);
  };
  RETURN;
}

// *********************************************************
//
// *********************************************************
void TAG005f9a_xxx(wordRectPos *P1, i16 P2, i16 P3, i16 P4)
{
  dReg D6, D7;
  wordRectPos rectPos_24;
  wordRectPos rectPos_16;
  wordRectPos rectPos_8;
  wordRectPos *rpA4;
//;;;;;;;;;;;;;;;;;;;;;
  rpA4 = P1;
  D7W = P2;
  if (P4 < 0)
  {
    rectPos_8.x1 = rpA4->x1;
    rectPos_8.x2 = rpA4->x2;
    rectPos_8.y1 = rpA4->y1;
    rectPos_8.y2 = rpA4->y2;
  }
  else
  {
    if (D7W < 0)
    {
      ExpandRectangle(rpA4, &rectPos_8, D7W, D7W);
      D7W = sw(-D7W);
    }
    else
    {
      rectPos_8.x1 = rpA4->x1;
      rectPos_8.x2 = rpA4->x2;
      rectPos_8.y1 = rpA4->y1;
      rectPos_8.y2 = rpA4->y2;
    };
    TAG00339e(&rectPos_8, P4);
  };
  if (D7W == 0) return;
  ExpandRectangle(&rectPos_8, &rectPos_16, D7W, D7W);
  D7W--;
  for (D6L=0; D6L<4; D6L++)
  {
    rectPos_24.x1 = rectPos_16.x1;
    rectPos_24.x2 = rectPos_16.x2;
    rectPos_24.y1 = rectPos_16.y1;
    rectPos_24.y2 = rectPos_16.y2;
    switch (D6W)
    {
    case 0:
      rectPos_24.x2 = sw(rectPos_24.x1 + D7W);
      break;
    case 1:
      rectPos_24.x1 = sw(rectPos_24.x2 - D7W);
      break;
    case 2:
      rectPos_24.y2 = sw(rectPos_24.y1 + D7W);
      break;
    case 3:
      rectPos_24.y1 = sw(rectPos_24.y2 - D7W);
      break;
    default:
      break;
    }; //switch
    TAG00339e(&rectPos_24, P3);
//
//
  };
}

// *********************************************************
//
// *********************************************************
void TAG005f9a_1(wordRectPos *P1, i16 P2, i16 P3)
{
  TAG005f9a_xxx(P1, 1, P2, P3);
}

// *********************************************************
//
// *********************************************************
//   TAG0060a4
i16 MinA(i16 P1, i16 P2)
{
//;;;;;;;;;;;;;;;;;;
  if (P1 > P2) return P2;
  else return P1;
}

// *********************************************************
//
// *********************************************************
i32 TAG0060c4()
{//(i32)
  dReg D7;
  ui32 *pi_12;
  pnt pnt_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pnt_4 = AssignMemory(14, 576);
  pi_12 = &VBLCount;
  D7L = 0;
  SetSupervisorMode(); //Note no pop...prepare for Clear
/*
  */
  ClearSupervisorMode();
  D7L = 7;
  ReleaseMem(14, (ui8 *)pnt_4);
  return (1);
}

// *********************************************************
//
// *********************************************************
//  TAG006282
i16 MinB(i16 P1, i16 P2)
{
//;;;;;;;;;;;
  if (P1 > P2) return P2;
  else return P1;
}

// *********************************************************
//
// *********************************************************
void TAG0062a2(wordRectPos *P1, ui8 *P2)
{//()
  dReg D0, D6, D7;
  wordRectPos *rpA0;
  ui8 *pnt_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;
  rpA0 = P1;
  D7W = sw(((rpA0->x2 + 15) << 4) / 8);
  D6W = sw(rpA0->y2 - rpA0->y1);
  pnt_4 = (ui8 *)AssignMemory(14, D6W * D7W);
  TAG0088b8(P1, pnt_4, 0);
  TAG0063a6(3);
  D0W = sw(rpA0->x2 - rpA0->x1);
  TAG0025a0(pnt_4, pnt_4, D0W, D6W, D0W, D6W, P2);
  TAG0088b8(P1, pnt_4, 1);
  ReleaseMem(14, pnt_4);
  return;
}

// *********************************************************
//
// *********************************************************
void TAG00636a()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TAG001964(1, 0x01060001, 0x010600ff, TAG007908);
}

// *********************************************************
//
// *********************************************************
void TAG006388()
{
//;;;;;;;;;;;;;;;;;;;;;;;
  TAG001964(1, 0x01060001, 0x010600ff, TAG007908);
}

// *********************************************************
//
// *********************************************************
void TAG0063a6(i16 P1)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  switch (P1)
  {
  case 1:
      // save A5 at jump instruction
      SetSupervisorMode();
      // Set some interrupt vectors.
      ClearSupervisorMode();
      break;
  case 2:
    SetSupervisorMode();
    //Restore VBL interrupt vector
    ClearSupervisorMode();
    return;
    /*


    // This becomes the VBL interrupt routine



    // replace VBL vector with (0x006436)





  if (data6488 != 0) return;
  data6488 = 1;
  A1 = f.Byte192;
  A0 = LoadPnt(A1+8); // -184(A5)
  A2 = LoadPnt(A1+12); // -180(A5)
  D1L = 0;
  do
  {
    D1W += wordGear(A0); // add the words in function MinA
    A0 += 2;
  } while (A0 < A2);
  D0W = wordGear(A1+24); // -168(A5)
  if (D0 != D1)
  {
    TAG006464(); // replace VBL vector???
  };
  wordGear(A1+24) = D1W; // new sum
  A0 = LoadPnt(A1+16); // -176(A5)
  A2 = LoadPnt(A1+20); // -172(A5);
  D0 = wordGear(A1+28); // -164(A5);
  D1L = 0;
  do
  {
    D1W += wordGear(A0);
    A0 += 2;
  } while (A0 < A2);
  if (D0W != D1W)
  {
    TAG006464();
  };
  wordGear(A1+28) = D1W;
  data006488 = 0;
*/
  case 3:
  default: break;
  }; //switch
}

// *********************************************************
//
// *********************************************************
i16 TAG0064fc(pnt P1, pnt P2)
{
  dReg D0, D1, D6, D7;
  aReg A3, A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = P1;
  A3 = P2;
  for (D6L=D7L=0; A4<A3; A4++)
  {
    D0L = (D6W++) & 7;
    D1W = (UI8)(*A4);
    D1W = sw(D1W * (D0W+1));
    D7W = (I16)(D7W + D1W);
  };
  return D7W;
}

// *********************************************************
//
// *********************************************************
//  TAG006538
pnt StrCpyTo(pnt P1, pnt P2, i16 P3)
{
  dReg D6, D7;
  aReg A3, A4;
//;;;;;;;;;;;;;;;;;;;;
  A4 = P1;
  A3 = P2;
  D7B = (i8)P3;
  while ((D6B = *(A4++)) != 0)
  {
    if (D7B == D6B) break;
    *(A3++) = D6B;
//
  };
  if (D6B != D7B) A4=NULL;
  *A3 = 0;
  return A4;
}

// *********************************************************
//
// *********************************************************
i32 TAG006570(pnt P1, i32 P2)
{
  dReg D7;
  i32 i_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7L = P2;
  if (D7L == 0)
  {
    return 1;
  };
  SetBufferLength(f.Word664, P2);
  SetBufferAddress(f.Word664, P1);
  i_4 = ReadFile(f.Word664);
  return i_4==0?1:0;
}

// *********************************************************
//
// *********************************************************
i32 TAG006636(pnt P1, i32 P2, i16 P3, i16 P4)
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D0W = sw(TAG006570(P1, P2));
  if (D0W != 0)
  {
    D0W = Unscramble((ui8 *)P1, P3, P2>>1);
    if (D0W == P4) return 1;
  };
  return 0;
}

// *********************************************************
//
// *********************************************************
//  TAG006672
i32 MyReadFirstBlock(pnt P1)
{//(i32)
  dReg D0, D5, D6, D7;
  aReg A4;
//;;;;;;;;;;;;;;
  A4 = P1;
  D0W = sw(TAG006570(A4, 512)); // First block of csbgame.dat
  if (D0W == 0)
  {
    TAG0093a0_17(20, 222);
    return (0);
  };
  for (D7L=32, D6L=0; D7W != 0; D7W--)
  {
    D6W = (UI16)(D6W + LE16(wordGear(A4)));
    D6W ^= LE16(wordGear(A4+2));
    D6W = (I16)(D6W - LE16(wordGear(A4+4)));
    D6W ^= LE16(wordGear(A4+6));
    A4 += 8;
//
//
  };
  Unscramble((ui8 *)A4, LE16(wordGear(P1+58)), 128);
  D5L = 0;
  D7L = 128;
  do
  {
    D5W = (I16)(D5W + LE16(wordGear(A4)));
    A4 += 2;
    D7W--;
  } while (D7W != 0);
  if (D5W != D6W)
  {
    TAG0093a0_17(20, 223);
  };
  return (D5W==D6W);
}

// *********************************************************
//
// *********************************************************
void TAG006718(i16 P1)
{//()
  i32 i;
  S4076 *p;
  p = &f.s4076_8816[P1];
  TAG006bfc(p->pnt4064); p->pnt4064 = NULL;
  TAG006bfc(p->pnt4068); p->pnt4068 = NULL;
  TAG006bfc(p->pnt4072); p->pnt4072 = NULL;
  TAG006bfc(p->pnt642);  p->pnt642  = NULL;
  TAG006bfc(p->pnt646);  p->pnt646  = NULL;
  TAG006bfc(p->pnt654);  p->pnt654  = NULL;
  TAG006bfc(p->pnt650);  p->pnt650  = NULL;
  TAG006bfc(p->pnt658);  p->pnt658  = NULL;
  TAG006bfc(p->pnt662);  p->pnt662  = NULL;
  TAG006bfc(p->pnt668);  p->pnt668  = NULL;

  for (i=0; i<16; i++)
  {
    TAG006bfc(p->pnt672[i]); p->pnt672[i] = NULL;
  };
  if (P1 == 0) f.Word8818 = 0;
  p->w0 = 0;
  return;
}

// *********************************************************
//
// *********************************************************
i32 TAG00686a(i16 P1)
{//(i32)
  dReg D0, D6;
  i32 i_18=0;
  i32 i_14;
  S4076 *s4076A4;
//;;;;;;;;
  s4076A4 = &f.s4076_8816[P1];
  f.Word664 = TAG00101c();
  if (f.Word664 == -1)
  {
    i_18 = 83;
  }
  else
  {
    TAG0076a0_40(20, f.Word664, gameName, -1);
    i_14 = TAG001096_2(f.Word664);
    if (i_14 != 0)
    {
      i_18 = 84;
    }
    else
    {
      for(;;)
      { // while (0) to avoid deeply nested if-else
        TAG006718(P1);
        {
          i32 size;
          i32 file = FindFileDescriptor(f.Word664)->fileHandle; 
          if (file < 0) break;
          size = ReadExtendedFeatures(file);
          //RC4_prepare_key((unsigned char *)hintKey,8);
        };
        D0W = sw(MyReadFirstBlock(s4076A4->FirstBlock2));
        if (D0W == 0) 
        {
          i_18 = 213;
          break;
        };
        TAG0083cc((pnt)MinA, (pnt)MinB);
              //********** nonsense deleted **********//
        D0W = sw(TAG006636(s4076A4->SecondBlock514, 
                          128, 
                          LE16(wordGear(s4076A4->FirstBlock2+312)), 
                          LE16(wordGear(s4076A4->FirstBlock2+344))));
        if (D0W == 0)
        {
          i_18 = 214;
          break;
        };
        D0L = 0;
        D0W = sw(16*LE16(wordGear(s4076A4->SecondBlock514+46)));
        s4076A4->pnt4064 = TAG006bc0(D0L);
        // Read Monster group data.
        D0W = sw(TAG006636(s4076A4->pnt4064,
                            16*LE16(wordGear(s4076A4->SecondBlock514+46)),
                            LE16(wordGear(s4076A4->FirstBlock2+314)),
                            LE16(wordGear(s4076A4->FirstBlock2+346)) ));
        if (D0W == 0)
        {
          i_18 = 215;
          break;
        };
        D0W = sw(TAG006636(s4076A4->Characters,
                            3328, // 4*800 + 128
                            LE16(wordGear(s4076A4->FirstBlock2+316)),
                            LE16(wordGear(s4076A4->FirstBlock2+348)) ));
        if (D0W == 0)
        {
          i_18 = 216;
          break;
        };
        if (*(s4076A4->FirstBlock2+301) == 1)
        {
          i_14 = TAG001096_11(f.Word664);
          TAG00181e(f.Word664);
          NotImpMsg(0x6a2e);
          /*
*/
          break;
        };
        //if (  (LE16(wordGear(s4076A4->FirstBlock2+378)) != 13)
        //    ||(LE16(wordGear(s4076A4->FirstBlock2+376)) != 1)  )
        //{
        //  i_18 = 224;
        //  break;
        //};
        f.Word662 = 0;
        if (P1 == 0)
        {
            f.Word8818 = LE16(wordGear(s4076A4->SecondBlock514+10));
        };
          s4076A4->w0 = 1;
        D6W = 100;
        i_14 = TAG001096_10(f.Word664); // Closes file
        TAG001064(f.Word664);
        return (D6W);
      };
      TAG001096_10(f.Word664);
    };
  };
  UI_MessageBox("Cannot read SaveFile\nPerhaps not Utilized",
                "SAVE FILE ERROR",
                MESSAGE_OK|MESSAGE_ICONERROR);
  UI_Die(0xa8a7);
  TAG001064(f.Word664);
  TAG0093a0_17(20,i_18);
  TAG006718(P1);
  if (i_18 == 84) return -2; else return -1;
}

// *********************************************************
//
// *********************************************************
pnt TAG006bc0(i32 P1)
{//(pnt)
  pnt  pnt_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pnt_4 = AssignMemory(17, P1);
  if (pnt_4 == NULL)
  {
    TAG0093a0_18(17, (i32)"get mem");
    for (;;) {};
  };
  return (pnt_4);
}

// *********************************************************
//
// *********************************************************
void TAG006bfc(pnt P1)
{//()
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (P1 == NULL) return;
  ReleaseMem(17, (ui8 *)P1);
  return;
}

// *********************************************************
//
// *********************************************************
i32 TAG006c1c(i16 FDnum, i32 P2, pnt P3)
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  SetBufferLength(FDnum, P2);
  SetBufferAddress(FDnum, P3);
  D0L = ReadFile(FDnum);
  return D0L==0?1:0;
}

// *********************************************************
//
// *********************************************************
void TAG006c5e()
{
  NotImpMsg(0x6c5e);
}

void PrintHint(FILE *file, char *src, i32 numHint)
{
  i32 n, len;
  char temp, *start, *end, *blank;
  start=src;
  for (n=0; n<numHint; n++)
  {
    while (*start != 0)
    {
      end=start;
      len=1;
      blank=NULL;
      while (*end!=0)
      { //locate last blank before column 60.
        if (len>60) break;
        len++;
        if (*end==' ') blank=end;
        end++;
      };
      if (*end!=0)
      {
        if (blank!=NULL) end=blank;
      };
      // end points to the first character of the next line.
      temp=*end;
      *end=0;
      if (*start == ' ') start++;
      fprintf(file, "    %s\n",start);
      *end=temp;
      start=end;
    };
    start++; // skip nul
  };
}

//*********************************************************
//
//*********************************************************
RESTARTABLE _TAG006c7e_xxx(i16 P1, i16 P2, i32 P3, i32, pnt P5)//(i32)
{ //(i32)                   @8    @10     @12      @16    @20
  static dReg D0;
  static HCTI *pHCTiP5;
  static i16  *wA0;
  static pnt  *pPnt_50;
  static HCTI *pHCTi_46;
  static HCTI *pHCTi_38;
  static HCTI *pHCTi_34 = NULL;
  static i16  w_30;
  static i16  w_28;
  static i32  i_26 = 0x1baddade;
  static i32  i_22;
  static i16  FDnum=0x2bad; //FileDescriptor number. w_18;
  static i16  w_16;
  static i16  w_14;
  static pnt  pnt_12;
  static pnt  pnt_8;
  static i32  i_4 = 0x1baddade;
  static char *temp;
  static unsigned char *pSeg3;
  static i32 offset;
  static i32 i, j, n, hintLen, numHint;
  static unsigned char *pEnt;
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
  END_RESTARTMAP
  //TAG009a02(17, P2);
  w_14 = TAG0064fc((pnt)TAG008a42, (pnt)TAG008c20);
  
        //********* Nonsense Deleted **************//

  switch (P2)
  {
  case 9:
    switch (P3)
    {
    case 501:
      TAG0078fa(pText16272[14]);
      TAG008c40_2(-1);
      DoMenu(_1_,
            //const pnt title1,        //8
            "SELECT SAVE GAME",
            //const pnt title2,        //12
            "",
            //const pnt opt1,          //16
            "CSBGAME",
            //const pnt opt2,          //20
            "CSBGAME2",
            //const pnt opt3,          //24
            "CSBGAME3",
            //const pnt opt4,          //28
            "CSBGAME4",
            //const i32 I1,            //32
            1,
            //const i32 ClearOldScreen,//34
            1,
            //const i32 FadeOldScreen) //36
            1);
      STShowCursor(HC53);
      WaitForMenuSelect(_5_,4,1,0,0);
      STHideCursor(HC53);
      switch (i16Result)
      {
      case 1: gameName = "1CSBGAME.DAT"; break;
      case 2: gameName = "1CSBGAME2.DAT";break;
      case 3: gameName = "1CSBGAME3.DAT";break;
      case 4: gameName = "1CSBGAME4.DAT";break;
      };
      do
      {
        TAG0076a0_12(_2_, P1, gameName, 0);
        w_14 = sw(TAG00686a(0));
        if (w_14 != 100)
        {
          TAG0051c2_35(17);

          
          //NotImpMsg(0x6d72);
          TAG008c40_3(17,6,0);
          TAG008c40_3(17,7,0);
          if (w_14 == -2)
          {
            w_16 = (i16)TAG0076a0_13(20);
            if (w_16 > 1)
            {
              D0L = (i32)pText16272[3];
            }
            else
            {
              D0L = (i32)pText16272[2];
            };
            pnt_8 = (pnt)D0L;
            TAG004e4c_6(_6_,17,pnt_8);
          }
          else
          {
            //NotImpMsg(0x6de2);
            TAG004e4c_6(_9_,17, pText16272[15]);
          };
          do
          {
            TAG008c40_5(_7_,17,0);
            w_16 = (i16)intResult;
            TAG008c40_8(_8_,17,w_16);
            w_16 = (i16)intResult;
            if (w_16 == 6)
            {
              w_14 = 0;
              continue;
            };
          } while (w_16 != 7);
          w_14 = 2;
          continue;
        }
        else
        {
          w_14 = 1;
        };
      } while (w_14 == 0);
      i_4 = w_14;
      break;
    case 2:
      pHCTiP5 = (HCTI *)P5;
      TAG0078fa(pText16272[10]);
      // Now we OPEN "hcsb.hct" using file descriptor P1
      TAG0076a0_12(_3_, P1, "0hcsb.hct", pText16272[6]);
      D0L = intResult;
      if (D0L == 0)
      {
        i_4 = 0;
      };
      FDnum = TAG00101c();
      TAG008a62();
      if (FDnum == -1)
      {
        i_26 = 87;
        w_16 = 1;
      }
      else
      {
        for (;;) //fake to avoid deeply nested if-else
        {
          TAG0076a0_40(P1, FDnum, "0hcsb.hct", -1);//set drive/file
          i_22 = TAG001096_2(FDnum);
          if (i_22 != 0) {i_26 = 85; w_16 = 1; break;};
          pHCTi_34 = pHCTiP5;
          pHCTi_38 = pHCTiP5;
          {
            i16 fileHandle;
            FILE_DESCRIPTOR *pFD;
            pFD = FindFileDescriptor(FDnum);
            fileHandle = pFD->fileHandle;
            if (hintKey[0] != 0)
            {
              SETENCIPHERED(fileHandle,
                            (unsigned char *)hintKey,
                            8);
            };
          };
          D0W = sw(TAG006c1c(FDnum, 2, (pnt)&w_28)); //Read 2 bytes to w_28
          if (D0W == 0) {i_26 = 200; w_16 = 1; break;};
          w_28 = LE16(w_28);
          if (w_28 != 2) {i_26 = 55; w_16 = 1; break;};
          D0W = sw(TAG006c1c(FDnum, 2, (pnt)&w_28));
          if (D0W == 0) {i_26 = 201; w_16 = 1; break;};
          w_28 = LE16(w_28);
          if (w_28 != 13) {i_26=81; w_16=1; break;};
          D0W = sw(TAG006c1c(FDnum, 2, (pnt)&w_28));
          if (D0W == 0) {i_26=202; w_16=1; break;};
          w_28 = LE16(w_28);
          D0L = 0;
          D0W = sw(2 * w_28);
          SetBufferLength(FDnum, D0L);
          i_22 = FileSeek(FDnum);
          if (i_22 != 0) {i_26=82; w_16=1; break;};
          D0W = sw(TAG006c1c(FDnum, 2, (pnt)&w_28));
          if (D0W == 0) {i_26=203; w_16=1; break;};
          w_28 = LE16(w_28);
          D0W = sw(TAG006c1c(FDnum, 2, (pnt)&w_30));
          if (D0W == 0) {i_26=204; w_16=1; break;};
          w_30 = LE16(w_30);
          wA0 = pHCTi_38->pSeg1Len; // Points to first of three words
          *wA0 = w_28;
          D0L = w_28 * w_30;
          pPnt_50 = pHCTi_34->pSeg1Addr; //First of three buffers
          *pPnt_50 = AssignMemory(17, D0L);
          D0L = w_28 * w_30;
          D0W = sw(TAG006c1c(FDnum, D0L, *pHCTi_34->pSeg1Addr));
          if (D0W == 0) {i_26=205; w_16=2; break;};
          if (dumpFile >= 0)
          {
            if (hintKey[0] != 0)
            {
              fprintf(GETFILE(dumpFile),
                  "Sorry.  The hint file is encrypted");
            }
            else
            {
              i32 k;
              i32 prevVal=0;
              i32 newVal;
              i32 val;
              unsigned char *pEntry;
              for (;;)
              {
                newVal=0x7fffffff;
                pEntry = (unsigned char *)*pHCTi_34->pSeg1Addr;
                for (k=0; k < w_28; k++, pEntry +=6)
                {
                  val= (pEntry[2]*256+pEntry[0])*256+pEntry[1];
                  if ( (val>prevVal) && (val<newVal) ) newVal=val;
                };
                if (newVal == 0x7fffffff) break;
                prevVal = newVal;
                pEntry = (unsigned char *)*pHCTi_34->pSeg1Addr;
                for (k=0; k < w_28; k++, pEntry +=6)
                {
                  val= (pEntry[2]*256+pEntry[0])*256+pEntry[1];
                  if (val != newVal) continue;
                  fprintf(GETFILE(dumpFile), "(%3d,%3d,%3d)  %5d\n",
                          pEntry[2],pEntry[0],pEntry[1],
                          LE16(wordGear(pEntry +4)));
                };
              };
            };
          };
          D0W = sw(TAG006c1c(FDnum, 2, (pnt)&w_28));
          if (D0W == 0) {i_26=206; w_16=2; break;};
          w_28 = LE16(w_28);
          D0W = sw(TAG006c1c(FDnum, 2, (pnt)&w_30));
          if (D0W == 0) {i_26=207; w_16=2; break;};
          w_30 = LE16(w_30);
          wA0 = pHCTi_38->pSeg2Len; // Points to second of three words
          *wA0 = w_28;
          D0L = w_28 * w_30;
          pPnt_50 = pHCTi_34->pSeg2Addr;  //Second of three buffers
          *pPnt_50 = AssignMemory(17, D0L);
          D0L = w_28 * w_30;  
          D0W = sw(TAG006c1c(FDnum, D0L, *pHCTi_34->pSeg2Addr)); 
          if (D0W == 0) {i_26=208; w_16=3; break;};
          D0W = sw(TAG006c1c(FDnum, 2, (pnt)&w_30));
          if (D0W == 0) {i_26=209; w_16=3; break;};
          w_30 = LE16(w_30);
          w_30 *= 2;
          D0L = 0;
          D0W = w_30;
          pPnt_50 = pHCTi_34->pSeg3Addr; //Third of three buffers.
          *pPnt_50 = AssignMemory(17, D0L);
          D0L = 0;
          D0W = w_30;  
          D0W = sw(TAG006c1c(FDnum, D0L, *pHCTi_34->pSeg3Addr)); 
          if (D0W == 0) {i_26=210; w_16=4; break;};
          SetBufferLength(FDnum, 0);
          pHCTi_46 = pHCTiP5;
          i_22 = TAG001096_11(FDnum);
          if (i_22 != 0) {i_26=95; w_16=4; break;};
          *pHCTi_46->pSeg3Len = TAG00181e(FDnum);
          if (dumpFile >= 0)
          {
            if (hintKey[0] == 0)
            {
              temp = (char *)UI_malloc(15009+5003, MALLOC050);
              pEnt = (unsigned char *)*pHCTi_34->pSeg2Addr;
              for (i=0; i<w_28; i++, pEnt+=26)
              {
                fprintf(GETFILE(dumpFile),"\n%3d %-22s %4d %4d\n",
                        i,
                        pEnt,
                        LE16(wordGear(pEnt+22)),
                        LE16(wordGear(pEnt+24)) );
                pSeg3 = (unsigned char *)*pHCTi_34->pSeg3Addr;
                n = LE16(wordGear(pEnt+22));
                offset = *pHCTi_46->pSeg3Len;;
                for (j=0; j < n; j++, pSeg3+=2) 
                {
                  offset+=LE16(wordGear(pSeg3));
                };
                SetBufferLength(FDnum, offset);
                FileSeek(FDnum);
                hintLen = LE16(wordGear(pSeg3));
                numHint = LE16(wordGear(pEnt+24));
                TAG003210(
                          FDnum,
                          hintLen,    //length of hint in file
                          f.Byte12792,//buffer address
                          (pnt)temp,       //temp buffer
                          (pnt)temp+15009);//temp buffer
                PrintHint(GETFILE(dumpFile),(char *)f.Byte12792,numHint);
              };
              UI_free (temp);
            };
            CLOSE(dumpFile);
            dumpFile = -1;
          };
          w_16 = 0;
          break;
        } ;
      };
      TAG001096_10(FDnum);
      TAG001064(FDnum);
      if (w_16 == 0)
      {
        i_4 = 1;
        break;
      };
      TAG0093a0_17(P1, i_26);
      if (w_16 >= 4)
      {
        ReleaseMem(17, (ui8 *)*pHCTi_34->pSeg3Addr);
      };
      if (w_16 >= 3)
      {
        ReleaseMem(17, (ui8 *)*pHCTi_34->pSeg2Addr);
      };
      if (w_16 >= 2)
      {
        ReleaseMem(17, (ui8 *)*pHCTi_34->pSeg1Addr);
      };
      i_4 = 0;
      break;
    default:
      NotImpMsg(0x7326);
      /*
*/
      break;
    }; //switch (P3)
    break;
  case 32:
    i_4 = 1;
    if (wordGear((pnt)P3) != 12)
    {
      i_26 = 58;
      w_16 = 1;
    }
    else
    {
      TAG0078fa(pText16272[11]);
      TAG0076a0_12(_4_,
                   P1, 
                   pointer8858[wordGear((pnt)P3+2)],
                   pText16272[6]);
      D0L = intResult;
      if (D0L == 0)
      {
        i_26 = 59;
        w_16 = 1;
      }
      else
      {
        TAG008a62();
        FDnum = TAG00101c();
        if (FDnum == -1)
        {
          i_26 = 86;
          w_16 = 1;
        }
        else
        {
          TAG0076a0_40(
                       P1,
                       f.Word664,
                       pointer8858[wordGear((pnt)P3+2)],
                       -1);
          i_22 = TAG001096_2(FDnum); //OPEN file
          if (i_22 != 0)
          {
            i_26 = 88;
            w_16 = 1;
          }
          else
          {
            {
              i16 fileHandle;
              FILE_DESCRIPTOR *pFD;
              pFD = FindFileDescriptor(FDnum);
              fileHandle = pFD->fileHandle;
              if (hintKey[0] != 0)
              {
                SETENCIPHERED(fileHandle,
                              (unsigned char *)hintKey,
                              8);
              };
            };
            SetBufferLength(FDnum, LoadLong((pnt)P3+4));//file offset
            i_22 = FileSeek(FDnum);
            if (i_22 != 0)
            {
              i_26 = 89;
              w_16 = 1;
            }
            else
            {
              pnt_8 = AssignMemory(P1, 15009);
              pnt_12 = AssignMemory(P1, 5003);
              TAG003210(
                        FDnum,
                        LoadLong((pnt)P3+8),//length of hint in file
                        (pnt)LoadPnt((pnt)P3+12),//buffer address
                        pnt_8,  //temp buffer
                        pnt_12);//temp buffer
              ReleaseMem(P1, (ui8 *)pnt_8);
              ReleaseMem(P1, (ui8 *)pnt_12);
              i_26 = 0;
            };
          };
        };
      };
    };
    i_4 = i_26;
    if (i_26 != 0)
    {
      TAG0093a0_17(P1, i_26);
    };
    i_22 = TAG001096_10(FDnum);
    TAG001064(FDnum);
      break;
  case 2:
      TAG006718(0);
      break;
  default:
      NotImpMsg(0x7518);
  /*
*/
      break;
  }; //switch (P2)
  //TAG009a1c(17, P2);
  RETURN_int(i_4);
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG006c7e_2(i16 P1)
{//()
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
  TAG006c7e_xxx(_1_, P1, 2, 0x1baddade, 0x1baddade, (pnt)0x1baddade);
  RETURN;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG006c7e_9(i16 P1, i32 P2, i32 P3, HCTI *P4)
{//(i32)
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
  TAG006c7e_xxx(_1_, P1, 9, P2, P3, (pnt)P4);
  RETURN_int(intResult);
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG006c7e_32(i16 P1, pnt P2)
{//(i32)
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
  TAG006c7e_xxx(_1_, P1, 32, (i32)P2, 0x1baddade, (pnt)0x1baddade);
  RETURN_int(intResult);
}

// *********************************************************
//
// *********************************************************
void TAG007540()
{
  NotImpMsg(0x7540);
}

// *********************************************************
//
// *********************************************************
void TAG0075c8()
{ //  set hard-disk handling vectors?????
}

// *********************************************************
//
// *********************************************************
void TAG00760a()
{
}

// *********************************************************
//
// *********************************************************

// *********************************************************
//
// *********************************************************
void TAG007560(i16 P1)
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D0W = sw(P1 + 'A');
  f.Byte658 = D0B;
  SetSupervisorMode();
  TAG0075c8();//Reroute hard-disk handling
  ClearSupervisorMode();
  OPEN((char *)&f.Byte658, "rb"); //set active drive/folder???
  SetSupervisorMode();  
  TAG00760a(); //restore hard-drive routines??
  ClearSupervisorMode();
//
}

// *********************************************************
//
// *********************************************************
i32 TAG0076a0_8(i16 P1, pnt nP2, i32 nP3)
{//(i32)
  i16 w_10;
  dReg D7;
  i32 i_14;
  i32 i_4;
  //case 0x08:
      w_10 = TAG00101c(); //Find empty file descriptor.
      if (w_10 != 0)
      {
        TAG0093a0_18(P1, 90);
      };
      TAG0076a0_40(P1, w_10, (char *)nP2, nP3); //Set Filename
      D7W = sw(intResult);
      TAG007560(D7W);//set current drive/folder???
      i_14 = TAG001096_2(w_10);
      TAG001096_10(w_10);
      TAG001064(w_10);
      if (i_14 == 0)
      {
        i_4 = 1;
      }
      else
      {
        i_4 = 0;
      };
      //break;
  return (i_4);
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG0076a0_12(i16 /*P1*/, pnt nP2, TEXT *nP3)
{//(i32)
  static dReg D0, D7;
  static i32 i_4;
  static i32 i_8;
  RESTARTMAP
    RESTART(6)
  END_RESTARTMAP
  //case 0x0c:
  i_4 = 1;
  TAG008c40_2(-1); //Clear two work areas???
  D7W = sw(TAG0076a0_13(20));
  if (D7W <= 1)
  {
    D0L = 32;
  }
  else
  {
    D0L = -1;
  };
  i_8 = D0L;      
  //while (TAG0076a0_8(20, nP2, i_8) != 1)
  for (;;)
  {
    if (TAG0076a0_8(20, nP2, i_8) == 1) break;
    if (nP3 == 0)
    {
      i_4 = 0;
      break;
    };
    TAG004e4c_8(_6_, 20, (TEXT *)nP3);
//
  };
  //break;
  RETURN_int(i_4);
}

// *********************************************************
//
// *********************************************************
i32 TAG0076a0_13(i16 P1)
{//(i32)
  dReg D7;
  i32 i_14;
  i16 w_10;
  i32 i_4;
  //case 0x0d:
  w_10 = TAG00101c(); //find empty file descriptor??
  if (w_10 == -1)
  {
    TAG0093a0_18(P1, 91);
  };
  TAG0015d6_128(w_10, 32);
  i_14 = TAG001096_22(w_10);
  if (i_14 != 0)
  {
    TAG0093a0_18(P1, 92);
  };
  D7W = sw(TAG00181e(w_10));
  TAG001064(w_10);
  if (D7W > 2) D7W = 2;
  i_4 = D7W;
  //break;
  return (i_4);
}

// *********************************************************
//
// *********************************************************
void TAG0076a0_40(i32 /*P1*/, i32 nP2, const char* nP3, i32 nP4)
{//()
  i32 i_18;
  i32 i_4;
  //case 0x28:
   if (nP4 == -1)
   {



    if (((pnt)nP3)[0] == 48) i_18 = 32;
    else
    {
      if (TAG0076a0_13(20) < 2) i_18 = 32;
      else i_18 = 33;
    };
  }
  else
  {
    i_18 = nP4;
  };
  ASSERT(i_18 != 0x1baddade,"i_18");
  SetDriveName((i32)nP2, i_18);//The drive portion of name
  SetFileName((i32)nP2, (pnt)nP3+1);//The filename itself
  i_4 = i_18 - 32;
  //break;
}

// *********************************************************
//
// *********************************************************
void TAG0078fa(TEXT *P1)
{
//;;;;;;;;;;;;;;;;;
  f.Pointer652 = P1;
}

// *********************************************************
//
// *********************************************************
i32 TAG007908(i32, pnt)
{
  NotImpMsg(0x7908); return 0;
}

// *********************************************************
//
// *********************************************************
i16 TAG00794e(pnt P1, pnt P2)
{
  dReg D7;
  aReg A3, A4;
//;;;;;;;;;;;;;;;;;;;
  A4 = P1;
  A3 = P2;
  D7L=0; 
  while (A4 < A3)
  {
    D7B ^= *(A4++);

  };
  return D7W;
}

// *********************************************************
//
// *********************************************************
void TAG00797a()
{
  NotImpMsg(0x797a);
}

// *********************************************************
//
// *********************************************************
void TAG00799a_1(i16 /*P1*/)
{//()
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(3, P2);
  f.Seg1Addr10550 = NULL;
  f.Seg2Addr10546 = NULL;
  f.Seg3Addr10542 = NULL;
  f.Word10486 = 0;
  //TAG009a1c(3,P2);
  return;
}

// *********************************************************
//
// *********************************************************
void TAG00799a_2(i16 /*P1*/)
{//()
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(3, P2);
  if (f.Word10486 == 0) return;
  if (f.Seg1Addr10550 != NULL)
  {
    ReleaseMem(17, (ui8 *)f.Seg1Addr10550);
  };
  if (f.Seg2Addr10546 != NULL)
  {
    ReleaseMem(17, (ui8 *)f.Seg2Addr10546);
  };
  if (f.Seg3Addr10542 != NULL)
  {
    ReleaseMem(17, (ui8 *)f.Seg3Addr10542);
  };
  f.Word10486 = 0;
  //TAG009a1c(3,P2);
  return;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG00799a_6(i16 P1)
{//()
  static dReg D0;
  static wordRectPos *pRectPos_42;
  static PALETTEPKT pkt_38;
  static i16 w_16;
  static i16 w_14;
  static TEXT *pText_8;
  static PAGE *pPage_8;
  static i32 i_4 = 0x1baddade;
  RESTARTMAP
    RESTART(9)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(3, P2);
  switch (P1)
  {
  case 2:
    w_16 = sw(TAG00799a_13(P1));   
    for (w_14=0; w_14<((f.NumHint10552<MAXHINTINDICES)?f.NumHint10552:MAXHINTINDICES); w_14++)
    {
      TAG008c40_3(P1, w_14+10, 0);  
      D0W = f.HintIndices10530[w_14];
      D0L = StrLen((char *)f.Seg2Addr10546+26*D0W) + 1;
      pText_8 = (TEXT *)TAG004a22_16(P1, D0L);
      pText_8->w0 = 3;
      D0W = f.HintIndices10530[w_14];
      //D0W = index of hint title (0xb4 = "DEMON DIRECTOR")
      StrCpyTo(f.Seg2Addr10546 + 26*D0W,
               (pnt)pText_8->pnt2,
               0);
      pRectPos_42 = (wordRectPos *)TAG008c40_12(P1, w_14+10, 3, NULL);
      pText_8->rectPos6.x1 = pRectPos_42->x1; 
      pText_8->rectPos6.x2 = pRectPos_42->x2;
      pText_8->rectPos6.y1 = pRectPos_42->y1;
      pText_8->rectPos6.y2 = pRectPos_42->y2;
      pkt_38.w0 = 13;
      pkt_38.pPalette6 = &f.Palette16608;
      D0W = LE16(f.Segment1_16872[8]);
      f.Palette16608.color[D0W] = LE16(f.Segment1_16872[36]);//Word16800;
      TAG005ae8_38(P1, &pkt_38);
      TAG00978a_27(19, 6, (pnt)pRectPos_42, 0x1baddade);
      TAG00978a_27(9, 20, (pnt)pText_8, 8);
      TAG008c40_12(
                   P1, 
                   w_14+10, 
                   5, 
                   f.Seg2Addr10546 + 26*f.HintIndices10530[w_14]);
//
//
    };
    TAG0051c2_31(_9_, P1);//New screen appears
    TAG002af4();
    TAG0051c2_36(3);
    for (;;)
    {
      pPage_8 = (PAGE *)TAG00978a_28(19, 6, -1);
      if (pPage_8 == NULL) break;
      if (pPage_8->w0 != 8) break;
      TAG0062a2((wordRectPos *)pPage_8->pText8, byte16528); 
      TAG00978a_29(19, pPage_8->w2);
    } ;
    TAG0051c2_37(3);
    TAG002b0c();
    break;
  default:
    NotImpMsg(0x7df6);
    /*
*/
    break;
  }; //switch (P1)
  //TAG009a1c(3,P2);
  RETURN_int(i_4);
//  RETURN;
}

// *********************************************************
//
// *********************************************************
//   TAG00799a_7
void SetSelectedLine(i16 /*P1*/, i32 nP2)
{//()
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(3, P2);
  f.Word10516 = sw(nP2);
  //TAG009a1c(3,P2);
  return;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG00799a_9(i16 P1)
{//()
  static dReg D5, D6, D7;
  static aReg A4;
  static i16 w_14;
  static i32 i_8;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(3, P2);
  TAG00799a_2(P1);
  w_14 = sw(TAG00835c_25(P1));
  f.HCTi10514.pSeg1Addr = &f.Seg1Addr10550;
  f.HCTi10514.pSeg2Addr = &f.Seg2Addr10546;
  f.HCTi10514.pSeg3Addr = &f.Seg3Addr10542;
  f.HCTi10514.pSeg1Len = &f.Seg1Len10538;
  f.HCTi10514.pSeg2Len = &f.Seg2Len10536;
  f.HCTi10514.pSeg3Len = &f.Seg3Len10534;
  for (;;)
  {
    //Next we read hcsb.hct
    TAG006c7e_9(_1_, P1, 2, w_14, &f.HCTi10514);
    i_8 = intResult;
    if (i_8 == 1) break;
    die(0,"Cannot read hcsb.hct");
    //TAG004e4c_8(_6_, 3, pText16272[8]);
  } ;
  TAG0060c4();
  D7W = LE16(wordGear(f.s4076_8816[0].SecondBlock514+12));
  //D7 = partyX
  D6W = LE16(wordGear(f.s4076_8816[0].SecondBlock514+14));
  //D6 = PartyY
  D5W = LE16(wordGear(f.s4076_8816[0].SecondBlock514+18));
  //D5 = party Level
  A4 = f.Seg1Addr10550;
  f.NumHint10552 = 0;
  for (w_14 = 0; w_14 < f.Seg1Len10538; w_14++, A4+=6)
  {
    if (A4[2] != D5B) continue;
    if (   (A4[0] != D7B)
        || (A4[1] != D6W)  )
    {
      if (A4[0] != -1) continue;
      if (A4[1] != -1) continue;
    };
    f.HintIndices10530[f.NumHint10552++] = LE16(wordGear(A4+4));
//      if (f.Word10552 >= 7) break;
//
  };
  f.Word10486 = 1;
  //TAG009a1c(3,P2);
  RETURN;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG00799a_12(i16 P1, i32 selectedLine, i32 pageNum)
{//(pnt)                                 P3              P4
  static dReg D0;
  static pnt  pnt_46;
  static pnt pseg2_20;
  static i16 w_14;
  static pnt   pnt_8;
  static i32 i_4 = 0x1baddade;
  RESTARTMAP
    RESTART(2)
    RESTART(10)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(3, P2);
           //P3=line # selected on screen (0, 1, 2...)
           //P4=page number within hint
  i_4 = 0;
  if (selectedLine > f.NumHint10552)
  {
    TAG0093a0_18(P1, 7);
    RETURN_pnt((pnt)i_4);
  };
  D0W = f.HintIndices10530[selectedLine];
  pseg2_20 = f.Seg2Addr10546 + 26*D0W;
  if (pageNum > LE16(wordGear(pseg2_20+24)) )
  { //Asking for illegal page number
    TAG0093a0_18(P1, 8);
    RETURN_pnt((pnt)i_4);
  };
  if (pageNum != 0)
  {
    i_4 = (i32)&f.Byte12792;
    for (w_14 = 1; w_14 < pageNum; w_14++)
    {
      for (pnt_46 = (pnt)i_4; *(pnt_46++) != 0;)
      {
      };
      i_4 = (i32)pnt_46;
//
//
    };
    RETURN_pnt((pnt)i_4);
  };
  pageNum = 1;
  pnt_8 = AssignMemory(P1, 16);
  wordGear(pnt_8) = 12;
  wordGear(pnt_8+2) = 2;
  // Now we set the file offset of hint data start.
  StoreLong(pnt_8+4, f.Seg3Len10534);
  for (w_14=0; 
       w_14 < LE16(wordGear(pseg2_20+22))+pageNum-1;
       w_14++)
  {
    // Now add to the file start offset the length of
    // each piece of data in front of the hint we want.
    D0L = LE16(wordGear(f.Seg3Addr10542 + 2*w_14));
    StoreLong(pnt_8+4, LoadLong(pnt_8+4) + D0L);
//
//
  }; 
  StoreLong(pnt_8+8, LE16(wordGear(f.Seg3Addr10542 + 2*w_14)));
  StorePnt(pnt_8+12, f.Byte12792);
  for (;;)
  {
    TAG006c7e_32(_2_, P1, pnt_8);
    D0L = intResult;
    if (D0L == 0) break;
    TAG004e4c_8(_10_, 3, pText16272[9]);
  } ;
  ReleaseMem(P1, (ui8 *)pnt_8);
  i_4 = (i32)pseg2_20;
  //TAG009a1c(3,P2);
  RETURN_pnt((pnt)i_4);
}

// *********************************************************
//
// *********************************************************
i32 TAG00799a_13(i16 P1)
{//(i32)
  dReg D0;
  i32 i_4 = 0x1baddade;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(3, P2);
  switch (P1)
  {
  case 1:
  case 2:
    i_4 = f.NumHint10552;
    break;
  case 4:
    D0W = f.HintIndices10530[f.Word10516];
    i_4 = LE16(wordGear(f.Seg2Addr10546 + 26*D0W + 24));
    break;
  case 3:
  default:
    NotImpMsg(0x7a42);
    /*
*/
    break;
  }; // switch (P1)

  //TAG009a1c(3,P2);
  return (i_4);

}

// *********************************************************
//
// *********************************************************
void TAG007fbc()
{
  NotImpMsg(0x7fbc);
}


// *********************************************************
//
// *********************************************************
void TAG007fdc_2(i16 P1)
{//()
  i16  w_10;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(19, P2);
  w_10 = TAG0064fc((pnt)TAG00797a, (pnt)TAG007fbc);
  w_10 = 23; //Seems like a better number

        //************** Nonsense Deleted ***********//

  if (f.pText10480 != NULL)
  {
    ReleaseMem(P1, (ui8 *)f.pText10480);
  };
  if (f.pText10476 != NULL)
  {
    ReleaseMem(P1, (ui8 *)f.pText10476);
  };
  f.pText10480 = NULL;
  f.pText10476 = NULL;
  long10472 = 9999999;
  long10468 = 9999999;
  return;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG007fdc_7(i16 P1,i32 nP2,i32 nP3,i32 /*nP4*/)
{//()
  static dReg D0;
  static i16  w_10;
  static pnt  pnt_8;
  RESTARTMAP
    RESTART(2)
    RESTART(3)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(19, P2);
  w_10 = TAG0064fc((pnt)TAG00797a, (pnt)TAG007fbc);
  w_10 = 23; //Seems like a better number

        //************** Nonsense Deleted ***********//

  if (nP2 != long10472)
  {
    if (f.pText10480 != NULL)
    {
      Free_TEXT(P1, f.pText10480);
    };
    if (f.pText10476 != 0)
    {
      Free_TEXT(P1, f.pText10476);
    };
    f.pText10480 = NULL;
    f.pText10476 = NULL;
  }
  else
  {
    if (P1 != 0)
    {
      if (f.pText10476 != NULL)
      {
        Free_TEXT(P1, f.pText10476);
      };
      f.pText10476 = NULL;
    };
  };
  long10472 = nP2;
  long10468 = nP3;
  if (f.pText10480 == NULL)
  {
    TAG00799a_12(_2_, P1, nP2, 0);//Get pressed button text
    pnt_8 = pntResult;
    if (pnt_8 == NULL) RETURN;
    D0L = StrLen((char *)pnt_8);
    f.pText10480 = (TEXT *)TAG004a22_16(P1, D0L + 1);
    StrCpyTo(pnt_8, (pnt)f.pText10480->pnt2, 0);
    f.pText10480->rectPos6.x1 = sw(rectPos14516.x1+10);
    f.pText10480->rectPos6.x2 = sw(rectPos14516.x2-10);
    f.pText10480->rectPos6.y1 = sw(rectPos14516.y1+5);
    f.pText10480->rectPos6.y2 = sw(rectPos14516.y1+30);
  };
  if (w_10 != 23)
  {
    TAG0093a0_18(P1, 508);
  };
  if (f.pText10476 == NULL)
  {
    TAG00799a_12(_3_, P1, nP2, nP3);
    pnt_8 = pntResult;
    if (pnt_8 == NULL) RETURN;
    D0L = StrLen((char *)pnt_8);
    f.pText10476 = (TEXT *)TAG004a22_16(P1, D0L+1);
    StrCpyTo(pnt_8, (pnt)f.pText10476->pnt2, 0);
    f.pText10476->rectPos6.x1 = sw(rectPos14516.x1+34);
    f.pText10476->rectPos6.x2 = sw(rectPos14516.x2-34);
    f.pText10476->rectPos6.y1 = 31;
    f.pText10476->rectPos6.y2 = 164;
  };
  TAG00978a_27(9, 34,(pnt)f.pText10476, 6);
  RETURN;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG007fdc_36(i16 P1, i32 nP2, i32 nP3)
{//()
  static dReg D0;
  static i16  w_10;
  RESTARTMAP
    RESTART(4)
    RESTART(8)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(19, P2);
  w_10 = TAG0064fc((pnt)TAG00797a, (pnt)TAG007fbc);
  w_10 = 23; //Seems like a better number

        //************** Nonsense Deleted ***********//

  if (nP3 == 0)
  {
    f.CurPage10484 = 1;
    f.Word10482 = sw(nP2);
  }
  else
  {
    f.CurPage10484 = sw(f.CurPage10484 + nP3);
  };
  TAG007fdc_7(_4_, 4, f.Word10482, f.CurPage10484, 0);
  TAG008c40_3(4, 5, 0);
  D0W = sw(TAG00799a_13(4)); //Number of pages
  if (D0W > f.CurPage10484)
  {
    TAG008c40_3(4, 4, 0);
  };
  if (f.CurPage10484 > 1)
  {
    TAG008c40_3(4, 3, 0);
  };
  TAG0051c2_31(_8_, P1);
  RETURN;
}

// *********************************************************
//
// *********************************************************
i32 TAG00835c_xxx(i16 P1, i16 P2)
{//(i32)
  i32 i_4;
  i_4 = 0x1baddade;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(8, P2);
  switch (P2)
  {
  case 25:
    i_4 = 0;
    break;
  default:
    TAG0093a0_18(P1, 48);
    break;
  }; //switch (P2)
  //TAG009a1c(8, P2);
  return (i_4);
}

// *********************************************************
//
// *********************************************************
i32 TAG00835c_25(i16 P1)
{//(i32)
  return (TAG00835c_xxx(P1, 25));
}

// *********************************************************
//
// *********************************************************
void TAG0083ac(i16,i16,i16,i16,i16)
{
  NotImpMsg(0x83ac);
}

// *********************************************************
//
// *********************************************************
i16 TAG0083cc(pnt P1, pnt P2)
{
  dReg D7;
  aReg A3, A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = P2;
  A3 = P1;
  D7L = 0;
  while (A3 < A4)
  {
    D7W = (I16)(D7W + wordGear(A3));
    A3 += 2;
  };
  return D7W;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _StartHint(const CSB_UI_MESSAGE *)
{//()
  static dReg D0;
  static TEXT  *pText_10;
  static i16 w_4;
  static i16 w_2;
  i16 temp;
  //static int iii;
  RESTARTMAP
    RESTART(1)
    RESTART(6)
    RESTART(7)
    RESTART(9)
    RESTART(11)
    RESTART(12)
    RESTART(14)
    RESTART(23)
    RESTART(24)
    RESTART(25)
    RESTART(26)
    RESTART(27)
    RESTART(28)
  END_RESTARTMAP
  //iii = sizeof S4076;
  ASSERT(sizeof S4076 == ((4076 + 3)&0xfffffffc),"s4076");
  memset(&f, 0, sizeof (f));
//
//file = hint.ftl      offset =92be
//

//file = hint.ftl      offset =92e8

//  w_4 = TAG00794e
  f.Word10464 = 1;
  f.Word16910 = 1;
  f.Word16912 = 1;
  f.Word16914 = 1;
  f.Word194 = 0;
  TAG009a40();
  TAG0051c2_1(_1_, 10); // **01cdc2 //Window appears
                        // hcsb.dat is opened and read
  TAG00799a_1(10);
  TAG008a62();
  TAG00978a_1(10);
  TAG008c40_1(10);
  w_4 = 1;
  TAG008c40_3(w_4, 1, 0);
  TAG008c40_3(w_4, 2, 0);
  TAG008c40_3(w_4, 8, 0);
  D0W = sw(TAG0076a0_13(w_4));
  if (D0W > 1)
  {
    pText_10 = pText16272[1];
  }
  else
  {
    pText_10 = pText16272[0];
  };
  TAG004e4c_6(_25_, w_4, pText_10);
  do
  {
    TAG008c40_5(_23_, w_4, w_4);//construct and display screen
              //Wait for user to select LOAD, DUMP, or EXIT
    temp = w_2 = sw(intResult); //1 = LOAD
    TAG008c40_8(_24_, w_4, w_2);//flash button??
    temp = w_2 = sw(intResult); // temp make it debugger visible!
    switch (w_2)
    {
    case 1:
    case 8: //DUMP
      switch (w_4)
      {
      case 1:
        if (w_2 == 8)
        {
          dumpFile = CREATE("HINTDUMP.TXT","w", true);
        };
        TAG007fdc_2(w_4);
        TAG006c7e_9(_6_, w_4, 501, NULL, NULL);
        D0L = intResult;
        if (D0L != 1)
        {
          NotImpMsg(0x85e0);
          /*
*/
          break;
        };
        TAG00799a_9(_7_, w_4);
        D0W = sw(TAG00799a_13(w_4));
        if (D0W > 0)
        {
          w_4 = 2;
          TAG008c40_3(w_4, 5, 0);
          TAG00799a_6(_9_, w_4);
        }
        else
        {
          w_4 = 2;
          TAG008c40_3(w_4, 5, 0);
          TAG004e4c_6(_26_, w_4, pText16272[4]);
        };
        break;
      default:
        NotImpMsg(0x86ca);
        /*
*/
        break;
      }; //switch (w_4)
      break;
    case 2:
      TAG005d2a(_28_, w_4, 31);
      break;
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
      //w_6 = TAG0064fc(TAG00684a, TAG006ba0);

          //****** Nonsense Deleted *******//      
      w_4 = 4;
      SetSelectedLine(w_4, w_2-10);
      TAG007fdc_36(_11_, w_4, w_2-10, 0);
      break;
    case 5:
      switch (w_4)
      {
      case 4:
        w_4 = 2;
        TAG008c40_3(w_4, 5, 0);
        TAG00799a_6(_12_, w_4);
        break;
      case 2:
        w_4 = 1;
        TAG008c40_3(w_4, 1, 0);
        TAG008c40_3(w_4, 2, 0);
        D0W = sw(TAG0076a0_13(w_4));
        if (D0W > 1)
        {
          pText_10 = pText16272[1];
        }
        else
        {
          pText_10 = pText16272[0];
        };
        TAG004e4c_6(_27_, w_4, pText_10);
        break;
      case 3:
      default:
        TAG0093a0_18(w_4, 75);
      };//switch (w_4)

      break;
    case 3:
    case 4:
      if (w_2 != 3)
      {
        D0L = 1;
      }
      else
      {
        D0L = -1;
      };
      TAG007fdc_36(_14_, w_4, 0, D0L);//Increment page number
      break;
    case 6:
    case 7:
    case 9:
    default:
      NotImpMsg(0x8874);
      /*
*/
      break;
    case 0:
      break;
    }; //switch (w_2)
  } while (w_2 != 2);
  TAG009a40();  
  RETURN;
}

// *********************************************************
//
// *********************************************************
void TAG008898(i16,i16)
{
  NotImpMsg(0x8898);
}

// *********************************************************
//
// *********************************************************
void TAG0088b8(wordRectPos *P1, ui8 *P2, i16 P3)
{
  dReg D6,D7;
  wordRectPos  *rpA4;
  wordRectPos  rectPos_8;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;
  rpA4 = P1;
  D7W = sw((((rpA4->x2 - rpA4->x1)+16)/16)*8);
  D6W = sw(rpA4->y2 - rpA4->y1+1);
  if (P3 != 0)
  {
    rectPos_8.x1 = rpA4->x1;
    rectPos_8.x2 = rpA4->x2;
    rectPos_8.y1 = rpA4->y1;
    rectPos_8.y2 = rpA4->y2;
    TAG001ec0((pnt)P2,
              f.Pointer12798,
              &rectPos_8,
              0,
              0,
              D7W,
              160,
              -1);
  }
  else
  {
    rectPos_8.x2 = sw(rpA4->x2-rpA4->x1);
    rectPos_8.y2 = sw(rpA4->y2-rpA4->y1);
    rectPos_8.y1 = 0;
    rectPos_8.x1 = 0;
    TAG001ec0(f.Pointer12798,
              (pnt)P2,
              &rectPos_8,
              rpA4->x1,
              rpA4->y1,
              160,
              D7W,
              -1);
  };
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG0089b0(i16, S20 *P2, T12 *)
{//()
  static TEXT *pText_4;
  static S20 *s20A3;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  s20A3 = P2;
  if (s20A3->pnt14 == NULL) RETURN;
  if (s20A3->w18 == 0) RETURN;
  pText_4 = (TEXT *)TAG004a22_16(5, 0);
  pText_4->rectPos6.x1 = s20A3->rectPos4.x1;
  pText_4->rectPos6.x2 = s20A3->rectPos4.x2;
  pText_4->rectPos6.y1 = s20A3->rectPos4.y1;
  pText_4->rectPos6.y2 = s20A3->rectPos4.y2;
  pText_4->pnt2 = (char*)s20A3->pnt14;
  DisplayText(_1_, 5, pText_4);
  ReleaseMem(5, (ui8 *)pText_4);
  RETURN;
}

// *********************************************************
//
// *********************************************************
void TAG008a42()
{
  NotImpMsg(0x8a42);
}

// *********************************************************
//
// *********************************************************
i32 TAG008a62()
{//(i32)
  dReg D7;
  ui32  *pi_12;
  pnt pnt_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pnt_4 = AssignMemory(14, 576);
  pi_12 = &VBLCount;   
  D7L = 0;
  SetSupervisorMode();// and set up to clear it later
  /*
*/
  ClearSupervisorMode();
  D7L = 7;
  ReleaseMem(14, (ui8 *)pnt_4);
  return (1);
}

// *********************************************************
//
// *********************************************************
void TAG008c20()
{
  NotImpMsg(0x8c20);
}


// *********************************************************
//
// *********************************************************
void TAG008c40_1(i16 P1)
{//()
  i32 i_4 = 0x1baddade;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(5, P2);
  i_4 = 0; // Just so result is defined.
  f.Word13900 = 0;
  f.Word13898 = 0;
  ClearMemory((ui8 *)t12_14106, 144);
  ClearMemory((ui8 *)f.Pointer13962, 48);
  f.Word13896 = EnableButtons(&clickMap13914);
  if (f.Word13896 != -1) return;
  TAG0093a0_18(P1, 37);
  //TAG009a1c(5, P2);
  return;
}

// *********************************************************
//
// *********************************************************
void TAG008c40_2(i16 /*P1*/)
{//()
  i32 i_4 = 0x1baddade;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(5, P2);
  f.Word13900 = 0;
  f.Word13898 = 0;
  ClearMemory((ui8 *)t12_14106, 144);
  ClearMemory((ui8 *)f.Pointer13962, 48);
  i_4 = 0; //Just so we return consistent result
  //TAG009a1c(5, P2);
  return;
}

// *********************************************************
//
// *********************************************************
void TAG008c40_3(i16 P1, i32 nP2, i32 nP3)
{//()
  dReg D7;
  aReg A0;
  T12 *t12A3;
  S20 *s20A4;
  pnt pnt_20;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(5, P2);
  D7W = sw(TAG008c40_12(P1, nP2, 0, NULL));
  if (D7W < 0) return;
  s20A4 = &s20_16208[D7W];
  if (s20A4->rectPos4.x1 != 0)
  {
    if (s20A4->rectPos4.x2 != 0)
    {
      t12A3 = &t12_14106[f.Word13900++];
      t12A3->b0 = s20A4->b2;
      MemoryMove((ui8 *)&s20A4->rectPos4,
                 (ui8 *)&t12A3->rectPos2,
                 1024,
                 1024,
                 8);
      t12A3->b1 = s20A4->b3;
      t12A3->w10 = 1;
      if (nP3 != 0)
      {
        s20A4->pnt14 = (char *)nP3;
      };
    };
  };      
  if (s20A4->w12 == 0) return;
  A0 = f.Pointer13962[f.Word13898++];
  pnt_20 = A0;
  A0[0] = s20A4->b2;
  A0[1] = s20A4->b3;
  wordGear(A0+2) = s20A4->w12; 
  //TAG009a1c(5, P2);
  return;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG008c40_5(i16 P1, i32 nP2)
{//(i32)
  static dReg D6;
  static i16 *wA0, *wA1;
  static PALETTEPKT pkt_38;
  static i32 word16808;
  static i32 word16806;
  static i32 word16800;
  static i32 i_4 = 0x1baddade;
  RESTARTMAP
    RESTART(26)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(5, P2);
  f.Long14110 = VBLCount;
  pkt_38.w0 = 13;
  pkt_38.pPalette6 = &f.Palette16608;
  if (f.Word13900 == 0)
  {
    while (f.Word12822 == 0) {};
    i_4 = 0;
    RETURN_int(i_4);
  };
  for (;;)
  {
    TAG001dde(_26_, &f.s6_13884);
    D6W = (I16)(intResult & 0xff);
    if (D6W != 0)
    {
      switch (nP2)
      {
      case 2:
          word16800 = LE16(f.Segment1_16872[36]);
          f.Palette16608.color[(UI16)(LE16(f.Segment1_16872[8]))]
                  = sw(word16800);
          TAG005ae8_38(P1, &pkt_38);
          break;
      default:
          break;
      };//switch
      break;
    };
    switch (nP2)
    {
    case 2: //Load Done screen
        word16806 = 20; //LE16(f.Segment1_16872[33]);
        if ((unsigned)f.Long14110+word16806 < VBLCount)
        {
          f.Long14110 = VBLCount;
          wA0 = &f.Palette16608.color[LE16(f.Segment1_16872[8])];
          wA1 = &f.Segment1_16872[f.Word14112+36];
          *wA0 = LE16(*wA1);
          word16808 = LE16(f.Segment1_16872[32]);
          f.Word14112 = sw((f.Word14112+1) % word16808);
          TAG005ae8_38(P1, &pkt_38);
          break;
        };
        //D7W = TAG0083cc((pnt)TAG0083ac, (pnt)TAG008898);
            
        //**** Deleted Nonsense *****//

        break;
    default:
        break;
    }; //switch
  } ;
  i_4 = D6W;
  //TAG009a1c(5, P2);
  RETURN_int(i_4);
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG008c40_6(i16 P1, i32 nP2)
{ //()
  //text to buttons??
  static dReg D0, D7;
  static T12 *t12A3;
  static S20 *s20A4;
  static i32 i_4 = 0x1baddade;
  RESTARTMAP
    RESTART(22)
    RESTART(23)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(5, P2);
  if (nP2 != 0)
  {
    D0W = sw(TAG008c40_12(P1, nP2, 1, NULL));
    t12A3 = &t12_14106[D0W];
    D0W = sw(TAG008c40_12(P1, nP2, 0, NULL));
    s20A4 = &s20_16208[D0W];
    TAG0089b0(_22_, P1,s20A4,t12A3);
    RETURN_int(i_4);
  };

  t12A3 = &t12_14106[0];
  for (D7L=0; D7W<f.Word13900; D7W++,t12A3++)
  {
    D0L = (UI8)(t12A3->b0);
    D0W = sw(TAG008c40_12(P1, D0L, 0, NULL));
    s20A4 = &s20_16208[D0W];
    TAG0089b0(_23_,P1, s20A4, t12A3);
//
//
  };
  //TAG009a1c(5, P2);
  RETURN_int(i_4);
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG008c40_8(i16 P1, i16 nP2)
{//(i32)
  static dReg D7;
  static wordRectPos *pRectPos_42;
  static TEXT * pText_16;
  static pnt pnt_12;
  static wordRectPos *pRectPos_8;
  static i32 long16864;
  static i32 i_8;
  static i32 i_4 = 0x1baddade;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(20)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(5, P2);
  i_4 = nP2;
  if (i_4 == 0) RETURN_int(i_4);
  pRectPos_8 = (wordRectPos *)TAG008c40_12(P1, i_4, 3, NULL);
  pnt_12 = (pnt)TAG008c40_12(P1, i_4, 4, NULL);//Text of button pressed
  if (pnt_12 == NULL)
  {
    TAG0093a0_18(P1, 68);
  };
  pText_16 = (TEXT *)TAG004a22_16(P1, 0);
  pRectPos_42 = pRectPos_8;
  pText_16->rectPos6.x1 = pRectPos_42->x1;
  pText_16->rectPos6.x2 = pRectPos_42->x2;
  pText_16->rectPos6.y1 = pRectPos_42->y1;
  pText_16->rectPos6.y2 = pRectPos_42->y2;
  pText_16->pnt2 = (char *)pnt_12;
  TAG002af4();
  TAG0051c2_36(P1);
  TAG004a22_26(P1, (UI16)(LE16(f.Segment1_16872[7])),0x1baddade);
  DisplayText(_1_, P1, pText_16);
  D7W = TAG0064fc((pnt)MinA, (pnt)MinB);
  TAG005a1e_5(_20_, P1, 20);
  i_8 = TAG008c40_12(P1, i_4, 6, NULL);
  TAG004a22_26(P1, i_8, 0x1baddade);
  DisplayText(_2_, P1, pText_16);
  TAG0051c2_37(P1);
  TAG002b0c();
  ReleaseMem(P1, (ui8 *)pText_16);
  long16864 = LE32(LoadLong((pnt)(f.Segment1_16872+8)));
  if ((long16864&0xffff) != i_8)
  {
    TAG004a22_26(P1, long16864&0xffff,0x1baddade);
  };
  TAG008c40_2(P1);
  //TAG009a1c(5, P2);
  RETURN_int(i_4);
}

// *********************************************************
//
// *********************************************************
i32 TAG008c40_12(i16 /*P1*/, i32 nP2, i32 nP3, pnt nP4)
{//(i32)
  dReg D0, D7;
  S20 *s20A0;
  i32 i_4 = 0x1baddade;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(5, P2);
  i_4 = -1;
  switch (nP3)
  {
  case 0: 
      for (D7L=0; D7L<20; D7L++)
      {
        s20A0 = &s20_16208[D7L];
        if (s20A0->b2 == nP2)
        {
          i_4 = D7L;
          break;
        };
      };
      break;
  case 1:
      NotImpMsg(0x8d26);
      /*
*/
      break;
  case 2:
      NotImpMsg(0x8d54);
      /*
*/
      break;
  case 3:
      for (D7L=0; D7L<20; D7L++)
      {
        D0B = s20_16208[D7W].b2;
        if (D0B == nP2)
        {
          i_4 = (i32)&s20_16208[D7W].rectPos4;
          break;
        };
//
//
      };
      break;
  case 4:
      for (D7L=0; D7L<20; D7L++)
      {
        D0B = s20_16208[D7L].b2;
        if (D0B == nP2)
        {
          i_4 = (i32)s20_16208[D7L].pnt14;
          break;
        };
//
//
      };
      break;
  case 5:
      for (D7L=0; D7L<20; D7L++)
      {
        D0B = s20_16208[D7L].b2;
        if (D0B != nP2) continue;
        s20_16208[D7W].pnt14 = (char *)nP4;
//
//
      };
      break;
  case 6:
      for (D7L=0; D7L<20; D7L++)
      {
        D0B = s20_16208[D7L].b2;
        if (D0B != nP2) continue;
        i_4 = s20_16208[D7L].w18;
        break;
//
//
      };
      break;
  default:
      break;
  }; //switch (P4)
  //TAG009a1c(5, P2);
  return (i_4);
}

// *********************************************************
//
// *********************************************************
void TAG008c40_30(i16 /*P1*/)
{//()
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(5, P2);
  //TAG009a1c(5, P2);
  return;
}


// *********************************************************
//
// *********************************************************
void TAG0093a0_17(i16,i32)
{//()
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(7, P2);
  return;
}

// *********************************************************
//
// *********************************************************
void TAG0093a0_18(i32 , i32)
{//()
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(7, P2);
  if (f.Word16508 == 1)
  {
    //TAG009410(_3_, f.Byte14210, 0x1baddade);
    char msg[100];
    sprintf(msg, "TAG009410 called.\n"
                 "Bytes14210 = 0x%02x,0x%02x",
                 f.Byte14210[0],f.Byte14210[1]);
    die(0,msg);
    //for (;;) {};
  };
  die(0, "Sorry about that");// TAG005a1e_5(_2_, P1, 360); // wait 360 VBLs
  //for (;;) {};
  return;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG009410(pnt,i32 P2)
{//()
  static i8 b_50[50];
  RESTARTMAP
    RESTART(2)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  atari_sprintf((char *)b_50, (const char*)f.Byte14208, P2);
  TAG0051c2_36(7);
  f.Pointer15794 = b_50;
  //TAG0089b0(_2_, 7, &s20_15808, 0);
  TAG0089b0(_2_, 7, &s20_16208[17], 0);
  TAG0051c2_37(7);
  RETURN;
}

// *********************************************************
//
// *********************************************************
//   TAG009462
void Clear_14608()
{
  for (i32 i=0; i<23; i++)
  {
    f.pBook14608[i] = NULL;
  };
}

// *********************************************************
//
// *********************************************************
i32 TAG00948c(i16 P1)
{//(i32)
  BOOK *pBook_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;
  pBook_4 = (BOOK *)AssignMemory(P1, 10);
  if (pBook_4 != NULL)
  {
    pBook_4->w0 = 0; //#pages?
    pBook_4->pNode6 = NULL; // last page?
    pBook_4->pNode2 = NULL; //first page
    f.pBook14608[P1] = pBook_4;
    return (0);
  };
  return (1);
}

// *********************************************************
//
// *********************************************************
pnt TAG0094de(i16 P1, PAGE *P2, NODE *P3)
{//(pnt)
  NODE  *pNode_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pNode_4 = (NODE *)AssignMemory(P1, 8);
  if (pNode_4 != NULL)
  {
    pNode_4->pPage0 = P2;
    pNode_4->pNode4 = P3;
  };
  return ((pnt)pNode_4);
}

// *********************************************************
//
// *********************************************************
//  TAG009516
i32 AddPage(i16 P1, PAGE *P2)
{//(i32)
  NODE *pNode_8;
  BOOK *pBook_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;       
  pBook_4 = f.pBook14608[P1];
  if (pBook_4 == NULL)
  {
    return (1);
  };
  pNode_8 = (NODE *)TAG0094de(P1, P2, NULL);
  if (pNode_8 != NULL)
  {
    if (pBook_4->w0 != 0) //#pages
    {
      pBook_4->pNode6->pNode4 = pNode_8;//Add us to end of list
    }
    else
    {
      pBook_4->pNode2 = pNode_8;//Set as first page
    };
    pBook_4->pNode6 = pNode_8;//Set as last page
    pBook_4->w0++; //Page count
    return (0);
  };
  return (1);
}

// *********************************************************
//
// *********************************************************
//  TAG00958a
i16 PageCount(i16 P1)
{
  BOOK *pBook_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pBook_4 = f.pBook14608[P1];
  if (pBook_4 == NULL) return 0;
  return pBook_4->w0;
}

// *********************************************************
//
// *********************************************************
pnt TAG0095b0(i32 P1, i32 P2)
{//(pnt)
  NODE *pNode_8;
  BOOK *pBook_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pBook_4 = f.pBook14608[P1];
  if (pBook_4 == NULL)
  {
    TAG0093a0_18(P1, 17);
    return NULL;
  };
  if ( (P2 <= 0) || (P2 > pBook_4->w0) )
  {
    TAG0093a0_18(P1, 18);
    return NULL;
  };
  for (pNode_8=pBook_4->pNode2; 
       (pNode_8!=NULL)&&(--P2 > 0);
       pNode_8 = pNode_8->pNode4 )
  {
//
//
  };
  if (pNode_8 == NULL)
  {
    TAG0093a0_18(P1, 19);
  };
  return ((pnt)pNode_8);
}

// *********************************************************
//
// *********************************************************
pnt TAG00964e(i32 P1, i32 P2)
{ //(pnt)
  //Remove page from book.
  NODE *nodeA0;
  BOOK *bookA1;
  PAGE *pPage_16;
  NODE *pNode_8=(NODE *)0x1baddade;
  BOOK *pBOOK_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pBOOK_4 = f.pBook14608[P1];
  if (pBOOK_4 == NULL)
  {
    TAG0093a0_18(P1, 20);
  };

  if ( (P2 <= 0) || (P2 > pBOOK_4->w0) )
  {
    TAG0093a0_18(P1, 21);
  };
  if (P2 == 1) // First page of book
  {
    pNode_8 = pBOOK_4->pNode2;
    nodeA0 = pBOOK_4->pNode2;
    bookA1 = pBOOK_4;
    bookA1->pNode2 = nodeA0->pNode4;
  }
  else
  {
    if (P2 == pBOOK_4->w0) // Last page of book
    {
      pBOOK_4->pNode6 = (NODE *)TAG0095b0(P1, P2-1);
      pntResult;
      pNode_8=pBOOK_4->pNode6->pNode4;
      pBOOK_4->pNode6->pNode4 = NULL;
    }
    else
    { // Middle page of book.
      NotImpMsg(0x9706);
      /*
*/
    };
  };
  pPage_16 = pNode_8->pPage0;
  ReleaseMem(P1, (ui8 *)pNode_8);
  pBOOK_4->w0--; //One fewer pages in book.
  if (pBOOK_4->w0 == 0) // Is book empty?
  {
    ReleaseMem(P1, (ui8 *)pBOOK_4);
    f.pBook14608[P1] = NULL;
  };
  return ((pnt)pPage_16);
}

// *********************************************************
//
// *********************************************************
void TAG00978a_1(i16 /*P1*/)
{//()
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(13, P2);
  Clear_14608();
  //TAG009a1c(13, P2);
  return;
}

// *********************************************************
//
// *********************************************************
void TAG00978a_3(i16 P1, PAGE *nP2)
{//()
  dReg D0;
  i32 i_4;
  PAGE *pPageP3;
  pPageP3 = (PAGE *)nP2;
  i_4 = 0x1baddade;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(13, P2);
  {
    if (f.pBook14608[P1] == NULL)
    {
      D0W = sw(TAG00948c(P1));
      if (D0W != 0)
      {
        TAG0093a0_18(P1, 26);
      };
    };
    D0W = sw(AddPage(P1, pPageP3));
    if (D0W != 0)
    {
      TAG0093a0_18(P1, 27);
    };
  };
  //TAG009a1c(13, P2);
  return;
}

// *********************************************************
//
// *********************************************************
pnt TAG00978a_12(i16 P1, i32 nP2)
{//(pnt)
  i32 i_4;
  PAGE *pPageP3;
  pPageP3 = (PAGE *)nP2;
  i_4 = 0x1baddade;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(13, P2);
  {
    NODE *pNode_8;
    pNode_8 = (NODE *)TAG0095b0(P1, nP2);//Find node of P3th page.
    pntResult;
    i_4 = (i32)pNode_8->pPage0;
  };
  //TAG009a1c(13, P2);
  return ((pnt)i_4);
}

// *********************************************************
//
// *********************************************************
i32 TAG00978a_13(i16 P1)
{//(i32)
  dReg D0;
  i32 i_4;
  i_4 = 0x1baddade;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(13, P2);
  D0W = PageCount(P1);
  i_4 = (UI16)(D0W);
  //TAG009a1c(13, P2);
  return (i_4);
}

// *********************************************************
//
// *********************************************************
pnt TAG00978a_21(i16 P1, i32 nP2)
{//(pnt)
  i32 i_4;
  PAGE *pPageP3;
  pPageP3 = (PAGE *)nP2;
  i_4 = 0x1baddade;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(13, P2);
  i_4 = (i32)TAG00964e(P1, nP2);
  //TAG009a1c(13, P2);
  return ((pnt)i_4);
}

// *********************************************************
//
// *********************************************************
void TAG00978a_27(i16 P1, i32 nP2, pnt nP3, i32 nP4)
{//()
  i32 i_4;
  PAGE *pPageP3;
  TEXT *pTextP4;
  pTextP4 = (TEXT *)nP3;
  pPageP3 = (PAGE *)nP2;
  i_4 = 0x1baddade;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(13, P2);
  {
    PAGE  *pPage_8;
    pPage_8 = (PAGE *)AssignMemory(P1, 16);
    pPage_8->w0 = 8;
    pPage_8->i4 = nP2;
    pPage_8->pText8 = pTextP4;
    pPage_8->i12 = nP4;
    TAG00978a_3(P1, pPage_8);//Add new page to book
  };
  //TAG009a1c(13, P2);
  return;
}

// *********************************************************
//
// *********************************************************
pnt TAG00978a_28(i16 P1,i16 nP2,i16 nP3)
{//(pnt)
  i16  w_18;
  i16  w_16;
  i16  w_14;
  PAGE *pPage_8;
  i32 i_4;
  PAGE *pPageP3;
  TEXT *pTextP4;
  pTextP4 = (TEXT *)((int)nP3);
  pPageP3 = (PAGE *)((int)nP2);
  i_4 = 0x1baddade;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(13, P2);
  i_4 = 0;
  w_16 = sw(TAG00978a_13(P1));
  w_18 = sw(nP3);
  for (w_14 = w_16; w_14>0; w_14--)
  {
    pPage_8 = (PAGE *)TAG00978a_12(P1, w_14);
    pntResult;
    if (pPage_8->w0 == 8)
    {
      if (   (nP2 == pPage_8->i4)
          || (nP2==-1)  )
      {
        if ((w_18--) <= 0)
        {
          pPage_8->w2 = w_14;
          i_4 = (i32)pPage_8;
          break;
        };
      };
    };
//
//
  };
  //TAG009a1c(13, P2);
  return ((pnt)i_4);
}

// *********************************************************
//
// *********************************************************
void TAG00978a_29(i16 P1, i16 nP2)
{//()
  PAGE *pPage_8;
  i32 i_4;
  PAGE *pPageP3;
  pPageP3 = (PAGE *)((int)nP2);
  i_4 = 0x1baddade;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //TAG009a02(13, P2);
  pPage_8 = (PAGE *)TAG00978a_21(P1, nP2);
  if (pPage_8->w0 != 8)
  {
    TAG0093a0_18(P1, 29);
  };
  ReleaseMem(P1, (ui8 *)pPage_8);
  //TAG009a1c(13, P2);
  return;
}

// *********************************************************
//
/*
// *********************************************************
void TAG009a02(i32,i32)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  f.Word10462++;
}

// *********************************************************
//
// *********************************************************
void TAG009a1c(i32,i32)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  f.Word10462--;
}
*/
// *********************************************************
//
// *********************************************************
void TAG009a40()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  f.Word10462 = 1;
}

// *********************************************************
//
// *********************************************************
void TAG009d36(i16 P1, i16 P2)
{
//;;;;;;;;;;;;;;;;;;;;;;
  f.Word13852[0] = P2;
  f.Word13876 = 122;
  f.Word13874 = 0;
  f.Word13870 = 1;
  f.Word13864 = P1;
  TAG00ae58();
}

// *********************************************************
//
// *********************************************************
void TAG009d5e(i16 P1)
{
//;;;;;;;;;;;;;;;;;;;;;;
  f.Word13876 = 123;
  f.Word13874 = 0;
  f.Word13870 = 0;
  f.Word13864 = P1;
  TAG00ae58();
}

// *********************************************************
//
// *********************************************************
void TAG009d7e(i16 P1, i16 *P2, i16 *P3, i16 *P4)
{
//;;;;;;;;;;;;;;;;;;;;;;;;
  f.Word13876 = 124;
  f.Word13874 = 0;
  f.Word13870 = 0;
  f.Word13864 = P1;
  TAG00ae58();
  *P2 = f.Word13596;
  *P3 = f.Word13084;
  *P4 = f.Word13082;
}

// *********************************************************
//
// *********************************************************
void TAG009db6(i16 P1, i32 P2, i32 *P3)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TAG00adf4(P2);
  f.Word13876 = 125;
  f.Word13874 = 0;
  f.Word13870 = 0;
  f.Word13864 = P1;
  TAG00ae58();
  TAG00ae38(P3);
}

// *********************************************************
//
// *********************************************************
void TAG009dea(i16 P1, i32 P2, i32 *P3)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TAG00adf4(P2); //Saves pointer at Word13862 and Word13860
  f.Word13876 = 126;
  f.Word13874 = 0;
  f.Word13870 = 0;
  f.Word13864 = P1;
  TAG00ae58();
  TAG00ae38(P3);
}

// *********************************************************
//
// *********************************************************
void TAG009ea0(i16 *,i16 *, i16 *)
{
  NotImpMsg(0x9ea0);
}

// *********************************************************
//
// *********************************************************
void TAG009f1c(i16 *P1, i16 *P2, i16 *P3)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  f.Pointer218 = P1;
  f.Pointer210 = P3;
  f.Pointer206 = P3 + 90;
  f.Word13876 = 100;
  f.Word13874 = 0;
  f.Word13870 = 11;
  f.Word13864 = *P2;
  TAG00ae58();
  *P2 = f.Word13864;
  f.Pointer218 = f.Word13852;
  f.Pointer210 = &f.Word13596;
  f.Pointer206 = &f.Word13084;
  f.Pointer214 = f.Byte13340;
}

// *********************************************************
//
// *********************************************************
void TAG009f80(i16 P1)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  f.Word13876 = 101;
  f.Word13874 = 0;
  f.Word13870 = 0;
  f.Word13864 = P1;
  TAG00ae58();
}

// *********************************************************
//
// *********************************************************
void TAG00a05c(i16 * *)
{
//;;;;;;;;;;;;;;;;;;
  /*
*/
}

i8 data00a0d6[] ALIGN4 =
{
    0,1,0,
    2,1,1,   
    2,1,1,   
    0,1,1,   
    2,1,1,
    1,1,1,
    0,0,0,
    0,0,0,
    0,0,0,
    0,1,0,
    0,1,0,
    3,5,0,
    5,5,0,
    0,1,1,
    2,1,0,
    0,7,1,
    2,1,0,
    0,0,0,
    0,0,0,
    0,0,0,
    1,1,1,
    2,1,1,
    2,1,1,
    2,1,1,
    1,1,2,
    1,1,1,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    2,1,1,
    1,1,1,
    6,1,1,
    4,1,1,
    1,3,1,
    2,1,1,
    4,2,1,
    8,1,1,
    0,0,0,
    0,0,0,
    1,1,1,
    9,1,1,
    1,1,1,
    1,1,0,
    0,5,1,
    3,3,1,
    2,2,1,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    4,3,0,
    8,3,0,
    6,1,0,
    8,1,0,
    8,1,0,
    4,1,1,
    3,1,1,
    0,5,0,
    1,1,1,
    0,5,0,
    0,1,1,
    0,1,1,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,2,2,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    5,1,0,
    5,1,0,
    1,1,0,
    1,1,0,
    2,5,0,
    6,1,0,
    2,1,0,
    1,1,0,
    6,5,0,
    0,0,0,
    0,1,1,
    0,1,0,
    2,1,0,
    2,1,1,
    1,1,1,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,1,2,
    3,1,2,
    1,1,1,
    1,1,1,
    0,1,1,
    0,1,2
};

// *********************************************************
//
// *********************************************************
i16 TAG00a07a(i16 P1)
{
  dReg D0;
  aReg A4;
  i32 i;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  f.Word380[0] = P1;
  A4 = data00a0d6;
  D0L = 3*(P1-10);
  A4 += D0L;
  for (i=0; i<4; i++)
  {
    f.Word380[i] = (UI8)(*(A4++));
//
//
  };
  TAG00a05c(f.Pointer244);
  return f.Word304[0];
}

// *********************************************************
//
// *********************************************************
i16 TAG00a238()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  f.Pointer268 = f.Word380;
  f.Pointer264 = f.Byte370;
  f.Pointer260 = f.Word338;
  f.Pointer256 = f.Word304;
  f.Pointer252 = f.Long288;
  f.Pointer248 = f.Byte276;
  f.Pointer244 = &f.Pointer268;
  TAG00a07a(10);
  f.Word240 = f.Word304[0];
  return f.Word240;

}

// *********************************************************
//
// *********************************************************
i16 TAG00a28c()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TAG00a07a(19);
  return 1;
}

// *********************************************************
//
// *********************************************************
i16 TAG00aa22(i16 *P1, i16 *P2, i16 *P3, i16 *P4)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TAG00a07a(77);
  *P1 = f.Word304[1];
  *P2 = f.Word304[2];
  *P3 = f.Word304[3];
  *P4 = f.Word304[4];
  return f.Word304[0];
}

// *********************************************************
//
// *********************************************************
void TAG00aa58(i16 P1,i32 P2)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  f.Word338[0] = P1;
  f.Long288[0] = P2;
  TAG00a07a(78);
}

// *********************************************************
// Parameter points into code!
// *********************************************************
void TAG00adf4(i32 P1)
{
  f.Word13862 = sw(P1 >> 16);
  f.Word13860 = (UI16)(P1 & 0xffff);
}

// *********************************************************
// Assembles Wod13858 and Word13856 into single 32-bit integer
// *********************************************************
void TAG00ae38(i32 *P1)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  *P1 = ((UI16)(f.Word13858)<<16) | uw(f.Word13856);
}

// *********************************************************
//
// *********************************************************
void TAG00ae58()
{
  //if (testfornotimplemented) NotImpMsg(0xae58);
//;;;;;;;;;;;;;;;;;;;;;;;
  /*
  */
}

// *********************************************************
//
// *********************************************************
pnt TAG00aeda(i16, i16)
{
  NotImpMsg(0xaeda); return NULL;
}

// *********************************************************
//
// *********************************************************
void TAG00aef0(i16)
{
  NotImpMsg(0xaef0);
}

// *********************************************************
//
// *********************************************************
void TAG00af06() //(i16 P1,i16 P2)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  f.Pointer184 = MinA;  
  f.Pointer180 = MinB;
  //if (testfornotimplemented) NotImpMsg(0xaf16);
  /*





*/
}
