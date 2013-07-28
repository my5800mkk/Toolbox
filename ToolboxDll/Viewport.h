/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2013.
//////////////////////////////////////////////////////////////////////////
// hWnd wrapper to handle drawing viewports.
//////////////////////////////////////////////////////////////////////////
// 01/06/2013 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#pragma once

#include "Toolbox.h"

#include  "DockWindow.h"

#include <IViewportManager.h>

class CViewport
	: public IToolboxViewport
{
public:
	CViewport(HWND hWnd);
	~CViewport();

	// IToolboxComponent
	virtual void OnTabActivated(IToolboxWindow *pTabContainer) override {}

	virtual RECT GetRect() override;

	virtual void Redraw() override;

	virtual void Update() override;

	virtual WIN_HWND GetHwnd() override { return m_hWnd; }

	virtual void Resize(RECT area) override;
	// ~IToolboxComponent

	void Render();

protected:
	HWND m_hWnd;

	IToolboxWindow *m_pOwner;

	CCamera m_camera;
};

