//-----------------------------------------------------------------------------
//     Author : hiyohiyo
//       Mail : hiyohiyo@crystalmark.info
//        Web : http://openlibsys.org/
//    License : The modified BSD license
//
//                     Copyright 2007-2008 OpenLibSys.org. All rights reserved.
//-----------------------------------------------------------------------------

#include "OpenLibSys.h"
#include <wdmsec.h>

static ULONG refCount;

/*
Return Value:
	STATUS_SUCCESS if the driver initialized correctly, otherwise an erroror
	indicating the reason for failure.
*/
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	UNICODE_STRING  ntDeviceName = RTL_CONSTANT_STRING(NT_DEVICE_NAME);
	UNICODE_STRING  win32DeviceName = RTL_CONSTANT_STRING(DOS_DEVICE_NAME);
	PDEVICE_OBJECT  deviceObject = NULL;

	// The driver is inherently insecure
	// Let's reduce the attack surface by allowing only Administrators to access the driver
	NTSTATUS status = IoCreateDeviceSecure(
		DriverObject,					// Our Driver Object
		0,								// We don't use a device extension
		&ntDeviceName,					// Device name 
		OLS_TYPE,						// Device type
		FILE_DEVICE_SECURE_OPEN,		// Device characteristics
		FALSE,							// Not an exclusive device
		&SDDL_DEVOBJ_SYS_ALL_ADM_ALL,
		NULL,							// Device class GUID
		&deviceObject);				    // Returned ptr to Device Object

	if (!NT_SUCCESS(status))
	{
		refCount = (ULONG)(-1);
		return status;
	}
	else
	{
		refCount = 0;
	}

	// Initialize the driver object with this driver's entry points.
	DriverObject->MajorFunction[IRP_MJ_CREATE] = OlsDispatch;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = OlsDispatch;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = OlsDispatch;
	DriverObject->DriverUnload = Unload;

	// Create a symbolic link between our device name  and the Win32 name
	status = IoCreateSymbolicLink(&win32DeviceName, &ntDeviceName);

	if (!NT_SUCCESS(status))
	{
		// Delete everything that this routine has allocated.
		IoDeleteDevice(deviceObject);
	}

	return status;
}

/*++
Routine Description:
	This routine is the dispatch handler for the driver.  It is responsible
	for processing the IRPs.

Arguments:
	pDO - Pointer to device object.
	pIrp - Pointer to the current IRP.

Return Value:
	STATUS_SUCCESS if the IRP was processed successfully, otherwise an erroror
	indicating the reason for failure.
--*/
NTSTATUS OlsDispatch(IN PDEVICE_OBJECT pDO, IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDO);
	// Initialize the irp info field.
	// This is used to return the number of bytes transfered.
	pIrp->IoStatus.Information = 0;
	PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

	// Set default return status
	NTSTATUS status = STATUS_NOT_IMPLEMENTED;

	// Dispatch based on major fcn code.
	switch (pIrpStack->MajorFunction)
	{
	case IRP_MJ_CREATE:
		if (refCount != (ULONG)(-1))
		{
			refCount++;
		}
		status = STATUS_SUCCESS;
		break;
	case IRP_MJ_CLOSE:
		if (refCount != (ULONG)(-1))
		{
			refCount--;
		}
		status = STATUS_SUCCESS;
		break;
	case IRP_MJ_DEVICE_CONTROL:
		// Dispatch on IOCTL
		switch (pIrpStack->Parameters.DeviceIoControl.IoControlCode)
		{
		case IOCTL_OLS_GET_DRIVER_VERSION:
			if (pIrpStack->Parameters.DeviceIoControl.OutputBufferLength < 4)
			{
				status = STATUS_BUFFER_TOO_SMALL;
				break;
			}
			*(PULONG)pIrp->AssociatedIrp.SystemBuffer = OLS_DRIVER_VERSION;
			pIrp->IoStatus.Information = 4;
			status = STATUS_SUCCESS;
			break;

		case IOCTL_OLS_GET_REFCOUNT:
			if (pIrpStack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(refCount))
			{
				status = STATUS_BUFFER_TOO_SMALL;
				break;
			}
			*(PULONG)pIrp->AssociatedIrp.SystemBuffer = refCount;
			pIrp->IoStatus.Information = sizeof(refCount);
			status = STATUS_SUCCESS;
			break;

		case IOCTL_OLS_READ_MSR:
			status = ReadMsr(
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
				(ULONG*)&pIrp->IoStatus.Information
			);
			break;
		case IOCTL_OLS_WRITE_MSR:
			status = WriteMsr(
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
				(ULONG*)&pIrp->IoStatus.Information
			);
			break;
		case IOCTL_OLS_READ_PMC:
			status = ReadPmc(
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
				(ULONG*)&pIrp->IoStatus.Information
			);
			break;
		case IOCTL_OLS_HALT:
		{
			{
				status = STATUS_NOT_SUPPORTED;
				break;
			}
			status = STATUS_SUCCESS;
			break;
		}
		case IOCTL_OLS_READ_IO_PORT:
		case IOCTL_OLS_READ_IO_PORT_BYTE:
		case IOCTL_OLS_READ_IO_PORT_WORD:
		case IOCTL_OLS_READ_IO_PORT_DWORD:
			status = ReadIoPort(
				pIrpStack->Parameters.DeviceIoControl.IoControlCode,
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
				(ULONG*)&pIrp->IoStatus.Information
			);
			break;
		case IOCTL_OLS_WRITE_IO_PORT:
		case IOCTL_OLS_WRITE_IO_PORT_BYTE:
		case IOCTL_OLS_WRITE_IO_PORT_WORD:
		case IOCTL_OLS_WRITE_IO_PORT_DWORD:
			status = WriteIoPort(
				pIrpStack->Parameters.DeviceIoControl.IoControlCode,
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
				(ULONG*)&pIrp->IoStatus.Information
			);
			break;

		case IOCTL_OLS_READ_PCI_CONFIG:
			status = ReadPciConfig(
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
				(ULONG*)&pIrp->IoStatus.Information
			);
			break;
		case IOCTL_OLS_WRITE_PCI_CONFIG:
			status = WritePciConfig(
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
				(ULONG*)&pIrp->IoStatus.Information
			);
			break;

		case IOCTL_OLS_READ_MEMORY:
			status = ReadMemory(
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
				(ULONG*)&pIrp->IoStatus.Information
			);
			break;
		case IOCTL_OLS_WRITE_MEMORY:
			status = WriteMemory(
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
				pIrp->AssociatedIrp.SystemBuffer,
				pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
				(ULONG*)&pIrp->IoStatus.Information
			);
			break;


		}
		break;
	}

	// We're done with I/O request.  Record the status of the I/O action.
	pIrp->IoStatus.Status = status;

	// Don't boost priority when returning since this took little time.
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return status;
}

VOID
Unload(
	PDRIVER_OBJECT DriverObject
)
/*++

Routine Description:

	This routine is called by the I/O system to unload the driver.

	Any resources previously allocated must be freed.

Arguments:

	DriverObject - a pointer to the object that represents our driver.

Return Value:

	None
--*/

{
	PDEVICE_OBJECT deviceObject = DriverObject->DeviceObject;
	UNICODE_STRING win32NameString;

	PAGED_CODE();

	// Create counted string version of our Win32 device name.
	RtlInitUnicodeString(&win32NameString, DOS_DEVICE_NAME);

	// Delete the link from our device name to a name in the Win32 namespace.
	IoDeleteSymbolicLink(&win32NameString);

	if (deviceObject != NULL)
	{
		IoDeleteDevice(deviceObject);
	}
}

// CPU Msr registers (Model-specific register)

NTSTATUS
ReadMsr(void* lpInBuffer,
	ULONG	nInBufferSize,
	void* lpOutBuffer,
	ULONG	nOutBufferSize,
	ULONG* lpBytesReturned)
{
	__try
	{
		UNREFERENCED_PARAMETER(nInBufferSize);
		if (nOutBufferSize < 8)
		{
			*lpBytesReturned = 0;
			return  STATUS_BUFFER_TOO_SMALL;
		}
#ifdef _ARM64_
		ULONGLONG data = _ReadStatusReg(*(ULONG*)lpInBuffer);
#else
		ULONGLONG data = __readmsr(*(ULONG*)lpInBuffer);
#endif
		memcpy((PULONG)lpOutBuffer, &data, 8);
		*lpBytesReturned = 8;
		return STATUS_SUCCESS;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		*lpBytesReturned = 0;
		return STATUS_UNSUCCESSFUL;
	}
}

NTSTATUS
WriteMsr(void* lpInBuffer,
	ULONG	nInBufferSize,
	void* lpOutBuffer,
	ULONG	nOutBufferSize,
	ULONG* lpBytesReturned)
{
	__try
	{
		UNREFERENCED_PARAMETER(lpOutBuffer);
		if (BufferSizeCheck(nInBufferSize, nOutBufferSize, lpBytesReturned, sizeof(OLS_WRITE_MSR_INPUT)) < 0) return STATUS_INVALID_PARAMETER;
		OLS_WRITE_MSR_INPUT* param = (OLS_WRITE_MSR_INPUT*)lpInBuffer;

#ifdef _ARM64_
		_WriteStatusReg(param->Register, param->Value.QuadPart);
#else
		__writemsr(param->Register, param->Value.QuadPart);
#endif
		* lpBytesReturned = 0;
		return STATUS_SUCCESS;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		*lpBytesReturned = 0;
		return STATUS_UNSUCCESSFUL;
	}
}


#if (_AMD64_ || __i386__)
NTSTATUS
ReadPmc(void* lpInBuffer,
	ULONG	nInBufferSize,
	void* lpOutBuffer,
	ULONG	nOutBufferSize,
	ULONG* lpBytesReturned)
{
	__try
	{
		if (lpOutBuffer == NULL || nOutBufferSize < 8)
		{
			*lpBytesReturned = 0;
			return  STATUS_BUFFER_TOO_SMALL;
		}

		ULONGLONG data = __readpmc(*(ULONG*)lpInBuffer);
		memcpy((PULONG)lpOutBuffer, &data, 8);
		*lpBytesReturned = 8;
		return STATUS_SUCCESS;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		*lpBytesReturned = 0;
		return STATUS_UNSUCCESSFUL;
	}
}
#endif

//-----------------------------------------------------------------------------
//
// IO Port
//
//-----------------------------------------------------------------------------

NTSTATUS
ReadIoPort(ULONG	ioControlCode,
	void* lpInBuffer,
	ULONG	nInBufferSize,
	void* lpOutBuffer,
	ULONG	nOutBufferSize,
	ULONG* lpBytesReturned)
{
	if (BufferSizeCheck(nInBufferSize, nOutBufferSize, lpBytesReturned, sizeof(ULONG_PTR)) < 0) return STATUS_INVALID_PARAMETER;

	ULONG nPort = *(ULONG*)lpInBuffer;

	switch (ioControlCode)
	{
	case IOCTL_OLS_READ_IO_PORT_BYTE:
		*(PUCHAR)lpOutBuffer = READ_PORT_UCHAR((PUCHAR)(ULONG_PTR)nPort);
		break;
	case IOCTL_OLS_READ_IO_PORT_WORD:
		*(PUSHORT)lpOutBuffer = READ_PORT_USHORT((PUSHORT)(ULONG_PTR)nPort);
		break;
	case IOCTL_OLS_READ_IO_PORT_DWORD:
		*(PULONG)lpOutBuffer = READ_PORT_ULONG((PULONG)(ULONG_PTR)nPort);
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
	void* lpInBuffer,
	ULONG	nInBufferSize,
	void* lpOutBuffer,
	ULONG	nOutBufferSize,
	ULONG* lpBytesReturned)
{
	UNREFERENCED_PARAMETER(lpOutBuffer);
	if (BufferSizeCheck(nInBufferSize, nOutBufferSize, lpBytesReturned, sizeof(OLS_WRITE_IO_PORT_INPUT)) < 0) return STATUS_INVALID_PARAMETER;

	OLS_WRITE_IO_PORT_INPUT* param = (OLS_WRITE_IO_PORT_INPUT*)lpInBuffer;
	ULONG nPort = param->PortNumber;

	switch (ioControlCode)
	{
	case IOCTL_OLS_WRITE_IO_PORT_BYTE:
		WRITE_PORT_UCHAR((PUCHAR)(ULONG_PTR)nPort, param->CharData);
		break;
	case IOCTL_OLS_WRITE_IO_PORT_WORD:
		WRITE_PORT_USHORT((PUSHORT)(ULONG_PTR)nPort, param->ShortData);
		break;
	case IOCTL_OLS_WRITE_IO_PORT_DWORD:
		WRITE_PORT_ULONG((PULONG)(ULONG_PTR)nPort, param->LongData);
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

NTSTATUS
ReadPciConfig(void* lpInBuffer,
	ULONG	nInBufferSize,
	void* lpOutBuffer,
	ULONG	nOutBufferSize,
	ULONG* lpBytesReturned)
{
	OLS_READ_PCI_CONFIG_INPUT* param;
	NTSTATUS status;

	if (BufferSizeCheck(nInBufferSize, nOutBufferSize, lpBytesReturned, sizeof(OLS_READ_PCI_CONFIG_INPUT)) < 0) return STATUS_INVALID_PARAMETER;

	param = (OLS_READ_PCI_CONFIG_INPUT*)lpInBuffer;

	status = pciConfigRead(param->PciAddress, param->PciOffset,
		lpOutBuffer, nOutBufferSize);

	if (status == STATUS_SUCCESS)
	{
		*lpBytesReturned = nOutBufferSize;
	}
	else
	{
		*lpBytesReturned = 0;
	}

	return status;
}

NTSTATUS
WritePciConfig(void* lpInBuffer,
	ULONG nInBufferSize,
	void* lpOutBuffer,
	ULONG nOutBufferSize,
	ULONG* lpBytesReturned)

{
	OLS_WRITE_PCI_CONFIG_INPUT* param;
	ULONG writeSize;

	if (nInBufferSize < offsetof(OLS_WRITE_PCI_CONFIG_INPUT, Data))
	{
		return STATUS_INVALID_PARAMETER;
	}

	param = (OLS_WRITE_PCI_CONFIG_INPUT*)lpInBuffer;
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

NTSTATUS pciConfigRead(ULONG pciAddress, ULONG offset, void* data, int length)
{
	PCI_SLOT_NUMBER slot;
	int error;
	ULONG busNumber;

	busNumber = PciGetBus(pciAddress);
	slot.u.AsULONG = 0;
	slot.u.bits.DeviceNumber = PciGetDev(pciAddress);
	slot.u.bits.FunctionNumber = PciGetFunc(pciAddress);
	error = HalGetBusDataByOffset(PCIConfiguration, busNumber, slot.u.AsULONG,
		data, offset, length);

	if (error == 0)
	{
		return OLS_ERROR_PCI_BUS_NOT_EXIST;
	}
	else if (length != 2 && error == 2)
	{
		return OLS_ERROR_PCI_NO_DEVICE;
	}
	else if (length != error)
	{
		return OLS_ERROR_PCI_READ_CONFIG;
	}

	return STATUS_SUCCESS;
}

NTSTATUS pciConfigWrite(ULONG pciAddress, ULONG offset, void* data, int length)
{
	PCI_SLOT_NUMBER slot;
	int error;
	ULONG busNumber;

	busNumber = PciGetBus(pciAddress);

	slot.u.AsULONG = 0;
	slot.u.bits.DeviceNumber = PciGetDev(pciAddress);
	slot.u.bits.FunctionNumber = PciGetFunc(pciAddress);
	error = HalSetBusDataByOffset(PCIConfiguration, busNumber, slot.u.AsULONG,
		data, offset, length);

	if (error != length)
	{
		return OLS_ERROR_PCI_WRITE_CONFIG;
	}

	return STATUS_SUCCESS;
}


//-----------------------------------------------------------------------------
//
// Physical Memory
//
//-----------------------------------------------------------------------------

NTSTATUS
ReadMemory(void* lpInBuffer,
	ULONG	nInBufferSize,
	void* lpOutBuffer,
	ULONG	nOutBufferSize,
	ULONG* lpBytesReturned)
{
	OLS_READ_MEMORY_INPUT* param;
	ULONG	size;
	PHYSICAL_ADDRESS address;
	PVOID	maped;
	BOOLEAN	error;

	if (nInBufferSize != sizeof(OLS_READ_MEMORY_INPUT))
	{
		return STATUS_INVALID_PARAMETER;
	}

	param = (OLS_READ_MEMORY_INPUT*)lpInBuffer;
	size = param->UnitSize * param->Count;

	if (nOutBufferSize < size)
	{
		return STATUS_INVALID_PARAMETER;
	}

	address.QuadPart = param->Address.QuadPart;

#ifndef _PHYSICAL_MEMORY_SUPPORT

	if (0x000C0000 > address.QuadPart
		|| (address.QuadPart + size - 1) > 0x000FFFFF)
	{
		return STATUS_INVALID_PARAMETER;
	}

#endif

	maped = MmMapIoSpace(address, size, FALSE);

	error = FALSE;
	switch (param->UnitSize) {
	case BYTE:
		READ_REGISTER_BUFFER_UCHAR(maped, lpOutBuffer, param->Count);
		break;
	case TWOBYTES:
		READ_REGISTER_BUFFER_USHORT(maped, lpOutBuffer, param->Count);
		break;
		// TODO: on x64 ULONG == 4 bytes? maybe 8 bytes and rework to x64
#if defined _M_X86
	case FOURBYTES:
		READ_REGISTER_BUFFER_ULONG(maped, lpOutBuffer, param->Count);
		break;
#elif defined _M_X64
	case EIGHTBYTES:
		READ_REGISTER_BUFFER_ULONG(maped, lpOutBuffer, param->Count);
		break;
#endif
	default:
		error = TRUE;
		break;
	}

	MmUnmapIoSpace(maped, size);

	if (error)
	{
		return STATUS_INVALID_PARAMETER;
	}

	*lpBytesReturned = nOutBufferSize;

	return STATUS_SUCCESS;
}

NTSTATUS
WriteMemory(void* lpInBuffer,
	ULONG	nInBufferSize,
	void* lpOutBuffer,
	ULONG	nOutBufferSize,
	ULONG* lpBytesReturned)
{
	UNREFERENCED_PARAMETER(lpInBuffer);
	UNREFERENCED_PARAMETER(nInBufferSize);
	UNREFERENCED_PARAMETER(lpOutBuffer);
	UNREFERENCED_PARAMETER(nOutBufferSize);

#ifdef _PHYSICAL_MEMORY_SUPPORT
	PHYSICAL_ADDRESS address;

	if (nInBufferSize < offsetof(OLS_WRITE_MEMORY_INPUT, Data))
	{
		return STATUS_INVALID_PARAMETER;
	}

	OLS_WRITE_MEMORY_INPUT* param = (OLS_WRITE_MEMORY_INPUT*)lpInBuffer;

	ULONG size = param->UnitSize * param->Count;
	if (nInBufferSize < size + offsetof(OLS_WRITE_MEMORY_INPUT, Data))
	{
		return STATUS_INVALID_PARAMETER;
	}

	address.QuadPart = param->Address.QuadPart;

	PVOID maped = MmMapIoSpace(address, size, FALSE);

	BOOLEAN error = FALSE;
	switch (param->UnitSize) {
	case BYTE:
		WRITE_REGISTER_BUFFER_UCHAR(maped, (UCHAR*)&param->Data, param->Count);
		break;
	case TWOBYTES:
		WRITE_REGISTER_BUFFER_USHORT(maped, (USHORT*)&param->Data, param->Count);
		break;
#if defined _M_X86
	case FOURBYTES:
		READ_REGISTER_BUFFER_ULONG(maped, (ULONG*)&param->Data, param->Count);
		break;
#elif defined _M_X64
	case EIGHTBYTES:
		READ_REGISTER_BUFFER_ULONG(maped, (ULONG*)&param->Data, param->Count);
		break;
#endif
	default:
		error = TRUE;
		break;
	}

	MmUnmapIoSpace(maped, size);

	if (error)
	{
		return STATUS_INVALID_PARAMETER;
	}

	*lpBytesReturned = 0;

	return STATUS_SUCCESS;

#else

	* lpBytesReturned = 0;

	return STATUS_INVALID_PARAMETER;

#endif
}
