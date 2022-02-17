#pragma once
class CHudVote {
public:
	int Init();
	int Draw(float flTime);
	void VidInit();
	void Reset();
	int HUD_KeyEvent(int eventcode, int keynum, const char* pszCurrentBinding);
	int StartVote();

	float m_flKeepTime;
	wchar_t m_VoteContend[128];
	wchar_t m_VoteYes[128];
	wchar_t m_VoteNo[128];

	bool m_bInVoting;
private:
	void DrawMultiLineStr(const wchar_t* str, int limite, int x, int y, int r, int g, int b, int *tall);
	GLint m_iYesIconTga;
	GLint m_iNoIconTga;

	wchar_t VoteTitle[256];
	wchar_t DefaultYes[64];
	wchar_t DefaultNo[64];

	size_t XOffset = 0;
	size_t YOffset = 0;
	size_t Height = 0;
	size_t Width = 0;
	Color OutlineColor;
	Color BackGoundColor;
	vgui::HFont HudFont;
};
extern CHudVote m_HudVote;