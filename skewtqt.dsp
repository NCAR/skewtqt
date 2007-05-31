# Microsoft Developer Studio Project File - Name="skewtqt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=skewtqt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "skewtqt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "skewtqt.mak" CFG="skewtqt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "skewtqt - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "skewtqt - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "skewtqt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD CPP -MD /W3 /I ".." /I "..\.." /I "c:\Projects\include" /I "c:\Projects\raddx" /I "c:\Projects" /I "c:\Projects\AvapsEd" /I "$(QTDIR)\include" /I "." /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_LIB" /D UNICODE /D QT_DLL /D QT_THREAD_SUPPORT /D "QT_NO_DEBUG" /FD /c -nologo -Zm200 -GX -GX -GR -O1 /Tp /GX 
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo /out:"c:\Projects\lib\skewtqt.lib" 


!ELSEIF  "$(CFG)" == "skewtqt - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "c:/Projects/lib"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "c:/Projects/lib"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD CPP -MDd /W3 /GZ /ZI /Od /I ".." /I "..\.." /I "c:\Projects\include" /I "c:\Projects\raddx" /I "c:\Projects" /I "c:\Projects\AvapsEd" /I "$(QTDIR)\include" /I "." /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_LIB" /D UNICODE /D QT_DLL /D QT_THREAD_SUPPORT /FD /c -nologo -Zm200 -GX -GX -GR  -Zi /Tp /GX /GR  
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo /out:"c:\Projects\lib\skewtqt.lib" 


!ENDIF 

# Begin Target

# Name "skewtqt - Win32 Release"
# Name "skewtqt - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=SkewtQtPlotWindow.cc
# End Source File
# Begin Source File

SOURCE=SkewtQtPlot.cc
# End Source File
# Begin Source File

SOURCE=SkewTAdapterQt.cc
# End Source File

# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=SkewtQtPlotWindow.h

USERDEP_SkewtQtPlotWindow=""$(QTDIR)\bin\moc.exe""

!IF  "$(CFG)" == "skewtqt - Win32 Release"

# Begin Custom Build - Moc'ing SkewtQtPlotWindow.h...
InputPath=.\SkewtQtPlotWindow.h


BuildCmds= \
	$(QTDIR)\bin\moc SkewtQtPlotWindow.h -o moc_SkewtQtPlotWindow.cpp \

"moc_SkewtQtPlotWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

# End Custom Build

!ELSEIF  "$(CFG)" == "skewtqt - Win32 Debug"

# Begin Custom Build - Moc'ing SkewtQtPlotWindow.h...
InputPath=.\SkewtQtPlotWindow.h


BuildCmds= \
	$(QTDIR)\bin\moc SkewtQtPlotWindow.h -o moc_SkewtQtPlotWindow.cpp \

"moc_SkewtQtPlotWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=SkewtQtPlot.h

USERDEP_SkewtQtPlot=""$(QTDIR)\bin\moc.exe""

!IF  "$(CFG)" == "skewtqt - Win32 Release"

# Begin Custom Build - Moc'ing SkewtQtPlot.h...
InputPath=.\SkewtQtPlot.h


BuildCmds= \
	$(QTDIR)\bin\moc SkewtQtPlot.h -o moc_SkewtQtPlot.cpp \

"moc_SkewtQtPlot.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

# End Custom Build

!ELSEIF  "$(CFG)" == "skewtqt - Win32 Debug"

# Begin Custom Build - Moc'ing SkewtQtPlot.h...
InputPath=.\SkewtQtPlot.h


BuildCmds= \
	$(QTDIR)\bin\moc SkewtQtPlot.h -o moc_SkewtQtPlot.cpp \

"moc_SkewtQtPlot.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=SkewTAdapterQt.h

USERDEP_SkewTAdapterQt=""$(QTDIR)\bin\moc.exe""

!IF  "$(CFG)" == "skewtqt - Win32 Release"

# Begin Custom Build - Moc'ing SkewTAdapterQt.h...
InputPath=.\SkewTAdapterQt.h


BuildCmds= \
	$(QTDIR)\bin\moc SkewTAdapterQt.h -o moc_SkewTAdapterQt.cpp \

"moc_SkewTAdapterQt.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

# End Custom Build

!ELSEIF  "$(CFG)" == "skewtqt - Win32 Debug"

# Begin Custom Build - Moc'ing SkewTAdapterQt.h...
InputPath=.\SkewTAdapterQt.h


BuildCmds= \
	$(QTDIR)\bin\moc SkewTAdapterQt.h -o moc_SkewTAdapterQt.cpp \

"moc_SkewTAdapterQt.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

# End Custom Build

!ENDIF 

# End Source File

# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group







# Begin Group "Generated"
# Begin Source File

SOURCE=moc_SkewtQtPlotWindow.cpp
# End Source File
# Begin Source File

SOURCE=moc_SkewtQtPlot.cpp
# End Source File
# Begin Source File

SOURCE=moc_SkewTAdapterQt.cpp
# End Source File





# Prop Default_Filter "moc"
# End Group
# End Target
# End Project

