#include "StdAfx.h"
#include "StartupSplash.h"

#include "resource1.h"

CStartupSplash *CStartupSplash::m_pSplash = nullptr;

CStartupSplash::CStartupSplash()
	: m_hBitmap(nullptr)
{
	m_pSplash = this;

	m_status = "Starting Toolbox";

	WNDCLASS wndClass;
	memset(&wndClass, 0, sizeof(WNDCLASS));

	wndClass.hInstance = GetModuleHandle(0);
	wndClass.lpszClassName = "CryToolboxSplash";
	wndClass.lpfnWndProc = WndProc;
	wndClass.hIcon = LoadIcon(g_pToolbox->GetDllHandle(), MAKEINTRESOURCE(IDI_ICON2));

	RegisterClass(&wndClass);

	RECT workAreaRect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0);

	int workAreaWidth = workAreaRect.right - workAreaRect.left;
	int workAreaHeight = workAreaRect.bottom - workAreaRect.top;

	int width = 512;
	int height = 256;

	// Create splash at the center of the current monitor.
	m_hWnd = CreateWindow("CryToolboxSplash", "CryENGINE Community Toolbox", WS_VISIBLE, (workAreaWidth - width) / 2, (workAreaHeight - height) / 2, width, height, nullptr, nullptr, wndClass.hInstance, nullptr);

	SetWindowLong(m_hWnd, GWL_STYLE, WS_VISIBLE);

	SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

	ShowWindow(m_hWnd, SW_SHOWNORMAL);
    SetFocus(m_hWnd);
    SetForegroundWindow(m_hWnd);

	UpdateWindow(m_hWnd);
}

CStartupSplash::~CStartupSplash()
{
	if(m_hBitmap != nullptr)
		DeleteObject(m_hBitmap);

	DestroyWindow(m_hWnd);
}

void CStartupSplash::SetStatus(const char *msg) 
{
	m_status = msg; 

	InvalidateRect(m_hWnd, NULL, TRUE);
	UpdateWindow(m_hWnd);
}

LRESULT CALLBACK CStartupSplash::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		{
			HBITMAP hBitmap = (HBITMAP)LoadImage(GetModuleHandle(0), "D:\\Dev\\CryENGINE_FullCode_GFx_PC_v3_4_5_6665 - Copy\\Game\\Toolbox\\Splash.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			if(hBitmap == nullptr)
				CryMessageBox("Failed to load startup splash!", "Toolbox splash load!", 0);

			m_pSplash->SetBitmap(hBitmap);
		}
		break;
	case WM_PAINT:
		{
			HBITMAP hBitmap = m_pSplash->GetBitmap();

			PAINTSTRUCT 	ps;
    		HDC 			hdc;
    		BITMAP 			bitmap;
    		HDC 			hdcMem;
			HGDIOBJ 		oldBitmap;

    		hdc = BeginPaint(hWnd, &ps);

    		hdcMem = CreateCompatibleDC(hdc);
			oldBitmap = SelectObject(hdcMem, hBitmap);

			GetObject(hBitmap, sizeof(bitmap), &bitmap);
			BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

			SelectObject(hdcMem, oldBitmap);
			DeleteDC(hdcMem);

			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, 0xAFAFBE);

			RECT area;
			GetClientRect(hWnd, &area);
			area.bottom -= 5;

			DrawText(hdc, m_pSplash->GetStatus(), -1, &area, DT_SINGLELINE | DT_CENTER | DT_BOTTOM);

    		EndPaint(hWnd, &ps);
		}
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}