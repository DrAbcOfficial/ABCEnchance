#pragma once
#define MAX_KEEP_DEATHMSG 5
#define MSG_BUF_SIZE 64
#define HUD_PRINTNOTIFY 1
#define MSG_SUICIDENOTIFY L" committed suicide."
#define MSG_KILLEDNOTIFY L" was killed by a "
#define MSG_PLAYERKILLNOTIFY L" : (.*) : "

typedef struct deathmsgItem_s{
	wchar_t victim[MSG_BUF_SIZE] = {};
	wchar_t executioner[MSG_BUF_SIZE] = {};
	wchar_t killer[MSG_BUF_SIZE] = {};
	float addTime = 0;
	int attackerIndex = -1;
	bool isYou = false;
	int damageType = 0;

	bool hide = false;
} deathmsgItem_t;
class CHudDeathMsg{
public:
	int Init(void);
	int Draw(float flTime);
	void Reset(void);
	void InsertNewDeathMsg(wchar_t* v, wchar_t* e, wchar_t* k, int kIndex, bool bIsYou, int iDamageType);
	void InsertNewMsg(const std::wstring &v, std::wstring &e, std::wstring &k);

	bool bIsDeathMsgOn;
private:
	deathmsgItem_t aryKeepMsg[MAX_KEEP_DEATHMSG];
	vgui::HFont HUDFont;
	Color BackGoundColor;
	Color BackGoundOutLineColor;
	Color VictimColor;
	Color AttackerColor;
	Color InflictorColor;

	size_t XOffset = 0;
	size_t YOffset = 0;
	size_t GapOffset = 0;
	size_t BackGoundWidth = 0;

	void PushDeathNotice(wchar_t* v, wchar_t* e, wchar_t* k);
};
extern CHudDeathMsg m_HudDeathMsg;