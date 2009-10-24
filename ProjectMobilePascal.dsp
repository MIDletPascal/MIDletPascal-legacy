# Microsoft Developer Studio Project File - Name="ProjectMobilePascal" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ProjectMobilePascal - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ProjectMobilePascal.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ProjectMobilePascal.mak" CFG="ProjectMobilePascal - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ProjectMobilePascal - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ProjectMobilePascal - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ProjectMobilePascal - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O1 /Ob0 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /c
# ADD BASE RSC /l 0x41a /d "NDEBUG"
# ADD RSC /l 0x41a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"libc.lib"

!ELSEIF  "$(CFG)" == "ProjectMobilePascal - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x41a /d "_DEBUG"
# ADD RSC /l 0x41a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ProjectMobilePascal - Win32 Release"
# Name "ProjectMobilePascal - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "lex"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lex\lex.yy.c
# End Source File
# End Group
# Begin Group "util"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\util\error.c
# End Source File
# Begin Source File

SOURCE=.\util\memory.c
# End Source File
# Begin Source File

SOURCE=.\util\strings.c
# End Source File
# End Group
# Begin Group "main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\main\main.c
# End Source File
# End Group
# Begin Group "parser"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\parser\parser.c
# End Source File
# Begin Source File

SOURCE=.\parser\stdpas.c
# End Source File
# End Group
# Begin Group "structures"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\structures\block.c
# End Source File
# Begin Source File

SOURCE=.\structures\identifier.c
# End Source File
# Begin Source File

SOURCE=.\structures\name_table.c
# End Source File
# Begin Source File

SOURCE=.\structures\string_list.c
# End Source File
# Begin Source File

SOURCE=.\structures\type.c
# End Source File
# Begin Source File

SOURCE=.\structures\type_list.c

!IF  "$(CFG)" == "ProjectMobilePascal - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "ProjectMobilePascal - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\structures\unit.c
# End Source File
# End Group
# Begin Group "classgen"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\classgen\bytecode.c
# End Source File
# Begin Source File

SOURCE=.\classgen\classgen.c
# End Source File
# Begin Source File

SOURCE=.\classgen\constant_pool.c
# End Source File
# Begin Source File

SOURCE=.\classgen\preverify.c
# End Source File
# End Group
# Begin Group "preverifier"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\preverifier\check_class.c

!IF  "$(CFG)" == "ProjectMobilePascal - Win32 Release"

# ADD CPP /O2 /Ob2

!ELSEIF  "$(CFG)" == "ProjectMobilePascal - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\preverifier\check_code.c

!IF  "$(CFG)" == "ProjectMobilePascal - Win32 Release"

# ADD CPP /O2 /Ob2

!ELSEIF  "$(CFG)" == "ProjectMobilePascal - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\preverifier\classloader.c

!IF  "$(CFG)" == "ProjectMobilePascal - Win32 Release"

# ADD CPP /Ot /Og /Oi

!ELSEIF  "$(CFG)" == "ProjectMobilePascal - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\preverifier\classresolver.c

!IF  "$(CFG)" == "ProjectMobilePascal - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "ProjectMobilePascal - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\preverifier\convert_md.c
# End Source File
# Begin Source File

SOURCE=.\preverifier\file.c
# End Source File
# Begin Source File

SOURCE=.\preverifier\inlinejsr.c
# End Source File
# Begin Source File

SOURCE=.\preverifier\jar.c

!IF  "$(CFG)" == "ProjectMobilePascal - Win32 Release"

# ADD CPP /Ox /Os /Ob2

!ELSEIF  "$(CFG)" == "ProjectMobilePascal - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\preverifier\jar_support.c
# End Source File
# Begin Source File

SOURCE=.\preverifier\main.c
# End Source File
# Begin Source File

SOURCE=.\preverifier\stubs.c
# End Source File
# Begin Source File

SOURCE=.\preverifier\sys_support.c
# End Source File
# Begin Source File

SOURCE=.\preverifier\utf.c
# End Source File
# Begin Source File

SOURCE=.\preverifier\util.c
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "lex_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lex\tokens.h
# End Source File
# End Group
# Begin Group "util_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\util\error.h
# End Source File
# Begin Source File

SOURCE=.\util\memory.h
# End Source File
# Begin Source File

SOURCE=.\util\message.h
# End Source File
# Begin Source File

SOURCE=.\util\strings.h
# End Source File
# End Group
# Begin Group "parser_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\parser\parser.h
# End Source File
# Begin Source File

SOURCE=.\parser\stdpas.h
# End Source File
# End Group
# Begin Group "structures_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\structures\block.h
# End Source File
# Begin Source File

SOURCE=.\structures\identifier.h
# End Source File
# Begin Source File

SOURCE=.\structures\name_table.h
# End Source File
# Begin Source File

SOURCE=.\structures\string_list.h
# End Source File
# Begin Source File

SOURCE=.\structures\type.h
# End Source File
# Begin Source File

SOURCE=.\structures\type_list.h
# End Source File
# Begin Source File

SOURCE=.\structures\unit.h
# End Source File
# End Group
# Begin Group "classgen_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\classgen\bytecode.h
# End Source File
# Begin Source File

SOURCE=.\classgen\classgen.h
# End Source File
# Begin Source File

SOURCE=.\classgen\constant_pool.h
# End Source File
# Begin Source File

SOURCE=.\classgen\preverify.h
# End Source File
# End Group
# Begin Group "preverifier_h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\preverifier\check_code.h
# End Source File
# Begin Source File

SOURCE=.\preverifier\convert_md.h
# End Source File
# Begin Source File

SOURCE=.\preverifier\jar.h
# End Source File
# Begin Source File

SOURCE=.\preverifier\jarint.h
# End Source File
# Begin Source File

SOURCE=.\preverifier\jartables.h
# End Source File
# Begin Source File

SOURCE=.\preverifier\locale_md.h
# End Source File
# Begin Source File

SOURCE=.\preverifier\oobj.h
# End Source File
# Begin Source File

SOURCE=.\preverifier\opcodes.h
# End Source File
# Begin Source File

SOURCE=.\preverifier\path.h
# End Source File
# Begin Source File

SOURCE=.\preverifier\path_md.h
# End Source File
# Begin Source File

SOURCE=.\preverifier\signature.h
# End Source File
# Begin Source File

SOURCE=.\preverifier\sys_api.h
# End Source File
# Begin Source File

SOURCE=.\preverifier\sysmacros_md.h
# End Source File
# Begin Source File

SOURCE=.\preverifier\tree.h
# End Source File
# Begin Source File

SOURCE=.\preverifier\typecodes.h
# End Source File
# Begin Source File

SOURCE=.\preverifier\typedefs.h
# End Source File
# Begin Source File

SOURCE=.\preverifier\typedefs_md.h
# End Source File
# Begin Source File

SOURCE=.\preverifier\utf.h
# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
