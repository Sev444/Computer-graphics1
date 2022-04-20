#include <windows.h>
#include <math.h>
#define PI 3.1415

struct Point {
	float x, y;
};
struct Line {
	Point beg, end;
};

Line line1 = { 10, 10, 100, 100 };
Line line2 = { 40, 80, 200, 500 };


void transfer(Line& line, float Dx, float Dy) {
	line.beg.x += Dx, line.beg.y += Dy;
	line.end.x += Dx, line.end.y += Dy;
}
void scale(Line& line, double Sx, double Sy) {
	line.end.x = line.beg.x + (line.end.x - line.beg.x) * Sx;
	line.end.y = line.beg.y + (line.end.y - line.beg.y) * Sy;
}
void rotate(Line& line, double angle) {
	line.end.x = line.beg.x + (line.end.x - line.beg.x) * cos(angle * PI / 180)
		- (line.end.y - line.beg.y) * sin(angle * PI / 180);
	line.end.y = line.beg.y + (line.end.x - line.beg.x) * sin(angle * PI / 180)
		+ (line.end.y - line.beg.y) * cos(angle * PI / 180);
}
void line_b(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color) {
	int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
	int dy = abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2, e2;
	while (1) {
		SetPixel(hdc, x1, y1, color);
		if (x1 == x2 && y1 == y2)
			break;
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			x1 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y1 += sy;
		}
	}
}

void control(int key, Line& line) {
	if (key == int('W')) transfer(line, 0, -2);
	if (key == int('A')) transfer(line, -2, 0);
	if (key == int('S')) transfer(line, 0, 2);
	if (key == int('D')) transfer(line, 2, 0);
	if (key == int('Z')) scale(line, 1.02, 1.02);
	if (key == int('X')) scale(line, 0.98, 0.98);
	if (key == int('Q')) rotate(line, 2);
	if (key == int('E')) rotate(line, -2);
}

void draw(HDC hdc, Line& line, COLORREF color) {
	HPEN hPen;
	hPen = CreatePen(PS_DASHDOT, 2, color);
	SelectObject(hdc, hPen);
	MoveToEx(hdc, line.beg.x, line.beg.y, NULL);
	LineTo(hdc, line.end.x, line.end.y);
	DeleteObject(hPen);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps; RECT Rect; HDC hdc, hCmpDC; HBITMAP hBmp;

	switch (messg) {

	case WM_PAINT: {
		GetClientRect(hWnd, &Rect);
		hdc = BeginPaint(hWnd, &ps);

		// �������� �������� ��������� ��� ������� �����������
		hCmpDC = CreateCompatibleDC(hdc);
		hBmp = CreateCompatibleBitmap(hdc, Rect.right - Rect.left, Rect.bottom - Rect.top);
		SelectObject(hCmpDC, hBmp);

		// �������� ������� ������
		LOGBRUSH br;
		br.lbStyle = BS_SOLID;
		br.lbColor = 0xFFFFFF;
		HBRUSH brush;
		brush = CreateBrushIndirect(&br);
		FillRect(hCmpDC, &Rect, brush);
		DeleteObject(brush);

		// ���������
		draw(hCmpDC, line1, 0x0000FF);
		draw(hCmpDC, line2, 0x00FF00);
		//line_b(hCmpDC, line1.beg.x, line1.beg.y, line1.end.x, line1.end.y, 0x0000FF);
		//line_b(hCmpDC, line2.beg.x, line2.beg.y, line2.end.x, line2.end.y, 0x00FF00);

		// �������� ����������� �� �������� ��������� �� �����
		SetStretchBltMode(hdc, COLORONCOLOR);
		BitBlt(hdc, 0, 0, Rect.right - Rect.left, Rect.bottom - Rect.top, hCmpDC, 0, 0, SRCCOPY);

		// ������� �������� ��������� �������
		DeleteDC(hCmpDC);
		DeleteObject(hBmp);
		hCmpDC = NULL;

		EndPaint(hWnd, &ps);
	} break;

	case WM_ERASEBKGND:
		return 1;
		break;

	case WM_KEYDOWN:
		if (GetKeyState(int('C')))
			control(int(wParam), line1);
		else
			control(int(wParam), line2);
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return (DefWindowProc(hWnd, messg, wParam, lParam));
	}
	//InvalidateRect(hWnd, NULL, TRUE);
	return (0);
}
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	HWND hWnd;
	MSG lpMsg;
	WNDCLASS WndClass;

	// ��������� ��������� ������ ����
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = L"CG_WAPI_Template";

	// ������������ ����� ����
	if (!RegisterClass(&WndClass)) {
		MessageBox(NULL, L"�� ���� ���������������� ����� ����!", L"������", MB_OK);
		return 0;
	}

	// ������� �������� ���� ����������
	hWnd = CreateWindow(
		L"CG_WAPI_Template", // ��� ������ 
		L"������������ ������� ������������ ������ �1", // ����� ���������
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, // ����� ���� 
		CW_USEDEFAULT, CW_USEDEFAULT, // ������� ������ �������� ���� 
		1000, 500, // ������ � ������ ���� 
		(HWND)NULL, // ��������� �� ������������ ���� NULL 
		(HMENU)NULL, // ������������ ���� ������ ���� 
		(HINSTANCE)hInstance, // ��������� �� ������� ����������
		NULL); // ���������� � �������� lParam � ������� WM_CREATE


	// ���� ���� �� ������� �������
	if (!hWnd) {
		MessageBox(NULL, L"�� ������� ������� ������� ����!", L"������", MB_OK);
		return 0;
	}

	// ���������� ����
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// ��������� ���� ��������� ��������� �� �������� ����������
	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg);
		DispatchMessage(&lpMsg);
	}
	return (int)lpMsg.wParam;
}