#include <metahook.h>
#include "parsemsg.h"
#include "pm_shared.h"
#include "msghook.h"
#include <regex>
#include "mathlib.h"
#include "cvar_hook.h"
#include "local.h"
#include "vguilocal.h"
#include "myconst.h"
#include "drawElement.h"
#include "hud.h"
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
			m_HudDeathMsg.InsertNewMsg(matched.prefix().str(), m_HudDeathMsg.szSuicide, m_HudDeathMsg.szEmpty);
			return 1;
		}
		found = regex_search(stdSzBuf, matched, parttenKilled);
		if (found)
		{
			wstring k = matched.suffix().str();
			k.erase(k.end() - 2);
			m_HudDeathMsg.InsertNewMsg(matched.prefix().str(), m_HudDeathMsg.szKilled, k);
			return 1;
		}
		found = regex_search(stdSzBuf, matched, parttenPlayer);
		if (found)
		{
			wstring e = matched.str(0);
			m_HudDeathMsg.InsertNewMsg(matched.suffix().str(), e.substr(3, e.length() - 7), matched.prefix().str());
			return 1;
		}
	}
	return m_pfnTextMsg(pszName, iSize, pbuf);
}
int CHudDeathMsg::Draw(float flTime)
{
	int iStartX = gScreenInfo.iWidth - XOffset;
	int x = iStartX;
	int y = YOffset;
	int startAlpha = 255;
	int iOffset = GapOffset;
	int iBackWidth = BackGoundWidth;
	int w;
	int h;
	float r, g, b, a;
	for each (deathmsgItem_t var in aryKeepMsg)
	{
		if (var.addTime <= 0)
			continue;
		if (var.addTime <= flTime)
		{
			memset(&var, 0, sizeof(var));
			continue;
		}
		if (var.addTime - 2 < flTime)
			a = (float)startAlpha * ((var.addTime - flTime) / gCVars.pDeathNoticeTime->value);
		else
			a = startAlpha;

		wchar_t buf[64];
		wsprintfW(buf, L"%s[%s]%s", var.killer, var.executioner,var.victim);
		GetStringSize(buf, &w, &h, HUDFont);
		x -= w;

		int br, bg, bb, dummy;
		BackGoundColor.GetColor(br, bg, bb, dummy);
		gEngfuncs.pfnFillRGBABlend(x - iBackWidth, y - iBackWidth, w + iBackWidth * 2, h + iBackWidth * 2, br, bg, bb, a / 2);
		
		r = 0.8;
		g = 0;
		b = 0;
		ColorCalcuAlpha(r, g, b, a);
		wsprintfW(buf, L"%s", var.killer);
		DrawVGUI2String(buf, x, y, r, g, b, HUDFont, true);
		GetStringSize(buf, &w, NULL, HUDFont);
		x += w;

		r = 0.7;
		g = 0.7;
		b = 0.7;
		ColorCalcuAlpha(r, g, b, a);
		wsprintfW(buf, L"[%s]", var.executioner);
		DrawVGUI2String(buf, x, y, r, g, b, HUDFont, true);
		GetStringSize(buf, &w, NULL, HUDFont);
		x += w;

		r = 0;
		ColorCalcuAlpha(r, g, b, a);
		wsprintfW(buf, L"%s", var.victim);
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
void CHudDeathMsg::InsertNewMsg(const wstring &v, wstring &e, wstring &k)
{
	//正常输出控制台
	gEngfuncs.Con_Printf("%s was killed by %s with %s\n", 
		UnicodeToUtf8(v.c_str()), 
		UnicodeToUtf8(k.empty() ? L"something" : k.c_str()), 
		UnicodeToUtf8(e.c_str()));
	for (int i = 0; i < MAX_KEEP_DEATHMSG; i++)
	{
		deathmsgItem_t a = aryKeepMsg[MAX_KEEP_DEATHMSG - 1];
		aryKeepMsg[MAX_KEEP_DEATHMSG - 1] = aryKeepMsg[i];
		aryKeepMsg[i] = a;
	}
	deathmsgItem_t item;
	wcscpy(item.victim, v.c_str());
	wcscpy(item.executioner, e.c_str());
	wcscpy(item.killer, k.c_str());
	item.addTime = gEngfuncs.GetClientTime() + gCVars.pDeathNoticeTime->value;
	aryKeepMsg[0] = item;
}
int CHudDeathMsg::Init(void)
{
	m_pfnTextMsg = HOOK_MESSAGE(TextMsg);
	gCVars.pDeathNoticeTime = CREATE_CVAR("hud_deathnotice_time", "6", FCVAR_VALUE, NULL);
	
	HUDFont = pScheme->GetFont("MainShitFont", true);
	XOffset = atof(pScheme->GetResourceString("DeathMsg.XOffset"));
	YOffset = atof(pScheme->GetResourceString("DeathMsg.YOffset"));
	GapOffset = atof(pScheme->GetResourceString("DeathMsg.GapOffset"));
	BackGoundWidth = atof(pScheme->GetResourceString("DeathMsg.BackGoundWidth"));

	BackGoundColor = pScheme->GetColor("DeathMsg.BackGoundColor", gDefaultColor);

	szSuicide = L"Suicide";
	szKilled = L"Killed";
	szEmpty = L"";
	return 1;
}