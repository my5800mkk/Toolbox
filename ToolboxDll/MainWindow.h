#pragma once

#include "ToolboxWindow.h"

class CMainWindow
	: public CToolboxWindow
{
public:
	~CMainWindow();

	// CToolboxWindow
	virtual void Initialize(WIN_HWND hWnd);

	virtual bool IsDockable() override { return false; }

	virtual void OnRender(int width, int height) override;
	virtual void PreloadAssets() override;

	virtual void OnClose() override;
	// ~CToolboxWindow

private:
	IToolboxWindow *m_pEditModeToolbar;
	IToolboxWindow *m_pRollupbarWindow;

	CButton *m_pLogoButton;
};