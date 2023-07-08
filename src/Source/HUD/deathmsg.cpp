#include <metahook.h>
#include <regex>

#include "parsemsg.h"
#include "pm_shared.h"
#include "cvar_hook.h"

#include "local.h"
#include "vguilocal.h"
#include "hud.h"
#include "glew.h"
#include "gl_draw.h"

#include "encode.h"

#include "mymathlib.h"
#include "vgui_controls/Controls.h"

#include "deathmsg.h"

CHudDeathMsg m_HudDeathMsg;
pfnUserMsgHook m_pfnDeathMsg;

using namespace std;
wregex parttenSuicide(MSG_SUICIDENOTIFY);
wregex parttenKilled(MSG_KILLEDNOTIFY);
wregex parttenPlayer(MSG_PLAYERKILLNOTIFY);

int __MsgFunc_DeathMsg(const char* pszName, int iSize, void* pbuf) {
	if(gCVars.pDeathNotice->value <= 0)
		return m_pfnDeathMsg(pszName, iSize, pbuf);
	BEGIN_READ(pbuf, iSize);
	int iType = READ_BYTE();
	if (iType == 0)
		m_HudDeathMsg.bIsDeathMsgOn = true;
	else {
		int iAttacker = READ_SHORT();
		wchar_t vbuf[MSG_BUF_SIZE], ebuf[MSG_BUF_SIZE], kbuf[MSG_BUF_SIZE];
		vgui::localize()->ConvertANSIToUnicode(READ_STRING(), kbuf, sizeof(kbuf));
		vgui::localize()->ConvertANSIToUnicode(READ_STRING(), ebuf, sizeof(ebuf));
		vgui::localize()->ConvertANSIToUnicode(READ_STRING(), vbuf, sizeof(vbuf));
		int iDMGType = READ_LONG();
		m_HudDeathMsg.InsertNewDeathMsg(vbuf, ebuf, kbuf, iAttacker, iType == 2, iDMGType);
	}
	return m_pfnDeathMsg(pszName, iSize, pbuf);
}

bool CHudDeathMsg::MsgFunc_TextMsg(const char* pszName, int iSize, void* pbuf) {
	if (gCVars.pDeathNotice->value <= 0)
		return false;
	if (!bIsDeathMsgOn) {
		BEGIN_READ(pbuf, iSize);
		if (READ_BYTE() == HUD_PRINTNOTIFY) {
			char* msg_text = READ_STRING();
			//什么鸡巴东西
			if (msg_text[0] == '\0')
				return false;
			wchar_t wideBuf[MSG_BUF_SIZE];
			vgui::localize()->ConvertANSIToUnicode(msg_text, wideBuf, sizeof(wideBuf));
			wstring stdSzBuf = wideBuf;
			//剩下的老子才没兴趣
			//正则捕获匹配
			wsmatch matched;
			bool found = false;
			found = regex_search(stdSzBuf, matched, parttenSuicide);
			if (found) {
				wstring suicidebuf = vgui::localize()->Find("DeathMsg_Suicide"), empty = L"";
				InsertNewMsg(matched.prefix().str(), suicidebuf, empty);
				return true;
			}
			found = regex_search(stdSzBuf, matched, parttenKilled);
			if (found) {
				wstring k = matched.suffix().str();
				wstring killBuf = vgui::localize()->Find("DeathMsg_MonsterKill");
				k.erase(k.end() - 2);
				InsertNewMsg(matched.prefix().str(), killBuf, k);
				return true;
			}
			found = regex_search(stdSzBuf, matched, parttenPlayer);
			if (found) {
				wstring e = matched.str();
				wstring sub = e.substr(3, e.length() - 6);
				wstring k = matched.prefix().str();
				InsertNewMsg(matched.suffix().str(), sub, k);
				return true;
			}
		}
	}
	return false;
}

int CHudDeathMsg::Draw(float flTime){
	int x = XOffset;
	int y = YOffset;
	int startAlpha = 255;
	int w;
	int h;
	int r, g, b, a;
	for (const deathmsgItem_t& var: aryKeepMsg){
		if (var.addTime <= 0)
			continue;
		if (var.addTime <= flTime)
			continue;
		if (var.addTime - 2 < flTime)
			a = (float)startAlpha * ((var.addTime - flTime) / gCVars.pDeathNoticeTime->value);
		else
			a = startAlpha;

		wchar_t buf[64];
		wsprintfW(buf, L"%s[%s]%s", var.killer, var.executioner,var.victim);
		GetStringSize(buf, &w, &h, HUDFont);
		x -= w;
		int br, bg, bb, ba;
		if (var.isYou) {
			int iOutLineWidth = 3;
			BackGoundOutLineColor.GetColor(br, bg, bb, ba);
			ba = a * 2 / 3;
			gEngfuncs.pfnFillRGBABlend(x - BackGoundWidth - iOutLineWidth, y - BackGoundWidth - iOutLineWidth,
				w + BackGoundWidth * 2 + iOutLineWidth, iOutLineWidth, br, bg, bb, ba);
			gEngfuncs.pfnFillRGBABlend(x + w + BackGoundWidth, y - BackGoundWidth - iOutLineWidth,
				iOutLineWidth, h + BackGoundWidth * 2 + iOutLineWidth * 2, br, bg, bb, ba);

			gEngfuncs.pfnFillRGBABlend(x - BackGoundWidth - iOutLineWidth, y - BackGoundWidth,
				iOutLineWidth, h + BackGoundWidth * 2 + iOutLineWidth, br, bg, bb, ba);
			gEngfuncs.pfnFillRGBABlend(x - BackGoundWidth - iOutLineWidth, y + h + BackGoundWidth,
				w + BackGoundWidth * 2 + iOutLineWidth, iOutLineWidth, br, bg, bb, ba);
		}
		BackGoundColor.GetColor(br, bg, bb, ba);
		gEngfuncs.pfnFillRGBABlend(x - BackGoundWidth, y - BackGoundWidth, 
			w + BackGoundWidth * 2, h + BackGoundWidth * 2, br, bg, bb, a / 2);
		
		AttackerColor.GetColor(r, g, b, ba);
		mathlib::ColorCalcuAlpha(r, g, b, a);
		wsprintfW(buf, L"%s", var.killer);
		DrawVGUI2String(buf, x, y, r, g, b, HUDFont, true);
		GetStringSize(buf, &w, NULL, HUDFont);
		x += w;

		InflictorColor.GetColor(r, g, b, ba);
		mathlib::ColorCalcuAlpha(r, g, b, a);
		wsprintfW(buf, L"[%s]", var.executioner);
		DrawVGUI2String(buf, x, y, r, g, b, HUDFont, true);
		GetStringSize(buf, &w, NULL, HUDFont);
		x += w;

		VictimColor.GetColor(r, g, b, ba);
		mathlib::ColorCalcuAlpha(r, g, b, a);
		wsprintfW(buf, L"%s", var.victim);
		DrawVGUI2String(buf, x, y, r, g, b, HUDFont, true);
		GetStringSize(buf, &w, &h, HUDFont);
		
		x = XOffset;
		y -= h + GapOffset;
	}
	return 1;
}
void CHudDeathMsg::Reset(void){
	bIsDeathMsgOn = false;
	memset(aryKeepMsg, 0, sizeof(aryKeepMsg));
}
void CHudDeathMsg::PushDeathNotice(wchar_t* v, wchar_t* e, wchar_t* k) {
	//无插件服务器输出控制台
	if (!bIsDeathMsgOn) {
		string cl, cv, ce, ck;
		UnicodeToUTF8((wstring)vgui::localize()->Find("DeathMsg_ConsolePrint"), cl);
		UnicodeToUTF8((wstring)v, cv);
		UnicodeToUTF8((wstring)e, ce);
		UnicodeToUTF8((wstring)k, ck);
		gEngfuncs.Con_Printf(const_cast<char*>(cl.c_str()), const_cast<char*>(cv.c_str()),
			ck[0] == 0 ? "something" : const_cast<char*>(ck.c_str()), const_cast<char*>(ce.c_str()));
	}
	for (size_t i = 0; i < MAX_KEEP_DEATHMSG; i++) {
		deathmsgItem_t a = aryKeepMsg[MAX_KEEP_DEATHMSG - 1];
		aryKeepMsg[MAX_KEEP_DEATHMSG - 1] = aryKeepMsg[i];
		aryKeepMsg[i] = a;
	}
}
void CHudDeathMsg::InsertNewDeathMsg(wchar_t* v, wchar_t* e, wchar_t* k, int kIndex, bool bIsYou, int iDamageType) {
	PushDeathNotice(v, e, k);
	aryKeepMsg[0].attackerIndex = kIndex;
	aryKeepMsg[0].isYou = bIsYou;
	aryKeepMsg[0].damageType = iDamageType;
	wcscpy_s(aryKeepMsg[0].killer, L"");
	cl_entity_t* killer = gEngfuncs.GetEntityByIndex(kIndex);
	if (!killer || !killer->model || killer->model->type == mod_brush) {
		switch (iDamageType) {
		case 1: case 2: case 4: case 8: case 16:
		case 32: case 64: case 128: case 256: case 512:
		case 1024: case 67108864: case 16384: case 32768:
		case 65536: case 131072: case 262144: case 1048576: 
		case 2097152: case 4194304: case 8388608: 
		case 16777216: break;
		default:iDamageType = 0; break;
		}
		char buf[26];
		sprintf_s(buf, sizeof(buf), "DeathMsg_DMGType_%d", iDamageType);
		wcscpy_s(aryKeepMsg[0].executioner, vgui::localize()->Find(buf));
	}
	else if (kIndex == gEngfuncs.GetLocalPlayer()->index)
		wcscpy_s(aryKeepMsg[0].executioner, vgui::localize()->Find("DeathMsg_Suicide"));
	//Monster
	else if(kIndex > 33){
		wcscpy_s(aryKeepMsg[0].executioner, vgui::localize()->Find("DeathMsg_MonsterKill"));
		wcscpy_s(aryKeepMsg[0].killer, k);
	}
	wcscpy_s(aryKeepMsg[0].victim, v);
	aryKeepMsg[0].addTime = gEngfuncs.GetClientTime() + gCVars.pDeathNoticeTime->value;
}
void CHudDeathMsg::InsertNewMsg(const wstring &v, wstring &e, wstring &k){
	wchar_t vbuf[MSG_BUF_SIZE], ebuf[MSG_BUF_SIZE], kbuf[MSG_BUF_SIZE];
	wcscpy_s(vbuf, v.c_str());
	wcscpy_s(ebuf, e.c_str());
	wcscpy_s(kbuf, k.c_str());
	PushDeathNotice(vbuf, ebuf, kbuf);
	wcscpy_s(aryKeepMsg[0].victim, vbuf);
	wcscpy_s(aryKeepMsg[0].executioner, ebuf);
	wcscpy_s(aryKeepMsg[0].killer, kbuf);
	aryKeepMsg[0].attackerIndex = -1;
	aryKeepMsg[0].isYou = false;
	aryKeepMsg[0].damageType = 0;
	aryKeepMsg[0].addTime = gEngfuncs.GetClientTime() + gCVars.pDeathNoticeTime->value;
}
int CHudDeathMsg::Init(void){
	m_pfnDeathMsg = HOOK_MESSAGE(DeathMsg);
	gCVars.pDeathNotice = CREATE_CVAR("hud_deathnotice", "1", FCVAR_VALUE, NULL);
	gCVars.pDeathNoticeTime = CREATE_CVAR("hud_deathnotice_time", "6", FCVAR_VALUE, NULL);
	return 1;
}

void CHudDeathMsg::VidInit(){
	HUDFont = pSchemeData->GetFont("MainShitFont", true);
	XOffset = GET_SCREEN_PIXEL(false, "DeathMsg.XOffset");
	YOffset = GET_SCREEN_PIXEL(true, "DeathMsg.YOffset");
	GapOffset = GET_SCREEN_PIXEL(true, "DeathMsg.GapOffset");
	BackGoundWidth = GET_SCREEN_PIXEL(true, "DeathMsg.BackGoundWidth");

	BackGoundColor = pSchemeData->GetColor("DeathMsg.BackGoundColor", gDefaultColor);
	BackGoundOutLineColor = pSchemeData->GetColor("DeathMsg.BackGoundOutLineColor", gDefaultColor);
	VictimColor = pSchemeData->GetColor("DeathMsg.VictimColor", gDefaultColor);
	AttackerColor = pSchemeData->GetColor("DeathMsg.AttackerColor", gDefaultColor);
	InflictorColor = pSchemeData->GetColor("DeathMsg.InflictorColor", gDefaultColor);
}
