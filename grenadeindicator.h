#pragma once
class CHudGrenadeIndicator {
public:
	int Init();
	int Draw(float flTime);
	void Reset();
	void Clear();
	int HUD_AddEntity(int type, cl_entity_s* ent, const char* modelname);
private:
	//Icon by Lorc under CC BY 3.0
	//https://game-icons.net/1x1/lorc/grenade.html
	GLint GrenadeIconTga;

	GLuint IconHalfSize;
	GLuint GrenadeIndex;
	std::vector<cl_entity_t*> aryGrenades;
};
extern CHudGrenadeIndicator m_HudGrenadeIndicator;