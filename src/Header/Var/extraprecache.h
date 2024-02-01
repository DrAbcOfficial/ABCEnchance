#pragma once

#define NL_PRESENT 0
#define NL_NEEDS_LOADED 1
#define NL_UNREFERENCED 2
#define NL_CLIENT 3

#define MAX_EXTRA_PRECACHENUM 255
#define EXTRPRECACHE_INDEX_BASE 2500000

model_t* GetExtraModelByModelIndex(int index);
int GetExtraModelIndex(const char* path);
int PrecacheExtraModel(const char* path);
void ClearExtraPrecache();