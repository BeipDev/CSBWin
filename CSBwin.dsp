# Microsoft Developer Studio Project File - Name="CSBwin" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CSBwin - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CSBwin.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CSBwin - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MSVC_INTEL" /D "_MIDI" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /profile /map /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy release executable
PostBuild_Cmds=copy release\CSBwin.exe .\*.*
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CSBwin - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MSVC_INTEL" /D "_MIDI" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /profile /debug /machine:I386

!ENDIF 

# Begin Target

# Name "CSBwin - Win32 Release"
# Name "CSBwin - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AsciiDump.cpp
# End Source File
# Begin Source File

SOURCE=.\Attack.cpp
# End Source File
# Begin Source File

SOURCE=.\Bitmaps.cpp
# End Source File
# Begin Source File

SOURCE=.\Chaos.cpp
# End Source File
# Begin Source File

SOURCE=.\Character.cpp
# End Source File
# Begin Source File

SOURCE=.\Code11f52.cpp
# End Source File
# Begin Source File

SOURCE=.\Code13ea4.cpp
# End Source File
# Begin Source File

SOURCE=.\Code17818.cpp
# End Source File
# Begin Source File

SOURCE=.\Code1f9e6.cpp
# End Source File
# Begin Source File

SOURCE=.\Code222ea.cpp
# End Source File
# Begin Source File

SOURCE=.\Code390e.cpp
# End Source File
# Begin Source File

SOURCE=.\Code51a4.cpp
# End Source File
# Begin Source File

SOURCE=.\Codea59a.cpp
# End Source File
# Begin Source File

SOURCE=.\CSBCode.cpp
# End Source File
# Begin Source File

SOURCE=.\CSBUI.cpp
# End Source File
# Begin Source File

SOURCE=.\CSBwin.cpp
# End Source File
# Begin Source File

SOURCE=.\CSBwin.rc
# End Source File
# Begin Source File

SOURCE=.\data.cpp
# End Source File
# Begin Source File

SOURCE=.\DSA.cpp
# End Source File
# Begin Source File

SOURCE=.\Graphics.cpp
# End Source File
# Begin Source File

SOURCE=.\Hint.cpp
# End Source File
# Begin Source File

SOURCE=.\Magic.cpp
# End Source File
# Begin Source File

SOURCE=.\md5C.cpp
# End Source File
# Begin Source File

SOURCE=.\Menu.cpp
# End Source File
# Begin Source File

SOURCE=.\Monster.cpp
# End Source File
# Begin Source File

SOURCE=.\Mouse.cpp
# End Source File
# Begin Source File

SOURCE=.\MoveObject.cpp
# End Source File
# Begin Source File

SOURCE=.\NewBugs.cpp
# End Source File
# Begin Source File

SOURCE=.\RC4.cpp
# End Source File
# Begin Source File

SOURCE=.\Recording.cpp

!IF  "$(CFG)" == "CSBwin - Win32 Release"

# ADD CPP /W4

!ELSEIF  "$(CFG)" == "CSBwin - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SaveGame.cpp
# End Source File
# Begin Source File

SOURCE=.\SmartDiscard.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound.cpp
# End Source File
# Begin Source File

SOURCE=.\Statistics.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\system.cpp
# End Source File
# Begin Source File

SOURCE=.\Timer.cpp
# End Source File
# Begin Source File

SOURCE=.\Translation.cpp
# End Source File
# Begin Source File

SOURCE=.\utility.cpp
# End Source File
# Begin Source File

SOURCE=.\VBL.cpp
# End Source File
# Begin Source File

SOURCE=.\Viewport.cpp
# End Source File
# Begin Source File

SOURCE=.\WinScreen.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CSB.h
# End Source File
# Begin Source File

SOURCE=.\CSBTypes.h
# End Source File
# Begin Source File

SOURCE=.\Data.h
# End Source File
# Begin Source File

SOURCE=.\Dispatch.h
# End Source File
# Begin Source File

SOURCE=.\Objects.h
# End Source File
# Begin Source File

SOURCE=.\Recording.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\UI.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\CSBwin.ico
# End Source File
# Begin Source File

SOURCE=.\cursor3.cur
# End Source File
# Begin Source File

SOURCE=.\small.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
