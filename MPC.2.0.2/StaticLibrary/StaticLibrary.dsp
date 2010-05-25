# Microsoft Developer Studio Project File - Name="StaticLibrary" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=StaticLibrary - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "StaticLibrary.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "StaticLibrary.mak" CFG="StaticLibrary - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "StaticLibrary - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "StaticLibrary - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "StaticLibrary - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /c
# ADD BASE RSC /l 0x41a /d "NDEBUG"
# ADD RSC /l 0x41a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "StaticLibrary - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x41a /d "_DEBUG"
# ADD RSC /l 0x41a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "StaticLibrary - Win32 Release"
# Name "StaticLibrary - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\structures\block.c
# End Source File
# Begin Source File

SOURCE=..\classgen\bytecode.c
# End Source File
# Begin Source File

SOURCE=..\preverifier\check_class.c
# End Source File
# Begin Source File

SOURCE=..\preverifier\check_code.c
# End Source File
# Begin Source File

SOURCE=..\classgen\classgen.c
# End Source File
# Begin Source File

SOURCE=..\preverifier\classloader.c
# End Source File
# Begin Source File

SOURCE=..\preverifier\classresolver.c
# End Source File
# Begin Source File

SOURCE=..\classgen\constant_pool.c
# End Source File
# Begin Source File

SOURCE=..\preverifier\convert_md.c
# End Source File
# Begin Source File

SOURCE=..\util\error.c
# End Source File
# Begin Source File

SOURCE=..\preverifier\file.c
# End Source File
# Begin Source File

SOURCE=..\structures\identifier.c
# End Source File
# Begin Source File

SOURCE=..\preverifier\inlinejsr.c
# End Source File
# Begin Source File

SOURCE=..\preverifier\jar.c
# End Source File
# Begin Source File

SOURCE=..\preverifier\jar_support.c
# End Source File
# Begin Source File

SOURCE=..\lex\lex.yy.c
# End Source File
# Begin Source File

SOURCE=..\preverifier\main.c
# End Source File
# Begin Source File

SOURCE=..\util\memory.c
# End Source File
# Begin Source File

SOURCE=..\structures\name_table.c
# End Source File
# Begin Source File

SOURCE=..\parser\parser.c
# End Source File
# Begin Source File

SOURCE=..\classgen\preverify.c
# End Source File
# Begin Source File

SOURCE=..\main\static_entry.c
# End Source File
# Begin Source File

SOURCE=..\parser\stdpas.c
# End Source File
# Begin Source File

SOURCE=..\structures\string_list.c
# End Source File
# Begin Source File

SOURCE=..\util\strings.c
# End Source File
# Begin Source File

SOURCE=..\preverifier\stubs.c
# End Source File
# Begin Source File

SOURCE=..\preverifier\sys_support.c
# End Source File
# Begin Source File

SOURCE=..\structures\type.c
# End Source File
# Begin Source File

SOURCE=..\structures\type_list.c
# End Source File
# Begin Source File

SOURCE=..\structures\unit.c
# End Source File
# Begin Source File

SOURCE=..\preverifier\utf.c
# End Source File
# Begin Source File

SOURCE=..\preverifier\util.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\structures\block.h
# End Source File
# Begin Source File

SOURCE=..\classgen\bytecode.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\check_code.h
# End Source File
# Begin Source File

SOURCE=..\classgen\classgen.h
# End Source File
# Begin Source File

SOURCE=..\classgen\constant_pool.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\convert_md.h
# End Source File
# Begin Source File

SOURCE=..\util\error.h
# End Source File
# Begin Source File

SOURCE=..\structures\identifier.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\jar.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\jarint.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\jartables.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\locale_md.h
# End Source File
# Begin Source File

SOURCE=..\util\memory.h
# End Source File
# Begin Source File

SOURCE=..\util\message.h
# End Source File
# Begin Source File

SOURCE=..\structures\name_table.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\oobj.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\opcodes.h
# End Source File
# Begin Source File

SOURCE=..\parser\parser.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\path.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\path_md.h
# End Source File
# Begin Source File

SOURCE=..\classgen\preverify.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\signature.h
# End Source File
# Begin Source File

SOURCE=..\main\static_entry.h
# End Source File
# Begin Source File

SOURCE=..\parser\stdpas.h
# End Source File
# Begin Source File

SOURCE=..\structures\string_list.h
# End Source File
# Begin Source File

SOURCE=..\util\strings.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\sys_api.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\sysmacros_md.h
# End Source File
# Begin Source File

SOURCE=..\lex\tokens.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\tree.h
# End Source File
# Begin Source File

SOURCE=..\structures\type.h
# End Source File
# Begin Source File

SOURCE=..\structures\type_list.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\typecodes.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\typedefs.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\typedefs_md.h
# End Source File
# Begin Source File

SOURCE=..\structures\unit.h
# End Source File
# Begin Source File

SOURCE=..\preverifier\utf.h
# End Source File
# End Group
# End Target
# End Project
