#include "StdAfx.h"
#include "Viewport.h"

#include "DockWindow.h"

CViewport::CViewport(HWND hWnd)
	: m_hWnd(hWnd)
	, m_pOwner(nullptr)
{
}

CViewport::~CViewport()
{
}

void CViewport::Redraw()
{
	Render();
}

void CViewport::Update()
{
	Render();
}

void CViewport::Render()
{
	gEnv->pRenderer->SetCurrentContext(m_hWnd);

	RECT windowRect;
	GetWindowRect(m_hWnd, &windowRect);

	int width = windowRect.right - windowRect.left;
	int height = windowRect.bottom - windowRect.top;

	gEnv->pRenderer->ChangeViewport(0, 0, width, height, true);

	gEnv->pRenderer->SetCamera(m_camera);

	SToolboxStyle *pWindowStyle = g_pToolbox->GetWindowStyle();

	gEnv->pRenderer->SetClearColor(Vec3(pWindowStyle->viewportClearColor.r, pWindowStyle->viewportClearColor.g, pWindowStyle->viewportClearColor.b));
	gEnv->pRenderer->ClearBuffer(FRT_CLEAR, &pWindowStyle->viewportClearColor);

	gEnv->pSystem->RenderBegin();
	
	gEnv->pSystem->SetViewCamera(m_camera);

	gEnv->pSystem->RenderEnd();

	// Restore context
	gEnv->pRenderer->MakeMainContextActive();
}

void CViewport::Resize(RECT area)
{
	int width = area.right - area.left;
	int height = area.bottom - area.top;

	SetWindowPos(m_hWnd, nullptr, area.left, area.top, width, height, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	gEnv->pRenderer->MakeMainContextActive();

	gEnv->pRenderer->ChangeViewport(0, 0, width, height, true);

	// Set up camera
	m_camera.SetFrustum(width, height, DEG2RAD(60));
	m_camera.SetMatrix(Matrix34::Create(Vec3(1, 1, 1), Quat(IDENTITY), Vec3(ZERO)));
}

RECT CViewport::GetRect()
{
	RECT rect;
	if(GetWindowRect(m_hWnd, &rect))
		return rect;

	memset(&rect, 0, sizeof(RECT));
	return rect;
}