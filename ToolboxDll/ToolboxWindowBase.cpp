#include "StdAfx.h"
#include "ToolboxWindowBase.h"

#include <IRenderer.h>
#include <IRenderAuxGeom.h>

CToolboxWindowBase::CToolboxWindowBase()
	: m_modifierState(eModifierState_None)
{
}

CToolboxWindowBase::~CToolboxWindowBase()
{
	DestroyWindow(m_hWnd);
}

void CToolboxWindowBase::Initialize(WIN_HWND hWnd)
{
	m_hWnd = (HWND)hWnd;

	if(!IsWindow(m_hWnd))
	{
		CryLogAlways("[Warning] Received invalid HWND in CToolboxWindowBase::Initialize, destroying");
		delete this;
	}

	PreloadAssets();

	// Create render context for this window handle.
	// Contexts only work from within the editor, and allow for rendering onto different windows.
	// Each context stores resolution independently.
	if(gEnv->pRenderer->CreateContext(m_hWnd))
	{
		// New contexts are automatically activated, re-enable the primary one.
		gEnv->pRenderer->MakeMainContextActive();
	}

	RECT workAreaRect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0);

	int width = workAreaRect.right - workAreaRect.left;
	int height = workAreaRect.bottom - workAreaRect.top;

	// Redraw
	SetWindowPos(m_hWnd, NULL, 0, 0, width, height, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

	ShowWindow(m_hWnd, SW_SHOWNORMAL);
    SetFocus(m_hWnd);
    SetForegroundWindow(m_hWnd);
}

void CToolboxWindowBase::PostInitialize()
{
	Redraw();
}

bool CToolboxWindowBase::CanResize(uint32 resizeState)
{
	// Can't resize if this side of the window is in direct contact with the edge of the work area.
	// Note that this is default Windows functionality.
	RECT windowRect;
	GetWindowRect(m_hWnd, &windowRect);

	HMONITOR monitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	if(monitor != nullptr)
	{
		MONITORINFO monitorInfo;
		monitorInfo.cbSize = sizeof(MONITORINFO);

		if(GetMonitorInfo(monitor, &monitorInfo))
		{
			if(resizeState & eModifierState_ResizeTop && windowRect.top <= monitorInfo.rcWork.top)
				return false;
			if(resizeState & eModifierState_ResizeBottom && windowRect.bottom >= monitorInfo.rcWork.bottom)
				return false;
			if(resizeState & eModifierState_ResizeLeft && windowRect.left <= monitorInfo.rcWork.left)
				return false;
			if(resizeState & eModifierState_ResizeRight && windowRect.right >= monitorInfo.rcWork.right)
				return false;
		}
	}

	return true;
}

void CToolboxWindowBase::Maximize()
{
	ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
}

void CToolboxWindowBase::Minimize()
{
	ShowWindow(m_hWnd, SW_SHOWMINIMIZED);
}

bool CToolboxWindowBase::IsMaximized()
{
	WINDOWPLACEMENT windowPlacement;
	if(GetWindowPlacement(m_hWnd, &windowPlacement))
		return windowPlacement.showCmd == SW_SHOWMAXIMIZED;

	return false;
}

bool CToolboxWindowBase::IsMinimized()
{
	WINDOWPLACEMENT windowPlacement;
	if(GetWindowPlacement(m_hWnd, &windowPlacement))
		return windowPlacement.showCmd == SW_SHOWMINIMIZED;

	return false;
}

IToolboxWindow *CToolboxWindowBase::GetWindowParent()
{
	return g_pToolbox->GetWindowManager()->FindToolboxWindow((WIN_HWND)GetParent(m_hWnd));
}

void CToolboxWindowBase::SetParentOf(IToolboxWindow *pChild)
{
	HWND childHwnd = (HWND)pChild->GetHwnd();

	// Assign to parent
	SetParent(childHwnd, m_hWnd);

	// Configure window style
	SetWindowLong(childHwnd, GWL_STYLE, WS_CHILD);

	m_children.push_back(pChild);
}

RECT CToolboxWindowBase::GetRect()
{
	RECT rect;
	if(GetWindowRect(m_hWnd, &rect))
		return rect;

	memset(&rect, 0, sizeof(RECT));
	return rect;
}

void CToolboxWindowBase::Resize(RECT area)
{
	SetWindowPos(m_hWnd, nullptr, area.left, area.top, area.right - area.left, area.bottom - area.top, 0);
}

void CToolboxWindowBase::Redraw()
{
	if(gEnv == nullptr)
		return;

	// Enable the render context for this window.
	gEnv->pRenderer->SetCurrentContext(m_hWnd);
	
	RECT clientRect;
	if(!GetClientRect(m_hWnd, &clientRect))
	{
		CryLogAlways("[Warning] Failed to get client rect for window in CToolboxWindowBase::Redraw");
		return;
	}

	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;

	// Update the resolution for this render context based on the client rect.
	// Note that we use the client rect and not the window rect. (Using window rect results in stretching)
	gEnv->pRenderer->ChangeViewport(0, 0, width, height);

	SToolboxStyle *pWindowStyle = g_pToolbox->GetWindowStyle();

	gEnv->pRenderer->SetClearColor(Vec3(pWindowStyle->backgroundColor.r, pWindowStyle->backgroundColor.g, pWindowStyle->backgroundColor.b));
	gEnv->pRenderer->ClearBuffer(FRT_CLEAR, &pWindowStyle->backgroundColor);

	gEnv->pRenderer->BeginFrame();

	gEnv->pRenderer->SetWireframeMode(R_SOLID_MODE);

	gEnv->pRenderer->Set2DMode(true, clientRect.right, clientRect.bottom);

	gEnv->pRenderer->EF_StartEf();

	OnRender(width, height);

	gEnv->pRenderer->GetIRenderAuxGeom()->Flush();

	OnPostRender();

	// Draw queued text
	{
		for each(SQueuedText *pQueuedText in m_queuedText)
		{
			pWindowStyle->pFont->DrawString((float)pQueuedText->x, (float)pQueuedText->y, pQueuedText->text, false, pQueuedText->drawContext);

			delete pQueuedText;
		}

		m_queuedText.clear();
	}

	gEnv->pRenderer->EF_EndEf3D(SHDF_STREAM_SYNC, -1, -1);

	gEnv->pRenderer->Set2DMode(false, clientRect.right, clientRect.bottom);

	gEnv->pRenderer->RenderDebug(false);

	gEnv->pRenderer->EndFrame();

	// Restore the main context
	gEnv->pRenderer->MakeMainContextActive();

	for(auto it = m_children.begin(); it != m_children.end(); ++it)
	{
		(*it)->Redraw();
	}
}

uint32 CToolboxWindowBase::ShowResizeCursor(int x, int y)
{
	uint32 resizeState = 0;

	RECT windowRect;
	if(!GetWindowRect(m_hWnd, &windowRect))
		return resizeState;

	SToolboxStyle *pStyle = g_pToolbox->GetWindowStyle();

	if(x <= pStyle->delimiterSize)
		resizeState |= eModifierState_ResizeLeft;
	if(y <= pStyle->delimiterSize)
		resizeState |= eModifierState_ResizeTop;

	if((windowRect.right - windowRect.left) - x <= pStyle->delimiterSize)
		resizeState |= eModifierState_ResizeRight;
	if((windowRect.bottom - windowRect.top) - y <= pStyle->delimiterSize)
		resizeState |= eModifierState_ResizeBottom;

	if(!CanResize(resizeState))
		return 0;

	if(resizeState != 0)
	{
		// Always append Resize flag to easily check if we're resizing, no matter the direction.
		resizeState |= eModifierState_Resize;

		if(((resizeState & eModifierState_ResizeLeft) && (resizeState & eModifierState_ResizeTop))
			|| ((resizeState & eModifierState_ResizeRight) && (resizeState & eModifierState_ResizeBottom)))
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
		}
		else if(((resizeState & eModifierState_ResizeLeft) && (resizeState & eModifierState_ResizeBottom))
			|| ((resizeState & eModifierState_ResizeRight) && (resizeState & eModifierState_ResizeTop)))
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENESW));
		}
		else if((resizeState & eModifierState_ResizeLeft) || (resizeState & eModifierState_ResizeRight))
		{
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		}
		else if((resizeState & eModifierState_ResizeTop) || (resizeState & eModifierState_ResizeBottom))
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENS));
		}
	}

	return resizeState;
}

void CToolboxWindowBase::DrawWindowBorder(float width, float height)
{
	IRenderAuxGeom *pRenderAuxGeom = gEnv->pRenderer->GetIRenderAuxGeom();

	width -= 1;
	height -= 1;

	ColorB borderColor = g_pToolbox->GetWindowStyle()->toolWindowBorderColor;

	// top
	gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(Vec3(0, 0, 0), borderColor, Vec3(width, 0, 0), borderColor);
	// left
	gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(Vec3(0, 0, 0), borderColor, Vec3(0, height, 0), borderColor);
	// bottom
	gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(Vec3(0, height, 0), borderColor, Vec3(width, height, 0), borderColor);
	// right
	gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(Vec3(width, 0, 0), borderColor, Vec3(width, height, 0), borderColor);
}

void CToolboxWindowBase::SetModifierState(unsigned int moveState)
{
	if(moveState != eModifierState_None)
	{
		SetCapture(m_hWnd);
		GetCursorPos(&m_lastCursorPosition);

		m_modifierState = moveState;
	}
	else
		m_modifierState = eModifierState_None;
}

//////////////////////////////////////////////////
// WndProc callbacks
//////////////////////////////////////////////////
void CToolboxWindowBase::OnRender(int width, int height)
{
	DrawWindowBorder((float)width, (float)height);
}

void CToolboxWindowBase::OnMouseMove(int x, int y)
{
	if(m_modifierState == eModifierState_MoveWindow)
	{
		// TODO: Shouldn't have to do this
		if(IsMaximized())
			ShowWindow(m_hWnd, SW_SHOWNORMAL);

		POINT cursorPoint;
		if(GetCursorPos(&cursorPoint))
		{
			// Move to the cursor position
			int windowX = cursorPoint.x - m_lastCursorPosition.x;
			int windowY = cursorPoint.y - m_lastCursorPosition.y;

			SetWindowPos(m_hWnd, nullptr, windowX, windowY, 0, 0, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOSIZE);
		}
	}
	else if(m_modifierState & eModifierState_Resize)
	{
		POINT cursorPoint;
		RECT windowRect;

		if(GetCursorPos(&cursorPoint) && GetWindowRect(m_hWnd, &windowRect))
		{
			if(m_modifierState & eModifierState_ResizeLeft)
			{
				windowRect.left = cursorPoint.x;
			}
			else if(m_modifierState & eModifierState_ResizeRight)
			{
				windowRect.right = cursorPoint.x;
			}
			
			if(m_modifierState & eModifierState_ResizeTop)
			{
				windowRect.top = cursorPoint.y;
			}
			else if(m_modifierState & eModifierState_ResizeBottom)
			{
				windowRect.bottom = cursorPoint.y;
			}

			int windowWidth = windowRect.right - windowRect.left;
			int windowHeight = windowRect.bottom - windowRect.top;

			if(windowWidth > 0 && windowHeight > 0)
			{
				//if(pDockOwner != nullptr)
					//pDockOwner->ResizeDockedComponent(this, windowWidth, windowHeight);
				//else
					SetWindowPos(m_hWnd, nullptr, windowRect.left, windowRect.top, windowWidth, windowHeight, 0);
			}
		}
	}
}

void CToolboxWindowBase::OnMouseLeave()
{
	SetModifierState(eModifierState_None);
}

void CToolboxWindowBase::OnLeftMouseButtonDown(int x, int y)
{
	SToolboxStyle *pStyle = g_pToolbox->GetWindowStyle();

	uint32 newResizeState = ShowResizeCursor(x, y);

	if(newResizeState != 0)
	{
		SetModifierState(newResizeState);
	}
	else if(y <= GetTopBarHeight(pStyle) && CanMove())
	{
		SetModifierState(eModifierState_MoveWindow);
	}
}

void CToolboxWindowBase::OnLeftMouseButtonUp(int x, int y)
{
	RECT windowRect;
	GetWindowRect(m_hWnd, &windowRect);

	if(m_modifierState == eModifierState_MoveWindow)
	{
		// Simulate aero snap on the top of the screen to maximize
		if(windowRect.top <= 0)
			Maximize();
		else // Simulate aero snap on the left-most or right-most side of the screen
		{
			POINT cursorPoint;
			GetCursorPos(&cursorPoint);

			HMONITOR monitor = MonitorFromPoint(cursorPoint, MONITOR_DEFAULTTONULL);
			if(monitor == nullptr)
				return;
		
			MONITORINFO monitorInfo;
			monitorInfo.cbSize = sizeof(MONITORINFO);

			if(!GetMonitorInfo(monitor, &monitorInfo))
				return;

			if(cursorPoint.x <= monitorInfo.rcWork.left || cursorPoint.x >= (monitorInfo.rcWork.right - 1))
			{
				int width = (int)((monitorInfo.rcWork.right - monitorInfo.rcWork.left) * 0.5f);
				int height = (monitorInfo.rcWork.bottom - monitorInfo.rcWork.top);

				if(cursorPoint.x <= monitorInfo.rcWork.left)
					SetWindowPos(m_hWnd, nullptr, monitorInfo.rcWork.left, 0, width, height, 0);
				else
					SetWindowPos(m_hWnd, nullptr, monitorInfo.rcWork.right - width, 0, width, height, 0);
			}
		}
	}

	ReleaseCapture();

	SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void CToolboxWindowBase::OnMove(int x, int y)
{
	Redraw();
}

void CToolboxWindowBase::OnResize(int width, int height)
{
	Redraw();
}

void CToolboxWindowBase::OnMaximize(int width, int height)
{
	HMONITOR monitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	if(monitor == nullptr)
		return;

	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFO);

	if(GetMonitorInfo(monitor, &monitorInfo))
	{
		int newWidth = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
		int newHeight = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

		SetWindowPos(m_hWnd, nullptr, monitorInfo.rcWork.left, monitorInfo.rcWork.top, newWidth, newHeight, 0);

		OnResize(newWidth, newHeight);
	}
}

void CToolboxWindowBase::OnCaptureChanged(HWND hWnd)
{
	if(m_modifierState != eModifierState_None && hWnd != m_hWnd)
		SetModifierState(eModifierState_None);

	SetCursor(LoadCursor(NULL, IDC_ARROW));
}