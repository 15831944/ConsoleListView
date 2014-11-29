#ifndef _LIST_H_
#define _LIST_H_

extern HWND g_hWndList;
extern HWND hListView;
extern HWND g_hStatus;

extern char *QueryCaption, *QueryText, *QueryDst;
extern int QueryLen;

#define IDC_LIST_LISTVIEW   301
#define IDC_LIST_REBAR      302
#define IDC_LIST_STATUS     303

#define ITEMOFFSET          2000

#define ID_FILE_NEW         40001
#define ID_FILE_OPEN        40002
#define ID_FILE_SAVEAS      40003

int WINAPI WinList(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndListProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int GetSelectedItemText(int idx, char *buf, int len, int sub);
BOOL CALLBACK QueryDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif