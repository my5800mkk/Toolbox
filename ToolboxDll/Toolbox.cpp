#include "stdafx.h"
#include "Toolbox.h"

#include "WindowManager.h"
#include "MainWindow.h"

#include "StartupSplash.h"

#include "ViewportManager.h"
#include "Viewport.h"

#include "SystemEventListener_Toolbox.h"

#include <IEntitySystem.h>

CToolboxApplication *g_pToolbox = 0;

CToolboxApplication::CToolboxApplication(SSystemInitParams &initParams, HMODULE hDll)
	: m_bStartedGameContext(false)
	, m_bGameMode(false)
	, m_hModule(hDll)
	, m_bLoadedLevel(false)
	, m_bInitialized(false)
{
	g_pToolbox = this;

	m_pStartupSplash = new CStartupSplash();

	m_pWindowManager = new CWindowManager();
	m_pViewportManager = new CViewportManager();

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

	m_bInitialized = true;

	Redraw();

	Run();
}

void CToolboxApplication::Run()
{
	for(;;)
	{
		MSG msg;

		if (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message != WM_QUIT)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
				break;
		}
		else
			Update();
	}
}

bool CToolboxApplication::Update()
{
	if(!m_bInitialized)
		return true;

	m_pViewportManager->Update();

	return true;
}

IToolboxWindowManager *CToolboxApplication::GetWindowManager()
{
	return m_pWindowManager;
}

IToolboxViewportManager *CToolboxApplication::GetViewportManager()
{
	return m_pViewportManager;
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
	m_toolboxStyle.viewportClearColor = ColorF(0.f);

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
	m_toolboxStyle.defaultFontColor = ColorB(51, 51, 51);
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

void CToolboxApplication::LoadLevel(const char *levelName)
{
	LOADING_TIME_PROFILE_SECTION(gEnv->pSystem);

	GetISystem()->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_LEVEL_LOAD_START, 0, 0);

	CryLogAlways("Loading level %s", levelName);

	CryLogAlways("Finished level %s", levelName);

	GetISystem()->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_LEVEL_LOAD_END, 0, 0);

	m_bLoadedLevel = true;
}

void CToolboxApplication::CreateLevel(const char *levelName)
{
	LOADING_TIME_PROFILE_SECTION(gEnv->pSystem);

	CryLogAlways("Creating empty level %s", levelName);

	string levelPath = PathUtil::GetGameFolder().append("/Levels/").append(levelName).append("/");
	string editorFile = levelPath.append(levelName).append(".tbx");

	gEnv->pCryPak->MakeDir(levelPath, true);

	XmlNodeRef todRoot = GetISystem()->LoadXmlFromFile("Toolbox/default_time_of_day.tod");
	if(todRoot)
	{
		ITimeOfDay *pTimeOfDay = gEnv->p3DEngine->GetTimeOfDay();
		pTimeOfDay->Serialize(todRoot, true);
		pTimeOfDay->SetTime(12.0f, true);
	}

	// Reset systems
	{
		gEnv->pEntitySystem->Reset();
		gEnv->p3DEngine->UnloadLevel();
		gEnv->pPhysicalWorld->SetupEntityGrid(2, Vec3(ZERO), 128, 128, 4, 4, 1);
	}

	if(!gEnv->p3DEngine->InitLevelForEditor(levelPath, ""))
	{
		ToolboxWarning("Failed to initialize level");
		return;
	}

	STerrainInfo terrainInfo;

	terrainInfo.nHeightMapSize_InUnits = terrainInfo.nSectorSize_InMeters = 256;
	terrainInfo.nUnitSize_InMeters = 1;
	terrainInfo.nSectorsTableSize_InSectors = 1;
	terrainInfo.fHeightmapZRatio = 0.003f;
	terrainInfo.fOceanWaterLevel = 20;

	gEnv->p3DEngine->CreateTerrain(terrainInfo);

	XmlNodeRef environmentRoot = GetISystem()->LoadXmlFromFile("Toolbox/default_environment_settings.xml");
	gEnv->p3DEngine->LoadEnvironmentSettingsFromXML(environmentRoot);

	GetISystem()->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_LEVEL_LOAD_END, 0, 0);

	m_bLoadedLevel = true;
}