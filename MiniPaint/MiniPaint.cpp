#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "KWnd.h"
#include "resource.h"

typedef struct {
	int id_shape;
	COLORREF color;
	int id_width;
} ShapeData;

HINSTANCE GLOBAL_HINSTANCE;

HDC hdc;
HDC hdcMeta;
HDC hdcMem;
HBITMAP hbmMem, hOld;
static HDC hCompatibleDC = 0;
static HDC hBitmapDC = 0;
static HBITMAP hCompatibleBitmap, hBitmap;
HENHMETAFILE hMetaFile;
ENHMETAHEADER emh;

RECT rect, rect1;
static bool flag, bScaled = FALSE;
static int f = 0;
int zDelta;
static double scale;
static int id;

static HMENU hMenu;
static int x1, y1, x2, y2;
static int xBegin = 0, yBegin = 0;
static int Rx, Ry, Rx1, Ry1;
static BOOL bTracking = FALSE;
static HPEN hPen, hOldPen;
static ShapeData shapeData;
HWND hMainWnd;

static CHOOSECOLOR cc;
static OPENFILENAME ofn;
static PRINTDLG pd;
static DOCINFO di;
char openFileName[260];
char filename[30] = "";
HANDLE hf; 

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void InitChooseColorStructure();
void InitOpenFileStructure();
void InitPrintDialogStructure();
void InitDocumentInfoStructure();

void openImageFile();
void printImageFile();
void createImageFile();
void saveImageFile();
void InitFigure();

void SetPen();
void SetPenWidth(int);
void SetPenColor(COLORREF);
void tryExitApp();
void closeMetaFile();

int WINAPI WinMain( HINSTANCE hInstance,	HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,	int nCmdShow)
{
	GLOBAL_HINSTANCE = hInstance;
	MSG msg;	
	KWnd mainWnd(L"An IAmMolodzec application", hInstance, nCmdShow, WndProc, 
		MAKEINTRESOURCE(IDR_MENU1), 100, 100, 400, 300);
	hMainWnd = mainWnd.GetHWnd();

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
		ShowWindow(hWnd, SW_MAXIMIZE);
		hdc = GetDC(hWnd);
		scale = 1;

		InitChooseColorStructure();
		InitOpenFileStructure();
		
		hMenu = GetMenu(hWnd);			
		GetClientRect(hWnd, &rect);

		srand(time(NULL));
		sprintf(filename, (char*)"image%d.emf", rand());
		hdcMeta = CreateEnhMetaFile(NULL, NULL, NULL, NULL);

		hPen = CreatePen(PS_SOLID, shapeData.id_width, shapeData.color);
		SelectObject(hdcMeta, hPen);

		hCompatibleDC = CreateCompatibleDC(hdc);
		hCompatibleBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);

		hBitmapDC = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);

		DeleteObject(SelectObject(hCompatibleDC, hCompatibleBitmap));
		DeleteObject(SelectObject(hCompatibleDC, (HBRUSH)WHITE_BRUSH));
		PatBlt(hCompatibleDC, 0, 0, rect.right, rect.bottom, PATCOPY);

		DeleteObject(SelectObject(hBitmapDC, hBitmap));
		DeleteObject(SelectObject(hBitmapDC, (HBRUSH)WHITE_BRUSH));
		PatBlt(hBitmapDC, 0, 0, rect.right, rect.bottom, PATCOPY);

		DeleteObject(SelectObject(hCompatibleDC, hPen));
		DeleteObject(SelectObject(hBitmapDC, hPen));

		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));

		shapeData.id_width = 0;
		shapeData.color = RGB(0, 0, 0);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_NEW:
			createImageFile();
			break;
		case ID_FILE_OPEN:
			if (GetOpenFileName(&ofn)){
				openImageFile();
			}
			break;
		case ID_FILE_SAVE:	
			if (GetSaveFileName(&ofn)){
				saveImageFile();
			}
			break;
		case ID_FILE_PRINT:	
			InitPrintDialogStructure();
			if (PrintDlg(&pd) == TRUE){
				Rx = GetDeviceCaps(pd.hDC, LOGPIXELSX);
				Ry = GetDeviceCaps(pd.hDC, LOGPIXELSY);
				InitDocumentInfoStructure();
				printImageFile();
			}
			break;
		case ID_FILE_EXIT:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		case ID_ABOUT:
			MessageBox(hWnd,
				L"MiniPaint\nVersion 1.0\nCopyright: Lebedeva Anastasiya Corporation, 2016.",
				L"About MiniPaint author", MB_OK);
			break;
		case ID_FIGURE_LINE:
			shapeData.id_shape = ID_FIGURE_LINE;
			InitFigure();
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case ID_FIGURE_PENCIL:
			shapeData.id_shape = ID_FIGURE_PENCIL;
			InitFigure();
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case ID_WIDTH_1:
			SetPenWidth(1);
			break;
		case ID_WIDTH_5:
			SetPenWidth(5);
			break;
		case ID_WIDTH_10:
			SetPenWidth(10);
			break;
		case ID_WIDTH_15:
			SetPenWidth(15);
			break;
		case ID_WIDTH_20:
			SetPenWidth(20);
			break;
		case ID_COLOR:	
			ChooseColor(&cc);
			SetPenColor(cc.rgbResult);
			break;
		case ID_ZOOM_ZOOM:
		case ID_ZOOM_PAN:
			bScaled = TRUE;
			shapeData.id_shape = ID_ZOOM_PAN;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		switch (shapeData.id_shape){
		case ID_FIGURE_PENCIL:
		case ID_FIGURE_LINE:
			x1 = x2 = (short)LOWORD(lParam);
			y1 = y2 = (short)HIWORD(lParam);
			break;
		case ID_ZOOM_PAN:
			x1 = x2 = (short)((short)LOWORD(lParam) / scale);
			y1 = y2 = (short)((short)HIWORD(lParam) / scale);
			break;
		}
		SetCapture(hMainWnd);
		bTracking = TRUE;
		break;
	case WM_LBUTTONUP:
		if (bTracking){
			bTracking = FALSE;
			SetROP2(hdc, R2_COPYPEN);
			SetROP2(hdcMeta, R2_COPYPEN);
			x2 = LOWORD(lParam);
			y2 = HIWORD(lParam);
			
			switch (shapeData.id_shape){
			case ID_FIGURE_PENCIL:				
				MoveToEx(hdc, x2, y2, NULL);
				MoveToEx(hdcMeta, x2, y2, NULL);
				break;
			case ID_FIGURE_LINE:
				LineTo(hdc, x2, y2);
				LineTo(hdcMeta, x2, y2);
				break;
			}
		}
		break;
	case WM_MOUSEMOVE:
		GetClientRect(hMainWnd, &rect);
		if (bTracking){			
			switch (shapeData.id_shape){
			case ID_FIGURE_PENCIL:
				SetROP2(hdc, R2_COPYPEN);
				SetROP2(hdcMeta, R2_COPYPEN);
				x2 = LOWORD(lParam);
				y2 = HIWORD(lParam);
				LineTo(hdc, x2, y2);
				LineTo(hdcMeta, x2, y2);
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
		MoveToEx(hdc, x1, y1, NULL);
		MoveToEx(hdcMeta, x1, y1, NULL);
		switch (shapeData.id_shape){
		case ID_FIGURE_PENCIL:				
			break;
		case ID_FIGURE_LINE:			
			LineTo(hdc, x1, y1);
			LineTo(hdcMeta, x1, y1);
			break;
		}
		break;
	case WM_CLOSE:		
		//tryExitApp();
		ReleaseDC(hMainWnd, hdc);
		SelectObject(hdc, hOldPen);
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

void tryExitApp(){
	int result = MessageBox(NULL, L"Do  you want to save image?",
		L"Exit", MB_YESNOCANCEL);
	switch (result){
		case IDYES:
			if (GetSaveFileName(&ofn)){
				saveImageFile();
				DestroyWindow(hMainWnd);
			}
			break;
		case IDNO:
			closeMetaFile();
			DeleteFile((LPCWSTR)filename);
			DestroyWindow(hMainWnd);
			break;
		default:
			break;
	}
}

void closeMetaFile(){
	hMetaFile = CloseEnhMetaFile(hdcMeta);
	DeleteEnhMetaFile(hMetaFile);
}

void InitChooseColorStructure(){
	cc.hInstance = (HWND)GLOBAL_HINSTANCE;
	cc.hwndOwner = hMainWnd;
	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.Flags = CC_ANYCOLOR | CC_FULLOPEN;
	cc.lpCustColors = (COLORREF*)malloc(16 * sizeof(COLORREF));
	cc.lCustData = 0;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = L"Choose color";
}

void InitOpenFileStructure(){
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hMainWnd;
	ofn.hInstance = GLOBAL_HINSTANCE;
	ofn.lpstrFile = (LPWSTR)openFileName;
	ofn.nMaxFile = sizeof(openFileName);
	ofn.lpstrFilter = L"Metafile (*.emf)\0*.emf\0Все файлы (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT|
		OFN_HIDEREADONLY | OFN_EXPLORER;
}

void InitPrintDialogStructure(){
	pd.lStructSize = sizeof(pd);
	pd.hwndOwner = hMainWnd;
	pd.hDevMode = NULL;
	pd.hDevNames = NULL;
	pd.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC;
	pd.nCopies = 1;
	pd.nFromPage = 0xFFFF;
	pd.nToPage = 0xFFFF;
	pd.nMinPage = 1;
	pd.nMaxPage = 0xFFFF;
}

void InitDocumentInfoStructure(){
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = L"Print Picture";
	di.fwType = NULL;
	di.lpszDatatype = NULL;
	di.lpszOutput = NULL;
}

void openImageFile(){
	InitFigure();
	hdcMeta = CreateEnhMetaFile(NULL, NULL, NULL, NULL);
	hMetaFile = GetEnhMetaFile((LPCWSTR)openFileName);
	GetEnhMetaFileHeader(hMetaFile, sizeof(ENHMETAHEADER), &emh);
	SetRect(&rect, emh.rclBounds.left, emh.rclBounds.top,
		emh.rclBounds.right, emh.rclBounds.bottom);
	DeleteObject(SelectObject(hdcMeta, hPen));
	PlayEnhMetaFile(hdcMeta, hMetaFile, &rect);

	GetClientRect(hMainWnd, &rect1);
	DeleteDC(hBitmapDC);
	hBitmapDC = CreateCompatibleDC(hdc);
	hBitmap = CreateCompatibleBitmap(hdc, rect1.right, rect1.bottom);

	DeleteObject(SelectObject(hBitmapDC, hBitmap));
	DeleteObject(SelectObject(hBitmapDC, (HBRUSH)WHITE_BRUSH));
	PatBlt(hBitmapDC, 0, 0, rect1.right, rect1.bottom, PATCOPY);
	
	DeleteObject(SelectObject(hBitmapDC, hPen));
	PlayEnhMetaFile(hBitmapDC, hMetaFile, &rect);

	InvalidateRect(hMainWnd, NULL, TRUE);
	UpdateWindow(hMainWnd);
	DeleteEnhMetaFile(hMetaFile);
}

void saveImageFile(){
	hMetaFile = CloseEnhMetaFile(hdcMeta);
	CopyEnhMetaFile(hMetaFile, (LPCWSTR)openFileName);
	GetEnhMetaFileHeader(hMetaFile, sizeof(ENHMETAHEADER), &emh);
	SetRect(&rect, emh.rclBounds.left, emh.rclBounds.top,
		emh.rclBounds.right, emh.rclBounds.bottom);
	DeleteObject(SelectObject(hdcMeta, hPen));
	PlayEnhMetaFile(hdcMeta, hMetaFile, &rect);
	CloseEnhMetaFile((HDC)hMetaFile);
	DeleteEnhMetaFile(hMetaFile);
	CloseHandle((HDC)hMetaFile);
}

void printImageFile(){
	StartDoc(pd.hDC, &di);
	StartPage(pd.hDC);
	GetClientRect(hMainWnd, &rect);
	hdcMem = CreateCompatibleDC(hdc);
	hbmMem = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
	Rx1 = GetDeviceCaps(hdcMem, LOGPIXELSX);
	Ry1 = GetDeviceCaps(hdcMem, LOGPIXELSY);
	hOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
	FillRect(hdcMem, &rect, WHITE_BRUSH);
	StretchBlt(hdcMem, 0, 0, (int)(rect.right * scale), (int)(rect.bottom * scale),
		hBitmapDC, xBegin, yBegin, rect.right, rect.bottom, SRCCOPY);
	SelectObject(hdcMem, (HBRUSH)GetStockObject(NULL_BRUSH));
	SelectObject(hdcMem, (HPEN)GetStockObject(BLACK_PEN));
	Rectangle(hdcMem, 0, 0, (int)(rect.right * scale), (int)(rect.bottom * scale));
	StretchBlt(pd.hDC, 0, 0, (int)((float)(0.91*rect.right*((float)(Rx / Rx1)))),
		(int)((float)(0.91*rect.bottom*((float)(Ry / Ry1)))),
		hdcMem, 0, 0, rect.right, rect.bottom, SRCCOPY);
	SelectObject(hdcMem, hOld);
	DeleteObject(hbmMem);
	DeleteDC(hdcMem);
	EndPage(pd.hDC);
	EndDoc(pd.hDC);
	DeleteDC(pd.hDC);
}

void createImageFile(){
	GetClientRect(hMainWnd, &rect);
	hdcMeta = CreateEnhMetaFile(NULL, NULL, NULL, NULL);
	flag = false;
	DeleteObject(SelectObject(hdcMeta, hPen));

	DeleteObject(hCompatibleDC);
	DeleteObject(hBitmapDC);

	hCompatibleDC = CreateCompatibleDC(hdc);
	hCompatibleBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
	DeleteObject(SelectObject(hCompatibleDC, hCompatibleBitmap));
	DeleteObject(SelectObject(hCompatibleDC, (HBRUSH)WHITE_BRUSH));
	PatBlt(hCompatibleDC, 0, 0, rect.right, rect.bottom, PATCOPY);
	DeleteObject(SelectObject(hCompatibleDC, hPen));

	hBitmapDC = CreateCompatibleDC(hdc);
	hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
	DeleteObject(SelectObject(hBitmapDC, hBitmap));
	DeleteObject(SelectObject(hBitmapDC, (HBRUSH)WHITE_BRUSH));
	PatBlt(hBitmapDC, 0, 0, rect.right, rect.bottom, PATCOPY);
	DeleteObject(SelectObject(hBitmapDC, hPen));

	InvalidateRect(hMainWnd, NULL, TRUE);
	UpdateWindow(hMainWnd);
}

void SetPen(){
	DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID, shapeData.id_width, shapeData.color);
	DeleteObject(SelectObject(hCompatibleDC, hPen));
	DeleteObject(SelectObject(hBitmapDC, hPen));
	DeleteObject(SelectObject(hdcMeta, hPen));
}
void SetPenWidth(int width){
	shapeData.id_width = width;
	SetPen();
}
void SetPenColor(COLORREF color){
	shapeData.color = color;	
	SetPen();
}

void InitFigure(){
	scale = 1;
	xBegin = 0;
	yBegin = 0;
}

