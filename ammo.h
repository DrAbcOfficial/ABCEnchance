class CHudCustomAmmo : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void Think(void);
	void Reset(void);
	int DrawWList(float flTime);
	void SlotInput(int iSlot, int fAdvance);

	float StartX = 48;
	float IconSize = 0.5;
	float ElementGap = 0.2;
	float BackGroundY = 0.95;
	float BackGroundLength = 3;
	float BackGroundAlpha = 128;

	int iSelectCyclerSpr = 0;

	Color Ammo1IconColor;
	Color Ammo1BigTextColor;
	Color Ammo1TextColor;
	Color Ammo2IconColor;
	Color Ammo2BigTextColor;
	Color Ammo2TextColor;
	Color BackGroundColor;

	vgui::HFont HUDFont;
	vgui::HFont HUDSmallFont;
	
	float m_fFade;
	RGBA  m_rgba;
	WEAPON* m_pWeapon;
	int	m_HUD_bucket0;
	int m_HUD_selection;

};
extern CHudCustomAmmo m_HudCustomAmmo;
extern int g_weaponselect;
extern WEAPON* gpActiveSel;
extern WEAPON* gpLastSel;