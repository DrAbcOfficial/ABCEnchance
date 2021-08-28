#pragma once
#define MAX_KEEP_DEATHMSG 5
typedef struct {
	wchar_t* victim;
	wchar_t* executioner;
	wchar_t* killer;
} deathmsgItem;
class CHudDeathMsg : public CHudBase
{
public:
	int Init(void);
	int Draw(float flTime);
	void Reset(void);
	void InsertNewMsg(const wchar_t* v, const wchar_t* e, const wchar_t* k);
private:
	deathmsgItem* aryKeepMsg[MAX_KEEP_DEATHMSG];
	vgui::HFont HUDFont;
};
extern CHudDeathMsg m_HudDeathMsg;