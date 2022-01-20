#pragma once
typedef struct buymenuitem_s{
	int id;
	int price;
	int modelindex;
	int rendermode;
	char name[64];
}buymenuitem_t;
enum EccoBuyMenuRendermode{
	INVALID = 0,
	NONE,
	ITEM,
	PISTOL,
	RIFEL,
	GRENADE,
	KNIFE
};
class CHudEccoBuyMenu {
public:
	int Init();
	void VidInit();
	int Draw(float flTime);
	void Reset();
	void Clear();
	void AddInfo(buymenuitem_t item);
	buymenuitem_t* GetInfo(int index);
	void OpenMenu();
	void CloseMenu();
	bool SelectMenu();

	int MenuList[10];
private:
	std::vector<buymenuitem_t> buymenuinfo;
	bool bOpenningMenu = false;
	float m_fAnimateTime = 0;
	int iNowChosenSlot = 0;

	cvar_t* pCVarIdealYaw = nullptr;
	cvar_t* pCVarIdealDist = nullptr;
	cvar_t* pCVarFollowAim = nullptr;

	int iAnimModelIndex = 0;
	int iPlayerModelIndex = 0;
	TEMPENTITY* pAnimeEntity = nullptr;
	TEMPENTITY* pShowEntity = nullptr;
	TEMPENTITY* pWeaponEntity = nullptr;

	float flOldCamYaw;
	float flOldCamDist;
	float flOldCamHeight;
	float flOldCamRight;
	float flOldFollowAim;
	bool bOldCamThirdperson;

	GLuint iBackgroundSpr = 0;

	float BuyMenuAnimateTime = 0;
	int BuyMenuCenterX = 0;
	int BuyMenuCenterY = 0;
	int BuyMenuModelSize = 0;
	int BuyMenuModelX = 0;
	int BuyMenuModelY = 0;
	int BuyMenuOffset = 0;
	int BuyMenuHeight = 0;

	vgui::HFont hFont = 0;
};
extern CHudEccoBuyMenu m_HudEccoBuyMenu;