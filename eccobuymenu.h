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
	int Init();
	int Draw(float flTime);
	void Reset();
	void Clear();
	void AddInfo(buymenuitem_t item);
	void OpenMenu(int ids[]);
private:
	std::vector<buymenuitem_t> buymenuinfo;
	bool bOpenningMenu;
	int* MenuList;
};
extern CHudEccoBuyMenu m_HudEccoBuyMenu;