# Microsoft Developer Studio Project File - Name="TimeSeries" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=TimeSeries - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TimeSeries.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TimeSeries.mak" CFG="TimeSeries - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TimeSeries - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TimeSeries - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "./include" /I "../shared/include" /I "../Wavelets/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "I386" /YX /FD /Zm200 /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "./include" /I "../shared/include" /I "../Wavelets/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "I386" /YX /FD /GZ /Zm200 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "TimeSeries - Win32 Release"
# Name "TimeSeries - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\ar.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\arfima.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\arima.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\arma.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\await.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\bestmean.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\bestmedian.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\crossval_core.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\etathetapred.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\evaluate_core.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\fileparameterset.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\fit.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\it.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\last.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\ma.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\managed.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\mean.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\newton.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\none.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\src\nr-internal.cpp"

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\pdqparamsets.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\poly.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\refit.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\tools.cpp

!IF  "$(CFG)" == "TimeSeries - Win32 Release"

# ADD CPP /I "../shared/include"

!ELSEIF  "$(CFG)" == "TimeSeries - Win32 Debug"

# ADD CPP /I "./include" /I "../shared/include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\wavelet.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
