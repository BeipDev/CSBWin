class FILENAME // To avoid malloc and memory leaks
{
private:
  char *m_name[3];
  void createThreeNames(const char *filename);
  char *createName(const char *folder, const char *file);
public:
  char *m_fName;
  FILE *Open(const char *name, const char *flags);
  FILE *Create(const char *name, const char *flags);
  i32 Rename(const char *oldname, const char *newname);
  i32 Unlink(const char *name);
  FILENAME(void);
  ~FILENAME(void);
};

FILENAME::FILENAME(void)
{
  m_name[0] = NULL;
  m_name[1] = NULL;
  m_name[2] = NULL;
  m_fName = NULL;
}

FILENAME::~FILENAME(void)
{
  i32 i;
  for (i=0; i<3; i++)
  {
    if (m_name[i] != NULL) UI_free(m_name[i]);
    m_name[i] = NULL;
  };
  if (m_fName != NULL) UI_free(m_fName);
}

char *FILENAME::createName(const char *folder, const char *file)
{
  char *result;
  if (file == NULL) return NULL;
  if (strlen(file) == 0) return NULL;
  if (folder == NULL)
  {
    result = (char *)UI_malloc(strlen(file) + 1, MALLOC045);
    if (result != NULL) strcpy(result, file);
  }
  else
  {
    result = (char *)UI_malloc(strlen(folder) + strlen(file) +1, MALLOC046);
    if (result != NULL)
    {
      strcpy(result,folder);
      strcat(result,file);
    };
  };
#ifdef _MSVC_INTEL
  return strlwr(result);
#endif
#ifdef _LINUX
  return result;
#endif
}


void FILENAME::createThreeNames(const char *filename)
{
  if (m_name[0] != NULL)
  {
    UI_free(m_name[0]);
    m_name[0] = NULL;
  };
  if (m_name[1] != NULL) 
  {
    UI_free(m_name[1]);
    m_name[1] = NULL;
  };
  if (m_name[2] != NULL) 
  {
    UI_free(m_name[2]);
    m_name[2] = NULL;
  };
  if (strchr(filename,':') != 0)
  {
    m_name[0] = createName(NULL,filename);
    return;
  };
  if (folderName!=NULL) m_name[0] = createName(folderName, filename);
  if (folderParentName != NULL) m_name[1] = createName(folderParentName, filename);
  if (root != NULL) m_name[2] = createName(root, filename);
}

FILE *FILENAME::Open(const char *name, const char *flags)
{
  FILE *result = NULL;
  i32 i;
  createThreeNames(name);
  for (i=0; i<3; i++)
  {
    if (m_name[i] == NULL) continue;
    result = UI_fopen(m_name[i], flags);
    if (result != NULL) 
    {
      if (TimerTraceActive) fprintf(GETFILE(TraceFile), "Opened %s\n",m_name[i]);
      m_fName = (char *)UI_malloc(strlen(m_name[i])+1,MALLOC106);
      strcpy(m_fName, m_name[i]);
      return result;
    };
  };
  return NULL;
}

FILE *FILENAME::Create(const char *name, const char *flags)
{
  i32 i;
  createThreeNames(name);
  for (i=0; i<3; i++)
  {
    if (m_name[i] ==NULL) continue;
    return UI_fopen(m_name[i], flags);
  };
  return NULL;
}

i32 FILENAME::Rename(const char *oldname, const char *newname)
{
  FILENAME newfile;
  i32 i;
  createThreeNames(oldname);
  newfile.createThreeNames(newname);
  for (i=0; i<3; i++)
  {
    if (m_name[i] == NULL) continue;
    return rename(m_name[i], newfile.m_name[i]);
  };
  return -1;
}

i32 FILENAME::Unlink(const char *name)
{
  i32 i;
  createThreeNames(name);
  for (i=0; i<3; i++)
  {
    if (m_name[i] == NULL) continue;
    return UI_DeleteFile(m_name[i]);
  };
  return 0;
}

void UnlinkFile(const char *name)
{
  FILENAME file;
  file.Unlink(name);
}

char *GETFILENAME(i32 f);

ui64 MODIFIEDTIME(i32 file)
{
  char *fn;
  HANDLE fh;
  WIN32_FIND_DATAA fData;
  fn=GETFILENAME(file);
  if (fn == NULL) return 0;
  fh = FindFirstFileA(fn, &fData);
  if (fh == INVALID_HANDLE_VALUE) return 0;
  FindClose(fh);
  return *(ui64 *)(&fData.ftLastWriteTime);
}

