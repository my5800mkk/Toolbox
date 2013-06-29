/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2013.
//////////////////////////////////////////////////////////////////////////
// Window manager class, used to create windows and handle their events.
//////////////////////////////////////////////////////////////////////////
// 01/06/2013 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#pragma once

#include <IToolboxWindowManager.h>

#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

struct SWindowFactory
{
	SWindowFactory(const char *_name, EWindowType _type, IToolboxWindow *(*_func)())
		: name(_name)
		, type(_type)
		, func(_func)
	{
	}

	string name;
	EWindowType type;

	IToolboxWindow *(*func)();
};

class CWindowManager
	: public IToolboxWindowManager
{
public:
	CWindowManager();
	~CWindowManager();

	// IToolboxWindowManager
	virtual IToolboxWindow *FindToolboxWindow(WIN_HWND hWnd) override;

	virtual void RegisterWindowType(const char *name, EWindowType type, IToolboxWindow *(*)()) override;

	virtual IToolboxWindow *SpawnWindow(const char *className, const char *name, int width, int height, int x, int y) override;
	
	virtual int GetWindowCount() override { return m_windows.size(); }
	virtual IToolboxWindow *GetWindow(int index) override { return m_windows.at(index); }
	// ~IToolboxWindowManager

	void Update();

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	bool RegisterWindowClass();
	bool RegisterDockedWindowClass();

	HWND CreateToolboxHwnd(const char *name, int width, int height, int x, int y);
	HWND CreateDockedHwnd(const char *name, int width, int height, int x, int y);

	std::vector<IToolboxWindow *> m_windows;
	std::vector<SWindowFactory> m_windowFactories;
};

