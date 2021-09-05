#include "CColor.h" 
#include <VGUI/IScheme.h>
#include <VGUI/ISurface.h>
#include <VGUI/ILocalize.h>

#define VGUI_CREATE_NEWTGA_TEXTURE(tex, path) tex=gHudDelegate->surface()->CreateNewTextureID();gHudDelegate->surface()->DrawSetTextureFile(tex, path, true, false)

typedef struct ScreenInfo_s{
	int iWidth;
	int iHeight;
} ScreenInfo_t;
extern vgui::ISchemeManager* g_pScheme;
extern vgui::ISurface* g_pSurface;
extern vgui::ILocalize* pLocalize;
extern vgui::IScheme* pScheme;
extern ScreenInfo_t gScreenInfo;
extern Color gDefaultColor;