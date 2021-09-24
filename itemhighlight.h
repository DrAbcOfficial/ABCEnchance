#pragma once
typedef struct cl_hightlight_s {
	char Name[64];
	//0 Item 1 Ammo 2 Weapon
	int Type = 0;
	int Index = -1;
}cl_hightlight_t;
class CHudItemHighLight {
public:
	int Init();
	void Reset();
	void Draw(float flTime);
	void AddEntity(int type, cl_entity_s* ent, const char* modelname);
private:
	std::map<int, cl_entity_t*> m_mapToBeDraw;
	std::map<int, cl_hightlight_t*> m_mapHighLightTable;
};
extern CHudItemHighLight m_HudItemHighLight;