#pragma once

#include <Windows.h>

struct IToolboxWindow;

enum EButtonEvent
{
	EButtonEvent_None = 0,

	EButtonEvent_OnClick,
	EButtonEvent_OnDoubleClick,
	EButtonEvent_OnRelease,

	EButtonEvent_OnMouseOver,
	
	EButtonEvent_Last,
};

struct SButtonEventState
{
	SButtonEventState()
		: pTexture(nullptr)
		, color(1.f) {}

	~SButtonEventState()
	{
		pTexture->Release();
	}

	ITexture *pTexture;

	ColorF color;
};

struct IButton
{
	typedef void (*TEventCallback)(IToolboxWindow *);

	virtual void AddEventCallback(EButtonEvent event, TEventCallback callback) = 0;

	/// <summary>
	/// Used to get the event states for certain actions
	/// The received object can be modified to for example create an alpha effect when the button is held.
	/// </summary>
	virtual SButtonEventState *GetEventState(EButtonEvent event) = 0;

	virtual Vec2 GetSize() = 0;
	virtual void SetSize(Vec2 size) = 0;

	virtual Vec2 GetPosition() = 0;
	virtual void SetPosition(Vec2 pos) = 0;

	virtual void Hide(bool hide) = 0;
	virtual bool IsHidden() = 0;
};