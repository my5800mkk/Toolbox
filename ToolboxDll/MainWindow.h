#pragma once

#include "DockWindow.h"

#include <IButton.h>

class CMainWindow
	: public CDockWindow
{
public:
	~CMainWindow();

	// CDockWindow
	virtual void Initialize(WIN_HWND hWnd);

	virtual bool IsDockable() override { return false; }

	virtual void OnRender(int width, int height) override;

	virtual void PreloadAssets() override;

	virtual void OnClose() override;
	// ~CDockWindow

	static void OnLogoPressed();

private:
	IToolboxWindow *m_pEditModeToolbar;
	IToolboxWindow *m_pRollupbarWindow;

	IButton *m_pLogoButton;
};