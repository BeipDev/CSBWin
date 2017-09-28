#include "stdafx.h"

#include "UI.h"

//#include <malloc.h>

#include <stdio.h>

#ifdef _MSVC_INTEL
#pragma warning(disable:4201) // non-standard construct
#include <mmsystem.h>
#pragma warning(default:4201)
#endif

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

#if defined _MSVC_CE2002ARM
#define MAXWAVE 1
#else

#if defined _LINUX
#define MAXWAVE 1
#else
#define MAXWAVE 3
#endif

#endif

//extern i8 data001f2a[];
//extern i8 data001f2c[];
//extern i8 data001f2e[];
//extern i8 data001f32[];
//extern i8 data001f76[];
//extern i8 data002036[];


extern i32 NoSpeedLimit;
extern i32 VBLMultiplier;

struct SNDHEAD
{
  i8  byte0[4];           //"RIFF"
  i32 Size;               //  6076  // #bytes after this integer
  i8  byte8[8];           //"WAVEfmt "
  i32 int16;              //    18
  i16 wFormatTag;         //     1
  i16 nChannels;          //     1
  i32 nSamplesPerSecond;  // 11025
  i32 nAvgBytesPerSec;    // 11025 // important
  i16 nBlockAlign;        //     1 // 2 for 16-bit
  i16 wBitsPerSample;     //     8
  //i16 cbSize;             //    40
  //i8  byte38[4];          // "fact"
  //i32 int42;              //     4
  //i32 numBytes46;         //
  i8  byte50[4];          // "data"
  i32 numSamples54;       //
  i8  sample58[1];
};



ui8 ChannelA, ChannelB, ChannelC;

i16 SampleCount  = 0;  //001f2a = 0; // sample count??
i16 SampleRepeat = 0;  //001f2c // repeat count for this sample??
pnt SamplePointer=NULL;//001f2e // nibble address??
                       // 2 * <byte address> + <nibble>
pnt pnt001f32=NULL;  // Points to 2036 or 1f76
i8 data001f36[] ALIGN4 ={0x00, //1f36 // Wavetable??
                  0x08, //1f3a // 16 values
                  0x0a, //1f3e // 1f36 followed by 1f76
                  0x0b, //1f42 //  followed by 1fb6.
                  0x0c, //1f46 // register 8 - value
                  0x0d, //1f4a // register 9 - value
                  0x0d, //1f4e // register a - value
                  0x0d, //1f52
                  0x0e, //1f56
                  0x0e, //1f5a
                  0x0e, //1f5e
                  0x0e, //1f62
                  0x0f, //1f66
                  0x0e, //1f6a
                  0x0e, //1f6e
                  0x0e};//1f72
i8 data001f76[] ALIGN4 ={0x00,
                  0x05,
                  0x07,
                  0x09,
                  0x09,
                  0x05,
                  0x0a,
                  0x0c,
                  0x08,
                  0x0a,
                  0x0c,
                  0x0d,
                  0x0b,
                  0x0e,
                  0x0e,
                  0x0e};
i8 data001fb6[] ALIGN4 ={0x00,
                  0x00,
                  0x00,
                  0x00,
                  0x00,
                  0x00,
                  0x06,
                  0x00,
                  0x00,
                  0x0a,
                  0x0a,
                  0x0a,
                  0x00,
                  0x0b,
                  0x0d,
                  0x0e};
i8 data001ff6[] ALIGN4 ={0x00,
                  0x05,
                  0x07,
                  0x08,
                  0x09,
                  0x09,
                  0x0a,
                  0x0a,
                  0x0b,
                  0x0b,
                  0x0b,
                  0x0b,
                  0x0c,
                  0x0c,
                  0x0c,
                  0x0c};
i8 data002036[] ALIGN4 ={0x00,
                  0x00,
                  0x01,
                  0x02,
                  0x02,
                  0x05,
                  0x02,
                  0x06,
                  0x03,
                  0x06,
                  0x07,
                  0x08,
                  0x04,
                  0x06,
                  0x08,
                  0x09};
i8 data002076[] ALIGN4 ={0x00,
                  0x00,
                  0x00,
                  0x00,
                  0x00,
                  0x02,
                  0x02,
                  0x01,
                  0x01,
                  0x02,
                  0x04,
                  0x05,
                  0x01,
                  0x02,
                  0x00,
                  0x00};

static ui8 volTable[256];
static i32 curVol=-1;
// *********************************************************
// Turn an atari graphic sound into a .wav file.
// *********************************************************
char *SoundDecode(pnt pGraphic, i32 numSample, i32 volume)
{ // volume is a divisor
  char *pN;
  FILE *f=NULL;
  unsigned char *pW;
  SNDHEAD *WavBuf;
  i32 i, n, repeat, sample, nibble;
  bool even;
#ifdef _SNDDEBUG
  static bool snddebug = true;
  if(snddebug) f=fopen("snddbg.txt","w");
  snddebug = false;
#endif
  if (!usingDirectX)
  {
    if (curVol != volume)
    {
      // Rebuild volume table
      i32 k;
      for (k=0; k<128; k++) 
        volTable[k] = (ui8)((k-128-volume/2)/volume + 128);
      for (k=128; k<256; k++)
        volTable[k] = (ui8)((k-128+volume/2)/volume + 128);
      curVol = volume;
    };
  };
  WavBuf = (SNDHEAD *)UI_malloc(numSample + 58, MALLOC042);
  if (WavBuf == NULL) die (0,"No Memory");
  pW = (unsigned char *)WavBuf->sample58;
  n = numSample; // Number of samples to store
  pN = (char *)pGraphic;
  if (f) fprintf(f,"Starting pN = %08x   Number of samples=%08x\n"
                 ,(ui32)pN, n);
  even=true;
  while (n > 0)
  {
    if (even)
    {
      sample = *pN & 0xf0;
      if (f) fprintf(f,"Even sample %d\n",sample);
      even = false;
    }
    else
    {
      sample = (*(pN++) << 4) & 0xf0;
      if (f) fprintf(f,"Odd sample %d\n",sample);
      even = true;
    };
    if (sample != 0)
    {
      if (f)
      {
        fprintf(f,"Non-zero sample  n=%d\n",n);
        fprintf(f,"***DEBUG store sample #%d\n",pW-(ui8*)WavBuf-58);
      };
      if (!usingDirectX)
      {
        *(pW++) = volTable[sample];
      }
      else
      {
        *(pW++) = (unsigned char)sample;
      };
      n--;
    }
    else
    {
      repeat = 0;
      for (;;)
      {
        if (even)
        {
          nibble = (*pN)>>4;
          if(f) fprintf(f,"Even nibble = %d\n",nibble);
          even=false;
        }
        else
        {
          nibble = *(pN++);
          if(f) fprintf(f,"Odd nibble = %d\n",nibble);
          even = true;
        };
        repeat = (repeat << 3) | (nibble & 7);
        if (f) fprintf(f,"Repeat = %d\n",repeat);
        if ((nibble & 8) == 0) break;
      };
      repeat += 3;
      if (repeat > n) repeat = n;
      n -= repeat;
      sample = *(pW-1);
      if(f) fprintf(f,"Sample=%d, repeat=%d,n=%d\n",sample,repeat,n);
      for (i=0; i<repeat; i++)
      {
        if (f)
        {
          fprintf(f,"***DEBUG store sample #%d\n",pW-(ui8 *)WavBuf-58);
        };
        *(pW++) = (ui8)sample;
      };
    };
  };
  if(f)
  {
    for (char *kk= (char *)pGraphic; kk<pN+1; kk+=8)
    {
      fprintf(f,"%08x ",kk-(char *)pGraphic);
      for (i32 kkk=0; kkk<8; kkk++)
      {
        fprintf(f,"%02x ", (*(kk+kkk))&0xff);
      };
      fprintf(f,"\n");
    };
    fprintf(f,"Ending pN = %08x\n", (ui32)pN);
    fclose(f);
    f=NULL;
  };
  ASSERT(n>=0,"n");
  memcpy(WavBuf->byte0,"RIFF",4);
  WavBuf->Size = numSample+sizeof(SNDHEAD)-8-1;
  memcpy(WavBuf->byte8,"WAVEfmt ",8);
  WavBuf->int16 = 16; // ????
  WavBuf->wFormatTag = 1;
  WavBuf->nChannels = 1;
  WavBuf->nSamplesPerSecond = 6000;
  WavBuf->nAvgBytesPerSec = 6000;
  WavBuf->nBlockAlign = 1;
  WavBuf->wBitsPerSample = 8;
  //WavBuf->cbSize = 40;
  //memcpy(WavBuf->byte38,"fact",4);
  //WavBuf->int42 = 4;
  //WavBuf->numBytes46 = numSample;
  memcpy(WavBuf->byte50,"data",4);
  WavBuf->numSamples54 = numSample;
  return (char *)WavBuf;
}

//===============================================
// Allocate memory and construct an internal .wav file from the
// custom sound.
//===============================================
char *SOUNDDATA::Decode(i32 volume)
{ // volume = divisor
  SNDHEAD *WavBuf;
  i32 i;
  if (!usingDirectX)
  {
    if (curVol != volume)
    {
      // Rebuild volume table
      for (i=0; i<128; i++) 
        volTable[i] = (ui8)((i-128-volume/2)/volume + 128);
      for (i=128; i<256; i++)
        volTable[i] = (ui8)((i-128+volume/2)/volume + 128);
      curVol = volume;
    };
  };
  WavBuf = (SNDHEAD *)UI_malloc(m_size + 58, MALLOC089);
  memcpy(WavBuf->byte0,"RIFF",4);
  WavBuf->Size = m_size+58-4;
  memcpy(WavBuf->byte8,"WAVEfmt ",8);
  WavBuf->int16 = 18; // ????
  WavBuf->wFormatTag = 1;
  WavBuf->nChannels = 1;
  WavBuf->nSamplesPerSecond = 11025;
  WavBuf->nAvgBytesPerSec = 11025;
  WavBuf->nBlockAlign = 1;
  WavBuf->wBitsPerSample = 8;
  //WavBuf->cbSize = 40;
  //memcpy(WavBuf->byte38,"fact",4);
  //WavBuf->int42 = 4;
  //WavBuf->numBytes46 = m_size;
  memcpy(WavBuf->byte50,"data",4);
  WavBuf->numSamples54 = m_size;
  if (!usingDirectX)
  {
    for (i=0; i<m_size; i++)
    {
      WavBuf->sample58[i] = volTable[m_sound[i]];
    }
  }
  else
  {
    memcpy(WavBuf->sample58, m_sound, m_size);
  };
  return (char *)WavBuf;
}  

// Made this a class so the destructor will
// clean things up when we stop the program.
class SOUNDER
{
  char *m_wave[MAXWAVE];
  char *m_toFree;
  i32   m_attenuation[MAXWAVE];
public:
  SOUNDER() {m_toFree=NULL;i32 i;for(i=0;i<MAXWAVE;i++)m_wave[i]=NULL;};
  ~SOUNDER() {Cleanup();};
  void Cleanup()
  {
    i32 i;
    if (m_toFree!=NULL) UI_free(m_toFree);
    m_toFree = NULL;
    if (m_wave[0] != NULL)
    {
      UI_StopSound();
      UI_free (m_wave[0]);
      m_wave[0] = NULL;
    };
    for (i=1; i<MAXWAVE; i++)
    {
      if (m_wave[i] != NULL)
      {
        UI_free(m_wave[i]);
        m_wave[i] = NULL;
      };
    }
  };
  void Sound(char *wave, i32 attenuation);
  i32 CheckQueue();
  void AddWave(char *wave, i32 attenuation);
#ifdef _MSVC_CE2002ARM
  char *Resample(char *wave);
#endif
};

#ifdef _MSVC_CE2002ARM

// At least one version of the PocketPC seems to crash the OS
// when we play sounds at 6000 samples/second.  Playing at
// 11025 appears to cause no problem.  So here we are, attempting
// to resample PCM sounds.  I'm going to try a simple linear
// interpolation and see if it is tolerable.

char *SOUNDER::Resample(char *wave)
{
  SNDHEAD *pSndHead;
  i32 numFullGroup, n;
  i32 i, nIn, nOut, newSize;
  ui8 *in, *out;
  SNDHEAD *pResult;
  pSndHead = (SNDHEAD *)wave;
  if (pSndHead->nSamplesPerSecond != 6000) return wave;
  nIn = pSndHead->numSamples54;
  nOut = (11 * nIn - 5)/6;
  newSize = sizeof (*pSndHead) + nOut - 1; //One byte of data in soundhead
  pResult = (SNDHEAD *)UI_malloc(newSize, MALLOC109);
  memcpy(pResult, wave, sizeof(*pSndHead));
  pResult->nAvgBytesPerSec = 11025;
  pResult->nSamplesPerSecond = 11025;
  pResult->numSamples54 = nOut;
  numFullGroup = (nIn-1)/6;
  in = (ui8 *)pSndHead->sample58;
  out = (ui8 *)pResult->sample58;
  for (i=0; i<numFullGroup; i++)
  {
    out[0]  = in[0];
    out[1]  = (5 *in[0] + 6*in[1] + 5)/11;
    out[2]  = (10*in[1] + 1*in[2] + 5)/11;
    out[3]  = (4* in[1] + 7*in[2] + 5)/11;
    out[4]  = (9* in[2] + 2*in[3] + 5)/11;
    out[5]  = (3* in[2] + 8*in[3] + 5)/11;
    out[6]  = (8* in[3] + 3*in[4] + 5)/11;
    out[7]  = (2* in[3] + 9*in[4] + 5)/11;
    out[8]  = (7* in[4] + 4*in[5] + 5)/11;
    out[9]  = (1* in[4] +10*in[5] + 5)/11;
    out[10] = (6* in[5] + 5*in[6] + 5)/11;
    out += 11;
    in += 6;
  };
  n = nOut - 11*numFullGroup;
  switch(n)
  {
  case 10:
    out[9]  = (1* in[4] +10*in[5] + 5)/11;
  case 9:
    out[8]  = (7* in[4] + 4*in[5] + 5)/11;
  case 8:
    out[7]  = (2* in[3] + 9*in[4] + 5)/11;
  case 7:
    out[6]  = (8* in[3] + 3*in[4] + 5)/11;
  case 6:
    out[5]  = (3* in[2] + 8*in[3] + 5)/11;
  case 5:
    out[4]  = (9* in[2] + 2*in[3] + 5)/11;
  case 4:
    out[3]  = (4* in[1] + 7*in[2] + 5)/11;
  case 3:
    out[2]  = (10*in[1] + 1*in[2] + 5)/11;
  case 2:
    out[1]  = (5 *in[0] + 6*in[1] + 5)/11;
  case 1:
    out[0]  = in[0];
  };
  UI_free(wave);
  return (char *)pResult;
}

#endif

i32 SOUNDER::CheckQueue()
{
  //If any sounds are waiting try to see if we can
  //play them now.  Return number waiting in queue.
#if MAXWAVE > 1
  i32 i;
  if (m_wave[1] != NULL)
  {
    if (UI_PlaySound(m_wave[1],SOUND_ASYNC|SOUND_MEMORY, m_attenuation[1]))
    {
      //I was getting an access violation in WINMM.DLL.
      //It seems that starting a new sound successfully does not
      //guarantee that WINMM.DLL is finished fiddling with the previous
      //sound!  So, I delay freeing the wave buffer by saving
      //it up in m_toFree.  That delay seems to be sufficient.
      //I have been unable to reproduce the access violation.
      if (this->m_toFree != NULL)
      {
        UI_free(m_toFree);
      };
      m_toFree = m_wave[0];
      for (i=0; i<MAXWAVE-1; i++)
      {
        m_wave[i] = m_wave[i+1];
        m_attenuation[i] = m_attenuation[i+1];
      };
#ifdef _MSVC_CE2002ARM
      if (m_wave[1] != NULL)
      {
        char *temp;
        temp = m_wave[1];
        m_wave[1] = Resample(temp);
        //UI_free(temp);
      };
#endif
      m_wave[i] = NULL;
    };
    for (i=1; i<MAXWAVE; i++)
    {
      if (m_wave[i] == NULL) break;
    };
    return i-1;
  }
  else
  {
    return 0;
  };
#else
  return 0;
#endif
}

#if MAXWAVE>1
void SOUNDER::AddWave(char *wave, i32 attenuation)
{
  i32 i;
  if (m_wave[0] == NULL)
  {
#ifdef _MSVC_CE2002ARM
    m_wave[0] = Resample(wave);
    //UI_free(wave);
#else
    m_wave[0] = wave;
#endif
    m_attenuation[0] = attenuation;
    UI_PlaySound(m_wave[0], SOUND_ASYNC|SOUND_MEMORY,m_attenuation[0]);
    return;
  };
  for (i=0; i<MAXWAVE; i++)
  {
    if (m_wave[i] == NULL)
    {
#ifdef _MSVC_CE2002ARM
      if (i == 1)
      {
        m_wave[i] = Resample(wave);
        //UI_free(wave);
      }
      else
      {
        m_wave[i] = wave;
      };
#else
      m_wave[i] = wave;
#endif
      m_attenuation[i] = attenuation;
      return;
    };
  };
  UI_free(wave);
}
#else //if MAXWAVE==1
// Linux uses SDL and therefore MAXWAVE must be 1 when using Linux.
#ifdef _LINUX
void SOUNDER::AddWave(char *wave, i32 attenuation)
{
  UI_PlaySound(wave, SOUND_ASYNC|SOUND_MEMORY,attenuation);
}
#else // if not _LINUX
void SOUNDER::AddWave(char *wave, i32 attenuation)
{
  bool success;
  if (m_wave[0] == NULL)
  {
#ifdef _MSVC_CE2002ARM
    m_wave[0] = Resample(wave);
#else
    m_wave[0] = wave;
#endif
    //UI_free(wave);
    m_attenuation[0] = attenuation;
    UI_PlaySound(m_wave[0], SOUND_ASYNC|SOUND_MEMORY,m_attenuation[0]);
    return;
  };
  if (m_toFree != NULL) UI_free(m_toFree);
  m_toFree = m_wave[0];
#ifdef _MSVC_CE2002ARM
  m_wave[0] = Resample(wave);
#else
  m_wave[0] = wave;
#endif
  //UI_free(wave);
  success = UI_PlaySound(m_wave[0], SOUND_ASYNC|SOUND_MEMORY,attenuation);
  if (success)
  {
    //UI_free(m_wave[0]);
    //m_wave[0] = wave;
    //m_attenuation[0] = attenuation;
  }
  else
  { 
    UI_StopSound();
    //UI_free(m_wave[0]);
    //m_wave[0] = NULL;
    success = UI_PlaySound(m_wave[0], SOUND_ASYNC|SOUND_MEMORY,attenuation);
    //if (success)
    //{
    //  m_wave[0] = wave;
    //  m_attenuation[0] = attenuation;
    //}
  };
}
#endif // _LINUX
#endif //MAXWAVE
void SOUNDER::Sound(char *wave, i32 attenuation)
{
  // We free wave.  Therefore you better not use
  // it again after you call us to play it!
  ASSERT(m_wave[0] != wave,"wave");
  ASSERT(wave != NULL,"wave");
  if (VBLMultiplier != 1) 
  {
    UI_free(wave);
    return;
  };

//First we try to remove any queued item
  CheckQueue();

// Now we want to add the new wave to the queue
  AddWave(wave, attenuation); //And play it if is the only one.

// And once again, see if we can remove any queued item.
  CheckQueue();
}


SOUNDER sounder;

// *********************************************************
//
// *********************************************************
//   TAG001d16
void StartSound(ui8 *SoundBytes,
                i32  /*timerData*/,
                i32  highVolume)
{
  i32 size, volume;
  char *Wave;
  if (SoundBytes == NULL)
  {
    sounder.Cleanup();
    return;
  };
  if (gameVolume == VOLUME_OFF) return;
  if (NoSound) return;
  //printf("StartSound @%d\n",(ui32)UI_GetSystemTime());
  size = LE16(wordGear(SoundBytes));
  volume = 1;
  if (highVolume==0) volume = 18;// attenuation
  volume *= volumeTable[gameVolume].divisor;
  Wave = SoundDecode((pnt)SoundBytes+2, size, volume);
  sounder.Sound(Wave, volumeTable[gameVolume].attenuation);
}


i32 SoundFilter(i32 soundNumber, i32 highVolume, const LOCATIONREL *soundLocr)
{
  static bool filterActive = false;
  // return value == 0 if sound was passed to a SoundFilter DSA
  // return vlaue == 1 if no Sound Filter exists.
  // return value == 2 if Sound Filter is already active.
  if (filterActive) return 2;
  {
    ui32 key, *pRecord;
    i32 len;
// ****************************************************************************
//  See if there is a Sound Filter to modify the description.
    key = (EDT_SpecialLocations<<24)|ESL_SOUNDFILTER;
    len = expool.Locate(key,&pRecord);
    if (len > 0)
    //Let the DSA process the parameters
    {
      RN obj;
      TIMER timer;
      i32 objectDB, actuatorType;
      DB3 * pActuator;
      LOCATIONREL locr;
      NEWDSAPARAMETERS dsaParameters;
      i32 dx, dy, distanceSquared, distance100;
      locr.Integer(*pRecord);
      dx = abs(d.partyX - soundLocr->x);
      dy = abs(d.partyY - soundLocr->y);
      distanceSquared = dx*dx + dy*dy;
      distance100 = 100 * ((dx>dy) ? dx : dy);
      if (distanceSquared > 0)
      {
        int d2 = distanceSquared * 10000;
        distance100 = (distance100 + d2/distance100)/2;
        distance100 = (distance100 + d2/distance100)/2;
        distance100 = (distance100 + d2/distance100)/2;
      };
      for (obj = FindFirstObject(
                      locr.l,
                      locr.x,
                      locr.y);
          obj != RNeof;
          obj = GetDBRecordLink(obj))
      {
        objectDB = obj.dbType();
        if (objectDB == dbACTUATOR)
        {
          pActuator = GetRecordAddressDB3(obj);
          actuatorType = pActuator->actuatorType();
          if (actuatorType == 47)
          {
            int currentLevel;
            DSAVARS dsaVars;
            currentLevel = d.LoadedLevel;
            LoadLevel(locr.l);
            timer.timerUByte9(0);//timerFunction
            timer.timerUByte8(0);//timerPosition
            timer.timerUByte7((ui8)locr.y);
            timer.timerUByte6((ui8)locr.x);
            timer.Time(locr.l << 24);
    
            pDSAparameters[1+0] = soundNumber+1;
            pDSAparameters[1+1] = highVolume;
            pDSAparameters[1+2] = distanceSquared;
            pDSAparameters[1+3] = soundLocr->l;
            pDSAparameters[1+4] = soundLocr->x;
            pDSAparameters[1+5] = soundLocr->y;
            pDSAparameters[1+6] = gameVolume;;
            pDSAparameters[1+7] = NoSound;
            pDSAparameters[1+8] = distance100;  // in units of 0.01
            pDSAparameters[0] = 9;
            filterActive = true;
            // Let the DSA issue the sound if it so pleases.
            ProcessDSAFilter(obj, &timer, locr, NULL, &dsaVars);
            LoadLevel(currentLevel);
            continue;
          };
        };
      };
    };    
    if (filterActive) 
    {
      filterActive = false;
      return 0;
    };
    return 1;
  // ****************************************************************************
  };
}


i32 CheckSoundQueue()
{
  return sounder.CheckQueue();
}


// *********************************************************
//
// *********************************************************
void TAG001e16(i16 P1)
{
  dReg D6, D7;
  aReg A0, A1;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //i32 saveD6=D6,saveD7=D7;
  D7W = P1;
  D6L = (i32)dosound(NULL);
  D7L = (D7L & 0xf) * 4;
  A1 = (pnt)0x00ff8800;
  A0 = (pnt)data001f76;
  dosound((ui8 *)D6L);
  //D6=saveD6;D7=saveD7;
}
/*
// *********************************************************
// This appears to be the interrupt routine to handle
// sound timer interrupts.  I think it operates at
// about 8 KHz.
// *********************************************************
void TAG001e50()
{
  dReg D6, D7;
  pnt pD7;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  //A0 = &data001f2a; // Sound Sample count??
  if (SampleCount != 0)
  {
    SampleCount--;
    //A0 = &data001f2c; // Sample repeat count??
//    A0 = &data001f2e; // Next nibble address
    if (SampleRepeat != 0) // Sample repeat count??
    {
      //A0 = &data001f2c;
      SampleRepeat--;
      return;
    };
    pD7 = SamplePointer;
    SamplePointer++;
    //A0 = &data001f32;
    if (((i32)pD7 & 1) == 0)
    {
      pD7 = (pnt)((i32)pD7 >> 1);
      D7B = (i8)((*pD7 >> 4) & 0x0f); // 4 * Upper nibble
      if (D7B == 0) goto tag001ede;
      //A1 = 0xffff8800;
      //A0 = data001f32;
      //longGear(A1) = longGear(A0+D7W-64); //Channel A
      //longGear(A1) = longGear(A0+D7W+0);  //Channel B
      //longGear(A1) = longGear(A0+D7W+64); //Channel C
      //return;
    }
    else
    {
      D7B = (i8)((*pD7)&15);
      if (D7B ==0) goto tag001ede;
      //D7B <<= 4; // 4 * Lower Nibble
      //A1 = 0xffff8800;
      //A0 = data001f32;
      //longGear(A1) = longGear(A0+D7W-64);
      //longGear(A1) = longGear(A0+D7W+0);
      //longGear(A1) = longGear(A0+D7W+64);
      //return;
    };
    //A1 = 0xffff8800;
    //A0 = data001f32;
    //longGear(A1) = longGear(A0+D7W-64);
    //longGear(A1) = longGear(A0+D7W+0);
    //longGear(A1) = longGear(A0+D7W+64);
    ChannelA = pnt001f32[D7W-16];
    ChannelB = pnt001f32[D7W+00];
    ChannelC = pnt001f32[D7W+16];
    return;
  };
  return;
tag001ede: // Come here when sample value is zero. (D7B)
  D6W = 0;
  for (;;)
  {
    //A0 = data001f2e;
    pD7 = SamplePointer;
    SamplePointer++;
    if (((i32)pD7 & 1) == 0)
    {
      pD7 = (pnt)((i32)pD7 >> 1); // The pointer part of the longword.
//
      D7B = (i8)(*pD7 >> 4); // upper nibble
      //D7B >>= 4;
    }
    else
    {
      pD7 = (pnt)((i32)pD7 >> 1);
      D7B = *pD7; // lower nibble
    };
    if ((D7B & 8) == 0) break;
    D6W = (i16)((D6W << 3) + (D7W & 7));
  };
  D6W = (i16)((D6W << 3) + (D7W & 7) + 2);
  //A0 = data001f2c;
  SampleRepeat = D6W;
  //AllowInterrupts
}
*/
/*
void playSound(i32 soundNum)
{
  ui32 size;
  static SNDHEAD *soundData;
  FILE *sndFile;
  sndFile = UI_fopen("socko.wav", "rb");
  if (sndFile==NULL) return;
  fseek(sndFile,0,SEEK_END);
  size = ftell(sndFile);
  fseek(sndFile,0,SEEK_SET);
  soundData = (SNDHEAD *)UI_malloc(size);
  if (size != fread(soundData,1,size,sndFile)) return;
  //soundData->SamplesPerSecond24 = 6000;
  //soundData->SamplesPerSecond28 = 6000;
  soundData->numSamples54 *= 2;
  soundData->nAvgBytesPerSec *= 2;
  soundData->numBytes46 *= 2;
  soundData->nBlockAlign = 2;
  soundData->wBitsPerSample *= 2;
  //sndPlaySound((char *)soundData,SND_SYNC|SND_MEMORY);
  if (soundNum==0) return;
  //if (soundNum==112)PlaySound("ugh.wav",NULL,SND_ASYNC);
  //if (soundNum==546)PlaySound("socko.wav",NULL,SND_ASYNC);
}

*/



/*
Playing WAVE Resources
You can use the PlaySound function to play a sound that is stored as a
resource. Although this is also possible using the sndPlaySound function,
sndPlaySound requires that you find, load, lock, unlock, and free the
resource; PlaySound achieves all of this with a single line of code.

PlaySound Example
PlaySound("SoundName", hInst, SND_RESOURCE | SND_ASYNC);

sndPlaySound Example
The SND_MEMORY flag indicates that the lpszSoundName parameter is a
pointer to an in-memory image of the WAVE file. To include a WAVE file as
a resource in an application, add the following entry to the application's
resource script (.RC) file.

soundName WAVE c:\sounds\bells.wav

The name soundName is a placeholder for a name that you supply to refer
to this WAVE resource. WAVE resources are loaded and accessed just like
other application-defined Windows resources. The PlayResource function
in the following example plays a specified WAVE resource.

BOOL PlayResource(LPSTR lpName)
{
    BOOL bRtn;
    LPSTR lpRes;
    HANDLE hResInfo, hRes;

    // Find the WAVE resource.

    hResInfo = FindResource(hInst, lpName, "WAVE");
    if (hResInfo == NULL)
        return FALSE;

    // Load the WAVE resource.

    hRes = LoadResource(hInst, hResInfo);
    if (hRes == NULL)
        return FALSE;

    // Lock the WAVE resource and play it.

    lpRes = LockResource(hRes);
    if (lpRes != NULL) {
        bRtn = sndPlaySound(lpRes, SND_MEMORY | SND_SYNC |
            SND_NODEFAULT);
        UnlockResource(hRes);
    }
    else
        bRtn = 0;

    // Free the WAVE resource and return success or failure.

    FreeResource(hRes);
    return bRtn;
}

To play a WAVE resource by using this function, pass to the function a
pointer to a string containing the name of the resource, as shown in
the following example.

PlayResource("soundName");

 */



SOUNDDATA::SOUNDDATA()
{
  m_sound = NULL;
  m_soundNum = -1;
  m_size = -1;
};

SOUNDDATA::~SOUNDDATA()
{
  Cleanup();
}

void SOUNDDATA::Allocate(i32 numSample)
{
  if (m_sound != NULL) UI_free(m_sound);
  m_sound = (ui8 *)UI_malloc(numSample, MALLOC090);
}


bool SOUNDDATA::ReadSound(i32 soundNum)
{
  static bool firstTime = true;
  ui32 numSample;
  unsigned char *p, prevByte;
  TEMPORARY_MEMORY fileData;
  ui32 i, fileSize;
  fileData.m = ReadCSBgraphic(
       CGT_Sound,
       soundNum, 
       44,
       &fileSize,
       true,
       MALLOC122);
  if (fileData.m == NULL)
  {
    if (!firstTime) return false;
    firstTime = false;
    UI_MessageBox("Cannot find Sound","error",MESSAGE_OK);
    return false;
  };
  for (p=fileData.m,i=0, prevByte=0; i<fileSize; i++)
  {
    *p = (unsigned char)(*p + prevByte);
    prevByte = *(p++);
  };
  numSample = *(i32 *)(fileData.m + 40);
  if (numSample + 44 > fileSize)
  {
    UI_MessageBox("Sound too short","error",MESSAGE_OK);
    return false;
  };
  Allocate(numSample);
  m_size = numSample;
  memcpy(m_sound, fileData.m + 44, numSample);
  return true;
}



void SOUNDDATA::Cleanup()
{
  if (m_sound != NULL)
  {
    UI_free(m_sound);
    m_sound = NULL;
  };
}


void PlayCustomSound(i32 soundNum, i32 volume, i32 /*flags*/)
{
  char *wav;
  SOUND *pSound;
  if (soundNum < 0)
  {
    soundNum = -soundNum-1;
    if (soundNum > 21) return;
    pSound = &d.sound1772[soundNum];
    StartSound(GetBasicGraphicAddress(pSound->word0|0x8000),// Start Sound
               (UI8)(pSound->byte3),
               volume);
  }
  else
  {
    if (!currentSound.ReadSound(soundNum)) return;
    if (gameVolume == VOLUME_OFF) return;
    if (NoSound) return;
    wav = currentSound.Decode(volume?volume:1);
    sounder.Sound(wav, volume + volumeTable[gameVolume].attenuation);
  };
}
