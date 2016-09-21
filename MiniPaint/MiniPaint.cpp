#include <windows.h>
#include "KWnd.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR mainMsg[] = L" I am maladzec :) ";

int WINAPI WinMain( HINSTANCE hInstance,	HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,	int nCmdShow)
{
	MSG msg;	
	KWnd mainWnd(L"An IAmMolodzec application", hInstance, nCmdShow, WndProc, NULL,50, 100, 200, 150);

	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	RECT rect;
	COLORREF textColor = RGB(0, 0, 0);
	COLORREF backColor = RGB(0, 155, 155);
	int userReply;

	switch (uMsg) {
	case WM_CREATE:
		SetClassLong(hWnd, GCL_HBRBACKGROUND, (LONG)CreateSolidBrush(RGB(200, 160, 255)));
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		
		//SetBkMode(hDC, TRANSPARENT);
		GetClientRect(hWnd, &rect);
		SetTextColor(hDC, textColor);
		SetBkColor(hDC, backColor);

		DrawText(hDC, mainMsg, -1, &rect, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		EndPaint(hWnd, &ps);
		break;
	case WM_CLOSE:
		userReply = MessageBox(hWnd, L"Are you sure you want to close he application?", L"", MB_YESNO | MB_ICONQUESTION);
		if (IDYES == userReply)
			DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(NULL);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return NULL;
}
 