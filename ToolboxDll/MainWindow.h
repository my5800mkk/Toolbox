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

private:
	// Button events
	static void OnLogoPressed(IToolboxWindow *pParentWindow);

	static void OnMinimizeButton(IToolboxWindow *pParentWindow);
	static void OnMaximizeButton(IToolboxWindow *pParentWindow);
	static void OnRestoreButton(IToolboxWindow *pParentWindow);
	static void OnCloseButton(IToolboxWindow *pParentWindow);

private:
	IToolboxWindow *m_pEditModeToolbar;
	IToolboxWindow *m_pRollupbarWindow;

	IToolboxWindow *m_pConsoleWindow;

	IButton *m_pLogoButton;

	IButton *m_pMinimizeButton;
	IButton *m_pMaximizeButton;
	IButton *m_pRestoreButton;
	IButton *m_pCloseButton;
};