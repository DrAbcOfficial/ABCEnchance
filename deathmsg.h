#pragma once
#define MAX_KEEP_DEATHMSG 5
#define MSG_BUF_SIZE 64
#define HUD_PRINTNOTIFY 1
#define MSG_SUICIDENOTIFY L" committed suicide."
#define MSG_KILLEDNOTIFY L" was killed by a "
#define MSG_PLAYERKILLNOTIFY L" : (.*) : "

typedef struct deathmsgItem_s{
	wchar_t victim[MSG_BUF_SIZE];
	wchar_t executioner[MSG_BUF_SIZE];
	wchar_t killer[MSG_BUF_SIZE];
	float addTime;
} deathmsgItem_t;
class CHudDeathMsg : public CHudBase
{
public:
	int Init(void);
	int Draw(float flTime);
	void Reset(void);
	void InsertNewMsg(const std::wstring &v, std::wstring &e, std::wstring &k);

	std::wstring szSuicide;
	std::wstring szKilled;
	std::wstring szEmpty;
private:
	deathmsgItem_t aryKeepMsg[MAX_KEEP_DEATHMSG];
	vgui::HFont HUDFont;
	Color BackGoundColor;
	float XOffset;
	float YOffset;
	float GapOffset;
	float BackGoundWidth;
};
extern CHudDeathMsg m_HudDeathMsg;