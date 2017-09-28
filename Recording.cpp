#include "stdafx.h"

#include <stdio.h>



#include "Dispatch.h"
#include "UI.h"

#include "CSB.h"
#include "Recording.h"
#include "Data.h"

void SmartDiscardTrace(FILE *f);
void DSAInstrumentation_Dump();
void CloseTraceFile();
void UnlinkFile(const char *name);



extern char *PlayfileName;
//extern TCHAR szCSBVersion[MAX_LOADSTRING];
extern bool version90Compatible;
extern bool TimerTraceActive;
extern i16 TraceFile;
extern bool RepeatGame;
extern i32 VBLMultiplier;
extern bool neophyteSkills;
extern bool RecordCommandOption;
extern bool NoRecordCommandOption;
bool OpenTraceFile();
extern ui32 numRandomCalls;
extern bool playback_71;


bool firstVersionWarning = true;
bool firstGraphicsWarning = true;
bool firstCSBgraphicsWarning = true;
bool checkDungeonSignature;
bool firstDungeonWarning = true;

RECORDFILE RecordFile;


struct TABLE
{
  TABLE(i32 size);

  void Clear();
  void Add(i32 a, i32 b);
  void Add(i32 a, i32 b, i32 c);
  void Get(i32 index, i32 *a, i32 *b);
  void Get(i32 index, i32 *a, i32 *b, i32 *c);

private:

   std::vector<i32> m_data;
   i32 m_entsize;
};

TABLE::TABLE(i32 size)
{
  m_entsize = size;
  m_data.reserve(m_entsize*3);
}

void TABLE::Clear()
{
  m_data.clear();
}

void TABLE::Add(i32 a, i32 b, i32 c)
{
  if (m_entsize!=3)
  {
    UI_MessageBox("Decompressing playfile","Internal Error", MESSAGE_OK);
    die(0x776a);
  };
  m_data.push_back(a);
  m_data.push_back(b);
  m_data.push_back(c);
}

void TABLE::Add(i32 a, i32 b)
{
  if (m_entsize!=2)
  {
    UI_MessageBox("Decompressing playfile","Internal Error", MESSAGE_OK);
    die(0x776a);
  };
  m_data.push_back(a);
  m_data.push_back(b);
}

void TABLE::Get(i32 index, i32 *a, i32 *b)
{
  ASSERT(m_entsize == 2, "suze");
  *a = m_data[index*m_entsize+0];
  *b = m_data[index*m_entsize+1];
}

void TABLE::Get(i32 index, i32 *a, i32 *b, i32 *c)
{
  ASSERT(m_entsize == 3,"size");
  *a = m_data[index*m_entsize+0];
  *b = m_data[index*m_entsize+1];
  *c = m_data[index*m_entsize+2];
}



TABLE tableXYF(3);
TABLE tableTXYF(2);
TABLE tableTXYFR(2);




void PLAYFILE::Open()
{
  char versionLine[200];
  m_file = OPEN(PlayfileName, "r");
  if (m_file < 0)
  {
    char msg[200];
    sprintf(msg,"Cannot open Playfile \"%s\"",PlayfileName);
    UI_MessageBox(msg,"",MESSAGE_OK);
    return;
  };
  GETS(versionLine,199,m_file);
  {
    int n;
    for (n = strlen(versionLine)-1; n>=0; n--)
    {
      if (versionLine[n] > 32) break;
      versionLine[n] = 0;
    };
  };
  if (strcmp(versionLine+1, szCSBVersion) != 0)
  {
    char msg[200];
    if (    (strcmp(szCSBVersion,"CSB for Windows Version 9.03")==0)
         && (strcmp(versionLine+1,"CSB for Windows Version 9.0") == 0)
       )
    {
      version90Compatible = true;
    }
    else
    {
      sprintf(msg,"Playback file is not the correct version");
      if (TimerTraceActive)
      {
        fprintf(GETFILE(TraceFile),"Wrong playback version\n");
        fprintf(GETFILE(TraceFile),"Expected version %s\n", szCSBVersion);
        fprintf(GETFILE(TraceFile),"File version %s\n", versionLine+1);
      };
      UI_MessageBox(msg,"",MESSAGE_OK);
    };
  };
  STHideCursor(HC35);
  m_eofEncountered = m_forceClose = false;
  tableXYF.Clear();
  tableTXYF.Clear();
  tableTXYFR.Clear();
  OnMouseSwitchAction(0); // Say that all mouse buttons are up.
};

void PLAYFILE::Close()
{
  if (m_file >= 0)
  {
    CLOSE(m_file);
    STShowCursor(HC35);
  };
  m_file = -1;
  m_time = -1;
}

bool PLAYFILE::IsOpen()
{
 if (m_file < 0) return false;
 return true;
}

void PLAYFILE::ReadEOF()
{
  //Force file close with next Play().
  m_forceClose = true;
}


//These are placed outside of PLAYFILE::Play because the debugger
// cannot see static variables within a function.
static char line[80];
static i32 f1,f2,f3,f4,f5;
bool PLAYFILE::Play(MouseQueueEnt *ent)
{
  ui32 deltatime;
  i32 numfield;
  if (m_file < 0) return false;
  while ((m_time == -1) || m_forceClose)
  { // Must read another record from file.
    i32 len;
    if (m_forceClose || GETS(line,80,m_file)==NULL)
    {
      m_eofEncountered = true;
      if (!m_forceClose && (d.Time <= m_oldTime)) return false;
      MouseQueueEnt tent;
      tent.num = 0x5555;
      tent.x = 0;
      tent.y = 0;
      d.RandomNumber ^= 0xaa00;
      RecordFile.Record(&tent);
      Close();
      if (!RepeatGame)
      {
        UI_MessageBox("End Of PlaybackFile","Info",MESSAGE_OK);
      };
      if (!RepeatGame)
      {
        NoSpeedLimit = 0;
        VBLMultiplier = 1;
      };
      return false;
    };
    len = strlen(line);
    while ( (len>0) && ((line[len-1] == '\n')||(line[len-1] == '\r')))
    {
      len--;
      line[len] = 0;
    };
    if (line[0]== '#')
    {
      int i;
      for (i = 1; i < len; i++)
      {
        if ((line[i] == '/') || (line[i] == ' '))
        {
          line[i] = 0;
          break;
        };
      };
      // This is to make Version 11.002 and later compatible with Version 11.1 playfile.
      if (strcmp(line,"#NeophyteSkills")==0)
      {
        neophyteSkills = true;
        RecordFile.Record("#NeophyteSkills\n");
        continue;
      };
      if (strcmp(line,"#DMRules")==0)
      {
        DM_rules = true;
        RecordFile.Record("#DMRules\n");
        continue;
      };
      if (strcmp(line,"#NoDMRules")==0)
      {
        DM_rules = false;
        RecordFile.Record("#NoDMRules\n");
        continue;
      };
      if (strcmp(line,"#ExtraTicks")==0)
      {
        extraTicks = true;
        continue;
      };
      if (strcmp(line,"#NoExtraTicks")==0)
      {
        extraTicks = false;
        continue;
      };
      if (strcmp(line,"#SpeedLimit") == 0)
      {
        NoSpeedLimit = 0;
        VBLMultiplier = 1;
        continue;
      };
      if (strcmp(line,"#NoSpeedLimit") == 0)
      {
        NoSpeedLimit = 0x20000000;
        continue;
      };
      if (strcmp(line,"#AutoEnlarge") == 0)
      {
        AutoEnlarge = true;
        continue;
      };
      if (strcmp(line,"#NoSound") == 0)
      {
        NoSound = true;
        continue;
      };
      if (strcmp(line,"#Sound") == 0)
      {
        NoSound = false;
        continue;
      };
      if (strcmp(line,"#NoSleep") == 0)
      {
        NoSleep = true;
        continue;
      };
      if (strcmp(line,"#Sleep") == 0)
      {
        NoSleep = false;
        continue;
      };
      if (strcmp(line,"#NoRecord") == 0)
      {
        NoRecordCommandOption = true;
        RecordCommandOption = false;
        continue;
      };
      if (memcmp(line,"#FastVBL",8) == 0)
      {
        i32 speed;
        VBLMultiplier = 4;
        if (sscanf(line+8,"%d",&speed) == 1)
        {
          if ( (speed>0) && (speed<100))
          {
            VBLMultiplier = speed;
          };
        };
        continue;
      };
      if (strcmp(line,"#TimerTraceOn") == 0)
      {
        if (TimerTraceActive) continue; // Ignore it
        if (AttackTraceActive || AITraceActive || OpenTraceFile())
        {
          TimerTraceActive = true;
          SmartDiscardTrace(GETFILE(TraceFile));
        };
        continue;
      };
      if (strcmp(line,"#DSATraceOn") == 0)
      {
#ifdef _MSVC_INTEL
        DSAIndex.TraceOverride(true);
#else
        DSATraceActive = true;
#endif
        continue;
      };
      if (strcmp(line,"#DSATraceOff") == 0)
      {
        DSAInstrumentation_Dump();
#ifdef _MSVC_INTEL
        DSAIndex.TraceOverride(false);
#else
        DSATraceActive = false;
#endif
        continue;
      };
      if (strcmp(line,"#TimerTraceOff") == 0)
      {
        SmartDiscardTrace(NULL);
        if (!TimerTraceActive) continue; //Ignore it
        TimerTraceActive = false;
        if (!AttackTraceActive && !AITraceActive)
        {
          CloseTraceFile();
        };
        continue;
      };
      if (strcmp(line,"#AITraceOn") == 0)
      {
        if (AITraceActive) continue; // Ignore it
        if (AttackTraceActive || TimerTraceActive || OpenTraceFile())
        {
          AITraceActive = true;
          SmartDiscardTrace(GETFILE(TraceFile));
        };
        continue;
      };
      if (strcmp(line,"#AITraceOff") == 0)
      {
        SmartDiscardTrace(NULL);
        if (!AITraceActive) continue; //Ignore it
        AITraceActive = false;
        if (!AttackTraceActive && !TimerTraceActive)
        {
          CloseTraceFile();
        };
        continue;
      };
      continue; //ignore it
    };
    if (line[0] == '+') // Increments rather than absolute values
    {
      numfield = sscanf(line+1,"%x %x %x %x %x\n",
                           &deltatime, &m_x, &m_y, &m_num, &m_ran);
      if (numfield != 5)
      {
        UI_MessageBox("Playback Plus line did not have 5 fields","Error",MESSAGE_OK);
        die(0x6656);
      };
      m_time = m_oldTime + deltatime;
      tableXYF.Add(m_x, m_y, m_num);
    }
    else
    {
      numfield = sscanf(line,"%08x %04x %04x %04x %08x\n",
                        &f1, &f2, &f3, &f4, &f5);
      switch (numfield)
      {
      case 1:  // tableTXYFRindex
        tableTXYFR.Get(f1, &f5, (i32 *)&m_ran);
        tableTXYF.Get(f5, (i32 *)&deltatime, &f5);
        tableXYF.Get(f5, &m_x, &m_y, &m_num);
        m_time = m_oldTime + deltatime;
        break;
      case 2:  // tableTXYFindex, random
        tableTXYF.Get(f1, (i32 *)&deltatime, &f5);
        tableXYF.Get(f5, &m_x, &m_y, &m_num);
        m_ran = f2;
        tableTXYFR.Add(f1, m_ran);
        m_time = m_oldTime + deltatime;
        break;
      case 3:  // deltaTime, tableXYFindex, random
        deltatime = f1;
        m_ran = f3;
        tableXYF.Get(f2, &m_x, &m_y, &m_num);
        tableTXYF.Add(deltatime, f2);
        m_time = m_oldTime + deltatime;
        break;
      case 5:
        m_time = f1;
        m_x = f2;
        m_y = f3;
        m_num = f4;
        m_ran = f5;
        break;
      default:
        UI_MessageBox("Improper playfile format","Error",MESSAGE_OK);
        die(0x3342);
      };
    };
  };
  ASSERT(  (m_time >= d.Time)
         ||(m_time == 0)
         ||(m_num == 0xffff)
         ||(m_num == 0x5555)
         ||(m_num == 0x6668) ,"playback");
  if (m_time > d.Time) return false;
  if (    (m_time!=0)
       && (m_ran!=d.RandomNumber)
       && (m_ran!=numRandomCalls-m_oldCallCount)
       && (m_num != 0x2222)  //Cycle random number generator
       && (m_num != 0xd7)    //Party died???
       && (m_num != 0x3333)  //Set Random number to xy
       && (m_num != 0x5555)  //Splice
       && (m_num != 0x6668)  //SaveFile signature (a no-op)
       && (m_num != 0xffff)  //Mouse moves from character order area
      )
  {
    i32 answer;

    answer = UI_MessageBox("Sequence/timing error\nShould we quit?",
                           "Playback error",
                           MESSAGE_YESNO);
    if (answer == MESSAGE_IDYES) die(0x7dbc);
    d.RandomNumber = m_ran;
  };
  {
    static i32 annotationFile = -1;
    static bool attempted = false;
    static i32 annotationTime = -1;
    if (!attempted)
    {
      attempted = true;
      annotationFile = OPEN("annotation.txt", "r");
    };
    while (annotationFile >= 0)
    {
      if (annotationTime < 0)
      {
        // Read the annotation file to find an entry for this time (or greater)
        char annotationLine[200];
        if (fgets(annotationLine, 199, GETFILE(annotationFile)) == NULL)
        {
          CLOSE(annotationFile);
          annotationFile = -1;
        }
        else
        {
          i32 len;
          len = strlen(annotationLine);
          if ( (len>0) && (annotationLine[0] == '#'))
          {
            if (sscanf(annotationLine+1,"%d",&annotationTime) != 1)
            {
              CLOSE(annotationFile);
              annotationFile = -1;
            };
          };
        };
      }
      else
      {
        break;
      };
    };
    if (annotationTime > 0)
    {
      if (annotationTime <= d.Time)
      {
        EditDialog annotationDialog;
        char annotationLine[200];
        char *annotation;
        i32 annotationLen = 0;
        annotationTime = -1;
        annotation = NULL;
        annotationLine[0] = 0;
        while (fgets(annotationLine, 198, GETFILE(annotationFile)) != NULL)
        {
          i32 lineLen;
          lineLen = strlen(annotationLine);
          annotationLine[lineLen-1] = 0x0d;
          annotationLine[lineLen] = 0x0a;
          annotationLine[lineLen+1] = 0;
          lineLen++;
          if (annotationLine[0] != '#')
          {
            annotation = (char *)UI_realloc(annotation, annotationLen+lineLen+1,MALLOC114);
            strcpy(annotation+annotationLen, annotationLine);
            annotationLen += lineLen;
          }
          else
          {
            if (sscanf(annotationLine+1,"%d",&annotationTime) != 1)
            {
              CLOSE(annotationFile);
              annotationFile = -1;
            };
            break;
          };
        };

        annotationDialog.m_initialText = annotation;
        annotationDialog.DoModal();
        UI_free(annotation);
      };
    };
  };
  if (m_num == 71)
  {
    playback_71 = true;
  };
  if (m_num == 0xd7) // Game over???
  {
    m_time = 0x7fffffff;
    return false;
  };
  ent->x = sw(m_x);
  ent->y = sw(m_y);
  ent->num = sw(m_num);
  m_oldTime = m_time;
  m_oldCallCount = numRandomCalls;
  m_time = -1;
  if (m_num == 0x3333)
  {
    d.RandomNumber = (m_x << 16) | (m_y & 0xffff);
    RecordFile.Record(ent);
    return false;
  };
  if (m_num == 0x2222)
  {
    i32 count;
    count = m_ran;
    while (count > 0)
    {
      STRandom();
      count--;
    };
    m_num = 0;
  };
  if (m_num == 0x2221)
  {
    RecordFile.CycleRandom(ent->y);
    return  Play(ent);
  };
  if (m_num == 0x6665)
  {
    if ( (((unsigned)m_x << 16) | m_y) != versionSignature)
    {
      if (firstVersionWarning)
      {
        ui32 hash;
        char msg[80];
        hash = versionSignature;
        sprintf(msg, "Recorded with Different\nCSBwin Version\n%08x", hash);
        UI_MessageBox(msg,"error",MESSAGE_OK);
        firstVersionWarning = false;
      };
    };
    RecordFile.Signature(versionSignature,0x6665);
    return Play(ent);
  };
  if (m_num == 0xdddd)
  {
    // Now here is a hack, first class.
    // We had Playback problems.  This little adjustment allows us to set
    // variables in the game to particular values needed to make the
    // playback work!  Oh, boy.
    *(ui16 *)(((ui8 *)&d)+m_x) = (ui16)m_y;
    return Play(ent);
  };
  if (m_num == 0x6666)
  {
    if ( (((unsigned)m_x << 16) | m_y) != graphicSignature1)
    {
      if (firstGraphicsWarning)
      {
        UI_MessageBox("Recorded with\nDifferent\nGraphics File","error",MESSAGE_OK);
        firstGraphicsWarning = false;
      };
    };
    RecordFile.Signature(graphicSignature1,0x6666);
    return Play(ent);
  };
  if (m_num == 0x666a)
  {
    if ( (((unsigned)m_x << 16) | m_y) != CSBgraphicSignature1)
    {
      if (firstCSBgraphicsWarning)
      {
        UI_MessageBox("Recorded with\nDifferent\nCSBgraphics File","error",MESSAGE_OK);
        firstCSBgraphicsWarning = false;
      };
    };
    RecordFile.Signature(CSBgraphicSignature1,0x666a);
    return Play(ent);
  };
  if (m_num == 0x6667)
  {
    if (checkDungeonSignature)
    {
      if ( (((unsigned)m_x << 16) | m_y) != dungeonSignature1)
      {
        if (firstDungeonWarning)
        {
          UI_MessageBox("Recorded with\nDifferent\ndungeon File","error",MESSAGE_OK);
          firstDungeonWarning = false;
        };
      };
    };
    RecordFile.Signature(dungeonSignature1,0x6667);
    return Play(ent);
  };
  if (  (m_num == 0x6668) || (m_num == 0x6669) )
  {
    RecordFile.Record(ent);
    return Play(ent);
  };
  if (m_num == 0x5555)
  {
    d.RandomNumber ^= 0xaa00;
    RecordFile.Record(ent);
    return  Play(ent);
  };
  if ((m_num == 0x2223) || (m_num == 0x2224) )
  { // What is this for?  Be patient and I will explain.
    // The recording files often contain game-save operations.
    // Saving a game modifies the game's state.  Unfortunately, that is
    // what the original Atari program did.
    // I want to remove these game-save operations from the recording
    // to make them 'cleaner'.  So I had to provide a substitute
    // operation that would change the game's state in the same way that
    // a save operation would change it.  The 2223 operation is designed
    // to do just that.  See example below.
    //
    // The 2224 function is used to splice two Recordnn.log files
    // together.  Since the save/restore cleared a few things
    // the 2224 function clears the same things.
    RecordFile.Record(ent);
    TAG00bd40(); //Remove the Active Monster (ITEM16) entries
    ProcessMonstersOnLevel();
    SmartDiscard(true); // Initialize Smart Discard Processing.

    if (m_num == 0x2224)
    {
      //d.Long16600 = -505; //No idea!
      totalMoveCount += 4; // See sample below where 4 commands are commented out before the save.
      checkDungeonSignature = false;
      if (d.AttackingCharacterOrdinal != 0)
      {
        d.AttackingCharacterOrdinal = 0;
        d.Word20252 = 1;
        DrawLegalAttackTypes();
      };
    };




  // Here is an example of removing a save point from a game
  // 000006e2 00a1 0079 0053 dd109237    //Select inventroy
  // 000006e3 0000 0000 0add 7a071db6    //Unclick
  // #000006e6 00b1 0029 008c c001ec2b   //Select disk icon
  // #000006e6 0000 0000 0add c001ec2b   //unclick
  // #000006e6 0069 006c 00d2 c001ec2b   //SAve and play
  // #000006e6 004b 0069 00d2 c001ec2b   //CSBGAME.dat
  // #000006e6 f2e5 643b 6668 c001ec2b   //Signature
  // 000006e6 0000 0000 2223 c001ec2b    //Do what the save would have done
  // 000006e7 0000 0000 0add fb6f9844    //unclick
  // 000006eb 004a 0069 000b 3329834b    //exit inventory






 // Here is an example of two record files pasted together.
 // 000330fe  0136 0037 0002 f2b7762c          file 1
 // 00033127  0065 005c 0053 4bca7f8a          file 1 select inventory
 // ##00033131  00ae 0027 008c 5ac82e95        file 1 select disk icon
 // ##00033131  008c 0070 00d3 5ac82e95        file 1 select save & quit
 // ##00033131  0091 006b 00d3 5ac82e95        file 1 select game2
 // #
 // #
 // #
 // #CSB for Windows Version 7.3
 // #                  0093 --> Freeze Game
 // #                  0094 --> un-Freeze Game
 // #                  00c9 --> Resume (at prison door)
 // #                  008c --> Select Disk option from inventory
 // #                  00d2 --> First Menu option
 // #                  00d3 --> Second Menu Option
 // #                  00d4 --> Third Menu Option
 // #                  0001 --> Turn Left
 // #                  0002 --> Turn Right
 // #                  0003 --> Move Forward
 // #                  0004 --> Slide Right
 // #                  0005 --> Move Backward
 // #                  0006 --> Slide Left
 // #                  0053 --> Select Inventory Screen
 // #                  000b --> Leave Inventory Screen
 // ##00000000  00fd 0056 00c9 00000cd9      file 2 resume
 // ##00000000  00e9 0071 00d3 00000cd9      file 2 select game 2
 // ##00033131  00c9 006e 00d2 7454166b      file 2 play
 // 00033131  00c9 006e 2224 5ac82e95        I inserted this to do what
 //                                           the save/reload does
 // 00000000  0000 0000 000b 00000000        I inserted this to get out
 //                                           of inventory mode
 // 00033149  00c9 006e 0003 44e9eb83        file 2 move
 // 00033155  00c9 006e 0005 802596e0        file 2 move
 // 0003315e  004f 0021 0053 4626f1c2        etc.
    return Play(ent);

  };
  return true;
}

void PLAYFILE::Backspace(MouseQueueEnt *
#ifdef _DEBUG
                         ent
#endif
                         )
{
  if (m_file < 0) return;
  ASSERT (m_time == -1, "playback");
  ASSERT (m_x == ent->x, "playback");
  ASSERT (m_y == ent->y, "playback");
  ASSERT (m_num == ent->num, "playback");
  m_time = d.Time;
}


PLAYFILE PlayFile;
bool PlayfileIsOpen() {return PlayFile.IsOpen();};
void PlayfileOpen(bool open)
{
  if (open)
  {
    PlayFile.Open();
    checkDungeonSignature = true;
  }
  else PlayFile.Close();
}





void RECORDFILE::PreOpen()
{
  // We write the first lines into memory.  Then later
  // the file may be opened for real and these lines will
  // be put on disk.
  m_isQueueingLines = true;
  //Record(DM_rules ? "#DMRules\n" : "#NoDMRules\n");
};

void RECORDFILE::Open()
{
  // We write the first lines into memory.  Then later
  // the file may be opened for real and these lines will
  // be put on disk.
  int tryAgain;
  if (m_fileNum >= 0) return; //Already open
#ifdef _LINUX
  char folder[10] = "logs/";
#else
  char folder[10] = "logs\\";
#endif
  for (tryAgain=0; tryAgain<2; tryAgain++)
  {
    for (i32 i=0; i<10000; i++)
    {
      char filename[100];
      sprintf(filename,"%sRecord%05d.log",folder,i);
      m_fileNum = OPEN(filename,"r");
      if (m_fileNum < 0)
      {
        char msg[300];
        m_fileNum = CREATE(filename,"w", false);
        if (m_fileNum < 0)
        {
          if (folder[0] != 0)
          {
            folder[0] = 0;
            i = -1;
            continue;
          };
          sprintf(msg,"Cannot open file %s for Recording",filename);
          UI_MessageBox(msg,
                        NULL,MESSAGE_OK);
        };
        fprintf(GETFILE(m_fileNum),"#%s\n",szCSBVersion);
        fprintf(GETFILE(m_fileNum),"#                  0001 --> Turn Left\n");
        fprintf(GETFILE(m_fileNum),"#                  0002 --> Turn Right\n");
        fprintf(GETFILE(m_fileNum),"#                  0003 --> Move Forward\n");
        fprintf(GETFILE(m_fileNum),"#                  0004 --> Slide Right\n");
        fprintf(GETFILE(m_fileNum),"#                  0005 --> Move Backward\n");
        fprintf(GETFILE(m_fileNum),"#                  0006 --> Slide Left\n");
        fprintf(GETFILE(m_fileNum),"#                  000b --> Leave Inventory Screen\n");
        fprintf(GETFILE(m_fileNum),"#                  0046 --> Click mouth to feed\n");
        fprintf(GETFILE(m_fileNum),"#                  0047 --> Click eye to show skills\n");
        fprintf(GETFILE(m_fileNum),"#                  0050 --> Click in Viewport\n"),
        fprintf(GETFILE(m_fileNum),"#                  0053 --> Select Inventory Screen\n");
        fprintf(GETFILE(m_fileNum),"#                  006f --> Select Attack Character/option\n");
        fprintf(GETFILE(m_fileNum),"#                  008c --> Select Disk option from inventory\n");
        fprintf(GETFILE(m_fileNum),"#                  0093 --> Freeze Game\n");
        fprintf(GETFILE(m_fileNum),"#                  0094 --> un-Freeze Game\n");
        fprintf(GETFILE(m_fileNum),"#                  00c9 --> Resume (at prison door)\n");
        fprintf(GETFILE(m_fileNum),"#                  00d2 --> First Menu option\n");
        fprintf(GETFILE(m_fileNum),"#                  00d3 --> Second Menu Option\n");
        fprintf(GETFILE(m_fileNum),"#                  00d4 --> Third Menu Option\n");
        fprintf(GETFILE(m_fileNum),"#                  0add --> Mouse button up\n");
        fprintf(GETFILE(m_fileNum),"#                  6665 --> Version Signiture\n");
        fprintf(GETFILE(m_fileNum),"#                  6666 --> Graphics Signiture\n");
        fprintf(GETFILE(m_fileNum),"#                  6667 --> Dungeon Signiture\n");
        fprintf(GETFILE(m_fileNum),"#                  6668 --> SaveGame Signiture\n");
        fprintf(GETFILE(m_fileNum),"#                  6669 --> LoadGame Signiture\n");
        fprintf(GETFILE(m_fileNum),"#                  666a --> CSBGraphic Signiture\n");
        fprintf(GETFILE(m_fileNum),"#                  ffff --> Drag out of chartacter order area\n");
        fprintf(GETFILE(m_fileNum),"## Parameters you can change\n");
        fprintf(GETFILE(m_fileNum),"##   #[No]DMRules\n");
        fprintf(GETFILE(m_fileNum),"##   #[No]ExtraTicks\n");
        fprintf(GETFILE(m_fileNum),"##   #[No]SpeedLimit //Quick Play\n");
        fprintf(GETFILE(m_fileNum),"##   #[No]Sleep //Skip over any delays for flashing buttons, etc\n");
        fprintf(GETFILE(m_fileNum),"##   #FastVBLnn // Set VBL multiplier (go fast); If not equal one then\n");
        fprintf(GETFILE(m_fileNum),"##              // Only display every third frame.  99 is a special case\n");
        fprintf(GETFILE(m_fileNum),"##              // that goes fast AND displays every frame\n");
        fprintf(GETFILE(m_fileNum),"##   #TimerTrace<On/Off>\n");
        fprintf(GETFILE(m_fileNum),"##   #AITrace<On/Off>\n");
        fprintf(GETFILE(m_fileNum),"##   #DSATrace<On/Off>\n");
        //Now dump any pre-open lines.
#ifdef MSVC_QUEUE
        for (std::list<std::string>::const_iterator j=m_lineQueue.begin(); j!=m_lineQueue.end(); ++j)
        {
          fputs(j->c_str(), GETFILE(m_fileNum));
        };
        m_lineQueue.clear();
#else
        {
          LQENTRY *pos;
          pos = m_lineQueue.GetHeadPosition(); 
          while (pos != NULL)
          {
            fputs(m_lineQueue.GetNext(pos), GETFILE(m_fileNum));
          };
        };
        m_lineQueue.clear();
#endif
        m_isQueueingLines = false;
        return;
      }
      else
      {
        CLOSE(m_fileNum);
      };
    };
    if (tryAgain == 0)
    {
      //Ask if we should delete files.
      tryAgain = false;
      int answer;
      answer = UI_MessageBox("Too many logs\nShould we delete the existing logs\nand start a new set of logs",
                             "Log file problem",
                             MESSAGE_YESNO);
      if (answer == MESSAGE_IDYES)
      {
        for (i32 i=0; i<100; i++)
        {
          char filename[100];
          sprintf(filename,"%sRecord%03d.log",folder,i);
#ifndef _LINUX
          UnlinkFile(filename);
#endif
        };
        continue;
      };
    };
    UI_MessageBox("Too many Record files",
              "",MESSAGE_OK);
  };
};

void RECORDFILE::Close()
{
  if (m_fileNum >= 0) CLOSE(m_fileNum);
  m_fileNum = -1;
  m_lineQueue.clear();
  m_isQueueingLines = false;
}

void RECORDFILE::CycleRandom(i32 n)
{
  if (m_fileNum >= 0)
  {
    fprintf(GETFILE(m_fileNum), "%08x 0000 %04x 2221 %08x\n",
                   d.Time, n, d.RandomNumber);
  };
  for (i32 i=0; i<=n; i++) STRandom();
}

void RECORDFILE::Signature(ui32 sig, ui32 type)
{
  if (m_fileNum >= 0)
  {
    fprintf(GETFILE(m_fileNum), "%08x %04x %04x %04x %08x\n",
                   d.Time,
                   (sig >> 16) & 0xffff,
                   sig & 0xffff,
                   type,
                   d.RandomNumber);
  };
  STRandom();
}

void RECORDFILE::Record(i32 x, i32 y, i32 f)
{
  MouseQueueEnt ent;
  ent.x = sw(x);
  ent.y = sw(y);
  ent.num = sw(f);
  Record(&ent);
}


void RECORDFILE::Record(const char *l)
{
  if (m_fileNum >= 0)
  {
    fprintf(GETFILE(m_fileNum), "%s", l);
    fflush(GETFILE(m_fileNum));
  }
  else
  {
    if (!m_isQueueingLines) return;
    m_lineQueue.push_back(l);
  };
}

void RECORDFILE::Record(MouseQueueEnt *ent)
{
  if (   (ent->num == 0x64)
       &&(ent->x == 0x114)
       &&(ent->y == 0x2f) ) return; //No operation
  if (ent->num < 0x2000)
  {
    totalMoveCount++;
  };
  if ((m_fileNum < 0)&& !m_isQueueingLines) return;
  if (m_fileNum >= 0)
  {
    if (!PlayFile.IsOpen() && d.ClockRunning)
    {
      if ((d.RandomNumber & 0x3651) == 0)
      {
        CycleRandom(gameSpeed);
      };
      if (   ((d.RandomNumber & 0x351a8) == 0)
          || (!m_graphicSignature) && (graphicSignature1!=0))
      {
        Signature(graphicSignature1, 0x6666);
        m_graphicSignature = true;
      };
      if (   ((d.RandomNumber & 0x351a4) == 0)
          || (!m_CSBgraphicSignature) && (CSBgraphicSignature1!=0))
      {
        Signature(CSBgraphicSignature1, 0x666a);
        m_CSBgraphicSignature = true;
      };
      if (   ((d.RandomNumber & 0x8a153) == 0)
          || (!m_dungeonSignature) && (dungeonSignature1!=0))
      {
        Signature(dungeonSignature1, 0x6667);
        m_dungeonSignature = true;
      };
      if (   ((d.RandomNumber & 0x8a163) == 0)
          || (!m_versionSignature) && (versionSignature!=0))
      {
        Signature(versionSignature, 0x6665);
        m_versionSignature = true;
      };
    };
    fprintf(GETFILE(m_fileNum), "%08x %04x %04x %04x %08x\n",
                     d.Time, ent->x, ent->y, ent->num, d.RandomNumber);
    fflush(GETFILE(m_fileNum));
  }
  else
  {
    char buf[40];
    sprintf(buf,
            "%08x %04x %04x %04x %08x\n",
            d.Time, ent->x, ent->y, ent->num, d.RandomNumber);
    m_lineQueue.push_back(buf);
  };
}

void RECORDFILE::Comment(const char *comment)
{
  if (m_fileNum < 0) return;
  fprintf(GETFILE(m_fileNum), "#%s\n", comment);
}


bool RecordfileIsOpen() {return RecordFile.IsOpen();};
void RecordfileOpen(bool open)
{
  if (open) RecordFile.Open();
  else RecordFile.Close();
}

void RecordfilePreOpen()
{
  RecordFile.PreOpen();
}


bool IsPlayFileOpen()                   {return PlayFile.IsOpen();}
bool PlayFile_Play(MouseQueueEnt *MQ)       {return PlayFile.Play(MQ);}
void PlayFile_Close()                   {PlayFile.Close();}
void PlayFile_Backspace(MouseQueueEnt *MQ)  {PlayFile.Backspace(MQ);}
void PlayFile_ReadEOF()                 {PlayFile.ReadEOF();}
bool PlayFile_IsEOF()                   {return PlayFile.IsEOF();}

void RecordFile_Open()                  {RecordFile.Open();};
void RecordFile_Close()                 {RecordFile.Close();}
bool IsRecordFileRecording()            {return RecordFile.IsRecording();}
void RecordFile_Record(MouseQueueEnt *MQ)   {RecordFile.Record(MQ);}
void RecordFile_Record(const char *l)    {RecordFile.Record(l);}
void RecordFile_Record(int x, int y, int z) {RecordFile.Record(x, y, z);}