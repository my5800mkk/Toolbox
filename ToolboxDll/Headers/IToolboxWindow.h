#ifndef __I_TOOLBOX_WINDOW_H__
#define __I_TOOLBOX_WINDOW_H__

#include <IToolboxComponent.h>

enum EDockState
{
	EDockState_LeftOf = 0,
	EDockState_RightOf,
	EDockState_TopOf,
	EDockState_BottomOf,
	
	EDockState_Last,
};

struct IDockWindow;

struct IIcon;
struct IButton;

struct IToolboxWindowListener
{
	virtual void OnRender(IToolboxWindow *pWindow, int width, int height) {}

	virtual void OnLeftMouseButtonDown(IToolboxWindow *pWindow, int x, int y) {}
	virtual void OnLeftMouseButtonUp(IToolboxWindow *pWindow, int x, int y) {}
	virtual void OnLeftMouseButtonDoubleClick(int x, int y) {}
};

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

	virtual int GetMinWidth() = 0;
	virtual int GetMinHeight() = 0;

	virtual void SetParentOf(IToolboxWindow *pChild) = 0;

	virtual IToolboxWindow *GetWindowParent() = 0;
	inline bool IsChild() { return GetWindowParent() != nullptr; }

	virtual const char *GetTitle() = 0;

	/////////////////////////
	/// Components
	/////////////////////////

	virtual IButton *CreateButton(const char *texturePath) = 0;

	/////////////////////////
	/// Listeners
	/////////////////////////

	virtual void RegisterListener(IToolboxWindowListener *pListener) = 0;
	virtual void UnregisterListener(IToolboxWindowListener *pListener) = 0;

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
	
	/// <summary>
	/// Gets the desired default width of the window when docked.
	/// </summary>
	virtual int GetDefaultDockedWidth() = 0;

	/// <summary>
	/// Gets the desired default height of the window when docked.
	/// </summary>
	virtual int GetDefaultDockedHeight() = 0;

	/////////////////////////
	/// Callbacks
	/////////////////////////

	virtual void OnRender(int width, int height) = 0;
	virtual void OnPostRender() {}

	virtual void OnClose() {}
	
	virtual void OnMouseMove(int x, int y) {}
	virtual void OnMouseLeave() {}
	virtual void OnMouseWheel(short delta) {}

	virtual void OnLeftMouseButtonDown(int x, int y) {}
	virtual void OnLeftMouseButtonUp(int x, int y) {}
	virtual void OnLeftMouseButtonDoubleClick(int x, int y) {}

	virtual void OnMove(int x, int y) {}
	virtual void OnChildMoved(IToolboxWindow *pWindow, int x, int y) {}

	virtual void OnResize(int width, int height) {}
	virtual void OnChildResized(IToolboxWindow *pWindow, int x, int y) {}

	virtual void OnHide() {}
	virtual void OnShow() {}

	virtual void OnMaximize(int width, int height) {}
	virtual void OnMinimize() {}

	virtual void OnCaptureChanged(HWND hWnd) {}
};

#endif // __I_TOOLBOX_WINDOW_H__