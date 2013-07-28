#ifndef __I_TOOLBOX_APPLICATION__
#define __I_TOOLBOX_APPLICATION__

struct IToolboxWindowManager;
struct IToolboxComponentManager;

struct IToolboxViewportManager;

struct IToolboxApplication
{
	virtual void Redraw() = 0;

	virtual void Release() = 0;

	virtual IToolboxWindowManager *GetWindowManager() = 0;

	virtual IToolboxViewportManager *GetViewportManager() = 0;

	/// <summary>
	/// Entry point of the dll, used to set up Toolbox.
	/// </summary>
	typedef IToolboxApplication *(*TEntryFunction)(SSystemInitParams &initParams);
};

#endif // __I_TOOLBOX_APPLICATION__