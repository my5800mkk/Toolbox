#include "StdAfx.h"
#include "Button.h"

CButton::CButton(IToolboxWindow *pParent, const char *texturePath) 
	: m_pOwner(pParent)
	, m_bHidden(false)
	, m_lastEvent(EButtonEvent_None)
	, m_bHasCallbacks(false)
{
	ITexture *pTexture = gEnv->pRenderer->EF_LoadTexture(texturePath, FT_NOMIPS | FT_FILESINGLE | FT_FILTER_LINEAR);
	CRY_ASSERT(pTexture);
	m_pEventStates[EButtonEvent_None].pTexture = pTexture;
	
	m_pEventStates[EButtonEvent_OnMouseOver].color.a = 0.2f;
	m_pEventStates[EButtonEvent_OnClick].color.a = 0.5f;

	m_position = Vec2(0, 0);
	m_size = Vec2((float)pTexture->GetWidth(), (float)pTexture->GetHeight());

	m_pOwner->RegisterListener(this);
}

CButton::~CButton()
{
	m_pOwner->UnregisterListener(this);	
}

void CButton::OnRender(IToolboxWindow *pWindow, int width, int height)
{
	if(m_bHidden)
		return;

	float s[4],t[4];

	s[0]=0;	t[0]=0;
	s[1]=1;	t[1]=0;
	s[2]=1;	t[2]=1;
	s[3]=0;	t[3]=1;

	SButtonEventState *pState = GetCurrentEventState();
	
	ITexture *pTexture = pState->pTexture;
	if(pTexture == nullptr)
		pTexture = m_pEventStates[EButtonEvent_None].pTexture;

	CryLogAlways("Current state %i has alpha %f", m_lastEvent, pState->color.a);
	gEnv->pRenderer->DrawImageWithUV(m_position.x, m_position.y, 0, m_size.x, m_size.y, pTexture->GetTextureID(), s, t, pState->color.r, pState->color.g, pState->color.b, pState->color.a);
}

SButtonEventState *CButton::GetCurrentEventState()
{
	return &m_pEventStates[m_lastEvent];
}

void CButton::AddEventCallback(EButtonEvent event, TEventCallback callback)
{
	m_eventCallbacks[event].push_back(callback);

	m_bHasCallbacks = true;
}

void CButton::TriggerCallbacks(EButtonEvent event)
{
	for each(auto callback in m_eventCallbacks[event])
		callback(m_pOwner);

	m_lastEvent = event;

	if(m_bHasCallbacks)
		m_pOwner->Redraw();
}

void CButton::OnMouseMove(IToolboxWindow *pWindow, int x, int y)
{
	if(CheckHit(x, y))
		TriggerCallbacks(EButtonEvent_OnMouseOver);
}

void CButton::OnLeftMouseButtonDown(IToolboxWindow *pWindow, int x, int y)
{
	if(CheckHit(x, y))
		TriggerCallbacks(EButtonEvent_OnClick);
}

void CButton::OnLeftMouseButtonUp(IToolboxWindow *pWindow, int x, int y)
{
	if(CheckHit(x, y))
		TriggerCallbacks(EButtonEvent_OnRelease);
}

void CButton::OnLeftMouseButtonDoubleClick(IToolboxWindow *pWindow, int x, int y)
{
	if(CheckHit(x, y))
		TriggerCallbacks(EButtonEvent_OnDoubleClick);
}

bool CButton::CheckHit(int x, int y)
{
	if(m_bHidden)
		return false;

	return x >= m_position.x && x <= m_position.x + m_size.x
		&& y >= m_position.y && y <= m_position.y + m_size.y;
}