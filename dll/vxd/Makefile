#-----------------------------------------------------------------------------
#     Author : hiyohiyo
#       Mail : hiyohiyo@crystalmark.info
#        Web : http://openlibsys.org/
#    License : The modified BSD license
#
#                                Copyright 2007 hiyohiyo, All rights reserved.
#-----------------------------------------------------------------------------
#
# How to build on Windows XP SP1 DDK (Win Me Free Build Environment)
#
# 0. Get Windows XP SP1 DDK from MSDN
# 1. Install Windows XP SP1 DDK with Win Me support
# 2. Open "Win Me Free Build Environment"
# 3. cd (WinRing0)\source\dll\vxd
# 4. nmake
#
# Compile Options
#_OPEN_LIB_SYS=

!ifdef _OPEN_LIB_SYS
VXDFILE = OpenLibSys.vxd
!else
VXDFILE = WinRing0.vxd
!endif

OBJDIR = bin
VXDWRAPSDIR = $(DDKROOT)\lib\win_me\

RC16 = $(DDKROOT)\bin\win_me\bin16\rc
ML   = $(DDKROOT)\bin\win_me\bin\ml
CL   = $(DDKROOT)\bin\x86\cl
LINK = $(DDKROOT)\bin\x86\link

!ifdef DEBUG
DDEBUG  =-DDEBLEVEL=1 -DDEBUG
!else
DDEBUG  =-DDEBLEVEL=0
!endif

CFLAGS  = -Zdp -Gs -c -DIS_32 $(DDEBUG)
LFLAGS  = -machine:i386 -debug:none kernel32.lib 
AFLAGS  = -coff -DBLD_COFF -DIS_32 -W2 -Zd -c -Cx -DMASM6

all: direxist $(OBJDIR)\OpenLS.vxd

direxist:
        if not exist $(OBJDIR)\nul    md $(OBJDIR)

$(OBJDIR)\OpenLS.res: OpenLS.rc
        $(RC16) -Fo$@ -r OpenLS.rc

$(OBJDIR)\OpenLS.obj: OpenLS.c
        $(CL) $(CFLAGS) -Fo$@ %s

$(OBJDIR)\ctrl.obj: ctrl.asm
        $(ML) $(AFLAGS) -Fo$@ %s

$(OBJDIR)\OpenLS.vxd: $(OBJDIR)\OpenLS.obj $(OBJDIR)\ctrl.obj\
                $(OBJDIR)\OpenLS.res OpenLS.def
        echo >NUL @<<OpenLS.crf
-MACHINE:i386 -DEBUG:NONE
-DEF:OpenLS.def -OUT:$(OBJDIR)\OpenLS.vxd -MAP:$(OBJDIR)\OpenLS.map
-VXD $(VXDWRAPSDIR)\vxdwraps.clb $(OBJDIR)\OpenLS.obj $(OBJDIR)\ctrl.obj 
<<
        $(LINK) @OpenLS.crf
        cd $(OBJDIR)
        mapsym OpenLS
        adrc2vxd OpenLS.vxd OpenLS.res
        copy /Y OpenLS.vxd ..\..\..\..\release\$(VXDFILE)
        cd ..
        
clean:
        -@if exist $(OBJDIR)\*.obj del $(OBJDIR)\*.obj
        -@if exist $(OBJDIR)\*.res del $(OBJDIR)\*.res
        -@if exist $(OBJDIR)\*.vxd del $(OBJDIR)\*.vxd
        -@if exist $(OBJDIR)\*.exp del $(OBJDIR)\*.exp
        -@if exist $(OBJDIR)\*.lib del $(OBJDIR)\*.lib
        -@if exist $(OBJDIR)\*.map del $(OBJDIR)\*.map
        -@if exist $(OBJDIR)\*.sym del $(OBJDIR)\*.sym
        -@if exist $(OBJDIR)\*.exe del $(OBJDIR)\*.exe
        -@if exist *.bak del *.bak
        -@if exist *.aps del *.aps
