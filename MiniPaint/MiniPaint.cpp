#include <windows.h>
#include <stdio.h>
#include "KWnd.h"
#include "resource.h"

typedef struct {
	int id_shape;
	COLORREF color;
	int id_width;
} ShapeData;

HDC hDC;
static HMENU hMenu;
static int x1, y1, x2, y2;
static BOOL bTracking = FALSE;
static HBRUSH hOldBrush;
static HPEN hPen, hOldPen;
static ShapeData shapeData;
UINT style;
CHOOSECOLOR* cc;
HWND canvas;
HWND hMainWnd;
HINSTANCE* GLOBAL_HINSTANCE;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void InitChooseColorStructure();
void SetPen();
void SetPenWidth(int);
void SetPenColor(COLORREF);
int CreateCanvas(HINSTANCE hInstance, HWND hwnd);

int WINAPI WinMain( HINSTANCE hInstance,	HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,	int nCmdShow)
{
	GLOBAL_HINSTANCE = (HINSTANCE*)malloc(sizeof(HINSTANCE));
	GLOBAL_HINSTANCE[0] = hInstance;
	MSG msg;	
	KWnd mainWnd(L"An IAmMolodzec application", hInstance, nCmdShow, WndProc, 
		MAKEINTRESOURCE(IDR_MENU1), 100, 100, 400, 300);
	hMainWnd = mainWnd.GetHWnd();

	CreateCanvas(GLOBAL_HINSTANCE[0], hMainWnd);

	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		InitChooseColorStructure();
		style = GetClassLong(hWnd, GCL_STYLE);
		SetClassLong(hWnd, GCL_STYLE, style|CS_DBLCLKS);
		hDC = GetDC(hWnd);
		hMenu = GetMenu(hWnd);
		
		hPen = CreatePen(PS_SOLID, shapeData.id_width, shapeData.color);
		SelectObject(hDC, hPen);
		hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(HOLLOW_BRUSH));
		shapeData.id_width = 0;
		shapeData.color = RGB(0, 0, 0);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_OPEN:
			break;
		case ID_FILE_SAVE:			
			break;
		case ID_FILE_PRINT:			
			break;
		case ID_FILE_EXIT:
			SendMessage(hWnd, WM_DESTROY, 0, 0);
			break;
		case ID_ABOUT:
			MessageBox(hWnd,
				L"MiniPaint\nVersion 1.0\nCopyright: Lebedeva Anastasiya Corporation, 2016.",
				L"About MiniPaint author", MB_OK);
			break;
		case ID_FIGURE_LINE:
			shapeData.id_shape = ID_FIGURE_LINE;
			break;
		case ID_FIGURE_PANSIL:
			shapeData.id_shape = ID_FIGURE_PANSIL;
			break;
		case ID_WIDTH_1:
			SetPenWidth(ID_WIDTH_1);
			break;
		case ID_WIDTH_5:
			SetPenWidth(ID_WIDTH_5);
			break;
		case ID_WIDTH_10:
			SetPenWidth(ID_WIDTH_10);
			break;
		case ID_WIDTH_15:
			SetPenWidth(ID_WIDTH_15);
			break;
		case ID_WIDTH_20:
			SetPenWidth(ID_WIDTH_20);
			break;
		case ID_COLOR:	
			ChooseColor(cc);
			SetPenColor(cc->rgbResult);
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_LBUTTONDOWN:
		bTracking = TRUE;
		SetROP2(hDC, R2_NOTXORPEN);
		x1 = x2 = LOWORD(lParam);
		y1 = y2 = HIWORD(lParam);
		switch (shapeData.id_shape){
		case ID_FIGURE_PANSIL:			
			MoveToEx(hDC, x1, y1,NULL);
			break;		
		case ID_FIGURE_LINE:
			MoveToEx(hDC, x1, y1, NULL);
			LineTo(hDC, x1, y1);
			break;
		}		
		break;

	case WM_LBUTTONUP:
		if (bTracking){
			bTracking = FALSE;
			SetROP2(hDC, R2_COPYPEN);
			x2 = LOWORD(lParam);
			y2 = HIWORD(lParam);
			
			switch (shapeData.id_shape){
			case ID_FIGURE_PANSIL:				
				MoveToEx(hDC, x2, y2, NULL);
				break;
			case ID_FIGURE_LINE:
				LineTo(hDC, x2, y2);
				break;
			}
		}
		break;
	case WM_MOUSEMOVE:
		if (bTracking){			
			switch (shapeData.id_shape){
			case ID_FIGURE_PANSIL:
				SetROP2(hDC, R2_COPYPEN);
				x2 = LOWORD(lParam);
				y2 = HIWORD(lParam);
				LineTo(hDC, x2, y2);
				break;
			case ID_FIGURE_LINE:
				x2 = LOWORD(lParam);
				y2 = HIWORD(lParam);
				break;
			}
		}
		break;
	case WM_LBUTTONDBLCLK:	
		bTracking = FALSE;
		x1 = LOWORD(lParam);
		y1 = HIWORD(lParam);
		switch (shapeData.id_shape){
		case ID_FIGURE_PANSIL:		
			MoveToEx(hDC, x1, y1, NULL);
			break;
		case ID_FIGURE_LINE:			
			MoveToEx(hDC, x1, y1, NULL);
			LineTo(hDC, x1, y1);
			break;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		SelectObject(hDC, hOldBrush);
		ReleaseDC(hWnd, hDC);
		PostQuitMessage(NULL);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return NULL;
}

void InitChooseColorStructure(){
	cc = (CHOOSECOLOR*)malloc(sizeof(CHOOSECOLOR));
	cc->hInstance = (HWND)GLOBAL_HINSTANCE[0];
	cc->hwndOwner = hMainWnd;
	cc->lStructSize = sizeof(CHOOSECOLOR);
	cc->Flags = CC_ANYCOLOR | CC_FULLOPEN;
	cc->lpCustColors = (COLORREF*)malloc(16 * sizeof(COLORREF));
	cc->lCustData = 0;
	cc->lpfnHook = NULL;
	cc->lpTemplateName = L"Choose color";
}
int CreateCanvas(HINSTANCE hInstance, HWND hwnd){
	DWORD dwExStyle = WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME;
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_SUNKEN;
	RECT rect;
	GetWindowRect(hMainWnd, &rect);
	canvas = CreateWindowEx(dwExStyle, L"STATIC", L"", dwStyle,
		0, 35, rect.right - rect.left - 15, 400, hwnd, NULL, hInstance, NULL);
	return 1;
}

void SetPen(){
	hPen = CreatePen(PS_SOLID, shapeData.id_width, shapeData.color);
	SelectObject(hDC, hPen);
}
void SetPenWidth(int width){
	shapeData.id_width = width;
	SetPen();
}
void SetPenColor(COLORREF color){
	shapeData.color = color;	
	SetPen();
}

