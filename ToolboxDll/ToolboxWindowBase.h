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
	virtual void OnTabActivated(IToolboxWindow *pTabContainer) override {}

	virtual RECT GetRect() override;

	virtual void Resize(RECT area) override;

	virtual void Redraw() override;
	virtual void Update() override {}
	
	virtual WIN_HWND GetHwnd() override { return m_hWnd; }
	// ~IToolboxComponent

	// IToolboxWindow
	virtual void Initialize(WIN_HWND hWnd) override;
	virtual void PostInitialize() override;

	virtual void Close() override;

	virtual bool CanResize(uint32 resizeState) override;
	virtual bool CanMove() override { return true; }

	virtual void Maximize() override;
	virtual void Minimize() override;
	virtual void Restore() override;

	virtual bool IsMaximized() override;
	virtual bool IsMinimized() override;

	virtual int GetMinWidth() override { return 100; }
	virtual int GetMinHeight() override { return 100; }

	virtual void SetParentOf(IToolboxWindow *pChild) override;
	virtual IToolboxWindow *GetWindowParent() override;

	virtual const char *GetTitle() override;

	virtual IButton *CreateButton(const char *texturePath) override;

	virtual void RegisterListener(IToolboxWindowListener *pListener);
	virtual void UnregisterListener(IToolboxWindowListener *pListener);

	virtual bool IsDockable() override { return true; }

	virtual IDockWindow *GetDockOwner() override { return m_pDockOwner; }
	virtual void OnDocked(IDockWindow *pOwner, EDockState dockState) override { m_pDockOwner = pOwner; m_dockState = dockState; }

	virtual int GetDefaultDockedWidth() { return GetMinWidth(); }
	virtual int GetDefaultDockedHeight() { return GetMinHeight(); }

	virtual void OnRender(int width, int height);
	
	virtual void OnMouseMove(int x, int y) override;
	virtual void OnMouseLeave() override;

	virtual void OnLeftMouseButtonDown(int x, int y) override;
	virtual void OnLeftMouseButtonUp(int x, int y) override;
	virtual void OnLeftMouseButtonDoubleClick(int x, int y) override;

	virtual void OnMove(int x, int y) override;
	//virtual void OnChildMoved(IToolboxWindow *pWindow, int x, int y) override;

	virtual void OnResize(int width, int height) override;
	//virtual void OnChildResized(IToolboxWindow *pWindow, int x, int y) override;

	virtual void OnMaximize(int width, int height) override;
	
	virtual void OnCaptureChanged(HWND hWnd) override;
	// ~IToolboxWindow

	virtual int GetTopBarHeight(SToolboxStyle *pStyle);

	virtual void DrawWindowBorder(float width, float height);

	virtual void PreloadAssets() {}

protected:
	void AddQueuedText(STextDrawContext drawContext, int x, int y, const char *title) { m_queuedText.push_back(new SQueuedText(drawContext, x, y, title)); }

	void SetModifierState(unsigned int moveState);

	HWND m_hWnd;

	unsigned int m_modifierState;
	POINT m_lastCursorPosition;

	IDockWindow *m_pDockOwner;
	EDockState m_dockState;

	std::vector<IToolboxWindow *> m_children;

	std::vector<IToolboxWindowListener *> m_listeners;

private:
	uint32 ShowResizeCursor(int x, int y);

	std::vector<SQueuedText *> m_queuedText;
};

#define INVOKE_LISTENERS(method) \
	for each(auto listener in m_listeners) \
		listener->method;

