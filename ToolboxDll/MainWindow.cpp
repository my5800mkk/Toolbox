#include "StdAfx.h"
#include "MainWindow.h"

#include "Viewport.h"

#include "TabbedWindow.h"

CMainWindow::~CMainWindow()
{
	SAFE_DELETE(m_pEditModeToolbar);
	SAFE_DELETE(m_pRollupbarWindow);
}

void CMainWindow::Initialize(WIN_HWND hWnd)
{
	CDockWindow::Initialize(hWnd);

	ShowWindow(m_hWnd, SW_MAXIMIZE);


	{
		CTabbedWindow *pTabbedWindow = static_cast<CTabbedWindow *>(g_pToolbox->GetWindowManager()->SpawnWindow("TabbedWindow", "Tab", 100, 100, 0, 0));
		DockWindow(pTabbedWindow, EDockFlag_Static, EDockState_TopOf, nullptr);
		
		pTabbedWindow->AttachTab(new CViewport((HWND)gEnv->pRenderer->GetHWND()));

		IToolboxWindow *pTestTab = g_pToolbox->GetWindowManager()->SpawnWindow("ToolboxWindow", "TestTab", 100, 100, 0, 0);
		pTabbedWindow->AttachTab(pTestTab, false);
	}

	{
		m_pEditModeToolbar = g_pToolbox->GetWindowManager()->SpawnWindow("EditModeToolbar", "EditMode", 40, 100, 0, 0);

		// Edit mode is placed at the left-most of the main window by default.
		DockWindow(m_pEditModeToolbar, EDockFlag_NoResize, EDockState_LeftOf, nullptr);
	}

	{
		m_pRollupbarWindow = g_pToolbox->GetWindowManager()->SpawnWindow("Rollupbar", "Rollupbar", 250, 100, 0, 0);

		// Dock the rollupbar to the right-most side of the window by default.
		DockWindow(m_pRollupbarWindow, 0, EDockState_RightOf, nullptr);
	}
}

void CMainWindow::PreloadAssets()
{
	m_pLogoButton = CreateButton("Toolbox/ce_logo.fw.dds");
	m_pLogoButton->SetPosition(Vec2(4, 4));

	m_pLogoButton->AddOnPressFunc(OnLogoPressed);
}

void CMainWindow::OnRender(int width, int height)
{
	CDockWindow::OnRender(width, height);
}

void CMainWindow::OnLogoPressed()
{
}

void CMainWindow::OnClose()
{
	if(gEnv && gEnv->pSystem)
		gEnv->pSystem->Quit();
}