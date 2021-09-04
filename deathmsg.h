#pragma once
#define MAX_KEEP_DEATHMSG 5
#define MSG_BUF_SIZE 64
#define HUD_PRINTNOTIFY 1
#define MSG_SUICIDENOTIFY L" committed suicide."
#define MSG_KILLEDNOTIFY L" was killed by a "
#define MSG_PLAYERKILLNOTIFY L" : (.*) : "

typedef struct {
	std::wstring victim;
	std::wstring executioner;
	std::wstring killer;
	float addTime;
} deathmsgItem;
class CHudDeathMsg : public CHudBase
{
public:
	int Init(void);
	int Draw(float flTime);
	void Reset(void);
	void InsertNewMsg(std::wstring v, std::wstring e, std::wstring k);
private:
	deathmsgItem aryKeepMsg[MAX_KEEP_DEATHMSG];
	vgui::HFont HUDFont;
	float XOffset;
	float YOffset;
	float GapOffset;
	float BackGoundWidth;

	Color BackGoundColor;
};
extern CHudDeathMsg m_HudDeathMsg;