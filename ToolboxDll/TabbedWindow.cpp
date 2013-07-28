#include "StdAfx.h"
#include "TabbedWindow.h"

#include "RenderUtils.h"

#include <IToolboxWindowManager.h>

CTabbedWindow::CTabbedWindow()
	: m_pActiveTab(nullptr)
{
}

CTabbedWindow::~CTabbedWindow()
{
}

void CTabbedWindow::Redraw()
{
	CDockWindow::Redraw();

	if(m_pActiveTab != nullptr)
		m_pActiveTab->Redraw();
}

void CTabbedWindow::OnRender(int width, int height)
{
	SToolboxStyle *pStyle = g_pToolbox->GetWindowStyle();

	int xOrigin = 0;
	int xEnd = 0;

	// Draw active tab
	for(int i = 0; i < m_windowTabs.size(); i++)
	{
		IToolboxComponent *pTabComponent = m_windowTabs.at(i);

		char title[256]; 
		GetWindowText((HWND)pTabComponent->GetHwnd(), title, 256);

		STextDrawContext drawContext = GetTabSize(title, xOrigin, xEnd, pStyle);

		ColorB bgColor;

		if(m_pActiveTab == pTabComponent)
		{
			drawContext.m_colorOverride = pStyle->activeTabFontColor;
			bgColor = pStyle->activeTabColor;
		}
		else
		{
			drawContext.m_colorOverride = pStyle->tabFontColor;
			bgColor = pStyle->backgroundColor;
		}
		
		SRenderUtils::DrawBox(Vec2((float)xOrigin, 0), Vec2((float)xEnd, (float)pStyle->tabHeight), bgColor);

		AddQueuedText(drawContext, xOrigin + pStyle->tabPaddingX, pStyle->tabPaddingY, title);
	}
}

void CTabbedWindow::Update()
{
	if(m_pActiveTab != nullptr)
		m_pActiveTab->Update();
}

void CTabbedWindow::AttachTab(IToolboxComponent *pWindow, bool activate)
{
	m_windowTabs.push_back(pWindow);

	if(activate || m_windowTabs.size() == 1)
		ActivateTab(m_windowTabs.size() - 1);
	else
	{
		ShowWindow((HWND)pWindow->GetHwnd(), SW_HIDE);
	}
}

bool CTabbedWindow::DetachTab(IToolboxComponent *pComponent)
{
	if(m_windowTabs.size() == 1 && !IsDocked())
		return false;

	int i = 0;
	for(auto it = m_windowTabs.begin(); it != m_windowTabs.end(); ++it)
	{
		IToolboxComponent *pTabComponent = *it;

		if(pTabComponent == pComponent)
		{
			if(pComponent == m_pActiveTab)
			{
				m_pActiveTab = nullptr;

				int iNextTab = (i > 0) ? (i - 1) : (i + 1);
				if(m_windowTabs.size() > 1)
					ActivateTab(iNextTab);
			}

			RECT componentWndRect;
			GetWindowRect((HWND)pComponent->GetHwnd(), &componentWndRect);

			// Create new tab window to hold the "detached" tab.
			CTabbedWindow *pNewTabbedWindow = static_cast<CTabbedWindow *>(g_pToolbox->GetWindowManager()->SpawnWindow("TabbedWindow", "Tab", componentWndRect.right - componentWndRect.left, componentWndRect.bottom - componentWndRect.top, componentWndRect.left, componentWndRect.top));

			pNewTabbedWindow->AttachTab(pComponent);

			m_windowTabs.erase(it);

			// Resize this
			OnResize(0, 0);

			return true;	
		}

		i++;
	}

	return false;
}

bool CTabbedWindow::ActivateTab(int index)
{
	// Properly deactivate the previously active tab.
	if(m_pActiveTab != nullptr)
		ShowWindow((HWND)m_pActiveTab->GetHwnd(), SW_HIDE);

	IToolboxComponent *pTabComponent = m_windowTabs.at(index);

	m_pActiveTab = pTabComponent;

	RECT tabContentArea;
	if(CalculateTabWorkingArea(&tabContentArea))
	{
		HWND componentHwnd = (HWND)pTabComponent->GetHwnd();

		// Set tab container title to that of the active tab
		char componentTitle[128];
		GetWindowText(componentHwnd, componentTitle, 128);
		SetWindowText(m_hWnd, componentTitle);

		// Assign to parent
		SetParent(componentHwnd, m_hWnd);

		// Configure window style
		SetWindowLong(componentHwnd, GWL_STYLE, WS_CHILD);

		pTabComponent->Resize(tabContentArea);

		ShowWindow(componentHwnd, SW_SHOWNORMAL);
		SetFocus(componentHwnd);
		SetForegroundWindow(componentHwnd);

		pTabComponent->OnTabActivated(this);

		// Force component redraw
		OnResize(0, 0);
	}

	return true;
}

bool CTabbedWindow::CalculateTabWorkingArea(LPRECT workingRect)
{
	RECT windowRect;
	if(!GetWindowRect(m_hWnd, &windowRect))
		return false;

	SToolboxStyle *pStyle = g_pToolbox->GetWindowStyle();

	// Tab area
	workingRect->top = pStyle->tabHeight;
	// Reserve some space for the infobar at the bottom of the window.
	workingRect->bottom = (windowRect.bottom - windowRect.top);

	// Offset by one as we have to be able to draw the border
	workingRect->left = 0;
	workingRect->right = (windowRect.right - windowRect.left);

	return true;
}

void CTabbedWindow::ResizeActiveTab()
{
	if(m_pActiveTab != nullptr)
	{
		RECT workingArea;
		CalculateTabWorkingArea(&workingArea);

		m_pActiveTab->Resize(workingArea);
	}
}

STextDrawContext CTabbedWindow::GetTabSize(const char *title, int &xOrigin, int &xEnd, SToolboxStyle *pStyle)
{
	// Start at the end
	xOrigin = xEnd;

	STextDrawContext textConfig;
	textConfig.SetSizeIn800x600(false);
	textConfig.SetSize(Vec2(pStyle->tabFontSize, pStyle->tabFontSize));

	Vec2 size = pStyle->pFont->GetTextSize(title, false, textConfig);
	xEnd = xOrigin + (int)size.x + pStyle->tabWidthOffset;

	return textConfig;
}

void CTabbedWindow::OnMove(int x, int y)
{
	CDockWindow::OnMove(x, y);

	ResizeActiveTab();
}

void CTabbedWindow::OnResize(int width, int height)
{
	CDockWindow::OnResize(width, height);

	ResizeActiveTab();
}

void CTabbedWindow::OnMouseMove(int x, int y)
{
	bool bHandled = false;

	if(m_modifierState == eModifierState_MoveTab)
	{
		POINT cursorPoint;
		if(GetCursorPos(&cursorPoint))
		{
			int windowX = cursorPoint.x - m_lastCursorPosition.x;
			int windowY = cursorPoint.y - m_lastCursorPosition.y;

			// Can't detach, move the tab container instead.
			if(m_windowTabs.size() == 1 && !IsDocked())
			{
				//SetModifierState(eModifierState_MoveWindow);
			}
			else if(abs(windowY) > 2) // detach tab
			{
				CryLogAlways("Detached tab, windowY was %i", windowY);
				DetachTab(m_pActiveTab);

				SetWindowPos((HWND)m_pActiveTab->GetHwnd(), nullptr, windowX, windowY, 0, 0, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOSIZE);

				SetModifierState(eModifierState_None);
			}
			else // Move tab along x-axis until it is outside the tab window, at which point it is detached
			{

			}

			bHandled = true;
		}
	}

	if(!bHandled)
		CDockWindow::OnMouseMove(x, y);
}

void CTabbedWindow::OnLeftMouseButtonDown(int x, int y)
{
	CDockWindow::OnLeftMouseButtonDown(x, y);

	POINT cursorPoint;

	if(GetCursorPos(&cursorPoint))
	{
		Vec2 origin(ZERO);

		SToolboxStyle *pStyle = g_pToolbox->GetWindowStyle();

		bool clickedTab = false;

		int xOrigin = 0;
		int xEnd = 0;

		// Check which tab was clicked
		for(int i = 0; i < m_windowTabs.size(); i++)
		{
			IToolboxComponent *pTabComponent = m_windowTabs.at(i);

			char title[256]; 
			GetWindowText((HWND)pTabComponent->GetHwnd(), title, 256);

 			GetTabSize(title, xOrigin, xEnd, pStyle);

			if(x >= xOrigin && x <= xEnd)
			{
				if(pTabComponent != m_pActiveTab)
					ActivateTab(i);

				SetModifierState(eModifierState_MoveTab);

				clickedTab = true;

				break;
			}
		}

		// Clicked on the tab window itself
		if(!clickedTab && !IsDocked())
		{
			//SetModifierState(eModifierState_MoveWindow);
		}
	}
}