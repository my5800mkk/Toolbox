#include "StdAfx.h"
#include "ToolboxWindowBase.h"

#include <IRenderer.h>
#include <IRenderAuxGeom.h>

CToolboxWindowBase::CToolboxWindowBase()
	: m_bMoving(false)
	, m_resizeState(0)
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

	if(gEnv->pRenderer->CreateContext(m_hWnd))
		gEnv->pRenderer->MakeMainContextActive();

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
			if(resizeState & EResizeState_Top && windowRect.top <= monitorInfo.rcWork.top)
				return false;
			if(resizeState & EResizeState_Bottom && windowRect.bottom >= monitorInfo.rcWork.bottom)
				return false;
			if(resizeState & EResizeState_Left && windowRect.left <= monitorInfo.rcWork.left)
				return false;
			if(resizeState & EResizeState_Right && windowRect.right >= monitorInfo.rcWork.right)
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

bool CToolboxWindowBase::GetWorkingArea(LPRECT workingRect)
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

	gEnv->pRenderer->SetCurrentContext(m_hWnd);
	
	RECT clientRect;
	if(!GetClientRect(m_hWnd, &clientRect))
	{
		CryLogAlways("[Warning] Failed to get client rect for window in CToolboxWindowBase::Redraw");
		return;
	}

	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;

	CryLogAlways("Changing client res to %ix%i", width, height);
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

	// Restore context
	gEnv->pRenderer->MakeMainContextActive();
}

uint32 CToolboxWindowBase::ShowResizeCursor(int x, int y)
{
	uint32 resizeState = 0;

	RECT windowRect;
	if(!GetWindowRect(m_hWnd, &windowRect))
		return resizeState;

	SToolboxStyle *pStyle = g_pToolbox->GetWindowStyle();

	if(x <= pStyle->delimiterSize)
		resizeState |= EResizeState_Left;
	if(y <= pStyle->delimiterSize)
		resizeState |= EResizeState_Top;

	if((windowRect.right - windowRect.left) - x <= pStyle->delimiterSize)
		resizeState |= EResizeState_Right;
	if((windowRect.bottom - windowRect.top) - y <= pStyle->delimiterSize)
		resizeState |= EResizeState_Bottom;

	if(!CanResize(resizeState))
		return 0;

	if(resizeState != 0)
	{
		if(((resizeState & EResizeState_Left) && (resizeState & EResizeState_Top))
			|| ((resizeState & EResizeState_Right) && (resizeState & EResizeState_Bottom)))
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
		}
		else if(((resizeState & EResizeState_Left) && (resizeState & EResizeState_Bottom))
			|| ((resizeState & EResizeState_Right) && (resizeState & EResizeState_Top)))
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENESW));
		}
		else if((resizeState & EResizeState_Left) || (resizeState & EResizeState_Right))
		{
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		}
		else if((resizeState & EResizeState_Top) || (resizeState & EResizeState_Bottom))
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

//////////////////////////////////////////////////
// WndProc callbacks
//////////////////////////////////////////////////
void CToolboxWindowBase::OnRender(int width, int height)
{
	DrawWindowBorder((float)width, (float)height);
}

void CToolboxWindowBase::OnMouseMove(int x, int y)
{
	if(m_bMoving)
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
	else if(m_resizeState != 0)
	{
		POINT cursorPoint;
		RECT windowRect;

		if(GetCursorPos(&cursorPoint) && GetWindowRect(m_hWnd, &windowRect))
		{
			if(m_resizeState & EResizeState_Left)
			{
				windowRect.left = cursorPoint.x;
			}
			else if(m_resizeState & EResizeState_Right)
			{
				windowRect.right = cursorPoint.x;
			}
			
			if(m_resizeState & EResizeState_Top)
			{
				windowRect.top = cursorPoint.y;
			}
			else if(m_resizeState & EResizeState_Bottom)
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
	m_bMoving = false;
}

void CToolboxWindowBase::OnLeftMouseButtonDown(int x, int y)
{
	m_lastCursorPosition.x = x;
	m_lastCursorPosition.y = y;
	
	SToolboxStyle *pStyle = g_pToolbox->GetWindowStyle();

	if(y <= GetTopBarHeight(pStyle) && CanMove())
	{
		SetCapture(m_hWnd);

		m_bMoving = true;
	}
	else
	{
		uint32 newResizeState = ShowResizeCursor(x, y);

		if(newResizeState != 0)
		{
			SetCapture(m_hWnd);

			m_resizeState = newResizeState;
		}
	}
}

void CToolboxWindowBase::OnLeftMouseButtonUp(int x, int y)
{
	RECT windowRect;
	GetWindowRect(m_hWnd, &windowRect);

	if(m_bMoving)
	{
		if(windowRect.top <= 0)
			Maximize();
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
	m_bMoving = hWnd == m_hWnd;

	if(!m_bMoving)
	{
		m_resizeState = 0;

		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
}