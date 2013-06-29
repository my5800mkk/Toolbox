#pragma once

#include "ToolboxWindowBase.h"

class CDialogWindow
	: public CToolboxWindowBase
{
public:
	~CDialogWindow() {}

	// CToolboxWindowBase
	virtual bool CanResize(uint32 resizeState) override { return false; }

	virtual void OnTabActivated(IToolboxWindow *pTabContainer) override {}

	virtual void Update() override {}

	virtual bool IsDockable() override { return false; }
	virtual IDockWindow *GetDockOwner() override { return nullptr; }
	virtual void OnDocked(IDockWindow *pOwner, EDockState dockState) override {}

	virtual void OnPostRender() override {}

	virtual void OnClose() override {}

	virtual void OnMouseWheel(short delta) override {}

	virtual void OnLeftMouseButtonDoubleClick(int x, int y) override {}

	virtual void OnHide() override {}
	virtual void OnShow() override {}

	virtual void OnMinimize() override {}
	// ~CToolboxWindowBase
};

