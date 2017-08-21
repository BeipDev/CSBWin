// CSB.h : main header file for the CSB application
//

#ifdef __GNUC__
#define ALIGN4 __attribute__((aligned(4)))
#else
#define ALIGN4
#endif

//#ifdef _DEBUG
#define TraceFlash(msg) traceFlash(msg)
void traceFlash(const char *msg, i32 x=-1, i32 y=-1);
void MemMove(ui8 *, ui8 *, int);
//#else
//#define TraceFlash(msg) ;
//#endif

#pragma pack(1)

#define MAX_LOADSTRING 100

#ifdef _LINUX
extern char szCSBVersion[MAX_LOADSTRING];
#else
extern char szCSBVersion[MAX_LOADSTRING];
#endif

#define FLOOR_BITMAP_SIZE 7840
#define CEILING_BITMAP_SIZE 3248
//#define TEMP_BITMAP_SIZE 10880
void AllocateTempBitmap(int size);
extern unsigned char *tempBitmap;
extern int tempBitmapSize;



enum GAMESTATE
{
  GAMESTATE_202 = 202,
  GAMESTATE_ResumeSavedGame   = 0,
  GAMESTATE_AtPrisonDoor      = 99,
  GAMESTATE_EnterPrison       = 1,
};

enum COLOR
{
  COLOR_7  =  7,
  COLOR_8  =  8,
  COLOR_11 = 11,
  COLOR_13 = 13
};

enum MALLOCID
{
  MALLOC001 = 1,
  MALLOC002,
  MALLOC003,
  MALLOC004,
  MALLOC005,
  MALLOC006,
  MALLOC007,
  MALLOC008,
  MALLOC009,
  MALLOC010,
  MALLOC011,
  MALLOC012,
  MALLOC013,
  MALLOC014,
  MALLOC015,
  MALLOC016,
  MALLOC017,
  MALLOC018,
  MALLOC019,
  MALLOC020,
  MALLOC021,
  MALLOC022,
  MALLOC023,
  MALLOC024,
  MALLOC025,
  MALLOC026,
  MALLOC027,
  MALLOC028,
  MALLOC029,
  MALLOC030,
  MALLOC031,
  MALLOC032,
  MALLOC033,
  MALLOC034,
  MALLOC035,
  MALLOC036,
  MALLOC037,
  MALLOC038,
  MALLOC039,
  MALLOC040,
  MALLOC041,
  MALLOC042,
  MALLOC043,
  MALLOC044,
  MALLOC045,
  MALLOC046,
  MALLOC047,
  MALLOC048,
  MALLOC049,
  MALLOC050,
  MALLOC051,
  MALLOC052,
  MALLOC053,
  MALLOC054,
  MALLOC055,
  MALLOC056,
  MALLOC057,
  MALLOC058,
  MALLOC059,
  MALLOC060,
  MALLOC061,
  MALLOC062,
  MALLOC063,
  MALLOC064,
  MALLOC065,
  MALLOC066,
  MALLOC067,
  MALLOC068,
  MALLOC069,
  MALLOC070,
  MALLOC071,
  MALLOC072,
  MALLOC073,
  MALLOC074,
  MALLOC075,
  MALLOC076,
  MALLOC077,
  MALLOC078,
  MALLOC079,
  MALLOC080,
  MALLOC081,
  MALLOC082,
  MALLOC083,
  MALLOC084,
  MALLOC085,
  MALLOC086,
  MALLOC087,
  MALLOC088,
  MALLOC089,
  MALLOC090,
  MALLOC091,
  MALLOC092,
  MALLOC093,
  MALLOC094,
  MALLOC095,
  MALLOC096,
  MALLOC097,
  MALLOC098,
  MALLOC099,
  MALLOC100,
  MALLOC101,
  MALLOC102,
  MALLOC103,
  MALLOC104,
  MALLOC105,
  MALLOC106,
  MALLOC107,
  MALLOC108,
  MALLOC109,
  MALLOC110,
  MALLOC111,
  MALLOC112,
  MALLOC113,
  MALLOC114,
  MALLOC115,
  MALLOC116,
  MALLOC117,
  MALLOC118,
  MALLOC119,
  MALLOC120,
  MALLOC121,
  MALLOC122,
  MALLOC123,
  MALLOC124,
};

class SKIN_CACHE
{
  i32  m_level;           // -1 if cache is empty;
  i8   m_columns[16][64]; //Records from Expool.
  i8   m_size[16];        //Record sizes; (-1 need to look) (0 no record exists)
  ui8 *defaultSkins;
  i32  Load(i32 level, i32 x);
public:
  SKIN_CACHE(void){m_level=-1;defaultSkins=NULL;};
  ~SKIN_CACHE(void){};
  ui8  GetSkin(i32 level, i32 x, i32 y);
  void SetSkin(i32 level, i32 x, i32 y, i8 skinNum);
  ui8  GetDefaultSkin(i32 level);
  void Cleanup(void){if (defaultSkins!=NULL)UI_free(defaultSkins);defaultSkins=NULL;m_level=-1;};
};

extern SKIN_CACHE skinCache;
extern bool drawAsSize4Monsters;
extern bool invisibleMonsters;


class TEMPORARY_MEMORY
{
public:
  ui8 *m;
  TEMPORARY_MEMORY(void){m=NULL;};
  ~TEMPORARY_MEMORY(void){if(m!=NULL)UI_free(m);};
};

i16  CLOSE(i32 handle);
class TEMPORARY_FILE
{
public:
  i32 f;
  TEMPORARY_FILE(void){f=-1;};
  ~TEMPORARY_FILE(void){Cleanup();};
  void Cleanup(void){if(f>=0)CLOSE(f);f=-1;};
};

class OVERLAYDATA
{
public:
  i32 m_overlayNumber;
  ui8 *m_overlay;
  ui32 *m_overlayPalette;
  i16  *m_table;
  i32  m_p1, m_p2, m_p3, m_p4;
  bool m_change;
  OVERLAYDATA(void);
  ~OVERLAYDATA(void);
  bool ReadOverlay(i32 overlayNumber);
  void Mirror(void);
  void CreateOverlayTable(i16 *palettte, bool useOverlay);
  void Allocate(void);
  void Cleanup(void);
};
extern OVERLAYDATA currentOverlay;

class SOUNDDATA
{
public:
  i32  m_soundNum;
  i32  m_size;    // Size of m_sound.
  ui8 *m_sound;   // The uncompressed PCM data
  SOUNDDATA(void);
  ~SOUNDDATA(void);
  bool ReadSound(i32 soundNum);
  void Allocate(i32 numSample);
  void Cleanup(void);
  char *Decode(i32 volume); //Allocate memory and create an internal .wav file
};
extern SOUNDDATA currentSound;

class SECTIONHEADER
{
public:
  unsigned int type;
  unsigned int id;
  unsigned int compressedSize;
  unsigned int uncompressedSize;
  unsigned int fileOffset;
  char name[16];
};

class OVLDECODE
{
  ui32 (*m_getCodeword)(void);
  ui32 *m_codes;
  ui8  *m_chars;
  ui32  m_bitsRemaining;
  ui32  m_codeword;
  ui32  m_tableLen;
  ui32  m_maxTableLen;
  ui32  m_codeSize;
  ui8  *m_stack;
  ui32  m_stklen;
  ui32  m_maxstack;
  ui32  m_a, m_b, m_c, m_ch;
  void  EnlargeStack(void);
  void  EnlargeTable(void);
public:
  OVLDECODE(ui32 getCodeword(void));
  ~OVLDECODE(void);
  bool GetBytes(void *buf, ui32 num);
};

enum CSB_GRAPHICTYPE
{
  CGT_ViewportOverlay         = 0,
  CGT_Portrait                = 1,
  CGT_Sound                   = 2,
  CGT_Backgrounds             = 3,
  CGT_OverlayPalette          = 4,
  CGT_AlternateMonsterGraphic = 5,
  CGT_WallDecoration          = 6,
  CGT_FloorDecoration         = 7,
};

typedef struct {
        ui32      biSize;
        i32       biWidth;
        i32       biHeight;
        ui16      biPlanes;
        ui16      biBitCount;
        ui32      biCompression;
        ui32      biSizeImage;
        i32       biXPelsPerMeter;
        i32       biYPelsPerMeter;
        i32       biClrUsed;
        ui32      biClrImportant;
} CSB_BITMAPINFOHEADER;


typedef struct {
        ui16    bfType;
        ui32    bfSize;
        ui16    bfReserved1;
        ui16    bfReserved2;
        ui32    bfOffBits;
} CSB_BITMAPFILEHEADER;


ui8 *ReadCSBgraphic(CSB_GRAPHICTYPE type,
                    ui32 id,
                    ui32 minimumSize,
                    ui32 *actualSize,
                    bool mustExist,
                    i32  mallocID);

struct BACKGROUND_MASK
{
  ui16  srcX;           // In pixel units.  Must be multiple of 16
  ui16  srcY;           // In pixel units
  i16   dstX;           // In pixel units
  ui16  dstY;           // In pixel units
  ui16  width;          // In pixel units
  ui16  height;         // In pixel units
  ui16 mask[1];
};

struct AFFINEMASK
{
  i32   Cxx;
  i32   Cxy;
  i32   Cxc;
  i32   Cyx;
  i32   Cyy;
  i32   Cyc;
  i32   srcX;
  i32   srcY;
  i32   srcWidth;
  i32   srcHeight;
  i32   dstWidth;
  i32   dstHeight;
};

struct BACKGROUND_GRAPHIC
{
  ui32 id;
  i32  size;
  ui8 *data;
};

class BACKGROUND_LIB
{
private:
  BACKGROUND_GRAPHIC *backgroundGraphics;
  i32 size;  // Size of backgroundGraphicsArray.
  i32 num;
  i32 sequence;
  i32  FindGraphicInCache(ui32 graphicID, bool mustExist);
  i32  GetBackgroundGraphic(ui32 graphicID, 
                             i32 minimumSize, 
                             ui32 *actualSize, 
                             bool mustExist);
  i32  BackgroundGraphicExists(ui32 graphicID, 
                                i32 minimumSize, 
                                ui32 *actualSize, 
                                bool mustExist);
  i32  CreateBackgroundGraphic(ui32 graphicID, 
                               ui32 graphicSize);
public:
  BACKGROUND_LIB(void);
  ~BACKGROUND_LIB(void);
  void Cleanup(void);
  //ui8  *GetSkins(i32 level); //levelwidth * level height
  ui16 *GetSkinDef(ui32 skinNum,
                   ui32 minimumSize,
                   ui32 *skinDefSize);
  BACKGROUND_MASK  *GetMask(ui32 graphicID, ui32 maskNumber, ui32 minimumSize);
  BACKGROUND_MASK  *MaskExists(ui32 graphicID, ui32 maskNumber);
  ui32 *GetBitmap(ui32 graphicID, ui32 minimumSize, ui32 *bitmapSize);
  void DumpGraphic(i32 graphicID, i32 maskNum, char *extension);
  void InsertBitmap(ui32 ID, ui32 size, ui32 *address);
  BACKGROUND_MASK *CreateNewMask(ui32 ID, ui32 num, BACKGROUND_MASK *pMask);
  AFFINEMASK *CreateNewAffineMask(ui32 ID, ui32 num, AFFINEMASK *paffine);
  void CreateVirtualMask(BACKGROUND_MASK *pOld, BACKGROUND_MASK *pNew, AFFINEMASK *paffine);
  void CreateVirtualBitmap(ui32        *pGraphic, 
                           i32          graphicSize, 
                           i32          virtualGraphicID, 
                           AFFINEMASK  *pMask);
  i32  GetDecorationBitmap(ui32 decorationID,
                            ui32 decorationNumber,
                            ui32 locationNumber,
                            ui16 **pBitmap,
                            ui8  **pLocation,
                            ui32 *actualSize);
  ui16 GetSkinID(ui32 skinNum, ui32 IDnum);
  ui16 *GetCode(ui32 skinNum);
  ui16 GetWallGraphicID(ui32 skinNum);
  ui16 GetFloorGraphicID(ui32 skinNum);
  ui16 GetMiddleGraphicID(ui32 skinNum);
  ui16 GetCeilingGraphicID(ui32 skinNum);
  ui16 GetWallDecorationID(ui32 skinNum, ui32 decorationNum);
  ui16 GetWallMaskID(ui32 skinNum);
  ui16 GetFloorMaskID(ui32 skinNum);
  ui16 GetMiddleMaskID(ui32 skinNum);
  ui16 GetCeilingMaskID(ui32 skinNum);
};

extern BACKGROUND_LIB backgroundLib;

struct LOCATIONREL
{
  i32 l;
  i32 x;
  i32 y;
  i32 p;
public:
  LOCATIONREL(void){l=-1;};
  LOCATIONREL(i32 L, i32 X, i32 Y, i32 P=0)
  {
    l=L; x=X; y=Y; p=P;
  };
  ui32 Integer(void) const; //Pack in a single 18-bit integer.
  LOCATIONREL *Integer(i32 i); //Unpack from 18-bit integer.
  bool IsValid(void) const;
};


struct MONSTERMOVEFILTERLOCATION
{
  LOCATIONREL locr;
  ui16        maxDistance;
  ui16        partyLevelOnly;
  RN          filterObj;
};

class MONSTERMOVEFILTERCACHE
{
  MONSTERMOVEFILTERLOCATION mmfloc[64];
  MONSTERMOVEFILTERLOCATION global;
public:
  MONSTERMOVEFILTERCACHE(void);
  ~MONSTERMOVEFILTERCACHE(void);
  MONSTERMOVEFILTERLOCATION *GetLocation(i32 level);
  void Clear(void);
};

extern MONSTERMOVEFILTERCACHE monsterMoveFilterCache;


struct EXTENDEDFEATURESBLOCK
{
  char sentinel[20];
  i32    dataMapLength;
  ui32   dataTypeMapChecksum;
  ui32   dataIndexMapChecksum;
  ui32   extendedFeaturesChecksum;
  char   version;
  char   flags;
enum FLAGS
{
  LevelDSAInfoPresent = 0x01,
  SimpleEncryption    = 0x04,
  IndirectText        = 0x08,
  AutoRecord          = 0x10,
  ExpandedPortraits   = 0x20,
  DMRules             = 0x40,  // Designer-specified
  BigActuators        = 0x80
};
  ui16      numDSA;
  ui32      editingOptions;
  ui32      gameInfoSize; // not including the trailing which is not written
  ui32      cellFlagArraySize;
  ui32      graphicsSignature1;
  ui32      graphicsSignature2;
  ui32      spellFilterLocation;
                    //Bit 31 set if location is valid.
  ui32      extendedFlags; //Why did I think 8 would suffice?
enum EXTENDEDFLAGS
{
  OverlayActive           = 0x00000001,
  InvisibleMonsters       = 0x00000002,
  DrawAsSize4Monsters     = 0x00000004,
  DMRulesOption           = 0x00000008,  //player-specified
  ExtendedWallDecorations = 0x00000010,
  SequencedTimers         = 0x00000020,
  DefaultDirectX          = 0x00000040,
  ExtendedTimers          = 0x00000080,  // 32-bit time + 8-bit level.
};
  i32       overlayOrdinal; //overlay Number + 1
  i32       overlayP1;
  i32       overlayP2;
  i32       overlayP3;
  i32       overlayP4;
  ui32      CSBgraphicsSignature1;
  ui32      CSBgraphicsSignature2;
  char      hintKey[8];
  char fill[512-104];
};



enum ICOUNTER
{
  icntPumpMsg = 0,
  icntVBL     = 1,
  icntVBLLoop = 2,
  icntRemoveCursor=3,
  icntCreateCursorBitmap=4,
  icntGetCursorPos=5,
  icntInvalidate=6,
  icntCopyViewportToScreen=7,
  icntFloorAndCeiling=8,
  icntPlaceCursor=9,
  icntTAG002336=10,
  icntDisplayCharacterDamage=11,
  icntTAG0197d2=12,

  numInstrumentation
};


class ICOUNTS
{
public:
  i32 m_InstrumentationCounts[numInstrumentation];
  ICOUNTS(void);
};

#define Instrumentation(i) InstrumentationCounts.m_InstrumentationCounts[i]++;

extern ICOUNTS InstrumentationCounts;

enum GRAPHICROOMTYPE
{
  gRoomSTONE      = 0,
  gRoomOPEN       = 1,
  gRoomPIT        = 2,
  gRoomTELEPORTER = 3,
  gRoomDOOREDGE   = 4, //A door seen on edge
  gRoomDOORFACING = 5, //A door seen on flat side
  gRoomSTAIREDGE  = 6, //Stairway at right angle to party facing
  gRoomSTAIRFACING= 7  //Stairway in direction party is facing
};

enum ROOMTYPE
{
  roomSTONE      = 0,
  roomOPEN       = 1,
  roomPIT        = 2,
  roomSTAIRS     = 3,
  roomDOOR       = 4,
  roomTELEPORTER = 5,
  roomFALSEWALL  = 6,
  roomDOOREDGE   = 16, //A door seen on edge
  roomDOORFACING = 17, //A door seen on flat side
  roomSTAIREDGE  = 18, //Stairway at right angle to party facing
  roomSTAIRFACING= 19, //Stairway in direction party is facing
  roomEXTERIOR   = 20  //Exterior wall of dungeon
};

enum ATTACKTYPE
{
  atk_JUGGLE      = 0, //Not in CSB
  atk_BLOCK       = 1,
  atk_CHOP        = 2,
  atk_SPEED       = 3, //Not in CSB
  atk_BLOWHORN    = 4,
  atk_FLIP        = 5,
  atk_PUNCH       = 6,
  atk_KICK        = 7,
  atk_WARCRY      = 8,
  atk_STAB1       = 9,
  atk_CLIMBDOWN   = 10,
  atk_FREEZELIFE  = 11,
  atk_HIT         = 12,
  atk_SWING       = 13,
  atk_STAB2       = 14,
  atk_THRUST      = 15,
  atk_JAB         = 16,
  atk_PARRY       = 17,
  atk_HACK        = 18,
  atk_BERZERK     = 19,
  atk_FIREBALL    = 20,
  atk_DISPELL     = 21,
  atk_CONFUSE     = 22,
  atk_LIGHTNING   = 23,
  atk_DISRUPT     = 24,
  atk_MELEE       = 25,
  atk_PRAY        = 26, // Not in CSB
  atk_INVOKE      = 27,
  atk_SLASH       = 28,
  atk_CLEAVE      = 29,
  atk_BASH        = 30,
  atk_STUN        = 31,
  atk_SHOOT       = 32,
  atk_SPELLSHIELD = 33,
  atk_FIRESHIELD  = 34,
  atk_FLUXCAGE    = 35,
  atk_HEAL        = 36,
  atk_CALM        = 37,
  atk_LIGHT       = 38,
  atk_WINDOW      = 39,
  atk_SPIT        = 40,
  atk_BRANDISH    = 41,
  atk_THROW       = 42,
  atk_FUSE        = 43
};


#define soundPRESSUREPAD  1
#define soundMONDEATH     4
#define soundTELEPORT    17
#define soundDOOROPENING 18

#define Luck      0 // What is this?  Rabbit's foot adds 10!
#define Strength  1
#define Dexterity 2
#define Wisdom    3
#define Vitality  4
#define AntiMagic 5
#define AntiFire  6






//inline ROOMTYPE RoomType(CELLFLAG cf) {return (ROOMTYPE)(cf>>5);};
// Not needed.  CELLFLAG automatically promoted to i32.
inline ROOMTYPE RoomType(i32 cf) {return (ROOMTYPE)(cf>>5);};


typedef ui16 DIRECTION;
extern DIRECTION dirNORTH, dirEAST, dirSOUTH, dirWEST;

class ICONDISPLAY
{
public:
  i16 pixelX;
  i16 pixelY;
private:
  i16 m_objectType; //OBJECTTYPE /cannot access directly because
                    //VC++ wants to allocate 32 bits.
public:
  OBJ_NAME_INDEX objectType(void) {return (OBJ_NAME_INDEX)m_objectType;};
  void objectType(OBJ_NAME_INDEX obj) {m_objectType=(i16)obj;};
};


struct SUMMARIZEROOMDATA
{
  i32 x, y, relativeCellNumber;
  i32 skinNumber;
  ROOMTYPE roomType;
  i32 championPortraitOrdinal;
  GRAPHICROOMTYPE graphicRoomType;
  RN  rn2;
  i32 decorations[3]; //right, front, left
  RN text[3]; //right, front, left
};

#define BITS0_0(x)   (UI8)((((x)    )&0x0001))
#define BITS0_1(x)   (UI8)((((x)    )&0x0003))
#define BITS0_2(x)   (UI8)((((x)    )&0x0007))
#define BITS0_3(x)   (UI8)((((x)    )&0x000f))
#define BITS0_4(x)   (UI8)((((x)    )&0x001f))
#define BITS0_5(x)   (UI8)((((x)    )&0x003f))
#define BITS0_6(x)   (UI8)((((x)    )&0x007f))
#define BITS0_7(x)   (UI8)((((x)    )&0x00ff))
#define BITS0_9(x)   (((x)    )&0x03ff)
#define BITS0_8(x)   (((x)    )&0x00ff)
#define BITS1_1(x)   (((x)>> 1)&0x0001)
#define BITS1_2(x)   (((x)>> 1)&0x0003)
#define BITS1_4(x)   (((x)>> 1)&0x000f)
#define BITS2_2(x)   (((x)>> 2)&0x0001)
#define BITS3_4(x)   (((x)>> 3)&0x0003)
#define BITS3_15(x)  (((x)>> 3)&0x1fff)
#define BITS4_5(x)   (UI8) ((((x)>> 4)&0x0003))
#define BITS4_7(x)   (UI8) ((((x)>> 4)&0x000f))
#define BITS4_9(x)   (UI8) ((((x)>> 4)&0x003f))
#define BITS4_11(x)  (UI8) ((((x)>> 4)&0x00ff))
#define BITS4_15(x)  (UI16)((((x)>> 4)&0x0fff))
#define BITS5_5(x)   (UI8) ((((x)>> 5)&0x0001))
#define BITS5_6(x)   (UI8) ((((x)>> 5)&0x0003))
#define BITS5_9(x)   (UI8) ((((x)>> 5)&0x001f))
#define BITS5_15(x)  (UI16)((((x)>> 5)&0x07ff))
#define BITS6_6(x)   (UI8)((((x)>> 6)&0x0001))
#define BITS6_7(x)   (UI8)((((x)>> 6)&0x0003))
#define BITS6_10(x)  (UI8)((((x)>> 6)&0x001f))
#define BITS6_11(x)  (UI8)((((x)>> 6)&0x003f))
#define BITS7_7(x)   (UI8)((((x)>> 7)&0x0001))
#define BITS7_8(x)   (UI8)((((x)>> 7)&0x0003))
#define BITS7_10(x)  (UI8)((((x)>> 7)&0x000f))
#define BITS7_11(x)  (UI8)((((x)>> 7)&0x001f))
#define BITS7_15(x)  (UI16)((((x)>> 7)&0x01ff))
#define BITS8_8(x)   (UI8)((((x)>> 8)&0x0001))
#define BITS8_11(x)  (UI8)((((x)>> 8)&0x000f))
#define BITS8_9(x)   (UI8)((((x)>> 8)&0x0003))
#define BITS8_12(x)  (UI8)((((x)>> 8)&0x001f))
#define BITS8_14(x)  (UI8)((((x)>> 8)&0x007f))
#define BITS8_15(x)  (UI8)((((x)>> 8)&0x00ff))
#define BITS9_9(x)   (UI8)((((x)>> 9)&0x0001))
#define BITS9_11(x)  (UI8)((((x)>> 9)&0x0007))
#define BITS9_12(x)  (UI8)((((x)>> 9)&0x000f))
#define BITS9_13(x)  (UI8)((((x)>> 9)&0x001f))
#define BITS9_15(x)  (UI8)((((x)>> 9)&0x007f))
#define BITS10_10(x) (UI8)((((x)>>10)&0x0001))
#define BITS10_13(x) (UI8)((((x)>>10)&0x000f))
#define BITS10_14(x) (UI8)((((x)>>10)&0x001f))
#define BITS10_11(x) (UI8)((((x)>>10)&0x0003))
#define BITS10_15(x) (UI8)((((x)>>10)&0x003f))
#define BITS11_11(x) (UI8)((((x)>>11)&0x0001))
#define BITS11_15(x) (UI8)((((x)>>11)&0x001f))
#define BITS12_12(x) (UI8)((((x)>>12)&0x0001))
#define BITS12_13(x) (UI8)((((x)>>12)&0x0003))
#define BITS12_15(x) (UI8)((((x)>>12)&0x000f))
#define BITS13_13(x) (UI8)((((x)>>13)&0x0001))
#define BITS13_14(x) (UI8)((((x)>>13)&0x0003))
#define BITS13_15(x) (UI8)((((x)>>13)&0x0007))
#define BITS14_14(x) (UI8)((((x)>>14)&0x0001))
#define BITS14_15(x) (UI8)((((x)>>14)&0x0003))
#define BITS15_15(x) (UI8)((((x)>>15)&0x0001))

#define SETBBITS0_0(d,s)   (d) = (I8)(((d)&0xfffe)|(((s)&0x0001)    ))
#define SETBBITS0_2(d,s)   (d) = (I8)(((d)&0xfff8)|(((s)&0x0007)    ))

#define SETWBITS0_0(d,s)   (d) = (i16)(((d)&0xfffe)|(((s)&0x0001)    ))
#define SETWBITS0_2(d,s)   (d) = (i16)(((d)&0xfff8)|(((s)&0x0007)    ))
#define SETWBITS0_3(d,s)   (d) = (i16)(((d)&0xfff0)|(((s)&0x000f)    ))
#define SETWBITS0_4(d,s)   (d) = (i16)(((d)&0xffe0)|(((s)&0x001f)    ))
#define SETWBITS0_5(d,s)   (d) = (i16)(((d)&0xffc0)|(((s)&0x003f)    ))
#define SETWBITS0_6(d,s)   (d) = (i16)(((d)&0xff80)|(((s)&0x007f)    ))
#define SETWBITS0_7(d,s)   (d) = (i16)(((d)&0xff00)|(((s)&0x00ff)    ))
#define SETWBITS3_15(d,s)  (d) = (i16)(((d)&0x0007)|(((s)&0x1fff)<< 3))
#define SETWBITS5_6(d,s)   (d) = (i16)(((d)&0xff9f)|(((s)&0x0003)<< 5))
#define SETWBITS5_9(d,s)   (d) = (i16)(((d)&0xfc1f)|(((s)&0x001f)<< 5))
#define SETWBITS6_7(d,s)   (d) = (i16)(((d)&0xff3f)|(((s)&0x0003)<< 6))
#define SETWBITS6_11(d,s)  (d) = (i16)(((d)&0xf03f)|(((s)&0x003f)<< 6))
#define SETWBITS7_7(d,s)   (d) = (i16)(((d)&0xff7f)|(((s)&0x0001)<< 7))
#define SETWBITS7_15(d,s)  (d) = (i16)(((d)&0x007f)|(((s)&0x01ff)<< 7))
#define SETWBITS8_8(d,s)   (d) = (i16)(((d)&0xfeff)|(((s)&0x0001)<< 8))
#define SETWBITS8_9(d,s)   (d) = (i16)(((d)&0xfcff)|(((s)&0x0003)<< 8))
#define SETWBITS8_14(d,s)  (d) = (i16)(((d)&0x80ff)|(((s)&0x007f)<< 8))
#define SETWBITS8_15(d,s)  (d) = (i16)(((d)&0x00ff)|(((s)&0x00ff)<< 8))
#define SETWBITS9_9(d,s)   (d) = (i16)(((d)&0xfdff)|(((s)&0x0001)<< 9))
#define SETWBITS9_11(d,s)  (d) = (i16)(((d)&0xf1ff)|(((s)&0x0007)<< 9))
#define SETWBITS9_12(d,s)  (d) = (i16)(((d)&0xe1ff)|(((s)&0x000f)<< 9))
#define SETWBITS9_13(d,s)  (d) = (i16)(((d)&0xc1ff)|(((s)&0x001f)<< 9))
#define SETWBITS10_10(d,s) (d) = (i16)(((d)&0xfbff)|(((s)&0x0001)<<10))
#define SETWBITS11_11(d,s) (d) = (i16)(((d)&0xf7ff)|(((s)&0x0001)<<11))
#define SETWBITS12_12(d,s) (d) = (i16)(((d)&0xefff)|(((s)&0x0001)<<12))
#define SETWBITS10_11(d,s) (d) = (i16)(((d)&0xf3ff)|(((s)&0x0003)<<10))
#define SETWBITS10_13(d,s) (d) = (i16)(((d)&0xc3ff)|(((s)&0x000f)<<10))
#define SETWBITS10_15(d,s) (d) = (i16)(((d)&0x03ff)|(((s)&0x003f)<<10))
#define SETWBITS12_13(d,s) (d) = (i16)(((d)&0xcfff)|(((s)&0x0003)<<12))
#define SETWBITS12_15(d,s) (d) = (i16)(((d)&0x0fff)|(((s)&0x000f)<<12))
#define SETWBITS13_13(d,s) (d) = (i16)(((d)&0xdfff)|(((s)&0x0001)<<13))
#define SETWBITS13_14(d,s) (d) = (i16)(((d)&0x9fff)|(((s)&0x0003)<<13))
#define SETWBITS14_14(d,s) (d) = (i16)(((d)&0xbfff)|(((s)&0x0001)<<14))
#define SETWBITS14_15(d,s) (d) = (i16)(((d)&0x3fff)|(((s)&0x0003)<<14))
#define SETWBITS15_15(d,s) (d) = (i16)(((d)&0x7fff)|(((s)&0x0001)<<15))
#define SETIBITS24_35(d,s) (d) = (i32)(((d)&0xffffff)|(((s)&0xff)<<24))

class DBCOMMON
{
  friend class DB0;
  friend class DB1;
  friend class DB2;
  friend class DB3;
  friend class DB4;
  friend class DB5;
  friend class DB6;
  friend class DB7;
  friend class DB8;
  friend class DB9;
  friend class DB10;
  friend class DB11;
  friend class DB14;
  friend class DB15;
  friend void ConvertListOfObjects(RN *, bool,i32 level, i32 x, i32 y);
//public:
  RN   m_link;
  void swapLink(void);
public:
  const RN link(void);
  RN *pLink(void);
  void link(RN rn);
  bool IsDBType(DBTYPE);
  class DB0 *CastToDB0(void);
  class DB1 *CastToDB1(void);
  class DB2 *CastToDB2(void);
  class DB3 *CastToDB3(void);
  class DB4 *CastToDB4(void);
  class DB5 *CastToDB5(void);
  class DB6 *CastToDB6(void);
  class DB7 *CastToDB7(void);
  class DB8 *CastToDB8(void);
  class DB9 *CastToDB9(void);
  class DB10 *CastToDB10(void);
  class DB11 *CastToDB11(void);
  class DB14 *CastToDB14(void);
  class DB15 *CastToDB15(void);
};

class DB0:public DBCOMMON
{ //Door
  friend class DATABASES;
  friend void DumpDB0(FILE *f, RN object, CELLFLAG cf);
  i16 word2; //bit 0 = 1 if open
             //bits 1-4 = ornateness
             //bit 5 = Mode of operation (0=sideways, 1=up)
             //bit 6 = door switch
             //bit 7 = Can open with fireball
             //bit 8 = Can open with axe
             //bits 9-15 ????
             // 0400 means reflect/rotate image randomly to cause appearance of movement
  void swap(void);
public:
  const ui8 doorType(void)   {return (UI8)(word2 & 1);};
  void doorType(i32 d){word2=(ui16)((word2&0xfffe)|(d&1));};
    // Each level can have two door types.
  const i32 ornateness(void){return (word2>>1) & 0xf;};
  void ornateness(i32 o){word2=(ui16)((word2&0xffe1)|((o&0xf)<<1));};
  const ui8 mode(void)   {return (UI8)((word2>>5) & 1);}; //up/down
  void mode(bool b){word2=(ui16)(b?(word2|0x0020):(word2&0xffdf));}; //up/down
  const bool canOpenWithFireball(void)    {return (word2>>7) & 1;};
  void canOpenWithFireball(bool b){word2=(ui16)(b?(word2|0x0080):(word2&0xff7f));};
  const bool canOpenWithAxe(void) {return (word2>>8) & 1;};
  void canOpenWithAxe(bool b){word2=(ui16)(b?(word2|0x0100):(word2&0xffef));};
  const ui8 doorSwitch(void) {return (UI8)((word2>>6) & 1);};
  void doorSwitch(bool b){word2=(ui16)(b?(word2|0x0040):(word2&0xffbf));};
};



class DB1:public DBCOMMON
{ //Teleporter
  friend void DumpDB1(FILE *f, RN object, CELLFLAG /*cf*/);
  friend class DATABASES;
  void swap(void);
private:
  i16 word2;    // bits 0-4   = mapX
                // bits 5-9   = mapY
                // bits 10-11 = new facing or rotation
                // bit 12     = 1= absolute facing; 0=rotate relative
                // bits 13-14 = 0 = objects only
                //              1 = monsters only
                //              2 = party or objects only
                //              3 = anything
                // bit 15     = audible buzz
  i16 word4;    // bits 8-15  = level
public:
  i32 destX(void) const {return BITS0_4(word2);};
  void destX(ui32 x) {SETWBITS0_4(word2,x);};
  i32 destY(void) const {return BITS5_9(word2);};
  void destY(ui32 y) {SETWBITS5_9(word2,y);};
  ui8 destLevel(void) const {return BITS8_15(word4);};
  void destLevel(ui32 l) {SETWBITS8_15(word4,l);};
  ui8 rotation(void) const {return BITS10_11(word2);};
  void rotation(i32 r) {SETWBITS10_11(word2,r);};
  ui8 facingMode(void) const {return BITS12_12(word2);};
  void facingMode(ui32 f) {SETWBITS12_12(word2,f);};
  ui8 what(void) const {return BITS13_14(word2);};
  void what(ui32 w) {SETWBITS13_14(word2,w);};
  ui8 audible(void) const {return BITS15_15(word2);};
  void audible(ui32 w) {SETWBITS15_15(word2,w);};
  void copyTeleporter(DB1 *pSrc)
  {
    word2 = pSrc->word2;
    word4 = pSrc->word4;
  };
};

class DB2:public DBCOMMON  // Text
{
  i16 word2;  // bit 0 = Show
              // bits 3-15
  friend class DATABASES;
  void swap(void);
public:
  void Clear(void);
  bool show(void) {return BITS0_0(word2) != 0;};
  void show(bool s) {SETWBITS0_0(word2,s?1:0);};
  i32 index(void) {return BITS3_15(word2);};
  void index(i32 i) {SETWBITS3_15(word2,i);};
};

// Action field in DB3 record
enum ACTUATORACTION
{
  actuatorAction_SET            = 0,
  actuatorAction_CLEAR          = 1,
  actuatorAction_TOGGLE         = 2,
  actuatorAction_CONSTPRESSURE  = 3
};

enum 
{
  //Bit flags in State
  PORTRAIT_InActive       = 1,
  PORTRAIT_HidePortrait   = 2,
  PORTRAIT_HideGraphic    = 4,
  // How to respond to messsages
  PORTRAIT_ResponseNone    = 0, //Do nothing
  PORTRAIT_ResponseSet     = 1, //Set Active or Show
  PORTRAIT_ResponseClear   = 2, //Set Inactive or Hide
  PORTRAIT_ResponseToggle  = 3, //Toggle Active or Show
  // Where to find 2-bit Response actions in 'responses' fields
  PORTRAIT_ActiveResponse   = 0,
  PORTRAIT_PortraitResponse = 2,
  PORTRAIT_GraphicResponse  = 4
};

class DB3:public DBCOMMON  // An actuator (pressure pad, keyhole, etc.)
{
  friend void DumpDB3(FILE *, RN, i32, i32, i32, CELLFLAG);
  friend class DATABASES;
  void swap(void);
private:
  i16 word2;  // bits 0-6 = Actuator Type
              //    0 = Do nothing
              //    1 = A simple wall switch that
              //        can be pressed with anything.
              //    2 = ?
              //    3 = Lock. Keys stays in hand
              //    4 = Lock. Key removed from hand.
              //    5 =
              //        Byte 8 (position) of Timer6 is bit number.
              //        Bits 7-15 are value.  Bits in this value
              //        are Set/Cleared/Toggled by Timer6.
              //        "position" of timer6 tells which bit to
              //        fiddle with.
              //        Actuator is 'pressed' when bits 7-10 are
              //        equal to bits 11-14.  (The first 4 bits of
              //        value equals second 4 bits).
              //    6 =
              //        In a closed (stone) room
              //           Bits 7-15 are a 9-bit counter.  A SET
              //           function increments and anything else
              //           decrements the counter.  You cannot change
              //           a counter that equals zero.  (Why?)
              //           The actuator is considered 'pressed'
              //           if the count is non-zero.
              //        In an open room this is a monster generator
              //        and is actuated by a timer event type 5.
              //           word 6 bits 8-15 non-zero causes once only.
              //                            A type 65 record is created and
              //                            the generator is disabled.
              //                            If >= 128 then the time
              //                            is queued to go off at
              //                            64 * ((word 6 bits 8-15)-126)
              //                            (I presume to reactivate generator!).
              //    7 = In stone - missile launcher. One predefined.
              //    8 = In open room:
              //        Operated only by party.  Value is
              //        an OBJECTTYPE that we search for in the
              //        party's possessions.  Actuated if party
              //        possesses such an object.
              //
              //        In Stone: A missile launcher. A spell.
              //          Value = 0  Fireballs
              //    9 = In stone - Missile Launcher. Two predefined.
              //   10 = In stone - Missile Launcher. Two spells.
              //   11 = Lock. Key removed from hand.
              //   14 = Missile Launcher. One available object.
              //        When a timer of type 6 is sent to
              //        an actuator of type 14 then the first
              //        object at the proper position in the same
              //        room as the actuator is removed
              //        from the room and launched as a missile
              //        in the adjacent room.
              //   15 = Missile Launcher. Two available objects.
              //        When a timer of type 6 is sent to
              //        an actuator of type 15 then the first two
              //        objects at the proper position in the same
              //        room as the actuator are removed
              //        from the room and launched as missiles
              //        in the adjacent room.
              //   47 = DSA
              // bits 7-15 = value
private:
  i16 word4;  // bit 2 = causes bits 1-6 of word 2 to be cleared. "Once only"???
              // bits 3-4 Action when switch changes state
              //           0=set when closed
              //           1=clear when closed
              //           2=toggle when closed
              //           3=set when closed, clear when opened
              // bit 5 = 0=normally open; 1=normally closed
              // bit 6 = make click sound
              // bits 7-10 = delta time (or number of monsters+1)
              //   For a monster generator, a value with bit 3
              //   set means to generate a random number modulo
              //   bits 0-2.
              // bit 11 = Local action only.  Like increase
              //          skill.
              // bits 12-15 = wall graphic index+1
private:
  i16 word6;  // (if word4 & 0x800)
                 // bits 4-15
              // (if ! word4 & 0x800)
                 // bits 4-5    position????
                 // bits 6_10   =mapX
                 // bits 11_15  =mapY
              // bits 4-7  hitPoint multiplier (Monster Generator)
              // bits 4-11 energy remaning in missile
              // bits 8_15 disable time (Monster Generator)
              // bits 12-15
private:
  i16 word8;
public:
  void Clear(void);
  i16 delay(void) {return (i16)BITS7_10(word4);};
  i16 targetX(void) {ASSERT((word4&0x800)==0, "No Target");return (i16)BITS6_10(word6);};
  i16 targetY(void) {ASSERT((word4&0x800)==0, "No Target");return (i16)BITS11_15(word6);};
  i16 targetPos(void) {ASSERT((word4&0x800)==0,"No Target");return (i16)BITS4_5(word6);};
  i16 localActionType(void) {ASSERT((word4&0x800)!=0,"No Target");return (i16)BITS4_15(word6);};
  i16 audible(void) {return (i16)BITS6_6(word4);};
  ACTUATORACTION action(void) {return ACTUATORACTION(BITS3_4(word4));};
  i16 onceOnly(void) {return (i16)BITS2_2(word4);};
  bool normallyClosed(void) {return BITS5_5(word4)!=0;};
  i16 localActionOnly(void) {return (i16)BITS11_11(word4);};
  ui16 value(void) {return BITS7_15(word2);};
  void value(i32 val) {SETWBITS7_15(word2,val);};
  i16 actuatorType(void) {return (i16)BITS0_6(word2);};
  void actuatorType(i32 type){SETWBITS0_6(word2,type);};
  void actuatorTypeOR(i32 bits){word2 |= bits;};
  i16 wallGraphicOrdinal(void) {return (i16)BITS12_15(word4);};
  i32 disableTime(void) {return BITS8_15(word6);}; //Monster Generator
  void disableTime(i32 t){SETWBITS8_15(word6,t);};
  i32 hitPointMultiplier(void) {return BITS4_7(word6);};//Monster Generator
           //If zero then level experience multiplier is used
  i32 energyRemaining(void) {return BITS4_11(word6);};
  ui8 andOrEdge(void){return BITS0_2(word6);};
  bool drawAsSize4(void){return BITS0_0(word6)!=0;};
  ui8 DSAstate(void){return BITS12_15(word2);};
  void DSAstate(ui8 s){SETWBITS12_15(word2,s);};
  ui8 DSAselector(void){return BITS7_11(word2);};
  ui32 ParameterA(void);
  void ParameterA(ui32 v);
  ui32 ParameterB(void);
  void ParameterB(ui32 v);
  bool BlockDisables(void){return BITS8_8(word4)!=0;};
  void BlockDisables(bool b){SETWBITS8_8(word4,b?1:0);};
  ui16 SetResponses(void){return BITS6_11(word6);};
  void SetResponses(ui16 v){SETWBITS6_11(word6,v);};
  ui16 ClearResponses(void){return BITS0_5(word6);};
  void ClearResponses(ui16 v){SETWBITS0_5(word6,v);};
  ui16 ToggleResponses(void){return BITS0_5(word4);};
  void ToggleResponses(ui16 v){SETWBITS0_5(word4,v);};
  ui16 DisableMessage(void){return BITS6_7(word4);};
  void DisableMessage(ui16 v){SETWBITS6_7(word4,v);};
  ui16 State(void){return BITS9_11(word4);};
  void State(ui16 v){SETWBITS9_11(word4,v);};
  ui16 Position(void){return BITS12_13(word6);};
  void Position(ui16 v){SETWBITS12_13(word6,v);};
  void ClearUnusedPor(void){SETWBITS14_15(word6,0);};
  void MakeBig(void);//Reformat to 10-byte size.
};



class DB4:public DBCOMMON // Monsters??
{
  friend class DATABASES;
  void swap(void);
  RN   possession2; // Swapped when read
               // //I think this is a list of possessions.
private:
  ui8  uByte4; // Index into d.MonsterDescriptor (ITEM26)

  ui8  uByte5; //ITEM16 index when the monster group is on
               // the same level as the party.  Else it is
               // four 2-bit fields with positions of each
               // group member.
public:
  ui16  hitPoints6[4]; //Swapped when read
                // Hit Points for each monster in group.
private:
  i16  word14;  // Swapped when read
                // Bits 0-3 = 6 =
                //  We set these bits to a 5 when a giggler stole
                //  some of our articles.  Fear???
                // Bits 5-6 = index into uByte12 of Item9664 (ITEM26)
                //            Number of monsters in group-1
                //            Comes from delay field of Timer5 entry
                //             when monster created
                // Bits 8-9 set random when created
                //  When not on same level as party then
                //   this is direction facing.
                // Bit 10 set to zero when created (important flag)
                // Bit 11 = invisible
                // Bit 12 = Draw as size 4.
                // Bit 13 means the monster ID is unique.  This was installed
                //        because the designer wanted to track a special monster
                //        by its ID but when the monster was killed another 
                //        monster might inherit its ID.  We clear this flag
                //        when we generate a monster.
                // Bit 14 means poisoned.  Only DSAs use this.
public:
  MONSTERTYPE monsterType(void) {return (MONSTERTYPE)(uByte4&0x1f);};
  void monsterType(i32 m) {ASSERT(m<27,"Illegal Monster Type");uByte4=(ui8)((m&0x1f)|(uByte4&0xe0));};
  i32 alternateGraphics(void){return (uByte4>>5)&0x7;};
  void alternateGraphics(i32 g){uByte4=(ui8)((uByte4&0x1f)|((g&7)<<5));};
  RN possession(void) {return possession2;};
  RN *pPossession(void) {return &possession2;};
  ui16 important(void){return BITS10_10(word14);};
  void important(i32 i){SETWBITS10_10(word14,i);};
  i32 facing(void){return BITS8_9(word14);};
  void facing(i32 f){SETWBITS8_9(word14,f);};
  ui16 numMonM1(void){return BITS5_6(word14);};
  void numMonM1(i32 n){SETWBITS5_6(word14,n);};
  bool invisible(void){return invisibleMonsters&&BITS11_11(word14);};
  void invisible(bool b){SETWBITS11_11(word14,b?1:0);};
  bool drawAsSize4(void){return drawAsSize4Monsters&&BITS12_12(word14);};
  void drawAsSize4(bool b){SETWBITS12_12(word14,b?1:0);};
  bool Unique(void){return drawAsSize4Monsters&&BITS13_13(word14)!=0;};
  void Unique(bool b){SETWBITS13_13(word14,b?1:0);};
  bool Poisoned(void){return drawAsSize4Monsters&&BITS14_14(word14)!=0;};
  void Poisoned(bool b){SETWBITS14_14(word14,b?1:0);};
  STATE_OF_FEAR fear(void){return (STATE_OF_FEAR)BITS0_3(word14);};
  void fear(STATE_OF_FEAR f){SETWBITS0_3(word14,f);};
  void possession(RN object) {possession2 = object;};
  ui8 groupIndex(void) {return uByte5;};
  void groupIndex(ui32 i);
  void Clear(void);
};

class DB5:public DBCOMMON
{
  friend class DATABASES;
  friend void DumpDB5(FILE *f, RN object, i32 /*level*/);
  void swap(void);
  i16 word2; //<bits 0-6> = weapon Type
             //             + 23 = Object Index
             // bit 7 = ??? (important item???)
             // bit 8 = Cursed
             // bit 9 = Poisoned
             //<bits 10-13> ;charges remaining
             // bit 14 = Broken
             // bit 15 = torch Type
public:
  WEAPONTYPE  weaponType(void){return (WEAPONTYPE)BITS0_6(word2);};
  void weaponType(WEAPONTYPE w){SETWBITS0_6(word2,w);};
  i32 cursed(void) {return BITS8_8(word2);};
  void cursed(i32 c) {SETWBITS8_8(word2,c);};
  i32 poisoned(void) {return BITS9_9(word2);};
  void poisoned(bool b) {SETWBITS9_9(word2,b?1:0);};
  ui8 charges(void){return BITS10_13(word2);};
  void charges(i32 n) {SETWBITS10_13(word2,n);};
  i32 broken(void) {return BITS14_14(word2);};
  void broken(bool b) {SETWBITS14_14(word2,b?1:0);};
  bool important(void){return BITS7_7(word2)!=0;};
  void important(bool i) {SETWBITS7_7(word2,i?1:0);};
  i32 litTorch(void) {return BITS15_15(word2);};
  void litTorch(i32 n) {SETWBITS15_15(word2, n);};
  void Clear(void);
};

class DB6:public DBCOMMON  //Sandals, robe, shield
{
  friend class DATABASES;
  void swap(void);
  friend void DumpDB6(FILE *f, RN object, i32 /*level*/);
  i16 word2; //<bits 0-6>  = clothing type
             // bit 7 = ??? (important item???)
             // bit 8 = cursed
             //<bits 9-12> = charges remaining
             // bit 13 = broken
public:
  i32 clothingType(void) {return BITS0_6(word2);};
  void clothingType(CLOTHINGTYPE c){SETWBITS0_6(word2,c);};
  bool important(void) {return (word2 & 0x80) != 0;};
  i32 charges(void) {return BITS9_12(word2);};
  void charges(i32 n) {SETWBITS9_12(word2, n);};
  i32 cursed(void) {return BITS8_8(word2);};
  void cursed(i32 c) {SETWBITS8_8(word2, c);};
  i32 broken(void) {return BITS13_13(word2);};
  void broken(bool b) {SETWBITS13_13(word2,b?1:0);};
  void Clear(void);
};

class DB7:public DBCOMMON
{ // Scroll?
  friend class DATABASES;
// old Scroll  i16 m_word2; //swapped when read
// old Scroll             // bits 0-9 = text index
// old Scroll             // bits 10-15 = ????
  void swap(void);
  RN m_RNtext;
  ui16 m_word4; // Bit 0 = open
                // Bits 1-15 unused
public:
  DB7(void);
// old Scroll  i32 textIndex(void) {return BITS0_9(m_word2);};
// old Scroll  i32 b10_15(void) {return BITS10_15(m_word2);};
// old Scroll  void b10_15(i32 n) {SETWBITS10_15(m_word2,n);};
  RN text(void){return m_RNtext;};
  void text(RN text){m_RNtext = text;};
  bool open(void){return BITS0_0(m_word4)!=0;};
  void open(bool o){SETWBITS0_0(m_word4,o?1:0);};
  void Clear(void);
  DB7& operator =(DB7& db7); //Undefined!!!
};

class DB8:public DBCOMMON
{ // potion??
  friend class DATABASES;
  friend void DumpDB8(FILE *f, RN object, i32 /*level*/);
  void swap(void);
  i16 word2;// <bits 0-7> Strength of potion
            // <bits 8-14> = potion type
            // bit 15 = ??? (important item??)
public:
  ui8 strength(void) {return BITS0_7(word2);};
  void strength(i32 s) {SETWBITS0_7(word2,s);};
  bool important(void) {return (word2&0x8000)!=0;};
  POTIONTYPE potionType(void) {return (POTIONTYPE)BITS8_14(word2);};
  void potionType(POTIONTYPE p){SETWBITS8_14(word2,p);};
  void Clear(void);
};

class DB9:public DBCOMMON // A chest??
{
  RN  m_contents;
  i16 m_word4;    // <Bits 1-2> + 1 = object index
  ui8 m_uByte6[2];
  friend class DATABASES;
  void swap(void);
public:
  const RN contents(void) {return m_contents;};
  RN *pContents(void){return &m_contents;};
  void contents(RN rn) {m_contents = rn;};
  const i32 word4(void) {return m_word4;};
  const ui32 uByte6(i32 n) {return m_uByte6[n];};
};

class DB10:public DBCOMMON  //corn, shank, necklace, miscellaneous
{
  friend void DumpDB10(FILE *f, RN object, i32 /*level*/);
  friend class DATABASES;
  void swap(void);
private:
  i16 word2; // For Water.Waterskin
             //     <Bits 0-6> = misc type
             // bit 7 = ??? (important item???)
             // bit 8 = cursed???
             //     Bits 14-15 = how full; 0=empty 3 = full
             //                = charges remaining
             //                = for bones this is the 'type of bones:
             //                    0 = inert bones.  Not associated with a champion
             //                    1 = bones associated with a champion's name but otherwise useless
             //                    2 = bones associated with a dead champion.  Put into Vi Alter!
public:
  MISCTYPE miscType(void) {return (MISCTYPE)BITS0_6(word2);};
  void miscType(MISCTYPE t) {ASSERT(t<53,"Illegal Misc Type"); SETWBITS0_6(word2, t);};
  bool important(void) {return BITS7_7(word2) != 0;};
  void important(bool b) {SETWBITS7_7(word2,b?1:0);};
  bool cursed(void) {return BITS8_8(word2)!=0;};
  void cursed(i32 c) {SETWBITS8_8(word2,c);};
  ui8 value(void) {return BITS14_15(word2);};
  void value(i32 n) {SETWBITS14_15(word2, n);};
  int subType(void){return BITS9_13(word2);};
  void subType(i32 n){SETWBITS9_13(word2, n);};
  void Clear(void);
};

class DB11:public DBCOMMON
{
  friend class DATABASES;
  friend class EXPOOL;
  ui16 size;
  ui32 d[63]; //Total size = 256 bytes
  void swap(void);
  void Size(i32 s){size = (ui16)s;};
};

class DB12:public DBCOMMON //Not used
{
};

class DB13:public DBCOMMON //Not used
{
};

class DB14:public DBCOMMON // Fireball.  Other missiles.  Thrown objects.
{
  RN  m_obj2;            //2 missile object dagger, fireball, etc
                       //
  ui8 m_rangeRemaining;  //4
  ui8 m_damageRemaining; //5
  ui16 m_timerIndex;     //6
  friend class DATABASES;
  void swap(void);
public:
  RN  flyingObject(void) {return m_obj2;};
  void flyingObject(RN rn) {m_obj2 = rn;};
  RN *pFlyingObject(void){return &m_obj2;};
  ui8 rangeRemaining(void) {return m_rangeRemaining;};
  void rangeRemaining(i32 e) {m_rangeRemaining=(ui8)e;};
   ui8 damageRemaining(void) {return m_damageRemaining;};
  void damageRemaining(i32 e) {m_damageRemaining=(ui8)e;};
  ui16 timerIndex(void) {return m_timerIndex;};
  void timerIndex(i32 i) {m_timerIndex = (ui16)i;};
  void Clear(void);
};

enum CLOUDTYPE
{
  CT_FIREBALL = 0,
  CT_DISPELL  = 3,
  CT_OPENDOOR = 4,
  CT_POISON   = 7,
  CT_DEATH    = 40,
  CT_FLUXCAGE = 50
};

class DB15:public DBCOMMON
{ // Puff of smoke when monster expires??
  i16 word2; // Bits 0-6 = type (50 is special fluxcage?)
             // Bit  7 = ???
             // Bits 8-15 = value.  Like Damage.
  friend class DATABASES;
  void swap(void);
public:           //
  CLOUDTYPE cloudType(void) {return (CLOUDTYPE)BITS0_6(word2);};
  void cloudType(CLOUDTYPE t){SETWBITS0_6(word2,t);};
  i32 b7(void) {return BITS7_7(word2);};
  void b7(i32 n){SETWBITS7_7(word2,n);};
  i32 value(void) {return BITS8_15(word2);};
  void value(i32 n) {SETWBITS8_15(word2,n);};
  void Clear(void);
};



class WEAPONDESC
{
public:
  ui8 weight;
  ui8 uByte1;
  ui8 uByte2;
  ui8 uByte3;
  i16 word4;
};



#define memDebugFlag 0xbaddadde

#define SET(reg,condition) (reg=(UI8)((condition)?-1:0))


struct wordRectPos {
  i16 x1; i16 x2; i16 y1; i16 y2;
  //void operator =(const wordRectPos& src){MemMove((ui8 *)this, (ui8 *)&src, 8);};
};

struct byteRectPos {
  ui8 x1; ui8 x2; ui8 y1; ui8 y2;
  ui8 uByte4; ui8 uByte5; ui8 uByte6; ui8 uByte7;
  //void operator =(const byteRectPos& src){MemMove((ui8 *)this, (ui8 *)&src, 8);};
};

union RectPos  {wordRectPos w; byteRectPos b;};

struct KeyXlate
{
  ui16 mouse;
  ui16 keyscan;
};


struct BYTE_RECTANGLE
{
  ui8 x1;
  ui8 x2;
  ui8 y1;
  ui8 y2;
};


enum CURSORFILTER_TYPE
{
  CURSORFILTER_Unknown             = 0,

  CURSORFILTER_ReadGame            = 1,   // P1 = P2 = P3 = P4 = 0
              
  CURSORFILTER_PickFromFloor       = 2,   // Can be Canceled.  The effect will
                                          // be as if a Screamer were sitting
                                          // on top of the object.
                                          // P1 = level
                                          // P2 = relative x
                                          // P3 = relative y
                                          // P4 = position

  CURSORFILTER_PlaceOnCharacter    = 3,   // 'PlaceOnCharacter' and 'PickFromCharacter'
                                          // are connected because it is possible to
                                          // swap the contents of the cursor with the
                                          // object in the character's possession.
                                          // In order to allow the filter to cancel the
                                          // entire operation, the filter is always called
                                          // with type 'CURSORFILTER_PlaceOnCharacter'
                                          // even if the cursor is totally empty.  In such
                                          // a case, the object is RNnul (0xffff).
                                          // The operation can be canceled by setting
                                          // the type to CURSORFILTER_Cancel.  The effect
                                          // will be as if the object in the cursor cannot
                                          // be placed in the particular possession slot.
                                          // As if you tried to place a chest on the 
                                          // character's head.
                                          // P1 = Character Index
                                          // P2 = Position on character

  CURSORFILTER_PickFromCharacter   = 4,   // This always follows the 'PlaceOnCharacter'
                                          // operation unless the operation has been
                                          // canceled or there is not object in the
                                          // particular possession slot.
                                          // See discussion at 'PlaceOnCharacter'.
                                          // P1 = Character Index
                                          // P2 = Position on character

  CURSORFILTER_Throw               = 5,   // Can be canceled.  The object will
                                          // remain in the hand.
                                          // P1 = Left or Right
  
  CURSORFILTER_EnteringPrison      = 6,   // P1 = P2 = P3 = P4 = 0

  CURSORFILTER_DropObject          = 7,   // Can be canceled.  The object will
                                          // remain in the hand.
                                          // P1 = level
                                          // P2 = relative x
                                          // P3 = relative y
                                          // P4 = position

  CURSORFILTER_Eat                 = 8,   // Gulp

  CURSORFILTER_ResumeSavedGame     = 9,

  CURSORFILTER_DSA_DEL             = 10,

  CURSORFILTER_DSA_ADD             = 11,

  CURSORFILTER_TakeFromTorchHolder = 12,  // Can be canceled.  The same effect
                                          // as if no object of the correct type
                                          // was available at the holder's location.
                                          // P1 = level
                                          // P2 = relative x
                                          // P3 = relative y
                                          // P4 = position

  CURSORFILTER_PlaceInTorchHolder  = 13,  // Can be canceled.  It would act as
                                          // if the torch holder already contained
                                          // a torch.
                                          // P1 = level
                                          // P2 = relative x
                                          // P3 = relative y
                                          // P4 = position

  CURSORFILTER_SwapRemove          = 14,  // 'SwapRemove' and 'SwapReplace'
                                          // are connected because they act together to
                                          // swap the contents of the cursor with the
                                          // object at the actuator's location..
                                          // In order to allow the filter to cancel the
                                          // entire operation, the filter is always called
                                          // with type 'CURSORFILTER_SwapRemove' first.
                                          // The operation can be canceled by setting
                                          // the type to CURSORFILTER_Cancel.  The effect
                                          // will be as if there is no object
                                          // at the actuator's location.
                                          // P1 = level        //Actuator type 16
                                          // P2 = relative x
                                          // P3 = relative y
                                          // P4 = position

  CURSORFILTER_SwapReplace         = 15,  // See discussion of 'SwapRemove'.
                                          // P1 = level        //Actuator type 16
                                          // P2 = relative x
                                          // P3 = relative y
                                          // P4 = position

  CURSORFILTER_GiftFromGod         = 16,  // Can be canceled.
                                          // P1 = level        //Actuator type 12
                                          // P2 = relative x
                                          // P3 = relative y
                                          // P4 = position

  CURSORFILTER_TakeKey             = 17,  // Can be canceled.  Everything will work
                                          // exactly as before but the key will
                                          // remain in the player's hand.
                                          // P1 = level
                                          // P2 = relative x
                                          // P3 = relative y
                                          // P4 = position

  CURSORFILTER_DSA_MoveFrom        = 18,

  CURSORFILTER_DSA_MoveTo          = 19,

  CURSORFILTER_Cancel              = 20,  // Some cursor actions can be canceled
                                          // by setting the type to this value.

};

struct CURSORFILTER_PACKET
{
  int                object; // = RNnul
  int                type;   // = CURSORFILTER_Unknown
  int                p1;     // = 0
  int                p2;     // = 0
  int                p3;     // = 0
  int                p4;     // = 0
  CURSORFILTER_PACKET(void);
};


enum EXPOOL_DATA_TYPE
{
  EDT_MessageParameters = 1,
  EDT_ExtendedCellFlags = 2, //one 32-bit word per flag
       ECF_FOOTPRINTS = 0,
  EDT_SpecialLocations  = 3,
       ESL_MONSTERATTACKFILTER    = 0,
       ESL_FEEDINGFILTER          = 1,
       ESL_MONSTERMOVEFILTER      = 2,
       ESL_VIEWINGFILTER          = 3,
       ESL_PARTYATTACKFILTER      = 4,
       ESL_INDIRECTACTIONS        = 5,
       ESL_ADJUSTSKILLSFILTER     = 6,
       ESL_DELETEMONSTERFILTER    = 7,
       ESL_SOUNDFILTER            = 8,
       ESL_CURSORFILTER           = 9,
       ESL_ACTIONNAMEFILTER       = 10,
       ESL_CHARDEATHFILTER        = 11,
       ESL_EQUIPFILTER            = 12,
       ESL_MISSILEENCOUNTERFILTER = 13,
       ESL_PARTYMOVEFILTER        = 14,
       ESL_DAMAGECHARFILTER       = 15,
  EDT_Skins             = 4,
  EDT_Database          = 5,
       EDBT_ObjectWeights         = 0,
       EDBT_DeleteDuplicateTimers = 1,
       EDBT_RuntimeFileSignatures = 2,
       EDBT_Debuging              = 3,
       EDBT_GlobalVariables       = 4,
       EDBT_DisableSaves          = 5,
       EDBT_MonsterNames          = 6,
       EDBT_DSAtraces             = 7,
       EDBT_AltMonGraphics        = 8,
  EDT_Statistics        = 6,
       ESTAT_NumMonsterKilled     = 0, // A record for each monster type (four sub-types)
  EDT_Palette           = 7,
       // 16-word (64 byte) records.  Subtype = 0 to 11.  0-3 = red; 4-7 = green; 8-11 = blue

       // A character removed from the party and waiting in the wings.  Eight 25-word records.
  EDT_Character         = 8,           // Keybits 24-31
                                       // Record number (0-7) in bits 16-23.
                                       // Fingerprint in bits 0-15.
  EDT_ChampionBones     = 9,           // Key = bonesObject.ConvertToOnteger()
                                       // Contents is fingerprint of dead champion.
  EDT_GlobalText        = 10,          // Key = 16-bit integer
};

bool GetExtendedCellFlag(
       i32 level, i32 x, i32 y, EXPOOL_DATA_TYPE ftype);

struct CHARDESC;

class EXPOOL
{
  friend i16 ReadDatabases(void);
  friend RESTARTABLE _DisplayDiskMenu(void);
  ui32 *m;
  void Setup(void);//After loading game.
  void enlarge(ui32 size);
public:
  EXPOOL(void);
  ~EXPOOL(void);
  void Cleanup(void);
  void Write(ui32 key, ui32 *record, ui32 size);
  // Read() and Locate() return record size (or -1).
  i32 Read(ui32 key, ui32 *record, ui32 max);
  i32 Locate(ui32 key, ui32 **record);
  bool IsOpen(void);
  //
  // Some functions to get/set/test particular records in expool.
  // I suppose they don't belong here but it works well for me.
  bool CheckChampionBonesRecord(const RN& bonesObject);
  bool CopyChampionBonesRecord(const RN& bonesObject, ui32 *pRecord);
  bool GetChampionBonesRecord(const RN& bonesObject, ui32 *pRecord);
  void AddChampionBonesRecord(const RN& bonesObject, ui32 fingerprint);
};


class DATABASES
{
  friend RESTARTABLE FindEmptyDBEntry(const i32,const DBTYPE,const bool);
  DBCOMMON *m_Address[16];
  i32       m_numEnt[16];
  i32       m_noEnlarge;//A bit for each DB.  Set when the
                        //player says NOT to enlarge the #entries.
  void swapDOOR(i32 dbNum);
  void swapTELEPORTER(i32 dbNum);
  void swapTEXT(i32 dbNum);
  void swapACTUATOR(i32 dbNum);
  void swapACTUATOR2(i32 dbNum);
  void swapMONSTER(i32 dbNum);
  void swapWEAPON(i32 dbNum);
  void swapCLOTHING(i32 dbNum);
  void swapSCROLL(i32 dbNum);
  void swapPOTION(i32 dbNum);
  void swapCHEST(i32 dbNum);
  void swapMISC(i32 dbNum);
  void swapEXPOOL(i32 dbNum);
  void swapMISSILE(i32 dbNum);
  void swapCLOUD(i32 dbNum);
public:
  DATABASES(void);
  ~DATABASES(void);
  void Allocate(i32 dbNum, i32 numEnt);
  void DeAllocate(i32 dbNum);
  i32 Enlarge(i32 dbNum);//Returns index (or -1 if not possible)
  RN AllocateSegment(i32 dbNum, DBTYPE dbType);
  void SetAddress(DBTYPE dbType, DBCOMMON *m);
  DBCOMMON *GetCommonAddress(RN obj);
  DBCOMMON *GetCommonAddress(DBTYPE dbType, i32 i);
  bool IsDBType(DBCOMMON *, DBTYPE);
  DBTYPE GetDBType(DBCOMMON *);
  i32    GetDBIndex(DBCOMMON *, DBTYPE dbType);
  RN FindEmptyDBEntry(DBTYPE dbType, bool important);
  void swap (i32 dbNum);
  i32 NumEntry(i32 dbNum);
  void NumEntry(i32 dbNum, i32 n);
  i32 Max(i32 dbNum){return ((dbNum==15) ? 768 : ((dbNum==3)?16000:8000));};
  // debug i32 Max(i32 dbNum){return ((dbNum==6) ? 300 : ((dbNum==3)?16000:8000));};
};


struct MOVEBUTN
{
  ui16  x1;
  ui16  x2;
  ui16  y1;
  ui16  y2;
};

struct btn
{
  i16  word0;
  i16  xMin;
  i16  xMax;
  i16  yMin;
  i16  yMax;
  i16  button; // 2=left 1=right
};

struct OBJDESC
{
private:
  i16 objType; // (see objects.h)
  ui8 graphicClass;   //d.s5604 index. (graphic info????)
  i8  attackClass;
public:
//private:
  i16 word4; // Mask of locations it can be carried. Head, foot, etc.
             //    0x0001 Can put in Mouth
             //    0x0002 Can wear on Head
             //    0x0004 Can wear around Neck
             //    0x0008 Can wear on Torso
             //    0x0010 Can wear on Legs
             //    0x0020 Can wear on Feet
             //    0x0040 Can carry in Quiver (ready)
             //    0x0080 Can carry in Quiver (not ready)
             //    0x0100 Can carry in Pouch.  Can pass thru doors (except keys)
             //    0x0400 Can carry in Chest
public:
  OBJ_NAME_INDEX objectType(void) {return (OBJ_NAME_INDEX)objType;};
  void objectType(OBJ_NAME_INDEX ot) {objType=(i16)ot;};
  i32  GraphicClass(void){return graphicClass;};
  i32  AttackClass(void){return attackClass;};
  void littleEndian(void)
  {
    objType = LE16(objType);
    word4 = LE16(word4);
  };
};

struct CLOTHINGDESC
{
private:
  ui8 m_weight;
  ui8 m_uByte1;
  ui8 m_uByte2;
  ui8 m_uByte3;
public:
  ui8 Weight(void) {return m_weight;};
  ui8 uByte1(void) {return m_uByte1;};
  ui8 uByte2(void) {return m_uByte2;};
};


struct MouseQueueEnt
{
  ui16 x;
  ui16 y;
  ui16 num;
};

bool mouseQueueIsEmpty(void);

class SPELL
{
public:
  i32  spellID; // Top 8 bits special??
  ui8  skillRequired;
  ui8  byte5;
  i16  word6; // bits 0-3 = spellType
              //    1 = needs flask
              //    2 = Missile type 0xff80 + bits4_9
              //    3 = Light, for example.
              // bits 4-9 = missile type;
              // bits 10-15 = time required for character to recover
};

#define tmrAct_SET    0
#define tmrAct_CLEAR  1
#define tmrAct_TOGGLE 2

enum TIMERTYPE
{
  TT_M3           = -3,  // Monster damaged by cloud or door.
  TT_M2           = -2,  // When a monster is damaged
  TT_M1           = -1,  // When a monster is attacked.  When we bump into a monster.
  TT_EMPTY        = 0,
  TT_1            = 1,
  TT_BASH_DOOR    = 2,
  TT_MAP_FIRST    = 5,
  TT_OPENROOM     = 5,
  TT_STONEROOM    = 6,
  TT_FALSEWALL    = 7,
  TT_TELEPORTER   = 8,
  TT_PITROOM      = 9,
  TT_DOOR         = 10,
  TT_MAP_LAST     = 10,
  TT_11 = 11, //Re-enable character attack.
  TT_12 = 12,
  TT_ViAltar      = 13,
  TT_20 = 20,
  TT_22 = 22,
  TT_24 = 24, //Remove Fluxcage?
  TT_25 = 25, //Cloud processing
  TT_28 = 28,
  TT_FIRST_MONSTER_AI = 29,
  TT_29 = 29,
  TT_30 = 30,
  TT_31 = 31, // When Party ran into monster
  TT_MONSTER_Agroup = 32,
  TT_MONSTER_A0     = 33,
  TT_MONSTER_A1     = 34,
  TT_MONSTER_A2     = 35,
  TT_MONSTER_A3     = 36,
  TT_MONSTER_Bgroup = 37,
  TT_MONSTER_B0     = 38,
  TT_MONSTER_B1     = 39,
  TT_MONSTER_B2     = 40,
  TT_MONSTER_B3     = 41,
  TT_LAST_MONSTER_AI = 41,
  TT_Missile0   = 48,     //Missile Timer - first move
  TT_Missile    = 49,     //Missile Timer - second and subsequent moves
  TT_53 = 53,
  TT_60 = 60,
  TT_61 = 61,
  TT_ReactivateGenerator = 65,
  TT_LIGHT = 70,  //Modify ambiont light level.
  TT_71 = 71,
  TT_72 = 72,
  TT_73 = 73,
  TT_74 = 74,
  TT_75 = 75,
  TT_77 = 77,
  TT_78 = 78,
  TT_79 = 79,
  TT_ParameterMessage = 101,
  TT_DESSAGE = 102,  // A standard message for DSAs only
};

class TIMER
{ //  We read them from
  // DUNGEON.DAT when game reloaded.
  friend class GameTimers;
  friend void swapTimers(void);
public:
  TIMER(void) {   m_timerSequence
                = m_timerUByte5 
                = m_timerUByte6 
                = m_timerUByte7 
                = m_timerUByte8 
                = m_timerUByte9 
                = m_timerLevel
                = unused13[0]
                = unused13[1]
                = unused13[2]
                = 0xcd;
              };
private:
  // As of version 12.4, we use the entire longword (32 bits)
  // for the time.  The level has been moved to the end of the structure.
  // 20130115 PRS
  ui32 m_time; // Top 8 bits=partyLevel, bottom 24 bits = time
public:
  inline ui32 Time(void){return m_time;};
  inline void Time(ui32 time){m_time = time;};
private:
  bool operator < (const TIMER& timer2) const;
private:
  ui8  timerFunction;
    //  1
    //    Happened when I pressed open door switch.
    //         byte 6 = mapX
    //         byte 7 = mapY
    //         byte 9 =
    //  2 Delayed door bashing.
    //         byte 5 = 0
    //         byte 6 = mapX
    //         byte 7 = mapY
    //  5
    //
    //         byte 6 = mapX
    //         byte 7 = mapY
    //         byte 9 = function
    //              for DB2 fiddle bit 0 of word 2
    //                = 0 means set
    //                = 1 means clear
    //                = 2 means toggle bit 0 of word 2
    //              for DB3 (Actuator)
    //  6 Fiddle with DB2 and DB3
    //    Appears to toggle things.
    //         byte 6 = mapX
    //         byte 7 = mapY
    //         byte 8 = position when sent to object
    //                    of DB type 2 (text??)
    //                  For DB type 3 (Actuators) the
    //                  position of the actuator is
    //                  ignored.  All actuators in the
    //                  target cell receive the timer.
    //         byte 9 = 0 means set bit
    //                = 1 means clear
    //                = 2 means toggle
    //    When sent to actuator type 14 ...(see actuator type 14)
    //  7 Change bit 0x04 in CELLFLAG
    //         byte 6 = mapX
    //         byte 7 = mapY
    //         byte 9 = 0 = Set Unconditionally
    //                = 1 = If party at X,Y requeue
    //                      If material monster at X,Y requeue
    //                      else clear
    //                = 2 = Toggle (same rules as set/clear)
    //  8 Change bit 0x08 in CELLFLAG.
    //    If the final state of the bit is a one then the
    //    function 'ProcessNewlyActiveRoom' is called.
    //    The timer goes away without restarting anything.
    //         byte 6 = mapX
    //         byte 7 = mapY
    //         byte 9 = tmrAct
    //                = 0 = tmrAct_SET
    //                = 1 = tmrAct_CLEAR
    //                = 2 = tmrAct_TOGGLE
    //  9
    //         byte 6 = mapX
    //         byte 7 = mapY
    //         byte 9 =
    // 10
    //         byte 6 = mapX
    //         byte 7 = mapY
    //         byte 9 =
    // 11 Disable action after casting spell or attacking or throwing
    //         byte 5 = chIdx
    //         byte 6 = 0
    // 12 When party moves. Byte 5=chIdx.  Removes timerIndex from chIdx
    // 13 = TT_ViAltar; Happened when I put bones in Altar of Vi.
    //         This has been changed in order to put the identity of the bones in the record.
    //         packedPos()  = position of bones
    //         pacedState() = revival state = 2, 1, 0
    //         byte 6       = mapX
    //         byte 7       = mapY
    //         objectD8     =  objBones
    // 20 Something to do with sound
    // 22
    //         Processing for function 22 disabled.
    // 24
    //         byte 6 = mapX
    //         byte 7 = mapY
    //         obj  8 = object name
    //
    // 25
    //         byte 6 = mapX
    //         byte 7 = mapY
    //         object name in word 8
    //         cloud processing?
    // 29 to 41 have everything to do with monster movement
    //         This is the heart of the monster AI.  Each monster
    //         gets to do its thing when its timer expires.
    // 32 Causes monster action
    //         byte 6 = mapX
    //         byte 7 = mapY
    //         byte 8 = additional time until function changed to 37
    // 37 When character dies and bones
    //       put on floor???
    //         byte5 comes from Item9660.byte2[0]
    //         byte6 = mapX
    //         byte7 = mapY
    //         byte 8 = 0
    // 48 and 49TAG01826c  Missile????
    //         byte5 = 0
    //         word6 objectID
    //         word8 bits 10-11 direction
    //               bits  5- 9 y
    //               bits  0- 4 x
    // 53 = Watchdog Timer
    // 60 and 61 = ???
    //    After processing this is reset to time+5
    //    This appears to be a pending monster generator.
    //    A monster was generated where a monster already
    //    existed.  Wait a while and try again.
    //         bytes 0,1,2,3 = level and time
    //         byte4 = timer type = 60 or 61
    //         byte5 = unused
    //         byte6 = mapX
    //         byte7 = mapY
    //         obj8  = object to move
    // 65
    //         byte 6 = mapX
    //         byte 7 = mapY
    //         All actuators at (X,Y) with actuatorType==0
    //           get set to actuatorType 6.  Used to
    //           reactivate monster generators and such.
    // 70 Adjust Brightness (light level)
    //    Timer is reset to d.Time+4 if index has
    //    not been reduced to zero.
    //         word6 = (+/-)index to d.Byte1074
    //                 Add or subtract from d.13282
    //
    // 71 Set by 3-2-6 spell (OH EW SAR) invisibility
    // 72 Character has used shield potion (Ya potion)
    //    It is time to remove the shield.
    //         byte 5 = character index
    //         word 6 = shield increment to be removed
    // 73 set by 3-2-5 spell (OH EW RA See thru walls)
    // 74 Set by protection spell .... word6 has value to subtract from d.PartyShield
    // 75 Character hit by missile??
    ///   Character Poison effect??
    //         byte 5 = character index
    //         word 6 = damage
    //         Byte 42 of the character has the timer index.
    // 77 Some sort of spell will wear off
    //    d.SpellShield was incremented by word6
    //         word 6 = value to subtract from d.SpellShield
    // 78 Some sort of spell will wear off
    //    d.FireShield was incremented by word6
    //         word 6 = value to subtract from d.FireShield
    // 79 Result of 1-1-5-2 spell ya-bro-ros (magic footprints)
public:
  TIMERTYPE Function(void)const{return (TIMERTYPE)timerFunction;};
  void Function(TIMERTYPE f)
  {
    ASSERT(f >= TT_EMPTY,"Illegal Timer Type");
    timerFunction = (ui8)f;
  };
private:
  ui8  m_timerUByte5;
    // Commonly set to zero.
    // When sound is queued contains byte 4 of d.sound1772 entry
    // (function 37) this came from monster descriptor byte 6
    // (function 11) contains hero index.
    // (function 12) hero index
    // (function 13) hero index
    // (function 72) hero index
    // (function 75) hero index
  ui8  m_timerUByte6;  //Commonly X coordinate
  ui8  m_timerUByte7;  //Commonly Y coordinate
  ui8  m_timerUByte8;
  ui8  m_timerUByte9;  //Commonly action (set, clear toggle
public:
  inline ui8  timerUByte5(void)const{return m_timerUByte5;};
  inline void timerUByte5(ui8 v){m_timerUByte5=v;};
  inline ui8  timerUByte6(void)const{return m_timerUByte6;};
  inline void timerUByte6(ui8 v){m_timerUByte6=v;};
  inline ui8  timerUByte7(void)const{return m_timerUByte7;};
  inline void timerUByte7(ui8 v){m_timerUByte7=v;};
  inline ui8  timerUByte8(void)const{return m_timerUByte8;};
  inline void timerUByte8(ui8 v){m_timerUByte8=v;};
  inline ui8  timerUByte9(void)const{return m_timerUByte9;};
  inline void timerUByte9(ui8 v){m_timerUByte9=v;};
private:
  ui16 m_timerSequence;  //Sequence when added to queue.
                  // The sequence number ensures that entries of equal time and priority are 
                  // removed in the same order as they were inserted..
  ui8  m_timerLevel;  // Dungeon level to which the timer refers.
  ui8  unused13[3];  //To make multiple of 32 bits.  Altogether 16 bytes.
public:
  inline int  Level(void) {return m_timerLevel;};
  inline void Level(ui8 level){m_timerLevel = level;};
  i32  TimerSequence(void){return m_timerSequence;};
  ui8  packedPos(void){return (ui8)((m_timerUByte5 >> 2) & 3);};
  void packedPos(ui32 p){m_timerUByte5=(ui8)((m_timerUByte5&0xf3)|((p&3)<<2));};
  ui8  packedState(void){return (ui8)(m_timerUByte5 & 3);};
  void packedState(ui32 s){m_timerUByte5=(ui8)((m_timerUByte5&0xfc)|(s&3));};
  RN& timerObj8(void)
  {
    ASSERT(   (timerFunction==TT_24)
           || (timerFunction==TT_25)
           || (timerFunction==TT_60)
           || (timerFunction==TT_61)
           || (timerFunction==TT_ViAltar)
           || (timerFunction==TT_ReactivateGenerator),"Illegal Timer Type");
    return *(RN *)(&m_timerUByte8);
  };
  void swapTimerWord6(void);
  void swapTimerObj6(void);
  void swapTimerWord8(void);
  void swapTimerObj8(void);
  void swapTimerSequence(void);
  RN& timerObj6(void)
  {
    ASSERT((timerFunction==TT_Missile0) || (timerFunction==TT_Missile),"Illegal Timer Type");
    return *(RN *)(&m_timerUByte6);
  };
  RN *pTimerObj6(void){return (RN *)&m_timerUByte6;};
  i16& timerWord6(void)
  {
    return *(i16 *)(&m_timerUByte6);
  };
  ui16& timerUWord6(void)
  {
    return *(ui16 *)(&m_timerUByte6);
  };
  i16& timerWord8(void)
  {
    return *(i16 *)(&m_timerUByte8);
  };
  RN *pTimerObj8(void){return (RN *)&m_timerUByte8;};

};

struct DUNGEONDATINDEX
{
private:
  ui16  sentinel;
  ui16  word2;
  ui16  word4;
  ui16  word6;
  ui16  word8;
  ui16  word10;
  ui16  dbSizes[16];
public:
  ui8  NumLevel(void) {return BITS8_15(word4);};
  ui16 Sentinel(void) {return sentinel;};
  ui16 DBSize(i32 dbNum) {return dbSizes[dbNum];};
  void DBSize(i32 dbNum, ui16 size) {dbSizes[dbNum] = size;};
  ui16 LegacyCellFlagArraySize(void) {return word2;};
  void LegacyCellFlagArraySize(ui16 n){word2=n;};
  ui16 NumWordsInTextArray(void) {return word6;};
  void NumWordsInTextArray(ui16 n) {word6 = n;};
  ui8  StartingPartyX(void) {return BITS0_4(word8);};
  ui8  StartingPartyY(void) {return BITS5_9(word8);};
  ui8  StartingPartyFacing(void) {return BITS10_11(word8);};
  ui16 ObjectListLength(void) {return word10;};
  void ObjectListLength(ui16 v) {word10 = v;};
  void Swap(void);
};

struct LEVELDESC
{ // Describes a level.  We read them from
  // DUNGEON.DAT
  friend void swapLevelDescriptors(i32);// Needs to swap words
  friend i16 ReadDatabases(void);
  ui16 cellOffset;  // littleEndian when read
                    //Offset of first cell in CELLFLAG array
                    //Actually, only the least significant 16 bits.
  i8  FILL2[4];
  ui8 offsetX; //6;
  ui8 offsetY; //7;
private:
  i16 word8;  // bits  0-5  = level number
              // bits  6-10 = # columns -1
              // bits 11-15 = # rows - 1
  i16 word10; // bits 0-3 = #wall decorations used on level
              // 4-7,     = #wall decorations that can ce used randomly
              // 8-11,    = #floor decorations used in this level
              // 12-15

  i16 word12; // bits 0-3,  = number of DoorDecorations
              // bits 4-7   = number monsters types
              // bits 12-15 = experience multiplier
  i16 word14; // bits 0-3   = Floor and ceiling graphic (Multiply
              //                by 2 and add 75 and 76 to get
              //                 graphic numbers)
              // bits 4-7
              // bits 8-11, // First door type on this level
              // bits 12-15 // Second door type on this level
public:
  ui8 LastColumn(void) { return BITS6_10(word8);};
  ui8 LastRow(void) { return BITS11_15(word8);};
  ui8 Level(void) { return BITS0_5(word8);};
  ui8 NumWallDecoration(void) {return BITS0_3(word10);};
  ui8 NumRandomWallDecoration(void) {return BITS4_7(word10);};
  ui8 NumFloorDecorations(void) {return BITS8_11(word10);};
  ui8 NumRandomFloorDecorations(void) {return BITS12_15(word10);};
  ui8 NumDoorDecorations(void) {return BITS0_3(word12);};
  ui8 numMonsterType(void) {return BITS4_7(word12);};
         //The monster types are listed just after the cellflags.
         //This is also the offset of the wall decoration
         //table in the information following the cellflags.
  ui8 experienceMultiplier(void) {return BITS12_15(word12);};
  ui8 backgroundGraphicSet(void) {return BITS0_3(word14);};
  ui8 wallGraphicSet(void) {return BITS4_7(word14);};
  ui8 Door1Type(void) {return BITS8_11(word14);};
  ui8 Door2Type(void) {return BITS12_15(word14);};
};

struct SOUND
{
  i16 word0;
  i8 byte2;
  i8 byte3;
  i8 byte4;
  i8 byte5;
  i8 byte6;
  i8 byte7;
};

struct GRAPHIC_CLASS // An object like an apple
{
  i8 objectGraphicIndex; //Graphic number-360 ??
  ui8 uByte1;
  i8 width;  // source graphic width in bytes (multiple of 8)
  i8 height; // source height
  i8 byte4;  // flag 0x01 means ???? (See DrawSingleObject)
             // flag 0x10 means ???? (See ProcessOneObject)
  i8 byte5;
};

struct ITEM12 //Occurs 27 times (for each monster type?)
{ // Don't know what this is.  Part of graphic #0x22e.
  i16 ITEM12_word0; // littleEndian when we read file.
  i16 ITEM12_word2; // initialized tp 0x1ef + nnnn*monster#
                    // Probably basic graphic number????
  ui8 frontRearByteWidth; //uByte4; //x Front or rear graphic
  ui8 frontRearHeight; //y Front or rear graphic
  ui8 sideByteWidth; //x Side graphic
  ui8 sideHeight; //y Side graphic
  ui8 attackByteWidth; //x Attack Graphic
  ui8 attackHeight; //y Attack Graphic
  ui8 uByte10; // bits 0-3 = transparent color
               // bits 4-7 index into ITEM110
  ui8 uByte11;
};

class SINGLE_MONSTER_STATUS
{
private:
  ui8 sms;
public:
  void Clear(void){sms=0;};
  bool TestAttacking(void){return (sms&0x80)!=0;};
  void SetAttacking(void){sms|=0x80;};
  void SetNotAttacking(void){sms&=0x7f;};
  bool TestMirror(void){return (sms&0x40)!=0;};
  void SetMirror(void){sms|=0x40;};
  void ClearMirror(void){sms&=0xbf;};
  void ToggleMirror(void){sms^=0x40;};
  ui8  Nibble0(void){return (ui8)(sms&7);};
  ui8  Nibble3(void){return (ui8)((sms>>3)&7);};
  void Nibble0(ui32 v){sms=(ui8)(sms&0xf8|v&7);};
  void Nibble3(ui32 v){sms=(ui8)(sms&0xc7|((v&7)<<3));};
};

struct ITEM16
{
  i16 word0;    // monster (DB4) index //swapped when read
private:
  ui8 facing_2; // Four 2-bit fields.
                // Each member of group is in 2 bits starting from bottom.
  ui8 pos_3;    // Four 2-bit fields
                // The position within the room of each
                // member of group.  If the monster occupies 2
                // corners of the room then its position
                // is either 0 or 2.
public:
  ui8 uByte4;   // Least significant byte of d.Time
                // set to d.Time-127
  ui8 uByte5;   // Direction facing??? Giggler got this set to
                // Random(64) + 20.  Time to pause????
  ui8 uByte6;   // mapX square being approached
  ui8 uByte7;   // mapY square being approached
  ui8 ubPrevX;   // 
  ui8 ubPrevY;   // compared to new mapY
  ui8 uByte10;  // mapX
  ui8 uByte11;  // mapY
  SINGLE_MONSTER_STATUS singleMonsterStatus[4];
                // Treated as array by TAG00ecca(ProcessTimer25)
                // Bits 2-0 contain positive or negative 2-bit number
                // Bits 5-3 contain positive or negative 2-bit number
                //
                // Some flags;
                // 0x40 set/cleared 50 percent chance in MonsterUpdateTime
                // 0x80 is cleared when Monster Movement
                //      Monster Attacking????
                //      Timers are removed;
                //      If Dispell Missile strikes Zytaz and 0x80
                //         is set then damage occurs
                  
  inline ui8& positions(void) {return pos_3;};
  inline ui8& facings(void) {return facing_2;};
};

struct MONSTERDESC;
struct ALTMONGRAPH;

struct MONSTERDESC_WORD4
{
  friend void DumpMonster(FILE *f, i32 i, MONSTERDESC& mtDesc);
  friend ALTMONGRAPH *FindAltMonsterGraphic(i32 altNum);
private:
  ui16 mdw4;
  ui16 Word(void){return mdw4;};
public:
  MONSTERDESC_WORD4(void){mdw4=0;};
  void Swap(void){mdw4=LE16(mdw4);};
  ui8  Nibble0(void)  {return (ui8)(mdw4&3);};
  ui8  Nibble12(void) {return (ui8)((mdw4>>12)&3);};
  ui8  Nibble14(void) {return (ui8)((mdw4>>14)&3);};
  bool NonAttackingMirrorOK(void) {return (mdw4&0x0004)!=0;};
  bool HasSideGraphic(void)       {return (mdw4&0x0008)!=0;};
  bool HasBackGraphic(void)       {return (mdw4&0x0010)!=0;};
  bool HasAttackGraphic(void)     {return (mdw4&0x0020)!=0;};
  bool HasBit7(void)              {return (mdw4&0x0080)!=0;};
  bool HasBit8(void)              {return (mdw4&0x0100)!=0;};
  bool AttackingMirrorOK(void)    {return (mdw4&0x0200)!=0;};
  bool HasBit10(void)             {return (mdw4&0x0400)!=0;};
};


struct MONSTERDESC
{ //
  friend void DumpMonster(FILE *f, i32 i, MONSTERDESC& monster);
  friend void fixItem26(MONSTERDESC *pItem);
  friend ALTMONGRAPH *FindAltMonsterGraphic(i32 altNum);
  friend void ReadTablesFromGraphicsFile(void);
  
  ui8 uByte0; // Bits 0-1 tested for == 1
              // This is the index of the entry itself!
              //  In other words, the monster type number.
  ui8 attackSound; // Sound Ordinal. If non-zero, causes sound in MonsterAttacks (TAG00c8c0).
private:
  i16 word2;  //swapped when read
              // bits 0-1 = 01 means monster occupies 2 of the 4 corners (worm)
              //          = 02 means monster occupies entire room (dragon)
              // bit 2 Can see in all directions
              // bit 3 tested when checking for 'Blocked'
              // bit 4 ????
              // bit 5 = Levitated? Cannot fall through pit.?
              // bit 6 = something to do with Dispell and passing through doors ...
              //         = nonMaterial
              // bit 7 = immune to missiles (except RNff83)
              // Bits 7-8 Size when passing through door?
              // Bit 9  = Leaves Droppings (like worm rounds)
              // bit 10 = something to do with throwing weapon at monster
              //          Asorbs arrows, slayers, darts, stars, daggers
              // bit 11 = can see invisible
              // Bit 12 = can see in dark.
              // Bit 13 = impervious to damage???
public:
  bool nonMaterial(void)    const {return (word2&0x40)!=0;};
  ui8  horizontalSize(void) const {return (ui8)(word2 & 3);};
  ui8  verticalSize(void)   const {return (ui8)((word2&0x180)>>7);};
  bool leavesDroppings(void)      {return (word2&0x200)!=0;};
  bool invincible(void)     const {return (word2&0x2000)!=0;};
  bool absorbsDaggersEtc(void)    {return (word2&0x400)!=0;};
  bool levitating(void)     const {return (word2&0x20)!=0;};
  bool seesInvisible(void)        {return (word2&0x800)!=0;};
  bool See360(void)               {return (word2&0x4)!=0;};
  bool word2Bit3(void)            {return (word2&0x8)!=0;};
  bool word2Bit4(void)            {return (word2&0x10)!=0;};
  bool canSeeInDark(void)         {return (word2&0x1000)!=0;};
  MONSTERDESC_WORD4 word4;  //littleEndian when read from file
              //  Bits 12-13 = ?
              //  Bits 14-15 = ?
              //Flag 0x0004 = ??????
              //Flag 0x0008 = Side View Exists??????
              //Flag 0x0010 = Back View Exists??????
              //Flag 0x0020 = Attack View Exists??????
              //Flag 0x0080 = ??????
              //Flag 0x0100 = ??????
              //Flag 0x0200 = ??????

private:
  ui8  movementTicks06;// subtracted from 255 --> TIMER.byte5..ticks per movement?
public:
  ui8  attackTicks07;
  //ui8  uByte8[6];//[0] = nominal HP???//resistance to magic????
  //               //[1] =
  //               //[2] =
  //               //[3] =
  //               //[4] = <Compared to character's quickness>
  //               //[5] =
  ui8  defense08;
  ui8  baseHealth09;
  ui8  attack10;
  ui8  poisonAttack11;
  ui8  dexterity12;
  ui8  unused13;
private:
  i16 word14; //swapped when read
              //bits 0-3 // How far monster can see...
              //  This is reduced 1/2 for each level of darkness
              //bits 8-11 = Smelling Power??
              //             Compared to distance to prey.
              //             Turn towards prey if this close??
              //bits 12-15 = ??? (actual values are 1, 3, 4, and 6)
              //             Compared to distance to prey.

  i16 word16; //swapped when read
              //bits 4-7 = bravery (15 = absolutely never afraid
              //           Resistance to WARCRY, CALM, BRANDISH, BLOWHORN
              //bits 8-11
              //bits12-15
  i16 word18; //swapped when read
              //Bits 4-7 =
              //Bits 8-11 =
  i16 word20; //swapped when read
              //bits 0-3 = ???
              //bits 4-7 = time increment when not attacking
              //bits 8-11 = time increment when attacking
public:
  ui8 uByte22[4];
  i16 word0(void)            const {return (i16)((uByte0<<8) | attackSound);};
  i16 word22(void)           const {return (i16)((uByte22[0]<<8) | uByte22[1]);};
  ui8 sightDistance(void)    const {return (UI8)(word14 & 15);};
  ui8 smellingDistance(void) const {return BITS8_11(word14);};
  ui8 word14_12_15(void)     const {return BITS12_15(word14);};
  ui8 bravery(void)          const {return BITS4_7(word16);};
  ui8 word16_8_11(void)      const {return BITS8_11(word16);};
  ui8 word16_12_15(void)     const {return BITS12_15(word16);};
  ui8 FireResistance(void)   const {return BITS4_7(word18);};
  ui8 PoisonResistance(void) const {return BITS8_11(word18);}
  int AdditionalDelayFromAttackToMove(void) const {return BITS8_11(word20);};;
  int AdditionalDelayFromMoveToAttack(void) const {return BITS4_7(word20);};;
  int Word20_0_3(void)                      const {return BITS0_3(word20);};
  ui8 timePerMove(void)      const {return movementTicks06;};
};

struct ITEM100s
{
  i8  byte0[10];
};

struct VIEWPORTPOS
{
  ITEM100s  s[10];
};

struct ITEM110s
{
  ui8 MonsterXYonScreen[10]; //Pairs of bytes...for each position in cell
};

struct ITEM110
{
  ITEM110s s[11];//indexed by relative cell number 0-10
};

struct ITEMQ
{ // Seems to have something to do with graphic cache.
  // The 'Q' stands for 'question mark'.
  i32  size;  // including this, the header, the
              // data for the graphic itself, and
              // 4 bytes at the end for the size
              // to be repeated.
  i16  unused1;//word4;
  i16  unused2;  // signed so test for -1 works
  i16  unused3;  // signed so test for -1 works
  i16  graphicNum;
  // The following four functions handle the
  // fetch/set of the pointers to the previous and
  // next list entry when this entry is in the
  // unused list.  The pointers use the same
  // space as word4, prev, next, and graphicNum.
  //ITEMQ *pgPrev(void){return *(ITEMQ **)(&word4);};
  //ITEMQ *pgNext(void){return *(ITEMQ **)(&next);};
  //void   pgPrev(ITEMQ *pg){(*(ITEMQ **)(&word4))=pg;};
  //void   pgNext(ITEMQ *pg){(*(ITEMQ **)(&next))=pg;};
};



class ATTRIBUTE // 3-byte structure in character
{
private:
  // Let me tell you why I hid these variables.....
  // From what I can tell, these are treated as unsigned
  // bytes.  But at least one of them can go negative.
  // The Luck-Minimum starts at 10 and is decreased by
  // three for each cursed object.  Gather four such
  // objects and you have set the minimum to a very
  // large number.  So I want to handle them exactly the
  // same except that when you ask for a number and it
  // has gone negative, I will return a zero.  But its
  // value will export to the Atari perfectly.
  ui8 ubMaximum;
  ui8 ubCurrent;
  ui8 ubMinimum;
public:
  ui8 Minimum(void);
  void Minimum(i32);
  void IncrMinimum(i32 increment); // Minus to decrement
  ui8 Maximum(void);
  void Maximum(i32);
  void IncrMaximum(i32 increment); // Minus to decrement
  ui8 Current(void);
  void Current(i32);
  void IncrCurrent(i32 increment); // Minus to decrement
};

#define LimitSkillExperience(a) ((a)>0x10000000?0x10000000:(a))

struct SKILL // 6-byte structure in character
{
  i16 tempAdjust;  // Temporary adjustment to experience.
  i32 experience;  // Experience in this skill
};

enum skillNames {
  SKILL_Fighter  = 0,
  SKILL_Ninja    = 1,
  SKILL_Priest   = 2,
  SKILL_Wizard   = 3
};

#define MAXHP 999
#define MAXSTAMINA 9999
#define MAXMANA 900


enum CHARFLAGS
{
  CHARFLAG_nonGraphicFlags  = 0x007f,
  CHARFLAG_attackDisable    = 0x0008,
  CHARFLAG_male             = 0x0010,
  CHARFLAG_cursor           = 0x0080,
  CHARFLAG_statsChanged     = 0x0100,  // Mana, hp, stamina, food, water, poison, etc
  CHARFLAG_possession       = 0x0200,
//  CHARFLAG_0280             = 0x0280,
//  CHARFLAG_0300             = 0x0300,
  CHARFLAG_positionChanged  = 0x0400,
//  CHARFLAG_0680             = 0x0680,
  CHARFLAG_chestOrScroll    = 0x0800,
//  CHARFLAG_0900             = 0x0900,
  CHARFLAG_portraitChanged  = 0x1000, // Born, died, shield change, etc
  CHARFLAG_ouches           = 0x2000,
//  CHARFLAG_2200             = 0x2200,
  CHARFLAG_viewportChanged  = 0x4000,
//  CHARFLAG_5b80             = 0x5b80,
  CHARFLAG_weaponAttack     = 0x8000, // Change in weapon or attack status
//  CHARFLAG_8008             = 0x8008,
//  CHARFLAG_9400             = 0x9400,
  CHARFLAG_a180             = 0xa180,
  CHARFLAG_graphicFlags     = 0xff80,
};

struct CHARDESC // character info???
{ 
  friend void swapCharacter(i32 i);
  char  name[8];  // 00
  char  title[16];// 08 // size??
  i16 wordx24; // Not swapped because I don't see it used.
  ui8 FILL26[28-26];
  ui8 facing;  //28      // Absolute (not relative to party!)
  ui8 charPosition; //29 // Absolute (not relative to party!)
  ui8 byte30;
  ui8 byte31;
   i8 attackType; //Signed so check for -1 works.
   i8 byte33;
   i8 incantation[4];//34;
        // [0] = power 96 through 111 --> 1 through 6
        // [1] =      102 through 107
        // [2] =      108 through 113
        // [3] =      114 through 119
  ui8 FILL38[40-38];
  ui8 facing3;         //40
  ui8 maxRecentDamage; //41
  ui8 poisonCount;         //Poison count.  A timer for each one??
  ui8 uByte43;
  i16 busyTimer;       // 44 // Timer Index. Signed so -1 works.
  i16 timerIndex;      //46;
  i16 charFlags; // ORed with 0x280
                 // 0x8000 while selecting attack option
  i16 ouches;    //50;// mask of damaged body parts.
private:
  i16 m_wHP;         //52
  i16 m_wMaxHP;      //54
  i16 m_wStamina;    //56;
  i16 m_wMaxStamina; //58;
  i16 m_wMana;       //60;
  i16 m_wMaxMana;    //62;
public:
  inline i16 HP(void){return m_wHP;};
  void HP(i32 h);
  void IncrHP(i32 i);
  inline i16 MaxHP(void){return m_wMaxHP;};
  void MaxHP(i32 m);
  void IncrMaxHP(i32 i);
  inline i16 Stamina(void){return m_wStamina;};
  void Stamina(i32 s);
  inline i16 MaxStamina(void){return m_wMaxStamina;};
  void MaxStamina(i32 m);
  void IncrMaxStamina(i32 i);
  inline i16 Mana(void) {return m_wMana;};
  void Mana(i32 m);
  void IncrMana(i32 i);
  inline i16 MaxMana(void) {return m_wMaxMana;};
  void MaxMana(i32 m);
  void IncrMaxMana(i32 i);
  void SaveToWings(void);
  bool GetFromWings(ui16 fingerPrint, bool testOnly);
  bool CopyCharacter(ui32 fingerprint);
  CHARDESC *LocateInWings(ui32 fingerprint);
  i16 word64; // In the original it appeared that this was
              // intended to hold an adjustment to damage
              // resistance.  When an attack was made by a
              // character a positive or negative value was
              // added to this word.  When the attack expired
              // (when the character's attack was no longer
              // disabled) the adjustment was subtracted.
              // Unfortuately, some attacks required no time
              // for recovery.  In such cases the adjustment
              // was made when the attack occurred but, since
              // it never timed out, the adjustment was never
              // canceled.  When a player did hundreds of these
              // attacks the adjustments added up until they
              // overflowed.  Then the value suddenly changed
              // from +32767 to -32768.  It was not too terrible
              // because the final computation was limited to
              // to the range (0, 100).  But it was noticeable
              // and probably hid smaller effects of armor, etc.
              // Since this word is used for only this one thing
              // and since only one adjustment can be in effect
              // at a time I have changed it to set the value
              // and clear the value instead of adding and
              // subtracting.  Wish me luck.
  i16 food;       //66;
  i16 water;      //68;
  ATTRIBUTE Attributes[7]; // 70 (maximum, current, minimum)
                           // 70 = [0] Luck??
                           // 73 = [1] Strength
                           // 76 = [2] Dexterity
                           // 79 = [3] Wisdom
                           // 82 = [4] Vitality
                           // 85 = [5] AntiMagic
                           // 88 = [6] AntiFire
  i8  FILL91;
  SKILL skills92[20]; // 92 //0 and 4-7  =Fighter
                            //1 and 8-11 =Ninja
                            //2 and 12-15=Priest
                            //3 and 16-19=Wizard
private:
  RN  possessions[30]; //212
public:
  inline RN  Possession(int i){return possessions[i];};
  void SetPossession(int i, RN obj, bool processFilter);
  void InitializePossessions(void);
  ui16 load;//272; In 10ths of KG
  ui16 shieldStrength; //274;
  ui32 talents;        // 276
  ui16 fingerPrint;    // 280
  ui16 causeOfDamage;  // 282 enum COD
  ui16 monsterCausingDamage; // 284
  ui8  FILL286[336-286];
//  ui8 FILL276[336-276];
  i8  portrait[1]; //336
  ui8 FILL337[800-337];
};

//typedef DUDAD16 *pCONTENTS;

struct PALETTE
{
  i16 color[16];
};

union dReg
{
  i8   B[4];
  ui8  UB[4];
  i16  W[2];
  ui16 UW[2];
  i32  L[1];
  ui32 UL[1];
};

#ifdef _littleEndian
#define loByte 0
#define loWord 0
#define hiWord 1
#else
#define loByte 3
#define loWord 1
#define hiWord 0
#endif


#define D0B (D0.B[loByte])
#define D1B (D1.B[loByte])
#define D2B (D2.B[loByte])
#define D3B (D3.B[loByte])
#define D4B (D4.B[loByte])
#define D5B (D5.B[loByte])
#define D6B (D6.B[loByte])
#define D7B (D7.B[loByte])

#define D0UB (D0.UB[loByte])
#define D3UB (D3.UB[loByte])
#define D4UB (D4.UB[loByte])
#define D5UB (D5.UB[loByte])
#define D6UB (D6.UB[loByte])
#define D7UB (D7.UB[loByte])

#define D0W (D0.W[loWord])
#define D1W (D1.W[loWord])
#define D2W (D2.W[loWord])
#define D3W (D3.W[loWord])
#define D4W (D4.W[loWord])
#define D5W (D5.W[loWord])
#define D6W (D6.W[loWord])
#define D7W (D7.W[loWord])

#define D0UW (D0.UW[loWord])
#define D1UW (D1.UW[loWord])
#define D2UW (D2.UW[loWord])
#define D3UW (D3.UW[loWord])
#define D4UW (D4.UW[loWord])
#define D5UW (D5.UW[loWord])
#define D6UW (D6.UW[loWord])
#define D7UW (D7.UW[loWord])

#define D0L (D0.L[0])
#define D1L (D1.L[0])
#define D2L (D2.L[0])
#define D3L (D3.L[0])
#define D4L (D4.L[0])
#define D5L (D5.L[0])
#define D6L (D6.L[0])
#define D7L (D7.L[0])

#define D0UL (D0.UL[0])
#define D1UL (D1.UL[0])
#define D2UL (D2.UL[0])


//Upper halves (first half) (most significant half)
#define D0H1 (D0.W[hiWord])
#define D1H1 (D1.W[hiWord])
#define D2H1 (D2.W[hiWord])
#define D3H1 (D3.W[hiWord])
#define D4H1 (D4.W[hiWord])
#define D5H1 (D5.W[hiWord])
#define D6H1 (D6.W[hiWord])
#define D7H1 (D7.W[hiWord])

#define D0UH1 (D0.UW[hiWord])
#define D1UH1 (D1.UW[hiWord])
#define D2UH1 (D2.UW[hiWord])
#define D3UH1 (D3.UW[hiWord])
#define D4UH1 (D4.UW[hiWord])
#define D5UH1 (D5.UW[hiWord])
#define D6UH1 (D6.UW[hiWord])
#define D7UH1 (D7.UW[hiWord])


//Lower halves (second half) (less significant half)
#define D0H2 (D0.W[loWord])
#define D1H2 (D1.W[loWord])
#define D2H2 (D2.W[loWord])
#define D3H2 (D3.W[loWord])
#define D4H2 (D4.W[loWord])
#define D5H2 (D5.W[loWord])
#define D6H2 (D6.W[loWord])
#define D7H2 (D7.W[loWord])

#define D0UH2 (D0.UW[loWord])
#define D1UH2 (D1.UW[loWord])
#define D2UH2 (D2.UW[loWord])
#define D3UH2 (D3.UW[loWord])
#define D4UH2 (D4.UW[loWord])
#define D5UH2 (D5.UW[loWord])
#define D7UH2 (D7.UW[loWord])


#define SWAP(d) d##L=d##UH1|((d##UH2)<<16)
//inline void SWAP(dReg& d) {d.L=d.UH[1]|(d.UH[0]<<16);};

//typedef i32 dReg;
typedef pnt aReg;

//static inline pnt UNKNOWN2pnt(UNKNOWN *p) {return (pnt)p;}
static inline i16 *puw2pw(ui16 *p) {return (i16 *)p;};
//static inline i16 *UNKNOWN2pWord(UNKNOWN *p) {return (i16 *)p;};

extern i32 GameMode;
//void SaveRegs(ui16 mask);
//void RestoreRegs(ui16 mask);

void logMsg(char *msg);
void AsciiDump(void);
bool verifyRectPos(RectPos *rectpos);
//void verifyppq(void);

//void display(CDC *pDC, i8 *STScreen);
void display(void);
//void pumper(void); // Pump any queued Windows messages
void checkVBL(void); //Sort of a pumper replacement.
void RevealViewport(void); // Debug aid
void ClearViewport(void); // Debug aid
//void BuildSmartDiscardTable(void);
void SmartDiscard(bool Initialize);

void printAbsLocation(FILE *f, i32 level, i32 x, i32 y);

//inline void SWAP(dReg& d)
//{
//
//};
void checkMemory(void);
void Terminate(void);
//void die(i32 errornum, CString msg="");
void die(i32 errornum, const char * = NULL);
void NotImplemented(i32 i);
void SorryNotImplemented(i32 i);
void HopefullyNotNeeded(i32 i);
//i16& wordGear(pnt p);
//i16& wordGear(ui8 *p);
#define wordGear(A) (*((i16 *)(A)))

#ifdef _DEBUG
i32& longGear(ui8 *p);
#else
#define longGear(A) (*((i32 *)(A)))
#endif

RN&  RNGear(pnt p);
pnt& pntGear(pnt p);
upnt& upntGear(pnt p);
void swap4words(i16 *words);

//void checkVBL(void)
void Invalidate(void);
void SetSupervisorMode(void);
void ClearSupervisorMode(void);
void DisableCursor(void);
i32  GetAvailMemory(void) ;
i16 Timer(pnt p, i16 d);


struct SPELL_PARAMETERS
{
  i32 action; //0 Proceed, using new parameters
              //1 Abort - no message
              //2 Abort - "That spell is difficult. Please study it again."
              //3 Abort = "THE SPELL FIZZLES."
              //4 Abort = "THE SPELL FIZZLES AND DIES."
  i32 incantation;
  i32 chIdx;
  i32 disableTime;
  i32 missileType;
  i32 locationAbs;
  i32 facing;
  i32 skillRequired;
  i32 spellByte5;
  i32 spellClass;
  i32 unused1;
  i32 unused2;
  i32 unused3;
  i32 unused4;
};

class FILETABLE
{
  FILE *m_file;
  bool m_enciphered;
  char *m_fileName;
public:
  FILETABLE(void);
  ~FILETABLE(void);
  FILE *GetFile(void){return m_file;};
  char *GetFileName(void){return m_fileName;};
  void SetFile(FILE *f, const char *fileName);
  void Enciphered(bool e){m_enciphered = e;};
  bool Enciphered(void){return m_enciphered;};
  void Cleanup(void);
};


enum MONSTEREVENTS
{
  MDF6TI_turnMonsterTowardParty                   = 0,
  TMAG_tunrMonsterGroup                           = 1,
  TMAG_turnMonsterGroup                           = 2,
  PIn_turnMonsterGroup                            = 3,
  PIn_moveTwoSquaresSucceeded                     = 4,
  PIn_moveTwoSquaresFailed                        = 5,
  SF_greaterThanSmellingDistance                  = 6,
  SF_return                                       = 7,
  SF_DeleteTimersMaybeSetFear                     = 8,
  SF_timeFuncMinusThree                           = 9,
  SF_TurnMonstersAsGroup                          = 10,
  SF_IncrementTime                                = 11,
  PIAF_processInvincible                          = 13,
  PIAF_standardFinish                             = 14,
  T524_possibleMove                               = 15,
  T524_moveSucceeded                              = 16,
  T524_moveFailed                                 = 17,
  T524_processInvincibleAndFinish                 = 18,
  T524_mabeDeleteTimersFear6TurnIndividuals       = 19,
  T524_ProcessInvincibleAndFinish                 = 20,
  IC29to41_atLeastOneMemberAlreadyDead            = 21,
  IC29to41_damageMonsterSucceeded                 = 22,
  IC29to41_atLeastOneMemberAlreadDead             = 23,
  IC29to41_damageMonsterFailed                    = 24,
  IC29to41_randomMoveSucceeded                    = 25,
  IC29to41_randomMoveFailed                       = 26,
  IC29to41_doNothing                              = 27,
  IC29to41_exitFalse                              = 28,
  TT29to41_TT31                                   = 29,
  TT29to41_fear5or6                               = 30,
  TT29to41_TT30                                   = 31,
  TT29to41_fear5or6doNothing                      = 32,
  TT29to41_TT30turnAsGroup                        = 33,
  TT29to41_TT30deleteTimersMaybeSetFear           = 34,
  TT29to41_TT30IncrementTimeByW52PlusRandom       = 35,
  TT29to41_TT30TryDirectionsD5toD4                = 36,
  TT29to41_TT29                                   = 37,
  TT29to41_TTmonsterA3                            = 38,
  TT29to41_blocked                                = 39,
  TT29to41_fearNot5or6                            = 40,
  TT29to41_MaybeDeleteTimersFear6TurnIndividuals  = 41,
  TT29to41_fear0or3                               = 42,
  TT29to41_fear6                                  = 43,
  TT29to41_w30w32GretaterThan3                    = 44,
  TT29to41_TTnot37                                = 45,
  TT29to41_fear5                                  = 46,
  TT29to41_attacking                              = 47,
  TT29to41_notAttacking                           = 48,
  IC29to41_Not32_33_37_38                         = 49,
  MoveObject_NotAllowedOnLevel                    = 50,
  MonMove_differentCell                           = 51,
};

enum DSAVAR_TYPE
{
  DVT_Undefined = 0,    //Never been referenced.
  DVT_WellDefined = 1,  //Has a nice value like 1 or 2.
  DVT_NonParameter = 2  //Non-existent parameter referenced by &PARAM@
};

struct DSAVARS
{
  ui8   definedFlags[100];
  ui32  values[100];
  DSAVARS(void){memset(definedFlags,DVT_Undefined,100);};
};

class MMRECORD
{
public:
  ui32        flgs[2];
  bool        CalledPreMoveFilter;
  LOCATIONREL locr;
  LOCATIONREL finalLocr;
  TIMER       timer;
  RN          objMonster;
  DSAVARS     dsaVars;

  MMRECORD(void){flgs[0]=flgs[1]=0;CalledPreMoveFilter=false;};
  void Setflg(MONSTEREVENTS f){flgs[f/32]|=(1<<(f%32));};
  bool Getflg(MONSTEREVENTS f){return (flgs[f/32] & (1<<(f%32)))!=0;};
};


enum ADJUSTSKILLSWHY
{
  ASW_Unknown                 = 0,
  ASW_PhysicalAttack          = 1,
  ASW_WarCryEtc               = 2,
  ASW_Attack                  = 3,
  ASW_CastSpell1              = 4,
  ASW_CastSpell2              = 5,
  ASW_MonsterDamagesCharacter = 6,
  ASW_SkillIncreaser1         = 7,
  ASW_SkillIncreaser2         = 8,
  ASW_ThrowByCharacter        = 9,
};

extern i32 adjustSkillsParameters[5];

enum DELETEMONSTERWHY
{
  DMW_Unknown                 = 0,
  DMW_Fusion                  = 1,
  DMW_MakeRoom                = 2,
  DMW_Damage                  = 3,
  DMW_Movement1               = 4,
  DMW_Movement2               = 5,
  DMW_numWhy                  = 6,
};

enum MONSTER_DAMAGE_RESULT
{
  NO_MONSTER_DIED = 0,
  SOME_NOT_ALL_MONSTERS_DIED=1,
  ALL_MONSTERS_DIED=2
};



class DELETEMONSTERPARAMETERS
{
public:
  i32              monsterID;
  i32              level;
  i32              x;
  i32              y;
  DELETEMONSTERWHY why;
  i32              dropResidue; // The pre-defined items like Worm Rounds or Screamer Slices
                                // 0=default; 1=drop; 2=noDrop;
  i32              deathCloud;  //0=default; 1=show cloud; 2=Do not show cloud;
  i32              index;       // index in group (-1=entire group).
  //
  DELETEMONSTERPARAMETERS(DELETEMONSTERWHY w, i32 idx);
};

void ProcessMonsterDeleteFilter(i32 mapX, i32 mapY, DELETEMONSTERPARAMETERS *pdmp, i32 attachedLevel);


enum HC
{
  HC1 = 1,
  HC2,
  HC3,
  HC4,
  HC5,
  HC6,
  HC7,
  HC8,
  HC9,
  HC10,
  HC11,
  HC12,
  HC13,
  HC14,
  HC15,
  HC16,
  HC17,
  HC18,
  HC19,
  HC20,
  HC21,
  HC22,
  HC23,
  HC24,
  HC25,
  HC26,
  HC27,
  HC28,
  HC29,
  HC30,
  HC31,
  HC32,
  HC33,
  HC34,
  HC35,
  HC36,
  HC37,
  HC38,
  HC39,
  HC40,
  HC41,
  HC42,
  HC43,
  HC44,
  HC45,
  HC46,
  HC47,
  HC48,
  HC49,
  HC50,
  HC51,
  HC52,
  HC53,
  HC54,
  HC55,
  HC56,
  HC57,
  HC58,
  HC59,
  HC60,
  HC61,
  HC62,
  HC63,
  HCyy,
  HCzz,
  HCaaa,
  HCbbb,
  HCccc,
  HCddd,
  HCeee,
  HCfff,
  HCggg,
  HChhh,
  HCiii,
  HCjjj,
  HCkkk,
  HClll,
  HCmmm,
};


//i32  DIRECT_CONIN(void); // Trap 1 #0x07
//i16  CONSTAT(void);      // Trap 1 #0x0b
i16  OPEN(const char *name, const char *flags);
ui64 MODIFIEDTIME(i32 file); // seconds since the beginning
void SETENCIPHERED(i16 f, unsigned char *key, i32 keylen);
FILETABLE *GETFILETABLE(i16 f);
FILE *GETFILE(i32 f);
i16  CREATE(const char *name, const char *flags, bool dieIfError);
i32  READ(i32 file, i32 len, ui8 *buf);
char *GETS(char *buf, i32 max, i16 file);
i32  WRITE(i16 file, i32 len, ui8 *buf);
i16  UNLINK(char *name);
pnt  GETDTA(void);
void SETDTA(pnt);
i32  LSEEK(i32 offset, i32 file, i32 origin);
i32  RENAME(i32 trash, const char *name1, const char *name2);
void PRINTLINE(const char *message);//(void)
void setpalette(PALETTE *palette);
void ForceScreenDraw(void);
//void VerifyGraphicFreeList(void);
i32 Smaller(i32, i32);
i32 Larger(i32, i32);
i32 TwoBitNibble(i32 packedValue, i32 nibbleNum);

#define StartCSB(n) CALL0(n,StartCSB)
RESTARTABLE _StartCSB(const CSB_UI_MESSAGE *);//(void)
#define StartChaos(n) CALL0(n,StartChaos)
RESTARTABLE _StartChaos(const CSB_UI_MESSAGE *);//(void)
#define StartHint(n) CALL0(n,StartHint)
RESTARTABLE _StartHint(const CSB_UI_MESSAGE *);//(void)
#define AskWhatToDo(n) CALL0(n,AskWhatToDo)
RESTARTABLE _AskWhatToDo(void);//(void)
void DispatchCSB(CSB_UI_MESSAGE *);
void DispatchCSB(CODESTATE);
//void InitializeDBank(void);
void jdisint(i16 P1);
void jenabint(i16 P1);
void xbtimer(i16 timer, i16 control, i16 data, void (*f)(void));
#define wvbl(n) CALL0(n,wvbl)
RESTARTABLE _wvbl(const CSB_UI_MESSAGE *);
ui8 *physbase(void);
i16  giaccess(i32 data, i32 Register);
void playSound(i32 soundNum);
ui8 *dosound(ui8 *P1); // Start sound stream
void setscreen(ui8 *logaddr, ui8 *physaddr, i16 res);
#define MainLoop(n) CALL0(n,MainLoop)
RESTARTABLE _MainLoop(const CSB_UI_MESSAGE *);//TAG00068e//(void)
//#define LoadPartyLevel(n,a) CALL1(n,LoadPartyLevel,a)
void LoadPartyLevel(const i32 P1);//TAG000850(void)
//   TAG00091c
void vblFlashButn(i16 x1, i16 x2, i16 y1, i16 y2); // called by VBL handler
void MemMove(ui8 *src, ui8 *dest, i32 byteCount); //TAG0009dc
void ClearMemory(ui8 *dest, i32 numByte); // TAG000a84
void fillWithByte(ui8 *addr, i16 num, i8 value, i16 spacing);//TAG000ac0
void fillMemory(i16 *pwAddr, i32 num, i16 value, i16 spacing); //TAG000af6
void TAG000bb0(dReg& D0, dReg& D4, dReg& D5); // called by vertical blank handler
void vblInterrupt(void);  // TAG000c3a
void TAG000ec6(void);
void SystemError(i16 errorNum);//TAG000efc //
void BLT2Viewport(ui8 *, RectPos *, i16, i16);//TAG000f4a
void BLT2Screen(ui8 *P1, RectPos *P2, i16 P3, i16 P4);//TAG000f7e
void SwapGraphic0x231(void);
void SetButtonPointers(void);
ui32 Signature(char *text);
void Signature(int fileHandle, ui32 *signature1, ui32 *signature2);
#define VBLDelay(n,a) CALL1(n,VBLDelay,a);
RESTARTABLE _VBLDelay(const i32 P1);//TAG000fb2 (void)
//i16  Abs(i32 x);//TAG00fd2
//i16  Min (i32 n, i32 m); //TAG000fe2
//i16  Max(i16, i16);//TAG000ff8
#define LIMIT(min,x,max)(((x)>(max))?(max):(((x)<(min))?(min):(x)))
i16  ApplyLimits(i32 min, i32 x, i32 max);//TAG00100e
i32  STRandom(void); //TAG001030
i32  STRandom(i32);
ui8  STRandomBool(void);//TAG001050
ui8  STRandom0_3(void);//TAG001074
i16  ScaledMultiply(i16 Op1, i16 scale, i16 Op2);//TAG001098
void TAG0010ae(void);
OBJ_NAME_INDEX GetBasicObjectType(RN);//TAG00114c
//OBJ_NAME_INDEX GetObjectNameIndex(RN);//TAG001188
void DrawNameOfHeldObject(RN);//TAG0012a0
void ClearHeldObjectName(void);//TAG00133e
void GetIconGraphic(OBJ_NAME_INDEX, ui8 *); //TAG00135c
void TAG0013e4(OBJ_NAME_INDEX, i32, i32);
void DrawItem(i32 squareNumber, OBJ_NAME_INDEX objectNameIndex);//TAG00144e
OBJ_NAME_INDEX ObjectAtLocation(i32 bodyLocation);//TAG001534
//   TAG00154c
void TextOut_OneLine(ui8 *dest,
                     i32 dstwidth,
                     i32 x,
                     i32 y,
                     i32 color,
                     i32 P6,
                     const char *text,
                     i32 maxLineLength,
                     bool translate);
//   TAG0017d0
void DrawText(ui8 *dest,
              i16,
              i16 x,
              i32 y,
              i16 textcolor,
              i16 backgroundColor,
              const char* text,
              i16 length);
//void SetPrintPosition(i16 column, i16 row);//TAG001836
//void RemoveTimedOutText(void);// CALL0(n,RemoveTimedOutText)
//#define RemoveTimedOutText(n) CALL0(n,RemoveTimedOutText)
//RESTARTABLE _RemoveTimedOutText(void);
//#define TAG001992(n) CALL0(n,TAG001992)
//#define CreateNewTextRow(n) CALL0(n,CreateNewTextRow) //TAG001992
//RESTARTABLE _TAG001992(void);//(void)
//RESTARTABLE _CreateNewTextRow(void);//(void)//TAG001992
//void Printf(i32 color, pnt text);//TAG0019f8
//#define PrintLines(n,a,b) CALL2(n,PrintLines,a,b)
//RESTARTABLE _PrintLines(i32 color, pnt text); //TAG001aa8//(void)
void PrintLines(i32 color, const char *text);//TAG001aa8
void QuePrintLines(i32 color, const char *text); //TAG001aa8
void PrintLinefeed(void);//TAG001c02
void TextToViewport(i32 x, i32 y, i32 color, const char* text, bool translate);//TAG001c16
void TextOutToScreen(i32 column,
                     i32 row,
                     i32 color,
                     i32,
                     const char* pnt,
                     bool translate=false);//TAG001c42
void TAG001c6e(void);
//      001ce8 = TRAP 14
//      001cf8 = TRAP 1
void StartSound(ui8 *P1, i32 P2, i32 highVolume); // TAG001d16
i32  SoundFilter(i32 soundNum, i32 highVolume, const LOCATIONREL *soundLocr);
i32 CheckSoundQueue(void);
void TAG001e16(i16 P1);
//void TAG001e50(void); // Timer 0 handler
//void TAG001ede(void); // Sound timer handler???
//void TAG0020ca(void);
void QueueSound(i32 soundNum , 
                i32 mapX, 
                i32 mapY, 
                i32 delay);//TAG00219a
void KeyclickOff(void); //TAG002164
void StartQueuedSound(void);//TAG002336;
void TAG0023b0(void);
void SetCursorShape(i16);//TAG002456
void CreateObjectCursor(pnt P1);//TAG002480
void TAG002572(void);
void TAG0025a6(i32 P1);//(void)
void TAG002818(void);
void RemoveCursor(void); // TAG00289a
void CreateCursorBitmap(void); //TAG00295e// called by vertical blank handler
void DrawCursor(void);//TAG002c9a; // called by vertical blank handler
void MouseInterrupt(aReg A0); //TAG002ef0
void OnMouseSwitchAction(i32 buttons, i32 x=-1, i32 y=-1);//TAG002f70
void STHideCursor(HC hc); // TAG002fd2
void STShowCursor(HC hc); // TAG003026
void TAG00306c(void);
void TAG0030e6(i32 divisor, i32 dividend, i32 *quotient, i32 *remainder);
//void TAG003158(i32,i32); // i32 (left on stack) = P1*P2 (inline it)
void MoveWords(aReg& A0, aReg A1, i32 numM1); // 0031c4 // Input = A0, A1, and D0
void SearchWordNE(dReg *D0, i16 **pwA0, i16 *A1);//TAG0031d6
pnt  StrCat(char *str1, const char *str2);//TAG0031e8
i16  StrCmp(char *, char *);//TAG003222
void StrCpy(char *dst, const char *src);//TAG003264
pnt  StrCpy(char *dest, const char *src, i16 numChar); //TAG00327c
i16  StrLen(const char *string);//TAG0032ba
const char *StrChr(const char *, i16);//TAG0032e4 Find character in string
void ModifyPalettes(i32, i32);//TAG003796
//#define ReadGraphicsForLevel(n) CALL0(n,ReadGraphicsForLevel)
void ReadGraphicsForLevel(void);//(void) TAG00390e
void ReadFloorAndCeilingBitmaps(i16 P1); //TAG00381a
void ReadWallBitmaps(i16 P1); //TAG00386a
void MarkViewportUpdated(i16 P1);//TAG0042da (void)
void FloorAndCeilingOnly(void);//TAG004308
void MakeMirror(ui8 *src, ui8 *dst, i32 width, i32 height);//TAG00434a
void BltShapeToViewport(ui8 *P1, RectPos *P2);//TAG00437e
void BltRectangleToViewport(ui8 *src, RectPos *P2);//TAG0043c4
void BltTempToViewport(RectPos *);//TAG00440a
void MirrorShapeBltToViewport(ui8 *P1, RectPos *P2);//TAG004450
void GraphicToViewport(i32 P1, RectPos *rect);//TAG0044b0
void MirrorGraphicToViewport(i32 graphic, RectPos *rect);//TAG0044fe
void TAG00456c(void);
i16  DrawWallDecoration(i32 graphicOrdinal, 
                        i32 relativePosition,
                        RN text,
                        i32 championPortraitOrdinal,
                        bool removeJoint=true); //TAG0045ae
void DrawFloorDecorations(i32 graphicOrdinal, i32 relativeCellNumber);//TAG004a1a
void TAG004b26(i32, i32);
void DrawDoorSwitch(i32 P1, i32 P2);//TAG004c5e
//   TAG004d9e
void DrawDoor(i32 P1, i32 P2, i16 *P3, i32 P4, i32 P5, RectPos *P6);
void CheckCeilingPit(i32 relativeCellNumber,
                     i32 x,
                     i32 y);
void DrawTeleporter(RectPos *P1, RectPos *rectpos);//TAG004f82
pnt  TAG005088(i16, i16, i16 *, i16 *);
//   TAG0051a4
void DrawRoomObjects(const RN firstObject,
                     const i32 facing,
                     const i32 mapX,
                     const i32 mapY,
                     i32 relativeCellNum,
                     i16 P6);
void DrawCellF3L1 (i32 facing, i32 mapX, i32 mapY);//TAG006b20
void DrawCellF3R1 (i32 facing, i32 mapX, i32 mapY);//TAG006ca2
void DrawCellF3   (i32 facing, i32 mapX, i32 mapY);//TAG006e66
void DrawCellF2L1 (i32 facing, i32 mapX, i32 mapY);//TAG0070d8
void DrawCellF2R1 (i32 facing, i32 mapX, i32 mapY);//TAG007290
void DrawCellF2   (i32 facing, i32 mapX, i32 mapY);//TAG0074f0
void DrawCellF1L1 (i32 facing, i32 mapX, i32 mapY);//TAG0076d0
void DrawCellF1R1 (i32 facing, i32 mapX, i32 mapY);//TAG00788c
void DrawCellF1   (i32 facing, i32 mapX, i32 mapY);//TAG007a4a
void DrawCellF0L1 (i32 facing, i32 mapX, i32 mapY);//TAG007d00
void DrawCellF0R1 (i32 facing, i32 mapX, i32 mapY);//TAG007de8
void DrawVellF0   (i32 facing, i32 mapX, i32 mapY);//TAG007ed2
void DrawViewport (i32 facing, i32 mapX, i32 mapY);//TAG00803a
//   TAG008498
void ShrinkBLT(ui8 *src,
               ui8 *dst,
               i32 srcWidth,
               i32 srcHeight,
               i32 dstWidth,
               i32 dstHeight,
               ui8 *pColorMap);
void Mirror(ui8 *bitmap, i32 width/*bytes*/, i32 height); //TAG008642
void TAG008840(ui8 *, i16, i16);
void TAG0088b2(ui8 *src,
               ui8 *dst,
               RectPos *dstPos, // Position in dest image
               i32 SrcOffsetX,  // Source Starting x Pixel Offset
               i32 SrcOffsetY,  // Source Starting y Pixel Offset
               i32 SrcByteWidth,   // #bytes per line in source bitmap
               i32 DestByteWidth,  // #bytes per line in destination bitmap
               i32 transparentColor);
void TAG008c98(ui8 *,
               ui8 *,
               ui8 *,
               ui8 *,
               RectPos *,
               i16,
               i16,
               i16,
               i16,
               i16,
               i16);
void Flood(ui8 *dst,   //TAG008e1c
           i16 color,
           i16 num); // number of 16-bit groups
//   TAG008e60
void FillRectangle(ui8 *, RectPos *, i32 color, i32 destWidth);
void ShadeRectangleInScreen(RectPos *, i16);//TAG009058
void TAG00907e(void (*f1)(i32,i32,i32),
               pnt (*f2)(i32),
               i16 P3, pnt P4, i16 *p5, i16 **P6, i16 P7);
bool MonsterAllowedOnLevel(RN monster, \
                           i32 level,
                           MONSTERTYPE mt=mon_undefined);//TAG009154
i32  GetObjectWeight(RN obj);//TAG0091e6
//OBJ_DESC_INDEX  GetObjectDescIndex(RN DBRecordName);//9308
i32  GetGraphicClass(RN obj); //TAG0093d4(RN obj);
i32  TAG009470(CLOTHINGDESC *, bool scale);
i16  MonsterPositionByte(DB4 *pDB4, i16 level, bool noItem16);//TAG0094da
//  i16  GetMonsterDescWord2(RN monster);//TAG0094ae
void SetMonsterPositionByte(DB4 *pDB4, i16, i16 level, bool noItem16); //TAG009512
i16  MonsterFacingByte(DB4 *pDB4, i16 level, bool noItem16);//TAG009550
//   TAG009598
void SetMonsterFacingByte(DB4 *pDB4, i32, i32 level, bool noItem16);
bool IsAlcove(i32 wallDecorationIndex); //TAG0095e8
//   TAG009622
void RelativeXY(i32 facing,  //   0=north, 1=east, etc
                i32 stepsForward, //(0, 1, -1)
                i32 stepsRight,   //(0, 1, -1)
                i32 *mapX,
                i32 *mapY);
CELLFLAG GetCellFlags(i32 mapX, i32 mapY);//TAG009672
CELLFLAG *GetCellFlagsAddress(i32 level, i32 x, i32 y);
CELLFLAG *GetCellFlagsAddress(const LOCATIONREL& locr);
CELLFLAG GetCellFlags(const LOCATIONREL& locr);
i16  GetCellflagRelative(i32 facing, //TAG0097a0
                         i32 stepsForward,
                         i32 stepsRight,
                         i32 x,
                         i32 y);
i16  GetCelltypeRelative(i32 facing, //TAG0097d6
                         i32 stepsForward,
                         i32 stepsRight,
                         i32 x,
                         i32 y);
//   TAG009800
i16  IncrementLevel(i32 level,
                    i32 delta,  // Increment to level
                    i32 *mapX,  // Adjusted for level offsets
                    i32 *mapY); // Adjusted for level offsets
i16  ExitStairsDirection(i32 x, i32 y, i32 level);//TAG009904

DBCOMMON *GetCommonAddress(RN object);//TAG009984
DB0     *GetRecordAddressDB0 (RN object);
DB1     *GetRecordAddressDB1 (RN object);
DB2     *GetRecordAddressDB2 (RN object);
DB3     *GetRecordAddressDB3 (RN object);
DB4     *GetRecordAddressDB4 (RN object);
DB5     *GetRecordAddressDB5 (RN object);
DB6     *GetRecordAddressDB6 (RN object);
DB7     *GetRecordAddressDB7 (RN object);
DB8     *GetRecordAddressDB8 (RN object);
DB9     *GetRecordAddressDB9 (RN object);
DB10    *GetRecordAddressDB10(RN object);
DB14    *GetRecordAddressDB14(RN object);
DB15    *GetRecordAddressDB15(RN object);
DB0     *GetRecordAddressDB0 (i32 i);
DB2     *GetRecordAddressDB2 (i32 i);
DB3     *GetRecordAddressDB3 (i32 i);
DB4     *GetRecordAddressDB4 (i32 i);
DB10    *GetRecordAddressDB10(i32 i);

//   TAG0099b4
DBCOMMON *FirstObjectRecordAddress(i32 mapX, i32 mapY);
DB0  *FirstObjectRecordAddressDB0(i32 mapX, i32 mapY);
DB0  *FirstObjectRecordAddressDB0(i32 level, i32 x, i32 y);
DB0  *FirstObjectRecordAddressDB0(const LOCATIONREL& locr);
DB1  *FirstObjectRecordAddressDB1(i32 mapX, i32 mapY);
DB1  *FirstObjectRecordAddressDB1(const LOCATIONREL& locr);
DB3  *FirstObjectRecordAddressDB3(i32 mapX, i32 mapY);
WEAPONDESC *TAG0099d2(RN obj);
RN   GetDBRecordLink(RN recName); //TAG009a02
i16  GetObjectListIndex(i32 mapX, i32 mapY);//TAG009a1e
RN   FindFirstObject(i32 mapX, i32 mapY);//TAG009a94
RN   FindFirstObject(i32 level, i32 x, i32 y);
RN   FindFirstObject(const LOCATIONREL& locr);
RN   FindFirstTeleporter(i32 level, i32 x, i32 y);
RN   FindFirstDrawableObject(i32 mapX, i32 mapY);//TAG009ac8
RN   FindFirstMonsterGenerator(const LOCATIONREL& locr);
SRCHPKT *SearchForObject(RN objID);
void AddObjectToRoom(RN DBRecordName,
                   RN  AddAfter,
                   i32 mapX,
                   i32 mapY,
                   MMRECORD *pmmr);//TAG009b08
//   TAG009c4e
void RemoveObjectFromRoom(RN P1, i32 mapX, i32 mapY, MMRECORD *pmmr);
void RemoveObjectFromRoom(RN P1, const LOCATIONREL& locr);
RN MakeMoreRoomInDB(DBTYPE dbType);//TAG009d9e (RN)
//RN   FindEmptyDBEntry(ui16);//TAG00a0b0
// replaced by FindEmptyDBnEntry(bool);
RN FindEmptyDB2Entry(bool important); //(RN)
RN FindEmptyDB3Entry(bool important); //(RN)
RN FindEmptyDB4Entry(bool important); //(RN)
RN FindEmptyDB5Entry(bool important); //(RN)
RN FindEmptyDB6Entry(bool important); //(RN)
RN FindEmptyDB8Entry(bool important); //(RN)
RN FindEmptyDB10Entry(bool important);//(RN)
RN FindEmptyDB14Entry(bool important);//(RN)
RN FindEmptyDB15Entry(bool important);//(RN)
void DeleteDBEntry(DBCOMMON *pDB);
RN   ConstructPredefinedMissile(ui16 missileType); //TAG00a16e
i32 DecodeText(char *rslt,DB2 *pText,i16 typeOfText, i32 max); //TAG00a27c
i16  Hash(i16 P1, i16 P2, i16 P3);//TAG00a41c
i16  RandomWallDecoration(i16 P1,i16 P2,i32 P3,i32 P4,i16 probability);//TAG00a44c
void TAG00a4a2(SUMMARIZEROOMDATA *P1,
               i16 P2,
               i16 P3,
               i16 P4,
               i32 facing,
               i32 mapX,
               i32 mapY,
               i32 P8);
void SummarizeRoomInfo(SUMMARIZEROOMDATA *rslt, //TAG00a59a
                       i32 facing);
void LoadLevel(i32 newLevel);//TAG00a9d4
void SetPartyLevel(i32 level);//TAG00aa9c
RN   FindFirstMonster(i32 x, i32 y);//TAG00ab6e
RN   FindFirstMonster(i32 level, i32 x, i32 y);
RN   FindFirstMissile(i32 x, i32 y);//TAG00ab6e
RN   FindNextMonster(RN obj);
RN   FindNextMissile(RN obj);
RN   FindFirstDoor(i32 x, i32 y);
RN   FindFirstDoor(i32 level, i32 x, i32 y);
RN   FindFirstText(const LOCATIONREL& locr);
i16  OrdinalOfMonsterAtPosition(DB4 *pDB4, i32 pos);//TAG00abae
i16  DetermineAttackOrdinal( //TAG00ac9e
               i32 attackX,
               i32 attackY,
               i32 partyX,
               i32 partyY,
               i32 charPosition);
//   TAG00ad18
i16  SetMonsterPositionBits(i16 positionWord,
                            i32 monsterNum,
                            i32 Pos);
i16  SetMonsterFacingBits(i16 facingWord,
                          i32 monsterNum,
                          i32 facing);
i32  NextMonsterUpdateTime(ITEM16 *, i16 monsterIndex, bool attacking);
void StartMonsterMovementTimers(i32 mapX, i32 mapY);//Tag00af1a
void DeleteMonsterMovementTimers(i32 mapX, i32 mapY);//TAG00afbc
//   TAG00b02c
void ClearAttacking_DeleteMovementTimers(ITEM16 *, i32, i32);
void AttachItem16ToMonster(RN P1, i32 mapX, i32 mapY);//TAG00b062
void DetachItem16(i32 item16Index);// TAG00b152
//   TAG00b1f0
RN   CreateMonster(i32, i32, i32 numMonM1, i32, i32, i32, bool, bool);//(RN)
void CreateAndDropObject(i32, i32, i32, i32, i32);//TAG00b364(void)
void TAG00b522(RN, i32, i32);//(void)
void DropMonsterRemains(i32, 
                        i32, 
                        RN, 
                        i16 bodyPartSoundDelay, 
                        i32 monsterAttachedToLevel, 
                        bool dropResidue);//TAG00b580(void)
void DeleteMonster(i32, i32, MMRECORD *pmmr);//TAG00b6ac (void)
//   TAG00b73a
MONSTER_DAMAGE_RESULT DamageMonster(
               DB4 *, //(i16)
               i32 grpIndex,
               i32 mapX,
               i32 mapY,
               i32 P5,
               i16,
               bool noItem16,
               MMRECORD *pmmr);
// TAG00baec
MONSTER_DAMAGE_RESULT DamageMonsterGroup(DB4 *, i32, i32, i32, i16, bool, MMRECORD *pmmr); //(i16)
void KillMonster(RN RNmon, ui32 indexInGroup);
void StealFromCharacter(DB4 *, i32);//TAG00bc12//(void)
i16  DeterminePoisonDamage(MONSTERTYPE mt, i16);
void TAG00bd40(void);
void ProcessMonstersOnLevel(void);//TAG00bd70
void InitializeItem16(void);//TAG00be06
bool BlockedTypeA(i32 mapX, i32 mapY);//TAG00be5a
bool StoneOrClosedFalseWall(i32 mapX, i32 mapY);//TAG00bf00
//   TAG00bf50
i16  TestDirectPathOpen(i32 mapX,
                        i32 mapY,
                        i32 partyX,
                        i32 partyY,
                        bool (*func)(i32,i32));
i16  Blocked(DB4 *, i32, i32, i32);//TAG00c202
//   TAG00c36c
i16  GetBestMonsterFacingP1(MONSTERDESC *, i32 x, i32 y);
//   TAG00c442
bool PossibleMove(const MONSTERDESC *, i32 x, i32 y, i32, i16);
//   TAG00c70c
i16  FindPossibleMove(const MONSTERDESC *, i32 x, i32 y, i16);
//   TAG00c756
i16  PossibleDoubleMove(const MONSTERDESC *, i32 x, i32 y, i32 direction);
//   TAG00c7bc
void TurnMonster(ITEM16 *,
                 i32 preferredDirection,
                 i32 monsterIndex,
                 bool);//TAG00c7bc
void TurnMonsterGroup(ITEM16 *, ////TAG00c86c
                      i32,   //preferred direction
                      i32,   //number of monsters
                      i32);  //Must-turn-as-group flag
bool MonsterAttacks(RN monster, DB4 *, i32 x, i32 y, i16);//TAG00c8c0//(i16)
void SetMonsterTimerB(TIMER *, const ui32 alternateTime); // TAG00cb68
//   TAG00cbc6
void ProcessTimers29to41(i32 mapX, i32 mapY, TIMERTYPE timeType, i16 P4);//(void)
//i16  TAG00dcf2(pnt P1);
void TAG00dd78( void *f(void));
i16 LaunchMissile(
               RN   object, //TAG00dd90(i16)
               i32  mapX,
               i32  mapY,
               i32  pos,
               i32  dir,
               i32  range,
               i32  damage,
               i32  decayRate,
               bool missileDelay);
void ThrowMissile(i32 missileType,
                  const LOCATIONREL& launchLocation,
                  i32 direction,
                  const LOCATIONREL& objectLocation,
                  i32 range,
                  i32 damage,
                  i32 deltaRangeDamage);
void CreateCloud(RN, i16, i32, i32, i32, i32, i16, MMRECORD *pmmrF);//(void)TAG00dea8
void TAG00e156(RN);
void ProcessObjectFromMissile(RN, RN *dest, i32, i32);//TAG00e17a(void)
i16  DetermineMagicDamage(DB14 *rec, RN object);//TAG00e218
//   TAG00e35a
i16  ProcessMissileEncounter(
                             i32 cellType, //-2=party; -1=monster? (i16)
                             i32 objectX,
                             i32 objectY,
                             i32 missileX,
                             i32 missileY,
                             i32 objectPos,
                             RN missile,
                             MMRECORD *pmmr);
i16  CheckMonsterVersusMissiles(i32 x, i32 y, ui16, MMRECORD *pmmr);//(i16)TAG00e8ce
void MissileTimer(TIMER *P1);//TAG00e962(void)
void ProcessTT_25(TIMER *P1);//TAG00ecca(void)
bool IsCellFluxcage(i32 mapX, i32 mapY); //TAG00f058
RN   IsLordChaosHere(i32 mapX, i32 mapY);//TAG00f0e8
bool OpenTeleporterPitOrDoor(i32 mapX, i32 mapY); //TAG00f132
//void FluxCage(i32, i32); //TAG00f182
RN   CreateFluxcage(i32 x, i32 y);
#define Fusion(n,a,b) CALL2(n,Fusion,a,b)
RESTARTABLE _Fusion(const i32 attackX, const i32 attackY);//TAG00f340//(void)
//   TAG00f4c8
i16  DistanceBetweenPoints(i32 mapX, i32 mapY, i32 partyX, i32 partyY);
//   TAG00f50a
bool IsItInThisDirection(DIRECTION, i32 X1, i32 Y1, i32 X2, i32 Y2);
DIRECTION GetPrimaryAndSecondaryDirection(
                            i32 oldX,
                            i32 oldY,
                            i32 newX,
                            i32 newY);//TAG00f582
void DetermineAttackOrder(ui8 attackedAbsPos[4],
                          i32 attackerX,
                          i32 attackerY,
                          i32 victimX,
                          i32 victimY,
                          i32 attackerPos);
i16 MonsterDamagesCharacter(DB4 *pDB4, i32 chIdx, bool supressPoison);//TAG00f6ee//(i16)
//i32 DeterminePhysicalAttackDamage(
//               CHARDESC *pChar, //(i16)
//               i32 chIdx,
//               DB4 *monsterRec,
//               i16,
//               i32 mapX,
//               i32 mapY,
//               i16,
//               i16,
//               i32 skillNumber,
//               bool vorpalOrDisrupt,
//               char *traceID=NULL);
i16  HitDoor(i32 mapX,   //TAG00fc20
             i32 mapY,
             i32 force,
             bool explosion,
             i32);
//void InitializeTimers(void);//TAG00fd1c
//bool TestTimerLess(TIMER *p1, TIMER *p2);//TAG00fd9e
i16  FindTimerPosition(i32 P1);//TAG00fdce
//void AdjustTimerPosition(i32 index); //TAG00fdf0(i32);
//void DeleteTimer(i32);//TAG00ff4c
//i16  SetTimer(TIMER *P1); // TAG00ffbe
//void GetNextTimerEntry(TIMER *P1, ui32 *index);//TAG01022c
//i16  CheckForTimerEvent(void);//TAG010260//returns 0 or 1
void ProcessTT_1(TIMER *P1);//TAG0102a4(void)
void ProcessTT_FALSEWALL(TIMER *P1, ui32 index);//TAG0104bc
void ProcessTT_BASH_DOOR(TIMER *P1);//TAG010576
void ProcessTT_DOOR(TIMER *P1);//TAG0105ba
void ProcessTT_OPENROOM(TIMER *P1, ui32 index);//TAG010642//(void)
void ProcessTT_ReactivateGenerator(TIMER *P1);//TAG0108a4
void TriggerMissileLauncher(DB3 *, const TIMER *);//TAG010910(void)
bool ProcessTT_STONEROOM(TIMER *P1,ui32 index);//TAG010b9a(void)
bool ProcessDSATimer5(RN objDSA,
                      const TIMER *pTimer,
                      LOCATIONREL locrSlave);
bool ProcessDSATimer6(RN objDSA,
                      const TIMER *pTimer,
                      LOCATIONREL locrSlave,
                      bool filter,
                      DSAVARS *dsaVars);
bool ProcessDSATimer7(RN objDSA,
                      const TIMER *pTimer,
                      LOCATIONREL locrSlave);
void ProcessDSAFilter(RN objDSA,
                      const TIMER *pTimer,
                      LOCATIONREL locrSlave,
                      bool *pFilterActive,
                      DSAVARS *dsaVars);
void WiggleEverything(i32 mapX, i32 mapY);//TAG010e98(void)
void ProcessTT_TELEPORTER(TIMER *P1);//TAG0110a6(void)
void ProcessTimer60and61(TIMER *P1);//TAG011192(void)
void ProcessTT_PITROOM(TIMER *P1);//TAG01111c//(void)
void EnableCharacterAction(i32 chIdx);//TAG011292
void TAG011366(i16 P1);
void ProcessTT_ViAltar(TIMER *P1);//TAG0113c4(void)
void SetWatchdogTimer(void); //TAG0114d0
void ProcessLightLevelTimer(TIMER *P1);
i16 TAG011594(i16 chIdx,  //(i16)
               CHARDESC *,
               i16 srcPossession,
               i16 dstPossession);
void TAG0115ee(i32 P1, i32 P2);//(void)
void MarkAllPortraitsChanged(void);//TAG011664
bool ProcessTimers(void);//TAG011692 // Parameter seems to be ignored;(void)
void TAG011718(i16, i16, i32);
i16  TeleportMonster(const DB1 *,RN object,i16 attachedLevel, bool noItem16);//TAG011a16
RN   TeleportMissile(const DB14 *pDB14,RN object);//TAG011b8e
bool Levitating(RN); //TAG011bf6(RN); //Any kind of item
void CreateMovementTimer(RN, i32, i32, i32, i16);//TAG011c3a
i16  CheckMoveFromMissile(i32, i32, i32, i32, RN, MMRECORD *pmmr);//(i16) //TAG011c9a
//   TAG0011f52
i16 MoveObject(const RN        object, //(i16)
               const i32       oldX,
               const i32       oldY,
                     i32       newX,
                     i32       newY,
               const DB1      *pPretendTeleporter,
                     MMRECORD *mmr);
//   TAG012a74
void CreateTimer(TIMERTYPE tt,  // timer function
                 i32 b6,    // Byte 6 - (byte 5 set to zero)
                 i32 b7,    // Byte 7
                 i32 b8,    // Byte 8
                 i32 b9,    // Byte 9
                 i32 time); // For currently loaded level
void SkillIncreaser(i32 skill, i32 adj, i32 singleMember);//TAG012ac0(void)
//TAG012b3c
void PerformLocalAction(i16 type, i32 mapX, i32 mapY, i32 pos);
void RotateActuatorList(void);//TAG012b80
void QueueSwitchAction(DB3 *, i32,i32,i32,i32);//(void)
//   TAG012d80
RN   FindObjectOfTypeAtPosition(i32, i32, ui16 pos, RN);
bool DoesPartyHave(OBJ_NAME_INDEX);//TAG012de0
i16  TouchWall(i32 mapX, i32 mapY, i32 direction);//TAG012eac(i16)
//   TAG013380
void PlaceOrRemoveObjectInRoom(
                       i32 mapX,
                       i32 mapY,
                       RN object,
                       bool currentRoom,
                       bool place,
                       MMRECORD *pmmr);
i16  TAG01385a(pnt);
void TAG0138ec(void);//(void)
//i32  DecodeCharacterValue(pnt, i16, bool allowTruncation = false);//TAG0139a2
void AddCharacter(i16 charID);//TAG0139be(void)
#define ReIncarnate(n,a) CALL1(n,ReIncarnate,a)
RESTARTABLE _ReIncarnate(CHARDESC *pChar);//TAG013ea4 (void)
//TAG014368
#define ResurrectReincarnateCancel(n,a) CALL1(n,ResurrectReincarnateCancel,a) 
RESTARTABLE _ResurrectReincarnateCancel(const i32);//(void) //TAG014368
void SetPartyFacing(i32 newDirection);//TAG01470c(void)
void BringCharacterToLife(ui16 chIdx);//TAG014676//(void)
i32  CharacterAtPosition(i32 pos);//TAG01474e
i16  GetCharacterToDamage(i32, i32, i32);//TAG01479c
void DrawLifeBars(i32);//TAG014832
const char*  TAG014af6(i32, i32, i32);
void PrintLifeForce(i32 line, i32 current, i32 max);//TAG014b32
void PrintLifeForces(CHARDESC *);//TAG014b9e
void DisplayBackpackItem(i32 chIdx, i32 itemPos);//TAG014bf4
//   TAG014de0
void DrawCharacterState(i32 P1); // box at top, direction, //(void)
void DrawAllCharacterState(void);//TAG015396 (void)
i16  TAG0153c2(i32 chIdx, i32, i32);
//   TAG015492
bool DrawModifiedObjectAtLocation(i32 ScreenLocation, RN object);
void DrawEightHands(void);//TAG015508//(void)
void ObjectToCursor(RN, i16);//TAG0156c4
RN RemoveObjectFromHand(void);//TAG01576a//(RN)
//   TAG0157e8
void AdjustStatsForItemCarried( CHARDESC *,
                                i32,
                                i32,
                                RN);
RN RemoveCharacterPossession(i32 chIdx, i32 possessionIndex);//TAG015a66(RN)
//   TAG015c4c
void AddCharacterPossession(i32 chIdx, RN object, i32 place);//(void)
void HandleClothingClick(i32 button);//TAG015e0c//(void)
i32  DetermineMastery(i32 chIdx, i32 skill, const char *traceID=NULL);//TAG015f1e
void AdjustSkills(i32, i32, i32, i32 why);//TAG01605a//(void)
i16  TAG016382(RN object);
i16  StaminaAdjustedAttribute(CHARDESC *, i16);//TAG0163c8
i16  TAG016426(CHARDESC *pCharacter, i16, i16);
bool IsCharacterLucky(CHARDESC *pCharacter, i32, const char *traceID=NULL);//TAG016476
i32  MaxLoad(CHARDESC *pCharacter);//TAG016508//10ths of KG
i16  TimeToMove(CHARDESC *pCharacter);// TAG01657e Speed of character???
i32  Quickness(CHARDESC *pCharacter, const char *traceID=NULL);//TAG016610
//   TAG0166b2
i32  DetermineThrowingDistance(i32 chIdx, i32 hand);
i16  TAG01680a(i32 chIdx, i32 possessionIndex);
void WakeUp(void);//TAG0169a8//(void)
i16  SearchFootprints(i32 mapX, i32 mapY); //TAG0169ec
void DeleteFootprintEntry(i16);//TAG016a76
void AddFootprintEntry(i32, i32, ui16);//TAG016b10
void DropAllPossessions(i32 chIdx);//TAG016be8//(void)
void KillCharacter(i32 chIdx);//TAG016c5a (void)
void DisplayCharacterDamage(void);//TAG016e54(void)
i32  DamageCharacter(i32 chIdx, i32 overdraw, i16, i16);//TAG017068
void PoisonCharacter(i32 P1, i32 P2);//TAG017274(void)
void CurePoison(i32 chIdx);//TAG017344
i16  DamageAllCharacters(i32,i16,i16);//TAG0173ae
void AdjustStamina(i32 chIdx, i32 decrement);//TAG01742a
i16  LaunchObject(CHARDESC *, 
                  RN  object, 
                  i32 range, 
                  i32 damage, 
                  i32 decayRate);//TAG017498(i16)
i16 CharacterThrowsSpell(i32, 
                         RN object, 
                         i32, 
                         i32 neededMana);//(i16) TAG0174ec
//   TAG017570
i16 ThrowByCharacter(i32 chIdx, i16, i16 LeftOrRight, int disableTime=-1,
                     i32 range=-1, i32 damage=-1, i32 decayRate=-1,
                     bool record=false, i32 x=0, i32 y=0, i32 f=0);//(i16)
i16 ThrowObjectInHand(i16 LeftOrRight);//TAG01772e(i16)
void DisableCharacterAction(i32 chIdx, i32 timeDelay);//TAG017756//(void)
void TenSecondUpdate(void);//TAG017818
void DrawSmallIcon(OBJ_NAME_INDEX objNI, i16 x, i16 y);//TAG017c7a
void ShowChestContents(RN object, DB9 *pDB9, i16);//TAG017ce4
void RepackChest(void);//TAG017da4
void PrintItemDesc(const char* txt, i32 color = COLOR_13);//TAG017e2a
i32  EncodeDescriptivePhrases(i32, i32, const char *, i32 *, char *, const char*, const char*);//TAG017ece
void SelectPaletteForLightLevel(void);//TAG017fa8
void NinetySecondUpdate(void);//TAG018124//(void)
void TAG018238(i16);
void DisplayScrollText_OneLine(i16, char *);//TAG01826c
void DisplayScroll(DB7 *pDB7);//TAG0182f0
void DescribeObject(RN, i16); //TAG0183ea
void CursorFilter(CURSORFILTER_PACKET *cfp);
void TAG00187e(void);
void DrawHorzBar(i32 x, i32 y, i32 width, i32 color);//TAG018836
void DrawFoodWaterBar(i16, i16, i16 color);//TAG018878
void DisplayFoodWater(void);//TAG0188e4
void TAG0189a8(void);
void TAG0189d4(void);
//   TAG018a58
void AdjustCurrentAttribute(CHARDESC *, i32 attr, i32 adj);
#define FeedCharacter(n) CALL0(n,FeedCharacter)
RESTARTABLE _FeedCharacter(void);//TAG018afc (void)
void QuitPressingMouth(void);//TAG019018(void);
void TAG019036(void);
#define ClickOnEye(n) CALL0(n,ClickOnEye);//TAG0191e8(n) CALL0(n,TAG0191e8)
RESTARTABLE _ClickOnEye(void);//(void)
void QuitPressingEye(void);//TAG019264(void);//(void)
void CharacterPortraitToStatusBox(i32);//TAG0192a4
void ShowHideInventory(i32 P1);//(void) //TAG0192f4
i16  TAG0194c0(pnt P1);
void DiscardAllInput(void);//TAG01953a(void);
  // TAG019570
i16  SearchButtonList(btn *P1, i32 x, i32 y, i32 buttons); // mouse
void OnMouseClick(i32 x, i32 y, i32 button);//TAG0195da
void OnMouseUnClick(void); //When left or right button is released.
void CauseFakeMouseClick(void);//TAG0196b6
void TAG0196da(i32 P1);
#define FlashButton(n,a,b,c,d) CALL4(n,FlashButton,a,b,c,d)
RESTARTABLE _FlashButton(const i32 x1,
                         const i32 x2,
                         const i32 y1,
                         const i32 y2); //TAG01979c(void)
void WaitForButtonFlash(void);//TAG0197d2 (void)
void TraverseStairway(i32 UpOrDown,bool setFacing);//TAG0197f6 (void)
#define TurnParty(n,a) CALL1(n,TurnParty,a)
RESTARTABLE _TurnParty(const i32 direction); //TAG01986c//1=left 2=right (void)
MOVEBUTN *MoveParty(i32 button); // CALL1(n,MoveParty,a)
void TAG019c34(i32 P1, i32 P2, i32 P3);//(void)
void SetHandOwner(i32);//TAG019cb2//(void)
#define MagicSelection(n,a) CALL1(n,MagicSelection,a)
RESTARTABLE _MagicSelection(const i32 button);//TAG019d90 (void)
#define HandleMagicClick(n,a,b) CALL2(n,HandleMagicClick,a,b)
RESTARTABLE _HandleMagicClick(const i32 clickX, const i32 clickY);//TAG019e4a//(void)
#define TAG019fac(n,a,b) CALL2(n,TAG019fac,a,b);
RESTARTABLE _TAG019fac(const i32 P1, const i32 P2);//(void)
void  TouchWallF1(void);//TAG01c0c2(void)
void  TAG01a148(i16);//(void)
void DropObject(i16 position);//TAG01a228//(void)
i32  TestThrow(i16 x, i16 y); //TAG01a34c(i16)
i16  TestInRectangle(ui8 *rectangle, i32 clickX, i32 clickY);//TAG01a3d6
void HandleClickInViewport(i32 P1, i32 P2);//TAG01a436(void)
#define TAG01a6ea(n,a,b) CALL2(n,TAG01a6ea,a,b)
RESTARTABLE _TAG01a6ea(const i32 P1, const i32 P2);//(void)
void DisplaySleepScreen(void);//TAG01a77c
#define HandleMouseEvents(n,a) CALL1(n,HandleMouseEvents,a)
RESTARTABLE _HandleMouseEvents(const i32 delta); //TAG01a7b2(bool)
//   TAG01ab88
void PrintWithSubstitution(const char *txt, ui32 color, bool translate);
bool ItemHasChargesLeft(void); //TAG01ac26
void GetLegalAttackTypes(ui8 *); //TAG01acb0
const char*  GetAttackTypeName(i32);//TAG01ad6c
#define FlashAttackDamage(n,a) CALL1(n,FlashAttackDamage,a)
RESTARTABLE _FlashAttackDamage(const i32);//TAG01adaa (void)
void DrawAttackButton(i32 chIdx);//TAG01af32
void DrawLegalAttackTypes(void); //TAG01b098
void TAG01b1c0(void);//(void)
void PrepareAttack(i16);//TAG01b1f8 Display weapon options?(void)
#define ExecuteAttack(n,a) CALL1(n,ExecuteAttack,a);
RESTARTABLE _ExecuteAttack(const i32);//TAG01b37c(i16)
#define TAG01b29a(n) CALL0(n,TAG01b29a)
RESTARTABLE _TAG01b29a(void);//(void)
void TAG01b408(i16);
void DrawSpellControls(i16); //TAG01b50e
void SelectMagicCaster(i16 chIdx);//TAG01b726
void DrawMovementButtons(void);//TAG01b880
void ReadSpellGraphic(void); //TAG01b8ae(void);
void DrawSpellMenuCharacters(i16 button);//TAG01b8ca
void DisplayMagicSelection(CHARDESC *chIndx);//TAG01b922
void TAG01b990(i16);//(void)
void BackspaceMagicSelection(void);//TAG01ba5a
i32  WarCryEtc(i32 chIdx,   //TAG01bac2//(i16)
               ATTACKTYPE attackType,
               i32 attackX,
               i32 attackY,
               const char *tracePrefix);
//i16 AttackWithPhysicalForce(i32 chIdx,  //TAG01bc2a(i16)
//              CHARDESC *pChar,
//              ATTACKTYPE attackType,
//              i32 attackX,
//              i32 attackY,
//              i32,
//              char *traceID=NULL);
//  TAG01bd5e
i16 MagicShield(CHARDESC *pChar, i32 spellShield, i32 strength, i32 mustHaveMana);
//   TAG01be38
void SetBrightnessTimer(i32 deltaBrightness, i32 timeDelay);
void DecrementChargesRemaining(CHARDESC *);//TAG01be82(void)
void SetCharToPartyFacing(CHARDESC *);//TAG01bf70
#define Attack(n,a,b) CALL2(n,Attack,a,b)
RESTARTABLE _Attack(const i32 chIdx, const ATTACKTYPE attackType);//TAG01bf9a(i16)
i16 CastMagic(const i32); //TAG01c826//(i16)
SPELL *Incantation2Spell(pnt incantation);//TAG01c88e
void SpellErrorMsg(CHARDESC *, i16, i16);//TAG01c90c
DB8 *GetFlaskInHand(CHARDESC *, RN *);//TAG01c9b2
i16 CastSpell(i16 chIdx, SPELL_PARAMETERS& spellParameters);
         //TAG01ca0c//(i16)
void DSACastSpell(bool filter);
void SubstDiskLetter(char *dest,const char *src, i16 flag);//TAG01cefa
i16  StreamInput(ui8 *dest, i32 num); // TAG01cf86
i16  WriteDataFile(i16 *, i32);//TAG01d004
//   TAG01d048
i16  Unscramble(ui8 *buf, i32 initialChecksum, i32 numword);
i16  GenChecksum(ui8 *buf, i16 initSum, i32 numword);//TAG01d076
//   TAG01d0a4
i16  UnscrambleStream(ui8 *buf,i32 size,i16 initialHash,i16 Checksum);
i16  WriteScrambled(ui8 *, i32, i16, i16 *);//TAG01d0ea
i16  FetchDataBytes(ui8 *dest, i16 *checksum, i32 numbyte); //TAG01d138
i16  WriteAndChecksum(ui8 *, i16 *, i32);//TAG01d182
#define WaitForMenuSelect(n,a,b,c,d) CALL4(n,WaitForMenuSelect,a,b,c,d)
RESTARTABLE _WaitForMenuSelect(const i32 P1,
                       const i32 P2,
                       const i32 P3,
                       const i32 P4); //(i16)
void CenteredText(ui8 *dest, const char *text, i16 x, i16 y);//TAG01d5aa
//   TAG01d5ee
i16  SplitLongLine(char *text, char *firstLine, char *secondLine);
#define DoMenu(n,a,b,c,d,e,f,g,h,i) CALL9(n,DoMenu,a,b,c,d,e,f,g,h,i)
RESTARTABLE _DoMenu(
            const char *title1,  //TAG01d678 (void)
            const char *title2,
            const char *opt1,
            const char *opt2,
            const char *opt3,
            const char *opt4,
            const i32 P7,
            const i32 ClearOldScreen,
            const i32 FadeOldScreen);
i16  ReadUnscrambleBlock(ui8 *P1);//TAG01db46
i16  ScrambleAndWrite(i16 *pwbuf);//TAG01dbb2
i16  Darken(i16 color);//TAG01dc4e
i16  InsertDisk(const i32 P1, const i32 P2);//(i16)
#define DisplayDiskMenu(n) CALL0(n,DisplayDiskMenu)
RESTARTABLE _DisplayDiskMenu(void);//(void)
i16  ReadDatabases(void);//TAG01e552
#define ReadEntireGame(n) CALL0(n,ReadEntireGame)
RESTARTABLE  _ReadEntireGame(void); //(i16) //TAG01eb18
//#define InsertDisk(n,a,b) CALL2(n,InsertDisk,a,b) //TAG01daa8
//RESTARTABLE _InsertDisk(const i32 P1, const i32 P2);//(i16)
#define FadeToPalette(n,a) CALL1(n,FadeToPalette,a);
RESTARTABLE _FadeToPalette(const PALETTE *palette); // TAG01f04e (void)
#define DisplayChaosStrikesBack(n) CALL0(n,DisplayChaosStrikesBack)
RESTARTABLE _DisplayChaosStrikesBack(void);//TAG01f140 (void)
#define OpenPrisonDoors(n) CALL0(n,OpenPrisonDoors)
RESTARTABLE _OpenPrisonDoors(void); //TAG01f47a (void)
#define TAG01f5ea(n) CALL0(n,TAG01f5ea)
RESTARTABLE _TAG01f5ea(void);//(void)
i32  AllocateExpandGraphic(i16 graphicNum, ui8 **address); //TAG01f6fe
#define TAG01f746(n) CALL0(n,TAG01f746)
RESTARTABLE _TAG01f746(void); //(void)
#define TAG01f928(n) CALL0(n,TAG01f928)
RESTARTABLE _TAG01f928(void); //(void)
void DisplayHeroName(i16, i16, i16, char *); //TAG01f990
#define ShowCredits(n,a) CALL1(n,ShowCredits,a)
RESTARTABLE _ShowCredits(const i32 P1);//(void)
//      TAG01fed6
#define ProcessTimersViewportAndSound(n) CALL0(n,ProcessTimersViewportAndSound)
RESTARTABLE _ProcessTimersViewportAndSound(void);
#define FusionSequence(n) CALL0(n,FusionSequence) //TAG01fefc
RESTARTABLE _FusionSequence(void); //(void)
void HangIfZero(i16 P1); // (0x020276)
void TAG020286(void);
void CountFloppyDrives(void); //TAG020466
#define SelectSaveGame(n,a,b,c) CALL3(n,SelectSaveGame,a,b,c)
RESTARTABLE _SelectSaveGame(const i32, const i32 checkExist, const i32 alwaysDate);//TAG0204bc(void)
void ExpandData(pnt src, pnt dst, i32 dstSize); //TAG0206f8
void TAG02076e(void);//(void)
void TAG0207cc(void);
void TAG020836(pnt P1);
i16  TAG020880(i16 P1, i16 P2, i16 P3);
void ReadTablesFromGraphicsFile(void);//TAG0208b4(void);
void TAG020fbc(void);
#define TAG021028(n) CALL0(n,TAG021028)
RESTARTABLE _TAG021028(void); //(void)
#define ShowPrisonDoor(n) CALL0(n,ShowPrisonDoor)
RESTARTABLE _GameSetup(i32 showPrisonDoor);//TAG0211a0 (void)
#define GameSetup(n,a) CALL1(n,GameSetup,a)
RESTARTABLE _ShowPrisonDoor(void);//TAG0211a0 (void)
void TAG02134a(pnt);
//   TAG0213ec //
void ExpandGraphic(i8 *src, // 1st 2 words are width and height.
                   ui8 *dst, // If NULL then physbase.
                   i16 dstX, // Ignored if dst != NULL
                   i16 dstY,//   or if graphic width==320
                   i32 maxSize = 999999);
i32  LocateNthGraphic(i32 n); //TAG021714
ui8 *allocateMemory(i32 size, i16 type); //TAG02175c
void FreeTempMemory(i32 size);//TAG0217d8
void TAG021800(i32 filesize);
//void RemoveFromFreeList(ITEMQ *P1);//TAG021822
//void FreeGraphicMemory(ITEMQ *P1);//TAG021888
void TAG021960(i16 *P1, i16 P2);
void ReadGraphic(i16 graphicNumber, ui8 *buffer, i32 maxSize=999999); // TAG021af2
//   TAG021c08
void InitializeCaches(ui8 *P1, i32 P2, ui8 *P3, i32 P4);
void TAG021cd8(void);
void openGraphicsFile(void); //TAG021d36
void OpenCSBgraphicsFile(void);
void closeGraphicsFile(void); //TAG021d76
void ReadGraphicsIndex(void); // TAG021d9a
void DeleteGraphic(i32 graphicNum);//TAG021f5e
//void CompactMemory(void);//TAG022132
void TAG0220fa(i32 P1);
ITEMQ *GetExpandedGraphicBuffer(i32 P1); //TAG0221f6
//#define LoadNeededGraphics(n,a,b) CALL2(n,LoadNeededGraphics,a,b)
//RESTARTABLE _LoadNeededGraphics(i16 *, const i32);//(void) //TAG0222ea
void LoadNeededGraphics(i16 *, const i32);//(void) //TAG0222ea
//void ClearGraphicList2(void);//TAG022720
//void TAG022778(i16 P1);
//ITEMQ *TAG02289a(i16 P1);
void TAG022a60(i16, ui8 *);
ui8 *GetBasicGraphicAddress(i32 graphicNum, i32 minimumWidth=0, i32 minimumHeight=0);//TAG022a92

//TAG022b86
void ReadAndExpandGraphic(
                  i32 graphicNum,
                  ui8 *dest,  // If NULL then physbase
                  i16 destX, // X and Y ignored if dest != NULL
                  i16 destY,
                  i32 maxSize = 999999);
bool  AllocateDerivedGraphic(i32 graphicNum); //TAG022c90
ui8  *GetDerivedGraphicAddress(i32); //TAG022d38(i32);
//void TAG022d5e(i16);
ui16 GetGraphicDecompressedSize(i32 P1); //TAG022d7a
i16  LZWGetNextCodeword(i16 P1, i32 *P2); //TAG022d90
void TAG022eec(i32 code, pnt *P2);
i32  LZWExpand(i16 handle, i32 graphicSize, ui8 *dest, ui8 *scratch, ui8 *stack); //TAG022f64


// Entries in Chaos.cpp
#define TAG00301c(n,a) CALL1(n,TAG00301c,a)
RESTARTABLE _TAG00301c(pnt);//{i16)
#define TAG000588(n,a,b,c,d) CALL4(n,TAG000588,a,b,c,d)
RESTARTABLE _TAG000588(pnt msg, i32, pnt, struct S12406 *);//(i16)
#define UtilityDialogBox(n,a,b,c,d) CALL4(n,UtilityDialogBox,a,b,c,d)
RESTARTABLE _UtilityDialogBox(const char *msg, i32, const char *, struct S12406 *);//(i16)
#define CheckChampionNames(n) CALL0(n,CheckChampionNames)
RESTARTABLE _CheckChampionNames(void);//TAG000e64//(i16)
#define TAG000fc4(n,a) CALL1(n,TAG000fc4,a)
RESTARTABLE _TAG000fc4(i32);//(void)
#define TAG000ede(n,a,b) CALL2(n,TAG000ede,a,b)
RESTARTABLE _TAG000ede(i32, char *);//(i16)
#define TAG00179c(n,a) CALL1(n,TAG00179c,a)
RESTARTABLE _TAG00179c(i32);//(i16)
#define ReadSavedGame(n,a) CALL1(n,ReadSavedGame,a)
RESTARTABLE _ReadSavedGame(i32);//TAG001868//(i16)
#define TAG00260e(n) CALL0(n,TAG00260e)
RESTARTABLE _TAG00260e(void);//(void)
#define TAG0026c4(n,a) CALL1(n,TAG0026c4,a)
RESTARTABLE _TAG0026c4(pnt);//(void)
#define TAG004518(n) CALL0(n,TAG004518)
RESTARTABLE _TAG004518(void);//(void)
#define InvertButton(n,a) CALL1(n,InvertButton,a)
RESTARTABLE _InvertButton(const wordRectPos *); //(void)
#define DrawCharacterDetails(n,a,b) CALL2(n,DrawCharacterDetails,a,b) //TAG003ab6
RESTARTABLE  _DrawCharacterDetails(const i32, const i32); //(void)
#define TAG003ab6(n,a,b) CALL2(n,TAG003ab6,a,b)
RESTARTABLE  _TAG003ab6(const i32, const i32); //(void)
#define TAG003f12(n) CALL0(n,TAG003f12)
RESTARTABLE _TAG003f12(void);//(void)
#define DrawCharacterEditorScreen(n) CALL0(n,DrawCharacterEditorScreen)
RESTARTABLE _DrawCharacterEditorScreen(void);//(void)
#define TAG004430(n,a) CALL1(n,TAG004430,a)
RESTARTABLE _TAG004430(i32);//(void)

//
// Entries in Hint.cpp
#define DisplayText(n,a,b) CALL2(n,DisplayText,a,b)
RESTARTABLE _DisplayText(const i16, const struct TEXT *);//TAG004a22_6 (void)
//#define TAG0051c2_xxx(n,a,b) CALL2(n,TAG0051c2_xxx,a,b)
//RESTARTABLE _TAG0051c2_xxx(const i16 P1, const i16 P2);
#define TAG005a1e_xxx(n,a,b,c) CALL3(n,TAG005a1e_xxx,a,b,c)
RESTARTABLE _TAG005a1e_xxx(const i16, const i16 P2, const i32 P3);
#define FadePalette(n,a,b,c,d,e) CALL5(n,FadePalette,a,b,c,d,e)
RESTARTABLE _FadePalette(PALETTE *P1,
                        const i16 *P2,
                        const i16 P3,
                        const i16 P4,
                        const i16 p5);
//#define TAG005ae8_xxx(n,a,b,c) CALL3(n,TAG005ae8_xxx,a,b,c)
//RESTARTABLE _TAG005ae8_xxx(i16 P1, i16 P2, pnt P3);
#define TAG0051c2_1(n,a) CALL1(n,TAG0051c2_1,a)
RESTARTABLE _TAG0051c2_1(const i16); //(void)
//#define TAG008c40_xxx(n,a,b,c,d,e) CALL5(n,TAG008c40_xxx,a,b,c,d,e)
//RESTARTABLE _TAG008c40_xxx(i16 P1, i16 P2, i32 P3, i32 P4, pnt P5);
//#define TAG008c40_1(n,a) CALL1(n,TAG008c40_1,a)
//RESTARTABLE _TAG008c40_1(i16);//(void)
//#define TAG008c40_2(n,a) CALL1(n,TAG008c40_2,a)
//RESTARTABLE _TAG008c40_2(i16);//(void)
#define TAG006c7e_xxx(n,a,b,c,d,e) CALL5(n,TAG006c7e_xxx,a,b,c,d,e)
RESTARTABLE _TAG006c7e_xxx(i16 P1, i16 P2, i32 P3, i32, pnt P5);//(i32)
#define TAG006c7e_2(n,a) CALL1(n,TAG006c7e_2,a)
RESTARTABLE _TAG006c7e_2(i16);//(void)
#define TAG006c7e_32(n,a,b) CALL2(n,TAG006c7e_32,a,b)
RESTARTABLE _TAG006c7e_32(i16, pnt);//(i32)
//#define TAG0076a0_8(n,a,b,c) CALL3(n,TAG0076a0_8,a,b,c)
//RESTARTABLE _TAG0076a0_8(i16, pnt, i32);//(i32)
#define TAG0076a0_12(n,a,b,c) CALL3(n,TAG0076a0_12,a,b,c)
RESTARTABLE _TAG0076a0_12(i16, pnt, TEXT *);//(i32)
//#define TAG0076a0_13(n,a) CALL1(n,TAG0076a0_13,a)
//RESTARTABLE _TAG0076a0_13(i16);//(i32)
//#define TAG0076a0_40(n,a,b,c,d) CALL4(n,TAG0076a0_40,a,b,c,d)
//RESTARTABLE _TAG0076a0_40(i32, i32, pnt, i32);//(i32)
//#define TAG002c5e(n,a,b,c) CALL3(n,TAG002c5e,a,b,c)
//RESTARTABLE _TAG002c5e(pnt, pnt, pnt);//(i32)
//#define TAG00686a(n,a) CALL1(n,TAG00686a,a)
//RESTARTABLE _TAG00686a(i16);//(i32)
#define TAG006c7e_9(n,a,b,c,d) CALL4(n,TAG006c7e_9,a,b,c,d)
RESTARTABLE _TAG006c7e_9(i16, i32, i32, struct HCTI *);//(i32)
//#define TAG00799a_xxx(n,a,b,c,d) CALL4(n,TAG00799a_xxx,a,b,c,d)
//RESTARTABLE _TAG00799a_xxx(i16 P1, i16 P2, i32 P3, i32 P4);//(i32)
//#define TAG00799a_1(n,a) CALL1(n,TAG00799a_1,a)
//RESTARTABLE _TAG00799a_1(i16);//(void)
//#define TAG00799a_2(n,a) CALL1(n,TAG00799a_2,a)
//RESTARTABLE _TAG00799a_2(i16);//(void)
#define TAG00799a_6(n,a) CALL1(n,TAG00799a_6,a)
RESTARTABLE _TAG00799a_6(i16);//(void)
//#define SetSelectedLine(n,a,b) CALL2(n,SetSelectedLine,a,b)
//RESTARTABLE _SetSelectedLine(i16, i32);//TAG00799a_7(void)
#define TAG00799a_9(n,a) CALL1(n,TAG00799a_9,a)
RESTARTABLE _TAG00799a_9(i16);//(void)
#define TAG00799a_12(n,a,b,c) CALL3(n,TAG00799a_12,a,b,c)
RESTARTABLE _TAG00799a_12(i16, i32, i32);//(pnt)
//#define TAG00799a_13(n,a) CALL1(n,TAG00799a_13,a)
//RESTARTABLE _TAG00799a_13(i16);//(i32)
//#define TAG007fdc_2(n,a) CALL1(n,TAG007fdc_2,a)
//RESTARTABLE _TAG007fdc_2(i16);//(void)
//#define TAG007fdc_xxx(n,a,b,c,d,e) CALL5(n,TAG007fdc_xxx,a,b,c,d,e)
//RESTARTABLE _TAG007fdc_xxx(i16 P1, i16 P2, i32 P3, i32 P4, i32);//(i32)
#define TAG007fdc_7(n,a,b,c,d) CALL4(n,TAG007fdc_7,a,b,c,d)
RESTARTABLE _TAG007fdc_7(i16, i32, i32, i32);//(void)
#define TAG007fdc_36(n,a,b,c) CALL3(n,TAG007fdc_36,a,b,c)
RESTARTABLE _TAG007fdc_36(i16, i32, i32);//(void)
//#define TAG008c40_3(n,a,b,c) CALL3(n,TAG008c40_3,a,b,c)
//RESTARTABLE _TAG008c40_3(i16, i32, i32);//(void)
#define TAG008c40_5(n,a,b) CALL2(n,TAG008c40_5,a,b)
RESTARTABLE _TAG008c40_5(i16, i32);//(i32)
#define TAG008c40_6(n,a,b) CALL2(n,TAG008c40_6,a,b)
RESTARTABLE _TAG008c40_6(i16, i32);//(void)
#define TAG008c40_8(n,a,b) CALL2(n,TAG008c40_8,a,b)
RESTARTABLE _TAG008c40_8(i16, i16);//(i32)
//#define TAG008c40_12(n,a,b,c,d) CALL4(n,TAG008c40_12,a,b,c,d)
//RESTARTABLE _TAG008c40_12(i16, i32, i32, pnt);//(i32)
//#define TAG008c40_30(n,a) CALL1(n,TAG008c40_30,a)
//RESTARTABLE _TAG008c40_30(i16);//(void)
#define TAG0051c2_31(n,a) CALL1(n,TAG0051c2_31,a)
RESTARTABLE _TAG0051c2_31(i16);//(void)
#define TAG004e4c_xxx(n,a,b,c) CALL3(n,TAG004e4c_xxx,a,b,c)
RESTARTABLE _TAG004e4c_xxx(i16 /*P1*/, i16 P2, i32 P3);
#define TAG004e4c_6(n,a,b) CALL2(n,TAG004e4c_6,a,b)
RESTARTABLE _TAG004e4c_6(i16, TEXT *);//(void)
#define TAG004e4c_8(n,a,b) CALL2(n,TAG004e4c_8,a,b)
RESTARTABLE _TAG004e4c_8(i16, TEXT *);//(void)
//#define TAG005ae8_9(n,a,b) CALL2(n,TAG005ae8_9,a,b)
//RESTARTABLE _TAG005ae8_9(i16, struct PALETTEPKT *);//(void)
//#define TAG0051c2_35(n,a) CALL1(n,TAG0051c2_35,a)
//RESTARTABLE _TAG0051c2_35(i16);//(void)
#define PaletteFade(n,a,b) CALL2(n,PaletteFade,a,b)
RESTARTABLE _PaletteFade(i16,struct PALETTEPKT *);//TAG005ae8_34//(void)
//#define TAG005ae8_1(n,a) CALL1(n,TAG005ae8_1,a)
//RESTARTABLE _TAG005ae8_1(i16);//(void)
#define TAG0051c2_2(n,a) CALL1(n,TAG0051c2_2,a)
RESTARTABLE _TAG0051c2_2(i32);//(void)
#define TAG005d2a(n,a,b) CALL2(n,TAG005d2a,a,b)
RESTARTABLE _TAG005d2a(i32, i32);//(void)
//#define TAG0093a0_xxx(n,a,b,c,d) CALL4(n,TAG0093a0_xxx,a,b,c,d)
//RESTARTABLE _TAG0093a0_xxx(i32 P1, i32 P2, i32 /*P3*/, i32 P4);
//#define TAG0093a0_17(n,a,b) CALL2(n,TAG0093a0_17,a,b)
//RESTARTABLE _TAG0093a0_17(i16, i32);//(void)
//#define MyReadFirstBlock(n,a) CALL1(n,MyReadFirstBlock,a)
//RESTARTABLE _MyReadFirstBlock(pnt);//TAG006672//(void)
//#define TAG0093a0_18(n,a,b) CALL2(n,TAG0093a0_18,a,b)
//RESTARTABLE _TAG0093a0_18(i32, i32);//(void)
//#define LZWRawChar(n,a,b) CALL2(n,LZWRawChar,a,b)
//RESTARTABLE _LZWRawChar(i16, i32 *);//TAG003022//(i32)
//#define TAG003210(n,a,b,c,d,e) CALL5(n,TAG003210,a,b,c,d,e)
//RESTARTABLE _TAG003210(i16, i32, pnt, pnt, pnt);//(i32)
//#define TAG004a22_xxx(n,a,b,c,d) CALL4(n,TAG004a22_xxx,a,b,c,d)
//RESTARTABLE _TAG004a22_xxx(i16 P1, i16 P2, i32 P3, i32 P4);
//#define TAG004a22_14(n,a) CALL1(n,TAG004a22_14,a)
//RESTARTABLE _TAG004a22_14(i16);//(pnt)
//#define TAG004a22_16(n,a,b) CALL2(n,TAG004a22_16,a,b)
//RESTARTABLE _TAG004a22_16(i16, i32);//result is guess//(pnt)
//#define Free_TEXT(n,a,b) CALL2(n,Free_TEXT,a,b)
//RESTARTABLE _Free_TEXT(i16, TEXT *);//TAG004a22_20//(void)
//#define TAG004a22_26(n,a,b,c) CALL3(n,TAG004a22_26,a,b,c)
//RESTARTABLE _TAG004a22_26(i16, i32, i32 = 0x1baddade);//(void)
//#define TAG004f3a_xxx(n,a,b,c) CALL3(n,TAG004f3a_xxx,,a,b,c)
//RESTARTABLE _TAG004f3a_xxx(i32 P1, i32 P2, i32 P3);
//#define TAG004f3a_1(n,a) CALL1(n,TAG004f3a_1,a)
//RESTARTABLE _TAG004f3a_1(i16);//(pnt)
//#define TAG004f3a_2(n,a) CALL1(n,TAG004f3a_2,a)
//RESTARTABLE _TAG004f3a_2(i16);//(void)
//#define AssignMemory(n,a,b) CALL2(n,AssignMemory,a,b)
//RESTARTABLE _AssignMemory(i16, i32);//TAG004f3a_14//(pnt)
//#define TAG002f6c(n,a,b,c,d) CALL4(n,TAG002f6c,a,b,c,d)
//RESTARTABLE _TAG002f6c(pnt, i16, i16, i16);//(i32)
//#define TAG003006(n,a,b) CALL2(n,TAG003006,a,b)
//RESTARTABLE _TAG003006(pnt, i16);//(i32)
//#define TAG004f3a_16(n,a,b) CALL2(n,TAG004f3a_16,a,b)
//RESTARTABLE _TAG004f3a_16(i16, i32);//(pnt)
//#define ReleaseMem(n,a,b) CALL2(n,ReleaseMem,a,b)
//RESTARTABLE _ReleaseMem(i32, pnt);//TAG004f3a_20//(pnt)
//#define TAG002e64(n) CALL0(n,TAG002e64)
//RESTARTABLE _TAG002e64(void);//(void)
//#define TAG0060c4(n) CALL0(n,TAG0060c4)
//RESTARTABLE _TAG0060c4(void);//(i32)
//#define TAG0062a2(n,a,b) CALL2(n,TAG0062a2,a,b)
//RESTARTABLE _TAG0062a2(wordRectPos *, pnt); //(void)
//#define TAG006bc0(n,a) CALL1(n,TAG006bc0,a)
//RESTARTABLE _TAG006bc0(i32);//(pnt)
//#define TAG006bfc(n,a) CALL1(n,TAG006bfc,a)
//RESTARTABLE _TAG006bfc(pnt);//(void)
//#define TAG006718(n,a) CALL1(n,TAG006718,a)
//RESTARTABLE _TAG006718(i16 P1);
//#define TAG00835c_xxx(n,a,b) CALL2(n,TAG00835c_xxx,a,b)
//RESTARTABLE _TAG00835c_xxx(i16 P1, i16 P2);
//#define TAG00835c_25(n,a) CALL1(n,TAG00835c_25,a)
//RESTARTABLE _TAG00835c_25(i16);//(i32)
//#define TAG008a62(n) CALL0(n,TAG008a62)
//RESTARTABLE _TAG008a62(void);//(i32)
//#define TAG00948c(n,a) CALL1(n,TAG00948c,a)
//RESTARTABLE _TAG00948c(i16);//(i32)
//#define TAG0094de(n,a,b,c) CALL3(n,TAG0094de,a,b,c)
//RESTARTABLE _TAG0094de(i16, struct PAGE *, struct NODE *);//(pnt)
//#define AddPage(n,a,b) CALL2(n,AddPage,a,b)
//RESTARTABLE _AddPage(i16, struct PAGE *);//TAG009516//(i32)
//#define TAG0095b0(n,a,b) CALL2(n,TAG0095b0,a,b)
//RESTARTABLE _TAG0095b0(i32, i32);//(pnt)
//#define TAG00964e(n,a,b) CALL2(n,TAG00964e,a,b)
//RESTARTABLE _TAG00964e(i32, i32);//(pnt)
//#define TAG00978a_xxx(n,a,b,c,d,e) CALL5(n,TAG00978a_xxx,a,b,c,d,e)
//RESTARTABLE _TAG00978a_xxx(i16 P1, i16 P2, i32 P3, i32 P4, i32 P5);
//#define TAG00978a_1(n,a) CALL1(n,TAG00978a_1,a)
//RESTARTABLE _TAG00978a_1(i16);//(void)
//#define TAG00978a_3(n,a,b) CALL2(n,TAG00978a_3,a,b)
//RESTARTABLE _TAG00978a_3(i16, struct PAGE *);//(void)
//#define TAG00978a_12(n,a,b) CALL2(n,TAG00978a_12,a,b)
//RESTARTABLE _TAG00978a_12(i16, i32);//(pnt)
//#define TAG00978a_13(n,a) CALL1(n,TAG00978a_13,a)
//RESTARTABLE _TAG00978a_13(i16);//(i32)
//#define TAG00978a_21(n,a,b) CALL2(n,TAG00978a_21,a,b)
//RESTARTABLE _TAG00978a_21(i16, i32);//(pnt)
//#define TAG00978a_27(n,a,b,c,d) CALL4(n,TAG00978a_27,a,b,c,d)
//RESTARTABLE _TAG00978a_27(i16, i32, pnt, i32);//(void)
//#define TAG00978a_28(n,a,b,c) CALL3(n,TAG00978a_28,a,b,c)
//RESTARTABLE _TAG00978a_28(i16, i16, i16);//(pnt)
//#define TAG00978a_29(n,a,b) CALL2(n,TAG00978a_29,a,b)
//RESTARTABLE _TAG00978a_29(i16, i16);//(void)
//#define TAG0051c2_37(n,a) CALL1(n,TAG0051c2_37,a)
//RESTARTABLE _TAG0051c2_37(i16);//(void)
//#define TAG0051c2_36(n,a) CALL1(n,TAG0051c2_36,a)
//RESTARTABLE _TAG0051c2_36(i16);//(void)
//#define TAG005ae8_38(n,a,b) CALL2(n,TAG005ae8_38,a,b)
//RESTARTABLE _TAG005ae8_38(i16,PALETTEPKT *);//(void)
#define TAG0089b0(n,a,b,c) CALL3(n,TAG0089b0,a,b,c)
RESTARTABLE _TAG0089b0(i16,struct S20 *,struct T12 *);//(void)
#define TAG005a1e_5(n,a,b) CALL2(n,TAG005a1e_5,a,b)
RESTARTABLE _TAG005a1e_5(i32, i32);//(void)
#define TAG009410(n,a,b) CALL2(n,TAG009410,a,b)
RESTARTABLE _TAG009410(pnt, i32);//(void)
#define TAG001dde(n,a) CALL1(n,TAG001dde,a)
RESTARTABLE _TAG001dde(struct S6 *);//(i32)
const char *TranslateLanguage(const char *);

extern ui32 *pDSAparameters;
class NEWDSAPARAMETERS
{
  ui32 *saveoldparameters;
  ui32 newparameters[101];
public:
  NEWDSAPARAMETERS(void)
  {
    saveoldparameters = pDSAparameters;
    pDSAparameters = newparameters;
    pDSAparameters[0] = 0;
  };
  ~NEWDSAPARAMETERS(void)
  {
    pDSAparameters = saveoldparameters;
  };
};




struct str10 // used in TAG01f140
{
  i16 w1;
  i16 w2;
  i16 w3;
  i16 w4;
  i16 w5;
};


enum PARTYMOVE_CONSTANTS
{
  PM_BEGINTURN   = 1,
  PM_STAIRWAY    = 2,
  PM_ATTEMPTMOVE = 3,

  PM_INHIBITMOVE = 0x0001,
  PM_SETDELAY    = 0x0002,
  PM_ADDDELAY    = 0x0004,
};


struct PARTYMOVEDATA
{
  ui32 moveType;     // PM_BEGINTURN
                     //   The party is about to turn.
                     //   'fromLocationType' is valid.
                     //      if locationType = 3 = stairwell then if you do not
                     //      inhibit the movement the filter will be called
                     //      again when the party is about to traverse the
                     //      stairwell.
                     //   'toLocationType' is valid.
                     //   'fromLocaation' is valid.
                     //   'toLocation' is valid unless locationType is stairwell.
                     //   'direction' is 0, 1, 2, 3 for right, left, up, down.
                     //   flags is 0.
                     //   Set flag PM_INHIBITMOVE to cancel the turning movement
                     // PM_STAIRWAY
                     //   The party is about to traverse a stairway.
                     //   'relDirection' = 'absDirection' = 0 for down, 1 for up
                     //   'flags' = 0
                     //   'fromLocation' is valid
                     //   'toLocation' is valid
                     //   'fromLocationType' is valid
                     //   'toLocationType' is valid and is equal to 3.
                     //   Set flag PM_INHIBITMOVE to cancel the movement
                     // PM_ATTEMPTMOVE
                     //   The party is about to attempt a move forward, backward, slide left, slide right
                     //   'relDirection' 0, 1, 2, or 3 for forward, right, backward, left
                     //   'absDirection is 0, 1, 2, or 3 for north, east, south, west
                     //   'flags' is zero
                     //   'fromLocation' is valid
                     //   'toLocation' is valid
                     //   'fromLocationType' is valid
                     //   'toLocationType is valid
                     //   'staminaAdjustments are valid and you can change them.  They will
                     //         not be applied if the move is inhibited by PM_INHIBITMOVE.
                     //   You can set delay to be the delay before the party can move again.
                     //      If you set PM_SETDELAY then this value will be used whether or not
                     //          the party moves.  They may be inhibited by a monster, for example.
                     //          If you set PM_INHIBITMOVE then the delay will be ignored.
                     //      If you set PM_ADDDELAY then this value will be added to the computed
                     //          delay only if the party actuall moves.
                     //   Set flag PM_INHIBITMOVE to cancel the movement.
  ui32 flags;
  ui32 delay;
  ui32 staminaAdjustment[4]; // decrement to stamina; -1 if character non-existent
  ui32 relDirection;    //
  ui32 absDirection;
  ui32 fromLocation; // location with pos = facing.
  ui32 toLocation;   // location with pos = facing.
  ui32 fromLocationType; // cellType (or roomType) 0=stone, etc.
  ui32 toLocationType;
};

void CallPartyMoveFilter(PARTYMOVEDATA *pmd);
