/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2013.
//////////////////////////////////////////////////////////////////////////
// Button class to handle click events.
//////////////////////////////////////////////////////////////////////////
// 01/06/2013 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#pragma once

#include <IButton.h>
#include <IToolboxWindow.h>

#include <ITexture.h>

class CButton
	: public IButton
	, public IToolboxWindowListener
{
public:
	CButton(IToolboxWindow *pParent, const char *texturePath);
	~CButton();

	// IButton
	virtual void AddEventCallback(EButtonEvent event, TEventCallback callback) override;

	virtual SButtonEventState *GetEventState(EButtonEvent event) override { return &m_pEventStates[event]; }

	virtual Vec2 GetSize() override { return m_size; }
	virtual void SetSize(Vec2 size) override { m_size = size; }

	virtual Vec2 GetPosition() override { return m_position; }
	virtual void SetPosition(Vec2 pos) override { m_position = pos; }

	virtual void Hide(bool hide) override { m_bHidden = hide; }
	virtual bool IsHidden() override { return m_bHidden; }
	// ~IButton

	// IToolboxWindowListener
	virtual void OnRender(IToolboxWindow *pWindow, int width, int height) override;

	virtual void OnMouseMove(IToolboxWindow *pWindow, int x, int y) override;

	virtual void OnLeftMouseButtonDown(IToolboxWindow *pWindow, int x, int y) override;
	virtual void OnLeftMouseButtonUp(IToolboxWindow *pWindow, int x, int y) override;
	virtual void OnLeftMouseButtonDoubleClick(IToolboxWindow *pWindow, int x, int y) override;
	// ~IToolboxWindowListener

	bool CheckHit(int x, int y);
	void TriggerCallbacks(EButtonEvent event);

	SButtonEventState *GetCurrentEventState();

protected:
	Vec2 m_size;
	Vec2 m_position;

	bool m_bHidden;
	bool m_bHasCallbacks;

	EButtonEvent m_lastEvent;

	std::vector<IButton::TEventCallback> m_eventCallbacks[EButtonEvent_Last];
	SButtonEventState m_pEventStates[EButtonEvent_Last];

	IToolboxWindow *m_pOwner;
};