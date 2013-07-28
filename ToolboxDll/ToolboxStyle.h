#ifndef __TOOLBOX_STYLE__
#define __TOOLBOX_STYLE__

#include <Cry_Vector3.h>

struct SToolboxStyle
{
	ColorF backgroundColor; 
	ColorF viewportClearColor;

	ColorB toolWindowBorderColor;

	ColorB delimiterColor;
	// Size (width or height depending on context) of delimiters which allow for resizing when dragged.
	int delimiterSize;

	// Height of the top bar containing application logo, sys buttons and context menus.
	int topBarHeight;
	// Height of the bar at the bottom of the window.
	int infoBarHeight;

	ColorB moverColor;
	// Size of the mover placed above dockable windows used to undock.
	int moverSize;

	ColorB activeTabColor;
	int tabHeight;
	int tabWidthOffset;
	int tabPaddingX;
	int tabPaddingY;
	float tabFontSize;
	ColorB tabFontColor;
	ColorB activeTabFontColor;

	IFFont *pFont;
	ColorB defaultFontColor;
};

#endif // __TOOLBOX_STYLE__