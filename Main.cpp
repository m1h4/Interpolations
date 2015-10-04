#include "Globals.h"
#include "Main.h"
#include "Resource.h"

#define _USE_MATH_DEFINES
#include <math.h>

double g_Power = 2.5;

typedef DOUBLE (*LPINTERPFUNCTION)(DOUBLE p1,DOUBLE p2,DOUBLE s);

double catmullrominterp(double p1,double p2,double p3,double p4,double s)
{
	double s2 = pow(s,2.0);
	double s3 = pow(s,3.0);

	return (p1 * (-s3 + 2.0 * s2 - s) + p2 * (3.0 * s3 - 5.0 * s2 + 2.0) + p3 * (-3.0 * s3 + 4.0 * s2 + s) + p4 * (s3 - s2)) / 2.0;
}

double hermiteinterp(double p1,double p2,double p3,double p4,double s)
{
	double t2 = s * s;
	double t3 = t2 * s;

	double kp0 = (2.0 * t2 - t3 - s) / 2.0;
	double kp1 = (3.0 * t3 - 5.0 * t2 + 2.0) / 2.0;
	double kp2 = (4.0 * t2 - 3.0 * t3 + s) / 2.0;
	double kp3 = (t3 - t2) / 2.0;

	return p1 * kp0 + p2 * kp1 + p3 * kp2 + p4 * kp3;
}

double linearinterp(double p1,double p2,double s,double p)
{
	return p1 + pow(s,p) * (p2 - p1);
}

double cubicinterp(double p1,double p2,double s)
{
	double s1 = 2.0 * pow(s,3.0);
	double s2 = 3.0 * pow(s,2.0);

	return p1 * (s1 - s2 + 1.0) + p2 * (s2 - s1);
}

double cosinterp(double p1,double p2,double s)
{
	double f1 = (1.0 - cos(M_PI * s)) * 0.5;

	return  p1 * (1.0 - f1) + p2 * f1;
}

INT WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR cmdLine,INT cmdShow)
{
	//InitCommonControls();

	WNDCLASSEX wcex;
	ZeroMemory(&wcex,sizeof(wcex));
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_CLASSDC|CS_DBLCLKS;
	wcex.lpfnWndProc	= WinProcedure;
	wcex.hInstance		= GetModuleHandle(NULL);
	wcex.hIcon			= LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_MAIN));
	wcex.hIconSm		= LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_MAIN));
	wcex.hCursor		= LoadCursor(NULL,IDC_ARROW);
	wcex.lpszClassName	= L"Interpolations";

	if(!RegisterClassEx(&wcex))
		return 1;

	HWND hWnd = CreateWindowEx(NULL,L"Interpolations",L"Interpolations",WS_OVERLAPPEDWINDOW|WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,MAIN_WINDOW_WIDTH,MAIN_WINDOW_HEIGHT,GetDesktopWindow(),NULL,GetModuleHandle(NULL),NULL);
	if(!hWnd)
		return 1;

	MSG msg;
	ZeroMemory(&msg,sizeof(msg));

	while(GetMessage(&msg,NULL,NULL,NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DestroyWindow(hWnd);
	UnregisterClass(L"Interpolations",GetModuleHandle(NULL));

	return (INT)msg.wParam;
}

LRESULT WINAPI WinProcedure(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
			return WinCreate(hWnd) ? 0 : -1;

		case WM_DESTROY:
			return WinDestroy(hWnd) ? 0 : -1;

		case WM_CLOSE:
			return WinClose(hWnd) ? 0 : -1;

		case WM_COMMAND:
			return WinCommand(hWnd,wParam,lParam) ? 0 : -1;

		case WM_CONTEXTMENU:
			return WinContextMenu(hWnd,wParam,lParam) ? 0 : -1;

		case WM_LBUTTONDOWN:
			return WinButtonDown(hWnd,msg,LOWORD(lParam),HIWORD(lParam)) ? 0 : -1;

		case WM_KEYDOWN:
			return WinKeyDown(hWnd,wParam,lParam) ? 0 : -1;

		case WM_SIZE:
			return WinSize(hWnd,msg,wParam,lParam) ? 0 : -1;

		case WM_PAINT:
			return WinPaint(hWnd) ? 0 : -1;
	}

	return DefWindowProc(hWnd,msg,wParam,lParam);
}
BOOL WinCreate(HWND hWnd)
{
	return TRUE;
}

BOOL WinDestroy(HWND hWnd)
{
	return TRUE;
}

BOOL WinClose(HWND hWnd)
{
	PostQuitMessage(NULL);

	return TRUE;
}

BOOL WinCommand(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	}

	return TRUE;
}

BOOL WinContextMenu(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return TRUE;
}

BOOL WinButtonDown(HWND hWnd,UINT button,LONG mx,LONG my)
{
	return TRUE;
}

BOOL WinSize(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	InvalidateRect(hWnd,NULL,FALSE);

	return TRUE;
}

BOOL WinKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	switch(wParam)
	{
	case VK_UP:
		g_Power += 0.1;
		InvalidateRect(hWnd,NULL,FALSE);
		break;

	case VK_DOWN:
		if(g_Power > 1.0)
			g_Power -= 0.1;
		InvalidateRect(hWnd,NULL,FALSE);
		break;
	}

	return TRUE;
}

VOID DrawLerpFunction(HDC hDC,int startX,int endX,int startY,int endY,int elements,double power)
{
	MoveToEx(hDC,startX,startY,NULL);
	LineTo(hDC,endX,startY);

	for(unsigned int i = startX; i <= endX; i += 8)
	{
		MoveToEx(hDC,i,startY + 2,NULL);
		LineTo(hDC,i,startY - 3);
	}

	MoveToEx(hDC,startX,startY,NULL);
	LineTo(hDC,startX,endY);

	for(unsigned int i = endY; i <= startY; i += 8)
	{
		MoveToEx(hDC,startX + 2,i,NULL);
		LineTo(hDC,startX - 3,i);
	}

	WCHAR text[256];
	wsprintf(text,L"Linear (Power %g)",power);
	RECT rect = {startX+10,endY,0,0};
	DrawText(hDC,text,-1,&rect,DT_NOCLIP);

	HGDIOBJ old = SelectObject(hDC,CreatePen(PS_DOT,1,RGB(192,192,192)));

	for(int i = 0; i < elements; i += 16)
	{
		MoveToEx(hDC,double(startX) + double(endX - startX) / double(elements) * double(i),startY,NULL);
		LineTo(hDC,double(startX) + double(endX - startX) / double(elements) * double(i),double(startY) + double(endY - startY) * linearinterp(0.0,1.0,double(i)/double(elements),power));
	}

	DeleteObject(SelectObject(hDC,old));

	for(int i = 0; i < elements; ++i)
	{
		MoveToEx(hDC,double(startX) + double(endX - startX) / double(elements) * double(i),double(startY) + double(endY - startY) * linearinterp(0.0,1.0,double(i)/double(elements),power),NULL);
		LineTo(hDC,double(startX) + double(endX - startX) / double(elements) * double(i+1),double(startY) + double(endY - startY) * linearinterp(0.0,1.0,double(i+1)/double(elements),power));
	}
}

VOID DrawInterpFunction(HDC hDC,int startX,int endX,int startY,int endY,int elements,LPINTERPFUNCTION function,LPTSTR name)
{
	MoveToEx(hDC,startX,startY,NULL);
	LineTo(hDC,endX,startY);

	for(unsigned int i = startX; i <= endX; i += 8)
	{
		MoveToEx(hDC,i,startY + 2,NULL);
		LineTo(hDC,i,startY - 3);
	}

	MoveToEx(hDC,startX,startY,NULL);
	LineTo(hDC,startX,endY);

	for(unsigned int i = endY; i <= startY; i += 8)
	{
		MoveToEx(hDC,startX + 2,i,NULL);
		LineTo(hDC,startX - 3,i);
	}

	RECT rect = {startX+10,endY,0,0};
	DrawText(hDC,name,-1,&rect,DT_NOCLIP);

	HGDIOBJ old = SelectObject(hDC,CreatePen(PS_DOT,1,RGB(192,192,192)));

	for(int i = 0; i < elements; i += 16)
	{
		MoveToEx(hDC,double(startX) + double(endX - startX) / double(elements) * double(i),startY,NULL);
		LineTo(hDC,double(startX) + double(endX - startX) / double(elements) * double(i),double(startY) + double(endY - startY) * function(0.0,1.0,double(i)/double(elements)));
	}

	DeleteObject(SelectObject(hDC,old));

	for(int i = 0; i < elements; ++i)
	{
		MoveToEx(hDC,double(startX) + double(endX - startX) / double(elements) * double(i),double(startY) + double(endY - startY) * function(0.0,1.0,double(i)/double(elements)),NULL);
		LineTo(hDC,double(startX) + double(endX - startX) / double(elements) * double(i+1),double(startY) + double(endY - startY) * function(0.0,1.0,double(i+1)/double(elements)));
	}
}

BOOL WinPaint(HWND hWnd)
{
	RECT client;
	GetClientRect(hWnd,&client);

	PAINTSTRUCT paint;
	HDC hDC = BeginPaint(hWnd,&paint);
	HDC hCompatibleDC = CreateCompatibleDC(hDC);
	HBITMAP hBackbuffer = CreateCompatibleBitmap(hDC,client.right-client.left,client.bottom-client.top);
	HBITMAP hOldBitmap  = (HBITMAP)SelectObject(hCompatibleDC,hBackbuffer);

	SetBkMode(hCompatibleDC,TRANSPARENT);
	DeleteObject(SelectObject(hCompatibleDC,GetStockObject(DEFAULT_GUI_FONT)));

	HBRUSH brush = CreateSolidBrush(RGB(255,255,255));
	FillRect(hCompatibleDC,&client,brush);
	DeleteObject(brush);

	int startX = client.left + 10;
	int endX = client.right / 2 - 10;
	int startY = client.bottom / 2 - 10;
	int endY = client.top + 10;
	int elements = 400;

	DrawInterpFunction(hCompatibleDC,startX,endX,startY,endY,elements,cubicinterp,L"Cubic");
	
	startY = client.bottom - 10;
	endY = client.bottom / 2 + 10;

	DrawInterpFunction(hCompatibleDC,startX,endX,startY,endY,elements,cosinterp,L"Cosinus");

	startX = client.right / 2 + 10;
	endX = client.right - 10;
	startY = client.bottom / 2 - 10;
	endY = client.top + 10;

	DrawLerpFunction(hCompatibleDC,startX,endX,startY,endY,elements,1.0 / g_Power);

	startY = client.bottom - 10;
	endY = client.bottom / 2 + 10;

	DrawLerpFunction(hCompatibleDC,startX,endX,startY,endY,elements,g_Power);

	//POINTF points[] = {{0.0f,0.0f},{0.3f,0.5f},{0.8f,0.7f},{1.0f,1.0f}};

	//for(int i = 0; i < elements; ++i)
	//{
	//	MoveToEx(hCompatibleDC,double(startX) + double(endX - startX) / double(elements) * double(i),double(startY) + double(endY - startY) * hermite(points[0].y,points[1].y,points[2].y,points[3].y,double(i)/double(elements)),NULL);
	//	LineTo(hCompatibleDC,double(startX) + double(endX - startX) / double(elements) * double(i+1),double(startY) + double(endY - startY) * hermite(points[0].y,points[1].y,points[2].y,points[3].y,double(i+1)/double(elements)));
	//}

	BitBlt(hDC,0,0,client.right-client.left,client.bottom-client.top,hCompatibleDC,0,0,SRCCOPY);
	SelectObject(hCompatibleDC,hOldBitmap);
	DeleteObject(hBackbuffer);
	DeleteDC(hCompatibleDC);
	EndPaint(hWnd,&paint);

	return TRUE;
}