#pragma once

#include "ToolboxWindowBase.h"

class CEditModeToolbar
	: public CToolboxWindowBase
{
public:
	// CToolboxWindowBase
	virtual int GetMinWidth() override { return 50; }
	virtual int GetMinHeight() override { return 50; }

	virtual void OnRender(int width, int height) override;

	virtual void PreloadAssets() override;
	// ~CToolboxWindowBase

protected:
	IButton *m_pSelectButton;
	IButton *m_pMoveButton;
	IButton *m_pRotateButton;
	IButton *m_pScaleButton;
};

  