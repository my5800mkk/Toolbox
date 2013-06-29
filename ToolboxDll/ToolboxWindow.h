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

class CToolboxWindow
	: public CToolboxWindowBase
	, public IDockWindow
{
public:
	CToolboxWindow();
	~CToolboxWindow();

	// IToolboxComponent
	virtual void OnTabActivated(IToolboxWindow *pTabContainer) override {}

	virtual void Redraw() override;
	virtual void Update() override;

	virtual WIN_HWND GetHwnd() override { return m_hWnd; }
	// ~IToolboxComponent

	// IToolboxWindow
	virtual bool CanResize(uint32 resizeState) override;
	virtual bool CanMove() override { return true; }

	// IToolboxWindow docking
	virtual bool IsDockable() override { return true; }

	virtual IDockWindow *GetDockOwner() override { return m_pDockOwner; }
	virtual void OnDocked(IDockWindow *pOwner, EDockState dockState) override { m_pDockOwner = static_cast<CToolboxWindow *>(pOwner); m_dockState = dockState; }

	// IToolboxWindow callbacks
	virtual void OnRender(int width, int height) override;
	virtual void OnPostRender() override {}

	virtual void OnClose() override {}

	virtual void OnMouseMove(int x, int y) override;
	virtual void OnMouseWheel(short delta) override {}

	virtual void OnLeftMouseButtonDoubleClick(int x, int y) override {}

	virtual void OnMove(int x, int y) override;
	virtual void OnResize(int width, int height) override;

	virtual void OnHide() override {}
	virtual void OnShow() override {}

	virtual void OnMinimize() override {}
	// ~IToolboxWindow

	// IDockWindow
	virtual uint32 GetAvailableDockStates() override;
	virtual bool ResizeDockedComponent(IToolboxWindow *pComponent, int width, int height) override;

	virtual bool CanDockWindow(IToolboxWindow *pWindow, EDockState dockState) override;
	virtual bool DockWindow(IToolboxWindow *pWindow, EDockState dockState, int minSize) override;
	virtual bool UnDockWindow(IToolboxWindow *pWindow) override;
	virtual void UnDockAllWindows() override;

	virtual SDockedWindow **GetDockedWindows() override { return m_dockedWindows; }
	// ~IDockWindow

	// CToolboxWindowBase
	virtual int GetTopBarHeight(SToolboxStyle *pStyle) override;

	virtual void DrawWindowBorder(float width, float height) override;
	// ~CToolboxWindowBase

	CButton *AddButton(int x, int y, const char *imagePath);

protected:
	void CalculateDockedWorkingArea(LPRECT rect, EDockState dockState, int size);

	void ResizeDockedWindows();

protected:
	IDockWindow *m_pDockOwner;
	EDockState m_dockState;

	// TODO: Remove
	std::vector<CButton *> m_buttons;

	SDockedWindow *m_dockedWindows[EDockState_Last];
};

