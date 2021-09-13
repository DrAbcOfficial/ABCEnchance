class CHudCustomAmmo : public CHudBase
{
public:
	void GLInit();
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void Reset(void);
	void SlotInput(int iSlot, int fAdvance);
	void ChosePlayerWeapon(void);
	void ClientMove(struct playermove_s* ppmove, qboolean server);
	void IN_Accumulate();
	void Clear();

	float m_fFade;
	bool m_bOpeningAnnularMenu = false;
	bool m_bSelectMenuDisplay = false;
	bool m_bSetedCursor = false;

	bool m_bIsOnTarget = false;
	WEAPON* m_pWeapon;

	float SelectCyclerHoldTime;

	bool m_bAcceptDeadMessage = false;
private:
	void DrawScreenQuad();
	void DrawSelectIcon(WEAPON* wp, int a, int xpos, int ypos);
	int DrawWList(float flTime);
	void SyncWeapon();
	float m_fAnimateTime;
	float m_fNextSyncTime;
	int	m_HUD_bucket0;

	float m_fCursorAngle;

	GLint m_hOldBuffer;
	GLuint m_hGaussianBufferVFBO;
	GLuint m_hGaussianBufferVTex;
	GLuint m_hGaussianBufferHFBO;
	GLuint m_hGaussianBufferHTex;

	float StartX = 48;
	float IconSize = 0.5;
	float ElementGap = 0.2;
	float BackGroundY = 0.95;
	float BackGroundLength = 3;

	GLint iSelectCyclerSpr = 0;
	GLint iSelectCyclerRinSpr = 0;
	GLint iSelectCyclerCursorPointer = 0;
	GLint iBackGroundTga = 0;

	Color Ammo1IconColor;
	Color Ammo1BigTextColor;
	Color Ammo1TextColor;
	Color Ammo2IconColor;
	Color Ammo2BigTextColor;
	Color Ammo2TextColor;

	Color SelectCyclerColor;
	Color SelectCyclerRinColor;
	Color SelectCyclerIconColor;
	Color SelectCyclerTextColor;
	Color SelectCyclerEmptyColor;
	Color SelectCyclerPointerColor;

	float SelectCyclerOffset;
	float SelectCyclerSize;
	float SelectCyclerRotate;
	float SelectCyclerAnimateTime;
	float SelectCyclerFadeTime;
	float SelectCyclerPointerSize;

	vgui::HFont HUDFont;
	vgui::HFont HUDSmallFont;
};
extern CHudCustomAmmo m_HudCustomAmmo;