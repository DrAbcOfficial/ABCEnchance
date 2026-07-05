#include <metahook.h>
#include "com_model.h"

#include "exportfuncs.h"
#include "extraprecache.h"

static model_t* g_ExtraPrecacheModel[MAX_EXTRA_PRECACHENUM] = {0};
static size_t g_iExtraPrecacheSize = 0;

model_t* GetExtraModelByModelIndex(int index){
	if (index >= EXTRAPRECACHE_INDEX_BASE && index < EXTRAPRECACHE_INDEX_BASE + MAX_EXTRA_PRECACHENUM)
		return g_ExtraPrecacheModel[index - EXTRAPRECACHE_INDEX_BASE];
	return nullptr;
}

int GetExtraModelIndex(const char* path){
	for (size_t i = 0; i < g_iExtraPrecacheSize; i++) {
		if (!g_ExtraPrecacheModel[i])
			continue;
		if (!strcmp(path, g_ExtraPrecacheModel[i]->name))
			return i + EXTRAPRECACHE_INDEX_BASE;
	}
	return -1;
}

int PrecacheExtraModel(const char* path){
	int index = GetExtraModelIndex(path);
	if (index > -1)
		return index;
	if (g_iExtraPrecacheSize >= MAX_EXTRA_PRECACHENUM)
		return -1;
	g_ExtraPrecacheModel[g_iExtraPrecacheSize] = gEngineStudio.Mod_ForName(path, true);
	if (!g_ExtraPrecacheModel[g_iExtraPrecacheSize])
		return -1;
	g_ExtraPrecacheModel[g_iExtraPrecacheSize]->needload = NL_CLIENT;
	index = g_iExtraPrecacheSize + EXTRAPRECACHE_INDEX_BASE;
	g_iExtraPrecacheSize++;
	return index;
}

void ClearExtraPrecache(){
	for (size_t i = 0; i < g_iExtraPrecacheSize; i++) {
		if (g_ExtraPrecacheModel[i]){
			g_ExtraPrecacheModel[i]->needload = NL_UNREFERENCED;
			g_ExtraPrecacheModel[i] = nullptr;
		}
	}
	g_iExtraPrecacheSize = 0;
}
