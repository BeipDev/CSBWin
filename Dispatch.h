

enum CODESTATE
{
  st_First = 0,
  st_AskWhatToDo,
  st_StartCSB,
  st_StartChaos,
  st_StartHint,
  st_DoMenu,
  st_OpenPrisonDoors,
  st_PrintLines,
  //st_LoadPartyLevel,
  st_MagicSelection,  
  st_SelectSaveGame,
  st_TAG01f746,
  st_FeedCharacter,
  st_TurnParty,
  st_FusionSequence,
//  st_CreateNewTextRow,
  st_TAG00301c, //in Chaos.cpp
  st_UtilityDialogBox, //st_TAG000588, //in Chaos.cpp
  st_TAG000ede, //in Chaos.cpp
  st_CheckChampionNames, //in Chaos.cpp
  st_TAG00179c, //in Chaos.cpp
  st_ReadSavedGame, //in Chaos.cpp
  st_TAG004518, //in Chaos.cpp
  st_TAG00260e, //in Chaos.cpp
  st_TAG0026c4, //in Chaos.cpp
  st_TAG000fc4, //in Chaos.cpp
  st_InvertButton, //in Chaos.cpp
  st_DrawCharacterEditorScreen, //in Chaos.cpp
  st_DrawCharacterDetails, //in Chaos.cpp - utility disc
  st_TAG004430, //in Chaos.cpp
  st_DisplayText,   //in Hint.cpp
  st_TAG005a1e_xxx, //in Hint.cpp
  st_FadePalette,   //in Hint.cpp
//  st_TAG005ae8_xxx, //in Hint.cpp
  st_TAG0051c2_1,   //in Hint.cpp
//  st_TAG008c40_1,   //in Hint.cpp
//  st_TAG008c40_xxx, //in Hint.cpp
//  st_TAG008c40_2,   //in Hint.cpp
  st_TAG006c7e_xxx, //in Hint.cpp
  st_TAG006c7e_2,   //in Hint.cpp
//  st_TAG0076a0_8,   //in Hint.cpp
  st_TAG0076a0_12,  //in Hint.cpp
//  st_TAG0076a0_13,  //in Hint.cpp
//  st_TAG0076a0_40,  //in Hint.cpp
//  st_TAG002c5e,     //in Hint.cpp
//  st_TAG00686a,     //in Hint.cpp
  st_TAG006c7e_9,   //in Hint.cpp
  st_TAG006c7e_32,  //in Hint.cpp
//  st_TAG00799a_xxx, //in Hint.cpp
//  st_TAG00799a_2,   //in Hint.cpp
//  st_TAG00799a_13,  //in Hint.cpp
  st_TAG00799a_12,  //in Hint.cpp
//  st_TAG007fdc_2,   //in Hint.cpp
//  st_TAG007fdc_xxx, //in Hint.cpp
  st_TAG007fdc_7,   //in Hint.cpp
//  st_TAG00799a_1,   //in Hint.cpp
  st_TAG00799a_9,   //in Hint.cpp
  st_TAG00799a_6,   //in Hint.cpp
//  st_SetSelectedLine,//in Hint.cpp
  st_TAG007fdc_36,  //in Hint.cpp
//  st_TAG002e64,     //in Hint.cpp
//  st_TAG008c40_3,   //in Hint.cpp
  st_TAG008c40_5,   //in Hint.cpp
  st_TAG008c40_6,   //in Hint.cpp
  st_TAG008c40_8,   //in Hint.cpp
//  st_TAG004f3a_2,   //in Hint.cpp
//  st_TAG008c40_12,  //in Hint.cpp
//  st_TAG008c40_30,  //in Hint.cpp
  st_TAG0051c2_31,  //in Hint.cpp
  st_TAG004e4c_xxx, //in Hint.cpp
  st_TAG004e4c_8,   //in Hint.cpp
//  st_Free_TEXT,     //in Hint.cpp
  st_TAG004e4c_6,   //in Hint.cpp
//  st_TAG0060c4,     //in Hint.cpp
//  st_TAG006bc0,     //in Hint.cpp
//  st_TAG006bfc,     //in Hint.cpp
//  st_TAG006718,     //in Hint.cpp
//  st_TAG003210,     //in Hint.cpp
//  st_TAG004a22_16,  //in Hint.cpp
//  st_TAG0062a2,     //in Hint.cpp
//  st_TAG00835c_25,  //in Hint.cpp
//  st_TAG00835c_xxx, //in Hint.cpp
//  st_TAG008a62,     //in Hint.cpp
//  st_TAG0094de,     //in Hint.cpp
//  st_TAG0095b0,     //in Hint.cpp
//  st_TAG00948c,     //in Hint.cpp
//  st_AddPage,       //in Hint.cpp
//  st_TAG00964e,     //in Hint.cpp
//  st_TAG005ae8_9,   //in Hint.cpp
//  st_TAG0051c2_35,  //in Hint.cpp
  st_PaletteFade,   //in Hint.cpp
//  st_TAG005ae8_1,   //in Hint.cpp
//  st_TAG00978a_13,  //in Hint.cpp
//  st_TAG00978a_21,  //in Hint.cpp
//  st_TAG00978a_27,  //in Hint.cpp
//  st_TAG00978a_28,  //in Hint.cpp
  st_TAG0051c2_2,   //in Hint.cpp
//  st_TAG00978a_29,  //in Hint.cpp
  st_TAG005d2a,     //in Hint.cpp
//  st_TAG0093a0_17,  //in Hint.cpp
//  st_MyReadFirstBlock,//in Hint.cpp
//  st_TAG0093a0_xxx, //in Hint.cpp
//  st_TAG0093a0_18,  //in Hint.cpp
//  st_TAG004f3a_16,  //in Hint.cpp
//  st_LZWRawChar,    //in Hint.cpp
//  st_TAG004a22_xxx, //in Hint.cpp
//  st_TAG004a22_14,  //in Hint.cpp
//  st_TAG004a22_26,  //in Hint.cpp
//  st_TAG004f3a_xxx, //in Hint.cpp
//  st_AssignMemory,  //in Hint.cpp
//  st_TAG002f6c,     //in Hint.cpp
//  st_ReleaseMem,    //in Hint.cpp
//  st_TAG005ae8_38,  //in Hint.cpp
//  st_TAG0051c2_36,  //in Hint.cpp
//  st_TAG0051c2_37,  //in Hint.cpp
  st_TAG005a1e_5,   //in Hint.cpp
  st_TAG0089b0,     //in Hint.cpp
  st_TAG009410,     //in Hint.cpp
  st_TAG001dde,     //in Hint.cpp
//  st_TAG0051c2_xxx, //in Hint.cpp
//  st_TAG004f3a_1,   //in Hint.cpp
//  st_TAG003006,     //in Hint.cpp
//  st_TAG00978a_1,   //in Hint.cpp
//  st_TAG00978a_xxx, //in Hint.cpp
//  st_TAG00978a_3,   //in Hint.cpp
//  st_TAG00978a_12,  //in Hint.cpp
  //st_TAG0192f4,
  st_TAG021028,
  st_TAG01a6ea,
  st_ReIncarnate,
  st_ProcessTimersViewportAndSound,
  st_ResurrectReincarnateCancel,
//  st_RemoveTimedOutText,
  //st_LoadNeededGraphics,
  //st_ReadGraphicsForLevel,
  st_WaitForMenuSelect,
  //st_InsertDisk,
  st_ReadEntireGame,
  st_HandleMagicClick,
  st_ProcessTimers,
  st_ProcessTT_ViAltar,
  st_TAG019fac,
  st_ClickOnEye,
  st_TAG01f928,
  st_DisplayDiskMenu,
  st_HandleMouseEvents,
  st_Attack,
  st_TAG02076e,
  st_FlashButton,
  st_MainLoop,
  st_DisplayChaosStrikesBack,
  st_FadeToPalette,
  st_ShowPrisonDoor,
  st_GameSetup,
  st_ShowCredits,
  st_wvbl,
  st_VBLDelay,
  st_TAG01b29a,
  st_TAG01f5ea,
  st_Fusion,
  st_ExecuteAttack,
  st_FlashAttackDamage,

  st_Last
};

enum RETURNS
{
  _0_ = 0,
  _1_,
  _2_,
  _3_,
  _4_,
  _5_,
  _6_,
  _7_,
  _8_,
  _9_,
  _10_,
  _11_,
  _12_,
  _13_,
  _14_,
  _15_,
  _16_,
  _17_,
  _18_,
  _19_,
  _20_,
  _21_,
  _22_,
  _23_,
  _24_,
  _25_,
  _26_,
  _27_,
  _28_,
  _29_,
  _30_,
  _31_,
  _32_,
  _33_,
  _34_,
  _35_,
  _36_,
  _37_,
  _38_,
  _39_,
  _40_,
  _41_,
  _42_,
  _43_,
  _44_,
  _45_,
  _46_,
  _47_,
  _48_,
  _49_,
  _50_,
  _51_
};

class DOESNOTEXIST
{
  i32 i;
};
extern  DOESNOTEXIST DoesNotExist;
typedef DOESNOTEXIST& RESTARTABLE;


extern i32 intResult;
extern i16 i16Result;
extern RN  RNResult;
extern bool boolResult;
extern pnt pntResult;


void _RETURN();
void _RETURN_i16(i16);
void _RETURN_int(i32);
void _RETURN_RN(RN);
void _RETURN_BOOL(bool);
void _RETURN_pnt(pnt);

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

//#define CALL(n,f,p1,p2,p3,p4,p5,p6) \
//  mystate = n; \
//  _CALL (st_##f,(i32)(p1),(i32)(p2),(i32)(p3),(i32)(p4),(i32)(p5),(i32)(p6)); \
//  return 0xfafa3434; \
//return##n:

#define CALL0(n,f) \
{ mystate = n; \
  _CALL0 (st_##f); \
  return DoesNotExist; \
return##n: ;};

#define CALL1(n,f,p1) \
{ mystate = n; \
  _CALL1 (st_##f,(i32)(p1)); \
  return DoesNotExist; \
return##n: ;};

#define CALL2(n,f,p1,p2) \
{ mystate = n; \
  _CALL2 (st_##f,(i32)(p1),(i32)(p2)); \
  return DoesNotExist; \
return##n: ;};

#define CALL3(n,f,p1,p2,p3) \
{ mystate = n; \
  _CALL3 (st_##f,(i32)(p1),(i32)(p2),(i32)(p3)); \
  return DoesNotExist; \
return##n: ;};

#define CALL4(n,f,p1,p2,p3,p4) \
{ mystate = n; \
  _CALL4 (st_##f,(i32)(p1),(i32)(p2),(i32)(p3),(i32)(p4)); \
  return DoesNotExist; \
return##n: ;};

#define CALL5(n,f,p1,p2,p3,p4,p5) \
{ mystate = n; \
  _CALL5 (st_##f,(i32)(p1),(i32)(p2),(i32)(p3),(i32)(p4),(i32)(p5)); \
  return DoesNotExist; \
return##n: ;};

#define CALL6(n,f,p1,p2,p3,p4,p5,p6) \
{ mystate = n; \
  _CALL6 (st_##f,(i32)(p1),(i32)(p2),(i32)(p3),(i32)(p4),(i32)(p5),i32(p6)); \
  return DoesNotExist; \
return##n: ;};

#define CALL8(n,f,p1,p2,p3,p4,p5,p6,p7,p8) \
{ mystate = n; \
  _CALL8 (st_##f,(i32)(p1),(i32)(p2),(i32)(p3),(i32)(p4),(i32)(p5),i32(p6),i32(p7),i32(p8)); \
  return DoesNotExist; \
return##n: ;};

#define CALL9(n,f,p1,p2,p3,p4,p5,p6,p7,p8,p9) \
{ mystate = n; \
  _CALL9 (st_##f,(i32)(p1),(i32)(p2),(i32)(p3),(i32)(p4),(i32)(p5),i32(p6),i32(p7),i32(p8),i32(p9)); \
  return DoesNotExist; \
return##n: ;};

#define RETURN {mystate=0; _RETURN(); return DoesNotExist;};
#define RETURN_RN(a) {mystate=0; _RETURN_RN(a); return DoesNotExist;};
#define RETURN_int(a) {mystate=0; _RETURN_int(a); return DoesNotExist;};
#define RETURN_i16(a) {mystate=0; _RETURN_i16(a); return DoesNotExist;};
#define RETURN_BOOL(a) {mystate=0; _RETURN_BOOL(a); return DoesNotExist;};
#define RETURN_pnt(a) {mystate=0; _RETURN_pnt(a); return DoesNotExist;};

#define WAITFORMESSAGE(n) {mystate=n; return DoesNotExist; return##n: ;};

#define RESTARTMAP \
  static i32 mystate=_0_; \
switch (mystate) { case _0_: goto return_0_;

#define RESTART(n) case _##n##_: goto return_##n##_;

#define END_RESTARTMAP default: ASSERT(0,"Illegal RESTART"); }; return_0_:

