#pragma once
#define MAX_EXTRA_PRECACHENUM 255
#define EXTRPRECACHE_INDEX_BASE 2500000

extern model_t* g_ExtraPreacheModel[MAX_EXTRA_PRECACHENUM];

int GetExtraModelIndex(char* path);
int PrecacheExtraModel(char* path);
void ClearExtraPrecache();