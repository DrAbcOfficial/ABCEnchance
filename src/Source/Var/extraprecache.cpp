#include <metahook.h>
#include "com_model.h"

#include "exportfuncs.h"
#include "extraprecache.h"

static model_t* g_ExtraPreacheModel[MAX_EXTRA_PRECACHENUM] = {0};
static size_t g_iExtraPrecaheSize = 0;

model_t* GetExtraModelByModelIndex(int index)
{
	if (index >= EXTRPRECACHE_INDEX_BASE && index < EXTRPRECACHE_INDEX_BASE + MAX_EXTRA_PRECACHENUM)
		return g_ExtraPreacheModel[index - EXTRPRECACHE_INDEX_BASE];

	return NULL;
}

int GetExtraModelIndex(const char* path)
{
	for (size_t i = 0; i < g_iExtraPrecaheSize; i++) {

		if (!g_ExtraPreacheModel[i])
			continue;

		if (!strcmp(path, g_ExtraPreacheModel[i]->name))
			return i + EXTRPRECACHE_INDEX_BASE;
	}

	return -1;
}

int PrecacheExtraModel(const char* path)
{
	int index = GetExtraModelIndex(path);
	if (index > -1)
		return index;

	g_ExtraPreacheModel[g_iExtraPrecaheSize] = IEngineStudio.Mod_ForName(path, true);
	g_ExtraPreacheModel[g_iExtraPrecaheSize]->needload = NL_CLIENT;

	index = g_iExtraPrecaheSize + EXTRPRECACHE_INDEX_BASE;
	g_iExtraPrecaheSize++;
	return index;
}

void ClearExtraPrecache()
{
	for (size_t i = 0; i < g_iExtraPrecaheSize; i++) {
		if (g_ExtraPreacheModel[i])
		{
			g_ExtraPreacheModel[i]->needload = NL_UNREFERENCED;
			g_ExtraPreacheModel[i] = nullptr;
		}
	}
	g_iExtraPrecaheSize = 0;
}