#include <metahook.h>
#include "vguilocal.h"

vgui::ISchemeManager* g_pScheme;
vgui::ISurface* pSurface;
vgui::ILocalize* pLocalize;
vgui::IScheme* pScheme;
SCREENINFO_s gScreenInfo;
Color gDefaultColor = Color(245, 230, 195, 255);