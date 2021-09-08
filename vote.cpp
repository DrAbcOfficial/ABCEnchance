#include <metahook.h>

#include "parsemsg.h"
#include "msghook.h"
#include "hud.h"
#include "local.h"
#include "vguilocal.h"
#include "weapon.h"
#include "CColor.h"
#include "CHudDelegate.h"
#include "drawElement.h"
#include "glew.h"

#include "vote.h"
#include <string>

CHudVote m_HudVote;
pfnUserMsgHook m_pfnVoteMenu;
pfnUserMsgHook m_pfnEndVote;
int __MsgFunc_VoteMenu(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	m_HudVote.m_flKeepTime = gEngfuncs.GetClientTime() + READ_BYTE();
	pLocalize->ConvertANSIToUnicode(READ_STRING(), m_HudVote.m_VoteContend, sizeof(m_HudVote.m_VoteContend));
	pLocalize->ConvertANSIToUnicode(READ_STRING(), m_HudVote.m_VoteYes, sizeof(m_HudVote.m_VoteYes));
	pLocalize->ConvertANSIToUnicode(READ_STRING(), m_HudVote.m_VoteNo, sizeof(m_HudVote.m_VoteNo));
	return m_HudVote.StartVote() ? 1 : m_pfnVoteMenu(pszName, iSize, pbuf);
}
int __MsgFunc_EndVote(const char* pszName, int iSize, void* pbuf)
{
	m_HudVote.m_bInVoting = false;
	return m_pfnEndVote(pszName, iSize, pbuf);
}
int CHudVote::Init()
{
	m_pfnVoteMenu = HOOK_MESSAGE(VoteMenu);
	m_pfnEndVote = HOOK_MESSAGE(EndVote);
	
	XOffset = atof(pScheme->GetResourceString("Vote.XOffset"));
	YOffset = atof(pScheme->GetResourceString("Vote.YOffset"));
	Height = atof(pScheme->GetResourceString("Vote.Height"));
	Width = atof(pScheme->GetResourceString("Vote.Width"));

	OutlineColor = pScheme->GetColor("Vote.OutlineColor", gDefaultColor);
	BackGoundColor = pScheme->GetColor("Vote.BackGoundColor", gDefaultColor);

	HudFont = pScheme->GetFont("HUDShitFont");

	wcscpy(VoteTitle, L"Started A new vote:");
	wcscpy(DefaultYes, L"Yes");
	wcscpy(DefaultNo, L"No");

	Reset();
	return 0;
}
void CHudVote::Reset()
{
	VGUI_CREATE_NEWTGA_TEXTURE(m_iYesIconTga, "abcenchance/tga/vote_yes");
	VGUI_CREATE_NEWTGA_TEXTURE(m_iNoIconTga, "abcenchance/tga/vote_no");
}
void CHudVote::DrawMultiLineStr(const wchar_t *str, int limite, int x, int y, int r, int g, int b)
{
	int totalW;
	int nowW = 0;
	int n;
	int c;
	std::wstring szbuf = str;
	wchar_t buf[128];
	GetStringSize(str, &totalW, NULL, HudFont);
	c = wcslen(str) * limite / totalW;
	for (int i = wcslen(str); i <= 0; i -= c)
	{
		szbuf.insert(i, L"\n");
	}
	wcscpy_s(buf, szbuf.c_str());
	DrawVGUI2String(buf, x, y, r, g, b, HudFont);
}
int CHudVote::Draw(float flTime)
{
	if (!m_bInVoting)
		return 0;
	//if (m_flKeepTime <= flTime)
	//	m_bInVoting = false;
	int r, g, b, a;
	int x, y, w, h;
	wchar_t buf[128];
	w = gScreenInfo.iWidth / Width;
	h = gScreenInfo.iHeight / Height;
	x = gScreenInfo.iWidth - XOffset - w;
	y = YOffset;
	OutlineColor.GetColor(r, g, b, a);
	gEngfuncs.pfnFillRGBABlend(x - 2, y - 2, w + 4, h + 4, r, g, b, a);
	BackGoundColor.GetColor(r, g, b, a);
	gEngfuncs.pfnFillRGBABlend(x, y, w, h, r, g, b, a);

	OutlineColor.GetColor(r, g, b, a);
	GetStringSize(VoteTitle, &w, &h, HudFont);
	x += h;
	y += 2;
	DrawMultiLineStr(VoteTitle, w, x, y, r, g, b);
	y += h + 2;
	gEngfuncs.pfnFillRGBABlend(x, y, w, 2, r, g, b, a);
	y += h;
	DrawMultiLineStr(m_VoteContend, w, x, y, r, g, b);
	y = YOffset + gScreenInfo.iHeight / Height;
	y -= 6 * h;

	x += 2.2 * h;
	gHudDelegate->surface()->DrawSetTexture(-1);
	gHudDelegate->surface()->DrawSetColor(255, 255, 255, 255);
	gHudDelegate->surface()->DrawSetTexture(m_iYesIconTga);
	gHudDelegate->surface()->DrawTexturedRect(x, y, x + 2 * h, y + 2 * h);
	x += 2.2 * h;
	wsprintfW(buf, L"F1  %s", !m_VoteYes[0] ? DefaultYes : m_VoteYes);
	DrawVGUI2String(buf, x, y, r, g, b, HudFont);
	y += 2.2 * h;
	x -= 2.2 * h;
	gHudDelegate->surface()->DrawSetColor(255, 255, 255, 255);
	gHudDelegate->surface()->DrawSetTexture(m_iNoIconTga);
	gHudDelegate->surface()->DrawTexturedRect(x, y, x + 2 * h, y + 2 * h);
	x += 2.2 * h;
	wsprintfW(buf, L"F2  %s", !m_VoteNo[0] ? DefaultNo : m_VoteNo);
	DrawVGUI2String(buf, x, y, r, g, b, HudFont);
	return 0;
}
int CHudVote::HUD_KeyEvent(int eventcode, int keynum, const char* pszCurrentBinding)
{
	//F1 135
	//F2 136
	if (m_bInVoting)
	{
		if (keynum == 135 || keynum == 136) {
			if (keynum == 135)
				ServerCmd("voteyes");
			else if (keynum == 136)
				ServerCmd("voteno");
			m_bInVoting = false;
			return 0;
		}
	}
	return 1;
}
int CHudVote::StartVote()
{
	if (m_bInVoting)
		return 0;
	m_bInVoting = true;
	return 1;
}