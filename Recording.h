

#ifdef CPP_LIST

#include <list>
#include <string>

#else

class LQENTRY
{
  friend class LINE_QUEUE;
  LQENTRY *m_pNext;
  char m_line[1];
};


class LINE_QUEUE
{
  LQENTRY *m_pFirst;
public:
  LINE_QUEUE(void){m_pFirst = NULL;};
  LQENTRY *GetHeadPosition(void){return m_pFirst;};
  char *GetNext(LQENTRY* &pos){LQENTRY* curr=pos; pos=curr->m_pNext; return curr->m_line;};
  void clear(void)
  {
    while (m_pFirst != NULL)
    {
      LQENTRY *first = m_pFirst;
      m_pFirst = m_pFirst->m_pNext;
      UI_free(first);
    };
  };
  void push_back(const char *line)
  {
    LQENTRY *newEnt, *last;
    newEnt = (LQENTRY *)UI_malloc(sizeof(LQENTRY) + strlen(line), MALLOC124);
    newEnt->m_pNext = NULL;
    strcpy(newEnt->m_line, line);
    last = m_pFirst;
    if (last == NULL)
    {
      m_pFirst = newEnt;
    }
    else
    {
      while (last->m_pNext != NULL)
      {
        last = last->m_pNext;
      };
      last->m_pNext = newEnt;
    };
  };
};
#endif


#define MAXLINEQUEUE 10

class RECORDFILE
{
private:
  i16  m_fileNum;
  bool m_isQueueingLines;
#ifdef MSVC_QUEUE
  std::list<std::string> m_lineQueue;
#else
  LINE_QUEUE m_lineQueue;
#endif
  bool m_graphicSignature;
  bool m_CSBgraphicSignature;
  bool m_dungeonSignature;
  bool m_versionSignature;
public:
  RECORDFILE(void) {m_fileNum = -1;m_isQueueingLines = false; m_lineQueue.clear();
                    m_graphicSignature=false;
                    m_dungeonSignature=false;
                    m_versionSignature=false;};
  ~RECORDFILE(void) {if (m_fileNum >= 0) CLOSE(m_fileNum);};
  bool IsOpen(void) {return m_fileNum >= 0;};
  bool IsRecording(void)
  {
    return (m_fileNum >= 0) || m_isQueueingLines;
  };
  void Open(void);
  void PreOpen(void);
  void Close(void);
  void Record(MouseQueueEnt *happening);
  void Record(i32 x, i32 y, i32 func);
  void Record(const char* line);
  void Comment(const char* comment);
  void CycleRandom(i32 n);
  void Signature(ui32 sig, ui32 type);
};

//extern RECORDFILE RecordFile;

class PLAYFILE
{
private:
  i32 m_time, m_x, m_y, m_num, m_oldTime, m_oldCallCount;
  ui32 m_ran;
  bool m_eofEncountered, m_forceClose;
  i16 m_file;
public:
  PLAYFILE(void) {m_file=-1; m_time=-1;m_eofEncountered=false;m_forceClose=false;};
  ~PLAYFILE(void) {if (m_file >= 0) CLOSE(m_file);};
  bool IsOpen(void);
  void Open(void);
  void Close(void);
  bool Play(MouseQueueEnt *);
  void Backspace(MouseQueueEnt *);
  void ReadEOF(void); //Force file close at EOF without
                      //advancing d.Time.
  bool IsEOF(void) {return m_eofEncountered;};
};

//extern PLAYFILE PlayFile;
