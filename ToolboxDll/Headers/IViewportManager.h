#pragma once

#include <IToolboxComponent.h>

struct IToolboxViewport
	: public IToolboxComponent
{

};

struct IToolboxViewportManager
{
	virtual IToolboxViewport *CreateViewport(WIN_HWND hWnd) = 0;

	virtual void Update() = 0;
};