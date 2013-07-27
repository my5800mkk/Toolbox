#pragma once

#include <Windows.h>

struct IButtonEventListener
{
	virtual void OnPress() = 0;
	virtual void OnRelease() = 0;
	virtual void OnDoublePress() = 0;
};

struct IButton
{
	typedef void (*TEventFunction)();

	virtual void AddListener(IButtonEventListener *pListener) = 0;
	virtual void RemoveListener(IButtonEventListener *pListener) = 0;

	virtual void AddOnPressFunc(TEventFunction eventFunc) = 0;
	virtual void AddOnReleaseFunc(TEventFunction eventFunc) = 0;
	virtual void AddOnDoublePressFunc(TEventFunction eventFunc) = 0;

	virtual Vec2 GetSize() = 0;
	virtual void SetSize(Vec2 size) = 0;

	virtual Vec2 GetPosition() = 0;
	virtual void SetPosition(Vec2 pos) = 0;
};