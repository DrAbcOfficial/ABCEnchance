#include <metahook.h>
#include "vguilocal.h"

vgui::ISchemeManager* g_pScheme;
vgui::ISurface* g_pSurface;
vgui::ILocalize* pLocalize;
vgui::IScheme* pScheme;
vgui::IScheme* pSchemeLang;
ScreenInfo_t gScreenInfo;
Color gDefaultColor = Color(245, 230, 195, 255);