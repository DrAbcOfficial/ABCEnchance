#include <VGUI/IScheme.h>
#include <VGUI/ISurface.h>
#include <VGUI/ILocalize.h>
#include <metahook.h>

vgui::ISchemeManager* g_pScheme;
vgui::ISurface* pSurface;
vgui::ILocalize* pLocalize;

vgui::HFont m_hFont;
vgui::IScheme* pScheme;

SCREENINFO_s gScreenInfo;