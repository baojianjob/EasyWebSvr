# Microsoft Developer Studio Project File - Name="EasyWebSvr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=EasyWebSvr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "EasyWebSvr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EasyWebSvr.mak" CFG="EasyWebSvr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EasyWebSvr - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "EasyWebSvr - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "EasyWebSvr - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /Zp4 /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"../Bin/EasyWebSvr.exe"

!ELSEIF  "$(CFG)" == "EasyWebSvr - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /Zp4 /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../BinD/EasyWebSvr.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "EasyWebSvr - Win32 Release"
# Name "EasyWebSvr - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseIO.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseLog.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\cmncode.cpp
# End Source File
# Begin Source File

SOURCE=.\core_log.cpp
# End Source File
# Begin Source File

SOURCE=.\CritSect.CPP
# End Source File
# Begin Source File

SOURCE=.\DirDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\FileDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\FlatBtn.cpp
# End Source File
# Begin Source File

SOURCE=.\MainWND.cpp
# End Source File
# Begin Source File

SOURCE=.\mtstring.cpp
# End Source File
# Begin Source File

SOURCE=.\MyWebSvr.cpp
# End Source File
# Begin Source File

SOURCE=.\NetSvr.cpp
# End Source File
# Begin Source File

SOURCE=.\PropSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\RespHdr.cpp
# End Source File
# Begin Source File

SOURCE=.\Socket.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# End Source File
# Begin Source File

SOURCE=.\StrList.cpp
# End Source File
# Begin Source File

SOURCE=.\SvrCfgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SvrLog.cpp
# End Source File
# Begin Source File

SOURCE=.\SysTray.cpp
# End Source File
# Begin Source File

SOURCE=.\Thread.cpp
# End Source File
# Begin Source File

SOURCE=.\WebSvr.cpp
# End Source File
# Begin Source File

SOURCE=.\WinMain.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\BaseCtrl.h
# End Source File
# Begin Source File

SOURCE=.\BaseDlg.h
# End Source File
# Begin Source File

SOURCE=.\BaseIO.h
# End Source File
# Begin Source File

SOURCE=.\BaseLog.h
# End Source File
# Begin Source File

SOURCE=.\BaseWnd.h
# End Source File
# Begin Source File

SOURCE=.\CalTIME.h
# End Source File
# Begin Source File

SOURCE=.\cmncode.h
# End Source File
# Begin Source File

SOURCE=.\core_log.h
# End Source File
# Begin Source File

SOURCE=.\CritSect.H
# End Source File
# Begin Source File

SOURCE=.\DirDLG.h
# End Source File
# Begin Source File

SOURCE=.\FileDLG.h
# End Source File
# Begin Source File

SOURCE=.\FlatBtn.h
# End Source File
# Begin Source File

SOURCE=.\MainWND.h
# End Source File
# Begin Source File

SOURCE=.\mtstring.h
# End Source File
# Begin Source File

SOURCE=.\MyWebSvr.h
# End Source File
# Begin Source File

SOURCE=.\NetSvr.h
# End Source File
# Begin Source File

SOURCE=.\PropSheet.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\RespHdr.h
# End Source File
# Begin Source File

SOURCE=.\Socket.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\StrList.h
# End Source File
# Begin Source File

SOURCE=.\SvrCfgDlg.h
# End Source File
# Begin Source File

SOURCE=.\SvrLog.h
# End Source File
# Begin Source File

SOURCE=.\SysTray.h
# End Source File
# Begin Source File

SOURCE=.\thread.h
# End Source File
# Begin Source File

SOURCE=.\TList.h
# End Source File
# Begin Source File

SOURCE=.\WebSvr.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\EasyWebSvr.ico
# End Source File
# Begin Source File

SOURCE=.\EasyWebSvr.rc
# End Source File
# Begin Source File

SOURCE=.\res\LedRun.ico
# End Source File
# Begin Source File

SOURCE=.\res\LedStop.ico
# End Source File
# Begin Source File

SOURCE=.\res\ShowMenu.ico
# End Source File
# Begin Source File

SOURCE=.\res\TrayRun.ico
# End Source File
# Begin Source File

SOURCE=.\res\TrayStop.ico
# End Source File
# End Group
# End Target
# End Project
