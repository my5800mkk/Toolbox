#include "StdAfx.h"
#include "SystemEventListener_Toolbox.h"

#include "Toolbox.h"

#include <Windows.h>

void CSystemEventListener_Toolbox::OnSystemEvent(ESystemEvent event, UINT_PTR wParam, UINT_PTR lParam)
{
	switch(event)
	{
	case ESYSTEM_EVENT_GAME_POST_INIT:
		{
			g_pToolbox->PostInit();
		}
		break;
	}
}