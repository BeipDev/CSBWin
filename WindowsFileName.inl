struct FILENAME // To avoid malloc and memory leaks
{
  std::string m_fName;

  FILE *Open(const char *name, const char *flags);
  FILE *Create(const char *name, const char *flags);
  i32 Rename(const char *oldname, const char *newname);
  i32 Unlink(const char *name);

private:
  std::string m_names[3];
  void createThreeNames(const char *filename);
  std::string createName(const char *folder, const char *file);
};

std::string FILENAME::createName(const char *folder, const char *file)
{
  if(!file)
    return nullptr;
  if(strlen(file) == 0)
    return nullptr;

  std::string result;
  if(!folder)
    result = file;
  else
  {
    result=folder;
    result+=file;
  }
#ifdef _MSVC_INTEL
  std::transform(result.begin(), result.end(), result.begin(), ::tolower);
#endif
  return result;
}


void FILENAME::createThreeNames(const char *filename)
{
   for(auto &name : m_names)
      name.clear();

  if(strchr(filename, ':') != 0)
  {
    m_names[0] = createName(nullptr, filename);
    return;
  }

  if(g_folderName)
    m_names[0] = createName(g_folderName, filename);
  if(!g_folderParentName.empty())
    m_names[1] = createName(g_folderParentName.c_str(), filename);
  if(!g_root.empty())
    m_names[2] = createName(g_root.c_str(), filename);
}

FILE *FILENAME::Open(const char *name, const char *flags)
{
  createThreeNames(name);
  for(unsigned i=0; i<3; i++)
  {
    if (m_names[i].empty()) continue;
    FILE *result = UI_fopen(m_names[i].c_str(), flags);
    if(result) 
    {
      if (TimerTraceActive) fprintf(GETFILE(TraceFile), "Opened %s\n",m_names[i].c_str());
      m_fName = m_names[i];
      return result;
    }
  }
  return nullptr;
}

FILE *FILENAME::Create(const char *name, const char *flags)
{
  createThreeNames(name);
  for(auto &name : m_names)
  {
    if(name.empty())
      continue;
    return UI_fopen(name.c_str(), flags);
  };
  return nullptr;
}

i32 FILENAME::Rename(const char *oldname, const char *newname)
{
  FILENAME newfile;
  createThreeNames(oldname);
  newfile.createThreeNames(newname);
  for(unsigned i=0; i<3; i++)
  {
    if(m_names[i].empty())
      continue;
    return rename(m_names[i].c_str(), newfile.m_names[i].c_str());
  };
  return -1;
}

i32 FILENAME::Unlink(const char *name)
{
  createThreeNames(name);
  for(auto &name : m_names)
  {
    if(name.empty())
      continue;
    return UI_DeleteFile(name.c_str());
  };
  return 0;
}

void UnlinkFile(const char *name)
{
  FILENAME file;
  file.Unlink(name);
}

const char *GETFILENAME(i32 f);

ui64 MODIFIEDTIME(i32 file)
{
  const char *fn=GETFILENAME(file);
  if (!fn) return 0;

  WIN32_FIND_DATA fData;
  HANDLE fh = FindFirstFile(fn, &fData);
  if (fh == INVALID_HANDLE_VALUE) return 0;
  FindClose(fh);
  return *(ui64 *)(&fData.ftLastWriteTime);
}

