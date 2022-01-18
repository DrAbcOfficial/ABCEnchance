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

	int MenuList[10];
private:
	std::vector<buymenuitem_t> buymenuinfo;
	bool bOpenningMenu;
	float m_fAnimateTime;

	GLuint iBackgroundSpr;

	float BuyMenuAnimateTime;
	int BuyMenuCenterX;
	int BuyMenuCenterY;
	int BuyMenuModelSize;
	int BuyMenuModelX;
	int BuyMenuModelY;
	int BuyMenuOffset;
	int BuyMenuHeight;

	GLint m_hOldBuffer = 0;
	GLuint m_hGaussianBufferVFBO = 0;
	GLuint m_hGaussianBufferVTex = 0;
	GLuint m_hGaussianBufferHFBO = 0;
	GLuint m_hGaussianBufferHTex = 0;

	vgui::HFont hFont;
};
extern CHudEccoBuyMenu m_HudEccoBuyMenu;