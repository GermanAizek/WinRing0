//-----------------------------------------------------------------------------
//     Author : hiyohiyo
//       Mail : hiyohiyo@crystalmark.info
//        Web : http://openlibsys.org/
//    License : The modified BSD license
//
//                          Copyright 2007 OpenLibSys.org. All rights reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Driver.h"
#include <tchar.h>
#include "OlsDll.h"
#include "OlsDef.h"

extern HANDLE gHandle;

static BOOL InstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverId, LPCTSTR DriverPath);
static BOOL RemoveDriver(SC_HANDLE hSCManager, LPCTSTR DriverId);
static BOOL StartDriver(SC_HANDLE hSCManager, LPCTSTR DriverId);
static BOOL StopDriver(SC_HANDLE hSCManager, LPCTSTR DriverId);
static BOOL SystemInstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverId, LPCTSTR DriverPath);
static BOOL IsSystemInstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverId, LPCTSTR DriverPath);

DWORD ManageDriver(LPCTSTR DriverId, LPCTSTR DriverPath, USHORT Function)
{
    SC_HANDLE	hSCManager = NULL;
    BOOL		rCode = FALSE;
    DWORD		error = NO_ERROR;

    if (DriverId == NULL || DriverPath == NULL)
    {
        return OLS_DLL_DRIVER_INVALID_PARAM;
    }
    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCManager == NULL)
    {
        return OLS_DLL_DRIVER_SC_MANAGER_NOT_OPENED;
    }

    switch (Function)
    {
    case OLS_DRIVER_INSTALL:
        if (InstallDriver(hSCManager, DriverId, DriverPath))
        {
            if (StartDriver(hSCManager, DriverId))
            {
                rCode = OLS_DLL_NO_ERROR;
            }
            else
            {
                rCode = OLS_DLL_DRIVER_SC_DRIVER_NOT_STARTED;
            }
        }
        else
        {
            rCode = OLS_DLL_DRIVER_SC_DRIVER_NOT_INSTALLED;
        }
        break;
    case OLS_DRIVER_REMOVE:
        if (!IsSystemInstallDriver(hSCManager, DriverId, DriverPath))
        {
            StopDriver(hSCManager, DriverId);
            if (RemoveDriver(hSCManager, DriverId))
            {
                rCode = OLS_DLL_NO_ERROR;
            }
            else
            {
                rCode = OLS_DLL_DRIVER_SC_DRIVER_NOT_REMOVED;
            }
        }
        break;
    case OLS_DRIVER_SYSTEM_INSTALL:
        if (IsSystemInstallDriver(hSCManager, DriverId, DriverPath))
        {
            rCode = OLS_DLL_NO_ERROR;
        }
        else
        {
            if (!OpenDriver())
            {
                StopDriver(hSCManager, DriverId);
                RemoveDriver(hSCManager, DriverId);
                if (InstallDriver(hSCManager, DriverId, DriverPath))
                {
                    StartDriver(hSCManager, DriverId);
                }
                OpenDriver();
            }
            if (SystemInstallDriver(hSCManager, DriverId, DriverPath))
            {
                rCode = OLS_DLL_NO_ERROR;
            }
            else
            {
                rCode = OLS_DLL_DRIVER_SC_DRIVER_NOT_INSTALLED;
            }
        }
        break;
    case OLS_DRIVER_SYSTEM_UNINSTALL:
        if (!IsSystemInstallDriver(hSCManager, DriverId, DriverPath))
        {
            rCode = OLS_DLL_NO_ERROR;
        }
        else
        {
            if (gHandle != INVALID_HANDLE_VALUE)
            {
                CloseHandle(gHandle);
                gHandle = INVALID_HANDLE_VALUE;
            }

            if (StopDriver(hSCManager, DriverId))
            {
                if (RemoveDriver(hSCManager, DriverId))
                {
                    rCode = OLS_DLL_NO_ERROR;
                }
                else
                {
                    rCode = OLS_DLL_DRIVER_SC_DRIVER_NOT_REMOVED;
                }
            }
        }
        break;
    default:
        rCode = OLS_DLL_UNKNOWN_ERROR;
        break;
    }

    if (hSCManager != NULL)
    {
        CloseServiceHandle(hSCManager);
    }

    return rCode;
}

BOOL InstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverId, LPCTSTR DriverPath)
{
    SC_HANDLE	hService = NULL;
    BOOL        rCode = FALSE;
    DWORD		error = NO_ERROR;

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

    return rCode;
}

BOOL SystemInstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverId, LPCTSTR DriverPath)
{
    SC_HANDLE	hService = NULL;
    BOOL		rCode = FALSE;

    hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);

    if (hService != NULL)
    {
        rCode = ChangeServiceConfig(hService,
            SERVICE_KERNEL_DRIVER,
            SERVICE_AUTO_START,
            SERVICE_ERROR_NORMAL,
            DriverPath,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
        );
        CloseServiceHandle(hService);
    }

    return rCode;
}

BOOL RemoveDriver(SC_HANDLE hSCManager, LPCTSTR DriverId)
{
    SC_HANDLE   hService = NULL;
    BOOL        rCode = FALSE;

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

    return rCode;
}

BOOL StartDriver(SC_HANDLE hSCManager, LPCTSTR DriverId)
{
    SC_HANDLE	hService = NULL;
    BOOL		rCode = FALSE;
    DWORD		error = NO_ERROR;

    hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);

    if (hService != NULL)
    {
        if (!StartService(hService, 0, NULL))
        {
            error = GetLastError();
            if (error == ERROR_SERVICE_ALREADY_RUNNING)
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

    return rCode;
}

BOOL StopDriver(SC_HANDLE hSCManager, LPCTSTR DriverId)
{
    SC_HANDLE		hService = NULL;
    BOOL			rCode = FALSE;
    SERVICE_STATUS	serviceStatus;
    DWORD		error = NO_ERROR;

    hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);

    if (hService != NULL)
    {
        rCode = ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus);
        error = GetLastError();
        CloseServiceHandle(hService);
    }

    return rCode;
}

BOOL IsSystemInstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverId, LPCTSTR DriverPath)
{
    SC_HANDLE hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);
    if (hService != NULL)
    {
        DWORD dwSize;
        if (!QueryServiceConfig(hService, NULL, 0, &dwSize))
        {
            LPQUERY_SERVICE_CONFIG lpqsc = reinterpret_cast<LPQUERY_SERVICE_CONFIG>(new BYTE[dwSize]);
            if (!lpqsc)
            {
                return FALSE;
            }

            if (!QueryServiceConfig(hService, lpqsc, dwSize, &dwSize))
            {
                if (lpqsc->dwStartType == SERVICE_AUTO_START)
                {
                    CloseServiceHandle(hService);
                    delete[] lpqsc;
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}