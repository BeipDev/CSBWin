#ifndef __TYPES__
#define __TYPES__



#ifdef TARGET_OS_MAC
	#if __MWERKS__
		#pragma reverse_bitfields	on
	#else
		#error All bitfields must be reversed in order to run!
	#endif
#define _bigEndian
// File structures that are saved in little-endian
// format must be converted to big-endian on the macintosh.
#endif

#if defined _MSVC_INTEL || defined _MSVC_CE2002ARM
#define _littleEndian
#endif

#ifdef _LINUX
# if (G_BYTE_ORDER == G_LITTLE_ENDIAN)
#  define _littleEndian
# endif
# if (G_BYTE_ORDER == G_BIG_ENDIAN)
#  define _bigEndian
# endif
#endif


#define         _max(a,b) (((a)>(b))?(a):(b))
#define         _min(a,b) (((a)<(b))?(a):(b))


// Format string for 64 bit integers\r
#if defined _MSVC_INTEL || defined _MSVC_CE2002ARM
#define INT64_FMT "%I64d"
#else
#define INT64_FMT "%lld"
#endif

#if defined _MSVC_INTEL || defined _MSVC_CE2002ARM 
typedef signed char i8;     // signed 8-bit integer
typedef unsigned char ui8;
typedef _int16 i16;  // signed 16-bit integer
typedef unsigned _int16 ui16;
typedef int i32;  // signed 32-bit integer
typedef unsigned int ui32;
typedef _int64 i64; //signed 64-bit integer
typedef unsigned _int64 ui64; //unsigned 64-bit integer
typedef i8 *pnt;
typedef ui8 *upnt;
typedef unsigned int bool32;
typedef unsigned _int16 HTIMER;  // A handle to a timer entry;
#else
# ifdef _LINUX
  typedef guint32 HWND;
  typedef void* HDC; // I might change this later...
  typedef signed char i8;     // signed 8-bit integer
  typedef unsigned char ui8;
  typedef gint16 i16;  // signed 16-bit integer
  typedef guint16 ui16;
  typedef gint32 i32;  // signed 32-bit integer
  typedef guint32 ui32;
  typedef gint64 i64; //signed 64-bit integer
  typedef i8 *pnt;
  typedef ui8 *upnt;
  typedef guint64 ui64;
  typedef guint32 bool32;
  typedef guint16 HTIMER;
  typedef struct
  {
      i32 x;
      i32 y;
  } POINT;
  typedef struct
  {
      i32 right;
      i32 left;
      i32 top;
      i32 bottom;
  } RECT;
# else
  typedef char i8;     // signed 8-bit integer
  typedef unsigned char ui8;
  typedef short i16;  // signed 16-bit integer
  typedef unsigned short ui16;
  typedef long i32;  // signed 32-bit integer
  typedef unsigned long ui32;
  typedef long long i64; //signed 64-bit integer
  typedef i8 * pnt;
  typedef ui8 * upnt;
# endif
#endif

void _Assert(bool, char *, i32, const char * = NULL);

#undef ASSERT
#ifdef _DEBUG
#define ASSERT(value, text) _Assert((value),__FILE__,__LINE__,text)
#else
#define ASSERT(value, text) ;
#endif

#define MAX_SCROLL_LINE_LENGTH 16
#define MAX_LINES_IN_SCROLL 9


// **********  How to make small integers from big integers

// The cast operators such as (i16) should not appear in the code!

#define I8   i8
#define UI8  ui8   // Use these when converting to a smaller
#define I16  i16   // type and you either:
#define UI16 ui16  //  - Don't care if it fits
                   //  - You can prove it fits



#ifdef _DEBUGCAST //Use these when you want to make
                  //a smaller integer and you would like
                  //us to check that it will fit.
extern i32 dbgcstint;
i8  _sb(i32 i, i32 line, char *file);
#define sb(i) (_sb((i),__LINE__,__FILE__))
ui8 _ub(i32 i, i32 line, char *file);
#define ub(i) (_ub((i),__LINE__,__FILE__))
i16 _sw(i32 i, i32 line, char *file);
#define sw(i) (_sw((i),__LINE__,__FILE__))
ui16 _uw(i32 i, i32 line, char *file);
#define uw(i) (_uw((i),__LINE__,__FILE__))
#else
#define sb(x) ((i8)(x))     //If no _DEBUGCAST then we simply
#define ub(x) ((ui8)(x))    //assume that all is well.
#define sw(x) ((i16)(x))
#define uw(x) ((ui16)(x))
#endif

#ifdef _littleEndian
//#define LE16(W)(((ui16)((((ui16)(W))*0x10001)>>8)&0xffff))
// The above definition caused Visoual Studio Community 2013 C++ 
// compiler to do something very clever but wrong when adding 128
// words in a loop.  Strange instructions I have never seen.  I think that
// carrys were being added.  Or something.  SO I did this instead.
inline ui16 LE16(ui16 W) {
  return ((ui16)((((W)& 0xff00) >> 8) | (((W)& 0xff) << 8)));
};
i32 LE32(i32 lng);
#else
#define LE16(x) ((ui16)(x))
#define LE32(x) (x)
#endif

#ifdef _bigEndian
#define BE16(W)((ui16)((((ui16)(W))*0x10001)>>8))
i32 BE32(i32 lng);
#else
#define BE16(x) ((ui16)(x))
#define BE32(x) (x)
#endif


enum STATE_OF_FEAR
{
  StateOfFear0 = 0,
  StateOfFear2 = 2,
  StateOfFear3 = 3,
  StateOfFear4 = 4,
  StateOfFear5 = 5,
  StateOfFear6 = 6,
  StateOfFear7 = 7,
};
  

#pragma pack(1)
typedef ui8 CELLFLAG;
    // For roomtype = roomSTONE
    //    bit 0 = Put random decoration on west side
    //    bit 1 = Put random decoration on south side
    //    bit 2 = Put random decoration on east side
    //    bit 3 = Put random decoration on north side
    //  Cells just outside the boundary of the level are
    //  assumed to be STONE with the proper random bit set.
    // For roomtype = roomDOOR
    //    bits 0-2 = door state (0, 1, or 5 == allow passage)
    //                  (I have yet to see the value of 1!)
    //                  (0, 4, and 5 are all I have seen).
    //              00 = open
    //              01 = almost open
    //              02 = half open
    //              03 = almost closed
    //              04 = closed
    //              05 = door has been bashed open.
    //    bit 3 = N/S door (direction of travel through door)
    // For roomtype = roomSTAIRS
    //    bit 2 (0x04) = stairs up
    //    bit 3 (0x08) = stairs north/south
    // For roomtype = roomPIT
    //    bit 0 means False Pit.  Don't fall thru even if open.
    //    bit 2 means Obscure (not obvious)
    //    if (0x08) then pit is open (you will fall through)
    // For roomtype = roomTELEPORTER
    //    bit 2 (0x04) means visible
    //    bit 3 (0x08) means teleporter is active
    // For roomtype = roomFALSEWALL
    //    bit 0 (0x1) means wall is visible but open to party
    //    bit 2 (0x4) means active (wall is invisible and open to party)
    //    /////////////// Don't think this is true ******bit 3 (0x8) active  (like teleporter)
    // bits 5-7 room type (see enum ROOMTYPE)


enum DBTYPE
{
 dbDOOR      =  0,
 dbTELEPORTER=  1,
 dbTEXT      =  2,
 dbACTUATOR  =  3,
 dbMONSTER   =  4,
 dbWEAPON    =  5,
 dbCLOTHING  =  6,
 dbSCROLL    =  7,
 dbPOTION    =  8,
 dbCHEST     =  9,
 dbMISC      = 10,
 dbEXPOOL    = 11,
 db12        = 12,
 db13        = 13,
 dbMISSILE   = 14,
 dbCLOUD     = 15,
 dbUNKNOWN   = 16
};


enum RNVAL
{
  RNeof             =0xfffe,
  RNnul             =0xffff,
  RNFireball        =0xff80, //fireball
  RNPoison          =0xff81,
  RNLightning       =0xff82,
  RNDispellMissile  =0xff83, //dispell - Damage non-Material
  RNZoSpell         =0xff84, //open door?
  RNPoisonBolt      =0xff86,
  RNPoisonCloud     =0xff87, //Poison Cloud
  RNMonsterDeath    =0xffa8, //MonsterDeath
  RNFluxCage        =0xffb2,
  RNffe4            =0xffe4, //Altar of Vi
  RNempty           =0x0000
};

//struct RNBF
//{
//  unsigned short m_idx:10; // Record number within database
//  unsigned short m_db:4;   // Database number
//  unsigned short m_pos:2;  // Position within room
//};

//struct RNRP
//{
//  unsigned short m_rid:14;
//  unsigned short m_pos:2;
//};

extern ui8 *dataTypeMap;
extern ui16 *dataIndexMap;
extern i32 dataMapLength;

class RN // Record Name
{
  friend struct DATABASES;
  //friend class TIMER;
  friend class DBCOMMON;
  friend class DB4;
  friend class DB9;
  friend struct DB14;
  //friend void DumpDB3(FILE *, RN, i32, i32, i32, CELLFLAG);
//  friend i16 TAG0093d4(RN);
  void swap() {indirectIndex=LE16(indirectIndex);};
  bool checkIndirectIndex() const;
private:
  //union {
  //  ui16 ui;
  //  RNBF bf;
  //  RNRP rp;
  //} d;
  ui16 indirectIndex;
  RN(ui32 pos, ui32 db, ui32 idx);
  bool operator >= (RNVAL i) const
  {
    ASSERT(i >= 0xff00, "Illegal RN");
    return indirectIndex>=i;
  };
  bool operator <  (RNVAL i) const
  {
    ASSERT(i >= 0xff00, "Illegal RN");
    return indirectIndex<i;
  };
public:
  RN() {indirectIndex = 0xffff;};
  //{
  //  ASSERT((pos<4) && (db<16) && (idx<1024));
  //  d.bf.m_pos=pos;
  //  d.bf.m_db=db;
  //  d.bf.m_idx=idx;
  //};
  RN(RNVAL i);
  RN(DBTYPE dbType, ui32 dbIndex);
  bool checkIndirectIndex(ui32 i) const;
  void ConstructFromInteger(i32 i)
  {
    indirectIndex = (UI16)(i);
    checkIndirectIndex();
  };
  void NonExtendedInteger(i32 i)
  {
    indirectIndex = (UI16)(i);
  };
  OBJ_NAME_INDEX NameIndex() const;
  OBJ_DESC_INDEX DescIndex() const;
  void CreateSpell(i32 n);  //result is RNFireball + n
  ui8  GetSpellType();
  void LittleEndian() {indirectIndex = LE16(indirectIndex);};
  DBTYPE dbType() const
  {
    if (!checkIndirectIndex()) return dbUNKNOWN;
    if (dataIndexMap[indirectIndex] == 0xffff) return dbUNKNOWN;
    return DBTYPE(dataTypeMap[indirectIndex]&0xf);
  };
  ui8 dbNum() const {return (ui8)dbType();};
  ui8 pos() const
  {
    checkIndirectIndex();
    return (UI8)((dataTypeMap[indirectIndex]>>4) & 3);
  };
  RN pos(ui32 p)
  {
    checkIndirectIndex();
    dataTypeMap[indirectIndex] = (ui8)
      ((dataTypeMap[indirectIndex]&0xcf)|((p&3)<<4));
    return *this;
  };
  ui16 idx() const
  {
    checkIndirectIndex();
    return dataIndexMap[indirectIndex];
  };
  //ui32 rid() const {return d.rp.m_rid;};
  bool operator == (RN rn) const
  {
    return indirectIndex == rn.indirectIndex;
  };
  bool operator != (RN rn) const
  {
    return indirectIndex != rn.indirectIndex;
  };
  bool operator == (RNVAL i) const
  {
    ASSERT((i >= 0xff00)||(i == RNempty), "Illegal RN");
    return indirectIndex==i;
  };
  bool IsSameObjectAs(RN obj) const
  {
    return indirectIndex == obj.indirectIndex;
  };
  bool operator != (RNVAL i) const
  {
    ASSERT((i >= 0xff00)||(i==0), "Illegal RN");
    return indirectIndex!=i;
  };
  ui16 ConvertToInteger() const {return indirectIndex;}; //USE SPARINGLY!
  bool IsMagicSpell(){return indirectIndex>=RNFireball;};
  bool IsNonPhysicalSpell(){return indirectIndex>=RNDispellMissile;};
  bool IsAKey() const;
  bool NonMaterial();  //Must be a monster!
  bool Levitating();   //Must be a monster!
  ui8  VerticalSize(); //Must be a monster!
};

enum PLACE
{
  PLACE_InDungeon = 1
};

struct SRCHPKT
{
  PLACE place;
  ui32  level;
  ui32  x;
  ui32  y;
};


struct BUTTON_DEF
{
  ui8 x1;
  ui8 x2;
  ui8 y1;
  ui8 y2;
};

void DebugLoad(void *, i32, i32, RN);
//#define DEBUGLOAD(a,b,c,d) DebugLoad((a), (b), (c), (d))
#define DEBUGLOAD(a,b,c,d)

#if defined _MSVC_CE2002ARM

class TEMPORARY_CHAR
{
public:
  TEMPORARY_CHAR();
  ~TEMPORARY_CHAR();
  char *text;
  void operator =(unsigned short *wtext);
  char *Text();
};

class TEMPORARY_WIDE
{
public:
  TEMPORARY_WIDE();
  ~TEMPORARY_WIDE();
  unsigned short *text;
  void operator =(const char *atext);
  unsigned short *Text();
};
#endif

#endif //__TYPES__
