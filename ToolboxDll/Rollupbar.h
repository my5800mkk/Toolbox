#pragma once

#include "ToolboxWindowBase.h"

class CRollupbar
	: public CToolboxWindowBase
{
public:
	// CToolboxWindowBase
	virtual int GetMinWidth() override { return 250; }
	virtual int GetMinHeight() override { return 250; }
	// ~CToolboxWindowBase
};

