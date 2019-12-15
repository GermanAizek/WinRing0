//-----------------------------------------------------------------------------
//     Author : hiyohiyo
//       Mail : hiyohiyo@crystalmark.info
//        Web : http://openlibsys.org/
//    License : The modified BSD license
//
//                          Copyright 2007 OpenLibSys.org. All rights reserved.
//-----------------------------------------------------------------------------

#include<Windows.h>
#include<tchar.h>
#include "DriverApi.h"

//Driver Id
#define OLS_DRIVER_ID		_T("WinRing0_1_2_0")
//Diever Show Name
#define DRIVER_ID			_T("WinRing0")
//The main Handle
extern HANDLE gHandle;
//-----------------------------------------------------------------------------
//
// Initialize
//
//-----------------------------------------------------------------------------

//please use absolute file path
BOOL Initialize(LPCTSTR DriverPath)
{
	//32 bit system use 32 bit dirver
	//64 bit system use 64 bit dirver
	//BOOL bIsWow64 = FALSE;
	//IsWow64Process(GetCurrentProcess(), &bIsWow64);
	//if (bIsWow64)InstallDriver(DRIVER_ID, DRIVER_x64);
	//else InstallDriver(DRIVER_ID, DRIVER_x86);
	if (InstallDriver(DRIVER_ID, DriverPath))
		if (StartDriver(DRIVER_ID))
			if(OpenDriver())return TRUE;
	return FALSE;
}

//-----------------------------------------------------------------------------
//
// Deinitialize
//
//-----------------------------------------------------------------------------

BOOL Deinitialize()
{
	StopDriver(DRIVER_ID);
	RemoveDriver(DRIVER_ID);
	return TRUE;
}


//-----------------------------------------------------------------------------
//
// Open Driver
//
//-----------------------------------------------------------------------------

BOOL OpenDriver()
{
	gHandle = CreateFile(
		_T("\\\\.\\") OLS_DRIVER_ID,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	int error = GetLastError();

	if (gHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	return TRUE;
}


//-----------------------------------------------------------------------------
//
// Install Driver
//
//-----------------------------------------------------------------------------

BOOL InstallDriver(LPCTSTR DriverId, LPCTSTR DriverPath)
{
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE	hService = NULL;
	BOOL        rCode = FALSE;
	DWORD		error = NO_ERROR;

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		return FALSE;
	}

	hService = CreateService(hSCManager,
		DriverId,
		DriverId,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		DriverPath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
		);

	if (hService == NULL)
	{
		error = GetLastError();
		if (error == ERROR_SERVICE_EXISTS)
		{
			rCode = TRUE;
		}
	}
	else
	{
		rCode = TRUE;
		CloseServiceHandle(hService);
	}
	CloseServiceHandle(hSCManager);
	return rCode;
}


//-----------------------------------------------------------------------------
//
// Remove Driver
//
//-----------------------------------------------------------------------------

BOOL RemoveDriver(LPCTSTR DriverId)
{
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE   hService = NULL;
	BOOL        rCode = FALSE;

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		return FALSE;
	}

	hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);
	if (hService == NULL)
	{
		rCode = TRUE;
	}
	else
	{
		rCode = DeleteService(hService);
		CloseServiceHandle(hService);
	}
	CloseServiceHandle(hSCManager);
	return rCode;
}


//-----------------------------------------------------------------------------
//
// Start Driver
//
//-----------------------------------------------------------------------------

BOOL StartDriver(LPCTSTR DriverId)
{
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE	hService = NULL;
	BOOL		rCode = FALSE;
	DWORD		error = NO_ERROR;

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		return FALSE;
	}

	hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);

	if (hService != NULL)
	{
		if (!StartService(hService, 0, NULL))
		{
			if (GetLastError() == ERROR_SERVICE_ALREADY_RUNNING)
			{
				rCode = TRUE;
			}
		}
		else
		{
			rCode = TRUE;
		}
		CloseServiceHandle(hService);
	}
	CloseServiceHandle(hSCManager);
	return rCode;
}


//-----------------------------------------------------------------------------
//
// Stop Driver
//
//-----------------------------------------------------------------------------

BOOL StopDriver(LPCTSTR DriverId)
{
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE		hService = NULL;
	BOOL			rCode = FALSE;
	SERVICE_STATUS	serviceStatus;
	DWORD		error = NO_ERROR;

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		return FALSE;
	}

	hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);

	if (hService != NULL)
	{
		rCode = ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus);
		CloseServiceHandle(hService);
	}
	CloseServiceHandle(hSCManager);
	return rCode;
}
