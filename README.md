# WinRing0
WinRing0 is a hardware access library for Windows.

## Features
WinRing0 library allows x86/x64 Windows applications to access
 * I/O port
 * MSR (Model-Specific Register)
 * PCI

## Copyrights

 * 2007-2009 [hiyohiyo](https://github.com/hiyohiyo) <[hiyohiyo@crystalmark.info](mailto:hiyohiyo@crystalmark.info)> - original author
 * 2019-2024 [GermanAizek](https://github.com/GermanAizek) <[GermanAizek@yandex.ru](mailto:GermanAizek@yandex.ru)> - current maintainer

SPECIAL THANKS
 * 2007 kashiwano masahiro <<http://www.otto.to/~kasiwano/>> - PCI Debug Library for Win32 
 * 2007 Yariv Kaplan, WinIo ([http://www.internals.com/](http://www.internals.com/))
 * 2008 habe - translation support & technical assistance
 * 2009 Patrick - bug patch
 * 2023 [Vitalii Khoruzhyi](https://github.com/Merlin1st) <<merlin_1st@livejournal.com>> - x64 build, additional features and work on code
 * 2023 [Sophie Lemos](https://github.com/SecurityAndStuff) <Press F to pay respects, thanks to microsoft-github> - fixed fatal BSOD error and porting to Windows ARM
 * 2023 [drvexplorer](https://github.com/drvexplorer) - I suggested using fileIoCreateDeviceSecure() and tidying up output files

## How to Use
Put WinRing0.dll, WinRing0x64.dll, WinRing0.sys, WinRing0x64.sys, and WinRing0.vxd into the directory where your application's executable file resides.

### C++
See also sample application.

#### Load-Time Dynamic Linking
 1. Add WinRing0.lib or WinRing0x64.lib to your project.
 2. Add #include "OlsApi.h" statement to your source file.
 3. Call InitializeOls().
 4. Call GetDllStatus() to check error.
 5. Call the library's functions.
 6. Call DeinitializeOls().
 
Reference : [Load-Time Dynamic Linking (MSDN)](https://docs.microsoft.com/en-us/windows/win32/dlls/load-time-dynamic-linking) 

#### Run-Time Dynamic Linking 
 1. Add #include "OlsApiInit.h" statement to your source file.
 2. Call InitOpenLibSys().
 3. Call GetDllStatus() to check error.
 4. Call the library's functions. *
 5. Call DeinitOpenLibSys().
 
*If you would like to call the library's functions on other source files, you should add #include "OlsApiInitExt.h" statement to the source files.
 
Reference : [Run-Time Dynamic Linking (MSDN)](https://docs.microsoft.com/en-us/windows/win32/dlls/run-time-dynamic-linking)

### C#
See also sample application.
 1. Put OpenLibSys.cs into your project.
 2. Add using OpenLibSys; statement to your source file.
 3. Call GetStatus() and GetDllStatus() to check error.
 4. Call the library's functions.
 
*Supported platform target is "x86", "x64" and "Any CPU". But WinRing0 does not support "IA64".

## How to Build (Requirement)

### Sample Code

#### C++ and C#
Required Visual Studio 2015 Community or higher because of using MFC.

### SYS (NT Driver)
Required WDK (Windows Driver Kits)
 1. [Get WDK from MSDN](https://docs.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk)
 2. Install WDK
 3. cd WinRing0/source/dll/sys
 4. build :trollface:

### VxD (9x Driver)
Required Windows XP SP1 DDK + Win Me support
 1. Get Windows XP SP1 DDK from MSDN
 2. Install Windows XP SP1 DDK with Win Me support
 3. Open "Win Me Free Build Environment"
 4. cd WinRing0\source\dll\vxd
 5. nmake
