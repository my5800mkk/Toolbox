#ifndef __SYSTEM_EVENT_LISTENER_TOOLBOX__
#define __SYSTEM_EVENT_LISTENER_TOOLBOX__

#include <ISystem.h>

class CSystemEventListener_Toolbox : public ISystemEventListener
{
	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam);
	// ~ISystemEventListener
};

static CSystemEventListener_Toolbox g_systemEventListener_Toolbox;

#endif // __SYSTEM_EVENT_LISTENER_TOOLBOX__