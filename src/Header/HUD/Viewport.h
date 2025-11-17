#pragma once

#include <vector>
#include "vgui_controls/Panel.h"

#include "core/resource/weaponresource.h"

#ifndef VGUI_IVIEWPORTPANEL_H
class IViewportPanel;
#endif

const enum {
	HUD_HIDEWEAPONS = (1 << 0),
	HUD_HIDEFLASHLIGHT = (1 << 1),
	HUD_HIDEALL = (1 << 2),
	HUD_HIDEHEALTH = (1 << 3),
	HUD_HIDESELECTION = (1 << 4),
	HUD_HIDEBATTERY = (1 << 5),
	HUD_HIDECUSTOM1 = (1 << 6),
	HUD_HIDECUSTOM2 = (1 << 7)
};
constexpr auto WEAPON_SUIT = 31;

class CPlayerInfoPanel;
class CMotdPanel;
class CSidePanel;
class CTextMenu;
class CFlashLightPanel;
class CNoticePanel;
class CCrosshairPanel;
class CEffectPanel;
class CHealthPanel;
class CAmmoPanel;
class CDmgTilesPanel;
class CGenadeIndicatorPanel;
class CDeathMsgPanel;
#ifdef __HAS_NETEASE_API
class CNeteasePanel;
#endif // __HAS_NETEASE_API
class CRadarPanel;
class CAmmoStackPanel;
class CItemStackPanel;
class CWeaponStackPanel;
class CWeaponChoosePanel;
class CItemHighLightPanel;

namespace vgui {
	class CScorePanel;
	class CVotePanel;
class CViewport : public Panel
{
	DECLARE_CLASS_SIMPLE(CViewport, Panel);

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
	const char* GetNextMap();
	void SetNextMap(const char* name);
	void Paint(void);
	void SetParent(vgui::VPANEL vPanel);
	void AddNewPanel(IViewportPanel* panel);
	void ActivateClientUI(void);
	void HideClientUI(void);
	bool KeyInput(int down, int keynum, const char* pszCurrentBinding);

	void AddEntity(int type, struct cl_entity_s* ent, const char* modelname);

	void SetInterMission(int intermission);
	int GetInterMission();

	bool LoacalPlayerAvilable();

	bool HasSuit();
	void WeaponBitsChangeCallback(int bits);
	bool IsHudHide(int HideToken);
	void LongjumpCallBack(bool state);

	HScheme GetBaseScheme();

	bool IsScoreBoardVisible();
	void ShowScoreBoard();
	void HideScoreBoard();
	long GetTimeEnd();
	const char* GetServerName();
	void SetServerName(const char*name);
	CScorePanel* GetScoreBoard();
	CVotePanel* GetVotePanel();
	CMotdPanel* GetMotdPanel();
	CAmmoPanel* GetAmmoPanel();
	Color GetPlayerColor(int index);
	bool IsPlayerTileEnable();

	bool IsVoteEnable();
	void StartVote(const char* szContent, const char* szYes, const char* szNo, int iVoteType);
	void EndVote();

	void AddPopNumber(vec3_t vecOrigin, Color& pColor, int value);

	void AppendMOTD(const char* szMessage);
	void ShowMOTD();
	void CloseMOTD();
	void FinishSendMOTD();
	
	void ShowSideText(bool state);

	bool MsgShowMenu(int slot, int time, int bits, const char* message);
	void ShowTextMenu(bool state);
	void SelectMenuItem(int slot);
	bool IsTextMenuOpen();

	void SetFlashLight(bool on, int battery);
	void SetFlashBattery(int battery);

	void ShowCrossHair(bool on);

	void SetHealth(int health);
	void SetArmor(int armor);
	void UpdateTiles(long tiles);

	void SetSpectate(bool state);

	void ShowDeathMsg(bool state);
	void ShowMusic(bool state);
#ifdef __HAS_NETEASE_API
	CNeteasePanel* GetMusicPanel();
private:
	CNeteasePanel* m_pNeteaseMusic = nullptr;
public:
#endif // __HAS_NETEASE_API

	CAmmoStackPanel* GetAmmoStackPanel();
	CItemStackPanel* GetItemStackPanel();
	CWeaponStackPanel* GetWeaponStackPanel();

	CWeaponChoosePanel* GetWeaponChoosePanel();

	void SetCurWeapon(Weapon* weapon);

	enum class HUDNOTICE {
		PRINTNOTIFY = 1,
		PRINTCENTER = 4
	};
	void ShowNotice(HUDNOTICE type, const char* message);

	CRadarPanel* GetRadarPanel();

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
	CHealthPanel* m_pHealthPanel = nullptr;
	CAmmoPanel* m_pAmmoPanel = nullptr;
	CDmgTilesPanel* m_pDmgTiles = nullptr;
	CGenadeIndicatorPanel* m_pGIndicator = nullptr;
	CDeathMsgPanel* m_pDeahMsg = nullptr;
	CRadarPanel* m_pRadar = nullptr;
	CAmmoStackPanel* m_pAmmoStack = nullptr;
	CItemStackPanel* m_pItemStack = nullptr;
	CWeaponStackPanel* m_pWeaponStack = nullptr;
	CWeaponChoosePanel* m_pWeaponChoose = nullptr;
	CItemHighLightPanel* m_pItemHighLightPanel = nullptr;

	vgui::HScheme m_hBaseScheme = 0;
	int m_iInterMission = 0;
	int m_bitsHideHUDDisplay = 0;

	char m_szServerName[MAX_SERVERNAME_LENGTH] = "<ERROR>";
	char m_szNextMapName[MAX_SERVERNAME_LENGTH] = "<ERROR>";
};
}

extern inline vgui::CViewport* GetBaseViewPort();