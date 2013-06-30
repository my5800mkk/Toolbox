#pragma once

#include "Toolbox.h"

#include <IToolboxWindow.h>

struct SQueuedText
{
	SQueuedText(STextDrawContext textConfig, int _x, int _y, const char *title)
		: drawContext(textConfig)
		, x(_x)
		, y(_y)
		, text(title) {}

	STextDrawContext drawContext;

	int x;
	int y;
	
	string text;
};

enum EWindowModifierState
{
	eModifierState_None = 0,

	eModifierState_MoveWindow = (1 << 0),
	eModifierState_MoveTab = (1 << 1),

	// Resize flags:
	// eModifierState_Resize is always set when resizing, others are to indicate the direction.
	eModifierState_Resize = (1 << 2),
	eModifierState_ResizeLeft = (1 << 3),
	eModifierState_ResizeRight = (1 << 4),
	eModifierState_ResizeTop = (1 << 5),
	eModifierState_ResizeBottom = (1 << 6),
};

class CToolboxWindowBase
	: public IToolboxWindow
{
public:
	CToolboxWindowBase();
	virtual ~CToolboxWindowBase();

	// IToolboxComponent
	virtual RECT GetRect() override;

	virtual void Resize(RECT area) override;

	virtual void Redraw();
	
	virtual WIN_HWND GetHwnd() override { return m_hWnd; }
	// ~IToolboxComponent

	// IToolboxWindow
	virtual void Initialize(WIN_HWND hWnd) override;
	virtual void PostInitialize() override;

	virtual bool CanResize(uint32 resizeState) override;
	virtual bool CanMove() override { return true; }

	virtual void Maximize() override;
	virtual void Minimize() override;

	virtual bool IsMaximized() override;
	virtual bool IsMinimized() override;

	virtual void OnRender(int width, int height);
	
	virtual void OnMouseMove(int x, int y) override;
	virtual void OnMouseLeave() override;
	virtual void OnMouseWheel(short delta) = 0;

	virtual void OnLeftMouseButtonDown(int x, int y) override;
	virtual void OnLeftMouseButtonUp(int x, int y) override;
	virtual void OnLeftMouseButtonDoubleClick(int x, int y) = 0;

	virtual void OnMove(int x, int y) override;
	virtual void OnResize(int width, int height) override;

	virtual void OnMaximize(int width, int height) override;
	virtual void OnMinimize() = 0;

	virtual void OnCaptureChanged(HWND hWnd) override;
	// ~IToolboxWindow

	virtual int GetTopBarHeight(SToolboxStyle *pStyle) { return pStyle->topBarHeight; }

	virtual void DrawWindowBorder(float width, float height);

	virtual void PreloadAssets() {}

protected:
	void AddQueuedText(STextDrawContext drawContext, int x, int y, const char *title) { m_queuedText.push_back(new SQueuedText(drawContext, x, y, title)); }

	void SetModifierState(unsigned int moveState);

	HWND m_hWnd;

	unsigned int m_modifierState;
	POINT m_lastCursorPosition;

private:
	uint32 ShowResizeCursor(int x, int y);

	std::vector<SQueuedText *> m_queuedText;
};

