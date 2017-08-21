# Microsoft Developer Studio Generated NMAKE File, Based on CSBwin.dsp
!IF "$(CFG)" == ""
CFG=CSBwin - Win32 Debug
!MESSAGE No configuration specified. Defaulting to CSBwin - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "CSBwin - Win32 Release" && "$(CFG)" != "CSBwin - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CSBwin.mak" CFG="CSBwin - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CSBwin - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CSBwin - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "CSBwin - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\CSBwin.exe"


CLEAN :
	-@erase "$(INTDIR)\AsciiDump.obj"
	-@erase "$(INTDIR)\Attack.obj"
	-@erase "$(INTDIR)\Bitmaps.obj"
	-@erase "$(INTDIR)\Chaos.obj"
	-@erase "$(INTDIR)\Character.obj"
	-@erase "$(INTDIR)\Code11f52.obj"
	-@erase "$(INTDIR)\Code13ea4.obj"
	-@erase "$(INTDIR)\Code17818.obj"
	-@erase "$(INTDIR)\Code1f9e6.obj"
	-@erase "$(INTDIR)\Code222ea.obj"
	-@erase "$(INTDIR)\Code390e.obj"
	-@erase "$(INTDIR)\Code51a4.obj"
	-@erase "$(INTDIR)\Codea59a.obj"
	-@erase "$(INTDIR)\CSBCode.obj"
	-@erase "$(INTDIR)\CSBUI.obj"
	-@erase "$(INTDIR)\CSBwin.obj"
	-@erase "$(INTDIR)\CSBwin.pch"
	-@erase "$(INTDIR)\CSBwin.res"
	-@erase "$(INTDIR)\data.obj"
	-@erase "$(INTDIR)\DSA.obj"
	-@erase "$(INTDIR)\Graphics.obj"
	-@erase "$(INTDIR)\Hint.obj"
	-@erase "$(INTDIR)\Magic.obj"
	-@erase "$(INTDIR)\md5C.obj"
	-@erase "$(INTDIR)\Menu.obj"
	-@erase "$(INTDIR)\Monster.obj"
	-@erase "$(INTDIR)\Mouse.obj"
	-@erase "$(INTDIR)\NewBugs.obj"
	-@erase "$(INTDIR)\RC4.obj"
	-@erase "$(INTDIR)\SaveGame.obj"
	-@erase "$(INTDIR)\SmartDiscard.obj"
	-@erase "$(INTDIR)\Sound.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\system.obj"
	-@erase "$(INTDIR)\Timer.obj"
	-@erase "$(INTDIR)\utility.obj"
	-@erase "$(INTDIR)\VBL.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\Viewport.obj"
	-@erase "$(INTDIR)\Win32_SoundMixer.obj"
	-@erase "$(INTDIR)\WinScreen.obj"
	-@erase "$(OUTDIR)\CSBwin.exe"
	-@erase "$(OUTDIR)\CSBwin.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MSVC_INTEL" /Fp"$(INTDIR)\CSBwin.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\CSBwin.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CSBwin.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\CSBwin.pdb" /map:"$(INTDIR)\CSBwin.map" /machine:I386 /out:"$(OUTDIR)\CSBwin.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AsciiDump.obj" \
	"$(INTDIR)\Attack.obj" \
	"$(INTDIR)\Bitmaps.obj" \
	"$(INTDIR)\Chaos.obj" \
	"$(INTDIR)\Character.obj" \
	"$(INTDIR)\Code11f52.obj" \
	"$(INTDIR)\Code13ea4.obj" \
	"$(INTDIR)\Code17818.obj" \
	"$(INTDIR)\Code1f9e6.obj" \
	"$(INTDIR)\Code222ea.obj" \
	"$(INTDIR)\Code390e.obj" \
	"$(INTDIR)\Code51a4.obj" \
	"$(INTDIR)\Codea59a.obj" \
	"$(INTDIR)\CSBCode.obj" \
	"$(INTDIR)\CSBUI.obj" \
	"$(INTDIR)\CSBwin.obj" \
	"$(INTDIR)\data.obj" \
	"$(INTDIR)\DSA.obj" \
	"$(INTDIR)\Graphics.obj" \
	"$(INTDIR)\Hint.obj" \
	"$(INTDIR)\Magic.obj" \
	"$(INTDIR)\md5C.obj" \
	"$(INTDIR)\Menu.obj" \
	"$(INTDIR)\Monster.obj" \
	"$(INTDIR)\Mouse.obj" \
	"$(INTDIR)\NewBugs.obj" \
	"$(INTDIR)\RC4.obj" \
	"$(INTDIR)\SaveGame.obj" \
	"$(INTDIR)\SmartDiscard.obj" \
	"$(INTDIR)\Sound.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\system.obj" \
	"$(INTDIR)\Timer.obj" \
	"$(INTDIR)\utility.obj" \
	"$(INTDIR)\VBL.obj" \
	"$(INTDIR)\Viewport.obj" \
	"$(INTDIR)\Win32_SoundMixer.obj" \
	"$(INTDIR)\WinScreen.obj" \
	"$(INTDIR)\CSBwin.res"

"$(OUTDIR)\CSBwin.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=copy release executable
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\CSBwin.exe"
   copy release\CSBwin.exe .\*.*
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "CSBwin - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\CSBwin.exe"


CLEAN :
	-@erase "$(INTDIR)\AsciiDump.obj"
	-@erase "$(INTDIR)\Attack.obj"
	-@erase "$(INTDIR)\Bitmaps.obj"
	-@erase "$(INTDIR)\Chaos.obj"
	-@erase "$(INTDIR)\Character.obj"
	-@erase "$(INTDIR)\Code11f52.obj"
	-@erase "$(INTDIR)\Code13ea4.obj"
	-@erase "$(INTDIR)\Code17818.obj"
	-@erase "$(INTDIR)\Code1f9e6.obj"
	-@erase "$(INTDIR)\Code222ea.obj"
	-@erase "$(INTDIR)\Code390e.obj"
	-@erase "$(INTDIR)\Code51a4.obj"
	-@erase "$(INTDIR)\Codea59a.obj"
	-@erase "$(INTDIR)\CSBCode.obj"
	-@erase "$(INTDIR)\CSBUI.obj"
	-@erase "$(INTDIR)\CSBwin.obj"
	-@erase "$(INTDIR)\CSBwin.pch"
	-@erase "$(INTDIR)\CSBwin.res"
	-@erase "$(INTDIR)\data.obj"
	-@erase "$(INTDIR)\DSA.obj"
	-@erase "$(INTDIR)\Graphics.obj"
	-@erase "$(INTDIR)\Hint.obj"
	-@erase "$(INTDIR)\Magic.obj"
	-@erase "$(INTDIR)\md5C.obj"
	-@erase "$(INTDIR)\Menu.obj"
	-@erase "$(INTDIR)\Monster.obj"
	-@erase "$(INTDIR)\Mouse.obj"
	-@erase "$(INTDIR)\NewBugs.obj"
	-@erase "$(INTDIR)\RC4.obj"
	-@erase "$(INTDIR)\SaveGame.obj"
	-@erase "$(INTDIR)\SmartDiscard.obj"
	-@erase "$(INTDIR)\Sound.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\system.obj"
	-@erase "$(INTDIR)\Timer.obj"
	-@erase "$(INTDIR)\utility.obj"
	-@erase "$(INTDIR)\VBL.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\Viewport.obj"
	-@erase "$(INTDIR)\Win32_SoundMixer.obj"
	-@erase "$(INTDIR)\WinScreen.obj"
	-@erase "$(OUTDIR)\CSBwin.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MSVC_INTEL" /Fp"$(INTDIR)\CSBwin.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\CSBwin.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CSBwin.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /profile /debug /machine:I386 /out:"$(OUTDIR)\CSBwin.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AsciiDump.obj" \
	"$(INTDIR)\Attack.obj" \
	"$(INTDIR)\Bitmaps.obj" \
	"$(INTDIR)\Chaos.obj" \
	"$(INTDIR)\Character.obj" \
	"$(INTDIR)\Code11f52.obj" \
	"$(INTDIR)\Code13ea4.obj" \
	"$(INTDIR)\Code17818.obj" \
	"$(INTDIR)\Code1f9e6.obj" \
	"$(INTDIR)\Code222ea.obj" \
	"$(INTDIR)\Code390e.obj" \
	"$(INTDIR)\Code51a4.obj" \
	"$(INTDIR)\Codea59a.obj" \
	"$(INTDIR)\CSBCode.obj" \
	"$(INTDIR)\CSBUI.obj" \
	"$(INTDIR)\CSBwin.obj" \
	"$(INTDIR)\data.obj" \
	"$(INTDIR)\DSA.obj" \
	"$(INTDIR)\Graphics.obj" \
	"$(INTDIR)\Hint.obj" \
	"$(INTDIR)\Magic.obj" \
	"$(INTDIR)\md5C.obj" \
	"$(INTDIR)\Menu.obj" \
	"$(INTDIR)\Monster.obj" \
	"$(INTDIR)\Mouse.obj" \
	"$(INTDIR)\NewBugs.obj" \
	"$(INTDIR)\RC4.obj" \
	"$(INTDIR)\SaveGame.obj" \
	"$(INTDIR)\SmartDiscard.obj" \
	"$(INTDIR)\Sound.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\system.obj" \
	"$(INTDIR)\Timer.obj" \
	"$(INTDIR)\utility.obj" \
	"$(INTDIR)\VBL.obj" \
	"$(INTDIR)\Viewport.obj" \
	"$(INTDIR)\Win32_SoundMixer.obj" \
	"$(INTDIR)\WinScreen.obj" \
	"$(INTDIR)\CSBwin.res"

"$(OUTDIR)\CSBwin.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("CSBwin.dep")
!INCLUDE "CSBwin.dep"
!ELSE 
!MESSAGE Warning: cannot find "CSBwin.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "CSBwin - Win32 Release" || "$(CFG)" == "CSBwin - Win32 Debug"
SOURCE=.\AsciiDump.cpp

"$(INTDIR)\AsciiDump.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Attack.cpp

"$(INTDIR)\Attack.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Bitmaps.cpp

"$(INTDIR)\Bitmaps.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Chaos.cpp

"$(INTDIR)\Chaos.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Character.cpp

"$(INTDIR)\Character.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Code11f52.cpp

"$(INTDIR)\Code11f52.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Code13ea4.cpp

"$(INTDIR)\Code13ea4.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Code17818.cpp

"$(INTDIR)\Code17818.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Code1f9e6.cpp

"$(INTDIR)\Code1f9e6.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Code222ea.cpp

"$(INTDIR)\Code222ea.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Code390e.cpp

"$(INTDIR)\Code390e.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Code51a4.cpp

"$(INTDIR)\Code51a4.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Codea59a.cpp

"$(INTDIR)\Codea59a.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\CSBCode.cpp

"$(INTDIR)\CSBCode.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\CSBUI.cpp

"$(INTDIR)\CSBUI.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\CSBwin.cpp

"$(INTDIR)\CSBwin.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\CSBwin.rc

"$(INTDIR)\CSBwin.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\data.cpp

"$(INTDIR)\data.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\DSA.cpp

"$(INTDIR)\DSA.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Graphics.cpp

"$(INTDIR)\Graphics.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Hint.cpp

"$(INTDIR)\Hint.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Magic.cpp

"$(INTDIR)\Magic.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\md5C.cpp

"$(INTDIR)\md5C.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Menu.cpp

"$(INTDIR)\Menu.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Monster.cpp

"$(INTDIR)\Monster.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Mouse.cpp

"$(INTDIR)\Mouse.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\NewBugs.cpp

"$(INTDIR)\NewBugs.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\RC4.cpp

"$(INTDIR)\RC4.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\SaveGame.cpp

"$(INTDIR)\SaveGame.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\SmartDiscard.cpp

"$(INTDIR)\SmartDiscard.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Sound.cpp

"$(INTDIR)\Sound.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "CSBwin - Win32 Release"

CPP_SWITCHES=/nologo /ML /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MSVC_INTEL" /Fp"$(INTDIR)\CSBwin.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\CSBwin.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CSBwin - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MSVC_INTEL" /Fp"$(INTDIR)\CSBwin.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\CSBwin.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\system.cpp

"$(INTDIR)\system.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Timer.cpp

"$(INTDIR)\Timer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\utility.cpp

"$(INTDIR)\utility.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\VBL.cpp

"$(INTDIR)\VBL.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Viewport.cpp

"$(INTDIR)\Viewport.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"


SOURCE=.\Win32_SoundMixer.cpp

!IF  "$(CFG)" == "CSBwin - Win32 Release"

CPP_SWITCHES=/nologo /ML /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MSVC_INTEL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Win32_SoundMixer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CSBwin - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MSVC_INTEL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Win32_SoundMixer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\WinScreen.cpp

"$(INTDIR)\WinScreen.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CSBwin.pch"



!ENDIF 

