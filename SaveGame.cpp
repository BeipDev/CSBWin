#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

#ifdef _MSVC_CE2002ARM
extern bool hidingMenu;
#endif

extern unsigned char *encipheredDataFile;
extern bool simpleEncipher;
extern bool RecordDesignOption;
extern bool DMRulesDesignOption;
extern bool invisibleMonsters;
extern bool DM_rules;
extern bool drawAsSize4Monsters;
extern bool RecordCommandOption;
extern bool NoRecordCommandOption;
extern bool RecordMenuOption;
extern bool NoRecordMenuOption;
extern bool extendedPortraits;
extern bool sequencedTimers;
extern bool extendedTimers;
extern bool DefaultDirectXOption;
extern i32 deleteDuplicateTimers;
extern bool extendedWallDecorations;
extern bool disableSaves;
extern i32 dbEntrySizes[16];
extern char hintKey[8];
extern OVERLAYDATA currentOverlay;
extern bool overlayActive;
extern i32 numGlobalVariables;
extern ui32 *globalVariables;
extern ui8 overlayPaletteRed[512];
extern ui8 overlayPaletteGreen[512];
extern ui8 overlayPaletteBlue[512];
void RC4_prepare_key(unsigned char *, i32);
void RC4_encipher(unsigned char *buffer_ptr,
                  i32 buffer_len,
                  i32 position);
void info(char *, unsigned int);
void SelectOverlay(i32, i32, i32, i32, i32);
i32 LoadLong(pnt addr);
void RecordFile_Record(const char *line);
void RecordFile_Record(i32 x, i32 y, i32 z);
void RecordFile_Record(MouseQueueEnt *MQ);
bool IsPlayFileOpen();
bool PlayFile_Play(MouseQueueEnt *MQ);
bool IsRecordFileRecording();
void RecordFile_Open();
void RecordFile_Close();

struct GAMEBLOCK1
{
   // Data below this line is 512 bytes long.  This
   // block is the first read from the game save file.
   // IN original code...starts at -740(A6)
   i8 Byte740[300];       //000
   i8 Byte22598;          //300
   ui8 Byte22596;         //301
   i8 FILL438[4];         //302
   i16 SaveOption;        //306
   i32 RandomGameID;      //308;// reversed//
   i16 Block2Hash;        //312 swapped
   i16 ITEM16Hash;        //314 swapped
   i16 CharacterHash;     //316 swapped
   i16 TimersHash;        //318 swapped
   i16 TimerQueHash;      //320 swapped
   i32 totalMoveCount;    //322
   i16 Hash326;           //326
   i16 Hash328;           //328
   i16 Hash330;           //330
   i16 Hash332;           //332
   i16 Hash334;           //334
   i16 Hash336;           //336
   i16 Hash338;           //338
   i16 Hash340;           //340
   i16 Hash342;           //342
   i16 Block2Checksum;    //344swapped
   i16 ITEM16Checksum;    //346 swapped
   i16 CharacterChecksum; //348 swapped
   i16 TimersChecksum;    //350 swapped
   i16 TimerQueChecksum;  //352 swapped
   i16 Checksum354;       //354
   i16 Checksum356;       //356
   i16 Checksum358;       //358
   i16 Checksum360;       //360
   i16 Checksum362;       //362
   i16 Checksum364;       //364
   i16 Checksum366;       //366
   i16 Checksum368;       //368
   i16 Checksum370;       //370
   i16 Checksum372;       //372
   i16 Checksum374;       //374
   i16 Word22594;         //376 swapped
   i16 Word22592;         //378 swapped
   i8 Byte22808[132];     //380 moved as a unit
};

struct GAMEBLOCK2
{
   // The following 128 bytes are the second thing
   // read from the game file.
   i32 Time;                  //000;  reversed
   i32 ranseed;               //004; reversed
   ui16 ObjectInHand;         //008; swapped
   i16 numcharacter;          //010; swapped
   i16 partyx;                //012; swapped
   i16 partyy;                //014; swapped
   i16 partyfacing;           //016; swapped
   i16 partyLevel;            //018; swapped
   i16 handChar;              //020; swapped Character index
   i16 MagicCaster;           //022; swapped
   i16 NumTimer;              //024; swapped
   i16 FirstAvailTimer;       //026; swapped
   i16 MaxTimers;             //028; swapped
   i16 ITEM16QueLen;          //030; swapped
   i32 LastMonsterAttackTime; //032; reversed
   i32 LastPartyMoveTime;     //036; A timestamp. reversed
   i16 partyMoveDisableTimer; //040; swapped
   i16 Word11712;             //042; swapped
   i16 Word11714;             //044; swapped
   i16 MaxITEM16;             //046; swapped
   ui16 TimerSequence;        //048; swapped
   i8 FILL180[78];            //050;
};

struct str1eb18
{
   GAMEBLOCK1 gb1;
   // Data above this line is 512 bytes long.  This
   // block is read from the game save file.

   GAMEBLOCK2 gb2;
   // The 128 bytes above this are the second thing read from
   // the game file.

   i8 Byte100[50];
   i8 Byte50[50];
};

void BO(i32 &longword)
{
   longword = LE32(longword);
}

void BO(i16 &word)
{
   word = LE16(word);
}

void BO(ui16 &word)
{
   word = LE16(word);
}

void SwapDataIndexMap()
{
   i32 i;
   for(i = 0; i < dataMapLength; i++)
   {
      dataIndexMap[i] = BE16(dataIndexMap[i]);
   };
}

void SwapIndirectTextIndex()
{
   i32 i;
   for(i = 0; i < d.dungeonDatIndex->NumWordsInTextArray(); i++)
   {
      d.indirectTextIndex[i] = BE32(d.indirectTextIndex[i]);
   };
}

void SwapExtendedFeaturesBlock(EXTENDEDFEATURESBLOCK *
#ifdef _bigEndian
                                   pefb)
{
   pefb->dataMapLength = BE32(pefb->dataMapLength);
   pefb->dataTypeMapChecksum = BE32(pefb->dataTypeMapChecksum);
   pefb->dataIndexMapChecksum = BE32(pefb->dataIndexMapChecksum);
   pefb->extendedFeaturesChecksum = BE32(pefb->extendedFeaturesChecksum);
   pefb->numDSA = BE16(pefb->numDSA);
   pefb->editingOptions = BE32(pefb->editingOptions);
   pefb->gameInfoSize = BE32(pefb->gameInfoSize);
   pefb->cellFlagArraySize = BE32(pefb->cellFlagArraySize);
   pefb->graphicsSignature1 = BE32(pefb->graphicsSignature1);
   pefb->graphicsSignature2 = BE32(pefb->graphicsSignature2);
#else
){
#endif
}

ui32 FormChecksum(ui8 *buf, i32 num)
{
   ui32 result = 0;
   i32 i;
   for(i = 0; i < num; i++)
   {
      result = result * ui32(0xbb40e62d) + 11 + buf[i];
   };
   return result;
}

void ReadDSAs(i32 handle, i32 numDSA)
{
   i32 i;
   i32 dsaNum;
   ui32 computedChecksum, fileChecksum;
   DSAIndex.Cleanup();
   if(numDSA != 0)
   {
      RCS(handle); //Discard old checksum and restart.
      for(i = 0; i < numDSA; i++)
      {
         RCS(&dsaNum);
         if(DSAIndex.GetDSA(dsaNum) != NULL)
         {
            die(0, "Duplicate DSA");
         };
         if(dsaNum > 255)
         {
            die(0, "DSA number greater than 255");
         };
         DSAIndex.SetDSA(dsaNum, new DSA);
         DSAIndex.GetDSA(dsaNum)->Read();
      };
      computedChecksum = RCS(handle);
      READ(handle, 4, (ui8 *)&fileChecksum);
      if(fileChecksum != computedChecksum)
      {
         die(0, "Improper DSA checksum");
      };
   };
   DSAIndex.IsLoaded(true); // Mark the DSA database 'Loaded' even if none exist.
}

void ReadGameInfo(i32 handle, i32 size)
{
   if(gameInfo != NULL)
      UI_free(gameInfo);
   gameInfo = NULL;
   if(size == 0)
   {
      gameInfo = (char *)UI_malloc(100, MALLOC035);
      strcpy(gameInfo, "No game information was provided");
      gameInfoSize = strlen(gameInfo) + 1; // Include the trailing nul
      return;
   };
   gameInfo = (char *)UI_malloc(size + 1, MALLOC036);
   READ(handle, size, (ui8 *)gameInfo);
   gameInfoSize = size + 1;
   gameInfo[size] = 0;
}

i32 WriteGameInfo(i32 handle, i32 size)
{ //Returns size.
   if(size == 0)
      return 0;
   WRITE((i16)handle, size, (ui8 *)gameInfo);
   return size;
}

void ClearDSALevelIndex()
{
   i32 i, j;
   for(i = 0; i < 64; i++)
   {
      for(j = 0; j < 32; j++)
      {
         DSALevelIndex[i][j] = 0xffff;
      };
   };
}

void ReadDSALevelIndex(i32 handle)
{
   unsigned char triple[3];
   for(;;)
   {
      READ(handle, 3, triple);
      if(triple[0] > 63)
         break;
      DSALevelIndex[triple[0]][triple[1]] = triple[2];
   };
}

i32 ReadExtendedFeatures(i32 handle)
{
   EXTENDEDFEATURESBLOCK efb;
   i32 n0, n1, n2;
   i32 startPos, endPos;
   i32 i;
   ui32 savechecksum;
   ExtendedFeaturesVersion = '@';
   indirectText = false;
   bigActuators = false;
   sequencedTimers = false;
   extendedTimers = false;
   DefaultDirectXOption = false;
   extendedWallDecorations = false;
   spellFilterLocation = 0;
   dataMapLength = 0;
   for(i = 0; i < 8; i++)
      hintKey[i] = 0;
   cellflagArraySize = 0;
   if(dataIndexMap != NULL)
      UI_free(dataIndexMap);
   dataIndexMap = NULL;
   if(dataTypeMap != NULL)
      UI_free(dataTypeMap);
   dataTypeMap = NULL;
   startPos = LSEEK(0, handle, SEEK_CUR);
   n0 = READ(handle, 512, (ui8 *)&efb);
   SwapExtendedFeaturesBlock(&efb);
   if(n0 != 512)
   {
      LSEEK(startPos, handle, SEEK_SET);
      return 0;
   };
   if(strcmp(efb.sentinel, " Extended Features ") != 0)
   {
      unsigned char simple[2];
      simple[0] = 1;
      simple[1] = 0;
      RC4_prepare_key(simple, 1);
      SwapExtendedFeaturesBlock(&efb);
      RC4_encipher((unsigned char *)(&efb), 512, 0);
      SwapExtendedFeaturesBlock(&efb);
      if((strcmp(efb.sentinel, " Extended Features ") == 0) && (efb.flags & EXTENDEDFEATURESBLOCK::SimpleEncryption))
      {
         GETFILETABLE((ui16)handle)->Enciphered(true);
         simpleEncipher = true;
      }
      else
      {
         LSEEK(startPos, handle, SEEK_SET);
         return 0;
      };
   };
   savechecksum = efb.extendedFeaturesChecksum;
   efb.extendedFeaturesChecksum = 0;
   SwapExtendedFeaturesBlock(&efb);
   if(FormChecksum((ui8 *)&efb, 512) != savechecksum)
   {
      SwapExtendedFeaturesBlock(&efb);
      LSEEK(startPos, handle, SEEK_SET);
      return 0;
   };
   SwapExtendedFeaturesBlock(&efb);
   dataTypeMap = (ui8 *)UI_malloc(efb.dataMapLength, MALLOC037);
   dataIndexMap = (ui16 *)UI_malloc(2 * efb.dataMapLength, MALLOC038);
   if((dataTypeMap == NULL) || (dataIndexMap == NULL))
   {
      UI_MessageBox("Cannot Allocate Memory", "Error", MB_OK);
      die(0x4445cc);
   };
   ExtendedFeaturesVersion = efb.version;
   if(efb.cellFlagArraySize != 0)
   {
      cellflagArraySize = efb.cellFlagArraySize;
   };
   n1 = READ(handle, efb.dataMapLength, dataTypeMap);
   n2 = READ(handle, 2 * efb.dataMapLength, (ui8 *)dataIndexMap);
   if((FormChecksum((ui8 *)dataTypeMap, efb.dataMapLength) != efb.dataTypeMapChecksum) || (FormChecksum((ui8 *)dataIndexMap, 2 * efb.dataMapLength) != efb.dataIndexMapChecksum) || (n1 != efb.dataMapLength) || (n2 != 2 * efb.dataMapLength))
   {
      UI_MessageBox("Extended features damaged", "Error", MB_OK);
      die(0xe3ee17);
   };
   spellFilterLocation = efb.spellFilterLocation;
   expectedGraphicSignature1 = efb.graphicsSignature1;
   expectedGraphicSignature2 = efb.graphicsSignature2;
   expectedCSBgraphicSignature1 = efb.CSBgraphicsSignature1;
   expectedCSBgraphicSignature2 = efb.CSBgraphicsSignature2;
   memcpy(hintKey, efb.hintKey, 8);
   dataMapLength = efb.dataMapLength;
   SwapDataIndexMap();
   ReadDSAs(handle, efb.numDSA);
   ReadGameInfo(handle, efb.gameInfoSize);
   ClearDSALevelIndex();
   if(efb.flags & EXTENDEDFEATURESBLOCK::LevelDSAInfoPresent)
   {
      ReadDSALevelIndex(handle);
   };
   DMRulesDesignOption =
       (efb.flags & EXTENDEDFEATURESBLOCK::DMRules) != 0;
   invisibleMonsters =
       (efb.extendedFlags & EXTENDEDFEATURESBLOCK::InvisibleMonsters) != 0;
   DM_rules =
       (efb.extendedFlags & EXTENDEDFEATURESBLOCK::DMRulesOption) != 0;
   RecordFile_Record(DM_rules ? "#DMRules\n" : "#NoDMRules\n");
   drawAsSize4Monsters =
       (efb.extendedFlags & EXTENDEDFEATURESBLOCK::DrawAsSize4Monsters) != 0;
   RecordDesignOption =
       (efb.flags & EXTENDEDFEATURESBLOCK::AutoRecord) != 0;
   extendedPortraits =
       (efb.flags & EXTENDEDFEATURESBLOCK::ExpandedPortraits) != 0;
   indirectText =
       (efb.flags & EXTENDEDFEATURESBLOCK::IndirectText) != 0;
   bigActuators =
       (efb.flags & EXTENDEDFEATURESBLOCK::BigActuators) != 0;
   sequencedTimers =
       (efb.extendedFlags & EXTENDEDFEATURESBLOCK::SequencedTimers) != 0;
   extendedTimers =
       (efb.extendedFlags & EXTENDEDFEATURESBLOCK::ExtendedTimers) != 0;
   DefaultDirectXOption =
       (efb.extendedFlags & EXTENDEDFEATURESBLOCK::DefaultDirectX) != 0;
   if(DefaultDirectXOption)
   {
      usingDirectX = true;
   };
   extendedWallDecorations =
       (efb.extendedFlags & EXTENDEDFEATURESBLOCK::ExtendedWallDecorations) != 0;
   overlayActive =
       (efb.extendedFlags & EXTENDEDFEATURESBLOCK::OverlayActive) != 0;
   if(efb.overlayOrdinal > 0)
   {
      SelectOverlay(efb.overlayOrdinal - 1,
                    efb.overlayP1,
                    efb.overlayP2,
                    efb.overlayP3,
                    efb.overlayP4);
   };
   endPos = LSEEK(0, handle, SEEK_CUR);
   return endPos - startPos;
};

void swapBlock1(GAMEBLOCK1 *b)
{ // Swap and reverse byte order in first 512-byte
   // block of savegame file.
   BO(b->RandomGameID);
   BO(b->Block2Hash);    //428
   BO(b->ITEM16Hash);    //426
   BO(b->CharacterHash); //424
   BO(b->TimersHash);    //422
   BO(b->TimerQueHash);  //420

   BO(b->Block2Checksum);    //396
   BO(b->ITEM16Checksum);    //394
   BO(b->CharacterChecksum); //392
   BO(b->TimersChecksum);    //390
   BO(b->TimerQueChecksum);  //388

   BO(b->Word22594); //364
   BO(b->Word22592); //362
}

void swapBlock2(GAMEBLOCK2 *b)
{ // Swap and reverse byte order in second 128-byte
   // block of savegame file.
   BO(b->Time);                  //228
   BO(b->ranseed);               //224
   BO(b->ObjectInHand);          //220
   BO(b->numcharacter);          //218
   BO(b->partyx);                //216
   BO(b->partyy);                //214
   BO(b->partyfacing);           //212
   BO(b->partyLevel);            //210
   BO(b->handChar);              //208
   BO(b->MagicCaster);           //206
   BO(b->NumTimer);              //204
   BO(b->FirstAvailTimer);       //202
   BO(b->MaxTimers);             //200
   BO(b->ITEM16QueLen);          //198
   BO(b->LastMonsterAttackTime); //196
   BO(b->LastPartyMoveTime);     //192
   BO(b->partyMoveDisableTimer); //188
   BO(b->Word11712);             //186
   BO(b->Word11714);             //184
   BO(b->MaxITEM16);             //182
   BO(b->TimerSequence);         //180
}

void swapITEM16s()
{ // Fix byte order in ITEM16 entries
   i32 i;
   for(i = 0; i < d.MaxITEM16; i++)
   {
      d.Item16[i].word0 = LE16(d.Item16[i].word0);
   };
}

void swapCharacter(i32 i)
{
   i32 j;
   CHARDESC *pc;
   i16 *pp;
   pc = &d.CH16482[i];
   pc->busyTimer = LE16(pc->busyTimer);
   pc->timerIndex = LE16(pc->timerIndex);
   pc->charFlags = LE16(pc->charFlags);
   pc->ouches = LE16(pc->ouches);
   pc->m_wHP = LE16(pc->m_wHP);
   pc->m_wMaxHP = LE16(pc->m_wMaxHP);
   pc->m_wStamina = LE16(pc->m_wStamina);
   pc->m_wMaxStamina = LE16(pc->m_wMaxStamina);
   pc->m_wMana = LE16(pc->m_wMana);
   pc->m_wMaxMana = LE16(pc->m_wMaxMana);
   pc->word64 = LE16(pc->word64);
   pc->food = LE16(pc->food);
   pc->water = LE16(pc->water);
   for(j = 0; j < 20; j++)
   {
      pc->skills92[j].tempAdjust = LE16(pc->skills92[j].tempAdjust);
      pc->skills92[j].experience = LE32(pc->skills92[j].experience);
   };
   for(j = 0; j < 30; j++)
   {
      pp = (i16 *)&pc->possessions[j];
      *pp = LE16(*pp);
   };
   pc->load = LE16(pc->load);
   pc->shieldStrength = LE16(pc->shieldStrength);
}

void swapPointer10454()
{
   for(i32 i = 0; i < d.numColumnPointers; i++)
      d.objectListIndex[i] = LE16(d.objectListIndex[i]);
}

void swapPRN10464(i32 num)
{
   i32 i;
   for(i = 0; i < num; i++)
      d.objectList[i].LittleEndian();
}

void swapCharacterData()
{
   i32 i;
   for(i = 0; i < 4; i++)
      swapCharacter(i);
   d.Brightness = LE16(d.Brightness);
   for(i = 0; i < 24; i++)
      d.PartyFootprints[i] = LE16(d.PartyFootprints[i]);
}

#ifdef _DEBUG
void recomputeLoads()
{
   i32 p, c;
   for(c = 0; c < d.NumCharacter; c++)
   {
      DEBUGLOAD(d.CH16482 + c, 0, 0, RNeof);
      d.CH16482[c].load = 0;
      for(p = 0; p < 30; p++)
      {
         DEBUGLOAD(d.CH16482 + c, GetObjectWeight(d.CH16482[c].possessions[p]), +1, d.CH16482[c].possessions[p]);
         d.CH16482[c].load = sw(d.CH16482[c].load + GetObjectWeight(d.CH16482[c].Possession(p)));
      };
   };
   //The item in hand gets added elsewhere.
}
#endif

void swapTimers()
{
   TIMER_SEARCH timerSearch;
   //for (i32 i=0; i<d.MaxTimer(); i++)
   while(timerSearch.FindNextTimer())
   {
      TIMER *pTimer;
      pTimer = timerSearch.TimerAddr();
      //pTimer->timerTime = LE32(pTimer->timerTime);
      pTimer->m_time = LE32(pTimer->m_time);
      pTimer->swapTimerSequence();
      switch(pTimer->Function())
      {
         case 24:
            pTimer->swapTimerObj8();
            break;
         case TT_ReactivateGenerator:
            pTimer->swapTimerObj8();
            break;
         case 25:
            pTimer->swapTimerObj8();
            break;
         case 48:
         case 49:
            pTimer->swapTimerObj6();  // The flying object
            pTimer->swapTimerWord8(); // x, y, and direction
            break;
         case TT_ViAltar:
         case 60:
         case 61:
            pTimer->swapTimerObj8();
            break;
         case 70:
            pTimer->swapTimerWord6();
            break;
         case 72:
            pTimer->swapTimerWord6();
            break;
         case 74:
            pTimer->swapTimerWord6();
            break;
         case 75:
            pTimer->swapTimerWord6();
            break;
         case 77:
            pTimer->swapTimerWord6();
            break;
         case 78:
            pTimer->swapTimerWord6();
            break;
      };
   };
}

void swapLevelDescriptors(i32 numLevel)
{
   for(i32 j = 0; j < numLevel; j++)
   {
      d.pLevelDescriptors[j].cellOffset =
          LE16(d.pLevelDescriptors[j].cellOffset);
      d.pLevelDescriptors[j].word8 = LE16(d.pLevelDescriptors[j].word8);
      d.pLevelDescriptors[j].word10 = LE16(d.pLevelDescriptors[j].word10);
      d.pLevelDescriptors[j].word12 = LE16(d.pLevelDescriptors[j].word12);
      d.pLevelDescriptors[j].word14 = LE16(d.pLevelDescriptors[j].word14);
   };
}

// *********************************************************
//
// *********************************************************
//  TAG01d004
i16 WriteDataFile(i16 *pwbuf, i32 length)
{
   i32 result;
   //;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   if(length == 0)
   {
      return 1;
   }
   else
   {
      result = WRITE(d.datafileHandle, length, (ui8 *)pwbuf);
      return (I16)(length == result ? 1 : 0);
   };
}

//  TAG01d076
i16 GenChecksum(ui8 *buf, i16 initSum, i32 numWord)
{
   i16 result;
   //;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   result = initSum;
   do
   {
      result = (UI16)(result + LE16(wordGear(buf)));
      result = (UI16)(result + (LE16(wordGear(buf)) ^ initSum));
      initSum = (UI16)(initSum + numWord);
      buf += 2;
      numWord--;
   } while(numWord != 0);
   return result;
}

// *********************************************************
//
// *********************************************************
//  TAG01d0ea
i16 WriteScrambled(pnt buf,
                   i32 byteCount,
                   i16 initialHash,
                   i16 *checksum)
{
   dReg D0, D6, D7;
   aReg A3;
   //;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   A3 = buf;
   D6W = sw(byteCount / 2);
   D0W = Unscramble((ui8 *)A3, initialHash, D6W);
   *checksum = D0W;
   D7W = WriteDataFile((i16 *)A3, byteCount);
   D0W = Unscramble((ui8 *)A3, initialHash, D6W);
   return D7W;
}

i16 WriteAndChecksum(ui8 *buf,
                     i16 *pRunningChecksum, //updated
                     i32 numWord)
{
   dReg D0, D6;
   i32 count;
   count = numWord;
   ui8 *uA3;
   //;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   uA3 = buf;
   D6W = 0;
   D0W = WriteDataFile((i16 *)uA3, numWord);
   //
   if(D0W == 0)
      return 0;
   while((count--) != 0)
   {
      D6W = (UI16)(D6W + *(uA3++));
      //
   };
   *pRunningChecksum = (UI16)(*pRunningChecksum + D6W);
   return 1;
}

// *********************************************************
// Scramble and write the first 512-byte block of gamesave file.
// *********************************************************
//  TAG01dbb2
i16 ScrambleAndWrite(i16 *pwbuf)
{
   dReg D5, D6, D7;
   i16 *pwA3;
   i32 i;
   //;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   D5W = 0;
   pwA3 = pwbuf + 128;
   for(i = 0; i < 128; i++)
   {
      D5W = (UI16)(D5W + LE16(*(pwA3++)));
      //    fprintf(a, "D5W = 0x%04x\n", D5W);
   };
   // D5 is the sum of the second half of the buffer.
   D7W = 32;
   D6W = 0;
   pwA3 = pwbuf;
   for(;;)
   { //trash the first 32 * 4 * 2 = 256 bytes.
      //D0W = 0;//STRandom();
      *pwA3 = 0; //STRandom();//(i16)STRandom();
      D6W = (UI16)(D6W + LE16(*(pwA3++)));
      //D0W = 0;//STRandom();
      *pwA3 = 0; //STRandom();//(i16)STRandom();
      D6W ^= LE16(*(pwA3++));
      //D0W = 0;//STRandom();
      *pwA3 = 0; //STRandom();//(i16)STRandom();
      D6W = (UI16)(D6W - LE16(*(pwA3++)));
      D7W--;
      if(D7W != 0)
      {
         //D0W = 0;//STRandom();
         *pwA3 = 0; //STRandom();//(i16)STRandom();
         D6W ^= LE16(*(pwA3++));
         continue;
      };
      *(pwA3++) = LE16((ui16)(D5W ^ D6W));
      break;
   };
   //Scramble the 2nd 256 bytes.
   Unscramble((ui8 *)pwA3, LE16(pwbuf[29]), 128); // 128 words = 256 bytes
   D7W = WriteDataFile(pwbuf, 512);
   //Then unscramble the 2nd 256 bytes again for use.
   Unscramble((ui8 *)pwA3, LE16(pwbuf[29]), 128);
   return D7W;
}

ui16 countDSAs()
{
   i32 i;
   ui16 numDSA = 0;
   for(i = 0; i < 256; i++)
   {
      if(DSAIndex.GetDSA(i) == NULL)
         continue;
      numDSA++;
   };
   return numDSA;
}

void WriteDSAs(i32 handle)
{
   i32 i;
   ui32 checksum;
   WCS(handle); //Discard old checksum and start new.
   for(i = 0; i < 256; i++)
   {
      if(DSAIndex.GetDSA(i) == NULL)
         continue;
      WCS(&i);
      DSAIndex.GetDSA(i)->Write();
   };
   checksum = WCS(handle);
   WRITE((i16)handle, 4, (ui8 *)&checksum);
}

void WriteDSALevelIndex(i32 handle)
{
   i32 level, index;
   ui16 DSAindex;
   unsigned char triple[3];
   for(level = 0; level < 64; level++)
   {
      for(index = 0; index < 32; index++)
      {
         DSAindex = DSALevelIndex[level][index];
         if(DSAindex < 256)
         {
            triple[0] = (unsigned char)level;
            triple[1] = (unsigned char)index;
            triple[2] = (unsigned char)DSAindex;
            WRITE((i16)handle, 3, (ui8 *)triple);
         };
      };
   };
   triple[0] = triple[1] = triple[2] = 255;
   WRITE((i16)handle, 3, (ui8 *)triple);
}

struct BlockDesc
{
   pnt address;
   ui16 size;
};

// *********************************************************
//
// *********************************************************
// TAG001de4c
RESTARTABLE _DisplayDiskMenu()
{ //void
   static dReg D0, D1, D5, D6, D7;
   static aReg A3;
   static DBTYPE dbType;
   static i32 dbNum;
   static i16 *pwA3;
   //static BlockDesc memBlocks[16];
   static GAMEBLOCK1 *gb1A2;
   static GAMEBLOCK2 gb2;

   static i8 LOCAL_106[50];
   static i8 LOCAL_56[50];
   static i16 LOCAL_6;
   static i16 LOCAL_4 = 0x0ff0; //Initialize to avoid warning
   static RN obj_LOCAL_4 = RNeof;
   static i16 LOCAL_2;
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
   //;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   ASSERT(sizeof(gb2) == 128, "gb2");
   DiskMenuNeeded = false;
   if(disableSaves)
   {
      DoMenu(_9_,
             NULL,
             "SAVES ARE DISABLED",
             "OK",
             NULL,
             NULL,
             NULL,
             0,
             0,
             0);
      STShowCursor(HCiii);
      WaitForMenuSelect(_6_, 1, 0, 0, 0);
      STHideCursor(HC56);
      TAG0207cc();
      RETURN;
   };
   D0W = 0x0ff0; // Initialize to avoid warning
   D6W = 0x0ff0; // Initialize to avoid warning
   D7W = 0;
   TAG02076e();
   D7W = 0;
   STHideCursor(HC45);
   A3 = NULL;
   gb1A2 = (GAMEBLOCK1 *)allocateMemory(512, 0);
   memset(gb1A2, 0, 512);
   SubstDiskLetter((char *)LOCAL_56, d.Pointer22860, 2);
   // Put the Game Save Disk in ~"
   SubstDiskLetter((char *)LOCAL_106, d.Pointer22888, 2);
   // "There is no disk in ~"
   do
   {
      DoMenu(_1_,
             A3,
             TranslateLanguage("READY TO SAVE GAME"), // "Put the Game Save Disk in ~"
             TranslateLanguage(d.Pointer22896),       // "Save and Play"
             TranslateLanguage(d.Pointer22900),       // "Save and Quit"
             //d.Pointer22904, // "Format Floppy"
             TranslateLanguage(d.Pointer22912), // "Cancel"
             0,
             0,
             0,
             0);
      STShowCursor(HC45);
      WaitForMenuSelect(_3_, 3, 0, 0, 0);
      STHideCursor(HC57);
      D5W = i16Result;
      //    if (D5W == 2)
      //    {
      //      A3 = NULL;
      //    }
      //    else
      //    {
      if(D5W == 3)
      {
#ifdef _MSVC_CE2002ARM
         hidingMenu = false;
#endif
         goto tag01e52c; // free memory and return.
      };
      D6W = (I16)((D5W == 1) ? 1 : 0);
      D0L = 0;
      STShowCursor(HC57);
      SelectSaveGame(_5_, 0, 0, 1); //TAG0204bc
      STHideCursor(HC57);
      D7W = 1;
      //    };
   } while(D7W == 0);
   DoMenu(_7_, NULL, d.Pointer22872, 0, 0, 0, 0, 0, 0, 0); // "Saving Game"
   UNLINK((char *)d.Pointer22980);                         //TRAP 1 #0x41 "CSBGAMEx.BAK"
   RENAME(0,                                               //TRAP 1 #0x56
          d.SaveGameFilename,
          d.Pointer22980);
   d.datafileHandle = CREATE(d.SaveGameFilename, // TRAP 1 #0x3c
                             "w+b",
                             true); // "CSBGAMEx.DAT"
   if(d.datafileHandle >= 0)
   {
      if(encipheredDataFile)
      {
         SETENCIPHERED(d.datafileHandle, encipheredDataFile, 64);
      };
      if(simpleEncipher)
      {
         unsigned char simple[2];
         simple[0] = 1;
         simple[1] = 0;
         RC4_prepare_key(simple, 1);
         GETFILETABLE(d.datafileHandle)->Enciphered(true);
      };
      {
         ASSERT(sizeof(EXTENDEDFEATURESBLOCK) == 512, "extendedFeatures");
         EXTENDEDFEATURESBLOCK efb;
         memset(&efb, 0, 512);
         strcpy(efb.sentinel, " Extended Features ");
         efb.dataMapLength = dataMapLength;
         efb.cellFlagArraySize = cellflagArraySize;
         SwapDataIndexMap();
         efb.dataTypeMapChecksum =
             FormChecksum((ui8 *)dataTypeMap, dataMapLength);
         efb.dataIndexMapChecksum =
             FormChecksum((ui8 *)dataIndexMap, 2 * dataMapLength);
         efb.version = ExtendedFeaturesVersion;
         efb.numDSA = countDSAs();
         efb.graphicsSignature1 = expectedGraphicSignature1;
         efb.graphicsSignature2 = expectedGraphicSignature2;
         efb.CSBgraphicsSignature1 = expectedCSBgraphicSignature1;
         efb.CSBgraphicsSignature2 = expectedCSBgraphicSignature2;
         efb.spellFilterLocation = spellFilterLocation;
         memcpy(efb.hintKey, hintKey, 8);
         efb.flags |= EXTENDEDFEATURESBLOCK::LevelDSAInfoPresent;
         if(simpleEncipher)
         {
            efb.flags |= EXTENDEDFEATURESBLOCK::SimpleEncryption;
         };
         if(invisibleMonsters)
            efb.extendedFlags |= EXTENDEDFEATURESBLOCK::InvisibleMonsters;

         if(DM_rules)
            efb.extendedFlags |= EXTENDEDFEATURESBLOCK::DMRulesOption;
         if(extendedWallDecorations)
            efb.extendedFlags |= EXTENDEDFEATURESBLOCK::ExtendedWallDecorations;
         if(DMRulesDesignOption)
            efb.flags |= EXTENDEDFEATURESBLOCK::DMRules;

         if(drawAsSize4Monsters)
            efb.extendedFlags |= EXTENDEDFEATURESBLOCK::DrawAsSize4Monsters;
         if(RecordDesignOption)
            efb.flags |= EXTENDEDFEATURESBLOCK::AutoRecord;
         efb.flags |= EXTENDEDFEATURESBLOCK::IndirectText;
         efb.flags |= EXTENDEDFEATURESBLOCK::ExpandedPortraits;
         efb.flags |= EXTENDEDFEATURESBLOCK::BigActuators;
         efb.extendedFlags |= EXTENDEDFEATURESBLOCK::SequencedTimers;
         efb.extendedFlags |= EXTENDEDFEATURESBLOCK::ExtendedTimers;
         efb.extendedFlags |= EXTENDEDFEATURESBLOCK::DefaultDirectX;
         if(overlayActive)
         {
            efb.extendedFlags |= EXTENDEDFEATURESBLOCK::OverlayActive;
         };
         efb.overlayOrdinal = currentOverlay.m_overlayNumber + 1;
         efb.overlayP1 = currentOverlay.m_p1;
         efb.overlayP2 = currentOverlay.m_p2;
         efb.overlayP3 = currentOverlay.m_p3;
         efb.overlayP4 = currentOverlay.m_p4;
         efb.gameInfoSize = 0;
         if(gameInfo != NULL)
            efb.gameInfoSize = gameInfoSize - 1;
         efb.extendedFeaturesChecksum = 0;
         SwapExtendedFeaturesBlock(&efb);
         efb.extendedFeaturesChecksum =
             BE32(FormChecksum((ui8 *)&efb, 512));
         WRITE(d.datafileHandle, 512, (ui8 *)&efb);
         SwapExtendedFeaturesBlock(&efb);
         WRITE(d.datafileHandle, dataMapLength, dataTypeMap);
         WRITE(d.datafileHandle, 2 * dataMapLength, (ui8 *)dataIndexMap);
         SwapDataIndexMap();
         // *****************************************
         if(efb.numDSA != 0)
         {
            WriteDSAs(d.datafileHandle);
            DSAIndex.WriteTracing();
         };

         WriteGameInfo(d.datafileHandle, efb.gameInfoSize);
         // *****************************************
         WriteDSALevelIndex(d.datafileHandle);
      };
      if(d.EmptyHanded == 0)
      {
         ASSERT(RememberToPutObjectInHand == -1, "objInHand");
         D1L = GetObjectWeight(d.objectInHand);
         LOCAL_4 = D1W;
         obj_LOCAL_4 = d.objectInHand;
         DEBUGLOAD(d.CH16482 + d.HandChar, D1W, -1, d.objectInHand);
         d.CH16482[d.HandChar].load =
             sw(d.CH16482[d.HandChar].load - D1W); //owner of cursor
      };
      gb2.Time = d.Time;
      gb2.ranseed = d.RandomNumber;
      if(TimerTraceActive)
      {
         fprintf(GETFILE(TraceFile), "Saving d.RandomNumber to savefile = %08x\n", d.RandomNumber);
      };
      gb2.numcharacter = d.NumCharacter;
      gb2.partyx = d.partyX;
      gb2.partyy = d.partyY;
      gb2.partyfacing = d.partyFacing;
      gb2.partyLevel = d.partyLevel;
      gb2.handChar = d.HandChar;
      gb2.MagicCaster = d.MagicCaster;
      gb2.NumTimer = (ui16)gameTimers.NumTimer();
      gb2.FirstAvailTimer = (ui16)gameTimers.FirstAvailTimer();
      gb2.MaxTimers = (ui16)gameTimers.MaxTimer();
      gb2.TimerSequence = gameTimers.TimerSequence();
      gb2.ITEM16QueLen = d.ITEM16QueLen;
      gb2.LastMonsterAttackTime = d.LastMonsterAttackTime;
      gb2.LastPartyMoveTime = d.LastPartyMoveTime;
      gb2.partyMoveDisableTimer = d.partyMoveDisableTimer;
      gb2.Word11712 = d.Word11712;
      gb2.Word11714 = d.Word11714;
      ASSERT(RememberToPutObjectInHand == -1, "objInHand");
      gb2.ObjectInHand = d.objectInHand.ConvertToInteger();
      gb2.MaxITEM16 = d.MaxITEM16;
      ClearMemory((ui8 *)gb2.FILL180, 80);
      //ClearMemory((pnt)&memBlocks, 96);
      //    A0 = LOCAL_234;
      //    pntGear(LOCAL_330) = A0;
      //memBlocks[0].address = (pnt)&gb2;
      //    wordGear(LOCAL_330+330-326) = 128;
      //memBlocks[0].size = 128;
      //    pntGear(LOCAL_330+330-324) = (pnt)d.Item16;
      //memBlocks[1].address = (pnt)d.Item16;
      //    wordGear(LOCAL_330+330-320) = 16 * d.MaxITEM16;
      //memBlocks[1].size = sw(16 * d.MaxITEM16);
      //    pntGear(LOCAL_330+330-318) = (pnt)&d.CH16482;
      //memBlocks[2].address = (pnt)d.CH16482;
      //    wordGear(LOCAL_330+330-314) = 3328;
      //memBlocks[2].size = 3328;
      //    pntGear(LOCAL_330+330-312) = (pnt)d.Timers;
      //memBlocks[3].address = (pnt)allocateTimers.Timers();
      //    wordGear(LOCAL_330+330-308) = 10 * d.MaxTimers;
      //memBlocks[3].size = sw(10 * d.MaxTimer());
      //    pntGear(LOCAL_330+330-306) = (pnt)d.timerQue;
      //memBlocks[4].address = (pnt)d.timerQue;
      //    wordGear(LOCAL_330+330-302) = 2 * d.MaxTimers;
      //memBlocks[4].size = sw(2 * d.MaxTimers);
      gb1A2->SaveOption = D6W;
      MemMove((ui8 *)d.Byte22808, (ui8 *)gb1A2->Byte22808, 132);
      gb1A2->RandomGameID = d.RandomGameID;
      gb1A2->Byte22596 = ub(d.Word22596);
      gb1A2->Byte22598 = (i8)d.Word22598;
      gb1A2->Word22594 = d.Word22594;
      gb1A2->Word22592 = d.Word22592;
      //  for (D5W=0; D5W<16; D5W++)
      //  {
      gb1A2->Block2Hash = 0;                  //STRandom();//(i16)STRandom();
      gb1A2->ITEM16Hash = 0;                  //STRandom();//(i16)STRandom();
      gb1A2->CharacterHash = 0;               //STRandom();//(i16)STRandom();
      gb1A2->TimersHash = 0;                  //STRandom();//(i16)STRandom();
      gb1A2->TimerQueHash = 0;                //STRandom();//(i16)STRandom();
      gb1A2->totalMoveCount = totalMoveCount; //STRandom();//(i16)STRandom();
      //gb1A2->Hash324       = 0; //STRandom();//(i16)STRandom();
      gb1A2->Hash326 = 0; //STRandom();//(i16)STRandom();
      gb1A2->Hash328 = 0; //STRandom();//(i16)STRandom();
      gb1A2->Hash330 = 0; //STRandom();//(i16)STRandom();
      gb1A2->Hash332 = 0; //STRandom();//(i16)STRandom();
      gb1A2->Hash334 = 0; //STRandom();//(i16)STRandom();
      gb1A2->Hash336 = 0; //STRandom();//(i16)STRandom();
      gb1A2->Hash338 = 0; //STRandom();//(i16)STRandom();
      gb1A2->Hash340 = 0; //STRandom();//(i16)STRandom();
      gb1A2->Hash342 = 0; //STRandom();//(i16)STRandom();
                          //     wordGear(A2+312+2*D5W) = D0W;
                          //
                          //  };
                          //for (D5W=0; D5W<5; D5W++)
                          //{
      //
      //.........................................................
      swapBlock2(&gb2);
      gb1A2->Block2Checksum = GenChecksum(
          (ui8 *)&gb2, //
          gb1A2->Block2Hash,
          128 / 2);
      swapBlock2(&gb2);
      //.........................................................
      swapITEM16s();
      gb1A2->ITEM16Checksum = GenChecksum(
          (ui8 *)d.Item16, //
          gb1A2->ITEM16Hash,
          (16 * d.MaxITEM16) / 2);
      swapITEM16s();
      //.........................................................
      swapCharacterData();
      gb1A2->CharacterChecksum = GenChecksum(
          (ui8 *)&d.CH16482, //
          gb1A2->CharacterHash,
          3328 / 2);
      swapCharacterData();
      //.........................................................
      swapTimers();
      gb1A2->TimersChecksum = GenChecksum(
          (ui8 *)gameTimers.pTimer(0),
          gb1A2->TimersHash,
          (sizeof(TIMER) * gameTimers.MaxTimer()) / 2);
      swapTimers();
      //.........................................................
      gameTimers.swapTimerQue();
      gb1A2->TimerQueChecksum = GenChecksum(
          (ui8 *)gameTimers.TimerQueue(),
          gb1A2->TimerQueHash,
          (2 * gameTimers.MaxTimer()) / 2);
      gameTimers.swapTimerQue();
      //wordGear(gb1A2+344+2*D5W) = D0W;
      //
      //};
      swapBlock1(gb1A2);
      D0W = ScrambleAndWrite((i16 *)gb1A2);
      swapBlock1(gb1A2);
      if(D0W == 0)
         goto tag01e4f0;
      //for (D5W=0; D5W<5; D5W++)
      //{
      swapBlock2(&gb2);
      D0W = WriteScrambled(
          (pnt)&gb2,
          128,
          gb1A2->Block2Hash,
          &LOCAL_2);
      if(D0W == 0)
         goto tag01e4f0;
      swapBlock2(&gb2);
      //.........................................................
      swapITEM16s();
      D0W = WriteScrambled(
          (pnt)d.Item16,
          16 * d.MaxITEM16,
          gb1A2->ITEM16Hash,
          &LOCAL_2);
      if(D0W == 0)
         goto tag01e4f0;
      swapITEM16s();
      //.........................................................
      swapCharacterData();
      D0W = WriteScrambled(
          (pnt)d.CH16482,
          3328,
          gb1A2->CharacterHash,
          &LOCAL_2);
      swapCharacterData();
      if(D0W == 0)
         goto tag01e4f0;
      //.........................................................
      swapTimers();
      D0W = WriteScrambled(
          (pnt)gameTimers.pTimer(0),
          sizeof(TIMER) * gameTimers.MaxTimer(),
          gb1A2->TimersHash,
          &LOCAL_2);
      swapTimers();
      if(D0W == 0)
         goto tag01e4f0;
      //.........................................................
      gameTimers.swapTimerQue();
      D0W = WriteScrambled(
          (pnt)gameTimers.TimerQueue(),
          2 * gameTimers.MaxTimer(),
          gb1A2->TimerQueHash,
          &LOCAL_2);
      gameTimers.swapTimerQue();
      if(D0W == 0)
         goto tag01e4f0;
      //
      //};
      pwA3 = &LOCAL_6;

      {
         //Write all Global variables to expool.
         i32 i, size;
         ui32 *pRecord;
         for(i = 0; i < numGlobalVariables / 16; i++)
         {
            size = expool.Locate((EDT_Database << 24) | (EDBT_GlobalVariables << 16) | i, &pRecord);
            if(size < 16)
               UI_MessageBox("Error saving global variables", "Warning", MESSAGE_OK);
            memcpy(pRecord, globalVariables + 16 * i, 16 * sizeof(*pRecord));
         };
      };
      {
         // Write current palette to expool
         int i;
         ui8 palette[3 * 512];
         memcpy(palette, overlayPaletteRed, 512);
         memcpy(palette + 512, overlayPaletteGreen, 512);
         memcpy(palette + 1024, overlayPaletteBlue, 512);
         for(i = 0; i < 24; i++)
         {
            ui32 key;
            ui32 junk[16];
            key = (EDT_Palette << 24) + i;
            expool.Read(key, junk, 16); // Discard the old record
            expool.Write(key, (ui32 *)(palette + 64 * i), 16);
         };
      };

      LOCAL_6 = 0; // initial hash
      d.dungeonDatIndex->Swap();
      D0W = WriteAndChecksum((ui8 *)d.dungeonDatIndex, pwA3, 44);
      d.dungeonDatIndex->Swap();
      if(D0W == 0)
         goto tag01e4f0;
      // ******

      swapLevelDescriptors(d.dungeonDatIndex->NumLevel());
      D0W = WriteAndChecksum(
          (ui8 *)d.pLevelDescriptors,
          pwA3,
          16 * d.dungeonDatIndex->NumLevel());
      swapLevelDescriptors(d.dungeonDatIndex->NumLevel());
      if(D0W == 0)
         goto tag01e4f0;
      // ******

      swapPointer10454();
      D0W = WriteAndChecksum(
          (ui8 *)d.objectListIndex,
          pwA3,
          2 * d.numColumnPointers);
      swapPointer10454();
      if(D0W == 0)
         goto tag01e4f0;
      // ******

      swapPRN10464(d.dungeonDatIndex->ObjectListLength());
      D0W = WriteAndChecksum(
          (ui8 *)d.objectList.data(),
          pwA3,
          2 * d.dungeonDatIndex->ObjectListLength());
      swapPRN10464(d.dungeonDatIndex->ObjectListLength());
      if(D0W == 0)
         goto tag01e4f0;
      // ******

      SwapIndirectTextIndex();
      D0W = WriteAndChecksum(
          (ui8 *)d.indirectTextIndex.data(),
          pwA3,
          4 * d.dungeonDatIndex->NumWordsInTextArray());
      SwapIndirectTextIndex();
      if(D0W == 0)
         goto tag01e4f0;
      // ******

      i32 sizeOfCompressedText = BE32(d.compressedText.size());
      D0W = WriteAndChecksum(
          (ui8 *)&sizeOfCompressedText,
          pwA3,
          4);
      if(D0W == 0)
         goto tag01e4f0;
      // ******

      D0W = WriteAndChecksum(
          (ui8 *)d.compressedText.data(),
          pwA3,
          2 * d.compressedText.size());
      if(D0W == 0)
         goto tag01e4f0;
      if(disableSaves)
      {
         expool.Write((EDT_Database << 24) | (EDBT_DisableSaves << 16), NULL, 0);
      };
      for(dbNum = 0; dbNum < 16; dbNum++)
      {
         D1W = d.dungeonDatIndex->DBSize(dbNum); //Number of entries
         D0L = dbEntrySizes[dbNum];
         //0e462 d0c0                     ADD.W    D0,A0
         if(D1W != 0)
         {
            db.swap(dbNum);
            D0W = WriteAndChecksum(
                (ui8 *)db.GetCommonAddress(DBTYPE(dbNum), 0),
                pwA3,
                D1W * D0W);

            db.swap(dbNum);
         }
         else
            D0W = 1;
         if(D0W == 0)
            goto tag01e4f0;
         //
      };
      D0W = WriteAndChecksum(
          (ui8 *)d.cellFlagArray,
          pwA3,
          cellflagArraySize);
      if(D0W == 0)
         goto tag01e4f0;
      *pwA3 = LE16(*pwA3);
      D0W = WriteDataFile(pwA3, 2); // Write final checksum
      *pwA3 = LE16(*pwA3);
      if(D0W == 0)
         goto tag01e4f0;
      {
         ui32 saveSignature1, saveSignature2;
         LSEEK(0, d.datafileHandle, SEEK_SET);
         Signature(d.datafileHandle,
                   &saveSignature1,
                   &saveSignature2);
         RecordFile_Record((saveSignature1 >> 16) & 0xffff,
                           saveSignature1 & 0xffff,
                           0x6668);
      };
      CLOSE(d.datafileHandle);
      // *****************************************************************
      // The following was an attempt to make it possible to
      // splice recordings from before and after a 'Save' operation.
      // The 'Restore' operation did a 'ProcessMonstersOnLevel'.  So
      // I figured that if I did it during a 'Save' then the results
      // would be identical.
      TAG00bd40(); //Remove the Active Monster (ITEM16) entries
      ProcessMonstersOnLevel();
      SmartDiscard(true); //Initialize Smart Discard.  0x2223
                          //in the playfile.log also does this
                          // *****************************************************************
      if(D6W == 0)        //if ("Save and Quit");
      {
         //STShowCursor(HCccc);
         STShowCursor(HC57); //20130821
         ShowCredits(_2_, 0);
      };
      if(d.EmptyHanded == 0)
      {
         DEBUGLOAD(d.CH16482 + d.HandChar, LOCAL_4, +1, obj_LOCAL_4);
         d.CH16482[d.HandChar].load =
             sw(d.CH16482[d.HandChar].load + LOCAL_4); //owner of cursor
      };
      d.CanRestartFromSavegame = 1;
      goto tag01e52c;
   };
tag01e4f0:
   CLOSE(d.datafileHandle);
   DoMenu(_8_,
          d.Pointer22828, // title
          NULL,           // title2
          d.Pointer22908, // opt1
          0,
          0,
          0,
          0,
          0,
          0);
   STShowCursor(HC57);
   WaitForMenuSelect(_4_, 1, 0, 0, 0);
   STHideCursor(HC57);
   D0W = i16Result;
tag01e52c:
   FreeTempMemory(512);
   InsertDisk(D0W, 0);
   TAG0207cc();
   STShowCursor(HC57);
   RETURN;
}

void CheckMonster(RN obj)
{
   DB4 *pMonster;
   RN poss;
   i32 numMonster, monsterSize, monsterType;
   i32 pos1, pos2;
   static bool warning4Issued = false, warning2Issued = false;
   pMonster = GetRecordAddressDB4(obj);
   numMonster = pMonster->numMonM1() + 1;
   monsterType = pMonster->monsterType();
   monsterSize = d.MonsterDescriptor[monsterType].horizontalSize();
   switch(monsterSize)
   {
      case 0: //Any number of monsters is OK.
         break;
      case 1: //size two.....worms perhaps?
         if(numMonster > 2)
         {
            if(!warning4Issued)
               UI_MessageBox("Too many monsters of size 2", "Fixing...", MESSAGE_OK);
            warning4Issued = true;
            pMonster->numMonM1(1);
            pMonster->groupIndex(2);
            pMonster->facing(0);
         };
         pos1 = pMonster->groupIndex() & 3;
         pos2 = (pMonster->groupIndex() >> 2) & 3;
         if((numMonster == 2) && ((pos1 ^ pos2) != 2))
         {
            //UI_MessageBox("Size 2 monsters improperly positioned","Fixed...",MESSAGE_OK);
            pMonster->groupIndex(2);
            pMonster->facing(0);
         };
         break;
      case 2:
         if(numMonster > 1)
         {
            if(!warning2Issued)
               UI_MessageBox("Too many monsters of size 4", "Fixing...", MESSAGE_OK);
            warning2Issued = true;
            pMonster->numMonM1(0);
            pMonster->groupIndex(255);
            pMonster->facing(0);
         };
         pos1 = pMonster->groupIndex();
         // It appears that position is not important for size 4 monsters
         if(pos1 != 255)
         {
            //  UI_MessageBox("Size 4 monsters improperly positioned","Fixed...",MESSAGE_OK);
            pMonster->groupIndex(255);
            pMonster->facing(0);
         };
         break;
      default:
         break;
   };
   for(poss = pMonster->possession(); poss != RNeof; poss = FindNextMonster(poss))
   {
      if(poss.dbType() != dbMONSTER)
         continue;
      CheckMonster(poss);
   };
}

void CheckMonsters()
{
   i32 numLevel, width, height;
   i32 level, x, y;
   RN obj;
   numLevel = d.dungeonDatIndex->NumLevel();
   for(level = 0; level < numLevel; level++)
   {
      width = d.pLevelDescriptors[level].LastColumn() + 1;
      for(x = 0; x < width; x++)
      {
         height = d.pLevelDescriptors[level].LastRow() + 1;
         for(y = 0; y < height; y++)
         {
            for(obj = FindFirstMonster(level, x, y);
                obj != RNeof;
                obj = FindNextMonster(obj))
            {
               CheckMonster(obj);
            };
         };
      };
   };
}

void CheckCelltypes()
{
   i32 numLevel, level, x, y;
   i32 width, height;
   ROOMTYPE roomtype;
   CELLFLAG *pcell;
   numLevel = d.dungeonDatIndex->NumLevel();
   for(level = 0; level < numLevel; level++)
   {
      width = d.pLevelDescriptors[level].LastColumn() + 1;
      for(x = 0; x < width; x++)
      {
         height = d.pLevelDescriptors[level].LastRow() + 1;
         for(y = 0; y < height; y++)
         {
            pcell = GetCellFlagsAddress(level, x, y);
            roomtype = (ROOMTYPE)((*pcell) >> 5);
            switch(roomtype)
            {
               case roomSTONE:
               case roomOPEN:
               case roomSTAIRS:
               case roomTELEPORTER:
               case roomPIT:
               case roomFALSEWALL:
               case roomDOOR:
                  break;
               default:
                  *pcell = (CELLFLAG)(((*pcell) & 0x1f) | (roomOPEN << 5));
            };
         };
      };
   };
}

void ExtendPortraits()
{
   i32 i;
   i32 numEnt;
   DB3 *pActuator;
   if(extendedPortraits)
      return;
   numEnt = db.NumEntry(dbACTUATOR);
   for(i = 0; i < numEnt; i++)
   {
      pActuator = GetRecordAddressDB3(i);
      if(pActuator->link() == RNnul)
         continue;
      if(pActuator->actuatorType() == 127)
      {
         pActuator->ClearResponses(0);
         pActuator->SetResponses(0);
         pActuator->ToggleResponses(0);
         pActuator->ClearUnusedPor();
         pActuator->State(0);
         pActuator->BlockDisables(false);
         pActuator->DisableMessage(0);
         pActuator->Position(0);
      };
   };
   extendedPortraits = true;
}

static str1eb18 b; // Our local variables

// *********************************************************
//
// *********************************************************
//          TAG01eb18
RESTARTABLE _ReadEntireGame()
{ //i16
   static dReg D0, D1, D5, D6, D7;
   static dReg saveD0;
   static aReg A3;
   i32 numLevel;
   RESTARTMAP
   //RESTART(1)
   RESTART(2)
   RESTART(3)
   RESTART(4)
   RESTART(5)
   RESTART(6)
   RESTART(7)
   //RESTART(8)
   RESTART(9)
   //RESTART(10)
   //RESTART(11)
   RESTART(12)
   RESTART(13)
   RESTART(14)
   RESTART(15)
   RESTART(16)
   END_RESTARTMAP
   //;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   ASSERT(sizeof(b) == 740, "b");
   indirectText = false;
   extendedPortraits = false; // 20150806
   memset(&b, 0, sizeof(b));
   D6L = 0;
   //SaveRegs(0x0710);
   D5W = 0;
   d.Word22596 = 2;
   d.Word22598 = 1;
   d.Word22594 = 1;
   d.Word22592 = 13;
   A3 = NULL;
   currentOverlay.Cleanup();
   if(d.gameState == GAMESTATE_ResumeSavedGame)
      goto tag01ebbe;
   D0W = InsertDisk(0, 1);
   //D0W = i16Result;
   if(D0W == 0)
      D0W = 1;
   else
      D0W = 0;
   D5W = D0W;
   d.datafileHandle = OPEN(dungeonName, "rb");
   if(d.datafileHandle < 0)
   {
      char *msg;
      msg = (char *)UI_malloc(strlen(dungeonName) + 100, MALLOC030);
      sprintf(msg, "Cannot open dungeon file '%s'", dungeonName);
      die(50, msg);
      UI_free(msg);
   };
tag01eb82:
   Signature(d.datafileHandle, &dungeonSignature1, &dungeonSignature2);
   if(encipheredDataFile != NULL)
   {
      SETENCIPHERED(d.datafileHandle, encipheredDataFile, 64);
   };
   d.CanRestartFromSavegame = 0;
   d.NumCharacter = 0;
   ASSERT(RememberToPutObjectInHand == -1, "objInHand");

   {
      CURSORFILTER_PACKET cfp;
      cfp.type = CURSORFILTER_ReadGame;
      cfp.object = RN(RNnul).ConvertToInteger();
      CursorFilter(&cfp);
   };
   d.objectInHand = RN(RNnul);

   if(IsPlayFileOpen())
   {
      MouseQueueEnt MQ;
      PlayFile_Play(&MQ);
      if(MQ.num != 0x3333)
      {
         i32 ans;
         ans = UI_MessageBox("Replay Starting Random Error", "Error", MESSAGE_YESNO);
         if(ans == MESSAGE_IDYES)
            die(0x7bbd);
      };
      // d.RandomNumber was set in PlayFile.Play.
   }
   else
   {
      MouseQueueEnt MQ;
      MQ.num = 0x3333;
      MQ.x = (ui16)((d.RandomNumber >> 16) & 0xffff);
      MQ.y = (ui16)(d.RandomNumber & 0xffff);
      RecordFile_Record(&MQ);
   };
   D0L = STRandom();
   D0L &= 0xffff;
   //SaveRegs(0x8000); // D0
   saveD0 = D0;
   D1L = STRandom();
   //RestoreRegs(0x0001);// D0
   D0 = saveD0;
   /////////
   D0L = D0UW * D1UW;
   //////////
   d.RandomGameID = D0L;
   goto tag01eec4;
tag01ebbe:
   A3 = NULL;
   SubstDiskLetter((char *)b.Byte50, d.Pointer22860, 2);
   SubstDiskLetter((char *)b.Byte100, d.Pointer22888, 2);
tag01ebec:
   if(!d.SingleFloppyDrive)
      goto tag01ec08;

   D0W = 1;
   if(D0W != 1)
      goto tag01ec12;
tag01ec08:
   if(A3 == NULL)
   {
      //STHideCursor(HC59);
      goto tag01ec6a;
   };
tag01ec12:
   DoMenu(_12_,
          A3,
          b.Byte50,
          d.Pointer22892,
          d.Pointer22912,
          NULL,
          NULL,
          1,
          1,
          1);
   STShowCursor(HC33);
   WaitForMenuSelect(_7_, 2, 1, 2, 1);
   STHideCursor(HC33);
   ExtendedFeaturesSize = 0;
   D7W = D0W = i16Result;
   if(D7W != 2)
      goto tag01ec6a;
tag01ec56:
   InsertDisk(0, 1);
   D0W = -1;
   RETURN_i16(D0W);
tag01ec6a:
   D0L = 1;
   STShowCursor(HC33);
   SelectSaveGame(_9_, 1, 1, 1); //TAG0204bc
   STHideCursor(HC33);
   d.Word22972 = 1;
   D6W = 0;
   D0W = OPEN(d.SaveGameFilename, "rb");
   d.datafileHandle = D0W;
   if(D0W >= 0)
      goto tag01ece8;
   OPEN(d.Pointer22980, "rb");
   d.datafileHandle = D0W;
   if(D0W < 0)
      goto tag01ecb2;
   D6W = 1;
   goto tag01ece8;
tag01ecb2:
   D0W = d.PartyHasDied;
   if(D0W != 0)
      goto tag01ecde;
   D0W = OPEN("DUNGEON.FTL", "rb");
   d.datafileHandle = D0W;
   if(D0W < 0)
      goto tag01ecde;
   D5W = 1;
   d.gameState = GAMESTATE_EnterPrison;
   goto tag01eb82;
tag01ecde:
   A3 = (aReg)d.Pointer22824;
   d.Word22972 = 3;
tag01ece8:
   Signature(d.datafileHandle, &dungeonSignature1, &dungeonSignature2);
   RecordFile_Record((dungeonSignature1 >> 16) & 0xffff,
                     dungeonSignature1 & 0xffff,
                     0x6669);
   if(encipheredDataFile)
   {
      SETENCIPHERED(d.datafileHandle, encipheredDataFile, 64);
   };
   if(d.Word22972 != 1)
      goto tag01ec12;
   // Read the first 512 bytes from the game file and
   // unscramble it.
   ExtendedFeaturesSize = ReadExtendedFeatures(d.datafileHandle);
   D0W = ReadUnscrambleBlock((ui8 *)&b.gb1); //TAG01db46
   if(D0W == 0)
      goto tag01efee;  //If error
   swapBlock1(&b.gb1); // Swap words/longs as necessary in block 1
   totalMoveCount = b.gb1.totalMoveCount;
   if(d.PartyHasDied == 0)
      goto tag01ed54;
   if(b.gb1.RandomGameID == d.RandomGameID)
      goto tag01ed54;
   A3 = (aReg)d.Pointer22852;
   goto tag01ebec;
tag01ed54:
   d.RandomGameID = b.gb1.RandomGameID;
   MemMove((ui8 *)b.gb1.Byte22808, (ui8 *)d.Byte22808, 132); //All zeroes in my CSBGAME2.DAT
   D0W = b.gb1.Byte22596;
   if(D0W == 1)
      goto tag01ed86;
   // Removed this to be able to read Atari DM saved game.  if (b.gb1.Word22594 != 1) goto tag01efee;
tag01ed86:
   // Read second block of data from game file
   D0W = UnscrambleStream((ui8 *)&b.gb2,         // buffer
                          128,                   // #bytes
                          b.gb1.Block2Hash,      // initial hash
                          b.gb1.Block2Checksum); // expected checksum
   if(D0W == 0)
      goto tag01efee;
   swapBlock2(&b.gb2); // fix byte order
   scrollingText.ResetTime(b.gb2.Time);
   d.Time = d.Long11732 = b.gb2.Time;
   parameterMessageSequence = 0;
   d.RandomNumber = b.gb2.ranseed;
   if(TimerTraceActive)
   {
      fprintf(GETFILE(TraceFile), "Setting d.RandomNumber from savefile = %08x\n", d.RandomNumber);
   };
   d.NumCharacter = b.gb2.numcharacter;
   d.partyX = b.gb2.partyx;
   d.partyY = b.gb2.partyy;
   d.partyFacing = b.gb2.partyfacing;
   d.partyLevel = b.gb2.partyLevel;
   d.HandChar = b.gb2.handChar; // chIdx
   d.MagicCaster = b.gb2.MagicCaster;
   gameTimers.Allocate(b.gb2.MaxTimers);
   gameTimers.NumTimer(b.gb2.NumTimer);
   gameTimers.FirstAvailTimer(b.gb2.FirstAvailTimer);
   gameTimers.TimerSequence(b.gb2.TimerSequence);
   d.ITEM16QueLen = b.gb2.ITEM16QueLen;
   d.LastMonsterAttackTime = b.gb2.LastMonsterAttackTime;
   d.LastPartyMoveTime = b.gb2.LastPartyMoveTime;
   d.partyMoveDisableTimer = b.gb2.partyMoveDisableTimer;
   d.Word11712 = b.gb2.Word11712;
   d.Word11714 = b.gb2.Word11714;

   d.objectInHand.NonExtendedInteger(b.gb2.ObjectInHand);
   {
      CURSORFILTER_PACKET cfp;
      cfp.object = d.objectInHand.ConvertToInteger();
      cfp.type = CURSORFILTER_ReadGame;
      CursorFilter(&cfp);
   };

   //RememberToPutObjectInHand = b.gb2.ObjectInHand;
   d.MaxITEM16 = b.gb2.MaxITEM16;
   do
   {
      HandleMouseEvents(_16_, -999887); //initialize
   } while(intResult & 1);
   D0W = d.PartyHasDied;
   if(D0W == 0)
   {
      gameTimers.InitializeTimers();
      InitializeItem16();
   };
   if(d.MaxITEM16 != 0)
   {
      D0W = UnscrambleStream((ui8 *)d.Item16,  // buffer
                             16 * d.MaxITEM16, // #bytes
                             b.gb1.ITEM16Hash,
                             b.gb1.ITEM16Checksum);
      if(D0W == 0)
         goto tag01efee;
      swapITEM16s();
   };
   D0W = UnscrambleStream((ui8 *)d.CH16482, // buffer
                          3328,             // # bytes
                          b.gb1.CharacterHash,
                          b.gb1.CharacterChecksum);
   if(D0W == 0)
      goto tag01efee;
   swapCharacterData();
   {
      int timerSize;
      //timerSize = sequencedTimers?sizeof(TIMER):10;
      timerSize = sequencedTimers ? 12 : 10;
      timerSize = extendedTimers ? 16 : timerSize;
      D0W = UnscrambleStream((ui8 *)gameTimers.pTimer(0),             // buffer
                             (ui16)gameTimers.MaxTimer() * timerSize, // # bytes
                             b.gb1.TimersHash,                        // initial hash
                             b.gb1.TimersChecksum);                   // expected checksum
   };
   if(D0W == 0)
      goto tag01efee;
   D0W = UnscrambleStream((ui8 *)gameTimers.TimerQueue(),  // buffer
                          (ui16)gameTimers.MaxTimer() * 2, // #bytes
                          b.gb1.TimerQueHash,              // initial hash
                          b.gb1.TimerQueChecksum);         // expected checksum
   if(D0W == 0)
      goto tag01efee;

   gameTimers.swapTimerQue();

   if(!sequencedTimers)
   {

      {
         int i;
         if(GETFILE(TraceFile) != NULL)
         {
            for(i = 0; i < gameTimers.m_numTimer; i++)
            {
               char *pTimer;
               int j;
               pTimer = (char *)gameTimers.m_timers.data() + 10 * gameTimers.m_timerQueue[i];
               fprintf(GETFILE(TraceFile), "%04x   ", gameTimers.m_timerQueue[i]);
               for(j = 0; j < sizeof(TIMER); j++)
               {
                  fprintf(GETFILE(TraceFile), "%02x ", *(pTimer + j) & 0xff);
               };
               fprintf(GETFILE(TraceFile), "\n");
            };
         };
      };

      gameTimers.ConvertToSequencedTimers();
   };

   if(GETFILE(TraceFile) != NULL)
   {
      for(int i = 0; i < gameTimers.m_numTimer; i++)
      {
         TIMER *pTimer = &gameTimers.m_timers[gameTimers.m_timerQueue[i]];
         fprintf(GETFILE(TraceFile), "%04x   ", gameTimers.m_timerQueue[i]);
         for(int j = 0; j < sizeof(TIMER); j++)
         {
            fprintf(GETFILE(TraceFile), "%02x ", (*((char *)pTimer + j)) & 0xff);
         }
         fprintf(GETFILE(TraceFile), "\n");
      }
   }

   if(!extendedTimers)
      gameTimers.ConvertToExtendedTimers();

   swapTimers();

   if(GETFILE(TraceFile) != NULL)
   {
      for(int i = 0; i < gameTimers.m_numTimer; i++)
      {
         TIMER *pTimer = &gameTimers.m_timers[gameTimers.m_timerQueue[i]];
         fprintf(GETFILE(TraceFile), "%04x   ", gameTimers.m_timerQueue[i]);
         for(int j = 0; j < sizeof(TIMER); j++)
         {
            fprintf(GETFILE(TraceFile), "%02x ", (*((char *)pTimer + j)) & 0xff);
         }
         fprintf(GETFILE(TraceFile), "\n");
      }
   }

tag01eec4:
   D0W = ReadDatabases();
   if(D0W == 0)
      goto tag01efee;
   {
      i32 i, size;
      ui32 *pRecord;
      //Remove all current Global Variables.
      if(globalVariables != NULL)
         UI_free(globalVariables);
      numGlobalVariables = 0;
      globalVariables = NULL;
      //Read all Global variables.
      for(i = 0; i < 999; i++)
      {
         size = expool.Locate((EDT_Database << 24) | (EDBT_GlobalVariables << 16) | i, &pRecord);
         if(size < 16)
            break;
         numGlobalVariables += 16;
         globalVariables = (ui32 *)UI_realloc(globalVariables, 16 * (i + 1) * sizeof(ui32), MALLOC105);
         memcpy(&globalVariables[16 * i], pRecord, 16 * sizeof(ui32));
      };
      // Now we will see if a record of DSA tracing flags exists.
      DSAIndex.ReadTracing();
   };
   {
      // Read the palette in effect at time of save.
      int i;
      ui32 *pRecord = NULL;
      ui8 palette[3 * 512];
      for(i = 0; i < 24; i++)
      {
         ui32 key;
         int size;
         key = (EDT_Palette << 24) + i;
         size = expool.Locate(key, &pRecord);
         if(size >= 16)
         {
            memcpy(palette + 64 * i, pRecord, 64);
         }
         else
         {
            pRecord = NULL;
            break;
         };
      };
      if(pRecord != NULL)
      {
         memcpy(overlayPaletteRed, palette + 0, 512);
         memcpy(overlayPaletteGreen, palette + 512, 512);
         memcpy(overlayPaletteBlue, palette + 1024, 512);
      };
   };
   {
      ui32 junk[32];
      disableSaves = false;
      while(expool.Read((EDT_Database << 24) | (EDBT_DisableSaves << 16), junk, 32) >= 0)
         disableSaves = true;
   };
   {
      ui32 *pRec;
      if(expool.Locate((EDT_Database << 24) + (EDBT_DeleteDuplicateTimers << 16), &pRec) < 0)
      {
         deleteDuplicateTimers = 1; // The old way
      }
      else
      {
         deleteDuplicateTimers = *pRec;
      };
   };

   {
      ui32 *pRec;
      if(expool.Locate((EDT_Database << 24) + ((EDBT_Debuging << 16) + 0), &pRec) < 1)
      {
         EDBT_Debuging_data = 0;
      }
      else
      {
         EDBT_Debuging_data = *pRec;
      };
   };

   EDBT_CSBGraphicsSignature_data = 0;
   EDBT_GraphicsSignature_data = 0;
   EDBT_CSBversion_data = 0;

   if(EDBT_Debuging_data == 0)
   {
      ui32 *pRec;
      if(expool.Locate((EDT_Database << 24) + (EDBT_RuntimeFileSignatures << 16) + 0, &pRec) < 1)
      {
         EDBT_CSBGraphicsSignature_data = 0;
      }
      else
      {
         EDBT_CSBGraphicsSignature_data = *pRec;
      };

      if(expool.Locate((EDT_Database << 24) + (EDBT_RuntimeFileSignatures << 16) + 2, &pRec) < 1)
      {
         EDBT_CSBversion_data = 0;
      }
      else
      {
         EDBT_CSBversion_data = *pRec;
      };

      if(expool.Locate((EDT_Database << 24) + (EDBT_RuntimeFileSignatures << 16) + 1, &pRec) < 1)
      {
         EDBT_GraphicsSignature_data = 0;
      }
      else
      {
         EDBT_GraphicsSignature_data = *pRec;
      };
   };

   openGraphicsFile(); // Force a graphics.dat signature check
   closeGraphicsFile();

   if(D0W == 0)
      goto tag01efee;
   CheckCelltypes();
   CheckMonsters();
   ExtendPortraits();
   if(D0W == 0)
   {
      CLOSE(d.datafileHandle);
      goto tag01efee;
   };
#ifdef _DEBUG
   //An early version made the character load weight wrong.
   //We will fix them up as we read the saved games.
   recomputeLoads();
#endif
   numLevel = d.dungeonDatIndex->NumLevel();
   if(numLevel > 2)
   {
      ItemsRemainingOK = true; //Enable Menu "view" "items remaining"
   };
   CLOSE(d.datafileHandle);
   if(d.gameState == GAMESTATE_ResumeSavedGame)
      goto tag01ef2c;
   gameTimers.InitializeTimers();
   InitializeItem16();
   do
   {
      HandleMouseEvents(_15_, -999887); //Cue to initialize
   } while(intResult & 1);
   if(D5W == 0)
      goto tag01ef1c;
   FadeToPalette(_5_, &d.Palette11914);
   wvbl(_2_);
   ClearMemory(d.LogicalScreenBase, 32000);
   FadeToPalette(_3_, &d.Palette11978);
tag01ef1c:
   InsertDisk(0, 1);
   goto tag01efe4;
tag01ef2c:
   D0W = (UI8)(b.gb1.Byte22598);
   d.Word22598 = D0W;
   D0W = (UI8)(b.gb1.Byte22596);
   if(D0W != 1)
      goto tag01ef82;
   D7W = (I16)d.dungeonDatIndex->NumLevel();
   if(D7W != 14)
      goto tag01ef64;
   if(d.dungeonDatIndex->Sentinel() != 99)
      goto tag01ef64;
   d.Word22592 = 10;
   goto tag01ef80;
tag01ef64:
   if(D7W != 10)
      goto tag01ef7c;
   if(d.dungeonDatIndex->Sentinel() != 76)
      goto tag01ef7c;
   d.Word22592 = 13;
   goto tag01ef80;
tag01ef7c:
   d.Word22592 = 0;
tag01ef80:
   goto tag01ef98;
tag01ef82:
   D0W = b.gb1.Byte22596;
   d.Word22596 = D0W;
   d.Word22594 = b.gb1.Word22594;
   d.Word22592 = b.gb1.Word22592;
tag01ef98:
   d.CanRestartFromSavegame = 1;
   D0W = D6W;
   if(D0W == 0)
      goto tag01efb8;
   RENAME(0, d.Pointer22980, d.SaveGameFilename);
tag01efb8:
   InsertDisk(1, 1);
   DoMenu(_13_, NULL, d.Pointer22876, NULL, NULL, NULL, NULL, 1, 1, 1);
tag01efe4:
   d.GameIsLost = 0;
   D0W = 1;
   RETURN_i16(D0W);
tag01efee:
   FadeToPalette(_4_, &d.Palette11978);
   D0W = d.PartyHasDied;
   if(D0W == 0)
      goto tag01f036;
   DoMenu(_14_,
          d.Pointer22856,
          d.Pointer22864,
          d.Pointer22908,
          NULL,
          NULL,
          NULL,
          1,
          1,
          1);
   STShowCursor(HC33);
   WaitForMenuSelect(_6_, 1, 1, 0, 0);
   STHideCursor(HC33);
   goto tag01ec56;
tag01f036:
   PRINTLINE(d.Pointer22856);
   die(0xbad);
   ;
   //goto tag01f036;
   //tag01f046:
   //RestoreRegs(0x08e0);
   RETURN_i16(D0W);
}

// *********************************************************
//
// *********************************************************
void ConvertListOfObjects(RN *pRN, bool DuplicateOK,
                          i32 level, i32 x, i32 y)
{
   i32 dbNum, pos, idx;
   i32 i, newDML, duplicate;
   DBCOMMON *pDB;
   while(*pRN != RNeof)
   {
      if((*pRN == RNnul) || pRN->IsMagicSpell())
      {
         UI_MessageBox("Illegal object", "Sorry", MB_OK);
         die(0xeed18);
      };
      dbNum = (pRN->ConvertToInteger() >> 10) & 15;
      pos = (pRN->ConvertToInteger() >> 14) & 3;
      idx = pRN->ConvertToInteger() & 0x3ff;
      if(idx >= db.NumEntry(dbNum))
      {
         char msg[80];
         sprintf(msg, "Illegal object at %d(%02d,%02d)", level, x, y);
         UI_MessageBox(msg, "Sorry", MB_OK);
         *pRN = RNeof;
         return;
      };
      pDB = db.GetCommonAddress(DBTYPE(dbNum), idx);
      duplicate = -1;
      for(i = 1; i < dataMapLength; i++)
      { //Search to see if it is already present
         if((dataTypeMap[i] & 15) != dbNum)
            continue;
         if(dataIndexMap[i] != idx)
            continue;
         if(DuplicateOK)
         {
            duplicate = i;
            break;
         }
         else
         {
            UI_MessageBox("Duplicated Object", "Sorry", MB_OK);
            die(0xccdde);
         };
      };
      if(duplicate >= 0)
      {
         pRN->ConstructFromInteger(duplicate);
      }
      else
      {
         newDML = 32 * (dataMapLength / 32) + 33;
         dataTypeMap = (ui8 *)UI_realloc(dataTypeMap, newDML, MALLOC051);
         dataIndexMap = (ui16 *)UI_realloc(dataIndexMap, 2 * newDML, MALLOC052);
         if(dataMapLength == 0)
         {
            // Don't use entry zero.
            dataTypeMap[0] = 0;
            dataIndexMap[0] = 0;
            dataMapLength = 1;
         };
         dataTypeMap[dataMapLength] = (ui8)((pos << 4) | dbNum);
         dataIndexMap[dataMapLength] = (ui16)idx;
         dataMapLength++;
         pRN->ConstructFromInteger(dataMapLength - 1);
      };
      if(duplicate == -1) //Don't convert objects twice.
      {
         switch(dbNum)
         {
            case dbCHEST: {
               DB9 *pChest;
               pChest = pDB->CastToDB9();
               if(pChest->contents() != RNeof)
               {
                  ConvertListOfObjects(pChest->pContents(), false, level, x, y);
               };
            };
            break;
            case dbMONSTER: {
               DB4 *pMonster;
               pMonster = pDB->CastToDB4();
               if(pMonster->possession() != RNeof)
               {
                  ConvertListOfObjects(pMonster->pPossession(), false, level, x, y);
               };
            };
            break;
            case dbMISSILE: {
               DB14 *pMissile;
               pMissile = pDB->CastToDB14();
               if((pMissile->flyingObject() != RNeof) && !pMissile->flyingObject().IsMagicSpell())
               {
                  ConvertListOfObjects(pMissile->pFlyingObject(), true, level, x, y);
               };
            };
            break;
            case dbSCROLL:
               //Nothing to do.  It has index of Text Entry.
               break;
         };
      };
      pRN = &(pDB->m_link);
   };
}

void ConvertTimers()
{
   TIMER_SEARCH timerSearch;
   //for (i32 i=0; i<d.MaxTimer(); i++)
   while(timerSearch.FindNextTimer())
   {
      TIMER *pTimer;
      pTimer = timerSearch.TimerAddr();
      switch(pTimer->Function())
      {
         case 24:
         case 25:
            ConvertListOfObjects(pTimer->pTimerObj8(), false, 0, -1, 0);
            break;
         case 48:
         case 49:
            ConvertListOfObjects(pTimer->pTimerObj6(), true, 0, -2, 0); // The flying object
            break;
         case 60:
         case 61:
            ConvertListOfObjects(pTimer->pTimerObj8(), false, 0, -3, 0);
            break;
      };
   };
}

void ConvertCharacters()
{
   i32 i, j;
   for(i = 0; i < d.NumCharacter; i++)
   {
      for(j = 0; j < 30; j++)
      {
         if(d.CH16482[i].Possession(j) != RNnul)
         {
            RN temp;
            temp = d.CH16482[i].Possession(j);
            ConvertListOfObjects(&temp, false, 0, -4, 0);
            d.CH16482[i].SetPossession(j, temp, true);
         };
      };
   };
}

i32 readOldScrollDatabase(i16 *pCheckSum)
{
   i32 entrySize, oldEntrySize;
   i32 i;
   ui8 *pOld, *pNew;
   i32 numEnt;
   i32 status;
   DBCOMMON *pDB;
   oldEntrySize = 4;
   entrySize = sizeof(DB7);
   numEnt = d.dungeonDatIndex->DBSize(dbSCROLL);
   if(numEnt == 0)
      return 1; //OK
   db.Allocate(dbSCROLL, numEnt);
   pDB = db.GetCommonAddress(DBTYPE(dbSCROLL), 0);
   status = FetchDataBytes((ui8 *)pDB, pCheckSum, (ui16)oldEntrySize * numEnt);
   if(status == 0)
      return 0;
   //Now make the entries the correct size.
   for(i = numEnt - 1; i >= 0; i--)
   {
      pOld = ((ui8 *)pDB) + oldEntrySize * i;
      *(ui16 *)pOld = LE16(*(ui16 *)pOld);             //The link
      *(ui16 *)(pOld + 2) = LE16(*(ui16 *)(pOld + 2)); //The text index and open flag.
      pNew = ((ui8 *)pDB) + entrySize * i;
      memmove(pNew, pOld, 4);
   };
   return 1; //OK
}

i32 AllocateIndirectIndex(DBTYPE dbType, i32 dbIndex)
{
   i32 emptyIndexEntry;
   DBCOMMON *pDBC;
   i32 i, newDML;
   emptyIndexEntry = -1;
   for(i = 1; i < dataMapLength; i++)
   {
      if(dataIndexMap[i] == 0xffff)
      {
         if(emptyIndexEntry < 0)
         {
            emptyIndexEntry = i;
         };
         continue;
      };
      if((dataTypeMap[i] & 15) != dbType)
         continue;
      pDBC = db.GetCommonAddress(dbType, dataIndexMap[i]);
      if(pDBC->link() == RNnul)
      {
         emptyIndexEntry = i;
         break;
      };
   };
   if(emptyIndexEntry == -1)
   {
      if(dataMapLength > 0xff00 - 100)
      {
         return -1;
      };
      newDML = 32 * (dataMapLength / 32) + 33;
      dataIndexMap = (ui16 *)UI_realloc(dataIndexMap, 2 * newDML, MALLOC053);
      dataTypeMap = (ui8 *)UI_realloc(dataTypeMap, newDML, MALLOC054);
      emptyIndexEntry = dataMapLength;
      dataMapLength++;
      dataIndexMap[emptyIndexEntry] = 0xffff;
   };
   dataIndexMap[emptyIndexEntry] = (ui16)dbIndex;
   dataTypeMap[emptyIndexEntry] = (ui8)dbType;
   return emptyIndexEntry;
}

void convertScrolls()
{
   RN RNtext;
   i32 numEnt;
   i32 i;
   i32 openFlag;
   DB7 *pScroll;
   ui16 *pWord;
   i32 indirectIndex;
   numEnt = d.dungeonDatIndex->DBSize(dbSCROLL);
   for(i = 0; i < numEnt; i++)
   {
      pScroll = db.GetCommonAddress(dbSCROLL, i)->CastToDB7();
      //The link is OK.
      pWord = (ui16 *)pScroll;
      indirectIndex = AllocateIndirectIndex(dbTEXT, pWord[1] & 0x3ff);
      openFlag = pWord[1] >> 10;
      RNtext.ConstructFromInteger(indirectIndex);
      pScroll->text(RNtext);
      pScroll->open(openFlag == 0);
   };
}

i32 readNewScrollDatabase(i16 *pCheckSum)
{
   i32 entrySize, numEnt;
   DBCOMMON *pDB;
   i32 status;
   entrySize = (UI8)(*(dbEntrySizes + dbSCROLL));
   numEnt = d.dungeonDatIndex->DBSize(dbSCROLL);
   db.Allocate(dbSCROLL, d.dungeonDatIndex->DBSize(dbSCROLL));
   if(d.dungeonDatIndex->DBSize(dbSCROLL) != 0)
   {
      pDB = db.GetCommonAddress(DBTYPE(dbSCROLL), 0);
      status = FetchDataBytes((ui8 *)pDB,
                              pCheckSum,

                              (ui16)entrySize * numEnt);
      if(status == 0)
         return 0;
      db.swap(dbSCROLL); // database
   };
   return 1; //OK
}

i32 readScrollDatabase(i16 *pCheckSum)
{
   //If extendedFeatures Version is less than 'B' then
   //we are going to read the old version of scrolls
   //database and convert to the new version.
   //The old version had a 10-bit index into the
   //text database.  Tne new version has an RN reference
   //to the text.
   if(ExtendedFeaturesVersion < 'B')
      return readOldScrollDatabase(pCheckSum);
   else
      return readNewScrollDatabase(pCheckSum);
}

i32 ComputeTextLength(ui16 *pCompText)
{ // Result is number of **16-bit WORDS** in text entry.
   i32 n1, n2, n3, len;
   len = 0;
   ui16 w;
   do
   {
      w = LE16(*pCompText);
      n1 = (w >> 10) & 31;
      n2 = (w >> 5) & 31;
      n3 = (w >> 0) & 31;
      pCompText++;
      len++;
   } while((n1 != 31) && (n2 != 31) && (n3 != 31));
   return len;
}

i32 ConvertToIndirectText(ui16 *text)
{
   d.indirectTextIndex.clear();
   d.compressedText.clear();
   for(int i = 0; i < db.NumEntry(dbTEXT); i++)
   {
      auto pDB2 = GetRecordAddressDB2(i);
      if(pDB2->link() == RNnul)
         continue;
      auto oldIndex = pDB2->index();
      if(oldIndex > d.dungeonDatIndex->NumWordsInTextArray())
      {
         UI_MessageBox("A Text Record has an illegal text index",
                       "Warning",
                       MESSAGE_IDOK);
         pDB2->index(0);
         continue;
      };
      auto len = ComputeTextLength(text + oldIndex);
      //Add one entry to m_indirectTextIndex;
      auto newIndex = d.indirectTextIndex.size();
      d.indirectTextIndex.push_back(d.compressedText.size());
      d.compressedText.insert(d.compressedText.end(), text + oldIndex, text + oldIndex + len);
      pDB2->index(newIndex);
   };
   d.dungeonDatIndex->NumWordsInTextArray((ui16)d.indirectTextIndex.size());
   return 1;
}

void MakeBigActuators()
{
   i8 *source, *destination;
   i32 oldSize, newSize, numEnt;
   i32 i, j;
   DB3 *pActuator;
   numEnt = d.dungeonDatIndex->DBSize(dbACTUATOR);
   newSize = dbEntrySizes[dbACTUATOR];
   oldSize = newSize - 2;
   source = destination = (i8 *)db.GetCommonAddress(dbACTUATOR, 0);
   source += numEnt * oldSize;
   destination += numEnt * newSize;
   for(i = 0; i < numEnt; i++)
   {
      //Insert two zero bytes.
      *(--destination) = 0;
      *(--destination) = 0;
      //copy oldSize bytes;
      for(j = 0; j < oldSize; j++)
      {
         *(--destination) = *(--source);
      };
   };
   //Now try to fix the parameters
   for(i = 0; i < numEnt; i++)
   {
      pActuator = (DB3 *)db.GetCommonAddress(dbACTUATOR, i);
      pActuator->MakeBig();
   };
}

// *********************************************************
//
// *********************************************************
//  TAG01e552
i16 ReadDatabases()
{
   //;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   dReg D0, D1, D3, D4, D5, D6;
   aReg A0, A3;
   i32 numLevel;
   i32 memSizeError;
   i32 celloffset = 0;
   LEVELDESC *pA0;
   //  i16 *pwA3;
   DBCOMMON *DBA3;
   CELLFLAG *pdD0;
   CELLFLAG **ppdA2;
   //  i16 LOCAL_30;
   i8 LOCAL_20[8];
   i32 filesize; // LOCAL_12
   i16 CheckSum;
   i16 LOCAL_6;
   CELLFLAG *LOCAL_4;
   CheckSum = 0;
   d.inStreamBuffered = 0; // Read from file
   if(d.gameState != GAMESTATE_ResumeSavedGame)
   {
      ExtendedFeaturesSize = ReadExtendedFeatures(d.datafileHandle);
      D0L = READ(d.datafileHandle, 8, (ui8 *)LOCAL_20);
      if(D0L != 8)
         return 0;
      if((ui16)LE16(wordGear(LOCAL_20)) != 33028)
      {
         LSEEK(ExtendedFeaturesSize, d.datafileHandle, SEEK_SET); // Rewind
         D0L = D0L >= 0 ? 1 : 0;                                  // Set if succes
      }
      else
      {
         d.Word22592 = LE16(wordGear(LOCAL_20 + 6));
         d.inStreamBuffered = 1; // Read from memory buffer
         d.inStreamLeft = LE32(LoadLong(LOCAL_20 + 2));
         d.inStreamNext = (pnt)allocateMemory(d.inStreamLeft, 2);
         D0L = LSEEK(0, d.datafileHandle, SEEK_END); // To eof
         D0L -= 8;                                   // File size -8
         if(ExtendedFeaturesVersion != '@')
            D0L -= ExtendedFeaturesSize;
         filesize = D0L;
         A3 = (aReg)allocateMemory(filesize, 2);
         D0L = LSEEK(8 + (ExtendedFeaturesVersion == '@' ? 0 : ExtendedFeaturesSize),
                     d.datafileHandle,
                     SEEK_SET);                             // To byte 8/of file.
         D0L = READ(d.datafileHandle, filesize, (ui8 *)A3); // Read entire file
         if(D0L != filesize)
            return 0;
         ExpandData(A3, d.inStreamNext, LE32(LoadLong(LOCAL_20 + 2)));
         TAG021800(filesize); //Release file buffer memory
      };
   };
   if(IsRecordFileRecording())
   {
      if(
#ifndef _DEBUG // Record Design option is default for _DEBUG builds
          RecordDesignOption &&
#endif
              !NoRecordCommandOption && !NoRecordMenuOption ||
          RecordCommandOption && !NoRecordMenuOption || RecordMenuOption)
      {
         RecordFile_Open();
         RecordMenuOption = true;
      }
      else
         RecordFile_Close();
   };

   if(d.PartyHasDied == 0)
   {
      d.dungeonDatIndex = (DUNGEONDATINDEX *)allocateMemory(44, 1);
   };
   D0W = FetchDataBytes((ui8 *)d.dungeonDatIndex, &CheckSum, 44);
   d.dungeonDatIndex->Swap(); //swapDungeonDatIndex();
   if(D0W == 0)
      return 0;
   if(d.gameState != GAMESTATE_ResumeSavedGame)
   {
      //D6W = D0W = d.dungeonDatIndex->Word8();
      d.partyX = d.dungeonDatIndex->StartingPartyX();
      //D0W = sw(D6W >> 5);
      //D6W = D0W;
      d.partyY = d.dungeonDatIndex->StartingPartyY();
      d.partyFacing = d.dungeonDatIndex->StartingPartyFacing();
      d.partyLevel = 0;
   };
   numLevel = d.dungeonDatIndex->NumLevel();
   if(cellflagArraySize == 0)
   {
      cellflagArraySize = d.dungeonDatIndex->LegacyCellFlagArraySize();
   }
   else
   {
      d.dungeonDatIndex->LegacyCellFlagArraySize(0);
   };
   D0W = d.PartyHasDied;
   if(D0W == 0)
   {
      d.pLevelDescriptors =
          (LEVELDESC *)allocateMemory(16 * numLevel, 1);
   };
   D0W = FetchDataBytes((ui8 *)d.pLevelDescriptors, &CheckSum, 16 * numLevel);
   if(D0W == 0)
      return 0;
   // We promised to littleEndian words 0, 8, 10, 12, and 14 of
   // each of the 16-byte structures.
   swapLevelDescriptors(numLevel);
   D0W = d.PartyHasDied;
   if(D0W == 0)
   {
      d.objectLevelIndex = (ui16 *)allocateMemory(2 * numLevel, 1);
   };
   D4W = 0;
   for(D5W = D4W; D5W < numLevel; D5W++)
   {
      d.objectLevelIndex[D5W] = D4W;
      D0W = d.pLevelDescriptors[D5W].LastColumn();
      D4W = sw(D4W + D0W + 1);
   };
   d.numColumnPointers = D4W;
   D5W = d.dungeonDatIndex->ObjectListLength();
   if(d.gameState != GAMESTATE_ResumeSavedGame)
   {
      d.dungeonDatIndex->ObjectListLength(uw(d.dungeonDatIndex->ObjectListLength() + 300));
   };
   if(d.PartyHasDied == 0)
   {
      g_objectListIndexSize = (D4W << 1) & 0xffff;
      d.objectListIndex = (ui16 *)allocateMemory(g_objectListIndexSize, 1);
      d.objectList.resize(d.dungeonDatIndex->ObjectListLength());
      d.indirectTextIndex.resize(d.dungeonDatIndex->NumWordsInTextArray() & 0xffff);
      d.compressedText.clear();
   }
   else
   {
      memSizeError = 0;
      if(g_objectListIndexSize < ((D4W << 1) & 0xffff))
      {
         memSizeError |= 1;
      };
      if(d.objectList.size() < d.dungeonDatIndex->ObjectListLength())
      {
         memSizeError |= 2;
      };
      if(d.indirectTextIndex.size() < (d.dungeonDatIndex->NumWordsInTextArray() & 0xffffU))
      {
         memSizeError |= 4;
      };
      if(memSizeError != 0)
      {
         UI_MessageBox("Something is very wrong.  The size\n"
                       "of some basic database has gotten\n"
                       "larger since we saved that game.\n"
                       "You can run it by exiting and 'Resuming'\n"
                       "from that savegame.",
                       "Error",
                       MESSAGE_OK);
         die(0x445c);
      };
   };
   D0W = FetchDataBytes((ui8 *)d.objectListIndex, &CheckSum, (ui16)D4W * 2);
   if(D0W == 0)
      return 0;
   swapPointer10454();
   D0W = FetchDataBytes((ui8 *)d.objectList.data(), &CheckSum, D5W * 2);
   if(D0W == 0)
      return 0;
   swapPRN10464(D5W);
   if(d.gameState != GAMESTATE_ResumeSavedGame)
   {
      for(D4W = 0; D4W < 300; D4W++)
      {
         d.objectList[D5W++] = RN(RNnul);
      };
   };

   std::unique_ptr<ui16[]> pTextArray;
   if(!indirectText)
   {
      pTextArray = std::make_unique<ui16[]>(d.dungeonDatIndex->NumWordsInTextArray());
      D0W = FetchDataBytes((ui8 *)pTextArray.get(), &CheckSum, d.dungeonDatIndex->NumWordsInTextArray() * 2);
      if(D0W == 0)
         return 0;
   }
   else //if m_indirectText
   {    //nothing is swapped!
      //fetch indirecttext(NumWordsInTextArray);
      D0W = FetchDataBytes((ui8 *)d.indirectTextIndex.data(), &CheckSum, d.dungeonDatIndex->NumWordsInTextArray() * 4);
      SwapIndirectTextIndex();
      if(D0W == 0)
         return 0;
      //fetch sizeof compressedText;
      d.compressedText.clear();
      i32 sizeOfCompressedText = 0;
      D0W = FetchDataBytes((ui8 *)&sizeOfCompressedText, &CheckSum, 4);
      sizeOfCompressedText = BE32(sizeOfCompressedText);
      if(D0W == 0)
         return 0;
      //fetch compressedText;
      if(sizeOfCompressedText > 1000000)
      {
         die(0x4ccce, "Excessive compressed text");
      };
      d.compressedText.resize(sizeOfCompressedText);
      D0W = FetchDataBytes(
          (ui8 *)&d.compressedText[0],
          &CheckSum,
          sizeOfCompressedText * 2);
      if(D0W == 0)
         return 0;
   };
   if(d.gameState != GAMESTATE_ResumeSavedGame)
   {
      gameTimers.Allocate(100);
      gameTimers.InitializeTimers();
   };
   for(D6W = 0; D6W < 16; D6W++)
   {
      //D5W = D6W;

      D5W = d.dungeonDatIndex->DBSize(D6W); // number of entries
      if(d.gameState != GAMESTATE_ResumeSavedGame)
      {
         D0W = D5W;
         A0 = d.Byte7302 + D6W;
         D3W = *((ui8 *)A0);
         D0W = sw(D0W + D3W);
         //D0W = sw(Smaller(D6W==15 ? 768 : 1024, D0W));
         d.dungeonDatIndex->DBSize(D6W, D0W);
      };
      if(D6W == dbSCROLL)
      {
         if(readScrollDatabase(&CheckSum) == 0)
            return 0;
         continue;
      };
      D4L = dbEntrySizes[D6W];
      if((D6W == dbACTUATOR) & !bigActuators)
      {
         D4W -= 2;
      };
      // Always allocate!!  If it is already allocated then
      // it will get released and reallocated to the
      // correct size!  This was a bad bug that made
      // some saved games unloadable.
      //if (d.Word22584 == 0)
      {
         //D0L = d.dungeonDatIndex[D6W+6] * D4W;
         //D0L &= 0xffff;
         //if (D0L != 0) A0 = allocateMemory(D0L,1);
         //else A0 = NULL;
         //d.misc1052eight[D6W] = (UNKNOWN *)A0;
         db.Allocate(D6W, d.dungeonDatIndex->DBSize(D6W));
         /*
      if (d.dungeonDatIndex[D6W+6] != 0)
      {
        d.misc10528[D6W] = db.GetCommonAddress(RN(0,D6W,0));
      }
      else
      {
        d.misc10528[D6W] = NULL;
      };
      */
      };
      if(d.dungeonDatIndex->DBSize(D6W) != 0)
      {
         DBA3 = db.GetCommonAddress(DBTYPE(D6W), 0);
         D0W = FetchDataBytes((ui8 *)DBA3, &CheckSum, (ui16)D4W * (ui16)D5W);
         if(D0W == 0)
            return 0;
         if((D6W == dbACTUATOR) && !bigActuators)
         {
            MakeBigActuators();
         }
         db.swap(D6W); // database
      };
      if(d.gameState != GAMESTATE_ResumeSavedGame)
      {
         if((D6W == dbMONSTER) || (D6W >= dbMISSILE))
         {
            D0W = d.dungeonDatIndex->DBSize(D6W);
            gameTimers.Allocate(sw(gameTimers.MaxTimer() + D0W));
         };
         /*
      All the enclosed code is to clear the extra
      entries that we allocated but did not initialize
      by reading from the file.
      But the new db.Allocate clears all the entries.
      Therefore, this code is not needed.  And that is
      nice because it is quite ugly.
      D1W = D4W >> 1;
      D4W = D1W;
      LOCAL_30 = D1W;
      D1L = D5UW * (ui16)LOCAL_30;
      //D1 <<= 1;
      pwA3 += D1L;
      D5W = (UI8)(d.Byte7302[D6W]); //# additional entries
      while (D5W != 0)
      {
        pwA3[0] = -1;
  //      D0W = D5W;
        D5W--;
        D1W = D4W;
        D1H1 = 0;
        //D1 <<= 1;
        pwA3 += D1L;

      };
      */
      };

   }; //for
   if(!indirectText)
   {
      if(ConvertToIndirectText(pTextArray.get()) == 0)
         return 0;
   };
   if(d.PartyHasDied == 0)
   {
      d.cellFlagArray = (CELLFLAG *)allocateMemory(cellflagArraySize, 1);
   };
   D0W = FetchDataBytes((ui8 *)d.cellFlagArray, &CheckSum, cellflagArraySize);
   if(D0W == 0)
      return 0;
   D0W = StreamInput((ui8 *)&LOCAL_6, 2); // Read without updating checksum
   LOCAL_6 = LE16(LOCAL_6);
   if((D0W != 0) && (LOCAL_6 != CheckSum))
      return 0;
   if(d.PartyHasDied == 0)
   {
      D0L = 4 * (d.numColumnPointers + numLevel);
      // D7W is number of index pointers at the
      //  front of d.10450.
      d.pppdPointer10450 = (CELLFLAG ***)allocateMemory(D0L, 1);
      ppdA2 = (CELLFLAG **)&d.pppdPointer10450[numLevel];
      // Need cast because the 10450 array is used for
      // two things.  See comments in data definitions.
      for(i32 level = 0; level < numLevel; level++)
      {
         d.pppdPointer10450[level] = ppdA2;
         //pntGear(A0) = A2;
         pdD0 = d.cellFlagArray;
         //pA0 = &d.pLevelDescriptors[level];
         //D3L = pA0->cellOffset;
         //D3H1 = 0;
         if(level == 0)
         {
            celloffset = d.pLevelDescriptors[0].cellOffset;
         }
         else
         {
            celloffset += (d.pLevelDescriptors[level].cellOffset - d.pLevelDescriptors[level - 1].cellOffset) & 0xffff;
         };
         pdD0 = pdD0 + celloffset; // Add Byte offset
         //pdD0 = (CELLFLAG *)((i32)pdD0 + D3L); // Add Byte offset
         LOCAL_4 = pdD0;
         *ppdA2 = pdD0;
         ppdA2++;
         for(i32 column = 1;
             column <= BITS6_10(d.pLevelDescriptors[level].word8);
             column++)
         {
            pA0 = &d.pLevelDescriptors[level];
            D1W = (I16)((pA0->word8 >> 11) & 0x1f);
            D1W++;
            pdD0 = LOCAL_4;
            D1L &= 0xffff;
            pdD0 = pdD0 + D1L; //add byte offset
            //pdD0 = (DUDAD16 *)((i32)pdD0+D1L); //add byte offset
            LOCAL_4 = pdD0;
            *ppdA2 = pdD0;
            ppdA2++;
         }; // for column
      };    /// for level
   };
   if(d.inStreamBuffered != 0)
   {
      d.inStreamBuffered = 0;
      TAG021800(LE32(LoadLong(LOCAL_20 + 2)));
   };

   if(ExtendedFeaturesVersion == '@')
   {
      // ****************************************************
      //
      // We need to expand the database to the 'linear'
      // model.
      // We need to search the dungeon and convert all
      // objects of class RN to the new model.
      // Should be fun.
      i32 level, x, y;
      CELLFLAG cf, *columnPointerCF;
      RN *columnPointerRN;
      for(level = 0; level < d.dungeonDatIndex->NumLevel(); level++)
      {
         for(x = 0; x <= d.pLevelDescriptors[level].LastColumn(); x++)
         {
            columnPointerCF = d.pppdPointer10450[level][x];
            columnPointerRN = d.objectList.data() + d.objectListIndex[d.objectLevelIndex[level] + x];
            for(y = 0; y <= d.pLevelDescriptors[level].LastRow(); y++)
            {
               cf = d.pppdPointer10450[level][x][y];
               if((cf & 0x10) == 0)
                  continue;
               ConvertListOfObjects(columnPointerRN, false, level, x + d.pLevelDescriptors[level].offsetX, y + d.pLevelDescriptors[level].offsetY);
               //And sub-lists, too. Chest/Monster/scroll/missile
               columnPointerRN++;
            };
         };
      };
      if(gameTimers.pTimer(0) != NULL)
         ConvertTimers();
      ConvertCharacters();
   };
   if(RememberToPutObjectInHand != -1)
   {
      die(0x40e);
   };
   if((ExtendedFeaturesVersion == '@') && (d.objectInHand != RNnul))
   {
      //CURSORFILTER_PACKET cfp;
      //cfp.type = CURSORFILTER_Unknown;
      //cfp.object = d.objectInHand.ConvertToInteger();
      //CursorFilter(&cfp);

      ConvertListOfObjects(&d.objectInHand, false, 0, -5, 0);
   };
   if(ExtendedFeaturesVersion == '@')
   {
      ExtendedFeaturesVersion = 'A';
   };
   RememberToPutObjectInHand = -1;
   if(ExtendedFeaturesVersion < 'B')
   {
      convertScrolls();
      ExtendedFeaturesVersion = 'B';
   };
   expool.Setup();
   return 1;
}
