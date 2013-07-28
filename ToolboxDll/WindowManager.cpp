#include "StdAfx.h"
#include "WindowManager.h"

#include "DockWindow.h"
#include "MainWindow.h"
#include "TabbedWindow.h"
#include "DialogWindow.h"

#include "EditModeToolbar.h"
#include "Rollupbar.h"

#include "resource1.h"

CWindowManager::CWindowManager()
{
	RegisterWindowClass();
	RegisterDockedWindowClass();
	
	///////////////////////////////////
	// Base window types
	///////////////////////////////////
	REGISTER_WINDOW_TYPE(this, "MainWindow", EWindowType_Window, CMainWindow);
	REGISTER_WINDOW_TYPE(this, "ToolboxWindow", EWindowType_Window, CDockWindow);
	REGISTER_WINDOW_TYPE(this, "TabbedWindow", EWindowType_TabContainer, CTabbedWindow);

	REGISTER_WINDOW_TYPE(this, "DialogWindow", EWindowType_Dialog, CDialogWindow);

	///////////////////////////////////
	// Specific windows
	///////////////////////////////////
	REGISTER_WINDOW_TYPE(this, "EditModeToolbar", EWindowType_Window, CEditModeToolbar);
	REGISTER_WINDOW_TYPE(this, "Rollupbar", EWindowType_Window, CRollupbar);
}

CWindowManager::~CWindowManager()
{
}

bool CWindowManager::RegisterWindowClass()
{
	WNDCLASS wndClass;
	memset(&wndClass, 0, sizeof(WNDCLASS));

	wndClass.hInstance = GetModuleHandle(0);
	wndClass.lpszClassName = "CryToolboxWindow";
	wndClass.lpfnWndProc = &WndProc;

	wndClass.hIcon = LoadIcon(g_pToolbox->GetDllHandle(), MAKEINTRESOURCE(IDI_ICON2));

	if(!RegisterClass(&wndClass))
	{
		CryMessageBox("Failed to register Toolbox window class!", "Toolbox window creation failed!", 0);
		return false;
	}

	return true;
}

void CWindowManager::RegisterWindowType(const char *name, EWindowType type, IToolboxWindow *(*func)())
{
	m_windowFactories.push_back(SWindowFactory(name, type, func));
}

IToolboxWindow *CWindowManager::SpawnWindow(const char *className, const char *name, int width, int height, int x, int y)
{
	for(auto it = m_windowFactories.begin(); it != m_windowFactories.end(); ++it)
	{
		auto factory = *it;

		if(!strcmp(factory.name.c_str(), className))
		{
			HWND hWnd = nullptr;
			switch(factory.type)
			{
			case EWindowType_DockedWindow:
				hWnd = CreateDockedHwnd(name, width, height, x, y);
				break;
			default:
			case EWindowType_Window:
				hWnd = CreateToolboxHwnd(name, width, height, x, y);
				break;
			}

			CRY_ASSERT(hWnd);
			IToolboxWindow *pWindow = factory.func();
			CRY_ASSERT(pWindow);

			pWindow->Initialize(hWnd);

			m_windows.push_back(pWindow);
			pWindow->PostInitialize();

			return pWindow;
		}
	}

	return nullptr;
}

void CWindowManager::CloseWindow(IToolboxWindow *pWindow)
{
	stl::find_and_erase(m_windows, pWindow);
	delete pWindow;
}

HWND CWindowManager::CreateToolboxHwnd(const char *name, int width, int height, int x, int y)
{
	HWND hWnd = CreateWindow("CryToolboxWindow", name, WS_VISIBLE, x, y, width, height, nullptr, nullptr, GetModuleHandle(0), nullptr);
	if(hWnd == nullptr)
	{
		CryMessageBox("Failed to create Toolbox window!", "Toolbox window creation failed!", 0);
		return nullptr;
	}

	// Remove default window style, we implement our own
	SetWindowLong(hWnd, GWL_STYLE, WS_VISIBLE | WS_THICKFRAME | WS_MAXIMIZEBOX);

	// Redraw
	SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

	ShowWindow(hWnd, SW_HIDE);

	return hWnd;
}

bool CWindowManager::RegisterDockedWindowClass()
{
	WNDCLASS wndClass;
	memset(&wndClass, 0, sizeof(WNDCLASS));

	wndClass.hInstance = GetModuleHandle(0);
	wndClass.lpszClassName = "CryToolboxDockedWindow";
	wndClass.lpfnWndProc = &WndProc;

	wndClass.hbrBackground = CreateSolidBrush(RGB(239, 239, 242));

	if(!RegisterClass(&wndClass))
	{
		CryMessageBox("Failed to register docked window class!", "Toolbox docked window creation failed!", 0);
		return false;
	}

	return true;
}

HWND CWindowManager::CreateDockedHwnd(const char *name, int width, int height, int x, int y)
{
	HWND hWnd = CreateWindow("CryToolboxDockedWindow", name, WS_VISIBLE, x, y, width, height, nullptr, nullptr, GetModuleHandle(0), nullptr);
	if(hWnd == nullptr)
	{
		CryMessageBox("Failed to create docked window!", "Docked window creation failed!", 0);
		return nullptr;
	}

	// Remove default window style, we implement our own
	SetWindowLong(hWnd, GWL_STYLE, WS_VISIBLE);

	// Redraw
	SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

	ShowWindow(hWnd, SW_HIDE);

	return hWnd;
}

void CWindowManager::Update()
{
	for(auto it = m_windows.begin(); it != m_windows.end(); ++it)
	{
		IToolboxWindow *pWindow = *it;

		pWindow->Update();
	}
}

IToolboxWindow *CWindowManager::FindToolboxWindow(WIN_HWND hWnd)
{
	for(auto it = m_windows.begin(); it != m_windows.end(); ++it)
	{
		IToolboxWindow *pWindow = *it;

		if(pWindow->GetHwnd() == hWnd)
			return pWindow;
	}

	return nullptr;
}

LRESULT CALLBACK CWindowManager::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	IToolboxWindow *pWindow = g_pToolbox->GetWindowManager()->FindToolboxWindow(hWnd);
	if(pWindow != nullptr)
	{
		switch(msg)
		{
			case WM_CLOSE:
				{
					pWindow->OnClose();
				}
				break;
			case WM_MOUSEMOVE:
				{ 
					pWindow->OnMouseMove(LOWORD(lParam), HIWORD(lParam));
				}
				break;
			case WM_MOUSELEAVE:
				{
					pWindow->OnMouseLeave();
				}
				break;
			case WM_MOUSEWHEEL:
				{
					pWindow->OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
				}
				break;
			case WM_LBUTTONDOWN:
				{
					pWindow->OnLeftMouseButtonDown(LOWORD(lParam), HIWORD(lParam));
				}
				break;
			case WM_LBUTTONUP:
				{
					pWindow->OnLeftMouseButtonUp(LOWORD(lParam), HIWORD(lParam));
				}
				break;
			case WM_LBUTTONDBLCLK:
				{
					pWindow->OnLeftMouseButtonDoubleClick(LOWORD(lParam), HIWORD(lParam));
				}
				break;
			case WM_MOVE:
				{
					pWindow->OnMove(LOWORD(lParam), HIWORD(lParam));

					if(IToolboxWindow *pWindowParent = pWindow->GetWindowParent())
						pWindowParent->OnChildMoved(pWindow, LOWORD(lParam), HIWORD(lParam));
				}
				break;
			case WM_SIZE:
				{
					switch(wParam)
					{
					case SIZE_MAXIMIZED:
						pWindow->OnMaximize(LOWORD(lParam), HIWORD(lParam));
						break;
					case SIZE_MINIMIZED:
						pWindow->OnMinimize();
						break;
					/*case SW_HIDE:
						pWindow->OnHide();
						break;
					case SW_SHOW:
						pWindow->OnShow();
						break;*/
					default:
						{
							pWindow->OnResize(LOWORD(lParam), HIWORD(lParam));

							if(IToolboxWindow *pWindowParent = pWindow->GetWindowParent())
								pWindowParent->OnChildResized(pWindow, LOWORD(lParam), HIWORD(lParam));
						}
					}
				}
				break;
			case WM_CAPTURECHANGED:
				{
					pWindow->OnCaptureChanged((HWND)lParam);
				}
				break;
			case WM_NCCALCSIZE:
				{
					LPRECT clientRect;
					if(wParam == 1)
						clientRect = &reinterpret_cast<LPNCCALCSIZE_PARAMS>(lParam)->rgrc[0];
					else
						clientRect = reinterpret_cast<LPRECT>(lParam);

					if(pWindow->IsMaximized())
					{
						int borderThickness = GetSystemMetrics(SM_CYSIZEFRAME);

						InflateRect(clientRect, borderThickness - 8, borderThickness - 8);
					}

					return 0;
				}
				break;
			case WM_PAINT:
				{
					PAINTSTRUCT paintStruct;
					BeginPaint(hWnd, &paintStruct);
					EndPaint(hWnd, &paintStruct);
				}
				return 0L;
				// Redraw when focus is changed, to get rid of the frame.
			case WM_KILLFOCUS:
			case WM_SETFOCUS:
				{
					pWindow->Redraw();
				}
				break;
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}