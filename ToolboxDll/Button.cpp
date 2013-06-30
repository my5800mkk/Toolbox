#include "StdAfx.h"
#include "Button.h"

CButton::CButton(const char *texturePath, Vec2 position)
	: m_position(position)
{
	m_pTexture = gEnv->pRenderer->EF_LoadTexture(texturePath, FT_NOMIPS | FT_FILESINGLE | FT_FILTER_LINEAR);
	CRY_ASSERT(m_pTexture);
}

CButton::~CButton()
{
	m_pTexture->Release();
}

void CButton::Draw()
{
	float s[4],t[4];

	s[0]=0;	t[0]=0;
	s[1]=1;	t[1]=0;
	s[2]=1;	t[2]=1;
	s[3]=0;	t[3]=1;

	gEnv->pRenderer->DrawImageWithUV(m_position.x, m_position.y, 0, (float)m_pTexture->GetWidth(), (float)m_pTexture->GetHeight(), m_pTexture->GetTextureID(), s, t);
}