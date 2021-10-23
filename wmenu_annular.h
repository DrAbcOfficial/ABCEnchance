#pragma once
class CWeaponMenuAnnular : public IWeaponSelect {
public:
	void Init();
	int DrawWList(float flTime);
	void ClientMove(struct playermove_s* ppmove, int server);
	void IN_Accumulate();
	void Reset();
	void GLInit();
	void Select();
	void Clear();
private:
	void DrawScreenQuad();
	void DrawSelectIcon(WEAPON* wp, int a, int xpos, int ypos, int index);

	GLint iSelectCyclerSpr = 0;
	GLint iSelectCyclerRinSpr = 0;
	GLint iSelectCyclerCursorPointer = 0;

	GLint m_hOldBuffer = 0;
	GLuint m_hGaussianBufferVFBO = 0;
	GLuint m_hGaussianBufferVTex = 0;
	GLuint m_hGaussianBufferHFBO = 0;
	GLuint m_hGaussianBufferHTex = 0;
};
extern CWeaponMenuAnnular m_HudWMenuAnnular;