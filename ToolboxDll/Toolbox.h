/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2013.
//////////////////////////////////////////////////////////////////////////
// Main module; handles toolbox initialization
//////////////////////////////////////////////////////////////////////////
// 29/05/2013 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __TOOLBOX_H__
#define __TOOLBOX_H__

#include <IToolboxApplication.h>
#include <ToolboxStyle.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "WindowManager.h"

#include <IGameFramework.h>

class CMainWindow;

class CStartupSplash;

class CViewport;

class CToolboxApplication
	: public IToolboxApplication
	, public ISystemUserCallback
{
public:
	CToolboxApplication(SSystemInitParams &initParams, HMODULE hDll);
	~CToolboxApplication();

	// IToolboxApplication
	virtual void Redraw();

	virtual void Release() { delete this; }

	virtual IToolboxWindowManager *GetWindowManager() { return m_pWindowManager; }
	// ~IToolboxApplication

	// ISystemUserCallback
	virtual bool OnError(const char *szErrorString) { return true; }

	virtual bool OnSaveDocument() { return true; }

	virtual void OnProcessSwitch() {}

	virtual void OnInitProgress(const char *sProgressMsg);

	virtual void OnInit(ISystem *pSystem);

	virtual void GetMemoryUsage(ICrySizer* pSizer) {}

	virtual void OnUpdate();
	// ~ISystemUserCallback

	void PostInit();

	bool StartGameContext(bool start);
	bool SetGameMode(bool gameMode);

	SToolboxStyle *GetWindowStyle() { return &m_toolboxStyle; }

	CStartupSplash *GetStartupSplash() { return m_pStartupSplash; }

	HMODULE GetDllHandle() { return m_hModule; }

protected:
	void SetupDefaultStyle();
	void SetupDarkStyle();

	CMainWindow *m_pMainWindow;
	CWindowManager *m_pWindowManager;

	SToolboxStyle m_toolboxStyle;

	CStartupSplash *m_pStartupSplash;

	bool m_bStartedGameContext;
	bool m_bGameMode;

	HMODULE	m_hModule;
};

extern CToolboxApplication *g_pToolbox;

#endif //__TOOLBOX_H__