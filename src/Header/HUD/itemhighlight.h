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
	void AddEntity(int type, cl_entity_s* ent, const char* modelname);

	void EraseHighLight(cl_entity_t* var);
	void CreateHighLight(cl_entity_t* var);
private:
	void LoadItemList();
	
	int m_iHighLightMdl;
	std::map<int, cl_hightlight_t*> m_mapHighLightTable;
	std::map<int, std::pair<TEMPENTITY*, TEMPENTITY*>> m_mapRestoredTent;
};
extern CHudItemHighLight g_HudItemHighLight;