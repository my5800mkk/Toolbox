#pragma once

#include <IToolboxWindow.h>

struct IDockableWindow;

struct SDockedWindow
{
	SDockedWindow()
		: pWindow(nullptr) {}

	SDockedWindow(IToolboxWindow *pWnd, EDockState dockState, int defaultSize)
		: pWindow(pWnd)
		, state(dockState)
		, size(defaultSize)
	{
		memset(&workArea, 0, sizeof(RECT));
	}

	IToolboxWindow *pWindow;

	EDockState state;
	
	int size;

	RECT workArea;
};

/// <summary>
/// Window that is capable of being docked onto.
/// </summary>
struct IDockWindow
{
	virtual unsigned int GetAvailableDockStates() = 0;
	virtual bool ResizeDockedComponent(IToolboxWindow *pDockableWindow, int width, int height) = 0;

	virtual bool CanDockWindow(IToolboxWindow *pWindow, EDockState dockState) = 0;
	virtual bool DockWindow(IToolboxWindow *pWindow, EDockState dockState, int minSize) = 0;
	virtual bool UnDockWindow(IToolboxWindow *pWindow) = 0;
	virtual void UnDockAllWindows() = 0;

	/// <summary>
	/// Gets array of size EDockState_Last that contains SDockedWindow pointers.
	/// </summary>
	virtual SDockedWindow **GetDockedWindows() = 0;
};