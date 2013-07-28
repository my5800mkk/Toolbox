#include "StdAfx.h"
#include "MainWindow.h"

#include "Viewport.h"

#include "TabbedWindow.h"

#include <IToolboxWindowManager.h>
#include <IViewportManager.h>

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
		
		pTabbedWindow->AttachTab(g_pToolbox->GetViewportManager()->CreateViewport((HWND)gEnv->pRenderer->GetHWND()));

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

	m_pLogoButton->AddEventCallback(EButtonEvent_OnClick, OnLogoPressed);

	m_pMinimizeButton = CreateButton("Toolbox/minimize.fw.dds");
	m_pMinimizeButton->AddEventCallback(EButtonEvent_OnClick, OnMinimizeButton);

	m_pMaximizeButton = CreateButton("Toolbox/maximize.fw.dds");
	m_pMaximizeButton->AddEventCallback(EButtonEvent_OnClick, OnMaximizeButton);

	m_pRestoreButton = CreateButton("Toolbox/restore.fw.dds");
	m_pRestoreButton->AddEventCallback(EButtonEvent_OnClick, OnRestoreButton);

	m_pCloseButton = CreateButton("Toolbox/close.fw.dds");
	m_pCloseButton->AddEventCallback(EButtonEvent_OnClick, OnCloseButton);
}

void CMainWindow::OnRender(int width, int height)
{
	static const int iconSize = 32;

	Vec2 pos = Vec2(width - (float)iconSize, -10);

	m_pCloseButton->SetPosition(pos);
	pos.x -= iconSize;

	if(IsMaximized())
	{
		m_pRestoreButton->SetPosition(pos);
		m_pRestoreButton->Hide(false);
		m_pMaximizeButton->Hide(true);
	}
	else
	{
		m_pMaximizeButton->SetPosition(pos);
		m_pMaximizeButton->Hide(false);
		m_pRestoreButton->Hide(true);
	}

	pos.x -= iconSize;
	m_pMinimizeButton->SetPosition(pos);

	CDockWindow::OnRender(width, height);
}

void CMainWindow::OnClose()
{
	if(gEnv && gEnv->pSystem)
		gEnv->pSystem->Quit();
}

void CMainWindow::OnLogoPressed(IToolboxWindow *pParentWindow)
{
}

void CMainWindow::OnMinimizeButton(IToolboxWindow *pParentWindow)
{
	pParentWindow->Minimize();
}

void CMainWindow::OnMaximizeButton(IToolboxWindow *pParentWindow)
{
	pParentWindow->Maximize();
}

void CMainWindow::OnRestoreButton(IToolboxWindow *pParentWindow)
{
	pParentWindow->Restore();
}

void CMainWindow::OnCloseButton(IToolboxWindow *pParentWindow)
{
	if(gEnv && gEnv->pSystem)
		gEnv->pSystem->Quit();
}