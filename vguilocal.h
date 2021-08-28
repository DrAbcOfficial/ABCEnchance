#include "CColor.h" 
#include <VGUI/IScheme.h>
#include <VGUI/ISurface.h>
#include <VGUI/ILocalize.h>

typedef struct {
	int iWidth;
	int iHeight;
} ScreenInfo;
extern vgui::ISchemeManager* g_pScheme;
extern vgui::ISurface* g_pSurface;
extern vgui::ILocalize* pLocalize;
extern vgui::IScheme* pScheme;
extern ScreenInfo gScreenInfo;
extern Color gDefaultColor;