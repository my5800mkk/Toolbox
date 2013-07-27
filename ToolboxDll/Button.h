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
	virtual void AddListener(IButtonEventListener *pListener) override { m_listeners.push_back(pListener); }
	virtual void RemoveListener(IButtonEventListener *pListener) override { stl::find_and_erase(m_listeners, pListener); }

	virtual void AddOnPressFunc(TEventFunction eventFunc) override { m_onPress = eventFunc; }
	virtual void AddOnReleaseFunc(TEventFunction eventFunc) override { m_onRelease = eventFunc; }
	virtual void AddOnDoublePressFunc(TEventFunction eventFunc) override { m_onDoublePress = eventFunc; }

	virtual Vec2 GetSize() override { return m_size; }
	virtual void SetSize(Vec2 size) override { m_size = size; }

	virtual Vec2 GetPosition() override { return m_position; }
	virtual void SetPosition(Vec2 pos) override { m_position = pos; }
	// ~IButton

	// IToolboxWindowListener
	virtual void OnRender(IToolboxWindow *pWindow, int width, int height) override;

	virtual void OnLeftMouseButtonDown(IToolboxWindow *pWindow, int x, int y) override;
	virtual void OnLeftMouseButtonUp(IToolboxWindow *pWindow, int x, int y) override;
	virtual void OnLeftMouseButtonDoubleClick(int x, int y) override;
	// ~IToolboxWindowListener

	bool CheckHit(int x, int y);

protected:
	ITexture *m_pTexture;

	Vec2 m_size;
	Vec2 m_position;

	std::vector<IButtonEventListener *> m_listeners;

	TEventFunction m_onPress;
	TEventFunction m_onRelease;
	TEventFunction m_onDoublePress;

	IToolboxWindow *m_pOwner;
};