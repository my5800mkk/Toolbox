#pragma once

#include "Toolbox.h"

class CStartupSplash
{
public:
	CStartupSplash();
	~CStartupSplash();

	void SetStatus(const char *msg);
	const char *GetStatus() { return m_status; }

protected:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND GetHwnd() { return m_hWnd; }

	void SetBitmap(HBITMAP hBitmap) { m_hBitmap = hBitmap; }
	HBITMAP GetBitmap() { return m_hBitmap; }

private:
	HWND m_hWnd;

	HBITMAP m_hBitmap;

	const char *m_status;

	static CStartupSplash *m_pSplash;
};

