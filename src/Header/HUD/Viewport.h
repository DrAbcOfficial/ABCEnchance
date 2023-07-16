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
#include <vgui2/interface/IViewportPanel.h>
#include <VGUI_controls/Controls.h>
#include <VGUI_controls/Panel.h>
#include <VGUI_controls/Frame.h>

class CPlayerInfoPanel;
class CScorePanel;
class CVotePanel;
class CMotdPanel;
class CSidePanel;
class CTextMenu;
class CFlashLightPanel;
class CNoticePanel;
class CCrosshairPanel;
class CEffectPanel;

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

	bool LoacalPlayerAvilable();

	bool IsInSpectate();
	bool HasSuit();
	bool IsHudHide(int HideToken);

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

	bool MsgShowMenu(const char* pszName, int iSize, void* pbuf);
	void ShowTextMenu(bool state);
	void SelectMenuItem(int slot);
	bool IsTextMenuOpen();

	void SetFlashLight(bool on, int battery);
	void SetFlashBattery(int battery);

	void ShowCrossHair(bool on);

	void SetHealth(int health);

	enum class HUDNOTICE {
		PRINTNOTIFY = 1,
		PRINTCENTER = 4
	};
	void ShowNotice(HUDNOTICE type, const char* message);

	long m_iTimeEnd = 0;

	cvar_t* m_pPlayerTitle = nullptr;
	cvar_t* m_pPlayerTitleDanger = nullptr;
	cvar_t* m_pPopNumber = nullptr;

	vec3_t m_vecClientEVPunch;
	vec2_t m_vecThirdPersonCrosshairPos;
private:
	std::vector<IViewportPanel*> m_Panels;
	CScorePanel* m_pScorePanel = nullptr;
	CVotePanel* m_pVotePanel = nullptr;
	CPlayerInfoPanel* m_pPlayerInfoPanels[32];
	CMotdPanel* m_pMOTDPanel = nullptr;
	CSidePanel* m_pSidePanel = nullptr;
	CTextMenu* m_pTextMenu = nullptr;
	CFlashLightPanel* m_pFlashLight = nullptr;
	CNoticePanel* m_pNotice = nullptr;
	CNoticePanel* m_pNoticeCenter = nullptr;
	CCrosshairPanel* m_pCrossHairPanel = nullptr;
	CEffectPanel* m_pEffectPanel = nullptr;

	int m_iInterMission;

	char m_szServerName[MAX_SERVERNAME_LENGTH] = "<ERROR>";
	char m_szNextMapName[MAX_SERVERNAME_LENGTH] = "<ERROR>";
};

extern CViewport *g_pViewPort;