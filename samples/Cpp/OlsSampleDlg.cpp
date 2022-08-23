//-----------------------------------------------------------------------------
//     Author : hiyohiyo
//       Mail : hiyohiyo@crystalmark.info
//        Web : http://openlibsys.org/
//    License : The modified BSD license
//
//                     Copyright 2007-2008 OpenLibSys.org. All rights reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "OlsSample.h"
#include "OlsSampleDlg.h"
#include "../../WinRing0Dll/OlsDef.h"

#ifdef RUN_TIME_DYNAMIC_LINKING
#include "../../WinRing0Dll/OlsApiInitExt.h"
#else // for Load-Time Dynamic Linking
#include "../../WinRing0Dll/OlsApi.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------
static BOOL IsNT();

//-----------------------------------------------------------------------------
// SM BIOS Support Code
//-----------------------------------------------------------------------------

#define MAP_MEMORY_SIZE 64*1024

typedef struct _DmiHeader
{
	BYTE Type;
	BYTE Length;
	WORD Handle;
}DmiHeader;

static CString DmiString(DmiHeader* dmi, UCHAR id, BOOL rn = TRUE)
{
	static CString cstr;
	char *p = (char *)dmi;

	p += dmi->Length;

	while(id > 1 && *p)
	{
		p += strlen(p);
		p++;
		id--;
	}
	// ASCII Filter
	for(DWORD i = 0; i < strlen(p); i++){
		if(p[i] < 32 || p[i] == 127){
			p[i]='.';
		}
	}
	cstr = p;
	if(rn){
		cstr += "\r\n";
	}

	return cstr;
}

static CString DmiStringB(BYTE b, BOOL rn = TRUE)
{
	static CString cstr;
	cstr.Format(_T("%d"), b);
	if(rn){
		cstr += "\r\n";
	}
	return cstr;
}

//-----------------------------------------------------------------------------
// COlsSampleDlg dialog
//-----------------------------------------------------------------------------

COlsSampleDlg::COlsSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COlsSampleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COlsSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT, m_Edit);
}

BEGIN_MESSAGE_MAP(COlsSampleDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, &COlsSampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &COlsSampleDlg::OnBnClickedOk)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_REFRESH, &COlsSampleDlg::OnBnClickedRefresh)
END_MESSAGE_MAP()


// COlsSampleDlg message handlers

BOOL COlsSampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_IsNT = IsNT();

	Refresh();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void COlsSampleDlg::Refresh()
{
	CString cstr, edit;

	switch(GetDllStatus())
	{
	case OLS_DLL_NO_ERROR:
		break;
	case OLS_DLL_UNSUPPORTED_PLATFORM:
		AfxMessageBox(_T("DLL Status Error!! UNSUPPORTED_PLATFORM"));
		EndDialog(0);
		return;
		break;
	case OLS_DLL_DRIVER_NOT_LOADED:
		AfxMessageBox(_T("DLL Status Error!! DRIVER_NOT_LOADED"));
		EndDialog(0);
		return;
		break;
	case OLS_DLL_DRIVER_NOT_FOUND:
		AfxMessageBox(_T("DLL Status Error!! DRIVER_NOT_FOUND"));
		EndDialog(0);
		return;
		break;
	case OLS_DLL_DRIVER_UNLOADED:
		AfxMessageBox(_T("DLL Status Error!! DRIVER_UNLOADED"));
		EndDialog(0);
		return;
		break;
	case OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK:
		AfxMessageBox(_T("DLL Status Error!! DRIVER_NOT_LOADED_ON_NETWORK"));
		EndDialog(0);
		return;
		break;
	case OLS_DLL_UNKNOWN_ERROR:
	default:
		AfxMessageBox(_T("DLL Status Error!! UNKNOWN_ERROR"));
		EndDialog(0);
		return;
		break;
	}

	//-----------------------------------------------------------------------------
	// Version Check
	//-----------------------------------------------------------------------------
	BYTE major, minor, revision, release;
	cstr.Format(_T("[DLL Version]\r\n"));
	edit += cstr;
	GetDllVersion(&major, &minor, &revision, &release);
	cstr.Format(_T("%d.%d.%d.%d\r\n"), major, minor, revision, release);
	edit += cstr;
	cstr.Format(_T("[Device Driver Version]\r\n"));
	edit += cstr;
	GetDriverVersion(&major, &minor, &revision, &release);
	cstr.Format(_T("%d.%d.%d.%d\r\n"), major, minor, revision, release);
	edit += cstr;
	cstr.Format(_T("[Device Driver Type]\r\n"));
	edit += cstr;
	switch(GetDriverType())
	{
	case OLS_DRIVER_TYPE_WIN_9X:
		edit += _T("OLS_DRIVER_TYPE_WIN_9X\r\n");
		break;
	case OLS_DRIVER_TYPE_WIN_NT:
		edit += _T("OLS_DRIVER_TYPE_WIN_NT\r\n");
		break;
	case OLS_DRIVER_TYPE_WIN_NT4: 
		edit += _T("OLS_DRIVER_TYPE_WIN_NT4\r\n");
		break;
	case OLS_DRIVER_TYPE_WIN_NT_X64:
		edit += _T("OLS_DRIVER_TYPE_WIN_NT_X64\r\n");
		break;
	case OLS_DRIVER_TYPE_WIN_NT_IA64:
		edit += _T("OLS_DRIVER_TYPE_WIN_NT_IA64\r\n");
		break;
	default:
		edit += _T("OLS_DRIVER_TYPE_UNKNOWN\r\n");
		break;
	}

	cstr.Format(_T("[DLL Status]\r\n"));
	edit += cstr;
	switch(GetDllStatus())
	{
	case OLS_DLL_NO_ERROR:
		edit += _T("OLS_DLL_NO_ERROR\r\n");
		break;
	case OLS_DLL_UNSUPPORTED_PLATFORM:
		edit += _T("OLS_DLL_UNSUPPORTED_PLATFORM\r\n");
		break;
	case OLS_DLL_DRIVER_NOT_LOADED:
		edit += _T("OLS_DLL_DRIVER_NOT_LOADED\r\n");
		break;
	case OLS_DLL_DRIVER_NOT_FOUND:
		edit += _T("OLS_DLL_NOT_FOUND_DRIVER\r\n");
		break;
	case OLS_DLL_DRIVER_UNLOADED:
		edit += _T("OLS_DLL_DRIVER_UNLOADED\r\n");
		break;
	case OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK:
		edit += _T("OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK\r\n");
		break;
	default:
		edit += _T("OLS_DLL_UNKNOWN_ERROR\r\n");
		break;
	}

	//-----------------------------------------------------------------------------
	// TSC
	//-----------------------------------------------------------------------------
	DWORD index, eax, ebx, ecx, edx;
	cstr.Format(_T("[TSC]\r\n"));
	edit += cstr;

	// RdtscPx uses Process Affinity Mask
	if(RdtscPx(&eax, &edx, 1))
	{
		cstr.Format(_T("63-32    31-0\r\n"));
		edit += cstr;
		cstr.Format(_T("%08X %08X\r\n"), edx, eax);
		edit += cstr;
	}
	else
	{
		edit += _T("Failure : Change Process Affinity Mask\r\n");
	}

	//-----------------------------------------------------------------------------
	// MSR
	//-----------------------------------------------------------------------------
	cstr.Format(_T("[MSR]\r\n"));
	edit += cstr;
	index = 0x00000010; // Time Stamp Counter
	// RdmsrTx uses Thread Affinity Mask
	if(RdmsrTx(index, &eax, &edx, 1))
	{
		cstr.Format(_T("index     63-32    31-0\r\n"));
		edit += cstr;
		cstr.Format(_T("%08X: %08X %08X\r\n"), index, edx, eax);
		edit += cstr;
	}
	else
	{
		edit += _T("Failure : Change Thread Affinity Mask\r\n");
	}

    eax = 0;
    edx = 0;
    ULONG result = SetThreadAffinityMask(GetCurrentThread(), 1);
    Rdmsr(0x19c, &eax, &edx);//read Temperature
    cstr.Format(_T("%08X: %08X %08X\r\n"), 0x19c, edx, eax);
    SetThreadAffinityMask(GetCurrentThread(), result);
    //char s[20];
    //sprintf(s, "%d", 100 - ((eax & 0x007f0000) >> 16));
    cstr.Format(_T("cpuÎÂ¶È: %d\r\n"), 100 - ((eax & 0x007f0000) >> 16));
    edit += cstr;

	//-----------------------------------------------------------------------------
	// CPUID (Standard/Extended)
	//-----------------------------------------------------------------------------
/*
	DWORD maxCpuid, maxCpuidEx;

	cstr.Format(_T("[CPUID]\r\n"));
	edit += cstr;
	cstr.Format(_T("index     EAX      EBX      ECX      EDX  \r\n"));
	edit += cstr;

	// Standard
	CpuidPx(0x00000000, &maxCpuid, &ebx, &ecx, &edx, 1);
	for(index = 0x00000000; index <= maxCpuid; index++)
	{
		CpuidPx(index, &eax, &ebx, &ecx, &edx, 1);
		cstr.Format(_T("%08X: %08X %08X %08X %08X\r\n"), index, eax, ebx, ecx, edx);
		edit += cstr;
	}

	// Extended
	CpuidPx(0x80000000, &maxCpuidEx, &ebx, &ecx, &edx, 1);
	for(index = 0x80000000; index <= maxCpuidEx; index++)
	{
		CpuidPx(index, &eax, &ebx, &ecx, &edx, 1);
		cstr.Format(_T("%08X: %08X %08X %08X %08X\r\n"), index, eax, ebx, ecx, edx);
		edit += cstr;
	}
*/
	//-----------------------------------------------------------------------------
	// I/O (Beep)
	//-----------------------------------------------------------------------------
	DWORD freq = 1193180000 / 440000; // 440Hz

	WriteIoPortByte(0x43, 0xB6);
	WriteIoPortByte(0x42, (BYTE)(freq & 0xFF));
	WriteIoPortByte(0x42, (BYTE)(freq >> 9));

	Sleep(100);

	WriteIoPortByte(0x61, (ReadIoPortByte(0x61) | 0x03));

	Sleep(1000);

	WriteIoPortByte(0x61, (ReadIoPortByte(0x61) & 0xFC));

	cstr.Format(_T("[I/O]\r\nBeep 440Hz\r\n"));
	edit += cstr;

	//-----------------------------------------------------------------------------
	// DMI
	//-----------------------------------------------------------------------------
	/* ReadDmi is obsolete.
	BYTE b[MAP_MEMORY_SIZE];
	BYTE *p = (BYTE *)b;
	DWORD j;

	cstr.Format(_T("[MEMORY]\r\n"));
	edit += cstr;

	if(ReadDmiMemory(b, MAP_MEMORY_SIZE, sizeof(BYTE)) == MAP_MEMORY_SIZE)
	{
		for(j = 0; j < MAP_MEMORY_SIZE; j += 16)
		{
			if(memcmp(p, "_SM_", 4) == 0)
			{
				cstr.Format(_T("SM BIOS Version : %s.%s"), DmiStringB(p[6], 0), DmiStringB(p[7], 0));
				break;
			}
			p+=16;
		}
		edit += cstr;
	}
	cstr.Format(_T("\r\n"));
	edit += cstr;
	*/

	//-----------------------------------------------------------------------------
	// PCI
	//-----------------------------------------------------------------------------
	DWORD address, value;

	SetPciMaxBusIndex(7);
	cstr.Format(_T("[PCI]\r\n"));
	edit += cstr;

	// All Device
	cstr.Format(_T("Bus Dev Fnc VendorDevice\r\n"));
	edit += cstr;
	for(DWORD bus = 0; bus <= 255; bus++)
	{
		for(DWORD dev = 0; dev < 32; dev++)
		{
			for(DWORD func = 0; func < 8; func++)
			{
				address = PciBusDevFunc(bus, dev, func);
				// No Error Check
				value = ReadPciConfigDword(address, 0x00);
				// Error Check
				// if(! ReadPciConfigDwordEx(address, 0x00, &value))
				//{
				//		AfxMessageBox(_T("(ReadPciConfig Read Error"));
				//}

				if((value & 0xFFFF) != 0xFFFF && (value & 0xFFFF) != 0x0000)
				{
					cstr.Format(_T("%02Xh %02Xh %02Xh %04Xh %04Xh\r\n"), 
						bus, dev, func,
						value & 0x0000FFFF,
						(value >> 16) & 0x0000FFFF
						);
					edit += cstr;
				}
			}
		}
	}

	// Host Bridge
	address = FindPciDeviceByClass(0x06, 0x00, 0x00, 0);
	if(address != 0xFFFFFFFF)
	{
        edit += _T("[PCI Confguration Space Dump] HostBridge\r\n"); 
        edit += _T("    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
        edit += _T("---------------------------------------------------\r\n");

        for (int i = 0; i < 256; i+= 16)
        {
			cstr.Format(_T("%02X|"), i);
			edit += cstr;
			for (int j = 0; j < 16; j++)
			{
				value = ReadPciConfigByte(address, i + j);
				cstr.Format(_T(" %02X"), value);
				edit += cstr;
			}
            edit += "\r\n";
        }
	}

	m_Edit.SetWindowText(edit);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void COlsSampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR COlsSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void COlsSampleDlg::OnBnClickedOk()
{
	OnOK();
}

void COlsSampleDlg::OnBnClickedCancel()
{
	OnCancel();
}


void COlsSampleDlg::OnDestroy()
{
	CDialog::OnDestroy();
}

void COlsSampleDlg::OnBnClickedRefresh()
{
	m_Edit.SetWindowText(_T(""));
	m_Edit.UpdateWindow();
	Refresh();
}

BOOL IsNT()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	return (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT);
}
