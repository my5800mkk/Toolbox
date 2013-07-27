/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2013.
//////////////////////////////////////////////////////////////////////////
// hWnd wrapper to handle drawing editor windows.
//////////////////////////////////////////////////////////////////////////
// 01/06/2013 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#pragma once

#include "ToolboxWindowBase.h"

#include <IDockWindow.h>
#include <vector>

class CViewport;

class CButton;

struct SDockedWindowData
{
	SDockedWindowData()
		: pWindow(nullptr) {}

	SDockedWindowData(IToolboxWindow *pWnd, EDockState dockState, unsigned int dockFlags)
		: pWindow(pWnd)
		, state(dockState)
		, flags(dockFlags)
		, size(0, 0)
	{
		memset(&pNeighbors, 0, sizeof(SDockedWindowData *) * EDockState_Last);

		size.x = pWindow->GetDefaultDockedWidth();
		size.y = pWindow->GetDefaultDockedHeight();
	}

	void Update()
	{
		for(int i = 0; i < EDockState_Last; i++)
		{
			if(pNeighbors[i] != nullptr)
				pNeighbors[i]->Update();
		}
	}

	void Redraw()
	{
		for(int i = 0; i < EDockState_Last; i++)
		{
			if(pNeighbors[i] != nullptr)
				pNeighbors[i]->Redraw();
		}
	}

	void ResizeNeighbors(LPRECT area);

	IToolboxWindow *pWindow;
	SDockedWindowData *pNeighbors[EDockState_Last];

	unsigned int flags;

	Vec2i size;

	EDockState state;
};

class CDockWindow
	: public CToolboxWindowBase
	, public IDockWindow
{
public:
	CDockWindow();
	~CDockWindow();

	// IToolboxComponent
	virtual void OnTabActivated(IToolboxWindow *pTabContainer) override {}

	virtual void Redraw() override;
	virtual void Update() override;

	virtual WIN_HWND GetHwnd() override { return m_hWnd; }

	virtual void OnResize(int width, int height) override;
	// ~IToolboxComponent

	// IDockWindow
	virtual bool DockWindow(IToolboxWindow *pWindow, unsigned int dockFlags, EDockState dockState, IToolboxWindow *pNeighbor) override;
	// ~IDockWindow

protected:
	// Recursively searches for the docked window data, starting in pDockedWindowData
	SDockedWindowData *FindDockedWindowData(IToolboxWindow *pWindow, SDockedWindowData *pDockedWindowData);

	// Gets the available customizable area.
	// Used to determine how much space docked components need to share.
	bool GetWorkingArea(LPRECT area);

private:
	SDockedWindowData *m_pWorkspaceWindowData;
};

