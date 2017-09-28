// *********************************************************
//  This collection of code is a translation of the
//  'Create New Adventure' function.  It reads an
//  old Dungeon Master  or Prison save file and creates
//  a new Chaos Strikes Back  save file.
// *********************************************************

#include "stdafx.h"

//#include "Objects.h"
#include "Dispatch.h"
#include "UI.h"
#include "CSB.h"
#include "Data.h"

#include <stdio.h>

static void DontDoThis(i32 n)
{
  char msg[1000];
  sprintf(msg, "Don't Do This #0x%x\n"
               "The UTILITY functions are designed to create a\n"
               " Chaos Strikes Back game from a Prison SAVEGAME\n"
               " and a 'mini.dat' file.  It will not import nor\n"
               " edit characters nor anything else.  You load a\n"
               " prison savegame, press 'Make New Adventure' twice\n"
               " choose a name for the new adventure, and quit.\n"
               " You can then RESTART at the prison door and\n"
               " and select your new game to play.  You will\n"
               " start naked in a dark room filled with worms.\n"
               " If you try to use this program in any other \n"
               " way you will probably be unhappy with the\n"
               " results.  But you need not complain.  I won't listen.", n);
  UI_MessageBox(msg, "Error",MESSAGE_OK);
}


#define CHAOSFILL(a,b) i8 fill##a[a-b];
i32 ReadExtendedFeatures(i32 handle);
ui32 FormChecksum(ui8 *buf, i32 num);
void info(char *, unsigned int);
ui16 countDSAs();
void WriteDSAs(i32 handle);
void WriteDSALevelIndex(i32 handle);

//extern bool ForcedScreenDraw;
extern i32 keyboardMode;
extern char ExtendedFeaturesVersion;
extern bool indirectText;
extern bool bigActuators;
extern bool sequencedTimers;
extern bool extendedTimers;
extern bool DefaultDirectXOption;
extern ui32 cellflagArraySize;
extern DSAINDEX DSAIndex;

#pragma pack(1)


void _CALL0(CODESTATE s);
void _CALL1(CODESTATE s,i32 a);
void _CALL2(CODESTATE s,i32 a,i32 b);
void _CALL3(CODESTATE s,i32 a,i32 b,i32);
void _CALL4(CODESTATE s,i32 a,i32 b,i32 c,i32 d);
void _CALL5(CODESTATE s,i32 a,i32 b,i32 c,i32 d,i32 e);
void _CALL6(CODESTATE s,i32 a,i32 b,i32 c,i32 d,i32 e,i32 f);
void _CALL7(CODESTATE s,i32 a,i32 b,i32 c,i32 d,i32 e,i32 f,i32 g);
void _CALL8(CODESTATE s,i32 a,i32 b,i32 c,i32 d,i32 e,i32 f,i32 g,i32 h);
void _CALL9(CODESTATE s,i32 a,i32 b,i32 c,i32 d,i32 e,i32 f,i32 g,i32 h,i32 i);


struct DlgButton
{
  i8          byte0;
  i8          byte1;
  wordRectPos rectPos2;
  i8          byte10;
  i8          byte11;
};

struct S12406
{
  i8        byte0;
  i8        byte1;
  DlgButton *pDlgButton2; // An array of DlgButton
  pnt       pnt6;
  pnt       pnt10;
};




struct STRUCT6
{
  i32   int0;
  i16   word4;
};

struct STRUCT12
{
  i32  long0;
  i16  word4;
  i16  word6;
  void (*fnc8)(pnt);
};


#define size3764 880

struct E
{
  // PALETTE palette24862; (Use palette24862)
  //pnt     Byte24830[16];(use byte24830)
  i16     Word24814;
  pnt     Pnt24812;
  i16     Word24808;
  i16     Word24806;
  i16     Word24804;
  i16     Word24802;
  i16     Word24800;
  i16     Word24798;
  i16     Word24796;
  i16     Word24794;
  //i8      Byte24792;
  //i8      adjust24792;
  pnt     Pnt24792;
  i32     Long24300;
  pnt     Pnt24296[100];
  pnt     Pnt23896[100];
  i16     Word23496;
  char    Byte23494[1];
  CHAOSFILL(23493,23414)
  //RectPos rectPos23414; (use rectPos23414)
  i16     Word23398;
  i16     Word23396;
  ui8     Byte23394[10608];//bitmap 102 lines by 104 bytes
  //pnt     UtilityDiskMessages; //12786
  //pnt     Pnt12782;  // to
  //pnt     Pnt12678;   // 12618
  //i32     Long12658;   // are
  //i32     Long12654;    // 42 string pointers.
  //                       // (use UtilityDiskMessages[i])
  //pnt     Pnt12446;//use pnt12446;
  //i8      Byte12406;(use s12406)
  // never referenced i8      Byte12392;
  //pnt     Pnt12364;(use pnt12364);
  i8      Byte12336[6]; //actually i32 and i16
  //pnt     Pnt12246;(use pnt12246)
  //pnt     Pnt12242;(use pnt12242)
  //pnt     Pnt12238;(use pnt12238)
  //pnt     Pnt12226;(use pnt12226)
  //pnt     Pnt12222;(use pnt12222)
  i16     Word12196;
  i16     GameFile;//12194;
  i16     Word12184;
  PALETTE palette10938;
  i32     Long9220;
  i16     FirstQueuedMouseEvent;//9216;
  i16     LastQueuedMouseEvent;//9214;
  i8      Byte9212;
  i8      Byte9211;
  i16     Word9210;
  i16     Word9208;
  i16     Word9206;
  i16     Word9204;
  i8      Byte9202[30];// 5 * (i32+i16) ???
  i8      Byte9172[6];//i32 and i16??
  //pnt     Pnt9142;//use pPnt9142
  //i16     Word9138[7];//use word9138
  i16     Word9124;
  i16     Word9122;
  i16     Word9120;
  i16     Word9116;
  i16     Word9110;
  i16     Word9108;
  i16     Word9106;
  i16     Word9104;
  i16     Word9102;
  i16     Word9098;
  i16     Word8842[1];
  //never referenced i8      Byte8586[1];
  i16     Word8330[1];
  i16     Word8328[1];
  i16     Word8074;
  i16     Word8072;
  i16     Word8070;
  i16     Word8068;
  i16     Word8066;
  pnt     Pnt8064;
  pnt     Pnt8060;
  i16     Word8056;
  i16     Word8054;
  ui8    *PhysicalBase;//8052;
  i16     Word8048;
  i16     Word7062;
  i16     ActualCharacter; // Word7060
  i8      CharacterImages[4*464]; // Byte7058
  i8      Byte5202[24];
  i16     Word5178;
  wordRectPos wRectPos5176;
  i16     Word5168;
  i16     EditBoxSelected; //Word5166 //either NAME or TITLE
  i16     EditBoxPosition; //Word5164
  //wordRectPos wRectPos5162;(use wRectPos5162)
  //wordRectPos wRectPos5154;(use wRectPos5154)
  //wordRectPos wRectPos5146;(use wRectPos5146)
  //wordRectPos wRectPos5138[4];(use wRectPos5138)
  //wordRectPos wRectPos5106[4];(use wRectPos5106)
  //wordRectPos wRectPos5066;(use wRectPos5066)
  //wordREctPos wRectPos5058;(use wREctPos5058)
  //DlgButton dlgButton5050[4];//(use dlgButton5050)
  i8      Byte4930[1];
  CHAOSFILL(4929,4712)
  //i8      Byte4712[16];(use byte4712)
  i16     Word4696;
  i8      Byte4694[464];
  CHAOSFILL(4230,4228)
  i8      Byte4228[464];
  i8      Byte3764[size3764];
  i16     Word3122;
  i16     Word826[5];
  i16     Word816[16];
  i16     Word784[17];
  i16     Word750[8];
  i16     Word734[6];
  i16     Word722[6];
  i16     *Pnt714[7];
  i16     Word686;
  i16     *pw664;
  pnt     Pnt660;
  i16     *pw656;
  i16     *pw652;
  pnt     pAllocDBank434;
  pnt     Pnt430;
  i16     Word426;
  i16     Word424;
  pnt     pDBank422;
  i16     Word418;
  i32     DBankLen416;
  i8      Byte412[8];
};

E e;

unsigned char SelectedColor;


PALETTE palette24862 = {
    0x0000,0x0333,0x0444,0x0310,0x0066,0x0420,0x0040,0x0060,
    0x0700,0x0750,0x0643,0x0770,0x0222,0x0555,0x0007,0x0777};

i8 byte24830[16] ALIGN4 = {4,6,4,8,16,4,4,4,4,8,4,0,0,0,8,4};

wordRectPos rectPos23414 = { 0, 193,  0, 101};
wordRectPos rectPos23406 = {62, 255, 48, 149};

static i32 textArraySize; //Number of words
static char *textArray;
static DB2 *DB2Address;
static i32 DB2Size;
static i32 compressedTextSize;
char *compressedText = NULL;

const char *positions = "CSBGAME\nCSBGAME2\nCSBGAME3\nCSBGAME4\nCANCEL";

// ARGHH!
const char *filenames[] ={
	"CSBGAME.DAT",
	"CSBGAME2.DAT",
	"CSBGAME3.DAT",
	"CSBGAME4.DAT",
};


const char* UtilityDiskMessages[43] =
{
  "\nPLEASE PUT A\nSAVE GAME DISK IN @",  //0
  "\nPUT PORTRAIT DISK\nIN @",            //1
  "\nPUT A BLANK DISK\nIN @",             //2
  "",                                     //3
  "",                                     //4
  "",                                     //5
  "\n\nREPLACE EXISTING SAVED GAME?",     //6
  "",                                     //7
  "\n\nREPLACE $'s PORTRAIT?",            //8
  "\n\nPLEASE WAIT . . .\nLOADING SAVED GAME",//9
  "\n\nPLEASE WAIT . . .\nSAVING GAME",   //10
  "",                                     //11
  "\n\nCAN'T FIND SAVED GAME",            //12
  "",                                     //13
  "\nSAVED GAME ERROR",                   //14
  "\nERROR WHILE\nSAVING GAME",           //15
  "",                                     //16
  "",                                     //17
  "",                                     //18
  "",                                     //19
  "\nEACH CHAMPION MUST\nHAVE A UNIQUE FIRST NAME!",//20
  "",                                               //21
  "\n\nPLEASE WAIT . .\nLOADING SAVED ADVENTURE",   //22
  "\n\nPLEASE WAIT . .\nSAVING NEW ADVENTURE",      //23
  "CAN'T FIND\nNEW ADVENTURE\nMINI.DAT",  //24
  "\nTHE NEW ADVENTURE\nIS DAMAGED",      //25
  "",                                     //26
  "",                                     //27
  "\nERROR WHILE SAVING\nNEW ADVENTURE",  //28
  "THE NEW ADVENTURE\nCAN ONLY BE MADE AFTER\nA SAVED GAME IS LOADED.",                                     //29
  "\nCAN'T MAKE NEW ADVENTURE\nFROM THIS SAVED GAME.\nALREADY UTILIZED.",//30
  "LOAD WHICH SAVED GAME?",               //31
  "OK",                                   //32
  "OK\nCANCEL",                           //33
  "SAVE\nFORMAT\nCANCEL",                 //34
  "MAKE NEW ADVENTURE\nFORMAT\nCANCEL",   //35
  "YES\nNO",                              //36
  "",                                     //37
  "",                                     //38
  "DUNGEON MASTER\nCHAOS STRIKES BACK\nCANCEL", //39
  "",                                     //40
  "",                                      //41
  "\nPLEASE CHOOSE A\nSAVE GAME SLOT",  //42
};


#ifdef _MSVC_INTEL
#pragma warning (disable:4305)
#pragma warning (disable:4309)
#endif

i8 byte12618[] ALIGN4 = {01,00,00,13,00,00,00,00};

DlgButton dlgButton_12610[] = {
      {0x01,0x00,{0x0050,0x00ed,0x0080,0x0088},0x00,0x01},
      {0x00,0x00,{0x0000,0x0000,0x0000,0x0000},0x00,0x00},
      {0x01,0x00,{0x0050,0x0094,0x0080,0x0088},0x00,0x01},
      {0x02,0x00,{0x00a5,0x00ed,0x0080,0x0088},0x00,0x01},
      {0x00,0x00,{0x0000,0x0000,0x0000,0x0000},0x00,0x00},
      {0x01,0x00,{0x0050,0x0094,0x006c,0x0074},0x00,0x01},
      {0x02,0x00,{0x00a5,0x00ed,0x006c,0x0074},0x00,0x01},
      {0x03,0x00,{0x007b,0x00c4,0x0080,0x0088},0x00,0x01},
      {0x00,0x00,{0x0000,0x0000,0x0000,0x0000},0x00,0x00},
      {0x01,0x00,{0x0050,0x00ed,0x006c,0x0074},0x00,0x01},
      {0x02,0x00,{0x0050,0x0094,0x0080,0x0088},0x00,0x01},
      {0x03,0x00,{0x00a5,0x00ed,0x0080,0x0088},0x00,0x01},
      {0x00,0x00,{0x0000,0x0000,0x0000,0x0000},0x00,0x00},
      {0x01,0x00,{0x0050,0x00ed,0x0058,0x0060},0x00,0x01},
      {0x02,0x00,{0x0050,0x00ed,0x006c,0x0074},0x00,0x01},
      {0x03,0x00,{0x0050,0x00ed,0x0080,0x0088},0x00,0x01},
      {0x00,0x00,{0x0000,0x0000,0x0000,0x0000},0x00,0x00} };


DlgButton four_dlgButon[]={
		{0x01,0x00,{0x0050,0x0094,0x0070,0x0078},0x00,0x01},
		{0x02,0x00,{0x00a5,0x00ed,0x0070,0x0078},0x00,0x01},
		{0x03,0x00,{0x0050,0x0094,0x0080,0x0088},0x00,0x01},
		{0x04,0x00,{0x00a5,0x00ed,0x0080,0x0088},0x00,0x01},
		{0x00,0x00,{0x0000,0x0000,0x0000,0x0000},0x00,0x00}};


#ifdef _MSVC_INTEL
#pragma warning (default:4305)
#pragma warning (default:4309)
#endif

S12406  s12406[] = {
  { 00,00, &dlgButton_12610[0],  byte12618, NULL },
  { 00,00, &dlgButton_12610[2],  NULL,      NULL },
  { 00,00, &dlgButton_12610[5],  NULL,      NULL },
  { 00,00, &dlgButton_12610[9],  NULL,      NULL },
  { 00,00, &dlgButton_12610[13], NULL,      NULL } };


S12406 fourbuttons[]={
  { 00,00, &four_dlgButon[0],	 NULL,      NULL } };



const char *pnt12246 = "DMGAME.DAT";
const char *pnt12242 = "DMGAME.BAK";
const char *pnt12238 = "CSBGAME.DAT";
const char *pnt12234 = "CSBGAME.BAK";
const char *pnt12222 = "DRIVE B:";
const char *pnt12226 = "DRIVE A:";

S12406  *pS12406_9166[6] = {NULL,NULL,NULL,NULL,NULL,NULL};//guessed size!
S12406  **ppS12406_9142 = pS12406_9166;

i16  word9138[] = {
          0x4737, //turn right
          0x4838, //forward
          0x4b34, //slide left
          0x4d36, //slide right
          0x5032, //backup
          0x5230, //turn left
          0x532e};//delete

ui32 FrameImage[246] = //byte8046
{
   0xffff0000,0xffff0000,0xffff0000,0xffff0000,
   0xfff00000,0xfff00000,0xbfff3fff,0x80000000,
   0xffffffff,0x00000000,0xfff0fff0,0x00000000,
   0xdfff5fff,0x80000000,0xffffffff,0x00000000,
   0xfff0ffd0,0x00200000,0xefff6fff,0x80000000,
   0xffffffff,0x00000000,0xfff0ff90,0x00600000,
   0xf7ff77ff,0x80000000,0xffffffff,0x00000000,
   0xfff0ff10,0x00e00000,0xfbff7bff,0x80000000,
   0xffffffff,0x00000000,0xfff0fe10,0x01e00000,
   0xfc307c30,0x83ff0030,0x00e400e4,0xffff00e4,
   0x03f00010,0xffe00000,0xfc607c60,0x83ff0060,
   0x01c801c8,0xffff01c8,0x03f00010,0xffe00000,
   0xfc807c80,0x83ff0080,0x03800380,0xffff0380,
   0x07f00410,0xffe00400,0xfc007c00,0x83ff0000,
   0x07000700,0xffff0700,0x0ff00c10,0xffe00c00,
   0xfc007c00,0x83ff0000,0x1e001e00,0xffff1e00,
   0x0bf00810,0xffe00800,0xfc007c00,0x83ff0000,
   0x7c007c00,0xffff7c00,0x13f01010,0xffe01000,
   0xfc007c00,0x83ff0000,0xf800f800,0xfffff800,
   0x23f02010,0xffe02000,0xfc017c01,0x83ff0001,
   0xf000f000,0xfffff000,0x43f04010,0xffe04000,
   0xfc037c03,0x83ff0003,0xe000e000,0xffffe000,
   0x03f00010,0xffe00000,0xfc077c07,0x83ff0007,
   0x00000000,0xffff0000,0x03f00010,0xffe00000,
   0xfc0c7c0c,0x83ff000c,0x00000000,0xffff0000,
   0x03f00010,0xffe00000,0xfc187c18,0x83ff0018,
   0x00000000,0xffff0000,0x03f00010,0xffe00000,
   0xfc207c20,0x83ff0020,0x00000000,0xffff0000,
   0x03f00010,0xffe00000,0xfc407c40,0x83ff0040,
   0x00000000,0xffff0000,0x03f00010,0xffe00000,
   0xfc807c80,0x83ff0080,0x00000000,0xffff0000,
   0x03f00010,0xffe00000,0xfc007c00,0x83ff0000,
   0x00000000,0xffff0000,0x23f02010,0xffe02000,
   0xfc007c00,0x83ff0000,0x00000000,0xffff0000,
   0x43f04010,0xffe04000,0xfc007c00,0x83ff0000,
   0x00010001,0xffff0001,0x83f08010,0xffe08000,
   0xfc007c00,0x83ff0000,0x00030003,0xffff0003,
   0x03f00010,0xffe00000,0xfc007c00,0x83ff0000,
   0x000e000e,0xffff000e,0x03f00010,0xffe00000,
   0xfc007c00,0x83ff0000,0x001c001c,0xffff001c,
   0x03f00010,0xffe00000,0xfc207c20,0x83ff0020,
   0x00380038,0xffff0038,0x03f00010,0xffe00000,
   0xfc407c40,0x83ff0040,0x00780078,0xffff0078,
   0x07f00410,0xffe00400,0xfd807d80,0x83ff0180,
   0x00700070,0xffff0070,0x0ff00c10,0xffe00c00,
   0xff007f00,0x83ff0300,0x00e000e0,0xffff00e0,
   0x3bf03810,0xffe03800,0xfe007e00,0x83ff0200,
   0x00e000e0,0xffff00e0,0x73f07010,0xffe07000,
   0xfc007c00,0x83ff0000,0x81c081c0,0xffff81c0,
   0x43f04010,0xffe04000,0xfc037c03,0x83ff0003,
   0x07800780,0xffff0780,0x83f08010,0xffe08000,
   0xfc0e7c0e,0x83ff000e,0x0f000f00,0xffff0f00,
   0x03f00010,0xffe00000,0xffff7c00,0x83ff0000,
   0xffff0000,0xffff0000,0xfff00210,0xfde00000,
   0xffff7800,0x87ff0000,0xffff0000,0xffff0000,
   0xfff00110,0xfee00000,0xffff7000,0x8fff0000,
   0xffff0000,0xffff0000,0xfff00090,0xff600000,
   0xffff6000,0x9fff0000,0xffff0000,0xffff0000,
   0xfff00050,0xffa00000,0xffff4000,0xbfff0000,
   0xffff0000,0xffff0000,0xfff00030,0xffc00000,
   0xffff7fff,0x80000000,0xffffffff,0x00000000,
   0xfff0fff0,0x00000000 };


wordRectPos wRectPos5162 = {0x009d,0x00fc,0x003c,0x0092};
wordRectPos wRectPos5154 = {0x011e,0x012f,0x002b,0x00a9};
wordRectPos wRectPos5146 = {0x011c,0x0131,0x0029,0x00ab};
wordRectPos wRectPos5138[4] =
{
  {0x000b,0x002a,0x000e,0x002a},
  {0x004e,0x006d,0x000e,0x002a},
  {0x0091,0x00b0,0x000e,0x002a},
  {0x00d3,0x00f2,0x000e,0x002a} };

wordRectPos wRectPos5106[7] =
{
  {0x000b,0x002a,0x000e,0x002a}, //[0]=5106
  {0x004e,0x006d,0x000e,0x002a}, //[1]=5098
  {0x0091,0x00b0,0x000e,0x002a}, //[2]=5090
  {0x00d3,0x00f2,0x000e,0x002a}, //[3]=5082
  {0x004d,0x0078,0x0038,0x0060}, //[4]=5074
  {0x0053,0x0072,0x003e,0x005a}, //[5]=5066
  {0x0000,0x0098,0x0072,0x00af}, //[6]=5058
};

DlgButton dlgButton5050[17] =
{
  {0x01,0,{  4, 48,  3, 42},0,1},//[ 0]=5050
  {0x02,0,{ 71,115,  3, 42},0,1},//[ 1]=5038
  {0x03,0,{138,182,  3, 42},0,1},//[ 2]=5026
  {0x04,0,{205,249,  3, 42},0,1},//[ 3]=5014
  {0x05,0,{157,252, 60,146},0,1},//[ 4]=5002
  {0x06,0,{157,252, 60,146},0,2},//[ 5]=4990
  {0x07,0,{286,303, 43,169},0,1},//[ 6]=4978
  {0x08,0,{156,196,159,167},0,1},//[ 7]=4966
  {0x09,0,{225,253,159,167},0,1},//[ 8]=4954
  {0x64,0,{350,349,250,249},0,0},//[ 9]=4942
  {0x0b,0,{  2, 92,186,194},0,1},//[10]=4930
  {0x0c,0,{102,192,186,194},0,1},//[11]=4918
  {0x0d,0,{202,316,186,194},0,1},//[12]=4906
  {0x0e,0,{ 15, 59, 87, 95},0,1},//[13]=4894
  {0x0f,0,{ 15,131,100,108},0,1},//[14]=4882
  {0x66,0,{350,349,250,249},0,0},//[15]=4870
  {0x11,0,{288,316,  5, 13},0,1},//[16]=4858
};

i8 byte4846[8] ALIGN4 = {0x09,0x00,0x61,0x00,0x01,0x00,0x3b,0x00};

S12406 s12406_4802 =
{
  01,00, &dlgButton5050[0], byte4846, NULL
};


const char* pnt4788[] =
{
  "FIGHTER",
  "NINJA",
  "PRIEST",
  "WIZARD"
};

const char* pnt4772[16] =
{
  "NEOPHYTE   ",
  "NOVICE     ",
  "APPRENTICE ",
  "JOURNEYMAN ",
  "CRAFTSMAN  ",
  "ARTISAN    ",
  "ADEPT      ",
  "EXPERT     ",
  "` MASTER   ",
  "a MASTER   ",
  "b MASTER   ",
  "c MASTER   ",
  "d MASTER   ",
  "e MASTER   ",
  "ARCHMASTER "
};

i8 byte4712[16] ALIGN4 = {0x00,0x0c,0x01,0x02,
                   0x0d,0x0f,0x03,0x05,
                   0x0a,0x08,0x09,0x0b,
                   0x07,0x06,0x04,0x0e };

i8 Byte1278[420] ALIGN4 = {
         0x00,0x06,0x1b,0x0a,0x0f,0x19,0x0c,0x06,0x06,0x0c,0x09,0x04,0x00,0x00,0x00,0x03,
         0x0e,0x02,0x1e,0x1e,0x06,0x1e,0x0e,0x1f,0x0e,0x0e,0x06,0x06,0x03,0x00,0x0c,0x0e,
         0x0e,0x0e,0x1e,0x0f,0x1e,0x1f,0x1f,0x0f,0x11,0x1f,0x01,0x09,0x08,0x11,0x11,0x0e,
         0x1e,0x0e,0x1e,0x0f,0x1f,0x11,0x11,0x11,0x11,0x11,0x1f,0x0f,0x18,0x0f,0x04,0x00,
         0x04,0x1f,0x00,0x04,0x18,0x01,0x00,0x06,0x1b,0x1f,0x14,0x1a,0x1a,0x06,0x0c,0x06,
         0x06,0x04,0x00,0x00,0x00,0x06,0x13,0x06,0x01,0x01,0x0a,0x10,0x10,0x01,0x11,0x11,
         0x06,0x06,0x06,0x0f,0x06,0x13,0x11,0x11,0x11,0x10,0x11,0x10,0x10,0x10,0x11,0x04,
         0x01,0x0a,0x08,0x1b,0x19,0x11,0x11,0x11,0x11,0x10,0x04,0x11,0x11,0x11,0x0a,0x11,
         0x02,0x0c,0x0c,0x03,0x0e,0x00,0x09,0x11,0x1f,0x0a,0x04,0x03,0x00,0x06,0x12,0x0a,
         0x0e,0x04,0x0c,0x0c,0x0c,0x06,0x0f,0x1f,0x00,0x0f,0x00,0x0c,0x15,0x02,0x0e,0x0e,
         0x12,0x1e,0x1e,0x02,0x0e,0x0f,0x00,0x00,0x0c,0x00,0x03,0x06,0x17,0x1f,0x1e,0x10,
         0x11,0x1e,0x1e,0x13,0x1f,0x04,0x01,0x0c,0x08,0x15,0x15,0x11,0x1e,0x11,0x1e,0x0e,
         0x04,0x11,0x11,0x15,0x04,0x0a,0x04,0x0c,0x06,0x03,0x1b,0x00,0x12,0x11,0x00,0x11,
         0x07,0x05,0x00,0x06,0x00,0x1f,0x05,0x0b,0x1d,0x00,0x0c,0x06,0x06,0x04,0x06,0x00,
         0x06,0x18,0x19,0x02,0x10,0x01,0x1f,0x01,0x11,0x04,0x11,0x01,0x06,0x06,0x06,0x0f,
         0x06,0x06,0x16,0x11,0x11,0x10,0x11,0x10,0x10,0x11,0x11,0x04,0x11,0x0a,0x08,0x11,
         0x13,0x11,0x10,0x11,0x14,0x01,0x04,0x11,0x0a,0x1b,0x0a,0x04,0x08,0x0c,0x03,0x03,
         0x00,0x00,0x09,0x11,0x1f,0x0a,0x04,0x09,0x00,0x00,0x00,0x0a,0x1e,0x13,0x1a,0x00,
         0x06,0x0c,0x09,0x04,0x06,0x00,0x06,0x10,0x0e,0x02,0x1f,0x1e,0x02,0x1e,0x0e,0x04,
         0x0e,0x0e,0x06,0x02,0x03,0x00,0x0c,0x00,0x10,0x11,0x1e,0x0f,0x1e,0x1f,0x10,0x0f,
         0x11,0x1f,0x0e,0x09,0x0f,0x11,0x11,0x0e,0x10,0x0e,0x13,0x1e,0x04,0x0f,0x04,0x11,
         0x11,0x04,0x1f,0x0f,0x01,0x0f,0x00,0x00,0x04,0x1f,0x00,0x04,0x18,0x1f,0x00,0x06,
         0x00,0x00,0x04,0x00,0x0d,0x00,0x00,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x00,0x00,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x06,0x0f,0x00,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x00,0x00,
         0x00,0x00,0x00,0x00};

ui32 Long954[4] = {
  0xfff0fff0, 0xfff8fff8, 0xfffcfffc, 0xfffefffe};
ui32 Long938[4] = {
  0x7fff7fff, 0x3fff3fff, 0x1fff1fff, 0x0fff0fff};

void reverseBMP(ui32 *bmp, i32 n)
{
  ASSERT(((int)bmp & 3) == 0,"bmp");
  for (i32 i=0; i<n; i++)
    bmp[i]=LE32(bmp[i]);
}

void InitializeE()
{
  memset(&e, 0, sizeof (e));
  reverseBMP(FrameImage, 246);
}


void pumper(); // Pump any queued Windows messages
ui8 *MALLOC(i32 size);
void  MFREE(ui8 *address);
//i32  DIRECT_CONIN(); // Trap 1 #0x07
//i16  CONSTAT();      // Trap 1 #0x0b
void SetSupervisorMode();
void ClearSupervisorMode();
//void ClearMemory(pnt dest, i16 numByte); // TAG000a84
ui8 *physbase();
void SetDLogicalBase(ui8 *);
//i16  OPEN(char *name, i32 ref);
//i16  CREATE(char *name, i16 flag);
i32  READ(i32 file, i32 len, ui8 *buf);
i32  WRITE(i16 file, i32 len, ui8 *buf);
i16  CLOSE(i32 handle);
void StrCpy(char *dst, const char *src);//TAG003264
i16  StrLen(const char *string);//TAG0032ba
void MemMove(ui8 *src, ui8 *dest, i16 byteCount); //TAG0009dc
//void TextToScreen(i32, i32, i32, i32, pnt);//TAG001c42
i16  Unscramble(ui8 *buf, i32 initialChecksum, i32 numwordM1);
i16  GenChecksum(ui8 *buf, i16 initSum, i32 numword);//TAG01d076
//   TAG000c04
static i16  MyWriteScrambled(ui8 *, i16, ui16, ui16 *);//TAG01d0ea



//     TAG0000d6
ui32 Times(ui32, ui32);
void   TAG00022a(i16);
void   TAG0002b0();
void   TAG0002c2();
void   TAG0002d0();
void   TAG00031c();
ui8   *AllocateMemory(i32);//TAG000328
void   ReleaseMemory(ui8 *);//TAG0003a0
char  *GetSubString(const char *, char *, i16); //TAG000472
void   PrintDialogText(const char *, i32, i32, i32); //TAG0004ec
void   TAG00054a();
void   TAG0006e4();
i16    TAG0008dc(i16);
i16    TAG000952();
void   TAG0009a8();
i16    ReadGameFile(ui8 *, i32);//TAG000aee
i16    WriteGameFile(ui8 *, i16);//TAG000b28
//i16    TAG000b94(pnt,i16,i16);(use GenChecksum)
//     TAG000bc8
static i16 MyUnscrambleStream(ui8 *buf,i32 size,i16 initialHash,i16 Checksum);
i16    ReadAndChecksum(ui8 *buf, i16 *pChecksum, i32 size);//TAG000c52
static i16 MyWriteAndChecksum(ui8 *, ui16 *, i32);//TAG000c98
i16    ReadGameBlock1(ui8 *);//TAG000cde
i16    UnscrambleBlock1(ui8 *, i16);//TAG000d08
i16    WriteFirstBlock(ui8 *, i16);//TAG000d7c
void   TAG00136c();
i16    TAG00142e(ui8 *);
void   TAG001676(ui8 *, i16);
void   TAG002724();
i16    TestInRectangle(wordRectPos *, i32 x, i32 y);//TAG002c70
void   TAG002ca8();
i16    TAG002cdc(DlgButton *, i32, i32, i32);
void   EnqueMouseClick(i32, i32, i32);//TAG002d3c
void   CheckPendingMouseClick();//TAG002e2c
void   TAG002e4a(i32);
i16    TAG002faa(S12406 *P1);
//i16    TAG002faa(pnt);
//     TAG003000
S12406 **SetActiveDialog(S12406 **); // Actually an array of (6) &S12406
void   TAG003106(i16);
void   TAG0031a6(i16);
void   TAG00323c();
void   TAG00324c();
void   TAG003264();
void   TAG003294(i16, i16);
void   TAG0032b4(i16, i16);
void   BitBltSquareImage(pnt, const wordRectPos *, i16, i16); //TAG0033b6
//     TAG0033e6
void   FillScreenRectangle(wordRectPos *, i16);
void   DrawNameTitleCarret(i16, i16); //TAG003406
//     TAG0034dc
void   ExpandRectangle(wordRectPos *, wordRectPos *, i16, i16);
//     TAG0035cc
void   DrawDialogBoxOutline(wordRectPos *, i16, i16, i16);
void   PrintCharacterName(i16); //TAG003748
void   DrawCharacterHeader(i16, i16); //TAG0037ac
void   AddDialogButton(DlgButton *, const char*, i16);//TAG003860
void   TAG0039a8();
void   FillCharacterNameEditBox(const char *, i16, i16);//TAG0039ea
void   PrintAttributeValue(const char *, i32, i32); //TAG003a7c
void   DrawMagnifiedCharacterImage(); //TAG0036c0
void   GetPalleteItemRect(i16, wordRectPos *); //TAG0038ea
void   DrawSelectedColor(i16); //TAG003922
void   TAG00439e();
//void TAG004a32(char *); //FadeToPalette
//   TAG004bbc
void TextOutB(ui8 *dest,
              i32 destWidth,
              i32 x,
              i32 y,
              i32 color,
              i32 P6,
              const char *text);
void   TextToScreen(i32, i32, i32, i32, const char *);//TAG004e76
i16    TAG004ec0(i32);
void   TAG004ea2(i16 *);
i16    TAG00507e();
i16    TAG0050d2();
i16    TAG005868(i16 *, i16 *, i16 *, i16 *);
void   TAG00589e(i16, i16);
//void TAG005c50(i32); //Trap #14
void   TAG005c92(pnt);
void   TAG005cd6(pnt *);
void   TAG005cf6();
void   TAG005d12(i16 *, i16 *, i16 *);
void   TAG005d8e(i16 *, i16 *, i16 *);
void   TAG005df2(i16);
void   TAG0061ba(i16, i16);
void   TAG0061e2(i16);
void   TAG006202(i16, i16 *, i16 *, i16 *);
void   TAG00623a(i16, char *, char **);
void   TAG00626e(i16, char *, char **);
pnt    FormatInteger(pnt, ui32, i16);//TAG0064b4
//void   TAG006520(pnt,pnt);StrCpy
//i16    TAG00654e(pnt, pnt);strcmp
//i16    TAG0065bc(pnt);//StrLen
i32    atari_sprintf(char *,const char*, i32, i32, i32=0); //TAG0065e0
i32    GetPixel(pnt, i16, i16, i16);//TAG006700
//void   TAG006892(...) //Use TAG0088b2 from Graphics.cpp
//void   TAG00716a(pnt, RectPos *, i16 color, i16 dstwidth); (FillRectangle)
ui8   *TAG0073b0(ui8 *);
ui8   *TAG0073d4(ui8 *);
void   TAG007416(char *, char *);
void   TAG00744c(ui8 *, ui8 *);
void   TAG0074ae(char *, i32, char *);
void   TAG0074ea(ui8 *, i32, ui8 *);
ui8   *TAG007552(i32, ui8 *, i16 *);
i16    TAG007904(char * *, i32);
ui8   *TAG007a1a(i16);
char  *TAG007ac0(ui8 **);
void   TAG007b68(ui8 *);
char  *TAG007bba(STRUCT6 *);
ui8   *TAG007cc6(i32, i16, char *);
ui8   *TAG007d4c(STRUCT12 *);
void   TAG007e16(ui8 *, ui8 *);
i32    TAG007e42(i16, i16);
void   TAG007f54();
ui8   *TAG007f64(i32, i16);
i32    TAG007f92(ui8 *);
ui8   *TAG007fe8(ui8 *);
void   TAG00801e(ui8 *);
i32    TAG0080e0(ui8 *);
//     TAG00858c
void   MemoryMove(ui8 *, ui8 *, i16, i16, i32);//like MemMove
//void TAG0086a8(pnt, i32);//ClearMemory
i32    TAG008788(i16, i32, ui32);
void   EditCharacterName(char key);

void StorePnt(ui8 *addr, ui8 *data)
{
  //A little explanation is in order here, perhaps.
  //Some platforms require that 32-bit words be on 4-byte
  //boundaries.  In the runtime we moved all 32-bit words
  //to conform with this requirement.  But here we simply
  //fetch and store them by referencing two 16-bit words.
#ifdef _bigEndian
  xxxxx
#else
  *((ui16 *)(addr)) = (ui16)((ui32)data);
  *((ui16 *)(addr+2)) = (ui16)(((ui32)data)>>16);
#endif
}
void StorePnt(aReg addr, aReg data)
{
  StorePnt((ui8 *)addr, (ui8 *)data);
}

void StorePnt(aReg addr, ui8 *data)
{
  StorePnt((ui8 *)addr, data);
}


ui8 *LoadPnt(ui8 *addr)
{
  //A little explanation is in order here, perhaps.
  //Some platforms require that 32-bit words be on 4-byte
  //boundaries.  In the runtime we moved all 32-bit words
  //to conform with this requirement.  But here we simply
  //fetch and store them by referencing two 16-bit words.
#ifdef _bigEndian
  xxxxx
#else
  return (ui8 *)((*(ui16 *)(addr)) | ((ui32)((*(ui16 *)(addr+2)))<<16));
#endif
}
ui8 *LoadPnt(aReg addr)
{
  return LoadPnt((ui8 *)addr);
}

void StoreLong(ui8 *addr, i32 data)
{
  //A little explanation is in order here, perhaps.
  //Some platforms require that 32-bit words be on 4-byte
  //boundaries.  In the runtime we moved all 32-bit words
  //to conform with this requirement.  But here we simply
  //fetch and store them by referencing two 16-bit words.
#ifdef _bigEndian
  xxxxx
#else
  *((ui16 *)(addr)) = (ui16)((ui32)data);
  *((ui16 *)(addr+2)) = (ui16)(((ui32)data)>>16);
#endif
}

void StoreLong(aReg addr, i32 data)
{
  StoreLong((ui8 *)addr, data);
}

i32 LoadLong(ui8 *addr)
{
  //A little explanation is in order here, perhaps.
  //Some platforms require that 32-bit words be on 4-byte
  //boundaries.  In the runtime we moved all 32-bit words
  //to conform with this requirement.  But here we simply
  //fetch and store them by referencing two 16-bit words.
#ifdef _bigEndian
  xxxxx
#else
  return (i32)((*(ui16 *)(addr)) | ((ui32)((*(ui16 *)(addr+2)))<<16));
#endif
}

i32 LoadLong(aReg addr)
{
  return LoadLong((ui8 *)addr);
}

void StorePnt(ui8 *addr, aReg data)
{
  //A little explanation is in order here, perhaps.
  //Some platforms require that 32-bit words be on 4-byte
  //boundaries.  In the runtime we moved all 32-bit words
  //to conform with this requirement.  But here we simply
  //fetch and store them by referencing two 16-bit words.
#ifdef _bigEndian
  xxxxx
#else
  *((ui16 *)((pnt)addr)) = (ui16)((ui32)data);
  *((ui16 *)((pnt)addr+2)) = (ui16)(((ui32)data)>>16);
#endif
}

void Storepi16(i16 **addr, i16 *data)
{
  //A little explanation is in order here, perhaps.
  //Some platforms require that 32-bit words be on 4-byte
  //boundaries.  In the runtime we moved all 32-bit words
  //to conform with this requirement.  But here we simply
  //fetch and store them by referencing two 16-bit words.
#ifdef _bigEndian
  xxxxx
#else
  *((ui16 *)((pnt)addr)) = (ui16)((ui32)data);
  *((ui16 *)((pnt)addr+2)) = (ui16)(((ui32)data)>>16);
#endif
}

i16 *Loadpi16(i16 **addr)
{
  //A little explanation is in order here, perhaps.
  //Some platforms require that 32-bit words be on 4-byte
  //boundaries.  In the runtime we moved all 32-bit words
  //to conform with this requirement.  But here we simply
  //fetch and store them by referencing two 16-bit words.
#ifdef _bigEndian
  xxxxx
#else
  return (i16 *)((*(ui16 *)((pnt)addr)) | ((ui32)((*(ui16 *)((pnt)addr+2)))<<16));
#endif
}



// *********************************************************
//
// *********************************************************
//   TAG0000d6
ui32 Times(ui32 P1, ui32 P2)
{
  return P1 * P2;
}

// *********************************************************
//
// *********************************************************
void TAG00022a(i16 P1)
{ // Set/unSet VBL interrupt routine?????
//;;;;;;;;;;;;;;;;;;;;;
  switch (P1)
  {
  case 1:
  SetSupervisorMode();
  //TAG0002b0();
  ClearSupervisorMode();
    break;
  case 2:
    SetSupervisorMode();
    //TAG0002c2();
    ClearSupervisorMode();
    break;




  }; //switch(P1)
}


// *********************************************************
//
// *********************************************************
void TAG0002d0()
{
  i32 i;
//;;;;;;;;;;;;;
  e.Word23496 = 0;
  for (i=0; i<100; i++)
  {
    StorePnt((ui8 *)&e.Pnt23896[i], (ui8 *)NULL);
    StorePnt((ui8 *)&e.Pnt24296[i], (ui8 *)NULL);

//
  };
  TAG007e42(100, 50);
}

// *********************************************************
//
// *********************************************************
void TAG00031c()
{
  TAG007f54();
}

// *********************************************************
//
// *********************************************************
//  TAG000328
ui8 *AllocateMemory(i32 P1)
{
  dReg D0;
  ui8 *pnt_8;
  ui8 *pnt_4;
//;;;;;;;;;;;;;;;;;;;
  pnt_4 = TAG007f64(P1, 0x400);
  D0B = (UI8)(pnt_4==NULL?1:0);
  D0W = sw(-D0B);
  if (D0W != 0)
  {
    die(0,"FATAL ERROR: OUT OF MEMORY\nREBOOT");
    for (;;) {};
  };
  pnt_8 = TAG007fe8(pnt_4);
  StorePnt((ui8 *)&e.Pnt23896[e.Word23496], pnt_8);
  StorePnt((ui8 *)&e.Pnt24296[e.Word23496], pnt_4);
  e.Word23496++;
  return pnt_8;
}

// *********************************************************
//
// *********************************************************
//   TAG0003a0
void ReleaseMemory(ui8 *P1)
{
  ui8 **pntA0, **pntA1;
  i16  w_6;
  ui8 *pnt_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pnt_4 = NULL;
  for (w_6 = 0; w_6<e.Word23496; w_6++)
  {
    pntA0 = (ui8 **)&e.Pnt23896[w_6];
    if (P1 != *pntA0) continue;
    pntA0 = (ui8 **)&e.Pnt24296[w_6];
    pnt_4 = *pntA0;
    e.Word23496--;
    if (w_6 < e.Word23496)
    {
      pntA0 = (ui8 **)&e.Pnt23896[w_6];
      pntA1 = (ui8 **)&e.Pnt23896[e.Word23496];
      pntA0 = pntA1;
      pntA0 = (ui8 **)&e.Pnt24296[w_6];
      pntA1 = (ui8 **)&e.Pnt24296[e.Word23496];
      pntA0 = pntA1;
    };
    pntA0 = (ui8 **)&e.Pnt23896[e.Word23496];
    *pntA0 = NULL;
    pntA0 = (ui8 **)&e.Pnt24296[e.Word23496];
    *pntA0 = NULL;
//
//
  };
  if (pnt_4 == NULL) return;
  TAG00801e(pnt_4);
  TAG007f92((ui8 *)pnt_4);
}

// *********************************************************
//
// *********************************************************
char *GetSubString(const char *P1,char *P2,i16 P3)
{
  dReg D6, D7;
  aReg A0, A3, A4;
  const char *pnt_4;
//;;;;;;;;;
  A4 = (aReg)P1;
  A3 = (aReg)P2;
  D7W = P3;
  while ( (D6B = *(A4++)) != 0)
  {
    if (D6B == D7B)
    {
      *A3 = 0;
      return (char *)A4;
    };
    if (D6B == 64)
    {
      pnt_4 = TranslateLanguage(pnt12222);
      goto tag0004aa;
    };
    if (D6B != 35) goto tag0004be;
    pnt_4 = TranslateLanguage(pnt12226);
tag0004aa:
    A0 = A3++;
    *A0 = *(pnt_4++);
    if(*A0 != 0) goto tag0004aa;
    A3--;
    continue;
tag0004be:
    if (D6B == 36)
    {
      pnt_4 = (char *)LoadPnt((ui8 *)&e.Pnt24792);
      goto tag0004aa;
    };
    *(A3++) = D6B;
//
  };
  A4 = NULL;
  *A3 = 0;
  return (char *)A4;
}



// *********************************************************
//
// *********************************************************
void PrintDialogText(const char *P1,i32 P2,i32 P3,i32 P4)
{
  dReg D0, D1;
  char b_40[40];
//;;;;;;;;;;;;;;;;;;
  while (P1 != NULL)
  {
    P1 = GetSubString(P1, b_40, 10);
    D0W = StrLen(b_40);
    D0W = sw(6 * D0W / 2);
    D1W = sw(P2 - D0W);
    TextToScreen(D1W, P3, P4, 12, b_40);
    P3 += 13;
//
  };
}

// *********************************************************
//
// *********************************************************
void TAG00054a()
{
//;;;;;;;;;;;;;;
  if (e.Word23398 == 0) return;
  e.Word23398 = 0;
  TAG0088b2(e.Byte23394, //src
            LoadPnt((ui8 *)&e.PhysicalBase), //dest
            (RectPos *)&rectPos23406,   //dest position
            0,
            0,
            104,
            160,
            -1);
  if (e.Word23396 != 0) e.Word23396--;
  TAG003264();
}

// *********************************************************
//
// *********************************************************
//          TAG000588
RESTARTABLE _UtilityDialogBox(const char *P1, const i32 p2, const char *p3, S12406 *P4)
{ //(i16)
  // Generic Diaog Box??
  static dReg D4, D5, D6;
  static DlgButton *sA4;
  static char b_78[50];
  static S12406 *pS12406_28[6];
  static S12406 **ppS12406_4;
  static i32 P2;
  static const char *P3;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
  END_RESTARTMAP
  P2 = p2;
  P3 = p3;
//;;;;;;;;;;;;;;;;;;;;
  ClearMemory((ui8 *)pS12406_28, 24);
  pS12406_28[0] = P4;
  D4W = uw(P2 & 0x8000);
  P2 &= 0x7fff;
  if (e.Word23396 == 0)
  {
    e.Word23396++;
    TAG00324c();
  };
  if (e.Word23398 == 0)
  {
    e.Word23398 = 1;
    TAG0088b2(LoadPnt((ui8 *)&e.PhysicalBase), //src
              (ui8 *)e.Byte23394,   //dst
              (RectPos *)&rectPos23414,  //dstPos
              62,             //srcOffsetX
              48,             //srcOffsetY
              160,            //srcByteWidth
              104,            //dstByteWidth
              -1);            //P8
  };
  DrawDialogBoxOutline(&rectPos23406, -2, 15, 12);
  PrintDialogText(P1, 159, 65, P2);
  if (P4 == 0) RETURN_i16(0);
  for (D5L=0,sA4=P4->pDlgButton2;
       (sA4->byte0!=0)&&(P3!=0);
       D5W++, sA4 ++)
  {
    P3 = GetSubString(P3, b_78, 10);
    AddDialogButton(sA4, b_78, 2);
//
//
  };
  ppS12406_4 = SetActiveDialog(pS12406_28);// save old value
  TAG003264();
  do
  {
    TAG00301c(_1_,e.Byte12336);
    D6W = i16Result;
  } while (D6W == 0);
  TAG00324c();
  if (D4W != 0)
  {
    if ( (D6W & 255) <= D5W )
    {
      InvertButton(_2_, &P4->pDlgButton2[D6W-1].rectPos2);
    };
  }
  else
  {
    TAG00054a();
  };
  SetActiveDialog(ppS12406_4);
  RETURN_i16(D6W);
}

void TAG0006e4()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;
  SetSupervisorMode();
  // SetCriticalErrorHandler();
  ClearSupervisorMode();
  e.Word12196 = 1;
}

// *********************************************************
//
// *********************************************************
i16 TAG000952()
{
//;;;;;;;;;;;;


  TAG0008dc(0);
  return 1;
}

// *********************************************************
//
// *********************************************************
void TAG0009a8()
{
  DontDoThis(0x9a8);
}

// *********************************************************
//
// *********************************************************
//  TAG000aee
i16 ReadGameFile(ui8 *buf, i32 len)
{
  dReg D0, D7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7L = len;
  if (D7L == 0) return 1;
  D0L = READ(e.GameFile, D7L, buf);
  if (D7L == D0L) return 1;
  return 0;
}

// *********************************************************
//
// *********************************************************
//  TAG000b28
i16 WriteGameFile(ui8 *buf, i16 size)
{
  dReg D0, D7;
//;;;;;;;;;;;;;;;;;;;
  D7L = size;
  if (D7L == 0) return 1;
  D0L = WRITE(e.GameFile, D7L, buf);
  if (D0L == D7L) return 1;
  return 0;
}

// *********************************************************
//
// *********************************************************
i16 TAG000b94(pnt buf, i16 initSum, i16 numword)
{
  return GenChecksum((ui8 *)buf, initSum, numword);
}

// *********************************************************
//
// *********************************************************
//    TAG000bc8
static i16 MyUnscrambleStream(ui8 *buf,
                            i32 size,
                            i16 initialHash,
                            i16 Checksum)
{
  dReg D0;
//;;;;
  D0W = ReadGameFile(buf, size);
  if (D0W == 0) return 0;
  D0W = Unscramble(buf, initialHash, size/2);
  if (D0W != Checksum) return 0;
  return 1;
}


// *********************************************************
//
// *********************************************************
//  TAG01d0ea
i16 MyWriteScrambled(ui8 *buf,
              i16 byteCount,
              ui16 initialHash,
              ui16 *checksum)
{
  dReg D0, D6, D7;
  aReg A3;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A3 = (aReg)buf;
  D6W = sw(byteCount/2);
  D0W = Unscramble((ui8 *)A3, initialHash, D6W);
  *checksum = D0W;
  D7W = WriteGameFile((ui8 *)A3, byteCount);
  D0W = Unscramble((ui8 *)A3, initialHash, D6W);
  return D7W;
}


// *********************************************************
//
// *********************************************************
//  TAG000c52
i16 ReadAndChecksum(ui8 *buf, i16 *pChecksum, i32 size)
{
  dReg D0, D6;
  //aReg A4;
  //i16 *wA3;
//;;;;;;;
  //A4 = P1;
  //wA3 = P2;
  //D7W = sw(P3);
  D6W = 0;
  D0W = ReadGameFile(buf, size);
  if (D0W == 0) return 0;
  while ((size--) != 0)
  {
    D6W = (UI16)(D6W + ub(*(buf++)));
  };
  *pChecksum = (UI16)(*pChecksum + D6W);
  return 1;
}

// *********************************************************
//
// *********************************************************
//  TAG000c98
i16 MyWriteAndChecksum(ui8 * P1, ui16 *P2, i32 P3)
{
  dReg D0, D6, D7;
  aReg A4;
  ui16 *uwA3;
//;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = (aReg)P1;
  uwA3 = P2;
  D7W = sw(P3);
  D6W = 0;
  D0W = WriteGameFile((ui8 *)A4, D7W);
  if (D0W == 0) return 0;
  while (D7W != 0)
  {
    D6W = (UI16)(D6W + ub(*(A4++)));

    D7W--;
  };
  *uwA3 = (UI16)(*uwA3 + D6W);
  return 1;
}

// *********************************************************
//
// *********************************************************
//  TAG000cde
i16 ReadGameBlock1(ui8*P1)
{
  dReg D0;
  aReg A4;
//;;;;;;;;;;;;;;;;;;;;;;
  A4 = (aReg)P1;
  D0W = ReadGameFile((ui8 *)A4, 512);
  if (D0W == 0) return 0;
  return 1;
}

// *********************************************************
//
// *********************************************************
//  TAG000d08
i16 UnscrambleBlock1(ui8 *P1,i16 P2)
{
  dReg D5, D6, D7;
  pnt A0, A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = (aReg)P1;
  D7L = 32;
  D6L = 0;
  do
  {
    D6W = (UI16)(D6W + LE16(wordGear(A4)));
    D6W ^= LE16(wordGear(A4+2));
    D6W = (UI16)(D6W - LE16(wordGear(A4+4)));
    D6W = (UI16)(D6W ^ LE16(wordGear(A4+6)));
    A4 += 8;
    D7W--;
  } while (D7W != 0);
  Unscramble((ui8 *)A4, LE16(wordGear(P1 + 2*P2)), 128);
  D5W = 0;
  D7W = 128;
  do
  {
  A0 = A4;
  A4 += 2;
  D5W = (UI16)(D5W + LE16(wordGear(A0)));
  D7W--;
  } while (D7W != 0);
  if (D5W == D6W) return 1;
  return 0;
}

// *********************************************************
//
// *********************************************************
//  TAG000d7c
i16 WriteFirstBlock(ui8 *buf, i16 offset)
{
  dReg D5, D6, D7;
  aReg A0, A4;
//;;;;;;;;;;;;;;;;;
  D5W = 0;
  D7W = 128;
  A4 = (aReg)buf+256;
  do
  {
    D5W = (I16)(D5W + LE16(wordGear(A4))); // Second half-block checksum
    A4 += 2;
    D7W--;
  } while (D7W != 0);
  D7W = 32;
  D6W = 0;
  A4 = (aReg)buf;
  for (;;)
  { // 32 times 4 random 16-bit words = 256 bytes.
    // Except we skip the last word in the block.
    wordGear(A4) = 0; //(i16)STRandom();
    D6W = (UI16)(D6W + LE16(wordGear(A4)));
    A4 += 2;
    wordGear(A4) = 0; //(i16)STRandom();
    D6W ^= LE16(wordGear(A4));
    A4 += 2;
    wordGear(A4) = 0; //(i16)STRandom();
    D6W = (UI16)(D6W - LE16(wordGear(A4)));
    A4 += 2;
    D7W--;
    if (D7W == 0) break;
    wordGear(A4) = 0; //(i16)STRandom();
    D6W ^= LE16(wordGear(A4));
    A4 += 2;
  } ;
  wordGear(A4) = LE16((ui16)(D6W ^ D5W));
  A4 += 2;
  A0 = (aReg)buf + 2*offset;
  Unscramble((ui8 *)A4, LE16(wordGear(A0)), 128);
  D7W = WriteGameFile(buf, 512);
  A0 = (aReg)buf + 2*offset;
  Unscramble((ui8 *)A4, LE16(wordGear(A0)), 128);
  return D7W;

}

// *********************************************************
//
// *********************************************************
i16 TAG0008dc(i16)
{
  return 1;
}

// *********************************************************
//
// *********************************************************
//  TAG000e64
RESTARTABLE _CheckChampionNames()
{//(i16)
  static dReg D0, D6, D7;
  static aReg A0, A4;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;
  for (D7L=0; D7W<e.Word24814; D7W++)
  {
    A4 = (aReg)LoadPnt((ui8 *)&e.Pnt24812) + 800*D7W + 772;
    if (A4[0] == 0)
    {
tag000e84:
      UtilityDialogBox(_1_,UtilityDiskMessages[20],8,UtilityDiskMessages[32],s12406);
      RETURN_i16(0);
    };
    for (D6W=sw(D7W+1); D6W<e.Word24814; D6W++)
    {
      A0 = (aReg)LoadPnt((ui8 *)&e.Pnt24812) + 800*D6W +772;
      D0W = sw(strcmp((char *)A4, (char *)A0));
      if (D0W == 0) goto tag000e84;
//
//
    };
//
  };
  RETURN_i16(1);
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG000ede(const i32 P1, char *P2)
{//(i16)
  static dReg D0;
  static aReg A0;
  static i16 w_2;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
    RESTART(4)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  do
  {
    for (;;)
    {
      if (P1 == 0)
      {
        UtilityDialogBox(_1_,
                  UtilityDiskMessages[2],
                  (unsigned)0x800f,
                  UtilityDiskMessages[35],
                  &s12406[3]);
        D0W = i16Result;
        w_2 = D0W;
      }
      else
      {
        if (P1 == 1)
        {
          A0 = (aReg)UtilityDiskMessages[0];
        }
        else
        {
          A0 = (aReg)UtilityDiskMessages[1];
        };
        UtilityDialogBox(_2_,
                  A0,
                  (unsigned)0x800f,
                  UtilityDiskMessages[34],
                  &s12406[2]);
        D0W = i16Result;
        w_2 = D0W;
      };
      if (w_2 == 3)
      {
        RETURN_i16(0);
      };
      D0W = TAG0008dc(1);

      if (w_2 != 2) break;
      TAG0009a8();
    } ;
	// ask to which game we want to write
	// inserted by LK
    UtilityDialogBox(_3_,
			UtilityDiskMessages[42],
			(unsigned)0x800f,
			positions,
			&fourbuttons[0]);

    strcpy(P2, filenames[i16Result - 1]);

	// end of insertion
    D0W = OPEN(P2, "rb");
    if (D0W < 0) break;
    if (P1 <= 1)
    {
      A0 = (aReg)UtilityDiskMessages[6];
    }
    else
    {
      A0 = (aReg)UtilityDiskMessages[8];
    };

    UtilityDialogBox(_4_,
              A0,
              (unsigned)0x8008,
              UtilityDiskMessages[36],
              &s12406[1]);
    D0W = i16Result;
  } while (D0W != 1);
  RETURN_i16(1);
}

/*
// *********************************************
static ui32 WCSRCSchecksum;

static ui32 chaosWCS()
{
  ui32 temp;
  temp = WCSRCSchecksum;
  WCSRCSchecksum = 0xffff;
  return temp;
}
*/
/*
static ui32 RCS(i32 handle)
{
  ui32 temp;
  WCSRCShandle = handle;
  temp = WCSRCSchecksum;
  WCSRCSchecksum = 0xffff;
  return temp;
}
*/
/*
void chaosWCS(ui8 *buf, i32 n)
{
  i32 i;
  for (i=0; i<n; i++)
  {
    WCSRCSchecksum *= (ui32)0xbb40e62d;
    WCSRCSchecksum += 11 + buf[i];
  };
  WriteGameFile((char *)buf, (ui16)n);
}
*/
/*

static void RCS(ui8 *buf, i32 n)
{
  i32 i;
  READ(WCSRCShandle, n, (char *)buf);
  for (i=0; i<n; i++)
  {
    WCSRCSchecksum *= (ui32)0xbb40e62d;
    WCSRCSchecksum += 11 + buf[i];
  };
}

static void WCS(ui32 *i, i32 n)
{
  WCS((ui8 *)i, 4*n);
}

static void RCS(ui32 *i, i32 n)
{
  RCS((ui8 *)i, 4*n);
}
*/
/*
void chaosWCS(i32 *i, i32 n)
{
  chaosWCS((ui8 *)i, 4*n);
}
/*
static void RCS(i32 *i, i32 n)
{
  RCS((ui8 *)i, 4*n);
}

static void WCS(ui16 *i, i32 n)
{
  WCS((ui8 *)i, 2*n);
}

static void RCS(ui16 *i, i32 n)
{
  RCS((ui8 *)i, 2*n);
}

static void WCS(char *c, i32 n)
{
  WCS((ui8 *)c, n);
}

static void RCS(char *c, i32 n)
{
  RCS((ui8 *)c, n);
}
*/
// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG000fc4(const i32 P1)
{//()
  static dReg D0, D1, D3, D7;
  static ui8 *pD0, *pD1, *pD2;
  static aReg A0, A1, A4;
  static ui16  *uwA3;
  static ui8 *pnt_116;
  static ui8 *pnt_112;
  static ui8 *pnt_108;
  static ui8 *pnt_104;
  static i8   b_100[96];
  static ui16 uw_4;
  static ui16 uw_2;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
    RESTART(4)
  END_RESTARTMAP
//;;;This is only function that calls 'MyWriteScrambled'
//;;; and this function is called from two places
  A4 = (aReg)LoadPnt((ui8 *)&e.Pnt24812) + 4112*P1;
  if (LoadPnt((ui8 *)&e.Pnt24812) == NULL) RETURN;
  if (e.Word24814 == 0) RETURN;
  CheckChampionNames(_1_);
  D0W = i16Result;
  if (D0W == 0) RETURN;
  TAG000ede(_2_,1-P1, A4+4);
  D0W = i16Result;
  if (D0W != 0)
  {
    if (P1 != 0)
    {
      A0 = (aReg)UtilityDiskMessages[23];
    }
    else
    {
      A0 = (aReg)UtilityDiskMessages[10];
    };
    UtilityDialogBox(_3_,A0, 15, 0, 0);
    e.GameFile = CREATE((char *)A4+4, "wb", true);
    if (e.GameFile < 0) goto tag001328;
    ClearMemory((ui8 *)b_100, 96);
    StorePnt(b_100, A4+550);
    wordGear(b_100+4) = 128;
    StorePnt(b_100+6, LoadPnt(A4+4100));
    wordGear(b_100+10) = sw(16 * LE16(wordGear(A4+596)));
    StorePnt(b_100+12, A4+772);
    wordGear(b_100+16) = 3328;
    StorePnt(b_100+18, LoadPnt(A4+4104));
    {
      int timerSize;
      //timerSize = sequencedTimers?sizeof(TIMER):10;
      timerSize = sequencedTimers?12:10;
      timerSize = extendedTimers?16:timerSize;
      wordGear(b_100+22) = sw(timerSize * LE16(wordGear(A4+578)));
    };
    StoreLong(b_100+24, LoadLong(A4+4108));
    wordGear(b_100+28) = sw(2 * LE16(wordGear(A4+578)));
    pnt_104 = (ui8 *)A4 + 38;
    if (wordGear(A4+2) == 2)
    {
      pnt_108 = pnt_104 + 344;
      pnt_112 = pnt_104 + 312;
      pnt_104[300] = 2;
    }
    else
    {
      pnt_108 = (ui8 *)A4 + 380;
      pnt_112 = (ui8 *)A4 + 348;
      A4[336] = 2;
    };
    for (D7W=0; D7W<16; D7W++)
    {
      pnt_116 = pnt_112 + 2*D7W;
      D0W = 0; //(i16)STRandom();
      wordGear(pnt_116) = D0W;
//
    };
    for (D7L=0; D7W<5; D7W++)
    {
      pD0 = pnt_108 + 2*D7W;
      pD1 = (ui8 *)b_100 + 6*D7W;
      pD2 = pnt_112 + 2*D7W;
      A0 = b_100 + 6*D7W;
      D3W = wordGear(A0+4); //Random number generated above
      pnt_116 = pD0;
      D3W /= 2;
      D0W = GenChecksum(LoadPnt(pD1), //buffer
                         LE16(wordGear(pD2)),//initial hash
                         D3W);        //# words
      wordGear(pnt_116) = LE16(D0W);
//
//
    };
    if (ExtendedFeaturesVersion != '@')
    {
      EXTENDEDFEATURESBLOCK efb;
      memset(&efb,0,512);
      strcpy(efb.sentinel, " Extended Features ");
      efb.dataMapLength = dataMapLength;
      efb.version = ExtendedFeaturesVersion;
      efb.dataIndexMapChecksum = FormChecksum((ui8 *)dataIndexMap,2*dataMapLength);
      efb.dataTypeMapChecksum = FormChecksum(dataTypeMap,dataMapLength);
      efb.numDSA = countDSAs();
      efb.flags |= EXTENDEDFEATURESBLOCK::BigActuators;
      if (indirectText)
      {
        efb.flags |= EXTENDEDFEATURESBLOCK::IndirectText;
      };
      if (sequencedTimers)
      {
        efb.extendedFlags |= EXTENDEDFEATURESBLOCK::SequencedTimers;
      };
      if (extendedTimers)
      {
        efb.extendedFlags |= EXTENDEDFEATURESBLOCK::ExtendedTimers;
      };
      if (DefaultDirectXOption)
      {
        efb.extendedFlags |= EXTENDEDFEATURESBLOCK::DefaultDirectX;
      };
      if (efb.numDSA != 0)
      {
        efb.flags |= EXTENDEDFEATURESBLOCK::LevelDSAInfoPresent;
      };
      efb.extendedFeaturesChecksum = 0;
      efb.extendedFeaturesChecksum = FormChecksum((ui8 *)&efb,512);
      WriteGameFile((ui8 *)&efb, 512);
      WriteGameFile(dataTypeMap, (ui16)dataMapLength);
      WriteGameFile((ui8 *)dataIndexMap, (ui16)(2*dataMapLength));
      if (efb.numDSA != 0)
      {
        WriteDSAs(e.GameFile);
        WriteDSALevelIndex(e.GameFile);
//{
        //i32 i;
        //ui32 checksum;
        //WCS(e.GameFile); //Discard old checksum and start new.
        //for (i=0; i<256; i++)
        //{
        //  if (DSAIndex.GetDSA(i) == NULL) continue;
        //  WCS(&i,1);
        //  DSAIndex.GetDSA(i)->Write();
        //};
        //checksum = WCS(0);
        //WriteGameFile((char *)&checksum, 4);
//}

      };
    };
    D0W = WriteFirstBlock((ui8 *)A4+38, wordGear(A4+36));
    if (D0W == 0) goto tag001328;
    for (D7W=0; D7W<5; D7W++)
    {
      D0W = MyWriteScrambled(LoadPnt(b_100 + 6*D7W),
                            wordGear(b_100 + 6*D7W + 4),
                            LE16(wordGear(pnt_112+2*D7W)),
                            &uw_2);
      if (D0W == 0) goto tag001328;
      A0 = (aReg)pnt_108 + 2*D7W;
      if (uw_2 == LE16(wordGear(A0))) continue;
      pD0 = pnt_108 + 2*D7W;
      pD1 = pnt_112 + 2*D7W;
      atari_sprintf(e.Byte23494,
                    "SAVEGAME():CHECKSUM %d != %d (KEY = %d)\n",
                    LE16(wordGear(pD0)),
                    uw_2,
                    LE16(wordGear(pD1)) );
      UI_MessageBox(e.Byte23494, NULL, MESSAGE_OK);
//
//

    };
    uwA3 = &uw_4;
    uw_4 = 0;
    D0W = MyWriteAndChecksum(LoadPnt(A4+690), uwA3, 44);
    if (D0W == 0) goto tag001328;

    D0W = sw(16 * (UI8)(LoadPnt(A4+690)[4]));
    D0W = MyWriteAndChecksum(LoadPnt(A4+686),uwA3, D0W); // levelDescriptors
    if (D0W == 0) goto tag001328;

    D0W = MyWriteAndChecksum(LoadPnt(A4+694), uwA3, 2*wordGear(A4+702)); //objectListIndex
    if (D0W == 0) goto tag001328;

    D0W = sw(2 * LE16(wordGear(LoadPnt(A4+690)+10)));
    D0W = MyWriteAndChecksum(LoadPnt(A4+704), uwA3, D0W);  // objectList
    if (D0W == 0) goto tag001328;

    D0W = sw(2 * LE16(wordGear(LoadPnt(A4+690)+6)));
    if (indirectText) D0W *= 2;
    D0W = MyWriteAndChecksum(LoadPnt(A4+678), uwA3, D0W);  // indirectTextIndex
    if (D0W == 0) goto tag001328;
    if (indirectText)
    {
      D0W = MyWriteAndChecksum((ui8 *)&compressedTextSize, uwA3, 4);
      if (D0W == 0) goto tag001328;
      D0W = MyWriteAndChecksum((ui8 *)compressedText, uwA3, 2*compressedTextSize);
      if (D0W == 0) goto tag001328;
    };

    {
      int dbNum;
      for (dbNum=0; dbNum<16; dbNum++)
      {
        ui16 entrySize;
        pD0 = (ui8 *)A4 + 4*dbNum;
        A0 = &byte24830[dbNum];
        entrySize = *A0;
        if ( (dbNum == dbSCROLL) && (ExtendedFeaturesVersion >= 'B'))
        {
          entrySize = 6;
        };
        if ( (dbNum == dbACTUATOR) && bigActuators)
        {
          entrySize = 10;
        };
        if (dbNum == dbEXPOOL)
        {
          entrySize = (ui16)sizeof(DB11);
        };
        A1 = (aReg)LoadPnt(A4+690) + 2*dbNum;
        D1W = sw(entrySize * LE16(wordGear(A1+12))); //# entries
        D0W = MyWriteAndChecksum(LoadPnt(pD0+708), uwA3, D1W);
        if (D0W == 0) goto tag001328;
//
//
      };
    };
    D0W = MyWriteAndChecksum(LoadPnt(A4+682),
                            uwA3,
                            LE16(wordGear(LoadPnt(A4+690)+2)));
    if (D0W == 0) goto tag001328;
    *uwA3 = LE16(*uwA3);
    WriteGameFile((ui8 *)uwA3, 2);
    if (D0W == 0)
    {
tag001328:
      if (P1 != 0)
      {
        A0 = (aReg)UtilityDiskMessages[28];
      }
      else
      {
        A0 = (aReg)UtilityDiskMessages[15];
      };
      UtilityDialogBox(_4_,A0, 8, UtilityDiskMessages[32], &s12406[0]);
    };
    CLOSE(e.GameFile);
    e.GameFile=-1;
  };
  TAG000952();
  TAG00054a();
  RETURN;
}

// *********************************************************
//
// *********************************************************
void TAG00136c()
{
  dReg D0;
  aReg A0;
  pnt pnt_8;
  char *pnt_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;
  pnt_4 = (char *)LoadPnt((ui8 *)&e.Pnt24812);
  e.Word24808 = (unsigned)0x91a7;
  A0 = (aReg)pnt_4 + 38;
  pnt_8 = A0;
  if (wordGear(pnt_4+2) == 2)
  {
    A0 = pnt_8;
    e.Word24806 = wordGear(A0+378);
    D0W = (UI8)(pnt_8[301]);
    if (D0W == 1)
    {
      e.Word24804 = 1;
      e.Word24806 = 10;
    }
    else
    {
      A0 = pnt_8;
      e.Word24804 = wordGear(A0+376);
      e.Word24806 = wordGear(A0+378);
    };
  }
  else
  {
    A0 = (aReg)pnt_4;
    e.Word24806 = wordGear(A0+414);
    if (A0[337] == 1)
    {
      e.Word24804 = 1;
      e.Word24806 = 10;
    }
    else
    {
      A0 = (aReg)pnt_4;
      e.Word24804 = wordGear(A0+412);
      e.Word24806 = wordGear(A0+414);
    };
  };
  e.Word24802 = 1;
  e.Word24800 = 1;
  e.Word24798 = 0;
  e.Word24796 = 0;
  e.Word24794 = 0;
  // never referenced e.Byte24792 = 0;
}




// *********************************************************
//
// *********************************************************
i16 TAG00142e(pnt P1)
{
  dReg D0, D1, D4, D5, D6, D7;
  aReg A0, A4;
  pnt pnt_8;
  i16 w_4;
  i16 w_2;
//;;;;;;;;;;;;;;;;;;;;
  A4 = P1;
  w_4 = 0;
  A0 = (aReg)AllocateMemory(44);
  StorePnt(A4+690, A0);
  D0W = ReadAndChecksum(LoadPnt(A4+690), &w_4, 44);   // d.dungeonDatIndex
  if (D0W == 0) return 0;
  D7W = (UI8)(*(LoadPnt(A4+690)+4));//Number of levels
  StorePnt(A4+686, AllocateMemory(16*D7W)); //For level descriptors
  D0W = ReadAndChecksum(LoadPnt(A4+686), &w_4, 16*D7W); //Level descriptors
  if (D0W == 0) return 0;
  StorePnt(A4+698, AllocateMemory(2*D7W));
  for (D4L=D5L=0; D5W<D7W; D5W++)
  { //For each level
    A0 = (aReg)LoadPnt(A4+698)+2*D5W;
    wordGear(A0) = LE16(D4W);
    A0 = (aReg)LoadPnt(A4+686) + 16*D5W;
    D0W = sw((LE16(wordGear(A0+8)) & 0x07c0)/64 + 1);
    D4W = sw(D4W + D0W);
//
//
  };
  wordGear(A4+702) = D4W;
  A0 = (aReg)LoadPnt(A4+690);
  D5W = LE16(wordGear(A0+10));

  StorePnt(A4+694, AllocateMemory(2*D4W));  // d.objectListIndex
  A0 = (aReg)LoadPnt(A4+690);
  D0W = LE16(wordGear(A0+10));

  StorePnt(A4+704, AllocateMemory(2*D0W));  // d.objectList
  A0 = (aReg)LoadPnt(A4+690);
  D0W = LE16(wordGear(A0+6));
  if (indirectText) D0W = (i16)(2 * D0W);
  StorePnt(A4+678, AllocateMemory(2*D0UW)); // d.indirectTextArray


  D0W = ReadAndChecksum(LoadPnt(A4+694), &w_4, 2*D4W); // d.objectListIndex
  if (D0W == 0) return 0;
  D0W = ReadAndChecksum(LoadPnt(A4+704), &w_4, 2*D5W); // d.objectList
  if (D0W == 0) return 0;

  A0 = (aReg)LoadPnt(A4+690);
  D0W = LE16(wordGear(A0+6));  // numberOfEntriesInTextArray
  if (indirectText) D0W = (i16)(2*D0W);
  D0W = ReadAndChecksum(LoadPnt(A4+678), &w_4, 2*D0W);  // d.indirectTextIndex
  textArraySize = D0W; //Number of words
  textArray = (char *)LoadPnt(A4+678);
  if (D0W == 0) return 0;
  if (indirectText)
  {
  //If the indirectText flag is set then read
  // 1) a 32-bit word giving the size of text
  // 2) the text itself;
    D0W = ReadAndChecksum((ui8 *)&compressedTextSize, &w_4, 4);  // d.sizeOfCompressedText
    if (D0W == 0) return 0;
    compressedText = (char *)malloc(2*compressedTextSize);
    D0W = ReadAndChecksum((ui8 *)compressedText, &w_4, 2*compressedTextSize);  // d.compressedText
    if (D0W == 0) return 0;
  };
  for (D6W=0; D6W<16; D6W++)
  {
    A0 = (aReg)LoadPnt(A4+690) + 2*D6W;
    D5W = LE16(wordGear(A0+12));
    A0 = byte24830 + D6W; // DB sizes?
    D4W = (UI8)(*A0);
    if ( (D6W==dbSCROLL) && (ExtendedFeaturesVersion>='B') )
    {
      D4W = 6;
    };
    if ( (D6W == dbACTUATOR) && bigActuators)
    {
      D4W = 10;
    };
    if (D6W==dbEXPOOL)
    {
      D4W = sizeof(DB11);
    };
    pnt_8 = A4 + 4*D6W;
    A0 = (aReg)LoadPnt(A4+690) + 2*D6W;
    D1L = D4UW * LE16(wordGear(A0+12));
    if (D6W == 2)
    {
      DB2Size = LE16(wordGear(A0+12));
    };
    A0 = pnt_8;
    StorePnt(A0+708, AllocateMemory(D1L));
    if (D6W == 2)
    {
      DB2Address = (DB2 *)LoadPnt(A0+708);
    };
    D0W = ReadAndChecksum(LoadPnt(A4+4*D6W+708), &w_4, D4UW*D5UW);
    if (D0W == 0) return 0;
//
    //if (D6W == 2) ReadExtendedFeatures();
  };
  A0 = (aReg)LoadPnt(A4+690);
  {
    ui32 cellArraySize;
    cellArraySize = LE16(wordGear(A0+2));
    if (cellflagArraySize != 0) cellArraySize = cellflagArraySize;
    StorePnt(A4+682, AllocateMemory(cellArraySize));
    D0W = ReadAndChecksum(LoadPnt(A4+682), &w_4, cellArraySize);
    if (D0W ==0) return 0;
  };
  D0W = ReadGameFile((ui8 *)&w_2, 2);//Final checksum
  if (D0W == 0) return 1;
  w_2 = LE16(w_2);
  if (w_2 == w_4) return 1;
  return 0;
}

// *********************************************************
//
// *********************************************************
void TAG001676(ui8 *P1, i16 P2)
{
  dReg D7;
  aReg A0, A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = (aReg)P1;
  if (LoadLong(A4+4100) != 0)
  {
    ReleaseMemory(LoadPnt(A4+4100));
  };
  StoreLong(A4+4100, 0);
  if (LoadLong(A4+4104) != 0)
  {
    ReleaseMemory(LoadPnt(A4+4104));
  };
  StorePnt(A4+4104, (ui8 *)NULL);
  if (LoadPnt(A4+4108) != NULL)
  {
    ReleaseMemory(LoadPnt(A4+4108));
  };
  StorePnt(A4+4108, (ui8 *)NULL);
  if (LoadPnt(A4+678) != NULL) ReleaseMemory(LoadPnt(A4+678));
  StorePnt(A4+678, (ui8 *)NULL);
  if (LoadPnt(A4+682) != NULL) ReleaseMemory(LoadPnt(A4+682));
  StorePnt(A4+682, (ui8 *)NULL);
  if (LoadPnt(A4+686) != NULL) ReleaseMemory(LoadPnt(A4+686));
  StorePnt(A4+686, (ui8 *)NULL);
  if (LoadPnt(A4+690) != NULL) ReleaseMemory(LoadPnt(A4+690));
  StorePnt(A4+690, (ui8 *)NULL);
  if (LoadPnt(A4+694) != NULL) ReleaseMemory(LoadPnt(A4+694));
  StorePnt(A4+694, (ui8 *)NULL);
  if (LoadPnt(A4+698) != NULL) ReleaseMemory(LoadPnt(A4+698));
  StorePnt(A4+698, (ui8 *)NULL);
  if (LoadPnt(A4+704) != NULL) ReleaseMemory(LoadPnt(A4+704));
  StorePnt(A4+704, (ui8 *)NULL);
  for (D7W=0; D7W<16; D7W++)
  {
    A0 = A4 + 708 + 4*D7W;
    if (LoadPnt(A0) != NULL) ReleaseMemory(LoadPnt(A0));
    StorePnt(A0, (ui8 *)NULL);
//
//
  };
  if (P2 == 0) e.Word24814 = 0;
  wordGear(A4) = 0;
}


// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG00179c(const i32 P1)
{//(i16)
  static dReg D0, D5, D6, D7;
  static aReg A0;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
  END_RESTARTMAP
//;;;;;;;;;;;;;;
  D7W = uw(P1);// (0x8001 for example)
  if (D7W == 2)
  {
    DontDoThis(0x17ae);


    D6W = 1;
  }
  else
  {
    D5L = 1;
    while (D7W & 0x8000)
    {
      D7W &= 0x7fff;
      if (D5W == 0)
      {
        UtilityDialogBox(_1_,
                         UtilityDiskMessages[27],
                         (unsigned)0x8008,
                         UtilityDiskMessages[32],
                         &s12406[0]);
      };
      D5L = 0;
      if (D7W == 0)
      {
        UtilityDialogBox(_2_,
                  UtilityDiskMessages[0],
                  (unsigned)0x800f,
                  UtilityDiskMessages[32],
                  &s12406[0]);
        D6W = i16Result;
      }
      else
      {
        if (D7W == 1)
        {
          A0 = (aReg)UtilityDiskMessages[42];
        }
        else
        {
          A0 = (aReg)UtilityDiskMessages[1];
        };
       UtilityDialogBox(_3_,
                    	  A0,
		                    (unsigned)0x800f,
                        positions,
                        &fourbuttons[0]);
        D6W = i16Result;
        // we dont need this coz there is 4 buttons with four saves, no cancel
		//if (D6W == 2) RETURN_i16(0);

      };
//
    };
    D0W = TAG0008dc(1);
  };
  RETURN_i16(D6W);
}

// *********************************************************
//
// *********************************************************
//  TAG001868
RESTARTABLE _ReadSavedGame(const i32 P1)
{//(i16)
  //P1 non-zero means read MINI.DAT
  static dReg D0, D7;
  static aReg A0, A1, A4;
  static ui8 *pnt_4654;
  static const char* pnt_4650;
  static ui8 *pnt_4646;
  static ui8 *pnt_4642;
  static i8  b_4638[512];
  static ui8 *pnt_4126;
  static i16 w_4122;
  static i16 w_4120;
  static i8  b_4118[4112];
  static i16 w_6;
  static i16 w_4;
  static i16 w_2;
  static char *savegame;

  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
    RESTART(4)
    RESTART(5)
    RESTART(6)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;Reads a gamefile????
  D7W = sw(P1);
  A4 = b_4118;
  ClearMemory((ui8 *)A4, 4112);
  pnt_4126 = (ui8 *)A4 + 38;
  w_6 = 1;
  do
  {
    if (D7W != 0)
    {//We are supposed to read MINI.DAT
      TAG000952();
      pnt_4650 = pnt12238;//Where game will be written
      e.GameFile = OPEN("mini.dat", "rb");
      if (e.GameFile < 0)
      {
        UtilityDialogBox(_1_,UtilityDiskMessages[24], 8, UtilityDiskMessages[32], s12406);
        RETURN_i16(-1);
      };
      w_2 = 1;
    }
    else
    {
      TAG00179c(_2_, w_6 | 0x8000);
      D0W = i16Result;
   	  savegame = (char*)filenames[D0W - 1];

      if (D0W == 0)
      {
        if (e.Word12184 == 0)
        {
          D0L = -1;
        }
        else
        {
          D0L = -99;
        };
        w_2 = D0W;
        goto tag001d9a;
      };
      w_2 = 1;
      w_4122 = 1;
      w_4120 = 1;
      pnt_4650 = pnt12246;
      e.GameFile = OPEN(pnt12246, "rb"); //"DMGAME.DAT"
      if (e.GameFile < 0)
      {
        pnt_4650 = pnt12242;
        e.GameFile = OPEN(pnt12242, "rb");//DMGAME.BAK
        if (e.GameFile < 0) w_4120 = 0;
      };
      if (e.GameFile >= 0) CLOSE(e.GameFile);
      //pnt_4654 = pnt12238;
	    pnt_4654 = (ui8 *)savegame;
      e.GameFile = OPEN(savegame, "rb");
      if (e.GameFile < 0)
      {
        //pnt_4654 = pnt12234;
        //e.GameFile = OPEN(pnt12234,"rb");
        if (e.GameFile < 0) w_4122 = 0;
      };
      if (e.GameFile >= 0) CLOSE(e.GameFile);
      if (w_4120 + w_4122 < 1)
      {
        UtilityDialogBox(_3_,
                  UtilityDiskMessages[12],
                  (unsigned)0x8008,
                  UtilityDiskMessages[32],
                  &s12406[0]);
        w_6 |= 0x8000;
        w_2 = 0;
      }
      else
      {
        if (w_4120 + w_4122 > 1)
        {
          UtilityDialogBox(_4_,
                    UtilityDiskMessages[31],
                    (unsigned)0x800f,
                    UtilityDiskMessages[39],
                    &s12406[4]);
          w_4 = i16Result;
          if (w_4 == 1) goto tag001a4c;
          if (w_4 == 2)
          {
            pnt_4650 = (char *)pnt_4654;
            goto tag001a4c;
          };
          if (e.Word12184 == 0)
          {
            D0L = 1;
          }
          else
          {
            D0L = 99;
          };
          w_2 = D0W;
          goto tag001d9a;
        };
        if (w_4122 != 0)
        {
          pnt_4650 = (char *)pnt_4654;
        };
tag001a4c:
        e.GameFile = OPEN(pnt_4650, "rb");
        if (e.GameFile < 0) w_2 = 0;
      };
    };
  } while (w_2 != 1);
  StrCpy((char *)A4+4,pnt_4650);
  if (D7W != 0)
  {
    A0 = (aReg)UtilityDiskMessages[22];
  }
  else
  {
    A0 = (aReg)UtilityDiskMessages[9];
  };
  UtilityDialogBox(_5_, A0, 15, NULL, NULL);
  wordGear(A4+36) = 29;
  wordGear(A4+2)  = 2;
  ReadExtendedFeatures(e.GameFile);
  D0W = ReadGameBlock1((ui8 *)A4+38);
  if (D0W == 0) goto tag001da8;
  MemoryMove((ui8 *)A4+38, (ui8 *)b_4638, 0x400, 0x400, 512);
  // We just saved the scrambled version at b_4638
  D0W = UnscrambleBlock1((ui8 *)A4+38, wordGear(A4+36));
  if (D0W == 0)
  { // It didn't work with that key!
    wordGear(A4+36) = 10;
    wordGear(A4+2) = 1;
    MemoryMove((ui8 *)b_4638, (ui8 *)A4+38, 0x400, 0x400, 512);
    // We have replaced buffer with original, scrambled version
    D0W = UnscrambleBlock1((ui8 *)A4+38, wordGear(A4+36));
    // Now unscramble with word 10 as initial hash.
    if (D0W == 0) goto tag001da8;
  };
  if (wordGear(A4+2) == 2)
  {
    pnt_4642 = pnt_4126 + 344;
    pnt_4646 = pnt_4126 + 312;
  }
  else
  {
    pnt_4642 = (ui8 *)A4 + 380;
    pnt_4646 = (ui8 *)A4 + 348;
  };
  D0W = MyUnscrambleStream((ui8 *)A4+550,  //block 2
                           128,
                           LE16(wordGear(pnt_4646)),
                           LE16(wordGear(pnt_4642))  );
  if (D0W == 0) goto tag001da8;
  StorePnt(A4+4100, AllocateMemory(16*LE16(wordGear(A4+596))));
  {
    int timerSize;
    //timerSize = sequencedTimers?sizeof(TIMER):10;
    timerSize = sequencedTimers?12:10;
    timerSize = extendedTimers?16:timerSize;
    StorePnt(A4+4104, AllocateMemory(timerSize*LE16(wordGear(A4+578))));
  };
  StorePnt(A4+4108, AllocateMemory(2*LE16(wordGear(A4+578))));
  D0W = MyUnscrambleStream(LoadPnt(A4+4100), //Item16
                          16*LE16(wordGear(A4+596)),
                          LE16(wordGear(pnt_4646+2)),
                          LE16(wordGear(pnt_4642+2)) );
  if (D0W == 0) goto tag001da8;
  // Next is the character data.
  D0W = MyUnscrambleStream((ui8 *)A4+772, //characters
                          3328,
                          LE16(wordGear(pnt_4646+4)),
                          LE16(wordGear(pnt_4642+4)) );
  if (D0W == 0) goto tag001da8;
  for (w_2=0; w_2<LE16(wordGear(A4+560)); w_2++)
  {
    for (w_4=0; w_4<8; w_4++)
    {
      A0 = A4 + 800*w_2 + w_4;
      if (A0[772]== 0) break;
//
//
    };
    while (w_4 < 8)
    {
      A0 = A4 + 800*w_2 + w_4;
      A0[772] = 0;
      w_4++;
//
    };
    for (w_4=0; w_4<20; w_4++)
    {
      A0 = A4 + 800*w_2 + w_4;
      if (A0[780] == 0) break;
//
//
    };
    while (w_4 < 20)
    {
      A0 = A4 + 800*w_2 + w_4;
      A0[780] =0;
      w_4++;
    };
//
//
  };
  {
    //int timerSize = sequencedTimers?sizeof(TIMER):10;
    int timerSize = sequencedTimers?12:10;
    timerSize = extendedTimers?16:timerSize;
    D0W = MyUnscrambleStream(LoadPnt(A4+4104), //Timers
                             timerSize * LE16(wordGear(A4+578)),
                             LE16(wordGear(pnt_4646+6)),
                             LE16(wordGear(pnt_4642+6))  );
  };
  if (D0W == 0) goto tag001da8;
  D0W = MyUnscrambleStream(LoadPnt(A4+4108),
                          2*LE16(wordGear(A4+578)),
                          LE16(wordGear(pnt_4646+8)),
                          LE16(wordGear(pnt_4642+8)) );
  if (D0W == 0) goto tag001da8;
  D0W = TAG00142e(A4); //Text (numWordsInTextArray)
  if (D0W == 0) goto tag001da8;
  e.Word12184 = 0;
  A0 = (aReg)LoadPnt((ui8 *)&e.Pnt24812) + 4112*D7W;
  if (wordGear(A0) != 0)
  {
    TAG001676(LoadPnt((ui8 *)&e.Pnt24812) + 4112*D7W, D7W);
  };
  if (D7W == 0)
  {
    e.Word24814 = LE16(wordGear(A4+560));
    TAG00136c();
  };
  wordGear(A4) = 1;
  A0 = (aReg)LoadPnt((ui8 *)&e.Pnt24812) + 4112*D7W;
  A1 = A4;
  memmove(A0, A1, 4112);
  w_2 = 1;
  CLOSE(e.GameFile);
  e.GameFile = -1;
tag001d9a:
  TAG000952();
  TAG00054a();
  RETURN_i16(w_2);

tag001da8:
  if (D7W != 0)
  {
    A0 = (aReg)UtilityDiskMessages[25];
  }
  else
  {
    A0 = (aReg)UtilityDiskMessages[14];
  };
  UtilityDialogBox(_6_, A0, 8, UtilityDiskMessages[32], &s12406[0]);
  TAG001676((ui8 *)A4, 0);
  w_2 = -1;
  if (e.GameFile >= 0)
  {
    CLOSE(e.GameFile);
    e.GameFile = -1;
  };
  goto tag001d9a;
  //RETURN_i16(0);
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG00260e()
{//(i16)
  static const char* pnt_12;
  static ui8 *pBlock1;
  static char *pnt_4;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pnt_4 = (char *)LoadPnt((ui8 *)&e.Pnt24812);
  pnt_12 = UtilityDiskMessages[30];
  for (;;)
  {
    if (   (wordGear(LoadPnt((ui8 *)&e.Pnt24812)) == 0)
        || (e.Word24814 ==0)  )
    {
      pnt_12 = UtilityDiskMessages[29];
    }
    else
    {
      pBlock1 = (ui8 *)pnt_4 + 38;
      if (wordGear((ui8 *)pnt_4+2) != 1) 
      {
        if (   (pBlock1[301] != 1)
            && (LE16(wordGear(pBlock1+376)) != 1)  ) break;//Word22594
        if (LE16(wordGear(pBlock1+378)) > 13) break; //Word22592
// original        if (LE16(wordGear(pBlock1+378)) > 12) break; //Word22592
        // Error if !DungeonMaster savegame?
      }
      else
      {
        if (pnt_4[337] > 1)
        {
          if (wordGear(pnt_4+412) != 1) break;
          if (wordGear(pnt_4+414) > 12) break;
        };
      };
      CheckChampionNames(_1_);
      RETURN_i16(i16Result);
    };
    break;
  };// while (false);
  UtilityDialogBox(_2_, pnt_12, 8, UtilityDiskMessages[32], s12406);
  RETURN_i16(0);
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG0026c4(pnt P1)
{//(i16)
  static const char* pnt_8;
  static pnt pnt_4;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;;;;;
  pnt_8 = UtilityDiskMessages[30];
  pnt_4 = P1 + 38;
  if (   (wordGear(P1+2) == 2)
      && ((UI8)(pnt_4[301]) >= 2)
      && (LE16(wordGear(pnt_4+378)) == 13) ) RETURN_i16(1);
  UtilityDialogBox(_1_, pnt_8, 8, UtilityDiskMessages[32], s12406);
  RETURN_i16(0);
}

// *********************************************************
//
// *********************************************************
void TAG002724()
{
  dReg D0, D6, D7;
  aReg A0, A1, A4;
  char *pnt_8;
  char *pnt_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;
  pnt_4 = (char *)LoadPnt((ui8 *)&e.Pnt24812) + 4150;
  if (wordGear(LoadPnt((ui8 *)&e.Pnt24812) + 2) == 2)
  {
    pnt_8 = (char *)LoadPnt((ui8 *)&e.Pnt24812) + 38;
  }
  else
  {
    pnt_8 = NULL;
  };
  if (pnt_8 != NULL)
  {
    MemoryMove((ui8 *)pnt_8+256, (ui8 *)pnt_4+256, 0x400, 0x400, 32);
    MemoryMove((ui8 *)pnt_8 +288,(ui8 *)pnt_4+288, 0x400, 0x400, 8);
    StoreLong((ui8 *)pnt_4+308, LoadLong((ui8 *)pnt_8+308) + 1);
    if (pnt_8[301] == 1)
    {
      ASSERT(0,"pnt_8");//check for LE
      wordGear(pnt_4 + 376) = 1;
    }
    else
    {
      wordGear(pnt_4+376) = wordGear(pnt_8+376);
    };
  }
  else
  {
    MemoryMove(LoadPnt((ui8 *)&e.Pnt24812)+294, (ui8 *)pnt_4+256, 0x400, 0x400, 32);
    MemoryMove(LoadPnt((ui8 *)&e.Pnt24812)+326, (ui8 *)pnt_4+288, 0x400, 0x400, 8);
    ASSERT(0,"pnt_8"); //check for LE
    StoreLong((ui8 *)pnt_4+308, LoadLong(LoadPnt((ui8 *)&e.Pnt24812)+344) + 1);
    if (LoadPnt((ui8 *)&e.Pnt24812)[337] ==1)
    {
      ASSERT(0,"pnt24812");//check for LE
      wordGear(pnt_4+376) = 1;
    }
    else
    {
      wordGear(pnt_4+376) = wordGear(LoadPnt((ui8 *)&e.Pnt24812)+412);
    };
  };
  StrCpy((char *)LoadPnt((ui8 *)&e.Pnt24812) + 4116, pnt12238);
  MemoryMove(LoadPnt((ui8 *)&e.Pnt24812)+772, LoadPnt((ui8 *)&e.Pnt24812)+4884, 0x400, 0x400, 3200);
  wordGear(LoadPnt((ui8 *)&e.Pnt24812) + 4672) = LE16(e.Word24814);
  for (D7W=0, A4 = (aReg)LoadPnt((ui8 *)&e.Pnt24812)+4884;
       D7W < e.Word24814;
       D7W++, A4+=800)
  {
    A0 = (aReg)LoadPnt((ui8 *)&e.Pnt24812);
    A1 = (aReg)LoadPnt((ui8 *)&e.Pnt24812);
    D6W = (UI16)(LE16(wordGear(A1+4678)) - LE16(wordGear(A0+566)));
    if (D6W != 0)
    {
      D6W += 4;
      A4[29] = (UI8)((A4[29] + D6W) & 3);
      A4[28] = (UI8)((A4[28] + D6W) & 3);
    };
    A4[30] = 1;
    A4[31] = 2;
    A4[32] = -1;
    A4[33] = 0;
    A4[34] = 0;
    A4[40] = 0;
    A4[41] = 0;
    A4[42] = 0;
    wordGear(A4+44) = (unsigned)0xffff;
    wordGear(A4+46) = (unsigned)0xffff;
    wordGear(A4+48) = LE16(LE16((ui16)(wordGear(A4+48) & 16)));
    wordGear(A4+50) = 0;
    wordGear(A4+52) = wordGear(A4+54);
    wordGear(A4+56) = wordGear(A4+58);
    wordGear(A4+60) = wordGear(A4+62);
    wordGear(A4+64) = 0;
    D0W = (i16)STRandom();
    D0W = sw((D0W & 255) + 1500);
    wordGear(A4+66) = LE16(D0W);
    D0W = (i16)STRandom();
    D0W = sw((D0W & 255) + 1500);
    wordGear(A4+68) = LE16(D0W);
    for (D6W=0; D6W<=6; D6W++)
    {
      A0 = A4 + 3*D6W + 70;
      A0[2] = 30;
      A0[1] = A0[0];
//
//
    };
    A4[72] = 10;
    for (D6W=0; D6W<20; D6W++)
    {
      A0 = A4 + 6*D6W + 92;
      wordGear(A0) = 0;
    };
    for (D6W=0; D6W<30; D6W++)
    { //possessions <- empty
      A0 = A4 + 2*D6W + 212;
      wordGear(A0) = (unsigned)0xffff;
    };
    wordGear(A4+272) = 0;
    wordGear(A4+274) = 0;
//
//
  };
}

// *********************************************************
//
// *********************************************************
//   TAG002a38
RESTARTABLE _StartChaos(const CSB_UI_MESSAGE *)
{//()
  static i16  w_130;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
    RESTART(3)
    RESTART(4)
  END_RESTARTMAP
//;; This is the main program
//;;;;;;;;;;Does something and then reads a gamefile and then
//;;;;;;;;;;does something else.
//;;;;;;;;;;Calls function that reads gamefile, fiddles with
//;;;;;;;;;;characters and writes gamefile.
  keyboardMode = 2;
  InitializeE();
  w_130 = 1;
// What sense does this make?????
  if (w_130 == 2)
  {
    DontDoThis(0x2a4a);


  };
  FadeToPalette(_1_,&e.palette10938);
  TAG0002d0();
  TAG0031a6(0);
  StorePnt((ui8 *)&e.PhysicalBase, physbase());
  SetDLogicalBase(LoadPnt((ui8 *)&e.PhysicalBase));
  TAG0032b4(1, e.Word9124);
  TAG003294(e.Word9124, 0);
  TAG00022a(1);
  TAG0006e4(); // Set critical error handler
  TAG00324c();
  ClearMemory(LoadPnt((ui8 *)&e.PhysicalBase), 32000);
  TAG003264();
  FadeToPalette(_2_,&palette24862);
  StorePnt((ui8 *)&e.Pnt24812, AllocateMemory(8224)); //Room for two games.?? (4112 byes each)
  ClearMemory(LoadPnt((ui8 *)&e.Pnt24812), 8224);
  w_130 = -1;
  while (w_130 == -1)
  {
    ReadSavedGame(_3_, 0);
    w_130 = i16Result;
//
  };

  // ROQUEN: Comparison always true
  //if (w_130 != 0xff9d)
  {
    TAG004518(_4_);
  };
  TAG001676(LoadPnt((ui8 *)&e.Pnt24812), 0);
  TAG001676(LoadPnt((ui8 *)&e.Pnt24812)+4112, 1);
  ReleaseMemory(LoadPnt((ui8 *)&e.Pnt24812));
  TAG000952();
  TAG00022a(2);
  TAG0032b4(2, e.Word9124);
  TAG00323c();
  TAG00031c();
  RETURN;
}

// *********************************************************
//
// *********************************************************
//  TAG002c18
//i16 Random()
//{
//  dReg D0;
//;;;;;;;;;;;;;;;;;
//  if (LoadLong(&e.Long9220) == 0)
//  {
//    StoreLong(&e.Long9220, LoadLong(&e.Long24300) + 1023013);
//  };
//  D0L = Times(LoadLong(&e.Long9220), 314159) + 11;
//  StoreLong(&e.Long9220, D0L);
//  return (D0L >> 8) & 0xffffff;
//}

// *********************************************************
//
// *********************************************************
//  TAG002c70
i16 TestInRectangle(wordRectPos *rect, i32 x, i32 y)
{
  dReg D6, D7;
  wordRectPos *rectA4;
  rectA4 = rect;
  D7W = sw(x);
  D6W = sw(y);
  if (D7W > rectA4->x2) return 0;
  if (D7W < rectA4->x1) return 0;
  if (D6W > rectA4->y2) return 0;
  if (D6W < rectA4->y1) return 0;
  return 1;
}

// *********************************************************
// Discard any queued keystrokes
// *********************************************************
void TAG002ca8()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;
  while (UI_CONSTAT() != 0)
  {
    UI_DIRECT_CONIN();
  };
  e.Byte9212 = -128;
  e.FirstQueuedMouseEvent = 0;
  e.LastQueuedMouseEvent = 4;
  e.Word9210 = 0;
  e.Byte9212 = 0;
}

// *********************************************************
//
// *********************************************************
i16 TAG002cdc(DlgButton *P1, i32 x, i32 y, i32 leftRight)
{
  dReg D0, D5, D6, D7;
  DlgButton *dlgButtonA4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dlgButtonA4 = P1;
  D7W = sw(x);
  D6W = sw(y);
  if (dlgButtonA4 == NULL) return 0;
  while ( (D5W = dlgButtonA4->byte0) != 0)
  {
    D0W = TestInRectangle(&dlgButtonA4->rectPos2, D7W, D6W);
    if (D0W != 0)
    {
      if (leftRight & dlgButtonA4->byte11)
      {
        if ((dlgButtonA4->byte1 & 1) == 0)
        {
          return D5W;
        };
      };
    };
    dlgButtonA4++;
  };
  return D5W;
}

// *********************************************************
//
// *********************************************************
//   TAG002d3c
void EnqueMouseClick(i32 P1,i32 P2,i32 P3)
{
  dReg D0, D4, D5, D6, D7;
  aReg A0;
  S12406 **pS12406A4;
  i16 w_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(P1);
  D6W = sw(P2);
  if (e.Byte9212 != 0)
  { // Que is busy.  Put click in pending state.
    e.Word9210 = 1;
    e.Word9208 = D7W;
    e.Word9206 = D6W;
    e.Word9204 = sw(P3);
    return;
  };
  D5W = sw(e.LastQueuedMouseEvent + 2);
  if (D5W > 4) D5W -= 5;
  if (D5W == e.FirstQueuedMouseEvent) return;
  for (w_2 = 0, pS12406A4 = ppS12406_9142;
       w_2 < 6;
       w_2++, pS12406A4++)
  {
    if (*pS12406A4 == NULL) continue;
    if ((*pS12406A4)->byte1 & 1) continue;
    D4W = TAG002cdc((*pS12406A4)->pDlgButton2, D7W, D6W, P3);
    if (D4W == 0) continue;
    D0W = D5W;
    D5W--;
    if (D0W == 0) D5W = 4;
    e.LastQueuedMouseEvent = D5W; //Last item in queue
    A0 = e.Byte9202 + 6*D5W;
    A0[0] = D4B;
    A0[1] = (i8)w_2;
    wordGear(A0+2) = D7W;
    wordGear(A0+4) = D6W;
    break;
//
//
  };
  e.Byte9212 = 0;
}



// *********************************************************
//
// *********************************************************
//   TAG002e2c
void CheckPendingMouseClick()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  if (e.Word9210 == 0) return;
  e.Word9210 = 0;
  EnqueMouseClick(e.Word9208, e.Word9206, e.Word9204);
}

// *********************************************************
//
// *********************************************************
void TAG002e4a(i32 P1)
{
  dReg D0, D1, D4, D5, D6, D7;
  aReg A0, A3;
  S12406 **pS12406A4;
  i16 w_4 = 0x1ccc;
  i16 w_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;
  D7L = P1;
  if (D7W == 0)
  {
    D7L >>= 8;
  }
  else
  {
    D0L = D7L & 0xff;
    D1L = D7L >> 8 & 0xff00;
    D4W = sw(D0W | D1W); // UpperHalf,LowerHalf in D4
    for (D5L=0;
         (D5L<7) && (D4W>=word9138[D5W]);
         D5W++)
    {
      if (D4W != word9138[D5W]) continue;
      D7L = D4UW;
      break;
//
//
    };
  };
  e.Byte9212 = -128;
  D6W = sw(e.LastQueuedMouseEvent + 2);
  if (D6W > 4) D6W -= 5;
  if (D6W != e.FirstQueuedMouseEvent)
  {
    w_2 = 0;
    for (D5L=0, pS12406A4 = ppS12406_9142;
         D5W < 6;
         D5W++, pS12406A4++)
    {
      if (*pS12406A4 == NULL) continue;
      if ((*pS12406A4)->byte1 & 1) continue;
      A3 = (*pS12406A4)->pnt6;
      if (A3 == NULL) continue;
      for ( ;
            (D4W = sw(A3[0]&255)) != 0;
            A3+=4)
      {
        if (A3[1] & 1) continue;
        if (wordGear(A3+2) == D7W)
        {
tag002f10:
          D0W = D6W;
          D6W--;
          if (D0W == 0) D6W = 4;
          e.LastQueuedMouseEvent = D6W;
          A0 = e.Byte9202+6*D6W;
          A0[0] = D4B;
          A0[1] = D5B;
          wordGear(A0+2) = (ui16)-1;
          wordGear(A0+4) = D7W;
          goto tag002f9a;
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
    };
    if (w_2 != 0)
    {
      D4W = w_2;
      ASSERT(w_4 != 0x1ccc,"w_4");
      D5W = w_4;
      goto tag002f10;
    };
  };
tag002f9a:
  e.Byte9212 = 0;
  CheckPendingMouseClick();
}

// *********************************************************
//
// *********************************************************
i16 TAG002faa(S12406 *P1)
{
  dReg D7;
  S12406 **pS12406A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;
  for (D7W=0, pS12406A4=ppS12406_9142; D7W<6; D7W++, pS12406A4++)
  {
    if (*pS12406A4 != NULL) continue;
    *pS12406A4 = P1;
    TAG002ca8();
    return D7W;
//
  };
  return -1;
}

// *********************************************************
//
// *********************************************************
//       TAG003000
S12406 **SetActiveDialog(S12406 **P1)
{
  S12406 **ppS12406_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;
  ppS12406_4 = ppS12406_9142;
  ppS12406_9142 = P1;
  TAG002ca8();
  return ppS12406_4;
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG00301c(pnt P1)
{//(i16)
  static dReg D0, D7;
  static S12406 *S12406A0;
  static aReg A0, A1, A3, A4;
  static i32 pressed_key;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;
  A4 = P1;
  //while (pumper(), wvbl(), CONSTAT() != 0)
  for (;;)
  {
    wvbl(_1_);
    if (UI_CONSTAT() == 0) break;
    pressed_key = UI_DIRECT_CONIN();
    TAG002e4a(pressed_key);
  	if(pressed_key) EditCharacterName((char)pressed_key);
  };
  e.Byte9212 = -128;
  D7W = sw(e.LastQueuedMouseEvent + 1);
  if (D7W > 4) D7W = 0;
  if (D7W == e.FirstQueuedMouseEvent)
  { // Means queue is empty
    A0 = A4;
    A1 = e.Byte9172;
    StoreLong(A0, LoadLong(A1));
    wordGear(A0+4) = wordGear(A1+4);
    A0 += 6;
    A1 += 6;
  }
  else
  {
    A0 = &e.Byte9202[6*e.FirstQueuedMouseEvent];
    A1 = A4;
    StoreLong(A1, LoadLong(A0));
    wordGear(A1+4) = wordGear(A0+4);
    e.FirstQueuedMouseEvent++;
    if (e.FirstQueuedMouseEvent > 4) e.FirstQueuedMouseEvent = 0;
  };
  e.Byte9212 = 0;
  CheckPendingMouseClick();
  if (*A4 != 0)
  {
    D7W = (UI8)(A4[1]);
    S12406A0 = ppS12406_9142[D7W];
    A4[1] = S12406A0->byte0;
    S12406A0 = ppS12406_9142[D7W];
    A3 = S12406A0->pnt10;
    if (A3 != NULL)
    {
      DontDoThis(0x30e0);; //What to do here?
      //(A3)(A4);
    };
  };
  if (A4[0] == 0) A4[1] =0;
  RETURN_i16 ( (UI16)(((A4[1]<<8)&0xff00) | (A4[0]&0xff)) );
}

// *********************************************************
//
// *********************************************************
void TAG003106(i16)
{
  DontDoThis(0x3106);
}

// *********************************************************
//
// *********************************************************
void TAG0031a6(i16 P1)
{
  i16 w_138[57];
  i16 w_24[10];
  i16 w_4;
  i16 w_2;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TAG00507e();
  e.Word9124 = TAG005868(&w_2, &w_2, &w_2, &w_2);
  for (w_2=0; w_2<10; w_2++)
  {
    w_24[w_2] = 1;
//
  };
  w_4 = 2;
  if (P1 == 0)
  {
    TAG005d8e(w_24, &e.Word9124, w_138);
  }
  else
  {
    TAG003106(1);
    TAG005d12(w_24, &e.Word9124, w_138);
    TAG003106(2);
  };
}

// *********************************************************
//
// *********************************************************
void TAG00323c()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TAG005df2(e.Word9124);
  TAG0050d2();
}

// *********************************************************
//
// *********************************************************
void TAG00324c()
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;
  D0W = e.Word8074;
  e.Word8074++;
  if (D0W == 0)
  {
    TAG0061e2(e.Word8054);
  };
}

// *********************************************************
//
// *********************************************************
void TAG003264()
{
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D0W = e.Word8074;
  e.Word8074--;
  if (D0W == 1)
  TAG0061ba(e.Word8054, 1);
}

// *********************************************************
//
// *********************************************************
void TAG003294(i16 P1, i16 P2)
{
//;;;;;;;;;;;;;;;;;;;;
  TAG00589e(0, P2);
  TAG0061ba(P1, 0);
}

// *********************************************************
//
// *********************************************************
void TAG00334c(dReg& D0, dReg& D1)
{
    if (e.Word8066 != 0)
    {
      D0W = e.Word8070;
      D1W = e.Word8068;
    }
    else
    {
      e.Word8070 = D0W;
      e.Word8068 = D1W;
    };
}

// *********************************************************
//
// *********************************************************
void TAG003372(dReg& D0)
{
  dReg D3;
  if (e.Word8056 == 0)
  {
    D3W = e.Word8072;
    if ( (D3W != 0) && (D0W!=0) )
    {
      e.Word8072 = D0W;
    }
    else
    {
      e.Word8072 = D0W;
      if (D0W != 0)
      {
        EnqueMouseClick(e.Word8070, e.Word8068, D0W);
      };
    };
    e.Word8056 = 0;
  };
}

// *********************************************************
//
// *********************************************************
void TAG0032b4(i16 P1, i16 P2)
{
  aReg A4;
  pnt  pnt_10;
  i16  w_6;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  switch (P1)
  {
  case 1:
    e.Word8054 = P2;
    TAG006202(e.Word8054, &w_6, &e.Word8070, &e.Word8068);
    A4 = (pnt)0x00334c;
    TAG00626e(e.Word8054, (char *)A4, (char **)&e.Pnt8064);
    A4 = (pnt)0x003372;
    TAG00623a(e.Word8054, (char *)A4, (char **)&e.Pnt8060);
    break;
  case 2:
    TAG00626e(e.Word8054, (char *)LoadPnt((ui8 *)&e.Pnt8064), (char **)&pnt_10);
    TAG00623a(e.Word8054, (char *)LoadPnt((ui8 *)&e.Pnt8060), (char **)&pnt_10);
    break;
//;;;;;;;;;;;;;;;;;;;;;;;;;
  default:{};
  }; //switch (P1);
}

// *********************************************************
//
// *********************************************************
//   TAG0033b6
void BitBltSquareImage(pnt P1,const wordRectPos *rect,i16 P3,i16 P4)
{
//;;;;;;;;;;;;;;;
  e.Word8048 = 0;
  TAG0088b2((ui8 *)P1,
            LoadPnt((ui8 *)&e.PhysicalBase),
            (RectPos *)rect,
            0,
            0,
            P3,
            160,
            P4);
}

// *********************************************************
//
// *********************************************************
//   TAG0033e6
void FillScreenRectangle(wordRectPos *rp, i16 P2)
{
//;;;;;;;;;;
  FillRectangle(LoadPnt((ui8 *)&e.PhysicalBase),
                (RectPos *)rp,
                P2,
                160);
}

// *********************************************************
//
// *********************************************************
//   TAG0034dc
void ExpandRectangle(wordRectPos *rp1,
                     wordRectPos *rp2,
                     i16 deltaX,
                     i16 deltaY)
{
  rp2->x1 = sw(rp1->x1 - deltaX);
  rp2->x2 = sw(rp1->x2 + deltaX);
  rp2->y1 = sw(rp1->y1 - deltaY);
  rp2->y2 = sw(rp1->y2 + deltaY);
}

// *********************************************************
//
// *********************************************************
//   TAG003406
void DrawNameTitleCarret(i16 P1, i16 P2)
{
  dReg D0;
  wordRectPos *rectA4;
//;;;;;;;;;;;;;;;;;;;;

  rectA4 = &dlgButton5050[P1+13].rectPos2;
  e.EditBoxSelected = P1;
  e.EditBoxPosition = P2;
  TAG00324c();
  if (e.Word5168 != 0)
  {
    FillScreenRectangle(&e.wRectPos5176, 1);
  };
  D0W = sw(rectA4->x1 + 6*P2 + 1);
  e.wRectPos5176.x2 = D0W;
  e.wRectPos5176.x1 = D0W;
  e.wRectPos5176.y1 = rectA4->y1;
  e.wRectPos5176.y2 = rectA4->y2;
  FillScreenRectangle(&e.wRectPos5176, 15);
  TAG003264();
  e.Word5168 = 1;
  e.Word5178 = 30;
}

// *********************************************************
//
// *********************************************************
//          _TAG00352c
RESTARTABLE _InvertButton(const wordRectPos *pRect)
{//()
  static dReg D5, D6, D7;
  static aReg A3;
  const static wordRectPos *rectA4;
  static wordRectPos rect_8;
  static i32 i;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;
  rectA4 = pRect;
  rect_8.x1 = 0;
  rect_8.y1 = 0;
  D6W = sw(rectA4->x2 - rectA4->x1);
  rect_8.x2 = sw(D6W + 1);
  D5W = sw(rectA4->y2 - rectA4->y1 + 1);
  rect_8.y2 = sw(D5W + 1);
  D6W = sw(((D6W + 15) & 0xfff0) / 2);
  TAG00324c();
  A3 = e.Byte3764;
  ASSERT(size3764 >= D6W*(rect_8.y2+1),"size3467");
  TAG0088b2(LoadPnt((ui8 *)&e.PhysicalBase), //src
            (ui8 *)A3,             //dst
            (RectPos *)&rect_8, //dst position
            rectA4->x1,     //src x offset
            rectA4->y1,     //src y offset
            160,            //src byte width
            D6W,            //dst byte width
            -1);
  for (D7W = sw((D6W * D5W)/4);
       D7W > 0;
       D7W--)
  {
    StoreLong(A3, LoadLong(A3) ^ -1); 
    A3 += 4;
  };
  BitBltSquareImage(e.Byte3764, rectA4, D6W, -1);
  for (i=0; i<5; i++)
  {
    wvbl(_1_);
  }
  TAG003264();
  RETURN;
}

// *********************************************************
//
// *********************************************************
void TAG004ea2(i16 *)
{
  HopefullyNotNeeded(0xcd89);
  // This appears to do nothing on the Atari.
  // A parameter of 115 does something.  But 200 does nothing.
//;;;;;;;;;;;;;;;;;;;;;;;;
}

//file = chaos.ftl      offset =68ec
//
char data004f1c[348] = {
           0, 1, 0, 2, 1, 1, 2, 1, 1, 0, 1, 1, 2, 1, 1, 1,
//004f2c
           1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
//004f3c
           0, 3, 5, 0, 5, 5, 0, 0, 1, 1, 2, 1, 0,16, 7, 1,
//004f4c
           2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2,
//004f5c
           1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 0, 0,
//004f6c
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1,
//004f7c
           6, 1, 1, 4, 1, 1, 1, 3, 1, 2, 1, 1, 4, 2, 1, 8,
//004f8c
           1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 9, 1, 1, 1, 1,
//004f9c
           1, 1, 1, 0, 0, 5, 1, 3, 3, 1, 2, 2, 1, 0, 0, 0,
//004fac
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//004fbc
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//004fcc
           0, 0, 0, 0, 4, 3, 0, 8, 3, 0, 6, 1, 0, 8, 1, 0,
//004fdc
           8, 1, 0, 4, 1, 1, 3, 1, 1, 0, 5, 0, 1, 1, 1, 0,
//004fec
           5, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
//004ffc
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//00500c
           0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//00501c
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 1,
//00502c
           0, 5, 1, 0, 1, 1, 0, 1, 1, 0, 2, 5, 0, 6, 1, 0,
//00503c
           2, 1, 0, 1, 1, 0, 6, 5, 0, 0, 0, 0, 0, 1, 1, 0,
//00504c
           1, 0, 2, 1, 0, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
//00505c
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 1, 2,
//00506c
           1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 2};


// *********************************************************
//
// *********************************************************
//   TAG0035cc
void DrawDialogBoxOutline(wordRectPos *rectPos,
                          i16 P2,
                          i16 P3,
                          i16 P4)
{
  dReg D6, D7;
  wordRectPos rectPos_24;
  wordRectPos rectPos_16;
  wordRectPos rectPos_8;
  wordRectPos *rpA4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  rpA4 = rectPos;
  D7W = P2;
  if (D7W < 0)
  {
    ExpandRectangle(rpA4, &rectPos_8, D7W, D7W);
    D7W = sw(-D7W);
  }
  else
  {
    MemoryMove((ui8 *)rpA4, (ui8 *)&rectPos_8, 0x400, 0x400, 8);
  };
  FillScreenRectangle(&rectPos_8, P4);
  if (D7W == 0) return;
  ExpandRectangle(&rectPos_8, &rectPos_16, D7W, D7W);
  D7W--;
  for (D6W=0; D6W<4; D6W++)
  {
    MemoryMove((ui8 *)&rectPos_16, (ui8 *)&rectPos_24, 0x400, 0x400, 8);
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
    };// case (D6W)
    FillScreenRectangle(&rectPos_24, P3);
//
  };
}

// *********************************************************
//
// *********************************************************
//   TAG0036c0
void DrawMagnifiedCharacterImage()
{
  dReg D0, D6, D7;
  aReg A4;
  wordRectPos rect_8;
//;;;;;;;;;;;;;;;;;;;;;;
  A4 = e.CharacterImages + 464*e.ActualCharacter;
  rect_8.x1 = wRectPos5162.x1;
  TAG00324c();
  for (D7W=0; D7W<32; D7W++)
  {
    rect_8.x2 = sw(rect_8.x1 + 2);
    rect_8.y1 = wRectPos5162.y1;
    for (D6W=0; D6W<29; D6W++)
    {
      rect_8.y2 = sw(rect_8.y1 + 2);
      D0W = sw(GetPixel(A4, D7W, D6W, 16));
      FillScreenRectangle(&rect_8, D0W);
      rect_8.y1 += 3;
//
    };
    rect_8.x1 += 3;
//
//
  };
  TAG003264();
}

// *********************************************************
//
// *********************************************************
//   TAG003748
void PrintCharacterName(i16 chIdx)
{
  dReg D0;
  aReg A1;
  wordRectPos *rectA4;
  char name[9];
  i32 i;
//;;;;;;;;;;;;;;
  ASSERT(chIdx < 4,"chidx");
  rectA4 = &wRectPos5138[chIdx];
  FillScreenRectangle(rectA4, 1);
  if (chIdx != e.ActualCharacter)
  {
    D0L = 15;
  }
  else
  {
    D0L = 9;
  };
  A1 = (aReg)LoadPnt((ui8 *)&e.Pnt24812) + 800*chIdx + 772;
  // character name should be redrawn completely
  strcpy(name, (char *)LoadPnt((ui8 *)&e.Pnt24812) + 800*chIdx + 772);
  for(i=0; i<8; i++)
  {
	  if((name[i]==0) || (name[i]<'A') || (name[i]>'Z'))
		  name[i] = 32;
  }
  name[8] = 0;
  TextToScreen(rectA4->x1+2, 9, D0W, 1, name);
}

// *********************************************************
//
// *********************************************************
//   TAG0037ac
void DrawCharacterHeader(i16 P1, i16 P2)
{
  aReg A0;
  wordRectPos *rectA0, *rectA1;
  DlgButton *dlgButtonA0;
  wordRectPos rect_8;
//;;;;;;;;;;;;;;;;;;;
  TAG00324c();
  if (P1 < e.Word24814)
  {
    PrintCharacterName(P1);
    rectA0 = &wRectPos5106[P1];
    DrawDialogBoxOutline(rectA0, 1, P2, 1);
    A0 = e.CharacterImages + 464*P1;
    rectA1 = &wRectPos5106[P1];
    BitBltSquareImage(A0, rectA1, 16, -1);
  }
  else
  {
    dlgButtonA0 = &dlgButton5050[P1];
    MemoryMove((ui8 *)dlgButtonA0+2, (ui8 *)&rect_8, 0x400, 0x400, 8);
    rect_8.y2--;
    FillScreenRectangle(&rect_8, 0);
  };
  TAG003264();
}

// *********************************************************
//
// *********************************************************
//   TAG003860
void AddDialogButton(DlgButton *P1, const char* P2, i16 P3)
{
  dReg D0, D1;
  DlgButton *dlgButtonA4;
  i16 w_18;
  wordRectPos rectPos_8;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  dlgButtonA4 = P1;
  TAG00324c();
  MemoryMove((ui8 *)&dlgButtonA4->rectPos2, (ui8 *)&rectPos_8, 0x400, 0x400, 8);
  DrawDialogBoxOutline(&rectPos_8, 2, P3, 0);
  w_18 = sw((rectPos_8.x1 + rectPos_8.x2)/2);
  D0W = sw(3* StrLen(P2));
  D1W = sw(w_18 - D0W);
  TextToScreen(D1W+1, rectPos_8.y2-2, 15, 0, P2);
  TAG003264();
}

// *********************************************************
//
// *********************************************************
//   TAG0038ea
void GetPalleteItemRect(i16 P1, wordRectPos *rect)
{
  wordRectPos *rectA4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  rectA4 = rect;
  rectA4->x1 = wRectPos5154.x1;
  rectA4->x2 = wRectPos5154.x2;
  rectA4->y1 = sw(wRectPos5154.y1 + 8*P1);
  rectA4->y2 = sw(rectA4->y1 + 6);
}

// *********************************************************
//
// *********************************************************
//   TAG003922
void DrawSelectedColor(i16 P1)
{
  dReg D0;
  wordRectPos rect_8;
//;;;;;;;;;;;;;;;;;;;;;;;
  if (P1 == e.Word7062) return;
  TAG00324c();
  if (e.Word7062 >= 0)
  {
    GetPalleteItemRect(e.Word7062, &rect_8);
    D0L = byte4712[e.Word7062];
    DrawDialogBoxOutline(&rect_8, 1, 0, D0W);
  };
  e.Word7062 = P1;
  GetPalleteItemRect(e.Word7062, &rect_8);
  D0W = byte4712[e.Word7062];
  DrawDialogBoxOutline(&rect_8, 1, 15, D0W);
  TAG003264();
}

// *********************************************************
//
// *********************************************************
void TAG0039a8()
{
  aReg A0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A0 = e.Byte5202 + 6*e.ActualCharacter;
  e.Word4696 = ub(A0[4]);
  A0 = e.CharacterImages + 464*e.ActualCharacter;
  MemoryMove((ui8 *)A0, (ui8 *)e.Byte4694, 0x400, 0x400, 464);
}

// *********************************************************
//
// *********************************************************
//   TAG0039ea
void FillCharacterNameEditBox(const char *P1, i16 P2, i16 P3)
{
  dReg D0, D1, D5, D6, D7;
  wordRectPos *rectA4;
//;;;;;;;;;;;;;;;;;;
  rectA4 = &dlgButton5050[P2].rectPos2;
  D5W = StrLen(P1);
  e.Word5168 = 0;
  FillScreenRectangle(rectA4, 1);
  D7W = sw(rectA4->x1 + 2);
  D6W = sw(rectA4->y2 - 2);
  TextToScreen(D7W, D6W, 9, 1, P1);
  D0W = sw(6*D5W + D7W);
  D1W = sw(P3 - D5W);
  TextToScreen(D0W,
               D6W,
               9,
               1,
               "___________________" + 19 - D1W);
}

// *********************************************************
//
// *********************************************************
//   TAG003a7c
void PrintAttributeValue(const char* P1, i32 P2, i32 P3)
{
  char b_20[20];
//;;;;;;;;;;;;;;;;;;;;;;
  atari_sprintf(b_20, "%s %d", (i32)P1, P2);
  TextToScreen(17, P3, 13, 0, b_20);
}

// *********************************************************
//
// *********************************************************
//          _TAG003ab6
RESTARTABLE _DrawCharacterDetails(const i32 P1, const i32 P2)
{//()
  static dReg D0, D3, D4, D5, D6, D7;
  static aReg A0, A1, A4;
  static char b_28[20];
  static wordRectPos rect_8;
  RESTARTMAP
    RESTART(1)
    RESTART(2)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;;;;;;;;
  D7W = sw(P1);
  D6W = e.ActualCharacter;
  if (D7W == D6W) RETURN;
  e.ActualCharacter = D7W;
  TAG00324c();
  if (D6W >= 0)
  {
    DrawCharacterHeader(D6W, 0);
  };
  if (P2 != 0)
  {
    TAG0039a8();
  };
  DrawCharacterHeader(D7W, 15);
  wvbl(_1_);
  BitBltSquareImage((i8 *)FrameImage, &wRectPos5106[4], 24, -1);
  wvbl(_2_);
  A0 = e.CharacterImages + 464*D7W;
  BitBltSquareImage(A0, &wRectPos5106[5], 16, 1);
  FillScreenRectangle(&wRectPos5106[6], 0);
  A4 = (aReg)LoadPnt((ui8 *)&e.Pnt24812) + 800*D7W + 772;
  FillCharacterNameEditBox((char *)A4, 13, 7);
  FillCharacterNameEditBox((char *)A4+8, 14, 19);
  D0W = StrLen((char *)A4);
  DrawNameTitleCarret(0, D0W);
  for (D5L=121, D6L=0; D6W<4; D6W++)
  {
    A0 = A4 + 6*D6W;
    D3L = LE32(LoadLong(A0+94));
    for (D4L=0; D3L >= 500; D3L>>=1, D4W++){};
//

    if (D4W == 0) continue;
    D4W--;
    if (D4W >= 15) D4W = 14;
    A0 = (aReg)pnt4772[D4W];
    A1 = (aReg)pnt4788[D6W];
    atari_sprintf(b_28, "%s %s", (i32)A0, (i32)A1);
    TextToScreen(17, D5W, 13, 0, (char *)b_28);
    D5W += 7;
//
  };
  if (D5W < 149)
  {
    rect_8.x1 = 17;
    rect_8.x2 = 131;
    rect_8.y1 = sw(D5W-6);
    rect_8.y2 = 149;
    FillScreenRectangle(&rect_8, 0);
  };
  PrintAttributeValue("HEALTH     ", LE16(wordGear(A4+54)), 154);
  PrintAttributeValue("STAMINA    ",LE16(wordGear(A4+58))/10, 161);
  PrintAttributeValue("MANA       ",LE16(wordGear(A4+62)), 168);
  DrawMagnifiedCharacterImage();
  TAG003264();
  RETURN;
}

// *********************************************************
//
// *********************************************************
//          _TAG003f12
RESTARTABLE _DrawCharacterEditorScreen()
{//()
  static dReg D7;
  static wordRectPos rect_8;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;;;
  TAG00324c();
  ClearMemory(LoadPnt((ui8 *)&e.PhysicalBase),32000);
  AddDialogButton(&dlgButton5050[10], "LOAD CHAMPIONS", 2);
  AddDialogButton(&dlgButton5050[11], "SAVE CHAMPIONS", 2);
  AddDialogButton(&dlgButton5050[12], "MAKE NEW ADVENTURE", 2);
  AddDialogButton(&dlgButton5050[7], "REVERT", 2);
  AddDialogButton(&dlgButton5050[8], "UNDO", 2);
  AddDialogButton(&dlgButton5050[16], "QUIT", 2);
  DrawDialogBoxOutline(&wRectPos5162, 3, 3, 0);
  DrawDialogBoxOutline(&wRectPos5146, 2, 2, 0);
  DrawCharacterDetails(_1_, 0, 1);
  for (D7W = 1; D7W < e.Word24814; D7W++)
  {
    DrawCharacterHeader(D7W, 0);
//
  };
  DrawSelectedColor(0);
  for (D7W=1; D7W<16; D7W++)
  {
    GetPalleteItemRect(D7W, &rect_8);
    FillScreenRectangle(&rect_8, byte4712[D7W]);
//
//
  };
  TAG003264();
  RETURN;
}

// *********************************************************
//
// *********************************************************
void TAG00439e()
{
  dReg D7;
//;;;;;;;;;;;;;;;
  for (D7W=0; D7W<e.Word24814; D7W++)
  {
    MemoryMove(LoadPnt(e.Byte5202 + 6*D7W),
               (ui8 *)e.Byte4228,
               0x400,
               0x400,
               464);
    MemoryMove((ui8 *)e.CharacterImages + 464*D7W,
               LoadPnt(e.Byte5202 + 6*D7W),
               0x400,
               0x400,
               464);
    MemoryMove((ui8 *)e.Byte4228,
               (ui8 *)e.CharacterImages + 464*D7W,
               0x400,
               0x400,
               464);
//
//
  };
}

// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG004430(i32 P1)
{//()
  static dReg D7;
  static aReg A0, A1;
  RESTARTMAP
    RESTART(1)
  END_RESTARTMAP
//;;;;;;;;;;;;;
  e.ActualCharacter = -1;
  for (D7L=0; D7W<4; D7W++)
  {
    if (D7W < e.Word24814)
    {
      A0 = e.Byte5202 + 6*D7W;
      StorePnt(A0, LoadPnt((ui8 *)&e.Pnt24812) + 800*D7W + 1108);
      A1 = e.CharacterImages + 464*D7W;
      MemoryMove(LoadPnt(A0), (ui8 *)A1, 0x400, 0x400, 464);
    }
    else
    {
      A0 = e.Byte5202 + 6*D7W;
      StoreLong(A0, 0);
      A0 = e.CharacterImages + 464*D7W;
      ClearMemory((ui8 *)A0, 464);
    };
    A0 = e.Byte5202 + 6*D7W;
    A0[4] = 0;
//
  };
  if (P1 == 0) RETURN;
  if (e.Word24814 != 0)
  {
    DrawCharacterDetails(_1_, 0, 1);
  }
  else
  {
    DrawCharacterHeader(0, 0);
  };
  for (D7W=1; D7W<4; D7W++)
  {
    DrawCharacterHeader(D7W, 0);
//
//
  };
  RETURN;
}


// *********************************************************
//
// *********************************************************
RESTARTABLE _TAG004518()
{//()
  static dReg D0;
  static unsigned char buffer[6];
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
  END_RESTARTMAP
//;;;Lots of something. Read game file.  Fiddle with characters
//;;;WriteGamefile
  e.Word8048 = 0;
  e.Word7062 = -1;
  TAG004430(_6_, 0);
  DrawCharacterEditorScreen(_7_);
  TAG002faa(&s12406_4802);
  do
  {
    TAG00301c(_1_, buffer);
    D0W = i16Result;
    switch (D0W & 255)
    {
    case 1:
    case 2:
    case 3:
    case 4:
	    DrawCharacterDetails( _11_, (D0W & 255)-1,1);
      if (e.Word5178 == 0) RETURN;
      break;
    case 5:
		  {
			  // image size 22x21
			  //i32 mousex=(buffer[2]-156)/4;
			  //i32 mousey=(buffer[4]-60)/4;
			  //i8* start = (i8*)(e.CharacterImages + 464*e.ActualCharacter) + (mousey * 22) + mousex;
			  //(*start) = SelectedColor;
			  // does not work - graphics is stored in planar structure, not chunky!
			  // must rewrite it later
			  BitBltSquareImage(e.CharacterImages + 464*e.ActualCharacter, &wRectPos5106[e.ActualCharacter], 16, -1);
			  BitBltSquareImage((i8 *)FrameImage, &wRectPos5106[4], 24, -1);
			  BitBltSquareImage(e.CharacterImages + 464*e.ActualCharacter, &wRectPos5106[5], 16, 1);
			  DrawMagnifiedCharacterImage();
		  // Edit character image
		  }
      break;
    case 6:
      DontDoThis(0x45d0);  goto case17;
      break;
    case 7:
		  // choose color
		  SelectedColor = (unsigned char)((buffer[4]-42)/8);
		  DrawSelectedColor(SelectedColor);
      break;
    case 8:
      //MessageBox(NULL,"I do not plan to implement this function","Error",MB_OK|MB_TASKMODAL);
      DontDoThis(0x4616);  goto case17;
      break;
    case 9:
	  // I have encountered it sometimes
      //DontDoThis(0x46ae);  goto case17;
      break;
    case 11:
      DontDoThis(0x476a); goto case17;
      break;
    case 12:
      DontDoThis(0x4872);  goto case17;
      break;
    case 13: //Make New Adventure
      InvertButton(_8_, &dlgButton5050[12].rectPos2);
      TAG00260e(_2_);
      D0W = i16Result;
      if (D0W != 0)
      {
        if (wordGear(LoadPnt((ui8 *)&e.Pnt24812)+4112) == 0)
        {
          ReadSavedGame(_3_, 1);
        };
        if (wordGear(LoadPnt((ui8 *)&e.Pnt24812)+4112) != 0)
        {
          TAG0026c4(_4_, LoadPnt((ui8 *)&e.Pnt24812) + 4112);
          D0W = i16Result;
          if (D0W != 0)
          {
            TAG00439e();
            TAG002724();
            TAG00439e();
            TAG000fc4(_5_, 1);
          };
        };
      };
      InvertButton(_9_, &dlgButton5050[12].rectPos2);
      break;
    case 14:
		  // Change character name
		  {
		    // First get character name and determine length
        i16 desired_pos, pos;
		    desired_pos = (i16)(((buffer[2]-17)/6) + 1);
		    char* name = (char *)LoadPnt((ui8 *)&e.Pnt24812) + 800*e.ActualCharacter + 772;
		    // Then get desired carret position and fix it
		    pos = (i16)strlen(name);

		    if(desired_pos > pos) desired_pos = pos;
		    // Draw carret
		    DrawNameTitleCarret(0, desired_pos);
		    break;

		  };
      break;
    case 15:
		  // Change character title
  		{
	    	// First get character name and determine length
		    i16 desired_pos = (i16)(((buffer[2]-17)/6) + 1);
		    i16 pos;
		    char* title = (char *)LoadPnt((ui8 *)&e.Pnt24812) + 800*e.ActualCharacter + 780;

		    // Then get desired carret position and fix it
		    pos = (i16)strlen(title);
		    if(desired_pos > pos) desired_pos = pos;

		    // Draw carret
		    //DrawNameTitleCarret(1,desired_pos);
		    desired_pos += 32; // I can see if editing name or title 'coz position could not be larger than 32
		  };
      break;
    case 16:
      DontDoThis(0x49c0);  goto case17;
      break;
    case 17:
case17:
      InvertButton(_10_,&dlgButton5050[16].rectPos2);
      RETURN;
    case 10:
    default:
      break;
    }; //case
  } while (e.Word5178 != 0);
  DontDoThis(0x49e0);
  RETURN;
};


// Internal function to make TextOut easier to read
void TAG004d4a(dReg& D2,
               dReg& D3,
               dReg& D4,
               dReg& D5,
               aReg& A2,
               aReg  A3,
               i32 A3inc,
               i32 jA0,
               i32 jA1)
{
  dReg D0, D1, D6, D7;
loop:
  StoreLong(A3+0, LoadLong(A3+0) & LE32(D2L));
  StoreLong(A3+4, LoadLong(A3+4) & LE32(D2L));
  D0L = (UI8)(*A2);
  switch (D5W)
  {
  case 0:
    D0W <<= 11-D3W;
    break;
  case 8:
    D0W >>= D3W-11;
    break;
  case 18:
    D0W <<= 27-D3W;
    break;
  default: DontDoThis(0x4d6e);
  };
  D1H1 = D0H2;
  D1H2 = D0H2;
  //D1W = D0W;
  //SWAP(D1);
  //D1W = D0W;
  //SWAP(D0);
  D0L = ((D0L & 0xffff)<<16) | ((D0L>>16) & 0xffff);
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
  default: DontDoThis(0x4db0);
  };
  D0L |= D2L;
  D0L ^= 0xffffffff;
  D0W = 0;
  D1L |= D2L;
  D1L ^= 0xffffffff;
  switch (jA1&0xffff)
  {
  case 1: D6L |= D0L; break;
  case 2: D6L |= D1L; break;
  case 3: D6L |= D1L; break;
  case 4: D7L |= D0L; break;
  case 5: D6L |= D0L; D7L |= D0L; break;
  case 6: D6L |= D1L; D7L |= D0L; break;
  case 7: D6L |= D1L; D7L |= D0L; break;
  case 8: D7L |= D1L; break;
  case 9: D6L |= D0L; D7L |= D1L; break;
  case 10: D6L |= D1L; D7L |= D1L; break;
  case 11: D6L |= D1L; D7L |= D1L; break;
  case 12: D7L |= D1L; break;
  case 13: D6L |= D0L; D7L |= D1L; break;
  case 14: D6L |= D1L; D7L |= D1L; break;
  case 15: D6L |= D1L; D7L |= D1L; break;
  case 0: break;
  default: DontDoThis(0x4de8);
  }
  StoreLong(A3+0, LoadLong(A3+0) | LE32(D6L));
  StoreLong(A3+4, LoadLong(A3+4) | LE32(D7L));
  D4W--;
  if (D4W==0) return;
  A2+=70;
  A3 += A3inc;
  goto loop;
}


// *********************************************************
//
// *********************************************************
//   TAG004bbc
void TextOutB(ui8 *dest,
              i32 destWidth,
              i32 x,
              i32 y,
              i32 color,
              i32 P6,
              const char *text)
{
  dReg D2, D3, D4, D5, D6, D7;
  i32 saveD2;
  aReg A2, A4, saveA2, saveA4;
  const char *A0;
  i32 jA0, jA1;
  A2 = NULL;
  A4  = (aReg)dest;
  D7W = sw(x);
  D6W = sw(y);
  //D5W = P5;
  D4W = sw(P6);
  A0  = text;
  if (*A0 == 0) return;
  A4 += (D6W - 4) * destWidth;
  A4 += (D7W & 0xfff0)>>1;
  D3W = (UI16)(D7W & 15);
  jA0 = color; // 16 colors
  jA1 = D4W;
tag004cce:
  for (;;)
  {
    do
    {
      D2L = 0;
      D2B = *text;
      if (D2W ==0) return;
      text++;
      A2 = Byte1278 + D2W - 32;
      if (D3W > 11) goto tag004d12;
      D4L = 6;
      D2L = 0x07ff07ff;
      D2L >>= D3W;
      D2H1 = D2W;
      D5L = 0;
      TAG004d4a(D2, D3, D4, D5, A2, A4, destWidth, jA0, jA1);
      D3W += 6;
    } while (D3W < 16);
    D3W &= 1;
    A4 += 8;
  };
tag004d12:
  D4L = 6;
  saveA4 = A4 + 8;
  saveA2 = A2;
  D2L = Long954[D3W-12];
  saveD2 = Long938[D3W-12];
  A2 = saveA2;
  D5L = 8;
  TAG004d4a(D2, D3, D4, D5, A2, A4, destWidth, jA0, jA1);
  D2L = saveD2;
  A2 = saveA2;
  A4 = saveA4;;
  D4L = 6;
  D5L = 18;
  TAG004d4a(D2, D3, D4, D5, A2, A4, destWidth, jA0, jA1);

  A4 = saveA4;
  D3W -= 10;
  goto tag004cce;



}

// *********************************************************
//
// *********************************************************
void TextToScreen(i32 xPixel,i32 yPixel,i32 P3,i32 P4,const char *P5)
{
  TextOutB(LoadPnt((ui8 *)&e.PhysicalBase),
           160,
           xPixel,
           yPixel,
           P3,
           P4,
           P5);
}


// *********************************************************
//
// *********************************************************
i16 TAG004ec0(i32 P1)
{
  aReg A1, A4;
  i16 *wA0;
  i32 i;
//;;;;;;;;;;;;;
  e.Word826[0] = sw(P1);
  ASSERT(P1<111,"P1>=111"); //Else expand table
  A4 = (aReg)data004f1c + 3 * (P1-10);
  for (i=1; i<5; i++)
  {
    wA0 = &e.Word826[i];
    A1 = A4;
    A4 += 1;
    *wA0 = (UI16)(*A1 & 0xff);
//
//
  };
  TAG004ea2(Loadpi16(&e.Pnt714[6]));
  return e.Word750[0];
//
//
//file = chaos.ftl      offset =6a48
//
}

// *********************************************************
//
// *********************************************************
i16 TAG00507e()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  Storepi16(&e.Pnt714[0], e.Word826);
  Storepi16(&e.Pnt714[1], e.Word816);
  Storepi16(&e.Pnt714[2], e.Word784);
  Storepi16(&e.Pnt714[3], e.Word750);
  Storepi16(&e.Pnt714[4], e.Word734);
  Storepi16(&e.Pnt714[5], e.Word722);
  Storepi16(&e.Pnt714[6], (i16 *)e.Pnt714);
  TAG004ec0(10);
  e.Word686 = e.Word750[0];
  return e.Word686;
}

// *********************************************************
//
// *********************************************************
i16 TAG0050d2()
{
  TAG004ec0(19);
  return 1;
}

// *********************************************************
//
// *********************************************************
i16 TAG005868(i16 *P1, i16 *P2, i16 *P3, i16 *P4)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TAG004ec0(77);
  *P1 = e.Word750[1];
  *P2 = e.Word750[2];
  *P3 = e.Word750[3];
  *P4 = e.Word750[4];
  return e.Word750[0];
}

// *********************************************************
//
// *********************************************************
void TAG00589e(i16 P1, i16 P2)
{
//;;;;;;;;;;;;;;;;
  e.Word784[0] = P1;
  e.Word734[0] = P2;
  TAG004ec0(78);
}

// *********************************************************
//
// *********************************************************
void TAG005c92(char *P1)
{ //Splits an address into two words.
  dReg D0;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D0L = (i32)P1 >> 16;
  e.Word9108 = D0W;
  e.Word9106 = (UI16)((i32)P1 & 0xffff);
}

// *********************************************************
//
// *********************************************************
void TAG005cd6(char * *P1)
{ //Reassemble a pointer from two words.
  dReg D0;
//;;;;;;;;;;;;;;;;
  D0L = e.Word9104 << 16;
  D0W = e.Word9102;
  StoreLong((pnt)P1, D0L);
  //*P1 = (pnt)D0L;
}

// *********************************************************
//
// *********************************************************
void TAG005cf6()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;
  //Trap #2 with parameter 115 does do something on the
  //Atari.  I don't know what.
  HopefullyNotNeeded(0xcd8a);


}

// *********************************************************
//
// *********************************************************
void TAG005d12(i16 *,i16 *,i16 *)
{
  DontDoThis(0x5d12);
}

// *********************************************************
//
// *********************************************************
void TAG005d8e(i16 *P1, i16 *P2, i16 *P3)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  Storepi16(&e.pw664, P1);
  Storepi16(&e.pw656, P3);
  Storepi16(&e.pw652, P3 + 45);
  e.Word9122 = 100;
  e.Word9120 = 0;
  e.Word9116 = 11;
  e.Word9110 = *P2;
  TAG005cf6();
  *P2 = e.Word9110;
  Storepi16(&e.pw664, &e.Word9098);
  Storepi16(&e.pw656, e.Word8842);
  Storepi16(&e.pw652, e.Word8330);
  // never referenced//e.Pnt660 = e.Byte8586;
}

// *********************************************************
//
// *********************************************************
void TAG005df2(i16 P1)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  e.Word9122 = 101;
  e.Word9120 = 0;
  e.Word9116 = 0;
  e.Word9110 = P1;
  TAG005cf6();
}

// *********************************************************
//
// *********************************************************
void TAG0061ba(i16 P1, i16 P2)
{
//;;;;;;;;;;;;;;;;
  e.Word9098 = P2;
  e.Word9122 = 122;
  e.Word9120 = 0;
  e.Word9116 = 1;
  e.Word9110 = P1;
  TAG005cf6();
}

// *********************************************************
//
// *********************************************************
void TAG0061e2(i16 P1)
{
//;;;;;;;;;;;;;;;;;;;
  e.Word9122 = 123;
  e.Word9120 = 0;
  e.Word9116 = 0;
  e.Word9110 = P1;
  TAG005cf6();
}

// *********************************************************
//
// *********************************************************
void TAG006202(i16 P1, i16 *P2, i16 *P3, i16 *P4)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  e.Word9122 = 124;
  e.Word9120 = 0;
  e.Word9116 = 0;
  e.Word9110 = P1;
  TAG005cf6();
  *P2 = e.Word8842[0];
  *P3 = e.Word8330[0];
  *P4 = e.Word8328[0];
}

// *********************************************************
//
// *********************************************************
void TAG00623a(i16 P1, char *P2, char **P3)
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  TAG005c92(P2);
  e.Word9122 = 125;
  e.Word9120 = 0;
  e.Word9116 = 0;
  e.Word9110 = P1;
  TAG005cf6();
  TAG005cd6(P3);
}

// *********************************************************
//
// *********************************************************
void TAG00626e(i16 P1, char *P2, char * *P3)
{
//;;;;;;;;;;;;;;;;;;
  TAG005c92(P2);
  e.Word9122 = 126;
  e.Word9120 = 0;
  e.Word9116 = 0;
  e.Word9110 = P1;
  TAG005cf6();
  TAG005cd6(P3);
}

// *********************************************************
//
// *********************************************************
//  TAG0064b4
pnt FormatInteger(pnt rslt, ui32 num, i16 hex)
{
  dReg D5, D6;
  aReg A3, A4;
  static char digits[] = "0123456789ABCDEF";
  char b_12[12];
  A4 = rslt;
  D6L = 0;
  A3 = (aReg)b_12;
  D5W = (UI16)(hex ? 16 : 10);
  do
  {
    *(A3++) = digits[num % D5W];
    num /= D5W;
    D6W++;
  } while (num != 0);
  while ((D6W--) != 0)
  {
    *(A4++) = *(--A3);
  };
  return A4;
}


// *********************************************************
//
// *********************************************************
//  TAG0065e0
i32 atari_sprintf(char *dst, const char* fmt, i32 V1, i32 V2, i32 V3)
{ // returns length of result
  dReg D5;
  aReg A2, A3, A4;
  bool fmtSpec, lSpec, uSpec, hex;
  pnt pnt_8;
  pnt pnt_4;
  i32 stack[]={V1,V2,V3};
  pnt_4=(pnt)stack;
//;;;;;;;;;;;;;;;;;;
  A4 = (aReg)dst;
  pnt_8 = A4;
  A3 = (aReg)fmt;
  //pnt_4 = (pnt)&V1;
  while (*A3 != 0)
  {
    if (*A3 != '%')
    {
      *(A4++) = *(A3++);
      continue; // That was rather the easier case.
    };

    A3++;
    fmtSpec=true;
    lSpec = false;
    uSpec = false;
    while (fmtSpec)
    {
      switch (*(A3++))
      {
      case 'l':
        lSpec = true;
        break;
      case 'u':
        uSpec = true;
        break;
      case 'd':
      case 'x':
        hex = (*A3 == 'x');
        if (lSpec)
        {
          D5L = LoadLong(pnt_4);//next value
          pnt_4 += 4;
          if ((!uSpec) && (!hex))
          {
            if (D5L & 0x80000000)
            {
              *(A4++) = '-';
              D5L = -D5L;
            }
          };
        }
        else
        {
          D5L = LoadLong(pnt_4) & 0xffff;
          pnt_4+=4;
          if ((!uSpec) && (!hex))
          {
            if (D5W & 0x8000)
            {
              *(A4++) = '-';
              D5W = sw(-D5W);
            };
          };
        };
        A4 = FormatInteger(A4, D5L, hex);
        fmtSpec = false;
        break;
      case 's':
        A2 = (aReg)LoadPnt(pnt_4);
        pnt_4 += 4;
        while (*A2 != 0)
        {
          *(A4++) = *(A2++);
        };
        fmtSpec = false;
        break;
      default:
        *(A4++) = *A3;
        fmtSpec = false;
      }; //case
    }; //while (fmtSpec)
//
//
  };
  *A4 = 0;
  return A4-pnt_8;
}

// *********************************************************
//
// *********************************************************
//  TAG006700
i32 GetPixel(pnt P1,i16 x,i16 y,i16 width)
{
  dReg D0, D1, D2, D3, D5;
  aReg A0;
//;;;;;;;;;;;;;;;;;;
  D1W = x;
  D2W = (UI16)(D1W & 15);
  A0 = P1;
  D0L = y * width;
  D1W = (UI16)((D1W / 2) & 0xf8);
  D0W = sw(D0W + D1W);
  A0 += D0W;
  D5W = sw(15 - D2W);
  D0W = (UI16)(((LE16(wordGear(A0+0))>>D5W)<<0) & 1);
  D1W = (UI16)(((LE16(wordGear(A0+2))>>D5W)<<1) & 2);
  D2W = (UI16)(((LE16(wordGear(A0+4))>>D5W)<<2) & 4);
  D3W = (UI16)(((LE16(wordGear(A0+6))>>D5W)<<3) & 8);
  return D3W | D2W | D1W | D0W;
}

// *********************************************************
//
// *********************************************************
ui8 *TAG0073b0(ui8 *P1)
{
  dReg D0;
  aReg A0, A4;
//;;;;;;;;;;;;;;;;;;;;;;
  A0 = (aReg)LoadPnt(P1);
  A4 = A0 - 2;
  D0L = 0;
  D0W = wordGear(A4);
  D0L += 12;
  return (ui8 *)A4 - D0L;
}

// *********************************************************
//
// *********************************************************
ui8 *TAG0073d4(pnt P1)
{
  dReg D7;
  aReg A3, A4;
//;;;;;;;;;;;;;;;;;;;
  A4 = P1;
  for (D7L=0, A3=(pnt)&e.pDBank422;
       D7W<e.Word424;
       D7W++, A3+=18)
  {
    if ((ui8 *)A4 >= LoadPnt(A3))
    {
      if ((ui8 *)A4 <= LoadPnt(A3) + LoadLong(A3+6)) break;
    };
//
//
  };
  if (D7W == e.Word424) A3=NULL;
  return (ui8 *)A3;
}

// *********************************************************
//
// *********************************************************
void TAG007416(char *P1,char *P2)
{
  aReg A2, A3, A4;
//;;;;;;;;;;
  A4 = (aReg)P1;
  A2 = (aReg)LoadPnt(A4+4);
  A3 = (aReg)LoadPnt(A4+8);
  if (A2 != NULL)
  {
    StorePnt(A2+8, A3);
  }
  else
  {
    StorePnt((ui8 *)P2+10, (ui8 *)A3);
  };
  if (A3 != NULL) StorePnt(A3+4, A2);
}

// *********************************************************
//
// *********************************************************
void TAG00744c(char *P1,char *P2)
{
  dReg D7;
  aReg A2, A3, A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = (aReg)P1;
  A3 = (aReg)LoadPnt((ui8 *)P2+10);
  A2 = NULL;
  D7L = LoadLong(A4) & 0x7fffffff;
  while (A3 != NULL)
  {
    if (D7L <= (LoadLong(A3) & 0x7fffffff))
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
    StorePnt((ui8 *)P2+10, (ui8 *)A4);
  };
  if (A3 != NULL)
  {
    StorePnt(A3+4, A4);
  };
  StorePnt(A4+8, A3);
  StorePnt(A4+4, A2);
}

void TAG0074ae(char *P1,i32 P2,char *P3)
{
  dReg D6, D7;
  aReg A3, A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = (aReg)P1;
  D7L = P2;
  A3 = (aReg)P3;
  D6L = D7L | 0x80000000;
  StoreLong(A4, D6L);
  StoreLong(A4+D7L-4, D6L);
  TAG00744c((char *)A4, (char *)A3);
}

// *********************************************************
//
// *********************************************************
void TAG0074ea(pnt P1, i32 P2, pnt P3)
{
  dReg D6, D7;
  aReg A0, A3, A4;
//;;;;;;;;;;;;;;
  A4 = P1;
  D7L = P2;
  A3 = P3;
  D6L = LoadLong(A4-4);
  if (D6L & 0x80000000)
  {
    D6L &= 0x7fffffff;
    A4 -= D6L;
    TAG007416((char *)A4, (char *)A3);
    D7L += D6L;
  };
  A0 = A4 + D7L;
  D6L = LoadLong(A0);
  if (D6L & 0x80000000)
  {
    D6L &= 0x7fffffff;
    TAG007416((char *)A4 + D7L, (char *)A3);
    D7L += D6L;
  };
  TAG0074ae((char *)A4, D7L, (char *)A3);
}

// *********************************************************
//
// *********************************************************
ui8 *TAG007552(i32 P1, ui8 *P2, i16 *P3)
{
  dReg D6, D7;
  aReg A2, A3, A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  D7L = P1;
  A4 = (aReg)P2;
  D7L |= 0x80000000;
  A3 = (aReg)LoadPnt(A4+10);
  A2 = NULL;
  while (A3 != NULL)
  {
    D6L = LoadLong(A3);
    if (D6L < D7L) break;
    if (D6L - D7L <= 34) A2 = A3;
    A3 = (aReg)LoadPnt(A3+8);
//
  };
  if (A2 == NULL)
  {
    A3 = (aReg)LoadPnt(A4+10);
    if (D7L > LoadLong(A3)) return (ui8 *)A2;
    A2 = A3;
    *P3 = 0;
  }
  else
  {
    *P3 = 1;
  };
  return (ui8 *)A2;
}

// *********************************************************
//
// *********************************************************
i16 TAG007904(char *,i32)
{
  DontDoThis(0x7904); return 0;
}

ui8 *TAG007a1a(i16 P1)
{
  dReg D6, D7;
  aReg A2, A3, A4;
  STRUCT6  s6_12;
//;;;;;;;;;;;;;;;;;;;;;;
  D7W = P1;
  A4 = NULL;
  D6L = 4 * D7UW + 20;
  A3 = (aReg)LoadPnt((ui8 *)&e.Pnt430);
  s6_12.word4 = 1024;
  s6_12.int0 = D6L;
  A2 = (aReg)TAG007bba(&s6_12);
  if (A2 != NULL)
  {
    A4 = (aReg)TAG007cc6(D6L, 1024, (char *)A2);
    if (A4 != NULL)
    {
      A4[4] = -1;
      A4[5] = -128;
      StoreLong(A4+6, 0);
      wordGear(A4+10) = D7W;
      wordGear(A4+12) = 0;
      wordGear(A4+14) = 0;
      ClearMemory((ui8 *)A4+16, D6L-20);
      if (A3 != NULL)
      {
        while (LoadLong(A3+6) != 0)
        {
          A3 = (aReg)LoadPnt(A3+6);
        };
        StorePnt(A3+6, A4);
      }
      else
      {
        StorePnt((ui8 *)&e.Pnt430, A4);
      };
    };
  };
  return (ui8 *)A4;
}

// *********************************************************
//
// *********************************************************
char *TAG007ac0(pnt *P1)
{
  dReg D0, D5, D6, D7;
  aReg A2, A3;
  pnt  *pA4;
  A3 = (pnt)0xbaddade; //debugging
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  pA4 = P1;
  D5L = 0;
  for (A2 = (aReg)LoadPnt((ui8 *)&e.Pnt430); A2 != NULL; A2=(aReg)LoadPnt(A2+6))
  {
    if (wordGear(A2+12) >= wordGear(A2+10)) continue;
    D6W = wordGear(A2+14);
    D0L = (D6UW << 2) & 0xffff;
    A3 = A2 + D0L + 16;
    for (D7W = wordGear(A2+10); (D7W--)!=0; )
    {
      if (wordGear(A2+10) == D6W)
      {
        D6L = 0;
        A3 = A2 + 16;
      };
      if (LoadPnt(A3) == NULL)
      {
        wordGear(A2+14) = D6W;
        wordGear(A2+12)++;
        D5W = 1;
        break;
      };
      D6W++;
      A3 += 4;
//
    };
    if (D5W) break;
//
//
  };
  if (D5W == 0)
  {
    A2 = (aReg)TAG007a1a(e.Word426);
    if (A2 != NULL)
    {
      A3 = A2 + 16;
      wordGear(A2+12)++;
      D5W = 1;
    };
  };
  if (D5W == 0) A3 = NULL;
  *pA4 = A2;
  ASSERT(A3 != (pnt)0xbaddade,"A3 not initialized");
  return (char *)A3;
}

// *********************************************************
//
// *********************************************************
void TAG007b68(pnt P1)
{
  dReg D0;
  aReg A0, A3, A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = P1;
  for (A3=(aReg)LoadPnt((ui8 *)&e.Pnt430); A3!=NULL; A3=(aReg)LoadPnt(A3+6))
  {
    if (A3 > A4) continue;
    D0L = 4 * (UI16)(wordGear(A3+10));
    A0 = A3 + D0L + 20;
    if (A4 >= A0) continue;
    StoreLong(A4, 0);
    wordGear(A3+12)--;
    D0L = A4 - A3;
    D0L -= 16;
    D0L /= 4;
    wordGear(A3+14) = D0W;
    break;
//
//
  };
}

// *********************************************************
//
// *********************************************************
char *TAG007bba(STRUCT6 *P1)
{
  dReg D0, D1, D3, D4, D5, D6, D7;
  aReg A2, A3;
  STRUCT6 *s6A4;
//;;;;;;;;;;;;;;;;
  s6A4 = P1;
  if (s6A4->word4 & 0x01)
  {
    D7L = 1;
    A3 = (aReg)&e.pDBank422;
  }
  else
  {
    D7L = -1;
    A3 = (pnt)&e.pDBank422 + 18*(e.Word424-1);
  };
  D5L = s6A4->int0;
  A2 = A3;
  for (;;)
  {
    D4L = 0;
    D3L = 0;
    for (D6L=0; D6W<e.Word424; D6W++, A3+=18*D7W)
    {
      D0W = sw(wordGear(A3+4) & s6A4->word4);
      D1W = (UI16)(s6A4->word4 & 0x7f8);
      if (D0W != D1W) continue;
      D4L = 1;
      D0L = LoadLong(LoadPnt(A3+10)) & 0x7fffffff;
      if (D0L >= D5L)
      {
        D3L = 1;
        break;
      };
//

    }; /// for (D6W)
    if (D4W == 0)
    {
      TAG008788(1, 0x010d0005, s6A4->word4 & 0xffff);
    };
    if (D3W == 0)
    {
      A3 = A2;
      for (D6L=0; D6W<e.Word424; D6W++, A3+=18*D7W)
      {
        D0W = sw(wordGear(A3+4) & s6A4->word4);
        D1W = (UI16)(s6A4->word4 & 0x07f8);
        if (D0W != D1W) continue;
        if (TAG007904((char *)A3, D5L) == 0) break;
//
//
      };
      if (e.Word424 == 0)
      {
        D0L = TAG008788(0, 0x010d0001, s6A4->word4 & 0xffff);
        if (D0L != 0x010d0001)
        {
          A3 = A2;
          continue;
        };
      };
    };
    return (char *)A3;
  } ;
}

// *********************************************************
//
// *********************************************************
ui8 *TAG007cc6(i32 P1, i16 P2, char *P3)
{
  dReg D5, D6, D7;
  aReg A0, A2, A3, A4;
  i16 w_2;
//;;;;;;;;;;;;;;;;;;;;;
  D7L = P1;
  D6W = P2;
  A4 = (aReg)P3;
  A3 = (aReg)TAG007552(D7L, (ui8 *)A4, &w_2);
  if (A3 != NULL)
  {
    TAG007416((char *)A3, (char *)A4);
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
      TAG0074ae((char *)A2, D5L, (char *)A4);
    };
    StoreLong(A4+14, LoadLong(A4+14) - D7L);
    StoreLong(A3, D7L);
    A0 = A3 + D7L;
    StoreLong(A0-4, D7L);
  };
  return (ui8 *)A3;
}


// *********************************************************
//
// *********************************************************
ui8 *TAG007d4c(STRUCT12 *P1)
{
  dReg D0, D7;
  aReg A0, A2, A3;
  STRUCT12 *s12A4;
  pnt   pnt_8;
  char *pnt_4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  s12A4 = P1;
  D0L = s12A4->word6 & 0xffff;
  D7L = D0L + s12A4->long0 + 18;
  if (D7L & 1) D7L++;
  s12A4->long0 = D7L;
  A2 = (aReg)TAG007ac0(&pnt_8);
  if (A2 == NULL) return NULL;
  pnt_4 = TAG007bba((STRUCT6 *)s12A4);

  if (   (pnt_4 != NULL)
      && ((A3 = (aReg)TAG007cc6(D7L, s12A4->word4, pnt_4))!=NULL) )
  {
    StorePnt(A2, A3 + (s12A4->word6&0xffff) + 14);
    A3[4] = 0;
    A3[5] = (UI8)(s12A4->word4 & 7);
    wordGear(A3+10) = s12A4->word4;
    StorePnt(A3+6, A2);
    A3 += 12;

    if (   (s12A4->fnc8 != NULL)
        && (s12A4->word6 != 0)  )
    {
      (*s12A4->fnc8)(A3);
    };
    A0 = A3 + (s12A4->word6 & 0xffff);
    wordGear(A0) = s12A4->word6;
    return (ui8 *)A2;
  }
  else
  {
    wordGear(pnt_8+12)--;
    return NULL;
  };
}

// *********************************************************
//
// *********************************************************
void TAG007e16(ui8 *P1,  ui8 *P2)
{
  dReg D7;
  aReg A3, A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = (aReg)P1;
  A3 = (aReg)P2;
  D7L = LoadLong(A4);
  StoreLong(A3+14,LoadLong(A3+14) + D7L);
  TAG0074ea(A4, D7L, A3);
}

// *********************************************************
//
// *********************************************************
i32 TAG007e42(i16 P1, i16 P2)
{
  dReg D5, D7;
  aReg A0, A3, A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //D7L = MALLOC(-1);
  D7L = 240000;
  if (D7L < 120000)
  {
    DontDoThis(0x7e62);


  };
  D7L = (D7L & -2) - 4000;
  A4 = (aReg)MALLOC(D7L);
  if (A4==NULL)
  {
    DontDoThis(0x7e96);


  };
  StorePnt((ui8 *)&e.pAllocDBank434, (ui8 *)A4);
  D7L &= -2;
  e.Word424 = 1;
  StorePnt((ui8 *)&e.pDBank422, (ui8 *)A4);
  StoreLong((pnt)&e.DBankLen416, D7L);
  e.Word418 = 2040;
  for (D5L=0, A3=(pnt)&e.pDBank422;
       D5W<e.Word424;
       A3+=18, D5W++)
  {
    A4 = (aReg)LoadPnt(A3);
    StoreLong(A4, 0);
    A0 = A4 + LoadLong(A3+6);
    StoreLong(A0-4, 0);
    A4 += 4;
    StorePnt(A3, A4);
    StoreLong(A3+6, LoadLong(A3+6) - 8);
    StoreLong(A3+14, LoadLong(A3+6));
    StoreLong(A3+10, 0);
    TAG0074ae((char *)A4, LoadLong(A3+14), (char *)A3);
//
//
  };
  e.Word426 = P2;
  StorePnt((ui8 *)&e.Pnt430, (ui8 *)NULL);
  if (TAG007a1a(P1) == NULL)
  {
    DontDoThis(0x7f34);


  };
  return 0;
}

// *********************************************************
//
// *********************************************************
void TAG007f54()
{
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  MFREE(LoadPnt((ui8 *)&e.pAllocDBank434));
}

// *********************************************************
//
// *********************************************************
ui8 *TAG007f64(i32 P1, i16 P2)
{
  STRUCT12 s_12;
//;;;;;;;;;;;;;;;;;;;;;;
  s_12.long0 = P1;
  s_12.word4 = P2;
  s_12.word6 = 0;
  s_12.fnc8 = NULL;
  return TAG007d4c(&s_12);
}

// *********************************************************
//
// *********************************************************
i32 TAG007f92(ui8 *P1)
{
  dReg D7;
  aReg A3, A4;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  A4 = (aReg)P1;
  D7L = TAG0080e0((ui8 *)A4);
  if (D7L == 0)
  {
    A3 = (aReg)TAG0073b0((ui8 *)A4);
    TAG007e16((ui8 *)A3, TAG0073d4(A3));
  };
  if ( (D7L == 0) || (D7L == 0x010d0003) )
  {
    TAG007b68(A4);
    D7L = 0;
  };
  return D7L;
}

// *********************************************************
//
// *********************************************************
ui8 *TAG007fe8(ui8 *P1)
{
  dReg D7;
  aReg A2, A3, A4;
//;;;;;;;;;;;;;;
  A4 = (aReg)P1;
  A2 = NULL;
  D7L = TAG0080e0((ui8 *)A4);
  if (D7L == 0)
  {
    A3 = (aReg)TAG0073b0((ui8 *)A4);
    A3[4]++;
    A2 = (aReg)LoadPnt(A4);
  };
  return (ui8 *)A2;
}

// *********************************************************
//
// *********************************************************
void TAG00801e(ui8 *P1)
{
  TAG0073b0(P1)[4]--;
}

// *********************************************************
//
// *********************************************************
i32 TAG0080e0(ui8 *P1)
{
  dReg D7;
  aReg A3, A4;
  D7L = 0xbaddade;//debugging
//;;;;;;;;
  A4 = (aReg)P1;
  if ( (A4 == NULL) || (LoadLong(A4)==0) )
  {
    TAG008788(1, 0x010d0002, (ui32)A4);
  }
  else
  {
    if (LoadLong(A4) == -1)
    {
      D7L = 0x010d0003;
    }
    else
    {
      A3 = (aReg)TAG0073b0((ui8 *)A4);
      if (TAG0073d4(A3) == NULL)
      {
        D7L = 0x010d0002;
      }
      else
      {
        D7L = 0;
      };
    };
  };
  ASSERT(D7L != 0xbaddade,"D7 not initialized");
  return D7L;
}

// *********************************************************
//
// *********************************************************
//   TAG00858c
void MemoryMove(ui8 *src,
                ui8 *dst,
                i16 , //garbage
                i16 , //garbage
                i32 len)
{
  MemMove(src, dst, len);
}

// *********************************************************
//
// *********************************************************
i32 TAG008788(i16,i32,ui32)
{
  DontDoThis(0x8788); return 0;
//;;;;;;;;;;;;;



//file = chaos.ftl      offset =a1bc


//file = chaos.ftl      offset =a1dc



}

void EditCharacterName(char key)
{
	char* name;
	i32 len;
	i32 max_len;
	i32 position;

	name = (char *)LoadPnt((ui8 *)&e.Pnt24812) + 800*e.ActualCharacter + 772;
	if(e.EditBoxSelected == 1) //TITLE
	{
		 name += 8;
		 max_len = 18;
	}
	else
		max_len = 7;
	len = strlen(name);

	// first check for backspace key
	if(key==8)
	{
		if(e.EditBoxPosition == 0)
			return;
		if(e.EditBoxPosition  == len ) // deleting from the end
		{
			name[--len] = 0;
		}
		else // deleting from the middle
		{ // insert in the middle
			i32 i;
			for(i = e.EditBoxPosition;i < max_len;i++)
				name[i-1] = name[i];
			name[len] = 0;
		}
		position = e.EditBoxPosition - 1;
	}
	else
	{
		if(len == max_len) // maximum name lenght
			return;

		if(key!=32)
			key &= ~32; // uppercase
		if ((key!=32) && ((key<'A') || (key>'Z'))) // bad key pressed
			return;

		if(e.EditBoxPosition == len) // appending to the end
			name[len++] = key;
		else
		{ // insert in the middle
			i32 i;
			for(i=max_len;i>=e.EditBoxPosition;i--)
				name[i+1] = name[i];
			name[e.EditBoxPosition] = key;
		}
		position = e.EditBoxPosition + 1;
	}

	if(e.EditBoxSelected == 0)
		FillCharacterNameEditBox(name, 13, 7);
	else
		FillCharacterNameEditBox(name, 14, 19);

	DrawNameTitleCarret(e.EditBoxSelected, (i16)position);
}
