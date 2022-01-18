#include <metahook.h>

#include "parsemsg.h"
#include "glew.h"
#include "hud.h"
#include "weapon.h"

#include "vguilocal.h"
#include "gldef.h"
#include "gl_shader.h"
#include "myconst.h"

#include "CHudDelegate.h"
#include "eccobuymenu.h"

CHudEccoBuyMenu m_HudEccoBuyMenu;
enum MetaHookMsgType {
	MHSV_CMD_QUERY_PLUGIN = 1,
	MHSV_CMD_QUERY_CVAR,
	MHSV_CMD_ECCO_INFO = 7,
	MHSV_CMD_ECCO_MENU = 8
};
int __MsgFunc_MetaHook(const char* pszName, int iSize, void* pbuf) {
	BEGIN_READ(pbuf, iSize);
	int type = READ_BYTE();
	switch (type) {
	case MHSV_CMD_ECCO_INFO: {
		buymenuitem_t item;
		item.id = READ_LONG();
		item.price = READ_LONG();
		item.modelindex = READ_LONG();
		item.rendermode = READ_BYTE();
		strcpy_s(item.name, READ_STRING());
		m_HudEccoBuyMenu.AddInfo(item);
		break;
	}
	case MHSV_CMD_ECCO_MENU: {
		int temp[10];
		memset(temp, -1, sizeof(temp));
		for (int l = READ_BYTE(); l > 0; l--) {
			temp[l - 1] = READ_LONG();
		}
		m_HudEccoBuyMenu.OpenMenu(temp);
		break;
	}
	default:break;
	}
	return 0;
}

int CHudEccoBuyMenu::Init(){
	gEngfuncs.pfnHookUserMsg("MetaHook", __MsgFunc_MetaHook);
	return 0;
}

int CHudEccoBuyMenu::Draw(float flTime){
	if (!bOpenningMenu)
		return 1;

	return 0;
}

void CHudEccoBuyMenu::Reset() {
	buymenuinfo.clear();
}

void CHudEccoBuyMenu::Clear() {
	buymenuinfo.clear();
}

void CHudEccoBuyMenu::AddInfo(buymenuitem_t item){
	buymenuinfo.push_back(item);
}

void CHudEccoBuyMenu::OpenMenu(int ids[]){
	bOpenningMenu = true;
	MenuList = ids;
}
