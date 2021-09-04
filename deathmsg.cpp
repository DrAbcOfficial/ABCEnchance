#include <metahook.h>
#include "parsemsg.h"
#include "pm_shared.h"
#include "msghook.h"
#include <regex>
#include "mathlib.h"

#include "cvardef.h"
#include "local.h"
#include "hud.h"
#include "vguilocal.h"
#include "drawElement.h"
#include "deathmsg.h"

CHudDeathMsg m_HudDeathMsg;
pfnUserMsgHook m_pfnTextMsg;

using namespace std;
wregex parttenSuicide(MSG_SUICIDENOTIFY);
wregex parttenKilled(MSG_KILLEDNOTIFY);
wregex parttenPlayer(MSG_PLAYERKILLNOTIFY);

int __MsgFunc_TextMsg(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int type = READ_BYTE();
	if (type == HUD_PRINTNOTIFY)
	{
		char* msg_text = READ_STRING();
		//什么鸡巴东西
		if (msg_text == (char*)0)
			return m_pfnTextMsg(pszName, iSize, pbuf);
		wchar_t wideBuf[MSG_BUF_SIZE];
		pLocalize->ConvertANSIToUnicode(msg_text, wideBuf, sizeof(wideBuf));
		wstring stdSzBuf = wideBuf;
		//剩下的老子才没兴趣
		//正则捕获匹配
		wsmatch matched;
		bool found = false;
		found = regex_search(stdSzBuf, matched, parttenSuicide);
		if (found)
		{
			m_HudDeathMsg.InsertNewMsg(matched.prefix().str(), L"Suicide", L"");
			return 1;
		}
		found = regex_search(stdSzBuf, matched, parttenKilled);
		if (found)
		{
			wstring k = matched.suffix().str();
			k.erase(k.end() - 2);
			m_HudDeathMsg.InsertNewMsg(matched.prefix().str(), L"Killed", k);
			return 1;
		}
		found = regex_search(stdSzBuf, matched, parttenPlayer);
		if (found)
		{
			wstring e = matched.str(0);
			m_HudDeathMsg.InsertNewMsg(matched.suffix().str(), e.substr(4, e.length() - 7), matched.prefix().str());
			return 1;
		}
	}
	return m_pfnTextMsg(pszName, iSize, pbuf);
}
int CHudDeathMsg::Draw(float flTime)
{
	int iStartX = gScreenInfo.iWidth - 64;
	int x = iStartX;
	int y = 256;
	int startAlpha = 254;
	int iOffset = 8;
	int iBackWidth = 3;
	int w;
	int h;
	float r, g, b, a;
	for each (deathmsgItem var in aryKeepMsg)
	{
		if (var.addTime <= 0)
			continue;
		if (var.addTime <= flTime)
		{
			var.killer.clear();
			var.executioner.clear();
			var.victim.clear();
			var.addTime = 0;
			continue;
		}
		if (var.addTime - 2 < flTime)
			a = (float)startAlpha * ((var.addTime - flTime) / gCVars.pDeathNoticeTime->value);
		else
			a = startAlpha;

		wchar_t buf[64];
		wsprintfW(buf, L"%s[%s]%s", var.killer.c_str(), var.executioner.c_str(),var.victim.c_str());
		GetStringSize(buf, &w, &h, HUDFont);
		x -= w;
		gEngfuncs.pfnFillRGBABlend(x - iBackWidth, y - iBackWidth, w + iBackWidth * 2, h + iBackWidth * 2, 180, 180, 180, a / 2);
		
		r = 0.8;
		g = 0;
		b = 0;
		ColorCalcuAlpha(r, g, b, a);
		wsprintfW(buf, L"%s", var.killer.c_str());
		DrawVGUI2String(buf, x, y, r, g, b, HUDFont, true);
		GetStringSize(buf, &w, NULL, HUDFont);
		x += w;

		r = 0.7;
		g = 0.7;
		b = 0.7;
		ColorCalcuAlpha(r, g, b, a);
		wsprintfW(buf, L"[%s]", var.executioner.c_str());
		DrawVGUI2String(buf, x, y, r, g, b, HUDFont, true);
		GetStringSize(buf, &w, NULL, HUDFont);
		x += w;

		r = 0;
		ColorCalcuAlpha(r, g, b, a);
		wsprintfW(buf, L"%s", var.victim.c_str());
		DrawVGUI2String(buf, x, y, r, g, b, HUDFont, true);
		GetStringSize(buf, &w, &h, HUDFont);
		
		x = iStartX;
		y -= h + iOffset;
	}
	return 1;
}
void CHudDeathMsg::Reset(void)
{
	memset(aryKeepMsg, 0, sizeof(aryKeepMsg));
}
void CHudDeathMsg::InsertNewMsg(wstring v, wstring e, wstring k)
{
	for (int i = 0; i < MAX_KEEP_DEATHMSG; i++)
	{
		deathmsgItem a = aryKeepMsg[MAX_KEEP_DEATHMSG - 1];
		aryKeepMsg[MAX_KEEP_DEATHMSG - 1] = aryKeepMsg[i];
		aryKeepMsg[i] = a;
	}
	deathmsgItem item;
	item.victim = v;
	item.executioner = e;
	item.killer = k;
	item.addTime = gEngfuncs.GetClientTime() + gCVars.pDeathNoticeTime->value;
	aryKeepMsg[0] = item;
}
int CHudDeathMsg::Init(void)
{
	m_pfnTextMsg = HOOK_MESSAGE(TextMsg);
	gCVars.pDeathNoticeTime = gEngfuncs.pfnRegisterVariable("hud_deathnotice_time", "6", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	HUDFont = pScheme->GetFont("MainShitFont", true);
	return 1;
}