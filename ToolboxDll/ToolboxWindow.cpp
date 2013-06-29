#include "StdAfx.h"
#include "ToolboxWindow.h"

#include "Viewport.h"

#include "Button.h"

#include "RenderUtils.h"

CToolboxWindow::CToolboxWindow()
	: m_pDockOwner(nullptr)
{
	memset(m_dockedWindows, 0, sizeof(SDockedWindow *) * EDockState_Last);
}

CToolboxWindow::~CToolboxWindow()
{
	UnDockAllWindows();
}

bool CToolboxWindow::CanResize(uint32 resizeState)
{
	// Standalone windows have different resizing rules
	if(IsDocked())
	{	
		if(m_dockState & EDockState_Left && resizeState != EResizeState_Right)
			return false;
		if(m_dockState & EDockState_Right && resizeState != EResizeState_Left)
			return false;
		if(m_dockState & EDockState_Bottom && resizeState != EResizeState_Top)
			return false;
		if(m_dockState & EDockState_Top && resizeState != EResizeState_Bottom)
			return false;

		return true;
	}

	return CToolboxWindowBase::CanResize(resizeState);
}

uint32 CToolboxWindow::GetAvailableDockStates()
{
	uint32 states = 0;

	if(m_dockedWindows[EDockState_Left] == nullptr)
		states |= EDockState_Left;
	if(m_dockedWindows[EDockState_Right] == nullptr)
		states |= EDockState_Right;
	if(m_dockedWindows[EDockState_Top] == nullptr)
		states |= EDockState_Top;
	if(m_dockedWindows[EDockState_Bottom] == nullptr)
		states |= EDockState_Bottom;

	return states;
}

bool CToolboxWindow::ResizeDockedComponent(IToolboxWindow *pComponent, int width, int height)
{
	for(int i = 0; i < EDockState_Last; i++)
	{
		SDockedWindow *pDockedWindow = m_dockedWindows[i];
		if(pDockedWindow == nullptr)
			continue;

		if(pDockedWindow->pWindow == pComponent)
		{
			int prevSize = pDockedWindow->size;

			switch(pDockedWindow->state)
			{
			case EDockState_Left:
			case EDockState_Right:
				{
					pDockedWindow->size = width;
				}
				break;
			case EDockState_Top:
			case EDockState_Bottom:
				{
					pDockedWindow->size = height;
				}
				break;
			}

			// Force component redraw
			OnResize(0, 0);
		}
	}

	return false;
}

void CToolboxWindow::ResizeDockedWindows()
{
	for(int i = 0; i < EDockState_Last; i++)
	{
		SDockedWindow *pDockedWindow = m_dockedWindows[i];

		if(pDockedWindow != nullptr)
		{
			memset(&pDockedWindow->workArea, 0, sizeof(RECT));
			CalculateDockedWorkingArea(&pDockedWindow->workArea, pDockedWindow->state, pDockedWindow->size);

			pDockedWindow->pWindow->Resize(pDockedWindow->workArea);
		}
	}
}

bool CToolboxWindow::CanDockWindow(IToolboxWindow *pWindow, EDockState dockState)
{
	if(!pWindow->IsDockable())
		return false;

	if(m_dockedWindows[dockState] != nullptr)
		return false;

	return true;
}

bool CToolboxWindow::GetWorkingArea(LPRECT workingRect)
{
	RECT windowRect;
	if(!GetWindowRect(m_hWnd, &windowRect))
		return false;

	SToolboxStyle *pStyle = g_pToolbox->GetWindowStyle();

	// Title area is static and can't be modified.
	workingRect->top = GetTopBarHeight(pStyle);
	// Reserve some space for the infobar at the bottom of the window.
	workingRect->bottom = (windowRect.bottom - windowRect.top) - pStyle->infoBarHeight;

	// Offset by one as we have to be able to draw the border
	workingRect->left = 1;
	workingRect->right = (windowRect.right - windowRect.left) - 1;

	return true;
}

void CToolboxWindow::CalculateDockedWorkingArea(LPRECT area, EDockState dockState, int size)
{
	RECT dockingArea;
	GetWorkingArea(&dockingArea);

	SToolboxStyle *pStyle = g_pToolbox->GetWindowStyle();

	switch(dockState)
	{
	case EDockState_Left:
		{
			area->left = dockingArea.left;
			area->right = size;
		}
		break;
	case EDockState_Right:
		{
			area->left = dockingArea.right - size;
			area->right = dockingArea.right;
		}
		break;
	case EDockState_Top:
		{
			area->top = dockingArea.top;
			area->bottom = size;
		}
		break;
	case EDockState_Bottom:
		{
			area->top = dockingArea.bottom - size;
			area->bottom = dockingArea.bottom;
		}
		break;
	case EDocKState_Center:
		{
			if(m_dockedWindows[EDockState_Left] != nullptr)
				area->left = m_dockedWindows[EDockState_Left]->workArea.right;
			else
				area->left = dockingArea.left;

			if(m_dockedWindows[EDockState_Right] != nullptr)
				area->right = m_dockedWindows[EDockState_Right]->workArea.left;
			else
				area->right = dockingArea.right;
		}
		break;
	}

	// Set height for left and right windows
	if(area->bottom == 0 && area->top == 0)
	{
		if(m_dockedWindows[EDockState_Top] != nullptr)
			area->top = m_dockedWindows[EDockState_Top]->workArea.bottom;
		else
			area->top = dockingArea.top;

		if(m_dockedWindows[EDockState_Bottom] != nullptr)
			area->bottom = m_dockedWindows[EDockState_Bottom]->workArea.top;
		else
			area->bottom = dockingArea.bottom;
	}
	else if(area->left == 0 && area->right == 0) // Set width for top and bottom windows
	{
		// Vertically docked windows are always 100% width.
		area->left = dockingArea.left;
		area->right = dockingArea.right;
	}
}

bool CToolboxWindow::DockWindow(IToolboxWindow *pComponent, EDockState dockState, int minSize)
{
	if(!pComponent->IsDockable())
		return false;

	// Another component was already docked to this point, has to be undocked first.
	if(m_dockedWindows[dockState] != nullptr)
		return false;

	SDockedWindow *pDockedWindow = new SDockedWindow(pComponent, dockState, minSize);
	m_dockedWindows[dockState] = pDockedWindow;

	CalculateDockedWorkingArea(&pDockedWindow->workArea, dockState, minSize);

	HWND componentHwnd = (HWND)pComponent->GetHwnd();

	// Assign to parent
	SetParent(componentHwnd, m_hWnd);

	// Configure window style
	SetWindowLong(componentHwnd, GWL_STYLE, WS_CHILD);

	pComponent->Resize(pDockedWindow->workArea);

	ShowWindow(componentHwnd, SW_SHOWNORMAL);
    SetFocus(componentHwnd);
    SetForegroundWindow(componentHwnd);

	pComponent->OnDocked(this, dockState);

	// Force redraw
	OnResize(0, 0);

	return true;
}

bool CToolboxWindow::UnDockWindow(IToolboxWindow *pComponent)
{
	for(int i = 0; i < EDockState_Last; i++)
	{
		SDockedWindow *pDockedWindow = m_dockedWindows[i];

		if(pDockedWindow != nullptr && pDockedWindow->pWindow == pComponent)
		{
			delete pDockedWindow;
			m_dockedWindows[i] = nullptr;
			
			HWND componentHwnd = (HWND)pComponent->GetHwnd();
			SetParent(componentHwnd, nullptr);

			// Remove WS_CHILD flag
			auto style = GetWindowLong(componentHwnd, GWL_STYLE);
			style &= ~(WS_CHILD);
			SetWindowLong(componentHwnd, GWL_STYLE, style);

			ShowWindow(componentHwnd, SW_SHOWNORMAL);
			SetFocus(componentHwnd);
			SetForegroundWindow(componentHwnd);

			// Force redraw
			OnResize(0, 0);

			return true;
		}
	}

	return false;
}

void CToolboxWindow::UnDockAllWindows()
{
	for(int i = 0; i < EDockState_Last; i++)
	{
		SDockedWindow *pDockedWindow = m_dockedWindows[i];

		if(pDockedWindow != nullptr)
		{
			delete pDockedWindow;
			m_dockedWindows[i] = nullptr;
		}
	}
}

void CToolboxWindow::Redraw()
{
	CToolboxWindowBase::Redraw();

	for(int i = 0; i < EDockState_Last; i++)
	{
		SDockedWindow *pComponentInfo = m_dockedWindows[i];
		if(pComponentInfo == nullptr)
			continue;

		pComponentInfo->pWindow->Redraw();
	}
}

void CToolboxWindow::OnRender(int width, int height)
{
	CToolboxWindowBase::OnRender(width, height);

	for each (auto button in m_buttons)
	{
		button->Draw();
	}

	// If docked, draw the docked window mover.
	if(IsDocked())
	{
		SToolboxStyle *pStyle = g_pToolbox->GetWindowStyle();

		SRenderUtils::DrawBox(Vec2(ZERO), Vec2((float)width, (float)pStyle->moverSize), pStyle->moverColor);
	}
}

void CToolboxWindow::DrawWindowBorder(float width, float height)
{
	if(IsDocked())
	{
		/*IRenderAuxGeom *pRenderAuxGeom = gEnv->pRenderer->GetIRenderAuxGeom();
		
		Vec3 start(ZERO);
		Vec3 end(ZERO);

		Vec3 increment(ZERO);

		SToolboxStyle *pStyle = g_pToolbox->GetWindowStyle();

		switch(m_dockState)
		{
		case EDockState_Bottom:
			break;
		case EDockState_Top:
			break;
		case EDockState_Left:
			{
				start = Vec3(width - pStyle->delimiterSize, 0, 0);
				end = Vec3(width - pStyle->delimiterSize, height, 0);

				increment.x = 1;
			}
			break;
		case EDockState_Right:
			{
				start = Vec3(pStyle->delimiterSize, 0, 0);
				end = Vec3(pStyle->delimiterSize, height, 0);

				increment.x = -1;
			}
			break;
		}

		ColorB delimiterColor = pStyle->delimiterColor;

		for(int i = 0; i < pStyle->delimiterSize; i++)
			gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(start + increment * i, delimiterColor, end + increment * i, delimiterColor);*/
	}
	else
		CToolboxWindowBase::DrawWindowBorder(width, height);
}

void CToolboxWindow::Update()
{
	for(int i = 0; i < EDockState_Last; i++)
	{
		SDockedWindow *pComponentInfo = m_dockedWindows[i];

		if(pComponentInfo != nullptr)
			pComponentInfo->pWindow->Update();
	}
}

CButton *CToolboxWindow::AddButton(int x, int y, const char *imagePath)
{
	ITexture *pTexture = gEnv->pRenderer->EF_LoadTexture(imagePath, FT_NOMIPS | FT_FILESINGLE | FT_FILTER_LINEAR);
	if(pTexture == nullptr)
		return nullptr;

	CButton *pButton = new CButton(pTexture, Vec2i(x, y));
	m_buttons.push_back(pButton);

	return pButton;
}

int CToolboxWindow::GetTopBarHeight(SToolboxStyle *pStyle)
{
	if(IsDocked())
		return pStyle->moverSize;

	return CToolboxWindowBase::GetTopBarHeight(pStyle);
}

//////////////////////////////////////////////////
// WndProc callbacks
//////////////////////////////////////////////////
void CToolboxWindow::OnMouseMove(int x, int y)
{
	// Set to true to skip the CToolboxWindowBase handling
	bool bHandled = false;

	// Used to check whether we're currently docked
	IDockWindow *pDockOwner = GetDockOwner();

	if(m_bMoving)
	{
		if(IsDocked())
		{
			// Undock the window
			pDockOwner->UnDockWindow(this);

			m_pDockOwner = nullptr;

			bHandled = true;
		}
	}

	if(!bHandled)
		CToolboxWindowBase::OnMouseMove(x, y);
}

void CToolboxWindow::OnMove(int x, int y)
{
	ResizeDockedWindows();

	CToolboxWindowBase::OnMove(x, y);
}

void CToolboxWindow::OnResize(int width, int height)
{
	ResizeDockedWindows();

	CToolboxWindowBase::OnResize(width, height);
}