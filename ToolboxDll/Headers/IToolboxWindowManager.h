#ifndef __I_TOOLBOX_WINDOW_MANAGER_H__
#define __I_TOOLBOX_WINDOW_MANAGER_H__

struct IToolboxWindow;

#define REGISTER_WINDOW_TYPE(host, name, type, impl) \
	(host)->RegisterWindowTypeT((name), (impl *)0, (type))	

enum EWindowType
{
	// Dockable and detachable
	EWindowType_Window,
	// Undetachable, but redockable
	EWindowType_DockedWindow,
	// Dummy window used to hold tabbed windows
	EWindowType_TabContainer,
	// Dialog window
	EWindowType_Dialog,
};

struct IToolboxWindowManager
{
	virtual IToolboxWindow *FindToolboxWindow(WIN_HWND hWnd) = 0;

	virtual void RegisterWindowType(const char *name, EWindowType type, IToolboxWindow *(*)()) = 0;

	template <class T>
	void RegisterWindowTypeT(const char *name, T *, EWindowType type)
	{
		struct Factory
		{
			static IToolboxWindow *Create()
			{
				return new T();
			}
		};
		
		RegisterWindowType(name, type, Factory::Create);
	}

	virtual IToolboxWindow *SpawnWindow(const char *className, const char *name, int width, int height, int x, int y) = 0;

	virtual int GetWindowCount() = 0;
	virtual IToolboxWindow *GetWindow(int index) = 0;
};

#endif // __I_TOOLBOX_WINDOW_MANAGER_H__