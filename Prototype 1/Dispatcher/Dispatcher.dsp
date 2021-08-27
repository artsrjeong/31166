# Microsoft Developer Studio Project File - Name="Dispatcher" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Dispatcher - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Dispatcher.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Dispatcher.mak" CFG="Dispatcher - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Dispatcher - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Dispatcher - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Dispatcher - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DISPATCHER_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DISPATCHER_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\Release/Dispatcher.dll"

!ELSEIF  "$(CFG)" == "Dispatcher - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DISPATCHER_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DISPATCHER_EXPORTS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../Debug/Dispatcher.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Dispatcher - Win32 Release"
# Name "Dispatcher - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\2DGroupDispatcher.cpp
# End Source File
# Begin Source File

SOURCE=.\assign.cpp
# End Source File
# Begin Source File

SOURCE=.\Asslib.cpp
# End Source File
# Begin Source File

SOURCE=.\crisppwt.cpp
# End Source File
# Begin Source File

SOURCE=.\Crisprrt.cpp
# End Source File
# Begin Source File

SOURCE=.\E2p.cpp
# End Source File
# Begin Source File

SOURCE="..\HoistwayPlanner\ESP-NSP.cpp"
# End Source File
# Begin Source File

SOURCE=.\Gcbinfad.cpp
# End Source File
# Begin Source File

SOURCE=..\GDInterfaceCOGS\GDInterfaceCOGS.cpp
# End Source File
# Begin Source File

SOURCE=..\HoistwayPlanner\hoistway_plan.cpp
# End Source File
# Begin Source File

SOURCE=..\HoistwayPlanner\HoistwayPlanGen.cpp
# End Source File
# Begin Source File

SOURCE=..\HoistwayPlanner\HoistwayPlanner.cpp
# End Source File
# Begin Source File

SOURCE=..\HoistwayPlanner\HoistwayPlanSelection.cpp
# End Source File
# Begin Source File

SOURCE=.\Infbld.cpp
# End Source File
# Begin Source File

SOURCE=.\infdes.cpp
# End Source File
# Begin Source File

SOURCE=.\Infocs.cpp
# End Source File
# Begin Source File

SOURCE=.\NPI.cpp
# End Source File
# Begin Source File

SOURCE=.\Pwt.cpp
# End Source File
# Begin Source File

SOURCE=.\rrt.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\HoistwayPlanner\timer.cpp
# End Source File
# Begin Source File

SOURCE=.\trafdat.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="..\HoistwayPlanner\ESP-NSP.h"
# End Source File
# Begin Source File

SOURCE=..\GDInterfaceCOGS\GDInterfaceCOGS.h
# End Source File
# Begin Source File

SOURCE=..\HoistwayPlanner\hoistway_plan.h
# End Source File
# Begin Source File

SOURCE=..\HoistwayPlanner\HoistwayPlanGen.h
# End Source File
# Begin Source File

SOURCE=..\HoistwayPlanner\HoistwayPlanner.h
# End Source File
# Begin Source File

SOURCE=..\HoistwayPlanner\HoistwayPlannerDefs.h
# End Source File
# Begin Source File

SOURCE=..\HoistwayPlanner\HoistwayPlannerDLL.h
# End Source File
# Begin Source File

SOURCE=..\HoistwayPlanner\HoistwayPlannerType.h
# End Source File
# Begin Source File

SOURCE=..\HoistwayPlanner\HoistwayPlanSelection.h
# End Source File
# Begin Source File

SOURCE=..\HoistwayPlanner\HoistwaySchedule.h
# End Source File
# Begin Source File

SOURCE=.\Infdes.h
# End Source File
# Begin Source File

SOURCE=.\NPI.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\HoistwayPlanner\timer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project