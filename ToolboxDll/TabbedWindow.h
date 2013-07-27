#pragma once

#include "DockWindow.h"

class CTabbedWindow
	: public CDockWindow
{
public:
	CTabbedWindow();
	~CTabbedWindow();

	// CDockWindow
	virtual void Redraw() override;
	virtual void OnRender(int width, int height) override;

	virtual void Update() override;

	virtual void OnMouseMove(int x, int y) override;
	virtual void OnLeftMouseButtonDown(int x, int y) override;

	virtual void OnMove(int x, int y) override;
	virtual void OnResize(int width, int height) override;

	virtual bool CanMove() override { return false; }
	// ~CDockWindow

	void AttachTab(IToolboxComponent *pComponent, bool activate = true);
	bool DetachTab(IToolboxComponent *pComponent);

	bool ActivateTab(int index);

protected:
	STextDrawContext GetTabSize(const char *title, int &xOrigin, int &xEnd, SToolboxStyle *pStyle);

private:
	bool CalculateTabWorkingArea(LPRECT area);
	void ResizeActiveTab();

	std::vector<IToolboxComponent *> m_windowTabs;
	IToolboxComponent *m_pActiveTab;
};

