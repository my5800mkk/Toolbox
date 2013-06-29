#ifndef __I_TOOLBOX_WINDOW_H__
#define __I_TOOLBOX_WINDOW_H__

#include <IToolboxComponent.h>

enum EResizeState
{
	EResizeState_Left = (1 << 0),
	EResizeState_Right = (1 << 1),
	EResizeState_Top = (1 << 2),
	EResizeState_Bottom = (1 << 3),
};

enum EDockState
{
	EDockState_Left = 0,
	EDockState_Right,
	EDockState_Top,
	EDockState_Bottom,
	
	EDocKState_Center,

	EDockState_Last,
};

struct IDockWindow;

struct IToolboxWindow
	: public IToolboxComponent
{
	virtual void Initialize(WIN_HWND hWnd) = 0;
	virtual void PostInitialize() = 0;

	virtual bool CanResize(uint32 resizeState) = 0;
	virtual bool CanMove() = 0;

	virtual void Maximize() = 0;
	virtual void Minimize() = 0;

	virtual bool IsMaximized() = 0;
	virtual bool IsMinimized() = 0;

	/////////////////////////
	/// Docking
	/////////////////////////

	/// <summary>
	/// Gets a boolean indicating whether this component can be docked onto windows.
	/// </summary>
	virtual bool IsDockable() = 0;

	inline bool IsDocked() { return GetDockOwner() != nullptr; }

	/// <summary>
	/// Gets the window that we're currently docked to.
	/// Returns null if not docked.
	/// </summary>
	virtual IDockWindow *GetDockOwner() = 0;

	virtual void OnDocked(IDockWindow *pOwner, EDockState dockState) = 0;

	/////////////////////////
	/// Callbacks
	/////////////////////////

	virtual void OnRender(int width, int height) = 0;
	virtual void OnPostRender() = 0;

	virtual void OnClose() = 0;
	
	virtual void OnMouseMove(int x, int y) = 0;
	virtual void OnMouseLeave() = 0;
	virtual void OnMouseWheel(short delta) = 0;

	virtual void OnLeftMouseButtonDown(int x, int y) = 0;
	virtual void OnLeftMouseButtonUp(int x, int y) = 0;
	virtual void OnLeftMouseButtonDoubleClick(int x, int y) = 0;

	virtual void OnMove(int x, int y) = 0;
	virtual void OnResize(int width, int height) = 0;

	virtual void OnHide() = 0;
	virtual void OnShow() = 0;

	virtual void OnMaximize(int width, int height) = 0;
	virtual void OnMinimize() = 0;

	virtual void OnCaptureChanged(HWND hWnd) = 0;
};

#endif // __I_TOOLBOX_WINDOW_H__