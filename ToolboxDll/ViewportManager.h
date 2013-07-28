#pragma once

#include <IViewportManager.h>

#include "Viewport.h"

class CViewportManager
	: public IToolboxViewportManager
{
public:
	CViewportManager();
	~CViewportManager();

	// IToolboxViewportManager
	virtual IToolboxViewport *CreateViewport(WIN_HWND hWnd) override;

	virtual void Update() override;
	// ~IToolboxViewportManager

protected:
	std::vector<IToolboxViewport *> m_viewports;
};

