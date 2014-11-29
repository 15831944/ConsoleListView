#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include <stdlib.h>
#include <commdlg.h>
#include "main.h"
#include "list.h"
#include "resource.h"

// Global variables
OPENFILENAME openFileName;
char programFileName[270];

// Global variables
HWND g_hWndList;
HWND hListView;
HWND g_hStatus;

int WINAPI WinList(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// prepare for XP style controls
	InitCommonControls();

	// Initialize variables
	static char szClassName[] = "ConsoleList";
	static char szWindowName[] = "ConsoleList";

	// Initialize global variables
	g_hInstance = GetModuleHandle(0);

	// Define & register window class
	WNDCLASSEX WndClass;

	memset(&WndClass, 0, sizeof(WndClass));
	WndClass.cbSize = sizeof(WndClass);						// size of structure
	WndClass.lpszClassName = szClassName;					// name of window class
	WndClass.lpfnWndProc = WndListProc;						// points to a window procedure
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
	g_hWndList = CreateWindowEx(WS_EX_WINDOWEDGE, szClassName, szWindowName, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, 0, 0, g_hInstance, 0);

	if (!g_hWndList)
	{
		return 0;
	}

	ShowWindow(g_hWndList, nCmdShow);
	UpdateWindow(g_hWndList);

	return 0;
}

LRESULT CALLBACK WndListProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CREATE:
		{
			INITCOMMONCONTROLSEX icex;

			icex.dwSize = sizeof(icex);
			icex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_USEREX_CLASSES;

			InitCommonControlsEx(&icex);

			// Create ListView Control
			HFONT hfDefault;
			//HWND hListView;

			hListView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, 0,
				WS_CHILD | WS_VISIBLE | LVS_REPORT, 0, 0, 0, 0,
				hWnd, (HMENU)IDC_LIST_LISTVIEW, GetModuleHandle(NULL), NULL);

			if (hListView == NULL)
				MessageBox(hWnd, "failed to create listview control", "error", MB_OK | MB_ICONERROR);

			hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hListView, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

			LVITEM lvItem;
			LVCOLUMN lvColumn;

			CHAR szText[81];
			HBITMAP hBitmap;
			HBITMAP hBitmapSmall;
			HDC hDC;
			int dIndex;
			int dX;
			int dY;

			HIMAGELIST g_hImageList;
			HIMAGELIST g_hSmallIL;

			// Create and initialize image list used with control
			g_hImageList = ImageList_Create(32, 32, ILC_COLORDDB | ILC_MASK, 11, 1);
			g_hSmallIL = ImageList_Create(16, 16, ILC_COLORDDB | ILC_MASK, 11, 1);
			// Load images based on color depth
			hDC = GetDC(hWnd);
			if(GetDeviceCaps(hDC, BITSPIXEL) <= 8)
			{
				hBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_DEFAULT));
				hBitmapSmall = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_DEFAULT));
			}
			else
			{
				hBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_DEFAULT));
				hBitmapSmall = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_DEFAULT));
			}
			ReleaseDC(hWnd, hDC);
			// Add images to image list
			ImageList_AddMasked(g_hImageList, hBitmap, RGB(255, 0, 255));
			ImageList_AddMasked(g_hSmallIL, hBitmapSmall, RGB(255, 0, 255));
			DeleteObject(hBitmap);
			DeleteObject(hBitmapSmall);

			// Set image list of list view
			ListView_SetImageList(hListView, g_hImageList, LVSIL_NORMAL);
			ListView_SetImageList(hListView, g_hSmallIL, LVSIL_SMALL);

			// Add columns
			lvColumn.mask = LVCF_TEXT|LVCF_WIDTH;

			// Column 0
			lvColumn.pszText = "Name";
			lvColumn.cx = ListView_GetStringWidth(hListView, "Jupiter")+32;
			ListView_InsertColumn(hListView, 0, &lvColumn);

			// Column 1
			lvColumn.pszText = "Size";
			lvColumn.cx = ListView_GetStringWidth(hListView, "0,000,000,000")+16;
			ListView_InsertColumn(hListView, 1, &lvColumn);

			// Column 2
			lvColumn.pszText = "Attributes";
			lvColumn.cx = ListView_GetStringWidth(hListView, "00,000.0")+16;
			ListView_InsertColumn(hListView, 2, &lvColumn);

			// Column 3
			lvColumn.pszText = "Mass (kg)";
			lvColumn.cx = ListView_GetStringWidth(hListView, "0.0000e00")+16;
			ListView_InsertColumn(hListView, 3, &lvColumn);

			// Add items to list view
			lvItem.state = 0;
			lvItem.stateMask = 0;
			lvItem.cchTextMax = 0;
			lvItem.lParam = 0;
			lvItem.iIndent = 0;

			ListView_InsertItem(hListView,&lvItem);

			for(dX=9;dX>=1;dX--)
			{
				LoadString(g_hInstance, ITEMOFFSET+(dX-1)*4, szText, sizeof(szText));

				lvItem.mask = LVIF_TEXT|LVIF_IMAGE;
				lvItem.iItem = 0;
				lvItem.iSubItem = 0;
				lvItem.pszText = szText;
				lvItem.iImage = dX-1;

				dIndex = ListView_InsertItem(hListView, &lvItem);

				// Add sub-items
				for(dY=1;dY<4;dY++)
				{
					LoadString(g_hInstance, ITEMOFFSET+((dX-1)*4)+dY, szText, sizeof(szText));

					lvItem.mask = TVIF_TEXT;
					lvItem.iItem = dIndex;
					lvItem.iSubItem = dY;
					lvItem.pszText = szText;

					ListView_SetItem(hListView, &lvItem);
				}
			}

			HCURSOR g_hCursor;
			HWND g_hUserToolTip;

			// Set value returned by 'cursor' in parser
			g_hCursor = ListView_GetHotCursor(hListView);

			// Set value returned by 'tooltip' in parser
			g_hUserToolTip = ListView_GetToolTips(hListView);

			// Create Rebar Control
			REBARINFO rbi;
			REBARBANDINFO rbbInfo;

			TBBUTTON tbbStruct[10];
			UINT dImages[] = { 0, 1, 2, -1, 3, 4, -1, 5, -1, 6 };

			COMBOBOXEXITEM cbeItem;
			LPSTR pLabels[] = { "Desktop","Network","3½ Floppy (A:)","HDD (C:)","Control Spy","CDROM (D:)","Printers","Control Panel","Network","Recycle Bin" };
			UINT dIndent[] = { 0, 1, 2, 2, 3, 2, 2, 2, 1, 1 };

			RECT rcControl;
			//HBITMAP hBitmap;
			//int dX;

			HIMAGELIST g_hToolbarIL = NULL;
			HIMAGELIST g_hToolbarHotIL = NULL;
			HIMAGELIST g_hComboBoxExIL = NULL;


			HWND hRebar;

			hRebar = CreateWindowEx(0, REBARCLASSNAME, NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
				CCS_ADJUSTABLE | RBS_VARHEIGHT | RBS_BANDBORDERS, 0, 0, 0, 0,
				hWnd, (HMENU)IDC_LIST_REBAR, GetModuleHandle(NULL), NULL);

			memset(&rbi, 0, sizeof(rbi));
			rbi.cbSize = sizeof(rbi);
			rbi.fMask = 0;
			rbi.himl = (HIMAGELIST)0;

			SendMessage(hRebar, RB_SETBARINFO, 0, (LPARAM)&rbi);

			// Toolbar child
			HWND hToolBar;

			hToolBar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
				WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | CCS_NORESIZE | CCS_NODIVIDER,
				0, 0, 0, 0, hRebar, NULL, GetModuleHandle(NULL), NULL);

			// Since created toolbar with CreateWindowEx, specify the button structure size
			// This step is not required if CreateToolbarEx was used
			SendMessage(hToolBar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

			// Create and initialize image list used for child toolbar
			g_hToolbarIL = ImageList_Create(20, 20, ILC_COLORDDB | ILC_MASK, 7, 1);
			hBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_INTERFACE));
			ImageList_AddMasked(g_hToolbarIL, hBitmap, RGB(255, 0, 255));
			DeleteObject(hBitmap);

			// Create and initialize hot image list used for child toolbar
			g_hToolbarHotIL = ImageList_Create(20, 20, ILC_COLORDDB | ILC_MASK, 7, 1);
			hBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_INTERFACEHOT));
			ImageList_AddMasked(g_hToolbarHotIL, hBitmap, RGB(255, 0, 255));
			DeleteObject(hBitmap);

			// Add image lists to toolbar
			SendMessage(hToolBar, TB_SETIMAGELIST, 0,(LPARAM)g_hToolbarIL);
			SendMessage(hToolBar, TB_SETHOTIMAGELIST, 0,(LPARAM)g_hToolbarHotIL);

			// Set toolbar padding
			SendMessage(hToolBar, TB_SETINDENT, 2, 0);

			// Add buttons to control
			for(dX=0;dX<10;dX++)
			{
				// Add button
				if(dImages[dX] != -1)
				{
					tbbStruct[dX].iBitmap = dImages[dX];
					tbbStruct[dX].fsStyle = TBSTYLE_BUTTON;
				}
				else
				{
					tbbStruct[dX].iBitmap = 0;
					tbbStruct[dX].fsStyle = TBSTYLE_SEP;
				}
				tbbStruct[dX].fsState = TBSTATE_ENABLED;
				tbbStruct[dX].idCommand = 40001 + dX;
				tbbStruct[dX].dwData = 0;
				tbbStruct[dX].iString = -1;
			}

			SendMessage(hToolBar, TB_ADDBUTTONS, 10, (LPARAM)tbbStruct);

			// Add Rebar band

			rbbInfo.cbSize = sizeof(REBARBANDINFO);

			rbbInfo.fMask = RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE|RBBIM_SIZE;
			rbbInfo.fStyle = RBBS_GRIPPERALWAYS|RBBS_CHILDEDGE;
			rbbInfo.hwndChild = hToolBar;
			rbbInfo.cxMinChild = 0;
			rbbInfo.cyMinChild = HIWORD(SendMessage(hToolBar,TB_GETBUTTONSIZE,0,0));
			rbbInfo.cx = 200;

			SendMessage(hRebar,RB_INSERTBAND,-1,(LPARAM)&rbbInfo);

			// COMBOBOXEX

			HWND hComboBoxEx;

			hComboBoxEx = CreateWindowEx(0, WC_COMBOBOXEX, NULL,
				WS_CHILD | WS_VISIBLE | CBS_DROPDOWN, //CBS_DROPDOWNLIST,
				0, 0, 300, 150, hRebar, NULL, GetModuleHandle(NULL), NULL);

			// Create and initialize image list used for child ComboBoxEx
			g_hComboBoxExIL = ImageList_Create(16, 16, ILC_COLORDDB | ILC_MASK, 10, 1);
			hBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_COMPUTER));
			ImageList_AddMasked(g_hComboBoxExIL, hBitmap, RGB(255, 0, 255));
			DeleteObject(hBitmap);

			// Assign image list to control
			SendMessage(hComboBoxEx, CBEM_SETIMAGELIST, 0, (LPARAM)g_hComboBoxExIL);

			// Add items to control
			for(dX=0;dX<10;dX++)
			{
				cbeItem.mask = CBEIF_IMAGE|CBEIF_SELECTEDIMAGE|CBEIF_TEXT|CBEIF_INDENT;
				cbeItem.iItem = -1;
				cbeItem.pszText = pLabels[dX];
				cbeItem.cchTextMax = strlen(pLabels[dX]);
				cbeItem.iImage = dX;
				cbeItem.iSelectedImage = dX;
				cbeItem.iIndent = dIndent[dX];

				SendMessage(hComboBoxEx, CBEM_INSERTITEM, 0, (LPARAM)&cbeItem);
			}

			// Select first item
			SendMessage(hComboBoxEx, CB_SETCURSEL, 4, 0);

			// Add Rebar band

			rbbInfo.cbSize = sizeof(REBARBANDINFO);
			rbbInfo.fMask = RBBIM_STYLE|RBBIM_CHILD|RBBIM_TEXT|RBBIM_CHILDSIZE|RBBIM_SIZE;
			rbbInfo.fStyle = RBBS_GRIPPERALWAYS|RBBS_CHILDEDGE;
			rbbInfo.hwndChild = hComboBoxEx;
			GetWindowRect(hComboBoxEx, &rcControl);
			rbbInfo.cxMinChild = 0;
			rbbInfo.cyMinChild = rcControl.bottom-rcControl.top;
			rbbInfo.lpText = "Address";
			rbbInfo.cx = 200;

			SendMessage(hRebar, RB_INSERTBAND, -1, (LPARAM)&rbbInfo);


			// Create StatusBar Control
			//HWND hStatus;
			int statwidths[] = {100, -1};

			g_hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL,
				WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,
				hWnd, (HMENU)IDC_LIST_STATUS, GetModuleHandle(NULL), NULL);

			SendMessage(g_hStatus, SB_SETPARTS, sizeof(statwidths)/sizeof(int), (LPARAM)statwidths);
			SendMessage(g_hStatus, SB_SETTEXT, 0, (LPARAM)"0 ms");
			SendMessage(g_hStatus, SB_SETTEXT, 1, (LPARAM)"Console Ready...");
		}
		break;
	case WM_SIZE:
		{
			HWND hTool;
			RECT rcTool;
			int iToolHeight;

			HWND hStatus;
			RECT rcStatus;
			int iStatusHeight;

			HWND hListView;
			int iListViewHeight;
			RECT rcClient;

			// Size toolbar and get height

			hTool = GetDlgItem(hWnd, IDC_LIST_REBAR);

			GetWindowRect(hTool, &rcTool);
			iToolHeight = rcTool.bottom - rcTool.top;

			// Resize rebar
			MoveWindow(hTool, 0, 0, LOWORD(lParam), iToolHeight, FALSE);

			// Size statusbar and get height

			hStatus = GetDlgItem(hWnd, IDC_LIST_STATUS);
			SendMessage(hStatus, WM_SIZE, 0, 0);

			GetWindowRect(hStatus, &rcStatus);
			iStatusHeight = rcStatus.bottom - rcStatus.top;

			// Calculate the remaining height and size richedit

			GetClientRect(hWnd, &rcClient);

			iListViewHeight = rcClient.bottom - iToolHeight - iStatusHeight;

			hListView = GetDlgItem(hWnd, IDC_LIST_LISTVIEW);
			SetWindowPos(hListView, NULL, 0, iToolHeight, rcClient.right, iListViewHeight, SWP_NOZORDER);

			InvalidateRect(hWnd, 0, FALSE);
		}
		break;
	case WM_CLOSE:
		{
			// Do not destroy window yet
			ShowWindow(hWnd, SW_HIDE);

			return 0;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_NOTIFY:
		{
			if (((NMHDR *)lParam)->code == RBN_HEIGHTCHANGE)
			{
				// Rebar height has changed, realign other controls

				RECT rc;
				GetClientRect(g_hWndList, &rc);
				PostMessage(g_hWndList, WM_SIZE, 0, ((rc.bottom - rc.top) << 16) | (rc.right - rc.left));

				return 0;
			}

			if (((NMHDR *)lParam)->hwndFrom == hListView)
			{
				switch (((NMHDR *)lParam)->code) 
				{
				case NM_DBLCLK:
					{
						if (ListView_GetSelectedCount(hListView) == 1)
						{
							char buf[MAX_PATH], Caption[MAX_PATH];
							int idx;

							idx = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);

							LVITEM Item;
							Item.mask = LVIF_TEXT | LVIF_PARAM;
							Item.iItem = idx;
							Item.iSubItem = 0;
							Item.pszText = Caption;
							Item.cchTextMax = sizeof(Caption);

							ListView_GetItem(hListView, &Item);

							SetWindowText(g_hWndList, Item.pszText);

						}
						return 0;
					}
				case NM_RCLICK:
					{
						POINT curs;
						HMENU PopupMenu;
						char WindowText[MAX_PATH + 1], buffer[MAX_PATH + 1];
						int ret = IDR_MENU_POPUPS;

						GetWindowText(g_hWndList, WindowText, sizeof(WindowText));

						strcpy(buffer, WindowText);

						GetCursorPos(&curs);
						PopupMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MENU_POPUPS));

						TrackPopupMenu(GetSubMenu(PopupMenu, 0),  TPM_LEFTALIGN | TPM_LEFTBUTTON,
							curs.x, curs.y, 0, g_hWndList, 0);

						DestroyMenu(PopupMenu);
						return 0;
					}
				}
			}
		}
		break;
	case WM_COMMAND:
		{
			unsigned long argl[10];
			char *argv[10], buffer[MAX_PATH + 1], buffer2[MAX_PATH + 1];
			int idx;

			switch(LOWORD(wParam))
			{
			case ID_FILE_NEW:
				SetWindowText(g_hWndList, "ID_FILE_NEW");
				break;
			case ID_FILE_OPEN:
				SetWindowText(g_hWndList, "ID_FILE_OPEN");
				break;
			case ID_FILE_SAVEAS:
				SetWindowText(g_hWndList, "ID_FILE_SAVEAS");
				break;
			}
		}
		break;
	case WM_GETMINMAXINFO:
		{
			// Set window size constraints
			((MINMAXINFO *)lParam)->ptMinTrackSize.x = 500;
			((MINMAXINFO *)lParam)->ptMinTrackSize.y = 400;
		}
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

int GetSelectedItemText(int idx, char *buf, int len, int sub)
{
	int newidx;
	LVITEM Item;

	newidx = ListView_GetNextItem(hListView, idx, LVNI_SELECTED);

	Item.mask = LVIF_TEXT | LVIF_PARAM;
	Item.iItem = newidx;
	Item.iSubItem = sub;
	Item.pszText = buf;
	Item.cchTextMax = len;

	ListView_GetItem(hListView, &Item);
	return newidx;
}