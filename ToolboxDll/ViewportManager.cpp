#include "StdAfx.h"
#include "ViewportManager.h"

#include "Viewport.h"

CViewportManager::CViewportManager()
{
}

CViewportManager::~CViewportManager()
{
}

void CViewportManager::Update()
{
	for each(auto viewport in m_viewports)
		viewport->Update();
}

IToolboxViewport *CViewportManager::CreateViewport(WIN_HWND hWnd)
{
	IToolboxViewport *pViewport = new CViewport((HWND)hWnd);
	m_viewports.push_back(pViewport);

	return pViewport;
}