#define WIN32_LEAN_AND_MEAN
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600
#endif

#pragma comment (lib, "wsock32.lib")
#pragma comment (lib, "comctl32.lib")
#pragma comment (lib, "winmm.lib")

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Windows Header Files:
#include <windows.h>
#include <Windowsx.h>
#include <commctrl.h>
#include <Shellapi.h>
#include <Shlwapi.h>
#include <mmsystem.h>

// C RunTime Header Files
//#include <stdlib.h>
//#include <malloc.h>
//#include <memory.h>
#include <tchar.h>

#include "main.h"
#include "list.h"
#include "resource.h"

#define TRAYICONID	1				// ID number for the Notify Icon
#define SWM_TRAYMSG	WM_APP			// the message ID sent to our window

#define SWM_SHOW	WM_APP + 1		// show the window
#define SWM_HIDE	WM_APP + 2		// hide the window
#define SWM_EXIT	WM_APP + 3		// close the window

// Global Variables:
HINSTANCE		g_hInstance;
HINSTANCE		hInst;	// current instance
NOTIFYICONDATA	niData;	// notify icon data
HINSTANCE		ghInstance;
HWND			ghWnd;

// Forward declarations of functions included in this code module:
BOOL				InitInstance(HINSTANCE, int);
BOOL				OnInitDialog(HWND hWnd);
void				ShowContextMenu(HWND hWnd);
ULONGLONG			GetDllVersion(LPCTSTR lpszDllName);

INT_PTR CALLBACK	DlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

// Message handler for the app
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC           hdc;
	PAINTSTRUCT   ps;
	int wmId, wmEvent;

	switch (message) 
	{
	case SWM_TRAYMSG:
		switch(lParam)
		{
		case WM_LBUTTONDBLCLK:
			ShowWindow(hWnd, SW_RESTORE);
			break;
		case WM_RBUTTONDOWN:
		case WM_CONTEXTMENU:
			ShowContextMenu(hWnd);
		}
		break;
	case WM_SYSCOMMAND:
		if((wParam & 0xFFF0) == SC_MINIMIZE)
		{
			ShowWindow(hWnd, SW_HIDE);
			return 1;
		}
		else if(wParam == IDM_ABOUT)
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam); 

		switch (wmId)
		{
		case SWM_SHOW:
			WinList(NULL, NULL, NULL, SW_HIDE);
			ShowWindow(g_hWndList, SW_RESTORE);
			break;
		case SWM_HIDE:
		case IDOK:
			ShowWindow(g_hWndList, SW_HIDE);
			break;
		case SWM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		}
		return 1;
	case WM_INITDIALOG:
		return OnInitDialog(hWnd);
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		niData.uFlags = 0;
		Shell_NotifyIcon(NIM_DELETE,&niData);
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
		}
		break;
	case WM_ERASEBKGND:
		return 1;
		break;
	case WM_MOVE:
		//call for a paint
		InvalidateRect(hWnd, NULL, FALSE);
		UpdateWindow(hWnd);
		break;
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		}
	case WM_SIZE:
		break;
	case WM_GETMINMAXINFO:
		{
			//Set window size constraints
			((MINMAXINFO *)lParam)->ptMinTrackSize.x = 500;
			((MINMAXINFO *)lParam)->ptMinTrackSize.y = 300;
			break;
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) return FALSE;

	ghInstance = GetModuleHandle(0);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int) msg.wParam;
}

//	Initialize the window and tray icon
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	// store instance handle and create dialog
	hInst = hInstance;

	static char szClassName[] = "Console";
	static char szWindowName[] = "Console";

	// Initialize global variables
	g_hInstance = GetModuleHandle(0);

	// Define & register window class
	WNDCLASSEX WndClass;
	MSG msg;

	memset(&WndClass, 0, sizeof(WndClass));
	WndClass.cbSize = sizeof(WndClass);						// size of structure
	WndClass.lpszClassName = szClassName;					// name of window class
	WndClass.lpfnWndProc = WndProc;							// points to a window procedure
	WndClass.hInstance = g_hInstance;						// handle to instance
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);		// predefined app. icon
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);		// small class icon
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);			// predefined arrow
	WndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);	// background brush
	WndClass.lpszMenuName = NULL;							// name of menu resource

	if (!RegisterClassEx(&WndClass))
	{
		return 0;
	}

	// Create actual window
	ghWnd = CreateWindowEx(WS_EX_WINDOWEDGE, szClassName, szWindowName, WS_OVERLAPPEDWINDOW, // | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 300, 0, 0, g_hInstance, 0);

	if (!ghWnd)
	{
		return 0;
	}

	//	ShowWindow(ghWnd, SW_SHOW);
	UpdateWindow(ghWnd);

	// Fill the NOTIFYICONDATA structure and call Shell_NotifyIcon

	// zero the structure - note:	Some Windows functions require this but
	//								I can't be bothered which ones do and
	//								which ones don't.
	ZeroMemory(&niData,sizeof(NOTIFYICONDATA));

	// get Shell32 version number and set the size of the structure
	//		note:	the MSDN documentation about this is a little
	//				dubious and I'm not at all sure if the method
	//				bellow is correct
	ULONGLONG ullVersion = GetDllVersion(_T("Shell32.dll"));
	if(ullVersion >= MAKEDLLVERULL(5, 0,0,0))
		niData.cbSize = sizeof(NOTIFYICONDATA);
	else niData.cbSize = NOTIFYICONDATA_V2_SIZE;

	// the ID number can be anything you choose
	niData.uID = TRAYICONID;

	// state which structure members are valid
	niData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

	// load the icon
	niData.hIcon = (HICON)LoadImage(hInstance,MAKEINTRESOURCE(IDI_STEALTHDLG),
		IMAGE_ICON, GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),
		LR_DEFAULTCOLOR);

	// the window to send messages to and the message to send
	//		note:	the message value should be in the
	//				range of WM_APP through 0xBFFF
	niData.hWnd = ghWnd;
	niData.uCallbackMessage = SWM_TRAYMSG;

	// tooltip message
	lstrcpyn(niData.szTip, _T("Time flies like an arrow but\n   fruit flies like a banana!"), sizeof(niData.szTip)/sizeof(TCHAR));

	Shell_NotifyIcon(NIM_ADD,&niData);

	// free icon handle
	if(niData.hIcon && DestroyIcon(niData.hIcon))
		niData.hIcon = NULL;

	// call ShowWindow here to make the dialog initially visible

	return TRUE;
}

BOOL OnInitDialog(HWND hWnd)
{
	HMENU hMenu = GetSystemMenu(hWnd,FALSE);
	if (hMenu)
	{
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
		AppendMenu(hMenu, MF_STRING, IDM_ABOUT, _T("About"));
	}
	HICON hIcon = (HICON)LoadImage(hInst,
		MAKEINTRESOURCE(IDI_STEALTHDLG),
		IMAGE_ICON, 0,0, LR_SHARED|LR_DEFAULTSIZE);
	SendMessage(hWnd,WM_SETICON,ICON_BIG,(LPARAM)hIcon);
	SendMessage(hWnd,WM_SETICON,ICON_SMALL,(LPARAM)hIcon);
	return TRUE;
}

// Name says it all
void ShowContextMenu(HWND hWnd)
{
	POINT pt;
	GetCursorPos(&pt);
	HMENU hMenu = CreatePopupMenu();
	if(hMenu)
	{
		if( IsWindowVisible(hWnd) )
			InsertMenu(hMenu, -1, MF_BYPOSITION, SWM_HIDE, _T("Hide"));
		else
			InsertMenu(hMenu, -1, MF_BYPOSITION, SWM_SHOW, _T("Show"));
		InsertMenu(hMenu, -1, MF_BYPOSITION, SWM_EXIT, _T("Exit"));

		// note:	must set window to the foreground or the
		//			menu won't disappear when it should
		SetForegroundWindow(hWnd);

		TrackPopupMenu(hMenu, TPM_BOTTOMALIGN,
			pt.x, pt.y, 0, hWnd, NULL );
		DestroyMenu(hMenu);
	}
}

// Get dll version number
ULONGLONG GetDllVersion(LPCTSTR lpszDllName)
{
	ULONGLONG ullVersion = 0;
	HINSTANCE hinstDll;
	hinstDll = LoadLibrary(lpszDllName);
	if(hinstDll)
	{
		DLLGETVERSIONPROC pDllGetVersion;
		pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, "DllGetVersion");
		if(pDllGetVersion)
		{
			DLLVERSIONINFO dvi;
			HRESULT hr;
			ZeroMemory(&dvi, sizeof(dvi));
			dvi.cbSize = sizeof(dvi);
			hr = (*pDllGetVersion)(&dvi);
			if(SUCCEEDED(hr))
				ullVersion = MAKEDLLVERULL(dvi.dwMajorVersion, dvi.dwMinorVersion,0,0);
		}
		FreeLibrary(hinstDll);
	}
	return ullVersion;
}