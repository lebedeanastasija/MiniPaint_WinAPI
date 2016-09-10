#include <windows.h>

//�������� ������� ��������� ���������
LRESULT CALLBACK WindowProcess(HWND, UINT, WPARAM, LPARAM);
TCHAR mainMessage[] = L"I am maladzec :)";

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR pCommandLine,
	int nCommandShow)
{
	TCHAR ClassName[] = L"MyClass";	
	HWND hWindow;//������� ���������� ����
	MSG msg;

	WNDCLASSEX windowClass;	//������� ��������� ��� ��������� � ������ ������ WNDCLASSEX
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProcess;//��������� �� ���������������� �������
	windowClass.lpszMenuName = L"IDR_MENU1";
	windowClass.lpszClassName = ClassName;
	windowClass.cbWndExtra = NULL;
	windowClass.cbClsExtra = NULL;
	windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.hInstance = hInstance;

	if (!RegisterClassEx(&windowClass)) {
		MessageBox(NULL, L"Class registration error!", L"Error", MB_OK);
		return NULL;
	}
	
	//CO����� ������ � ���������� ��� �� ������
	hWindow = CreateWindow(
		ClassName,
		L"Mini Paint", //window's name
		WS_OVERLAPPEDWINDOW | WS_VSCROLL, //window viewing mode
		CW_USEDEFAULT, // window x-state
		NULL, //window y-state
		CW_USEDEFAULT, // window width
		NULL, //window height
		HWND(NULL), // parent window handler
		NULL, //menu handler
		HINSTANCE(hInstance), //application instance
		NULL); //get nothing from WndProc
			
	if (!hWindow) {
		MessageBox(NULL, L"Window creation fail!", L"Error", MB_OK);
		return NULL;
	}

	ShowWindow(hWindow, nCommandShow);
	UpdateWindow(hWindow);

	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;//���������� �������� ��� ������� ��� ������
}

LRESULT CALLBACK WindowProcess(HWND hWindow, //indow handler
						 UINT uMessage, //message for OS
						 WPARAM wParameter, //parametres
						 LPARAM lParameter) //message for the next applying
{
	HDC hDeviceContext;
	PAINTSTRUCT paintStruct;
	RECT rectPlace;
	COLORREF colorText = RGB(255, 0, 0);

	switch (uMessage) {
	case WM_PAINT:
		hDeviceContext = BeginPaint(hWindow, &paintStruct);
		GetClientRect(hWindow, &rectPlace);
		SetTextColor(hDeviceContext, colorText);
		DrawText(hDeviceContext, mainMessage, -1, &rectPlace, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		EndPaint(hWindow, &paintStruct);
		break;
	case WM_DESTROY:
		PostQuitMessage(NULL);
		break;
	default:
		return DefWindowProc(hWindow, uMessage, wParameter, lParameter);
	}
	return NULL;
}


