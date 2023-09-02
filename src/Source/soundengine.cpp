#include <metahook.h>
#include "soundengine.h"

namespace FModEngine {
	static HMODULE g_hFmod = nullptr;
	static FMOD_SYSTEM* g_pFMODSystem = nullptr;

#define FMOD_FUNCTION_DEFINE(name) static decltype(name) * g_pfn##name
	FMOD_FUNCTION_DEFINE(FMOD_System_Create);
	FMOD_FUNCTION_DEFINE(FMOD_System_SetCallback);
	FMOD_FUNCTION_DEFINE(FMOD_System_Init);
	FMOD_FUNCTION_DEFINE(FMOD_System_Close);
	FMOD_FUNCTION_DEFINE(FMOD_System_Release);
	FMOD_FUNCTION_DEFINE(FMOD_System_CreateSound);
	FMOD_FUNCTION_DEFINE(FMOD_Sound_GetLength);
	FMOD_FUNCTION_DEFINE(FMOD_Sound_Release);

	void InitFModLibrary() {
		g_hFmod = GetModuleHandle("svencoop/fmodex.dll");
		if (!g_hFmod)
			g_hFmod = LoadLibrary("svencoop/fmodex.dll");
		if (g_hFmod) {
#define FMOD_DLSYM_CLIEN(name) g_pfn##name = (decltype(g_pfn##name))GetProcAddress(g_hFmod, #name);
			FMOD_DLSYM_CLIEN(FMOD_System_Create);
			FMOD_DLSYM_CLIEN(FMOD_System_Init);
			FMOD_DLSYM_CLIEN(FMOD_System_Close);
			FMOD_DLSYM_CLIEN(FMOD_System_Release);
			FMOD_DLSYM_CLIEN(FMOD_System_CreateSound);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetLength);
			FMOD_DLSYM_CLIEN(FMOD_Sound_Release);
		}
		g_pfnFMOD_System_Create(&g_pFMODSystem);
	}
	void FreeFModLibrary() {
		FreeLibrary(g_hFmod);
		g_pfnFMOD_System_Close(g_pFMODSystem);
	}
	FMOD_SYSTEM* GetSysTem() {
		return g_pFMODSystem;
	}
}