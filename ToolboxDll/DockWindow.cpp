#include "StdAfx.h"
#include "DockWindow.h"

#include "Viewport.h"

#include "Button.h"

#include "RenderUtils.h"

CDockWindow::CDockWindow()
{
	m_pWorkspaceWindowData = new SDockedWindowData(this, EDockState_Last, EDockFlag_Static | EDockFlag_ResizeNeighborsOnly);
}

CDockWindow::~CDockWindow()
{
	SAFE_DELETE(m_pWorkspaceWindowData);
}

bool CDockWindow::GetWorkingArea(LPRECT workingRect)
{
	RECT windowRect;
	if(!GetWindowRect(m_hWnd, &windowRect))
		return false;

	SToolboxStyle *pStyle = g_pToolbox->GetWindowStyle();
	
	workingRect->top = GetTopBarHeight(pStyle);
	workingRect->bottom = (windowRect.bottom - windowRect.top) - pStyle->infoBarHeight;

	workingRect->left = 0;
	workingRect->right = (windowRect.right - windowRect.left);

	if(!IsMaximized())
	{
		workingRect->bottom -= 1;
		workingRect->left += 1;
		workingRect->right -= 1;
	}

	return true;
}

bool CDockWindow::DockWindow(IToolboxWindow *pComponent, unsigned int dockFlags, EDockState dockState, IToolboxWindow *pNeighbor)
{
	if(!pComponent->IsDockable())
		return false;
	
	if(pNeighbor != nullptr && pNeighbor->GetDockOwner() != this)
		return false;

	if(SDockedWindowData *pNeighborData = FindDockedWindowData(pNeighbor, m_pWorkspaceWindowData))
	{
		// Make sure the dock slot is free
		if(pNeighborData->pNeighbors[dockState] != nullptr)
			return false;

		SDockedWindowData *pDockedWindowData = new SDockedWindowData(pComponent, dockState, dockFlags);
		pNeighborData->pNeighbors[dockState] = pDockedWindowData;

		SetParentOf(pComponent);

		HWND componentHwnd = (HWND)pComponent->GetHwnd();

		ShowWindow(componentHwnd, SW_SHOWNORMAL);
		SetFocus(componentHwnd);
		SetForegroundWindow(componentHwnd);

		pComponent->OnDocked(this, dockState);

		// Force redraw
		OnResize(0, 0);

		return true;
	}

	return false;
}

SDockedWindowData *CDockWindow::FindDockedWindowData(IToolboxWindow *pWindow, SDockedWindowData *pDockedWindowData)
{
	if(pDockedWindowData->pWindow == pWindow || pWindow == nullptr)
		return pDockedWindowData;

	for(int i = 0; i < EDockState_Last; i++)
	{
		SDockedWindowData *pData = pDockedWindowData->pNeighbors[i];
		if(pData != nullptr)
		{
			if(pData->pWindow == pWindow)
				return pData;

			pData = FindDockedWindowData(pWindow, pData);
			if(pData != nullptr)
				return pData;
		}
	}

	return nullptr;
}

void CDockWindow::Redraw()
{
	CToolboxWindowBase::Redraw();

	m_pWorkspaceWindowData->Redraw();
}

void CDockWindow::Update()
{
	m_pWorkspaceWindowData->Update();
}

void CDockWindow::OnResize(int width, int height)
{
	// Resize docked windows
	RECT workingArea;
	GetWorkingArea(&workingArea);

	// Resize the window and its neighbors. (Working area is shared)
	m_pWorkspaceWindowData->ResizeNeighbors(&workingArea);
}

void SDockedWindowData::ResizeNeighbors(LPRECT area)
{
	RECT originalArea = *area;

	SDockedWindowData *pLastNeighbor = nullptr;
	EDockState lastDockState = EDockState_Last;

	for(int i = 0; i < EDockState_Last; i++)
	{
		SDockedWindowData *pNeighborData = pNeighbors[i];

		if(pNeighborData != nullptr)
		{
			RECT workingArea = *area;

			RECT currentArea = pNeighborData->pWindow->GetRect();

			switch(i)
			{
			case EDockState_LeftOf:
				{
					area->left += pNeighborData->size.x;
					workingArea.right = area->left;
				}
				break;
			case EDockState_RightOf:
				{
					area->right -=  pNeighborData->size.x;
					workingArea.left = area->right;
				}
				break;
			case EDockState_TopOf:
				{
					area->top += pNeighborData->size.y;
					workingArea.bottom = area->top;
				}
				break;
			case EDockState_BottomOf:
				{
					area->bottom -= pNeighborData->size.y;
					workingArea.top = area->bottom;
				}
				break;
			}

			pNeighborData->ResizeNeighbors(&workingArea);

			pLastNeighbor = pNeighborData;
			lastDockState = (EDockState)i;
		}
	}

	if((flags & EDockFlag_ResizeNeighborsOnly))
	{
		if(pLastNeighbor == nullptr)
			return;

		switch(lastDockState)
		{
		case EDockState_LeftOf:
			{
				area->left -= pLastNeighbor->size.x;
			}
			break;
		case EDockState_RightOf:
			{
				area->right +=  pLastNeighbor->size.x;
			}
			break;
		case EDockState_TopOf:
			{
				area->top -= pLastNeighbor->size.y;
			}
			break;
		case EDockState_BottomOf:
			{
				area->bottom -= pLastNeighbor->size.y;
			}
			break;
		}

		pLastNeighbor->ResizeNeighbors(area);
	}
	else
		pWindow->Resize(*area);
}