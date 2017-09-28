#include "stdafx.h"
#include <stdio.h>

#include "UI.h"

//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"
#include "Data.h"

const int SDListSize = 200; // 1600 bytes.  Not much by today's standard.

// A multipler to decide at what point to begin deletes.
#define FRACTION_LIMIT 3/4

static i32 currentDBNum;
static i32 currentLevel;
static i32 currentX;
static i32 currentY;
static i32 currentIndex;
static i32 discardFailure;//consecutive discards that failed
static enum {
  ModeNotActive,
  ModeNewDB,  //currentDB is the new DB to process
  ModeCount,  //count number of each type of object.
  ModeScan,   //scan dungeon for objects; put in priority-sorted list
  ModeDiscard //discard objects based on priority
} currentMode = ModeNotActive;

//struct SDCounts
//{
//  i32 count;
//  i32 unimportant;
//};

//SDCounts objectCount[180];; //how many of each exists./ how many non-important
static i32 totalCount;

static FILE *m_trace;

class SDOBJ
{
  friend class SDOBJLIST;
  ui32 m_object;
public:
  ui32 m_priority;
  void Get(i32 *level, i32 *x, i32 *y, RN *obj);
  SDOBJ();
  SDOBJ(i32 level, i32 x, i32 y, RN *obj, ui32 priority);
};

SDOBJ::SDOBJ(i32 level, i32 x, i32 y, RN *obj, ui32 priority)
{
  m_object = (((((level<<5)+x)<<5)+y)<<16)+obj->ConvertToInteger();
  m_priority = priority;
}

SDOBJ::SDOBJ()
{
  RN obj;
  m_object = obj.ConvertToInteger();
  m_priority = 0;
}

void SDOBJ::Get(i32 *level, i32 *x, i32 *y, RN *obj)
{
  *level    = (m_object >> 26) & 0x3f;
  *x        = (m_object >> 21) & 0x1f;
  *y        = (m_object >> 16) & 0x1f;
  obj->ConstructFromInteger((m_object >>  0) & 0xffff);
}


/*
static float ComputePriority(SDOBJ *pSDobj)
{
  i32 level, x, y, objIndex;
  SDENT *psdEnt;
  float priority;
  pSDobj->Get(&level, &x, &y, &objIndex);
  psdEnt = &sdTable[objIndex];
  priority = psdEnt->y_intercept;
  priority += float(STRandom(100000))/100000; // 0.0 to 0.99999
  if (m_trace)
  {
    fprintf(m_trace,"ComputePriority for SmartDiscard priority = %f",
             priority);
  };
  if (abs(level - d.partyLevel) > 1)
  {
    priority += psdEnt->level;
    if (m_trace)
    {
      fprintf(m_trace," add level=%f",priority);
    };
  };
  if (objcount[objIndex] > psdEnt->minimum)
  {
    priority += psdEnt->slope * (objcount[objIndex]-psdEnt->minimum);
    if (m_trace)
    {
      fprintf(m_trace," add slope=%f",priority);
    };
  }
  else
  {
    priority = -1.0f;
    if (m_trace)
    {
      fprintf(m_trace," set to %f",priority);
    };
  };
  if (level == d.partyLevel) priority = -1.0f;
  if (m_trace)
  {
    fprintf(m_trace," party level so %f",priority);
  };
  if (m_trace)
  {
    fprintf(m_trace,"\n");
  };
  return priority;
}
*/


class SDOBJLIST
{
  i32   m_numEntry;
  SDOBJ m_sdobj[SDListSize];
  // We made the entries larger (to include a priority) so
  // it seemed wise to limit the size of this list.
  // The insertion function discards entries that will replace
  // a higher-priority entry.
  //ancientComment//old comment//We make the list larger than 1023 because it is built
  //ancientComment//old comment//over a period of minutes and objects can come and go and
  //ancientComment//old comment//move around and get listed twice and all sorts of bad
  //ancientComment//old comment//things.  We do not rely on 1050 being enough; we always
  //ancientComment//old comment//check when adding another object that room exists.  We
  //ancientComment//old comment//ignore objects if they don't fit.
public:
  SDOBJLIST() {m_numEntry = 0;};
  void Clear() {m_numEntry = 0;};
  void AddObjectIncr(SDOBJ *pSDobj);
  void AddObjectDecr(SDOBJ *pSDobj);
  bool DeleteObjectIncr(SDOBJ *pSDobj);
  bool DeleteObjectDecr(SDOBJ *pSDobj);
  bool SearchStillAvailable(SDOBJ *pSDobj);
  void Reverse(); // Invert so largest is first.
//  float TopPriority() {return ComputePriority(&m_sdobj[0]);};
};

void SDOBJLIST::AddObjectDecr(SDOBJ *pSDobj)
{ // 
//  float priority, parentPriority;
  i32 index, parent;
  if (m_numEntry >= SDListSize) return; //ignore it.
//  priority = ComputePriority(pSDobj);
  index = m_numEntry;
  m_numEntry++;
  while (index != 0)
  {
    parent = (index-1)/2;
//    parentPriority = ComputePriority(&m_sdobj[parent]);
//    if (parentPriority >= priority) break;
    if (m_sdobj[parent].m_priority >= pSDobj->m_priority) break;
    m_sdobj[index] = m_sdobj[parent];
    index = parent;
  };
  m_sdobj[index] = *pSDobj;
}

void SDOBJLIST::AddObjectIncr(SDOBJ *pSDobj)
{ // 
//  float priority, parentPriority;
  i32 index, parent;
  if (m_numEntry >= SDListSize)
  {
    if (pSDobj->m_priority > m_sdobj[0].m_priority)
    {
      this->DeleteObjectIncr(NULL);
    }
    else
    {
      return; //ignore it.
    };
  };
//  priority = ComputePriority(pSDobj);
  index = m_numEntry;
  m_numEntry++;
  while (index != 0)
  {
    parent = (index-1)/2;
//    parentPriority = ComputePriority(&m_sdobj[parent]);
//    if (parentPriority >= priority) break;
    if (m_sdobj[parent].m_priority <= pSDobj->m_priority) break;
    m_sdobj[index] = m_sdobj[parent];
    index = parent;
  };
  m_sdobj[index] = *pSDobj;
}

bool SDOBJLIST::DeleteObjectIncr(SDOBJ *pSDobj)
{ // Deletes the topmost object
  i32 index, c1Index, c2Index, parent;
  ui32 priority;
//  float priority, parentPriority, c1Priority, c2Priority;
  ASSERT(m_numEntry >= 0,"numEntry");
  if (m_numEntry == 0) return false;
  if (pSDobj != NULL)
  {
    *pSDobj = m_sdobj[0];
  };
  index = 0; // entry to be replaced.
  while (index < m_numEntry)
  {
    c1Index = 2*index+1;
    c2Index = c1Index+1;
    if (c1Index >= m_numEntry) break;
    if (c2Index >= m_numEntry)
    {
      m_sdobj[index] = m_sdobj[c1Index];
      m_numEntry--;
      return true;
    };
//    c1Priority = ComputePriority(&m_sdobj[c1Index]);
//    c2Priority = ComputePriority(&m_sdobj[c2Index]);
//    if (c1Priority > c2Priority)
//    {
//      m_sdobj[index] = m_sdobj[c1Index];
//      index = c1Index;
//    }
//    else
    if (m_sdobj[c1Index].m_priority < m_sdobj[c2Index].m_priority)
    {
      m_sdobj[index] = m_sdobj[c1Index];
      index = c1Index;
    }
    else
    {
      m_sdobj[index] = m_sdobj[c2Index];
      index = c2Index;
    };
  };
  m_numEntry--;
  if (index == m_numEntry) return true;
//  priority = ComputePriority(&m_sdobj[m_numEntry]);
  priority = m_sdobj[m_numEntry].m_priority;
  while (index != 0)
  {
    parent = (index-1)/2;
//    parentPriority = ComputePriority(&m_sdobj[parent]);
//    if (parentPriority >= priority) break;
    if (m_sdobj[parent].m_priority <= priority) break;
    m_sdobj[index] = m_sdobj[parent];
    index = parent;
  };
  m_sdobj[index] = m_sdobj[m_numEntry];
  return true;
}

bool SDOBJLIST::DeleteObjectDecr(SDOBJ *pSDobj)
{ // Deletes the topmost object
  i32 index, c1Index, c2Index, parent;
  ui32 priority;
//  float priority, parentPriority, c1Priority, c2Priority;
  ASSERT(m_numEntry >= 0,"numEntry");
  if (m_numEntry == 0) return false;
  if (pSDobj != NULL)
  {
    *pSDobj = m_sdobj[0];
  };
  index = 0; // entry to be replaced.
  while (index < m_numEntry)
  {
    c1Index = 2*index+1;
    c2Index = c1Index+1;
    if (c1Index >= m_numEntry) break;
    if (c2Index >= m_numEntry)
    {
      m_sdobj[index] = m_sdobj[c1Index];
      m_numEntry--;
      return true;
    };
//    c1Priority = ComputePriority(&m_sdobj[c1Index]);
//    c2Priority = ComputePriority(&m_sdobj[c2Index]);
//    if (c1Priority > c2Priority)
//    {
//      m_sdobj[index] = m_sdobj[c1Index];
//      index = c1Index;
//    }
//    else
    if (m_sdobj[c1Index].m_priority > m_sdobj[c2Index].m_priority)
    {
      m_sdobj[index] = m_sdobj[c1Index];
      index = c1Index;
    }
    else
    {
      m_sdobj[index] = m_sdobj[c2Index];
      index = c2Index;
    };
  };
  m_numEntry--;
  if (index == m_numEntry) return true;
//  priority = ComputePriority(&m_sdobj[m_numEntry]);
  priority = m_sdobj[m_numEntry].m_priority;
  while (index != 0)
  {
    parent = (index-1)/2;
//    parentPriority = ComputePriority(&m_sdobj[parent]);
//    if (parentPriority >= priority) break;
    if (m_sdobj[parent].m_priority >= priority) break;
    m_sdobj[index] = m_sdobj[parent];
    index = parent;
  };
  m_sdobj[index] = m_sdobj[m_numEntry];
  return true;
}

void SDOBJLIST::Reverse()
{ // Reverse the list to be in 'high priority first' order
  int num, high, low;
  SDOBJ temp;
  num = m_numEntry;
  high = num-1;
  low = 0;
  m_numEntry = 0;
  while (high > low)
  {
    temp = m_sdobj[low];
    AddObjectDecr(&m_sdobj[high]);
    m_sdobj[high] = temp;
    high--;
    low++;
  };
  while (low < num)
  {
    temp = m_sdobj[low];
    AddObjectDecr(&temp);
    low++;
  };
}

bool SDOBJLIST::SearchStillAvailable(SDOBJ *pSDobj)
{
  int i;
  for (i=0; i<m_numEntry; i++)
  {
    if (this->m_sdobj[i].m_object == pSDobj->m_object)
    {
      return m_sdobj[i].m_priority >= pSDobj->m_priority;
    };
  };
  return false;
}

static SDOBJLIST objlist;

CELLFLAG *GetCellFlagsAddress(i32 level, i32 x, i32 y)
{
  return &d.pppdPointer10450[level][x][y];
}
  
CELLFLAG *GetCellFlagsAddress(const LOCATIONREL& locr)
{
  return &d.pppdPointer10450[locr.l][locr.x][locr.y];
}
  

RN FindFirstObject(i32 level, i32 x, i32 y)
{
//  TAG009a1e
//i16 GetObjectListIndex(i32 mapX,i32 mapY)
//{ // Index in ??? of the object-list for this cell.
  CELLFLAG *pCF;
  i32 i, index, levIndex;
  pCF = d.pppdPointer10450[level][x];
  if (x < 0) return RNeof;
  if (x > d.pLevelDescriptors[level].LastColumn()) return RNeof;
  if (y < 0) return RNeof;
  if (y > d.pLevelDescriptors[level].LastRow()) return RNeof;
  if ((pCF[y] & 16) == 0) return RNeof; // Nothing here!
  levIndex = d.objectLevelIndex[level]; // First column in level
  index = d.objectListIndex[levIndex + x];
  for (i=0; i<y; i++)
  {
    if ((pCF[i] & 16) != 0) index++;
  };
  return d.objectList[index];  
}

SRCHPKT *SearchForObject(RN objID)
{
  static SRCHPKT srchPkt;
  RN *pRN, obj;
  ui32 len;
  ui32 i;
  i32 index, numLev, level, levIndex;
  i32 col0Index, lastCol, col, colIndex;
  i32 numRow, row;
  len = d.dungeonDatIndex->ObjectListLength();
  for (i=0, pRN=d.objectList.data(); i<len; i++,pRN++)
  {
    if (*pRN == objID)
    {
      srchPkt.place = PLACE_InDungeon;
      index = pRN-d.objectList.data();
      numLev = d.dungeonDatIndex->NumLevel();
      for (level=numLev-1; level>=0; level--)
      {
        levIndex = d.objectLevelIndex[level]; // First column in level
        col0Index = d.objectListIndex[levIndex + 0];
        if (index >= col0Index) break;
      };
      if (level < 0) return NULL;
      lastCol = d.pLevelDescriptors[level].LastColumn();
      for (col=lastCol; col>=0; col--)
      {
        levIndex = d.objectLevelIndex[level]; // First column in level
        colIndex = d.objectListIndex[levIndex + col];
        if (index >= colIndex) break;
      };
      if (col < 0) return NULL;
      numRow = d.pLevelDescriptors[level].LastRow()-1;
      for (row=0; row<numRow; row++)
      {
        for (obj=FindFirstObject(level,col,row);
             obj!=RNeof;
             obj=GetDBRecordLink(obj))
        {
          if (obj ==objID)
          {
            srchPkt.level = level;
            srchPkt.x     = col;
            srchPkt.y     = row;
            return &srchPkt;
          };
        };
      };
      return NULL;
    };
  };
  return NULL;
};


RN FindFirstObject(const LOCATIONREL& locr)
{
  return FindFirstObject(locr.l, locr.x, locr.y);
}


int ScanOneCell(SDOBJLIST *pSDObjList)
{
  DB3  *pActuatorDB;
  DB5  *pWeaponDB;
  DB6  *pClothingDB;
  DB10 *pMiscDB;
  RN obj;
  SDOBJ sdobj;
  int result, pos;
  ui32 priority;
  bool pileExists = false;
  ui16 pileSize[_max((int)weapon_numTypes, _max((int)clothing_numTypes, (int)misc_numTypes))][4];
  bool isTriggered = false;
  result = 0;
  for (obj = FindFirstObject(currentLevel, currentX, currentY);
       obj != RNeof;
       obj = GetDBRecordLink(obj))
  {
    result++;
    // Why is this test here?  If a non-disabled actuator is
    // in the list then we ignore the rest of the list.  What is 
    // about????  
    // Perhaps we don't want to remove an object that is currently
    // keeping some sort of actuator active?  I suppost that sounds
    // reasonable.  How can we get around this problem.  I have a
    // case where there are 7000 BOULDERS on an actuator!!!!
    // We could:
    // 1) Check to see if the object is triggering the actuator.
    // 2) We could keep track of the object types on the actuator.
    //    One is enough!
    // 3) Ignore actuators that are not triggered by objects.
    // 4) What else?
    // Well, we have to try something.  I will ignore actuators
    // that clearly require no triggering (AND gates, DSA, and such)
    // and I will keep track of the kinds of objects we have encountered
    // so that we are forced to leave only one of each type on the 
    // actuator.
    if (!isTriggered && (obj.dbType() == dbACTUATOR))
    {
      pActuatorDB = GetRecordAddressDB3(obj);
      switch (pActuatorDB->actuatorType())
      {
      case 0: break;  // Disabled
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9: 
      case 16:  // Swap Object
              isTriggered = true;
              break;  // Wall switch / PressurePad
      };
    };
    if (obj.dbNum() != currentDBNum) continue;
    if (!pileExists)
    {
      pileExists = true;
      memset(pileSize,0,sizeof(pileSize));
    };
    pos = obj.pos();
    if (currentLevel != d.partyLevel)
    {
      priority = 20 + abs(currentLevel - d.partyLevel);
    }
    else
    {
      priority = abs(currentX-d.partyX) + abs(currentY-d.partyY);
    };      
    switch (currentDBNum)
    {
    case dbCLOTHING:
      pClothingDB = GetRecordAddressDB6(obj);
      if (m_trace!=NULL)
      {
        fprintf(m_trace,"scan clothing item \n");
      };
      {
        int idx;
        idx = pClothingDB->clothingType();
        pileSize[idx][pos]++;
        priority += pileSize[idx][pos];
        if (pileSize[idx][pos] > 20)
        {
          priority += 10*(pileSize[idx][pos] - 20);
        };
        if (isTriggered)
        {
          if (pileSize[idx][pos] == 1)break;
        };
        if (pClothingDB->important()) break;
        sdobj = SDOBJ(currentLevel,
                      currentX,
                      currentY,
                      &obj,
                      priority); //idx);
      };
      pSDObjList->AddObjectIncr(&sdobj);
      break;
    
    
    
    
    case dbWEAPON:
      pWeaponDB = GetRecordAddressDB5(obj);
      if (m_trace!=NULL)
      {
        fprintf(m_trace,"scan weapon item \n");
      };
      {
        int idx;
        idx = pWeaponDB->weaponType();
        pileSize[idx][pos]++;
        priority += pileSize[idx][pos];
        if (pileSize[idx][pos] > 20)
        {
          priority += pileSize[idx][pos] - 20;
        };
        if (isTriggered)
        {
          if (pileSize[idx][pos] == 1)break;
        };
        if (pWeaponDB->important()) break;
        sdobj = SDOBJ(currentLevel,
                      currentX,
                      currentY,
                      &obj,
                      priority); //idx);
      };
      pSDObjList->AddObjectIncr(&sdobj);
      break;
    case dbMISC:
      pMiscDB = GetRecordAddressDB10(obj);
      if (m_trace!=NULL)
      {
        fprintf(m_trace,"scan misc item \n");
      };
      {
        int idx;
        idx = pMiscDB->miscType();
        pileSize[idx][pos]++;
        priority += pileSize[idx][pos];
        if (pileSize[idx][pos] > 20)
        {
          priority += pileSize[idx][pos] - 20;
        };
        if (isTriggered)
        {
          if (pileSize[idx][pos] == 1)break;
        };
        if (pMiscDB->important()) break;
        sdobj = SDOBJ(currentLevel,
                      currentX,
                      currentY,
                      &obj,
                      priority); //idx);
      };
      pSDObjList->AddObjectIncr(&sdobj);
      break;
    };
  };
  return result;
}


static int ScanDungeon()
{
  //Return value is number of objects examined.
  //Return value is 99999 if scan is complete.
  i32 result;
  i32 numLevel, levelWidth, levelHeight;
  numLevel = d.dungeonDatIndex->NumLevel();
  if (currentLevel >= numLevel)
  {
    currentMode = ModeDiscard;
    objlist.Reverse();  // Invert so largest is first.
    discardFailure = 0;
    if (m_trace!=NULL)
    {
      fprintf(m_trace,"switch to Discard Mode\n");
    };
    return 99999;
  };
  levelWidth = d.pLevelDescriptors[currentLevel].LastColumn() + 1;
  if (currentX >= levelWidth)
  {
    currentX = 0;
    currentLevel++;
    if (m_trace!=NULL)fprintf(m_trace,"set level to %02x\n",currentLevel);
    return 0;
  };
  levelHeight = d.pLevelDescriptors[currentLevel].LastRow() + 1;
  if (currentY >= levelHeight)
  {
    currentX++;
    currentY = 0;
    if (m_trace!=NULL)fprintf(m_trace,"set X to %02x\n",currentX);
    return 0;
  };
  result = ScanOneCell(&objlist);
  currentY++;
  if (m_trace!=NULL)fprintf(m_trace,"SmartDiscard set Y to %02x\n",currentY);
  return result;
}

static void DiscardObject()
{
//  float priority;
  i32 oldLevel;
  SDOBJ sdobj;
  SDOBJLIST checkList;
  RN obj, discardObj;
  if (!objlist.DeleteObjectDecr(&sdobj))
  {
    currentMode = ModeNewDB;
    return;
  };
//  priority = ComputePriority(&sdobj);
//  if (   (priority < 0)
//      || (priority < objlist.TopPriority()))
      
//  {
//    discardFailure++;
//    objlist.AddObject(&sdobj);
//    if (discardFailure > 500) currentMode = ModeNewDB;
//    return;
//  };


  
  sdobj.Get(&currentLevel, &currentX, &currentY, &discardObj);
/*  for (obj = FindFirstObject(level, x, y);
       obj != RNeof;
       obj = GetDBRecordLink(obj))
  {
    if (obj.dbNum() != currentDBNum) continue;
    switch(currentDBNum)
    {
    case dbMISC:
      index = GetRecordAddressDB10(obj)->miscType() + 127;
      break;
    case dbWEAPON:
      index = GetRecordAddressDB5(obj)->weaponType() + 23;
      break;
    case dbCLOTHING:
      index = GetRecordAddressDB6(obj)->clothingType() + 69;
      break;
    default:
      UI_MessageBox("Unknown DB type in SD",
                    "Warning",
                    MESSAGE_OK | MESSAGE_ICONWARNING);
      currentMode = ModeNotActive;
      return;
    };
*/
  ScanOneCell(&checkList);
  if (checkList.SearchStillAvailable(&sdobj))
  {
  // if (obj != discardObj) continue;
  // It appears we can discard this object.
    oldLevel = d.LoadedLevel;
    LoadLevel(currentLevel);
    MoveObject(discardObj, currentX, currentY, -1, 0, NULL, NULL);
    //db.GetCommonAddress(obj)->link(RNnul);
    DeleteDBEntry(db.GetCommonAddress(discardObj));
    LoadLevel(oldLevel);
    discardFailure = 0;
    totalCount--;
    return;
  };
  discardFailure++;
  totalCount--;
  return;

  
}

void SmartDiscard(bool Initialize)
{
  i32 i;
  i32 repeat;
  DBTYPE newType;
  DBCOMMON *pCommon;
  i32 objIndex;
  if (m_trace!=NULL)
  {
    fprintf(GETFILE(TraceFile),"SmartDiscard ");
  };
  if (Initialize)
  {
    if (m_trace!=NULL)
    {
      fprintf(m_trace,"Initialize\n");
    };
    currentDBNum = 10;
    currentMode = ModeNewDB;
    return;
  };
  switch (currentMode)
  {
  case ModeNotActive: 
    if (m_trace!=NULL) fprintf(m_trace,"not active\n");
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"SmartDiscard - Not Active\n");
    };
    return;
  case ModeNewDB:
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"SmartDiscard - NewDB\n");
    };
    for (i=currentDBNum+1; i<16; i++)
    {
      //if (dataMap[i] == dataMap[currentDBNum]) break;
      if (i == currentDBNum) break;
    };
    if (i > 15)
    {
      switch (currentDBNum)
      {
      case dbMISC:     newType = dbWEAPON;   break;
      case dbWEAPON:   newType = dbCLOTHING; break;
      case dbCLOTHING: newType = dbMISC;     break;
      default: UI_MessageBox("Illegal DB",
                             "Warning",
                             MESSAGE_OK | MESSAGE_ICONWARNING);
               currentMode = ModeNotActive;
               return;
      };
      for (i=0; i<16; i++)
      {
        if (i == newType) break;
      };
    };
    currentDBNum = i;
    if (m_trace!=NULL) fprintf(m_trace,"set DB to %d\n",currentDBNum);
//    memset(objectCount, 0, 180 * sizeof (SDCounts));
    totalCount = 0;
    currentIndex = 0;
    currentMode = ModeCount;
    return;
  case ModeCount:
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"SmartDiscard - Mode Count\n");
    };
    for (repeat=0; repeat<50; repeat++)
    {
      bool important;
      if (currentIndex >= db.NumEntry(currentDBNum))
      { // We have finished counting
        int max;
        max = db.Max(currentDBNum)*FRACTION_LIMIT;
        if (totalCount < max)
        {
          currentMode = ModeNewDB;
          return;
        };
        currentLevel = 0;
        currentX = 0;
        currentY = 0;
        objlist.Clear(); // Clear list of prioritized objects
        currentMode = ModeScan;
        if (m_trace!=0)
        {
          fprintf(m_trace,"switch to Scan Mode\n");
        };
        return;
      };
      pCommon = db.GetCommonAddress(DBTYPE(currentDBNum),currentIndex);
      currentIndex++;
      if (pCommon->link() == RNnul) continue;
      switch (currentDBNum)
      {
      case dbMISC:
        {
          DB10 *pDB10;
          pDB10 = pCommon->CastToDB10();
          objIndex = pDB10->miscType() + 127;
          important = pDB10->important();
          if (m_trace!=NULL)fprintf(m_trace,"count MISC ");
        };
        break;
      case dbWEAPON:
        {
          DB5 *pDB5;
          pDB5 = pCommon->CastToDB5();
          objIndex = pDB5->weaponType() + 23;
          important = pDB5->important();
          if (m_trace!=NULL)fprintf(m_trace,"count WEAPON ");
        };
        break;
      case dbCLOTHING:
        {
          DB6 *pDB6;
          pDB6 = pCommon->CastToDB6();
          objIndex = pDB6->clothingType() + 69;
          important = pDB6->important();
          if (m_trace!=NULL)fprintf(m_trace,"count CLOTHING ");
        };
        break;
      default:
        UI_MessageBox("SmartDiscard Unknown DB Type",
                      "Warning",
                      MESSAGE_OK | MESSAGE_ICONWARNING);
        currentMode = ModeNotActive;
        return;
      };
      ASSERT(objIndex < 180,"objIndex");
//      objectCount[objIndex].count++;
//      if (!important) objectCount[objIndex].unimportant++;
      totalCount++;
//      if (m_trace!=NULL)
//      {
//        fprintf(m_trace,"index %02x count = %04x total=%04x\n",
//                      objIndex, objectCount[objIndex].count, totalCount);
//      };
    }; //repeat
    return;
  case ModeScan:
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"SmartDiscard - ModeScan\n");
    };
    for (repeat=50; repeat>0; repeat-=ScanDungeon());
    return;
  case ModeDiscard:
    if (TimerTraceActive)
    {
      fprintf(GETFILE(TraceFile),"SmartDiscard - ModeDiscard\n");
    };
    if (totalCount < db.Max(currentDBNum)*FRACTION_LIMIT)
    {
      currentMode = ModeNewDB;
      if (m_trace!=NULL)
      {
        fprintf(m_trace,"fewer than maximum. Set mode to NewDB\n");
      };
      return;
    };
    DiscardObject();
    if (m_trace!=NULL) fprintf(m_trace,"Discard an object\n");
    return;
  };
  UI_MessageBox("Unknown state in SmartDiscard",
                "Warning",
                MESSAGE_OK | MESSAGE_ICONWARNING);
  currentMode = ModeNotActive;
}

struct INTERIMTABLE
{
  const char *name;
  i32   num;
};

/*
INTERIMTABLE interimTable[] = 
{
  {"DRAGON STEAK" , 0xaf},
  {"BOULDER"      , 0x80},
  {"FOOT PLATE"   , 0x74},
  {"LEG PLATE"    , 0x5d},
  {"TORSO PLATE"  , 0x4d},
  {"ARMET"        , 0x64},
  {"STONE CLUB"   , 0x30},
  {"CLUB"         , 0x2f},
  {"DAGGER"       , 0x20},
  {"SWORD"        , 0x22},
  {"ROCK"         , 0x36},
  {"WOODEN SHIELD", 0x6c},
  {"FALCHION"     , 0x21}
};
*/
/*
void BuildSmartDiscardTable()
{
  //Back in the ole days there was a limit of 1020 items
  //in a database and it was critical to discard the 
  //proper ones when space was needed.  Now we have a limit
  //of a total of 65000 objects and we will try to get by
  //without discarding any of them for now.
  //If we reinstate this algorithm we will have to revise
  //it somehow to allow for renamed objects.  Perhaps the
  //designer can tell us how to do it.
  //
  //I am going to build a local table with the object names
  //and numbers.  That will get us by for now. PRS 22Nov2003
  SD *psd;
  i32 i, j, oType;
//  for (i=0; i<199; i++) sdTable[i].minimum = -1;
  for (psd = sdList.First(); psd!=NULL; psd = sdList.Next(psd))
  {
    //Look through the object names to find the object type
    //for this SD entry
    for (i=0; i<sizeof(interimTable)/sizeof(interimTable[0]); i++)
    {
      if (strcmp(psd->name, interimTable[i].name) == 0)
      {
        oType = interimTable[i].num;
        //oType = object type.  Now we need to look through
        //the object table to find the index of an entry
        //with this object type.
        for (j=0; j<180; j++)
        {
          if (d.ObjDesc[j].objectType() == oType) break;
        };
        if (j==180) j=0; //scroll...we don't use it
//        sdTable[j].minimum     = psd->minimum;
//        sdTable[j].level       = psd->level;
//        sdTable[j].y_intercept = psd->y_intercept;
//        sdTable[j].slope       = psd->slope;
        break;
      };
    };
    if (i < sizeof(interimTable)/sizeof(interimTable[0])) continue;
    if (strcmp(psd->name, "DEFAULT") == 0)
    {
      for (i=0; i<199; i++)
      {
//        if (sdTable[i].minimum == -1)
        {
//          sdTable[i].minimum     = psd->minimum;
//          sdTable[i].level       = psd->level;
//          sdTable[i].y_intercept = psd->y_intercept;
//          sdTable[i].slope       = psd->slope;
        };
      };
      continue;
    };
    UI_MessageBox(psd->name,
                  "Unknown SmartDiscard Object",
                  MESSAGE_OK | MESSAGE_ICONWARNING);
  };
  //currentDB = dbMISC; //
  //currentMode = ModeNewDB;
  SmartDiscard(true);
}
*/

void SmartDiscardTrace(FILE *f) 
{
  m_trace = f;
}