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
	GRENADE
};
class CHudEccoBuyMenu {
public:
	void GLInit();
	int Init();
	int Draw(float flTime);
	void Reset();
	void Clear();
	void AddInfo(buymenuitem_t item);
	void OpenMenu();
	void CloseMenu();
	bool SelectMenu();

	int MenuList[10];
private:
	std::vector<buymenuitem_t> buymenuinfo;
	bool bOpenningMenu = false;
	float m_fAnimateTime = 0;
	int iNowChosenSlot = 0;

	GLuint iBackgroundSpr = 0;

	float BuyMenuAnimateTime = 0;
	int BuyMenuCenterX = 0;
	int BuyMenuCenterY = 0;
	int BuyMenuModelSize = 0;
	int BuyMenuModelX = 0;
	int BuyMenuModelY = 0;
	int BuyMenuOffset = 0;
	int BuyMenuHeight = 0;

	GLint m_hOldBuffer = 0;
	GLuint m_hGaussianBufferVFBO = 0;
	GLuint m_hGaussianBufferVTex = 0;
	GLuint m_hGaussianBufferHFBO = 0;
	GLuint m_hGaussianBufferHTex = 0;
	GLuint m_hGaussianBufferModelFBO = 0;
	GLuint m_hGaussianBufferModelTex = 0;

	vgui::HFont hFont = 0;
};
extern CHudEccoBuyMenu m_HudEccoBuyMenu;