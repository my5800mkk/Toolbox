#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

struct IToolboxWindow;

struct IToolboxComponent
{
	/// <summary>
	/// Called when this component is set as the active tab in a tab container.
	/// </summary>
	virtual void OnTabActivated(IToolboxWindow *pTabContainer) = 0;

	virtual RECT GetRect() = 0;

	virtual void Resize(RECT area) = 0;

	virtual void Redraw() = 0;
	virtual void Update() = 0;

	virtual WIN_HWND GetHwnd() = 0;
};