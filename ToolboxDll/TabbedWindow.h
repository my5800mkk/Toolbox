#pragma once

#include "ToolboxWindow.h"

class CTabbedWindow
	: public CToolboxWindow
{
public:
	CTabbedWindow();
	~CTabbedWindow();

	// CToolboxWindow
	virtual void Redraw() override;
	virtual void OnRender(int width, int height) override;

	virtual void Update() override;

	virtual void OnMouseMove(int x, int y) override;
	virtual void OnLeftMouseButtonDown(int x, int y) override;

	virtual void OnMove(int x, int y) override;
	virtual void OnResize(int width, int height) override;

	virtual void OnCaptureChanged(HWND hWnd) override;

	virtual bool CanMove() override { return false; }
	// ~CToolboxWindow

	void AttachTab(IToolboxComponent *pComponent, bool activate = true);
	bool DetachTab(IToolboxComponent *pComponent);

	bool ActivateTab(int index);

	void OnDetach();

protected:
	STextDrawContext GetTabSize(const char *title, int &xOrigin, int &xEnd, SToolboxStyle *pStyle);

private:
	bool CalculateTabWorkingArea(LPRECT area);
	void ResizeActiveTab();

	std::vector<IToolboxComponent *> m_windowTabs;
	IToolboxComponent *m_pActiveTab;

	bool m_bHoldingTab;
};

