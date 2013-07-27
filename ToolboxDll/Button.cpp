#include "StdAfx.h"
#include "Button.h"

CButton::CButton(IToolboxWindow *pParent, const char *texturePath) 
	: m_pOwner(pParent)
	, m_onPress(nullptr)
	, m_onRelease(nullptr)
	, m_onDoublePress(nullptr)
{
	m_pTexture = gEnv->pRenderer->EF_LoadTexture(texturePath, FT_NOMIPS | FT_FILESINGLE | FT_FILTER_LINEAR);
	CRY_ASSERT(m_pTexture);

	m_position = Vec2(0, 0);
	m_size = Vec2((float)m_pTexture->GetWidth(), (float)m_pTexture->GetHeight());

	m_pOwner->RegisterListener(this);
}

CButton::~CButton()
{
	m_pTexture->Release();

	m_pOwner->UnregisterListener(this);	
}

void CButton::OnRender(IToolboxWindow *pWindow, int width, int height)
{
	float s[4],t[4];

	s[0]=0;	t[0]=0;
	s[1]=1;	t[1]=0;
	s[2]=1;	t[2]=1;
	s[3]=0;	t[3]=1;

	gEnv->pRenderer->DrawImageWithUV(m_position.x, m_position.y, 0, m_size.x, m_size.y, m_pTexture->GetTextureID(), s, t);
}

void CButton::OnLeftMouseButtonDown(IToolboxWindow *pWindow, int x, int y)
{
	if(CheckHit(x, y))
	{
		for each(auto listener in m_listeners)
			listener->OnPress();

		if(m_onPress != nullptr)
			m_onPress();
	}
}

void CButton::OnLeftMouseButtonUp(IToolboxWindow *pWindow, int x, int y)
{
	if(CheckHit(x, y))
	{
		for each(auto listener in m_listeners)
			listener->OnRelease();

		if(m_onRelease != nullptr)
			m_onRelease();
	}
}

void CButton::OnLeftMouseButtonDoubleClick(int x, int y)
{
	if(CheckHit(x, y))
	{
		for each(auto listener in m_listeners)
			listener->OnDoublePress();

		if(m_onDoublePress != nullptr)
			m_onDoublePress();
	}
}

bool CButton::CheckHit(int x, int y)
{
	return x >= m_position.x && x <= m_position.x + m_size.x
		&& y >= m_position.y && y <= m_position.y + m_size.y;
}