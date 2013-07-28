#if !defined(AFX_STDAFX_H__B36C365D_F0EA_4545_B3BC_1E0EAB3B5E43__INCLUDED_)
#define AFX_STDAFX_H__B36C365D_F0EA_4545_B3BC_1E0EAB3B5E43__INCLUDED_

#include <CryModuleDefs.h>

#define _FORCEDLL

#ifndef _RELEASE
#define USE_CRY_ASSERT
#endif

// Insert your headers here
#include <platform.h>
#include <algorithm>
#include <vector>
#include <memory>
#include <list>
#include <functional>
#include <limits>

#include <smartptr.h>

#include <CryThread.h>
#include <Cry_Math.h>
#include <ISystem.h>
#include <I3DEngine.h>
#include <IInput.h>
#include <IConsole.h>
#include <ITimer.h>
#include <ILog.h>
#include <IGameplayRecorder.h>
#include <ISerialize.h>

#undef NULL
#define NULL nullptr

#ifndef TOOLBOXDLL_EXPORTS
#define TOOLBOXDLL_EXPORTS
#endif

#ifdef TOOLBOXDLL_EXPORTS
#define TOOLBOX_API DLL_EXPORT
#else
#define TOOLBOX_API
#endif

inline void ToolboxWarning( const char *format,... ) PRINTF_PARAMS(1, 2);
inline void ToolboxWarning( const char *format,... )
{
	if (!format)
		return;
	va_list args;
	va_start(args, format);
	GetISystem()->WarningV(VALIDATOR_MODULE_EDITOR, VALIDATOR_WARNING, 0, nullptr, format, args );
	va_end(args);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //AFX_STDAFX_H__B36C365D_F0EA_4545_B3BC_1E0EAB3B5E43__INCLUDED_