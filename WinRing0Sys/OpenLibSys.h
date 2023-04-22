//-----------------------------------------------------------------------------
//     Author : hiyohiyo
//       Mail : hiyohiyo@crystalmark.info
//        Web : http://openlibsys.org/
//    License : The modified BSD license
//
//                     Copyright 2007-2008 OpenLibSys.org. All rights reserved.
//-----------------------------------------------------------------------------

/*
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <WinUser.h>
#undef _SLIST_HEADER_
#include <windows.h>
*/

#undef _RTL_RUN_ONCE_DEF
//#define DECLSPEC_DEPRECATED_DDK_WINXP
#include <ntddk.h>

/******************************************************************************
				0192  *        RtlConvertUlongToLargeInteger   (NTDLL.@)
4c1fa161a… Jon *0193  *
				0194  * Convert a 32 bit unsigned integer into 64 bits.
				0195  *
				0196  * PARAMS
				0197  *  a [I] Number to convert
				0198  *
				0199  * RETURNS
				0200  *  a.
d76f9f963… Alex*0201  */
//ULONGLONG WINAPI RtlConvertUlongToLargeInteger(ULONG a)
//{
//	return a;
//}

//#include <devioctl.h>
//#include <wdm.h>

#ifndef RC_INVOKED
#include <stddef.h>
#endif

#if defined(_MSC_VER)
#include <intrin.h>
#endif

#include "../WinRing0Dll/OlsIoctl.h"

#define NT_DEVICE_NAME	L"\\Device\\WinRing0_1_2_0"
#define DOS_DEVICE_NAME	L"\\DosDevices\\WinRing0_1_2_0"

enum UNIT {
	BYTE = 1,
	TWOBYTES = 2,
	FOURBYTES = 4,
	EIGHTBYTES = 8
};

//-----------------------------------------------------------------------------
//
// Function Prototypes
//
//-----------------------------------------------------------------------------

NTSTATUS	DriverEntry(
				IN PDRIVER_OBJECT DriverObject,
				IN PUNICODE_STRING RegistryPath
			);

NTSTATUS	OlsDispatch(
				IN PDEVICE_OBJECT pDO,
				IN PIRP pIrp
			);

VOID		Unload(
				IN PDRIVER_OBJECT DriverObject
			);

//-----------------------------------------------------------------------------
//
// Function Prototypes for Control Code
//
//-----------------------------------------------------------------------------

NTSTATUS	ReadMsr(
				void *lpInBuffer, 
				ULONG nInBufferSize, 
				void *lpOutBuffer, 
				ULONG nOutBufferSize, 
				ULONG *lpBytesReturned
			);

NTSTATUS	WriteMsr(
				void *lpInBuffer, 
				ULONG nInBufferSize, 
				void *lpOutBuffer, 
				ULONG nOutBufferSize, 
				ULONG *lpBytesReturned
			);
			
NTSTATUS	ReadPmc(
				void *lpInBuffer, 
				ULONG nInBufferSize, 
				void *lpOutBuffer, 
				ULONG nOutBufferSize, 
				ULONG *lpBytesReturned
			);

NTSTATUS	ReadIoPort(
				ULONG ioControlCode,
				void *lpInBuffer, 
				ULONG nInBufferSize, 
				void *lpOutBuffer, 
				ULONG nOutBufferSize, 
				ULONG *lpBytesReturned
			);

NTSTATUS	WriteIoPort(
				ULONG ioControlCode,
				void *lpInBuffer, 
				ULONG nInBufferSize, 
				void *lpOutBuffer, 
				ULONG nOutBufferSize, 
				ULONG *lpBytesReturned
			);

NTSTATUS	ReadPciConfig(
				void *lpInBuffer, 
				ULONG nInBufferSize, 
				void *lpOutBuffer, 
				ULONG nOutBufferSize, 
				ULONG *lpBytesReturned
			);

NTSTATUS	WritePciConfig(
				void *lpInBuffer, 
				ULONG nInBufferSize, 
				void *lpOutBuffer, 
				ULONG nOutBufferSize, 
				ULONG *lpBytesReturned
			);
			
NTSTATUS	ReadMemory(
				void *lpInBuffer, 
				ULONG nInBufferSize, 
				void *lpOutBuffer, 
				ULONG nOutBufferSize, 
				ULONG *lpBytesReturned
			);

NTSTATUS	WriteMemory(
				void *lpInBuffer, 
				ULONG nInBufferSize, 
				void *lpOutBuffer, 
				ULONG nOutBufferSize, 
				ULONG *lpBytesReturned
			);


//-----------------------------------------------------------------------------
//
// Support Function Prototypes
//
//-----------------------------------------------------------------------------

NTSTATUS pciConfigRead(ULONG pciAddress, ULONG offset, void *data, int length);
NTSTATUS pciConfigWrite(ULONG pciAddress, ULONG offset, void *data, int length);

inline NTSTATUS BufferSizeCheck(ULONG nInBufferSize, ULONG nOutBufferSize, ULONG* lpBytesReturned, ULONG size)
{
	if (nInBufferSize == 0)
	{
		*lpBytesReturned = 0;
		return STATUS_INVALID_PARAMETER;
	}
	if (nOutBufferSize < size)
	{
		*lpBytesReturned = 0;
		return STATUS_BUFFER_TOO_SMALL;
	}
	return STATUS_SUCCESS;
}