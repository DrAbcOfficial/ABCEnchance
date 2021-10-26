#include <metahook.h>
#include "com_model.h"

#include "exportfuncs.h"
#include "extraprecache.h"

model_t* g_ExtraPreacheModel[MAX_EXTRA_PRECACHENUM];
int iExtraPrecaheSize = 0;
int GetExtraModelIndex(char* path) {
	for (int i = 0; i < iExtraPrecaheSize; i++) {
		if (!g_ExtraPreacheModel[i])
			continue;
		if (!strcmp(path, g_ExtraPreacheModel[i]->name))
			return i + EXTRPRECACHE_INDEX_BASE;
	}
	return -1;
}
int PrecacheExtraModel(char* path) {
	int index = GetExtraModelIndex(path);
	if (index > -1)
		return index;
	g_ExtraPreacheModel[iExtraPrecaheSize] = gEngineStudio.Mod_ForName(path, true);
	index = iExtraPrecaheSize + EXTRPRECACHE_INDEX_BASE;
	iExtraPrecaheSize++;
	return index;
}
void ClearExtraPrecache(){
	for (model_t* var : g_ExtraPreacheModel) {
		if (var)
			var = nullptr;
	}
	iExtraPrecaheSize = 0;
}