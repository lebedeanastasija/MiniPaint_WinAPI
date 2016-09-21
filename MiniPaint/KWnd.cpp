#define _CRT_SECURE_NO_WARNINGS
#include "KWnd.h"

KWnd::KWnd(LPCTSTR windowName, HINSTANCE hInst, int cmdShow,
	LRESULT(WINAPI *pWndProc)(HWND, UINT, WPARAM, LPARAM),
	LPCTSTR menuName, int x, int y, int width,int height,
	UINT classStyle, DWORD windowStyle, HWND hParent)
{	
	TCHAR szClassName[] = L"MyClass";

	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = pWndProc;
	wc.cbWndExtra = 0;
	wc.cbClsExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = menuName;
	wc.lpszClassName = szClassName;

	if (!RegisterClassEx(&wc)) {
		char msg[100] = "Can not register class: ";
		strcat(msg, (char*)szClassName);
		MessageBox(NULL, (LPCWSTR)msg, L"Error", MB_OK);
		return;
	}

	hWnd = CreateWindow(szClassName, windowName, windowStyle,
		x, y, width, height, hParent,HMENU(NULL), hInst, NULL);
	if (!hWnd){
		char text[100] = "Can not create window: ";
		strcat(text, (char*)windowName);
		MessageBox(NULL, (LPCWSTR)text, L"Error", MB_OK);
		return;
	}
	ShowWindow(hWnd, cmdShow);
}