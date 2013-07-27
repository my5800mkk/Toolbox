#include "StdAfx.h"
#include "EditModeToolbar.h"

#include <IButton.h>

#define ICON_SIZE 32

void CEditModeToolbar::PreloadAssets()
{
	m_pSelectButton = CreateButton("Toolbox/icon.tif");

	m_pMoveButton = CreateButton("Toolbox/icon.tif");
	m_pRotateButton = CreateButton("Toolbox/icon.tif");
	m_pScaleButton = CreateButton("Toolbox/icon.tif");
}

void CEditModeToolbar::OnRender(int width, int height)
{
	SToolboxStyle *pStyle = g_pToolbox->GetWindowStyle();

	Vec2 pos = Vec2((width - ICON_SIZE) / 2.f, GetTopBarHeight(pStyle) + 10.f);

	m_pSelectButton->SetPosition(pos);

	pos.y += m_pSelectButton->GetSize().y + 10;

	m_pMoveButton->SetPosition(pos);

	pos.y += m_pSelectButton->GetSize().y + 10;

	m_pRotateButton->SetPosition(pos);

	pos.y += m_pSelectButton->GetSize().y + 10;

	m_pScaleButton->SetPosition(pos);

	CToolboxWindowBase::OnRender(width, height);
}