#include "stdafx.h"
#include "Toolbox.h"

#include "WindowManager.h"
#include "MainWindow.h"

#include "StartupSplash.h"

#include "Viewport.h"

#include "SystemEventListener_Toolbox.h"

CToolboxApplication *g_pToolbox = 0;

CToolboxApplication::CToolboxApplication(SSystemInitParams &initParams, HMODULE hDll)
	: m_bStartedGameContext(false)
	, m_bGameMode(false)
	, m_hModule(hDll)
{
	g_pToolbox = this;

	m_pStartupSplash = new CStartupSplash();

	m_pWindowManager = new CWindowManager();

	initParams.pUserCallback = this;
	initParams.bEditor = true;
	initParams.sLogFileName = "Toolbox.log";
}

CToolboxApplication::~CToolboxApplication()
{
	g_pToolbox = nullptr;
}

void CToolboxApplication::OnInit(ISystem *pSystem)
{
	ModuleInitISystem(pSystem, "Toolbox");

	if(!pSystem->GetISystemEventDispatcher()->RegisterListener(&g_systemEventListener_Toolbox))
	{
		CryMessageBox("Failed to register Toolbox system event dispatcher!", "Toolbox startup failed!", 0);
		return;
	}
}

void CToolboxApplication::OnInitProgress(const char *sProgressMsg)
{
	m_pStartupSplash->SetStatus(sProgressMsg);
}

void CToolboxApplication::PostInit()
{
	SAFE_DELETE(m_pStartupSplash);

	IFFont *pRobotoFont = gEnv->pCryFont->NewFont("roboto");
	if(!pRobotoFont->Load("Fonts/roboto.xml"))
	{
		CryMessageBox("Failed to load Roboto font!", "Roboto font load failed!!", 0);
	}

	SetupDefaultStyle();
	//SetupDarkStyle();

	RECT workAreaRect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0);

	int width = workAreaRect.right - workAreaRect.left;
	int height = workAreaRect.bottom - workAreaRect.top;

	m_pMainWindow = static_cast<CMainWindow *>(m_pWindowManager->SpawnWindow("MainWindow", "CryENGINE Community Toolbox", width, height, 0, 0));

	StartGameContext(true);

	Redraw();
}

bool CToolboxApplication::StartGameContext(bool start)
{
	bool ok = false;

	if (start == m_bStartedGameContext)
	{
		ok = true;
	}
	else if (start)
	{
		CryLogAlways("Entering game mode");

		SGameContextParams ctx;

		SGameStartParams gameParams;
		gameParams.flags = eGSF_Server
			| eGSF_NoSpawnPlayer
			| eGSF_Client
			| eGSF_NoLevelLoading
			| eGSF_BlockingClientConnect
			| eGSF_NoGameRules
			| eGSF_NoQueries
			| eGSF_LocalOnly;

		gameParams.connectionString = "";
		gameParams.hostname = "localhost";
		gameParams.port = 0xed17;
		gameParams.pContextParams = &ctx;
		gameParams.maxPlayers = 1;

		if (gEnv->pGameFramework->StartGameContext( &gameParams ))
			ok = true; 
	}
	else
	{
		CryLogAlways("Exiting game mode");

		gEnv->pGameFramework->EndGameContext();
		gEnv->pNetwork->SyncWithGame(eNGS_Shutdown);
		ok = true;
	}

	m_bStartedGameContext = start && ok;

	return ok;
}

bool CToolboxApplication::SetGameMode(bool bGameMode)
{
	m_bGameMode = bGameMode;
	bool on = bGameMode;

	bool ok = StartGameContext(on);
	if (ok)
	{
		if(gEnv->IsEditor())
			gEnv->pGame->EditorResetGame(bGameMode);

		gEnv->pGameFramework->OnEditorSetGameMode(bGameMode);
	}
	else
	{
		CryLogAlways("Failed configuring net context");
	}
	return ok;
}

void CToolboxApplication::SetupDefaultStyle()
{
	m_toolboxStyle = SToolboxStyle();

	m_toolboxStyle.backgroundColor = ColorF(239 / 255.f, 239 / 255.f, 242 / 255.f);
	m_toolboxStyle.viewportClearColor = ColorF(0, 0, 0);

	m_toolboxStyle.toolWindowBorderColor = ColorB(214, 214, 214);

	m_toolboxStyle.delimiterColor = ColorB(150, 150, 147);
	m_toolboxStyle.delimiterSize = 5;

	m_toolboxStyle.topBarHeight = 35;
	m_toolboxStyle.infoBarHeight = 25;

	m_toolboxStyle.moverColor = ColorB(150, 150, 147);
	m_toolboxStyle.moverSize = 9;

	m_toolboxStyle.activeTabColor = ColorB(90, 142, 194);
	m_toolboxStyle.tabHeight = 20;
	m_toolboxStyle.tabWidthOffset = 25;
	m_toolboxStyle.tabPaddingX = 4;
	m_toolboxStyle.tabPaddingY = 2;
	m_toolboxStyle.tabFontSize = 16.f;
	m_toolboxStyle.tabFontColor = ColorB(51, 51, 51);
	m_toolboxStyle.activeTabFontColor = ColorB(255, 255, 255);

	m_toolboxStyle.pFont = gEnv->pCryFont->GetFont("roboto");
}

void CToolboxApplication::SetupDarkStyle()
{
	SetupDefaultStyle();

	m_toolboxStyle.backgroundColor = ColorF(16 / 255.f, 16 / 255.f, 13 / 255.f);
}

void CToolboxApplication::Redraw()
{
	m_pMainWindow->Redraw();
}

void CToolboxApplication::OnUpdate()
{
	m_pWindowManager->Update();
}