#include "stdafx.h"

#include "Toolbox.h"

#include <Windows.h>

HMODULE hDll;

extern "C"
{
	TOOLBOX_API IToolboxApplication *InitToolbox(SSystemInitParams &initParams)
	{
		// Don't override Sandbox.
		if(initParams.bEditor)
			return nullptr;

		return new CToolboxApplication(initParams, hDll);
	}
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
    switch ( ul_reason_for_call )
    {
    case DLL_PROCESS_ATTACH:
		hDll = hModule;
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}