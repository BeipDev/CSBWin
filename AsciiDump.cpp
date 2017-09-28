 
#include "stdafx.h"

#include "UI.h"

#include <stdio.h>

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

void printAbsLocation(FILE *f, i32 level, i32 x, i32 y);


extern i32 dbEntrySizes[16];
extern unsigned char *encipheredDataFile;
extern bool simpleEncipher;

struct MISCENTRY
{
  unsigned char level; //101-104 in character's possession. 
                       //0xff if unknown.
                       //0xfe if unused and available
  unsigned char x;
  unsigned char y;
  unsigned char container; //0 if lying loose
                           //1 if in chest
                           //2 if in monster's possession
                           //3 if in character's possession
};

class MISCTABLE
{
  MISCENTRY *miscEntry;
public:
  MISCTABLE() {miscEntry = NULL;};
  ~MISCTABLE() {DeAllocate();};
  void DeAllocate()
  {
    if (miscEntry!=NULL) UI_free(miscEntry); 
    miscEntry=NULL;
  };
  void Allocate();
  void Print(FILE *f);
  void MarkLocation(RN obj, i32 level, i32 x, i32 y, i32 container);
};

void MISCTABLE::Allocate()
{
  i32 i;
  DB10 *pDB10;
  ASSERT(miscEntry == NULL,"Double MISC allocate");
  miscEntry = (MISCENTRY *)
           UI_malloc(db.NumEntry(dbMISC) * dbEntrySizes[dbMISC],
           MALLOC001);
  for (i=0; i<db.NumEntry(dbMISC); i++)
  {
    miscEntry[i].level = 0xff; // Unknown
    pDB10 = GetRecordAddressDB10(i);
    if (pDB10->link() == RNnul) miscEntry[i].level = 0xfe;
  };
}

void MISCTABLE::MarkLocation(RN obj, i32 level, i32 x, i32 y, i32 container)
{
  i32 index;
  if (obj.dbType() != dbMISC) return;
  index = obj.idx();
  miscEntry[index].level = (UI8)level;
  miscEntry[index].x = (UI8)x;
  miscEntry[index].y = (UI8)y;
  miscEntry[index].container = (UI8)container;
}

void MISCTABLE::Print(FILE *f)
{
  i32 i;
  fprintf(f,"\n\nLocation of all Miscellaneous items in database\n");
  for (i=0; i<db.NumEntry(dbMISC); i++)
  {
    fprintf(f,"%3d  ",i);
    if (miscEntry[i].level == 0xff)
    {
      fprintf(f,"Unknown location.  Lost\n");
    }
    else if (miscEntry[i].level == 0xfe)
    {
      fprintf(f,"Unused entry...available\n");
    }
    else if ( /* (miscEntry[i].level>=0) && */ // ROQUEN: must be true (unsigned value)
              (miscEntry[i].level<=99) )
    {
      printAbsLocation(f,miscEntry[i].level,miscEntry[i].x,miscEntry[i].y);
      if (miscEntry[i].container == 0)
      {
        fprintf(f," lying loose\n");
      }
      else if (miscEntry[i].container == 1)
      {
        fprintf(f," in a chest\n");
      }
      else if (miscEntry[i].container == 2)
      {
        fprintf(f," in monster's possession\n");
      }
      else if (miscEntry[i].container == 4)
      {
        fprintf(f," in timer queue monster's possession\n");
      }
      else
      {
        fprintf(f,"\n");
      };
    }
    else if (   (miscEntry[i].level>=101)
             && (miscEntry[i].level<=104) )
    {
      fprintf(f," In character's possession\n");
    }
    else
    {
      fprintf(f,"Internal ASCII Dump error\n");
    };
  };
}

MISCTABLE miscTable;


void MarkBackpackLocations()
{
  i32 ch;
  i32 poss;
  RN containedObject;
  CHARDESC *pch;
  DB9 *pDB9;
  for (ch=0; ch <d.NumCharacter; ch++)
  {
    pch = d.CH16482+ch;
    for (poss=0; poss<30; poss++)
    {
      if (pch->Possession(poss) != RNnul)
      {
        if (pch->Possession(poss).dbType() == dbMISC)
        {
          miscTable.MarkLocation(pch->Possession(poss),
                                 101 + ch,
                                 d.partyX,
                                 d.partyY,
                                 3);
        }
        else if (pch->Possession(poss).dbType() == dbCHEST)
        {
          pDB9 = GetRecordAddressDB9(pch->Possession(poss));
          containedObject = pDB9->contents();
          while (containedObject != RNeof)
          {
            if (containedObject.dbType() == dbMISC)
            {
              miscTable.MarkLocation(containedObject,
                                     d.partyLevel,
                                     d.partyX,
                                     d.partyY,
                                     3);
            };
            containedObject = GetDBRecordLink(containedObject);
          };
        };
      };
    };
  };
}

void MarkQueuedMonsterPossessionLocations()
{
  //i32 i;
  RN obj, poss;
  TIMER *pTimer;
  DB4 *pMon;
  TIMER_SEARCH timerSearch;
  while (timerSearch.FindNextTimer())
  //for (i=1; i<gameTimers.NumTimer(); i++) xxTIMERxx Change to FindNextTimer
  {
    pTimer = timerSearch.TimerAddr();
    //if (   (d.pTimer(d.TimerQue(i))->Function() == TT_60)
    //     ||(d.pTimer(d.TimerQue(i))->Function() == TT_61) )
    if (   (pTimer->Function() == TT_60)
         ||(pTimer->Function() == TT_61) )
    {
      //pTimer = d.pTimer(d.TimerQue(i));
      obj = pTimer->timerObj8();
      if (obj.dbType() != dbMONSTER) continue;
      pMon = GetRecordAddressDB4(obj);
      poss = pMon->possession();
      for (poss = pMon->possession();
           poss != RNeof;
           poss = GetDBRecordLink(poss))
      {
        miscTable.MarkLocation(
                     poss, 
                     //(pTimer->timerTime >> 24) & 0xff,
                     pTimer->Level(),
                     pTimer->timerUByte6(),
                     pTimer->timerUByte7(),
                     4);
      };
    };
  };
}

class TARGETS
{
  i32 m_numTarget;
  i32 *m_targets;
  i32 m_allocated;
public:
  TARGETS();
  ~TARGETS();
  void Clear();
  void Add(i32 typ, i32 sl, i32 dx, i32 sy, i32 tl, i32 tx, i32 ty);
  i32 Search(i32 indx, i32 tl, i32 tx, i32 ty,
            i32 *type, i32 *level, i32 *x, i32 *y);
};

TARGETS::TARGETS()
{
  m_numTarget=0;
  m_allocated=0;
  m_targets=NULL;
}

TARGETS::~TARGETS()
{
  Clear();
}

void TARGETS::Clear()
{
  if (m_targets!=NULL) UI_free(m_targets);
  m_targets=NULL;
  m_numTarget=0;
  m_allocated=0;
}

void TARGETS::Add(i32 t, i32 sl, i32 sx, i32 sy, i32 tl, i32 tx, i32 ty)
{
  if (m_allocated <= m_numTarget)
  {
    m_allocated += m_allocated/3 + 1;
    m_targets = (i32 *)UI_realloc(m_targets,m_allocated*sizeof(i32),
                                  MALLOC059);
  };
  m_targets[m_numTarget++] = 
       (((((((((((t<<4)|sl)<<5)|sx)<<5)|sy)<<4)|tl)<<5)|tx)<<5)|ty;
}

i32 TARGETS::Search(i32 indx, i32 tl, i32 tx, i32 ty,
                    i32 *type, i32 *sl, i32 *sx, i32 *sy)
{
  i32 r, target;
  indx++;
  if (indx >= m_numTarget) return -1;
  target = ((tl<<5)|tx)<<5|ty;
  while (indx < m_numTarget)
  {
    if ((m_targets[indx] & 0x3fff) == target)
    {
      r = m_targets[indx];
      *type = (r >> 28) & 0xf;
      *sl   = (r >> 24) & 0xf;
      *sx   = (r >> 19) & 0x1f;
      *sy   = (r >> 14) & 0x1f;
      return indx;
    };
    indx++;
  }
  return -1;
}


TARGETS Targets;

void PrintBinaryValue(FILE *f, i32 val, i32 num)
{
  while (num > 0)
  {
    num--;
    fprintf(f, "%c", val & (1<<num) ? '1' : '0');
  };
}

void printAbsLocation(FILE *f, i32 level, i32 x, i32 y)
{
  x += d.pLevelDescriptors[level].offsetX;
  y += d.pLevelDescriptors[level].offsetY;
  fprintf(f,"%02d(%02d,%02d)",level, x, y);
}

void printLocation(char *f, i32 level, i32 x, i32 y)
{
  sprintf(f,"%02d(%02d,%02d)",level, x, y);
}


const char *MonsterName(MONSTERTYPE mt)
{
  const char *name;
  switch (mt)
  {
  case 0x00: name = "Scorpion"; break;
  case 0x01: name = "Slime_Devil"; break;
  case 0x02: name = "Giggler"; break;
  case 0x03: name = "Flying_Eye"; break;
  case 0x04: name = "Hellhound"; break;
  case 0x06: name = "Screamer"; break;
  case 0x07: name = "Rock_Pile"; break;
  case 0x08: name = "Rive"; break;
  case 0x09: name = "Stone_Golem"; break;
  case 0x0a: name = "Mummy"; break;
  case 0x0b: name = "Black_Flame"; break;
  case 0x0c: name = "Skeleton"; break;
  case 0x0d: name = "Couatl"; break;
  case 0x0e: name = "Vexirk"; break;
  case 0x0f: name = "Worm"; break;
  case 0x10: name = "Ant_Man"; break;
  case 0x11: name = "Muncher"; break;
  case 0x12: name = "Deth_Knight"; break;
  case 0x13: name = "Zytaz"; break;
  case 0x14: name = "Water_Elemental"; break;
  case 0x15: name = "Oitu"; break;
  case 0x16: name = "Demon"; break;
  case 0x17: name = "Lord_Chaos"; break;
  case 0x18: name = "Dragon"; break;
  case 0x1a: name = "Grey_Lord"; break;
  default: name = "Unrecognized_Type";
  };
  return name;
}

void Info(FILE *f)
{
  fprintf(f,"HINT DB3 = switch or actuator\n");
  fprintf(f,"HINT CF  = Cell flags\n");
  fprintf(f,"HINT L   = dungeon level\n");
  fprintf(f,"HINT TX  = target X\n");
  fprintf(f,"HINT TY  = target Y\n");
  fprintf(f,"HINT TP  = target position\n");
  fprintf(f,"HINT OO  = once only\n");
  fprintf(f,"HINT au  = audible\n");
  fprintf(f,"HINT NC  = normally closed\n");
  fprintf(f,"HINT NO  = normally open\n");
  fprintf(f,"HINT DL  = delay\n");
  fprintf(f,"HINT act = action\n");
  fprintf(f,"HINT LK  = link (pos,db,index)\n");
  fprintf(f,"HINT obj = object (pos,db,index)\n");
  fprintf(f,"HINT disable = #clock ticks monster generator is disabled\n");
  fprintf(f,"HINT num = number of monsters to generate\n");
  fprintf(f,"\n\n");
}

void GetText(char *result, i32 index)
{
  i32 bitnum, LOCAL_4;
  char SUBST28 = '/'; //What to substitute for code decimal 28.
  char *LOCAL_8;
  char *A2 = (char *)(d.compressedText + d.indirectTextIndex[index]);
  dReg D0, D4, D7;
  LOCAL_4 = 0;
  D7W = 1;
  bitnum = 10;
  for (;;)
  {
    if (bitnum < 0)
    {
      A2 += 2;
      bitnum = 10;
    };
    D4W = (i16)((LE16(wordGear(A2)) >> bitnum) & 0x1f);
    bitnum -= 5;
    if (LOCAL_4 != 0)
    {
      *result = 0;
      if (LOCAL_4 == 30)
      {
        if (D7W != 0)
        {
          LOCAL_8 = d.Byte10016 + 8*D4W;
        }
        else
        {
          LOCAL_8 = d.Byte10336 + 8*D4W;
        };
      }
      else
      {
        if (D7W != 0)
        {
          LOCAL_8 = d.Byte10080 + 2*D4W;
        }
        else
        {
          LOCAL_8 = d.Byte10080 + 2*D4W;
        };
      };
      StrCat(result, LOCAL_8);
      D0W = StrLen(LOCAL_8);
      result += D0W;
      LOCAL_4 = 0;
    }
    else
    {
      if (D4W < 28)
      {
        if (D7W != 0)
        {
          if (D4W== 26)
          {
            D4W = 32;
          }
          else
          {
            if (D4W ==27)
            {
              D4W = 46;
            }
            else
            {
               D4W += 65;
            };
          };
        };
        *(result++) = D4B;
      }
      else
      {
        if (D4W == 28)
        {
          *(result++) = SUBST28;
        }
        else
        {
          if (D4W <= 30)
          {
            LOCAL_4 = D4W;
          }
          else
          {
            break;
          };
        };
      };
    };
  };
  *(result++) = 0;
}

void PrintTimer5Results(FILE *f, 
                        i32 level, i32 x, i32 y, i32 /*pos*/, 
                        i32 action)
{
  RN curObj;
  DB3 *pDB3;
  i32 actuatorType;
  curObj = FindFirstObject(x, y);
  while (curObj != RNeof)
  {
    switch (curObj.dbType())
    {
    case dbACTUATOR: // Actuator
      pDB3 = GetRecordAddressDB3(curObj);
      actuatorType = pDB3->actuatorType();
      switch (actuatorType)
      {
      case 0:
        fprintf(f,"      ");
        fprintf(f,"Actuator %04d at ",curObj.idx());
        printAbsLocation(f,level,x,y);
        fprintf(f," is disabled\n");
        break;
      case 3: //Does nothing so don't mention it.
        break;
      case 6:
        fprintf(f,"      Create monsters at ");
        printAbsLocation(f,level,x,y);
        fprintf(f,"\n");
        break;
      case 7: //Does nothing so don't mention it.
        break;
      case 48:
        fprintf(f, "      ");
        switch (action)
        {
        case 0: fprintf(f,"Increment"); break;
        default: fprintf(f,"Decrement");
        };
        fprintf(f," Counter at ");
        printAbsLocation(f,level,x,y);
        fprintf(f,"\n");
        break;
      default:
        fprintf(f,"      Unknown timer 5 results\n");
      };
    };
    curObj = GetDBRecordLink(curObj);
  };
}


void PrintTimer6Results(FILE *f, 
                        i32 level, i32 x, i32 y, i32 pos, 
                        i32 action)
{
  RN curObj;
  DB3 *pDB3;
  i32 actuatorType;
  curObj = FindFirstObject(x, y);
  while (curObj != RNeof)
  {
    switch (curObj.dbType())
    {
    case dbACTUATOR: // Actuator
      pDB3 = GetRecordAddressDB3(curObj);
      actuatorType = pDB3->actuatorType();
      switch (actuatorType)
      {
      case 0:
        fprintf(f,"      ");
        fprintf(f,"Actuator %04d at ",curObj.idx());
        printAbsLocation(f,level,x,y);
        fprintf(f," is disabled\n");
        break;
      case 1: break; //does nothing
      case 4: break; //does nothing
      case 5:
        fprintf(f, "      ");
        switch (action)
        {
        case 0: fprintf(f,"Set"); break;
        case 1: fprintf(f,"Clear"); break;
        case 2: fprintf(f,"Toggle"); break;
        default: fprintf(f,"Hold bit set on"); break;
        };
        fprintf(f," bit %d in ACTUATOR %d at ",pos,curObj.idx());
        printAbsLocation(f, level, x, y);
        fprintf(f, "\n");
        break;
      case 6:
      case 48:
        fprintf(f, "      ");
        switch (action)
        {
        case 0: fprintf(f,"Increment"); break;
        default: fprintf(f,"Decrement");
        };
        fprintf(f," Counter at ");
        printAbsLocation(f,level,x,y);
        fprintf(f,"\n");
        break;
      case 8:
        fprintf(f,"      ");
        fprintf(f,"Launches missile at ");
        printAbsLocation(f,level,x,y);
        fprintf(f,"\n");
        break;
      case 10:
        fprintf(f,"      ");
        fprintf(f,"Launches missile at ");
        printAbsLocation(f,level,x,y);
        fprintf(f,"\n");
        break;
      case 14:
        fprintf(f,"      ");
        fprintf(f,"Launches missile at ");
        printAbsLocation(f,level,x,y);
        fprintf(f,"\n");
        break;
      case 15:
        fprintf(f,"      ");
        fprintf(f,"Launches missiles at ");
        printAbsLocation(f,level,x,y);
        fprintf(f,"\n");
        break;
      case 18:
        fprintf(f,"      End of Game???\n");
        break;
      default:
        fprintf(f,"      Unknown timer 6 results\n");
      };
    };
    curObj = GetDBRecordLink(curObj);
  };
}

void PrintTimer7Results(FILE *f, 
                        i32 level, i32 x, i32 y, i32 /*pos*/, 
                        i32 action)
{
  const char *act;
  fprintf(f, "      ");
  switch (action)
  {
  case 0: act = "Open"; break;
  case 1: act = "Close"; break;
  case 2: act = "Toggle"; break;
  case 3: act = "Hold open"; break;
  default: act = "?????";
  };
  fprintf(f,"%s FalseWall at ", act);
  printAbsLocation(f,level,x,y);
  fprintf(f,"\n");
}



void PrintTimer8Results(FILE *f, 
                        i32 level, i32 x, i32 y, i32 /*pos*/, 
                        i32 action)
{
  const char *act;
  fprintf(f, "      ");
  switch (action)
  {
  case 0: act = "Activate"; break;
  case 1: act = "Deactivate"; break;
  case 2: act = "Toggle"; break;
  default: act = "Hold";
  };
  fprintf(f,"%s Teleporter at ", act);
  printAbsLocation(f,level,x,y);
  fprintf(f,"\n");
}

void PrintTimer9Results(FILE *f, 
                        i32 level, i32 x, i32 y, i32 /*pos*/, 
                        i32 action)
{
  const char *act;
  fprintf(f, "      ");
  switch (action)
  {
  case 0: act = "Open"; break;
  case 1: act = "Close"; break;
  case 2: act = "Toggle"; break;
  default: act = "Hold";
  };
  fprintf(f,"%s Pit at ", act);
  printAbsLocation(f,level,x,y);
  fprintf(f,"\n");
}

void PrintTimer10Results(FILE *f, 
                        i32 level, i32 x, i32 y, i32 /*pos*/, 
                        i32 action)
{
  const char *act;
  fprintf(f, "      ");
  switch (action)
  {
  case 0: act = "Open"; break;
  case 1: act = "Close"; break;
  case 2: act = "Toggle"; break;
  default: act = "Hold";
  };
  fprintf(f,"%s Door at ", act);
  printAbsLocation(f,level,x,y);
  fprintf(f,"\n");
}


void PrintActuatorResult(FILE *f, 
                         i32 level, i32 x, i32 y, i32 pos, 
                         i32 action)
{
  CELLFLAG cf;
  ROOMTYPE rt;
  cf = GetCellFlags(x, y);
  rt = (ROOMTYPE)(cf>>5);
  switch (rt)
  {
  case roomSTONE:
    PrintTimer6Results(f,level,x,y,pos,action);
    break;
  case roomOPEN:
    PrintTimer5Results(f,level,x,y,pos,action);
    break;
  case roomPIT:
    PrintTimer9Results(f,level,x,y,pos,action);
    break;
  case roomTELEPORTER:
    PrintTimer8Results(f,level,x,y,pos,action);
    break;
  case roomFALSEWALL:
    PrintTimer7Results(f,level,x,y,pos,action);
    break;
  case roomDOOR:
    PrintTimer10Results(f,level,x,y,pos,action);
    break;
  default:
    fprintf(f,"      Unknown Actuator results\n");
  };
}

void PrintLink(FILE *f, RN link)
{
  if (link==RNeof)
  {
    fprintf(f,"LK=eof       ");
  }
  else
  {
    fprintf(f,"LK=%d,%02d,%04d ",
            link.pos(),link.dbType(),link.idx());
  };
}

void DumpMonsterGenerator(FILE *f, DB3 *pDB3)
{
  i32 num, react;
  fprintf(f,"     %03d %s ",pDB3->value(),
                MonsterName((MONSTERTYPE)pDB3->value()));
  fprintf(f,"Monster Generator num=");
  num=pDB3->delay();
  if (num&0x08) fprintf(f,"1->%d ",num&7);
  else fprintf(f,"%d ",num);
  fprintf(f,"OO=%d ",pDB3->onceOnly());
  react = pDB3->disableTime();
  if (react >= 128) react = 64 * (react-126);
  fprintf(f,"Disable=%04d ",react);
}

void DumpDB0(FILE *f, RN object, CELLFLAG cf)
{
  DB0 *pDB0;
  pDB0 = GetRecordAddressDB0(object);
  if (pDB0->link() == RNnul)
  {
    fprintf(f, "DB0 NULL record\n");
    return;
  };
  fprintf(f, "    DOOR %04d ",object.idx());
  PrintLink(f,pDB0->link());
  fprintf(f,"0x%04x ",(ui16)pDB0->word2);
  fprintf(f,cf&8?"N/S ":"E/W ");
  if (pDB0->doorSwitch()) fprintf(f,"w/Switch ");
  fprintf(f,pDB0->doorType()?"Type 1   ":"Type 0 ");
  if (pDB0->canOpenWithFireball())
  {
    fprintf(f,"Open with Fireball ");
  };
  if (pDB0->canOpenWithAxe())
  {
    fprintf(f,"Can open with Axe ");
  };
  fprintf(f,"opens=%s ",pDB0->mode()?"up ":"sideways ");
  fprintf(f,"\n");
}

void DumpDB1(FILE *f, RN object, CELLFLAG /*cf*/)
{
  DB1 *pDB1;
  const char *sNoise, *sPos, *sWho;
  pDB1 = GetRecordAddressDB1(object);
  if (pDB1->link() == RNnul)
  {
    fprintf(f, "DB1 NULL record\n");
    return;
  };
  fprintf(f, "    TELEPORTER %04d ",object.idx());
  PrintLink(f,pDB1->link());
  sPos = pDB1->facingMode() ? "face" : "rotate";
  sNoise = pDB1->audible()?"Buzz":"No Buzz";
  switch(pDB1->what())
  {
  case 0: sWho="Objects Only"; break;
  case 1: sWho="Monsters Only"; break;
  case 2: sWho="Objects and Party"; break;
  default: sWho="Everything"; break;
  };
  fprintf(f," to ");
  printAbsLocation( f,
                    pDB1->destLevel(),
                    pDB1->destX(),
                    pDB1->destY());
  fprintf(f, " %s %d %s %s\n",
            sPos, pDB1->rotation(), sNoise, sWho);
}

void printWallGraphic(FILE *f,DB3 *pDB3, RN object,
                         i32 level, i32 x, i32 y)
{
  i32 wallGraphicOrdinal;
  i32 wallGraphicIndex;
  i32 wallGraphicNum;
  i32 pos;
  LEVELDESC *pLevelDesc;
  CELLFLAG adjCell;
  ui8 *pWallDecorationTOC;
  //First determine if graphic would face a stone wall.
  pos = object.pos();
  pLevelDesc = &d.pLevelDescriptors[level];
  pWallDecorationTOC =   d.LevelCellFlags[d.width-1]
                       + d.height
                       + pLevelDesc->numMonsterType();
  switch (pos)
  {
  case 0: y--; break;
  case 1: x++; break;
  case 2: y++; break;
  case 3: x--; break;
  };
  if ( (x<0)||(x>=d.width)||(y<0)||(y>=d.height))
  {
    fprintf(f,"Graphic unreachable.");
    return;
  };
  adjCell = GetCellFlags(x, y);
  if ((adjCell>>5) == roomSTONE)
  {
    fprintf(f,"Graphic unreachable.");
    return;
  };
  wallGraphicOrdinal = pDB3->wallGraphicOrdinal();
  if (wallGraphicOrdinal==0) 
  {
    fprintf(f,"No Wall Graphic.");
    return;
  };
  wallGraphicIndex = wallGraphicOrdinal-1;
  wallGraphicNum = pWallDecorationTOC[wallGraphicIndex];
  switch (wallGraphicNum)
  {
  case 1:
    fprintf(f,"Graphic=Square Alcove.");
    break;
  case 2:
    fprintf(f,"Graphic=VI Altar.");
    break;
  case 3:
    fprintf(f,"Graphic=Ornate Alcove.");
    break;
  case 5:
    fprintf(f,"Graphic=Plain Keyhole.");
    break;
  case 6:
    fprintf(f,"Graphic=Wall Ring.");
    break;
  case 7:
    fprintf(f,"Graphic=Small Switch.");
    break;
  case 8:
    fprintf(f,"Graphic=Dent.");
    break;
  case 11:
    fprintf(f,"Graphic-Wall Crack.");
    break;
  case 12:
    fprintf(f,"Graphic=Slime Outlet.");
    break;
  case 14:
    fprintf(f,"Graphic=Tiny Switch.");
    break;
  case 15:
    fprintf(f,"Graphic=Green Button(Out).");
    break;
  case 16:
    fprintf(f,"Graphic=Blue Button(Out).");
    break;
  case 17:
    fprintf(f,"Graphic=Coin Slot.");
    break;
  case 18:
    fprintf(f,"Graphic=Double Keyhole.");
    break;
  case 19:
    fprintf(f,"Graphic=Square Keyhole.");
    break;
  case 20:
    fprintf(f,"Graphic=Winged Keyhole.");
    break;
  case 21:
    fprintf(f,"Graphic=Block Keyhole.");
    break;
  case 22:
    fprintf(f,"Graphic=Stone Keyhole.");
    break;
  case 23:
    fprintf(f,"Graphic=Cross Keyhole.");
    break;
  case 24:
    fprintf(f,"Graphic=Gold/Turquoise Keyhole.");
    break;
  case 25:
    fprintf(f,"Graphic=Skeleton Keyhole.");
    break;
  case 26:
    fprintf(f,"Graphic=Gold Keyhole.");
    break;
  case 27:
    fprintf(f,"Graphic=Turquoise Keyhole.");
    break;
  case 28:
    fprintf(f,"Graphic=Emerald Keyhole.");
    break;
  case 29:
    fprintf(f,"Graphic=Ruby Keyhole.");
    break;
  case 30:
    fprintf(f,"Graphic=Ra Keyhole.");
    break;
  case 31:
    fprintf(f,"Graphic=Master Keyhole.");
    break;
  case 32:
    fprintf(f,"Graphic=Gem Hole.");
    break;
  case 35:
    fprintf(f,"Graphic=Water Fountain");
    break;
  case 36:
    fprintf(f,"Graphic=Manacles.");
    break;
  case 37:
    fprintf(f,"Graphic=Demon Face.");
    break;
  case 38:
    fprintf(f,"Graphic=Empty Torch Holder.");
    break;
  case 40:
    fprintf(f,"Graphic=Poison Holes.");
    break;
  case 41:
    fprintf(f,"Graphic=Fireball Holes.");
    break;
  case 42:
    fprintf(f,"Graphic=Dagger Holes.");
    break;
  case 43:
    fprintf(f,"Graphic=Mirror.");
    break;
  case 44:
    fprintf(f,"Graphic=Lever(Up).");
    break;
  case 45:
    fprintf(f,"Graphic=Lever(Down).");
    break;
  case 47:
    fprintf(f,"Graphic=Red Button(Out).");
    break;
  case 48:
    fprintf(f,"Graphic=Eye Switch.");
    break;
  case 49:
    fprintf(f,"Graphic=Big Switch(Out).");
    break;
  case 50:
    fprintf(f,"Graphic=Crack Switch(Out).");
    break;
  case 54:
    fprintf(f,"Graphic=Big Switch(In).");
    break;
  default:
    fprintf(f,"Unknown Wall Graphic ordinal = %d num= %d.",
               wallGraphicOrdinal, wallGraphicNum);
  };
}

void DumpDB2(FILE *f, DB2 *pDB2, CELLFLAG /*cf*/)
{
  i32 index = pDB2->index();
  fprintf(f,"    Text index %04x\n",index);
  char rslt[1000];
  GetText(rslt, index);
  fprintf(f, "    %s\n",rslt);
  
  //if (pDB5->link() == RNnul)
  //{
  //  fprintf(f, "    object NULL record\n");
  //  return;
  //};
  //fprintf(f, "    object %04d ",object.idx());
  //PrintLink(f,pDB5->link());
  //fprintf(f,"0x%04x ",
  //           (ui16)pDB5->word2);
  //ot = GetModifiedObjectType(object);
  //fprintf(f," objecttype = %03d ", ot);
  //fprintf(f,"%s ",d.ObjectNames[ot]);
  //fprintf(f,"\n");
}



void DumpDB3(FILE *f, RN object, 
             i32 level, i32 x, i32 y, CELLFLAG cf)
{
  DB3 *pDB3;
  ROOMTYPE rt;
  ACTUATORACTION actuatorAction;
  const char *possession;
  rt = (ROOMTYPE)(cf>>5);
  pDB3 = GetRecordAddressDB3(object);
  if (pDB3->link() == RNnul)
  {
    fprintf(f, "    ACTUATOR NULL record\n");
    return;
  };
  fprintf(f, "    ACTUATOR");
  fprintf(f,"[%04x %04x %04x %04x] ",
             (ui16)pDB3->link().ConvertToInteger(),
             (ui16)pDB3->word2,
             (ui16)pDB3->word4,
             (ui16)pDB3->word6);
  if (pDB3->localActionOnly())
  {//Some sort of local action
    fprintf(f,"typ=%02d val=%03d\n",
               pDB3->actuatorType(),
               pDB3->value());
    switch (pDB3->actuatorType())
    {
    case 0:
      fprintf(f,"      Actuator disabled.\n");
      break;
    case 1:
      fprintf(f,"      Rotate Actuator List (In/Out, etc).\n");
      break;
    case 3:
      if (pDB3->localActionType() == 10)
      {
        fprintf(f,"      Unknown increase in skill???\n");
      }
      else
      {
        fprintf(f,"      Rotate Actuator List (In/Out, etc).\n");
      };
      break;
    case 6:
      DumpMonsterGenerator(f, pDB3);
      fprintf(f,"\n");
      break;
    case 8:
      fprintf(f,"      ");
      printWallGraphic(f,pDB3,object,level,x,y);
      fprintf(f,"  Not affected by touching.  Launches ");
      switch(pDB3->value())
      {
      case 0: fprintf(f,"Fireball missiles\n"); break;
      case 1: fprintf(f,"Poison Blobs\n"); break;
      case 2: fprintf(f,"Lightning Bolts\n"); break;
      case 3: fprintf(f,"Dispells\n"); break;
      case 6: fprintf(f,"Poison Bolts\n"); break;
      case 7: fprintf(f,"Poison Clouds\n"); break;
      default: fprintf(f,"????? missiles\n");
      };
      break;
    case 10:
      fprintf(f,"      ");
      printWallGraphic(f,pDB3,object,level,x,y);
      fprintf(f,"  Not affected by touching.  Launches two ");
      switch(pDB3->value())
      {
      case 0: fprintf(f,"Fireball missiles\n"); break;
      case 1: fprintf(f,"Poison Blobs\n"); break;
      case 2: fprintf(f,"Lightning Bolts\n"); break;
      case 3: fprintf(f,"Dispells\n"); break;
      case 6: fprintf(f,"Poison Bolts\n"); break;
      case 7: fprintf(f,"Poison Clouds\n"); break;
      default: fprintf(f,"missiles of type 0x%04x\n",
                 pDB3->value() + 0xff80);
      };
      break;

/*
    case 8:
      switch (cf>>5)
      {
      char *missile;
      case 0: //stone
        switch (pDB3->value())
        {
        case 0: missile = "Fireball"; break;
        case 3: missile = "ff83"; break;
        case 7: missile = "ff87"; break;
        default: missile = "????"; break;
        };
        fprintf(f,"%s launcher\n", missile);
        break;
      case 1:
        fprintf(f,"Item possession test\n");
        break;
      case 14
        fprintf(f,"Single object missile launcher\n");
        break;
      case 15:
        fprintf(f,"Double object missile launcher\n");
        break;
      default:
        fprintf(f, "Function unknown in this context\n");
      };
      break;
*/
      case 13:
    case 14:
      fprintf(f,"      ");
      printWallGraphic(f,pDB3,object,level,x,y);
      fprintf(f,"  Not affected by touching.  Launches "
                "one of whatever is in the proper position\n");
      break;
    case 15:
      fprintf(f,"      ");
      printWallGraphic(f,pDB3,object,level,x,y);
      fprintf(f,"  Not affected by touching.  Launches "
                "two of whatever is in the proper position\n");
      break;
    default: fprintf(f,"      Function unknown\n");
    };
  }
  else
  {
    fprintf(f,"typ=%02d ",pDB3->actuatorType());
    fprintf(f,"val=%03d ",pDB3->value());
    fprintf(f,"Target=");
    printAbsLocation(f,
                     level,
                     pDB3->targetX(),
                     pDB3->targetY());
    fprintf(f,"%1d ",pDB3->targetPos());
    fprintf(f,"OO=%d ",pDB3->onceOnly());
    fprintf(f,"au=%d ",pDB3->audible());
    fprintf(f,"act=%d ",pDB3->action());
    fprintf(f,pDB3->normallyClosed()?"NC ":"NO ");
    fprintf(f,"DL=%02d \n",pDB3->delay());
    if (rt == roomSTONE)
    {
      fprintf(f,"      ");
      switch (pDB3->actuatorType())
      {
      case 0:
        printWallGraphic(f,pDB3,object,level,x,y);
        fprintf(f," Actuator Disabled.\n");
        break;
      case 1:
        actuatorAction=pDB3->action();
        fprintf(f,"Simple Pushbutton");
        if (actuatorAction==actuatorAction_CONSTPRESSURE)
        {
          fprintf(f,"-ConstPressure so no action.  ");
          break;
        };
        fprintf(f,"-Press with anything.  ");
        printWallGraphic(f,pDB3,object,level,x,y);  
        fprintf(f,"\n");
        break;
      case 2:
        printWallGraphic(f,pDB3,object,level,x,y);
        fprintf(f, "  Operated by %s hand.\n",
                pDB3->normallyClosed()?"empty":"full");
        break;
      case 3:
        printWallGraphic(f,pDB3,object,level,x,y);
        fprintf(f, " Operated by %s",
                     d.ObjectNames[pDB3->value()]);
        fprintf(f, " and object remains in hand.\n");
        break;
      case 4:
        printWallGraphic(f,pDB3,object,level,x,y);
        fprintf(f, " Operated by %s",
                     d.ObjectNames[pDB3->value()]);
        fprintf(f, " and object removed from hand.\n");
        break;
      case 5:
        fprintf(f,"And/Nor Gate.  Current binary value = ");
        PrintBinaryValue(f,pDB3->value()&15,4);
        fprintf(f,".  Target binary value = ");
        PrintBinaryValue(f,(pDB3->value() >> 4) & 15, 4);
        fprintf(f,"\n");
        break;
      case 6:
      case 48:
        fprintf(f,"A counter.  Current value is %d.  ",
                       pDB3->value());
        if (pDB3->normallyClosed())
        {
          if (pDB3->action() != 3)
          {
            fprintf(f,"\nVery Strange...Normally closed and action = %d\n",
                          pDB3->action());
          };
        };
        printWallGraphic(f,pDB3,object,level,x,y);
        fprintf(f,"  Not affected by touching.\n");
        break;
      case 13:
        printWallGraphic(f,pDB3,object,level,x,y);
        fprintf(f," It holds a %s\n",d.ObjectNames[pDB3->value()]);
        break;
      case 14:
        printWallGraphic(f,pDB3,object,level,x,y);
        fprintf(f,"  Actuator not affected by touching.\n");
        break;
      case 18:
        printWallGraphic(f,pDB3,object,level,x,y);
        fprintf(f,"  End of game Sequence?\n");
        break;
      default: fprintf(f,"Unknown actuator type=%d\n",
                       pDB3->actuatorType());
      }; // switch (Actuator type)
      PrintActuatorResult(f,
                          level,
                          pDB3->targetX(),
                          pDB3->targetY(),
                          pDB3->targetPos(),
                          pDB3->action());
    }
    else
    { // Open room.  A pressure pad
      fprintf(f,"      ");
      switch(pDB3->actuatorType())
      {
      case 0:
        fprintf(f,"Actuator disabled\n");
        break;
      case 1:
        fprintf(f,"Pressure pad operated by anything.\n");
        break;
      case 2:
        fprintf(f,"Pressure pad operated by party or monsters.\n");
        break;
      case 3:
        fprintf(f,"Pressure pad operated by party");
        if (pDB3->value() != 0)
        {
          fprintf(f,"-facing ");
          switch (pDB3->value()-1)
          {
          case 0: fprintf(f,"North"); break;
          case 1: fprintf(f,"East"); break;
          case 2: fprintf(f,"South"); break;
          case 3: fprintf(f,"West"); break;
          default: fprintf(f," %d", pDB3->value());
          };
        }
        else
        {
          fprintf(f," stepping ");
          if (pDB3->normallyClosed())
          {
            fprintf(f, "off");
          }
          else
          {
            fprintf(f,"on");
          };
          fprintf(f," the pad");
        };
        fprintf(f,"\n");
        break;
      case 4:
        fprintf(f,"A pressure pad operated only by a ");
        fprintf(f,"%s\n", d.ObjectNames[pDB3->value()]);
        break;
      case 7:
        fprintf(f,"A pressure pad operated by monsters.\n");
        break;
      case 8:
        if (pDB3->normallyClosed())
        {
          possession = "does not have";
        }
        else
        {
          possession = "has";
        };
        fprintf(f,"A pressure pad operated only if party "
                  "%s a %s\n",possession,d.ObjectNames[pDB3->value()]);
        break;
      case 48:
        fprintf(f,"A counter.  Current value is %d.  ",
                       pDB3->value());
        if (pDB3->normallyClosed())
        {
          if (pDB3->action() != 3)
          {
            fprintf(f,"\nVery Strange...Normally closed and action = %d\n",
                          pDB3->action());
          };
        };
        //printFloorGraphic(f,pDB3,object,level,x,y);
        fprintf(f,"  Not affected by touching.\n");
        break;
      default:
        fprintf(f,"Unknown Actuator type= %d.\n",
                   pDB3->actuatorType());
      };
      PrintActuatorResult(f,
                          level,
                          pDB3->targetX(),
                          pDB3->targetY(),
                          pDB3->targetPos(),
                          pDB3->action());
    };
  };
}

void DumpDB4(FILE *f, RN object, i32 level, i32 x, i32 y)
{
  DB4 *pDB4;
  MONSTERTYPE mt;
  char *obName;
  OBJ_NAME_INDEX objNI;
  i32 i;
  RN possession;
  pDB4 = GetRecordAddressDB4(object);
  if (pDB4->link() == RNnul)
  {
    fprintf(f, "Monster NULL record\n");
    return;
  };
  fprintf(f, "    MONSTER %04d ",object.idx());
  PrintLink(f,pDB4->link());
  mt = pDB4->monsterType();
  fprintf(f," monstertype = %02d ", mt);
  fprintf(f,"%s ",MonsterName(mt));
  fprintf(f," number=%d ",pDB4->numMonM1()+1);
  fprintf(f,"HP=");
  for (i=0;i<=pDB4->numMonM1();i++)
  {
    if (i != 0) fprintf(f,",");
    fprintf(f,"%04d",pDB4->hitPoints6[i]);
  };
  if (pDB4->possession()!=RNeof)
  {
    possession = pDB4->possession();
    fprintf(f,"possessions=");
    while (possession != RNeof)
    {
      if (possession.dbType() == dbMISC)
      {
        miscTable.MarkLocation(possession, level, x, y, 2);
      };
      //ot = GetObjectNameIndex(possession);
      objNI = possession.NameIndex();
      obName = d.ObjectNames[objNI];
      fprintf(f,"<%s>", obName);
      possession = GetDBRecordLink(possession);
    };
  };
  fprintf(f,"\n     ");
  for (i=0; i<sizeof(DB4); i++)
  {
    fprintf(f, "%02x ",*(((ui8 *)pDB4)+i));
  };
  fprintf(f,"\n");
  if (pDB4->possession()!=RNeof)
  {
    possession = pDB4->possession();
    while (possession != RNeof)
    {
      objNI = possession.NameIndex();
      obName = d.ObjectNames[objNI];
      fprintf(f,"      0x%04x <%s>\n", possession.ConvertToInteger(), obName);
      possession = GetDBRecordLink(possession);
    };
  };
}

void DumpDB5(FILE *f, RN object, i32 /*level*/)
{
  DB5 *pDB5;
  OBJ_NAME_INDEX objNI;
  pDB5 = GetRecordAddressDB5(object);
  if (pDB5->link() == RNnul)
  {
    fprintf(f, "    object NULL record\n");
    return;
  };
  fprintf(f, "    object %04d ",object.idx());
  PrintLink(f,pDB5->link());
  fprintf(f,"0x%04x ",
             (ui16)pDB5->word2);
  objNI = object.NameIndex();
  fprintf(f," objecttype = %03d ", objNI);
  fprintf(f,"%s ",d.ObjectNames[objNI]);
  fprintf(f,"\n");
}



void DumpDB6(FILE *f, RN object, i32 /*level*/)
{
  DB6 *pDB6;
  OBJ_NAME_INDEX ot;
  char *obName;
  pDB6 = GetRecordAddressDB6(object);
  if (pDB6->link() == RNnul)
  {
    fprintf(f, "Clothing Item NULL record\n");
    return;
  };
  fprintf(f, "    CLOTHING ITEM %04d ",object.idx());
  PrintLink(f,pDB6->link());
  fprintf(f,"0x%04x ",
             (ui16)pDB6->word2);
  ot = object.NameIndex();
  obName = d.ObjectNames[ot];
  fprintf(f, "(%03d=%s)", ot, obName);
  fprintf(f,"\n");
}

void DumpDB7(FILE *f, RN object)
{
  DB7 *pDB7;
  pDB7 = GetRecordAddressDB7(object);
  fprintf(f,"    Scroll - Text object = %04x\n",pDB7->text().ConvertToInteger());
  //GetText(rslt, pDB7->textIndex());
  //fprintf(f,"    %s\n",rslt);    
  DumpDB2(f, GetRecordAddressDB2(pDB7->text()),0);
  //if (pDB5->link() == RNnul)
  //{
  //  fprintf(f, "    object NULL record\n");
  //  return;
  //};
  //fprintf(f, "    object %04d ",object.idx());
  //PrintLink(f,pDB5->link());
  //fprintf(f,"0x%04x ",
  //           (ui16)pDB5->word2);
  //ot = GetModifiedObjectType(object);
  //fprintf(f," objecttype = %03d ", ot);
  //fprintf(f,"%s ",d.ObjectNames[ot]);
  //fprintf(f,"\n");
}



void DumpDB8(FILE *f, RN object, i32 /*level*/)
{
  DB8 *pDB8;
  OBJ_NAME_INDEX objNI;
  char *obName;
  pDB8 = GetRecordAddressDB8(object);
  if (pDB8->link() == RNnul)
  {
    fprintf(f, "Potion NULL record\n");
    return;
  };
  fprintf(f, "    POTION %04d ",object.idx());
  PrintLink(f,pDB8->link());
  fprintf(f,"0x%04x ",
             (ui16)pDB8->word2);
  objNI = object.NameIndex();
  obName = d.ObjectNames[objNI];
  fprintf(f, "(%03d=%s)", objNI, obName);
  fprintf(f,"\n");
}

void DumpDB9(FILE *f, RN object, i32 level, i32 x, i32 y)
{
  DB9 *pDB9;
  OBJ_NAME_INDEX objNI;
  RN containedObject;
  char *obName;
  pDB9 = GetRecordAddressDB9(object);
  if (pDB9->link() == RNnul)
  {
    fprintf(f, "    NULL Chest record!\n");
    return;
  };
  fprintf(f, "    Chest %04d ",object.idx());
  PrintLink(f,pDB9->link());
  fprintf(f,"0x%04x 0x%04x 0x%02x 0x%02x",
             (ui16)pDB9->contents().ConvertToInteger(),
             (ui16)pDB9->word4(),
             (ui16)pDB9->uByte6(0),
             (ui16)pDB9->uByte6(1));
  containedObject = pDB9->contents();
  while (containedObject != RNeof)
  {
    if (containedObject.dbType() == dbMISC)
    {
      miscTable.MarkLocation(containedObject,level,x,y,1);
    };
    objNI = containedObject.NameIndex();
    obName = d.ObjectNames[objNI];
    fprintf(f,"<%s>", obName);
    containedObject = GetDBRecordLink(containedObject);
  };
  fprintf(f,"\n");
  containedObject = pDB9->contents();
  while (containedObject != RNeof)
  {
    objNI = containedObject.NameIndex();
    obName = d.ObjectNames[objNI];
    fprintf(f,"      0x%04x <%s>\n", containedObject.ConvertToInteger(), obName);
    if (containedObject.dbType() == dbSCROLL)
    {      
      DumpDB7(f, containedObject);
    };
    containedObject = GetDBRecordLink(containedObject);
  };
}



void DumpDB10(FILE *f, RN object, i32 /*level*/)
{
  DB10 *pDB10;
  OBJ_NAME_INDEX objNI;
  char *obName;
  pDB10 = GetRecordAddressDB10(object);
  if (pDB10->link() == RNnul)
  {
    fprintf(f, "Miscellaneous Item NULL record\n");
    return;
  };
  fprintf(f, "    MISCELLANEOUS ITEM %04d ",object.idx());
  PrintLink(f,pDB10->link());
  fprintf(f,"0x%04x ",
             (ui16)pDB10->word2);
  objNI = object.NameIndex();
  obName = d.ObjectNames[objNI];
  fputs(obName, f);
  fprintf(f,"\n");
}


void DumpDB(FILE *f, RN object, CELLFLAG cf, i32 level, i32 x, i32 y)
{
  if (f==NULL) return;
  switch(object.dbType())
  {
  case dbDOOR:
    DumpDB0(f, object, cf);
    break;
  case dbTELEPORTER:
    DumpDB1(f, object, cf);
    break;
  case dbTEXT:
    DumpDB2(f, GetRecordAddressDB2(object), cf);
    break;
  case dbACTUATOR:
    DumpDB3(f, object, level, x, y, cf);
    break;
  case dbMONSTER:
    DumpDB4(f, object, level, x, y);
    break;
  case dbWEAPON:
    DumpDB5(f, object, level);
    break;
  case dbCLOTHING:
    DumpDB6(f, object, level);
    break;
  case dbSCROLL:
    DumpDB7(f, object);
    break;
  case dbPOTION:
    DumpDB8(f, object, level);
    break;
  case dbCHEST:
    DumpDB9(f, object, level, x, y);
    break;
  case dbMISC:
    DumpDB10(f, object, level);
    miscTable.MarkLocation(object, level, x, y, 0);
    break;
  default:
    fprintf(f,"    unimplemented \n");
  };
}

void printSTONE(FILE *f, CELLFLAG /*cf*/)
{
  fprintf(f, " STONE ");
};

void printOPEN(FILE *f, CELLFLAG /*cf*/)
{
  fprintf(f, " OPEN ");
};

void printPIT(FILE *f, CELLFLAG /*cf*/)
{
  fprintf(f, " PIT ");
};

void printSTAIRS(FILE *f, CELLFLAG /*cf*/)
{
  fprintf(f, " STAIRS ");
};

void printTELEPORTER(FILE *f, CELLFLAG cf)
{
  fprintf(f, " TELEPORTER ");
  if (cf & 0x08) fprintf(f,"Active ");
  else fprintf(f,"Inactive ");
};

void printFALSEWALL(FILE *f, CELLFLAG cf)
{
  fprintf(f, " FALSEWALL ");
  fprintf(f,cf&4?"Invisible ":"Visible ");
  fprintf(f,cf&5?"Passable ":"Impassable ");
};

void printDOOR(FILE *f, CELLFLAG cf)
{
  i32 doorCondition;
  const char *p=NULL;
  fprintf(f, " DOOR ");
  doorCondition = cf & 7;
  switch(doorCondition)
  {
  case 0: p = "Open"; break;
  case 1: p = "passable"; break;
  case 2: p = "impassable"; break;
  case 3: p = "Impassable.  Perhaps passable to monsters."; break;
  case 4: p = "Closed. Perhaps passable to monsters."; break;
  case 5: p = "bashed open"; break;
  case 6: p = "impassable"; break;
  case 7: p = "impassable"; break;
  };
  fputs(p,f);
};


void GetTargetDB0(RN , CELLFLAG , i32 , i32 , i32 )
{
}

void GetTargetDB1(RN object, CELLFLAG, i32 level, i32 x, i32 y)
{ //Teleporter
  DB1 *pDB1;
  i32 tl, tx, ty;
  pDB1 = GetRecordAddressDB1(object);
  tl = pDB1->destLevel();
  tx = pDB1->destX();
  ty = pDB1->destY();
  Targets.Add(1,level,x,y,tl,tx,ty);
}

void GetTargetDB3(RN object, CELLFLAG, i32 level, i32 x, i32 y)
{
  DB3 *pDB3;
  i32 tx, ty;
  pDB3 = GetRecordAddressDB3(object);
  if (pDB3->localActionOnly()) return;
  tx = pDB3->targetX();
  ty = pDB3->targetY();
  Targets.Add(3,level,x,y,level,tx,ty);
}

void GetTargetDB4(RN, CELLFLAG, i32, i32, i32)
{
}

void GetTargetDB5(RN, CELLFLAG, i32, i32, i32)
{
}

void GetTargetDB6(RN, CELLFLAG, i32, i32, i32)
{
}

void GetTargetDB9(RN, CELLFLAG, i32, i32, i32)
{
}

void GetTargetDB10(RN, CELLFLAG, i32, i32, i32)
{
}


void getTargetDB(RN object, CELLFLAG cf, i32 level, i32 x, i32 y)
{
  switch(object.dbType())
  {
  case dbDOOR:
    GetTargetDB0(object, cf, level, x, y);
    break;
  case dbTELEPORTER:
    GetTargetDB1(object, cf, level, x, y);
    break;
  case dbTEXT:
    break;
  case dbACTUATOR:
    GetTargetDB3(object, cf, level, x, y);
    break;
  case dbMONSTER:
    GetTargetDB4(object, cf, level, x, y);
    break;
  case dbWEAPON:
    GetTargetDB5(object, cf, level, x, y);
    break;
  case dbCLOTHING:
    GetTargetDB6(object, cf, level, x, y);
    break;
  case dbCHEST:
    GetTargetDB9(object, cf, level, x, y);
    break;
  case dbMISC:
    GetTargetDB10(object, cf, level, x, y);
    break;
  };
}


void DumpLevelCells(FILE *f, i32 level)
{
  CELLFLAG cf;
  ROOMTYPE rt;
  i32 x, y;
  i32 indx, type,sl,sx,sy;
  RN object;
  LoadLevel(level);
  fprintf(f, "Level %1d Width=%02d Height=%02d\n",
             level, d.width,d.height);
  for (x=0; x<d.width; x++)
    for (y=0; y<d.height; y++)
    {
      fprintf(f,"CF ");
      printAbsLocation(f,level,x,y);
      cf = GetCellFlags(x,y);
      fprintf(f,"0x%02x=",cf);
      rt = (ROOMTYPE)(cf>>5);
      switch (rt)
      {
      case roomSTONE:      printSTONE(f,cf);      break;
      case roomOPEN:       printOPEN(f,cf);       break;
      case roomPIT:        printPIT(f,cf);        break;
      case roomSTAIRS:     printSTAIRS(f,cf);     break;
      case roomDOOR:       printDOOR(f,cf);       break;
      case roomTELEPORTER: printTELEPORTER(f,cf); break;
      case roomFALSEWALL:  printFALSEWALL(f,cf);  break;
      default: fprintf(f,"Roomtype =???\n");
      };
      fprintf(f, "\n");
      object=FindFirstObject(x, y);
      while (object != RNeof)
      {
        fprintf(f, "  OBJ ");
        printAbsLocation(f,level,x,y);
        fprintf(f, "%02d obj=%1d,%02d,%04d\n",
               object.pos(),
               object.pos(),object.dbType(),object.idx());
        DumpDB(f, object, cf, level, x, y);
        if (object == RNnul) break;
        object=GetDBRecordLink(object);
      };
      indx=-1;
      for (;;)
      {
        indx = Targets.Search(indx,level,x,y,&type,&sl,&sx,&sy);
        if (indx==-1) break;
        switch(type)
        {
        case 1: 
          fprintf(f,"  Destination of teleporter at ");
          printAbsLocation(f, sl, sx, sy);
          fprintf(f,"\n");
          break;
        case 3:
          fprintf(f,"  Target of actuator at ");
          printAbsLocation(f, sl, sx, sy);
          fprintf(f, "\n");
          break;
        }; // switch (type)
      }; //while (1)
    };
}

void ConstructTargetList(i32 level)
{
  i32 x, y;
  CELLFLAG cf;
  RN object;
  LoadLevel(level);
  for (x=0; x<d.width; x++)
    for (y=0; y<d.height; y++)
    {
      cf = GetCellFlags(x,y);
      object=FindFirstObject(x, y);
      while (object != RNeof)
      {
        if (object == RNnul) break;
        getTargetDB(object, cf, level, x, y);
        object=GetDBRecordLink(object);
      };
  };
}

void DumpCells(FILE *f)
{
  i32 level, numLevel;
  numLevel = d.dungeonDatIndex->NumLevel();
  fprintf(f, "Dungeon NumLevel=%d\n",numLevel);  
  Targets.Clear();
  for (level=0; level<numLevel; level++)
  {
    ConstructTargetList(level);
  };
  for (level=0; level<numLevel; level++)
  {
    DumpLevelCells(f, level);
  };
}

void DumpBit(FILE *f, const char *mask, const char *name, bool bit)
{
  fprintf(f,"           %s = %s %s\n",
          mask, bit?"   ":"Not", name);
}

void DumpMonster(FILE *f, i32 i, MONSTERDESC& mtDesc)
{
  i32 j;
  fprintf(f, "\nMonster %d  %s\n", i, MonsterName((MONSTERTYPE) i));
  fprintf(f, "  byte 0 = 0x%02x Monster Type\n",mtDesc.uByte0);
  fprintf(f, "  byte 1 = 0x%02x Sound Ordinal\n",mtDesc.attackSound);
  fprintf(f, "  word 2 = 0x%04x\n",(UI16)(mtDesc.word2));
  fprintf(f, "           bits 0-1 = %d = Horizontal Size\n",mtDesc.horizontalSize());
  DumpBit(f, "0x0004", "Sees All Directions",mtDesc.See360());
  DumpBit(f, "0x0008", "Blocked?"           ,mtDesc.word2Bit3());
  DumpBit(f, "0x0010", "????"               ,mtDesc.word2Bit4());
  DumpBit(f, "0x0020", "Levitating"         ,mtDesc.levitating());
  DumpBit(f, "0x0040", "nonMaterial"        ,mtDesc.nonMaterial());
  fprintf(f, "           bits 7-8 = %d = Vertical Size\n"
                                            ,mtDesc.verticalSize());
  DumpBit(f, "0x0200", "Object Dropper"     ,mtDesc.leavesDroppings());
  DumpBit(f, "0x0400", "AbsorbsDaggersEtc"  ,mtDesc.absorbsDaggersEtc());
  DumpBit(f, "0x0800", "Sees invisible"     ,mtDesc.seesInvisible());
  DumpBit(f, "0x1000", "Sees in dark"       ,mtDesc.canSeeInDark());
  DumpBit(f, "0x2000", "Invincible"         ,mtDesc.invincible());
  fprintf(f, "  word 4 = 0x%04x\n",(UI16)(mtDesc.word4.Word()));
  fprintf(f, "  bytes movementTicks and attackTicks = 0x%02x 0x%02x\n",
                     mtDesc.movementTicks06, mtDesc.attackTicks07);
  //for (j=0; j<6; j++)
  fprintf(f, "   defense      = 0x%02x\n", mtDesc.defense08);
  fprintf(f, "   baseHealth   = 0x%02x\n", mtDesc.baseHealth09);
  fprintf(f, "   attack       = 0x%02x\n", mtDesc.attack10);
  fprintf(f, "   poisonAttack = 0x%02x\n", mtDesc.poisonAttack11);
  fprintf(f, "   dexterity    = 0x%02x\n", mtDesc.dexterity12);
  fprintf(f, "  word 14 = 0x%04x \n", (UI16)(mtDesc.word14));
  fprintf(f, "    smellingDistance=%02d \n",mtDesc.smellingDistance());
  fprintf(f, "    sightDistance=%02d \n",mtDesc.sightDistance());
  fprintf(f, "  word 16 = 0x%04x",(UI16)(mtDesc.word16));
    fprintf(f," bravery = %d\n",mtDesc.bravery());
  fprintf(f, "  word 18 = 0x%04x\n",(UI16)(mtDesc.word18));
  fprintf(f, "  word 20 = 0x%04x\n",(UI16)(mtDesc.word20));
  for (j=0; j<4; j++)
      fprintf(f, "   byte22[%d] = 0x%02x\n", j, mtDesc.uByte22[j]);   
}

void DumpMonsters(FILE *f)
{
  for (i32 i=0; i<27; i++) DumpMonster(f, i, d.MonsterDescriptor[i]);
}

void DumpAttacks(FILE *f)
{
  fprintf(f,"\n\n");
  fprintf(f,"Attack  Attack      Time   Skill  Defense    Energy  Strength  Damage Prob-\n");
  fprintf(f,"Type    Name               Number                    Required         ability\n");
  for (i32 i=0; i<44; i++)
  {
    fprintf(f," %02d     %-11s   %02d     %02d     0x%02x      0x%02x      %02d       0x%02x  0x%02x\n",
                  i,                       // Attack number
                  GetAttackTypeName(i),    // Attack Name
                  (UI8)(d.experienceForAttacking[i]),   // Time
                  (UI8)(d.SkillNumber[i]), // Skill Number
                  (UI8)(d.Byte20090[i]),   // Defense
                  (UI8)(d.Byte20046[i]),   // Energy
                  (UI8)(d.Byte20002[i]),   // Strength Required
                  (UI8)(d.Byte19958[i]),   // Damage
                  (UI8)(d.Byte19914[i]));  // Probability
    if (i%5 == 4) fprintf(f,"\n");
  };
}

void DumpScroll(FILE *f, i32 indx, i32 n)
{
  fprintf(f,"Object Index %3d = Scroll   Type %3d  Object Type = %3d = %s\n",
               indx, n,
               d.ObjDesc[indx].objectType(),
               d.ObjectNames[d.ObjDesc[indx].objectType()]);
}

void DumpChest(FILE *f, i32 indx, i32 n)
{
  fprintf(f,"Object Index %3d = Chest    Type %3d  Object Type = %3d = %s\n",
               indx, n,
               d.ObjDesc[indx].objectType(),
               d.ObjectNames[d.ObjDesc[indx].objectType()]);
}

void DumpPotion(FILE *f, i32 indx, i32 n)
{
  fprintf(f,"Object Index %3d = Potion   Type %3d  Object Type = %3d = %s\n",
               indx, n,
               d.ObjDesc[indx].objectType(),
               d.ObjectNames[d.ObjDesc[indx].objectType()]);
}

void DumpWeapon(FILE *f, i32 indx, i32 n)
{
  fprintf(f,"Object Index %3d = Weapon   Type %3d  Object Type = %3d = %s\n",
               indx, n,
               d.ObjDesc[indx].objectType(),
               d.ObjectNames[d.ObjDesc[indx].objectType()]);
}

void DumpClothing(FILE *f, i32 indx, i32 n)
{
  fprintf(f,"Object Index %3d = Clothing Type %3d  Object Type = %3d = %s\n",
               indx, n,
               d.ObjDesc[indx].objectType(),
               d.ObjectNames[d.ObjDesc[indx].objectType()]);
}

void DumpClothing(FILE *f)
{
  i32 i;
  fprintf(f,"\n\nClothing\n");
  fprintf(f," type    weight   Byte1  Byte2\n");
  for (i=0; i<58; i++)
  {
    fprintf(f,     "%3d      %3d      0x%02x   0x%02x\n",
               i, 
               d.ClothingDesc[i].Weight(),
               d.ClothingDesc[i].uByte1(),
               d.ClothingDesc[i].uByte2());

  };
}

void DumpWeapons(FILE *f)
{
  i32 i;
  fprintf(f,"\n\nWeapons\n");
  fprintf(f," type, weight, Byte1 Byte2 Byte3 Word4\n");
  for (i=0; i<46; i++)
  {
    fprintf(f," %3d     %3d   0x%02x  0x%02x  0x%02x  0x%04x\n",
               i,
               d.weapons[i].weight,
               d.weapons[i].uByte1,
               d.weapons[i].uByte2,
               d.weapons[i].uByte3,
               d.weapons[i].word4   );
  };
}

void DumpMiscObject(FILE *f, i32 indx, i32 n)
{
  fprintf(f,"Object Index %3d = Misc     Type %3d  Object Type = %3d = %s\n",
               indx, n,
               d.ObjDesc[indx].objectType(),
               d.ObjectNames[d.ObjDesc[indx].objectType()]);
}

void DumpObjects(FILE *f)
{
  i32 i;
  fprintf (f, "\n\nObject table\n");
  for (i=0; i<=0; i++) DumpScroll(f, i, i-0);
  for (i=1; i<=1; i++) DumpChest(f, i, i-1);
  for (i=2; i<=22; i++) DumpPotion(f, i, i-2);
  for (i=23; i<=68; i++) DumpWeapon(f, i, i-23);
  for (i=69; i<=126; i++) DumpClothing(f, i, i-69);
  for (i=127; i<=179; i++) DumpMiscObject(f, i, i-127);
}

void OneLevelInfo(FILE *f, i32 level, LEVELDESC *pLD)
{
  CELLFLAG *pEndOfCellFlags;
  i32 numMonster;
  i32 i;
  fprintf(f, "  Width = %d, Height = %d\n",
             pLD->LastColumn()+1, pLD->LastRow()+1);
  fprintf(f, "  Experience Multiplier = %d\n",
             pLD->experienceMultiplier());
  pEndOfCellFlags = d.pppdPointer10450[level][pLD->LastColumn()];
  pEndOfCellFlags += pLD->LastRow() + 1;
  numMonster = pLD->numMonsterType();
  fprintf(f, "  Monsters allowed = ");
  for (i=0; i<numMonster; i++)
  {
    fprintf(f,"<%s>", MonsterName((MONSTERTYPE)pEndOfCellFlags[i]));
  };
  fprintf(f,"\n");
}

void LevelInfo(FILE *f)
{
  i32 i, numLevel;
  numLevel = d.dungeonDatIndex->NumLevel();
  fprintf(f,"\n\nLevel information\n");
  for (i=0; i<numLevel; i++)
  {
    fprintf(f,"Level %d\n", i);
    OneLevelInfo(f, i, &d.pLevelDescriptors[i]);
  };
  fprintf(f,"\n\n");
}

void ListHeroes(FILE *f)
{
  fprintf(f," name    hp  sta  man str dex wis vit am af\n");  
  fprintf(f,"TOADROT  125  94  46  48  58  32  31  54 54\n");
  fprintf(f,"ALGOR    150 125  40  64  41  34  46  52 58\n");
  fprintf(f,"DEMA     160 137  32  68  34  30  56  38 64\n");
  fprintf(f,"VEN      103  85  75  46  30  62  59  39 61\n");
  fprintf(f,"MANTIA   100 111  86  44  64  68  44  61 30\n");
  fprintf(f,"GNATU    102 130  75  30  68  53  41  55 38\n");
  fprintf(f,"SLOGAR   125 132  35  72  64  37  36  53 31\n");
  fprintf(f,"STING    150 135  34  69  62  34  48  64 31\n");
  fprintf(f,"PETAL    123 125  88  33  66  61  62  64 34\n");
  fprintf(f,"BUZZZZZ  140  81  90  31  69  60  62  65 38\n");
  fprintf(f,"TULA     120  72  68  39  62  56  49  56 55\n");
  fprintf(f,"ITZA     150 122  81  58  51  54  54  48 46\n");
  fprintf(f,"LANA     160 122  60  57  63  47  64  44 48\n");
  fprintf(f,"TUNDA    175 152  52  64  58  43  64  47 51\n");
  fprintf(f,"SKELAR   215 135  24  68  60  38  30  61 59\n");
  fprintf(f,"DETH     192 122  38  56  65  33  31  58 56\n");
  fprintf(f,"NECRO    188  82  64  38  52  62  32  64 60\n");
  fprintf(f,"PLAGUE   205  75  80  32  33  66  34  64 64\n");
  fprintf(f,"LETA     115 115  70  32  30  76  54  30 30\n");
  fprintf(f,"AROC     135 100  75  54  39  44  34  44 35\n");
  fprintf(f,"AIRWING  135 100  75  64  54  40  49  30 30\n");
  fprintf(f,"TALON    120 101  42  35  58  36  30  37 30\n");
}


void DumpTimer(FILE *f, HTIMER i)
{
  TIMER *pTimer = gameTimers.pTimer(i);
  //fprintf(f,"%2d",pTimer->timerTime>>24);
  fprintf(f,"%2d",pTimer->Level());
  fprintf(f," %6d %4d", pTimer->timerUByte6(), pTimer->timerUByte7());
  fprintf(f," %7d", pTimer->Time());
  fprintf(f," %3d", pTimer->Function());
  fprintf(f,"\n");
}

void PopTimer(ui16 *que, i32 i, i32 n)
{
  i32 j, k;
  j = i*2+1;
  k = j+1;
  if (j >= n) return;
  if (k >= n)
  {
    que[i] = que[j];
    PopTimer(que, j, n);
    return;
  };
  //Determine the lower and pop it.
  //if (TestTimerLess(gameTimers.pTimer(que[j]), gameTimers.pTimer(que[k]))) k=j;
  if (gameTimers.pTimer(que[j]) < gameTimers.pTimer(que[k])) k=j;
  que[i] = que[k];
  PopTimer(que, k, n);
}


void DumpTimers(FILE *f)
{
  i32 n;
  ui16 *que;
  n = gameTimers.FirstAvailTimer();
  que = (ui16 *)UI_malloc(2*n, MALLOC002);
  memcpy(que, gameTimers.m_timerQueue, 2*n);
  fprintf(f,"\n\nActive Timer Entries - Current time is %d - Number of entries = %d\n\n",
                          d.Time, n);
  fprintf(f,"level  x    y     time  function\n");
  while (n > 0)
  {
    DumpTimer(f, que[0]);
    PopTimer(que,0,n);
    n--;
  };
  UI_free(que);
}

void SorryEncrypted()
{
  UI_MessageBox("This function is disabled\n"
                "for encrypted dungeons.",
                "Sorry",
                MESSAGE_OK);
}

void DumpPalette(FILE *f, PALETTE *pal, const char *title)
{
  i32 i, r, g, b, color;
  fprintf(f,"\nPalette name = '%s'\n", title);
  fprintf(f,"            R  G  B\n");
  for (i=0; i<16; i++)
  {
    color = pal->color[i];
    r = (color>>8) & 7;
    g = (color>>4) & 7;
    b = (color>>0) & 7;
    fprintf(f, "Color %2d  %3d%3d%3d\n",i,r,g,b);
  };
}



void DumpPalettes(FILE *f)
{
  i32 special, i;
  char palettename[30];
  DumpPalette(f,&d.Palette11978,"Palette11978");
  for (i=0; i<6; i++)
  {
    sprintf(palettename, "viewport palette number %d", i);
    DumpPalette(f,&d.Palette552[i], palettename);
  };
  fprintf(f,"Special Colors (for monsters on different levels)\n");
  fprintf(f," Color   R  G  B     R  G  B     R  G  B     R  G  B     R  G  B     R  G  B\n");                 
  for (special=0; special<13; special++)
  {
    fprintf(f," %2d ", special);
    for (i=0; i<6; i++)
    {
      fprintf(f," / %3d", (LE16(d.specialColors[special][i])>>8)&7);
      fprintf(f,"%3d", (LE16(d.specialColors[special][i])>>4)&7);
      fprintf(f,"%3d", (LE16(d.specialColors[special][i])>>0)&7);
    };
    fprintf(f,"     %3d %3d", (LE16(d.specialColors[special][6])>>8) & 0xff, LE16(d.specialColors[special][6])&0xff);
    fprintf(f,"\n");
  };
  fprintf(f,"Color mapping for distance\n");
  for (i=0; i<16; i++)
  {
    fprintf(f,"Color %2d maps to %3d   %3d\n",i, (ui8)d.Byte6628[i], (ui8)d.Byte6612[i]);
  };
}

void AsciiDump()
{
  if (   (encipheredDataFile != NULL)
      || simpleEncipher)
  {
    SorryEncrypted();
    return;
  };
  i16 F=CREATE("ASCIIDUMP.TXT", "w", true);
  FILE *f = GETFILE(F);
  if (f == NULL)
  {
    UI_MessageBox("Cannot open ASCIIDUMP.TXT",NULL,MESSAGE_OK);
    return;
  };
  Info(f);
  LevelInfo(f);
  miscTable.Allocate(); 
  DumpCells(f); // Also fills miscTable;
  MarkBackpackLocations();
  MarkQueuedMonsterPossessionLocations();
  miscTable.Print(f);
  miscTable.DeAllocate();
  DumpMonsters(f);
  DumpAttacks(f);
  DumpClothing(f);
  DumpWeapons(f);
  DumpObjects(f);
  ListHeroes(f);
  DumpTimers(f);
  DumpPalettes(f);
  //DumpDB3(f);
  CLOSE(F);
}


#define ANY 0     // exclude nothing
#define KEY 1     // Cannot put keys here
#define MONSTER 2 // Monsters cannot be here

char ignoreLocations[] =
{
    9,31,36,ANY,
    7, 9,26,ANY,
    7,14,34,ANY,
    5,14,25,KEY,
    5,15,25,KEY,
    5,15,26,KEY,
    5,15,27,KEY,
    5,15,28,KEY,
    5,16,24,KEY,
    5,16,25,KEY,
    5,16,26,KEY,
    5,16,27,KEY,
    5,16,28,KEY,
    5,16,29,KEY,
    5,17,24,KEY,
    5,17,25,KEY,
    5,17,26,KEY,
    5,17,28,KEY,
    5,17,29,KEY,
    5,18,24,KEY,
    5,18,25,KEY,
    5,18,26,KEY,
    5,18,27,KEY,
    5,18,28,KEY,
    5,19,24,KEY,
    5,19,27,KEY,
    5,20,24,KEY,
    5,20,25,KEY,
    6,21,30,ANY,
    6,22,29,ANY,
    6,22,30,ANY,
    6,22,31,ANY,
    6,23,30,ANY,
    0,17,18,ANY,
    0,17,19,ANY,
    0,17,20,ANY,
    0,17,21,ANY,
    0,17,40,ANY,
};

bool Ignore(i32 level, i32 x, i32 y, i32 what, i32 mode)
{
  i32 i;
  if (mode != 0) return false;
  for (i=0; i<sizeof (ignoreLocations); i+=4)
  {
    if (ignoreLocations[i] != level) continue;
    if (ignoreLocations[i+1] != x)   continue;
    if (ignoreLocations[i+2] != y)   continue;
    if ((ignoreLocations[i+3] & what) == 0) return true;
  };
  return false;
}

i32 ListRemainingDB(i32 pass,
                    char *line,
                    RN object,
                    CELLFLAG /*cf*/,
                    i32 level,
                    i32 x, 
                    i32 y,
                    i32 mode)
{
  i32 objType;
  objType = object.dbType();
  if (objType == dbDOOR)       return 0;
  if (objType == dbTELEPORTER) return 0;
  if (objType == dbTEXT)       return 0;
  if (objType == dbACTUATOR)   return 0;
  if (objType == dbMISSILE)    return 0;
  if (objType == dbCLOUD)      return 0;
  if (objType == dbMONSTER)
  {
    DB4 *pDB4;
    MONSTERTYPE mt;
    if (pass!=2) return 0;
    if (Ignore(level,x,y,MONSTER,mode)) return 0;
    pDB4 = GetRecordAddressDB4(object);
    if (pDB4->monsterType() == mon_LordChaos) return 0;
    mt = pDB4->monsterType();
    sprintf(line,"%s",MonsterName(mt));
    return 2;
  };
  if (objType == dbWEAPON)
  {
    DB5 *pDB5;
    WEAPONTYPE wpnType;
    OBJ_NAME_INDEX objNI;
    if (pass!=1) return 0;
    if (Ignore(level,x,y,ANY,mode)) return 0;
    pDB5 = GetRecordAddressDB5(object);
    wpnType = pDB5->weaponType();
    if (!pDB5->important()) return 0;
    if (wpnType == weapon_Rock)      return 0;
    if (wpnType == weapon_StoneClub) return 0;
    objNI = object.NameIndex();
    sprintf(line,"%s ",d.ObjectNames[objNI]);
    return 1;
  };
  if (objType == dbCLOTHING)
  {
    DB6 *pDB6;
    //CLOTHINGTYPE clothingType;
    OBJ_NAME_INDEX objNI;
    if (pass!=1) return 0;
    if (Ignore(level,x,y,ANY,mode)) return 0;
    pDB6 = GetRecordAddressDB6(object);
    if (!pDB6->important()) return 0;
    objNI = object.NameIndex();
    strcpy(line, d.ObjectNames[objNI]);
    return 1;
  };
  if (objType == dbSCROLL) //scroll
  {
    if (pass!=1) return 0;
    if (Ignore(level,x,y,ANY,mode)) return 0;
    sprintf(line,"Scroll");
    return 1;
  };
  if (objType == dbPOTION) //potion
  {
    OBJ_NAME_INDEX objNI;
    DB8 *pDB8;
    if (pass!=1) return 0;
    if (Ignore(level,x,y,ANY,mode)) return 0;
    pDB8 = GetRecordAddressDB8(object);
    if (!pDB8->important()) return 0;
    objNI = object.NameIndex();
    strcpy(line, d.ObjectNames[objNI]);
    return 1;
  };
  if (objType == dbCHEST) // a chest
  {
    if (pass!=1) return 0;
    if (Ignore(level,x,y,ANY,mode)) return 0;
    sprintf(line," CHEST");
    return 3;
  };
  if (objType == dbMISC)
  {
    DB10 *pDB10;
    char *obName;
    MISCTYPE miscType;
    OBJ_NAME_INDEX objNI;
    if (pass!=1) return 0;
    if (Ignore(level,x,y,ANY,mode)) return 0;
    pDB10 = GetRecordAddressDB10(object);
    miscType = pDB10->miscType();
    if (!pDB10->important()) return 0;
    if (miscType == misc_Boulder)     return 0;
    if (miscType == misc_Cheese)      return 0;
    if (miscType == misc_Bread)       return 0;
    if (miscType == misc_DragonSteak) return 0;
    if (miscType == misc_Shank)       return 0;
    sprintf(line,"%04x",object.ConvertToInteger());
    if (pDB10->link() == RNnul) return 0;
    objNI = object.NameIndex();
    obName = d.ObjectNames[objNI];
    strcpy(line, obName);
    return 1;
  };
  sprintf(line," %04x",object.ConvertToInteger());
  return 1;
}
    
bool listRemainingDBObject(char *line, RN object)
{
  i32 objType;
  objType = object.dbType();
  if (objType == dbDOOR) return false;
  if (objType == dbTELEPORTER) return false;
  if (objType == dbTEXT) return false;
  if (objType == dbACTUATOR) return false;
  if (objType == dbMONSTER) //monster
  {
    DB4 *pDB4;
    MONSTERTYPE monsterType;
    pDB4 = GetRecordAddressDB4(object);
    monsterType = pDB4->monsterType();
    sprintf(line,"%s",MonsterName(monsterType));
    return true;
  };
  if (objType == dbWEAPON)
  {
    DB5 *pDB5;
    WEAPONTYPE wpnType;
    OBJ_NAME_INDEX objNI;
    pDB5 = GetRecordAddressDB5(object);
    wpnType = pDB5->weaponType();
    if (!pDB5->important()) return false;
    if (wpnType == weapon_Rock)      return false;
    if (wpnType == weapon_StoneClub) return false;
    objNI = object.NameIndex();
    sprintf(line,"%s ",d.ObjectNames[objNI]);
    return true;
  };
  if (objType == dbCLOTHING)
  {
    DB6 *pDB6;
    //CLOTHINGTYPE clothingType;
    OBJ_NAME_INDEX objNI;
    pDB6 = GetRecordAddressDB6(object);
    if (!pDB6->important()) return false;
    objNI = object.NameIndex();
    strcpy(line, d.ObjectNames[objNI]);
    return true;
  };
  if (objType == dbSCROLL) //scroll
  {
    sprintf(line,"Scroll");
    return true;
  };
  if (objType == dbPOTION) //potion
  {
    OBJ_NAME_INDEX objNI;
    DB8 *pDB8;
    pDB8 = GetRecordAddressDB8(object);
    if (!pDB8->important()) return false;
    objNI = object.NameIndex();
    strcpy(line, d.ObjectNames[objNI]);
    return true;
  };
  if (objType == dbCHEST) // a chest
  {
    sprintf(line," CHEST");
    return true;
  };
  if (objType == dbMISC)
  {
    DB10 *pDB10;
    char *obName;
    MISCTYPE miscType;
    OBJ_NAME_INDEX objNI;
    pDB10 = GetRecordAddressDB10(object);
    miscType = pDB10->miscType();
    if (!pDB10->important()) return false;
    if (miscType == misc_Boulder)     return false;
    if (miscType == misc_Cheese)      return false;
    if (miscType == misc_Bread)       return false;
    if (miscType == misc_DragonSteak) return false;
    if (miscType == misc_Shank)       return false;
    sprintf(line,"%04x",object.ConvertToInteger());
    if (pDB10->link() == RNnul) return false;
    objNI = object.NameIndex();
    obName = d.ObjectNames[objNI];
    strcpy(line, obName);
    return true;
  };
  sprintf(line," %04x",object.ConvertToInteger());
  return true;
}

void ItemsRemaining(i32 mode)
{//Mode 0 = CSB Challenge mode
 //Mode 1 = Print all remaining items
  i32 level, numLevel, saveLoadedLevel;
  i32 numCorbum, totalLine, answer=MESSAGE_IDYES;
  i32 i, chIdx;
  CELLFLAG cf;
  ROOMTYPE rt;
  i32 x, y, pass, n;
  RN object, possession, containedObject;
  DB10 *pDB10;
  DB9 *pDB9;
  MISCTYPE miscType;
  LISTING listing;
  char line[80];
  char itemName[40];
  if (   (encipheredDataFile != NULL)
      || simpleEncipher)
  {
    SorryEncrypted();
    return;
  };
  if (mode == 0)
  {
    UI_MessageBox("1-All important items must be either in\n"
                  "     your possession or stored in the\n"
                  "     room around 05(17,26)\n"
                  "2-You must be standing in front of the FulYa pit\n"
                  "3-You must have four corbum in your possession",
                  "How to Win",MESSAGE_OK);
  };
  saveLoadedLevel = d.LoadedLevel;
  numLevel = d.dungeonDatIndex->NumLevel();
  totalLine = 0;
  if (mode == 0)
  {
    if (   (d.partyLevel!= 0)
        || (d.partyX != 20-d.pLevelDescriptors[0].offsetX)
        || (d.partyY != 38-d.pLevelDescriptors[0].offsetY) )
    {
      listing.AddLine("You are not in front of FulYa Pit!");
      totalLine++;
    };
  };
  numCorbum = 0;
  for (chIdx = 0; chIdx < d.NumCharacter; chIdx++)
  {
    for (i=0; i<30; i++)
    {
      possession = d.CH16482[chIdx].Possession(i);
      if (possession == RNnul) continue;
      if (possession.dbType()==dbMISC)
      {
        pDB10 = GetRecordAddressDB10(possession);
        miscType = pDB10->miscType();
        if (miscType == misc_Corbum)
        {
          numCorbum++;
        };
      };
      if (possession.dbType() == dbCHEST)
      {
        pDB9 = GetRecordAddressDB9(possession);
        containedObject = pDB9->contents();
        while (containedObject != RNeof)
        {
          if (containedObject.dbType() == dbMISC)
          {
            pDB10 = GetRecordAddressDB10(containedObject);
            miscType = pDB10->miscType();
            if (miscType == misc_Corbum)
            {
              numCorbum++;
            };
          };
          containedObject = GetDBRecordLink(containedObject);
        };
      };
    };
  };
  if (mode == 0)
  {
    if (numCorbum != 4)
    {
      listing.AddLine("You do not have the four Corbum!");
      totalLine++;
    };
  };
  for (pass=1; pass<=2; pass++)
  {
    for (level=numLevel-1; level>=0; level--)
    {
      LoadLevel(level);
      for (x=0; x<d.width; x++)
      {
        for (y=0; y<d.height; y++)
        {
          cf = GetCellFlags(x,y);
          rt = (ROOMTYPE)(cf>>5);
          object=FindFirstObject(x, y);
          while (object != RNeof)
          {
            n = ListRemainingDB(pass,
                                itemName, 
                                object, cf, level, 
                                x + d.pLevelDescriptors[level].offsetX,
                                y + d.pLevelDescriptors[level].offsetY,
                                mode);
            if (n > 0)
            {
              printLocation(line,level,
                            x + d.pLevelDescriptors[level].offsetX,
                            y + d.pLevelDescriptors[level].offsetY);
              sprintf(line+strlen(line),"%d",object.pos());
              sprintf(line+strlen(line)," %s",itemName);
              listing.AddLine(line);
              totalLine++;
              if (n==2)
              { //A monster!
                DB4 *pMon;
                RN  poss;
                // for each item in monster's possession
                pMon = GetRecordAddressDB4(object);
                poss = pMon->possession();
                for (poss = pMon->possession();
                     poss != RNeof;
                     poss = GetDBRecordLink(poss))
                {
                  if (listRemainingDBObject(itemName, poss))
                  {
                    sprintf(line,"  %s",itemName);
                    listing.AddLine(line);
                    totalLine++;
                  };
                };
              };
              if (n==3)
              { //A chest!
                DB9 *pDB9a;
                RN  contents;
                // for each item in the chest
                pDB9a = GetRecordAddressDB9(object);
                containedObject = pDB9a->contents();
                while (containedObject != RNeof)
                {
                  if (listRemainingDBObject(itemName, containedObject))
                  {
                    sprintf(line,"  %s",itemName);
                    listing.AddLine(line);
                    totalLine++;
                  };
                  containedObject = GetDBRecordLink(containedObject);
                };
              };
            };
/*
            if (numLine >= MAXLINE)
            {
              sprintf(listing+strlen(listing),
                      "Would you like to see more?");
              answer = UI_MessageBox(listing, 
                                     "Remaining tasks", 
                                     MESSAGE_YESNO);
              listing[0] = 0;
              totalLine += numLine;
              numLine = 0;
              if (answer == MESSAGE_IDNO)
              {
                x=1000; y=1000; level=0; break;
              };
            };
*/
            object=GetDBRecordLink(object);
          };
        };
      };
    };
    if (answer == MESSAGE_IDNO) break;
  };
  if (totalLine > 0)
  {
    listing.DisplayList("Remaining tasks");
  };
  if (mode == 0)
  {
    if (totalLine == 0)
    {
      UI_MessageBox("*** CONGRATULATIONS ****",
                    "You Win",
                    MESSAGE_OK);
    };
  };
  LoadLevel(saveLoadedLevel);
}
