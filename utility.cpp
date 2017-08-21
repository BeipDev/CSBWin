#include "stdafx.h"


#include "UI.h"
#include "Dispatch.h"

#include <stdio.h>

//#include "Objects.h"

#include "CSB.h"


#pragma pack(1)  



typedef pnt aReg;

struct STRUCT148
{
  ui8  *((*malloc)(i32 size));                 //0  //148
  void  (*free)(ui8 *address);                  //4  //144
  void  (*seek)(i32 handle, i32 address);      //8  //140
  void  (*read)(i32 handle, i32 size, ui8 *buf);//12 //138
  i32   handle;                                //16 //132      
  i32   i_20;                                  //20 //128
  i8   *p_24;                                  //24 //124
  i8   *pJumpTable;                            //28 //120
  i8   *p_32;                                  //32 //116
  i8   *p_36;                                  //36 //112
};


#define longGear(A) (*((i32 *)(A)))
//i32& longGear(pnt p);
pnt& pntGear(pnt p);
#define littleEndian(W)((ui16)((((ui16)(W))*0x10001)>>8))

#ifdef _MSVC_INTEL
i32 LE32(i32 lng);
#endif

struct ITEM20
{
  i32 Long0;
  i16 Word4;
  i32 Long6;
  i16 Word10;
  i16 Word12;
  i32 Long14;
  i16 Word18;
};


void  SetSupervisorMode(void);
void  ClearSupervisorMode(void);
void  CONOUT(i32) {NotImplemented(0xffff71);};
ui8  *MALLOC(i32 size);

void  MFREE(ui8 *address);
//i32   DIRECT_CONIN(void){NotImplemented(0xffff79); return 0;}; // Trap 1 #0x07
//i16   CONSTAT(void);     // Trap 1 #0x0b
i32   LSEEK(i32 offset, i32 file, i32 origin);
//i16   OPEN(char *name, i32 ref);
i32   READ(i32 file, i32 len, ui8 *buf);
i16   CLOSE(i32 handle);
ui8  *MallocMemory(i32 size);//TAG0000b4
void  FreeMemory(ui8 *address);//TAG0000ec
void  FileSeek(i32 handle, i32 offset);//TAG00010e
void  ReadFile(i32 handle, i32 size, ui8 *buf);//  TAG000144
void  TAG00053a(i16, i32);
void  TAG0007a8(i32);
i32   TAG0007aa(void); // TRAP #1 save registers
void  SetCriticalErrorHandler(void);//TAG0007ce
i16   TAG0009ee(i32);
void  TAG000a60(i32);
i16   LoadProgram(STRUCT148 *s148);//TAG000ad2
i16   CheckSum(i8 *, i8 *);//TAG00127c
i16   TAG0012f6(i8 *, i32);
i16   TAG00132a(i8 *, i32);
//i16   TAG00183c(i16);
void  TAG00187a(i32);
void  TAG001890(i32);
i16   TAG0018d6(ITEM20 *);
i16   TAG001928(ITEM20 *);
i32   TAG001a56(i16, i32, i16);
void  TAG001b6c(i32);
void  TAG001b82(i16 handle);
i16   TAG001cd4(i16, i32, i16);
void  TAG001e5e(void);


#define UTILFILL(a,b) i8 fill##a[a-b];

struct B
{
  i16  Word2132[(2132-1828)/2]; //[76];
  UTILFILL(1980,1828)
  ui16 uWord1828;
  UTILFILL(1826,1752)
  ui16 uWord1752;
  ITEM20 Item20[73];//1750
  i8   Byte290[16]; // "0123456789ABCDEF"
  i8   Byte274[5];  // "A:\F"
  UTILFILL(269,160)
  ui16 uWord160;
  ui16 uWord158;
  UTILFILL(156,112)
  i32  Long112;
  i8   Byte108[13];
  UTILFILL(95,10)
  i32  Long10;
  i16  Word6;
  i32  IBank;  //4 // what we put in A5
};

ui8 *MALLOC(i32 size)
{
  if (size == -1) return (ui8 *)1000000;
  return (ui8 *)UI_malloc(size, MALLOC067);
}

void MFREE(ui8 *address)
{
  UI_free (address);
}

B b;

void SETBLOCK(i8 * /*addr*/, i32 /*size*/, i32 /*zero*/)
{
}

// *********************************************************
//
// *********************************************************
//  TAG0000b4
ui8 *MallocMemory(i32 size)
{
  ui8 *p_4;
  p_4 = MALLOC(size);
  if (p_4 == NULL)
  {
    TAG000a60(1);
  };
  return p_4;
}

// *********************************************************
//
// *********************************************************
//  TAG0000ec
void FreeMemory(ui8 *address)
{
  if (address != NULL)
  {
    MFREE(address);
  };
}

// *********************************************************
//
// *********************************************************
//   TAG00010e
void FileSeek(i32 handle, i32 offset)
{
  dReg D0;
  D0L = LSEEK(offset, handle, SEEK_SET);
  if (D0L != offset)
  {
    TAG000a60(3);
  };
}

// *********************************************************
//
// *********************************************************
//  TAG000144
void ReadFile(i32 handle, i32 size, ui8 * buffer)
{
  dReg D0;
  D0L = READ(handle, size, buffer);
  if (D0L != size)
  {
    TAG000a60(4);
  };
}

// *********************************************************
//
// *********************************************************
static char *TAG0004fe(char *dest, const char *src)
{
  const ui8 *A2;
  ui8 *A3, *p_4;
  
  A3 = (ui8 *)dest;
  A2 = (ui8 *)src;
  p_4 = A3;
  while ((*p_4 = *A2) != 0)
  {
    p_4++;
    A2++;
//
  };
  return (char *)A3;
}

// *********************************************************
//
// *********************************************************
void TAG00053a(i16, i32)
{
  i16 w_160;
  STRUCT148 s_148;
  i8   b_108[13];
  i8  *p_8;
  i16  w_4;
  i16  w_2;
  s_148.p_36 = NULL;
  w_160 = 0;
  TAG0004fe((char *)b_108, "FTLCODE");
  //TAG0004fe(b_108, "CHAOS.FTL");

  while (UI_CONSTAT() != 0)
  {
    UI_DIRECT_CONIN();
//
  };
  b.uWord158 = 0xff62;
  b.uWord160 = 0;
  for (;;)
  {
    SetCriticalErrorHandler();
    if (b.uWord158 != 0xff62) break;
    b.uWord158 = 0;
    b.Long112 = 0;

    w_4 = OPEN((char *)b_108, "rb");
    if (w_4 < 0)
    {
      die(0, "Cannot open file FTLCODE");
    };
    if (   (w_4 >= 0)
        || ((w_2=TAG0009ee(0x500))==0)  )
    {
      s_148.handle = w_4 & 0xffff;
      s_148.malloc = MallocMemory;
      s_148.free = FreeMemory;
      s_148.seek = FileSeek;
      s_148.read = ReadFile;
      w_2 = LoadProgram(&s_148);
      CLOSE(w_4);
      if (w_2 == 0)
      {
        p_8 = (i8 *)MallocMemory(s_148.i_20+4);
//
        if (   (p_8 != NULL) 
            || ((w_2=TAG0009ee(0x501))==0)  )
        {
          s_148.p_32 = p_8 + ((s_148.i_20+1) & 0xfffffffe);
          NotImplemented(0x6fe);
        /*
*/
          NotImplemented(0x724);
  /*  
          A0 = s_148.p_24;
*/
        };
      };
    };
    if (s_148.p_36 != NULL)
    {
      FreeMemory((ui8 *)s_148.p_36);
    };
    w_160++;
  };
  for (;;) {};
}


// *********************************************************
//
// *********************************************************
void TAG0007a8(i32)
{
}

// *********************************************************
//
// *********************************************************
//   TAG0007ce
void SetCriticalErrorHandler(void)
{
  SetSupervisorMode();
// Set critical error handler to:
  ClearSupervisorMode();
}

// *********************************************************
//
// *********************************************************
i16 TAG0009ee(i32)
{
  NotImplemented(0x9ee); return 0;
}

// *********************************************************
//
// *********************************************************
void TAG000a60(i32 errorCode)
{
  char msg[1000];
  sprintf(msg,"Loader Error\n0x%08x", errorCode);
  UI_MessageBox(msg,NULL,MESSAGE_OK);
  die(0xbad);
  // Print "Loader Error"
  // convert error number???
  // "0x"
  // Printing stack????
}
struct SEGMENTDESC
{
  i16 SegmentNumber; //0
  i16 One;           //2 //Always contains a one.
  i32 Offset;        //4 //File offset of segment
  i32 Length;        //8 //Length of segment
};
// *********************************************************
//
// *********************************************************
//  TAG000ad2
i16 LoadProgram(STRUCT148 *s148)
{
  dReg D0, D3, D7;
  aReg A0;
  bool exitFlag; //Escape from 'do-loop'.
  i8  *p_128;
  SEGMENTDESC  *pSegDesc;//p_118;
  i8   b_114[40];
  i8   b_74[20];
  i8  *p_54;
  i8  *p_50;
  i8  *p_46;
  i8  *p_42;
  i8  *p_38;
  i8  *p_34;
  i8  *p_30;
  i32  Segment18;//26
  i32  Segment17;//22
  i32  Segment16;//18
  i32  i_14;
  i32  i_10;
  i32  i_6;
  i16  w_2;
  pSegDesc = NULL;
  s148->p_36 = NULL;
  p_50 = NULL;
  s148->read(s148->handle, 20, (ui8 *)b_74);
  exitFlag=false;
  for (;;)
  { // This is a fake loop.  There are a lot of jumps
    // to the instruction just beyond the end of this
    // loop and I wanted to avoid complicated if-statements
    // and/or goto-statements.  A break-statement fit
    // the need.
    // We also need a way to 'break out' of nested 
    // case-structures and for-loops.  The boolean
    // variable 'exitFlag' provides an escape.


    if (    (b_74[0]!=0x61)
         || (b_74[1]!=0x60)
         || (b_74[4]!=0)
         || (b_74[5]!=2)
         || (b_74[6]!=1)    
         || (b_74[7]!=0) )
    {
    D7W = TAG0009ee(1);
      if (D7W != 0) break;
    };
    if (littleEndian(wordGear(b_74+18)) > 100) 
    {
//
      D7W = TAG0009ee(26);
      if (D7W !=0) break;
    };
    i_14 = (12 * littleEndian(wordGear(b_74+18))) & 0xffff;
    pSegDesc = (SEGMENTDESC *)s148->malloc(i_14);
    s148->read(s148->handle, i_14, (ui8 *)pSegDesc);
    D0W = CheckSum(b_74, (i8 *)pSegDesc);
    if (D0W != littleEndian(wordGear(b_74+2)))
    {
      D7W = TAG0009ee(14);
      if (D7W != 0) break;
    };
    Segment18 = -1;
    Segment17 = -1;
    Segment16 = -1;
    for (D7W=0; D7W<littleEndian(wordGear(b_74+18)); D7W++)
    {
      D0W = littleEndian(pSegDesc[D7W].SegmentNumber);
      switch (D0W)
      {
      case 16:
        if (littleEndian(pSegDesc[D7W].One) != 1)
        {
          D7W = TAG0009ee(20);
          if (D7W != 0)
          {
            exitFlag=true;
            break;
          };
        };
        if (Segment16 == -1)
        {
          Segment16 = D7W & 0xffff;
        }
        else
        {
          D7W = TAG0009ee(23);
          if (D7W != 0)
          {
            exitFlag=true;
            break;
          };
        };
        break;

      case 17:
        if (littleEndian(pSegDesc[D7W].One) != 1)
        {
          D7W = TAG0009ee(21);
          if (D7W != 0)
          {
            exitFlag=true;
            break;
          };
        };
        if (Segment17 == -1)
        {
          Segment17 = D7W & 0xffff;
        }
        else
        {
          D7W = TAG0009ee(24);
          if (D7W != 0)
          {
            exitFlag=true;
            break;
          };
        };
        break;

      case 18:
        if (littleEndian(pSegDesc[D7W].One) != 1)
        {
          D7W = TAG0009ee(22);
          if (D7W != 0)
          {
            exitFlag=true;
            break;
          };
        };
        if (Segment18 == -1)
        {
          Segment18 = D7W & 0xffff;
        }
        else
        {
          D7W = TAG0009ee(25);
          if (D7W != 0)
          {
            exitFlag=true;
            break;
          };
        };
        break;
      default: break;
      };
      if (exitFlag) break;
//
    }; //for (D7W)
    if (exitFlag) break;
    if ( (Segment16==-1)||(Segment17==-1)||(Segment18==-1) )
    {
      D7W = TAG0009ee(2);
      if (D7W != 0) break;
    };
    D0L = LE32(pSegDesc[Segment16].Offset);
    s148->seek(s148->handle, D0L); //To Segment 16..jump table??
    s148->read(s148->handle, 40, (ui8 *)b_114);
    i_14 =  LE32(longGear(b_114+4))
          + LE32(longGear(b_114))
          + LE32(longGear(b_114+20))
          + 12;
    p_42 = (i8 *)s148->malloc(i_14);
    s148->p_36 = p_42;
    p_30 = p_42;
    D0L = LE32(longGear(b_114+4));
    p_34 = p_30 + ((D0L+1) & 0xfffffffe);
    D0L = LE32(longGear(b_114));
    p_38 = p_34 + ((D0L+1) & 0xfffffffe);
    i_14 = LE32(pSegDesc[Segment16].Length);
    D0L = LE32(pSegDesc[Segment17].Length);
    if (D0L > i_14) i_14 = D0L;
    if (i_14 > LE32(longGear(b_114+20)))
    {
      p_50 = (i8 *)s148->malloc(i_14);
      p_54 = p_50;
    }
    else
    {
      p_54 = p_38;
    };
    p_42 = p_54;
    p_46 = p_34 + LE32(longGear(b_114+12));
    D0L = LE32(pSegDesc[Segment16].Length);
    i_14 = D0L - 40;
    s148->read(s148->handle, i_14, (ui8 *)p_42);
    w_2 = TAG0012f6(p_42, i_14);
    w_2 = sw(w_2 + TAG0012f6(b_114, 40));
    D0W = littleEndian(wordGear(b_114+34));
    if (w_2 - D0W != D0W)
    {
      D7W = TAG0009ee(15);
      if (D7W != 0) break;
    };
    while (p_42  < (p_128 = p_38 + i_14))
    { // Construct jump table
#ifdef _MSVC_INTEL
#pragma warning(disable:4310)
#endif
      wordGear(p_46+2) = littleEndian(0x4ef9); //Jump Absolute
#ifdef _MSVC_INTEL
#pragma warning(default:4310)
#endif
      D0L = LE32(longGear(p_42));
      pntGear(p_46+4) = D0L + p_38;
      p_46 += 8;
      p_42 += 4;

    };
    p_42 = p_54;
    p_46 = p_30;
    i_14 = LE32(pSegDesc[Segment17].Length);
    D0L = LE32(pSegDesc[Segment17].Offset);
    s148->seek(s148->handle, D0L); //Go to segment 17
    s148->read(s148->handle, i_14, (ui8 *)p_42);
    w_2 = TAG00132a(p_42, i_14);
    if (w_2 != littleEndian(wordGear(b_114+38)))
    {
      D7W = TAG0009ee(16);
      if (D7W != 0) break;
    };
    i_6 = LE32(longGear(p_42 + 2));
    i_10 = LE32(longGear(p_42 + 6));
    p_42 += 10;
    while (i_6 > 0)
    {
      D7W = littleEndian(wordGear(p_42));
      wordGear(p_46) = littleEndian(D7W);
      p_42 += 2;
      i_6 -= 2;
      p_46 += 2;
      if (D7W == 0)
      {
        D7W = littleEndian(wordGear(p_42));
        p_42 += 2;
        i_6 -= 2;
        while (D7W > 0)
        {
          *p_46 = 0;
          p_46 += 1;
          D7W--;
//
        };
      };
//
    };
    while (i_10 > 0)
    {
      D3W = littleEndian(wordGear(p_42));
      D3L = D3W;
      A0 = p_34 + D3L;
      D0L = LE32(longGear(A0));
      longGear(A0) = LE32((i32)p_34 + D0L);
      p_42 += 2;
      i_10 -= 2;
//
    };
    p_42 = p_38;
    i_14 = LE32(pSegDesc[Segment18].Length);
    D0L = LE32(pSegDesc[Segment18].Offset);
    s148->seek(s148->handle, D0L);
    s148->read(s148->handle, i_14, (ui8 *)p_42);
    w_2 = TAG00132a(p_42, i_14);
//
    s148->p_24 = p_34;
    s148->pJumpTable = p_34 + LE32(longGear(b_114+12)) + 2;
    s148->i_20 = LE32(longGear(b_114+16));
    D7W = 0;
    break;
  } //for (;;);//See comment at top of loop
  if (pSegDesc != NULL) s148->free((ui8 *)pSegDesc);
 
  if (p_50 != NULL) s148->free((ui8 *)p_50);
  if ( (D7W != 0) && (s148->p_36 != NULL) )
  {
    s148->free((ui8 *)s148->p_36);
    s148->p_36 = NULL;
  };
  return D7W;
}

// *********************************************************
//
// *********************************************************
//  TAG00127c
i16 CheckSum(i8 *P1, i8 *P2)
{
  ui8 *ubA3;
  dReg D0, D5, D6, D7;
  D6W = 0;
  ubA3 = (ui8 *)P1;
  for (D7W=4; D7W<20; D7W++)  
  {
    D6W = sw(D6W + D7W * ubA3[D7W]);
//
//
  };
  ubA3 = (ui8 *)P2;
  D5L = 12 * (UI16)(littleEndian(wordGear(P1+18)));
  for (D7W = 0; D7W<D5W; D7W++)
  {
    D0W = ubA3[D7W];
    D6W = sw(D6W + D0W * ((D7W&255)+1));
//
//
  };
  return D6W;
}

// *********************************************************
//
// *********************************************************
i16 TAG0012f6(i8 *P1, i32 size)
{
  dReg D6, D7;
  ui16 *uwA3;
  uwA3 = (ui16 *)P1;
  for (D7W=sw(size), D6W=0; D7W>0; D7W-=2)
  {
    D6W = (UI16)(D6W + littleEndian(*(uwA3++)));
//
  };
  return D6W;
}

// *********************************************************
//
// *********************************************************
i16 TAG00132a(i8 *P1,i32 size)
{
  dReg D6, D7;
  ui8 *ubA3;
  ubA3 = (ui8 *)P1;
  for (D6W=0, D7L=size; D7L>0; D7L--)
  {
    D6W = (UI16)(D6W + *(ubA3++));
//
//
  };
  return D6W;
}
/*
// *********************************************************
//
// *********************************************************
i16 TAG00183c(i16 handle)
{
  TAG001b82(handle);
  //if (handle <= 0x8300)  // ROQUEN: This is always true...
  if (handle <= -32000)
  {
    return 0;
  };
  b.uWord1828 = CLOSE(handle);
  if (b.uWord1828 != 0)
  {
    return -1;
  }
  else
  {
    return 0;
  };
}
*/
// *********************************************************
//
// *********************************************************
void TAG00187a(i32)
{
  // TRAP 1  -  TERM
  NotImplemented(0x188c);
  die(0xbad);
}

/*
// *********************************************************
//
// *********************************************************
void TAG001890(i32 P1)
{
  ITEM20 *pi20A3;
  for (pi20A3 = b.Item20;
       pi20A3 < b.Item20 + 1460/20;
       pi20A3++)
  {
    if ((pi20A3->Word10 & 3) != 0)
    {
      TAG0018d6(pi20A3);
    };
//
  };
  TAG00187a(P1);
}
*/
/*
// *********************************************************
//
// *********************************************************
i16 TAG0018d6(ITEM20 *P1)
{
  dReg D0;
  ITEM20 *i20A3;
  i20A3 = P1;
  D0W = TAG001928(i20A3);
  if (D0W != 0)
  {
    D0W = -1;
  }
  else
  {
    if (i20A3->Word10 & 16)
    {
      TAG001b6c(i20A3->Long6);
    };
    i20A3->Word10 = 0;
    D0W = TAG00183c(i20A3->Word12);
    if (D0W != 0)
    {
      D0W = -1;
    }
    else
    {
      D0W = 0;
    };
  };
  return D0W;
}
*/
// *********************************************************
//
// *********************************************************
i16 TAG001928(ITEM20 *P1)
{
  dReg D0, D7;
  ITEM20 *i20A3;
  i20A3 = P1;
  if ((i20A3->Word10 & 3) == 0)
  {
    return -1;
  }
  else
  {
    D7L = i20A3->Long0 - i20A3->Long6;
    if (i20A3->Word10 & 0x80)
    {
      if ((i20A3->Word10 & 2) == 0)
      {
        return -1;
      };
      if (i20A3->Word10 & 4)
      {
        TAG001a56(i20A3->Word12, 0, 2);
      };
      NotImplemented(0x1990); // Need longword  
      D0W = TAG001cd4(i20A3->Word12, i20A3->Long6, D7W);
      if (D0W == -1)
      {
        return -1;
      };
      i20A3->Word10 &= 0xff7f;
      i20A3->Long14 += D7W;
    }
    else
    {
      if (i20A3->Word12 > 0)
      {
        i20A3->Long14 = TAG001a56(i20A3->Word12, -i20A3->Word4, 1);
      };
    };
    i20A3->Long0 = i20A3->Long6;
    i20A3->Word4 = 0;
    return 0;
  };
}

// *********************************************************
//
// *********************************************************
i32 TAG001a56(i16,i32,i16)
{
  NotImplemented(0x1a56); return 0;
}

// *********************************************************
//
// *********************************************************
void TAG001b6c(i32)
{
  NotImplemented(0x1b6c);
}

// *********************************************************
//
// *********************************************************
void TAG001b82(i16 P1)
{
  dReg D0;
  i32 i;
  for (i=0; i<76; i++)
  {
    D0W = b.Word2132[2*i];
    if (D0W == P1)
    {
      b.Word2132[2*i] = 0;
    };
//
  };
}

// *********************************************************
//
// *********************************************************
i16 TAG001cd4(i16,i32,i16)
{
  NotImplemented(0x1cd4); return 0;
}

// *********************************************************
//
// *********************************************************
void TAG001e5e(void)
{
  strcpy((char *)b.Byte274, "A:\\F");
  memcpy(b.Byte290, "0123456789ABCDEF", 16);
  b.Item20[0].Long0 = 0;
  b.Item20[0].Word4 = 0;
  b.Item20[0].Long6 = 0;
  b.Item20[0].Word10 = 9;
  b.Item20[0].Word12 = (ui16)0x8300;
  b.Item20[0].Long14 = 0;
  b.Item20[0].Word18 = 0;
  b.Item20[1].Long0 = 0;
  b.Item20[1].Word4 = 0;
  b.Item20[1].Long6 = 0;
  b.Item20[1].Word10 = 258;
  b.Item20[1].Word12 = (ui16)0x8300;
  b.Item20[1].Long14 = 0;
  b.Item20[1].Word18 = 0x200;
  b.Item20[2].Long0 = 0;
  b.Item20[2].Word4 = 0;
  b.Item20[2].Long6 = 0;
  b.Item20[2].Word10 = 0x102;
  b.Item20[2].Word12 = (ui16)0x8300;
  b.Item20[2].Long14 = 0;
  b.Item20[2].Word18 = 0x200;
  b.uWord1752 = 0x200;
}
