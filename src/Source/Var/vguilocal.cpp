#include <metahook.h>
#include "mymathlib.h"
#include "vguilocal.h"

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