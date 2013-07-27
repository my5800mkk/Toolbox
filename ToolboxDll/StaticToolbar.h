#pragma once

#include "ToolboxWindowBase.h"

class CStaticToolbar
	: public CToolboxWindowBase
{
	// CToolboxWindowBase
	virtual bool CanResize(uint32 resizeState) override { return false; }
	virtual bool CanMove() override { return false; }
	// ~CToolboxWindowBase
};