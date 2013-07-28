#include "StdAfx.h"
#include "Console.h"

#include "RenderUtils.h"

void CConsoleWindow::OnRender(int width, int height)
{
	SToolboxStyle *pStyle = g_pToolbox->GetWindowStyle();

	STextDrawContext textConfig;
	textConfig.SetSizeIn800x600(false);
	textConfig.SetSize(Vec2(20, 20));
	textConfig.SetColor(ColorF(0.51f));

	AddQueuedText(textConfig, 5, 5, GetTitle());

	auto consoleTextStart = Vec2(5, 30);
	auto consoleTextEnd = Vec2(width - 10.f, height - 35.f);

	SRenderUtils::DrawBox(consoleTextStart, consoleTextEnd, ColorB(255, 255, 255));

	//pStyle->defaultFontColor

	CToolboxWindowBase::OnRender(width, height);
}