#pragma once
typedef struct buymenuitem_s{
	int id;
	int price;
	int modelindex;
	int sequence;
	char name[64];
}buymenuitem_t;
class CHudEccoBuyMenu {
public:
	void GLInit();
	int Init();
	void VidInit();
	int Draw(float flTime);
	void Reset();
	void Clear();
	bool AddEntity(int type, cl_entity_s* ent, const char* modelname);
	void AddInfo(buymenuitem_t item);
	buymenuitem_t* GetInfo(int index);
	void OpenMenu();
	void CloseMenu();
	bool SelectMenu();
	void SlotCallBack(int slot);
	bool IsOpen();

	std::vector<int> MenuList;
private:
	void CreateLight();
	void ClearTempEnt();
	void ChangePage();
	int GetMenuId(int i);

	std::vector<buymenuitem_t> buymenuinfo;
	bool bOpenningMenu = false;
	float m_fAnimateTime = 0;
	int iNowChosenSlot = 0;
	int iNextPageBase = 0;
	int iNowBackUpSlot = 0;

	Color TextColor;
	Color ButtonColor;

	cvar_t* pCVarIdealYaw = nullptr;
	cvar_t* pCVarIdealDist = nullptr;
	cvar_t* pCVarFollowAim = nullptr;

	dlight_t* pLight = nullptr;
	TEMPENTITY* pShowEnt = nullptr;
	TEMPENTITY* pWeaponEnt = nullptr;
	int iNowSelectedId = -1;

	float flOldCamYaw;
	float flOldCamDist;
	float flOldCamHeight;
	float flOldCamRight;
	float flOldFollowAim;
	bool bOldCamThirdperson;

	GLuint iBackgroundSpr = 0;
	GLuint iCenterSpr = 0;
	GLuint iBackgroundTga = 0;

	float BuyMenuAnimateTime = 0;
	float BuyMenuModelSize = 0;
	float BuyMenuCamYaw = 0;
	float BuyMenuCamDist = 0;
	float BuyMenuCamHeight = 0;
	float BuyMenuCamRight = 0;
	int BuyMenuCenterX = 0;
	int BuyMenuCenterY = 0;
	int BuyMenuOffset = 0;
	int BuyMenuHeight = 0;

	vgui::HFont hFont = 0;

	GLint m_hOldBuffer = 0;
	GLuint m_hGaussianBufferFBO = 0;
	GLuint m_hGaussianBufferTex = 0;
};
extern CHudEccoBuyMenu m_HudEccoBuyMenu;