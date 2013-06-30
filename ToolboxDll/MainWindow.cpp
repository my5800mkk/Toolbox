#include "StdAfx.h"
#include "MainWindow.h"

#include "Viewport.h"

#include "TabbedWindow.h"

#include "Button.h"

CMainWindow::~CMainWindow()
{
	SAFE_DELETE(m_pEditModeToolbar);
	SAFE_DELETE(m_pRollupbarWindow);
}

void CMainWindow::Initialize(WIN_HWND hWnd)
{
	CToolboxWindow::Initialize(hWnd);

	ShowWindow(m_hWnd, SW_MAXIMIZE);

	{
		m_pEditModeToolbar = g_pToolbox->GetWindowManager()->SpawnWindow("EditModeToolbar", "EditMode", 100, 100, 0, 0);

		// Edit mode is placed at the left-most of the main window by default.
		DockWindow(m_pEditModeToolbar, EDockState_Left, 50);
	}

	{
		CTabbedWindow *pTabbedWindow = static_cast<CTabbedWindow *>(g_pToolbox->GetWindowManager()->SpawnWindow("TabbedWindow", "Tab", 100, 100, 0, 0));
	
		// Dock the main tab container to the right of the editmode toolbar.
		DockWindow(pTabbedWindow, EDocKState_Center, 0);

		pTabbedWindow->AttachTab(new CViewport((HWND)gEnv->pRenderer->GetHWND()));

		IToolboxWindow *pTestTab = g_pToolbox->GetWindowManager()->SpawnWindow("ToolboxWindow", "TestTab", 100, 100, 0, 0);
		pTabbedWindow->AttachTab(pTestTab, false);
	}

	{
		m_pRollupbarWindow = g_pToolbox->GetWindowManager()->SpawnWindow("ToolboxWindow", "Rollupbar", 100, 100, 0, 0);

		// Dock the rollupbar to the right-most side of the window by default.
		DockWindow(m_pRollupbarWindow, EDockState_Right, 250);
	}
}

void CMainWindow::PreloadAssets()
{
	m_pLogoButton = new CButton("Toolbox/ce_logo.fw.dds", Vec2(4, 4));
}

void CMainWindow::OnRender(int width, int height)
{
	CToolboxWindow::OnRender(width, height);

	m_pLogoButton->Draw();
}

void CMainWindow::OnClose()
{
	if(gEnv && gEnv->pSystem)
		gEnv->pSystem->Quit();
}