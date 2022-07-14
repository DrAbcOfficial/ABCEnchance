#include "Color.h" 
#include <vgui/IScheme.h>
#define VGUI_CREATE_NEWTGA_TEXTURE(tex, path) tex=vgui::surface()->CreateNewTextureID();vgui::surface()->DrawSetTextureFile(tex, path, true, false)

typedef struct ScreenInfo_s{
	int iWidth;
	int iHeight;
} ScreenInfo_t;
extern vgui::IScheme* pSchemeData;
extern ScreenInfo_t gScreenInfo;
extern Color gDefaultColor;