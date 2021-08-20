#include <metahook.h>
#include "CColor.h" 
#include <VGUI/IScheme.h>
#include <VGUI/ISurface.h>
#include <VGUI/ILocalize.h>

vgui::ISchemeManager* g_pScheme;
vgui::ISurface* pSurface;
vgui::ILocalize* pLocalize;
vgui::IScheme* pScheme;
SCREENINFO_s gScreenInfo;
Color gDefaultColor = Color(245, 230, 195, 255);