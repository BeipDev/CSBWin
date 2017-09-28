#include "stdafx.h"

#include <stdio.h>

#include "UI.h"

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"


void MD5Init();
void MD5Update(ui8 *data, ui32 len);
void MD5Final(ui8 *digest);
void CleanupCustomPhrases();
void CleanupTranslations();
void DSAInstrumentation_Dump();
void FILETABLECleanup();
bool IsRecordFileRecording();

extern ui32 numRandomCalls;
extern char *PlayfileName;
extern char szCSBVersion[];
extern i32 VBLMultiplier;
extern bool fullscreenRequested;
//extern bool fullscreenActive;
extern bool RecordCommandOption;
extern bool NoRecordCommandOption;
extern char *finalEditText;
void AtariMemCleanup();

#define MaxTraceFiles 100
i16   TraceFile  = -1;
i16   GraphicTraceFile = -1;
char *g_folderName = NULL;
std::string g_folderParentName;
std::string g_root;
VIDEOMODE videoMode;
std::unique_ptr<ui8[]> g_tempBitmap;
int g_tempBitmapSize = 0;

ui8 *dataTypeMap = NULL;
ui16 *dataIndexMap = NULL;
i32 dataMapLength = 0;
ui32 graphicSignature1=0, graphicSignature2=0;
ui32 expectedGraphicSignature1=0, expectedGraphicSignature2=0;
ui32 CSBgraphicSignature1=0, CSBgraphicSignature2=0;
ui32 expectedCSBgraphicSignature1=0, expectedCSBgraphicSignature2=0;
ui32 EDBT_CSBGraphicsSignature_data = 0xffffffff;
ui32 EDBT_GraphicsSignature_data = 0xffffffff;
ui32 EDBT_CSBversion_data = 0xffffffff;
ui32 EDBT_Debuging_data = 0xffffffff;
ui32 dungeonSignature1=0, dungeonSignature2=0;
ui32 versionSignature = 0;
ui32 spellFilterLocation = 0;
ui32 cellflagArraySize;
bool disableSaves = false;
bool bigActuators = false;
bool sequencedTimers = false;
bool extendedTimers = false;
bool DefaultDirectXOption = false;
bool extendedWallDecorations;
i32  deleteDuplicateTimers = -1;
OVERLAYDATA currentOverlay;
SOUNDDATA currentSound;
TEMPORARY_FILE CSBgraphicsFile;
bool overlayActive = false;
i32  xGraphicJitter=0;
i32  yGraphicJitter=0;
i32  xOverlayJitter=0;
i32  yOverlayJitter=0;
bool jitterChanged = false;
ui8 overlayPaletteRed[512];
ui8 overlayPaletteGreen[512];
ui8 overlayPaletteBlue[512];
bool invisibleMonsters = false;
bool drawAsSize4Monsters = false;
i32 g_objectListIndexSize = 0;
EXPOOL expool;
BACKGROUND_LIB backgroundLib;
i32 adjustSkillsParameters[5];
bool monsterMoveFilterActive = false;
i32 numGlobalVariables = 0;
ui32 *globalVariables = NULL;
ui32 parameterMessageSequence = 0;
bool playback_71 = false;
bool neophyteSkills = false;

MouseQueueEnt MouseQueue[MOUSEQLEN]; //Moved to global space to enlarge.//16852
MouseQueueEnt *pMouseQueue = MouseQueue;


ui8         monsterMoveInhibit[4]; //N, E, S, W

DSAINDEX DSAIndex;
ui16 DSALevelIndex[64][32];



void AllocateTempBitmap(int size)
{
  if (g_tempBitmapSize < size)
  {
    g_tempBitmap = std::make_unique<ui8[]>(size);
    g_tempBitmapSize = size;
  }
}


void ClearOverlayPalette()
{
  int i, intensity;
  //ui16 result[512];
  //Set overlay palette to the identity palette
  // We translate the color intensities of the Atari palette (0 - 7)
  // to 8-bit color intensities (0 - 255) and split the three
  // colors components into three arrays.
  // Now, for each color word ( rrrgggbbb ) we can do a table
  // lookup to determine, for example, the red component.
  for (i=0; i<512; i++)
  {
    intensity=((i>>6)&7);
    intensity = (intensity<<5)|(intensity<<2)|(intensity>>1);
    overlayPaletteRed[i] = (ui8)intensity;
    intensity=((i>>3)&7);
    intensity = (intensity<<5)|(intensity<<2)|(intensity>>1);
    overlayPaletteGreen[i] = (ui8)intensity;
    intensity=((i>>0)&7);
    intensity = (intensity<<5)|(intensity<<2)|(intensity>>1);
    overlayPaletteBlue[i] = (ui8)intensity;
    //result[i] =   ((overlayPaletteRed[i]>>3)<<10)
    //            | ((overlayPaletteGreen[i]>>3)<<5)
    //            | ((overlayPaletteBlue[i]>>3));
  };
  //FILE *f = fopen("identityPalette.bin","wb");
  //fwrite(result,2,512,f);
  //fclose(f);
}
void CloseTraceFile()
{
  if (TraceFile >= 0) CLOSE(TraceFile);
  TraceFile = -1;
}

void CloseGraphicTraceFile()
{
  CLOSE(GraphicTraceFile);
  GraphicTraceFile = -1;
}

void SmartDiscardTrace(FILE *f);

bool OpenTraceFile()
{//Return true if file successfully.
  i32 i=0, f;
  bool result = false;
  const char *folder;
#ifdef _LINUX
  for (f=0, folder="traces/"; f<2; f++, folder="")
#else
  for (f=0, folder="\\traces\\"; f<2; f++, folder="")
#endif
  {
    for (i=0; i<MaxTraceFiles; i++)
    {
      char filename[100];
      sprintf(filename,"%sTRACE%03d.txt",folder,i);
      TraceFile = OPEN(filename,"r");
      if (TraceFile < 0)
      {
        char  msg[100];;
        TraceFile = CREATE(filename,"w", false);
        if (TraceFile < 0)
        {
          if (f==0) break;
          sprintf(msg,"Cannot open file %s for Trace",filename);
          UI_MessageBox(msg,
              "",MESSAGE_OK);
        }
        else
        {
          result = true;
        };
        break;
      }
      else
      {
        CLOSE (TraceFile);
      };
    };
    if (result) break;
  };
  if (i==MaxTraceFiles)
      UI_MessageBox("Too many Trace files",
              "",MESSAGE_OK);
  return result;
};


bool OpenGraphicTraceFile()
{//Return true if file successfully.
  GraphicTraceFile = OPEN("GraphicTrace.txt","w");
  if (GraphicTraceFile < 0) return false;
  GraphicTraceActive = true;
  return true;
}

i32 millisecondsPerVBL = 17;
i32 VBLperTimer = 10;
bool TimerTraceActive=false;
#ifndef _MSVC_INTEL
bool DSATraceActive = false;
#endif
bool AttackTraceActive = false;
bool AITraceActive = false;
bool GraphicTraceActive = false;
i32  traceViewportDrawing = 0;
i32 GameMode=0;
//i32 clockSpeed = 10;
bool BeginRecordOK = false;
bool RepeatGame = false;
bool GameIsComplete;
bool ItemsRemainingOK = false;
bool gameFrozen = false;
i32 DrawViewportCount = 0;
DOESNOTEXIST DoesNotExist;
bool DiskMenuNeeded;
i32  MostRecentlyAdjustedSkills[2] = {0,0}; //Just for Player display
i32  LatestSkillValues[2] = {0,0}; //Just for Player display.
i32  AdjustedSkillNumber = 0;
bool AutoEnlarge = false;
bool ThreeDMovements = false; //For 3DMaze
bool GravityGame = false;     //For 3DMaze
unsigned char cipherkey[64];
unsigned char *encipheredDataFile = NULL;//cipherkey;
bool simpleEncipher = false;
i32  nextGravityMoveTime;
char moveHistory[4] = {0,1,2,0};
bool gravityMove = false;
i32  ExtendedFeaturesSize = 0;
char ExtendedFeaturesVersion = '@';
bool NoSound = false;
bool NoSleep = false;
bool DM_rules = false;
bool indirectText = false;
i32  totalMoveCount;
bool version90Compatible = false;
i32 RememberToPutObjectInHand = -1;
i32 segSize[5]   = {0x02,0x02,0x02,0x02,0x00};
i32 segSrcX[5]   = {0x00,0x00,0xe0,0x00,0x00};
i32 segSrcY[5]   = {0x21,0x00,0x21,0xa9,0x00};
i32 segWidth[5]  = {0xe0, 320,0x60, 320,0x02};
i32 segHeight[5] = {0x88,0x21,0x88,0x1f,0x02};
i32 segX[5]      = {0x00,0x00,0xe0,0x00,0x20};
i32 segY[5]      = {0x21,0x00,0x21,0xa9,0x31};
i32 *videoSegSize   = segSize;
i32 *videoSegSrcX   = segSrcX;
i32 *videoSegSrcY   = segSrcY;
i32 *videoSegWidth  = segWidth;
i32 *videoSegHeight = segHeight;
i32 *videoSegX      = segX;
i32 *videoSegY      = segY;
char *gameInfo = NULL;
i32   gameInfoSize;  //Binary information (includes the trailing NUL)
char hintKey[8];

SPEEDTABLE speedTable[SPEED_NUMSPEED];
SPEEDS gameSpeed = SPEED_NORMAL;
VOLUMETABLE volumeTable[VOLUME_NUMVOLUME];
VOLUMES gameVolume = VOLUME_FULL;
bool playerClock = false;
bool extraTicks = true;
bool usingDirectX = true;

//Locations in Atari screen pixel units.
// Viewport, Portraits, Spells&Commands, Text
void TranslateFullscreen(i32 a, i32 b, i32& x, i32& y)
{//Translate physical pixel units on actual screen.
 // to pixel units on Atari screen
    i32 i, left, top, right, bottom;
  for (i=0; i<4; i++)
  {
    if (videoSegSize[i] == 0) continue;
    i32 size = videoSegSize[i];
    left   = videoSegSrcX[i];
    top    = videoSegSrcY[i];
    right  = left + videoSegWidth[i]-1;
    bottom = top  + videoSegHeight[i]-1;
    if (a < videoSegX[i]) continue;
    if (a >= videoSegX[i] + size*(videoSegWidth[i])) continue;
    if (b < videoSegY[i])    continue;
    if (b >= videoSegY[i] + size*(videoSegHeight[i])) continue;
    x = (a - videoSegX[i] + size/2)/size + left;
    y = (b - videoSegY[i] + size/2)/size + top;
    if (x > right-1) x = right-1;
    if (y > bottom-1) y = bottom-1;
    return;
  };
}

bool GetVideoRectangle(i32 i, RECT *rect)
{
  i32 j;
  if (i>3) return false;
  for (j=0; j<4; j++)
  {
    if (videoSegSize[j] == 0) continue;
    if (i == 0)
    {
      rect->top = videoSegY[j];
      rect->left = videoSegX[j];
      rect->right = videoSegX[j] + videoSegSize[j]*videoSegWidth[j];
      rect->bottom = videoSegY[j] + videoSegSize[j]*videoSegHeight[j];
      return true;
    };
    i--;
  };
  return false;
}

/*
SDLIST::~SDLIST()
{
  SD *temp;
  while (psd != NULL)
  {
    temp = psd;
    psd = psd->pnext;
    delete temp;
  };
}

SD *SDLIST::newSD(char *name,
                  i32 minimum,
                  float level,
                  float y_intercept,
                  float slope)
{
  SD *temp;
  if (strlen(name) > 15) return NULL;
  temp = new SD;
  temp->pnext = psd;
  psd = temp;
  strcpy(temp->name,name);
  temp->minimum = minimum;
  temp->level = level;
  temp->y_intercept = y_intercept;
  temp->slope = slope;
  return temp;
}

SD *SDLIST::First()
{
  return psd;
}

SD *SDLIST::Next(SD *ent)
{
  return ent->pnext;
}
*/

/*
i32 AddSD(char *name,
          i32 min,
          float level,
          float y_intercept,
          float slope)
{
  if(sdList.newSD(name,min,level,y_intercept,slope)==NULL) return 1;
  return 0;
}

// The list of Smart Discards from the config.txt file
SDLIST sdList;
SDENT sdTable[199];

*/


DATABASES db;

// *********************************************************
// Object Database class member functions
// *********************************************************

i32 dbEntrySizes[16] =
{
  sizeof (DB0),
  sizeof (DB1),
  sizeof (DB2),
  sizeof (DB3),
  sizeof (DB4),
  sizeof (DB5),
  sizeof (DB6),
  sizeof (DB7),
  sizeof (DB8),
  sizeof (DB9),
  sizeof (DB10),
  sizeof (DB11),
  sizeof (DB12),
  sizeof (DB13),
  sizeof (DB14),
  sizeof (DB15)
};

const char *dbNames[16] =
{
  "Doors",        //0
  "Teleporters",  //1
  "Scrolls",      //2
  "Actuators",    //3
  "Monsters",     //4
  "Weapons",      //5
  "Clothes",      //6
  "Scrolls",      //7
  "potions",      //8
  "chests",       //9
  "food, misc",   //10
  "??????",       //11
  "??????",       //12
  "??????",       //13
  "Missiles",     //14
  "Clouds"        //15
};

DATABASES::DATABASES ()
{
  i32 i;
  for (i=0; i<16; i++)
  {
    m_Address[i] = NULL;
    m_numEnt[i] = 0;
  };
}

DATABASES::~DATABASES ()
{
  i32 i;
  for (i=0; i<16; i++) DeAllocate(i);
}

RN::RN(ui32,ui32,ui32)
{
  NotImplemented(0x7cd0);
}

RN::RN(DBTYPE dbType, ui32 dbIndex)
{
  i32 emptyIndexEntry;
  i32 i, newDML;
  DBCOMMON *pDBC;
  emptyIndexEntry = -1;
  for (i=dataMapLength-1; i>0; i--)
  {
    if (dataIndexMap[i] == 0xffff)
    {
      emptyIndexEntry = i;
      continue;
    };
    if ( (dataTypeMap[i] & 15) != dbType) continue;
    if (dataIndexMap[i] == dbIndex)
    {
      indirectIndex = (ui16)i;
      return; //Already exists
    };
    pDBC = db.GetCommonAddress(dbType, dataIndexMap[i]);
    if (pDBC->link() == RNnul)
    {
      emptyIndexEntry = i;
      continue;
    };
  };
  if (emptyIndexEntry == -1)
  {
    if (dataMapLength > 64000)
    {
      UI_MessageBox("Object Indirect Index Overflow","Error",MB_OK);
      die(0x22dde5);
    };
    newDML = 32*(dataMapLength/32) + 33;
    dataTypeMap = (ui8 *)UI_realloc(dataTypeMap, newDML, MALLOC062);
    dataIndexMap = (ui16 *)UI_realloc(dataIndexMap, 2*newDML, MALLOC063);
    if ( (dataTypeMap == NULL) || (dataIndexMap == NULL))
    {
      UI_MessageBox("Cannot allocate memory for object index",
                    "Error", MB_OK);
      die(0xa1c445);
    };
    emptyIndexEntry = dataMapLength;
    dataMapLength++;
  };
  indirectIndex = (ui16)emptyIndexEntry;
  dataIndexMap[emptyIndexEntry] = (ui16)dbIndex;
  dataTypeMap[emptyIndexEntry] = (ui8)dbType;
  return;
}

RN::RN(RNVAL i)
{
  if (   (i < 0xff80)
      && (i != 0))
  {
    UI_MessageBox("Illegal RNVAL","Sys Error",MB_OK);
    die(0xace321);
  };
  indirectIndex = (ui16)i;
};


bool RN::checkIndirectIndex() const
{
  if (    (indirectIndex < dataMapLength)
       && (indirectIndex > 0) )return true;
  UI_MessageBox("Illegal object Indirect Index","error",MB_OK);
#ifdef _DEBUG
  die(0x6ccee);
#endif
  return false;
}

bool RN::checkIndirectIndex(ui32 i) const
{
  if (    (i < (unsigned)dataMapLength)
       && (i > 0) )return true;
  return false;
}


void RN::CreateSpell(i32 n)
{
  ASSERT(n<128,"spell");
  indirectIndex = (ui16)(0xff80 + n);
}

ui8 RN::GetSpellType()
{
  if (indirectIndex < 0xff80)
  {
    UI_MessageBox("Illegal Spell object","Error",MB_OK);
    die(0xc8d9a3);
  };
  return (ui8)(indirectIndex & 0x7f);
}

bool RN::NonMaterial()
{
  DBTYPE dbType;
  DB4 *pDB_4;
  dbType = this->dbType();
  ASSERT(dbType == dbMONSTER,"monster");
  if (dbType != dbMONSTER) return false;
  pDB_4 = GetRecordAddressDB4(*this);
  ASSERT(pDB_4->monsterType() < 27,"monsterType");
  return d.MonsterDescriptor[pDB_4->monsterType()].nonMaterial();
}



//   TAG009308
OBJ_DESC_INDEX  RN::DescIndex() const
{
  //UNKNOWN *unkA3;
  i32 DBType;
  OBJ_DESC_INDEX result;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //unkA3 = GetRecordAddress(object);
  DBType = -1;
  if (*this != RNempty) DBType = dbType();
  switch (DBType)
  {
  case dbSCROLL:
      result = objDI_Scroll;
      break;
  case dbCHEST:
      result = (OBJ_DESC_INDEX)(BITS1_2(GetRecordAddressDB9(*this)->word4())+objDI_Chest);
      break;
  case dbMISC:
      result = (OBJ_DESC_INDEX)(GetRecordAddressDB10(*this)->miscType() + objDI_Compass);
      break;
  case dbWEAPON:
      result = (OBJ_DESC_INDEX)(GetRecordAddressDB5(*this)->weaponType() + objDI_EyeOfTime);
      break;
  case dbCLOTHING:
    //result is object index
      result = (OBJ_DESC_INDEX)(GetRecordAddressDB6(*this)->clothingType() + objDI_Cape);
      break;
  case dbPOTION:
      //result is object index.
      result = (OBJ_DESC_INDEX)(GetRecordAddressDB8(*this)->potionType() + objDI_MonPotionA);
      break;
  default:
      result = (OBJ_DESC_INDEX)(-1);
  };
  return result;
}




// *********************************************************
//
// *********************************************************
//         TAG001188
OBJ_NAME_INDEX RN::NameIndex() const
{
  dReg D0;
  OBJ_NAME_INDEX objNID7;
  DBCOMMON *dbA3;
  objNID7 = GetBasicObjectType(*this);
  if (objNID7 == objNI_NotAnObject) return objNID7;
  // Firstly, if it is not a modifiable object or
  //  a flask of some sort, then return it as is.
  if (   (objNID7 > objNI_LastModifiableObject)
      || (objNID7 < objNI_FirstModifiableObject) )
  {
    if ( (objNID7 < objNI_FirstFullFlask) || (objNID7 > objNI_LastFullFlask) )
    { // Not a filled flask
      if (objNID7 != objNI_EmptyFlask) return objNID7;
    };
  };
  dbA3 = GetCommonAddress(*this);
  if (objNID7 == objNI_Compass_N)
  {
    objNID7 = (OBJ_NAME_INDEX)(objNID7 + d.partyFacing);
    return objNID7;
  };
  if (objNID7 == objNI_Torch_a)
  {
    //D0W = dbA3->word(2);
    //D0W = BITS15_15(D0W);
    D0W = (i16)dbA3->CastToDB5()->litTorch();
    if (D0W)
    {
      D0W = (i16)dbA3->CastToDB5()->charges();
      D0W = (UI8)(d.Byte640[D0W]);
      objNID7 = (OBJ_NAME_INDEX)(objNID7 + D0W);
    };
    return objNID7;
  };
  if (objNID7 == objNI_OpenScroll)
  {
    D0W = (i16)dbA3->CastToDB7()->open();
    if (!D0W)
    {
      objNID7 = objNI_Scroll;
    };
    return objNID7;
  };
  if (   (objNID7==objNI_Waterskin)
      || (objNID7==objNI_Illumulet_a)
      || (objNID7==objNI_JewelSymal_a) )
  {
    D0W = (i16)dbA3->CastToDB10()->value();
    if (D0W)
    {
      objNID7 = (OBJ_NAME_INDEX)(objNID7 + 1);
    };
    return objNID7;
  };
  if (   (objNID7 == objNI_Storm_a)
      || (objNID7 == objNI_Flamitt_a)
      || (objNID7 == objNI_StormRing_a)
      || (objNID7 == objNI_RABlade_a)
      || (objNID7 == objNI_EyeOfTime_a)
      || (objNID7 == objNI_StaffOfClaws_a) )
  {
    D0W = (i16)dbA3->CastToDB5()->charges();
    if (D0W)
    {
      objNID7 = (OBJ_NAME_INDEX)(objNID7 + 1);
    };
  };
  return objNID7;
}



bool RN::IsAKey() const
{
  OBJ_NAME_INDEX objNI;
  if ( dbType() != dbMISC ) return false;
  objNI = NameIndex();
  if (objNI < objNI_FirstKey) return false;
  if (objNI > objNI_LastKey) return false;
  return true;
}

bool RN::Levitating()
{
  DBTYPE dbType;
  DB4 *pDB_4;
  dbType = this->dbType();
  ASSERT(dbType == dbMONSTER,"levitate");
  if (dbType != dbMONSTER) return false;
  pDB_4 = GetRecordAddressDB4(*this);
  ASSERT(pDB_4->monsterType() < 27,"monsterType");
  return d.MonsterDescriptor[pDB_4->monsterType()].levitating();
}

ui8 RN::VerticalSize()
{
  DBTYPE dbType;
  DB4 *pDB_4;
  dbType = this->dbType();
  ASSERT(dbType == dbMONSTER,"vsize");
  if (dbType != dbMONSTER) return 1;
  pDB_4 = GetRecordAddressDB4(*this);
  ASSERT(pDB_4->monsterType() < 27,"monsterType");
  return d.MonsterDescriptor[pDB_4->monsterType()].verticalSize();
}

const RN DBCOMMON::link()
{
  return m_link;
}

RN *DBCOMMON::pLink()
{
  return &m_link;
}

void DBCOMMON::link(RN rn)
{
  m_link = rn;
}

DBCOMMON *DATABASES::GetCommonAddress(RN object)
{
  i32 dbNum, index;
  dbNum = object.dbNum();
  index = object.idx();
  ASSERT(index < m_numEnt[dbNum],"numEnt");
  return (DBCOMMON *)
       (((char *)m_Address[dbNum]) + index * dbEntrySizes[dbNum]);
}

DBCOMMON *DATABASES::GetCommonAddress(DBTYPE dbType, i32 i)
{
  i32 dbNum, index;
  dbNum = dbType;
  index = i;
  ASSERT(index < m_numEnt[dbNum],"numEnt");
  return (DBCOMMON *)
       (((char *)m_Address[dbNum]) + index * dbEntrySizes[dbType]);
}

void DATABASES::SetAddress(DBTYPE dbType, DBCOMMON *m)
{
  i32 dbNum;
  dbNum = dbType;
  m_Address[dbNum] = m;
}

i32 DATABASES::NumEntry(i32 dbNum)
{
  return m_numEnt[dbNum];
}

void DATABASES::NumEntry(i32 dbNum, i32 n)
{
  m_numEnt[dbNum] = n;
  d.dungeonDatIndex->DBSize(dbNum, (ui16)n);
}

void DATABASES::Allocate(i32 dbNum, i32 numEnt)
{
  DeAllocate(dbNum); // In case something is there already.
  ASSERT((dbNum & 0xfffffff0) == 0, "dbnum");
  dbNum &= 15;
  //ASSERT(numEnt <= ((dbNum==15) ? 768 : (dbNum==3)?16000:8000), "database overflow");
  ASSERT(numEnt <= Max(dbNum), "database overflow");
  m_numEnt[dbNum] = numEnt;
  if (numEnt > 0)
  {
    m_Address[dbNum] = (DBCOMMON *)UI_malloc(dbEntrySizes[dbNum] * numEnt, MALLOC047);
    if (m_Address[dbNum]==NULL)
    {
      UI_MessageBox("Cannot allocate memory",NULL,MESSAGE_OK);
      die (0xbad);
    };
    pnt p = (pnt)m_Address[dbNum];
    for (i32 i=0; i<m_numEnt[dbNum]; i++)
    {
      memset(p,0,dbEntrySizes[dbNum]);
      *(RN *)p = RNnul; // Mark entry as empty
      p += dbEntrySizes[dbNum];
    }
  }
  else
  {
    m_Address[dbNum] = NULL;
  };
}

void DATABASES::DeAllocate(i32 dbNum)
{
  ASSERT((dbNum & 0xfffffff0) == 0, "dbnum");
  dbNum &= 15;
  if (m_Address[dbNum] != NULL) UI_free(m_Address[dbNum]);
  m_Address[dbNum] = NULL;
  m_numEnt[dbNum] = 0;
}

i32 DATABASES::Enlarge(i32 dbNum)
{ // Returns index of newly available entry or -1
  i32 max, iNew, i;
  i32 result;
  i32 newSize;
  DBCOMMON *pC;
  char msg[1000];
  ASSERT( (dbNum & 0xfffffff0) == 0,"dbnum");
  if (m_noEnlarge & (1 << dbNum)) return RNnul;
//  max = (dbNum==15) ? 1000 : ((dbNum==3)?16000:8000);
  max = Max(dbNum);
  if (m_numEnt[dbNum] >= max)
  {
    return -1;//Already as big as possible.
  };

  sprintf(msg,
             "Database Number %d (containing %s)\n"
             "has become full.  There is a rather arbitrary\n"
             "limit of %d items in this database.  The\n"
             "original Atari program handled this by\n"
             "discarding items at random.  It is possible\n"
             "to increase the size to %d but of course this\n"
             "will be 'non-standard'.\n"
             "\n"
             "Do you want me to enlarge the database?"
             ,dbNum
             ,dbNames[dbNum]
             ,m_numEnt[dbNum]
             ,max);
        AutoEnlarge = true; // ***************** PRS 26 Octover 2002
  if (AutoEnlarge) result = MESSAGE_IDYES;
  else result = UI_MessageBox(msg,NULL,MESSAGE_YESNO);
  //if (result == MESSAGE_IDNO)
  //{
  //  m_noEnlarge |= 1 << dbNum;
  //  return -1;
  //};
  newSize = m_numEnt[dbNum] + 100;
  m_Address[dbNum] =
     (DBCOMMON *)UI_realloc(m_Address[dbNum],
         newSize*dbEntrySizes[dbNum],
         MALLOC064);
  if (m_Address[dbNum] == NULL)
  {
    UI_MessageBox("Cannot allocate memory",NULL,MESSAGE_OK);
    die(0);
  };
  //d.dungeonDatIndex[dbType+6] = sw(max);
  d.dungeonDatIndex->DBSize(dbNum, uw(newSize));
  // We need to clear all the entries.
  iNew = m_numEnt[dbNum]; // First entry to clear
  m_numEnt[dbNum] = newSize;
  for (i=iNew; i<newSize; i++)
  {
    //pC = GetCommonAddress(RN(0,dbNum,i));
    pC = (DBCOMMON *)((pnt)m_Address[dbNum] + i*dbEntrySizes[dbNum]);
    memset((char *)pC,0,dbEntrySizes[dbNum]);
    pC->link(RNnul);
  };
  //pC = GetCommonAddress(RN(0,dbNum,iNew));
  pC = (DBCOMMON *)((pnt)m_Address[dbNum] + iNew*dbEntrySizes[dbNum]);
  pC->link(RNeof);
  return iNew;
}



DBTYPE DATABASES::GetDBType(DBCOMMON *pDB)
{
  i32 dbNum;
  for (dbNum=0; dbNum<16; dbNum++)
  {
    if (  (pDB >= m_Address[dbNum])
        &&(pDB < m_Address[dbNum] + dbEntrySizes[dbNum]/sizeof(*m_Address[0]) * m_numEnt[dbNum])) return (DBTYPE)dbNum;
  };
  return (DBTYPE)dbNum;
}

bool DATABASES::IsDBType(DBCOMMON *pDB, DBTYPE type)
{
  return GetDBType(pDB) == type;
}

i32 DATABASES::GetDBIndex(DBCOMMON *pDB, DBTYPE dbType)
{
  return ((char *)pDB - (char *)m_Address[dbType])/dbEntrySizes[dbType];
}

RN DATABASES::AllocateSegment(i32 /*dbNum*/, DBTYPE /*dbType*/)
{
  NotImplemented(0x7ea8);
  /*
  i32 entrySize;
  ASSERT(m_Address[dbNum] == NULL);
  ASSERT(m_numEnt[dbNum] == 0);
  entrySize = dbEntrySizes[dbType];
  m_Address[dbNum] = (DBCOMMON *)UI_malloc(2*entrySize);
  m_numEnt[dbNum] = 2;
  dataMap[dbNum] = dbType;
  return RN(0, dbNum, 1);
  */
  return RNnul;
}

// TAG00a0b0
RN DATABASES::FindEmptyDBEntry(DBTYPE dbType, bool important)
{//(RN)
  // important means to assign even if database is
  // nearly full.  Only used to assign fallen hero's bones
  // as far as I know.
  RN result;
  i32 newDML, index;
  DBCOMMON *pDBC;
  i32 entrySize;
  i32 i;
  i32 emptyIndexEntry, emptyDBEntry;
  ASSERT((dbType&0x7fff) < 16,"dbtype");
  entrySize = dbEntrySizes[dbType];
  emptyDBEntry = -1;
  emptyIndexEntry = -1;
  for (i=1; i<dataMapLength; i++)
  {
    if (dataIndexMap[i] == 0xffff)
    {
      if (emptyIndexEntry < 0)
      {
        emptyIndexEntry = i;
        if (emptyDBEntry != -1) break;
      };
      continue;
    };
    if ( (dataTypeMap[i] & 15) != dbType) continue;
    pDBC = (DBCOMMON *)((pnt)m_Address[dbType] + entrySize*dataIndexMap[i]);
    if (pDBC->link() == RNnul)
    {
      emptyDBEntry = dataIndexMap[i];
      emptyIndexEntry = i;
      break;
    };
  };
  if (emptyDBEntry == -1)
  {
    pDBC = (DBCOMMON *)((pnt)m_Address[dbType] + entrySize);
    for (i=1; i<m_numEnt[dbType]; i++)
    {
      if (pDBC->link() == RNnul)
      {
        emptyDBEntry = i;
        break;
      };
      pDBC = (DBCOMMON *)((pnt)pDBC + entrySize);
    };
  };
  if (emptyDBEntry == -1)
  {
    index = db.Enlarge(dbType);
    if (index != -1) return RN(dbType,index);
    return RNnul;
  };
  if (emptyIndexEntry == -1)
  {
    if (dataMapLength > 0xff00 - 100)
    {
      if (!important) return RNnul;
    };
    newDML = 32*(dataMapLength/32) + 33;
    dataIndexMap = (ui16 *)UI_realloc(dataIndexMap, 2*newDML, MALLOC065);
    dataTypeMap  = (ui8  *)UI_realloc(dataTypeMap, newDML, MALLOC066);
    emptyIndexEntry = dataMapLength;
    dataMapLength++;
    dataIndexMap[emptyIndexEntry] = 0xffff;
  };
  dataIndexMap[emptyIndexEntry] = (ui16)emptyDBEntry;
  dataTypeMap[emptyIndexEntry] = (ui8)dbType;
  pDBC = (DBCOMMON *)((pnt)m_Address[dbType]+emptyDBEntry*entrySize);
  ClearMemory((ui8 *)pDBC, entrySize);
  pDBC->link(RNeof);
  result.ConstructFromInteger(emptyIndexEntry);
  return result;
}

void DeleteDBEntry(DBCOMMON *pDB)
{
  DBTYPE dbType;
  i32 dbIndex;
  i32 i, j;
  dbType = db.GetDBType(pDB);
  if (dbType == dbUNKNOWN) return;
  dbIndex = db.GetDBIndex(pDB, dbType);
  for (i=0, j=-1; i<dataMapLength; i++)
  {
    if ((dataTypeMap[i] & 0xf) != dbType) continue;
    if (dataIndexMap[i] != dbIndex) continue;
    if (j != -1) return; //Cloned!!!
    j = i;
    if ((dataTypeMap[i] & 0x80) != 0) return; //Cloned!!!
  };
  pDB->link(RNnul);
  if (j != -1)
  {
    dataTypeMap[j] = 0xff;
    dataIndexMap[j] = 0xffff;
  };
}

void DATABASES::swapDOOR(i32 dbNum)
{
  DB0 *pDB0;
  pDB0 = m_Address[dbNum]->CastToDB0();
  for (i32 i=0; i<m_numEnt[dbNum]; i++)
  {
    pDB0[i].swap();
  };
}

void DATABASES::swapTELEPORTER(i32 dbNum)
{
  DB1 *pDB1;
  pDB1 = m_Address[dbNum]->CastToDB1();
  for (i32 i=0; i<m_numEnt[dbNum]; i++)
  {
    pDB1[i].swap();
  };
}

void DATABASES::swapTEXT(i32 dbNum)
{
  DB2 *pDB2;
  pDB2 = m_Address[dbNum]->CastToDB2();
  for (i32 i=0; i<m_numEnt[dbNum]; i++)
  {
    pDB2[i].swap();
  };
}

void DATABASES::swapACTUATOR(i32 dbNum)
{
  DB3 *pDB3;
  pDB3 = m_Address[dbNum]->CastToDB3();
  for (i32 i=0; i<m_numEnt[dbNum]; i++)
  {
    pDB3[i].swap();
  };
}



void DATABASES::swapMONSTER(i32 dbNum)
{
  DB4 *pDB4;
  pDB4 = m_Address[dbNum]->CastToDB4();
  for (i32 i=0; i<m_numEnt[dbNum]; i++)
  {
    pDB4[i].swap();
  };
}

void DATABASES::swapWEAPON(i32 dbNum)
{
  DB5 *pDB5;
  pDB5 = m_Address[dbNum]->CastToDB5();
  for (i32 i=0; i<m_numEnt[dbNum]; i++)
  {
    pDB5[i].swap();
  };
}

void DATABASES::swapCLOTHING(i32 dbNum)
{
  DB6 *pDB6;
  pDB6 = m_Address[dbNum]->CastToDB6();
  for (i32 i=0; i<m_numEnt[dbNum]; i++)
  {
    pDB6[i].swap();
  };
}

void DATABASES::swapSCROLL(i32 dbNum)
{
  DB7 *pDB7;
  pDB7 = m_Address[dbNum]->CastToDB7();
  for (i32 i=0; i<m_numEnt[dbNum]; i++)
  {
    pDB7[i].swap();
  };
}

void DATABASES::swapPOTION(i32 dbNum)
{
  DB8 *pDB8;
  pDB8 = m_Address[dbNum]->CastToDB8();
  for (i32 i=0; i<m_numEnt[dbNum]; i++)
  {
    pDB8[i].swap();
  };
}

void DATABASES::swapCHEST(i32 dbNum)
{
  DB9 *pDB9;
  pDB9 = m_Address[dbNum]->CastToDB9();
  for (i32 i=0; i<m_numEnt[dbNum]; i++)
  {
    pDB9[i].swap();
  };
}

void DATABASES::swapMISC(i32 dbNum)
{
  DB10 *pDB10;
  pDB10 = m_Address[dbNum]->CastToDB10();
  for (i32 i=0; i<m_numEnt[dbNum]; i++)
  {
    pDB10[i].swap();
  };
}

void DATABASES::swapEXPOOL(i32 dbNum)
{
  DB11 *pDB11;
  pDB11 = m_Address[dbNum]->CastToDB11();
  for (i32 i=0; i<m_numEnt[dbNum]; i++)
  {
    pDB11[i].swap();
  };
}

void DATABASES::swapMISSILE(i32 dbNum)
{
  DB14 *pDB14;
  pDB14 = m_Address[dbNum]->CastToDB14();
  for (i32 i=0; i<m_numEnt[dbNum]; i++)
  {
    pDB14[i].swap();
  };
}

void DATABASES::swapCLOUD(i32 dbNum)
{
  DB15 *pDB15;
  pDB15 = m_Address[dbNum]->CastToDB15();
  for (i32 i=0; i<m_numEnt[dbNum]; i++)
  {
    pDB15[i].swap();
  };
}




void DATABASES::swap(i32 dbNum)
{
  switch (dbNum)
  {
    // We need to littleEndian some of the words in these
    // various databases.
  case dbDOOR:       swapDOOR(dbNum);       break;
  case dbTELEPORTER: swapTELEPORTER(dbNum); break;
  case dbTEXT:       swapTEXT(dbNum);       break;
  case dbACTUATOR:   swapACTUATOR(dbNum);   break;
  case dbMONSTER:    swapMONSTER(dbNum);    break;
  case dbWEAPON:     swapWEAPON(dbNum);     break;
  case dbCLOTHING:   swapCLOTHING(dbNum);   break;
  case dbSCROLL:     swapSCROLL(dbNum);     break;
  case dbPOTION:     swapPOTION(dbNum);     break;
  case dbCHEST:      swapCHEST(dbNum);      break;
  case dbMISC:       swapMISC(dbNum);       break;
  case dbEXPOOL:     swapEXPOOL(dbNum);     break;
  case db12:                                break;
  case db13:                                break;
  case dbMISSILE:    swapMISSILE(dbNum);    break;
  case dbCLOUD  :    swapCLOUD(dbNum);      break;
  default: ASSERT(0,"dbNum");
  };
}


bool DBCOMMON::IsDBType(DBTYPE dbtype)
{
  return db.IsDBType(this, dbtype);
}

DB0 *DBCOMMON::CastToDB0()
{
  ASSERT(IsDBType(dbDOOR),"door");
  return (DB0 *)this;
}

DB1 *DBCOMMON::CastToDB1()
{
  ASSERT(IsDBType(dbTELEPORTER),"teleporter");
  return (DB1 *)this;
}

DB2 *DBCOMMON::CastToDB2()
{
  ASSERT(IsDBType(dbTEXT),"text");
  return (DB2 *)this;
}

DB3 *DBCOMMON::CastToDB3()
{
  ASSERT(IsDBType(dbACTUATOR),"actuator");
  return (DB3 *)this;
}

DB4 *DBCOMMON::CastToDB4()
{
  ASSERT(IsDBType(dbMONSTER),"monster");
  return (DB4 *)this;
}

DB5 *DBCOMMON::CastToDB5()
{
  ASSERT(IsDBType(dbWEAPON),"weapon");
  return (DB5 *)this;
}

DB6 *DBCOMMON::CastToDB6()
{
  ASSERT(IsDBType(dbCLOTHING),"clothing");
  return (DB6 *)this;
}

DB7 *DBCOMMON::CastToDB7()
{
  ASSERT(IsDBType(dbSCROLL),"scroll");
  return (DB7 *)this;
}

DB8 *DBCOMMON::CastToDB8()
{
  ASSERT(IsDBType(dbPOTION),"potion");
  return (DB8 *)this;
}

DB9 *DBCOMMON::CastToDB9()
{
  ASSERT(IsDBType(dbCHEST),"chest");
  return (DB9 *)this;
}

DB10 *DBCOMMON::CastToDB10()
{
  ASSERT(IsDBType(dbMISC),"misc");
  return (DB10 *)this;
}

DB11 *DBCOMMON::CastToDB11()
{
  ASSERT(IsDBType(dbEXPOOL),"expool");
  return (DB11 *)this;
}

DB14 *DBCOMMON::CastToDB14()
{
  ASSERT(IsDBType(dbMISSILE),"missile");
  return (DB14 *)this;
}

DB15 *DBCOMMON::CastToDB15()
{
  ASSERT(IsDBType(dbCLOUD),"cloud");
  return (DB15 *)this;
}

void DBCOMMON::swapLink()
{
  m_link.swap();
}

void DB0::swap()
{
  swapLink();
  word2 = LE16(word2);
}

void DB1::swap()
{
  swapLink();
  word2 = LE16(word2); // x, y, flags
  word4 = LE16(word4); // level
}

void DB2::swap()
{
  swapLink();
  word2 = LE16(word2); // x, y, flags
}

void DB2::Clear()
{
  m_link = RNeof;
  word2 = 0;
}

void DB3::Clear()
{
  m_link = RNeof;
  word2 = 0;
  word4 = 0;
  word6 = 0;
  word8 = 0;
}

void DB3::swap()
{
  swapLink();
  word2 = LE16(word2); // x, y, flags
  word4 = LE16(word4);
  word6 = LE16(word6);
}

void DB3::MakeBig()
{
  word8 = 0;
  if (actuatorType() != 47) return; //If not DSA
  word8 = (ui16)(((word6 >> 12) & 0xc0) | ((word4 >> 14) & 0x03));
  word6 &= 0x3fff;
  word4 &= 0x3fff;
}

ui32 DB3::ParameterA()
{
  return (ui16)word4 | ((word8 & 3) << 16);
}

ui32 DB3::ParameterB()
{
  return (ui16)word6 | ((word8 & 0x0c) << 14);
}

void DB3::ParameterA(ui32 v)
{
  word4 = (ui16)v;
  word8 = (ui16)((word8 & 0xfffc) | ((v >> 16) & 3));
}

void DB3::ParameterB(ui32 v)
{
  word6 = (ui16)v;
  word8 = (ui16)((word8 & 0xfff3) | ((v >> 14) & 0x0c));
}

void DB4::swap()
{
  swapLink();
  possession2.swap();
  hitPoints6[0]  = LE16(hitPoints6[0]);
  hitPoints6[1]  = LE16(hitPoints6[1]);
  hitPoints6[2]  = LE16(hitPoints6[2]);
  hitPoints6[3]  = LE16(hitPoints6[3]);
  word14 = LE16(word14);
}

void DB4::groupIndex(ui32 i)
{
  if (TimerTraceActive)
  {
    if ((i32)this == 0x87e03c)
    {
      fprintf(GETFILE(TraceFile),"Set 105f groupIndex to %02x\n",i);
    };
  };
  uByte5 = (ui8)i;
};


void DB4::Clear()
{
  m_link = RNeof;
  possession2 = RNeof;
  uByte4 = 0;
  uByte5 = 0;
  hitPoints6[0] = 0;
  hitPoints6[1] = 0;
  hitPoints6[2] = 0;
  hitPoints6[3] = 0;
  word14 = 0;
}

void DB5::swap()
{
  swapLink();
  word2  = LE16(word2);
}

void DB5::Clear()
{
  m_link = RNeof;
  word2 = 0;
}

void DB6::swap()
{
  swapLink();
  word2  = LE16(word2);
}

void DB6::Clear()
{
  m_link = RNeof;
  word2 = 0;
}


void DB7::swap()
{
  swapLink();
  m_RNtext.LittleEndian();
  m_word4  = LE16(m_word4);
}

void DB8::Clear()
{
  m_link = RNeof;
  word2 = 0;
}

void DB8::swap()
{
  swapLink();
  word2  = LE16(word2);
}

void DB9::swap()
{
  swapLink();
  m_contents.swap();
  m_word4 = LE16(m_word4);
}

void DB10::swap()
{
  swapLink();
  word2  = LE16(word2);
}

void DB10::Clear()
{
  m_link = RNeof;
  word2 = 0;
}

void DB11::swap()
{
  int i;
  swapLink();
  size = LE16(size);
  for (i=0; i<63; i++)
  {
    d[i] = LE32(d[i]);
  };
}

void DB14::swap()
{
  swapLink();
  m_obj2.swap();
  m_timerIndex  = LE16(m_timerIndex);
}

void DB14::Clear()
{
  m_link = RNeof;
  m_obj2 = RNeof;
  m_rangeRemaining = 0;
  m_damageRemaining = 0;
  m_timerIndex = 0;
}

void DB15::swap()
{
  swapLink();
  word2  = LE16(word2);
}

void DB15::Clear()
{
  m_link = RNeof;
  word2 = 0;
}


EXPOOL::EXPOOL()
{
  m = NULL;
}

EXPOOL::~EXPOOL()
{
}

void EXPOOL::Setup()
{
  if (db.NumEntry(dbEXPOOL) == 0)
  {
    m = (ui32 *)UI_malloc(256, MALLOC085);
    memset(m, 0 , 256);
    db.SetAddress(dbEXPOOL, (DBCOMMON *)m);
    ((DB11 *)m)->link(RNeof);
    ((DB11 *)m)->size = 0;
    db.NumEntry(dbEXPOOL, 1);
  }
  else
  {
    m = (ui32 *)db.GetCommonAddress(dbEXPOOL, 0);
  };
}


void EXPOOL::Cleanup()
{
  m = NULL;
}

void EXPOOL::Write(ui32 key, ui32 *record, ui32 size)
{
  ui32 hash = key * 0xbb40e62d;
  ui32 hashi = 32 + (hash >> 27);
  ui32 result;
  ASSERT(size <= 29, "size");
  if (m[size+2] == 0) enlarge(size+2);
  result = m[size+2];
  m[size+2] = m[result];
  if ((m[hashi] & 0x80000000) != 0)
  {
    hashi = (m[hashi] & 0x7fffffff) + ((hash >> 21) & 0x3f);
  };
  m[result] = m[hashi];
  m[hashi] = result;
  m[result+1] = key;
  memcpy(m+result+2, record, 4*size);
}

i32 EXPOOL::Read(ui32 key, ui32 *record, ui32 max)
{ //Returns size of record (or -1)
  ui32 hash = key * 0xbb40e62d;
  ui32 hashi = 32 + (hash >> 27);
  ui32 p, size, len;
  if ((m[hashi] & 0x80000000) != 0)
  {
    hashi = (m[hashi] & 0x7fffffff) + ((hash >> 21) & 0x3f);
  };
  while (m[hashi] != 0)
  {
    p = m[hashi];
    if (m[p+1] == key)
    {
      size = ((DB11 *)(m+(p & 0xffffffc0)))->size;
      len = size-2;
      if (len > max) len = max;
      memcpy(record, m+p+2, 4*(len));
      m[hashi] = m[p];
      m[p] = m[size];
      m[size] = p;
      return size-2;
    };
    hashi = m[hashi];
  };
  return -1;
}

bool EXPOOL::IsOpen()
{
  return m!=NULL;
}

i32 EXPOOL::Locate(ui32 key, ui32 **record)
{ //Returns size of record (or -1)
  ui32 hash = key * 0xbb40e62d;
  ui32 hashi = 32 + (hash >> 27);
  ui32 p, size;
  if (m == NULL) return -1;
  if ((m[hashi] & 0x80000000) != 0)
  {
    hashi = (m[hashi] & 0x7fffffff) + ((hash >> 21) & 0x3f);
  };
  while (m[hashi] != 0)
  {
    p = m[hashi];
    if (m[p+1] == key)
    {
      size = ((DB11 *)(m+(p & 0xffffffc0)))->size;
      *record = m+p+2;
      //memcpy(record, m+p+2, 4*(size-2));
      //m[hashi] = m[p];
      //m[p] = m[size];
      //m[size] = p;
      return size-2;
    };
    hashi = m[hashi];
  };
  return -1;
}

void EXPOOL::enlarge(ui32 size)
{
  ui32 n, p, i;
  ui32 numBlock;
  DB11 *pDB11;
  numBlock = db.NumEntry(dbEXPOOL);
  numBlock++;
  m = (ui32 *)UI_realloc(m, 256*numBlock, MALLOC086);
  p = 64*(numBlock-1) + 1;
  //e = p+64;
  n = 63 / size;
  pDB11 = (DB11 *)m + numBlock-1;
  memset(pDB11, 0,256);
  pDB11->link(RNeof);
  pDB11->Size(size);
  for (i=0; i<n; i++, p+=size)
  {
    m[p] = p+size;
  };
  m[p-size] = m[size];
  m[size] = 64*(numBlock-1) + 1;
  db.NumEntry(dbEXPOOL, numBlock);
  db.SetAddress(dbEXPOOL, (DBCOMMON *)m);
}


void EXPOOL::AddChampionBonesRecord(const RN& bonesObject, ui32 fingerprint)
{
  ui32 record[1];
  ui32 key;
  key = (EDT_ChampionBones << 24) | (bonesObject.ConvertToInteger() & 0xffff);
  record[0] = fingerprint & 0xffff;
  Write(key, record, 1);
}

bool EXPOOL::CheckChampionBonesRecord(const RN& bonesObject)
{
  ui32 *pRecord;
  ui32 key;
  i32 size;
  key = (EDT_ChampionBones << 24) | (bonesObject.ConvertToInteger() & 0xffff);
  size = Locate(key, &pRecord);
  return size == 1;
}

bool EXPOOL::CopyChampionBonesRecord(const RN& bonesObject, ui32 *pResult)
{
  ui32 *pRecord;
  ui32 key;
  i32 size;
  key = (EDT_ChampionBones << 24) | (bonesObject.ConvertToInteger() & 0xffff);
  size = Locate(key, &pRecord);
  if (size != 1) return false;
  *pResult = *pRecord;
  return true;
}

bool EXPOOL::GetChampionBonesRecord(const RN& bonesObject, ui32 *pResult)
{
  ui32 key;
  i32 size;
  key = (EDT_ChampionBones << 24) | (bonesObject.ConvertToInteger() & 0xffff);
  size = Read(key, pResult, 1);
  return size==1;
}




bool GetExtendedCellFlag(
       i32 level, i32 x, i32 y, EXPOOL_DATA_TYPE ftype)
{
  ui32 *record;
  i32 len;
  len = expool.Locate(
                (EDT_ExtendedCellFlags<<24) + (level<<5) + x,
                &record);
  if (len < 8) return false;
  return (record[ftype] & (1<<y)) != 0;
}

// *********************************************************
// End of struct DATABASES member functions
// *********************************************************

void ATTRIBUTE::Current(i32 value)
{
  ubCurrent = ub(value);
}

void ATTRIBUTE::Maximum(i32 value)
{
  ubMaximum = ub(value);
}

void ATTRIBUTE::Minimum(i32 value)
{
  ubMinimum = ub(value);
}

ui8 ATTRIBUTE::Maximum()
{
  if (ubMaximum > 240) return 0; //It went negative
  return ubMaximum;
}

ui8 ATTRIBUTE::Minimum()
{
  if (ubMinimum > 240) return 0; //It went negative
  return ubMinimum;
}

ui8 ATTRIBUTE::Current()
{
  if (ubCurrent > 240) return 0; //It went negative
  return ubCurrent;
}

void ATTRIBUTE::IncrMinimum(i32 incr)
{
  ubMinimum = (UI8)(ubMinimum + incr);// It can go negative!
}

void ATTRIBUTE::IncrMaximum(i32 incr)
{
  ubMaximum = ub(ubMaximum + incr);
}

void ATTRIBUTE::IncrCurrent(i32 incr)
{
  ubCurrent = ub(ubCurrent + incr);
}




bool verticalIntEnabled = false;
i32 NoSpeedLimit= 0;
i32 VBLMultiplier = 1;
bool NoClock=false;
i32 NumberFloppyDrives = 1;
i32 dbgcstint;
i32 GameTime;

i32 keyboardMode=1; // 1 while adventuring
                    // 2 while ReIncarnation

i32 data000b2c; // Contains A4 for use by interrupt handler
i32 data000b30; // Contains A5 for use by interrupt handler

DIRECTION dirNORTH = 0;
DIRECTION dirEAST  = 1;
DIRECTION dirSOUTH = 2;
DIRECTION dirWEST  = 3;

const char *tracePrefix = "                                      ";
const char *initialTracePrefix(bool trace)
{
  return trace?tracePrefix+strlen(tracePrefix)-2:NULL;
}
const char *nextTracePrefix(const char *trace)
{
  return trace==NULL?NULL:trace-2;
}

void DUNGEONDATINDEX::Swap()
{
  i32 i;
  sentinel = LE16(sentinel);
  word2 = LE16(word2);
  word4 = LE16(word4);
  word6 = LE16(word6);
  word8 = LE16(word8);
  word10= LE16(word10);
  for (i=0; i<16; i++)
  {
    dbSizes[i] = LE16(dbSizes[i]);
  };
}



// *********************************************
static i32 WCSRCShandle;
static ui32 WCSRCSchecksum;

ui32 WCS(i32 handle)
{
  ui32 temp;
  WCSRCShandle = handle;
  temp = WCSRCSchecksum;
  WCSRCSchecksum = 0xffff;
  return temp;
}

ui32 RCS(i32 handle)
{
  ui32 temp;
  WCSRCShandle = handle;
  temp = WCSRCSchecksum;
  WCSRCSchecksum = 0xffff;
  return temp;
}

void WCS(ui8 *buf, i32 n)
{
  i32 i;
  for (i=0; i<n; i++)
  {
    WCSRCSchecksum *= (ui32)0xbb40e62d;
    WCSRCSchecksum += 11 + buf[i];
  };
  WRITE((ui16)WCSRCShandle, n, (ui8 *)buf);
}

void RCS(ui8 *buf, i32 n)
{
  i32 i;
  READ(WCSRCShandle, n, (ui8 *)buf);
  for (i=0; i<n; i++)
  {
    WCSRCSchecksum *= (ui32)0xbb40e62d;
    WCSRCSchecksum += 11 + buf[i];
  };
}

void WCS(ui32 *i, i32 n)
{
  WCS((ui8 *)i, 4*n);
}

void RCS(ui32 *i, i32 n)
{
  RCS((ui8 *)i, 4*n);
}

void WCS(i32 *i, i32 n)
{
  WCS((ui8 *)i, 4*n);
}

void RCS(i32 *i, i32 n)
{
  RCS((ui8 *)i, 4*n);
}

void WCS(ui16 *i, i32 n)
{
  WCS((ui8 *)i, 2*n);
}

void RCS(ui16 *i, i32 n)
{
  RCS((ui8 *)i, 2*n);
}

void WCS(char *c, i32 n)
{
  WCS((ui8 *)c, n);
}

void RCS(char *c, i32 n)
{
  RCS((ui8 *)c, n);
}

void CHARDESC::HP(i32 m)
{
  ASSERT((unsigned)m <= MAXHP, "maxhp");
  m_wHP = sw(m);
}

void CHARDESC::MaxHP(i32 m)
{
  ASSERT((unsigned)m <= MAXHP, "maxhp");
  m_wMaxHP = sw(m);
}


void CHARDESC::IncrHP(i32 inc)
{
  m_wHP = sw(m_wHP + inc);
  ASSERT((unsigned)m_wHP <= MAXHP, "maxhp");
}

void CHARDESC::IncrMaxHP(i32 inc)
{
  m_wMaxHP = sw(m_wMaxHP + inc);
  ASSERT((unsigned)m_wMaxHP <= MAXHP, "maxhp");
}

void CHARDESC::Stamina(i32 m)
{
  ASSERT((unsigned)m <= MAXSTAMINA, "maxstamina");
  m_wStamina = sw(m);
}

void CHARDESC::MaxStamina(i32 m)
{
  ASSERT((unsigned)m <= MAXSTAMINA, "maxstamina");
  m_wMaxStamina = sw(m);
}

void CHARDESC::IncrMaxStamina(i32 inc)
{
  m_wMaxStamina = sw(m_wMaxStamina + inc);
  ASSERT((unsigned)m_wMaxStamina <= MAXSTAMINA, "maxstamina");
}

void CHARDESC::Mana(i32 m)
{
  ASSERT((unsigned)m <= MAXMANA, "maxmana");
  m_wMana = sw(m);
}

void CHARDESC::MaxMana(i32 m)
{
  ASSERT((unsigned)m <= MAXMANA,"maxmana");
  m_wMaxMana = sw(m);
}

void CHARDESC::IncrMana(i32 inc)
{
  m_wMana = sw(m_wMana + inc);
  ASSERT((unsigned)m_wMana <= MAXMANA,"maxmana");
}

void CHARDESC::IncrMaxMana(i32 inc)
{
  m_wMaxMana = sw(m_wMaxMana + inc);
  ASSERT((unsigned)m_wMaxMana <= MAXMANA,"maxmana");
}

void Signature(int fileHandle, ui32 *signature1, ui32 *signature2)
{//result will be 8 bytes (6-bytes MD5 + 2 zeroes)
  FILE *f;
  ui32 sum;
  i32 len;
  ui8 buf[1000];
  ui8 result[16];
  f = GETFILE(fileHandle);
  if (f == NULL) return;
  sum = 0;
  MD5Init();
  while ( (len=fread(buf,1,1000,f)) > 0)
  {
//    for (i=0; i<len; i++)
//    {
//      sum = (sum & 0x80000000) ? (sum<<1)^0x8d : sum<<1;
//      sum += buf[i];
//    };
    MD5Update(buf, len);
  };
  fseek(f,0,SEEK_SET);
  MD5Final(result);
  *signature1 =  result[0]
              | (result[1]<<8)
              | (result[2]<<16)
              | (result[3]<<24);
  *signature2 =  result[4]
              | (result[5]<<8)
              | (result[6]<<16)
              | (result[7]<<24);
  if (*signature1 == 0) *signature1 = 1; //one in 4 billion
//  return sum == 0 ? 1 : sum;
}

ui32 Signature(char *text)
{
  ui32 sum;
  i32 i;
  sum = 0;
  for (i=0; text[i]!=0; i++)
  {
    sum = (sum & 0x80000000) ? (sum<<1)^0x8d : sum<<1;
    sum += text[i];
  };
  return sum == 0 ? 1 : sum;
}

void FreeIfNonNULL(void **pointer)
{
  if (*pointer != NULL)
  {
    UI_free(*pointer);
    *pointer = NULL;
  };
}

void CleanupGraphics();
void Cleanup(bool programTermination)
{
  int i;
  bool recording;
  recording = IsRecordFileRecording();
  if (programTermination)
  {
    //FreeIfNonNULL((void **)&g_folderParentName);
    //FreeIfNonNULL((void **)&g_folderName);
    g_root.clear();
    FreeIfNonNULL((void **)&globalVariables); numGlobalVariables = 0;
    FreeIfNonNULL((void **)&finalEditText);
    CleanupTranslations();
    gameTimers.Cleanup();
  };
  CleanupGraphics();
  DSAIndex.Cleanup();
  FreeIfNonNULL((void **)&d.Item16);
  d.compressedText.clear();
  d.indirectTextIndex.clear();
  FreeIfNonNULL((void **)&gameInfo);
  d.objectList.clear();
  FreeIfNonNULL((void **)&dataIndexMap);
  FreeIfNonNULL((void **)&dataTypeMap);
  StartSound(NULL,0,0);
  expool.Cleanup();
  for (i=0; i<16; i++)
  {
    db.DeAllocate(i);
  };
  CleanupCustomPhrases();
  gameTimers.Cleanup();
  if (TimerTraceActive)
  {
    CloseTraceFile();
    TimerTraceActive = false;
  };

//  if (recording)
//  {
//    RecordFile.Close();
//  };
  FILETABLECleanup();
  AtariMemCleanup();
  //if (TimerTraceActive && !programTermination)
  //{
  //  TimerTraceActive = false;
  //  OpenTraceFile();
  //  TimerTraceActive = true;\
  //};
//  if (recording && !programTermination)
//  {
//    RecordFile.Open();
//  };
}


SKIN_CACHE skinCache;


i32 SKIN_CACHE::Load(i32 level, i32 x)
{
  ui32 *pExpoolRecord;
  i32 i, column, recId, recSize;
  if (level != m_level)
  {
    m_level = level;
    for (i=0; i<16; i++)
    {
      m_size[i] = -1;
    };
  };
  column = x / 2;
  if (m_size[column] == 0) return column;
  if (m_size[column] == -1)
  {
    recId = (EDT_Skins << 24) + (level << 4) + (x / 2);
    recSize = 4* expool.Locate(recId, &pExpoolRecord);
    if (recSize <= 0) recSize = 0;
    m_size[column] = (i8)recSize;
    if (m_size[column] == 0) return column;
    if (m_size[column] > 64) m_size[column] = 64;
    memcpy(m_columns[column], pExpoolRecord, m_size[column]);
  };
  return column;
}

ui8 SKIN_CACHE::GetSkin(i32 level, i32 x, i32 y)
{
  if (level < 0) return 0;
  i32 column, index;
  if ( x < 0 )
  {
    return 0;
  };
  if ( y < 0 )
  {
    return 0;
  };
  if ( x > d.pLevelDescriptors[level].LastColumn())
  {
    return 0;
  };
  if ( y > d.pLevelDescriptors[level].LastRow())
  {
    return 0;
  };
  column = Load(level, x);
  if (m_size[column] == 0) return 0;
  index = (2 * y) + (x & 1);
  if (index >= m_size[column]) return 0;
  return m_columns[column][index];
}

ui8 SKIN_CACHE::GetDefaultSkin(i32 level)
{
  ui32 recId;
  ui32 *pExpoolRecord;
  i32 recSize;
  if (!expool.IsOpen()) return 0;
  if (defaultSkins == NULL)
  {
    defaultSkins = (ui8 *)UI_malloc(64, MALLOC098);
    memset(defaultSkins, 0, 64);
    recId = (EDT_Skins << 24) + 0x800000;
    recSize = 4 * expool.Locate(recId, &pExpoolRecord);
    if (recSize > 0)
    {
      if (recSize > 64) recSize = 64;
      memcpy(defaultSkins, pExpoolRecord, recSize);
    };
  };
  return defaultSkins[level];
}



void SKIN_CACHE::SetSkin(i32 level, i32 x, i32 y, i8 skinNum)
{
  i32 i, column, index, recId, recSize;
  column = Load(level, x);
  index = (2 * y) + (x & 1);
  if ( index < m_size[column])
  {
    if (skinNum == m_columns[column][index]) return;
  }
  else
  {
    if (skinNum == 0) return;
  };
  //return;
  // pDoc->SetModifiedFlag();
  recId = (EDT_Skins << 24) + (level << 4) + (x / 2);
  recSize = 4 * expool.Read(recId, (ui32 *)m_columns[column], 64); // read and delete old record
  if (recSize < 0) recSize = 0;
  if (recSize > 64) recSize = 64;
  
  // Expand the cache record to size = 64
  // works return;
  if (recSize < 64) memset(m_columns[column]+recSize, 0, 64-recSize);
  // works return;
  m_columns[column][index] = skinNum;
  // works return;
  for (i=63; i>=0; i--)
  {
    if (m_columns[column][i] != 0) break;
  };
  if (i < 0) return;
  expool.Write(recId, (ui32 *)m_columns[column], (i+4)/4);
  m_size[column] = (i8)(i+1);
}
