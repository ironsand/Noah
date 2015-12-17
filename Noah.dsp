# Microsoft Developer Studio Project File - Name="Noah" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Noah - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "Noah.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "Noah.mak" CFG="Noah - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "Noah - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "Noah - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Noah"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Noah - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "tmp/rel"
# PROP Intermediate_Dir "tmp/rel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /O1 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_VISUALC" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib lz32.lib version.lib /nologo /entry:"kilib_startUp" /subsystem:windows /pdb:none /machine:I386 /out:"Release/Noah.exe" /opt:"nowin98"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "Noah - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "tmp/dbg"
# PROP Intermediate_Dir "tmp/dbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_VISUALC" /D "KILIB_LOG" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib lz32.lib version.lib /nologo /entry:"kilib_startUp" /subsystem:windows /debug /machine:I386 /out:"Debug/Noah.exe" /pdbtype:sept
# SUBTRACT LINK32 /profile /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "Noah - Win32 Release"
# Name "Noah - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ArcACE.cpp
# End Source File
# Begin Source File

SOURCE=.\ArcB2e.cpp
# End Source File
# Begin Source File

SOURCE=.\ArcCpt.cpp
# End Source File
# Begin Source File

SOURCE=.\ArcDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\Archiver.cpp
# End Source File
# Begin Source File

SOURCE=.\ArcMsc.cpp
# End Source File
# Begin Source File

SOURCE=.\Noah.cpp
# End Source File
# Begin Source File

SOURCE=.\NoahAM.cpp
# End Source File
# Begin Source File

SOURCE=.\NoahCM.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SubDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ArcACE.h
# End Source File
# Begin Source File

SOURCE=.\ArcB2e.h
# End Source File
# Begin Source File

SOURCE=.\ArcCpt.h
# End Source File
# Begin Source File

SOURCE=.\ArcDLL.h
# End Source File
# Begin Source File

SOURCE=.\Archiver.h
# End Source File
# Begin Source File

SOURCE=.\ArcMsc.h
# End Source File
# Begin Source File

SOURCE=.\NoahAM.h
# End Source File
# Begin Source File

SOURCE=.\NoahApp.h
# End Source File
# Begin Source File

SOURCE=.\NoahCM.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SubDlg.h
# End Source File
# Begin Source File

SOURCE=.\unacedef.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icons\arc_b2e.ico
# End Source File
# Begin Source File

SOURCE=.\b2e.ico
# End Source File
# Begin Source File

SOURCE=.\default1.bin
# End Source File
# Begin Source File

SOURCE=.\noah.ico
# End Source File
# Begin Source File

SOURCE=.\Noah.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "K.I.LIB"

# PROP Default_Filter ""
# Begin Group "Source"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\kilib\kl_app.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_carc.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_cmd.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_dnd.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_file.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_find.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_reg.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_rythp.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_str.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_wcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_wnd.cpp
# End Source File
# End Group
# Begin Group "Header"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\kilib\kl_app.h
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_carc.h
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_cmd.h
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_dnd.h
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_file.h
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_find.h
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_misc.h
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_reg.h
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_rythp.h
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_str.h
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_wcmn.h
# End Source File
# Begin Source File

SOURCE=.\kilib\kl_wnd.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\kilib\kilib.h
# End Source File
# Begin Source File

SOURCE=.\kilib\kilibext.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\manifest.xml
# End Source File
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# Begin Source File

SOURCE=.\Release\todo.txt
# End Source File
# End Target
# End Project
