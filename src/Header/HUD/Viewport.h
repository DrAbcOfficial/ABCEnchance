#pragma once

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

#include <vector>

#include <vgui/VGUI.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <vgui/IScheme.h>
#include <vgui/IVGui.h>
#include <vgui/IInput.h>
#include <VGUI2/IViewportPanel.h>
#include <VGUI_controls/Controls.h>
#include <VGUI_controls/Panel.h>
#include <VGUI_controls/Frame.h>

class CPlayerInfoPanel;
class CScorePanel;
class CVotePanel;
class CMotdPanel;
class CSidePanel;
class ISchemel;

class CViewport : public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CViewport, vgui::Panel);

public:
	CViewport(void);
	virtual ~CViewport(void);

public:
	//ClientVGUI Interface
	void Start(void);
	void Init(void);
	void VidInit(void);
	void Reset();
	void Think(void);
	char* GetNextMap();
	void Paint(void);
	void SetParent(vgui::VPANEL vPanel);
	void AddNewPanel(IViewportPanel* panel);
	void ActivateClientUI(void);
	void HideClientUI(void);
	bool KeyInput(int down, int keynum, const char* pszCurrentBinding);

	void SetInterMission(int intermission);
	int GetInterMission();

	bool IsScoreBoardVisible();
	void ShowScoreBoard();
	void HideScoreBoard();
	long GetTimeEnd();
	char* GetServerName();
	CScorePanel* GetScoreBoard();
	CVotePanel* GetVotePanel();
	CMotdPanel* GetMotdPanel();
	Color GetPlayerColor(int index);
	bool IsPlayerTileEnable();

	bool IsVoteEnable();
	void StartVote(char* szContent, char* szYes, char* szNo, int iVoteType);
	void EndVote();

	void AddPopNumber(vec3_t vecOrigin, Color& pColor, int value);

	void AppendMOTD(char* szMessage);
	void ShowMOTD();
	void CloseMOTD();
	void ForeceBuildPage();
	
	void ShowSideText(bool state);

	long m_iTimeEnd = 0;

	cvar_t* m_pPlayerTitle = nullptr;
	cvar_t* m_pPlayerTitleDanger = nullptr;
	cvar_t* m_pPopNumber = nullptr;
private:
	std::vector<IViewportPanel*> m_Panels;
	CScorePanel* m_pScorePanel = nullptr;
	CVotePanel* m_pVotePanel = nullptr;
	CPlayerInfoPanel* m_pPlayerInfoPanels[32];
	CMotdPanel* m_pMOTDPanel = nullptr;
	CSidePanel* m_pSidePanel = nullptr;

	int m_iInterMission;

	char m_szServerName[MAX_SERVERNAME_LENGTH] = "<ERROR>";
	char m_szNextMapName[MAX_SERVERNAME_LENGTH] = "<ERROR>";
};

extern CViewport *g_pViewPort;