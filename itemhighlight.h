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
	void LoadItemList();
	void CreateHighLight(cl_entity_t* var);
	void EraseHighLight(cl_entity_t* var);

	int m_iHighLightMdl;
	std::map<int, cl_entity_t*> m_mapToBeDraw;
	std::map<int, cl_hightlight_t*> m_mapHighLightTable;
	std::map<cl_entity_t*, std::pair<TEMPENTITY*, TEMPENTITY*>> m_mapRestoredTent;
};
extern CHudItemHighLight m_HudItemHighLight;