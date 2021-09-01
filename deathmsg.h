#pragma once
#define MAX_KEEP_DEATHMSG 5
#define MSG_BUF_SIZE 64
#define HUD_PRINTNOTIFY 1
#define MSG_SUICIDENOTIFY L" committed suicide."
#define MSG_KILLEDNOTIFY L" was killed by a "
#define MSG_PLAYERKILLNOTIFY L" : (.*) : "

typedef struct {
	wchar_t victim[MSG_BUF_SIZE];
	wchar_t executioner[MSG_BUF_SIZE];
	wchar_t killer[MSG_BUF_SIZE];
	float addTime;
} deathmsgItem;
class CHudDeathMsg : public CHudBase
{
public:
	int Init(void);
	int Draw(float flTime);
	void Reset(void);
	void InsertNewMsg(const wchar_t* v, const wchar_t* e, const wchar_t* k);
private:
	deathmsgItem aryKeepMsg[MAX_KEEP_DEATHMSG];
	vgui::HFont HUDFont;
};
extern CHudDeathMsg m_HudDeathMsg;