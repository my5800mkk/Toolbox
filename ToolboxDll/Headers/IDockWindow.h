#pragma once

#include <IToolboxWindow.h>

struct IDockableWindow;

enum EDockFlags
{
	EDockFlag_NoResize = (1 << 0),
	EDockFlag_NoMove = (1 << 1),

	/// <summary>
	/// If set, the Resize function of the window itself will never be called.
	/// </summary>
	EDockFlag_ResizeNeighborsOnly = (1 << 2),

	EDockFlag_Static = EDockFlag_NoResize | EDockFlag_NoMove,
};

/// <summary>
/// Window that is capable of being docked onto.
/// </summary>
struct IDockWindow\
{
	virtual bool DockWindow(IToolboxWindow *pWindow, unsigned int dockFlags, EDockState dockState, IToolboxWindow *pNeighbor) = 0;
};