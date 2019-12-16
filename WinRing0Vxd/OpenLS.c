//-----------------------------------------------------------------------------
//     Author : hiyohiyo
//       Mail : hiyohiyo@crystalmark.info
//        Web : http://openlibsys.org/
//    License : The modified BSD license
//
//                     Copyright 2007-2008 OpenLibSys.org. All rights reserved.
//-----------------------------------------------------------------------------

#define WIN40COMPAT   1
#define WANTVXDWRAPS

#include <basedef.h>
#include <vmm.h>

#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG

#include <vwin32.h>
#include <vpicd.h>
#include <vxdwraps.h>
#include <winerror.h>
#include <stddef.h>

#include "OpenLS.h"
#include "OlsIoctl.h"

//-----------------------------------------------------------------------------
//
// W32_DeviceIOControl
//
//-----------------------------------------------------------------------------

DWORD _stdcall W32_DeviceIOControl(DWORD	dwService,
                                   DWORD	dwDDB,
                                   DWORD	hDevice,
                                   LPDIOC	lpDIOCParms)
{
    DWORD dwRetVal = 0;
	int		count;
	int		index;

	switch(dwService)
	{
		case DIOC_OPEN:
			dwRetVal = NO_ERROR;
			break;
			case DIOC_CLOSEHANDLE:
			dwRetVal = CleanUp();
			break;
		case IOCTL_OLS_GET_DRIVER_VERSION:
			*(PULONG)lpDIOCParms->lpvOutBuffer = OLS_DRIVER_VERSION;
			*(PULONG)lpDIOCParms->lpcbBytesReturned = 4;
			dwRetVal = NO_ERROR;
			break;

		case IOCTL_OLS_READ_MSR:
			dwRetVal = ReadMsr(
				(void *)lpDIOCParms->lpvInBuffer,
				lpDIOCParms->cbInBuffer,
				(void *)lpDIOCParms->lpvOutBuffer,
				lpDIOCParms->cbOutBuffer,
				(void *)lpDIOCParms->lpcbBytesReturned
				);
			break;
		case IOCTL_OLS_WRITE_MSR:
			dwRetVal = WriteMsr(
				(void *)lpDIOCParms->lpvInBuffer,
				lpDIOCParms->cbInBuffer,
				(void *)lpDIOCParms->lpvOutBuffer,
				lpDIOCParms->cbOutBuffer,
				(void *)lpDIOCParms->lpcbBytesReturned
				);
			break;
		case IOCTL_OLS_READ_PMC:
			dwRetVal = ReadPmc(
				(void *)lpDIOCParms->lpvInBuffer,
				lpDIOCParms->cbInBuffer,
				(void *)lpDIOCParms->lpvOutBuffer,
				lpDIOCParms->cbOutBuffer,
				(void *)lpDIOCParms->lpcbBytesReturned
				);
			break;
		case IOCTL_OLS_HALT:
			__asm hlt;
			dwRetVal = NO_ERROR;
			break;

		case IOCTL_OLS_READ_IO_PORT:
		case IOCTL_OLS_READ_IO_PORT_BYTE:
		case IOCTL_OLS_READ_IO_PORT_WORD:
		case IOCTL_OLS_READ_IO_PORT_DWORD:
			dwRetVal = ReadIoPort(
				dwService,
				(void *)lpDIOCParms->lpvInBuffer,
				lpDIOCParms->cbInBuffer,
				(void *)lpDIOCParms->lpvOutBuffer,
				lpDIOCParms->cbOutBuffer,
				(void *)lpDIOCParms->lpcbBytesReturned
				);
			break;
		case IOCTL_OLS_WRITE_IO_PORT:
		case IOCTL_OLS_WRITE_IO_PORT_BYTE:
		case IOCTL_OLS_WRITE_IO_PORT_WORD:
		case IOCTL_OLS_WRITE_IO_PORT_DWORD:
			dwRetVal = WriteIoPort(
				dwService,
				(void *)lpDIOCParms->lpvInBuffer,
				lpDIOCParms->cbInBuffer,
				(void *)lpDIOCParms->lpvOutBuffer,
				lpDIOCParms->cbOutBuffer,
				(void *)lpDIOCParms->lpcbBytesReturned
				);
			break;

		case IOCTL_OLS_READ_PCI_CONFIG:
	        dwRetVal = ReadPciConfig(
				(void *)lpDIOCParms->lpvInBuffer,
				lpDIOCParms->cbInBuffer,
				(void *)lpDIOCParms->lpvOutBuffer,
				lpDIOCParms->cbOutBuffer,
				(void *)lpDIOCParms->lpcbBytesReturned
				);
			break;
		case IOCTL_OLS_WRITE_PCI_CONFIG:
	        dwRetVal = WritePciConfig(
				(void *)lpDIOCParms->lpvInBuffer,
				lpDIOCParms->cbInBuffer,
				(void *)lpDIOCParms->lpvOutBuffer,
				lpDIOCParms->cbOutBuffer,
				(void *)lpDIOCParms->lpcbBytesReturned
				);
			break;

		case IOCTL_OLS_READ_MEMORY:
	        dwRetVal = ReadMemory(
				(void *)lpDIOCParms->lpvInBuffer,
				lpDIOCParms->cbInBuffer,
				(void *)lpDIOCParms->lpvOutBuffer,
				lpDIOCParms->cbOutBuffer,
				(void *)lpDIOCParms->lpcbBytesReturned
				);
			break;
		case IOCTL_OLS_WRITE_MEMORY:
	        dwRetVal = WriteMemory(
				(void *)lpDIOCParms->lpvInBuffer,
				lpDIOCParms->cbInBuffer,
				(void *)lpDIOCParms->lpvOutBuffer,
				lpDIOCParms->cbOutBuffer,
				(void *)lpDIOCParms->lpcbBytesReturned
				);
			break;

		default:
	        dwRetVal = ERROR_NOT_SUPPORTED;
    }
    return dwRetVal;
}

//-----------------------------------------------------------------------------
//
// CPU
//
//-----------------------------------------------------------------------------

NTSTATUS
ReadMsr(void	*lpInBuffer, 
		ULONG	nInBufferSize, 
		void	*lpOutBuffer, 
		ULONG	nOutBufferSize, 
		ULONG	*lpBytesReturned)
{
	ULONG ulECX = *(ULONG*)lpInBuffer;
	ULONG ulEAX = 0;
	ULONG ulEDX = 0;

	_asm
	{
		mov ecx, ulECX
		rdmsr
		mov ulEAX, eax
		mov ulEDX, edx
	}
	memcpy((PULONG)lpOutBuffer, &ulEAX, 4);
	memcpy((PULONG)lpOutBuffer+1, &ulEDX, 4);

	*lpBytesReturned = 8;
    return STATUS_SUCCESS;
}


NTSTATUS
WriteMsr(	void	*lpInBuffer, 
			ULONG	nInBufferSize, 
			void	*lpOutBuffer, 
			ULONG	nOutBufferSize, 
			ULONG	*lpBytesReturned)
{
	OLS_WRITE_MSR_INPUT* param = (OLS_WRITE_MSR_INPUT*)lpInBuffer;

	ULONG ulECX = param->Register;
	ULONG ulEAX = param->Value.LowPart;
	ULONG ulEDX = param->Value.HighPart;

	_asm
	{
		mov eax, ulEAX
		mov edx, ulEDX
		mov ecx, ulECX
		wrmsr
	}
	*lpBytesReturned = 0;
	return STATUS_SUCCESS;
}

NTSTATUS
ReadPmc(void	*lpInBuffer, 
		ULONG	nInBufferSize, 
		void	*lpOutBuffer, 
		ULONG	nOutBufferSize, 
		ULONG	*lpBytesReturned)
{
	ULONG ulECX = *(ULONG*)lpInBuffer;
	ULONG ulEAX = 0;
	ULONG ulEDX = 0;

	_asm
	{
		mov ecx, ulECX
		rdpmc
		mov ulEAX, eax
		mov ulEDX, edx
	}
	memcpy((PULONG)lpOutBuffer, &ulEAX, 4);
	memcpy((PULONG)lpOutBuffer+1, &ulEDX, 4);

	*lpBytesReturned = 8;
	
    return STATUS_SUCCESS;
}

//-----------------------------------------------------------------------------
//
// I/O Port
//
//-----------------------------------------------------------------------------

NTSTATUS
ReadIoPort( ULONG	ioControlCode, 
			void	*lpInBuffer, 
			ULONG	nInBufferSize, 
			void 	*lpOutBuffer, 
			ULONG	nOutBufferSize, 
			ULONG	*lpBytesReturned)
{
	ULONG nPort = *(ULONG*)lpInBuffer;

	switch(ioControlCode)
	{
		case IOCTL_OLS_READ_IO_PORT_BYTE:
			*(PUCHAR)lpOutBuffer = _inp((USHORT)nPort);
			break;
		case IOCTL_OLS_READ_IO_PORT_WORD:
			*(PUSHORT)lpOutBuffer = _inpw((USHORT)nPort);
			break;
		case IOCTL_OLS_READ_IO_PORT_DWORD:
			*(PULONG)lpOutBuffer = _inpd((USHORT)nPort);
			break;
		default:
			*lpBytesReturned = 0;
			return STATUS_INVALID_PARAMETER;
			break;
	}
	
	*lpBytesReturned = nInBufferSize;
	return STATUS_SUCCESS;
}

NTSTATUS
WriteIoPort(ULONG	ioControlCode, 
			void	*lpInBuffer, 
			ULONG	nInBufferSize, 
			void	*lpOutBuffer, 
			ULONG	nOutBufferSize, 
			ULONG	*lpBytesReturned)
{
	ULONG nPort;
	OLS_WRITE_IO_PORT_INPUT* param;
	
	param = (OLS_WRITE_IO_PORT_INPUT*)lpInBuffer;
	nPort = param->PortNumber;

	switch(ioControlCode)
	{
		case IOCTL_OLS_WRITE_IO_PORT_BYTE:
			_outp((USHORT)nPort, param->CharData);
			break;
		case IOCTL_OLS_WRITE_IO_PORT_WORD:
			_outpw((USHORT)nPort, param->ShortData);
			break;
		case IOCTL_OLS_WRITE_IO_PORT_DWORD:
			_outpd((USHORT)nPort, param->LongData);
			break;
		default:
			return STATUS_INVALID_PARAMETER;
			break;
	}

	return STATUS_SUCCESS;
}

//-----------------------------------------------------------------------------
//
// PCI
//
//-----------------------------------------------------------------------------

DWORD
ReadPciConfig(	void	*lpInBuffer, 
				ULONG	nInBufferSize, 
				void	*lpOutBuffer, 
				ULONG	nOutBufferSize, 
				ULONG	*lpBytesReturned)
{
	OLS_READ_PCI_CONFIG_INPUT *param;
	DWORD status;

    if(nInBufferSize != sizeof(OLS_READ_PCI_CONFIG_INPUT))
    {
        return STATUS_INVALID_PARAMETER;
    }
	param	= (OLS_READ_PCI_CONFIG_INPUT *) lpInBuffer;

	status = pciConfigRead(param->PciAddress, param->PciOffset,
						lpOutBuffer, nOutBufferSize);

	if(status == STATUS_SUCCESS) {
	    *lpBytesReturned = nOutBufferSize;
    } else {
	    *lpBytesReturned = 0;
	}

    return status;
}

NTSTATUS
WritePciConfig(	void	*lpInBuffer, 
				ULONG	nInBufferSize, 
				void	*lpOutBuffer, 
				ULONG	nOutBufferSize, 
				ULONG	*lpBytesReturned)

{
	OLS_WRITE_PCI_CONFIG_INPUT *param;
	ULONG writeSize;
	NTSTATUS status;

	if(nInBufferSize < offsetof(OLS_WRITE_PCI_CONFIG_INPUT, Data))
	{
		return STATUS_INVALID_PARAMETER;
	}

	param = (OLS_WRITE_PCI_CONFIG_INPUT *)lpInBuffer;
	writeSize = nInBufferSize - offsetof(OLS_WRITE_PCI_CONFIG_INPUT, Data);
	
	*lpBytesReturned = 0;

	return pciConfigWrite(param->PciAddress, param->PciOffset,
							&param->Data, writeSize);
}

//-----------------------------------------------------------------------------
//
// Support Function
//
//-----------------------------------------------------------------------------

NTSTATUS
pciConfigRead(ULONG pciAddress, ULONG loffsetadd, char *data, int len)
{
	BYTE status, maxbusnumber;
	WORD offsetadd = loffsetadd;

	while(len > 0)
	{
		if(len >=4 && (offsetadd & 3) == 0)
		{
			DWORD d;
			__asm mov  ax, word ptr 0xb10a;  
			__asm mov  bx, word ptr pciAddress;
			__asm mov  di, offsetadd
			CallPciBios();
			__asm mov  d, ecx;
			__asm mov  status, ah;
			*(DWORD*)data = d;
			data += 4;
			len -= 4;
			offsetadd += 4;
		}
		else if(len >=2 && (offsetadd & 1) == 0)
		{
			WORD d;
			__asm mov  ax, word ptr 0xb109;  
			__asm mov  bx, word ptr pciAddress;
			__asm mov  di, offsetadd;
			CallPciBios();
			__asm mov  d, cx;
			__asm mov  status, ah;
			*(WORD*)data = d;
			data += 2;
			len -= 2;
			offsetadd += 2;
		}
		else
		{
			BYTE d;
			__asm mov  ax, word ptr 0xb108;  
			__asm mov  bx, word ptr pciAddress;
			__asm mov  di, offsetadd;
			CallPciBios();
			__asm mov  d, cl;
			__asm mov  status, ah;
			*(WORD*)data = d;
			data += 1;
			len -= 1;
			offsetadd += 1;
		}
		
		if(status != 0)
		{
			return STATUS_UNSUCCESSFUL;
		}
	}
	return STATUS_SUCCESS;
}

NTSTATUS
pciConfigWrite(ULONG pciAddress, ULONG loffsetadd, char *data, int len)
{
	BYTE status, maxbusnumber;
	WORD offsetadd = loffsetadd;

	while(len >0)
	{
		if(len >=4 && (offsetadd & 3) ==0)
		{
			DWORD d = *(DWORD*)data;
			__asm mov  ax, word ptr 0xb10d;  
			__asm mov  bx, word ptr pciAddress;
			__asm mov  di, offsetadd;
			__asm mov  ecx, d;
			CallPciBios();
			__asm mov status, ah
			data += 4;
			len -= 4;
			offsetadd += 4;
		}
		else if(len >=2 && (offsetadd & 1) == 0)
		{
			WORD d = *(DWORD*)data;
			__asm mov  ax, word ptr 0xb10c;  
			__asm mov  bx, word ptr pciAddress;
			__asm mov  di, offsetadd;
			__asm mov  cx, d;
			CallPciBios();
			__asm mov status, ah
			data += 2;
			len -= 2;
			offsetadd += 2;
		}
		else
		{
			BYTE d = *(DWORD*)data;
			__asm mov  ax, word ptr 0xb10b;  
			__asm mov  bx, word ptr pciAddress;
			__asm mov  di, offsetadd;
			__asm mov  cl, d ;
			CallPciBios();
			__asm mov status, ah
			data += 1;
			len -= 1;
			offsetadd += 1;
		}
		
		if(status != 0)
		{
			 return STATUS_UNSUCCESSFUL;
		}
	}
	return STATUS_SUCCESS;
}	

//-----------------------------------------------------------------------------
//
// Physical Memory
//
//-----------------------------------------------------------------------------

NTSTATUS
ReadMemory(	void	*lpInBuffer, 
			ULONG	nInBufferSize, 
			void	*lpOutBuffer, 
			ULONG	nOutBufferSize, 
			ULONG	*lpBytesReturned)
{
	int i;
	ULONG	size;
	PVOID	maped;
	OLS_READ_MEMORY_INPUT *param;
	
	param = (OLS_READ_MEMORY_INPUT *)lpInBuffer;

	if(nInBufferSize != sizeof(OLS_READ_MEMORY_INPUT))
	{
		return STATUS_INVALID_PARAMETER;
	}

	size = param->UnitSize * param->Count;

	if(nOutBufferSize < size)
	{
		return STATUS_INVALID_PARAMETER;
	}

#ifndef _PHYSICAL_MEMORY_SUPPORT

	if(0x000C0000 > param->Address.LowPart 
	|| (param->Address.LowPart + size - 1) > 0x000FFFFF)
	{
		return STATUS_INVALID_PARAMETER;
	}

#endif

	maped = _MapPhysToLinear(param->Address.LowPart, size, 0);

	switch(param->UnitSize)
	{
		case 1:
			for(i = 0; i< param->Count; i++){
				 ((UCHAR*)lpOutBuffer)[i] = ((UCHAR*)maped)[i];
			}
			break;
		case 2:
			for(i = 0; i< param->Count; i++){
				((USHORT*)lpOutBuffer)[i] = ((USHORT*)maped)[i];
			}
			break;
		case 4:
			for(i = 0; i< param->Count; i++){
				((ULONG*)lpOutBuffer)[i] = ((ULONG*)maped)[i];
			}
			break;
		default:
			return STATUS_INVALID_PARAMETER;
			break;
	}

	*lpBytesReturned = nOutBufferSize;
	return STATUS_SUCCESS;
}

NTSTATUS
WriteMemory(void	*lpInBuffer, 
			ULONG	nInBufferSize, 
			void	*lpOutBuffer, 
			ULONG	nOutBufferSize, 
			ULONG	*lpBytesReturned)
{
#ifdef _PHYSICAL_MEMORY_SUPPORT

	OLS_WRITE_MEMORY_INPUT *param;
	ULONG	size;
	PVOID	maped;
	int i;

	if(nInBufferSize < offsetof(OLS_WRITE_MEMORY_INPUT, Data))
	{
		return STATUS_INVALID_PARAMETER;
	}

	param = (OLS_WRITE_MEMORY_INPUT *)lpInBuffer;

	size = param->UnitSize * param->Count;
	if(nInBufferSize < size + offsetof(OLS_WRITE_MEMORY_INPUT, Data))
	{
		return STATUS_INVALID_PARAMETER;
	}

	maped = _MapPhysToLinear(param->Address.LowPart, size, 0);

	switch(param->UnitSize)
	{
		case 1:
			for(i = 0; i< param->Count; i++){
				 ((UCHAR*)maped)[i] = ((UCHAR*)param->Data)[i];
			}
			break;
		case 2:
			for(i = 0; i< param->Count; i++){
				((USHORT*)maped)[i] = ((USHORT*)param->Data)[i];
			}
			break;
		case 4:
			for(i = 0; i< param->Count; i++){
				((ULONG*)maped)[i] = ((ULONG*)param->Data)[i];
			}
			break;
		default:
			return STATUS_INVALID_PARAMETER;
			break;
	}

	*lpBytesReturned = 0;
	
	return STATUS_SUCCESS;

#else

	*lpBytesReturned = 0;
	
	return STATUS_INVALID_PARAMETER;

#endif

}
