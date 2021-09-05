#pragma once
class CHudVote : CHudBase {
public:
	int Init();
	int Draw(float flTime);
	void Reset();
	int HUD_KeyEvent(int eventcode, int keynum, const char* pszCurrentBinding);
	int StartVote();

	float m_flKeepTime;
	wchar_t m_VoteContend[128];
	wchar_t m_VoteYes[128];
	wchar_t m_VoteNo[128];

	bool m_bInVoting;
private:
	void DrawMultiLineStr(const wchar_t* str, int &x, int &y, int r, int g, int b);
	GLint m_iYesIconTga;
	GLint m_iNoIconTga;

	float XOffset;
	float YOffset;
	float Height;
	float Width;

	Color OutlineColor;
	Color BackGoundColor;

	wchar_t VoteTitle[64];
	wchar_t DefaultYes[16];
	wchar_t DefaultNo[16];

	vgui::HFont HudFont;
};
extern CHudVote m_HudVote;