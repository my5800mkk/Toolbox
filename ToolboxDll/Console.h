#pragma once

#include "ToolboxWindowBase.h"

class CConsoleWindow
	: public CToolboxWindowBase
{
public:
	// CToolboxWindowBase
	virtual void OnRender(int width, int height) override;
	// ~CToolboxWindowBase
};

