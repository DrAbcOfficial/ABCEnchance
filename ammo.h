class CHudCustomAmmo{
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
	void DrawSelectIcon(WEAPON* wp, int a, int xpos, int ypos, int index);
	int DrawWList(float flTime);
	void SyncWeapon();
	float m_fAnimateTime = 0;
	float m_fNextSyncTime = 0;

	float m_fCursorAngle = 0;

	GLint m_hOldBuffer = 0;
	GLuint m_hGaussianBufferVFBO = 0;
	GLuint m_hGaussianBufferVTex = 0;
	GLuint m_hGaussianBufferHFBO = 0;
	GLuint m_hGaussianBufferHTex = 0;

	float IconSize = 0.5F;
	float ElementGap = 0.2F;
	float BackGroundY = 0.95F;
	float BackGroundLength = 3.0F;

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

	float SelectCyclerOffset = 0;
	float SelectCyclerSize = 0;
	float SelectCyclerRotate = 0;
	float SelectCyclerAnimateTime = 0;
	float SelectCyclerFadeTime = 0;
	float SelectCyclerPointerSize = 0;

	vgui::HFont HUDFont = 0;
	vgui::HFont HUDSmallFont = 0;
};
extern CHudCustomAmmo m_HudCustomAmmo;