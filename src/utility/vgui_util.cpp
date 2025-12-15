#include <metahook.h>

#include "Controls.h"

#include "utility/vgui_util.h"

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