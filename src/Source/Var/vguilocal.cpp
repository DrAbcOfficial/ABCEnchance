#include <metahook.h>
#include "mymathlib.h"
#include "vguilocal.h"
#include "Controls.h"

vgui::IScheme* pSchemeData;
ScreenInfo_t gScreenInfo;
Color gDefaultColor = Color(245, 230, 195, 255);

Color g_aryVGUIColorCode[] = {
	Color(255,255,255,255),		//��
	Color(128,128,128,255),		//��
	Color(0,0,0,255),			//��
	Color(255,0,0,255),			//��
	Color(255, 127, 255, 255),	//��
	Color(255, 255,255,255),	//��
	Color(0,255,0,255),			//��
	Color(0,255,255),			//��
	Color(0,0,255,255),			//��
	Color(127,0,255,255)		//��
};

void VGUI_CREATE_NEWTGA_TEXTURE(int tex, const char* path){
	tex = vgui::surface()->CreateNewTextureID(); 
	vgui::surface()->DrawSetTextureFile(tex, path, true, false);
}

size_t GET_SCREEN_PIXEL(bool h, const char* str){
	return  CMathlib::GetScreenPixel(h ? ScreenHeight() : ScreenWidth(), atof(pSchemeData->GetResourceString(str)));
}

size_t ScreenHeight(){
	return gScreenInfo.iHeight;
}
size_t ScreenWidth() {
	return gScreenInfo.iWidth;
}
