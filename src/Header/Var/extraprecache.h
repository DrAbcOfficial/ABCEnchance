#pragma once
#define MAX_EXTRA_PRECACHENUM 255
#define EXTRPRECACHE_INDEX_BASE 2500000

extern model_t* g_ExtraPreacheModel[MAX_EXTRA_PRECACHENUM];

int GetExtraModelIndex(const char* path);
int PrecacheExtraModel(const char* path);
void ClearExtraPrecache();