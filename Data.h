
#pragma pack(1)


#define maxFilesOpen 8
#define NumExpandedGraphics 1275
#define MAXLOAD 2000 // Else ASSERTION!
                     // I had some troubles with loads.
                     // Loads can be larger but this is a nice
                     // number for debugging purposes.


#if (defined _MSVC_INTEL) || (defined _MSVC_CE2002ARM)
#define LOW_I16(X) ((i16)((i32)X))
#else


#define LOW_I16(X) ((i16)((intptr_t)(X)))
//#define LOW_U16(X) ((u16)((intptr_t)(X)))
#endif

struct MouseQueueEnt;
struct ITEMQ;

void TranslateFullscreen(i32 a, i32 b, i32& x, i32& y);
void Cleanup(bool programTermination);

enum SPEEDS
{
  SPEED_GLACIAL,
  SPEED_MOLASSES,
  SPEED_VERYSLOW,
  SPEED_SLOW,
  SPEED_NORMAL,
  SPEED_FAST,
  SPEED_QUICK,
  SPEED_NUMSPEED
};

enum VOLUMES
{
  VOLUME_FULL      = 0,
  VOLUME_HALF      = 1,
  VOLUME_QUARTER   = 2,
  VOLUME_EIGHTH    = 3,
  VOLUME_OFF       = 4,
  VOLUME_NUMVOLUME = 5
};

class SPEEDTABLE
{
public:
  i32 vblPerTick;
};

class VOLUMETABLE
{
public:
  i32 divisor;
  i32 attenuation; //decibels
};

enum VIDEOMODE
{
  VM_DEFAULT,     // When we don't know what to do.
  VM_PRESENTS,    // While the word "Presents" is on screen
  VM_CENTERMENU,  // While a Menu (such as game selection) is on screen
  VM_LEFTMENU,    // While a Menu (such as save game menu) is on screen
  VM_ADVENTURE,   // While the party is adventuring
  VM_THEEND,      // The words "The End" appear on the screen
  VM_CREDITS,     // While author credits are on screen
  VM_CAST,        // Cast of characters and stats when game is won
  VM_PRISONDOOR,  // While door to prison is showing
  VM_INVENTORY,   // Hero's inventory screen.
  VM_FROZEN,      // Game is frozen.
  VM_SLEEPING     // Sleep during adventure
};

extern VIDEOMODE videoMode;
extern SPEEDTABLE speedTable[SPEED_NUMSPEED];
extern VOLUMETABLE volumeTable[VOLUME_NUMVOLUME];
extern bool usingDirectX;
extern bool playerClock;
extern bool extraTicks;
extern SPEEDS gameSpeed;
extern VOLUMES gameVolume;
extern i32 totalMoveCount;
extern bool NoSound;
extern bool NoSleep;
extern bool AutoEnlarge;
extern bool ThreeDMovements;
extern bool GravityGame;
extern bool gravityMove;
extern bool DM_rules;
extern bool indirectText;
extern bool bigActuators;
//extern bool SequencedTimers;
extern bool disableSaves;
extern i32 nextGravityMoveTime;
extern char moveHistory[4];
extern i32  ExtendedFeaturesSize;
extern char ExtendedFeaturesVersion;
extern i32 RememberToPutObjectInHand;
extern char *dungeonName;
extern char *folderName;
extern char *folderParentName;
//extern char *folderSavedGame;
extern char *root;
extern i32  *videoSegSize;
extern i32  *videoSegSrcX;
extern i32  *videoSegSrcY;
extern i32  *videoSegWidth;
extern i32  *videoSegHeight;
extern i32  *videoSegX;
extern i32  *videoSegY;
extern i32  *videoPaletteNum;
extern char *gameInfo;
extern i32   gameInfoSize;
extern ui32 graphicSignature1, graphicSignature2;
extern ui32 expectedGraphicSignature1, expectedGraphicSignature2;
extern ui32 CSBgraphicSignature1, CSBgraphicSignature2;
extern ui32 expectedCSBgraphicSignature1, expectedCSBgraphicSignature2;
extern ui32 dungeonSignature1, dungeonSignature2;
extern ui32 versionSignature;
extern ui32 spellFilterLocation;
extern ui32 cellflagArraySize;
extern ui32 EDBT_CSBGraphicsSignature_data;
extern ui32 EDBT_GraphicsSignature_data;
extern ui32 EDBT_CSBversion_data;
extern ui32 EDBT_Debuging_data;
extern EXPOOL expool;
extern bool monsterMoveFilterActive;
extern ui32 parameterMessageSequence;


// File handles are indexes into this table.
extern FILETABLE fileTable[maxFilesOpen];
extern i16 TraceFile;
extern i16 GraphicTraceFile;


extern bool TimerTraceActive;
#ifndef _MSVC_INTEL
extern bool DSATraceActive;
#endif
extern bool AttackTraceActive;
extern bool AITraceActive;
extern bool GraphicTraceActive;
extern i32  traceViewportDrawing;

extern i32 NoSpeedLimit;
extern bool NoClock;
extern bool BeginRecordOK;
extern bool ItemsRemainingOK;
extern i32 keyboardMode;
extern i32 NumberFloppyDrives;
extern bool gameFrozen;
extern i32 screenSize;
extern bool DiskMenuNeeded;
extern i32 GameTime;
extern i32  MostRecentlyAdjustedSkills[2];
extern i32  LatestSkillValues[2];
extern i32  AdjustedSkillNumber;
extern const char *tracePrefix;
const char *initialTracePrefix(bool trace);
const char *nextTracePrefix(const char *trace);


struct STRUCT5688
{
  ui8 uByte0;
  ui8 uByte1;
  ui8 width;  //uByte2;
  ui8 height; //uByte3;
  i16 word4;
};

struct SD
{
  SD *pnext;
  char name[16];
  i32 minimum;
  float level;
  float y_intercept;
  float slope;
};

/*
class SDLIST
{ // A class so destructor will release memory
  SD *psd;
public:
  SDLIST(void) { psd=NULL; };
  ~SDLIST(void);
  SD *newSD(char *,i32,float,float,float);
  SD *First(void);
  SD *Next(SD *ent);
};
*/
/*
extern SDLIST sdList;

struct SDENT
{
  i32 minimum;
  float level;
  float y_intercept;
  float slope;
};

extern SDENT sdTable[199];
*/
#define FILL(a,b) i8 fill##a[a-b];

#define IllegalWallDecorationWarning 1

struct DECORATION_DESC
{
  BYTE_RECTANGLE rect;
  ui8 width; // #bytes
  ui8 height;
};


class LEGALATTACKS
{
public:
  ui8 byte0[8];
};


struct DBank {
  DBank(void);
  ~DBank(void);
  void Initialize(void);
  friend class GameTimers;
  void AllocateDerivedGraphicCacheIndex(void);
  i16  GetDerivedGraphicCacheIndex(i32 graphicNum);
  void SetDerivedGraphicCacheIndex(i32 graphicNum, i32 index);
  void SetDerivedGraphicSize(i32 graphicNum, i32 size);
  ui16 GetDerivedGraphicSize(i32 graphicNum);
  void AllocateDerivedGraphicsSizesBuffer(void);
  i32 unusedLong23326;    //  Static memory remaining size.
  ui8 *Pointer23322;  // Start of remainder of static memory.
  ui8 *Pointer23318;  // Next available memory location.  Increments as memory allocated.
  pnt Pointer23314;
  pnt Pointer23310;
  i32 iAvailableMemory; //Long23306;     // 'malloc' memory remaining size
  KeyXlate *pKeyXlate1; //16884;
  KeyXlate *pKeyXlate2; //16880;
  btn *SecondaryButtonList;//16876;
  btn *PrimaryButtonList;  //16872;
  ui8 *pEndOfAvailMemory;//pnt Pointer23298;  // Address of end of avail 'malloc' memory.
  ui8 *Pointer23294;  // Address of end of 'malloc' memory
  ui8 *Pointer22968; // Graphic #534
  ui8 *Pointer22964; // Graphic #535
  ui8 *Pointer22960; // Graphic #5
  ui8 *Pointer22956; // Graphic #4 Prison entrance minus door
  ui8 *Pointer22952; // Graphic #2 Prison door - left
  ui8 *Pointer22948; //  ShrinkBLT (Graphic #2)
  ui8 *Pointer22944; //  ShrinkBLT (Graphic #2)
  ui8 *Pointer22940; //  ShrinkBLT (Graphic #2)
  ui8 *Pointer22936; // Graphic #3 Prison door - right
  ui8 *Pointer22932; //  ShrinkBLT (Graphic #3)
  ui8 *Pointer22928; //  ShrinkBLT (Graphic #3)
  ui8 *Pointer22924; //  ShrinkBLT (Graphic #3)
  ui8 *Pointer22920;
  ui8 *Pointer22916; // Prison dungeon interior
  
  const char* Pointer22912;
  const char* Pointer22908;
  const char* Pointer22904;
  const char* Pointer22900;
  const char* Pointer22896;
  const char* Pointer22892;
  const char* Pointer22888;
  const char* Pointer22884;
  const char* Pointer22880;
  const char* Pointer22876;
  const char* Pointer22872;
  const char* Pointer22868;
  const char* Pointer22864;
  const char* Pointer22860;
  const char* Pointer22856;
  const char* Pointer22852;
  const char* Pointer22848;
  const char* Pointer22844;
  const char* Pointer22840;
  const char* Pointer22836;
  const char* Pointer22832;
  const char* Pointer22828;
  const char* Pointer22824;
  const char* Pointer22820;
  const char* Pointer22816;
  const char* Pointer22812;
  LEVELDESC *pLevelDescriptors;//10442;
                        // points to array of 16-byte structures
                        // read from dungeon.dat.  Parallels the
                        // array at Pointer10458.
                        // Words 0, 8, 10, 12, and 14 are switched
                        // to littleEndian when we read the file.
  ui16 *objectLevelIndex;// *puwPointer10458;
                         //Index in pwPointer10454 of first
                         //column in each level.
  ui16 *objectListIndex; // *pwPointer10454;
                         // One word for each DUDAD (numDUDADpointers).
                         // Small integers (0x00 - 0x41) sorted with
                         // a couple of duplicates.
                         // LittleEndian when read from dungeon.dat.
  CELLFLAG ***pppdPointer10450;
      // Actually, the 10450 array has two parts.  The first
      // <numlevel> pointers in the array point repectively to the
      // second part for each level.
      // Each level then has an array of pointers to the
      // contents of the cells in each of the maze's columns.
  DUNGEONDATINDEX *dungeonDatIndex;
                        //10446; // points to 22-word array.
                        // All 22 words swapped when read.
                        // Word[0] used by TAG00a41c.
                        // Word[2] bits 8-15 = number of levels
                        // Words reversed when read from file
                        // Word[5] = size of array 10464
                        // At runtime, word[i+6] is the number
                        // of entries allocated for database[i].
                        // In general, many will be empty.
  CELLFLAG *cellFlagArray;//pdPointer10438;
     // This is the CELLFLAG array.  Seldom used.  10450 is set
     // to point to the start of each level and to the start
     // of each column within each level.




  i16 LZWrepeatChar; // (23408) repeated by character 0x90 0xxx sequence
                     //    except that 0x90 0x00 means 0x90.
  i16 Word23406;
  i16 LZWBitNumber; // (23404) Bit number within 9-byte I/O buffer
  i8  RightOneMask[9];      // 23402
  FILL(23393,23392)
  i8  RightZeroMask[9];     // 23392
  FILL(23383,23382)
  i16 Word23382;
  i16 LZWNextCode;    // (23380)
  ui8 IOBuffer[12]; // (23378)how big should this be????? At least 9!
  i16 Word23366;
  i16 LZWMaxCode;  //(23364)Maximum code value (eg: 511 for 9 bit code)
  i16 LZWCodeSize; //(23362)Number of bits.
  i16 LZWrepeatFlag; //(23360)
  i32 unusedLong23358; // copied from 11718 = d.Time
                 // While d.Time == this value the second part
                 // of the graphic list is not cleared.
  i16 newFlags;//Unused23354;//BigMemory; //23354// Set if lots of memory. (>370000 avail)
       // This word is now used for something entirely different.
       // See the flags definitions at top of struct.
  ITEMQ *unusedpqFirstGraphic2;//23352; //First ITEMQ in 2nd part of list
  ITEMQ *unusedpqLastGraphic; //23348; // Last ITEMQ in list?
  ITEMQ *pqFirstGraphic;//23344; // First ITEMQ in list
            // There is a list of graphics.  The first entry in the list
            // is   pqFirstGraphic and the last is    pqLastGraphic.
            // ** BUT ** the list is divided into two parts and the first
            // entry in the second part is   pqFirstGraphic2.
  ITEMQ *pgUnused; //23340; First free graphics buffer
  i32 iAvailableGraphicMemory; //space remaining?
  ITEMQ **pGraphicCachePointers;  //23332; // pointer to 5100 bytes initially zero
                                  // Each pointer points to an ITEMQ
                                  // which is followed by a bitmap
                                  // and another longword with the size repeated.
                                  // So each buffer has the form:
                                  //  i32 size (Negative if allocated)
                                  //  i16 flag
                                  //  i16 index of previous
                                  //  i16 index of next
                                  //  i16 graphic number
                                  //  (size - 16) bytes of graphic
                                  //  i32 size (Negative if allocated)
  i16 Word23328;
  i16 CacheInvalid;  // (23290) non-zero if contents of ClusterCache is invalid
  ui8 *ClusterCache;  // (23288) Pointer to 1024 bytes
  ui8 *pStartAvailMemory;// Pointer23284;  // 10000 below initially allocated memory blocks
                        //This is also the end of graphic memory.
  ui8 *pStartMemory;  // Start of memory.
                     // Also the start of Graphic memory.
  private:
  i16 *pwDerivedGraphicSizes;//Pointer23274;  // Pointer to 1460 bytes of memory
  i16 *pDerivedGraphicCacheIndex;//23270;
                        // 730 words filled with -1 initially
                        // For each graphic number, this is the index in
                        // the graphic cache.
  public:
  i16 *GraphicIndex0; //pwPointer23266;  
          // GraphicIndex0 contains NumGraphic(from graphics.dat) 
          // entries.
          // Each graphics index in ppq23332.
          // 23266 and 23270 contain the graphics buffer index for each
          // of the graphics numbers.
          // There are two types of graphics:  Those with graphics numbers
          // with bit 15 set (0x8000) are listed in 23270 and those with
          // bit 15 clear are listed in 23266.
          // A graphic is located at ppExpandedGraphics[pwPointernnnn[graphicNum]].
  pnt *ppUnExpandedGraphics;// *ppPointer23262;  // See immediately below.
  ui16 *GraphicDecompressedSizes; //(23258) Sizes of decompressed graphics
  ui16 *GraphicCompressedSizes;    //(23254) Array .. size of each graphic.
  i32 GraphicFileEOF;   //(23250) End+1 of GRAPHICS.DAT file
  ui8 *SavedCursorPixels; //Pointer23198;
  ui8 *ScreenCursorAddress;// Area saved uder cursor//Pointer23194;
  pnt Pointer23190;
  pnt Pointer23186;
  pnt Pointer23182;
  pnt Pointer23178;
  ui8 *CursorBitmap; //Pointer23174; Adjusted to be on word boundary
  pnt Pointer23170; // used in VBL handler
  const char* Pointer16822[7];//"STRENGTH", "DEXTERITY", etc
  const char* DirectionNames[4];//16794;
  pnt Pointer16778;
  i16 *pwPointer16774; // Points to d.Word918 initially
  const char* Pointer16770[17];//"Novice", "Master", etc
  pnt Pointer12926;
  ui8 *newTextLine; //12922
           // This line contains the text bitmap to
           // be scrolled into the bottom of the
           // screen. 12916 is set to 0 to get the
           // process started. (qv)
  char *ObjectNames[199];//12786;
  ui32 RandomNumber;      // 11986  Initially set from GEMDOS Random function
  ui8 *LogicalScreenBase; // 11982
  ui8 *compressedGraphic0;//Pointer11782; // Graphic zero?? Menu graphic?
  ui8 *pViewportBMP; //11682; // 15232 bytes - This is an image of the
  ITEM16 *pI1613134;
  i32 Time13130;


  i32 LastPartyMoveTime;// Long12954; // Time at which party last moved.
  i32 LastMonsterAttackTime; //Long12950; // Time we last called MonsterAttacks(TAG00c8c0)???
  i32 TextTimeout[4];// 12946[4];
                    // One per text line at
                    // bottom of screen
                    // This is the time at which the line
                    //  should be removed from the screen.
  ui32 *indirectTextIndex; //NumWordsInTextArray() entries.
  ui32  sizeOfCompressedText;
  ui16 *compressedText;//Pointer10346; //Do your own littleEndian!
                    // Something to do with text.
  i16 *Pointer2526[3]; //&Word2050, &Word2210, &Word2318
  pnt Pointer2250; // 5 Longwords moved between here and 2110
  pnt Pointer2246; // 5 Longwords moved between here and 2110
  pnt Pointer2242; // 5 Longwords moved between here and 2110
  pnt Pointer2238; // 5 Longwords moved between here and 2110
  pnt Pointer2234; // 5 Longwords moved between here and 2110
  pnt Pointer2230; // 5 Longwords moved between here and 2110
  pnt Pointer2226; // 5 Longwords moved between here and 2110
  pnt Pointer2222; // 5 Longwords moved between here and 2110
  pnt Pointer2218; // 5 Longwords moved between here and 2110
  pnt Pointer2214; // 5 Longwords moved between here and 2110


  ui8 *pDoorBitmaps[8];
  //pnt Pointer2142; // [0]  1504 bytes  //F1               Door Facing Right Frame
  //pnt Pointer2138; // [1]  1968 bytes  //F0               Door Edge
  //pnt Pointer2134; // [2]  1504 bytes  //F1               Door Facing Left Frame
  //pnt Pointer2130; // [3]  1560 bytes  //F2               Door Facing Left Frame
  //pnt Pointer2126; // [4]   704 bytes  //F3               Door Facing Left Frame
  //pnt Pointer2122; // [5]   688 bytes  //F3L1 & F3R1      Door Facing Left Frame
  //pnt Pointer2118; // [6]   256 bytes  //F1R1 & F1 & F1L1 Door Facing Top Track
  //pnt Pointer2114; // [7]   144 bytes  //F2L1 & F2 & F2R1 Door Facing Top Track)
  ui8 *pWallBitmaps[7];
  // 5 pointerss moved as group between here and 2230
  //pnt Pointer2110; // [0]  2176 bytes  //F0R1
  //pnt Pointer2106; // [1]  2176 bytes  //F0L1
  //pnt Pointer2102; // [2]  1408 bytes  //F1L1, F1, F1R1
  //pnt Pointer2098; // [3]  5112 bytes  //F2L1, F2, F2R1
  //pnt Pointer2094; // [4]  3264 bytes  //F3L1, F3, F3R1
  //pnt Pointer2090; // [5]   392 bytes  //F3L2
  //pnt Pointer2086; // [6]   392 bytes  //F3R2

  ui8 *pViewportFloor;   //2082 - 7840 byte  //Address of floor in viewportBMP
  ui8 *pViewportBlack;   //2078 - 4144 bytes //Address of space between floor
                                            // and ceiling in viewportBMP
  ui8 *pCeilingBitmap;   //2074 - 3248 bytes
  ui8 *pFloorBitmap;     //2070 - 7840 bytes

  pnt Pointer2066[4];
  btn *Pointer18986;
  btn *Pointer18982;
  KeyXlate *pKeyXlate18978;
  KeyXlate *pKeyXlate18974;
  btn *pButn18970[2][4]; // The menu button lists
  ITEM16 *Item16;  //13010; // Word13012  16-byte units?
                   // Initialized with -1 in word 0 of each enrty.
                   // Constructed by AttachItem16ToMonster???
                   // This appears to be a queue of some sort.
                   // Word13014 is maybe the number of active
                   // entries.  TAG00bd40 seems to remove the
                   // active entries.
  ui32 WatchdogTime;//13006; // Always one TIMER in queue.
  pnt unused12998; // *timerQue;//12998; // Appears to point to an array of words
                     // that in turn serve as
                     // indexes into array Pointer12994;
  i32 unused12994; //TIMER *Timers;// 12994;//Array of TIMER structures
  i32 Time;// 11718;// time (units of "clockTicks") See "Speed"
                    // Should be about 1/6 seconds



// ************Only 32-bit variables above this line

  i8  Byte22570[186];
  //i16 Word22386;
  i16 Word22384;
  FILL(22382,21994)

  i8  Byte21994[576]; // Magic menu???
  i8  Byte21418[576]; // Magic menu???
  i8  Byte20842[576]; // The magic menu outline, maybe???


  i16 ClusterInCache;// (23276) Current cluster in ClusterCache
  ui16 NumGraphic;      //(23246) First two bytes of file are put here.(handle Word23242)
                  // This is the number of graphics in GRAPHICS.DAT
                  // Sizes put at *Pointer23254
                  // Second block put at *Pointer23258;
                  // *Pointer23262 is a buffer twice the block size
                  //   with first half cleared.
                  // *Pointer 23266 is a blocksize buffer filled with -1;
  i16 Word23244; // File open count? Reference count?
  i16 GraphicHandle; // (23242)File handle
  FILL(23240,23236)

    // VBL handler fetches these function pointers but
    // it appears that the use of the pointers has
    // been 'NOP'ed out.
  void (*pFunc23236)(i32 P1);
  void (*pFunc23232)(void);
  i16 Unused23228;  //Always zero.  I removed it.Word23228;
  i16 Word23226;
  i16 OnMouseSwitchActionLock;//Word23224; // Test/Set for Mouse interrupt
  RectPos wRectPos23222;
  RectPos wRectPos23214;
  i8  Byte23206[4];
  i8  Byte23202[4];
  i16 CursorWidth; //Word23166// in 16-pixel units
  i16 CursorHeight; //Word23164
  i16 NewCursorY; //Word23162// new cursor x pos
  i16 NewCursorX; //Word23160// new cursor y pos
  i16 Word23158; // Cursor Hotspot y???
  i16 Word23156; // Cursor Hotspot x???
  i16 CurCursorY; //Word23154// current cursor x pos
  i16 CurCursorX; //Word23152// current cursor y pos
  i16 Word23150;
  i16 CursorShape;//23148; 0 = Arrow
  i16 Word23146; // used in vertical blank handler
  i16 Word23144; // Special cursor flag??? Ordinal
                 // of character whose position is moving
  i16 NewCursorShape; //Word23142;// 0 or 1
  i16 PressingEyeOrMouth;//Word23140; // used in mouse interrupt handler
                 // Set when showing skill levels
  i16 Word23138; // used in vertical blank handler
  i16 Word23136; // Cursor is being displayed. Save area valid.
  i16 Word23134; // Referenced in mouse interrupt
  i16 Word23132; // used in vertical blank handler
  i16 Word23130;
  i16 CurMouseY; //Word23128;
  i16 CurMouseX; //Word23126;
  i16 NewMouseY; //Word23124;
  i16 NewMouseX; //Word23122;
  i16 MouseSwitches;//23120; // Mouse interrupt
  i16 Word23118; // Hide cursor counter???
  i16 Word23116;
  i8  Byte23114;
  FILL(23113,23046)
  i16 Word23046;   //if non-zero then volume
  i16 CurrentSound;//23044;
  i8  Byte23042; // Save old keyclick state from $484
  FILL(23041,23040)
  pnt Pointer23040; // Pointer to $484 = conterm = keyclick
  i16 Word23036;
  FILL(23034,23032)
  i16 UseByteCoordinates;//23032; // Affects whether bytes or
                         // words are used in RectPos in
                         //  Bitmap operations
  char  Byte23030[23030-23024];
  //FILL(23029,23024)
  char  Byte23024[23024-23008];
  //FILL(23023,23008)
  char  Byte23008[23008-22992];
  //FILL(23007,22992)
  i16 DualFloppyDrive;//Word22992; // Set to 1 if 2 floppy drives
  FILL(22990,22988)
  const char *Pointer22988; // Set to "DRIVE."
  const char *Pointer22984; // Set to "DRIVE."
  char *Pointer22980;
  char *SaveGameFilename;//22976;
  i16 Word22972;
  i16 SingleFloppyDrive; //Word22970; // Set to 1 if not 2 floppy drives
  i8  Byte22808[132]; // read/write as unit to CSBGAME.DAT
  i8  Byte22676;
  FILL(22675,22608)
  i32 inStreamLeft; // 22608 // #bytes left in input stream
  pnt inStreamNext; // 22604 // Next byte in input stream
  i16 inStreamBuffered; // 22600 flag if input from buffer
  i16 Word22598;
  i16 Word22596; // equals 1 or 2 ; I have never seen anything
                 // but a 2.
  i16 Word22594;
  i16 Word22592; // From bytes 6 and 7 of dungeon.dat file = 0x000c
                 // distinguish between DM and CSB????
  i32 RandomGameID;//22590; // Set to random when prison opened
  i16 CanRestartFromSavegame;// Word22586;
                 //Set to 1 after loading savegame.
                 //Set to 1 after saving game.
                 //Set to 0 after loading dungeon.
                 //Set to 0 after winning game with 4 corbum.
                 //Set to zero after printing "THE END"
                 //Set to zero before killing entire party.
  i16 PartyHasDied;//Word22584;  //Set when party died
  FILL (22582,22578)
  i16 datafileHandle; // 22578// Dungeon.dat file handle
  FILL(22576,22572)
  RN  MonsterUnderAttack;//22572
  i16 MagicCaster;//20266;
  i16 attackDamageToDisplay;//Word20264;
  i16 unused20262;//i16 Word20262;
  i8  PossibleAttackTypes[4];//20260; // Up to 4 different attack types
  i8  Byte20256[4]; // Parallel to 20260.  See GetLegalAttackTypes. Never used that I can see.
  i16 Word20252;
  i16 Word20250;
  i16 Word20248;// compared to button # (#Attack options???)
  i16 AttackingCharacterOrdinal;//20246;
  i8  Byte20244[2];
                    // Start of graphic 0x230
  i16 Word20242[4]; // Rectpos
  RectPos  wRectPos20234; //Swapped when read
  RectPos  wRectPos20226; //Swapped when read
  RectPos  wRectPos20218; //Swapped when read
  RectPos  wRectPos20210; //Swapped when read
  RectPos  wRectPos20202; //Swapped when read
  ui8 Byte20194[16];   // Color map for ShrinkBLT
  //FILL(20193,20178)
  i8  experienceForAttacking[44]; //Byte20178[44];// Indexed by attack type
  i8  SkillNumber[44];// 20134;
                   //Which skill needed for attack??
                   //small integers. Largest = 18.
                   //Indexed by attack type.
  i8  Byte20090[44];
  i8  Byte20046[44];//Indexed by attack type
  i8  Byte20002[44];//Strength required????
  i8  Byte19958[44];
  i8  Byte19914[44];// indexed by attack type
                    // very small integers (2, 6,12, 5) the
                    // largest is 38.
  i8  AttackNames[300];//19870; //"KICK PUNCH BASH etc"
  LEGALATTACKS legalAttacks[44];//  i8  Byte19570[1];// Groups of eight?. Weapon action???//  FILL(19569,19218)
  i16 Unused19218;//Word19218; // 12777 is some magic number here
  SPELL Spells[25];//19216 8 bytes each // Long0, word6 swapped when read
  //                  ID   Type
  // [00] 1-4     0x666f00  3 Party Shield
  // [01] 1-5-2   0x667073  3 Magic Footprints
  // [02] 3-2-6   0x686d77  3 Invisibility
  // [03] 3-1     0x686c00  2 Poison Cloud Missile ff87
  // [04] 3-2-5   0x686d76  3 See Thru Walls
  // [05] 3-3-5   0x686e76  2 Missile ff82
  // [06] 3-4-5   0x686f76  3 Light
  // [07] 4       0x690000  3 Torch
  // [08] 4-4     0x696f00  2 Missile ff80 Fireball
  // [09] 4-5-1   0x697072  1 Strength Potion - KU Potion
  // [10] 4-5-4   0x697075  3 FireShield
  // [11] 5-2     0x6a6d00  2 Missile ff83 Damage non-Material
  // [12] 5-1     0x6a6c00  3 Poison Bolt
  // [13] 5-4-6   0x6a6f77  3 Darkness
  // [14] 6       0x6b0000  2 Missile ff84 Open Door
  // [15] 1-5     0x667000  1 Magic Shield Potion(12) YA Potion
  // [16] 1       0x660000  1 Stamina Potion(11) MON Potion
  // [17] 1-5-3   0x667074  1 Wisdom Potion(8) DANE Potion
  // [18] 1-5-4   0x667075  1 Vitality Potion(9) NETA potion
  // [19] 2       0x670000  1 Potion(14) Cure Wounds - VI Potion
  // [20] 2-5     0x677000  1 Potion(10) Cure Poison - ANTIVENOM
  // [21] 3-5-2   0x687073  1 Dexterity Potion(6) ROS Potion
  // [22] 6-5-5   0x6b7076  1 Mana Potion(13) EE Potion
  // [23] 6-1     0x6b6c76  1 Poison Potion(3) VEN Potion
  // [24] 6-3-5   0x6b6e76  3 ZoKathRa
  i8  Byte19016[6];
  i8  Byte19010[24];
                       // Last part of graphic 0x230
  i8  Byte18938[2];      // Start of graphic 0x231
  btn Buttons18936[5];   // swapped when read
  btn Buttons18876[4];   //    ""      ...
  btn Buttons18828[3];   //    ""      ...
  btn Buttons18792[2];   //    ""      ...
  btn Buttons18768[5];   //    ""      ...
  btn Buttons18708[4];   //    ""      ...
  btn Buttons18660[3];   //    ""      ...
  btn Buttons18624[2]; //      ""    ...
  i8  Byte18600[8];    // RectPos swapped when read        ...
  i8  Byte18592[8];    // RectPos swapped when read        ...
  i8  Byte18584[8];    // RectPos swapped when read        ...
  i8  Byte18576[8];    // RectPos swapped when read        ...
  i16 Word18568[4];    // deltax[dir] NESW swapped when read        ...
  i16 Word18560[4];    // deltay[dir] NESW swapped when read         ...
  i16 Word18552[28];   // Swapped when read.  Magic buttons???         ...
  MOVEBUTN MoveButn18496[4]; // MF, MR, MB, ML Swapped when read
  ui8 DropAreas[16];//18464 X andY in screen coordinates
  i8  Byte18448[8];     // KeyXlate swapped when read        ...
  i8  Byte18440[12];    //  KeyXlate swapped when read        ...
  KeyXlate Byte18428[7];//  KeyXlate swapped when read        ...
  i8  Byte18400[28];    //  KeyXlate swapped when read
  btn Buttons18372[4];  //
  btn Buttons18324[9];  // Swapped when read... chest buttons
  btn Buttons18216[13]; // Swapped when read
  btn Buttons18060[9];  // Swapped when read
  btn Buttons17952[5];  // Swapped when read
  btn Buttons17892[5];  // Swapped when read
  btn Buttons17832[3];  // Swapped when read         ...
  btn Buttons17796[3];  // Swapped when read  [864-4*12]
  btn Buttons17760[38]; // Swapped when read
  btn Buttons17304[9];  // Swapped when read         ...
  btn Buttons17196[20]; // Swapped when read         ...
  btn Buttons16956[2];  // Swapped when read         ...
  btn Buttons16932[4];  // Swapped when read
                        // End of graphic 0x231
  i16 Word16868; // Mouse click something
  i16 FakeMouseButton;//16866; // Fake Mouse click buttons
  i16 FakeMouseY;     //16864; // Fake Mouse click y
  i16 FakeMouseX;     //16862; // Fake Mouse click x
  i16 FakeMouseClick; //16860; // Fake Mouse click occurred
    // Fake mouse clicks are also created when we need to
    // save a click while the Test/Set word is set.  The
    // click is then acted upon later.

    // Some sort of queue.
  i16 MouseInterlock;//16858; // Test/Set so we don't reference the queue twice.
  i16 MouseQEnd;  //16856; Last entry in queue (0 to MOUSEQLEN-1)
  i16 MouseQStart;//16854; Next entry to remove from queue (0 to MOUSEQLEN-1)
  //              // The queue is empty when Start = End+1
  MouseQueueEnt ExMouseQueue[5]; //Moved to global space to enlarge.//16852
  i8  Byte16702;
  FILL(16701,16634)
  RN  objOpenChest;//16634; The one opened and on display
  RN  rnChestContents[8];//16632;
            // Contents of opened chest.  Commonly
            // referenced as 16692[place] because
            // place number 30 is the start of the
            // chest.
  i16 DisplayResurrectChestOrScroll;// 16616;
                 // Set to 2 if scroll in weapon hand
                 // Set to 4 if chest in weapon hand
                 // Set to 5 during "Resurrect or Reincarnate"
                 // if ==4 then we search chest as well as backpack.
  i16 SelectedCharacterOrdinal;//16614; Selected to see backpack.
  i16 TextOutY; //Word16612;
  i16 TextOutX;
  i16 Word16608;
  i8  Byte16606[6];
  i32 Long16600;
  const char* Pointer16596[4];//"FIGHTER", "NINJA", etc
  i16 Word16580;
  i16 EmptyHanded;//16578;
  RN  objectInHand;//16576; //object in hand cursor?
  i16 ObjectType16574; // OBJECTTYPE//must say i16 to keep space use to 2 bytes
  pnt Pointer16572;
  i16 HandChar;// 16568; //Whose hand does that cursor belong to?
                 //Contains character index or -1
  i16 PendingOuches[4];//16566// Main loop 'or's these to characters' ouches
  i16 PendingDamage[4];//16558// Main loop subtracts from HP
  i8  Byte16550;
  FILL(16549,16482)
    // 3328 bytes read into byte16482 .... 16482-3328 = 13154
  CHARDESC CH16482[4];  //part of 16482
  i16 Brightness;  //13282;//part of 16482 //swapped when read
  i8  SeeThruWalls;//13280;//part of 16482
  i8  MagicFootprintsActive; //Byte13279;       //part of 16482
  i16 PartyShield;// Word13278; //part of 16482 //Party shield timer
  i16 FireShield;//Word13276;       //part of 16482 //Spell effect
  i16 SpellShield;//Word13274;       //part of 16482 //Spell effect
  i8  NumFootprintEnt;//13272;//part of 16482  #entries in 13268???
  ui8 freezeLifeTimer;// uByte13271;      //part of 16482 //Life frozen time
  i8  IndexOfFirstMagicFootprint; //Byte13270; //part of 16482  index of entry in 13268
  i8  IndexOfLastMagicFootprint;  //Byte13269; //part of 16482  index of entry in 13268
  i16 PartyFootprints[24];//Word13268[24];   //part of 16482 history of moves???
                          // bits 0-4 = mapX
                          // bits 5_9 = mapY
                          // bits 10_15 = level
  i8  Byte13220[24]; // parallels 13268??
                     // Is this a once-only kind of thing?
  i8  Invisible;     //13196;//part of 16482
  i8  Byte13195[41]; //part of 16482
  /////////////  End of Character portion of save file /////////////
  i16 DelayedActuatorActionPos;//13154; // -1 means position ignored?
  i16 DelayedActuatorActionY;//13152; //mapY
  i16 DelayedActuatorActionX;//13150; //mapX
  i16 DelayedActuatorAction;
  i16 SupressPitDamage;// 13146; //set to 1 when we climb rope
         // Perhaps this is to avoid damage from
         // falling into pit.  This gets cleared
         // after we fall one floor,
  i16 NewPos;  //13144; //objectPosition
                        //Set to new position by MoveObject
  i16 NewDir;  //13142;
                        //Set to new direction by MoveObject
  i16 NewLevel;//13140; //objectLevel
                        //Set to new level by MoveObject
  i16 NewY;    //13138; //Set to newY when party moves
                        //Set to newY by MoveObject
  i16 NewX;    //13136; //Set to newX when party moves.
                        //Set to newX by MoveObject
  ui8 uByte13126[8];
  i8  Byte13118;
  FILL(13117,13050)
  i8  Byte13050[4]; // stack
  i16 Word13046; // # entries in 13050
  i16 moveOntoPartyPosition;//Word13044;
  i16 Word13042;
  RN  Obj13040;
  i16 Word13038;
  i16 FluxCageCount; //Word13036;
  i8  FluxCages[4]; // for each direction.  Byte13034[4];
  i8  DirectionHasBeenTested[4]; //Byte13030[4]; // each directioon. (PossibleMove())
  i16 SecondaryDirectionToParty; //Word13026;
  i16 PrimaryDirectionToParty;//Word13024; //
  i16 OrthogonalDistance2Party; // Word13022; // used while computing monster movement
  RN  LastMonsterMoved; //Obj13020; //Any timer 29 to 41.
  i16 monsterY; //Word13018;
  i16 monsterX; //Word13016; Most recently processed monster movement timer
  i16 ITEM16QueLen;//13014;
  i16 MaxITEM16;   //13012;//#entries in 13010.  60 under some circumstances.
  private:
  i16 unused13002; //firstAvailTimer;//13002;
  i16 unused13000; //numTimer;//13000; // ++/-- counter
  i16 unused12990; //MaxTimers;//12990; //# items in Pointer12994 and 12998 array?
  public:
  //  i16 FirstAvailTimer(void){return firstAvailTimer;};
  //  void FirstAvailTimer(i32 v){firstAvailTimer=(i16)v;};
  //  void IncrementFirstAvailTimer(void){firstAvailTimer++;};
  //  i16 NumTimer(void) {return numTimer;};
  //  void NumTimer(i16 v) {numTimer=v;};
  //  void DecrementNumTimer(void){numTimer--;};
  //  void IncrementNumTimer(void){numTimer++;};
  //  TIMER *pTimer(i32 index) {return Timers + index;};
  //  i16 TimerQue(i32 index) {return timerQue[index];};
  //  void TimerQue(i32 index, i16 val){timerQue[index]=val;};
  //  i16 MaxTimer(void){return MaxTimers;};
  //  void MaxTimer(i32 v){MaxTimers=(i16)v;};
  pnt Pointer12988[6];

  i16 Word12964; // These two words set by
  i16 Word12962; // function that determines damage due to missile
                 // 12962 a function of damage???
                 // 12964 = 1, 3, 5, or 7???

  i16 unused12960;  //MissileDelay; // Word12960; 
                    // Shooter sets to 1 during launch so missile 
                    // will not stop before leaving the wall.
  i16 MonsterDamageResult; //0=none died; 1=some but not all died; 2=all died;Word12958;
  DIRECTION SecondaryDirection; //Dir12956; // Set to random 1 or 3 when bump into wall
  i16 PrintColumn;//12930; // Current scrolling text column number.
  i16 PrintRow;//12928; In scrolling area.  0 to 3
  i16 PushTextUp; //12918 used in vertical blank handler
  i16 TextScanlineScrollCount; //12916; //
           // This is the number of lines of Pointer12922
           // that have already been scrolled onto the
           // text area at the bottom of the screen.
           // Set to -1 if no text is in Pointer12922.
           // Set to 0 when a new text line needs to be scrolled into the window
  char Byte12914[128];
  //FILL(12913,12786)
  i16 VBLInterruptActive; //Word11990; // used in vertical blank handler
  i16 unused11988; //Word11988; // used in vertical blank handler
  PALETTE Palette11978;
  PALETTE Palette11946;  // Current palette???
  PALETTE Palette11914;  // Used as palette
  FILL(11882,11850)
  i8  Byte11850;
  FILL(11849,11782)
  i16 Word11778; // used in vertical blank handler
  i16 Word11776; // used in vertical blank handler
  i16 FlashButnActive; //Word11774;
  i16 FlashButnY2; //Word11772; //
  i16 FlashButnY1; //Word11770; //
  i16 FlashButnX2; //Word11768; //
  i16 FlashButnX1; //Word11766; //
  i16 Word11764; // Menu option result???
  i16 QuitPressingMouth;
  i16 PressingMouth; // Set when showing food/water
  i16 QuitPressingEye;
  i16 PressingEye;//Word11756; // Set when showing skill levels or viewing object with eye
  i16 unused11754; //Word11754;
  i16 Unused11752;//Word11752;
  i16 Word11750; // used in vertical blank handler
  i16 newPartyLevel; //Word11748;
  i16 ShowCursor1;         //Word11746;
  i16 ShowCursor16572; // Word11744;
  i16 ViewportUpdated; //11742;
                    // Set when new Viewport has been created.  VBLinterrupt
                    // then copies Viewport to screen and clears the flag.
  i16 Word11740;
  i16 DynamicPaletteSwitching;//11738;
          // During the adventuring state we change the
          // palette twice during each vertical scan.  The
          // portraits get Palette11978, the viewport and
          // buttons get Palette11946, and the text gets
          // Palette11978 again.
  i16 clockTick; //11736;// Every 'Speed' vbls.
                         // Also set by player actions .
  FILL(11734,11732)
  i32 Long11732; // compared to 11718 = Time.  Set equal to
                 // Time when a game is loaded.
  i16 Speed;     // 11728; Number of VBLs per "clockTick"(11736)
  i16 vblCount; //11726; // Increment by vbl interrupt.
                         // Cleared in main loop.
  i16 Unused11724; //Always zero.  I removed it.Word11724; // bit 0 important
  i16 Word11722; // bit 0 important
  i16 Unused11720; // Always zeroWord11720;
  i16 Word11714;   // Restored from Game Save file ; Direction of last THROW
  i16 Word11712;   // Restored from Game Save file ; Party movement disable timer after a 'THROW'
                                                // ; To prevent running into object thrown???
  i16 partyMoveDisableTimer; //Word11710;   // Restored from Game Save file ; Party movement disable timer
  i16 partyLevel;  // 11708  See TAG016e54
  i16 partyFacing; // 11706
  i16 partyY;      // 11704;
  i16 partyX;      // 11702
  i16 NumCharacter; //11700; // # characters????
  // 11698 seems to be an index into an array of palettes
  // in TAG00c202 this number is divided by 2.
  i16 CurrentPalette; //11698// used in vertical blank handler
                 // Palette to use based on light level
                 // (d.Brightness) plus torches
  i16 GameIsLost; //Word11696; // The game is lost.
                 //There are no living characters.
  i16 Word11694; //Set when game is won.
                 //This stops monster movement, prevents
                 //processing of Timer function 24, and
                 //stops any damage to characters.
  i16 ClockRunning;//11692
  i16 PartySleeping;//11690;
  i16 PotentialCharacterOrdinal;//11688; // Almost but not quite added to party
  i16 gameState; //Word11686; See enum GAMESTATE
  i16 Word11684; // Set when menu is showing???
                    // viewport
  i8  Byte11678;
  FILL(11677,11270)
  i16 Word11270;
  FILL(11268,10654)
  ui8 LevelCleared[16];//10654;//One per DBNum. Initially all zero.
                       //This is the level we most recently discared
                       //and object from when we needed room for
                       //additional objects.
  i8  Byte10638;
  FILL(10637,10574)
  RN  rn10574[5];// Is RN right??? Is 5 right???
  BUTTON_DEF ViewportObjectButtons[6];//10564;
                    // DrawViewport constructs a button for
                    // each object that can be clicked.
  RN  unused10540; //rn10540; //Replaced by entries in SUUMARIZEROOMDATA
                            //This used to conatain the wall text record
  i16 unused10538;//ChampionPortraitOrdinal;//Word10538;
  i16 FacingWaterFountain;//Word10536; Fountain immediately in front of party
  i16 FacingViAltar; //Word10534;
  i16 FacingAlcove;//Word10532 ; Alcove immediately in front of party.
  i16 CellTypeJustAhead;//10530;
                // Cell type of cell immediately ahead of party
                // Set by DrawCellF1 // Convert to/from ROOMTYPE
                // because we need it to be 16 bits.
  //DBCOMMON *misc10528[16];
  pnt unused10528[16];  // Placeholder.
  //This has been replaced by an instance of DATABASES named "db".
          // Array of pointers to miscellaneous data.
          // Each points to an array.  The dungeonDatIndex[i+6]
          // defines the number of entries read from dungeon.dat
          // and is updated to the total number of entries at runtime.
          // Byte7302[i] defines the number of extra entries
          // allocated for each array.  And Byte7286[i] defines
          // the (byte) size of each entry.  The empty entries
          // have word 0 set to -1.  Several of the arrays
          // have zero space allocated.
          //  0 -  4 +   0  4-byte entries  //Doors???
          //       word2 bit 0 = door condition???
          //  1 -  0 +   0  6-byte entries
          //  2 - 26 +   0  2-word entries both words swapped when read
          //                2nd word is Compressed Text ID.
          //  3 - 30 +   0  8-byte entries
          //       First guess.  Pressure pad????
          // Databases 0, 1, 2, and 3 cannot be drawn?????
          //  4 - 12 +  75 16-byte entries //Entries class DB4
          //       I guess type 4 is monsters.  Just a guess.
          //       ITEM16.word0 contains an index into this dataase.
          //  5 -  0 + 100  4-byte entries
          //       A torch seems to be in DB5.
          //       A sword
          //  6 -  0 + 120  4-byte entries
          //       A small Shield is in DB6
          //  7 -  0 +   0  4-byte entries
          //       Text (scroll???)
          //  8 -  0 +   5  4-byte entries
          //  9 -  0 +   0  8-byte entries // Chests???
          // 10 -  0 + 140  4-byte entries //BITS0_6(word[1])+127=weaponIndex???
          //                   corn        // word2 = 0x..85 bones; chIdx in bits 14,15
          // 11 -  0 +   0  0-byte entries
          // 12 -  0 +   0  0-byte entries
          // 13 -  0 +   0  0-byte entries
          // 14 -  0 +  60  8-byte entries //Entries class DB14
                                           //Fireball??? Missile???
          //                               //word 6 = timer index
          // 15 -  0 +  50  4-byte entries

  RN *objectList; //prn10464;
               // One word for each room containing some sort
               // of object.  10420 is a shortcut giving the
               // index of the first entry for each column of
               // the current level.  The word contains the
               // DBRecordName
               // of the first object in the room (an RN).
               // Points to array of words..  The first small
               // part of this aray is read from dungeon.dat
               // and 300 additional words are allocated and
               // filled with -1;
               // The size of this array is found in
               // dungeonDatIndex[5] (10446)
               // littleEndian applied when read from file.
               // Bits 10-13 are a field..4 is special
  ui16 numColumnPointers; //10460;// total in both halves of 10450
                         // All levels included.
                         // This includes a pointer to the first
                         // column of each level and a pointer for
                         // each column in each level.  So it is
                         // the total number of columns in the dungeon
                         // plus the number of levels.
  i16 DoorTOC[2];   //Word10434
                    // Characteristics of the two door types on this level.
                    // Bits 8-15 are flags
                    //   0x01 has to do with monster passage
                    //   0x02 has to do with missile encounters
                    //   0x04 causes 'lightning' door.  Random orientation.
                    // Low byte is strength of door.
                    // Copied from d.Word9760 which was swapped when read.
  i16 height;//10430 //Dungeon level height
  i16 width; //10428;//Dungeon level width
  i16 LoadedLevel;//10426; // Currently loaded level
  CELLFLAG **LevelCellFlags; //10424
         // This points to an array of pointers to the
         // first CELLFLAG in each column of the currently
         // loaded level.
         // You should generally reference it as:
         //   CELLFLAG cf = d.LevelCellFlags[x][y];
         //       *** NOTE ***
         // At the the end of the cellflags for each level there
         // are some tables that describe such things as
         // which wall decorations are used on the level,
         //  - monsters allowed on level
         //  - wall decorations used on level
         // what monsters are allowed on the level, etc.
         // See the function SetPartyLevel().
  ui16 *pCurrentLevelObjectIndex;
                       //pwPointer10420;
                       //for each column of the level there is a
                       // index of the first object-list in that
                       // column.
  LEVELDESC *pCurLevelDesc;//10416; // current Level descriptor. See 10442.
  i16 FountainGraphicIndex[1]; //Word10412[1];
  i16 AlcoveGraphicIndices[3]; //Word10410[3];  //The indicies on this level of alcove wall decorations
  i16 OrnateAlcoveGraphicIndex; //Word10404;     //The index on this level of the ornate alcove
  i16 NumWallDecoration;//10402;
  CELLFLAG *EndOfCELLFLAGS;//10400; // Points to last CELLFLAG +1;
  i8  DoorDecorationTOC[18];//Byte10396
  i8  FloorDecorationTOC[16];//Byte10378
  ui8 WallDecorationTOC[16];//10362[1];//For current level
                            //Input from dungeon.dat following cellflags
  i8  Byte10342[2];
  // *******************************************************
  // Start graphic 0x22f
  // *******************************************************
  char         Byte10340[4];
  char         Byte10336[256]; //       ...
  char         Byte10080[64];  //       ...
  char         Byte10016[256]; //       ...
  i16          DoorCharacteristics[4]; //9760// swapped when read.
  i16          MonsterDroppings[40];   //9752//
  ui8          uByte9672[8];   // Sound numbers???
  MONSTERDESC  MonsterDescriptor[27];//9664
  i16          FoodValue[8];//8962 // obj_Apple to obj_DragonSteak      ...
                // swapped when read
  i8           Byte8946[54];   //
  i16          Unused8892;       //       ...
  CLOTHINGDESC ClothingDesc[58]; //8890
  WEAPONDESC   weapons[46];//8658   // Used for Database5 objects
                 // word4 swapped when read;
                 // pDB5->weaponType() is index
  OBJDESC      ObjDesc[180];//8382
                 // -object Type
                 // -effect of objects when attacking?
                 // -Words 0 and 4 swapped when read
                 // -
                 // -Word4 bit 0 = consumable??
                 // -indexed by Object Index
                 //    = scroll type   +  0 //  1 Scroll type
                 //    = chest type    +  1 //  1 Chest types
                 //    = potion type   +  2 // 21 Potion types
                 //    = weapon type   + 23 // 46 Weapon types
                 //    = clothing type + 69 // 58 Clothing types
                 //    = food type     +127 // 53 Misc types

  i8           Byte7302[16];   //  #extra entries in each 'misc' database     ...
  i8           unusedDBEntrySize[16];//7286 // Size of entries in each 'misc' database     ...
                        // Always an even number! Sometimes zero.
  i16          DeltaY[4]; //7270  // deltay for 4 directions
  i16          DeltaX[4]; //7262    // deltax for 4 directions End of 22f
                 // deltax and deltay littleEndian when expanded
  i16          Word7254;
  i16          CurrentWallGraphic;//Word7252; // Some sort of graphic number????
  i16          CurrentFloorAndCeilingGraphic; //Word7250;
  i8           Byte7248[2];
  //*******************************************************
  //              Start of graphic 0x22e
  //*******************************************************
  i16 Word7246[12];  // Swapped when read
  i16 Word7222[30];  // Swapped when read
  i16 Word7162[60];  // Swapped when read
  i16 Word7042[30];  // Swapped when read
  ITEM110 s6982[3]; // Three groups of 110
  i8  smallOffsets[24];//Byte6652[24];  // +/- small integers to shift image (three distances and 4 offsets)
  ui8 Byte6628[16]; //Color mapping
  ui8 Byte6612[16]; //Colors map for objects three squares ahead
  i16 specialColors[13][7];//Word6596
  ITEM12 Item6414[27];//      ...
  VIEWPORTPOS viewportPosition[3]; //s6090
                     //      ... 100-byte entries each with 10-byte entries
                     // Nominal positions in viewport for
                     //  [0] Missiles ??
                     //  [1] A sword on floor pointed here
                     //  [2] ???
                     // Each 100-byte entry has 10 entries, one for each
                     // relative room number (9=room party is in).  And each
                     // 10-byte entry has 5 2-byte entries, one for each relative
                     // position within the room and one for ??? (object in niche?).
                     //   [0] = far left
                     //   [1] = far right
                     //   [2] = near right
                     //   [3] = near left
                     //   [4] = niche??
  ui8 smallOffsetIndicies[16][2]; //uByte5790[16][2]; // index to smallOffsets for x and y directions
  i8  Byte5758[6];   //
  i8  Byte5752[8];   //      ...
  ui8 Byte5744[16];  //ColorMap      ...
  ui8 Byte5728[16];  //ColorMap      ...
  ui8 Byte5712[16];  //
  i8  Byte5696[8];   //      ...
  STRUCT5688 struct5688[14];//i8  Byte5688[84];  // You must littleEndian these.. (14) 6-byte entries
  GRAPHIC_CLASS s5604[85];   //      ... (85) 6-byte entries
  i8  Byte5094[6];   //      ...
  i8  Byte5088[12];  //      ...
  i8  Byte5076[6];   //      ...
  i8  Byte5070[12];  //      ... 6-byte RectPos for doors?
  i8  Byte5058[60];  //      ... 18-byte entries????
  //i8  Byte4998[3][9][6]; // 6-byte entries; each 6-byte entry
  DECORATION_DESC floorDecorDesc[3][9];
                         // consists of
                         // 4-byte rectpos
                         // 1-byte (width)
                         // 1-byte (height)
                         // [3] for 3 different ???
                         // [9] for 9 different relative room positions
                         // [6] for rectpos + width + height
  DECORATION_DESC wallDecorationPosition[8][13]; //  Byte4836[8][13][6];  //      ...
  //i8  Byte4776[564]; //      ... 78-byte entries?
  i8  Byte4212[16];  //      ...
  i8  Byte4196[4];   //      ...
  i8  Byte4192[4];   //      ...
  ui8 Byte4188[16];  // ColorMap     ...
  ui8 Byte4172[16];  // ColorMap     ...
  ui8 MediumColorMapping[16]; //Byte4156[16];  //      ...
  ui8 FarColorMapping[16];    //Byte4140[16];  //      ...
  i8  Byte4124[2];   //      ...
  i8  Byte4122[12];  //      ...
  i8  Byte4110[10];  //      ...
  i8  Byte4100[60];  // For each of 60 possible wall decorations this is the
                     // index of the associated screen positioning parameters...
  i16 Word4040[1];   //      ...
  i8  Byte4038[4];   //      ... These are wall graphics that define
                     //          alcoves.  The numbers here are 1, 2, and 3
                     //          in CSB.  Square Alcove, VI Altar, Ornate Alcove.
  //i8  Byte4034[10];  //      ...
  ui8 RelativeCellGraphicIncrement[10]; //Byte4034
                                    //Graphic number increment for
                                    //each relative cell# (0 to 9)
  i8  WallDecorationDerivedGraphicOffset[12]; // For all but relative position 12 //Byte4024[12];  //      ...
  i16 Word4012;      //      ...
  RectPos teleporterRectangles[12];
  //i8  Byte4010[8];   //      ...[0] teleporter F3
  //i8  Byte4002[8];   //      ...[1] teleporter F3L1
  //i8  Byte3994[8];   //      ...[2] teleporter F3R1 //Bad????
  //i8  Byte3986[8];   //      ...[3] teleporter F2
  //i8  Byte3978[8];   //      ...[4] teleporter F2L1
  //i8  Byte3970[8];   //      ...[5] teleporter F2R1
  //i8  Byte3962[8];   //      ...[6] teleporter F1
  //i8  Byte3954[8];   //      ...[7] teleporter F1L1
  //i8  Byte3946[8];   //      ...[8] teleporter F1R1
  //i8  Byte3938[8];   //      ...[9] teleporter F0
  //i8  Byte3930[8];   //      ...[10]teleporter F0L1
  //i8  Byte3922[8];   //      ...[11]teleporter F0R1
  RectPos DoorRectsF1R1[10];
  //i8  Byte3914[80];  //      ...//F1R1 // 10 RectPos 1-closed 3-up 3-left 3-right
  RectPos DoorRectsF1[10];
  //i8  Byte3834[80];  //      ...//F1
  RectPos DoorRectsF1L1[10];
  //i8  Byte3754[80];  //      ...//F1L1
  RectPos DoorRectsF2R1[10];
  //i8  Byte3674[80];  //      ...//F2R1
  RectPos DoorRectsF2[10];
  //i8  Byte3594[80];  //      ...//F2
  RectPos DoorRectsF2L1[10];
  //i8  Byte3514[80];  //      ...//F2L1
  RectPos DoorRectsF3R1[10];
  //i8  Byte3434[80];  //      ...//F3R1
  RectPos DoorRectsF3[10];
  //i8  Byte3354[80];  //      ...//F3
  RectPos DoorRectsF3L1[10];
  //i8  Byte3274[80];  //      ...//F3L1
  //
  RectPos DoorTrackTopRect[7];
  //i8  Byte3194[8];   // RectPos  ...[0]F1R1
  //i8  Byte3186[8];   // RectPos  ...[1]F1
  //i8  Byte3178[8];   // RectPos  ...[2]F1L1
  //i8  Byte3170[8];   // RectPos  ...[3]F2R1
  //i8  Byte3162[8];   // RectPos  ...[4]F2
  //i8  Byte3154[8];   // RectPos  ...[5]F2L1
  //i8  Byte3146[8];   // RectPos  ...[6]F0

  RectPos DoorFrameRect[8];
  //i8  Byte3138[8];   // RectPos  ...[0] 1/2 of F1 (right)
  ////i8  Byte3134;      //      ...
  ////i8  Byte3133;      //      ...
  ////i8  Byte3132[2];   //      ...
  //i8  Byte3130[8];   // RectPos  ...[1] 1/2 of F1 Frame (left)
  //i8  Byte3122[8];   //          ...[2] 1/2 of F2 Frame (right)
  //i8  Byte3114[8];   // RectPos  ...[3] 1/2 of F2 Frame (left)
  //i8  Byte3106[8];   //          ...[4] 1/2 of F3 Frame (right)
  //i8  Byte3098[8];   // RectPos  ...[5] 1/2 of F3 Frame (left)
  //i8  Byte3090[8];   //          ...[6] F3R1 Frame
  //i8  Byte3082[8];   // RectPos  .. [7] F3L1 Frame

  RectPos wallRectangles[14];
  //i8  Byte3074[8];   // RectPos  .../[0]Walls F3
  //i8  Byte3066[8];   // RectPos  .../[1]Walls F3L1
  //i8  Byte3058[8];   // RectPos  .../[2]Walls F3R1
  //i8  Byte3050[8];   // RectPos  .../[3]Walls F2
  //i8  Byte3042[8];   // RectPos  .../[4]Walls F2L1
  //i8  Byte3034[8];   // RectPos  .../[5]Walls F2R1
  //ui8 uByte3026[8];  // RectPos  .../[6]Walls F1
  //i8  Byte3018[8];   // RectPos  .../[7]Walls F1L1
  //i8  Byte3010[8];   // RectPos  .../[8]Walls F1R1
  //i8  Byte3002[8];   //      ...    /[9]Local Cell
  //ui8 uByte2994[8];  // RectPos  .../[10]Walls F0L1
  //i8  Byte2986[8];   // RectPos  .../[11]Walls F0R1
  //i8  Byte2978[8];   // RectPos  .../[12]Walls F3R2
  //i8  Byte2970[8];   // RectPos  .../[13]Walls F3L2
  i8  Byte2962[4];   // byte RectPos     ...
  i8  Byte2958[4];   // byte Rectpos ...
  RectPos CeilingPit[9]; //right-to-left then front-to-back
  //i8  Byte2954[8];   //byte RectPos      ...[0]Ceiling Pit F0R1
  //i8  Byte2946[8];   //byte RectPos      ...[1]Ceiling Pit F0
  //i8  Byte2938[8];   //byte RectPos      ...[2]Ceiling Pit F0L1
  //i8  Byte2930[8];   //byte RectPos      ...[3]Ceiling Pit F1R1
  //i8  Byte2922[8];   //byte RectPos      ...[4]Ceiling Pit F1
  //i8  Byte2914[8];   //byte RectPos      ...[5]Ceiling Pit F1L1
  //i8  Byte2906[8];   //byte RectPos      ...[6]Ceiling Pit F2R1
  //i8  Byte2898[8];   //byte RectPos      ...[7]Ceiling Pit F2
  //i8  Byte2890[8];   //byte Rectpos      ...[8]Ceiling Pit F2L1
  RectPos FloorPitRect[12]; // right-to-left then front to back
  //i8  Byte2882[8];   //RectPos (byte)      ...[0]Floor Pit F0R1
  //i8  Byte2874[8];   //RectPos (byte)      ...[1]Floor Pit F0
  //i8  Byte2866[8];   //RectPos (byte)      ...[2]Floor Pit F0L1
  //i8  Byte2858[8];   //RectPos (byte)      ...[3]Floor Pit F1R1
  //i8  Byte2850[8];   //RectPos (byte)      ...[4]Floor Pit F1
  //i8  Byte2842[8];   //RectPos (byte)      ...[5]Floor Pit F1L1
  //i8  Byte2834[8];   //RectPos (byte)      ...[6]Floor Pit F2R1
  //i8  Byte2826[8];   //RectPos (byte)      ...[7]Floor Pit F2
  //i8  Byte2818[8];   //RectPos (byte)      ...[8]Floor Pit F2L1
  //i8  Byte2810[8];   //RectPos (byte)      ...[9]Floor Pit F3R1
  //i8  Byte2802[8];   //RectPos (byte)      ...[10]Floor Pit F3
  //i8  Byte2794[8];   //RectPos (byte)      ...[11]Floor Pit F3L1
  RectPos StairEdgeRect[8];
  //i8  Byte2786[8];   //RectPos (byte)      ...[0]Stair Edge F0R1
  //i8  Byte2778[8];   //RectPos (byte)      ...[1]Stair Edge F0L1 
  //i8  Byte2770[8];   //RectPos (byte)      ...[2]Stair Edge F1R1 Down
  //i8  Byte2762[8];   //RectPos (byte)      ...[3]Stair Edge F1L1 Down
  //i8  Byte2754[8];   //RectPos (byte)      ...[4]Stair Edge F1R1 Up
  //i8  Byte2746[8];   //RectPos (byte)      ...[5]Stair Edge F1L1 Up
  //i8  Byte2738[8];   //RectPos (byte)      ...[6]Stair Edge F2R1
  //i8  Byte2730[8];   //RectPos (byte)      ...[7]Stair Edge F2L1
  RectPos StairFacingDownRect[11];
  //i8  Byte2722[8];   //RectPos (byte)      ...[0]Stair Facing F0 (L or R?)
  //i8  Byte2714[8];   //RectPos (byte)      ...[1]Stair Facing F0 (L or R?)
  //i8  Byte2706[8];   //RectPos (byte)      ...[2]Stair Facing F1R1
  //i8  Byte2698[8];   //RectPos (byte)      ...[3]Stair Facing F1
  //i8  Byte2690[8];   //RectPos (byte)      ...[4]Stair Facing F1L1
  //i8  Byte2682[8];   //RectPos (byte)      ...[5]Stair Facing F2R1
  //i8  Byte2674[8];   //RectPos (byte)      ...[6]Stair Facing F2
  //i8  Byte2666[8];   //RectPos (byte)      ...[7]Stair Facing F2L1
  //i8  Byte2658[8];   //RectPos (byte)      ...[8]Stair Facing F3R1
  //i8  Byte2650[8];   //RectPos (byte)      ...[9]Stair Facing F3
  //i8  Byte2642[8];   //RectPos (byte)      ...[10]Stair Facing F3L1
  RectPos StairFacingUpRect[11];
  //i8  Byte2634[8];   //RectPos (byte)      ...[0]Stair Facing F0
  //i8  Byte2626[8];   //RectPos (byte)      ...[1]Stair Facing F0
  //i8  Byte2618[8];   //RectPos (byte)      ...[2]Stair Facing F1R1
  //i8  Byte2610[8];   //RectPos (byte)      ...[3]Stair Facing F1
  //i8  Byte2602[8];   //RectPos (byte)      ...[4]Stair Facing F1L1
  //i8  Byte2594[8];   //RectPos (byte)      ...[5]Stair Facing F2R1
  //i8  Byte2586[8];   //RectPos (byte)      ...[6]Stair Facing F2
  //i8  Byte2578[8];   //RectPos (byte)      ...[7]Stair Facing F2L1
  //i8  Byte2570[8];   //RectPos (byte)      ...[8]Stair Facing F3R1
  //i8  Byte2562[8];   //RectPos (byte)      ...[9]Stair Facing F3
  //i8  Byte2554[8];   //RectPos (byte)      ...[10]Stair Facing F3L1
  i8  Byte2546[4];   //
  ui8 uByte2542[4];  //Rectpos (byte)
  RectPos SeeThruWallsRect; //i16 Word2538[2];   //      ... rectpos (byte)
                            //ui8 uByte2534[4];   //      ... rectpos?
  //i16 Word2532[1];   //
  i16 Word2530[2];   // last part of graphic at 7246
//********************************************************
//  End graphic 0x22e
//********************************************************
  i16 Word2514[1][2];
  FILL(2510,2446)
  //i16 Word2446[16][2];  
  i16 LoadedFloorDecorations[16][2]; //Word2446
                        // Fifteen 2-word entries for floor decorations
                        // Plus one 2-word entry for footprints
                        // First word is graphic number of first
                        // graphic for this decoration.  There are
                        // generally several graphics....for the 
                        // various relative cell numbers.
                        // Second word is FLOOR_DECOR_DESC index
  //FILL(2445,2386)
  //i16 Word2386;
  //i16 Word2384;
  i16 WallDecorationsForLevel[16][2]; 
                      //[0] = Graphic number of first of two graphics for each
                      //      of up to 16 decorations.
                      //[1] = 
                      //Wall decoration graphic numbers start at 121
                      //Word2382[16][2]; //Wall decorations for loaded level.
                      // One per BITS0_3(d.pCurLevelDesc[0].word10)
          
  i16 Word2318[34];
  //FILL(2316,2250)


  i16 Word2210[34];


  i16 Word2050[34];
//  FILL(2048,1982)
  i8  Byte1982;
  FILL(1981,1918)
  i16 DoorGraphic[3][2]; // Two door types at three distances
  //i16 Word1918[2]; //[0]Door F1R1 & F1 & F1L1
  //i16 Word1914[2]; //[1]Door F2L1 & F2 & F2R1 
  //i16 Word1910[2]; //[2]Door F3L1 & F3 & F3R1
  i16 StairEdgeGraphic[4];
  //i16 Word1906;//[0]Stair Edge F0L1 & F0R1
  //i16 Word1904;//[1]Stair Edge F1R1 & F1L1 Down
  //i16 Word1902;//[2]Stair Edge F1R1 & F1L1 Up
  //i16 Word1900;//[3]Stair Edge F2L1 & F2R1
  i16 StairFacingDownGraphic[7];
  //i16 Word1898;//[0]F0          Stair Facing Down
  //i16 Word1896;//[1]F1          Stair Facing Down
  //i16 Word1894;//[2]F1R1 & F1L1 Stair Facing Down
  //i16 Word1892;//[3]F2          Stair Facing Down
  //i16 Word1890;//[4]F2L1 & F2R1 Stair Facing Down
  //i16 Word1888;//[5]F3          Stair Facing Down
  //i16 Word1886;//[6]F3R1 & F3L1 Stair Facing Down
  i16 StairFacingUpGraphic[7];
  //i16 Word1884;//[0]F0          Stair Facing Up
  //i16 Word1882;//[1]F1          Stair Facing Up
  //i16 Word1880;//[2]F1R1 & F1L1 Stair Facing Up
  //i16 Word1878;//[3]F2          Stair Facing Up
  //i16 Word1876;//[4]F2L1 & F2R1 Stair Facing Up
  //i16 Word1874;//[5]F3          Stair Facing Up
  //i16 Word1872;//[6]F3R1        Stair Facing Up
  i16 Word1870; // A Constant 1 IMHO.  Set to 1 by TAG00456c
  i16 Word1868;
  i16 UseMirrorImages; //1866;
  ui8 *Pointer1864[1]; // ColorMaphow many???
  ui8 *Pointer1860; //ColorMap
  ui8 *Pointer1856; //ColorMap
  ui8 *Pointer1852; //ColorMap
  pnt unused1848; //pnt Pointer1848; // 8880 Bytes. Temp during draw viewport
  i16 Word1844;
  i16 Word1842;
  i16 Word1840;
  i16 Unused1838;//Word1838;
  i16 Word1836; // Set to 15 by TAG00456c
  i16 Unused1834;//Word1834;
  i8  Byte1832[2];
// ***********************************************************************
//           // Start of graphic 0x232
// ***********************************************************************
  i8  Byte1830[2];
  i32 Long1828[4]; // masks for text !not reversed!
  i32 Long1812[4]; // masks for text !not reversed!
  RectPos wRectPos1796;   // swapped when read       ...
  RectPos wRectPos1788;   // swapped when read      ...
  RectPos wRectPos1780;   // swapped when read      ...
  SOUND sound1772[22]; //        ...
  i8  Byte1596[8];
  i16 Word1588;      //
  i16 DropOrder[30];//1586 //Order in which possessions are dropped
  i16 Word1526[4][3];   // swapped when read
  i16 Word1502[4][6];   // swapped when read
  RectPos wRectPos1454[4];  // RectPos swapped when read/facing
  i8  SpecialChars[6]; //1422 comma,period,semi-colon,colon,space;
  i8  Byte1416[2];
  i8  Byte1414[2];
  i8  Byte1412[6];
  i16 Word1406[4];   // Rectpos swapped when read
  i16 Word1398[4];   // RectPos swapped when read
  i8  Byte1390[4];   //
  i8  Byte1386[4];   //
  ui8 Byte1382[16];  //ColorMap
  ui8 Byte1366[16];  //ColorMap
  i8  Byte1350[128]; //        ...
  i8  Byte1222[128]; // Arrow cursor...
  i8  Byte1094[8];   // RectPos swapped when read
  i16 PaletteBrightness[6];//1086;   // swapped when read
                     // Value of Brightness for each palette.
  i16 Word1074[16];  // swapped when read       ...
                     // [2] added to 13282 when we put a
                     // Illumulet necklace on a character
  i16 CarryLocation[38];//1042 //swapped when read
                     // Word for each location that
                     // an object can be carried.
                     // See HandleClothingClick for a
                     // list of the locations and their
                     // numbers.  For example, not all
                     // objects can be put in all locations.
                     // You cannot put shoes on your head!
                     // There are 30 locations for body and
                     // backpack and 8 for the chest.
  RectPos wRectPos966;// swapped when read
  RectPos wRectPos958;// swapped when read
  RectPos wRectPos950;// swapped when read
  RectPos wRectPos942;// swapped when read
  RectPos wRectPos934;// swapped when read
  RectPos wRectPos926;// swapped when read
  i16 Unused918;//Word918;        //
  ICONDISPLAY IconDisplay[46]; //916// swapped when read
                      // 46 entries.
                      //   8 for hands in portraits
                      //  30 for clothing/backpack
                      //   8 for chest
                      //
  i8  Byte640[1];     //        ...
  FILL(639,624)       //
  RectPos wRectPos624;// Swapped when read
  FILL(616,612)       //
  i16 Word612[7];     // Swapped when read
                      // The various objects in the game
                      // have bitmaps grouped in larger
                      // graphics.  This array tells the
                      // first OBJECTTYPE in each of the
                      // seven graphics.
  FILL(598,590)
  ui8 uByte590[4];    //
  i8  Byte586[8][4];// Function of direction facing/moving?
  i16 unused554; //Word554;       // Swapped when read       ...
  PALETTE Palette552[6]; // Swapped when read  ...
//  i8  Byte520[160];  //        ...
  PALETTE Palette360;   // Palette...// Swapped when read from file
  PALETTE Palette328;   // Palette...// Swapped when read from file
  ui16 DefaultGraphicList[70];  //uw296  //  Bytes swapped when read from file      ...
  ui8 IdentityColorMap[16];   //        ... 16 bytes = { 0, 10, 20, 30, ...}
  i16 Word140[4];    // RectPos    We swapped the bytes immediately after reading from file
  i16 Word132[4];    // RectPos             "
  i16 Word124[4];    //              "
  i16 Word116[4];    //              "
  i16 Word108[4];    //              "
  i16 Word100[4];    // RectPos      "
  i16 Word92[4];     // RectPos      "
  i16 Word84[4];     // RectPos      "
  i16 Word76[4];     // RectPos      "
  i16 Word68[4];     // RectPos      "
  i16 Word60[4];     // RectPos      "
  i16 Word52[4];     // RectPos      "
  i16 Word44[4];     // RectPos      "
  i16 Word36[4];     // RectPos      "
  i16 Word28[4];     // RectPos?     "
  i16 Word20[4];     //              "
  i16 Word12[4];     // Rectpos
                     // End of graphic 0x232
  pnt Pointer4;







};


//extern i32 D0,D1,D2,D3,D4,D5,D6,D7;
//extern pnt A0,A1,A2,A3,A4,A5,A6,A7;


extern DBank d;
extern DATABASES db;

extern i32 data000b2c;
extern i32 data000b30;
//extern i16 data009032[];


extern bool verticalIntEnabled;

//#define NoKeyQue
#ifdef NoKeyQue
#define MOUSEQLEN 3
#else
#define MOUSEQLEN 8
#endif


extern MouseQueueEnt *pMouseQueue; //Moved to global space to enlarge.//16852

class TIMER_SEARCH;

class GameTimers
{
  friend class TIMER_SEARCH;
  friend RESTARTABLE _ReadEntireGame(void);
//  friend void AdjustTimerPosition(i32 timerPosition);
//  friend void DeleteTimer(i32 timerIndex);
//  friend i16 SetTimer(TIMER *pNewTimer);
  friend void DumpTimers(FILE *f);
  friend RESTARTABLE _DisplayDiskMenu(void);
  friend i16 ReadDatabases(void);
  HTIMER  *m_timerQueue;
  HTIMER  *m_searchList;
  TIMER *m_timers;
  ui16   m_numTimer;
  ui16   m_maxTimers; //m_size;
  HTIMER m_firstAvailTimer;
  //i16    m_MaxTimersLoading;
  ui16   m_timerSequence;
  bool   m_searchActive;
  //i16    m_FirstAvailTimerLoading;
  //i16    m_numTimersLoading;
public:
  GameTimers(void);
  ~GameTimers(void);
  void InitializeTimers(void);
  void Cleanup(void);
  void Allocate(i32 numEnt);
  void ConvertToSequencedTimers(void);
  void ConvertToExtendedTimers(void);
  void GetNextTimerEntry(TIMER *P1, ui32 *index);
  bool CheckForTimerEvent(void);
private:
  inline i32 NumTimer(void){return m_numTimer;};
  inline void NumTimer(i32 numTimer){m_numTimer=(i16)numTimer;};
  //void DecrementNumTimer();
  //void IncrementNumTimer();
  i16 FindTimerPosition(HTIMER P1);
  //void AdjustTimerPosition(i32 timerPosition);
  bool CheckTimers(void);
  ui16 TimerSequence(void)const{return m_timerSequence;};
  void TimerSequence(ui16 sequence){m_timerSequence = sequence;};
  inline HTIMER FirstAvailTimer(void){return m_firstAvailTimer;};
  inline void FirstAvailTimer(HTIMER firstAvailTimer){m_firstAvailTimer=(HTIMER)firstAvailTimer;};
  //void IncrementFirstAvailTimer(void);
  //void TimerQueue(i32 index, HTIMER value);
  ui16 *TimerQueue(void){return m_timerQueue;}; // Point to first word of timer queue
  //TIMER *Timers(void){return m_timers;}; // {return d.Timers;};
  inline i32 MaxTimer(void) const {return m_maxTimers;};
  i32 CreateSearchList(void); // Returns length of list.
public:
  //void AdjustTimerPriority(TIMER_SEARCH& timerSearch);
  void AdjustTimerPriority(HTIMER hTimer);
  void AdjustTimerQueue(i32 timerQueueIndex);
  /*inline*/ TIMER *pTimer(HTIMER i); //{return m_timers+i;};
  //void DeleteTimer(TIMER_SEARCH& timerSearch);
  void DeleteTimer(HTIMER timerIndex, const char *tag = NULL);
  HTIMER SetTimer(TIMER *pNewTimer);
  void swapTimerQue(void)
  {
    for (i32 i=0; i<m_maxTimers; i++)
      m_timerQueue[i]=LE16(m_timerQueue[i]);
  };
  void Clear(void){m_numTimer=0; m_firstAvailTimer=0;};
  //i16 MaxTimersLoading(void){return m_MaxTimersLoading;};
  //i16 FirstAvailTimerLoading(void){return m_FirstAvailTimerLoading;};
  //i16 numTimersLoading(void){return m_numTimersLoading;};
  //void MaxTimersLoading(i32 v){m_MaxTimersLoading=(i16)v;};
  //void FirstAvailTimerLoading(i32 v){m_FirstAvailTimerLoading=(i16)v;};
  //void numTimersLoading(i32 v){m_numTimersLoading=(i16)v;};
};

extern GameTimers gameTimers;


class TIMER_SEARCH
{
private:
  i32 m_index;
  i32 m_numEntry;
  i16 FindQPos(void);
public:
  TIMER_SEARCH(void){m_index=-1;m_numEntry=gameTimers.CreateSearchList();};
  ~TIMER_SEARCH(void){gameTimers.m_searchActive=false;};
  bool FindNextTimer(void);
  inline TIMER *TimerAddr(void)const{return gameTimers.pTimer(gameTimers.m_searchList[m_index]);};
  inline HTIMER TimerHandle(void) const {return gameTimers.m_searchList[m_index];};
  void AdjustTimerPriority(void);
  void DeleteTimer(void);
  //inline ui16 TimerQueueIndex(void) const {return m_timerQueueIndex;};
};




#define NUMOLDLINE 20
#define MAXLINEWIDTH 80
#define NUMFUTURELINE 99

class PIECE_OF_TEXT
{
  friend class LINEQUEUE;
private:
  PIECE_OF_TEXT *m_next;
public:
  i32            m_row;
  i32            m_col;
  i32            m_color;
  i32            m_printLinesCount;
  //char text[];
};

class LINEQUEUE
{
private:
  i32 m_maxLines;
  PIECE_OF_TEXT *m_firstText, *m_lastText;
public:
  LINEQUEUE(i32 maxLines);
  ~LINEQUEUE(void);
  void Cleanup(void);
  void AddText(i32 row, i32 column, i32 color, const char *text, i32 printLinesCount);
  bool IsEmpty(void){return m_firstText==NULL;};
  PIECE_OF_TEXT *Peek(void){return m_firstText;};
  void DiscardFirst(void);
  i32 MaxPrintLinesCount(void);
};

class SCROLLING_TEXT
{
private:
  LINEQUEUE m_pastLines;
  LINEQUEUE m_currentLines;
  LINEQUEUE m_futureLines;
  void ScrollUp(void);
  void RemoveTimedOutText(void);
  i32       m_printLinesCount[4];
public:
  SCROLLING_TEXT(void);
  ~SCROLLING_TEXT(void);
  void Cleanup(void);
  void Serialize(FILE *f, bool saving); // saving versus loading
  void vbl(void);
  void ClockTick(void);
  void ResetTime(i32 newTime);
  void SetPrintPosition(i32 column, i32 row);
  void Printf(i32 color, const char* text, i32 printLinesCount);
  void CreateNewTextRow(i32 printLinesCount);
  void SetNewSpeed(i32 oldSpeed, i32 newSpeed);
  void DiscardText(void);
};

extern SCROLLING_TEXT scrollingText;

//********************************************//(WCS/RCS)
// A few functions to "Write/Read w/Check Sum"//(WCS/RCS)
ui32 WCS(i32 handle);                         //(WCS/RCS)
ui32 RCS(i32 handle);                         //(WCS/RCS)
void WCS(char *, i32 count=1);                //(WCS/RCS)
void RCS(char *, i32 count=1);                //(WCS/RCS)
void WCS(ui16 *, i32 count=1);                //(WCS/RCS)
void RCS(ui16 *, i32 count=1);                //(WCS/RCS)
void WCS(ui8 *, i32 count=1);                 //(WCS/RCS)
void RCS(ui8 *, i32 count=1);                 //(WCS/RCS)
void WCS(i32 *, i32 count=1);                 //(WCS/RCS)
void RCS(i32 *, i32 count=1);                 //(WCS/RCS)
void WCS(ui32 *, i32 count=1);                //(WCS/RCS)
void RCS(ui32 *, i32 count=1);                //(WCS/RCS)
//********************************************//(WCS/RCS)
//
//********************************************//DSA
enum DSACOMMAND                               //DSA
{                                             //DSA
  DSACMD_MESSAGE          = 1,                //DSA
  DSACMD_OVERRIDE         = 2,                //DSA
  DSACMD_NOOP             = 3,                //DSA
  DSACMD_COPYTELEPORTER   = 4,                //DSA
  DSACMD_GOSUB            = 5,                //DSA
  DSACMD_LOAD             = 6,                //DSA
  DSACMD_FETCH            = 7,                //DSA
  DSACMD_EQUAL            = 8,                //DSA
  DSACMD_QUESTION         = 9,                //DSA
  DSACMD_RANDOM           =10,                //DSA
  DSACMD_AMPERSAND        =11,                //DSA
  DSACMD_JUMP             =12,                //DSA
  DSACMD_STORE            =13,                //DSA
  DSACMD_COPYTELEPORTER32 =14,                //DSA
  DSACMD_MESSAGE32        =15,                //DSA
  DSACMD_CASE             =16,                //DSA
  DSACMD_VARIABLEFETCH    =17,                //DSA
  DSACMD_VARIABLESTORE    =18,                //DSA
  DSACMD_GLOBALFETCH      =19,                //DSA
  DSACMD_GLOBALSTORE      =20,                //DSA
  DSACMD_AMPERSAND2       =21,                //DSA
  DSACMD_DESSAGE32        =22,                //DSA
};                                            //DSA
                                              //DSA
enum STKOP                                    //DSA
{                                             //DSA
    STKOP_Noop                        =  0,   //DSA
    STKOP_Plus                        =  1,   //DSA
    STKOP_Roll                        =  2,   //DSA
    STKOP_Pick                        =  3,   //DSA
    STKOP_Neg                         =  4,   //DSA
    STKOP_Equal                       =  5,   //DSA
    STKOP_Poke                        =  6,   //DSA
    STKOP_Drop                        =  7,   //DSA
    STKOP_Type                        =  8,   //DSA
    STKOP_Del                         =  9,   //DSA
    STKOP_Add                         = 10,   //DSA
    STKOP_Dup                         = 11,   //DSA
    STKOP_1Minus                      = 12,   //DSA
    STKOP_Swap                        = 13,   //DSA
    STKOP_1Plus                       = 14,   //DSA
    STKOP_Over                        = 15,   //DSA
    STKOP_MinusRoll                   = 16,   //DSA
    STKOP_Rot                         = 17,   //DSA
    STKOP_MinusRot                    = 18,   //DSA
    STKOP_And                         = 19,   //DSA
    STKOP_Shift                       = 20,   //DSA
    STKOP_Comp                        = 21,   //DSA
    STKOP_Or                          = 22,   //DSA
    STKOP_2Dup                        = 23,   //DSA
    STKOP_Slash                       = 24,   //DSA
    STKOP_Percent                     = 25,   //DSA
    STKOP_SetNewState                 = 26,   //DSA
    STKOP_Less                        = 27,   //DSA
    STKOP_2Drop                       = 28,   //DSA
    STKOP_NotEqual                    = 29,   //DSA
    STKOP_2Pick                       = 30,   //DSA
    STKOP_RShift                      = 31,   //DSA
    STKOP_Not                         = 32,   //DSA
    STKOP_FalsePit                    = 33,   //DSA
    STKOP_GeneratorDelayStore         = 34,   //DSA
    STKOP_Overlay                     = 35,   //DSA
    STKOP_DisableSaves                = 36,   //DSA
    STKOP_Gear                        = 37,   //DSA
    STKOP_Fetch                       = 38,   //DSA
    STKOP_Store                       = 39,   //DSA
    STKOP_ParamFetch                  = 40,   //DSA
    STKOP_ParamStore                  = 41,   //DSA
    STKOP_Message                     = 42,   //DSA
    STKOP_Cast                        = 43,   //DSA
    STKOP_FetchExCellFlg              = 44,   //DSA
    STKOP_StoreExCellFlg              = 45,   //DSA
    STKOP_ChPoss                      = 46,   //DSA
    STKOP_Say                         = 47,   //DSA
    STKOP_Loc2AbsCoord                = 48,   //DSA
    STKOP_Mastery                     = 49,   //DSA
    STKOP_ULess                       = 50,   //DSA
    STKOP_GetCurse                    = 51,   //DSA
    STKOP_SetCurse                    = 52,   //DSA
    STKOP_GetCharges                  = 53,   //DSA
    STKOP_SetCharges                  = 54,   //DSA
    STKOP_GetBroken                   = 55,   //DSA
    STKOP_SetBroken                   = 56,   //DSA
    STKOP_CellFetch                   = 57,   //DSA
    STKOP_CellStore                   = 58,   //DSA
    STKOP_GlobalFetch                 = 59,   //DSA
    STKOP_MonsterFetch                = 60,   //DSA
    STKOP_Throw                       = 61,   //DSA
    STKOP_TeleportParty               = 62,   //DSA
    STKOP_MonsterStore                = 63,   //DSA
    STKOP_PartyFetch                  = 64,   //DSA
    STKOP_CharFetch                   = 65,   //DSA
    STKOP_CharStore                   = 66,   //DSA
    STKOP_Random                      = 67,   //DSA
    STKOP_CreateCloud                 = 68,   //DSA
    STKOP_Sound                       = 69,   //DSA
    STKOP_Xor                         = 70,   //DSA
    STKOP_MonBlk                      = 71,   //DSA
    STKOP_Describe                    = 72,   //DSA
    STKOP_Move                        = 73,   //DSA
    STKOP_GetPoisoned                 = 74,   //DSA
    STKOP_SetPoisoned                 = 75,   //DSA
    STKOP_Copy                        = 76,   //DSA
    STKOP_MonPoss                     = 77,   //DSA
    STKOP_Palette                     = 78,   //DSA
    STKOP_I_Del                       = 79,   //DSA
    STKOP_I_Add                       = 80,   //DSA
    STKOP_I_CreateCloud               = 81,   //DSA
    STKOP_I_Cast                      = 82,   //DSA
    STKOP_I_TeleportParty             = 83,   //DSA
    STKOP_I_MonsterStore              = 84,   //DSA
    STKOP_I_CharStore                 = 85,   //DSA
    STKOP_I_Move                      = 86,   //DSA
    STKOP_I_Copy                      = 87,   //DSA
    STKOP_I_CellStore                 = 88,   //DSA
    STKOP_I_Throw                     = 89,   //DSA
    STKOP_I_Indirect                  = 90,   //DSA
    STKOP_I_Delay                     = 91,   //DSA
    STKOP_SetAdjustSkillsParameters   = 92,   //DSA
    STKOP_I_DelMon                    = 93,   //DSA
    STKOP_I_InsMon                    = 94,   //DSA
    STKOP_DelMon                      = 95,   //DSA
    STKOP_InsMon                      = 96,   //DSA
    STKOP_TimeFetch                   = 97,   //DSA
    STKOP_JumpGear                    = 98,   //DSA
    STKOP_GosubGear                   = 99,   //DSA
    STKOP_GeneratorDelayFetch         = 100,  //DSA
    STKOP_MissileInfoFetch            = 101,  //DSA
    STKOP_MissileInfoStore            = 102,  //DSA
    STKOP_TextFetch                   = 103,  //DSA
    STKOP_TextSay                     = 104,  //DSA
    STKOP_MonLandD                    = 105,  //DSA
    STKOP_CountInjury                 = 106,  //DSA
    STKOP_Neighbors                   = 107,  //DSA
    STKOP_BitCount                    = 108,  //DSA
    STKOP_ThisCell                    = 109,  //DSA
    STKOP_CausePoison                 = 110,  //DSA
    STKOP_I_CausePoison               = 111,  //DSA
    STKOP_MultiplierFetch             = 112,  //DSA
    STKOP_ExperiencePlus              = 113,  //DSA
    STKOP_IsCarried                   = 114,  //DSA
    STKOP_DiscardText                 = 115,  //DSA
    STKOP_WhereIsChar                 = 116,  //DSA
    STKOP_WhoHasTalent                = 117,  //DSA
    STKOP_SwapCharacter               = 118,  //DSA
    STKOP_FilteredCast                = 119,  //DSA
    STKOP_I_FilteredCast              = 120,  //DSA
    STKOP_GlobalTextStore             = 121,  //DSA
    STKOP_CharNameFetch               = 122,  //DSA
    STKOP_ObjectID                    = 123,  //DSA
    STKOP_GetSubType                  = 124,  //DSA
    STKOP_SetSubType                  = 125,  //DSA
    STKOP_I_SwapCharacter             = 126,  //DSA
    STKOP_PartyDistance               = 129,  //DSA
    STKOP_DSAInfoFetch                = 130,  //DSA
    STKOP_GetSkin                     = 131,  //DSA
    STKOP_SetSkin                     = 132,  //DSA
    STKOP_ThisDSAId                   = 133,  //DSA
    STKOP_TalentsFetch                = 134,  //DSA
    STKOP_TalentsStore                = 135,  //DSA
    STKOP_VSET                        = 136,  //DSA
    STKOP_Jitter                      = 137,  //DSA
    STKOP_ModifyMessage               = 138,  //DSA
    STKOP_NumParam                    = 139,  //DSA
};                                            //DSA
                                              //DSA
enum MESSAGETYPE                              //DSA
{                                             //DSA
  MSG_NULL   =  0,                            //DSA
  MSG_SET    =  1,                            //DSA
  MSG_CLEAR  =  2,                            //DSA
  MSG_TOGGLE =  3                             //DSA
};                                            //DSA
                                              //DSA
enum TARGETTYPE                               //DSA
{                                             //DSA
  TARGET_A    = 0,                            //DSA
  TARGET_B    = 1,                            //DSA
  TARGET_ABS  = 2,                            //DSA
  TARGET_GEAR = 3                             //DSA
};                                            //DSA
                                              //DSA
struct DSAgeneralCmd                          //DSA
{                                             //DSA
private:                                      //DSA
  ui32 cmdCode:6; //DSACOMMAND                //DSA
  i32          unused:10;                     //DSA
public:                                       //DSA
  DSACOMMAND CmdCode(void)                    //DSA
      {return (DSACOMMAND)cmdCode;};          //DSA
};                                            //DSA
                                              //DSA
struct DSAmessageCmd                          //DSA
{                                             //DSA
private:                                      //DSA
  ui32 cmdCode          :6; //DSACOMMAND      //DSA
  ui32 msgType          :2; //MESSAGETYPE     //DSA
  ui32 delay            :2; //0/X/Y/3=explicit//DSA
  ui32 target           :2; //TARGETTYPE      //DSA
  i32          nextState:4; //relative        //DSA
  enum                                        //DSA
  {                                           //DSA
    MAXSTATE     = -8,                        //DSA
    MAXDELAY     = 0,                         //DSA
    COMMANDCODE  = DSACMD_MESSAGE             //DSA
  };                                          //DSA
public:                                       //DSA
  i32 NextState(void){return nextState;};     //DSA
  void NextState(i32 n){nextState=n;};        //DSA
  bool IsMaxState(void)                       //DSA
        {return nextState==-MAXSTATE;};       //DSA
  ui32 Delay(void){return delay;};            //DSA
  void Delay(ui32 dly){delay=dly;};               //DSA
  MESSAGETYPE MsgType(void)                   //DSA
        {return (MESSAGETYPE)msgType;};       //DSA
  void MsgType(MESSAGETYPE m){msgType=m;};    //DSA
  TARGETTYPE Target(void)                     //DSA
        {return (TARGETTYPE)target;};         //DSA
  void Target(TARGETTYPE t){target=t;};       //DSA
  bool StateFits(i32 r)                       //DSA
        {return abs(r)<MAXSTATE;};            //DSA
  void SetMaxState(void){nextState=MAXSTATE;};//DSA
  DSACOMMAND CmdCode(void)                    //DSA
  {return (DSACOMMAND)(cmdCode=COMMANDCODE);};//DSA
};                                            //DSA
                                              //DSA
struct DSAcopyTeleporterCmd                   //DSA
{                                             //DSA
private:                                      //DSA
  ui32 cmdCode            :6; //DSACOMAND     //DSA
  ui32 targetfrom         :2; //TARGETTYPE    //DSA
  ui32 targetto           :2; //TARGETTYPE    //DSA
  i32 nextState           :6; //relative      //DSA
  enum                                        //DSA
  {                                           //DSA
    MAXSTATE      = -32,                      //DSA
    COMMANDCODE   = DSACMD_COPYTELEPORTER     //DSA
  };                                          //DSA
public:                                       //DSA
  i32 NextState(void){return nextState;};     //DSA
  void NextState(i32 n){nextState=n;};        //DSA
  bool IsMaxState(void)                       //DSA
        {return nextState==-MAXSTATE;};       //DSA
  bool StateFits(i32 r)                       //DSA
        {return abs(r)<MAXSTATE;};            //DSA
  void SetMaxState(void){nextState=MAXSTATE;};//DSA
  TARGETTYPE TargetFrom(void)                 //DSA
        {return (TARGETTYPE)targetfrom;};     //DSA
  void TargetFrom(TARGETTYPE t)               //DSA
        {targetfrom=t;};                      //DSA
  TARGETTYPE TargetTo(void)                   //DSA
        {return (TARGETTYPE)targetto;};       //DSA
  void TargetTo(TARGETTYPE t){targetto=t;};   //DSA
  DSACOMMAND CmdCode(void)                    //DSA
  {return (DSACOMMAND)(cmdCode=COMMANDCODE);};//DSA
};                                            //DSA
                                              //DSA
enum LOADTYPE                                 //DSA
{                                             //DSA
  LOAD_INTEGER32 = 30,                        //DSA
  LOAD_ABS32     = 29,                        //DSA
  LOAD_DOLLAR    = 28,                        //DSA
  LOAD_ABS       = 27,                        //DSA
  LOAD_INTEGER   = 26                         //DSA
};                                            //DSA
                                              //DSA
struct DSAloadCmd                             //DSA
{                                             //DSA
private:                                      //DSA
  ui32 cmdCode          :6; //DSACOMMAND      //DSA
  ui32 where            :5; //0->25 = A->Z    //DSA
                            //26=integer      //DSA
                            //27=abs location //DSA
                            //28=$            //DSA
                            //29=ABSLOC32     //DSA
                            //30=INTEGER32    //DSA
  i32          nextState:5;                   //DSA
enum                                          //DSA
{                                             //DSA
  MAXSTATE      = -16,                        //DSA
  COMMANDCODE   = DSACMD_LOAD                 //DSA
};                                            //DSA
public:                                       //DSA
  i32 NextState(void){return nextState;};     //DSA
  void NextState(i32 n){nextState=n;};        //DSA
  bool IsMaxState(void)                       //DSA
      {return nextState==-MAXSTATE;};         //DSA
  bool StateFits(i32 r)                       //DSA
      {return abs(r)<=MAXSTATE;};             //DSA
  void SetMaxState(void){nextState=MAXSTATE;};//DSA
  ui32 Where(void){return where;};            //DSA
  void Where(ui32 w){where=w;};               //DSA
  DSACOMMAND CmdCode(void)                    //DSA
  {                                           //DSA
    return (DSACOMMAND)(cmdCode=COMMANDCODE); //DSA
  };                                          //DSA
};                                            //DSA
                                              //DSA
struct DSAstoreCmd                            //DSA
{                                             //DSA
private:                                      //DSA
  ui32 cmdCode          :6; //DSACOMMAND      //DSA
  ui32 where            :5; //0->25 = A->Z    //DSA
  i32          nextState:5;                   //DSA
enum                                          //DSA
{                                             //DSA
  MAXSTATE      = -16,                        //DSA
  COMMANDCODE   = DSACMD_STORE                //DSA
};                                            //DSA
public:                                       //DSA
  i32 NextState(void){return nextState;};     //DSA
  void NextState(i32 n){nextState=n;};        //DSA
  bool IsMaxState(void)                       //DSA
      {return nextState==-MAXSTATE;};         //DSA
  bool StateFits(i32 r)                       //DSA
      {return abs(r)<=MAXSTATE;};             //DSA
  void SetMaxState(void){nextState=MAXSTATE;};//DSA
  ui32 Where(void){return where;};            //DSA
  void Where(ui32 w){where=w;};               //DSA
  DSACOMMAND CmdCode(void)                    //DSA
  {                                           //DSA
    return (DSACOMMAND)(cmdCode=COMMANDCODE); //DSA
  };                                          //DSA
};                                            //DSA
                                              //DSA
struct DSAoverrideCmd                         //DSA
{                                             //DSA
private:                                      //DSA
  ui32 cmdCode          :6; //DSACOMMAND      //DSA
  ui32 what             :3;                   //DSA
  ui32 value            :3;                   //DSA
  i32          nextState:4;                   //DSA
  enum                                        //DSA
  {                                           //DSA
    MAXSTATE    = -8,                         //DSA
    COMMANDCODE = DSACMD_OVERRIDE,            //DSA
    OVERRIDE_P  = 1                           //DSA
  };                                          //DSA
public:                                       //DSA
  i32 NextState(void){return nextState;};     //DSA
  void NextState(i32 n){nextState=n;};        //DSA
  bool IsMaxState(void)                       //DSA
        {return nextState==-MAXSTATE;};       //DSA
  ui32 What(void){return what;};              //DSA
  void What(ui32 w){what=w;};                 //DSA
  ui32 Value(void){return value;};            //DSA
  void Value(ui32 v){value=v;};               //DSA
  bool StateFits(i32 r)                       //DSA
        {return abs(r)<=MAXSTATE;};           //DSA
  void SetMaxState(void){nextState=MAXSTATE;};//DSA
  DSACOMMAND CmdCode(void)                    //DSA
  {return (DSACOMMAND)(cmdCode=COMMANDCODE);};//DSA
};                                            //DSA
                                              //DSA
struct DSAnoopCmd                             //DSA
{                                             //DSA
private:                                      //DSA
  ui32 cmdCode  :6;          //DSACOMMAND     //DSA
  i32  nextState:10;                          //DSA
  enum                                        //DSA
  {                                           //DSA
    MAXSTATE    = -512,                       //DSA
    COMMANDCODE = DSACMD_NOOP                 //DSA
  };                                          //DSA
public:                                       //DSA
  i32 NextState(void){return nextState;};     //DSA
  void NextState(i32 n){nextState=n;};        //DSA
  bool IsMaxState(void)                       //DSA
        {return nextState==-MAXSTATE;};       //DSA
  bool StateFits(i32 r)                       //DSA
        {return abs(r)<=MAXSTATE;};           //DSA
  void SetMaxState(void){nextState=MAXSTATE;};//DSA
  DSACOMMAND CmdCode(void)                    //DSA
  {return (DSACOMMAND)(cmdCode=COMMANDCODE);};//DSA
};                                            //DSA
                                              //DSA
struct DSAjumpCmd                             //DSA
{                                             //DSA
private:                                      //DSA
  ui32 cmdCode  :6;                           //DSA
  ui32 row      :6;                           //DSA
  ui32 column   :1;                           //DSA
  i32  nextState:3;                           //DSA
  enum                                        //DSA
  {                                           //DSA
    MAXSTATE    = -4,                         //DSA
    COMMANDCODE = DSACMD_JUMP                 //DSA
  };                                          //DSA
public:                                       //DSA
  i32 NextState(void){return nextState;};     //DSA
  void NextState(i32 n){nextState=n;};        //DSA
  i32 Row(void){return row;};                 //DSA
  void Row(i32 r){row=r;};                    //DSA
  i32 Column(void){return column;};           //DSA
  void Column(i32 c){column=c;};              //DSA
  bool IsMaxState(void)                       //DSA
        {return nextState==-MAXSTATE;};       //DSA
  bool StateFits(i32 r)                       //DSA
        {return abs(r)<MAXSTATE;};            //DSA
  void SetMaxState(void){nextState=MAXSTATE;};//DSA
  DSACOMMAND CmdCode(void)                    //DSA
  {return (DSACOMMAND)(cmdCode=COMMANDCODE);};//DSA
};                                            //DSA
                                              //DSA
                                              //DSA
struct DSAgosubCmd                            //DSA
{                                             //DSA
private:                                      //DSA
  ui32 cmdCode  :6;                           //DSA
  ui32 row      :6;                           //DSA
  ui32 column   :1;                           //DSA
  i32  nextState:3;                           //DSA
  enum                                        //DSA
  {                                           //DSA
    MAXSTATE    = -4,                         //DSA
    COMMANDCODE = DSACMD_GOSUB                //DSA
  };                                          //DSA
public:                                       //DSA
  i32 NextState(void){return nextState;};     //DSA
  void NextState(i32 n){nextState=n;};        //DSA
  i32 Row(void){return row;};                 //DSA
  void Row(i32 r){row=r;};                    //DSA
  i32 Column(void){return column;};           //DSA
  void Column(i32 c){column=c;};              //DSA
  bool IsMaxState(void)                       //DSA
        {return nextState==-MAXSTATE;};       //DSA
  bool StateFits(i32 r)                       //DSA
        {return abs(r)<=MAXSTATE;};           //DSA
  void SetMaxState(void){nextState=MAXSTATE;};//DSA
  DSACOMMAND CmdCode(void)                    //DSA
  {return (DSACOMMAND)(cmdCode=COMMANDCODE);};//DSA
};                                            //DSA
                                              //DSA
struct DSAquestionCmd                         //DSA
{                                             //DSA
private:                                      //DSA
  ui32 cmdCode   :6;                          //DSA
  i32  nextState :4;                          //DSA
  ui32 ifColumn  :1;                          //DSA
  ui32 ifCMD     :2;                          //DSA
                  //0=None,1=Jump,2=GoSub     //DSA
  ui32 elseColumn:1;                          //DSA
  ui32 elseCMD   :2;                          //DSA
                  //0=None,1=Jump,2=GoSub     //DSA
  enum                                        //DSA
  {                                           //DSA
    MAXSTATE    = 2,                          //DSA
    COMMANDCODE = DSACMD_QUESTION             //DSA
  };                                          //DSA
public:                                       //DSA
  i32 NextState(void){return nextState;};     //DSA
  void NextState(i32 n){nextState=n;};        //DSA
  bool IfColumn(void){return ifColumn!=0;};   //DSA
  void IfColumn(bool b){ifColumn=b?1:0;};     //DSA
  i32 IfCmd(void){return ifCMD;};             //DSA
  void IfCmd(i32 n){ifCMD=n;};                //DSA
  bool ElseColumn(void)                       //DSA
              {return elseColumn!=0;};        //DSA
  void ElseColumn(bool b){elseColumn=b?1:0;}; //DSA
  i32 ElseCmd(void){return elseCMD;};         //DSA
  void ElseCmd(i32 n){elseCMD=n;};            //DSA
  bool IsMaxState(void)                       //DSA
              {return nextState==-MAXSTATE;}; //DSA
  bool StateFits(i32 r)                       //DSA
              {return abs(r)<MAXSTATE;};      //DSA
  void SetMaxState(void){nextState=MAXSTATE;};//DSA
  DSACOMMAND CmdCode(void)                    //DSA
  {                                           //DSA
    return (DSACOMMAND)(cmdCode=COMMANDCODE); //DSA
  };                                          //DSA
};                                            //DSA
                                              //DSA
struct DSAequalCmd                            //DSA
{                                             //DSA
private:                                      //DSA
  ui32 cmdCode  :6;  //DSACOMMAND             //DSA
  i32  nextState:10;                          //DSA
  enum                                        //DSA
  {                                           //DSA
    MAXSTATE    = -512,                       //DSA
    COMMANDCODE = DSACMD_EQUAL                //DSA
  };                                          //DSA
public:                                       //DSA
  i32 NextState(void){return nextState;};     //DSA
  void NextState(i32 n){nextState=n;};        //DSA
  bool IsMaxState(void)                       //DSA
          {return nextState==-MAXSTATE;};     //DSA
  bool StateFits(i32 r)                       //DSA
          {return abs(r)<MAXSTATE;};          //DSA
  void SetMaxState(void){nextState=MAXSTATE;};//DSA
  DSACOMMAND CmdCode(void)                    //DSA
  {                                           //DSA
    return (DSACOMMAND)(cmdCode=COMMANDCODE); //DSA
  };                                          //DSA
};                                            //DSA
                                              //DSA
struct DSAcaseCmd                             //DSA
{                                             //DSA
private:                                      //DSA
  ui32 cmdCode  :6;  //DSACOMMAND             //DSA
  i32  nextState:10;                          //DSA
  enum                                        //DSA
  {                                           //DSA
    MAXSTATE    = -512,                       //DSA
    COMMANDCODE = DSACMD_CASE                 //DSA
  };                                          //DSA
public:                                       //DSA
  i32 NextState(void){return nextState;};     //DSA
  void NextState(i32 n){nextState=n;};        //DSA
  bool IsMaxState(void)                       //DSA
          {return nextState==-MAXSTATE;};     //DSA
  bool StateFits(i32 r)                       //DSA
          {return abs(r)<MAXSTATE;};          //DSA
  void SetMaxState(void){nextState=MAXSTATE;};//DSA
  DSACOMMAND CmdCode(void)                    //DSA
  {                                           //DSA
    return (DSACOMMAND)(cmdCode=COMMANDCODE); //DSA
  };                                          //DSA
};                                            //DSA
                                              //DSA
                                              //DSA
                                              //DSA
struct DSAvariableFetchCmd                    //DSA
{                                             //DSA
private:                                      //DSA
  unsigned int cmdCode  :6;  //DSACOMMAND     //DSA
  unsigned int index    :7;                   //DSA
               // 0-99 the variable to fetch  //DSA
  signed int   nextState:3;                   //DSA
  enum                                        //DSA
  {                                           //DSA
    MAXSTATE    = -4,                         //DSA
    COMMANDCODE = DSACMD_VARIABLEFETCH        //DSA
  };                                          //DSA
public:                                       //DSA
  int NextState(void){return nextState;};     //DSA
  void NextState(int n){nextState=n;};        //DSA
  bool IsMaxState(void)                       //DSA
    {return nextState==-MAXSTATE;};           //DSA
  bool StateFits(int r)                       //DSA
    {return abs(r)<MAXSTATE;};                //DSA
  void SetMaxState(void)                      //DSA
    {nextState=MAXSTATE;};                    //DSA
  unsigned int Index(void){return index;};    //DSA
  void Index(unsigned int n){index=n;};       //DSA
  void CmdCode(DSACOMMAND cmd){cmdCode=cmd;}; //DSA
  DSACOMMAND CmdCode(void)                    //DSA
  {                                           //DSA
    return(DSACOMMAND)(cmdCode=COMMANDCODE);  //DSA
  };                                          //DSA
};                                            //DSA
                                              //DSA
struct DSAvariableStoreCmd                    //DSA
{                                             //DSA
private:                                      //DSA
  unsigned int cmdCode  :6;  //DSACOMMAND     //DSA
  unsigned int index    :7;                   //DSA
         // 0-99 the variable to store        //DSA
  signed int   nextState:3;                   //DSA
  enum                                        //DSA
  {                                           //DSA
    MAXSTATE    = -4,                         //DSA
    COMMANDCODE = DSACMD_VARIABLESTORE        //DSA
  };                                          //DSA
public:                                       //DSA
  int NextState(void){return nextState;};     //DSA
  void NextState(int n){nextState=n;};        //DSA
  bool IsMaxState(void)                       //DSA
    {return nextState==-MAXSTATE;};           //DSA
  bool StateFits(int r)                       //DSA
    {return abs(r)<MAXSTATE;};                //DSA
  void SetMaxState(void){nextState=MAXSTATE;};//DSA
  unsigned int Index(void){return index;};    //DSA
  void Index(unsigned int n){index=n;};       //DSA
  DSACOMMAND CmdCode(void)                    //DSA
  {                                           //DSA
    return (DSACOMMAND)(cmdCode=COMMANDCODE); //DSA
  };                                          //DSA
};                                            //DSA
                                              //DSA
struct DSAglobalFetchCmd                      //DSA
{                                             //DSA
private:                                      //DSA
  unsigned int cmdCode  :6;  //DSACOMMAND     //DSA
  unsigned int index    :7;                   //DSA
               // 0-99 the variable to fetch  //DSA
  signed int   nextState:3;                   //DSA
  enum                                        //DSA
  {                                           //DSA
    MAXSTATE    = -4,                         //DSA
    COMMANDCODE = DSACMD_GLOBALFETCH          //DSA
  };                                          //DSA
public:                                       //DSA
  int NextState(void){return nextState;};     //DSA
  void NextState(int n){nextState=n;};        //DSA
  bool IsMaxState(void)                       //DSA
    {return nextState==-MAXSTATE;};           //DSA
  bool StateFits(int r)                       //DSA
    {return abs(r)<MAXSTATE;};                //DSA
  void SetMaxState(void)                      //DSA
    {nextState=MAXSTATE;};                    //DSA
  unsigned int Index(void){return index;};    //DSA
  void Index(unsigned int n){index=n;};       //DSA
  void CmdCode(DSACOMMAND cmd){cmdCode=cmd;}; //DSA
  DSACOMMAND CmdCode(void)                    //DSA
  {                                           //DSA
    return(DSACOMMAND)(cmdCode=COMMANDCODE);  //DSA
  };                                          //DSA
};                                            //DSA
                                              //DSA
struct DSAglobalStoreCmd                      //DSA
{                                             //DSA
private:                                      //DSA
  unsigned int cmdCode  :6;  //DSACOMMAND     //DSA
  unsigned int index    :7;                   //DSA
         // 0-99 the variable to store        //DSA
  signed int   nextState:3;                   //DSA
  enum                                        //DSA
  {                                           //DSA
    MAXSTATE    = -4,                         //DSA
    COMMANDCODE = DSACMD_GLOBALSTORE          //DSA
  };                                          //DSA
public:                                       //DSA
  int NextState(void){return nextState;};     //DSA
  void NextState(int n){nextState=n;};        //DSA
  bool IsMaxState(void)                       //DSA
    {return nextState==-MAXSTATE;};           //DSA
  bool StateFits(int r)                       //DSA
    {return abs(r)<MAXSTATE;};                //DSA
  void SetMaxState(void){nextState=MAXSTATE;};//DSA
  unsigned int Index(void){return index;};    //DSA
  void Index(unsigned int n){index=n;};       //DSA
  DSACOMMAND CmdCode(void)                    //DSA
  {                                           //DSA
    return (DSACOMMAND)(cmdCode=COMMANDCODE); //DSA
  };                                          //DSA
};                                            //DSA
                                              //DSA
                                              //DSA
struct DSAampersandCmd                        //DSA
{                                             //DSA
private:                                      //DSA
  ui32 cmdCode  :6;  //DSACOMMAND             //DSA
  ui32 subCode  :7;                           //DSA
  i32  nextState:3;                           //DSA
  enum                                        //DSA
  {                                           //DSA
    MAXSTATE    = -4,                         //DSA
    COMMANDCODE = DSACMD_AMPERSAND            //DSA
  };                                          //DSA
public:                                       //DSA
  i32 NextState(void){return nextState;};     //DSA
  void NextState(i32 n){nextState=n;};        //DSA
  bool IsMaxState(void)                       //DSA
      {return nextState==-MAXSTATE;};         //DSA
  bool StateFits(i32 r)                       //DSA
      {return abs(r)<MAXSTATE;};              //DSA
  void SetMaxState(void){nextState=MAXSTATE;};//DSA
  STKOP SubCode(void){return (STKOP)subCode;};//DSA
  void SubCode(STKOP n){subCode=n;};          //DSA
  DSACOMMAND CmdCode(void)                    //DSA
  {return (DSACOMMAND)(cmdCode=COMMANDCODE);};//DSA
};                                            //DSA
                                              //DSA
struct DSAfetchCmd                            //DSA
{                                             //DSA
private:                                      //DSA
  ui32 cmdCode  :6;          //DSACOMMAND     //DSA
  i32  nextState:10;                          //DSA
  enum                                        //DSA
  {                                           //DSA
    MAXSTATE    = -512,                       //DSA
    COMMANDCODE = DSACMD_FETCH                //DSA
  };                                          //DSA
public:                                       //DSA
  i32 NextState(void){return nextState;};     //DSA
  void NextState(i32 n){nextState=n;};        //DSA
  bool IsMaxState(void)                       //DSA
          {return nextState==-MAXSTATE;};     //DSA
  bool StateFits(i32 r)                       //DSA
          {return abs(r)<MAXSTATE;};          //DSA
  void SetMaxState(void){nextState=MAXSTATE;};//DSA
  DSACOMMAND CmdCode(void)                    //DSA
  {                                           //DSA
    return (DSACOMMAND)(cmdCode=COMMANDCODE); //DSA
  };                                          //DSA
};                                            //DSA
                                              //DSA
                                              //DSA
union DSAcommand                              //DSA
{                                             //DSA
  DSAmessageCmd          msgCmd;              //DSA
  DSAgeneralCmd          generalCmd;          //DSA
  DSAoverrideCmd         overrideCmd;         //DSA
  DSAnoopCmd             noopCmd;             //DSA
  DSAcopyTeleporterCmd   copyTeleporterCmd;   //DSA
  DSAgosubCmd            gosubCmd;            //DSA
  DSAloadCmd             loadCmd;             //DSA
  DSAstoreCmd            storeCmd;            //DSA
  DSAquestionCmd         questionCmd;         //DSA
  DSAequalCmd            equalCmd;            //DSA
  DSAfetchCmd            fetchCmd;            //DSA
  DSAjumpCmd             jumpCmd;             //DSA
  DSAampersandCmd        ampersandCmd;        //DSA
  DSAcaseCmd             caseCmd;             //DSA
  DSAvariableFetchCmd    variableFetchCmd;    //DSA
  DSAvariableStoreCmd    variableStoreCmd;    //DSA
  DSAglobalFetchCmd      globalFetchCmd;      //DSA
  DSAglobalStoreCmd      globalStoreCmd;      //DSA
};                                            //DSA
                                              //DSA
class DSAAction                               //DSA
{                                             //DSA
  friend class DSAState;                      //DSA
  i32         m_column;                       //DSA
  i32         m_numPgmWords;                  //DSA
  ui16       *m_program;                      //DSA
public:                                       //DSA
  DSAAction(void);                            //DSA
  ~DSAAction(void);                           //DSA
  i32 Column(void){return m_column;};         //DSA
  void Column(i32 column){m_column = column;};//DSA
  void Write(void);                           //DSA
  void Read(void);                            //DSA
  ui16 *Program(void){return m_program;};     //DSA                   //DSA
  i32  ProgramSize(void)                      //DSA
  {                                           //DSA
    return m_numPgmWords;                     //DSA
  };                                          //DSA
};                                            //DSA
                                              //DSA
class DSAState                                //DSA
{                                             //DSA
  i32         m_numAction;                    //DSA
  DSAAction **m_pActions;                     //DSA
public:                                       //DSA
  DSAState(void);                             //DSA
  ~DSAState(void);                            //DSA
  bool operator !=(DSAState&);                //DSA
  void Write(void);                           //DSA
  void Read(void);                            //DSA
  ui16 *Program(i32 column);                  //DSA
  i32  ProgramSize(i32 column);               //DSA
};                                            //DSA
                                              //DSA
class DSA                                     //DSA
{                                             //DSA
  char       m_description[80];               //DSA
  ui16       m_state;                         //DSA
  char       m_localState;                    //DSA
  ui8        m_groupID;                       //DSA
  i32        m_numState;                      //DSA
  i32        m_firstDisplayedState;           //DSA
  DSAState **m_pStates;                       //DSA
public:                                       //DSA
  DSA(void);                                  //DSA
  ~DSA(void);                                 //DSA
  char *Description(void)                     //DSA
  {                                           //DSA
    return m_description;                     //DSA
  };                                          //DSA
  ui8 GroupID(void){return m_groupID;};       //DSA
  i32 LocalState(void)                        //DSA
  {                                           //DSA
    return m_localState;                      //DSA
  };                                          //DSA
  i32 State(void){return m_state;};           //DSA
  bool IsMaster(void)                         //DSA
  {                                           //DSA
    return m_localState!=3;                   //DSA
  };                                          //DSA
  void State(i32 s){m_state = (ui16)s;};      //DSA
  i32 NumState(void){return m_numState;};     //DSA
  void Write(void);                           //DSA
  void Read(void);                            //DSA
  DSAState *StatePointer(i32 stateNum);       //DSA
};                                            //DSA
                                              //DSA
class DSAINDEX                                //DSA
{                                             //DSA
  DSA *m_pDSA[256];                           //DSA
  ui32 m_tracing[8]; //One bit per DSA        //DSA
  ui32 m_savedTracing[8];                     //DSA
  bool m_isLoaded;   //Are DSAs loaded?       //DSA
  i32  m_traceOverride; //-1=NONE; 256=ALL    //DSA 
public:                                       //DSA
  DSAINDEX(void);                             //DSA
  ~DSAINDEX(void);                            //DSA
  DSA *GetDSA(i32 index);                     //DSA
  void SetDSA(i32 index, DSA *dsa);           //DSA
  void Cleanup(void);                         //DSA
  void SaveTracing(void);                     //DSA
  void RestoreTracing(ui32 *pArray);          //DSA
  void ReadTracing(void);                     //DSA
  void WriteTracing(void);                    //DSA
  void IsLoaded(bool b){m_isLoaded=b;};       //DSA
  bool IsLoaded(void){return m_isLoaded;};    //DSA
  bool IsTracing(i32 dsaIndex);               //DSA
  void SetTracing(i32 dsaIndex);              //DSA
  void NoTracing(void);                       //DSA
  void AllTracing(void);                      //DSA
  bool AnyTraceActive(void);                  //DSA
  void TraceOverride(i32 t)                   //DSA
        {m_traceOverride=t;};                 //DSA
  i32  TraceOverride(void)                    //DSA
        {return m_traceOverride;};            //DSA   
};                                            //DSA
extern DSAINDEX DSAIndex;                     //DSA
extern ui16 DSALevelIndex[64][32];            //DSA
extern i32 objectListIndexSize;
extern i32 objectListSize;
extern i32 indirectTextIndexSize;
